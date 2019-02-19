#include <shared/bsl.h>

#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/*
 * $Id: arad_tbl_access.c,v 1.186 Broadcom SDK $
 *
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
 *
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_MEM

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/mem.h>
#include <soc/error.h>

#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_chip_tbls.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_api_mgmt.h>
#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_pmf_low_level_fem_tag.h>
#include <soc/dpp/ARAD/arad_ports.h>
#include <soc/dpp/ARAD/arad_cnm.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/drv.h> /* for assert() */
#include <soc/dcmn/dcmn_mem.h> 

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

#define ARAD_WRED_NOF_DROP_PRECEDENCE   (ARAD_NOF_DROP_PRECEDENCE)


#define ARAD_QDR_QDR_DLL_MEM_TBL_ENTRY_SIZE 1


#define ARAD_IHB_TCAM_HIT_INDICATION_NOF_TCAM_ENTRIES_PER_LINE  (8)
#define ARAD_IHB_TCAM_HIT_INDICATION_TCAM_ENTRY_TO_BIT(line)    (line % ARAD_IHB_TCAM_HIT_INDICATION_NOF_TCAM_ENTRIES_PER_LINE)
#define ARAD_IHB_TCAM_HIT_INDICATION_TCAM_ENTRY_TO_LINE(line)   (line / ARAD_IHB_TCAM_HIT_INDICATION_NOF_TCAM_ENTRIES_PER_LINE)

#define ARAD_IRE_NIF_CTXT_MAP_TBL_ENTRY_SIZE                                               1
#define ARAD_IRE_NIF_PORT_TO_CTXT_BIT_MAP_TBL_ENTRY_SIZE                                   7
#define ARAD_IRE_RCY_CTXT_MAP_TBL_ENTRY_SIZE                                               1
#define ARAD_IRE_CPU_CTXT_MAP_TBL_ENTRY_SIZE                                               1
#define ARAD_IRE_TDM_CONFIG_TBL_ENTRY_SIZE                                                 3
#define ARAD_IDR_CONTEXT_MRU_TBL_ENTRY_SIZE                                                2
#define ARAD_IRR_SNOOP_MIRROR_TABLE0_TBL_ENTRY_SIZE                                        2
#define ARAD_IRR_SNOOP_MIRROR_TABLE1_TBL_ENTRY_SIZE                                        4
#define ARAD_IRR_DESTINATION_TABLE_TBL_ENTRY_SIZE                                          1
#define ARAD_IRR_LAG_TO_LAG_RANGE_TBL_ENTRY_SIZE                                           1
#define ARAD_IRR_LAG_MAPPING_TBL_ENTRY_SIZE                                                1
#define ARAD_IRR_SMOOTH_DIVISION_TBL_ENTRY_SIZE                                            1
#define ARAD_IRR_TRAFFIC_CLASS_MAPPING_TBL_ENTRY_SIZE                                      1
#define ARAD_IRR_STACK_TRUNK_RESOLVE_TBL_ENTRY_SIZE                                        1
#define ARAD_IRR_STACK_FEC_RESOLVE_TBL_ENTRY_SIZE                                          1
#define ARAD_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_ENTRY_SIZE                                    2
#define ARAD_QDR_QDR_DLL_MEM_TBL_ENTRY_SIZE                                                1
#define ARAD_IQM_STATIC_TBL_ENTRY_SIZE                                                     1
#define ARAD_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_ENTRY_SIZE                              1
#define ARAD_IQM_CREDIT_DISCOUNT_TABLE_TBL_ENTRY_SIZE                                      1
#define ARAD_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_ENTRY_SIZE                          3
#define ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_ENTRY_SIZE                          1
#define ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_ENTRY_SIZE                          1
#define ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_ENTRY_SIZE                          1
#define ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_ENTRY_SIZE                          1
#define ARAD_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_A_TBL_ENTRY_SIZE                         3
#define ARAD_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_ENTRY_SIZE                                1
#define ARAD_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_ENTRY_SIZE                         1
#define ARAD_IQM_SYSTEM_RED_TBL_ENTRY_SIZE                                                 6
#define ARAD_IQM_CNM_PARAMETERS_TABLE_TBL_ENTRY_SIZE                                       5
#define ARAD_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_ENTRY_SIZE                          1
#define ARAD_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_ENTRY_SIZE                   1
#define ARAD_IPS_FLOW_ID_LOOKUP_TABLE_TBL_ENTRY_SIZE                                       1
#define ARAD_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_ENTRY_SIZE                                    1
#define ARAD_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_ENTRY_SIZE                                  1
#define ARAD_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_ENTRY_SIZE                                  2
#define ARAD_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_ENTRY_SIZE                          2
#define ARAD_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_ENTRY_SIZE                      2
#define ARAD_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_ENTRY_SIZE                           2
#define ARAD_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_ENTRY_SIZE                           1
#define ARAD_EGQ_CCM_TBL_ENTRY_SIZE                                                        1
#define ARAD_EGQ_PMC_TBL_ENTRY_SIZE                                                        1
#define ARAD_EGQ_DWM_TBL_ENTRY_SIZE                                                        1
#define ARAD_EGQ_FQP_NIF_PORT_MUX_TBL_ENTRY_SIZE                                           1
#define ARAD_EGQ_PQP_NIF_PORT_MUX_TBL_ENTRY_SIZE                                           1
#define ARAD_IHB_PINFO_LBP_TBL_ENTRY_SIZE                                                  2
#define ARAD_IHB_HEADER_PROFILE_TBL_ENTRY_SIZE                                             1
#define ARAD_IHB_FEM0_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE                           1
#define ARAD_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE                                 ((139/32) + 1) /* 19b FEM case*/
#define ARAD_IHB_PINFO_LBP_TBL_ENTRY_SIZE                                                  2
#define ARAD_IHB_HEADER_PROFILE_TBL_ENTRY_SIZE                                             1
#define ARAD_CFC_CALRX_TBL_ENTRY_SIZE                                                      1
#define ARAD_CFC_OOB_SCH_MAP_TBL_ENTRY_SIZE                                                1
#define ARAD_CFC_CALTX_TBL_ENTRY_SIZE                                                      1
#define ARAD_CFC_CALRX_TBL_ENTRY_SIZE                                                1
#define ARAD_CFC_OOB_SCH_MAP_TBL_ENTRY_SIZE                                              1
#define ARAD_CFC_OOB_SCH_MAP_TBL_ENTRY_SIZE                                              1
#define ARAD_CFC_CALTX_TBL_ENTRY_SIZE                                                1
#define ARAD_SCH_DRM_TBL_ENTRY_SIZE                                                        1
#define ARAD_SCH_DSM_TBL_ENTRY_SIZE                                                        1
#define ARAD_SCH_FDMS_TBL_ENTRY_SIZE                                                       1
#define ARAD_SCH_SHDS_TBL_ENTRY_SIZE                                                       2
#define ARAD_SCH_SEM_TBL_ENTRY_SIZE                                                        1
#define ARAD_SCH_FSF_TBL_ENTRY_SIZE                                                        1
#define ARAD_SCH_FGM_TBL_ENTRY_SIZE                                                        1
#define ARAD_SCH_SHC_TBL_ENTRY_SIZE                                                        1
#define ARAD_SCH_SCC_TBL_ENTRY_SIZE                                                        1
#define ARAD_SCH_SCT_TBL_ENTRY_SIZE                                                        2
#define ARAD_SCH_FQM_TBL_ENTRY_SIZE                                                        1
#define ARAD_SCH_FFM_TBL_ENTRY_SIZE                                                        1
#define ARAD_SCH_SCHEDULER_INIT_TBL_ENTRY_SIZE                                             1
#define ARAD_SCH_FORCE_STATUS_MESSAGE_TBL_ENTRY_SIZE                                       1


#define ARAD_PP_IHP_MACT_AGING_CONFIGURATION_TABLE_TBL_ENTRY_SIZE                              ( 1)
#define ARAD_PP_IHP_TM_PORT_PARSER_PROGRAM_POINTER_CONFIG_TBL_ENTRY_SIZE                                   ( 1)
#define ARAD_PP_IHP_VIRTUAL_PORT_FEM_BIT_SELECT_TABLE_TBL_ENTRY_SIZE                           ( 1)
#define ARAD_PP_IHP_VIRTUAL_PORT_FEM_MAP_INDEX_TABLE_TBL_ENTRY_SIZE                            ( 1)
#define ARAD_PP_IHP_VIRTUAL_PORT_FEM_FIELD_SELECT_MAP_TBL_ENTRY_SIZE                           ( 4)
#define ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_ENTRY_SIZE                        ( 4)
#define ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_BIT_SELECT_TABLE_TBL_ENTRY_SIZE                 ( 1)
#define ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_MAP_INDEX_TABLE_TBL_ENTRY_SIZE                  ( 1)
#define ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_FIELD_SELECT_MAP_TBL_ENTRY_SIZE                 ( 1)
#define ARAD_PP_IHP_PARSER_PROGRAM_TBL_ENTRY_SIZE                                              ( 1)
#define ARAD_PP_IHP_VLAN_PORT_MEMBERSHIP_TBL_ENTRY_SIZE                                        ( 5)
#define ARAD_PP_IHP_SEM_RESULT_TABLE_TBL_ENTRY_SIZE                                            ( 2)
#define ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_ENTRY_SIZE                                    ( 5)
#define ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_ENTRY_SIZE                                    ( 6)
#define ARAD_PP_IHP_VTT2ND_KEY_CONSTRUCTION0_TBL_ENTRY_SIZE                                    ( 5)
#define ARAD_PP_IHP_VTT2ND_KEY_CONSTRUCTION1_TBL_ENTRY_SIZE                                    ( 6)
#define ARAD_PP_IHP_TC_DP_MAP_TABLE_TBL_ENTRY_SIZE                                                ( 2)
#define ARAD_PP_IHP_TERMINATION_PROFILE_TABLE_TBL_ENTRY_SIZE                                   ( 1)
#define ARAD_PP_IHP_ACTION_PROFILE_MPLS_VALUE_TBL_ENTRY_SIZE                                   ( 1)
#define ARAD_PP_IHB_FEC_ENTRY_ACCESSED_TBL_ENTRY_SIZE                                          ( 1)
#define ARAD_PP_IHB_PATH_SELECT_TBL_ENTRY_SIZE                                                 ( 1)
#define ARAD_PP_IHB_TCAM_ACTION_TBL_ENTRY_SIZE                                                 ( 1)
#define ARAD_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ENTRY_SIZE                               ( 1)
#define ARAD_PP_EGQ_VSI_MEMBERSHIP_TBL_ENTRY_SIZE                                              ( 9)
#define ARAD_PP_EPNI_TX_TAG_TABLE_TBL_ENTRY_SIZE                                               ( 9)
#define ARAD_PP_EPNI_PCP_DEI_MAP_TBL_ENTRY_SIZE                                                ( 2)
#define ARAD_PP_EPNI_EGRESS_EDIT_CMD_TBL_ENTRY_SIZE                                            ( 2)
#define ARAD_PP_EPNI_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ENTRY_SIZE                              ( 1)
#define ARAD_PP_EPNI_ISID_TABLE_TBL_ENTRY_SIZE                                                    ( 1)
#define ARAD_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_ENTRY_SIZE                                       ( 1)
#define ARAD_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_ENTRY_SIZE                             ( 1) 
#define ARAD_PP_IHB_FLP_PROCESS_TBL_ENTRY_SIZE 3
#define ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_ENTRY_SIZE 6
#define ARAD_PP_IHB_LPM_TBL_ENTRY_SIZE 1
#define ARAD_PP_IHB_PMF_PASS_2_KEY_UPDATE_TBL_ENTRY_SIZE 1

#define ARAD_IQM_CNM_DS_TBL_DATA_SAMPLING_EN_2_BIT     4
#define ARAD_IQM_CNM_DS_TBL_DATA_SAMPLING_PROFILE_LSB  5
#define ARAD_IQM_CNM_DS_TBL_DATA_SAMPLING_PROFILE_MSB  7


#define ARAD_PP_VTT_PD_BITMAP_NOF_FIELDS  4
#define ARAD_PP_FLP_PD_BITMAP_NOF_FIELDS  7

#define ARAD_PP_IHP_LIF_TABLE_ENTRY_SIZE 4


static const int egq_vsi_membership_backup = EGQ_VSI_MEMBERSHIPm;
static const int irr_snoop_mirror_table_0_backup = IRR_SNOOP_MIRROR_TABLE_0m;
static const int irr_snoop_mirror_table_1_backup = IRR_SNOOP_MIRROR_TABLE_1m;
#undef EGQ_VSI_MEMBERSHIPm
#define EGQ_VSI_MEMBERSHIPm \
    (SOC_IS_JERICHO(unit) ? EGQ_VSI_MEMBERSHIP_MEMORY_VSI_MEMBERSHIPm : egq_vsi_membership_backup)
#undef IRR_SNOOP_MIRROR_TABLE_0m
#define IRR_SNOOP_MIRROR_TABLE_0m \
    (SOC_IS_JERICHO(unit) ? IRR_SNOOP_MIRROR_DEST_TABLEm : irr_snoop_mirror_table_0_backup)
#undef IRR_SNOOP_MIRROR_TABLE_1m
#define IRR_SNOOP_MIRROR_TABLE_1m \
    (SOC_IS_JERICHO(unit) ? IRR_SNOOP_MIRROR_TM_TABLEm : irr_snoop_mirror_table_1_backup)



/* } */

/*************
 *  MACROS   *
 *************/
/* { */

#define SOC_DPP_MEM_FIELD32_GET(field_get,table_name,field_name,exit_num) \
{ \
    res=SOC_E_NONE;\
  if(SOC_E_NONE == res) { \
      field_get = soc_mem_field32_get( \
                      unit, \
                      table_name, \
                      data, \
                      field_name); \
   } \
   SOC_SAND_CHECK_FUNC_RESULT(res, exit_num, exit); \
}

#define SOC_DPP_MEM_FIELD32_SET(field_set,table_name,field_name,exit_num) \
{ \
          res=SOC_E_NONE;\
    if(SOC_E_NONE == res) { \
        soc_mem_field32_set( \
          unit, \
          table_name, \
          data, \
          field_name, \
          field_set ); \
        } \
    SOC_SAND_CHECK_FUNC_RESULT(res, exit_num, exit); \
}


/* for odd entry jump to odd table*/

/* for odd entry jump to odd table*/

/* for odd entry jump to odd table*/

/* for odd entry jump to odd table*/

#define ARAD_PP_TBL_FEC_ARRAY_INDEX(_entry_index) ((_entry_index)%2)
#define ARAD_PP_TBL_FEC_ENTRY_INDEX(_entry_index) ((_entry_index)/2)

/* Layout is: 8 banks, each has (FEC_NUM / 8 * 2) lines with 2 fields - data0 and data1. */
#define JERICHO_PP_FEC_BANK_SIZE(unit)                        (SOC_DPP_DEFS_GET(unit, nof_fecs) / (8 * 2))
#define JERICHO_PP_TBL_FEC_ARRAY_INDEX(unit, _entry_index)    ((_entry_index) / JERICHO_PP_FEC_BANK_SIZE(unit))
#define JERICHO_PP_TBL_FEC_ENTRY_INDEX(unit, _entry_index)    ((_entry_index) % JERICHO_PP_FEC_BANK_SIZE(unit))
#define JERICHO_PP_TBL_FEC_ENTRY_OFFSET(unit, _entry_index)   ((_entry_index) % 2)

/* Simple TCAM print */
#define ARAD_PP_TBL_ACC_TCAM_DEBUG_PRINT 0
/*#define ARAD_PP_TBL_ACC_TCAM_DEBUG_PRINT 1*/

/* Low-Level TCAM print for all the transaction */
#define ARAD_PP_TBL_ACC_TCAM_LOW_LEVEL_DEBUG_PRINT 0
/*#define ARAD_PP_TBL_ACC_TCAM_LOW_LEVEL_DEBUG_PRINT 1*/

#define ARAD_PP_TBL_ACC_TCAM_DEBUG_ACTION_PRINT 0
/*#define ARAD_PP_TBL_ACC_TCAM_DEBUG_ACTION_PRINT 1*/

/* Conversation from entry offset to lif bank id (0-3 in arad, 0-7 in Jericho).
   In Arad it's simply the MSBs, in Jericho it's 2 MSBs + 1 LSB */
#define LIF_TABLE_ENTRY_OFFSET_TO_BANK_ID(_unit, entry_offset)   \
  ((((entry_offset) & SOC_DPP_DEFS_GET(_unit, inlif_bank_msbs_mask)) >> \
    (SOC_DPP_DEFS_GET(_unit, inlif_bank_msbs_mask_start) - SOC_DPP_DEFS_GET(_unit, inlif_bank_lsbs_mask_end))) \
  | ((entry_offset) & SOC_DPP_DEFS_GET(_unit, inlif_bank_lsbs_mask)))

#define LIF_TABLE_ENTRY_OFFSET_TO_OFFSET_IN_BANK(_unit, entry_offset) \
  (((entry_offset) & ~(SOC_DPP_DEFS_GET(_unit, inlif_bank_msbs_mask) | SOC_DPP_DEFS_GET(_unit, inlif_bank_lsbs_mask))) \
   >> SOC_DPP_DEFS_GET(_unit, inlif_bank_lsbs_mask_end))



#define ARAD_MBMP_SET_DYNAMIC(_mem)      _SHR_PBMP_PORT_ADD(arad_mem_is_dynamic_bmp[_mem/32], _mem%32)
#define ARAD_MBMP_IS_DYNAMIC(_mem)       _SHR_PBMP_MEMBER(arad_mem_is_dynamic_bmp[_mem/32], _mem%32)

/* Conversion FLP instruction valid bitmap from ARAD to Jericho
 * - during Jericho porting, use instruction 0-2 similarly, 
 * and instructions 3-5 as Jericho instructions 4-6. */
#define FLP_INSTR_VALID_BMP_ARAD_TO_JER(inst_valid_bitmap)  \
  (((inst_valid_bitmap & 0x38) << 1) | ((inst_valid_bitmap) & 7))

#ifdef EMULATION_FAST_WRITE_INSTEAD_OF_DMA
/* Used to debug the emulation fast write mechanism. */
/* If TRUE then fast write will be used both for fill_table_with_entry and for fill_table with callback. */
static const char _arad_tbl_access_emulation_fast_write_debug = FALSE;
#endif /* EMULATION_FAST_WRITE_INSTEAD_OF_DMA */

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

static _shr_pbmp_t arad_mem_is_dynamic_bmp[NUM_SOC_MEM/32];
static uint32 *_arad_fill_table_with_variable_values_by_caching_buffer[BCM_MAX_NUM_UNITS];
static sal_mutex_t _arad_fill_table_with_variable_values_by_caching_buffer_lock[BCM_MAX_NUM_UNITS];
/* 64KB buffer */
static const uint32 _arad_fill_table_with_variable_values_by_caching_buffer_len = (1 << 16) / sizeof(_arad_fill_table_with_variable_values_by_caching_buffer[0]);

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

/*
 * determinding dynamic memories
 */
void arad_tbl_default_dynamic_set(void) {

    sal_memset(arad_mem_is_dynamic_bmp, 0, sizeof(arad_mem_is_dynamic_bmp));
    ARAD_MBMP_SET_DYNAMIC(CRPS_CRPS_0_CNTS_MEMm);
    ARAD_MBMP_SET_DYNAMIC(CRPS_CRPS_0_OVTH_MEMm);
    ARAD_MBMP_SET_DYNAMIC(CRPS_CRPS_1_CNTS_MEMm);
    ARAD_MBMP_SET_DYNAMIC(CRPS_CRPS_1_OVTH_MEMm);
    ARAD_MBMP_SET_DYNAMIC(CRPS_CRPS_2_CNTS_MEMm);
    ARAD_MBMP_SET_DYNAMIC(CRPS_CRPS_2_OVTH_MEMm);
    ARAD_MBMP_SET_DYNAMIC(CRPS_CRPS_3_CNTS_MEMm);
    ARAD_MBMP_SET_DYNAMIC(CRPS_CRPS_3_OVTH_MEMm);
    ARAD_MBMP_SET_DYNAMIC(EGQ_CBMm);
    ARAD_MBMP_SET_DYNAMIC(EGQ_DCMm);
    ARAD_MBMP_SET_DYNAMIC(EGQ_FBMm);
    ARAD_MBMP_SET_DYNAMIC(EGQ_FDMm);
    ARAD_MBMP_SET_DYNAMIC(EGQ_PDCMm);
    ARAD_MBMP_SET_DYNAMIC(EGQ_PDCMAXm);
    ARAD_MBMP_SET_DYNAMIC(EGQ_PQSMm);
    ARAD_MBMP_SET_DYNAMIC(EGQ_PQSMAXm);
    ARAD_MBMP_SET_DYNAMIC(EGQ_QDCMm);
    ARAD_MBMP_SET_DYNAMIC(EGQ_QDCMAXm);
    ARAD_MBMP_SET_DYNAMIC(EGQ_QM_0m);
    ARAD_MBMP_SET_DYNAMIC(EGQ_QM_1m);
    ARAD_MBMP_SET_DYNAMIC(EGQ_QM_2m);
    ARAD_MBMP_SET_DYNAMIC(EGQ_QM_3m);
    ARAD_MBMP_SET_DYNAMIC(EGQ_QP_CBMm);
    ARAD_MBMP_SET_DYNAMIC(EGQ_QQSMm);
    ARAD_MBMP_SET_DYNAMIC(EGQ_QQSMAXm);
    ARAD_MBMP_SET_DYNAMIC(EGQ_RDMMCm);
    ARAD_MBMP_SET_DYNAMIC(EGQ_RDMUCm);
    ARAD_MBMP_SET_DYNAMIC(EGQ_RPDMm);
    ARAD_MBMP_SET_DYNAMIC(EGQ_RRDMm);
    ARAD_MBMP_SET_DYNAMIC(EGQ_TCG_CBMm);
    ARAD_MBMP_SET_DYNAMIC(EDB_ESEM_MANAGEMENT_REQUESTm);
    ARAD_MBMP_SET_DYNAMIC(FCR_FCR_CRM_Am);
    ARAD_MBMP_SET_DYNAMIC(FCR_FCR_CRM_Bm);
    ARAD_MBMP_SET_DYNAMIC(IDR_COMPLETE_PCm);
    ARAD_MBMP_SET_DYNAMIC(IDR_CONTEXT_COLORm);
    ARAD_MBMP_SET_DYNAMIC(IDR_CONTEXT_SIZEm);
    ARAD_MBMP_SET_DYNAMIC(IDR_GLOBAL_METER_STATUSm);
    ARAD_MBMP_SET_DYNAMIC(IDR_MCDA_DYNAMICm);
    ARAD_MBMP_SET_DYNAMIC(IDR_MCDB_DYNAMICm);
    ARAD_MBMP_SET_DYNAMIC(IHB_CPU_TRAP_CODE_CTRm);
    ARAD_MBMP_SET_DYNAMIC(IHB_FEC_ENTRY_ACCESSEDm);
    ARAD_MBMP_SET_DYNAMIC(IHB_OAM_COUNTER_FIFOm);
    ARAD_MBMP_SET_DYNAMIC(PPDB_A_OEMA_MANAGEMENT_REQUESTm);
    ARAD_MBMP_SET_DYNAMIC(PPDB_A_OEMB_MANAGEMENT_REQUESTm);
    ARAD_MBMP_SET_DYNAMIC(IHB_TCAM_ACTION_HIT_INDICATIONm);
    ARAD_MBMP_SET_DYNAMIC(IHB_TCAM_ACTION_HIT_INDICATION_24m);
    ARAD_MBMP_SET_DYNAMIC(IHB_TCAM_ACTION_HIT_INDICATION_25m);
    ARAD_MBMP_SET_DYNAMIC(IHB_TCAM_ACTION_HIT_INDICATION_26m);
    ARAD_MBMP_SET_DYNAMIC(IHB_TCAM_ACTION_HIT_INDICATION_27m);
    ARAD_MBMP_SET_DYNAMIC(IHB_TCAM_BANKm);
    ARAD_MBMP_SET_DYNAMIC(IHB_TCAM_ENTRY_PARITYm);
    ARAD_MBMP_SET_DYNAMIC(IHB_TCAM_ENTRY_PARITY_12m);
    ARAD_MBMP_SET_DYNAMIC(IHB_TCAM_ENTRY_PARITY_13m);
    ARAD_MBMP_SET_DYNAMIC(IHB_TIME_STAMP_FIFOm);
    ARAD_MBMP_SET_DYNAMIC(IHP_ISA_MANAGEMENT_REQUESTm);
    ARAD_MBMP_SET_DYNAMIC(IHP_ISB_MANAGEMENT_REQUESTm);
    ARAD_MBMP_SET_DYNAMIC(IHP_LIF_ACCESSEDm);
    ARAD_MBMP_SET_DYNAMIC(IPS_CRBALm);
    ARAD_MBMP_SET_DYNAMIC(IPS_MAXQSZm);
    ARAD_MBMP_SET_DYNAMIC(IPS_QDESCm);
    ARAD_MBMP_SET_DYNAMIC(IPS_QDESC_TABLEm);
    ARAD_MBMP_SET_DYNAMIC(IPS_QSZm);
    ARAD_MBMP_SET_DYNAMIC(IPT_BDQm);
    ARAD_MBMP_SET_DYNAMIC(IPT_EGQCTLm);
    ARAD_MBMP_SET_DYNAMIC(IPT_EGQDATAm);
    ARAD_MBMP_SET_DYNAMIC(IPT_EGQ_TXQ_RD_ADDRm);
    ARAD_MBMP_SET_DYNAMIC(IPT_EGQ_TXQ_WR_ADDRm);
    ARAD_MBMP_SET_DYNAMIC(IPT_FDTCTLm);
    ARAD_MBMP_SET_DYNAMIC(IPT_FDTDATAm);
    ARAD_MBMP_SET_DYNAMIC(IPT_FDT_TXQ_RD_ADDRm);
    ARAD_MBMP_SET_DYNAMIC(IPT_FDT_TXQ_WR_ADDRm);
    ARAD_MBMP_SET_DYNAMIC(IPT_MOP_MMUm);
    ARAD_MBMP_SET_DYNAMIC(IPT_PCQm);
    ARAD_MBMP_SET_DYNAMIC(IPT_SOP_MMUm);
    ARAD_MBMP_SET_DYNAMIC(IQM_BDBLLm);
    ARAD_MBMP_SET_DYNAMIC(IQM_CNG_QUE_SETm);
    ARAD_MBMP_SET_DYNAMIC(IQM_CPDMDm);
    ARAD_MBMP_SET_DYNAMIC(IQM_DBFFMm);
    ARAD_MBMP_SET_DYNAMIC(IQM_DELFFMm);
    ARAD_MBMP_SET_DYNAMIC(IQM_FLUSCNTm);
    ARAD_MBMP_SET_DYNAMIC(IQM_MNUSCNTm);
    ARAD_MBMP_SET_DYNAMIC(IQM_PDMm);
    ARAD_MBMP_SET_DYNAMIC(IQM_PQDMDm);
    ARAD_MBMP_SET_DYNAMIC(IQM_TAILm);
    ARAD_MBMP_SET_DYNAMIC(IQM_VSQ_A_MX_OCm);
    ARAD_MBMP_SET_DYNAMIC(IQM_VSQ_B_MX_OCm);
    ARAD_MBMP_SET_DYNAMIC(IQM_VSQ_C_MX_OCm);
    ARAD_MBMP_SET_DYNAMIC(IQM_VSQ_D_MX_OCm);
    ARAD_MBMP_SET_DYNAMIC(IQM_VSQ_E_MX_OCm);
    ARAD_MBMP_SET_DYNAMIC(IQM_VSQ_F_MX_OCm);
    ARAD_MBMP_SET_DYNAMIC(IQM_VS_QA_AVGm);
    ARAD_MBMP_SET_DYNAMIC(IQM_VS_QA_QSZm);
    ARAD_MBMP_SET_DYNAMIC(IQM_VS_QB_AVGm);
    ARAD_MBMP_SET_DYNAMIC(IQM_VS_QB_QSZm);
    ARAD_MBMP_SET_DYNAMIC(IQM_VS_QC_AVGm);
    ARAD_MBMP_SET_DYNAMIC(IQM_VS_QC_QSZm);
    ARAD_MBMP_SET_DYNAMIC(IQM_VS_QD_AVGm);
    ARAD_MBMP_SET_DYNAMIC(IQM_VS_QD_QSZm);
    ARAD_MBMP_SET_DYNAMIC(IQM_VS_QE_AVGm);
    ARAD_MBMP_SET_DYNAMIC(IQM_VS_QE_QSZm);
    ARAD_MBMP_SET_DYNAMIC(IQM_VS_QF_AVGm);
    ARAD_MBMP_SET_DYNAMIC(IQM_VS_QF_QSZm);
    ARAD_MBMP_SET_DYNAMIC(IRE_CTXT_MEM_CONTROLm);
    ARAD_MBMP_SET_DYNAMIC(IRR_FREE_PCB_MEMORYm);
    ARAD_MBMP_SET_DYNAMIC(IRR_ISF_MEMORYm);
    ARAD_MBMP_SET_DYNAMIC(IRR_IS_FREE_PCB_MEMORYm);
    ARAD_MBMP_SET_DYNAMIC(IRR_IS_PCB_LINK_TABLEm);
    ARAD_MBMP_SET_DYNAMIC(IRR_LAG_NEXT_MEMBERm);
    ARAD_MBMP_SET_DYNAMIC(IRR_MCR_MEMORYm);
    ARAD_MBMP_SET_DYNAMIC(IRR_PCB_LINK_TABLEm);
    ARAD_MBMP_SET_DYNAMIC(MMU_DRAM_ADDRESS_SPACEm);
    ARAD_MBMP_SET_DYNAMIC(MMU_FDFm);
    ARAD_MBMP_SET_DYNAMIC(MMU_IDFm);
    ARAD_MBMP_SET_DYNAMIC(MMU_RAFA_RADDR_STATUSm);
    ARAD_MBMP_SET_DYNAMIC(MMU_RAFB_RADDR_STATUSm);
    ARAD_MBMP_SET_DYNAMIC(MMU_RAFC_RADDR_STATUSm);
    ARAD_MBMP_SET_DYNAMIC(MMU_RAFD_RADDR_STATUSm);
    ARAD_MBMP_SET_DYNAMIC(MMU_RAFE_RADDR_STATUSm);
    ARAD_MBMP_SET_DYNAMIC(MMU_RAFF_RADDR_STATUSm);
    ARAD_MBMP_SET_DYNAMIC(MMU_RAFG_RADDR_STATUSm);
    ARAD_MBMP_SET_DYNAMIC(MMU_RAFH_RADDR_STATUSm);
    ARAD_MBMP_SET_DYNAMIC(MMU_RAF_WADDRm);
    ARAD_MBMP_SET_DYNAMIC(MMU_RDFA_WADDR_STATUSm);
    ARAD_MBMP_SET_DYNAMIC(MMU_RDFB_WADDR_STATUSm);
    ARAD_MBMP_SET_DYNAMIC(MMU_RDFC_WADDR_STATUSm);
    ARAD_MBMP_SET_DYNAMIC(MMU_RDFD_WADDR_STATUSm);
    ARAD_MBMP_SET_DYNAMIC(MMU_RDFE_WADDR_STATUSm);
    ARAD_MBMP_SET_DYNAMIC(MMU_RDFF_WADDR_STATUSm);
    ARAD_MBMP_SET_DYNAMIC(MMU_RDFG_WADDR_STATUSm);
    ARAD_MBMP_SET_DYNAMIC(MMU_RDFH_WADDR_STATUSm);
    ARAD_MBMP_SET_DYNAMIC(MMU_RDF_RADDRm);
    ARAD_MBMP_SET_DYNAMIC(MMU_WAFA_HALFA_RADDR_STATUSm);
    ARAD_MBMP_SET_DYNAMIC(MMU_WAFA_HALFB_RADDR_STATUSm);
    ARAD_MBMP_SET_DYNAMIC(MMU_WAFB_HALFA_RADDR_STATUSm);
    ARAD_MBMP_SET_DYNAMIC(MMU_WAFB_HALFB_RADDR_STATUSm);
    ARAD_MBMP_SET_DYNAMIC(MMU_WAFC_HALFA_RADDR_STATUSm);
    ARAD_MBMP_SET_DYNAMIC(MMU_WAFC_HALFB_RADDR_STATUSm);
    ARAD_MBMP_SET_DYNAMIC(MMU_WAFD_HALFA_RADDR_STATUSm);
    ARAD_MBMP_SET_DYNAMIC(MMU_WAFD_HALFB_RADDR_STATUSm);
    ARAD_MBMP_SET_DYNAMIC(MMU_WAFE_HALFA_RADDR_STATUSm);
    ARAD_MBMP_SET_DYNAMIC(MMU_WAFE_HALFB_RADDR_STATUSm);
    ARAD_MBMP_SET_DYNAMIC(MMU_WAFF_HALFA_RADDR_STATUSm);
    ARAD_MBMP_SET_DYNAMIC(MMU_WAFF_HALFB_RADDR_STATUSm);
    ARAD_MBMP_SET_DYNAMIC(MMU_WAFG_HALFA_RADDR_STATUSm);
    ARAD_MBMP_SET_DYNAMIC(MMU_WAFG_HALFB_RADDR_STATUSm);
    ARAD_MBMP_SET_DYNAMIC(MMU_WAFH_HALFA_RADDR_STATUSm);
    ARAD_MBMP_SET_DYNAMIC(MMU_WAFH_HALFB_RADDR_STATUSm);
    ARAD_MBMP_SET_DYNAMIC(MMU_WAF_HALFA_WADDRm);
    ARAD_MBMP_SET_DYNAMIC(MMU_WAF_HALFB_WADDRm);
    ARAD_MBMP_SET_DYNAMIC(NBI_MLF_RX_MEM_A_CTRLm);
    ARAD_MBMP_SET_DYNAMIC(NBI_MLF_RX_MEM_B_CTRLm);
    ARAD_MBMP_SET_DYNAMIC(NBI_MLF_TX_MEM_CTRLm);
    ARAD_MBMP_SET_DYNAMIC(NBI_RBINS_MEMm);
    ARAD_MBMP_SET_DYNAMIC(NBI_RLENG_MEMm);
    ARAD_MBMP_SET_DYNAMIC(NBI_RPKTS_MEMm);
    ARAD_MBMP_SET_DYNAMIC(NBI_RTYPE_MEMm);
    ARAD_MBMP_SET_DYNAMIC(NBI_TBINS_MEMm);
    ARAD_MBMP_SET_DYNAMIC(NBI_TLENG_MEMm);
    ARAD_MBMP_SET_DYNAMIC(NBI_TPKTS_MEMm);
    ARAD_MBMP_SET_DYNAMIC(NBI_TTYPE_MEMm);
    ARAD_MBMP_SET_DYNAMIC(OAMP_RMAPEM_MANAGEMENT_REQUESTm);
    ARAD_MBMP_SET_DYNAMIC(OCB_OCBM_EVENm);
    ARAD_MBMP_SET_DYNAMIC(OCB_OCBM_ODDm);
    ARAD_MBMP_SET_DYNAMIC(RTP_CRMAm);
    ARAD_MBMP_SET_DYNAMIC(RTP_DRMAm);
    ARAD_MBMP_SET_DYNAMIC(RTP_RRMAm);
    ARAD_MBMP_SET_DYNAMIC(SCH_BUCKET_DEFICIT__BDFm);
    ARAD_MBMP_SET_DYNAMIC(SCH_FLOW_INSTALLED_MEMORY__FIMm);
    ARAD_MBMP_SET_DYNAMIC(SCH_FLOW_STATUS_MEMORY__FSMm);
    ARAD_MBMP_SET_DYNAMIC(SCH_PORT_QUEUE_SIZE__PQSm);
    ARAD_MBMP_SET_DYNAMIC(SCH_TOKEN_MEMORY_CONTROLLER__TMCm);
}

/*
 * Returns TRUE if the memory is dynamic
 */
int arad_tbl_is_dynamic(soc_mem_t mem) {
    return ARAD_MBMP_IS_DYNAMIC(mem);
}

STATIC uint32
_soc_mem_sbusdma_clear_pcid(SOC_SAND_IN int unit,SOC_SAND_IN  soc_mem_t mem, 
                              SOC_SAND_IN int copyno, SOC_SAND_IN void *data)
{
  uint32    err = 0;

  int    index_min, index_max, blk;
  int    array_index_max = 0;
  int entry_indx, array_index;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  index_min = soc_mem_index_min(unit, mem);
  index_max = soc_mem_index_max(unit, mem);

  if (SOC_MEM_IS_ARRAY(unit, mem)) 
  {
    soc_mem_array_info_t *maip = SOC_MEM_ARRAY_INFOP(unit, mem);
    if (maip) 
    {
        array_index_max = maip->numels - 1;
    }
  }

  SOC_MEM_BLOCK_ITER(unit, mem, blk) 
  {
    if (copyno != COPYNO_ALL && copyno != blk) 
    {
        continue;
    }

    for(array_index = 0; array_index <= array_index_max; ++array_index) 
    {
      for(entry_indx = index_min; entry_indx <= index_max; ++entry_indx) 
      {
          err = soc_mem_array_write(
                  unit,
                  mem,
                  array_index,
                  blk,
                  entry_indx,
                  (void*)data
                );
          SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 40, exit);
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in _soc_mem_sbusdma_clear_pcid()",0,0);
}

#ifdef EMULATION_FAST_WRITE_INSTEAD_OF_DMA
STATIC inline int _arad_fill_table_with_entry_emulation_fast_write_callback(
   SOC_SAND_IN int unit, 
   SOC_SAND_IN int copyno, 
   SOC_SAND_IN int array_index, 
   SOC_SAND_IN int index, 
   SOC_SAND_OUT uint32 *value, 
   SOC_SAND_IN int entry_sz,
   SOC_SAND_IN void *opaque)
{
  const uint32 *data = opaque;
  int entry_word_idx;
  for (entry_word_idx = 0; entry_word_idx < entry_sz; entry_word_idx++) {
    value[entry_word_idx] = data[entry_word_idx];
  }
  return 0;
}

STATIC int inline _arad_tbl_access_get_entry_bit_len(int unit, soc_mem_t mem)
{
  int field_idx;
  int entry_bit_len = -1;

  /* Find the biggest field length. */
  for (field_idx = 0; field_idx < SOC_MEM_INFO(unit, mem).nFields; field_idx++) {
    uint16 cur_len = SOC_MEM_INFO(unit, mem).fields[field_idx].bp + SOC_MEM_INFO(unit, mem).fields[field_idx].len;
    entry_bit_len = SOC_SAND_MAX(entry_bit_len, cur_len);
  }

  return entry_bit_len;
}

#endif /* EMULATION_FAST_WRITE_INSTEAD_OF_DMA */

/* Fill the whole table with the given entry, uses fast DMA filling when run on real hardware */
uint32
  arad_fill_table_with_entry(
    SOC_SAND_IN int       unit,
    SOC_SAND_IN soc_mem_t mem,
    SOC_SAND_IN int       copyno,
    SOC_SAND_IN void      *data
  )
{
  uint32    res = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (SOC_IS_JERICHO(unit)) {
    res = dcmn_fill_table_with_entry(unit, mem, copyno, data);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 222, exit);
    goto exit;
  }

#ifdef EMULATION_FAST_WRITE_INSTEAD_OF_DMA

  if (SOC_DPP_CONFIG(unit)->arad->init.arad_emulation_system || _arad_tbl_access_emulation_fast_write_debug) {
    /* If the size of the table is lower than this, then even in emulation it is written directly, instead of doing fast write. */
    const int direct_write_table_size_threshold = 100;    
    int tbl_nof_entries = SOC_MEM_INFO(unit, mem).index_max - SOC_MEM_INFO(unit, mem).index_min + 1;

    /* If the data is 0, then we skip, since we assume the emulation starts with zeroed memory. */
    if (!SHR_BITNULL_RANGE(data, 0, _arad_tbl_access_get_entry_bit_len(unit, mem)) && tbl_nof_entries >= direct_write_table_size_threshold) {
      /* Otherwise, if the table is large enough, we use the callback to write the value. */
      soc_error_t new_rv;
      new_rv = arad_fill_table_with_variable_values_by_caching(unit, mem, 0, copyno, -1, -1, 
                                                               _arad_fill_table_with_entry_emulation_fast_write_callback, data);
      if (new_rv != SOC_E_NONE) {
        SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 123, exit);
      }
    } 

    /* In emulation debug we still want to write the table. */
    /* Additionally if the table is small enough we just write it directly, even in the emulation. */
    if (!_arad_tbl_access_emulation_fast_write_debug && tbl_nof_entries >= direct_write_table_size_threshold) {
      SOC_SAND_EXIT_NO_ERROR;
    }
  }
#endif /* EMULATION_FAST_WRITE_INSTEAD_OF_DMA */

#ifdef PLISIM
  if (SAL_BOOT_PLISIM) {
      res = _soc_mem_sbusdma_clear_pcid(unit, mem, copyno, data);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else
#endif
  {
    if (!soc_mem_slamable(unit, mem, copyno))
    {
      res = _soc_mem_sbusdma_clear_pcid(unit, mem, copyno, data);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }
    else {
      res = _soc_mem_sbusdma_clear(unit, mem, copyno, (void*)data);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fill_table_with_entry()",0,0);
}

/* Fill the specified part of the table with the given entry, uses fast DMA filling when run on real hardware */
uint32
  arad_fill_partial_table_with_entry(
    SOC_SAND_IN int       unit,
    SOC_SAND_IN soc_mem_t mem,
    SOC_SAND_IN unsigned  array_index_start,
    SOC_SAND_IN unsigned  array_index_end,
    SOC_SAND_IN int       copyno,
    SOC_SAND_IN int       index_start,
    SOC_SAND_IN int       index_end,
    SOC_SAND_IN void      *data
  )
{
  uint32 err = 0;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (!soc_mem_slamable(unit, mem, copyno)
#ifdef PLISIM
      || (SAL_BOOT_PLISIM)
#endif
   )
  {
    int index, blk;
    unsigned array_index;

    SOC_MEM_BLOCK_ITER(unit, mem, blk) 
    {
      if (copyno != COPYNO_ALL && copyno != blk) {
        continue;
      }
      for (array_index = array_index_start; array_index <= array_index_end; ++array_index) 
      {
        for (index = index_start; index <= index_end; ++index) 
        {
          err = soc_mem_array_write(unit, mem, array_index, blk, index, (void*)data);
          SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 10, exit);
        }
      }
    }
  }
else {
    err = _soc_mem_sbusdma_clear_specific(unit, mem, array_index_start, array_index_end, copyno, index_start, index_end, (void*)data);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_fill_partial_table_with_entry()",0,0);
}

#ifdef EMULATION_FAST_WRITE_INSTEAD_OF_DMA

#include <sal/appl/io.h>
extern size_t strnlen(const char *s, size_t maxlen);
extern int system(const char *command);

#define EMULATION_DUMP_MEM_FILE_NAME_FORMAT "tmp_emul_mem_%s.dump"

/**
 * Starts the dump by opening a file to dump to. 
 * @f should later be used for fast_write_do_buffer and fast_write_end.
 * 
 * @param unit 
 * @param mem - The memory to dump
 * @param f - The opened file.
 * 
 * @return soc_error_t 
 */
STATIC soc_error_t _arad_fill_table_with_variable_values_by_caching_emulation_fast_write_begin(
    SOC_SAND_IN int       unit,
    SOC_SAND_IN soc_mem_t mem,
    SOC_SAND_OUT FILE **f
    )
{
  char fname[1024];

  SOCDNX_INIT_FUNC_DEFS;

  sal_snprintf(fname, sizeof(fname), EMULATION_DUMP_MEM_FILE_NAME_FORMAT , SOC_MEM_NAME(unit, mem));
  *f = sal_fopen(fname, "w");
  if (!f) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("There was an error opening file %s.\n"), fname));
  }

exit:
  SOCDNX_FUNC_RETURN;
}

/**
 * This function dumps a buffer to the dump file. 
 * The indexes are only used to calculate the length to dump, and are written directly to file without padding or overwriting older do_buffer calls. 
 * 
 * @param unit 
 * @param mem - The memory to dump.
 * @param index_start - The index to start at.
 * @param index_end - The index to stop at.
 * @param entry_data - The buffer to dump to file.
 * @param f - A file opened by fast_write_begin. 
 * 
 * @return soc_error_t 
 */
STATIC soc_error_t _arad_fill_table_with_variable_values_by_caching_emulation_fast_write_do_buffer(
    SOC_SAND_IN int       unit,
    SOC_SAND_IN soc_mem_t mem,
    SOC_SAND_IN int       index_start,
    SOC_SAND_IN int       index_end,
    SOC_SAND_IN uint32 *  entry_data,
    SOC_SAND_INOUT FILE *f
  )
{
  int entry_idx;
  int entry_word_index;
  int entry_size_in_words;
  int entry_bit_len = -1;
  int entry_hex_digit_len;
  int entry_last_word_index;
  int n;
  
  SOCDNX_INIT_FUNC_DEFS;

  /* Size of entry in words. */
  entry_size_in_words = soc_mem_entry_words(unit, mem);
  /* Length of entry in bits. */
  entry_bit_len = _arad_tbl_access_get_entry_bit_len(unit, mem);
  /* Length of entry in hex digits, equal to length in bits divided by 4 and rounded up. */
  entry_hex_digit_len = (entry_bit_len + 3) / 4;
  /* Index of the most significant word in the entry. */
  entry_last_word_index = entry_size_in_words - 1;

  /* First write the buffer to file. */
  for (entry_idx = 0; entry_idx < index_end - index_start + 1; entry_idx++) {
    /* For each entry print words from last to first. */
    /* In the last word print only hex digits with mask != 0 (i.e. at least a single bit is used in the hex digit boundary). */
    /* Example: If the entry is 9 bits, then print 3 hex digits. */

    /* The rest of the words are printed in full. */
    for (entry_word_index = entry_last_word_index; entry_word_index >= 0; entry_word_index--) {
      int hex_digit_num;
      int bit_num;
      uint32 word_bit_mask;

      /* How many hex digits to print in this word? */
      hex_digit_num = entry_hex_digit_len - entry_word_index * 8;
      hex_digit_num = SOC_SAND_MIN(hex_digit_num, 8);

      bit_num = entry_bit_len - entry_word_index * 32;
      bit_num = SOC_SAND_MIN(bit_num, 32);

      word_bit_mask = bit_num == 32 ? ~(uint32)0 : (1 << bit_num) - 1;
      
      n = sal_fprintf(f, "%0*X", hex_digit_num, entry_data[entry_idx * entry_size_in_words + entry_word_index] & word_bit_mask);
      SOCDNX_VERIFY(n == hex_digit_num);
    }

    n = sal_fprintf(f, "\n");
    SOCDNX_VERIFY(n == 1);
    
  }

exit:
  SOCDNX_FUNC_RETURN;
}

/**
 * When done doing fast_write_do_buffer this function should be called to flush the file and then issue the fast write command.
 * 
 * @param unit 
 * @param mem - The memory to do fast write for.
 * @param f - The file opened by fast_write_begin.
 * 
 * @return STATIC soc_error_t  
 */
STATIC soc_error_t _arad_fill_table_with_variable_values_by_caching_emulation_fast_write_end(
    SOC_SAND_IN int       unit,
    SOC_SAND_IN soc_mem_t mem,
    SOC_SAND_INOUT FILE *f
    )
{
  char cmd[1024];
  char fname[1024];
  int int_ret;

  SOCDNX_INIT_FUNC_DEFS;

  sal_fclose(f);

  sal_snprintf(fname, sizeof(fname), EMULATION_DUMP_MEM_FILE_NAME_FORMAT, SOC_MEM_NAME(unit, mem));
  
  int_ret = snprintf(cmd, sizeof(cmd), "python fast-write.py %s", fname);
  SOCDNX_VERIFY(int_ret <= sizeof(cmd));

  /* Next issue the fast write command. */
  int_ret = system(cmd);
  if (int_ret == -1) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("There was an error executing system(%s).\n"), cmd));
  } else if (int_ret == 1) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("There was an error doing fast-write (system(%s)). For details see the server log.\n"), cmd));
  } else if (int_ret != 0) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_FAIL, (_BSL_SOCDNX_MSG("There was an error executing system(%s).\n"), cmd));
  }

exit:
  SOCDNX_FUNC_RETURN;
}


#endif /* EMULATION_FAST_WRITE_INSTEAD_OF_DMA */

soc_error_t arad_tbl_access_deinit(SOC_SAND_IN int unit)
{

  SOCDNX_INIT_FUNC_DEFS;

  /* If it is PCID, the function returns. */
#ifdef PLISIM
  if (SAL_BOOT_PLISIM) {
    SOCDNX_FUNC_RETURN;
  }
#endif

  if (_arad_fill_table_with_variable_values_by_caching_buffer[unit]) {
    soc_cm_sfree(unit, _arad_fill_table_with_variable_values_by_caching_buffer[unit]);
    _arad_fill_table_with_variable_values_by_caching_buffer[unit] = NULL;
  }

  if (_arad_fill_table_with_variable_values_by_caching_buffer_lock[unit]) {
    sal_mutex_destroy(_arad_fill_table_with_variable_values_by_caching_buffer_lock[unit]);
    _arad_fill_table_with_variable_values_by_caching_buffer_lock[unit] = NULL;
  }

  SOCDNX_FUNC_RETURN;
}


soc_error_t arad_tbl_access_init_unsafe(SOC_SAND_IN int unit)
{

  SOCDNX_INIT_FUNC_DEFS;

  /* If it is PCID, the function returns. */
#ifdef PLISIM
  if (SAL_BOOT_PLISIM) {
    SOCDNX_FUNC_RETURN;
  }
#endif

  _arad_fill_table_with_variable_values_by_caching_buffer_lock[unit] = sal_mutex_create("_arad_fill_table_with_variable_values_by_caching_buffer_lock[unit]");
  if (_arad_fill_table_with_variable_values_by_caching_buffer_lock[unit] == NULL) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Could not allocate memory for a mutex.\n")));
  }

  /* Allocate 64KB of DMA memory. */
  _arad_fill_table_with_variable_values_by_caching_buffer[unit] = soc_cm_salloc(unit, _arad_fill_table_with_variable_values_by_caching_buffer_len * 
                                                                      sizeof(*_arad_fill_table_with_variable_values_by_caching_buffer[0]), 
                                                                      "_arad_fill_table_with_variable_values_by_caching_buffer[unit]");

  if (_arad_fill_table_with_variable_values_by_caching_buffer[unit] == NULL) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("Could not allocate a DMA buffer of length %d bytes.\n"), 
                                          _arad_fill_table_with_variable_values_by_caching_buffer_len * 
                                                                      sizeof(*_arad_fill_table_with_variable_values_by_caching_buffer[0])));
  }

exit:
  SOCDNX_FUNC_RETURN;
}

soc_error_t 
  arad_fill_table_with_variable_values_by_caching(
    SOC_SAND_IN int       unit,
    SOC_SAND_IN soc_mem_t mem,
    SOC_SAND_IN unsigned  array_index,
    SOC_SAND_IN int       copyno,
    SOC_SAND_IN int       index_start,
    SOC_SAND_IN int       index_end,
    SOC_SAND_IN arad_fill_table_with_variable_values_by_caching_callback callback,
    SOC_SAND_IN void *    opaque
  )
{
  int index;
  /* Index inside buffer */
  unsigned int buf_idx = 0;
  /* Index to first entry of the buffer. */
  unsigned int buf_start_entry_idx;
  uint32 *buf = _arad_fill_table_with_variable_values_by_caching_buffer[unit];
  uint32 old_rv;
#ifdef EMULATION_FAST_WRITE_INSTEAD_OF_DMA
  soc_error_t new_rv;
  uint8 do_emul = SOC_DPP_CONFIG(unit)->arad->init.arad_emulation_system || _arad_tbl_access_emulation_fast_write_debug ? TRUE : FALSE;
  FILE *emul_dump_file;
#endif /* EMULATION_FAST_WRITE_INSTEAD_OF_DMA */
  uint8 do_dma = TRUE;
  int real_index_start;
  int real_index_end;
  int entry_size_in_words;
  int buffer_len_in_words = _arad_fill_table_with_variable_values_by_caching_buffer_len / 4;
  
  SOCDNX_INIT_FUNC_DEFS;

  if (!soc_mem_is_valid(unit, mem)) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_MEMORY, (_BSL_SOCDNX_MSG("%d: Invalid memory specified for unit %d.\n"),  mem, unit));
  }

  if (callback == NULL) {
    SOC_EXIT;
  }

  entry_size_in_words = soc_mem_entry_words(unit, mem);
  SOCDNX_VERIFY(entry_size_in_words <= buffer_len_in_words);
  
  real_index_start = index_start == -1 ? SOC_MEM_INFO(unit, mem).index_min : index_start;
  real_index_end = index_end == -1 ? SOC_MEM_INFO(unit, mem).index_max : index_end;
  
  /* If no DMA we use direct write. */
  if (!soc_mem_slamable(unit, mem, copyno)
#ifdef PLISIM
      || (SAL_BOOT_PLISIM)
#endif
      )
  {
    int index, blk;
    uint32 buf[SOC_MAX_MEM_WORDS];

    SOC_MEM_BLOCK_ITER(unit, mem, blk) 
    {
      if (copyno != COPYNO_ALL && copyno != blk) {
        continue;
      }
      for (index = real_index_start; index <= real_index_end; ++index) 
      {
        sal_memset(buf, 0, sizeof(buf[0]) * entry_size_in_words);

        if (callback(unit, copyno, array_index, index, buf, entry_size_in_words, opaque) != 0) {
          SOC_EXIT;
        }

        old_rv = soc_mem_array_write(unit, mem, array_index, blk, index, (void*)&buf);
        SOCDNX_SAND_IF_ERR_EXIT(old_rv);
      }
    }

    SOCDNX_FUNC_RETURN;
  }

  SOCDNX_VERIFY(buf != NULL);

#ifdef EMULATION_FAST_WRITE_INSTEAD_OF_DMA
  /* In emulation begin the fast write. */
  if (do_emul) {
    new_rv = _arad_fill_table_with_variable_values_by_caching_emulation_fast_write_begin(unit, mem, &emul_dump_file);
    SOCDNX_IF_ERR_EXIT(new_rv);
  }
#endif /* EMULATION_FAST_WRITE_INSTEAD_OF_DMA */

  sal_mutex_take(_arad_fill_table_with_variable_values_by_caching_buffer_lock[unit], sal_mutex_FOREVER);
    
  for (index = real_index_start, buf_start_entry_idx = real_index_start; index <= real_index_end; ++index) 
  {
    /* First clear the entry and then fill it. */
    sal_memset(buf + buf_idx, 0, sizeof(buf[0]) * entry_size_in_words);
    if (callback(unit, copyno, array_index, index, buf + buf_idx, entry_size_in_words, opaque) != 0) {
      SOC_EXIT;
    }

    buf_idx += entry_size_in_words;
    /* In case entry_sz == 1, last (fillable) entry is len - 1. */
    /* In case entry_sz > 1, last (fillable) entry is len - entry_sz. */
    /* Therefore if (buf_idx > len - entry_sz) we must flush the buffer.*/
    if (buf_idx > buffer_len_in_words - entry_size_in_words) {
      /* Flush the buffer. */
#ifdef EMULATION_FAST_WRITE_INSTEAD_OF_DMA
      if (do_emul) {
        do_dma = FALSE;
        new_rv = _arad_fill_table_with_variable_values_by_caching_emulation_fast_write_do_buffer(unit, mem, buf_start_entry_idx, index, buf, emul_dump_file);
        SOCDNX_IF_ERR_EXIT(new_rv);
      } 
#endif /* EMULATION_FAST_WRITE_INSTEAD_OF_DMA */
      if (do_dma) {
/*        old_rv = soc_mem_write_range(unit, mem, copyno, buf_start_entry_idx, index, buf);*/
        old_rv = soc_mem_array_write_range(unit, 0, mem, array_index, copyno, buf_start_entry_idx, index, buf);

        SOCDNX_SAND_IF_ERR_EXIT(old_rv);
      }

      buf_start_entry_idx = index + 1;
      buf_idx = 0;
      
    }
  }

  if (buf_start_entry_idx <= real_index_end) {
    /* Flush the buffer. */
#ifdef EMULATION_FAST_WRITE_INSTEAD_OF_DMA
    if (do_emul) {
      do_dma = FALSE;
      new_rv = _arad_fill_table_with_variable_values_by_caching_emulation_fast_write_do_buffer(unit, mem, buf_start_entry_idx, index - 1, buf, emul_dump_file);
      SOCDNX_IF_ERR_EXIT(new_rv);
    } 
#endif /* EMULATION_FAST_WRITE_INSTEAD_OF_DMA */
    if (do_dma) {
/*      old_rv = soc_mem_write_range(unit, mem, copyno, buf_start_entry_idx, index - 1, buf);*/
      old_rv = soc_mem_array_write_range(unit, 0, mem, array_index, copyno, buf_start_entry_idx, index - 1, buf);
      SOCDNX_SAND_IF_ERR_EXIT(old_rv);
    }

  }

  sal_mutex_give(_arad_fill_table_with_variable_values_by_caching_buffer_lock[unit]);

#ifdef EMULATION_FAST_WRITE_INSTEAD_OF_DMA
  /* In emulation end the fast write. */
  if (do_emul) {
    new_rv = _arad_fill_table_with_variable_values_by_caching_emulation_fast_write_end(unit, mem, emul_dump_file);
    SOCDNX_IF_ERR_EXIT(new_rv);
  }
#endif /* EMULATION_FAST_WRITE_INSTEAD_OF_DMA */

exit:
  SOCDNX_FUNC_RETURN;
}

int
arad_fill_memory_with_incremental_field(const int unit, const soc_mem_t mem, const soc_field_t field,
                                unsigned array_index_min, unsigned array_index_max,
                                const int copyno,
                                int index_min, int index_max,
                                const void *initial_entry)
{
    int    rv = 0, mem_words, mem_size, entry_words, indices_num;
    int    index, blk, tmp;
    unsigned array_index;
    uint32 *buf;
    uint32 *buf2;
    const uint32 *input_entry = initial_entry;
    uint32 field_buf[4] = {0}; /* To hold the field, max size 128 bits */
    int dmaable = soc_mem_dmaable(unit, mem, SOC_MEM_BLOCK_ANY(unit, mem)); /* check if we can use DMA */

    if (initial_entry == NULL) {
        return SOC_E_PARAM;
    }

    /* get legal values for indices, if too small/big use the memory's boundaries */
    tmp = soc_mem_index_min(unit, mem);
    if (index_min < soc_mem_index_min(unit, mem)) {
      index_min = tmp;
    }
    if (index_max < index_min) {
        index_max = index_min;
    } else {
         tmp = soc_mem_index_max(unit, mem);
         if (index_max > tmp) {
             index_max = tmp;
         }
    }

    entry_words = soc_mem_entry_words(unit, mem);
    indices_num = index_max - index_min + 1;
    mem_words = indices_num * entry_words;
    mem_size = mem_words * 4;

    /* get the initial field from the input */
    soc_mem_field_get(unit, mem, initial_entry, field, field_buf);

    if (dmaable) {
        buf = soc_cm_salloc(unit, mem_size, "mem_clear_buf"); /* allocate DMA memory buffer */
    } else {
        buf = (uint32*)soc_sand_os_malloc_any_size(mem_size, "mem_clear_buf");
    }
    if (buf == NULL) {
        return SOC_E_MEMORY;
    }

    /* get legal values for memory array indices */
    if (SOC_MEM_IS_ARRAY(unit, mem)) {
        soc_mem_array_info_t *maip = SOC_MEM_ARRAY_INFOP(unit, mem);
        if (maip) {
            if (array_index_max >= maip->numels) {
                array_index_max = maip->numels - 1;
            }
        } else {
            array_index_max = 0;
        }
        if (array_index_min > array_index_max) {
            array_index_min = array_index_max;
        }
    } else {
        array_index_min = array_index_max = 0;
    }

    /* fill the allocated memory with the input entry */
    for (index = 0; index < mem_words; ++index) {
        buf[index] = input_entry[index % entry_words];
    }

    SOC_MEM_BLOCK_ITER(unit, mem, blk) {
        if (copyno != COPYNO_ALL && copyno != blk) {
            continue;
        }
        for (array_index = array_index_min; array_index <= array_index_min; ++array_index) {
            /* update the field of all the entries in the buffer */
            for (index = 0, buf2 = buf; index < indices_num; ++index, buf2+=entry_words) {
                soc_mem_field_set(unit, mem, buf2, field, field_buf); /* set the index */
                /* increment the field, to be used in next entry */
                if (!++field_buf[0]) {
                    if (!++field_buf[1]) {
                        if (!++field_buf[2]) {
                            ++field_buf[3];
                        }
                    }
                }
            }

#ifdef PLISIM
            if (SAL_BOOT_PLISIM) {
                for (index = index_min, buf2 = buf; index <= index_max; ++index, buf2+=entry_words) {
                    if ((rv = soc_mem_array_write(unit, mem, array_index, blk, index, buf2)) < 0) {
                        LOG_INFO(BSL_LS_SOC_MEM,
                                 (BSL_META_U(unit,
                                             "Write ERROR: table %s.%d[%d]: %s\n"),
                                             SOC_MEM_UFNAME(unit, mem), copyno==COPYNO_ALL ? 0 : copyno, index,
                                  soc_errmsg(rv)));
                    }
                }
            }
            else 
#endif
            {
                if ((rv = soc_mem_array_write_range(unit, 0, mem, array_index, blk, index_min, index_max, buf)) < 0) {
                    LOG_INFO(BSL_LS_SOC_MEM,
                             (BSL_META_U(unit,
                                         "Write ERROR: table %s.%d[%d-%d]: %s\n"),
                                         SOC_MEM_UFNAME(unit, mem), copyno==COPYNO_ALL ? 0 : copyno, index_min, index_max,
                              soc_errmsg(rv)));
                }
            }
        }
    }
    if (dmaable) {
        soc_cm_sfree(unit, buf);
    } else  {
        soc_sand_os_free(buf);
    }
    return rv;
}



void arad_tbl_access_tcam_print(SOC_SAND_IN ARAD_PP_IHB_TCAM_BANK_TBL_DATA *data)
{
#if ARAD_PP_TBL_ACC_TCAM_LOW_LEVEL_DEBUG_PRINT
    uint32
        indx;
    char
        string_to_print[10*ARAD_PP_IHB_TCAM_DATA_WIDTH + 20];

    if(!data->valid) {
        LOG_DEBUG(BSL_LS_SOC_MEM,
                  (BSL_META(SOC_DPP_MSG("      "
"Entry is invalid \n\r"))));
        return;
    }

    sal_sprintf(string_to_print, "      ");
    sal_sprintf(string_to_print + sal_strlen(string_to_print) , " ");
    for(indx = 0; indx < ARAD_PP_IHB_TCAM_DATA_WIDTH; ++indx) {
        sal_sprintf(string_to_print + sal_strlen(string_to_print) ,"%08x:", data->value[ARAD_PP_IHB_TCAM_DATA_WIDTH-indx-1]);
    }
    LOG_DEBUG(BSL_LS_SOC_MEM,
              (BSL_META(SOC_DPP_MSG("%s\n\r"), string_to_print)));
    
    sal_sprintf(string_to_print, "      ");
    sal_sprintf(string_to_print + sal_strlen(string_to_print) ," ");
    for(indx = 0; indx < ARAD_PP_IHB_TCAM_DATA_WIDTH; ++indx) {
        sal_sprintf(string_to_print + sal_strlen(string_to_print) ,"%08x:", data->mask[ARAD_PP_IHB_TCAM_DATA_WIDTH-indx-1]);
    }
    LOG_DEBUG(BSL_LS_SOC_MEM,
              (BSL_META(SOC_DPP_MSG("%s\n\r"), string_to_print)));
#endif
    return;
}
#if ARAD_PP_TBL_ACC_TCAM_LOW_LEVEL_DEBUG_PRINT
STATIC 
void arad_tbl_access_buffer_print(uint32 *data, uint32 len)
{
    uint32
        indx;

    for(indx = 0; indx < len; ++indx) {
        LOG_CLI((BSL_META("%08x:"), data[len-indx-1]));
    }
    LOG_CLI((BSL_META("\n\r")));
    return;
}

#endif

/*
 * Read indirect table nif_ctxt_map_tbl from block IRE,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_ire_nif_ctxt_map_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IRE_NIF_CTXT_MAP_TBL_DATA* IRE_nif_ctxt_map_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_IRE_NIF_CTXT_MAP_TBL_ENTRY_SIZE];
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IRE_NIF_CTXT_MAP_TBL_GET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_sand_os_memset(
          IRE_nif_ctxt_map_tbl_data,
          0x0,
          sizeof(ARAD_IRE_NIF_CTXT_MAP_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_mem_read(
          unit,
          IRE_NIF_CTXT_MAPm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

    err=SOC_E_NONE;
    if(SOC_E_NONE == err) {
          IRE_nif_ctxt_map_tbl_data->reassembly_context = soc_mem_field32_get(
                  unit,
                  IRE_NIF_CTXT_MAPm,
                  data,
                  REASSEMBLY_CONTEXTf);
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 35, exit);

    if(SOC_E_NONE == err) {
          IRE_nif_ctxt_map_tbl_data->port_termination_context = soc_mem_field32_get(
                  unit,
                  IRE_NIF_CTXT_MAPm,
                  data,
                  PORT_TERMINATION_CONTEXTf);
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ire_nif_ctxt_map_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table nif_ctxt_map_tbl from block IRE,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_ire_nif_ctxt_map_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IRE_NIF_CTXT_MAP_TBL_DATA* IRE_nif_ctxt_map_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_IRE_NIF_CTXT_MAP_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IRE_NIF_CTXT_MAP_TBL_SET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  soc_mem_field32_set(
          unit,
          IRE_NIF_CTXT_MAPm,
          data,
          REASSEMBLY_CONTEXTf,
          IRE_nif_ctxt_map_tbl_data->reassembly_context );

  soc_mem_field32_set(
          unit,
          IRE_NIF_CTXT_MAPm,
          data,
          PORT_TERMINATION_CONTEXTf,
          IRE_nif_ctxt_map_tbl_data->port_termination_context );

  err = soc_mem_write(
          unit,
          IRE_NIF_CTXT_MAPm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ire_nif_ctxt_map_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table rcy_ctxt_map_tbl from block IRE,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_ire_rcy_ctxt_map_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IRE_RCY_CTXT_MAP_TBL_DATA* IRE_rcy_ctxt_map_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_IRE_RCY_CTXT_MAP_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IRE_RCY_CTXT_MAP_TBL_GET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_sand_os_memset(
          IRE_rcy_ctxt_map_tbl_data,
          0x0,
          sizeof(ARAD_IRE_RCY_CTXT_MAP_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_mem_read(
          unit,
          IRE_RCY_CTXT_MAPm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);


  IRE_rcy_ctxt_map_tbl_data->reassembly_context   = soc_mem_field32_get(
                  unit,
                  IRE_RCY_CTXT_MAPm,
                  data,
                  REASSEMBLY_CONTEXTf            );

  IRE_rcy_ctxt_map_tbl_data->port_termination_context   = soc_mem_field32_get(
                  unit,
                  IRE_RCY_CTXT_MAPm,
                  data,
                  PORT_TERMINATION_CONTEXTf            );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ire_rcy_ctxt_map_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table rcy_ctxt_map_tbl from block IRE,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_ire_rcy_ctxt_map_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IRE_RCY_CTXT_MAP_TBL_DATA* IRE_rcy_ctxt_map_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_IRE_RCY_CTXT_MAP_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IRE_RCY_CTXT_MAP_TBL_SET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  soc_mem_field32_set(
          unit,
          IRE_RCY_CTXT_MAPm,
          data,
          PORT_TERMINATION_CONTEXTf,
          IRE_rcy_ctxt_map_tbl_data->port_termination_context );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

  soc_mem_field32_set(
          unit,
          IRE_RCY_CTXT_MAPm,
          data,
          REASSEMBLY_CONTEXTf,
          IRE_rcy_ctxt_map_tbl_data->reassembly_context );

  err = soc_mem_write(
          unit,
          IRE_RCY_CTXT_MAPm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ire_rcy_ctxt_map_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table rcy_ctxt_map_tbl from block IRE,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_ire_cpu_ctxt_map_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IRE_CPU_CTXT_MAP_TBL_DATA* IRE_cpu_ctxt_map_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_IRE_CPU_CTXT_MAP_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IRE_CPU_CTXT_MAP_TBL_GET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_sand_os_memset(
          IRE_cpu_ctxt_map_tbl_data,
          0x0,
          sizeof(ARAD_IRE_CPU_CTXT_MAP_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_mem_read(
          unit,
          IRE_CPU_CTXT_MAPm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);


    err=SOC_E_NONE;
    if(SOC_E_NONE == err) {
          IRE_cpu_ctxt_map_tbl_data->reassembly_context   = soc_mem_field32_get(
                  unit,
                  IRE_CPU_CTXT_MAPm,
                  data,
                  REASSEMBLY_CONTEXTf            );
  }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 35, exit);

    if(SOC_E_NONE == err) {
          IRE_cpu_ctxt_map_tbl_data->port_termination_context   = soc_mem_field32_get(
                  unit,
                  IRE_CPU_CTXT_MAPm,
                  data,
                  PORT_TERMINATION_CONTEXTf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ire_cpu_ctxt_map_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table rcy_ctxt_map_tbl from block IRE,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_ire_cpu_ctxt_map_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IRE_CPU_CTXT_MAP_TBL_DATA* IRE_cpu_ctxt_map_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_IRE_CPU_CTXT_MAP_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IRE_CPU_CTXT_MAP_TBL_SET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

err=SOC_E_NONE;
if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          IRE_CPU_CTXT_MAPm,
          data,
          PORT_TERMINATION_CONTEXTf,
          IRE_cpu_ctxt_map_tbl_data->port_termination_context );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          IRE_CPU_CTXT_MAPm,
          data,
          REASSEMBLY_CONTEXTf,
          IRE_cpu_ctxt_map_tbl_data->reassembly_context );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_mem_write(
          unit,
          IRE_CPU_CTXT_MAPm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ire_cpu_ctxt_map_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table dynamic_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_iqm_dynamic_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IQM_DYNAMIC_TBL_DATA* IQM_dynamic_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_IQM_DYNAMIC_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IQM_DYNAMIC_TBL_GET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_sand_os_memset(
          IQM_dynamic_tbl_data,
          0x0,
          sizeof(ARAD_IQM_DYNAMIC_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_mem_read(
          unit,
          IQM_PQDMDm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

    err=SOC_E_NONE;
    if(SOC_E_NONE == err) {
          IQM_dynamic_tbl_data->pq_head_ptr   = soc_mem_field32_get(
                  unit,
                  IQM_PQDMDm,
                  data,
                  PQ_HEAD_PTRf          );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 40, exit);

    if(SOC_E_NONE == err) {
          IQM_dynamic_tbl_data->que_not_empty   = soc_mem_field32_get(
                  unit,
                  IQM_PQDMDm,
                  data,
                  QUE_NOT_EMPTYf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 41, exit);

    if(SOC_E_NONE == err) {
          IQM_dynamic_tbl_data->pq_inst_que_size   = soc_mem_field32_get(
                  unit,
                  IQM_PQDMDm,
                  data,
                  PQ_INST_QUE_SIZEf          );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 42, exit);

    if(SOC_E_NONE == err) {
          IQM_dynamic_tbl_data->pq_inst_que_buff_size   = soc_mem_field32_get(
                  unit,
                  IQM_PQDMDm,
                  data,
                  PQ_INST_QUE_BUFF_SIZEf        );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 42, exit);

    if(SOC_E_NONE == err) {
          IQM_dynamic_tbl_data->pq_avrg_szie   = soc_mem_field32_get(
                  unit,
                  IQM_PQDMDm,
                  data,
                  PQ_AVRG_SIZEf               );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 43, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_iqm_dynamic_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table dynamic_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_iqm_dynamic_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IQM_DYNAMIC_TBL_DATA* IQM_dynamic_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_IQM_DYNAMIC_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IQM_DYNAMIC_TBL_SET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

 
 




err=SOC_E_NONE;
if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          IQM_MEM_8000000m,
          data,
          ITEM_0_20f /* PQ_HEAD_PTRf */,
          IQM_dynamic_tbl_data->pq_head_ptr );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          IQM_MEM_8000000m,
          data,
          ITEM_21_21f /* QUE_NOT_EMPTYf */,
          IQM_dynamic_tbl_data->que_not_empty );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 31, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          IQM_MEM_8000000m,
          data,
          ITEM_22_48f /* PQ_INST_QUE_SIZEf */,
          IQM_dynamic_tbl_data->pq_inst_que_size );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 32, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          IQM_MEM_8000000m,
          data,
          ITEM_49_69f /* PQ_INST_QUE_BUFF_SIZE */,
          IQM_dynamic_tbl_data->pq_inst_que_buff_size );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 32, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          IQM_MEM_8000000m,
          data,
          ITEM_70_81f /* PQ_AVRG_SZIEf */,
          IQM_dynamic_tbl_data->pq_avrg_szie );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 33, exit);

  err = soc_mem_write(
          unit,
          IQM_MEM_8000000m,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 34, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_iqm_dynamic_tbl_set_unsafe()",0,0);
}
/*
 * Read indirect table static_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_iqm_static_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IQM_STATIC_TBL_DATA* IQM_static_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_IQM_STATIC_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IQM_STATIC_TBL_GET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_sand_os_memset(
          IQM_static_tbl_data,
          0x0,
          sizeof(ARAD_IQM_STATIC_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

 
  err = soc_mem_read(
          unit,
          IQM_PQDMSm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

    err=SOC_E_NONE;
    if(SOC_E_NONE == err) {
          IQM_static_tbl_data->credit_class   = soc_mem_field32_get(
                  unit,
                  IQM_PQDMSm,
                  data,
                  CREDIT_CLASSf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 40, exit);

    if(SOC_E_NONE == err) {
          IQM_static_tbl_data->rate_class   = soc_mem_field32_get(
                  unit,
                  IQM_PQDMSm,
                  data,
                  RATE_CLASSf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 41, exit);

    if(SOC_E_NONE == err) {
          IQM_static_tbl_data->connection_class   = soc_mem_field32_get(
                  unit,
                  IQM_PQDMSm,
                  data,
                  CONNECTION_CLASSf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 42, exit);

    if(SOC_E_NONE == err) {
          IQM_static_tbl_data->traffic_class   = soc_mem_field32_get(
                  unit,
                  IQM_PQDMSm,
                  data,
                  TRAFFIC_CLASSf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 43, exit);

    if(SOC_E_NONE == err) {
          IQM_static_tbl_data->que_signature   = soc_mem_field32_get(
                  unit,
                  IQM_PQDMSm,
                  data,
                  QUE_SIGNATUREf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 44, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_iqm_static_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table static_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_iqm_static_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IQM_STATIC_TBL_DATA* IQM_static_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_IQM_STATIC_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IQM_STATIC_TBL_SET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

 
 




err=SOC_E_NONE;
if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          IQM_PQDMSm,
          data,
          CREDIT_CLASSf,
          IQM_static_tbl_data->credit_class );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          IQM_PQDMSm,
          data,
          RATE_CLASSf,
          IQM_static_tbl_data->rate_class );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 31, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          IQM_PQDMSm,
          data,
          CONNECTION_CLASSf,
          IQM_static_tbl_data->connection_class );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 32, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          IQM_PQDMSm,
          data,
          TRAFFIC_CLASSf,
          IQM_static_tbl_data->traffic_class );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 33, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          IQM_PQDMSm,
          data,
          QUE_SIGNATUREf,
          IQM_static_tbl_data->que_signature );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 34, exit);

  err = soc_mem_write(
          unit,
          IQM_PQDMSm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 35, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_iqm_static_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table packet_queue_red_weight_table_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_iqm_packet_queue_red_weight_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_DATA* IQM_packet_queue_red_weight_table_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_GET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_sand_os_memset(
          IQM_packet_queue_red_weight_table_tbl_data,
          0x0,
          sizeof(ARAD_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

 
 




  err = soc_mem_read(
          unit,
          IQM_PQWQm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

    err=SOC_E_NONE;
    if(SOC_E_NONE == err) {
          IQM_packet_queue_red_weight_table_tbl_data->pq_weight   = soc_mem_field32_get(
                  unit,
                  IQM_PQWQm,
                  data,
                  PQ_WEIGHTf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 40, exit);

    if(SOC_E_NONE == err) {
          IQM_packet_queue_red_weight_table_tbl_data->avrg_en   = soc_mem_field32_get(
                  unit,
                  IQM_PQWQm,
                  data,
                  AVRG_ENf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_iqm_packet_queue_red_weight_table_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table packet_queue_red_weight_table_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_iqm_packet_queue_red_weight_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_DATA* IQM_packet_queue_red_weight_table_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_SET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

 
 




err=SOC_E_NONE;
if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          IQM_PQWQm,
          data,
          PQ_WEIGHTf,
          IQM_packet_queue_red_weight_table_tbl_data->pq_weight );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          IQM_PQWQm,
          data,
          AVRG_ENf,
          IQM_packet_queue_red_weight_table_tbl_data->avrg_en );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_mem_write(
          unit,
          IQM_PQWQm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_iqm_packet_queue_red_weight_table_tbl_set_unsafe()",0,0);
}
/*
 * Read indirect table credit_discount_table_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_iqm_credit_discount_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IQM_CREDIT_DISCOUNT_TABLE_TBL_DATA* IQM_credit_discount_table_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_IQM_CREDIT_DISCOUNT_TABLE_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IQM_CREDIT_DISCOUNT_TABLE_TBL_GET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_sand_os_memset(
          IQM_credit_discount_table_tbl_data,
          0x0,
          sizeof(ARAD_IQM_CREDIT_DISCOUNT_TABLE_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

 
 




  err = soc_mem_read(
          unit,
          IQM_CRDTDISm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

    err=SOC_E_NONE;
    if(SOC_E_NONE == err) {
          IQM_credit_discount_table_tbl_data->crdt_disc_val   = soc_mem_field32_get(
                  unit,
                  IQM_CRDTDISm,
                  data,
                  CRDT_DISC_VALf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 40, exit);

    if(SOC_E_NONE == err) {
          IQM_credit_discount_table_tbl_data->discnt_sign   = soc_mem_field32_get(
                  unit,
                  IQM_CRDTDISm,
                  data,
                  DISCNT_SIGNf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_iqm_credit_discount_table_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table credit_discount_table_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_iqm_credit_discount_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IQM_CREDIT_DISCOUNT_TABLE_TBL_DATA* IQM_credit_discount_table_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_IQM_CREDIT_DISCOUNT_TABLE_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IQM_CREDIT_DISCOUNT_TABLE_TBL_SET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

 
 




err=SOC_E_NONE;
if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          IQM_CRDTDISm,
          data,
          CRDT_DISC_VALf,
          IQM_credit_discount_table_tbl_data->crdt_disc_val );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          IQM_CRDTDISm,
          data,
          DISCNT_SIGNf,
          IQM_credit_discount_table_tbl_data->discnt_sign );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_mem_write(
          unit,
          IQM_CRDTDISm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_iqm_credit_discount_table_tbl_set_unsafe()",0,0);
}

/* 
 * This procedure is meant to determine the number of bits in the mantissa for 
 * max queue size, 
 * and max queue size in BDs. 
 *  
 * Arad and Jericho share this code, 
 * But they have different values in the mantissa exponnent, for taildrop configuration
 * Moreover the CNRED/PQRED configuartion are different in Jericho.
 */
uint32
    arad_iqm_packet_queue_red_parameters_tail_drop_mantissa_nof_bits (
       SOC_SAND_IN   int             unit,
       SOC_SAND_IN   uint32             drop_precedence_ndx,
       SOC_SAND_OUT  uint32*            max_mantissa_size,
       SOC_SAND_OUT  uint32*            max_bds_mantissa_size
       )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IQM_PACKET_QUEUE_RED_PARAMETERS_TAIL_DROP_MANTISSA_NOF_BITS);
    if (drop_precedence_ndx == SOC_TMC_NOF_DROP_PRECEDENCE) {
        /* 
         * If receives drop_precedence_ndx == SOC_TMC_NOF_DROP_PRECEDENCE will read from the ECN (IQM_CNREDm) configuration.
         */
        if (SOC_IS_JERICHO(unit)){
            *max_mantissa_size = JERICHO_IQM_CNRED_PQ_MAX_QUE_SIZE_MANTISSA;
            *max_bds_mantissa_size = JERICHO_IQM_CNRED_PQ_MAX_QUE_BUFF_SIZE_MANTISSA;
        } else {
            *max_mantissa_size = ARAD_IQM_CNRED_PQ_MAX_QUE_SIZE_MANTISSA;
            *max_bds_mantissa_size = ARAD_IQM_CNRED_PQ_MAX_QUE_BUFF_SIZE_MANTISSA;
        }
    } else {
        /* 
         * If receives drop_precedence_ndx == SOC_TMC_NOF_DROP_PRECEDENCE will read from the IQM_PQRED configuration.
         */
        if (SOC_IS_JERICHO(unit)){
            *max_mantissa_size = JERICHO_IQM_PQRED_PQ_MAX_QUE_SIZE_MANTISSA;
            *max_bds_mantissa_size = JERICHO_IQM_PQRED_PQ_MAX_QUE_BUFF_SIZE_MANTISSA;
        } else {
            *max_mantissa_size = ARAD_IQM_PQRED_PQ_MAX_QUE_SIZE_MANTISSA;
            *max_bds_mantissa_size = ARAD_IQM_PQRED_PQ_MAX_QUE_BUFF_SIZE_MANTISSA;
         }
    }
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_iqm_packet_queue_red_parameters_tail_drop_mantissa_nof_bits()",0, drop_precedence_ndx);
}
/*
 * Read indirect table packet_queue_red_parameters_table_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 *
 * If receives drop_precedence_ndx == SOC_TMC_NOF_DROP_PRECEDENCE will read from the ECN (IQM_CNREDm) configuration.
 */
uint32
  arad_iqm_packet_queue_red_parameters_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32             rt_cls_ndx,
    SOC_SAND_IN   uint32             drop_precedence_ndx,
    SOC_SAND_OUT  ARAD_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_DATA* IQM_packet_queue_red_parameters_table_tbl_data
  )
{
  uint32 exp_man, err;
  uint32 entry_offset, data[ARAD_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_ENTRY_SIZE] = {0};
  soc_mem_t mem;
  int32 
      core_id = SOC_CORE_ALL;
  uint32 
      max_mantissa_size,
      max_bds_mantissa_size; 

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_GET_UNSAFE);

  SOC_SAND_CHECK_FUNC_RESULT(soc_sand_os_memset(IQM_packet_queue_red_parameters_table_tbl_data, 0x0,
    sizeof(ARAD_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_DATA)), 15, exit);

  if (drop_precedence_ndx == SOC_TMC_NOF_DROP_PRECEDENCE) {
    mem = IQM_CNREDm;
    entry_offset = rt_cls_ndx;
  } else { 
    mem = IQM_PQREDm;
    entry_offset = (rt_cls_ndx * ARAD_WRED_NOF_DROP_PRECEDENCE) + drop_precedence_ndx;
  }
  err = arad_iqm_packet_queue_red_parameters_tail_drop_mantissa_nof_bits(unit,
                                                                         drop_precedence_ndx,
                                                                         &max_mantissa_size,
                                                                         &max_bds_mantissa_size);
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);
  
  err = soc_mem_read(unit, mem, IQM_BLOCK(unit, core_id), entry_offset, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);
  exp_man = soc_mem_field32_get(unit, mem, data, PQ_MAX_QUE_SIZEf);
  arad_iqm_mantissa_exponent_get(unit, exp_man, max_mantissa_size,
    &(IQM_packet_queue_red_parameters_table_tbl_data->pq_max_que_size_mnt),
    &(IQM_packet_queue_red_parameters_table_tbl_data->pq_max_que_size_exp));

  exp_man = soc_mem_field32_get(unit, mem, data, PQ_MAX_QUE_BUFF_SIZEf);
  arad_iqm_mantissa_exponent_get(unit, exp_man, max_bds_mantissa_size,
    &(IQM_packet_queue_red_parameters_table_tbl_data->pq_max_que_size_bds_mnt),
    &(IQM_packet_queue_red_parameters_table_tbl_data->pq_max_que_size_bds_exp));

  IQM_packet_queue_red_parameters_table_tbl_data->pq_wred_en = soc_mem_field32_get(unit, mem, data, PQ_WRED_ENf);
  IQM_packet_queue_red_parameters_table_tbl_data->pq_c2 = soc_mem_field32_get(unit, mem, data, PQ_C_2f);
  IQM_packet_queue_red_parameters_table_tbl_data->pq_c3 = soc_mem_field32_get(unit, mem, data, PQ_C_3f);
  IQM_packet_queue_red_parameters_table_tbl_data->pq_c1 = soc_mem_field32_get(unit, mem, data, PQ_C_1f);
  IQM_packet_queue_red_parameters_table_tbl_data->pq_avrg_max_th = soc_mem_field32_get(unit, mem, data, PQ_AVRG_MAX_THf);
  IQM_packet_queue_red_parameters_table_tbl_data->pq_avrg_min_th = soc_mem_field32_get(unit, mem, data, PQ_AVRG_MIN_THf);
  IQM_packet_queue_red_parameters_table_tbl_data->pq_wred_pckt_sz_ignr = soc_mem_field32_get(unit, mem, data, PQ_WRED_PCKT_SZ_IGNRf);
  IQM_packet_queue_red_parameters_table_tbl_data->addmit_logic =
    drop_precedence_ndx == SOC_TMC_NOF_DROP_PRECEDENCE ? 0 :
    soc_mem_field32_get(unit, IQM_PQREDm, data, ADDMIT_LOGICf);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_iqm_packet_queue_red_parameters_table_tbl_get_unsafe()", rt_cls_ndx, drop_precedence_ndx);
}

/*
 * Write indirect table packet_queue_red_parameters_table_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 *
 * If receives drop_precedence_ndx == SOC_TMC_NOF_DROP_PRECEDENCE will read from the ECN (IQM_CNREDm) configuration.
 */

uint32
  arad_iqm_packet_queue_red_parameters_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32             rt_cls_ndx,
    SOC_SAND_IN   uint32             drop_precedence_ndx,
    SOC_SAND_IN   ARAD_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_DATA* IQM_packet_queue_red_parameters_table_tbl_data
  )
{
  uint32 exp_man, err, entry_offset, data[ARAD_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_ENTRY_SIZE] = {0};
  soc_mem_t mem;
  int32 
      core_id = SOC_CORE_ALL;
  uint32 
      max_mantissa_size,
      max_bds_mantissa_size;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_SET_UNSAFE);

  if (drop_precedence_ndx == SOC_TMC_NOF_DROP_PRECEDENCE) {
    mem = IQM_CNREDm;
    entry_offset = rt_cls_ndx;
  } else { 
    mem = IQM_PQREDm;
    entry_offset = (rt_cls_ndx * ARAD_WRED_NOF_DROP_PRECEDENCE) + drop_precedence_ndx;
  }
  err = arad_iqm_packet_queue_red_parameters_tail_drop_mantissa_nof_bits(unit,
                                                                   drop_precedence_ndx,
                                                                   &max_mantissa_size,
                                                                   &max_bds_mantissa_size);
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);
  
  arad_iqm_mantissa_exponent_set(unit,
      IQM_packet_queue_red_parameters_table_tbl_data->pq_max_que_size_mnt,
      IQM_packet_queue_red_parameters_table_tbl_data->pq_max_que_size_exp,
      max_mantissa_size, &exp_man);
  soc_mem_field32_set(unit, mem, data, PQ_MAX_QUE_SIZEf, exp_man);

  arad_iqm_mantissa_exponent_set( unit,
      IQM_packet_queue_red_parameters_table_tbl_data->pq_max_que_size_bds_mnt,
      IQM_packet_queue_red_parameters_table_tbl_data->pq_max_que_size_bds_exp,
      max_bds_mantissa_size, &exp_man);
  soc_mem_field32_set(unit, mem, data, PQ_MAX_QUE_BUFF_SIZEf, exp_man);

  soc_mem_field32_set(unit, mem, data, PQ_WRED_ENf, IQM_packet_queue_red_parameters_table_tbl_data->pq_wred_en);
  soc_mem_field32_set(unit, mem, data, PQ_C_2f, IQM_packet_queue_red_parameters_table_tbl_data->pq_c2);
  soc_mem_field32_set(unit, mem, data, PQ_C_3f, IQM_packet_queue_red_parameters_table_tbl_data->pq_c3);
  soc_mem_field32_set(unit, mem, data, PQ_C_1f, IQM_packet_queue_red_parameters_table_tbl_data->pq_c1);
  soc_mem_field32_set(unit, mem, data, PQ_AVRG_MAX_THf, IQM_packet_queue_red_parameters_table_tbl_data->pq_avrg_max_th);
  soc_mem_field32_set(unit, mem, data, PQ_AVRG_MIN_THf, IQM_packet_queue_red_parameters_table_tbl_data->pq_avrg_min_th);
  soc_mem_field32_set(unit, mem, data, PQ_WRED_PCKT_SZ_IGNRf, IQM_packet_queue_red_parameters_table_tbl_data->pq_wred_pckt_sz_ignr);
  if (drop_precedence_ndx != SOC_TMC_NOF_DROP_PRECEDENCE) {
    soc_mem_field32_set(unit, IQM_PQREDm, data, ADDMIT_LOGICf, IQM_packet_queue_red_parameters_table_tbl_data->addmit_logic);
  }

  err = soc_mem_write(unit, mem, IQM_BLOCK(unit, core_id), entry_offset, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_iqm_packet_queue_red_parameters_table_tbl_set_unsafe()",0,0);
}

/*
 * Fill indirect table packet_queue_red_parameters_table_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_iqm_packet_queue_red_parameters_table_tbl_fill_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   ARAD_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_DATA* IQM_packet_queue_red_parameters_table_tbl_data
  )
{
  uint32 exp_man, err;
  uint32 data[ARAD_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_SET_UNSAFE);

  err = soc_sand_os_memset(&(data[0]), 0x0, sizeof(data));
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 10, exit);

  arad_iqm_mantissa_exponent_set(unit, IQM_packet_queue_red_parameters_table_tbl_data->pq_max_que_size_mnt,
                                 IQM_packet_queue_red_parameters_table_tbl_data->pq_max_que_size_exp, 6, &exp_man);
  soc_mem_field32_set(unit, IQM_PQREDm, data, PQ_MAX_QUE_SIZEf, exp_man);
  soc_mem_field32_set(unit, IQM_PQREDm, data, PQ_WRED_ENf, IQM_packet_queue_red_parameters_table_tbl_data->pq_wred_en);
  soc_mem_field32_set(unit, IQM_PQREDm, data, PQ_C_2f, IQM_packet_queue_red_parameters_table_tbl_data->pq_c2);
  soc_mem_field32_set(unit, IQM_PQREDm, data, PQ_C_3f, IQM_packet_queue_red_parameters_table_tbl_data->pq_c3);
  soc_mem_field32_set(unit, IQM_PQREDm, data, PQ_C_1f, IQM_packet_queue_red_parameters_table_tbl_data->pq_c1);
  soc_mem_field32_set(unit, IQM_PQREDm, data, PQ_AVRG_MAX_THf, IQM_packet_queue_red_parameters_table_tbl_data->pq_avrg_max_th);
  soc_mem_field32_set(unit, IQM_PQREDm, data, PQ_AVRG_MIN_THf, IQM_packet_queue_red_parameters_table_tbl_data->pq_avrg_min_th);
  soc_mem_field32_set(unit, IQM_PQREDm, data, PQ_WRED_PCKT_SZ_IGNRf, IQM_packet_queue_red_parameters_table_tbl_data->pq_wred_pckt_sz_ignr);
  soc_mem_field32_set(unit, IQM_PQREDm, data, ADDMIT_LOGICf, IQM_packet_queue_red_parameters_table_tbl_data->addmit_logic);

  err = arad_fill_table_with_entry(unit, IQM_PQREDm, MEM_BLOCK_ANY, data); /* fill memory with the entry in data */
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_iqm_packet_queue_red_parameters_table_tbl_fill_unsafe()",0,0);
}

uint32
  arad_ihb_fem_map_index_table_table_tbl_fill_unsafe(
    SOC_SAND_IN   int                                             unit,
    SOC_SAND_IN   ARAD_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA* ihb_fem0_4b_1st_pass_map_index_table_tbl_data
  )
{
  uint32 err;
  uint32 data[ARAD_IHB_FEM0_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  err = soc_sand_os_memset(&(data[0]), 0x0, sizeof(data));
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 10, exit);

  soc_mem_field32_set(unit, IHB_FEM_MAP_INDEX_TABLEm, data, MAP_INDEXf, ihb_fem0_4b_1st_pass_map_index_table_tbl_data->map_index);
  soc_mem_field32_set(unit, IHB_FEM_MAP_INDEX_TABLEm, data, MAP_DATAf, ihb_fem0_4b_1st_pass_map_index_table_tbl_data->map_data);
  soc_mem_field32_set(unit, IHB_FEM_MAP_INDEX_TABLEm, data, VALIDf, ihb_fem0_4b_1st_pass_map_index_table_tbl_data->is_action_valid);

  err = arad_fill_table_with_entry(unit, IHB_FEM_MAP_INDEX_TABLEm, MEM_BLOCK_ANY, data); /* fill memory with the entry in data */
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ihb_fem_map_index_table_table_tbl_fill_unsafe()",0,0);
}

uint32
  arad_ihb_fem_map_table_table_tbl_fill_unsafe(
    SOC_SAND_IN   int                                             unit,
    SOC_SAND_IN   ARAD_IHB_FEM_MAP_TABLE_TBL_DATA* arad_ihb_fem_map_table_table_tbl_fill_unsafe
  )
{
    uint32 tbl_ndx, bit_ndx, fem_id, output_size_in_bits;
  uint32 err, 
      fem_map_tbl_reference_index,
      res;
  uint32 tbl_data[ARAD_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE];
  ARAD_PMF_FEM_NDX   
      fem_ndx;
  soc_mem_t
      fem_map_table_reference,
      fem_map_table;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  err = soc_sand_os_memset(&(tbl_data[0]), 0x0, sizeof(tbl_data));
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 10, exit);

  ARAD_PMF_FEM_NDX_clear(&fem_ndx);
  for (fem_id = 0; fem_id <= ARAD_PMF_LOW_LEVEL_FEM_ID_MAX; fem_id++) {
      fem_ndx.id = fem_id;
        res = arad_pmf_fem_output_size_get(
              unit,
              &fem_ndx,
              &output_size_in_bits
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

      /*
       *    Init the table data - set all the values to '0'
       */
      tbl_ndx = fem_id;
      fem_map_table = Arad_pmf_fem_map_tbl[tbl_ndx];
      fem_map_tbl_reference_index = (output_size_in_bits == ARAD_PMF_FEM_SIZE_1ST_SIZE)? 0:((output_size_in_bits == ARAD_PMF_FEM_SIZE_2ND_SIZE)? 1:2);
      res = arad_pmf_fem_map_tbl_reference(
              unit,
              fem_map_tbl_reference_index,
              &fem_map_table_reference
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

      for (bit_ndx = 0; bit_ndx < output_size_in_bits; ++bit_ndx)
      {
        soc_mem_field32_set(unit, fem_map_table_reference, tbl_data, Arad_pmf_fem_map_field_select_field[bit_ndx], 0x3e);
      }

      /*
       *    Set the table entry data
       */
      soc_mem_field32_set(unit, fem_map_table_reference, tbl_data, ACTION_TYPEf, 0x3c);

      if (output_size_in_bits != ARAD_PMF_FEM_SIZE_1ST_SIZE)
      {
        soc_mem_field32_set(unit, fem_map_table_reference, tbl_data, OFFSETf, 0x0);
      }

      err = arad_fill_table_with_entry(unit, fem_map_table, MEM_BLOCK_ANY, tbl_data); /* fill memory with the entry in data */
      SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 40, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ihb_fem_map_table_table_tbl_fill_unsafe()",0,0);
}

/*
 * Read indirect table vsq_descriptor_rate_class_group_a_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_iqm_vsq_descriptor_rate_class_group_a_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_DATA* IQM_vsq_descriptor_rate_class_group_a_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_GET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_sand_os_memset(
          IQM_vsq_descriptor_rate_class_group_a_tbl_data,
          0x0,
          sizeof(ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

 
 




  err = soc_mem_read(
          unit,
          IQM_VSQDRC_Am,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

    err=SOC_E_NONE;
    if(SOC_E_NONE == err) {
          IQM_vsq_descriptor_rate_class_group_a_tbl_data->vsq_rc_a   = soc_mem_field32_get(
                  unit,
                  IQM_VSQDRC_Am,
                  data,
                  VSQ_RC_Af            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_iqm_vsq_descriptor_rate_class_group_a_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table vsq_descriptor_rate_class_group_a_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_iqm_vsq_descriptor_rate_class_group_a_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_DATA* IQM_vsq_descriptor_rate_class_group_a_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_SET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

 
 




err=SOC_E_NONE;
if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          IQM_VSQDRC_Am,
          data,
          VSQ_RC_Af,
          IQM_vsq_descriptor_rate_class_group_a_tbl_data->vsq_rc_a );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_mem_write(
          unit,
          IQM_VSQDRC_Am,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_iqm_vsq_descriptor_rate_class_group_a_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table vsq_descriptor_rate_class_group_b_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_iqm_vsq_descriptor_rate_class_group_b_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_DATA* IQM_vsq_descriptor_rate_class_group_b_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_GET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_sand_os_memset(
          IQM_vsq_descriptor_rate_class_group_b_tbl_data,
          0x0,
          sizeof(ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

 
 




  err = soc_mem_read(
          unit,
          IQM_VSQDRC_Bm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

    err=SOC_E_NONE;
    if(SOC_E_NONE == err) {
          IQM_vsq_descriptor_rate_class_group_b_tbl_data->vsq_rc_b   = soc_mem_field32_get(
                  unit,
                  IQM_VSQDRC_Bm,
                  data,
                  VSQ_RC_Bf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_iqm_vsq_descriptor_rate_class_group_b_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table vsq_descriptor_rate_class_group_b_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_iqm_vsq_descriptor_rate_class_group_b_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_DATA* IQM_vsq_descriptor_rate_class_group_b_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_SET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

 
 




err=SOC_E_NONE;
if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          IQM_VSQDRC_Bm,
          data,
          VSQ_RC_Bf,
          IQM_vsq_descriptor_rate_class_group_b_tbl_data->vsq_rc_b );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_mem_write(
          unit,
          IQM_VSQDRC_Bm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_iqm_vsq_descriptor_rate_class_group_b_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table vsq_descriptor_rate_class_group_c_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_iqm_vsq_descriptor_rate_class_group_c_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_DATA* IQM_vsq_descriptor_rate_class_group_c_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_GET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_sand_os_memset(
          IQM_vsq_descriptor_rate_class_group_c_tbl_data,
          0x0,
          sizeof(ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

 
 




  err = soc_mem_read(
          unit,
          IQM_VSQDRC_Cm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

    err=SOC_E_NONE;
    if(SOC_E_NONE == err) {
          IQM_vsq_descriptor_rate_class_group_c_tbl_data->vsq_rc_c   = soc_mem_field32_get(
                  unit,
                  IQM_VSQDRC_Cm,
                  data,
                  VSQ_RC_Cf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_iqm_vsq_descriptor_rate_class_group_c_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table vsq_descriptor_rate_class_group_c_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_iqm_vsq_descriptor_rate_class_group_c_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_DATA* IQM_vsq_descriptor_rate_class_group_c_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_SET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

 
 




err=SOC_E_NONE;
if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          IQM_VSQDRC_Cm,
          data,
          VSQ_RC_Cf,
          IQM_vsq_descriptor_rate_class_group_c_tbl_data->vsq_rc_c );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_mem_write(
          unit,
          IQM_VSQDRC_Cm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_iqm_vsq_descriptor_rate_class_group_c_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table vsq_descriptor_rate_class_group_d_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_iqm_vsq_descriptor_rate_class_group_d_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_DATA* IQM_vsq_descriptor_rate_class_group_d_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_GET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_sand_os_memset(
          IQM_vsq_descriptor_rate_class_group_d_tbl_data,
          0x0,
          sizeof(ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

 
 




  err = soc_mem_read(
          unit,
          IQM_VSQDRC_Dm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

    err=SOC_E_NONE;
    if(SOC_E_NONE == err) {
          IQM_vsq_descriptor_rate_class_group_d_tbl_data->vsq_rc_d   = soc_mem_field32_get(
                  unit,
                  IQM_VSQDRC_Dm,
                  data,
                  VSQ_RC_Df            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_iqm_vsq_descriptor_rate_class_group_d_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table vsq_descriptor_rate_class_group_d_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_iqm_vsq_descriptor_rate_class_group_d_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_DATA* IQM_vsq_descriptor_rate_class_group_d_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_SET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

 
 




err=SOC_E_NONE;
if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          IQM_VSQDRC_Dm,
          data,
          VSQ_RC_Df,
          IQM_vsq_descriptor_rate_class_group_d_tbl_data->vsq_rc_d );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_mem_write(
          unit,
          IQM_VSQDRC_Dm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_iqm_vsq_descriptor_rate_class_group_d_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table vsq_flow_control_parameters_table_group_a_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */


void
  arad_iqm_mantissa_exponent_get(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              reg_val,
    SOC_SAND_IN   uint32              mantissa_size,
    SOC_SAND_OUT  uint32              *mantissa,
    SOC_SAND_OUT  uint32              *exponent
  )
{
    *mantissa = SOC_SAND_GET_BITS_RANGE(reg_val, mantissa_size-1, 0);
    *exponent = reg_val >> mantissa_size;
}


void
  arad_iqm_mantissa_exponent_set(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              mantissa,
    SOC_SAND_IN   uint32              exponent,
    SOC_SAND_IN   uint32              mantissa_size,
    SOC_SAND_OUT  uint32              *reg_val
  )
{
    *reg_val = mantissa + (exponent << mantissa_size);
}

/* abstraction of the IQM_VQFCPR_M tables */
const soc_mem_t mem_arr_IQM_VQFCPR_M[ARAD_NOF_VSQ_GROUPS] = {IQM_VQFCPR_MAm, IQM_VQFCPR_MBm,IQM_VQFCPR_MCm,IQM_VQFCPR_MDm,IQM_VQFCPR_MEm,IQM_VQFCPR_MFm};

uint32
  arad_iqm_vsq_flow_control_parameters_table_group_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              group_id,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_DATA* IQM_vsq_flow_control_parameters_table_group_tbl_data
  )
{
  uint32
      mnt_exp = 0,
    err;
  uint32
    data[ARAD_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_GET_UNSAFE);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    group_id, ARAD_NOF_VSQ_GROUPS-1,
    ARAD_ITM_VSQ_GROUP_ID_OUT_OF_RANGE_ERR, 5, exit
  );

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_sand_os_memset(
          IQM_vsq_flow_control_parameters_table_group_tbl_data,
          0x0,
          sizeof(ARAD_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

 
 




  err = soc_mem_read(
          unit,
          mem_arr_IQM_VQFCPR_M[group_id],
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

    err=SOC_E_NONE;
    if(SOC_E_NONE == err) {
          IQM_vsq_flow_control_parameters_table_group_tbl_data->wred_en   = soc_mem_field32_get(
                  unit,
                  mem_arr_IQM_VQFCPR_M[group_id],
                  data,
                  WRED_ENf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 40, exit);

    if(SOC_E_NONE == err) {
          IQM_vsq_flow_control_parameters_table_group_tbl_data->avrg_size_en   = soc_mem_field32_get(
                  unit,
                  mem_arr_IQM_VQFCPR_M[group_id],
                  data,
                  AVRG_SIZE_ENf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 41, exit);

    if(SOC_E_NONE == err) {
          IQM_vsq_flow_control_parameters_table_group_tbl_data->red_weight_q   = soc_mem_field32_get(
                  unit,
                  mem_arr_IQM_VQFCPR_M[group_id],
                  data,
                  RED_WEIGHT_Qf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 42, exit);

    if(SOC_E_NONE == err) {
        mnt_exp = soc_mem_field32_get(
                  unit,
                  mem_arr_IQM_VQFCPR_M[group_id],
                  data,
                  SET_THRESHOLD_WORDSf            );

    arad_iqm_mantissa_exponent_get(
        unit,
        mnt_exp,
        5,
        &(IQM_vsq_flow_control_parameters_table_group_tbl_data->set_threshold_words_mnt),
        &(IQM_vsq_flow_control_parameters_table_group_tbl_data->set_threshold_words_exp));
    
            
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 43, exit);

    if(SOC_E_NONE == err) {
        mnt_exp = soc_mem_field32_get(
                  unit,
                  mem_arr_IQM_VQFCPR_M[group_id],
                  data,
                  CLEAR_THRESHOLD_WORDSf            );
        arad_iqm_mantissa_exponent_get(
            unit,
            mnt_exp,
            5,
            &(IQM_vsq_flow_control_parameters_table_group_tbl_data->clear_threshold_words_mnt),
            &(IQM_vsq_flow_control_parameters_table_group_tbl_data->clear_threshold_words_exp));
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 45, exit);

    if(SOC_E_NONE == err) {
          mnt_exp = soc_mem_field32_get(
                  unit,
                  mem_arr_IQM_VQFCPR_M[group_id],
                  data,
                  SET_THRESHOLD_BDf            );
          arad_iqm_mantissa_exponent_get(
              unit,
              mnt_exp,
              4,
              &(IQM_vsq_flow_control_parameters_table_group_tbl_data->set_threshold_bd_mnt),
              &(IQM_vsq_flow_control_parameters_table_group_tbl_data->set_threshold_bd_exp));
             
                 
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 47, exit);


    err=SOC_E_NONE;
    if(SOC_E_NONE == err) {
            mnt_exp = soc_mem_field32_get(
                  unit,
                  mem_arr_IQM_VQFCPR_M[group_id],
                  data,
                  CLEAR_THRESHOLD_BDf            );
            arad_iqm_mantissa_exponent_get(
                unit,
                mnt_exp,
                4,
                &(IQM_vsq_flow_control_parameters_table_group_tbl_data->clear_threshold_bd_mnt),
                &(IQM_vsq_flow_control_parameters_table_group_tbl_data->clear_threshold_bd_exp));
             }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 49, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_iqm_vsq_flow_control_parameters_table_group_a_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table vsq_flow_control_parameters_table_group_a_tbl from block IQM,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_iqm_vsq_flow_control_parameters_table_group_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              group_id,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_DATA* IQM_vsq_flow_control_parameters_table_group_tbl_data
  )
{
  uint32
    mnt_exp,
    err;
  uint32
    data[ARAD_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_SET_UNSAFE);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    group_id, ARAD_NOF_VSQ_GROUPS-1,
    ARAD_ITM_VSQ_GROUP_ID_OUT_OF_RANGE_ERR, 5, exit
  );

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

 
 




err=SOC_E_NONE;
if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          mem_arr_IQM_VQFCPR_M[group_id],
          data,
          WRED_ENf,
          IQM_vsq_flow_control_parameters_table_group_tbl_data->wred_en );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          mem_arr_IQM_VQFCPR_M[group_id],
          data,
          AVRG_SIZE_ENf,
          IQM_vsq_flow_control_parameters_table_group_tbl_data->avrg_size_en );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 31, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          mem_arr_IQM_VQFCPR_M[group_id],
          data,
          RED_WEIGHT_Qf,
          IQM_vsq_flow_control_parameters_table_group_tbl_data->red_weight_q );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 32, exit);

  arad_iqm_mantissa_exponent_set(
      unit,
      IQM_vsq_flow_control_parameters_table_group_tbl_data->set_threshold_words_mnt,
      IQM_vsq_flow_control_parameters_table_group_tbl_data->set_threshold_words_exp,
      5,
      &mnt_exp);


if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          mem_arr_IQM_VQFCPR_M[group_id],
          data,
          SET_THRESHOLD_WORDSf,
          mnt_exp );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 33, exit);

  arad_iqm_mantissa_exponent_set(
      unit,
      IQM_vsq_flow_control_parameters_table_group_tbl_data->clear_threshold_words_mnt,
      IQM_vsq_flow_control_parameters_table_group_tbl_data->clear_threshold_words_exp,
      5,
      &mnt_exp);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          mem_arr_IQM_VQFCPR_M[group_id],
          data,
          CLEAR_THRESHOLD_WORDSf,
          mnt_exp);
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 35, exit);


  arad_iqm_mantissa_exponent_set(
      unit,
      IQM_vsq_flow_control_parameters_table_group_tbl_data->set_threshold_bd_mnt,
      IQM_vsq_flow_control_parameters_table_group_tbl_data->set_threshold_bd_exp,
      4,
      &mnt_exp);
if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          mem_arr_IQM_VQFCPR_M[group_id],
          data,
          SET_THRESHOLD_BDf,
          mnt_exp );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 37, exit);

  arad_iqm_mantissa_exponent_set(
      unit,
      IQM_vsq_flow_control_parameters_table_group_tbl_data->clear_threshold_bd_mnt,
      IQM_vsq_flow_control_parameters_table_group_tbl_data->clear_threshold_bd_exp,
      4,
      &mnt_exp);
if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          mem_arr_IQM_VQFCPR_M[group_id],
          data,
          CLEAR_THRESHOLD_BDf,
          mnt_exp);
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 39, exit);

  err = soc_mem_write(
          unit,
          mem_arr_IQM_VQFCPR_M[group_id],
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 37, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_iqm_vsq_flow_control_parameters_table_group_a_tbl_set_unsafe()",0,0);
}

/* abstraaction of the IQM_VQPR_M? tables */
const soc_mem_t mem_arr_IQM_VQPR_M[ARAD_NOF_VSQ_GROUPS] = {IQM_VQPR_MAm, IQM_VQPR_MBm,IQM_VQPR_MCm,IQM_VQPR_MDm,IQM_VQPR_MEm,IQM_VQPR_MFm};

/*
 * Read indirect table vsq_queue_parameters_table_group_a_tbl from block IQM,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_iqm_vsq_queue_parameters_table_group_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              group_id,
    SOC_SAND_IN   uint32              vsq_rt_cls_ndx,
    SOC_SAND_IN   uint32             drop_precedence_ndx,
    SOC_SAND_OUT  ARAD_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA* IQM_vsq_queue_parameters_table_group_tbl_data
  )
{
  uint32
      exp_man,
    err;
  uint32
    entry_offset,
    data[ARAD_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_A_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_GET_UNSAFE);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    group_id, ARAD_NOF_VSQ_GROUPS-1,
    ARAD_ITM_VSQ_GROUP_ID_OUT_OF_RANGE_ERR, 5, exit
  );

  /* since IQM_VQPR_M[A-F] have the same fields in hardware, but currently in the description
     IQM_VQPR_M[E-F] have different fields, we use mem_arr_IQM_VQPR_M[group_id] for memory
     access and IQM_VQPR_MAm for filed access */
  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_sand_os_memset(
          IQM_vsq_queue_parameters_table_group_tbl_data,
          0x0,
          sizeof(ARAD_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  entry_offset =
    (vsq_rt_cls_ndx * ARAD_WRED_NOF_DROP_PRECEDENCE) + drop_precedence_ndx;



  err = soc_mem_read(
          unit,
          mem_arr_IQM_VQPR_M[group_id],
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

    err=SOC_E_NONE;
    if(SOC_E_NONE == err) {
          IQM_vsq_queue_parameters_table_group_tbl_data->c2   = soc_mem_field32_get(
                  unit,
                  IQM_VQPR_MAm,
                  data,
                  C_2f            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 40, exit);

    if(SOC_E_NONE == err) {
          IQM_vsq_queue_parameters_table_group_tbl_data->c3   = soc_mem_field32_get(
                  unit,
                  IQM_VQPR_MAm,
                  data,
                  C_3f            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 41, exit);

    if(SOC_E_NONE == err) {
          IQM_vsq_queue_parameters_table_group_tbl_data->c1   = soc_mem_field32_get(
                  unit,
                  IQM_VQPR_MAm,
                  data,
                  C_1f            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 42, exit);

    if(SOC_E_NONE == err) {
          IQM_vsq_queue_parameters_table_group_tbl_data->max_avrg_th   = soc_mem_field32_get(
                  unit,
                  IQM_VQPR_MAm,
                  data,
                  MAX_AVRG_THf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 43, exit);

    if(SOC_E_NONE == err) {
          IQM_vsq_queue_parameters_table_group_tbl_data->min_avrg_th   = soc_mem_field32_get(
                  unit,
                  IQM_VQPR_MAm,
                  data,
                  MIN_AVRG_THf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 44, exit);
    if(SOC_E_NONE == err) {
          IQM_vsq_queue_parameters_table_group_tbl_data->vq_wred_pckt_sz_ignr   = soc_mem_field32_get(
                  unit,
                  IQM_VQPR_MAm,
                  data,
                  VQ_WRED_PCKT_SZ_IGNRf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 45, exit);

    if(SOC_E_NONE == err) {
          exp_man = soc_mem_field32_get(
                  unit,
                  IQM_VQPR_MAm,
                  data,
                  VQ_MAX_SIZE_BDSf            );

          arad_iqm_mantissa_exponent_get(
              unit,
              exp_man,
              7,
              &(IQM_vsq_queue_parameters_table_group_tbl_data->vq_max_szie_bds_mnt),
              &(IQM_vsq_queue_parameters_table_group_tbl_data->vq_max_szie_bds_exp)
              );

        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 46, exit);

    if(SOC_E_NONE == err) {
          exp_man   = soc_mem_field32_get(
                  unit,
                  IQM_VQPR_MAm,
                  data,
                  VQ_MAX_SIZE_WORDSf            );
          arad_iqm_mantissa_exponent_get(
              unit,
              exp_man,
              7,
              &(IQM_vsq_queue_parameters_table_group_tbl_data->vq_max_size_words_mnt),
              &(IQM_vsq_queue_parameters_table_group_tbl_data->vq_max_size_words_exp)
              );

        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 48, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_iqm_vsq_queue_parameters_table_group_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table vsq_queue_parameters_table_group_a_tbl from block IQM,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_iqm_vsq_queue_parameters_table_group_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              group_id,
    SOC_SAND_IN   uint32              vsq_rt_cls_ndx,
    SOC_SAND_IN   uint32             drop_precedence_ndx,
    SOC_SAND_IN   ARAD_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA* IQM_vsq_queue_parameters_table_group_tbl_data
  )
{
  uint32
    err;
  uint32
    entry_offset,
      exp_man,
    data[ARAD_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_A_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_SET_UNSAFE);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    group_id, ARAD_NOF_VSQ_GROUPS-1,
    ARAD_ITM_VSQ_GROUP_ID_OUT_OF_RANGE_ERR, 5, exit
  );

  /* since IQM_VQPR_M[A-F] have the same fields in hardware, but currently in the description
     IQM_VQPR_M[E-F] have different fields, we use mem_arr_IQM_VQPR_M[group_id] for memory
     access and IQM_VQPR_MAm for filed access */
  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  entry_offset =
    (vsq_rt_cls_ndx * ARAD_WRED_NOF_DROP_PRECEDENCE) + drop_precedence_ndx;



  err=SOC_E_NONE;
  if(SOC_E_NONE == err) {
          soc_mem_field32_set(
          unit,
          IQM_VQPR_MAm,
          data,
          C_2f,
          IQM_vsq_queue_parameters_table_group_tbl_data->c2 );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

  if(SOC_E_NONE == err) {
          soc_mem_field32_set(
          unit,
          IQM_VQPR_MAm,
          data,
          C_3f,
          IQM_vsq_queue_parameters_table_group_tbl_data->c3 );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 31, exit);

  if(SOC_E_NONE == err) {
          soc_mem_field32_set(
          unit,
          IQM_VQPR_MAm,
          data,
          C_1f,
          IQM_vsq_queue_parameters_table_group_tbl_data->c1 );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 32, exit);

  if(SOC_E_NONE == err) {
          soc_mem_field32_set(
          unit,
          IQM_VQPR_MAm,
          data,
          MAX_AVRG_THf,
          IQM_vsq_queue_parameters_table_group_tbl_data->max_avrg_th );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 33, exit);

  if(SOC_E_NONE == err) {
          soc_mem_field32_set(
          unit,
          IQM_VQPR_MAm,
          data,
          MIN_AVRG_THf,
          IQM_vsq_queue_parameters_table_group_tbl_data->min_avrg_th );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 34, exit);
  if(SOC_E_NONE == err) {
          soc_mem_field32_set(
          unit,
          IQM_VQPR_MAm,
          data,
          VQ_WRED_PCKT_SZ_IGNRf,
          IQM_vsq_queue_parameters_table_group_tbl_data->vq_wred_pckt_sz_ignr );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 35, exit);

  arad_iqm_mantissa_exponent_set(
      unit,
      IQM_vsq_queue_parameters_table_group_tbl_data->vq_max_szie_bds_mnt,
      IQM_vsq_queue_parameters_table_group_tbl_data->vq_max_szie_bds_exp,
      7,
      &exp_man
     );

  if(SOC_E_NONE == err) {
          soc_mem_field32_set(
          unit,
          IQM_VQPR_MAm,
          data,
          VQ_MAX_SIZE_BDSf,
          exp_man);
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 36, exit);

  arad_iqm_mantissa_exponent_set(
      unit,
      IQM_vsq_queue_parameters_table_group_tbl_data->vq_max_size_words_mnt,
      IQM_vsq_queue_parameters_table_group_tbl_data->vq_max_size_words_exp,
      7,
      &exp_man
     );

  if(SOC_E_NONE == err) {
          soc_mem_field32_set(
          unit,
          IQM_VQPR_MAm,
          data,
          VQ_MAX_SIZE_WORDSf,
          exp_man);
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 38, exit);

  err = soc_mem_write(
          unit,
          mem_arr_IQM_VQPR_M[group_id],
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_iqm_vsq_queue_parameters_table_group_a_tbl_set_unsafe()",0,0);
}


/*
 * Init indirect table vsq_queue_parameters_table_group_a_tbl from block IQM,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_iqm_vsq_queue_parameters_table_group_tbl_fill_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   ARAD_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA* IQM_vsq_queue_parameters_table_group_tbl_data
  )
{
  uint32 err;
  uint32 exp_man, data[ARAD_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_A_TBL_ENTRY_SIZE];
  unsigned group_id;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_SET_UNSAFE);
  /* since IQM_VQPR_M[A-F] have the same fields in hardware, but currently in the description
     IQM_VQPR_M[E-F] have different fields, we use mem_arr_IQM_VQPR_M[group_id] for memory
     access and IQM_VQPR_MAm for filed access */

  err = soc_sand_os_memset( &(data[0]), 0x0, sizeof(data));
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  soc_mem_field32_set(unit, IQM_VQPR_MAm, data, C_2f, IQM_vsq_queue_parameters_table_group_tbl_data->c2);
  soc_mem_field32_set(unit, IQM_VQPR_MAm, data, C_3f, IQM_vsq_queue_parameters_table_group_tbl_data->c3);
  soc_mem_field32_set(unit, IQM_VQPR_MAm, data, C_1f, IQM_vsq_queue_parameters_table_group_tbl_data->c1 );
  soc_mem_field32_set(unit, IQM_VQPR_MAm, data, MAX_AVRG_THf, IQM_vsq_queue_parameters_table_group_tbl_data->max_avrg_th );
  soc_mem_field32_set(unit, IQM_VQPR_MAm, data, MIN_AVRG_THf, IQM_vsq_queue_parameters_table_group_tbl_data->min_avrg_th );
  soc_mem_field32_set(unit, IQM_VQPR_MAm, data, VQ_WRED_PCKT_SZ_IGNRf, IQM_vsq_queue_parameters_table_group_tbl_data->vq_wred_pckt_sz_ignr );

  arad_iqm_mantissa_exponent_set(unit, IQM_vsq_queue_parameters_table_group_tbl_data->vq_max_szie_bds_mnt,
                                 IQM_vsq_queue_parameters_table_group_tbl_data->vq_max_szie_bds_exp, 7, &exp_man);
  soc_mem_field32_set(unit, IQM_VQPR_MAm, data, VQ_MAX_SIZE_BDSf, exp_man);
  arad_iqm_mantissa_exponent_set(unit, IQM_vsq_queue_parameters_table_group_tbl_data->vq_max_size_words_mnt,
                                 IQM_vsq_queue_parameters_table_group_tbl_data->vq_max_size_words_exp, 7, &exp_man);
  soc_mem_field32_set(unit, IQM_VQPR_MAm, data, VQ_MAX_SIZE_WORDSf, exp_man);


  for (group_id = 0; group_id < ARAD_NOF_VSQ_GROUPS; ++group_id) { /* loop over the 6 memories */
      err = arad_fill_table_with_entry(unit, mem_arr_IQM_VQPR_M[group_id] , MEM_BLOCK_ANY, data); /* fill memory with the entry in data */
      SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_iqm_vsq_queue_parameters_table_group_a_tbl_fill_unsafe()",0,0);
}


uint32
  arad_iqm_vsq_flow_control_parameters_table_group_tbl_fill_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   ARAD_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_DATA* iqm_vsq_flow_control_parameters_table_group_tbl_data
  )
{
  uint32 err;
  uint32 exp_man, data[ARAD_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_ENTRY_SIZE];
  unsigned group_id;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_SET_UNSAFE);
  /* since IQM_VQPR_M[A-F] have the same fields in hardware, but currently in the description
     IQM_VQPR_M[E-F] have different fields, we use mem_arr_IQM_VQPR_M[group_id] for memory
     access and IQM_VQPR_MAm for filed access */

  err = soc_sand_os_memset( &(data[0]), 0x0, sizeof(data));
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);


  soc_mem_field32_set(unit, IQM_VQFCPR_MAm, data, WRED_ENf, iqm_vsq_flow_control_parameters_table_group_tbl_data->wred_en);
  soc_mem_field32_set(unit, IQM_VQFCPR_MAm, data, AVRG_SIZE_ENf, iqm_vsq_flow_control_parameters_table_group_tbl_data->avrg_size_en);
  soc_mem_field32_set(unit, IQM_VQFCPR_MAm, data, RED_WEIGHT_Qf, iqm_vsq_flow_control_parameters_table_group_tbl_data->red_weight_q);

  arad_iqm_mantissa_exponent_set(unit, iqm_vsq_flow_control_parameters_table_group_tbl_data->set_threshold_words_mnt,
                                 iqm_vsq_flow_control_parameters_table_group_tbl_data->set_threshold_words_exp, 5, &exp_man);
  soc_mem_field32_set(unit, IQM_VQFCPR_MAm, data, SET_THRESHOLD_WORDSf, exp_man);
  arad_iqm_mantissa_exponent_set(unit, iqm_vsq_flow_control_parameters_table_group_tbl_data->clear_threshold_words_mnt,
                                 iqm_vsq_flow_control_parameters_table_group_tbl_data->clear_threshold_words_exp, 5, &exp_man);
  soc_mem_field32_set(unit, IQM_VQFCPR_MAm, data, CLEAR_THRESHOLD_WORDSf, exp_man);
  arad_iqm_mantissa_exponent_set(unit, iqm_vsq_flow_control_parameters_table_group_tbl_data->set_threshold_bd_mnt,
                                 iqm_vsq_flow_control_parameters_table_group_tbl_data->set_threshold_bd_exp, 4, &exp_man);
  soc_mem_field32_set(unit, IQM_VQFCPR_MAm, data, SET_THRESHOLD_BDf, exp_man);
  arad_iqm_mantissa_exponent_set(unit, iqm_vsq_flow_control_parameters_table_group_tbl_data->clear_threshold_bd_mnt,
                                 iqm_vsq_flow_control_parameters_table_group_tbl_data->clear_threshold_bd_exp, 4, &exp_man);
  soc_mem_field32_set(unit, IQM_VQFCPR_MAm, data, CLEAR_THRESHOLD_BDf, exp_man);


  for (group_id = 0; group_id < ARAD_NOF_VSQ_GROUPS; ++group_id) { /* loop over the 6 memories */
      err = arad_fill_table_with_entry(unit, mem_arr_IQM_VQFCPR_M[group_id] , MEM_BLOCK_ANY, data); /* fill memory with the entry in data */
      SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_iqm_vsq_flow_control_parameters_table_group_tbl_fill_unsafe()",0,0);
}

/*
 * Read indirect table system_red_parameters_table_tbl from block IQM,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_iqm_system_red_parameters_table_tbl_get_unsafe(
    SOC_SAND_IN   int                                       unit,
    SOC_SAND_IN   uint32                                        rt_cls_ndx,
    SOC_SAND_IN   uint32                                       drop_precedence_ndx,
    SOC_SAND_OUT  ARAD_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_DATA* IQM_system_red_parameters_table_tbl_data
  )
{
  uint32
    err;
  uint32
    entry_offset,
    data[ARAD_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_GET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_sand_os_memset(
          IQM_system_red_parameters_table_tbl_data,
          0x0,
          sizeof(ARAD_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

 
 


  entry_offset =
    (rt_cls_ndx * ARAD_WRED_NOF_DROP_PRECEDENCE) + drop_precedence_ndx;



  err = soc_mem_read(
          unit,
          IQM_SPRDPRMm,
          MEM_BLOCK_ANY,
          entry_offset,
    data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

    err=SOC_E_NONE;
    if(SOC_E_NONE == err) {
          IQM_system_red_parameters_table_tbl_data->adm_th   = soc_mem_field32_get(
                  unit,
                  IQM_SPRDPRMm,
                  data,
                  ADM_THf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 41, exit);

    if(SOC_E_NONE == err) {
        IQM_system_red_parameters_table_tbl_data->sys_red_en   = soc_mem_field32_get(
                  unit,
                  IQM_SPRDPRMm,
                data,
                  SRED_ENf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 40, exit);

    if(SOC_E_NONE == err) {
          IQM_system_red_parameters_table_tbl_data->prob_th   = soc_mem_field32_get(
                  unit,
                  IQM_SPRDPRMm,
                  data,
                  PROB_THf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 42, exit);

    if(SOC_E_NONE == err) {
          IQM_system_red_parameters_table_tbl_data->drp_th   = soc_mem_field32_get(
                  unit,
                  IQM_SPRDPRMm,
                  data,
                  DRP_THf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 43, exit);

    if(SOC_E_NONE == err) {
          IQM_system_red_parameters_table_tbl_data->drp_prob_indx1   = soc_mem_field32_get(
                  unit,
                  IQM_SPRDPRMm,
                  data,
                  DRP_PROB_INDX_1f            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 44, exit);

    if(SOC_E_NONE == err) {
          IQM_system_red_parameters_table_tbl_data->drp_prob_indx2   = soc_mem_field32_get(
                  unit,
                  IQM_SPRDPRMm,
                  data,
                  DRP_PROB_INDX_2f            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 45, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_iqm_system_red_parameters_table_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table system_red_parameters_table_tbl from block IQM,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_iqm_system_red_parameters_table_tbl_set_unsafe(
    SOC_SAND_IN   int                                       unit,
    SOC_SAND_IN   uint32                                        rt_cls_ndx,
    SOC_SAND_IN   uint32                                       drop_precedence_ndx,
    SOC_SAND_IN   ARAD_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_DATA* IQM_system_red_parameters_table_tbl_data
  )
{
  uint32
    err;
  uint32
    entry_offset,
    data[ARAD_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_SET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

 
 


  entry_offset =
    (rt_cls_ndx * ARAD_WRED_NOF_DROP_PRECEDENCE) + drop_precedence_ndx;



err=SOC_E_NONE;
if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          IQM_SPRDPRMm,
        data,
          SRED_ENf,
        IQM_system_red_parameters_table_tbl_data->sys_red_en );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          IQM_SPRDPRMm,
          data,
          ADM_THf,
          IQM_system_red_parameters_table_tbl_data->adm_th );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 31, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          IQM_SPRDPRMm,
          data,
          PROB_THf,
          IQM_system_red_parameters_table_tbl_data->prob_th );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 32, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          IQM_SPRDPRMm,
          data,
          DRP_THf,
          IQM_system_red_parameters_table_tbl_data->drp_th );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 33, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          IQM_SPRDPRMm,
          data,
          DRP_PROB_INDX_1f,
          IQM_system_red_parameters_table_tbl_data->drp_prob_indx1 );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 34, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          IQM_SPRDPRMm,
          data,
          DRP_PROB_INDX_2f,
          IQM_system_red_parameters_table_tbl_data->drp_prob_indx2 );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 35, exit);

  err = soc_mem_write(
          unit,
          IQM_SPRDPRMm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 35, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_iqm_system_red_parameters_table_tbl_set_unsafe()",0,0);
}
/*
 * Read indirect table system_red_drop_probability_values_tbl from block IQM,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_iqm_system_red_drop_probability_values_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_DATA* IQM_system_red_drop_probability_values_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_GET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_sand_os_memset(
          IQM_system_red_drop_probability_values_tbl_data,
          0x0,
          sizeof(ARAD_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

 
 




  err = soc_mem_read(
          unit,
          IQM_SRDPRBm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

err=SOC_E_NONE;
    if(SOC_E_NONE == err) {
          IQM_system_red_drop_probability_values_tbl_data->drp_prob   = soc_mem_field32_get(
                  unit,
                  IQM_SRDPRBm,
                  data,
                  DRP_PROBf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_iqm_system_red_drop_probability_values_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table system_red_drop_probability_values_tbl from block IQM,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_iqm_system_red_drop_probability_values_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_DATA* IQM_system_red_drop_probability_values_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_SET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

 
 




err=SOC_E_NONE;
if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          IQM_SRDPRBm,
          data,
          DRP_PROBf,
          IQM_system_red_drop_probability_values_tbl_data->drp_prob );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_mem_write(
          unit,
          IQM_SRDPRBm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_iqm_system_red_drop_probability_values_tbl_set_unsafe()",0,0);
}
/*
 * Read indirect table system_red_tbl from block IQM,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_iqm_system_red_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IQM_SYSTEM_RED_TBL_DATA* IQM_system_red_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_IQM_SYSTEM_RED_TBL_ENTRY_SIZE];
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IQM_SYSTEM_RED_TBL_GET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_sand_os_memset(
          IQM_system_red_tbl_data,
          0x0,
          sizeof(ARAD_IQM_SYSTEM_RED_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

   err = soc_mem_read(
          unit,
          IQM_SRCQRNGm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

  IQM_system_red_tbl_data->qsz_rng_th[0] = soc_mem_field32_get(unit, IQM_SRCQRNGm, data, QSZ_RNG_TH_0f);
  IQM_system_red_tbl_data->qsz_rng_th[1] = soc_mem_field32_get(unit, IQM_SRCQRNGm, data, QSZ_RNG_TH_1f);
  IQM_system_red_tbl_data->qsz_rng_th[2] = soc_mem_field32_get(unit, IQM_SRCQRNGm, data, QSZ_RNG_TH_2f);
  IQM_system_red_tbl_data->qsz_rng_th[3] = soc_mem_field32_get(unit, IQM_SRCQRNGm, data, QSZ_RNG_TH_3f);
  IQM_system_red_tbl_data->qsz_rng_th[4] = soc_mem_field32_get(unit, IQM_SRCQRNGm, data, QSZ_RNG_TH_4f);
  IQM_system_red_tbl_data->qsz_rng_th[5] = soc_mem_field32_get(unit, IQM_SRCQRNGm, data, QSZ_RNG_TH_5f);
  IQM_system_red_tbl_data->qsz_rng_th[6] = soc_mem_field32_get(unit, IQM_SRCQRNGm, data, QSZ_RNG_TH_6f);
  IQM_system_red_tbl_data->qsz_rng_th[7] = soc_mem_field32_get(unit, IQM_SRCQRNGm, data, QSZ_RNG_TH_7f);
  IQM_system_red_tbl_data->qsz_rng_th[8] = soc_mem_field32_get(unit, IQM_SRCQRNGm, data, QSZ_RNG_TH_8f);
  IQM_system_red_tbl_data->qsz_rng_th[9] = soc_mem_field32_get(unit, IQM_SRCQRNGm, data, QSZ_RNG_TH_9f);
  IQM_system_red_tbl_data->qsz_rng_th[10] = soc_mem_field32_get(unit, IQM_SRCQRNGm, data, QSZ_RNG_TH_10f);
  IQM_system_red_tbl_data->qsz_rng_th[11] = soc_mem_field32_get(unit, IQM_SRCQRNGm, data, QSZ_RNG_TH_11f);
  IQM_system_red_tbl_data->qsz_rng_th[12] = soc_mem_field32_get(unit, IQM_SRCQRNGm, data, QSZ_RNG_TH_12f);
  IQM_system_red_tbl_data->qsz_rng_th[13] = soc_mem_field32_get(unit, IQM_SRCQRNGm, data, QSZ_RNG_TH_13f);
  IQM_system_red_tbl_data->qsz_rng_th[14] = soc_mem_field32_get(unit, IQM_SRCQRNGm, data, QSZ_RNG_TH_14f);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_iqm_system_red_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table system_red_tbl from block IQM,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_iqm_system_red_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IQM_SYSTEM_RED_TBL_DATA* IQM_system_red_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_IQM_SYSTEM_RED_TBL_ENTRY_SIZE];
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IQM_SYSTEM_RED_TBL_SET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  soc_mem_field32_set(unit, IQM_SRCQRNGm, data, QSZ_RNG_TH_0f, IQM_system_red_tbl_data->qsz_rng_th[0]);
  soc_mem_field32_set(unit, IQM_SRCQRNGm, data, QSZ_RNG_TH_1f, IQM_system_red_tbl_data->qsz_rng_th[1]);
  soc_mem_field32_set(unit, IQM_SRCQRNGm, data, QSZ_RNG_TH_2f, IQM_system_red_tbl_data->qsz_rng_th[2]);
  soc_mem_field32_set(unit, IQM_SRCQRNGm, data, QSZ_RNG_TH_3f, IQM_system_red_tbl_data->qsz_rng_th[3]);
  soc_mem_field32_set(unit, IQM_SRCQRNGm, data, QSZ_RNG_TH_4f, IQM_system_red_tbl_data->qsz_rng_th[4]);
  soc_mem_field32_set(unit, IQM_SRCQRNGm, data, QSZ_RNG_TH_5f, IQM_system_red_tbl_data->qsz_rng_th[5]);
  soc_mem_field32_set(unit, IQM_SRCQRNGm, data, QSZ_RNG_TH_6f, IQM_system_red_tbl_data->qsz_rng_th[6]);
  soc_mem_field32_set(unit, IQM_SRCQRNGm, data, QSZ_RNG_TH_7f, IQM_system_red_tbl_data->qsz_rng_th[7]);
  soc_mem_field32_set(unit, IQM_SRCQRNGm, data, QSZ_RNG_TH_8f, IQM_system_red_tbl_data->qsz_rng_th[8]);
  soc_mem_field32_set(unit, IQM_SRCQRNGm, data, QSZ_RNG_TH_9f, IQM_system_red_tbl_data->qsz_rng_th[9]);
  soc_mem_field32_set(unit, IQM_SRCQRNGm, data, QSZ_RNG_TH_10f, IQM_system_red_tbl_data->qsz_rng_th[10]);
  soc_mem_field32_set(unit, IQM_SRCQRNGm, data, QSZ_RNG_TH_11f, IQM_system_red_tbl_data->qsz_rng_th[11]);
  soc_mem_field32_set(unit, IQM_SRCQRNGm, data, QSZ_RNG_TH_12f, IQM_system_red_tbl_data->qsz_rng_th[12]);
  soc_mem_field32_set(unit, IQM_SRCQRNGm, data, QSZ_RNG_TH_13f, IQM_system_red_tbl_data->qsz_rng_th[13]);
  soc_mem_field32_set(unit, IQM_SRCQRNGm, data, QSZ_RNG_TH_14f, IQM_system_red_tbl_data->qsz_rng_th[14]);

  err = soc_mem_write(
          unit,
          IQM_SRCQRNGm,
          MEM_BLOCK_ANY,
          entry_offset,
            data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 45, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_iqm_system_red_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table system_physical_port_lookup_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_indirect_base_queue_to_system_physical_port_tbl_get_unsafe(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 entry_offset,
    SOC_SAND_OUT ARAD_BASE_Q_TO_SYS_PORT_TABLE_TBL_DATA* system_physical_port_tbl_data
  )
{
  uint32 err;
  uint32 data[ARAD_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_GET_UNSAFE);
  /* assert(ARAD_IS_VOQ_MAPPING_INDIRECT(unit)); */

  err = soc_sand_os_memset(&(data[0]), 0x0, sizeof(data));
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_sand_os_memset(system_physical_port_tbl_data, 0x0, sizeof(ARAD_BASE_Q_TO_SYS_PORT_TABLE_TBL_DATA));
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_mem_read(unit, IPS_QPM_1_SYS_REDm, MEM_BLOCK_ANY, entry_offset, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

  system_physical_port_tbl_data->sys_phy_port = soc_mem_field32_get(unit, IPS_QPM_1_SYS_REDm, data, SYS_PHY_PORTf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_indirect_base_queue_to_system_physical_port_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table system_physical_port_lookup_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_indirect_base_queue_to_system_physical_port_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_BASE_Q_TO_SYS_PORT_TABLE_TBL_DATA* system_physical_port_tbl_data
  )
{
  uint32 err;
  uint32 data[ARAD_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_SET_UNSAFE);
  /* assert(ARAD_IS_VOQ_MAPPING_INDIRECT(unit)); */

  err = soc_sand_os_memset(&(data[0]), 0x0, sizeof(data));
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  soc_mem_field32_set(unit, IPS_QPM_1_SYS_REDm, data, SYS_PHY_PORTf, system_physical_port_tbl_data->sys_phy_port);

  err = soc_mem_write(unit, IPS_QPM_1_SYS_REDm, MEM_BLOCK_ALL, entry_offset, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_indirect_base_queue_to_system_physical_port_tbl_set_unsafe()",0,0);
}


/*
 * Write indirect table system_physical_port_lookup_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_indirect_base_queue_to_system_physical_port_tbl_region_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   uint32              region_size,
    SOC_SAND_IN   ARAD_BASE_Q_TO_SYS_PORT_TABLE_TBL_DATA* system_physical_port_tbl_data
  )
{
  uint32 err;
  uint32 *data = NULL, *cur_data;
  uint32 flags=0;
  int index_max, index_min;
  int i; 
  int dmaable = soc_mem_dmaable(unit, IPS_QPM_1_SYS_REDm, SOC_MEM_BLOCK_ANY(unit, IPS_QPM_1_SYS_REDm)); /* check if we can use DMA */
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_SET_UNSAFE);
  index_min=entry_offset;
  index_max=entry_offset+region_size-1;

  /* assert(ARAD_IS_VOQ_MAPPING_INDIRECT(unit)); */
  if (dmaable) {
      data = (uint32 *)soc_cm_salloc(unit, region_size*ARAD_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_ENTRY_SIZE*sizeof(uint32), "IPS_QPM_1_SYS_REDm"); /* allocate DMA memory buffer */
  } else {
      data = (uint32*)soc_sand_os_malloc_any_size(region_size*ARAD_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_ENTRY_SIZE*sizeof(uint32), "IPS_QPM_1}");
  }
  if (data == NULL) {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 10, exit);
  }
  err = soc_sand_os_memset(data, 0x0, region_size*ARAD_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_ENTRY_SIZE*sizeof(uint32));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  for (i=0, cur_data=data; i<region_size; i++, cur_data++) {
      soc_mem_field32_set(unit, IPS_QPM_1_SYS_REDm, cur_data, SYS_PHY_PORTf, system_physical_port_tbl_data->sys_phy_port);
  }
  err = soc_mem_array_write_range(unit, flags, IPS_QPM_1_SYS_REDm, 0, MEM_BLOCK_ALL, index_min, index_max, data);
  SOC_SAND_CHECK_FUNC_RESULT(err, 30, exit);

exit:
    if (data != NULL) {
        if (dmaable) {
            soc_cm_sfree(unit, data);
        } else  {
            soc_sand_os_free(data);
        }
    }
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_indirect_base_queue_to_system_physical_port_tbl_region_set_unsafe()",0,0);
}
const soc_field_t qpm2_field_per_base_queue[BASE_QUEUES_PER_QPM_2_ENTRY] = {DEST_PORT_0f, DEST_PORT_1f, DEST_PORT_2f, DEST_PORT_3f};

/*
 * Read direct tables QPM_1,QPM_2 to get a mapping from a base queue to fap module x port
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_direct_base_queue_to_system_physical_port_tbl_get_unsafe(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 base_queue,
    SOC_SAND_OUT ARAD_BASE_Q_TO_MOD_PORT_TABLE_TBL_DATA* mod_port_tbl_data
  )
{
  uint32 err;
  uint32 data = 0;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  /* assert(ARAD_IS_VOQ_MAPPING_DIRECT(unit)); */

  err = READ_IPS_QPM_1_NO_SYS_REDm(unit, MEM_BLOCK_ANY, base_queue, &data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 10, exit);

  mod_port_tbl_data->fap_id = soc_mem_field32_get(unit, IPS_QPM_1_NO_SYS_REDm, &data, DEST_DEVf);
  mod_port_tbl_data->fap_port_id = soc_mem_field32_get(unit, IPS_QPM_1_NO_SYS_REDm, &data, DEST_PORT_MSBf) << BASE_QUEUES_PER_QPM_FAP_PORT_MSB_OFFSET;

  data = 0;
  err = READ_IPS_QPM_2_NO_SYS_REDm(unit, MEM_BLOCK_ANY, base_queue / BASE_QUEUES_PER_QPM_2_ENTRY, &data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 20, exit);
  mod_port_tbl_data->fap_port_id |= soc_mem_field32_get(unit, IPS_QPM_2_NO_SYS_REDm, &data, qpm2_field_per_base_queue[base_queue % BASE_QUEUES_PER_QPM_2_ENTRY]);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_direct_base_queue_to_system_physical_port_tbl_get_unsafe()",base_queue ,0);
}

/*
 * Write direct tables QPM_1,QPM_2 to set a mapping from a base queue to fap module x port
 * Write indirect table system_physical_port_lookup_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_direct_base_queue_to_system_physical_port_tbl_set_unsafe(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 base_queue,
    SOC_SAND_IN ARAD_BASE_Q_TO_MOD_PORT_TABLE_TBL_DATA* mod_port_tbl_data
  )
{
  uint32 err;
  uint32 data = 0;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  /* assert(ARAD_IS_VOQ_MAPPING_DIRECT(unit)); */

  soc_mem_field32_set(unit, IPS_QPM_1_NO_SYS_REDm, &data, DEST_DEVf, mod_port_tbl_data->fap_id);
  soc_mem_field32_set(unit, IPS_QPM_1_NO_SYS_REDm, &data, DEST_PORT_MSBf, mod_port_tbl_data->fap_port_id >> BASE_QUEUES_PER_QPM_FAP_PORT_MSB_OFFSET);
  err = WRITE_IPS_QPM_1_NO_SYS_REDm(unit, MEM_BLOCK_ALL, base_queue, &data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 10, exit);

  data = 0;
  err = READ_IPS_QPM_2_NO_SYS_REDm(unit, MEM_BLOCK_ANY, base_queue / BASE_QUEUES_PER_QPM_2_ENTRY, &data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 20, exit);

  soc_mem_field32_set(unit, IPS_QPM_2_NO_SYS_REDm, &data, qpm2_field_per_base_queue[base_queue % BASE_QUEUES_PER_QPM_2_ENTRY], mod_port_tbl_data->fap_port_id & BASE_QUEUES_PER_QPM_FAP_PORT_NO_MSB_MASK );

  err = WRITE_IPS_QPM_2_NO_SYS_REDm(unit, MEM_BLOCK_ALL, base_queue / BASE_QUEUES_PER_QPM_2_ENTRY, &data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_direct_base_queue_to_system_physical_port_tbl_set_unsafe()",base_queue ,0);
}

/*
 * Write direct tables QPM_1,QPM_2 to set a mapping from a base queue to fap module x port
 * Write indirect table system_physical_port_lookup_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_direct_base_queue_to_system_physical_port_tbl_region_set_unsafe(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 entry_offset,
    SOC_SAND_IN uint32 region_size,
    SOC_SAND_IN ARAD_BASE_Q_TO_MOD_PORT_TABLE_TBL_DATA* mod_port_tbl_data
  )
{
  uint32 err;
  uint32 *data = NULL, *cur_data;
  uint32 flags=0;
  int index_max, index_min;
  int i; 
  int dmaable = soc_mem_dmaable(unit, IPS_QPM_2_NO_SYS_REDm, SOC_MEM_BLOCK_ANY(unit, IPS_QPM_2_NO_SYS_REDm)); /* check if we can use DMA */
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  index_min=entry_offset;
  index_max=entry_offset+region_size-1;

  if (dmaable) {
      data = (uint32 *)soc_cm_salloc(unit, region_size*sizeof(uint32), "IPS_QPM_2_NO_SYS_REDm"); /* allocate DMA memory buffer */
  } else {
      data = (uint32*)soc_sand_os_malloc_any_size(region_size*sizeof(uint32), "IPS_QPM_2_NO_SYS_REDm");
  }
  if (data == NULL) {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 10, exit);
  }
  err = soc_sand_os_memset(data, 0x0, region_size*sizeof(uint32));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  /* assert(ARAD_IS_VOQ_MAPPING_DIRECT(unit)); */

  for (i=0, cur_data=data; i<region_size; i++, cur_data++) {
    soc_mem_field32_set(unit, IPS_QPM_1_NO_SYS_REDm, cur_data, DEST_DEVf, mod_port_tbl_data->fap_id);
    soc_mem_field32_set(unit, IPS_QPM_1_NO_SYS_REDm, cur_data, DEST_PORT_MSBf, mod_port_tbl_data->fap_port_id >> BASE_QUEUES_PER_QPM_FAP_PORT_MSB_OFFSET);
  }
  err = soc_mem_array_write_range(unit, flags, IPS_QPM_1_NO_SYS_REDm, 0, MEM_BLOCK_ALL, index_min , index_max, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);


  err = soc_sand_os_memset(data, 0x0, region_size*sizeof(uint32));
  SOC_SAND_CHECK_FUNC_RESULT(err, 40, exit);


  err = soc_mem_array_read_range(unit, IPS_QPM_2_NO_SYS_REDm, 0, MEM_BLOCK_ANY, index_min / BASE_QUEUES_PER_QPM_2_ENTRY, index_max / BASE_QUEUES_PER_QPM_2_ENTRY, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 50, exit);

  for (i=0, cur_data=data; i<region_size; i++, cur_data++) {
      soc_mem_field32_set(unit, IPS_QPM_2_NO_SYS_REDm, cur_data, qpm2_field_per_base_queue[(entry_offset+i)% BASE_QUEUES_PER_QPM_2_ENTRY], mod_port_tbl_data->fap_port_id & BASE_QUEUES_PER_QPM_FAP_PORT_NO_MSB_MASK );
  }
  err = soc_mem_array_write_range(unit, flags, IPS_QPM_2_NO_SYS_REDm, 0, MEM_BLOCK_ALL, index_min / BASE_QUEUES_PER_QPM_2_ENTRY , index_max / BASE_QUEUES_PER_QPM_2_ENTRY, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 60, exit);

exit:
    if (data != NULL) {
        if (dmaable) {
            soc_cm_sfree(unit, data);
        } else  {
            soc_sand_os_free(data);
        }
    }
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_direct_base_queue_to_system_physical_port_tbl_region_set_unsafe()",entry_offset ,0);
}


/*
 * Read indirect table destination_device_and_port_lookup_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_indirect_sysport_to_modport_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_MOD_PORT_TBL_DATA* IPS_destination_device_and_port_lookup_table_tbl_data
  )
{
  uint32 err;
  uint32 data[ARAD_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_GET_UNSAFE);
  /* assert(ARAD_IS_VOQ_MAPPING_INDIRECT(unit)); */

  err = soc_sand_os_memset(&(data[0]), 0x0, sizeof(data));
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_sand_os_memset(IPS_destination_device_and_port_lookup_table_tbl_data, 0x0, sizeof(ARAD_MOD_PORT_TBL_DATA));
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_mem_read(unit, IPS_QPM_2_SYS_REDm, MEM_BLOCK_ANY, entry_offset, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

  IPS_destination_device_and_port_lookup_table_tbl_data->dest_port = soc_mem_field32_get(unit, IPS_QPM_2_SYS_REDm, data, DEST_PORTf);
  IPS_destination_device_and_port_lookup_table_tbl_data->dest_dev = soc_mem_field32_get(unit, IPS_QPM_2_SYS_REDm, data, DEST_DEVf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_indirect_sysport_to_modport_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table destination_device_and_port_lookup_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_indirect_sysport_to_modport_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_MOD_PORT_TBL_DATA* IPS_destination_device_and_port_lookup_table_tbl_data
  )
{
  uint32 err;
  uint32 data[ARAD_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_SET_UNSAFE);
  /* assert(ARAD_IS_VOQ_MAPPING_INDIRECT(unit)); */

  err = soc_sand_os_memset(&(data[0]), 0x0, sizeof(data));
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  soc_mem_field32_set(unit, IPS_QPM_2_SYS_REDm, data, DEST_PORTf, IPS_destination_device_and_port_lookup_table_tbl_data->dest_port);
  soc_mem_field32_set(unit, IPS_QPM_2_SYS_REDm, data, DEST_DEVf, IPS_destination_device_and_port_lookup_table_tbl_data->dest_dev);

  err = soc_mem_write(unit, IPS_QPM_2_SYS_REDm, MEM_BLOCK_ALL, entry_offset, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_indirect_sysport_to_modport_tbl_set_unsafe()",0,0);
}
/*
 * Read indirect table flow_id_lookup_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_ips_flow_id_lookup_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IPS_FLOW_ID_LOOKUP_TABLE_TBL_DATA* IPS_flow_id_lookup_table_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_IPS_FLOW_ID_LOOKUP_TABLE_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IPS_FLOW_ID_LOOKUP_TABLE_TBL_GET_UNSAFE);

  err = soc_sand_os_memset(&(data[0]), 0x0, sizeof(data));
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);
  err = soc_sand_os_memset(IPS_flow_id_lookup_table_tbl_data, 0x0, sizeof(ARAD_IPS_FLOW_ID_LOOKUP_TABLE_TBL_DATA));
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_mem_read(unit, IPS_FLOW_ID_LOOKUP_TABLEm, MEM_BLOCK_ANY, entry_offset, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

  IPS_flow_id_lookup_table_tbl_data->base_flow = soc_mem_field32_get(unit, IPS_FLOW_ID_LOOKUP_TABLEm, data, BASE_FLOWf);
  IPS_flow_id_lookup_table_tbl_data->sub_flow_mode = soc_mem_field32_get(unit, IPS_FLOW_ID_LOOKUP_TABLEm, data, SUB_FLOW_MODEf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ips_flow_id_lookup_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table flow_id_lookup_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_ips_flow_id_lookup_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IPS_FLOW_ID_LOOKUP_TABLE_TBL_DATA* IPS_flow_id_lookup_table_tbl_data
  )
{
  uint32 err;
  uint32 data[ARAD_IPS_FLOW_ID_LOOKUP_TABLE_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IPS_FLOW_ID_LOOKUP_TABLE_TBL_SET_UNSAFE);

  err = soc_sand_os_memset(&(data[0]), 0x0, sizeof(data));
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  soc_mem_field32_set(unit, IPS_FLOW_ID_LOOKUP_TABLEm, data, BASE_FLOWf, IPS_flow_id_lookup_table_tbl_data->base_flow);
  soc_mem_field32_set(unit, IPS_FLOW_ID_LOOKUP_TABLEm, data, SUB_FLOW_MODEf, IPS_flow_id_lookup_table_tbl_data->sub_flow_mode);

  err = soc_mem_write(unit, IPS_FLOW_ID_LOOKUP_TABLEm, MEM_BLOCK_ALL, entry_offset, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ips_flow_id_lookup_table_tbl_set_unsafe()",0,0);
}
/*
 * Read indirect table queue_type_lookup_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_ips_queue_type_lookup_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_DATA* IPS_queue_type_lookup_table_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_GET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_sand_os_memset(
          IPS_queue_type_lookup_table_tbl_data,
          0x0,
          sizeof(ARAD_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

 
 




  err = soc_mem_read(
          unit,
          IPS_QTYPEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

err=SOC_E_NONE;
    if(SOC_E_NONE == err) {
          IPS_queue_type_lookup_table_tbl_data->queue_type_lookup_table   = soc_mem_field32_get(
                  unit,
                  IPS_QTYPEm,
                  data,
                  QTYPEf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ips_queue_type_lookup_table_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table flow_id_lookup_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_ips_flow_id_lookup_table_tbl_region_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   uint32              region_size,
    SOC_SAND_IN   ARAD_IPS_FLOW_ID_LOOKUP_TABLE_TBL_DATA* IPS_flow_id_lookup_table_tbl_data
  )
{
  uint32 err;
  uint32 *data = NULL, *cur_data;
  uint32 flags=0;
  int index_max, index_min;
  int i; 
  int dmaable = soc_mem_dmaable(unit, IPS_FLOW_ID_LOOKUP_TABLEm, SOC_MEM_BLOCK_ANY(unit, IPS_FLOW_ID_LOOKUP_TABLEm)); /* check if we can use DMA */
   
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IPS_FLOW_ID_LOOKUP_TABLE_TBL_SET_UNSAFE);
  index_min=entry_offset;
  index_max=entry_offset+region_size-1;
  if (dmaable) {
      data = (uint32 *)soc_cm_salloc(unit, region_size*sizeof(uint32), "IPS_FLOW_ID_LOOKUP_TABLEm"); /* allocate DMA memory buffer */
  } else {
      data = (uint32*)soc_sand_os_malloc_any_size(region_size*sizeof(uint32), "IPS_FLOW_ID_LOOKUP_TABLEm}");
  }
  if (data == NULL) {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 10, exit);
  }

  err = soc_sand_os_memset(data, 0x0, region_size*sizeof(uint32));
  SOC_SAND_CHECK_FUNC_RESULT(err, 20, exit);

  for (i=0, cur_data=data; i<region_size; i++, cur_data++) {
      *cur_data = 0;
      soc_mem_field32_set(unit, IPS_FLOW_ID_LOOKUP_TABLEm, cur_data, BASE_FLOWf, IPS_flow_id_lookup_table_tbl_data->base_flow);
      soc_mem_field32_set(unit, IPS_FLOW_ID_LOOKUP_TABLEm, cur_data, SUB_FLOW_MODEf, IPS_flow_id_lookup_table_tbl_data->sub_flow_mode);
  }

  err = soc_mem_array_write_range(unit, flags, IPS_FLOW_ID_LOOKUP_TABLEm, 0, MEM_BLOCK_ALL, index_min , index_max, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);


exit:
    if (data != NULL) {
        if (dmaable) {
            soc_cm_sfree(unit, data);
        } else  {
            soc_sand_os_free(data);
        }
    }
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ips_flow_id_lookup_table_tbl_region_set_unsafe()",0,0);
}

/*
 * Write indirect table queue_type_lookup_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_ips_queue_type_lookup_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_DATA* IPS_queue_type_lookup_table_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_SET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

 
 




err=SOC_E_NONE;
if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          IPS_QTYPEm,
          data,
          QTYPEf,
          IPS_queue_type_lookup_table_tbl_data->queue_type_lookup_table );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_mem_write(
          unit,
          IPS_QTYPEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ips_queue_type_lookup_table_tbl_set_unsafe()",0,0);
}
/*
 * Read indirect table queue_priority_map_select_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_ips_queue_priority_map_select_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_DATA* IPS_queue_priority_map_select_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_GET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_sand_os_memset(
          IPS_queue_priority_map_select_tbl_data,
          0x0,
          sizeof(ARAD_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

 
 




  err = soc_mem_read(
          unit,
          IPS_QPRISELm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

err=SOC_E_NONE;
    if(SOC_E_NONE == err) {
          IPS_queue_priority_map_select_tbl_data->queue_priority_map_select   = soc_mem_field32_get(
                  unit,
                  IPS_QPRISELm,
                  data,
                  QPRISELf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ips_queue_priority_map_select_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table queue_priority_map_select_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_ips_queue_priority_map_select_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_DATA* IPS_queue_priority_map_select_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_SET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

 
 




err=SOC_E_NONE;
if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          IPS_QPRISELm,
          data,
          QPRISELf,
          IPS_queue_priority_map_select_tbl_data->queue_priority_map_select );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_mem_write(
          unit,
          IPS_QPRISELm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ips_queue_priority_map_select_tbl_set_unsafe()",0,0);
}
/*
 * Read indirect table queue_priority_maps_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_ips_queue_priority_maps_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_DATA* IPS_queue_priority_maps_table_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_GET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_sand_os_memset(
          IPS_queue_priority_maps_table_tbl_data,
          0x0,
          sizeof(ARAD_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

 
 




  err = soc_mem_read(
          unit,
          IPS_Q_PRIORITY_BIT_MAPm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

err=SOC_E_NONE;
    if(SOC_E_NONE == err) {
             soc_mem_field_get(
                  unit,
                  IPS_Q_PRIORITY_BIT_MAPm,
                  data,
                  Q_PRIORITY_BIT_MAPf,
                IPS_queue_priority_maps_table_tbl_data->queue_priority_maps_table            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ips_queue_priority_maps_table_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table queue_priority_maps_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_ips_queue_priority_maps_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_DATA* IPS_queue_priority_maps_table_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_SET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

 
 




err=SOC_E_NONE;
if(SOC_E_NONE == err) {
            soc_mem_field_set(
          unit,
          IPS_Q_PRIORITY_BIT_MAPm,
          data,
          Q_PRIORITY_BIT_MAPf,
          (uint32*)&(IPS_queue_priority_maps_table_tbl_data->queue_priority_maps_table[0]) );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_mem_write(
          unit,
          IPS_Q_PRIORITY_BIT_MAPm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ips_queue_priority_maps_table_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table queue_size_based_thresholds_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_ips_queue_size_based_thresholds_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_DATA* IPS_queue_size_based_thresholds_table_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_GET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_sand_os_memset(
          IPS_queue_size_based_thresholds_table_tbl_data,
          0x0,
          sizeof(ARAD_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

 
 




  err = soc_mem_read(
          unit,
          IPS_QSZTHm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

err=SOC_E_NONE;
    if(SOC_E_NONE == err) {
          IPS_queue_size_based_thresholds_table_tbl_data->off_to_slow_msg_th   = soc_mem_field32_get(
                  unit,
                  IPS_QSZTHm,
                  data,
                  OFF_TO_SLOW_MSG_THf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 40, exit);

    if(SOC_E_NONE == err) {
          IPS_queue_size_based_thresholds_table_tbl_data->off_to_norm_msg_th   = soc_mem_field32_get(
                  unit,
                  IPS_QSZTHm,
                  data,
                  OFF_TO_NORM_MSG_THf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 41, exit);

    if(SOC_E_NONE == err) {
          IPS_queue_size_based_thresholds_table_tbl_data->slow_to_norm_msg_th   = soc_mem_field32_get(
                  unit,
                  IPS_QSZTHm,
                  data,
                  SLOW_TO_NORM_MSG_THf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 42, exit);

    if(SOC_E_NONE == err) {
          IPS_queue_size_based_thresholds_table_tbl_data->norm_to_slow_msg_th   = soc_mem_field32_get(
                  unit,
                  IPS_QSZTHm,
                  data,
                  NORM_TO_SLOW_MSG_THf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 43, exit);

    if(SOC_E_NONE == err) {
          IPS_queue_size_based_thresholds_table_tbl_data->fsm_th_mul   = soc_mem_field32_get(
                  unit,
                  IPS_QSZTHm,
                  data,
                  FSM_TH_MULf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 44, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ips_queue_size_based_thresholds_table_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table queue_size_based_thresholds_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_ips_queue_size_based_thresholds_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_DATA* IPS_queue_size_based_thresholds_table_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_SET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

 
 




err=SOC_E_NONE;
if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          IPS_QSZTHm,
          data,
          OFF_TO_SLOW_MSG_THf,
          IPS_queue_size_based_thresholds_table_tbl_data->off_to_slow_msg_th );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          IPS_QSZTHm,
          data,
          OFF_TO_NORM_MSG_THf,
          IPS_queue_size_based_thresholds_table_tbl_data->off_to_norm_msg_th );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 31, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          IPS_QSZTHm,
          data,
          SLOW_TO_NORM_MSG_THf,
          IPS_queue_size_based_thresholds_table_tbl_data->slow_to_norm_msg_th );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 32, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          IPS_QSZTHm,
          data,
          NORM_TO_SLOW_MSG_THf,
          IPS_queue_size_based_thresholds_table_tbl_data->norm_to_slow_msg_th );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 33, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          IPS_QSZTHm,
          data,
          FSM_TH_MULf,
          IPS_queue_size_based_thresholds_table_tbl_data->fsm_th_mul );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 34, exit);

  err = soc_mem_write(
          unit,
          IPS_QSZTHm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 35, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ips_queue_size_based_thresholds_table_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table credit_balance_based_thresholds_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_ips_credit_balance_based_thresholds_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_DATA* IPS_credit_balance_based_thresholds_table_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_GET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_sand_os_memset(
          IPS_credit_balance_based_thresholds_table_tbl_data,
          0x0,
          sizeof(ARAD_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

 
 




  err = soc_mem_read(
          unit,
          IPS_CRBALTHm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

err=SOC_E_NONE;
    if(SOC_E_NONE == err) {
          IPS_credit_balance_based_thresholds_table_tbl_data->backoff_enter_qcr_bal_th   = soc_mem_field32_get(
                  unit,
                  IPS_CRBALTHm,
                  data,
                  BACKOFF_ENTER_Q_CR_BAL_THf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 40, exit);

    if(SOC_E_NONE == err) {
          IPS_credit_balance_based_thresholds_table_tbl_data->backoff_exit_qcr_bal_th   = soc_mem_field32_get(
                  unit,
                  IPS_CRBALTHm,
                  data,
                  BACKOFF_EXIT_Q_CR_BAL_THf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 41, exit);

    if(SOC_E_NONE == err) {
          IPS_credit_balance_based_thresholds_table_tbl_data->backlog_enter_qcr_bal_th   = soc_mem_field32_get(
                  unit,
                  IPS_CRBALTHm,
                  data,
                  BACKLOG_ENTER_Q_CR_BAL_THf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 42, exit);

    if(SOC_E_NONE == err) {
          IPS_credit_balance_based_thresholds_table_tbl_data->backlog_exit_qcr_bal_th   = soc_mem_field32_get(
                  unit,
                  IPS_CRBALTHm,
                  data,
                  BACKLOG_EXIT_Q_CR_BAL_THf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 43, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ips_credit_balance_based_thresholds_table_tbl_get_unsafe()",0,0);
}
/*
 * Write indirect table credit_balance_based_thresholds_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_ips_credit_balance_based_thresholds_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_DATA* IPS_credit_balance_based_thresholds_table_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_SET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

 
 




err=SOC_E_NONE;
if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          IPS_CRBALTHm,
          data,
          BACKOFF_ENTER_Q_CR_BAL_THf,
          IPS_credit_balance_based_thresholds_table_tbl_data->backoff_enter_qcr_bal_th );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          IPS_CRBALTHm,
          data,
          BACKOFF_EXIT_Q_CR_BAL_THf,
          IPS_credit_balance_based_thresholds_table_tbl_data->backoff_exit_qcr_bal_th );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 31, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          IPS_CRBALTHm,
          data,
          BACKLOG_ENTER_Q_CR_BAL_THf,
          IPS_credit_balance_based_thresholds_table_tbl_data->backlog_enter_qcr_bal_th );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 32, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          IPS_CRBALTHm,
          data,
          BACKLOG_EXIT_Q_CR_BAL_THf,
          IPS_credit_balance_based_thresholds_table_tbl_data->backlog_exit_qcr_bal_th );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 33, exit);

  err = soc_mem_write(
          unit,
          IPS_CRBALTHm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 34, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ips_credit_balance_based_thresholds_table_tbl_set_unsafe()",0,0);
}
/*
 * Read indirect table empty_queue_credit_balance_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_ips_empty_queue_credit_balance_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_DATA* IPS_empty_queue_credit_balance_table_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_GET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_sand_os_memset(
          IPS_empty_queue_credit_balance_table_tbl_data,
          0x0,
          sizeof(ARAD_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

 
 




  err = soc_mem_read(
          unit,
          IPS_EMPTYQCRBALm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

err=SOC_E_NONE;
    if(SOC_E_NONE == err) {
          IPS_empty_queue_credit_balance_table_tbl_data->empty_qsatisfied_cr_bal   = soc_mem_field32_get(
                  unit,
                  IPS_EMPTYQCRBALm,
                  data,
                  EMPTY_Q_SATISFIED_CR_BALf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 40, exit);

    if(SOC_E_NONE == err) {
          IPS_empty_queue_credit_balance_table_tbl_data->max_empty_qcr_bal   = soc_mem_field32_get(
                  unit,
                  IPS_EMPTYQCRBALm,
                  data,
                  MAX_EMPTY_Q_CR_BALf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 41, exit);

    if(SOC_E_NONE == err) {
          IPS_empty_queue_credit_balance_table_tbl_data->exceed_max_empty_qcr_bal   = soc_mem_field32_get(
                  unit,
                  IPS_EMPTYQCRBALm,
                  data,
                  EXCEED_MAX_EMPTY_Q_CR_BALf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 42, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ips_empty_queue_credit_balance_table_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table empty_queue_credit_balance_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_ips_empty_queue_credit_balance_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_DATA* IPS_empty_queue_credit_balance_table_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_SET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

 
 




err=SOC_E_NONE;
if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          IPS_EMPTYQCRBALm,
          data,
          EMPTY_Q_SATISFIED_CR_BALf,
          IPS_empty_queue_credit_balance_table_tbl_data->empty_qsatisfied_cr_bal );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          IPS_EMPTYQCRBALm,
          data,
          MAX_EMPTY_Q_CR_BALf,
          IPS_empty_queue_credit_balance_table_tbl_data->max_empty_qcr_bal );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 31, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          IPS_EMPTYQCRBALm,
          data,
          EXCEED_MAX_EMPTY_Q_CR_BALf,
          IPS_empty_queue_credit_balance_table_tbl_data->exceed_max_empty_qcr_bal );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 32, exit);

  err = soc_mem_write(
          unit,
          IPS_EMPTYQCRBALm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 33, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ips_empty_queue_credit_balance_table_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table credit_watchdog_thresholds_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_ips_credit_watchdog_thresholds_table_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_DATA* IPS_credit_watchdog_thresholds_table_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_GET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_sand_os_memset(
          IPS_credit_watchdog_thresholds_table_tbl_data,
          0x0,
          sizeof(ARAD_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

 
 




  err = soc_mem_read(
          unit,
          IPS_CRWDTHm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

err=SOC_E_NONE;
    if(SOC_E_NONE == err) {
          IPS_credit_watchdog_thresholds_table_tbl_data->wd_status_msg_gen_period   = soc_mem_field32_get(
                  unit,
                  IPS_CRWDTHm,
                  data,
                  WD_STATUS_MSG_GEN_PERIODf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 40, exit);

    if(SOC_E_NONE == err) {
          IPS_credit_watchdog_thresholds_table_tbl_data->wd_delete_qth   = soc_mem_field32_get(
                  unit,
                  IPS_CRWDTHm,
                  data,
                  WD_DELETE_Q_THf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ips_credit_watchdog_thresholds_table_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table credit_watchdog_thresholds_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_ips_credit_watchdog_thresholds_table_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_DATA* IPS_credit_watchdog_thresholds_table_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_SET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

 
 




err=SOC_E_NONE;
if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          IPS_CRWDTHm,
          data,
          WD_STATUS_MSG_GEN_PERIODf,
          IPS_credit_watchdog_thresholds_table_tbl_data->wd_status_msg_gen_period );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          IPS_CRWDTHm,
          data,
          WD_DELETE_Q_THf,
          IPS_credit_watchdog_thresholds_table_tbl_data->wd_delete_qth );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_mem_write(
          unit,
          IPS_CRWDTHm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ips_credit_watchdog_thresholds_table_tbl_set_unsafe()",0,0);
}

STATIC soc_mem_t
  arad_egq_scm_chan_arb_id2scm_id(
    SOC_SAND_IN  ARAD_OFP_RATES_EGQ_CHAN_ARB_ID chan_arb_id
  )
{  
  soc_mem_t
    egq_scm_name;
  /*
   *  Go to the correct table
   */
  switch (chan_arb_id)
  {
  case ARAD_OFP_RATES_EGQ_CHAN_ARB_00:
    egq_scm_name = EGQ_CH_0_SCMm;
    break;
  case ARAD_OFP_RATES_EGQ_CHAN_ARB_01:
    egq_scm_name = EGQ_CH_1_SCMm;
    break;
  case ARAD_OFP_RATES_EGQ_CHAN_ARB_02:
    egq_scm_name = EGQ_CH_2_SCMm;
    break;
  case ARAD_OFP_RATES_EGQ_CHAN_ARB_03:
    egq_scm_name = EGQ_CH_3_SCMm;
    break;
  case ARAD_OFP_RATES_EGQ_CHAN_ARB_04:
    egq_scm_name = EGQ_CH_4_SCMm;
    break;
  case ARAD_OFP_RATES_EGQ_CHAN_ARB_05:
    egq_scm_name = EGQ_CH_5_SCMm;
    break;
  case ARAD_OFP_RATES_EGQ_CHAN_ARB_06:
    egq_scm_name = EGQ_CH_6_SCMm;
    break;
  case ARAD_OFP_RATES_EGQ_CHAN_ARB_07:
    egq_scm_name = EGQ_CH_7_SCMm;
    break;
  case ARAD_OFP_RATES_EGQ_CHAN_ARB_CPU:
    egq_scm_name = EGQ_CH_8_SCMm;
    break;
  case ARAD_OFP_RATES_EGQ_CHAN_ARB_RCY:
    egq_scm_name = EGQ_CH_9_SCMm;
    break;
  case ARAD_OFP_RATES_EGQ_CHAN_ARB_NON_CHAN:
    egq_scm_name = EGQ_NONCH_SCMm;
    break;
  default:
    egq_scm_name = EGQ_CH_0_SCMm;
  }

  return egq_scm_name;
}

/*
 * Read indirect table scm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  arad_egq_scm_tbl_get_unsafe(
    SOC_SAND_IN   int                       unit,
    SOC_SAND_IN   ARAD_OFP_RATES_CAL_INFO        *cal_info,
    SOC_SAND_IN   uint32                        entry_offset,
    SOC_SAND_OUT  ARAD_EGQ_SCM_TBL_DATA*          EGQ_nif_scm_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_EGQ_NIF_SCM_TBL_ENTRY_SIZE];
  soc_mem_t
    egq_scm_name;
  soc_field_t
    egq_cr_field_name,
    egq_index_field_name;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGQ_SCM_TBL_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(cal_info);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_sand_os_memset(
          EGQ_nif_scm_tbl_data,
          0x0,
          sizeof(ARAD_EGQ_SCM_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  switch (cal_info->cal_type)
  {
  case ARAD_OFP_RATES_EGQ_CAL_CHAN_ARB:
    if (cal_info->chan_arb_id < 0 || cal_info->chan_arb_id > ARAD_OFP_RATES_EGQ_CHAN_ARB_NON_CHAN)
    {
      SOC_SAND_SET_ERROR_CODE(ARAD_OFP_RATES_CHAN_ARB_INVALID_ERR, 20, exit);
    }
    egq_scm_name = arad_egq_scm_chan_arb_id2scm_id(cal_info->chan_arb_id);
    egq_cr_field_name = PORT_CR_TO_ADDf;
    egq_index_field_name = OFP_INDEXf;
    break;
  case ARAD_OFP_RATES_EGQ_CAL_PORT_PRIORITY:
    egq_scm_name = EGQ_QP_SCMm;
    egq_cr_field_name = QPAIR_CR_TO_ADDf;
    egq_index_field_name = QPAIR_INDEXf;
    break;
  case ARAD_OFP_RATES_EGQ_CAL_TCG:
    egq_scm_name = EGQ_TCG_SCMm;
    egq_cr_field_name = TCG_CR_TO_ADDf;
    egq_index_field_name = TCG_INDEXf;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(ARAD_OFP_RATES_EGQ_CAL_INVALID_ERR, 25, exit);
  }
  

  err = soc_mem_read(
          unit,
          egq_scm_name,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

err=SOC_E_NONE;
    if(SOC_E_NONE == err) {
          EGQ_nif_scm_tbl_data->port_cr_to_add = soc_mem_field32_get(
                  unit,
                  egq_scm_name,
                  data,
                  egq_cr_field_name    );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 40, exit);

    if(SOC_E_NONE == err) {
          EGQ_nif_scm_tbl_data->ofp_index = soc_mem_field32_get(
                  unit,
                  egq_scm_name,
                  data,
                egq_index_field_name );
  }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egq_scm_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table scm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_egq_scm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   ARAD_OFP_RATES_CAL_INFO *cal_info,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_EGQ_SCM_TBL_DATA* EGQ_nif_scm_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_EGQ_NIF_SCM_TBL_ENTRY_SIZE];
  soc_mem_t
    egq_scm_name;
  soc_field_t
    egq_cr_field_name,
    egq_index_field_name;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGQ_SCM_TBL_SET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  switch (cal_info->cal_type)
  {
  case ARAD_OFP_RATES_EGQ_CAL_CHAN_ARB:
    if (cal_info->chan_arb_id < 0 || cal_info->chan_arb_id > ARAD_OFP_RATES_EGQ_CHAN_ARB_NON_CHAN)
    {
      SOC_SAND_SET_ERROR_CODE(ARAD_OFP_RATES_CHAN_ARB_INVALID_ERR, 20, exit);
    }
     /*
      *  Go to the correct table, per chan arb id
      */
    egq_scm_name = arad_egq_scm_chan_arb_id2scm_id(cal_info->chan_arb_id);
    egq_cr_field_name = PORT_CR_TO_ADDf;
    egq_index_field_name = OFP_INDEXf;
    break;
  case ARAD_OFP_RATES_EGQ_CAL_PORT_PRIORITY:
    egq_scm_name = EGQ_QP_SCMm;
    egq_cr_field_name = QPAIR_CR_TO_ADDf;
    egq_index_field_name = QPAIR_INDEXf;
    break;
  case ARAD_OFP_RATES_EGQ_CAL_TCG:
    egq_scm_name = EGQ_TCG_SCMm;
    egq_cr_field_name = TCG_CR_TO_ADDf;
    egq_index_field_name = TCG_INDEXf;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(ARAD_OFP_RATES_EGQ_CAL_INVALID_ERR, 25, exit);
  }  

err=SOC_E_NONE;
if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          egq_scm_name,
          data,
          egq_cr_field_name,
          EGQ_nif_scm_tbl_data->port_cr_to_add);
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          egq_scm_name,
          data,
          egq_index_field_name,
          EGQ_nif_scm_tbl_data->ofp_index );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_mem_write(
          unit,
          egq_scm_name,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egq_scm_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table ccm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_egq_ccm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_EGQ_CCM_TBL_DATA* EGQ_ccm_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_EGQ_CCM_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGQ_CCM_TBL_GET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_sand_os_memset(
          EGQ_ccm_tbl_data,
          0x0,
          sizeof(ARAD_EGQ_CCM_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_mem_read(
          unit,
          EGQ_CCMm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

err=SOC_E_NONE;
    if(SOC_E_NONE == err) {
          EGQ_ccm_tbl_data->interface_select   = soc_mem_field32_get(
              unit,
                  EGQ_CCMm,
                  data,
                  INTERFACE_SELECTf
          );
  }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egq_ccm_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table ccm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_egq_ccm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_EGQ_CCM_TBL_DATA* EGQ_ccm_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_EGQ_CCM_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGQ_CCM_TBL_SET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err=SOC_E_NONE;
  if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          EGQ_CCMm,
          data,
          INTERFACE_SELECTf,
          EGQ_ccm_tbl_data->interface_select
      );
  }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_mem_write(
          unit,
          EGQ_CCMm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egq_ccm_tbl_set_unsafe()",0,0);
}

STATIC
  uint32
    arad_egq_pmc_names_get(
      SOC_SAND_IN   int             unit,
      SOC_SAND_IN   ARAD_OFP_RATES_CAL_INFO *cal_info,
      SOC_SAND_OUT  soc_mem_t             *egq_pmc_name,
      SOC_SAND_OUT  soc_field_t           *egq_cr_field_name
    )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(cal_info);
  SOC_SAND_CHECK_NULL_INPUT(egq_pmc_name);
  SOC_SAND_CHECK_NULL_INPUT(egq_cr_field_name);

  switch (cal_info->cal_type)
  {
  case ARAD_OFP_RATES_EGQ_CAL_CHAN_ARB:
    *egq_pmc_name = EGQ_PMCm;
    *egq_cr_field_name = PORT_MAX_CREDITf;    
    break;
  case ARAD_OFP_RATES_EGQ_CAL_PORT_PRIORITY:
    *egq_pmc_name = EGQ_QP_PMCm;
    *egq_cr_field_name = QPAIR_MAX_CREDITf;
    break;
  case ARAD_OFP_RATES_EGQ_CAL_TCG:
    *egq_pmc_name = EGQ_TCG_PMCm;
    *egq_cr_field_name = TCG_MAX_CREDITf;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(ARAD_OFP_RATES_EGQ_CAL_INVALID_ERR, 25, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egq_pmc_names_get()",0,0);
}

/*
 * Read indirect table pmc_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_egq_pmc_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   ARAD_OFP_RATES_CAL_INFO *cal_info,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_EGQ_PMC_TBL_DATA* EGQ_pmc_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_EGQ_PMC_TBL_ENTRY_SIZE];
  soc_mem_t
    egq_pmc_name;
  soc_field_t
    egq_max_credit_field_name;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGQ_PMC_TBL_GET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_sand_os_memset(
          EGQ_pmc_tbl_data,
          0x0,
          sizeof(ARAD_EGQ_PMC_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = arad_egq_pmc_names_get(
          unit,
          cal_info,
          &egq_pmc_name,
          &egq_max_credit_field_name
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_mem_read(
          unit,
          egq_pmc_name,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

err=SOC_E_NONE;
    if(SOC_E_NONE == err) {
          EGQ_pmc_tbl_data->port_max_credit   = soc_mem_field32_get(
                  unit,
                  egq_pmc_name,
                  data,
                  egq_max_credit_field_name            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egq_pmc_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table pmc_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_egq_pmc_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   ARAD_OFP_RATES_CAL_INFO *cal_info,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_EGQ_PMC_TBL_DATA* EGQ_pmc_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_EGQ_PMC_TBL_ENTRY_SIZE];
  soc_mem_t
    egq_pmc_name;
  soc_field_t
    egq_max_credit_field_name;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGQ_PMC_TBL_SET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = arad_egq_pmc_names_get(
          unit,
          cal_info,
          &egq_pmc_name,
          &egq_max_credit_field_name
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

err=SOC_E_NONE;
  if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          egq_pmc_name,
          data,
          egq_max_credit_field_name,
          EGQ_pmc_tbl_data->port_max_credit
      );
  }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_mem_write(
          unit,
          egq_pmc_name,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egq_pmc_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table dwm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_egq_dwm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_EGQ_DWM_TBL_DATA* EGQ_dwm_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_EGQ_DWM_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGQ_DWM_TBL_GET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_sand_os_memset(
          EGQ_dwm_tbl_data,
          0x0,
          sizeof(ARAD_EGQ_DWM_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

  err = soc_mem_read(
          unit,
          EGQ_DWMm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

err=SOC_E_NONE;
    if(SOC_E_NONE == err) {
          EGQ_dwm_tbl_data->mc_or_mc_low_queue_weight = soc_mem_field32_get(
                  unit,
                  EGQ_DWMm,
                  data,
                  MC_OR_MC_LOW_QUEUE_WEIGHTf);
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 40, exit);

    if(SOC_E_NONE == err) {
          EGQ_dwm_tbl_data->uc_or_uc_low_queue_weight   = soc_mem_field32_get(
                  unit,
                  EGQ_DWMm,
                  data,
                  UC_OR_UC_LOW_QUEUE_WEIGHTf);
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egq_dwm_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table dwm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_egq_dwm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_EGQ_DWM_TBL_DATA* EGQ_dwm_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_EGQ_DWM_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EGQ_DWM_TBL_SET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

err=SOC_E_NONE;
  if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          EGQ_DWMm,
          data,
          MC_OR_MC_LOW_QUEUE_WEIGHTf,
          EGQ_dwm_tbl_data->mc_or_mc_low_queue_weight );
  }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

  if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          EGQ_DWMm,
          data,
          UC_OR_UC_LOW_QUEUE_WEIGHTf,
          EGQ_dwm_tbl_data->uc_or_uc_low_queue_weight );
  }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_mem_write(
          unit,
          EGQ_DWMm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egq_dwm_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table ppct_tbl from block EGQ,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

int
  arad_egq_ppct_tbl_get_unsafe(
    SOC_SAND_IN   int                       unit,
    SOC_SAND_IN   uint32                    base_q_pair,
    SOC_SAND_IN   int                       core_id,
    SOC_SAND_OUT  ARAD_EGQ_PPCT_TBL_DATA*   EGQ_ppct_tbl_data
  )
{
    uint32
        rv, tmp = 0;
    uint32
        data[SOC_DPP_DEFS_MAX(EGQ_PPCT_NOF_LONGS)];
 
    SOCDNX_INIT_FUNC_DEFS;

    sal_memset(EGQ_ppct_tbl_data, 0x0, sizeof(ARAD_EGQ_PPCT_TBL_DATA));

    rv = READ_EGQ_PER_PORT_CONFIGURATION_TABLE_PPCTm(unit, EGQ_BLOCK(unit, core_id), base_q_pair, data);
    SOCDNX_IF_ERR_EXIT(rv);

    if(SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        EGQ_ppct_tbl_data->base_q_pair_num = soc_mem_field32_get(unit, EGQ_PER_PORT_CONFIGURATION_TABLE_PPCTm, data, BASE_Q_PAIR_NUMf);
    } else {
        EGQ_ppct_tbl_data->base_q_pair_num = base_q_pair;
    }
    EGQ_ppct_tbl_data->cgm_interface = soc_mem_field32_get(unit, EGQ_PER_PORT_CONFIGURATION_TABLE_PPCTm, data, CGM_INTERFACEf);
    EGQ_ppct_tbl_data->cgm_port_profile = soc_mem_field32_get(unit, EGQ_PER_PORT_CONFIGURATION_TABLE_PPCTm, data, CGM_PORT_PROFILEf);
    EGQ_ppct_tbl_data->cnm_intrcpt_drop_en = soc_mem_field32_get(unit, EGQ_PER_PORT_CONFIGURATION_TABLE_PPCTm, data, CNM_INTRCPT_DROP_ENf);
    EGQ_ppct_tbl_data->cnm_intrcpt_fc_en = soc_mem_field32_get(unit, EGQ_PER_PORT_CONFIGURATION_TABLE_PPCTm, data, CNM_INTRCPT_FC_ENf);

    tmp = soc_mem_field32_get(unit, EGQ_PER_PORT_CONFIGURATION_TABLE_PPCTm, data, CNM_INTRCPT_FC_VEC_INDEXf);
    if(SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        SHR_BITCOPY_RANGE(&(EGQ_ppct_tbl_data->cnm_intrcpt_fc_vec_llfc),0,&tmp,0,5);
        SHR_BITCOPY_RANGE(&(EGQ_ppct_tbl_data->cnm_intrcpt_fc_vec_pfc),0,&tmp,5,8);   
    } else {
        SHR_BITCOPY_RANGE(&(EGQ_ppct_tbl_data->cnm_intrcpt_fc_vec_llfc),0,&tmp,0,8);
        SHR_BITCOPY_RANGE(&(EGQ_ppct_tbl_data->cnm_intrcpt_fc_vec_pfc),0,&tmp,8,7);  
    }

    EGQ_ppct_tbl_data->disable_filtering = soc_mem_field32_get(unit, EGQ_PER_PORT_CONFIGURATION_TABLE_PPCTm, data, DISABLE_FILTERINGf);
    EGQ_ppct_tbl_data->cos_map_profile = soc_mem_field32_get(unit, EGQ_PER_PORT_CONFIGURATION_TABLE_PPCTm, data, COS_MAP_PROFILEf);
    EGQ_ppct_tbl_data->ad_count_out_port_flag = soc_mem_field32_get(unit, EGQ_PER_PORT_CONFIGURATION_TABLE_PPCTm, data, ADVA_COUNT_OUT_PORT_FLAGf);
    EGQ_ppct_tbl_data->is_stacking_port = soc_mem_field32_get(unit, EGQ_PER_PORT_CONFIGURATION_TABLE_PPCTm, data, IS_STACKING_PORTf);
    EGQ_ppct_tbl_data->lb_key_min = soc_mem_field32_get(unit, EGQ_PER_PORT_CONFIGURATION_TABLE_PPCTm, data, LB_KEY_MINf);
    EGQ_ppct_tbl_data->lb_key_max = soc_mem_field32_get(unit, EGQ_PER_PORT_CONFIGURATION_TABLE_PPCTm, data, LB_KEY_MAXf);
    EGQ_ppct_tbl_data->pmf_data = soc_mem_field32_get(unit, EGQ_PER_PORT_CONFIGURATION_TABLE_PPCTm, data, PMF_DATAf);
    EGQ_ppct_tbl_data->peer_tm_domain_id = soc_mem_field32_get(unit, EGQ_PER_PORT_CONFIGURATION_TABLE_PPCTm, data, PEER_TM_DOMAIN_IDf);
    EGQ_ppct_tbl_data->port_type = soc_mem_field32_get(unit, EGQ_PER_PORT_CONFIGURATION_TABLE_PPCTm, data, PORT_TYPEf);

    if (SOC_IS_JERICHO(unit)) {
        EGQ_ppct_tbl_data->second_range_lb_key_min = soc_mem_field32_get(unit, EGQ_PER_PORT_CONFIGURATION_TABLE_PPCTm, data, SECOND_RANGE_LB_KEY_MINf);
        EGQ_ppct_tbl_data->second_range_lb_key_max = soc_mem_field32_get(unit, EGQ_PER_PORT_CONFIGURATION_TABLE_PPCTm, data, SECOND_RANGE_LB_KEY_MAXf);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Write indirect table ppct_tbl from block EGQ,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

int
  arad_egq_ppct_tbl_set_unsafe(
    SOC_SAND_IN   int                       unit,
    SOC_SAND_IN   uint32                    base_q_pair,
    SOC_SAND_IN   int                       core_id,
    SOC_SAND_IN   ARAD_EGQ_PPCT_TBL_DATA*   EGQ_ppct_tbl_data
  )
{
    uint32 tmp = 0;
    uint32 data[SOC_DPP_DEFS_MAX(EGQ_PPCT_NOF_LONGS)];
    int rv;
 
    SOCDNX_INIT_FUNC_DEFS;

    sal_memset(data, 0, sizeof(uint32) * SOC_DPP_DEFS_MAX(EGQ_PPCT_NOF_LONGS));

    if(SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        soc_mem_field32_set(unit, EGQ_PER_PORT_CONFIGURATION_TABLE_PPCTm, data, BASE_Q_PAIR_NUMf, EGQ_ppct_tbl_data->base_q_pair_num);
    }

    soc_mem_field32_set(unit, EGQ_PER_PORT_CONFIGURATION_TABLE_PPCTm, data, CGM_INTERFACEf, EGQ_ppct_tbl_data->cgm_interface);
    soc_mem_field32_set(unit, EGQ_PER_PORT_CONFIGURATION_TABLE_PPCTm, data, CGM_PORT_PROFILEf, EGQ_ppct_tbl_data->cgm_port_profile);
    soc_mem_field32_set(unit, EGQ_PER_PORT_CONFIGURATION_TABLE_PPCTm, data, CNM_INTRCPT_DROP_ENf, EGQ_ppct_tbl_data->cnm_intrcpt_drop_en);
    soc_mem_field32_set(unit, EGQ_PER_PORT_CONFIGURATION_TABLE_PPCTm, data, CNM_INTRCPT_FC_ENf, EGQ_ppct_tbl_data->cnm_intrcpt_fc_en);
        
    if(SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        SHR_BITCOPY_RANGE(&tmp,0,&(EGQ_ppct_tbl_data->cnm_intrcpt_fc_vec_llfc),0,5);
        SHR_BITCOPY_RANGE(&tmp,5,&(EGQ_ppct_tbl_data->cnm_intrcpt_fc_vec_pfc),0,8);  
    } else {
        SHR_BITCOPY_RANGE(&tmp,0,&(EGQ_ppct_tbl_data->cnm_intrcpt_fc_vec_llfc),0,8);
        SHR_BITCOPY_RANGE(&tmp,8,&(EGQ_ppct_tbl_data->cnm_intrcpt_fc_vec_pfc),0,7);  
    }
    soc_mem_field32_set(unit, EGQ_PER_PORT_CONFIGURATION_TABLE_PPCTm, data, CNM_INTRCPT_FC_VEC_INDEXf, tmp);

    soc_mem_field32_set(unit, EGQ_PER_PORT_CONFIGURATION_TABLE_PPCTm, data, DISABLE_FILTERINGf, EGQ_ppct_tbl_data->disable_filtering);
    soc_mem_field32_set(unit, EGQ_PER_PORT_CONFIGURATION_TABLE_PPCTm, data, COS_MAP_PROFILEf, EGQ_ppct_tbl_data->cos_map_profile);
    soc_mem_field32_set(unit, EGQ_PER_PORT_CONFIGURATION_TABLE_PPCTm, data, ADVA_COUNT_OUT_PORT_FLAGf, EGQ_ppct_tbl_data->ad_count_out_port_flag);
    soc_mem_field32_set(unit, EGQ_PER_PORT_CONFIGURATION_TABLE_PPCTm, data, IS_STACKING_PORTf, EGQ_ppct_tbl_data->is_stacking_port);
    soc_mem_field32_set(unit, EGQ_PER_PORT_CONFIGURATION_TABLE_PPCTm, data, LB_KEY_MINf, EGQ_ppct_tbl_data->lb_key_min);
    soc_mem_field32_set(unit, EGQ_PER_PORT_CONFIGURATION_TABLE_PPCTm, data, LB_KEY_MAXf, EGQ_ppct_tbl_data->lb_key_max);
    soc_mem_field32_set(unit, EGQ_PER_PORT_CONFIGURATION_TABLE_PPCTm, data, PMF_DATAf, EGQ_ppct_tbl_data->pmf_data);
    soc_mem_field32_set(unit, EGQ_PER_PORT_CONFIGURATION_TABLE_PPCTm, data, PEER_TM_DOMAIN_IDf, EGQ_ppct_tbl_data->peer_tm_domain_id);
    soc_mem_field32_set(unit, EGQ_PER_PORT_CONFIGURATION_TABLE_PPCTm, data, PORT_TYPEf, EGQ_ppct_tbl_data->port_type);

    if (SOC_IS_JERICHO(unit)) {
        soc_mem_field32_set(unit, EGQ_PER_PORT_CONFIGURATION_TABLE_PPCTm, data, SECOND_RANGE_LB_KEY_MINf, EGQ_ppct_tbl_data->second_range_lb_key_min);
        soc_mem_field32_set(unit, EGQ_PER_PORT_CONFIGURATION_TABLE_PPCTm, data, SECOND_RANGE_LB_KEY_MAXf, EGQ_ppct_tbl_data->second_range_lb_key_max);
    }

    rv = WRITE_EGQ_PER_PORT_CONFIGURATION_TABLE_PPCTm(unit, EGQ_BLOCK(unit, core_id), base_q_pair, data);
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}


#ifdef BCM_88660_A0

/*
 * Read indirect table egq_per_port_lb_range from block EGQ,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_egq_per_port_lb_range_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32             entry_offset,
    SOC_SAND_OUT  ARAD_PER_PORT_LB_RANGE_TBL_DATA* EGQ_per_port_lb_range_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_EGQ_PER_PORT_LB_RANGE_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(EGQ_per_port_lb_range_tbl_data);

  err = soc_sand_os_memset(
          EGQ_per_port_lb_range_tbl_data,
          0x0,
          sizeof(ARAD_PER_PORT_LB_RANGE_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_mem_read(
          unit,
          EGQ_PER_PORT_LB_RANGEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

  SHR_BITCOPY_RANGE(&(EGQ_per_port_lb_range_tbl_data->lb_key_min), 0, data, 0, 8);
  SHR_BITCOPY_RANGE(&(EGQ_per_port_lb_range_tbl_data->lb_key_max), 0, data, 8, 8);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egq_per_port_lb_range_get_unsafe()",0,0);
}

/*
 * Write indirect table egq_per_port_lb_range from block EGQ,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_egq_per_port_lb_range_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_PER_PORT_LB_RANGE_TBL_DATA* EGQ_per_port_lb_range_tbl_data
  )
{
  uint32
    res, err;
  uint32    
    data[ARAD_EGQ_PER_PORT_LB_RANGE_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(EGQ_per_port_lb_range_tbl_data);

  ARAD_CLEAR(data, uint32, ARAD_EGQ_PER_PORT_LB_RANGE_TBL_ENTRY_SIZE);
  
  SHR_BITCOPY_RANGE(data, 0, &(EGQ_per_port_lb_range_tbl_data->lb_key_min), 0, 8);
  SHR_BITCOPY_RANGE(data, 8, &(EGQ_per_port_lb_range_tbl_data->lb_key_max), 0, 8);          

  err = soc_mem_write(
          unit,
          EGQ_PER_PORT_LB_RANGEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 35, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egq_per_port_lb_range_tbl_set_unsafe()",0,0);
}

#endif /*BCM_88660_A0*/

/*
 * CAL table read and write: 
 *  
 * All scheduler calendars share the same table.
 *  The order is:
 *  MAL0-CALA
 *  MAL0-CALB
 *  MAL1-CALA
 *  MAL1-CALB
 *  ...
 *  CPU-CALA
 *  CPU-CALB
 *  RCY-CALA
 *  RCY-CALB
 */ 

#define ARAD_OFP_RATES_CAL_CHAN_ARB_BASE_SCH_OFFSET          0x0800
#define ARAD_OFP_RATES_CAL_LEN_SCH_OFFSET                    0x1000

int
  arad_sch_cal_tbl_get(
    SOC_SAND_IN   int                    unit,
    SOC_SAND_IN   uint32                 sch_offset,
    SOC_SAND_IN   int                    core,
    SOC_SAND_IN   uint32                 sch_to_get /*A (0) or B (1)*/,
    SOC_SAND_IN   uint32                 slots_count,
    SOC_SAND_OUT  uint32*                slots
  )
{
    uint32
        cal_offset,
        slot,
        entry;

    SOCDNX_INIT_FUNC_DEFS;

    cal_offset = ARAD_OFP_RATES_CAL_CHAN_ARB_BASE_SCH_OFFSET * (sch_offset / 8) + ((sch_offset % 8) * ARAD_OFP_NOF_RATES_CAL_SETS + sch_to_get) * ARAD_OFP_RATES_CAL_LEN_SCH_OFFSET;

    for(slot=0; slot < slots_count; slot++) {
        entry = 0;
        SOCDNX_IF_ERR_EXIT(READ_SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR__CALm(unit, MEM_BLOCK_ALL, cal_offset+slot, &entry));
        soc_mem_field_get(unit, SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR__CALm, &entry, HR_SELf, &(slots[slot]));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
  arad_sch_cal_max_size_get(
    SOC_SAND_IN   int                    unit,
    SOC_SAND_IN   uint32                 sch_offset,
    SOC_SAND_OUT  uint32*                max_cal_size
   )
{
    SOCDNX_INIT_FUNC_DEFS;

    *max_cal_size = ARAD_OFP_RATES_CAL_LEN_SCH_MAX;

    SOCDNX_FUNC_RETURN;
}

int
  arad_sch_cal_tbl_set(
    SOC_SAND_IN   int                    unit,
    SOC_SAND_IN   uint32                 sch_offset,
    SOC_SAND_IN   int                    core,
    SOC_SAND_IN   uint32                 sch_to_set /*A (0) or B (1)*/,
    SOC_SAND_IN   uint32                 slots_count,
                  uint32*                slots
  )
{
    uint32
        cal_offset,
        slot,
        entry;

    SOCDNX_INIT_FUNC_DEFS;

    cal_offset = ARAD_OFP_RATES_CAL_CHAN_ARB_BASE_SCH_OFFSET * (sch_offset / 8) + ((sch_offset % 8) * ARAD_OFP_NOF_RATES_CAL_SETS + sch_to_set) * ARAD_OFP_RATES_CAL_LEN_SCH_OFFSET;

    for (slot=0; slot < slots_count; slot++) {
        entry = 0;
        soc_mem_field_set(unit, SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR__CALm, &entry, HR_SELf, &(slots[slot]));
        SOCDNX_IF_ERR_EXIT(WRITE_SCH_SCHEDULER_CREDIT_GENERATION_CALENDAR__CALm(unit, MEM_BLOCK_ALL, cal_offset+slot, &entry));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Read indirect table drm_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_sch_drm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_SCH_DRM_TBL_DATA* SCH_drm_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_SCH_DRM_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_DRM_TBL_GET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_sand_os_memset(
          SCH_drm_tbl_data,
          0x0,
          sizeof(ARAD_SCH_DRM_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

 
 




  err = soc_mem_read(
          unit,
          SCH_DEVICE_RATE_MEMORY__DRMm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

err=SOC_E_NONE;
    if(SOC_E_NONE == err) {
          SCH_drm_tbl_data->device_rate   = soc_mem_field32_get(
                  unit,
                  SCH_DEVICE_RATE_MEMORY__DRMm,
                  data,
                  DEVICE_RATEf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sch_drm_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table drm_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_sch_drm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_SCH_DRM_TBL_DATA* SCH_drm_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_SCH_DRM_TBL_ENTRY_SIZE];
 
 
 
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_DRM_TBL_SET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

 
 




err=SOC_E_NONE;
if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          SCH_DEVICE_RATE_MEMORY__DRMm,
          data,
          DEVICE_RATEf,
          SCH_drm_tbl_data->device_rate );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_mem_write(
          unit,
          SCH_DEVICE_RATE_MEMORY__DRMm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 31, exit);
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sch_drm_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table dsm_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_sch_dsm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_SCH_DSM_TBL_DATA* SCH_dsm_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_SCH_DSM_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_DSM_TBL_GET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_sand_os_memset(
          SCH_dsm_tbl_data,
          0x0,
          sizeof(ARAD_SCH_DSM_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

 
 




  err = soc_mem_read(
          unit,
          SCH_DUAL_SHAPER_MEMORY__DSMm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

err=SOC_E_NONE;
    if(SOC_E_NONE == err) {
          SCH_dsm_tbl_data->dual_shaper_ena   = soc_mem_field32_get(
                  unit,
                  SCH_DUAL_SHAPER_MEMORY__DSMm,
                  data,
                  DUAL_SHAPER_ENAf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sch_dsm_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table dsm_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_sch_dsm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_SCH_DSM_TBL_DATA* SCH_dsm_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_SCH_DSM_TBL_ENTRY_SIZE];
 
 
 
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_DSM_TBL_SET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

 
 




err=SOC_E_NONE;
if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          SCH_DUAL_SHAPER_MEMORY__DSMm,
          data,
          DUAL_SHAPER_ENAf,
          SCH_dsm_tbl_data->dual_shaper_ena );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_mem_write(
          unit,
          SCH_DUAL_SHAPER_MEMORY__DSMm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 31, exit);
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sch_dsm_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table fdms_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_sch_fdms_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_SCH_FDMS_TBL_DATA* SCH_fdms_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_SCH_FDMS_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_FDMS_TBL_GET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_sand_os_memset(
          SCH_fdms_tbl_data,
          0x0,
          sizeof(ARAD_SCH_FDMS_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

 
 




  err = soc_mem_read(
          unit,
          SCH_FLOW_DESCRIPTOR_MEMORY_STATIC__FDMSm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

err=SOC_E_NONE;
    if(SOC_E_NONE == err) {
          SCH_fdms_tbl_data->sch_number   = soc_mem_field32_get(
                  unit,
                  SCH_FLOW_DESCRIPTOR_MEMORY_STATIC__FDMSm,
                  data,
                  SCH_NUMBERf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 40, exit);

    if(SOC_E_NONE == err) {
          SCH_fdms_tbl_data->cos   = soc_mem_field32_get(
                  unit,
                  SCH_FLOW_DESCRIPTOR_MEMORY_STATIC__FDMSm,
                  data,
                  COSf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 41, exit);

    if(SOC_E_NONE == err) {
          SCH_fdms_tbl_data->hrsel_dual   = soc_mem_field32_get(
                  unit,
                  SCH_FLOW_DESCRIPTOR_MEMORY_STATIC__FDMSm,
                  data,
                  HR_SEL_DUALf            );    
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 42, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sch_fdms_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table fdms_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_sch_fdms_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_SCH_FDMS_TBL_DATA* SCH_fdms_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_SCH_FDMS_TBL_ENTRY_SIZE];
 
 
 
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_FDMS_TBL_SET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

 
 




err=SOC_E_NONE;
if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          SCH_FLOW_DESCRIPTOR_MEMORY_STATIC__FDMSm,
          data,
          SCH_NUMBERf,
          SCH_fdms_tbl_data->sch_number );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          SCH_FLOW_DESCRIPTOR_MEMORY_STATIC__FDMSm,
          data,
          COSf,
          SCH_fdms_tbl_data->cos );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 31, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          SCH_FLOW_DESCRIPTOR_MEMORY_STATIC__FDMSm,
          data,
          HR_SEL_DUALf,
          SCH_fdms_tbl_data->hrsel_dual );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 32, exit);

  err = soc_mem_write(
          unit,
          SCH_FLOW_DESCRIPTOR_MEMORY_STATIC__FDMSm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 33, exit);
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sch_fdms_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table shds_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_sch_shds_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_SCH_SHDS_TBL_DATA* SCH_shds_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_SCH_SHDS_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_SHDS_TBL_GET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_sand_os_memset(
          SCH_shds_tbl_data,
          0x0,
          sizeof(ARAD_SCH_SHDS_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

 
 




  err = soc_mem_read(
          unit,
          SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC__SHDSm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

err=SOC_E_NONE;
    if(SOC_E_NONE == err) {
          SCH_shds_tbl_data->peak_rate_man_even   = soc_mem_field32_get(
                  unit,
                  SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC__SHDSm,
                  data,
                  PEAK_RATE_MAN_EVENf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 40, exit);

    if(SOC_E_NONE == err) {
          SCH_shds_tbl_data->peak_rate_exp_even   = soc_mem_field32_get(
                  unit,
                  SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC__SHDSm,
                  data,
                  PEAK_RATE_EXP_EVENf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 41, exit);

    if(SOC_E_NONE == err) {
          SCH_shds_tbl_data->max_burst_even   = soc_mem_field32_get(
                  unit,
                  SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC__SHDSm,
                  data,
                  MAX_BURST_EVENf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 42, exit);

    if(SOC_E_NONE == err) {
          SCH_shds_tbl_data->slow_rate2_sel_even   = soc_mem_field32_get(
                  unit,
                  SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC__SHDSm,
                  data,
                  SLOW_RATE_2_SEL_EVENf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 43, exit);

    if(SOC_E_NONE == err) {
          SCH_shds_tbl_data->peak_rate_man_odd   = soc_mem_field32_get(
                  unit,
                  SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC__SHDSm,
                  data,
                  PEAK_RATE_MAN_ODDf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 44, exit);

    if(SOC_E_NONE == err) {
          SCH_shds_tbl_data->peak_rate_exp_odd   = soc_mem_field32_get(
                  unit,
                  SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC__SHDSm,
                  data,
                  PEAK_RATE_EXP_ODDf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 45, exit);

    if(SOC_E_NONE == err) {
          SCH_shds_tbl_data->max_burst_odd   = soc_mem_field32_get(
                  unit,
                  SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC__SHDSm,
                  data,
                  MAX_BURST_ODDf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 46, exit);

    if(SOC_E_NONE == err) {
          SCH_shds_tbl_data->slow_rate2_sel_odd   = soc_mem_field32_get(
                  unit,
                  SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC__SHDSm,
                  data,
                  SLOW_RATE_2_SEL_ODDf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 47, exit);

    if(SOC_E_NONE == err) {
          SCH_shds_tbl_data->max_burst_update_even   = soc_mem_field32_get(
                  unit,
                  SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC__SHDSm,
                  data,
                  MAX_BURST_UPDATE_EVENf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 48, exit);

    if(SOC_E_NONE == err) {
          SCH_shds_tbl_data->max_burst_update_odd   = soc_mem_field32_get(
                  unit,
                  SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC__SHDSm,
                  data,
                  MAX_BURST_UPDATE_ODDf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 49, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sch_shds_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table shds_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_sch_shds_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_SCH_SHDS_TBL_DATA* SCH_shds_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_SCH_SHDS_TBL_ENTRY_SIZE];
 
 
 
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_SHDS_TBL_SET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

 
 




err=SOC_E_NONE;
if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC__SHDSm,
          data,
          PEAK_RATE_MAN_EVENf,
          SCH_shds_tbl_data->peak_rate_man_even );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC__SHDSm,
          data,
          PEAK_RATE_EXP_EVENf,
          SCH_shds_tbl_data->peak_rate_exp_even );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 31, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC__SHDSm,
          data,
          MAX_BURST_EVENf,
          SCH_shds_tbl_data->max_burst_even );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 32, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC__SHDSm,
          data,
          SLOW_RATE_2_SEL_EVENf,
          SCH_shds_tbl_data->slow_rate2_sel_even );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 33, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC__SHDSm,
          data,
          PEAK_RATE_MAN_ODDf,
          SCH_shds_tbl_data->peak_rate_man_odd );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 34, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC__SHDSm,
          data,
          PEAK_RATE_EXP_ODDf,
          SCH_shds_tbl_data->peak_rate_exp_odd );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 35, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC__SHDSm,
          data,
          MAX_BURST_ODDf,
          SCH_shds_tbl_data->max_burst_odd );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 36, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC__SHDSm,
          data,
          SLOW_RATE_2_SEL_ODDf,
          SCH_shds_tbl_data->slow_rate2_sel_odd );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 37, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC__SHDSm,
          data,
          MAX_BURST_UPDATE_EVENf,
          SCH_shds_tbl_data->max_burst_update_even );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 38, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC__SHDSm,
          data,
          MAX_BURST_UPDATE_ODDf,
          SCH_shds_tbl_data->max_burst_update_odd );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 39, exit);

  err = soc_mem_write(
          unit,
          SCH_SHAPER_DESCRIPTOR_MEMORY_STATIC__SHDSm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 40, exit);
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sch_shds_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table sem_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  arad_sch_sem_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_SCH_SEM_TBL_DATA* SCH_sem_tbl_data
  )
{
  uint32
    res;
  uint32
    data[ARAD_SCH_SEM_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_SEM_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_SCH_SEM_TBL_ENTRY_SIZE);
  ARAD_CLEAR(SCH_sem_tbl_data, ARAD_SCH_SEM_TBL_DATA, 1);

  res = soc_mem_read(unit, SCH_SCHEDULER_ENABLE_MEMORY__SEMm, MEM_BLOCK_ANY, entry_offset, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  SCH_sem_tbl_data->sch_enable = soc_mem_field32_get(unit, SCH_SCHEDULER_ENABLE_MEMORY__SEMm, data, SCH_ENABLEf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sch_sem_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table sem_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_sch_sem_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_SCH_SEM_TBL_DATA* SCH_sem_tbl_data
  )
{
  uint32
    res;
  uint32
    data[ARAD_SCH_SEM_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_SEM_TBL_SET_UNSAFE);
  ARAD_CLEAR(data, uint32, ARAD_SCH_SEM_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, SCH_SCHEDULER_ENABLE_MEMORY__SEMm, data, SCH_ENABLEf, SCH_sem_tbl_data->sch_enable);

  res = soc_mem_write(
          unit,
          SCH_SCHEDULER_ENABLE_MEMORY__SEMm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 31, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sch_sem_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table fsf_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_sch_fsf_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_SCH_FSF_TBL_DATA* SCH_fsf_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_SCH_FSF_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_FSF_TBL_GET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_sand_os_memset(
          SCH_fsf_tbl_data,
          0x0,
          sizeof(ARAD_SCH_FSF_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

 
 




  err = soc_mem_read(
          unit,
          SCH_FLOW_SUB_FLOW__FSFm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

err=SOC_E_NONE;
    if(SOC_E_NONE == err) {
          SCH_fsf_tbl_data->sfenable   = soc_mem_field32_get(
                  unit,
                  SCH_FLOW_SUB_FLOW__FSFm,
                  data,
                  SF_ENABLEf            
        );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sch_fsf_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table fsf_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_sch_fsf_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_SCH_FSF_TBL_DATA* SCH_fsf_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_SCH_FSF_TBL_ENTRY_SIZE];
 
 
 
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_FSF_TBL_SET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

 
 




err=SOC_E_NONE;
if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          SCH_FLOW_SUB_FLOW__FSFm,
          data,
          SF_ENABLEf,
          SCH_fsf_tbl_data->sfenable
        );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_mem_write(
          unit,
          SCH_FLOW_SUB_FLOW__FSFm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 31, exit);
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sch_fsf_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table fgm_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_sch_fgm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_SCH_FGM_TBL_DATA* SCH_fgm_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_SCH_FGM_TBL_ENTRY_SIZE];
 
 
 
  uint32
    fld_idx = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_FGM_TBL_GET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_sand_os_memset(
          SCH_fgm_tbl_data,
          0x0,
          sizeof(ARAD_SCH_FGM_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

 
 




  err = soc_mem_read(
          unit,
          SCH_FLOW_GROUP_MEMORY__FGMm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

  for (fld_idx = 0; fld_idx < ARAD_TBL_FGM_NOF_GROUPS_ONE_LINE; fld_idx++)
  {
err=SOC_E_NONE;
    if(SOC_E_NONE == err) {
          SCH_fgm_tbl_data->flow_group[fld_idx]     = soc_mem_field32_get(
                  unit,
                  SCH_FLOW_GROUP_MEMORY__FGMm,
                  data,
                  FLOW_GROUP_0f + fld_idx            
        );
        }
    SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 20, exit);
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sch_fgm_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table fgm_tbl from block SCH,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_sch_fgm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_SCH_FGM_TBL_DATA* SCH_fgm_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_SCH_FGM_TBL_ENTRY_SIZE];
  uint32
    fld_idx = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_FGM_TBL_SET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);


  for (fld_idx = 0; fld_idx < ARAD_TBL_FGM_NOF_GROUPS_ONE_LINE; fld_idx++)
  {
err=SOC_E_NONE;
if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          SCH_FLOW_GROUP_MEMORY__FGMm,
            data,
          FLOW_GROUP_0f + fld_idx,
            SCH_fgm_tbl_data->flow_group[fld_idx]
          );
        }
    SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 20, exit);
  }

  err = soc_mem_write(
          unit,
          SCH_FLOW_GROUP_MEMORY__FGMm,
          MEM_BLOCK_ANY,
          entry_offset,
            data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 33, exit);
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sch_fgm_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table shc_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_sch_shc_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_SCH_SHC_TBL_DATA* SCH_shc_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_SCH_SHC_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_SHC_TBL_GET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_sand_os_memset(
          SCH_shc_tbl_data,
          0x0,
          sizeof(ARAD_SCH_SHC_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

 
 




  err = soc_mem_read(
          unit,
          SCH_HR_SCHEDULER_CONFIGURATION__SHCm,
          MEM_BLOCK_ANY,
          entry_offset,
            data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

err=SOC_E_NONE;
    if(SOC_E_NONE == err) {
          SCH_shc_tbl_data->hrmode   = soc_mem_field32_get(
                  unit,
          SCH_HR_SCHEDULER_CONFIGURATION__SHCm,
                  data,
                  HR_MODEf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 40, exit);

  SOC_SAND_CHECK_FUNC_RESULT(err, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sch_shc_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table shc_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_sch_shc_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_SCH_SHC_TBL_DATA* SCH_shc_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_SCH_SHC_TBL_ENTRY_SIZE];
 
 
 
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_SHC_TBL_SET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

 
 




err=SOC_E_NONE;
if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          SCH_HR_SCHEDULER_CONFIGURATION__SHCm,
          data,
          HR_MODEf,
          SCH_shc_tbl_data->hrmode );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_mem_write(
          unit,
          SCH_HR_SCHEDULER_CONFIGURATION__SHCm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 33, exit);
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sch_shc_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table scc_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_sch_scc_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_SCH_SCC_TBL_DATA* SCH_scc_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_SCH_SCC_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_SCC_TBL_GET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_sand_os_memset(
          SCH_scc_tbl_data,
          0x0,
          sizeof(ARAD_SCH_SCC_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

 
 




  err = soc_mem_read(
          unit,
          SCH_CL_SCHEDULERS_CONFIGURATION__SCCm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

err=SOC_E_NONE;
    if(SOC_E_NONE == err) {
          SCH_scc_tbl_data->clsch_type   = soc_mem_field32_get(
                  unit,
                  SCH_CL_SCHEDULERS_CONFIGURATION__SCCm,
                  data,
                  CL_SCH_TYPEf            
        );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sch_scc_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table scc_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_sch_scc_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_SCH_SCC_TBL_DATA* SCH_scc_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_SCH_SCC_TBL_ENTRY_SIZE];
 
 
 
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_SCC_TBL_SET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

 
 




err=SOC_E_NONE;
if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          SCH_CL_SCHEDULERS_CONFIGURATION__SCCm,
          data,
          CL_SCH_TYPEf,
          SCH_scc_tbl_data->clsch_type
        );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_mem_write(
          unit,
          SCH_CL_SCHEDULERS_CONFIGURATION__SCCm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 31, exit);
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sch_scc_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table sct_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_sch_sct_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_SCH_SCT_TBL_DATA* SCH_sct_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_SCH_SCT_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_SCT_TBL_GET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_sand_os_memset(
          SCH_sct_tbl_data,
          0x0,
          sizeof(ARAD_SCH_SCT_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

 
 




  err = soc_mem_read(
          unit,
          SCH_CL_SCHEDULERS_TYPE__SCTm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

err=SOC_E_NONE;
    if(SOC_E_NONE == err) {
          SCH_sct_tbl_data->clconfig   = soc_mem_field32_get(
                  unit,
          SCH_CL_SCHEDULERS_TYPE__SCTm,
                  data,
                  CL_CONFIGf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 40, exit);

err=SOC_E_NONE;
    if(SOC_E_NONE == err) {
          SCH_sct_tbl_data->af0_inv_weight   = soc_mem_field32_get(
                  unit,
                  SCH_CL_SCHEDULERS_TYPE__SCTm,
                  data,
                  AF_0_INV_WEIGHTf            
        );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 41, exit);

    if(SOC_E_NONE == err) {
          SCH_sct_tbl_data->af1_inv_weight   = soc_mem_field32_get(
                  unit,
                  SCH_CL_SCHEDULERS_TYPE__SCTm,
                  data,
                  AF_1_INV_WEIGHTf            
        );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 42, exit);

    if(SOC_E_NONE == err) {
          SCH_sct_tbl_data->af2_inv_weight   = soc_mem_field32_get(
                  unit,
                  SCH_CL_SCHEDULERS_TYPE__SCTm,
                  data,
                  AF_2_INV_WEIGHTf            
        );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 43, exit);

    if(SOC_E_NONE == err) {
          SCH_sct_tbl_data->af3_inv_weight   = soc_mem_field32_get(
                  unit,
                  SCH_CL_SCHEDULERS_TYPE__SCTm,
                  data,
                  AF_3_INV_WEIGHTf            
        );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 44, exit);

    if(SOC_E_NONE == err) {
          SCH_sct_tbl_data->wfqmode   = soc_mem_field32_get(
                  unit,
                  SCH_CL_SCHEDULERS_TYPE__SCTm,
                  data,
                  WFQ_MODEf            
        );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 45, exit);

    if(SOC_E_NONE == err) {
          SCH_sct_tbl_data->enh_clen   = soc_mem_field32_get(
                  unit,
                  SCH_CL_SCHEDULERS_TYPE__SCTm,
                  data,
                  ENH_CL_ENf            
        );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 46, exit);

    if(SOC_E_NONE == err) {
          SCH_sct_tbl_data->enh_clsphigh   = soc_mem_field32_get(
                  unit,
                  SCH_CL_SCHEDULERS_TYPE__SCTm,
                  data,
                  ENH_CLSP_HIGHf            
        );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 47, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sch_sct_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table sct_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_sch_sct_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_SCH_SCT_TBL_DATA* SCH_sct_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_SCH_SCT_TBL_ENTRY_SIZE];
 
 
 
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_SCT_TBL_SET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

 
 




err=SOC_E_NONE;
if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          SCH_CL_SCHEDULERS_TYPE__SCTm,
          data,
          CL_CONFIGf,
          SCH_sct_tbl_data->clconfig );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          SCH_CL_SCHEDULERS_TYPE__SCTm,
          data,
          AF_0_INV_WEIGHTf,
          SCH_sct_tbl_data->af0_inv_weight
        );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 31, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          SCH_CL_SCHEDULERS_TYPE__SCTm,
          data,
          AF_1_INV_WEIGHTf,
          SCH_sct_tbl_data->af1_inv_weight
        );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 32, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          SCH_CL_SCHEDULERS_TYPE__SCTm,
          data,
          AF_2_INV_WEIGHTf,
          SCH_sct_tbl_data->af2_inv_weight
        );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 33, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          SCH_CL_SCHEDULERS_TYPE__SCTm,
          data,
          AF_3_INV_WEIGHTf,
          SCH_sct_tbl_data->af3_inv_weight
        );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 34, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          SCH_CL_SCHEDULERS_TYPE__SCTm,
          data,
          WFQ_MODEf,
          SCH_sct_tbl_data->wfqmode
        );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 35, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          SCH_CL_SCHEDULERS_TYPE__SCTm,
          data,
          ENH_CL_ENf,
          SCH_sct_tbl_data->enh_clen
        );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 36, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          SCH_CL_SCHEDULERS_TYPE__SCTm,
          data,
          ENH_CLSP_HIGHf,
          SCH_sct_tbl_data->enh_clsphigh
        );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 37, exit);

  err = soc_mem_write(
          unit,
          SCH_CL_SCHEDULERS_TYPE__SCTm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 38, exit);
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sch_sct_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table fqm_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_sch_fqm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_SCH_FQM_TBL_DATA* SCH_fqm_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_SCH_FQM_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_FQM_TBL_GET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_sand_os_memset(
          SCH_fqm_tbl_data,
          0x0,
          sizeof(ARAD_SCH_FQM_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

 
 




  err = soc_mem_read(
          unit,
          SCH_FLOW_TO_QUEUE_MAPPING__FQMm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

err=SOC_E_NONE;
    if(SOC_E_NONE == err) {
          SCH_fqm_tbl_data->base_queue_num   = soc_mem_field32_get(
                  unit,
                  SCH_FLOW_TO_QUEUE_MAPPING__FQMm,
                  data,
                  BASE_QUEUE_NUMf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 40, exit);

    if(SOC_E_NONE == err) {
          SCH_fqm_tbl_data->sub_flow_mode   = soc_mem_field32_get(
                  unit,
                  SCH_FLOW_TO_QUEUE_MAPPING__FQMm,
                  data,
                  SUB_FLOW_MODEf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 41, exit);

    if(SOC_E_NONE == err) {
          SCH_fqm_tbl_data->flow_slow_enable   = soc_mem_field32_get(
                  unit,
                  SCH_FLOW_TO_QUEUE_MAPPING__FQMm,
                  data,
                  FLOW_SLOW_ENABLEf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 42, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sch_fqm_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table fqm_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_sch_fqm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_SCH_FQM_TBL_DATA* SCH_fqm_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_SCH_FQM_TBL_ENTRY_SIZE];
 
 
 
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_FQM_TBL_SET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

 
 



err=SOC_E_NONE;
if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          SCH_FLOW_TO_QUEUE_MAPPING__FQMm,
          data,
          BASE_QUEUE_NUMf,
          SCH_fqm_tbl_data->base_queue_num );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          SCH_FLOW_TO_QUEUE_MAPPING__FQMm,
          data,
          SUB_FLOW_MODEf,
          SCH_fqm_tbl_data->sub_flow_mode );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 31, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          SCH_FLOW_TO_QUEUE_MAPPING__FQMm,
          data,
          FLOW_SLOW_ENABLEf,
          SCH_fqm_tbl_data->flow_slow_enable );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 32, exit);

  err = soc_mem_write(
          unit,
          SCH_FLOW_TO_QUEUE_MAPPING__FQMm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 33, exit);
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sch_fqm_tbl_set_unsafe()",0,0);
}

/*
 * Read indirect table ffm_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_sch_ffm_tbl_get_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_OUT  ARAD_SCH_FFM_TBL_DATA* SCH_ffm_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_SCH_FFM_TBL_ENTRY_SIZE];
 
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_FFM_TBL_GET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

  err = soc_sand_os_memset(
          SCH_ffm_tbl_data,
          0x0,
          sizeof(ARAD_SCH_FFM_TBL_DATA)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 15, exit);

 
 




  err = soc_mem_read(
          unit,
          SCH_FLOW_TO_FIP_MAPPING__FFMm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

err=SOC_E_NONE;
    if(SOC_E_NONE == err) {
          SCH_ffm_tbl_data->device_number   = soc_mem_field32_get(
                  unit,
                  SCH_FLOW_TO_FIP_MAPPING__FFMm,
                  data,
                  DEVICE_NUMBERf            );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sch_ffm_tbl_get_unsafe()",0,0);
}

/*
 * Write indirect table ffm_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_sch_ffm_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_SCH_FFM_TBL_DATA* SCH_ffm_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_SCH_FFM_TBL_ENTRY_SIZE];
 
 
 
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_FFM_TBL_SET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

 
 




err=SOC_E_NONE;
if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          SCH_FLOW_TO_FIP_MAPPING__FFMm,
          data,
          DEVICE_NUMBERf,
          SCH_ffm_tbl_data->device_number );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

  err = soc_mem_write(
          unit,
          SCH_FLOW_TO_FIP_MAPPING__FFMm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 31, exit);
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sch_ffm_tbl_set_unsafe()",0,0);
}
/*
 * Write indirect table scheduler_init_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */


uint32
  arad_sch_scheduler_init_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_SCH_SCHEDULER_INIT_TBL_DATA* SCH_scheduler_init_tbl_data
  )
{
  uint32 err, res, reg_val,i;
  uint32 data[ARAD_SCH_SCHEDULER_INIT_TBL_ENTRY_SIZE] = {0};
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_SCHEDULER_INIT_TBL_SET_UNSAFE);

  /* clear on set to all the sch memory error interrupts */  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,WRITE_SCH_PAR_ERR_INTERRUPT_REGISTERr(unit,  0xffffffff));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  31,  exit, ARAD_REG_ACCESS_ERR,WRITE_SCH_ECC_1B_ERR_INTERRUPT_REGISTERr(unit,  0xffffffff));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  32,  exit, ARAD_REG_ACCESS_ERR,WRITE_SCH_ECC_2B_ERR_INTERRUPT_REGISTERr(unit,  0xffffffff));

  soc_mem_field32_set(unit, SCH_SCHEDULER_INITm, data, SCH_INITf, SCH_scheduler_init_tbl_data->schinit);

  /* keep current timeout and set a new timeout needed for writing this special table */
  SOC_SAND_CHECK_FUNC_RESULT(READ_CMIC_SBUS_TIMEOUTr(unit, &reg_val), 10, exit);
  SOC_SAND_CHECK_FUNC_RESULT(WRITE_CMIC_SBUS_TIMEOUTr(unit, 0xffffffff), 20, exit);

   if (!SOC_IS_ARADPLUS(unit)) {
      /* write the table entry */
      err = WRITE_SCH_SCHEDULER_INITm(unit, MEM_BLOCK_ANY, entry_offset, data);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 40, exit);
   } else {
       
        for (i=0; i<5; i++) {
            err = WRITE_SCH_SCHEDULER_INITm(unit, MEM_BLOCK_ANY, entry_offset, data);
            if (SOC_E_NONE == err) {
                break;
            } else {
                sal_sleep(1);
            }
        }
        SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 41, exit);
   }
  
  /* restore original timeout */
  SOC_SAND_CHECK_FUNC_RESULT(WRITE_CMIC_SBUS_TIMEOUTr(unit, reg_val), 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sch_scheduler_init_tbl_set_unsafe()",0,0);
}

/*
 * Write indirect table force_status_message_tbl from block SCH,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_sch_force_status_message_tbl_set_unsafe(
    SOC_SAND_IN   int             unit,
    SOC_SAND_IN   uint32              entry_offset,
    SOC_SAND_IN   ARAD_SCH_FORCE_STATUS_MESSAGE_TBL_DATA* SCH_force_status_message_tbl_data
  )
{
  uint32
    err;
  uint32
    data[ARAD_SCH_FORCE_STATUS_MESSAGE_TBL_ENTRY_SIZE];
 
 
 
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_SCH_FORCE_STATUS_MESSAGE_TBL_SET_UNSAFE);

  err = soc_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(err, 10, exit);

 
 




err=SOC_E_NONE;
if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          SCH_FORCE_STATUS_MESSAGEm,
          data,
          MESSAGE_FLOW_IDf,
          SCH_force_status_message_tbl_data->message_flow_id );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

if(SOC_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          SCH_FORCE_STATUS_MESSAGEm,
          data,
          MESSAGE_TYPEf,
          SCH_force_status_message_tbl_data->message_type );
        }
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 31, exit);

  err = soc_mem_write(
          unit,
          SCH_FORCE_STATUS_MESSAGEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 32, exit);
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_sch_force_status_message_tbl_set_unsafe()",0,0);
}


/*
 * Get the number of banks and number of Drams
 */
uint32
  arad_mmu_dram_address_space_info_get_unsafe(
    SOC_SAND_IN    int                               unit,
    SOC_SAND_OUT   uint32                                *nof_drams,
    SOC_SAND_OUT   uint32                                *nof_banks,
    SOC_SAND_OUT   uint32                                *nof_cols
  )
{
  uint32
    res;
  uint32
    reg_val,
    tmp_val;
   
    

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_MMU_DRAM_ADDRESS_SPACE_INFO_GET_UNSAFE);


  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,READ_MMU_GENERAL_CONFIGURATION_REGISTER_2r(unit, &reg_val ));

  ARAD_FLD_FROM_REG(MMU_GENERAL_CONFIGURATION_REGISTER_2r, DRAM_NUMf, tmp_val, reg_val, 40, exit);

 /*
  * check IF DRAM ID in range.
  */
  switch(tmp_val) {
  case 0x3:
    *nof_drams = 2;
    break;
  case 0x2:
    *nof_drams = 3;
    break;
  case 0x1:
    *nof_drams = 4;
    break;
  case 0x0:
      *nof_drams = 6;
    break;
  case 0x4:
  default:
    *nof_drams = 8;
    break;
  }
 /*
  * check If BANK ID in range.
  */
  ARAD_FLD_FROM_REG(MMU_GENERAL_CONFIGURATION_REGISTERr, DRAM_BANK_NUMf, tmp_val, reg_val, 60, exit);

  switch(tmp_val)
  {
    case 0x0:
      *nof_banks = ARAD_DRAM_NUM_BANKS_4;
      break;
    case 0x1:
    default:
      *nof_banks = ARAD_DRAM_NUM_BANKS_8;
      break;
  }

  ARAD_FLD_FROM_REG(MMU_GENERAL_CONFIGURATION_REGISTERr, DRAM_COL_NUMf, tmp_val, reg_val, 80, exit);

  switch(tmp_val) {
  case 0x0:
    *nof_cols = 256;
    break;
  case 0x1:
    *nof_cols = 512;
    break;
  case 0x2:
    *nof_cols = 1024;
    break;
  case 0x3:
    *nof_cols = 2048;
    break;
  case 0x4:
    *nof_cols = 4096;
    break;
  default:
    *nof_cols = 8192;
    break;
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_mmu_dram_address_space_info_get()",0,0);
}

/* 
 *  TM-Tables - B
 */ 

/*
 * Read indirect table nif_port_to_ctxt_bit_map_tbl from block IRE,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  arad_ire_nif_port_to_ctxt_bit_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IRE_NIF_PORT_TO_CTXT_BIT_MAP_TBL_DATA  *IRE_nif_port_to_ctxt_bit_map_tbl_data
  )
{
  uint32
    res, err = SOC_SAND_OK;
  uint32
    data[ARAD_IRE_NIF_PORT_TO_CTXT_BIT_MAP_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IRE_NIF_PORT_TO_CTXT_BIT_MAP_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_IRE_NIF_PORT_TO_CTXT_BIT_MAP_TBL_ENTRY_SIZE);
  ARAD_CLEAR(IRE_nif_port_to_ctxt_bit_map_tbl_data, ARAD_IRE_NIF_PORT_TO_CTXT_BIT_MAP_TBL_DATA, 1);

  err = soc_mem_read(
          unit,
          IRE_NIF_PORT_TO_CTXT_BIT_MAPm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(err, 30, exit);

  soc_mem_field_get(
            unit,
            IRE_NIF_PORT_TO_CTXT_BIT_MAPm,
            data,
            CONTEXTS_BIT_MAPPINGf,
            (uint32*)&(IRE_nif_port_to_ctxt_bit_map_tbl_data->contexts_bit_mapping[0]) );  
  
  IRE_nif_port_to_ctxt_bit_map_tbl_data->parity   = soc_mem_field32_get(
                  unit,
                  IRE_NIF_PORT_TO_CTXT_BIT_MAPm,
                  data,
                  PARITYf            );
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ire_nif_port_to_ctxt_bit_map_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table nif_port_to_ctxt_bit_map_tbl from block IRE,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  arad_ire_nif_port_to_ctxt_bit_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IRE_NIF_PORT_TO_CTXT_BIT_MAP_TBL_DATA  *IRE_nif_port_to_ctxt_bit_map_tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_IRE_NIF_PORT_TO_CTXT_BIT_MAP_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IRE_NIF_PORT_TO_CTXT_BIT_MAP_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_IRE_NIF_PORT_TO_CTXT_BIT_MAP_TBL_ENTRY_SIZE);

 
            soc_mem_field_set(
          unit,
          IRE_NIF_PORT_TO_CTXT_BIT_MAPm,
          data,
          CONTEXTS_BIT_MAPPINGf,
          (uint32*)&(IRE_nif_port_to_ctxt_bit_map_tbl_data->contexts_bit_mapping[0]));
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_mem_write(
          unit,
          IRE_NIF_PORT_TO_CTXT_BIT_MAPm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ire_nif_port_to_ctxt_bit_map_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table tdm_config_tbl from block IRE,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  arad_ire_tdm_config_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IRE_TDM_CONFIG_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_IRE_TDM_CONFIG_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IRE_TDM_CONFIG_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_IRE_TDM_CONFIG_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_IRE_TDM_CONFIG_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IRE_TDM_CONFIGm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

          tbl_data->mode = soc_mem_field32_get(
                  unit,
                  IRE_TDM_CONFIGm,
                  data,
                  MODEf            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

          tbl_data->cpu   = soc_mem_field32_get(
                  unit,
                  IRE_TDM_CONFIGm,
                  data,
                  CPUf            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  soc_mem_field_get(
                  unit,
                  IRE_TDM_CONFIGm,
                  data,
                  HEADERf,
              tbl_data->header);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  tbl_data->add_packet_crc  = soc_mem_field32_get(
                  unit,
                  IRE_TDM_CONFIGm,
                  data,
                  ADD_PACKET_CRCf            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  tbl_data->link_mask_ptr = soc_mem_field32_get(
                  unit,
                  IRE_TDM_CONFIGm,
                  data,
                  LINK_MASK_PTRf            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);
          tbl_data->parity   = soc_mem_field32_get(
                  unit,
                  IRE_TDM_CONFIGm,
                  data,
                  PARITYf            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ire_tdm_config_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table tdm_config_tbl from block IRE,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  arad_ire_tdm_config_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IRE_TDM_CONFIG_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_IRE_TDM_CONFIG_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IRE_TDM_CONFIG_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_IRE_TDM_CONFIG_TBL_ENTRY_SIZE);

      soc_mem_field32_set(
          unit,
          IRE_TDM_CONFIGm,
          data,
          MODEf,
          tbl_data->mode );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

            soc_mem_field32_set(
          unit,
          IRE_TDM_CONFIGm,
          data,
          CPUf,
          tbl_data->cpu );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

            soc_mem_field_set(
          unit,
          IRE_TDM_CONFIGm,
          data,
          HEADERf,
          (uint32*)&(tbl_data->header[0]));
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  soc_mem_field32_set(
          unit,
          IRE_TDM_CONFIGm,
          data,
          LINK_MASK_PTRf,
          tbl_data->link_mask_ptr );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

            soc_mem_field32_set(
          unit,
          IRE_TDM_CONFIGm,
          data,
          ADD_PACKET_CRCf,
          tbl_data->add_packet_crc );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

  res = soc_mem_write(
          unit,
          IRE_TDM_CONFIGm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ire_tdm_config_tbl_set_unsafe()", entry_offset, 0);
}
/*
 * Read indirect table context_mru_tbl from block IDR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  arad_idr_context_mru_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IDR_CONTEXT_MRU_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_IDR_CONTEXT_MRU_TBL_ENTRY_SIZE];
 
 
 

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IDR_CONTEXT_MRU_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_IDR_CONTEXT_MRU_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_IDR_CONTEXT_MRU_TBL_DATA, 1);

 
 



  

  res = soc_mem_read(
          unit,
          IDR_CONTEXT_MRUm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

          tbl_data->org_size   = soc_mem_field32_get(
                  unit,
                  IDR_CONTEXT_MRUm ,
                  data,
                  ORG_SIZEf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

          tbl_data->size   = soc_mem_field32_get(
                  unit,
                  IDR_CONTEXT_MRUm,
                  data,
                  SIZEf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

          tbl_data->parity   = soc_mem_field32_get(
                  unit,
                  IDR_CONTEXT_MRUm,
                  data,
                  PARITYf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_idr_context_mru_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table context_mru_tbl from block IDR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  arad_idr_context_mru_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IDR_CONTEXT_MRU_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_IDR_CONTEXT_MRU_TBL_ENTRY_SIZE];
 
 
 

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IDR_CONTEXT_MRU_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_IDR_CONTEXT_MRU_TBL_ENTRY_SIZE);

 
 



  

            soc_mem_field32_set(
          unit,
          IDR_CONTEXT_MRUm,
          data,
          ORG_SIZEf,
          tbl_data->org_size );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

            soc_mem_field32_set(
          unit,
          IDR_CONTEXT_MRUm,
          data,
          SIZEf,
          tbl_data->size );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);


  res = soc_mem_write(
          unit,
          IDR_CONTEXT_MRUm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 33, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_idr_context_mru_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table snoop_mirror_table0_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  arad_irr_snoop_mirror_table0_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IRR_SNOOP_MIRROR_TABLE0_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_IRR_SNOOP_MIRROR_TABLE0_TBL_ENTRY_SIZE];
 
 
  soc_mem_t mem=IRR_SNOOP_MIRROR_DEST_TABLEm ;

 

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IRR_SNOOP_MIRROR_TABLE0_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_IRR_SNOOP_MIRROR_TABLE0_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_IRR_SNOOP_MIRROR_TABLE0_TBL_DATA, 1);

 
 



  

  res = soc_mem_read(
          unit,
          mem,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

          tbl_data->destination   = soc_mem_field32_get(
                  unit,
                  mem,
                  data,
                  DESTINATIONf            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

          tbl_data->tc   = soc_mem_field32_get(
                  unit,
                  mem,
                  data,
                  TCf            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

          tbl_data->tc_ow   = soc_mem_field32_get(
                  unit,
                  mem,
                  data,
                  TC_OWf            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

          tbl_data->dp   = soc_mem_field32_get(
                  unit,
                  mem,
                  data,
                  DPf            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

          tbl_data->dp_ow   = soc_mem_field32_get(
                  unit,
                  mem,
                  data,
                  DP_OWf            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);
  if ((!SOC_IS_JERICHO(unit))) {

      tbl_data->parity   = soc_mem_field32_get(
                  unit,
                  mem,
                  data,
                  PARITYf            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_irr_snoop_mirror_table0_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table snoop_mirror_table0_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  arad_irr_snoop_mirror_table0_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IRR_SNOOP_MIRROR_TABLE0_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_IRR_SNOOP_MIRROR_TABLE0_TBL_ENTRY_SIZE];
  soc_mem_t mem=  IRR_SNOOP_MIRROR_DEST_TABLEm;

 
 

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IRR_SNOOP_MIRROR_TABLE0_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_IRR_SNOOP_MIRROR_TABLE0_TBL_ENTRY_SIZE);

 
 



  

            soc_mem_field32_set(
          unit,
          mem,
          data,
          DESTINATIONf,
          tbl_data->destination );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

            soc_mem_field32_set(
          unit,
          mem,
          data,
          TCf,
          tbl_data->tc );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

            soc_mem_field32_set(
          unit,
          mem,
          data,
          TC_OWf,
          tbl_data->tc_ow );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

            soc_mem_field32_set(
          unit,
          mem,
          data,
          DPf,
          tbl_data->dp );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

            soc_mem_field32_set(
          unit,
          mem,
          data,
          DP_OWf,
          tbl_data->dp_ow );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_mem_write(
          unit,
          mem,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 36, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_irr_snoop_mirror_table0_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table snoop_mirror_table1_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  arad_irr_snoop_mirror_table1_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IRR_SNOOP_MIRROR_TABLE1_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_IRR_SNOOP_MIRROR_TABLE1_TBL_ENTRY_SIZE];
  soc_mem_t mem=  IRR_SNOOP_MIRROR_TM_TABLEm;
 
 
 

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IRR_SNOOP_MIRROR_TABLE1_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_IRR_SNOOP_MIRROR_TABLE1_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_IRR_SNOOP_MIRROR_TABLE1_TBL_DATA, 1);

 
 



  

  res = soc_mem_read(
          unit,
          mem,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
/*
          tbl_data->meter_ptr0   = soc_mem_field32_get(
                  unit,
                  IRR_SNOOP_MIRROR_TABLE_1m,
                  data,
                  METER_PTR_0f            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

          tbl_data->meter_ptr0_ow   = soc_mem_field32_get(
                  unit,
                  IRR_SNOOP_MIRROR_TABLE_1m,
                  data,
                  METER_PTR_0_OWf            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

          tbl_data->meter_ptr1   = soc_mem_field32_get(
                  unit,
                  IRR_SNOOP_MIRROR_TABLE_1m,
                  data,
                  METER_PTR_1f            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

          tbl_data->meter_ptr1_ow   = soc_mem_field32_get(
                  unit,
                  IRR_SNOOP_MIRROR_TABLE_1m,
                  data,
                  METER_PTR_1_OWf            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);
*/
          tbl_data->counter_ptr0   = soc_mem_field32_get(
                  unit,
                  mem,
                  data,
                  COUNTER_PTR_0f            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

          tbl_data->counter_ptr0_ow   = soc_mem_field32_get(
                  unit,
                  mem,
                  data,
                  COUNTER_PTR_0_OWf            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

          tbl_data->counter_ptr1   = soc_mem_field32_get(
                  unit,
                  mem,
                  data,
                  COUNTER_PTR_1f            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

          tbl_data->counter_ptr1_ow   = soc_mem_field32_get(
                  unit,
                  mem,
                  data,
                  COUNTER_PTR_1_OWf            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);
/*
          tbl_data->dp_cmd   = soc_mem_field32_get(
                  unit,
                  IRR_SNOOP_MIRROR_TABLE_1m,
                  data,
                  DP_CMDf            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

          tbl_data->dp_cmd_ow   = soc_mem_field32_get(
                  unit,
                  IRR_SNOOP_MIRROR_TABLE_1m,
                  data,
                  DP_CMD_OWf            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);
*/

  if (!SOC_IS_JERICHO(unit)) {
      tbl_data->parity   = soc_mem_field32_get(
                  unit,
                  mem,
                  data,
                  PARITYf            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_irr_snoop_mirror_table1_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table snoop_mirror_table1_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  arad_irr_snoop_mirror_table1_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IRR_SNOOP_MIRROR_TABLE1_TBL_DATA  *tbl_data
  )
{
    uint32 zero=0;
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_IRR_SNOOP_MIRROR_TABLE1_TBL_ENTRY_SIZE];
  soc_mem_t mem=IRR_SNOOP_MIRROR_TM_TABLEm;
  soc_field_t field;
 
 

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IRR_SNOOP_MIRROR_TABLE1_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_IRR_SNOOP_MIRROR_TABLE1_TBL_ENTRY_SIZE);

 
 



  
/*
            soc_mem_field32_set(
          unit,
          IRR_SNOOP_MIRROR_TABLE_1m,
          data,
          METER_PTR_0f,
          tbl_data->meter_ptr0 );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

            soc_mem_field32_set(
          unit,
          IRR_SNOOP_MIRROR_TABLE_1m,
          data,
          METER_PTR_0_OWf,
          tbl_data->meter_ptr0_ow );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

            soc_mem_field32_set(
          unit,
          IRR_SNOOP_MIRROR_TABLE_1m,
          data,
          METER_PTR_1f,
          tbl_data->meter_ptr1 );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

            soc_mem_field32_set(
          unit,
          IRR_SNOOP_MIRROR_TABLE_1m,
          data,
          METER_PTR_1_OWf,
          tbl_data->meter_ptr1_ow );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);
*/
            soc_mem_field32_set(
          unit,
          mem,
          data,
          COUNTER_PTR_0f,
          tbl_data->counter_ptr0 );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  soc_mem_field32_set(
          unit,
          mem,
          data,
          COUNTER_PTR_0_OWf,
          tbl_data->counter_ptr0_ow );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  /* Perform the same between OW and update */
  soc_mem_field32_set(
          unit,
          mem,
          data,
          COUNTER_PTR_0_UPDATEf,
          tbl_data->counter_ptr0_ow );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

            soc_mem_field32_set(
          unit,
          mem,
          data,
          COUNTER_PTR_1f,
          tbl_data->counter_ptr1 );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

            soc_mem_field32_set(
          unit,
          mem,
          data,
          COUNTER_PTR_1_OWf,
          tbl_data->counter_ptr1_ow );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  /* Perform the same between OW and update */
  soc_mem_field32_set(
          unit,
          mem,
          data,
          COUNTER_PTR_1_UPDATEf,
          tbl_data->counter_ptr1_ow );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);


/*
            soc_mem_field32_set(
          unit,
          IRR_SNOOP_MIRROR_TABLE_1m,
          data,
          DP_CMDf,
          tbl_data->dp_cmd );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

            soc_mem_field32_set(
          unit,
          IRR_SNOOP_MIRROR_TABLE_1m,
          data,
          DP_CMD_OWf,
          tbl_data->dp_cmd_ow );
  SOC_SAND_CHECK_FUNC_RESULT(res, 39, exit);
*/

  if ((SOC_IS_JERICHO(unit))) {

    /* Header delta size. */
        field = HEADER_DELTAf ;/*field size in bits (8) */
        soc_mem_field32_set(unit, mem, data, field, zero);

    /* In PP port. */
        field = IN_PP_PORTf ;/*field size in bits (8) */
        soc_mem_field32_set(unit, mem, data, field, zero);


    /* Statistics Vsq pointer. */
        field = ST_VSQ_PTRf ;/*field size in bits (8) */
        soc_mem_field32_set(unit, mem, data, field, zero);

    /* If set then Header delta size is overwritten. */
        field = HEADER_DELTA_OWf ;/*field size in bits (1) */
        soc_mem_field32_set(unit, mem, data, field, zero);

    /* If set then In PP port is overwritten. */
        field = IN_PP_PORT_OWf ;/*field size in bits (1) */
        soc_mem_field32_set(unit, mem, data, field, zero);

    /* Ignore CP. */
        field = IGNORE_CPf ;/*field size in bits (1) */
        soc_mem_field32_set(unit, mem, data, field, zero);

    /* If set then Ethernet encapsulation is overwritten. */
        field = ETH_ENCAPSULATION_OWf ;/*field size in bits (1) */
        soc_mem_field32_set(unit, mem, data, field, zero);

    /* LAG load-balancing key. */
        field = LAG_LB_KEYf ;/*field size in bits (16) */
        soc_mem_field32_set(unit, mem, data, field, zero);

    /* Ethernet encapsulation. */
        field = ETH_ENCAPSULATIONf ;/*field size in bits (2) */
        soc_mem_field32_set(unit, mem, data, field, zero);

    /* If set then Header truncate size is overwritten. */
        field = HEADER_TRUNCATE_OWf ;/*field size in bits (1) */
        soc_mem_field32_set(unit, mem, data, field, zero);

    /* If set then Statistics Vsq pointer is overwritten. */
        field = ST_VSQ_PTR_OWf ;/*field size in bits (1) */
        soc_mem_field32_set(unit, mem, data, field, zero);

    /* If set then Header append size is overwritten. */
        field = HEADER_APPEND_OWf ;/*field size in bits (1) */
        soc_mem_field32_set(unit, mem, data, field, zero);

    /* Header truncate size. */
        field = HEADER_TRUNCATEf ;/*field size in bits (8) */
        soc_mem_field32_set(unit, mem, data, field, zero);

    /* Header append size. */
        field = HEADER_APPENDf ;/*field size in bits (8) */
        soc_mem_field32_set(unit, mem, data, field, zero);

    /* If set then Ignore CP is overwritten. */
        field = IGNORE_CP_OWf ;/*field size in bits (1) */
        soc_mem_field32_set(unit, mem, data, field, zero);

    /* If set then LAG load-balancing key is overwritten. */
        field = LAG_LB_KEY_OWf ;/*field size in bits (1) */
        soc_mem_field32_set(unit, mem, data, field, zero);



  }
  res = soc_mem_write(
          unit,
          mem,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_irr_snoop_mirror_table1_tbl_set_unsafe()", entry_offset, 0);
}
/*
 * Read indirect table destination_table_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  arad_irr_destination_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IRR_DESTINATION_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32 res = SOC_SAND_OK;
  uint32 data[ARAD_IRR_DESTINATION_TABLE_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IRR_DESTINATION_TABLE_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_IRR_DESTINATION_TABLE_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_IRR_DESTINATION_TABLE_TBL_DATA, 1);

  res = soc_mem_read(unit, IRR_DESTINATION_TABLEm, MEM_BLOCK_ANY, entry_offset, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->queue_number = soc_mem_field32_get(unit, IRR_DESTINATION_TABLEm, data, QUEUE_NUMBERf);
  tbl_data->tc_profile   = soc_mem_field32_get(unit, IRR_DESTINATION_TABLEm, data, TC_PROFILEf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_irr_destination_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table destination_table_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  arad_irr_destination_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IRR_DESTINATION_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32 res = SOC_SAND_OK;
  uint32 data[ARAD_IRR_DESTINATION_TABLE_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IRR_DESTINATION_TABLE_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_IRR_DESTINATION_TABLE_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IRR_DESTINATION_TABLEm, data, QUEUE_NUMBERf, tbl_data->queue_number);
  soc_mem_field32_set(unit, IRR_DESTINATION_TABLEm, data, TC_PROFILEf, tbl_data->tc_profile);

  if ((SOC_IS_JERICHO(unit))) {
      soc_mem_field32_set(unit, IRR_DESTINATION_TABLEm, data, VALID_0f, (uint32)1);
      soc_mem_field32_set(unit, IRR_DESTINATION_TABLEm, data, VALID_1f, (uint32)1);
  }

  res = soc_mem_write(unit, IRR_DESTINATION_TABLEm, MEM_BLOCK_ALL, entry_offset, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 34, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_irr_destination_table_tbl_set_unsafe()", entry_offset, 0);
}
/*
 * Read indirect table lag_to_lag_range_tbl from block IRR,
 * doesn't take semaphore. 
 *  
 * Arad+: 
 *   Also read the (IHB)IPP_LAG_TO_LAG_RANGE table which is
 *   duplicated from the TM, with the Mode field changed to
 *   IsStateful (if 1 then the LAG is stateful).
 *   For get, we only use the IsStateful bit.
 *   Specifically, the Range and Parity fields are ignored.
 *  
 * Must only be called from a function taking the device semaphore
 */
uint32
  arad_irr_lag_to_lag_range_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IRR_LAG_TO_LAG_RANGE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_IRR_LAG_TO_LAG_RANGE_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IRR_LAG_TO_LAG_RANGE_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_IRR_LAG_TO_LAG_RANGE_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_IRR_LAG_TO_LAG_RANGE_TBL_DATA, 1);

  res = soc_mem_read(unit, IRR_LAG_TO_LAG_RANGEm, MEM_BLOCK_ANY, entry_offset, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  tbl_data->range   = soc_mem_field32_get(unit, IRR_LAG_TO_LAG_RANGEm, data, RANGEf);
  tbl_data->mode    = soc_mem_field32_get(unit, IRR_LAG_TO_LAG_RANGEm, data, MODEf);

#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit)) {
    ARAD_CLEAR(data, uint32, ARAD_IRR_LAG_TO_LAG_RANGE_TBL_ENTRY_SIZE);

    res = READ_IHB_IPP_LAG_TO_LAG_RANGEm(unit, MEM_BLOCK_ANY, entry_offset, data);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    tbl_data->is_stateful = soc_mem_field32_get(unit, IHB_IPP_LAG_TO_LAG_RANGEm, data, IS_STATEFULf);
  }  
#endif /* BCM_88660_A0 */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_irr_lag_to_lag_range_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table lag_to_lag_range_tbl from block IRR,
 * doesn't take semaphore. 
 * 
 * Arad+: 
 *   Also update the (IHB)IPP_LAG_TO_LAG_RANGE table which is
 *   duplicated from the TM, with the Mode field changed to
 *   IsStateful (if 1 then the LAG is stateful).
 *  
 * Must only be called from a function taking the device semaphore
 */
uint32
  arad_irr_lag_to_lag_range_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IRR_LAG_TO_LAG_RANGE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_IRR_LAG_TO_LAG_RANGE_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IRR_LAG_TO_LAG_RANGE_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_IRR_LAG_TO_LAG_RANGE_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IRR_LAG_TO_LAG_RANGEm, data, RANGEf, tbl_data->range);
  soc_mem_field32_set(unit, IRR_LAG_TO_LAG_RANGEm, data, MODEf, tbl_data->mode);

  res = soc_mem_write(unit, IRR_LAG_TO_LAG_RANGEm, MEM_BLOCK_ANY, entry_offset, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit)) {
    ARAD_CLEAR(data, uint32, ARAD_IRR_LAG_TO_LAG_RANGE_TBL_ENTRY_SIZE);

    soc_mem_field32_set(unit, IHB_IPP_LAG_TO_LAG_RANGEm, data, RANGEf, tbl_data->range);
    soc_mem_field32_set(unit, IHB_IPP_LAG_TO_LAG_RANGEm, data, IS_STATEFULf, tbl_data->is_stateful);

    res = soc_mem_write(unit, IHB_IPP_LAG_TO_LAG_RANGEm, MEM_BLOCK_ANY, entry_offset, data);
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
  }  
#endif /* BCM_88660_A0 */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_irr_lag_to_lag_range_tbl_set_unsafe()", entry_offset, 0);
}
/*
 * Read indirect table lag_mapping_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  arad_irr_lag_mapping_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN   uint32              lag_ndx,
    SOC_SAND_IN   uint32              port_ndx,
    SOC_SAND_OUT ARAD_IRR_LAG_MAPPING_TBL_DATA  *tbl_data
  )
{
  uint32
      entry_offset,
    res = SOC_SAND_OK;
  uint32
    data[ARAD_IRR_LAG_MAPPING_TBL_ENTRY_SIZE];
 
 
 

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IRR_LAG_MAPPING_TBL_GET_UNSAFE);

  entry_offset =
    (lag_ndx * arad_ports_lag_nof_lag_entries_get_unsafe(unit)) | port_ndx;

  ARAD_CLEAR(data, uint32, ARAD_IRR_LAG_MAPPING_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_IRR_LAG_MAPPING_TBL_DATA, 1);

 
 



  

  res = soc_mem_read(
          unit,
          IRR_LAG_MAPPINGm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

          tbl_data->destination   = soc_mem_field32_get(
                  unit,
                  IRR_LAG_MAPPINGm,
                  data,
                  DESTINATIONf            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_irr_lag_mapping_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table lag_mapping_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  arad_irr_lag_mapping_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN   uint32              lag_ndx,
    SOC_SAND_IN   uint32              port_ndx,
    SOC_SAND_IN  ARAD_IRR_LAG_MAPPING_TBL_DATA  *tbl_data
  )
{
  uint32
      entry_offset,
    res = SOC_SAND_OK;
  uint32
    data[ARAD_IRR_LAG_MAPPING_TBL_ENTRY_SIZE];
 
 
 

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IRR_LAG_MAPPING_TBL_SET_UNSAFE);

  entry_offset =
    (lag_ndx * arad_ports_lag_nof_lag_entries_get_unsafe(unit)) | port_ndx;

  ARAD_CLEAR(data, uint32, ARAD_IRR_LAG_MAPPING_TBL_ENTRY_SIZE);

 
 



  

            soc_mem_field32_set(
          unit,
          IRR_LAG_MAPPINGm,
          data,
          DESTINATIONf,
          tbl_data->destination );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_mem_write(
          unit,
          IRR_LAG_MAPPINGm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_irr_lag_mapping_tbl_set_unsafe()", entry_offset, 0);
}
/*
 * Read indirect table smooth_division_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  arad_irr_smooth_division_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN   uint32              lag_size,
    SOC_SAND_IN   uint32              hash_val,
    SOC_SAND_OUT ARAD_IRR_SMOOTH_DIVISION_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    entry_offset,
    data[ARAD_IRR_SMOOTH_DIVISION_TBL_ENTRY_SIZE];
 
 
 

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IRR_SMOOTH_DIVISION_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_IRR_SMOOTH_DIVISION_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_IRR_SMOOTH_DIVISION_TBL_DATA, 1);

 
 


  entry_offset = lag_size << ARAD_IRR_GLAG_DEVISION_HASH_NOF_BITS;
  entry_offset |= hash_val;


  

  res = soc_mem_read(
          unit,
          IRR_SMOOTH_DIVISIONm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

          tbl_data->member   = soc_mem_field32_get(
                  unit,
                  IRR_SMOOTH_DIVISIONm,
                  data,
                  MEMBER_0f            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_irr_smooth_division_tbl_get_unsafe()", lag_size, hash_val);
}

/*
 * Write indirect table smooth_division_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  arad_irr_smooth_division_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN   uint32              lag_size,
    SOC_SAND_IN   uint32              hash_val,
    SOC_SAND_IN  ARAD_IRR_SMOOTH_DIVISION_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    entry_offset,
    data[ARAD_IRR_SMOOTH_DIVISION_TBL_ENTRY_SIZE];
 
 
 SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IRR_SMOOTH_DIVISION_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_IRR_SMOOTH_DIVISION_TBL_ENTRY_SIZE);

 entry_offset = lag_size << (ARAD_IRR_GLAG_DEVISION_HASH_NOF_BITS-1);
 entry_offset |= (hash_val/2);

  res = soc_mem_read(
          unit,
          IRR_SMOOTH_DIVISIONm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 32, exit);

  if(hash_val%2 == 0) {
      soc_mem_field32_set(
              unit,
              IRR_SMOOTH_DIVISIONm,
              data,
              MEMBER_0f,
              tbl_data->member );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }
  else{
      soc_mem_field32_set(
              unit,
              IRR_SMOOTH_DIVISIONm,
              data,
              MEMBER_1f,
              tbl_data->member );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

  res = soc_mem_write(
          unit,
          IRR_SMOOTH_DIVISIONm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_irr_smooth_division_tbl_set_unsafe()", lag_size, hash_val);
}

/*
 * Read indirect table traffic_class_mapping_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  arad_irr_traffic_class_mapping_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IRR_TRAFFIC_CLASS_MAPPING_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_IRR_TRAFFIC_CLASS_MAPPING_TBL_ENTRY_SIZE];
  int32
    tc_ndx = 0;
  uint32
    tmp = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IRR_TRAFFIC_CLASS_MAPPING_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_IRR_TRAFFIC_CLASS_MAPPING_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_IRR_TRAFFIC_CLASS_MAPPING_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IRR_TRAFFIC_CLASS_MAPPINGm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  for (tc_ndx = 0; tc_ndx < ARAD_NOF_TRAFFIC_CLASSES; ++tc_ndx)
  {
    tmp = soc_mem_field32_get(
                  unit,
                  IRR_TRAFFIC_CLASS_MAPPINGm,
                  data,
                  TRAFFIC_CLASS_MAPPINGf );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    SHR_BITCOPY_RANGE(&(tbl_data->traffic_class_mapping[tc_ndx]), 0, &tmp, (tc_ndx*3), 3);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_irr_traffic_class_mapping_tbl_get_unsafe()", entry_offset, tc_ndx);
}

/*
 * Write indirect table traffic_class_mapping_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  arad_irr_traffic_class_mapping_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IRR_TRAFFIC_CLASS_MAPPING_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK,
    tmp = 0,
    err;
  uint32
    data[ARAD_IRR_TRAFFIC_CLASS_MAPPING_TBL_ENTRY_SIZE];
 int32
    tc_ndx = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IRR_TRAFFIC_CLASS_MAPPING_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_IRR_TRAFFIC_CLASS_MAPPING_TBL_ENTRY_SIZE);

  for (tc_ndx = 0; tc_ndx < ARAD_NOF_TRAFFIC_CLASSES; ++tc_ndx)
  {
      SHR_BITCOPY_RANGE(&tmp,(tc_ndx*3),&(tbl_data->traffic_class_mapping[tc_ndx]),0,3);      
  }

err=SOC_E_NONE;
  if (SOC_E_NONE == err) 
  {
      soc_mem_field32_set(
              unit,
              IRR_TRAFFIC_CLASS_MAPPINGm,
              data,
              TRAFFIC_CLASS_MAPPINGf,
              tmp);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
  }

  res = soc_mem_write(
          unit,
          IRR_TRAFFIC_CLASS_MAPPINGm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_irr_traffic_class_mapping_tbl_set_unsafe()", entry_offset, tc_ndx);
}

uint32
  arad_irr_stack_fec_resolve_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             entry_offset,
    SOC_SAND_OUT ARAD_IRR_STACK_FEC_RESOLVE_TBL_DATA  *tbl_data
  )
{
  uint32 res = SOC_SAND_OK;
  uint32 data[ARAD_IRR_STACK_FEC_RESOLVE_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_CLEAR(data, uint32, ARAD_IRR_STACK_FEC_RESOLVE_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_IRR_STACK_FEC_RESOLVE_TBL_DATA, 1);

  res = soc_mem_read(unit, IRR_STACK_FEC_RESOLVEm, MEM_BLOCK_ANY, entry_offset, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->stack_lag = soc_mem_field32_get(unit, IRR_STACK_FEC_RESOLVEm, data, STACK_LAGf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_irr_stack_fec_resolve_table_tbl_get_unsafe()", entry_offset, 0);
}


uint32
  arad_irr_stack_fec_resolve_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IRR_STACK_FEC_RESOLVE_TBL_DATA  *tbl_data
  )
{
  uint32 res = SOC_SAND_OK;
  uint32 data[ARAD_IRR_STACK_FEC_RESOLVE_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_CLEAR(data, uint32, ARAD_IRR_STACK_FEC_RESOLVE_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IRR_STACK_FEC_RESOLVEm, data, STACK_LAGf, tbl_data->stack_lag);

  res = soc_mem_write(unit, IRR_STACK_FEC_RESOLVEm, MEM_BLOCK_ALL, entry_offset, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 34, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_irr_stack_fec_resolve_table_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  arad_irr_stack_trunk_resolve_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             entry_offset,
    SOC_SAND_OUT ARAD_IRR_STACK_TRUNK_RESOLVE_TBL_DATA  *tbl_data
  )
{
  uint32 res = SOC_SAND_OK;
  uint32 data[ARAD_IRR_STACK_TRUNK_RESOLVE_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_CLEAR(data, uint32, ARAD_IRR_STACK_TRUNK_RESOLVE_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_IRR_STACK_TRUNK_RESOLVE_TBL_DATA, 1);

  res = soc_mem_read(unit, IRR_STACK_TRUNK_RESOLVEm, MEM_BLOCK_ANY, entry_offset, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->base_queue = soc_mem_field32_get(unit, IRR_STACK_TRUNK_RESOLVEm, data, BASE_QUEUEf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_irr_stack_trunk_resolve_table_tbl_get_unsafe()", entry_offset, 0);
}


uint32
  arad_irr_stack_trunk_resolve_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IRR_STACK_TRUNK_RESOLVE_TBL_DATA  *tbl_data
  )
{
  uint32 res = SOC_SAND_OK;
  uint32 data[ARAD_IRR_STACK_TRUNK_RESOLVE_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_CLEAR(data, uint32, ARAD_IRR_STACK_TRUNK_RESOLVE_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IRR_STACK_TRUNK_RESOLVEm, data, BASE_QUEUEf, tbl_data->base_queue);

  res = soc_mem_write(unit, IRR_STACK_TRUNK_RESOLVEm, MEM_BLOCK_ALL, entry_offset, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 34, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_irr_stack_trunk_resolve_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table tm_port_sys_port_config_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
int
  arad_ihp_tm_port_sys_port_config_tbl_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 entry_offset,
    SOC_SAND_IN  int                    core,
    SOC_SAND_OUT ARAD_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_DATA  *tbl_data
  )
{
    uint32 data[ARAD_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_ENTRY_SIZE];
 
    SOCDNX_INIT_FUNC_DEFS;
 
    sal_memset(data, 0, sizeof(uint32)*ARAD_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_ENTRY_SIZE);
    sal_memset(tbl_data, 0, sizeof(ARAD_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_DATA));

    SOCDNX_IF_ERR_EXIT(soc_mem_read(
          unit,
          IHP_PTC_SYS_PORT_CONFIGm,
          IHP_BLOCK(unit, core),
          entry_offset,
          data
        ));

    tbl_data->system_port_offset1 = soc_mem_field32_get(
                  unit,
                  IHP_PTC_SYS_PORT_CONFIGm,
                  data,
                  OFFSETf);

    tbl_data->system_port_value = soc_mem_field32_get(
                  unit,
                  IHP_PTC_SYS_PORT_CONFIGm,
                  data,
                  VALUEf);

    tbl_data->system_port_profile   = soc_mem_field32_get(
                  unit,
                  IHP_PTC_SYS_PORT_CONFIGm,
                  data,
                  PROFILEf);

    tbl_data->system_port_value_to_use   = soc_mem_field32_get(
                  unit,
                  IHP_PTC_SYS_PORT_CONFIGm,
                  data,
                  VALUE_TO_USEf);

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Write indirect table tm_port_sys_port_config_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
int
  arad_ihp_tm_port_sys_port_config_tbl_set_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32             entry_offset,
    SOC_SAND_IN  int                core,
    SOC_SAND_IN  ARAD_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_DATA  *tbl_data
  )
{
    uint32 data[ARAD_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_ENTRY_SIZE];
    SOCDNX_INIT_FUNC_DEFS;
 
    sal_memset(data, 0, sizeof(uint32)*ARAD_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_ENTRY_SIZE);

    soc_mem_field32_set(
          unit,
          IHP_PTC_SYS_PORT_CONFIGm,
          data,
          OFFSETf,
          tbl_data->system_port_offset1);

    soc_mem_field32_set(
          unit,
          IHP_PTC_SYS_PORT_CONFIGm,
          data,
          VALUEf,
          tbl_data->system_port_value);

    soc_mem_field32_set(
          unit,
          IHP_PTC_SYS_PORT_CONFIGm,
          data,
          PROFILEf,
          tbl_data->system_port_profile);

    soc_mem_field32_set(
          unit,
          IHP_PTC_SYS_PORT_CONFIGm,
          data,
          VALUE_TO_USEf,
          tbl_data->system_port_value_to_use);

    SOCDNX_IF_ERR_EXIT(soc_mem_write(
          unit,
          IHP_PTC_SYS_PORT_CONFIGm,
          IHP_BLOCK(unit, core),
          entry_offset,
          data
        ));

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Read indirect table pct_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
int
  arad_egq_pct_tbl_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 q_pair,
    SOC_SAND_IN  int                    core,
    SOC_SAND_OUT ARAD_EGQ_PCT_TBL_DATA  *tbl_data
  )
{
    uint32 data[ARAD_EGQ_PCT_TBL_ENTRY_SIZE];
 
    SOCDNX_INIT_FUNC_DEFS;

    sal_memset(data, 0, sizeof(uint32)*ARAD_EGQ_PCT_TBL_ENTRY_SIZE);
    sal_memset(tbl_data, 0, sizeof(ARAD_EGQ_PCT_TBL_DATA));

    SOCDNX_IF_ERR_EXIT(soc_mem_read(
          unit,
          EGQ_PORT_CONFIGURATION_TABLE_PCTm,
          EGQ_BLOCK(unit, core),
          q_pair,
          data
        ));

    tbl_data->prog_editor_value = soc_mem_field32_get(unit, EGQ_PORT_CONFIGURATION_TABLE_PCTm, data, PRGE_VARf);
    tbl_data->prog_editor_value = soc_mem_field32_get(unit, EGQ_PORT_CONFIGURATION_TABLE_PCTm, data, PRGE_VARf);
    tbl_data->prog_editor_profile = soc_mem_field32_get(unit, EGQ_PORT_CONFIGURATION_TABLE_PCTm, data, PRGE_PROFILEf);
    /* tbl_data->outbound_mirr = soc_mem_field32_get(unit, EGQ_PORT_CONFIGURATION_TABLE_PCTm, data, OUTBOUND_MIRRf); */
    tbl_data->port_profile = soc_mem_field32_get(unit, EGQ_PORT_CONFIGURATION_TABLE_PCTm, data, CGM_PORT_PROFILEf);
    tbl_data->port_ch_num = soc_mem_field32_get(unit, EGQ_PORT_CONFIGURATION_TABLE_PCTm, data, PORT_CH_NUMf);
    tbl_data->port_type = soc_mem_field32_get(unit, EGQ_PORT_CONFIGURATION_TABLE_PCTm, data, PORT_TYPEf);
    tbl_data->cr_adjust_type = soc_mem_field32_get(unit, EGQ_PORT_CONFIGURATION_TABLE_PCTm, data, CR_ADJUST_TYPEf);
    tbl_data->cos_map_profile = soc_mem_field32_get(unit, EGQ_PORT_CONFIGURATION_TABLE_PCTm, data, EGRESS_TCf);
    tbl_data->mirror_enable = soc_mem_field32_get(unit, EGQ_PORT_CONFIGURATION_TABLE_PCTm, data, MIRROR_ENABLEf);
    /* tbl_data->mirror_cmd = soc_mem_field32_get(unit, EGQ_PORT_CONFIGURATION_TABLE_PCTm, data, MIRROR_CMDf); */
    tbl_data->mirror_channel = soc_mem_field32_get(unit, EGQ_PORT_CONFIGURATION_TABLE_PCTm, data, MIRROR_CHANNELf);
    tbl_data->ecc = soc_mem_field32_get(unit, EGQ_PORT_CONFIGURATION_TABLE_PCTm, data, ECCf);
  
exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Write indirect table pct_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
int
  arad_egq_pct_tbl_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 q_pair,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  ARAD_EGQ_PCT_TBL_DATA  *tbl_data
  )
{
    uint32 data[ARAD_EGQ_PCT_TBL_ENTRY_SIZE];
 
    SOCDNX_INIT_FUNC_DEFS;

    sal_memset(data, 0, sizeof(uint32)*ARAD_EGQ_PCT_TBL_ENTRY_SIZE);

    SOCDNX_IF_ERR_EXIT(soc_mem_read(
          unit,
          EGQ_PORT_CONFIGURATION_TABLE_PCTm,
          EGQ_BLOCK(unit, core),
          q_pair,
          data
        ));

    soc_mem_field32_set(unit, EGQ_PORT_CONFIGURATION_TABLE_PCTm, data, PRGE_VARf, tbl_data->prog_editor_value);
    soc_mem_field32_set(unit, EGQ_PORT_CONFIGURATION_TABLE_PCTm, data, PRGE_PROFILEf, tbl_data->prog_editor_profile);
    /* soc_mem_field32_set(unit, EGQ_PORT_CONFIGURATION_TABLE_PCTm, data, OUTBOUND_MIRRf, tbl_data->outbound_mirr); */
    soc_mem_field32_set(unit, EGQ_PORT_CONFIGURATION_TABLE_PCTm, data, CGM_PORT_PROFILEf, tbl_data->port_profile);
    soc_mem_field32_set(unit, EGQ_PORT_CONFIGURATION_TABLE_PCTm, data, PORT_CH_NUMf, tbl_data->port_ch_num);
    soc_mem_field32_set(unit, EGQ_PORT_CONFIGURATION_TABLE_PCTm, data, PORT_TYPEf, tbl_data->port_type);
    soc_mem_field32_set(unit, EGQ_PORT_CONFIGURATION_TABLE_PCTm, data, CR_ADJUST_TYPEf, tbl_data->cr_adjust_type);
    soc_mem_field32_set(unit, EGQ_PORT_CONFIGURATION_TABLE_PCTm, data, EGRESS_TCf, tbl_data->cos_map_profile);
    soc_mem_field32_set(unit, EGQ_PORT_CONFIGURATION_TABLE_PCTm, data, MIRROR_ENABLEf, tbl_data->mirror_enable);
    /* soc_mem_field32_set(unit, EGQ_PORT_CONFIGURATION_TABLE_PCTm, data, MIRROR_CMDf, tbl_data->mirror_cmd); */
    soc_mem_field32_set(unit, EGQ_PORT_CONFIGURATION_TABLE_PCTm, data, MIRROR_CHANNELf, tbl_data->mirror_channel);
    soc_mem_field32_set(unit, EGQ_PORT_CONFIGURATION_TABLE_PCTm, data, ECCf, tbl_data->ecc);

    SOCDNX_IF_ERR_EXIT(soc_mem_write(
          unit,
          EGQ_PORT_CONFIGURATION_TABLE_PCTm,
          EGQ_BLOCK(unit, core),
          q_pair,
          data
        ));

exit:
    SOCDNX_FUNC_RETURN;
}
/*
 * Read indirect table tc_dp_map_table_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
STATIC
  void
    arad_egq_tc_dp_map_table_entry_translate_unsafe(
      SOC_SAND_IN  int             unit,
      SOC_SAND_IN  ARAD_EGQ_TC_DP_MAP_TBL_ENTRY       *entry,
      SOC_SAND_OUT uint32                         *entry_offset
    )
{
  uint32
    offset = 0;

  offset |= SOC_SAND_SET_BITS_RANGE(entry->dp, 1, 0);
  offset |= SOC_SAND_SET_BITS_RANGE(entry->tc, 4, 2);  
  offset |= SOC_SAND_SET_BITS_RANGE(entry->is_egr_mc, 5, 5);
  offset |= SOC_SAND_SET_BITS_RANGE(entry->map_profile, 8, 6);

  *entry_offset = offset;
}

void
  ARAD_EGQ_TC_DP_MAP_TBL_ENTRY_clear(
    SOC_SAND_OUT ARAD_EGQ_TC_DP_MAP_TBL_ENTRY       *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(ARAD_EGQ_TC_DP_MAP_TBL_ENTRY));
  info->tc = 0;
  info->dp = 0;  
  info->is_egr_mc = 0;
  info->map_profile = 0;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void 
  ARAD_PP_IHP_MACT_FLUSH_DB_TBL_DATA_clear(
     ARAD_PP_IHP_MACT_FLUSH_DB_TBL_DATA *info
     )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
    SOC_SAND_CHECK_NULL_INPUT(info);

   info->compare_valid = 0;
   info->compare_key_data_location= 0;
   info->compare_key_20_data= 0;
   info->compare_key_20_mask=0;
   COMPILER_64_ZERO(info->compare_payload_data);
   COMPILER_64_ZERO(info->compare_payload_mask);
   info->compare_accessed_data= 0;
   info->compare_accessed_mask= 0;
   info->action_drop= 0;
   COMPILER_64_ZERO(info->action_transplant_payload_data);
   COMPILER_64_ZERO(info->action_transplant_payload_mask);
   info->action_transplant_accessed_clear= 0;

exit:
    SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);

}


int
  arad_egq_tc_dp_map_tbl_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  int                            core,
    SOC_SAND_IN  ARAD_EGQ_TC_DP_MAP_TBL_ENTRY   *entry,
    SOC_SAND_OUT ARAD_EGQ_TC_DP_MAP_TBL_DATA    *tbl_data
  )
{
    uint32 entry_offset = 0;
    uint32 data = 0;
    SOCDNX_INIT_FUNC_DEFS;
 
    sal_memset(tbl_data, 0, sizeof(ARAD_EGQ_TC_DP_MAP_TBL_DATA));

    arad_egq_tc_dp_map_table_entry_translate_unsafe(unit, entry, &entry_offset);

    SOCDNX_IF_ERR_EXIT(soc_mem_read(unit, EGQ_TC_DP_MAPm, EGQ_BLOCK(unit, core), entry_offset, &data));

    tbl_data->tc = soc_mem_field32_get(unit, EGQ_TC_DP_MAPm, &data, EGRESS_TCf);
    tbl_data->dp = soc_mem_field32_get(unit, EGQ_TC_DP_MAPm, &data, CGM_MC_DPf);

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Write indirect table tc_dp_map_table_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
int
  arad_egq_tc_dp_map_tbl_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  int                            core,
    SOC_SAND_IN  ARAD_EGQ_TC_DP_MAP_TBL_ENTRY   *entry,
    SOC_SAND_IN  ARAD_EGQ_TC_DP_MAP_TBL_DATA    *tbl_data
  )
{
    uint32 entry_offset = 0;
    uint32 data = 0;

    SOCDNX_INIT_FUNC_DEFS;

    arad_egq_tc_dp_map_table_entry_translate_unsafe(unit, entry, &entry_offset);

    soc_mem_field32_set(unit, EGQ_TC_DP_MAPm, &data, EGRESS_TCf, tbl_data->tc);
    soc_mem_field32_set(unit, EGQ_TC_DP_MAPm, &data, CGM_MC_DPf, tbl_data->dp);

    SOCDNX_IF_ERR_EXIT(soc_mem_write(unit, EGQ_TC_DP_MAPm, EGQ_BLOCK(unit, core), entry_offset, &data));

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Write indirect table fqp_nif_port_mux_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
int
  arad_egq_fqp_nif_port_mux_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_EGQ_FQP_NIF_PORT_MUX_TBL_DATA  *tbl_data
  )
{
    uint32 data[ARAD_EGQ_FQP_NIF_PORT_MUX_TBL_ENTRY_SIZE];
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    sal_memset(data, 0, sizeof(uint32) * ARAD_EGQ_FQP_NIF_PORT_MUX_TBL_ENTRY_SIZE);
  
    soc_mem_field32_set(
          unit,
          EGQ_FQP_NIF_PORT_MUXm,
          data,
          FQP_NIF_PORT_MUXf,
          tbl_data->fqp_nif_port_mux);

    rv = soc_mem_write(
          unit,
          EGQ_FQP_NIF_PORT_MUXm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  arad_egq_fqp_nif_port_mux_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT  ARAD_EGQ_FQP_NIF_PORT_MUX_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_EGQ_FQP_NIF_PORT_MUX_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_CLEAR(data, uint32, ARAD_EGQ_FQP_NIF_PORT_MUX_TBL_ENTRY_SIZE);
  
    res = soc_mem_read(
          unit,
          EGQ_FQP_NIF_PORT_MUXm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

   tbl_data->fqp_nif_port_mux = soc_mem_field32_get(
          unit,
          EGQ_FQP_NIF_PORT_MUXm,
          data,
          FQP_NIF_PORT_MUXf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egq_fqp_nif_port_mux_tbl_set_unsafe()", entry_offset, 0);
}


/*
 * Write indirect table fqp_nif_port_mux_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
int
  arad_egq_pqp_nif_port_mux_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_EGQ_PQP_NIF_PORT_MUX_TBL_DATA  *tbl_data
  )
{
    uint32 data[ARAD_EGQ_PQP_NIF_PORT_MUX_TBL_ENTRY_SIZE];
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    sal_memset(data, 0, sizeof(uint32)*ARAD_EGQ_PQP_NIF_PORT_MUX_TBL_ENTRY_SIZE);
  
    soc_mem_field32_set(
          unit,
          EGQ_PQP_NIF_PORT_MUXm,
          data,
          PQP_NIF_PORT_MUXf,
          tbl_data->pqp_nif_port_mux );

    rv = soc_mem_write(
          unit,
          EGQ_PQP_NIF_PORT_MUXm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
    SOCDNX_IF_ERR_EXIT(rv);

exit:
    SOCDNX_FUNC_RETURN;
}

uint32
  arad_egq_pqp_nif_port_mux_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT  ARAD_EGQ_PQP_NIF_PORT_MUX_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_EGQ_PQP_NIF_PORT_MUX_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_CLEAR(data, uint32, ARAD_EGQ_PQP_NIF_PORT_MUX_TBL_ENTRY_SIZE);
  
    res = soc_mem_read(
          unit,
          EGQ_PQP_NIF_PORT_MUXm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

   tbl_data->pqp_nif_port_mux = soc_mem_field32_get(
          unit,
          EGQ_PQP_NIF_PORT_MUXm,
          data,
          PQP_NIF_PORT_MUXf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_egq_fqp_nif_port_mux_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table pinfo_pmf_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  arad_ihb_pinfo_lbp_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_PINFO_LBP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_IHB_PINFO_LBP_TBL_ENTRY_SIZE];
 
 
 

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IHB_PINFO_LBP_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_IHB_PINFO_LBP_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_IHB_PINFO_LBP_TBL_DATA, 1);

 
 



  

  res = soc_mem_read(
          unit,
          IHB_PINFO_LBPm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->counter_compension   = soc_mem_field32_get(
                  unit,
                  IHB_PINFO_LBPm,
                  data,
                  COUNTER_COMPENSIONf            );

exit:
SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ihb_pinfo_lbp_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_ihb_pinfo_lbp_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_PINFO_LBP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_IHB_PINFO_LBP_TBL_ENTRY_SIZE];
 
 
 

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IHB_PINFO_LBP_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_IHB_PINFO_LBP_TBL_ENTRY_SIZE);

 
 



  

            soc_mem_field32_set(
          unit,
          IHB_PINFO_LBPm,
          data,
          COUNTER_COMPENSIONf,
          tbl_data->counter_compension );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_mem_write(
          unit,
          IHB_PINFO_LBPm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ihb_pinfo_lbp_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  arad_ihb_pinfo_pmf_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_IHB_PINFO_PMF_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[SOC_DPP_DEFS_MAX(IHB_PINFO_PMF_NOF_LONGS)];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IHB_PINFO_PMF_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, SOC_DPP_DEFS_MAX(IHB_PINFO_PMF_NOF_LONGS));
  ARAD_CLEAR(tbl_data, ARAD_IHB_PINFO_PMF_TBL_DATA, 1);

  res = soc_mem_read(unit, IHB_PINFO_PMFm, MEM_BLOCK_ANY, entry_offset, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->port_pmf_profile = soc_mem_field32_get(unit, IHB_PINFO_PMFm, data, PROGRAM_SELECTION_PROFILEf); 
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ihb_pinfo_pmf_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table pinfo_pmf_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  arad_ihb_pinfo_pmf_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_IHB_PINFO_PMF_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[SOC_DPP_DEFS_MAX(IHB_PINFO_PMF_NOF_LONGS)];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IHB_PINFO_PMF_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, SOC_DPP_DEFS_MAX(IHB_PINFO_PMF_NOF_LONGS));

  soc_mem_field32_set(unit, IHB_PINFO_PMFm, data, PROGRAM_SELECTION_PROFILEf, tbl_data->port_pmf_profile); 
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_mem_write(unit, IHB_PINFO_PMFm, MEM_BLOCK_ANY, entry_offset, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_ihb_pinfo_pmf_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihb_pmf_program_selection_cam_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHB_PMF_PROGRAM_SELECTION_CAM_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[SOC_DPP_DEFS_MAX(IHB_PMF_PROGRAM_SELECTION_CAM_NOF_LONGS)];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_PMF_PROGRAM_SELECTION_CAM_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, SOC_DPP_DEFS_MAX(IHB_PMF_PROGRAM_SELECTION_CAM_NOF_LONGS));
  ARAD_CLEAR(tbl_data, ARAD_PP_IHB_PMF_PROGRAM_SELECTION_CAM_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHB_PMF_PROGRAM_SELECTION_CAMm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->parser_leaf_context = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, PARSER_LEAF_CONTEXTf);
  tbl_data->in_port_profile = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, IN_PORT_PROFILEf);
  tbl_data->ptc_profile = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, PTC_PROFILEf);
  tbl_data->packet_format_code = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, PACKET_FORMAT_CODEf);
  tbl_data->packet_format_qualifier_0 = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, PACKET_FORMAT_QUALIFIER_0f);
  tbl_data->packet_format_qualifier_fwd = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, PACKET_FORMAT_QUALIFIER_FWDf);
  tbl_data->forwarding_code = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, FORWARDING_CODEf);
  tbl_data->forwarding_offset_index = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, FORWARDING_OFFSET_INDEXf);
  tbl_data->forwarding_offset_index_ext = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, FORWARDING_OFFSET_INDEX_EXTf);
  tbl_data->cpu_trap_code = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, CPU_TRAP_CODEf);
  tbl_data->in_lif_profile = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, IN_LIF_PROFILEf);
  tbl_data->out_lif_range = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, OUT_LIF_RANGEf);
  tbl_data->llvp_incoming_tag_structure = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, LLVP_INCOMING_TAG_STRUCTUREf);
  tbl_data->vt_processing_profile = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, VT_PROCESSING_PROFILEf);
  tbl_data->vt_lookup_0_found = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, VT_LOOKUP_0_FOUNDf);
  tbl_data->vt_lookup_1_found = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, VT_LOOKUP_1_FOUNDf);
  tbl_data->tt_processing_profile = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, TT_PROCESSING_PROFILEf);
  tbl_data->tt_lookup_0_found = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, TT_LOOKUP_0_FOUNDf);
  tbl_data->tt_lookup_1_found = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, TT_LOOKUP_1_FOUNDf);
  tbl_data->fwd_prcessing_profile = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, FWD_PRCESSING_PROFILEf);
  tbl_data->lem_1st_lkp_found = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, LEM_1ST_LKP_FOUNDf);
  tbl_data->lem_2nd_lkp_found = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, LEM_2ND_LKP_FOUNDf);
  tbl_data->lpm_1st_lkp_found = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, LPM_1ST_LKP_FOUNDf);
  tbl_data->lpm_2nd_lkp_found = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, LPM_2ND_LKP_FOUNDf);
  tbl_data->tcam_found = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, SOC_IS_JERICHO(unit)? TCAM_0_FOUNDf: TCAM_FOUNDf);
  tbl_data->tcam_traps_found = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, SOC_IS_JERICHO(unit)? TCAM_TRAPS_0_FOUNDf: TCAM_TRAPS_FOUNDf);
  tbl_data->elk_error = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, ELK_ERRORf);
  tbl_data->elk_found_result = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, ELK_FOUND_RESULTf);
  tbl_data->mask_parser_leaf_context = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_PARSER_LEAF_CONTEXTf);
  tbl_data->mask_in_port_profile = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_IN_PORT_PROFILEf);
  tbl_data->mask_ptc_profile = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_PTC_PROFILEf);
  tbl_data->mask_packet_format_code = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_PACKET_FORMAT_CODEf);
  tbl_data->mask_packet_format_qualifier_0 = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_PACKET_FORMAT_QUALIFIER_0f);
  tbl_data->mask_packet_format_qualifier_fwd = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_PACKET_FORMAT_QUALIFIER_FWDf);
  tbl_data->mask_forwarding_code = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_FORWARDING_CODEf);
  tbl_data->mask_forwarding_offset_index = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_FORWARDING_OFFSET_INDEXf);
  tbl_data->mask_forwarding_offset_index_ext = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_FORWARDING_OFFSET_INDEX_EXTf);
  tbl_data->mask_cpu_trap_code = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_CPU_TRAP_CODEf);
  tbl_data->mask_in_lif_profile = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_IN_LIF_PROFILEf);
  tbl_data->mask_out_lif_range = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_OUT_LIF_RANGEf);
  tbl_data->mask_llvp_incoming_tag_structure = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_LLVP_INCOMING_TAG_STRUCTUREf);
  tbl_data->mask_vt_processing_profile = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_VT_PROCESSING_PROFILEf);
  tbl_data->mask_vt_lookup_0_found = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_VT_LOOKUP_0_FOUNDf);
  tbl_data->mask_vt_lookup_1_found = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_VT_LOOKUP_1_FOUNDf);
  tbl_data->mask_tt_processing_profile = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_TT_PROCESSING_PROFILEf);
  tbl_data->mask_tt_lookup_0_found = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_TT_LOOKUP_0_FOUNDf);
  tbl_data->mask_tt_lookup_1_found = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_TT_LOOKUP_1_FOUNDf);
  tbl_data->mask_fwd_prcessing_profile = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_FWD_PRCESSING_PROFILEf);
  tbl_data->mask_lem_1st_lkp_found = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_LEM_1ST_LKP_FOUNDf);
  tbl_data->mask_lem_2nd_lkp_found = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_LEM_2ND_LKP_FOUNDf);
  tbl_data->mask_lpm_1st_lkp_found = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_LPM_1ST_LKP_FOUNDf);
  tbl_data->mask_lpm_2nd_lkp_found = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_LPM_2ND_LKP_FOUNDf);
  tbl_data->mask_tcam_found = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, SOC_IS_JERICHO(unit)? MASK_TCAM_0_FOUNDf: MASK_TCAM_FOUNDf);
  tbl_data->mask_tcam_traps_found = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, SOC_IS_JERICHO(unit)? MASK_TCAM_TRAPS_0_FOUNDf: MASK_TCAM_TRAPS_FOUNDf);
  tbl_data->mask_elk_error = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_ELK_ERRORf);
  tbl_data->mask_elk_found_result = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_ELK_FOUND_RESULTf);
  tbl_data->program = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, PROGRAMf);
  tbl_data->valid = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, VALIDf);


#ifdef BCM_88660_A0
  /* Additional field */
  if (SOC_IS_ARADPLUS(unit)) {
        tbl_data->stamp_native_vsi = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, STAMP_NATIVE_VSIf);
        tbl_data->mask_stamp_native_vsi = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_STAMP_NATIVE_VSIf);
    }
#endif /* BCM_88660_A0 */

  if (SOC_IS_JERICHO(unit)) {
    tbl_data->in_rif_profile = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, IN_RIF_PROFILEf);
    tbl_data->mask_in_rif_profile = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_IN_RIF_PROFILEf);
    tbl_data->tcam_found_1 = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, TCAM_1_FOUNDf);
    tbl_data->mask_tcam_found_1 = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_TCAM_1_FOUNDf);
    tbl_data->tcam_traps_found_1 = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, TCAM_TRAPS_1_FOUNDf);
    tbl_data->mask_tcam_traps_found_1 = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_TCAM_TRAPS_1_FOUNDf);
    tbl_data->rpf_stamp_native_vsi = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, RPF_STAMP_NATIVE_VSIf);
    tbl_data->mask_rpf_stamp_native_vsi = soc_mem_field32_get(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_RPF_STAMP_NATIVE_VSIf);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_pmf_program_selection_cam_tbl_get_unsafe()", entry_offset, 0);
}

/* Fill only the buffer */
uint32
  arad_pp_ihb_pmf_program_selection_cam_tbl_data_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  ARAD_PP_IHB_PMF_PROGRAM_SELECTION_CAM_TBL_DATA *tbl_data,
    SOC_SAND_OUT  uint32 data[SOC_DPP_DEFS_MAX(IHB_PMF_PROGRAM_SELECTION_CAM_NOF_LONGS)]
  )
{
  uint32
    res = SOC_SAND_OK;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_PMF_PROGRAM_SELECTION_CAM_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, SOC_DPP_DEFS_MAX(IHB_PMF_PROGRAM_SELECTION_CAM_NOF_LONGS));

  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, PARSER_LEAF_CONTEXTf, tbl_data->parser_leaf_context);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, IN_PORT_PROFILEf, tbl_data->in_port_profile);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, PTC_PROFILEf, tbl_data->ptc_profile);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, PACKET_FORMAT_CODEf, tbl_data->packet_format_code);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, PACKET_FORMAT_QUALIFIER_0f, tbl_data->packet_format_qualifier_0);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, PACKET_FORMAT_QUALIFIER_FWDf, tbl_data->packet_format_qualifier_fwd);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, FORWARDING_CODEf, tbl_data->forwarding_code);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, FORWARDING_OFFSET_INDEXf, tbl_data->forwarding_offset_index);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, FORWARDING_OFFSET_INDEX_EXTf, tbl_data->forwarding_offset_index_ext);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, CPU_TRAP_CODEf, tbl_data->cpu_trap_code);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, IN_LIF_PROFILEf, tbl_data->in_lif_profile);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, OUT_LIF_RANGEf, tbl_data->out_lif_range);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, LLVP_INCOMING_TAG_STRUCTUREf, tbl_data->llvp_incoming_tag_structure);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, VT_PROCESSING_PROFILEf, tbl_data->vt_processing_profile);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, VT_LOOKUP_0_FOUNDf, tbl_data->vt_lookup_0_found);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, VT_LOOKUP_1_FOUNDf, tbl_data->vt_lookup_1_found);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, TT_PROCESSING_PROFILEf, tbl_data->tt_processing_profile);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, TT_LOOKUP_0_FOUNDf, tbl_data->tt_lookup_0_found);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, TT_LOOKUP_1_FOUNDf, tbl_data->tt_lookup_1_found);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, FWD_PRCESSING_PROFILEf, tbl_data->fwd_prcessing_profile);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, LEM_1ST_LKP_FOUNDf, tbl_data->lem_1st_lkp_found);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, LEM_2ND_LKP_FOUNDf, tbl_data->lem_2nd_lkp_found);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, LPM_1ST_LKP_FOUNDf, tbl_data->lpm_1st_lkp_found);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, LPM_2ND_LKP_FOUNDf, tbl_data->lpm_2nd_lkp_found);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, SOC_IS_JERICHO(unit)? TCAM_0_FOUNDf: TCAM_FOUNDf, tbl_data->tcam_found);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, SOC_IS_JERICHO(unit)? TCAM_TRAPS_0_FOUNDf: TCAM_TRAPS_FOUNDf, tbl_data->tcam_traps_found);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, ELK_ERRORf, tbl_data->elk_error);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, ELK_FOUND_RESULTf, tbl_data->elk_found_result);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_PARSER_LEAF_CONTEXTf, tbl_data->mask_parser_leaf_context);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_IN_PORT_PROFILEf, tbl_data->mask_in_port_profile);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_PTC_PROFILEf, tbl_data->mask_ptc_profile);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_PACKET_FORMAT_CODEf, tbl_data->mask_packet_format_code);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_PACKET_FORMAT_QUALIFIER_0f, tbl_data->mask_packet_format_qualifier_0);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_PACKET_FORMAT_QUALIFIER_FWDf, tbl_data->mask_packet_format_qualifier_fwd);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_FORWARDING_CODEf, tbl_data->mask_forwarding_code);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_FORWARDING_OFFSET_INDEXf, tbl_data->mask_forwarding_offset_index);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_FORWARDING_OFFSET_INDEX_EXTf, tbl_data->mask_forwarding_offset_index_ext);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_CPU_TRAP_CODEf, tbl_data->mask_cpu_trap_code);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_IN_LIF_PROFILEf, tbl_data->mask_in_lif_profile);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_OUT_LIF_RANGEf, tbl_data->mask_out_lif_range);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_LLVP_INCOMING_TAG_STRUCTUREf, tbl_data->mask_llvp_incoming_tag_structure);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_VT_PROCESSING_PROFILEf, tbl_data->mask_vt_processing_profile);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_VT_LOOKUP_0_FOUNDf, tbl_data->mask_vt_lookup_0_found);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_VT_LOOKUP_1_FOUNDf, tbl_data->mask_vt_lookup_1_found);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_TT_PROCESSING_PROFILEf, tbl_data->mask_tt_processing_profile);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_TT_LOOKUP_0_FOUNDf, tbl_data->mask_tt_lookup_0_found);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_TT_LOOKUP_1_FOUNDf, tbl_data->mask_tt_lookup_1_found);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_FWD_PRCESSING_PROFILEf, tbl_data->mask_fwd_prcessing_profile);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_LEM_1ST_LKP_FOUNDf, tbl_data->mask_lem_1st_lkp_found);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_LEM_2ND_LKP_FOUNDf, tbl_data->mask_lem_2nd_lkp_found);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_LPM_1ST_LKP_FOUNDf, tbl_data->mask_lpm_1st_lkp_found);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_LPM_2ND_LKP_FOUNDf, tbl_data->mask_lpm_2nd_lkp_found);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, SOC_IS_JERICHO(unit)? MASK_TCAM_0_FOUNDf: MASK_TCAM_FOUNDf, tbl_data->mask_tcam_found);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, SOC_IS_JERICHO(unit)? MASK_TCAM_TRAPS_0_FOUNDf: MASK_TCAM_TRAPS_FOUNDf, tbl_data->mask_tcam_traps_found);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_ELK_ERRORf, tbl_data->mask_elk_error);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_ELK_FOUND_RESULTf, tbl_data->mask_elk_found_result);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, PROGRAMf, tbl_data->program);
  soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, VALIDf, tbl_data->valid);

#ifdef BCM_88660_A0
  /* Additional field */
    if (SOC_IS_ARADPLUS(unit)) {
        soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, STAMP_NATIVE_VSIf, tbl_data->stamp_native_vsi);
        soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_STAMP_NATIVE_VSIf, tbl_data->mask_stamp_native_vsi);
    }
#endif /* BCM_88660_A0 */

    if (SOC_IS_JERICHO(unit)) {
      soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, IN_RIF_PROFILEf, tbl_data->in_rif_profile);
      soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_IN_RIF_PROFILEf, tbl_data->mask_in_rif_profile);
      soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, TCAM_1_FOUNDf, tbl_data->tcam_found_1);
      soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_TCAM_1_FOUNDf, tbl_data->mask_tcam_found_1);
      soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, TCAM_TRAPS_1_FOUNDf, tbl_data->tcam_traps_found_1);
      soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_TCAM_TRAPS_1_FOUNDf, tbl_data->mask_tcam_traps_found_1);
      soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, RPF_STAMP_NATIVE_VSIf, tbl_data->rpf_stamp_native_vsi);
      soc_mem_field32_set(unit, IHB_PMF_PROGRAM_SELECTION_CAMm, data, MASK_RPF_STAMP_NATIVE_VSIf, tbl_data->mask_rpf_stamp_native_vsi);
    }

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_pmf_program_selection_cam_tbl_data_set_unsafe()", 0, 0);
}

uint32
  arad_pp_ihb_slb_program_selection_cam_tbl_data_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  ARAD_PP_IHB_SLB_PROGRAM_SELECTION_CAM_TBL_DATA *tbl_data,
    SOC_SAND_OUT  uint32 data[ARAD_PP_IHB_SLB_PROGRAM_SELECTION_CAM_TBL_ENTRY_SIZE]
  )
{
  uint32
    res = SOC_SAND_OK;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHB_SLB_PROGRAM_SELECTION_CAM_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, PARSER_LEAF_CONTEXTf, tbl_data->parser_leaf_context);
  soc_mem_field32_set(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, VSI_PROFILEf, tbl_data->vsi_profile);
  soc_mem_field32_set(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, IN_LIF_PROFILEf, tbl_data->in_lif_profile);
  soc_mem_field32_set(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, SELECTED_FLP_PROGRAMf, tbl_data->flp_program);
  soc_mem_field32_set(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, PACKET_FORMAT_CODEf, tbl_data->packet_format_code);
  soc_mem_field32_set(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, FORWARDING_HEADER_NEXT_PROTOCOLf, tbl_data->pfq_next_protocol_fwd);
  soc_mem_field32_set(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, FORWARDING_HEADER_PLUS_ONE_NEXT_PROTOCOLf, tbl_data->pfq_next_protocol_fwd_next);
  soc_mem_field32_set(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, FORWARDING_CODEf, tbl_data->forwarding_code);
  soc_mem_field32_set(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, FORWARDING_OFFSET_INDEXf, tbl_data->forwarding_offset_index);
  soc_mem_field32_set(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, DESTINAITON_MSBSf, tbl_data->destination_msbs);
  soc_mem_field32_set(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, UNKNOWN_ADDRf, tbl_data->unknown_addr);

  soc_mem_field32_set(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, PARSER_LEAF_CONTEXT_MASKf, tbl_data->parser_leaf_context_mask);
  soc_mem_field32_set(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, VSI_PROFILE_MASKf, tbl_data->vsi_profile_mask);
  soc_mem_field32_set(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, IN_LIF_PROFILE_MASKf, tbl_data->in_lif_profile_mask);
  soc_mem_field32_set(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, FLP_PROGRAM_MASKf, tbl_data->flp_program_mask);
  soc_mem_field32_set(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, PACKET_FORMAT_CODE_MASKf, tbl_data->packet_format_code_mask);
  soc_mem_field32_set(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, FORWARDING_HEADER_NEXT_PROTOCOL_MASKf, tbl_data->pfq_next_protocol_fwd_mask);
  soc_mem_field32_set(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, FORWARDING_HEADER_PLUS_ONE_NEXT_PROTOCOL_MASKf, tbl_data->pfq_next_protocol_fwd_next_mask);
  soc_mem_field32_set(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, FORWARDING_CODE_MASKf, tbl_data->forwarding_code_mask);
  soc_mem_field32_set(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, FORWARDING_OFFSET_INDEX_MASKf, tbl_data->forwarding_offset_index_mask);
  soc_mem_field32_set(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, DESTINAITON_MSBS_MASKf, tbl_data->destination_msbs_mask);
  soc_mem_field32_set(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, UNKNOWN_ADDR_MASKf, tbl_data->unknown_addr_mask);

  soc_mem_field32_set(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, PROGRAMf, tbl_data->program);
  soc_mem_field32_set(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, VALIDf, tbl_data->valid);

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_slb_program_selection_cam_tbl_data_set_unsafe()", 0, 0);
}

uint32
  arad_pp_ihb_slb_program_selection_cam_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHB_SLB_PROGRAM_SELECTION_CAM_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHB_SLB_PROGRAM_SELECTION_CAM_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHB_SLB_PROGRAM_SELECTION_CAM_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHB_SLB_PROGRAM_SELECTION_CAM_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->parser_leaf_context = soc_mem_field32_get(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, PARSER_LEAF_CONTEXTf);
  tbl_data->vsi_profile = soc_mem_field32_get(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, VSI_PROFILEf);
  tbl_data->in_lif_profile = soc_mem_field32_get(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, IN_LIF_PROFILEf);
  tbl_data->flp_program = soc_mem_field32_get(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, SELECTED_FLP_PROGRAMf);
  tbl_data->packet_format_code = soc_mem_field32_get(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, PACKET_FORMAT_CODEf);
  tbl_data->pfq_next_protocol_fwd = soc_mem_field32_get(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, FORWARDING_HEADER_NEXT_PROTOCOLf);
  tbl_data->pfq_next_protocol_fwd_next = soc_mem_field32_get(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, FORWARDING_HEADER_PLUS_ONE_NEXT_PROTOCOLf);
  tbl_data->forwarding_code = soc_mem_field32_get(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, FORWARDING_CODEf);
  tbl_data->forwarding_offset_index = soc_mem_field32_get(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, FORWARDING_OFFSET_INDEXf);
  tbl_data->destination_msbs = soc_mem_field32_get(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, DESTINAITON_MSBSf);
  tbl_data->unknown_addr = soc_mem_field32_get(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, UNKNOWN_ADDRf);

  tbl_data->parser_leaf_context_mask = soc_mem_field32_get(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, PARSER_LEAF_CONTEXT_MASKf);
  tbl_data->vsi_profile_mask = soc_mem_field32_get(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, VSI_PROFILE_MASKf);
  tbl_data->in_lif_profile_mask = soc_mem_field32_get(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, IN_LIF_PROFILE_MASKf);
  tbl_data->flp_program_mask = soc_mem_field32_get(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, FLP_PROGRAM_MASKf);
  tbl_data->packet_format_code_mask = soc_mem_field32_get(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, PACKET_FORMAT_CODE_MASKf);
  tbl_data->pfq_next_protocol_fwd_mask = soc_mem_field32_get(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, FORWARDING_HEADER_NEXT_PROTOCOL_MASKf);
  tbl_data->pfq_next_protocol_fwd_next_mask = soc_mem_field32_get(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, FORWARDING_HEADER_PLUS_ONE_NEXT_PROTOCOL_MASKf);
  tbl_data->forwarding_code_mask = soc_mem_field32_get(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, FORWARDING_CODE_MASKf);
  tbl_data->forwarding_offset_index_mask = soc_mem_field32_get(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, FORWARDING_OFFSET_INDEX_MASKf);
  tbl_data->destination_msbs_mask = soc_mem_field32_get(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, DESTINAITON_MSBS_MASKf);
  tbl_data->unknown_addr_mask = soc_mem_field32_get(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, UNKNOWN_ADDR_MASKf);

  tbl_data->program = soc_mem_field32_get(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, PROGRAMf);
  tbl_data->valid = soc_mem_field32_get(unit, IHP_CONSISTENT_HASHING_PROGRAM_SEL_TCAMm, data, VALIDf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_slb_program_selection_cam_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihb_pmf_program_selection_cam_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHB_PMF_PROGRAM_SELECTION_CAM_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[SOC_DPP_DEFS_MAX(IHB_PMF_PROGRAM_SELECTION_CAM_NOF_LONGS)];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_PMF_PROGRAM_SELECTION_CAM_TBL_SET_UNSAFE);

  res = arad_pp_ihb_pmf_program_selection_cam_tbl_data_set_unsafe(
            unit,
            tbl_data,
            data
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_mem_write(
          unit,
          IHB_PMF_PROGRAM_SELECTION_CAMm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_pmf_program_selection_cam_tbl_set_unsafe()", entry_offset, 0);
}


/*
 * Read indirect table copy_engine1_program_tbl from block EPNI,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  arad_epni_lfem_field_select_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              tbl_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_EPNI_LFEM_FIELD_SELECT_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EPNI_LFEM0_FIELD_SELECT_MAP_TBL_GET_UNSAFE);

  ARAD_CLEAR(tbl_data, ARAD_EPNI_LFEM_FIELD_SELECT_MAP_TBL_DATA, 1);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_EPNI_LFEM_FIELD_SELECT_MAPm(unit, tbl_ndx, MEM_BLOCK_ANY, entry_offset, tbl_data->lfem_field_select_map));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_epni_lfem_field_select_map_tbl_get_unsafe()", tbl_ndx, entry_offset);
}

uint32
  arad_epni_lfem_field_select_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              tbl_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_EPNI_LFEM_FIELD_SELECT_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_EPNI_LFEM0_FIELD_SELECT_MAP_TBL_SET_UNSAFE);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 35, exit, WRITE_EPNI_LFEM_FIELD_SELECT_MAPm(unit, tbl_ndx, MEM_BLOCK_ANY, entry_offset, (void*)(tbl_data->lfem_field_select_map)));

  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit); 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_epni_lfem_field_select_map_tbl_set_unsafe()", tbl_ndx, entry_offset);
}

uint32
  arad_pp_egq_pmf_program_selection_cam_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_EGQ_PMF_PROGRAM_SELECTION_CAM_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EGQ_PMF_PROGRAM_SELECTION_CAM_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EGQ_PMF_PROGRAM_SELECTION_CAM_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EGQ_PMF_PROGRAM_SELECTION_CAM_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_EGQ_PMF_PROGRAM_SELECTION_CAM_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          EGQ_PMF_PROGRAM_SELECTION_CAMm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->egress_pmf_profile = soc_mem_field32_get(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, data, EGRESS_PMF_PROFILEf);
  tbl_data->format_code = soc_mem_field32_get(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, data, FORMAT_CODEf);
  tbl_data->value_1 = soc_mem_field32_get(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, data, VALUE_1f);
  tbl_data->value_2 = soc_mem_field32_get(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, data, VALUE_2f);
  tbl_data->header_code = soc_mem_field32_get(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, data, HEADER_CODEf);
  tbl_data->qualifier = soc_mem_field32_get(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, data, QUALIFIERf);
  tbl_data->ethernet_tag_format = soc_mem_field32_get(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, data, ETHERNET_TAG_FORMATf);
  tbl_data->egress_pmf_profile_mask = soc_mem_field32_get(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, data, EGRESS_PMF_PROFILE_MASKf);
  tbl_data->format_code_mask = soc_mem_field32_get(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, data, FORMAT_CODE_MASKf);
  tbl_data->value_1_mask = soc_mem_field32_get(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, data, VALUE_1_MASKf);
  tbl_data->value_2_mask = soc_mem_field32_get(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, data, VALUE_2_MASKf);
  tbl_data->header_code_mask = soc_mem_field32_get(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, data, HEADER_CODE_MASKf);
  tbl_data->qualifier_mask = soc_mem_field32_get(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, data, QUALIFIER_MASKf);
  tbl_data->ethernet_tag_format_mask = soc_mem_field32_get(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, data, ETHERNET_TAG_FORMAT_MASKf);
  tbl_data->program = soc_mem_field32_get(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, data, PROGRAMf);
  tbl_data->valid = soc_mem_field32_get(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, data, VALIDf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_egq_pmf_program_selection_cam_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_egq_pmf_program_selection_cam_tbl_data_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  ARAD_PP_EGQ_PMF_PROGRAM_SELECTION_CAM_TBL_DATA *tbl_data,
    SOC_SAND_OUT uint32   data[ARAD_PP_EGQ_PMF_PROGRAM_SELECTION_CAM_TBL_ENTRY_SIZE]
  )
{
  uint32
    res = SOC_SAND_OK;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EGQ_PMF_PROGRAM_SELECTION_CAM_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EGQ_PMF_PROGRAM_SELECTION_CAM_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, data, EGRESS_PMF_PROFILEf, tbl_data->egress_pmf_profile);
  soc_mem_field32_set(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, data, FORMAT_CODEf, tbl_data->format_code);
  soc_mem_field32_set(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, data, VALUE_1f, tbl_data->value_1);
  soc_mem_field32_set(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, data, VALUE_2f, tbl_data->value_2);
  soc_mem_field32_set(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, data, HEADER_CODEf, tbl_data->header_code);
  soc_mem_field32_set(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, data, QUALIFIERf, tbl_data->qualifier);
  soc_mem_field32_set(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, data, ETHERNET_TAG_FORMATf, tbl_data->ethernet_tag_format);
  soc_mem_field32_set(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, data, EGRESS_PMF_PROFILE_MASKf, tbl_data->egress_pmf_profile_mask);
  soc_mem_field32_set(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, data, FORMAT_CODE_MASKf, tbl_data->format_code_mask);
  soc_mem_field32_set(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, data, VALUE_1_MASKf, tbl_data->value_1_mask);
  soc_mem_field32_set(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, data, VALUE_2_MASKf, tbl_data->value_2_mask);
  soc_mem_field32_set(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, data, HEADER_CODE_MASKf, tbl_data->header_code_mask);
  soc_mem_field32_set(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, data, QUALIFIER_MASKf, tbl_data->qualifier_mask);
  soc_mem_field32_set(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, data, ETHERNET_TAG_FORMAT_MASKf, tbl_data->ethernet_tag_format_mask);
  soc_mem_field32_set(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, data, PROGRAMf, tbl_data->program);
  soc_mem_field32_set(unit, EGQ_PMF_PROGRAM_SELECTION_CAMm, data, VALIDf, tbl_data->valid);

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_egq_pmf_program_selection_cam_tbl_data_set_unsafe()", 0, 0);
}



uint32
  arad_pp_egq_pmf_program_selection_cam_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_EGQ_PMF_PROGRAM_SELECTION_CAM_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EGQ_PMF_PROGRAM_SELECTION_CAM_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EGQ_PMF_PROGRAM_SELECTION_CAM_TBL_SET_UNSAFE);

  res = arad_pp_egq_pmf_program_selection_cam_tbl_data_set_unsafe(
            unit,
            tbl_data,
            data
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  

  res = soc_mem_write(
          unit,
          EGQ_PMF_PROGRAM_SELECTION_CAMm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_egq_pmf_program_selection_cam_tbl_set_unsafe()", entry_offset, 0);
}


/*
 * Read indirect table calrx_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  arad_cfc_oob_calrx_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_TMC_FC_OOB_ID          if_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_CFC_CALRX_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_CFC_CALRX_TBL_ENTRY_SIZE];
 
 uint32
   tbl;
 

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_CLEAR(data, uint32, ARAD_CFC_CALRX_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_CFC_CALRX_TBL_DATA, 1);

   tbl = (if_ndx == SOC_TMC_FC_OOB_ID_A ? CFC_SPI_OOB_RX_0_CALm : CFC_SPI_OOB_RX_1_CALm);
 



  

  res = soc_mem_read(
          unit,
          tbl,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

          tbl_data->fc_index   = soc_mem_field32_get(
                  unit,
                  tbl,
                  data,
                  FC_INDEXf            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

          tbl_data->fc_dest_sel   = soc_mem_field32_get(
                  unit,
                  tbl,
                  data,
                  FC_DST_SELf            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cfc_oob_calrx_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table calrx_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  arad_cfc_oob_calrx_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_TMC_FC_OOB_ID          if_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_CFC_CALRX_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_CFC_CALRX_TBL_ENTRY_SIZE];
 uint32
   tbl;
 
 

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_CLEAR(data, uint32, ARAD_CFC_CALRX_TBL_ENTRY_SIZE);

  tbl = (if_ndx == SOC_TMC_FC_OOB_ID_A ? CFC_SPI_OOB_RX_0_CALm : CFC_SPI_OOB_RX_1_CALm);

 



  

            soc_mem_field32_set(
          unit,
          tbl,
          data,
          FC_INDEXf,
          tbl_data->fc_index );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

            soc_mem_field32_set(
          unit,
          tbl,
          data,
          FC_DST_SELf,
          tbl_data->fc_dest_sel );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_mem_write(
          unit,
          tbl,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cfc_oob_calrx_tbl_set_unsafe()", entry_offset, 0);
}
/*
 * Read indirect table caltx_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  arad_cfc_oob_caltx_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_TMC_FC_OOB_ID                   if_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_CFC_CALTX_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_CFC_CALTX_TBL_ENTRY_SIZE];
  uint32
    tbl;
 
 

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_CLEAR(data, uint32, ARAD_CFC_CALTX_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_CFC_CALTX_TBL_DATA, 1);

 
  tbl = (if_ndx == SOC_TMC_FC_OOB_ID_A ? CFC_SPI_OOB_TX_0_CALm : CFC_SPI_OOB_TX_1_CALm);




  

  res = soc_mem_read(
          unit,
          tbl,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);


          tbl_data->fc_index   = soc_mem_field32_get(
                  unit,
                  tbl,
                  data,
                  FC_INDEXf            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

          tbl_data->fc_source_sel   = soc_mem_field32_get(
                  unit,
                  tbl,
                  data,
                  FC_SRC_SELf            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cfc_oob_caltx_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table caltx_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  arad_cfc_oob_caltx_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_TMC_FC_OOB_ID          if_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_CFC_CALTX_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_CFC_CALTX_TBL_ENTRY_SIZE];
  uint32
    tbl;
 
 

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_CLEAR(data, uint32, ARAD_CFC_CALTX_TBL_ENTRY_SIZE);

  tbl = (if_ndx == SOC_TMC_FC_OOB_ID_A ? CFC_SPI_OOB_TX_0_CALm : CFC_SPI_OOB_TX_1_CALm);

 



  

            soc_mem_field32_set(
          unit,
          tbl,
          data,
          FC_INDEXf,
          tbl_data->fc_index );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

            soc_mem_field32_set(
          unit,
          tbl,
          data,
          FC_SRC_SELf,
          tbl_data->fc_source_sel );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_mem_write(
          unit,
          tbl,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cfc_oob_caltx_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table ilkn_calrx_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  arad_cfc_ilkn_calrx_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_TMC_FC_OOB_ID          if_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_CFC_CALRX_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_CFC_CALRX_TBL_ENTRY_SIZE];
  uint32
    tbl = 0;
 
 

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_CLEAR(data, uint32, ARAD_CFC_CALRX_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_CFC_CALRX_TBL_DATA, 1);

  tbl = (if_ndx == SOC_TMC_FC_OOB_ID_A ? CFC_ILKN_RX_0_CALm : CFC_ILKN_RX_1_CALm);
 



  

  res = soc_mem_read(
          unit,
          tbl,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

          tbl_data->fc_index   = soc_mem_field32_get(
                  unit,
                  tbl,
                  data,
                  FC_INDEXf            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

          tbl_data->fc_dest_sel   = soc_mem_field32_get(
                  unit,
                  tbl,
                  data,
                  FC_DST_SELf            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cfc_ilkn_calrx_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_cfc_hcfc_bitmap_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32  entry_offset,
    SOC_SAND_IN  ARAD_CFC_HCFC_BITMAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK,
    i;
  uint32
    data[ARAD_CFC_HCFC_BITMAP_TBL_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  for(i=0; i<ARAD_CFC_HCFC_BITMAP_TBL_SIZE; i++)
  {
    data[i] = tbl_data->bitmap[i];
  }

  res = soc_mem_write(
          unit,
          CFC_CAT_2_TC_MAP_HCFCm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cfc_hcfc_bitmap_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  arad_cfc_hcfc_bitmap_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32  entry_offset,
    SOC_SAND_OUT ARAD_CFC_HCFC_BITMAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK,
    i;
  uint32
    data[ARAD_CFC_HCFC_BITMAP_TBL_SIZE + 1];

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  
  res = soc_sand_os_memset(
        data,
        0x0,
        sizeof(uint32) * ARAD_CFC_HCFC_BITMAP_TBL_SIZE
      );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 5, exit);
  
  res = soc_mem_read(
          unit,
          CFC_CAT_2_TC_MAP_HCFCm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  for(i=0; i<ARAD_CFC_HCFC_BITMAP_TBL_SIZE; i++)
  {
    tbl_data->bitmap[i] = data[i];
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cfc_hcfc_bitmap_tbl_get_unsafe()", entry_offset, 0);
}


/*
 * Write indirect table ilkn_calrx_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  arad_cfc_ilkn_calrx_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_TMC_FC_OOB_ID          if_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_CFC_CALRX_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_CFC_CALRX_TBL_ENTRY_SIZE];
  uint32
    tbl = 0;
 
 

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_CLEAR(data, uint32, ARAD_CFC_CALRX_TBL_ENTRY_SIZE);

 
  tbl = (if_ndx == SOC_TMC_FC_OOB_ID_A ? CFC_ILKN_RX_0_CALm : CFC_ILKN_RX_1_CALm);




  

            soc_mem_field32_set(
          unit,
          tbl,
          data,
          FC_INDEXf,
          tbl_data->fc_index );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

            soc_mem_field32_set(
          unit,
          tbl,
          data,
          FC_DST_SELf,
          tbl_data->fc_dest_sel );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  /*
  soc_mem_field32_set(
    unit,
    tbl,
    data,
    PARITYf,
    tbl_data->parity );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);
*/
  res = soc_mem_write(
          unit,
          tbl,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cfc_ilkn_calrx_tbl_set_unsafe()", entry_offset, 0);
}
/*
 * Read indirect table ilkn_caltx_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  arad_cfc_ilkn_caltx_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_TMC_FC_OOB_ID          if_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_CFC_CALTX_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_CFC_CALTX_TBL_ENTRY_SIZE];
 uint32
   tbl;
 
 

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_CLEAR(data, uint32, ARAD_CFC_CALTX_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_CFC_CALTX_TBL_DATA, 1);

  tbl = (if_ndx == SOC_TMC_FC_OOB_ID_A ? CFC_ILKN_TX_0_CALm : CFC_ILKN_TX_1_CALm); 



  

  res = soc_mem_read(
          unit,
          tbl,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

          tbl_data->fc_index   = soc_mem_field32_get(
                  unit,
                  tbl,
                  data,
                  FC_INDEXf            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

          tbl_data->fc_source_sel   = soc_mem_field32_get(
                  unit,
                  tbl,
                  data,
                  FC_SRC_SELf            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cfc_ilkn_caltx_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table ilkn_caltx_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  arad_cfc_ilkn_caltx_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_TMC_FC_OOB_ID          if_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_CFC_CALTX_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_CFC_CALTX_TBL_ENTRY_SIZE];
 uint32
   tbl;
 
 

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_CLEAR(data, uint32, ARAD_CFC_CALTX_TBL_ENTRY_SIZE);

  tbl = (if_ndx == SOC_TMC_FC_OOB_ID_A ? CFC_ILKN_TX_0_CALm : CFC_ILKN_TX_1_CALm); 

 



  

            soc_mem_field32_set(
          unit,
          tbl,
          data,
          FC_INDEXf,
          tbl_data->fc_index );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

            soc_mem_field32_set(
          unit,
          tbl,
          data,
          FC_SRC_SELf,
          tbl_data->fc_source_sel );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_mem_write(
          unit,
          tbl,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cfc_ilkn_caltx_tbl_set_unsafe()", entry_offset, 0);
}

/* 
 *  PP-Tables - B
 */ 


uint32
  arad_pp_ihp_flush_db_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_MACT_FLUSH_DB_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[SOC_DPP_DEFS_MAX(PPDB_LARGE_EM_FLUSH_DB_NOF_LONGS)];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_MACT_FLUSH_DB_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, SOC_DPP_DEFS_MAX(PPDB_LARGE_EM_FLUSH_DB_NOF_LONGS));
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_MACT_FLUSH_DB_TBL_DATA, 1);

  /* read from SW as it HW has no read 
  res = soc_mem_read(
          unit,
          PPDB_B_LARGE_EM_FLUSH_DBm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);*/

  res = arad_pp_sw_db_mact_flush_db_get(unit,entry_offset,data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->compare_valid = soc_mem_field32_get(unit, PPDB_B_LARGE_EM_FLUSH_DBm, data, COMPARE_VALIDf);
  tbl_data->compare_key_data_location = soc_mem_field32_get(unit, PPDB_B_LARGE_EM_FLUSH_DBm, data, COMPARE_KEY_DATA_LOCATIONf);
  tbl_data->compare_key_20_data = soc_mem_field32_get(unit, PPDB_B_LARGE_EM_FLUSH_DBm, data, COMPARE_KEY_20_DATAf);
  tbl_data->compare_key_20_mask = soc_mem_field32_get(unit, PPDB_B_LARGE_EM_FLUSH_DBm, data, COMPARE_KEY_20_MASKf);
  soc_mem_field64_get(unit, PPDB_B_LARGE_EM_FLUSH_DBm, data, COMPARE_PAYLOAD_DATAf, &tbl_data->compare_payload_data);
  soc_mem_field64_get(unit, PPDB_B_LARGE_EM_FLUSH_DBm, data, COMPARE_PAYLOAD_MASKf, &tbl_data->compare_payload_mask);
  tbl_data->compare_accessed_data = soc_mem_field32_get(unit, PPDB_B_LARGE_EM_FLUSH_DBm, data, COMPARE_ACCESSED_DATAf);
  tbl_data->compare_accessed_mask = soc_mem_field32_get(unit, PPDB_B_LARGE_EM_FLUSH_DBm, data, COMPARE_ACCESSED_MASKf);
  tbl_data->action_drop = soc_mem_field32_get(unit, PPDB_B_LARGE_EM_FLUSH_DBm, data, ACTION_DROPf);
  soc_mem_field64_get(unit, PPDB_B_LARGE_EM_FLUSH_DBm, data, ACTION_TRANSPLANT_PAYLOAD_DATAf, &tbl_data->action_transplant_payload_data);
  soc_mem_field64_get(unit, PPDB_B_LARGE_EM_FLUSH_DBm, data, ACTION_TRANSPLANT_PAYLOAD_MASKf, &tbl_data->action_transplant_payload_mask);
  tbl_data->action_transplant_accessed_clear = soc_mem_field32_get(unit, PPDB_B_LARGE_EM_FLUSH_DBm, data, ACTION_TRANSPLANT_ACCESSED_CLEARf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_flush_db_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_flush_db_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_MACT_FLUSH_DB_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[SOC_DPP_DEFS_MAX(PPDB_LARGE_EM_FLUSH_DB_NOF_LONGS)];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_MACT_FLUSH_DB_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, SOC_DPP_DEFS_MAX(PPDB_LARGE_EM_FLUSH_DB_NOF_LONGS));

  soc_mem_field32_set(unit, PPDB_B_LARGE_EM_FLUSH_DBm, data, COMPARE_VALIDf, tbl_data->compare_valid);
  soc_mem_field32_set(unit, PPDB_B_LARGE_EM_FLUSH_DBm, data, COMPARE_KEY_DATA_LOCATIONf, tbl_data->compare_key_data_location);
  soc_mem_field32_set(unit, PPDB_B_LARGE_EM_FLUSH_DBm, data, COMPARE_KEY_20_DATAf, tbl_data->compare_key_20_data);
  soc_mem_field32_set(unit, PPDB_B_LARGE_EM_FLUSH_DBm, data, COMPARE_KEY_20_MASKf, tbl_data->compare_key_20_mask);
  soc_mem_field64_set(unit, PPDB_B_LARGE_EM_FLUSH_DBm, data, COMPARE_PAYLOAD_DATAf, tbl_data->compare_payload_data);
  soc_mem_field64_set(unit, PPDB_B_LARGE_EM_FLUSH_DBm, data, COMPARE_PAYLOAD_MASKf, tbl_data->compare_payload_mask);
  soc_mem_field32_set(unit, PPDB_B_LARGE_EM_FLUSH_DBm, data, COMPARE_ACCESSED_DATAf, tbl_data->compare_accessed_data);
  soc_mem_field32_set(unit, PPDB_B_LARGE_EM_FLUSH_DBm, data, COMPARE_ACCESSED_MASKf, tbl_data->compare_accessed_mask);
  soc_mem_field32_set(unit, PPDB_B_LARGE_EM_FLUSH_DBm, data, ACTION_DROPf, tbl_data->action_drop);
  soc_mem_field64_set(unit, PPDB_B_LARGE_EM_FLUSH_DBm, data, ACTION_TRANSPLANT_PAYLOAD_DATAf, tbl_data->action_transplant_payload_data);
  soc_mem_field64_set(unit, PPDB_B_LARGE_EM_FLUSH_DBm, data, ACTION_TRANSPLANT_PAYLOAD_MASKf, tbl_data->action_transplant_payload_mask);
  soc_mem_field32_set(unit, PPDB_B_LARGE_EM_FLUSH_DBm, data, ACTION_TRANSPLANT_ACCESSED_CLEARf, tbl_data->action_transplant_accessed_clear);

  res = soc_mem_write(
          unit,
          PPDB_B_LARGE_EM_FLUSH_DBm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

  res = arad_pp_sw_db_mact_flush_db_set(unit,entry_offset,data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_flush_db_tbl_set_unsafe()", entry_offset, 0);
}



uint32
  arad_pp_ihp_mact_aging_configuration_table_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_MACT_AGING_CONFIGURATION_TABLE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_MACT_AGING_CONFIGURATION_TABLE_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_MACT_AGING_CONFIGURATION_TABLE_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_MACT_AGING_CONFIGURATION_TABLE_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_MACT_AGING_CONFIGURATION_TABLE_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          PPDB_B_LARGE_EM_AGING_CONFIGURATION_TABLEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->aging_cfg_info_delete_entry = soc_mem_field32_get(unit, PPDB_B_LARGE_EM_AGING_CONFIGURATION_TABLEm, data, AGING_CFG_INFO_DELETE_ENTRYf);
  tbl_data->aging_cfg_info_create_aged_out_event = soc_mem_field32_get(unit, PPDB_B_LARGE_EM_AGING_CONFIGURATION_TABLEm, data, AGING_CFG_INFO_CREATE_AGED_OUT_EVENTf);
  tbl_data->aging_cfg_info_create_refresh_event = soc_mem_field32_get(unit, PPDB_B_LARGE_EM_AGING_CONFIGURATION_TABLEm, data, AGING_CFG_INFO_CREATE_REFRESH_EVENTf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_mact_aging_configuration_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_mact_aging_configuration_table_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_MACT_AGING_CONFIGURATION_TABLE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_MACT_AGING_CONFIGURATION_TABLE_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_MACT_AGING_CONFIGURATION_TABLE_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_MACT_AGING_CONFIGURATION_TABLE_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, PPDB_B_LARGE_EM_AGING_CONFIGURATION_TABLEm, data, AGING_CFG_INFO_DELETE_ENTRYf, tbl_data->aging_cfg_info_delete_entry);
  soc_mem_field32_set(unit, PPDB_B_LARGE_EM_AGING_CONFIGURATION_TABLEm, data, AGING_CFG_INFO_CREATE_AGED_OUT_EVENTf, tbl_data->aging_cfg_info_create_aged_out_event);
  soc_mem_field32_set(unit, PPDB_B_LARGE_EM_AGING_CONFIGURATION_TABLEm, data, AGING_CFG_INFO_CREATE_REFRESH_EVENTf, tbl_data->aging_cfg_info_create_refresh_event);

  res = soc_mem_write(
          unit,
          PPDB_B_LARGE_EM_AGING_CONFIGURATION_TABLEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_mact_aging_configuration_table_tbl_set_unsafe()", entry_offset, 0);
}


int
  arad_pp_ihp_tm_port_parser_program_pointer_config_tbl_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 entry_offset,
    SOC_SAND_IN  int                    core,
    SOC_SAND_OUT ARAD_PP_IHP_TM_PORT_PARSER_PROGRAM_POINTER_CONFIG_TBL_DATA  *tbl_data
  )
{
    uint32 data[ARAD_PP_IHP_TM_PORT_PARSER_PROGRAM_POINTER_CONFIG_TBL_ENTRY_SIZE];
 
    SOCDNX_INIT_FUNC_DEFS;

    sal_memset(data, 0, sizeof(uint32) * ARAD_PP_IHP_TM_PORT_PARSER_PROGRAM_POINTER_CONFIG_TBL_ENTRY_SIZE);
    sal_memset(tbl_data, 0, sizeof(ARAD_PP_IHP_TM_PORT_PARSER_PROGRAM_POINTER_CONFIG_TBL_DATA));

    SOCDNX_IF_ERR_EXIT(soc_mem_read(
          unit,
          IHP_PTC_PARSER_PROGRAM_POINTER_CONFIGm,
          IHP_BLOCK(unit, core),
          entry_offset,
          data
        ));

    tbl_data->parser_program_pointer_offset = soc_mem_field32_get(
                  unit,
                  IHP_PTC_PARSER_PROGRAM_POINTER_CONFIGm,
                  data,
                  OFFSETf);

    tbl_data->parser_program_pointer_value = soc_mem_field32_get(
                  unit,
                  IHP_PTC_PARSER_PROGRAM_POINTER_CONFIGm,
                  data,
                  VALUEf);

    tbl_data->parser_program_pointer_profile = soc_mem_field32_get(
                  unit,
                  IHP_PTC_PARSER_PROGRAM_POINTER_CONFIGm,
                  data,
                  PROFILEf);

    tbl_data->parser_program_pointer_value_to_use = soc_mem_field32_get(
                  unit,
                  IHP_PTC_PARSER_PROGRAM_POINTER_CONFIGm,
                  data,
                  VALUE_TO_USEf);

exit:
    SOCDNX_FUNC_RETURN;
}

int
  arad_pp_ihp_tm_port_parser_program_pointer_config_tbl_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                 entry_offset,
    SOC_SAND_IN  int                    core,
    SOC_SAND_IN  ARAD_PP_IHP_TM_PORT_PARSER_PROGRAM_POINTER_CONFIG_TBL_DATA  *tbl_data
  )
{
    uint32 data[ARAD_PP_IHP_TM_PORT_PARSER_PROGRAM_POINTER_CONFIG_TBL_ENTRY_SIZE];
 
    SOCDNX_INIT_FUNC_DEFS;

    sal_memset(data, 0, sizeof(uint32)*ARAD_PP_IHP_TM_PORT_PARSER_PROGRAM_POINTER_CONFIG_TBL_ENTRY_SIZE);

    soc_mem_field32_set(
          unit,
          IHP_PTC_PARSER_PROGRAM_POINTER_CONFIGm,
          data,
          OFFSETf,
          tbl_data->parser_program_pointer_offset);

    soc_mem_field32_set(
          unit,
          IHP_PTC_PARSER_PROGRAM_POINTER_CONFIGm,
          data,
          VALUEf,
          tbl_data->parser_program_pointer_value);

    soc_mem_field32_set(
          unit,
          IHP_PTC_PARSER_PROGRAM_POINTER_CONFIGm,
          data,
          PROFILEf,
          tbl_data->parser_program_pointer_profile);

    soc_mem_field32_set(
          unit,
          IHP_PTC_PARSER_PROGRAM_POINTER_CONFIGm,
          data,
          VALUE_TO_USEf,
          tbl_data->parser_program_pointer_value_to_use);

    SOCDNX_IF_ERR_EXIT(soc_mem_write(
          unit,
          IHP_PTC_PARSER_PROGRAM_POINTER_CONFIGm,
          IHP_BLOCK(unit, core),
          entry_offset,
          data
        ));

exit:
    SOCDNX_FUNC_RETURN;
}
uint32
  arad_pp_ihp_virtual_port_fem_bit_select_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_VIRTUAL_PORT_FEM_BIT_SELECT_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_VIRTUAL_PORT_FEM_BIT_SELECT_TABLE_TBL_ENTRY_SIZE];
 
 
 

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_VIRTUAL_PORT_FEM_BIT_SELECT_TABLE_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_VIRTUAL_PORT_FEM_BIT_SELECT_TABLE_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_VIRTUAL_PORT_FEM_BIT_SELECT_TABLE_TBL_DATA, 1);

 
 



  res = soc_mem_read(
          unit,
          IHP_VIRTUAL_PORT_FEM_BIT_SELECT_TABLEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

          tbl_data->virtual_port_fem_bit_select   = soc_mem_field32_get(
                  unit,
                  IHP_VIRTUAL_PORT_FEM_BIT_SELECT_TABLEm,
                  data,
                  VIRTUAL_PORT_FEM_BIT_SELECTf            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_virtual_port_fem_bit_select_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_virtual_port_fem_bit_select_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_VIRTUAL_PORT_FEM_BIT_SELECT_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_VIRTUAL_PORT_FEM_BIT_SELECT_TABLE_TBL_ENTRY_SIZE];
 
 
 

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_VIRTUAL_PORT_FEM_BIT_SELECT_TABLE_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_VIRTUAL_PORT_FEM_BIT_SELECT_TABLE_TBL_ENTRY_SIZE);

 
 



            soc_mem_field32_set(
          unit,
          IHP_VIRTUAL_PORT_FEM_BIT_SELECT_TABLEm,
          data,
          VIRTUAL_PORT_FEM_BIT_SELECTf,
          tbl_data->virtual_port_fem_bit_select );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_mem_write(
          unit,
          IHP_VIRTUAL_PORT_FEM_BIT_SELECT_TABLEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_virtual_port_fem_bit_select_table_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_virtual_port_fem_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              pgm_ndx,
   SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_VIRTUAL_PORT_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_VIRTUAL_PORT_FEM_MAP_INDEX_TABLE_TBL_ENTRY_SIZE];
 
 
 

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_VIRTUAL_PORT_FEM_MAP_INDEX_TABLE_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_VIRTUAL_PORT_FEM_MAP_INDEX_TABLE_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_VIRTUAL_PORT_FEM_MAP_INDEX_TABLE_TBL_DATA, 1);

 
 



  res = soc_mem_read(
          unit,
          IHP_VIRTUAL_PORT_FEM_MAP_INDEX_TABLEm,
          MEM_BLOCK_ANY,
          entry_offset + (pgm_ndx << ARAD_PP_IHP_FEM_SEL_BITS_SIZE_IN_BITS),
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

          tbl_data->virtual_port_fem_map_index   = soc_mem_field32_get(
                  unit,
                  IHP_VIRTUAL_PORT_FEM_MAP_INDEX_TABLEm,
                  data,
                  VIRTUAL_PORT_FEM_MAP_INDEXf            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

          tbl_data->virtual_port_fem_map_data   = soc_mem_field32_get(
                  unit,
                  IHP_VIRTUAL_PORT_FEM_MAP_INDEX_TABLEm,
                  data,
                  VIRTUAL_PORT_FEM_MAP_DATAf            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_virtual_port_fem_map_index_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_virtual_port_fem_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              pgm_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_VIRTUAL_PORT_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_VIRTUAL_PORT_FEM_MAP_INDEX_TABLE_TBL_ENTRY_SIZE];
 
 
 

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_VIRTUAL_PORT_FEM_MAP_INDEX_TABLE_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_VIRTUAL_PORT_FEM_MAP_INDEX_TABLE_TBL_ENTRY_SIZE);

 
 



            soc_mem_field32_set(
          unit,
          IHP_VIRTUAL_PORT_FEM_MAP_INDEX_TABLEm,
          data,
          VIRTUAL_PORT_FEM_MAP_INDEXf,
          tbl_data->virtual_port_fem_map_index );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

            soc_mem_field32_set(
          unit,
          IHP_VIRTUAL_PORT_FEM_MAP_INDEX_TABLEm,
          data,
          VIRTUAL_PORT_FEM_MAP_DATAf,
          tbl_data->virtual_port_fem_map_data );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_mem_write(
          unit,
          IHP_VIRTUAL_PORT_FEM_MAP_INDEX_TABLEm,
          MEM_BLOCK_ANY,
          entry_offset + (pgm_ndx << ARAD_PP_IHP_FEM_SEL_BITS_SIZE_IN_BITS),
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_virtual_port_fem_map_index_table_tbl_set_unsafe()", entry_offset, 0);
}

static
    soc_field_t
        Arad_virtual_port_fem_field_select_field[SOC_DPP_DEFS_MAX(VIRTUAL_PORT_NOF_BITS)] = {
            VIRTUAL_PORT_FEM_FIELD_SELECT_0f, VIRTUAL_PORT_FEM_FIELD_SELECT_1f, VIRTUAL_PORT_FEM_FIELD_SELECT_2f, 
            VIRTUAL_PORT_FEM_FIELD_SELECT_3f, VIRTUAL_PORT_FEM_FIELD_SELECT_4f, VIRTUAL_PORT_FEM_FIELD_SELECT_5f, 
            VIRTUAL_PORT_FEM_FIELD_SELECT_6f, VIRTUAL_PORT_FEM_FIELD_SELECT_7f, VIRTUAL_PORT_FEM_FIELD_SELECT_8f, 
            VIRTUAL_PORT_FEM_FIELD_SELECT_9f, VIRTUAL_PORT_FEM_FIELD_SELECT_10f, VIRTUAL_PORT_FEM_FIELD_SELECT_11f, 
            VIRTUAL_PORT_FEM_FIELD_SELECT_12f, VIRTUAL_PORT_FEM_FIELD_SELECT_13f
#ifdef BCM_JERICHO_SUPPORT
          , VIRTUAL_PORT_FEM_FIELD_SELECT_14f
#endif /* BCM_JERICHO_SUPPORT */
        };
static
    soc_field_t
        Arad_src_system_port_fem_field_select_field[ARAD_PORTS_FEM_SYSTEM_PORT_SIZE] = {
            SRC_SYSTEM_PORT_FEM_FIELD_SELECT_0f, SRC_SYSTEM_PORT_FEM_FIELD_SELECT_1f, SRC_SYSTEM_PORT_FEM_FIELD_SELECT_2f, 
            SRC_SYSTEM_PORT_FEM_FIELD_SELECT_3f, SRC_SYSTEM_PORT_FEM_FIELD_SELECT_4f, SRC_SYSTEM_PORT_FEM_FIELD_SELECT_5f, 
            SRC_SYSTEM_PORT_FEM_FIELD_SELECT_6f, SRC_SYSTEM_PORT_FEM_FIELD_SELECT_7f, SRC_SYSTEM_PORT_FEM_FIELD_SELECT_8f, 
            SRC_SYSTEM_PORT_FEM_FIELD_SELECT_9f, SRC_SYSTEM_PORT_FEM_FIELD_SELECT_10f, SRC_SYSTEM_PORT_FEM_FIELD_SELECT_11f, 
            SRC_SYSTEM_PORT_FEM_FIELD_SELECT_12f, SRC_SYSTEM_PORT_FEM_FIELD_SELECT_13f, SRC_SYSTEM_PORT_FEM_FIELD_SELECT_14f, 
            SRC_SYSTEM_PORT_FEM_FIELD_SELECT_15f
        };

static
    soc_field_t
        Arad_parser_program_pointer_fem_field_select_field[ARAD_PORTS_FEM_PARSER_PROGRAM_POINTER_SIZE] = {
            PARSER_PROGRAM_POINTER_FEM_FIELD_SELECT_0f, PARSER_PROGRAM_POINTER_FEM_FIELD_SELECT_1f, 
            PARSER_PROGRAM_POINTER_FEM_FIELD_SELECT_2f, PARSER_PROGRAM_POINTER_FEM_FIELD_SELECT_3f, 
        };

uint32
  arad_pp_ihp_virtual_port_fem_field_select_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_VIRTUAL_PORT_FEM_FIELD_SELECT_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_VIRTUAL_PORT_FEM_FIELD_SELECT_MAP_TBL_ENTRY_SIZE];
  uint32
    field_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_VIRTUAL_PORT_FEM_FIELD_SELECT_MAP_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_VIRTUAL_PORT_FEM_FIELD_SELECT_MAP_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_VIRTUAL_PORT_FEM_FIELD_SELECT_MAP_TBL_DATA, 1);

 
 



  res = soc_mem_read(
          unit,
          IHP_VIRTUAL_PORT_FEM_FIELD_SELECT_MAPm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  for (field_ndx = 0; field_ndx < SOC_DPP_DEFS_GET(unit, virtual_port_nof_bits); ++field_ndx)
  {
            tbl_data->virtual_port_fem_field_select[field_ndx]     = soc_mem_field32_get(
                  unit,
                  IHP_VIRTUAL_PORT_FEM_FIELD_SELECT_MAPm,
                    data,
                  Arad_virtual_port_fem_field_select_field[field_ndx]            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40 + field_ndx, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_virtual_port_fem_field_select_map_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_virtual_port_fem_field_select_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_VIRTUAL_PORT_FEM_FIELD_SELECT_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_VIRTUAL_PORT_FEM_FIELD_SELECT_MAP_TBL_ENTRY_SIZE];
  uint32
    field_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_VIRTUAL_PORT_FEM_FIELD_SELECT_MAP_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_VIRTUAL_PORT_FEM_FIELD_SELECT_MAP_TBL_ENTRY_SIZE);

 
 



  for (field_ndx = 0; field_ndx < SOC_DPP_DEFS_GET(unit, virtual_port_nof_bits); ++field_ndx)
  {
            soc_mem_field32_set(
          unit,
          IHP_VIRTUAL_PORT_FEM_FIELD_SELECT_MAPm,
            data,
          Arad_virtual_port_fem_field_select_field[field_ndx],
            tbl_data->virtual_port_fem_field_select[field_ndx] );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30 + field_ndx, exit);
  }

  res = soc_mem_write(
          unit,
          IHP_VIRTUAL_PORT_FEM_FIELD_SELECT_MAPm,
          MEM_BLOCK_ANY,
          entry_offset,
            data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 36, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_virtual_port_fem_field_select_map_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_src_system_port_fem_field_select_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_ENTRY_SIZE];
  uint32
    field_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_DATA, 1);

 
 



  res = soc_mem_read(
          unit,
          IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAPm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  for (field_ndx = 0; field_ndx < ARAD_PORTS_FEM_SYSTEM_PORT_SIZE; ++field_ndx)
  {
            tbl_data->src_system_port_fem_field_select[field_ndx]     = soc_mem_field32_get(
                  unit,
                  IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAPm,
                    data,
                  Arad_src_system_port_fem_field_select_field[field_ndx]            );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40 + field_ndx, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_src_system_port_fem_field_select_map_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_src_system_port_fem_field_select_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_ENTRY_SIZE];
  uint32
    field_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_ENTRY_SIZE);

 
 



  for (field_ndx = 0; field_ndx < ARAD_PORTS_FEM_SYSTEM_PORT_SIZE; ++field_ndx)
  {
            soc_mem_field32_set(
          unit,
          IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAPm,
            data,
          Arad_src_system_port_fem_field_select_field[field_ndx],
            tbl_data->src_system_port_fem_field_select[field_ndx] );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30 + field_ndx, exit);
  }


  res = soc_mem_write(
          unit,
          IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAPm,
          MEM_BLOCK_ANY,
          entry_offset,
            data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 43, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_src_system_port_fem_field_select_map_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_parser_program_pointer_fem_bit_select_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_BIT_SELECT_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_BIT_SELECT_TABLE_TBL_ENTRY_SIZE];
 
 
 

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_BIT_SELECT_TABLE_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_BIT_SELECT_TABLE_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_BIT_SELECT_TABLE_TBL_DATA, 1);

 
 



  res = soc_mem_read(
          unit,
          IHP_PARSER_PROGRAM_POINTER_FEM_BIT_SELECT_TABLEm,
          MEM_BLOCK_ANY,
          entry_offset,
            data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

          tbl_data->parser_program_pointer_fem_bit_select   = soc_mem_field32_get(
                  unit,
                  IHP_PARSER_PROGRAM_POINTER_FEM_BIT_SELECT_TABLEm,
                  data,
                  PARSER_PROGRAM_POINTER_FEM_BIT_SELECTf            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_parser_program_pointer_fem_bit_select_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_parser_program_pointer_fem_bit_select_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_BIT_SELECT_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_BIT_SELECT_TABLE_TBL_ENTRY_SIZE];
 
 
 

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_BIT_SELECT_TABLE_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_BIT_SELECT_TABLE_TBL_ENTRY_SIZE);

 
 



            soc_mem_field32_set(
          unit,
          IHP_PARSER_PROGRAM_POINTER_FEM_BIT_SELECT_TABLEm,
          data,
          PARSER_PROGRAM_POINTER_FEM_BIT_SELECTf,
          tbl_data->parser_program_pointer_fem_bit_select );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_mem_write(
          unit,
          IHP_PARSER_PROGRAM_POINTER_FEM_BIT_SELECT_TABLEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_parser_program_pointer_fem_bit_select_table_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_parser_program_pointer_fem_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              pgm_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_MAP_INDEX_TABLE_TBL_ENTRY_SIZE];
 
 
 

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_MAP_INDEX_TABLE_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_MAP_INDEX_TABLE_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_MAP_INDEX_TABLE_TBL_DATA, 1);

 
 



  res = soc_mem_read(
          unit,
          IHP_PARSER_PROGRAM_POINTER_FEM_MAP_INDEX_TABLEm,
          MEM_BLOCK_ANY,
          entry_offset + (pgm_ndx << ARAD_PP_IHP_FEM_SEL_BITS_SIZE_IN_BITS),
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

          tbl_data->parser_program_pointer_fem_map_index   = soc_mem_field32_get(
                  unit,
                  IHP_PARSER_PROGRAM_POINTER_FEM_MAP_INDEX_TABLEm,
                  data,
                  PARSER_PROGRAM_POINTER_FEM_MAP_INDEXf            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

          tbl_data->parser_program_pointer_fem_map_data   = soc_mem_field32_get(
                  unit,
                  IHP_PARSER_PROGRAM_POINTER_FEM_MAP_INDEX_TABLEm,
                  data,
                  PARSER_PROGRAM_POINTER_FEM_MAP_DATAf            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_parser_program_pointer_fem_map_index_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_parser_program_pointer_fem_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              pgm_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_MAP_INDEX_TABLE_TBL_ENTRY_SIZE];
 
 
 

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_MAP_INDEX_TABLE_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_MAP_INDEX_TABLE_TBL_ENTRY_SIZE);

 
 



            soc_mem_field32_set(
          unit,
          IHP_PARSER_PROGRAM_POINTER_FEM_MAP_INDEX_TABLEm,
          data,
          PARSER_PROGRAM_POINTER_FEM_MAP_INDEXf,
          tbl_data->parser_program_pointer_fem_map_index );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

            soc_mem_field32_set(
          unit,
          IHP_PARSER_PROGRAM_POINTER_FEM_MAP_INDEX_TABLEm,
          data,
          PARSER_PROGRAM_POINTER_FEM_MAP_DATAf,
          tbl_data->parser_program_pointer_fem_map_data );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_mem_write(
          unit,
          IHP_PARSER_PROGRAM_POINTER_FEM_MAP_INDEX_TABLEm,
          MEM_BLOCK_ANY,
          entry_offset + (pgm_ndx << ARAD_PP_IHP_FEM_SEL_BITS_SIZE_IN_BITS),
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_parser_program_pointer_fem_map_index_table_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_parser_program_pointer_fem_field_select_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_FIELD_SELECT_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
      field_ndx,
    data[ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_FIELD_SELECT_MAP_TBL_ENTRY_SIZE];
 
 
 

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_FIELD_SELECT_MAP_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_FIELD_SELECT_MAP_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_FIELD_SELECT_MAP_TBL_DATA, 1);

 
 



  res = soc_mem_read(
          unit,
          IHP_PARSER_PROGRAM_POINTER_FEM_FIELD_SELECT_MAPm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  for (field_ndx = 0; field_ndx < ARAD_PORTS_FEM_PARSER_PROGRAM_POINTER_SIZE; ++field_ndx)
  {
      tbl_data->parser_program_pointer_fem_field_select[field_ndx]   = soc_mem_field32_get(
            unit,
            IHP_PARSER_PROGRAM_POINTER_FEM_FIELD_SELECT_MAPm,
              data,
              Arad_parser_program_pointer_fem_field_select_field[field_ndx]            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_parser_program_pointer_fem_field_select_map_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_parser_program_pointer_fem_field_select_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_FIELD_SELECT_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
      field_ndx,
    data[ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_FIELD_SELECT_MAP_TBL_ENTRY_SIZE];
 
 
 

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_FIELD_SELECT_MAP_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_FIELD_SELECT_MAP_TBL_ENTRY_SIZE);

 
 


  for (field_ndx = 0; field_ndx < ARAD_PORTS_FEM_PARSER_PROGRAM_POINTER_SIZE; ++field_ndx)
  {
      soc_mem_field32_set(
            unit,
            IHP_PARSER_PROGRAM_POINTER_FEM_FIELD_SELECT_MAPm,
            data,
            Arad_parser_program_pointer_fem_field_select_field[field_ndx],
            tbl_data->parser_program_pointer_fem_field_select[field_ndx] );
     SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

  res = soc_mem_write(
          unit,
          IHP_PARSER_PROGRAM_POINTER_FEM_FIELD_SELECT_MAPm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 36, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_parser_program_pointer_fem_field_select_map_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_parser_program_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              array_index,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_PARSER_PROGRAM_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_PARSER_PROGRAM_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_PARSER_PROGRAM_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_PARSER_PROGRAM_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_PARSER_PROGRAM_TBL_DATA, 1); 

  res = soc_mem_array_read(
          unit,
          IHP_PARSER_PROGRAMm,
          array_index,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

          tbl_data->next_addr_base   = soc_mem_field32_get(
                  unit,
                  IHP_PARSER_PROGRAMm,
                  data,
                  NEXT_ADDR_BASEf            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

          tbl_data->macro_sel   = soc_mem_field32_get(
                  unit,
                  IHP_PARSER_PROGRAMm,
                  data,
                  MACRO_SELf            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_parser_program_tbl_get_unsafe()", array_index, entry_offset);
}

uint32
  arad_pp_ihp_parser_program_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              array_index,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_PARSER_PROGRAM_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_PARSER_PROGRAM_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_PARSER_PROGRAM_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_PARSER_PROGRAM_TBL_ENTRY_SIZE);

    soc_mem_field32_set(
          unit,
          IHP_PARSER_PROGRAMm,
          data,
          NEXT_ADDR_BASEf,
          tbl_data->next_addr_base );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

            soc_mem_field32_set(
          unit,
          IHP_PARSER_PROGRAMm,
          data,
          MACRO_SELf,
          tbl_data->macro_sel );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_mem_array_write(
          unit,
          IHP_PARSER_PROGRAMm,
          array_index,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_parser_program_tbl_set_unsafe()", array_index, entry_offset);
}

uint32
  arad_pp_ihp_lif_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_LIF_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK,
    entry_offset_in_bank,
    bank_id;
  soc_reg_above_64_val_t
    data;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_LIF_TABLE_TBL_GET_UNSAFE);

  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_LIF_TABLE_TBL_DATA, 1);
  SOC_REG_ABOVE_64_CLEAR(data);

  bank_id = LIF_TABLE_ENTRY_OFFSET_TO_BANK_ID(unit, entry_offset);
  entry_offset_in_bank = LIF_TABLE_ENTRY_OFFSET_TO_OFFSET_IN_BANK(unit, entry_offset);

  res = READ_PPDB_B_LIF_TABLEm(unit, bank_id, MEM_BLOCK_ANY, entry_offset_in_bank, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  tbl_data->type = soc_mem_field32_get(unit, PPDB_B_LIF_TABLEm, data, TYPEf);

  soc_mem_field_get(unit, PPDB_B_LIF_TABLEm, data, LIF_TABLE_ENTRYf, tbl_data->lif_table_entry);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_sem_result_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_lif_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_LIF_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK,
    entry_offset_in_bank,
    bank_id;
  soc_reg_above_64_val_t
    data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_LIF_TABLE_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_SEM_RESULT_TABLE_TBL_ENTRY_SIZE);
  SOC_REG_ABOVE_64_CLEAR(data);

  soc_mem_field32_set(unit, PPDB_B_LIF_TABLEm, data, TYPEf, tbl_data->type);
  soc_mem_field_set(unit, PPDB_B_LIF_TABLEm, data, LIF_TABLE_ENTRYf, (uint32*)tbl_data->lif_table_entry);

  bank_id = LIF_TABLE_ENTRY_OFFSET_TO_BANK_ID(unit, entry_offset);
  entry_offset_in_bank = LIF_TABLE_ENTRY_OFFSET_TO_OFFSET_IN_BANK(unit, entry_offset);

  res = WRITE_PPDB_B_LIF_TABLEm(unit, bank_id, MEM_BLOCK_ANY, entry_offset_in_bank, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_sem_result_table_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_vtt_pp_port_vsi_profiles_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_VTT_PP_PORT_VSI_PROFILES_TBL_DATA  *tbl_data
  )
{
   uint32
    res = SOC_SAND_OK,
    data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_VTT_PP_PORT_VSI_PROFILES_TBL_GET_UNSAFE);

  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_VTT_PP_PORT_VSI_PROFILES_TBL_DATA, 1);

  res = READ_IHP_VTT_PP_PORT_VSI_PROFILESm(unit, MEM_BLOCK_ANY, entry_offset, &data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 11, exit);

  tbl_data->ac_p2p_to_ac_vsi_profile = soc_mem_field32_get(unit, IHP_VTT_PP_PORT_VSI_PROFILESm, &data, AC_P2P_TO_AC_VSI_PROFILEf);
  tbl_data->ac_p2p_to_pwe_vsi_profile = soc_mem_field32_get(unit, IHP_VTT_PP_PORT_VSI_PROFILESm, &data, AC_P2P_TO_PWE_VSI_PROFILEf);
  tbl_data->ac_p2p_to_pbb_vsi_profile = soc_mem_field32_get(unit, IHP_VTT_PP_PORT_VSI_PROFILESm, &data, AC_P2P_TO_PBB_VSI_PROFILEf);
  tbl_data->label_pwe_p2p_vsi_profile = soc_mem_field32_get(unit, IHP_VTT_PP_PORT_VSI_PROFILESm, &data, LABEL_PWEP2P_VSI_PROFILEf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_vtt_pp_port_vsi_profiles_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_vtt_pp_port_vsi_profiles_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32             entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_VTT_PP_PORT_VSI_PROFILES_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK,
    data = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_VTT_PP_PORT_VSI_PROFILES_TBL_SET_UNSAFE);

  soc_mem_field32_set(unit, IHP_VTT_PP_PORT_VSI_PROFILESm, &data, AC_P2P_TO_AC_VSI_PROFILEf, tbl_data->ac_p2p_to_ac_vsi_profile);
  soc_mem_field32_set(unit, IHP_VTT_PP_PORT_VSI_PROFILESm, &data, AC_P2P_TO_PWE_VSI_PROFILEf, tbl_data->ac_p2p_to_pwe_vsi_profile);
  soc_mem_field32_set(unit, IHP_VTT_PP_PORT_VSI_PROFILESm, &data, AC_P2P_TO_PBB_VSI_PROFILEf, tbl_data->ac_p2p_to_pbb_vsi_profile);
  soc_mem_field32_set(unit, IHP_VTT_PP_PORT_VSI_PROFILESm, &data, LABEL_PWEP2P_VSI_PROFILEf, tbl_data->label_pwe_p2p_vsi_profile);

  res = WRITE_IHP_VTT_PP_PORT_VSI_PROFILESm(unit, MEM_BLOCK_ANY, entry_offset, &data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 22, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_vtt_pp_port_vsi_profiles_tbl_set_unsafe()", entry_offset, 0);
}


uint32
  arad_pp_ihp_stp_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_STP_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32 res = SOC_SAND_OK;
  uint32 data[SOC_DPP_DEFS_MAX(IHP_STP_TABLE_NOF_LONGS)];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_STP_TABLE_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, SOC_DPP_DEFS_MAX(IHP_STP_TABLE_NOF_LONGS));
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_STP_TABLE_TBL_DATA, 1);

  res = READ_IHP_STP_TABLEm(unit, MEM_BLOCK_ANY, entry_offset, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
  soc_mem_field_get(unit, IHP_STP_TABLEm, data, PER_PORT_STP_STATEf, tbl_data->per_port_stp_state);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_stp_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_stp_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_STP_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32 res = SOC_SAND_OK;
  uint32 data[SOC_DPP_DEFS_MAX(IHP_STP_TABLE_NOF_LONGS)];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_STP_TABLE_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, SOC_DPP_DEFS_MAX(IHP_STP_TABLE_NOF_LONGS));
 
  /* the casting below is because soc_mem_field_set mistakenly does not define its constant input as const */
  soc_mem_field_set(unit, IHP_STP_TABLEm, data, PER_PORT_STP_STATEf, (uint32*)(tbl_data->per_port_stp_state));
  res = WRITE_IHP_STP_TABLEm(unit, MEM_BLOCK_ALL, entry_offset, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_stp_table_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_vrid_my_mac_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_VRID_MY_MAC_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_VRID_MY_MAC_MAP_TBL_GET_UNSAFE);

  SOC_REG_ABOVE_64_CLEAR(tbl_data->vrid_my_mac_map);

  res = READ_IHP_VRID_MY_MAC_MAPm(unit, MEM_BLOCK_ANY, entry_offset, tbl_data->vrid_my_mac_map);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_vrid_my_mac_map_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_vrid_my_mac_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_VRID_MY_MAC_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_VRID_MY_MAC_MAP_TBL_SET_UNSAFE);

  res = WRITE_IHP_VRID_MY_MAC_MAPm(unit, MEM_BLOCK_ANY, entry_offset, (void*)tbl_data->vrid_my_mac_map);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_vrid_my_mac_map_tbl_set_unsafe()", entry_offset, 0);
}
uint32
  arad_pp_ihp_isem_1st_program_selection_cam_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_ISEM_1ST_PROGRAM_SELECTION_CAM_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHP_VTT_1ST_KEY_PROG_SEL_TCAMm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->in_pp_port_vt_profile = soc_mem_field32_get(unit, IHP_VTT_1ST_KEY_PROG_SEL_TCAMm, data, IN_PP_PORT_VT_PROFILEf);         
  tbl_data->in_pp_port_vt_profile_mask = soc_mem_field32_get(unit, IHP_VTT_1ST_KEY_PROG_SEL_TCAMm, data, IN_PP_PORT_VT_PROFILE_MASKf);    
  tbl_data->packet_format_code = soc_mem_field32_get(unit, IHP_VTT_1ST_KEY_PROG_SEL_TCAMm, data, PACKET_FORMAT_CODEf);            
  tbl_data->packet_format_code_mask = soc_mem_field32_get(unit, IHP_VTT_1ST_KEY_PROG_SEL_TCAMm, data, PACKET_FORMAT_CODE_MASKf);       
  tbl_data->packet_format_qualifier_1 = soc_mem_field32_get(unit, IHP_VTT_1ST_KEY_PROG_SEL_TCAMm, data, PACKET_FORMAT_QUALIFIER_1f);     
  tbl_data->packet_format_qualifier_1_mask = soc_mem_field32_get(unit, IHP_VTT_1ST_KEY_PROG_SEL_TCAMm, data, PACKET_FORMAT_QUALIFIER_1_MASKf);
  tbl_data->packet_format_qualifier_2 = soc_mem_field32_get(unit, IHP_VTT_1ST_KEY_PROG_SEL_TCAMm, data, PACKET_FORMAT_QUALIFIER_2f);     
  tbl_data->packet_format_qualifier_2_mask = soc_mem_field32_get(unit, IHP_VTT_1ST_KEY_PROG_SEL_TCAMm, data, PACKET_FORMAT_QUALIFIER_2_MASKf);
  tbl_data->parser_leaf_context = soc_mem_field32_get(unit, IHP_VTT_1ST_KEY_PROG_SEL_TCAMm, data, PARSER_LEAF_CONTEXTf);           
  tbl_data->parser_leaf_context_mask = soc_mem_field32_get(unit, IHP_VTT_1ST_KEY_PROG_SEL_TCAMm, data, PARSER_LEAF_CONTEXT_MASKf);      
  tbl_data->ptc_vt_profile = soc_mem_field32_get(unit, IHP_VTT_1ST_KEY_PROG_SEL_TCAMm, data, PTC_VT_PROFILEf);                
  tbl_data->ptc_vt_profile_mask = soc_mem_field32_get(unit, IHP_VTT_1ST_KEY_PROG_SEL_TCAMm, data, PTC_VT_PROFILE_MASKf);           
  tbl_data->valid = soc_mem_field32_get(unit, IHP_VTT_1ST_KEY_PROG_SEL_TCAMm, data, VALIDf);                         
  tbl_data->llvp_prog_sel = soc_mem_field32_get(unit, IHP_VTT_1ST_KEY_PROG_SEL_TCAMm, data, PROGRAMf);    
  if (SOC_IS_JERICHO(unit)) {
      tbl_data->incoming_tag_structure_mask = soc_mem_field32_get(unit, IHP_VTT_1ST_KEY_PROG_SEL_TCAMm, data, INCOMING_TAG_STRUCTURE_MASKf);
      tbl_data->packet_format_qualifier_0_mask = soc_mem_field32_get(unit, IHP_VTT_1ST_KEY_PROG_SEL_TCAMm, data, PACKET_FORMAT_QUALIFIER_0_MASKf);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_isem_1st_program_selection_cam_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_isem_1st_program_selection_convert_tbl_data_to_data(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA *tbl_data,
    SOC_SAND_OUT uint32 *data
  )
{
  uint32
    res = SOC_SAND_OK;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHP_VTT_1ST_KEY_PROG_SEL_TCAMm, data, IN_PP_PORT_VT_PROFILEf, tbl_data->in_pp_port_vt_profile);
  soc_mem_field32_set(unit, IHP_VTT_1ST_KEY_PROG_SEL_TCAMm, data, IN_PP_PORT_VT_PROFILE_MASKf, tbl_data->in_pp_port_vt_profile_mask);
  soc_mem_field32_set(unit, IHP_VTT_1ST_KEY_PROG_SEL_TCAMm, data, PACKET_FORMAT_CODEf, tbl_data->packet_format_code);
  soc_mem_field32_set(unit, IHP_VTT_1ST_KEY_PROG_SEL_TCAMm, data, PACKET_FORMAT_CODE_MASKf, tbl_data->packet_format_code_mask);
  soc_mem_field32_set(unit, IHP_VTT_1ST_KEY_PROG_SEL_TCAMm, data, PACKET_FORMAT_QUALIFIER_1f, tbl_data->packet_format_qualifier_1);
  soc_mem_field32_set(unit, IHP_VTT_1ST_KEY_PROG_SEL_TCAMm, data, PACKET_FORMAT_QUALIFIER_1_MASKf, tbl_data->packet_format_qualifier_1_mask);
  soc_mem_field32_set(unit, IHP_VTT_1ST_KEY_PROG_SEL_TCAMm, data, PACKET_FORMAT_QUALIFIER_2f, tbl_data->packet_format_qualifier_2);
  soc_mem_field32_set(unit, IHP_VTT_1ST_KEY_PROG_SEL_TCAMm, data, PACKET_FORMAT_QUALIFIER_2_MASKf, tbl_data->packet_format_qualifier_2_mask);
  soc_mem_field32_set(unit, IHP_VTT_1ST_KEY_PROG_SEL_TCAMm, data, PARSER_LEAF_CONTEXTf, tbl_data->parser_leaf_context);
  soc_mem_field32_set(unit, IHP_VTT_1ST_KEY_PROG_SEL_TCAMm, data, PARSER_LEAF_CONTEXT_MASKf, tbl_data->parser_leaf_context_mask);
  soc_mem_field32_set(unit, IHP_VTT_1ST_KEY_PROG_SEL_TCAMm, data, PTC_VT_PROFILEf, tbl_data->ptc_vt_profile);
  soc_mem_field32_set(unit, IHP_VTT_1ST_KEY_PROG_SEL_TCAMm, data, PTC_VT_PROFILE_MASKf, tbl_data->ptc_vt_profile_mask);
  soc_mem_field32_set(unit, IHP_VTT_1ST_KEY_PROG_SEL_TCAMm, data, VALIDf, tbl_data->valid);
  soc_mem_field32_set(unit, IHP_VTT_1ST_KEY_PROG_SEL_TCAMm, data, PROGRAMf, tbl_data->llvp_prog_sel);
  soc_mem_field32_set(unit, IHP_VTT_1ST_KEY_PROG_SEL_TCAMm, data, VALIDf, tbl_data->valid);
  if (SOC_IS_JERICHO(unit)) {
      soc_mem_field32_set(unit, IHP_VTT_1ST_KEY_PROG_SEL_TCAMm, data, INCOMING_TAG_STRUCTURE_MASKf, tbl_data->incoming_tag_structure_mask);
      soc_mem_field32_set(unit, IHP_VTT_1ST_KEY_PROG_SEL_TCAMm, data, PACKET_FORMAT_QUALIFIER_0_MASKf, tbl_data->packet_format_qualifier_0_mask);
  }
  ARAD_DO_NOTHING_AND_EXIT; 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_isem_1st_program_selection_convert_tbl_data_to_data()", 0, 0);
}

uint32
  arad_pp_ihp_isem_1st_program_selection_cam_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_VTT1ST_KEY_PROGRAM_SELECTION_CAM_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_ISEM_1ST_PROGRAM_SELECTION_CAM_TBL_SET_UNSAFE);

  res = arad_pp_ihp_isem_1st_program_selection_convert_tbl_data_to_data(
          unit,
          tbl_data,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_mem_write(
          unit,
          IHP_VTT_1ST_KEY_PROG_SEL_TCAMm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_isem_1st_program_selection_cam_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_isem_2nd_program_selection_cam_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_ISEM_2ND_PROGRAM_SELECTION_CAM_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHP_VTT_2ND_KEY_PROG_SEL_TCAMm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->in_pp_port_tt_profile = soc_mem_field32_get(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data,IN_PP_PORT_TT_PROFILEf);
  tbl_data->in_pp_port_tt_profile_mask = soc_mem_field32_get(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data,IN_PP_PORT_TT_PROFILE_MASKf);
  tbl_data->packet_format_code = soc_mem_field32_get(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data,PACKET_FORMAT_CODEf);
  tbl_data->packet_format_code_mask = soc_mem_field32_get(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data,PACKET_FORMAT_CODE_MASKf);
  tbl_data->packet_format_qualifier_1 = soc_mem_field32_get(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data,PACKET_FORMAT_QUALIFIER_1f);
  tbl_data->packet_format_qualifier_1_mask = soc_mem_field32_get(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data,PACKET_FORMAT_QUALIFIER_1_MASKf);
  tbl_data->packet_format_qualifier_2 = soc_mem_field32_get(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data, PACKET_FORMAT_QUALIFIER_2f);
  tbl_data->packet_format_qualifier_2_mask = soc_mem_field32_get(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data,PACKET_FORMAT_QUALIFIER_2_MASKf);
  if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      tbl_data->packet_format_qualifier_3_outer_vid_bits = soc_mem_field32_get(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data,PACKET_FORMAT_QUALIFIER_3_OUTER_VID_BITSf);
      tbl_data->packet_format_qualifier_3_outer_vid_bits_mask = soc_mem_field32_get(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data, PACKET_FORMAT_QUALIFIER_3_OUTER_VID_BITS_MASKf);
  }
  tbl_data->my_mac = soc_mem_field32_get(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data,MY_MACf);
  tbl_data->my_mac_mask = soc_mem_field32_get(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data, MY_MAC_MASKf);
  if(SOC_IS_ARAD_B0_AND_ABOVE(unit))
  {
      tbl_data->my_b_mac_mc = soc_mem_field32_get(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data, MY_B_MAC_MCf);
      tbl_data->my_b_mac_mc_mask = soc_mem_field32_get(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data, MY_B_MAC_MC_MASKf);
  }
  tbl_data->parser_leaf_context = soc_mem_field32_get(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data,PARSER_LEAF_CONTEXTf);
  tbl_data->parser_leaf_context_mask = soc_mem_field32_get(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data,PARSER_LEAF_CONTEXT_MASKf);
  tbl_data->ptc_tt_profile = soc_mem_field32_get(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data,PTC_TT_PROFILEf);
  tbl_data->ptc_tt_profile_mask = soc_mem_field32_get(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data,PTC_TT_PROFILE_MASKf);
  tbl_data->da_is_all_r_bridges = soc_mem_field32_get(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data,DA_IS_ALL_R_BRIDGESf);
  tbl_data->da_is_all_r_bridges_mask = soc_mem_field32_get(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data,DA_IS_ALL_R_BRIDGES_MASKf);
  tbl_data->isa_lookup_found = soc_mem_field32_get(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data,ISA_LOOKUP_FOUNDf);
  tbl_data->isa_lookup_found_mask = soc_mem_field32_get(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data,ISA_LOOKUP_FOUND_MASKf);
  tbl_data->isb_lookup_found = soc_mem_field32_get(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data,ISB_LOOKUP_FOUNDf);
  tbl_data->isb_lookup_found_mask = soc_mem_field32_get(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data,ISB_LOOKUP_FOUND_MASKf);
  tbl_data->tcam_lookup_match = soc_mem_field32_get(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data,TCAM_LOOKUP_MATCHf);
  tbl_data->tcam_lookup_match_mask = soc_mem_field32_get(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data,TCAM_LOOKUP_MATCH_MASKf);
  tbl_data->packet_is_compatible_mc = soc_mem_field32_get(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data,PACKET_IS_COMPATIBLE_MCf);
  tbl_data->packet_is_compatible_mc_mask = soc_mem_field32_get(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data,PACKET_IS_COMPATIBLE_MC_MASKf);
  tbl_data->vlan_is_designated = soc_mem_field32_get(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data,VLAN_IS_DESIGNATEDf);
  tbl_data->vlan_is_designated_mask = soc_mem_field32_get(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data,VLAN_IS_DESIGNATED_MASKf);
  tbl_data->vt_processing_profile = soc_mem_field32_get(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data,VT_PROCESSING_PROFILEf);
  tbl_data->vt_processing_profile_mask = soc_mem_field32_get(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data,VT_PROCESSING_PROFILE_MASKf);
  tbl_data->valid = soc_mem_field32_get(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data,VALIDf);
  tbl_data->llvp_prog_sel = soc_mem_field32_get(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data,PROGRAMf);
  if (SOC_IS_JERICHO(unit)) {
      tbl_data->vt_in_lif_profile_mask = soc_mem_field32_get(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data, VT_IN_LIF_PROFILE_MASKf);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_isem_2nd_program_selection_cam_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_isem_2nd_program_selection_convert_tbl_data_to_data(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA *tbl_data,
    SOC_SAND_OUT uint32 *data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data, IN_PP_PORT_TT_PROFILEf,tbl_data->in_pp_port_tt_profile);
  soc_mem_field32_set(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data, IN_PP_PORT_TT_PROFILE_MASKf, tbl_data->in_pp_port_tt_profile_mask);
  soc_mem_field32_set(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data, PACKET_FORMAT_CODEf, tbl_data->packet_format_code);
  soc_mem_field32_set(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data, PACKET_FORMAT_CODE_MASKf, tbl_data->packet_format_code_mask);
  soc_mem_field32_set(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data, PACKET_FORMAT_QUALIFIER_1f, tbl_data->packet_format_qualifier_1);
  soc_mem_field32_set(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data, PACKET_FORMAT_QUALIFIER_1_MASKf, tbl_data->packet_format_qualifier_1_mask);
  soc_mem_field32_set(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data, PACKET_FORMAT_QUALIFIER_2f, tbl_data->packet_format_qualifier_2);
  soc_mem_field32_set(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data, PACKET_FORMAT_QUALIFIER_2_MASKf, tbl_data->packet_format_qualifier_2_mask);
  if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      soc_mem_field32_set(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data, PACKET_FORMAT_QUALIFIER_3_OUTER_VID_BITSf, tbl_data->packet_format_qualifier_3_outer_vid_bits);
      soc_mem_field32_set(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data, PACKET_FORMAT_QUALIFIER_3_OUTER_VID_BITS_MASKf, tbl_data->packet_format_qualifier_3_outer_vid_bits_mask);
  }
  soc_mem_field32_set(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data, MY_MACf, tbl_data->my_mac);
  soc_mem_field32_set(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data, MY_MAC_MASKf, tbl_data->my_mac_mask);
  if(SOC_IS_ARAD_B0_AND_ABOVE(unit))
  {
      soc_mem_field32_set(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data, MY_B_MAC_MCf, tbl_data->my_b_mac_mc);
      soc_mem_field32_set(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data, MY_B_MAC_MC_MASKf, tbl_data->my_b_mac_mc_mask);
  }
  soc_mem_field32_set(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data, PARSER_LEAF_CONTEXTf, tbl_data->parser_leaf_context);
  soc_mem_field32_set(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data, PARSER_LEAF_CONTEXT_MASKf, tbl_data->parser_leaf_context_mask);
  soc_mem_field32_set(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data, PTC_TT_PROFILEf, tbl_data->ptc_tt_profile);
  soc_mem_field32_set(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data, PTC_TT_PROFILE_MASKf, tbl_data->ptc_tt_profile_mask );
  soc_mem_field32_set(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data, DA_IS_ALL_R_BRIDGESf, tbl_data->da_is_all_r_bridges );
  soc_mem_field32_set(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data, DA_IS_ALL_R_BRIDGES_MASKf, tbl_data->da_is_all_r_bridges_mask);
  soc_mem_field32_set(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data, ISA_LOOKUP_FOUNDf, tbl_data->isa_lookup_found);
  soc_mem_field32_set(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data, ISA_LOOKUP_FOUND_MASKf, tbl_data->isa_lookup_found_mask);
  soc_mem_field32_set(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data, ISB_LOOKUP_FOUNDf, tbl_data->isb_lookup_found);
  soc_mem_field32_set(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data, ISB_LOOKUP_FOUND_MASKf, tbl_data->isb_lookup_found_mask);
  soc_mem_field32_set(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data, TCAM_LOOKUP_MATCHf, tbl_data->tcam_lookup_match);
  soc_mem_field32_set(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data, TCAM_LOOKUP_MATCH_MASKf, tbl_data->tcam_lookup_match_mask);
  soc_mem_field32_set(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data, PACKET_IS_COMPATIBLE_MCf, tbl_data->packet_is_compatible_mc);
  soc_mem_field32_set(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data, PACKET_IS_COMPATIBLE_MC_MASKf, tbl_data->packet_is_compatible_mc_mask);
  soc_mem_field32_set(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data, VLAN_IS_DESIGNATEDf, tbl_data->vlan_is_designated);
  soc_mem_field32_set(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data, VLAN_IS_DESIGNATED_MASKf, tbl_data->vlan_is_designated_mask);
  soc_mem_field32_set(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data, VT_PROCESSING_PROFILEf, tbl_data->vt_processing_profile);
  soc_mem_field32_set(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data, VT_PROCESSING_PROFILE_MASKf, tbl_data->vt_processing_profile_mask);
  soc_mem_field32_set(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data, VALIDf, tbl_data->valid);
  soc_mem_field32_set(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data, PROGRAMf, tbl_data->llvp_prog_sel);
  if (SOC_IS_JERICHO(unit)) {
      soc_mem_field32_set(unit, IHP_VTT_2ND_KEY_PROG_SEL_TCAMm, data, VT_IN_LIF_PROFILE_MASKf, tbl_data->vt_in_lif_profile_mask);
  }
  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_isem_2nd_program_selection_convert_tbl_data_to_data()", 0, 0);
}

uint32
  arad_pp_ihp_isem_2nd_program_selection_cam_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_VTT2ND_KEY_PROGRAM_SELECTION_CAM_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_ISEM_2ND_PROGRAM_SELECTION_CAM_TBL_SET_UNSAFE);

  res = arad_pp_ihp_isem_2nd_program_selection_convert_tbl_data_to_data(
          unit,
          tbl_data,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_mem_write(
          unit,
          IHP_VTT_2ND_KEY_PROG_SEL_TCAMm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_isem_2nd_program_selection_cam_tbl_set_unsafe()", entry_offset, 0);
}

/* 
 * in jericho, 2 new copy engines in both IHP_VTT_1ST_LOOKUP_PROGRAM_0/1 
 * Temporary bypass to work on Arad microcode: 
 * - do not use the 2 new copy engines 
 * - encode differently key_valids_1 accordingly 
 */
uint32
  arad_pp_ihp_vtt_key_construction_key_valids_1_jericho_wa_build(
    SOC_SAND_IN  uint32              key_valids_1
  )
{
    /* Move bits 3:2 to bits 5:4 instead */
    return ((key_valids_1 & 0x3) + (((key_valids_1 & 0xC) >> 2) << 4));
}

uint32
  arad_pp_ihp_vtt_key_construction_key_valids_1_jericho_wa_parse(
    SOC_SAND_IN  uint32              key_valids_1
  )
{
    /* Take bits 3:2 from bits 5:4 instead */
    return ((key_valids_1 & 0x3) + (((key_valids_1 & 0x30) >> 4) << 2));
}


uint32
  arad_pp_ihp_vtt1st_key_construction0_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_ENTRY_SIZE];
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHP_VTT_1ST_LOOKUP_PROGRAM_0m,
          MEM_BLOCK_ANY,
          entry_offset,
          data          
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->key_16b_inst0 = soc_mem_field32_get(
                  unit,
                  IHP_VTT_1ST_LOOKUP_PROGRAM_0m,
                  data,
                  KEY_16B_INST_0f    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  tbl_data->key_16b_inst1 = soc_mem_field32_get(
                  unit,
                  IHP_VTT_1ST_LOOKUP_PROGRAM_0m,
                  data,
                  KEY_16B_INST_1f    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  tbl_data->key_32b_inst0 = soc_mem_field32_get(
                  unit,
                  IHP_VTT_1ST_LOOKUP_PROGRAM_0m,
                  data,
                  KEY_32B_INST_0f    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  tbl_data->key_32b_inst1 = soc_mem_field32_get(
                  unit,
                  IHP_VTT_1ST_LOOKUP_PROGRAM_0m,
                  data,
                  KEY_32B_INST_1f    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  tbl_data->isa_key_valids_1 = soc_mem_field32_get(
                  unit,
                  IHP_VTT_1ST_LOOKUP_PROGRAM_0m,
                  data,
                  ISA_KEY_VALIDSf    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

  tbl_data->isb_key_valids_1 = soc_mem_field32_get(
                  unit,
                  IHP_VTT_1ST_LOOKUP_PROGRAM_0m,
                  data,
                  ISB_KEY_VALIDSf    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 54, exit);

  tbl_data->tcam_key_valids_1 = soc_mem_field32_get(
                  unit,
                  IHP_VTT_1ST_LOOKUP_PROGRAM_0m,
                  data,
                  TCAM_KEY_VALIDSf    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 56, exit);

  if (SOC_IS_JERICHO(unit)) {
      /* Temp WA to work on Arad microcode */
      tbl_data->isa_key_valids_1 = arad_pp_ihp_vtt_key_construction_key_valids_1_jericho_wa_parse(tbl_data->isa_key_valids_1);
      tbl_data->isb_key_valids_1 = arad_pp_ihp_vtt_key_construction_key_valids_1_jericho_wa_parse(tbl_data->isb_key_valids_1);
      tbl_data->tcam_key_valids_1 = arad_pp_ihp_vtt_key_construction_key_valids_1_jericho_wa_parse(tbl_data->tcam_key_valids_1);
  }

  tbl_data->key_program_variable = soc_mem_field32_get(
                  unit,
                  IHP_VTT_1ST_LOOKUP_PROGRAM_0m,
                  data,
                  KEY_PROGRAM_VARIABLEf    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 58, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_vtt1st_key_construction0_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_vtt1st_key_construction0_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_DATA  *tbl_data
  )
{
  uint32
      isa_key_valids_1,
      isb_key_valids_1,
      tcam_key_valids_1,
    res = SOC_SAND_OK;
  uint32    
    data[ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_ENTRY_SIZE);

  soc_mem_field32_set(
          unit,
          IHP_VTT_1ST_LOOKUP_PROGRAM_0m,
          data,
          KEY_16B_INST_0f,
          tbl_data->key_16b_inst0 );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

    soc_mem_field32_set(
          unit,
          IHP_VTT_1ST_LOOKUP_PROGRAM_0m,
          data,
          KEY_16B_INST_1f,
          tbl_data->key_16b_inst1 );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  soc_mem_field32_set(
          unit,
          IHP_VTT_1ST_LOOKUP_PROGRAM_0m,
          data,
          KEY_32B_INST_0f,
          tbl_data->key_32b_inst0 );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

    soc_mem_field32_set(
          unit,
          IHP_VTT_1ST_LOOKUP_PROGRAM_0m,
          data,
          KEY_32B_INST_1f,
          tbl_data->key_32b_inst1 );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  if (SOC_IS_JERICHO(unit)) {
      /* Temp WA to work on Arad microcode */
      isa_key_valids_1 = arad_pp_ihp_vtt_key_construction_key_valids_1_jericho_wa_build(tbl_data->isa_key_valids_1);
      isb_key_valids_1 = arad_pp_ihp_vtt_key_construction_key_valids_1_jericho_wa_build(tbl_data->isb_key_valids_1);
      tcam_key_valids_1 = arad_pp_ihp_vtt_key_construction_key_valids_1_jericho_wa_build(tbl_data->tcam_key_valids_1);
  } else {
      isa_key_valids_1 = tbl_data->isa_key_valids_1;
      isb_key_valids_1 = tbl_data->isb_key_valids_1;
      tcam_key_valids_1 = tbl_data->tcam_key_valids_1;
  }

  soc_mem_field32_set(
          unit,
          IHP_VTT_1ST_LOOKUP_PROGRAM_0m,
          data,
          ISA_KEY_VALIDSf,
          isa_key_valids_1 );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  soc_mem_field32_set(
          unit,
          IHP_VTT_1ST_LOOKUP_PROGRAM_0m,
          data,
          ISB_KEY_VALIDSf,
          isb_key_valids_1 );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  soc_mem_field32_set(
          unit,
          IHP_VTT_1ST_LOOKUP_PROGRAM_0m,
          data,
          TCAM_KEY_VALIDSf,
          tcam_key_valids_1 );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  soc_mem_field32_set(
          unit,
          IHP_VTT_1ST_LOOKUP_PROGRAM_0m,
          data,
          KEY_PROGRAM_VARIABLEf,
          tbl_data->key_program_variable );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  res = soc_mem_write(
          unit,
          IHP_VTT_1ST_LOOKUP_PROGRAM_0m,
          MEM_BLOCK_ANY,
          entry_offset,
          data          
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 55, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_vtt1st_key_construction0_tbl_set_unsafe()", entry_offset, 0);
}


uint32
  arad_pp_ihp_vtt_flp_key_construction_pd_bitmap_strength_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint8              is_for_flp, 
    SOC_SAND_IN  uint32             fld_ndx,
    SOC_SAND_OUT uint32            *fld_location,
    SOC_SAND_OUT uint32            *fld_strength
  )
{
  uint32
    fld_location_vtt[ARAD_PP_VTT_PD_BITMAP_NOF_FIELDS] = {2,5,4,3}, /* TCAM-LP, TCAM-HP, ISEM-A, ISEM-B*/
    fld_strength_vtt[ARAD_PP_VTT_PD_BITMAP_NOF_FIELDS] = {1,3,2,2}, /* TCAM-LP, TCAM-HP, ISEM-A, ISEM-B*/
    fld_location_flp[ARAD_PP_FLP_PD_BITMAP_NOF_FIELDS] = {0,0,0,0,0,0,0}, /* ELK-FWD, LEM-2, TCAM, LPM-2, ELK-Ext, LEM-1, LPM-1 */
    fld_strength_flp[ARAD_PP_FLP_PD_BITMAP_NOF_FIELDS] = {1,1,1,1,1,1,1}; /* ELK-FWD, LEM-2, TCAM, LPM-2, ELK-Ext, LEM-1, LPM-1 */

 
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  *fld_location = (is_for_flp)? fld_location_flp[fld_ndx]: fld_location_vtt[fld_ndx];
  *fld_strength = (is_for_flp)? fld_strength_flp[fld_ndx]: fld_strength_vtt[fld_ndx];

  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_vtt_flp_key_construction_pd_bitmap_strength_get_unsafe()", 0, 0);
}


uint32
  arad_pp_ihp_vtt_flp_key_construction_pd_bitmap_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint8              is_for_flp,
    SOC_SAND_IN  uint32             *data,
    SOC_SAND_IN  soc_mem_t          mem_id, 
    SOC_SAND_IN  soc_field_t        *field_array, /* VTT: TCAM-LP, TCAM-HP, ISEM-A, ISEM-B */
    SOC_SAND_OUT uint32            *pd_bitmap
  )
{
  uint32
    fld_val,
    fld_ndx,
    fld_location_lcl,
    fld_strength_lcl,
    res = SOC_SAND_OK;

 
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);


  *pd_bitmap = 0;

  for (fld_ndx = 0; fld_ndx < ((is_for_flp)? ARAD_PP_FLP_PD_BITMAP_NOF_FIELDS: ARAD_PP_VTT_PD_BITMAP_NOF_FIELDS); fld_ndx++) {
      fld_val = soc_mem_field32_get(
                      unit,
                      mem_id,
                      data,
                      field_array[fld_ndx]);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10 + fld_ndx, exit);

      res = arad_pp_ihp_vtt_flp_key_construction_pd_bitmap_strength_get_unsafe(
                unit,
                is_for_flp,
                fld_ndx,
                &fld_location_lcl,
                &fld_strength_lcl
              );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30 + fld_ndx, exit);

      if (fld_val == fld_strength_lcl) {
          SHR_BITSET(pd_bitmap, fld_location_lcl);
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_vtt_flp_key_construction_pd_bitmap_get_unsafe()", 0, 0);
}

uint32
  arad_pp_ihp_vtt_flp_key_construction_pd_bitmap_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint8              is_for_flp,
    SOC_SAND_INOUT  uint32             *data,
    SOC_SAND_IN  soc_mem_t          mem_id, 
    SOC_SAND_IN  soc_field_t        *field_array, /* VTT: TCAM-LP, TCAM-HP, ISEM-A, ISEM-B */
    SOC_SAND_IN  uint32             pd_bitmap
  )
{
  uint32
    fld_val,
    fld_ndx,
    fld_location_lcl,
    fld_strength_lcl,
    pd_bitmap_lcl[1],
    res = SOC_SAND_OK;

 
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  /* 
   * Since the TCAM appears twice in pd_bitmap, prefer: 
   * 1. Set to 0 all the field values 
   * 2. Change their value only if the bit in pd_bitmap is set 
   */
  for (fld_ndx = 0; fld_ndx < ((is_for_flp)? ARAD_PP_FLP_PD_BITMAP_NOF_FIELDS: ARAD_PP_VTT_PD_BITMAP_NOF_FIELDS); fld_ndx++) {
      fld_val = 0;

      soc_mem_field32_set(
              unit,
              mem_id,
              data,
              field_array[fld_ndx],
              fld_val);
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

  for (fld_ndx = 0; fld_ndx < ((is_for_flp)? ARAD_PP_FLP_PD_BITMAP_NOF_FIELDS: ARAD_PP_VTT_PD_BITMAP_NOF_FIELDS); fld_ndx++) {
      res = arad_pp_ihp_vtt_flp_key_construction_pd_bitmap_strength_get_unsafe(
                unit,
                is_for_flp,
                fld_ndx,
                &fld_location_lcl,
                &fld_strength_lcl
              );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30 + fld_ndx, exit);

      *pd_bitmap_lcl = pd_bitmap;
      if (SHR_BITGET(pd_bitmap_lcl, fld_location_lcl)) {
          fld_val = fld_strength_lcl;
          soc_mem_field32_set(
                  unit,
                  mem_id,
                  data,
                  field_array[fld_ndx],
                  fld_val);
          SOC_SAND_CHECK_FUNC_RESULT(res, 54, exit);
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_vtt_flp_key_construction_pd_bitmap_set_unsafe()", 0, 0);
}


uint32
  arad_pp_ihp_vtt1st_key_construction1_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32    
    data[ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_ENTRY_SIZE];
 
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_DATA, 1);

 
  res = soc_mem_read(
          unit,
          IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
          MEM_BLOCK_ANY,
          entry_offset,
          data          
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->key_16b_inst2   = soc_mem_field32_get(
                  unit,
                  IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
                  data,
                  SOC_IS_JERICHO(unit)? KEY_16B_INST_4f: KEY_16B_INST_2f);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  tbl_data->key_16b_inst3   = soc_mem_field32_get(
                  unit,
                  IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
                  data,
                  SOC_IS_JERICHO(unit)? KEY_16B_INST_5f: KEY_16B_INST_3f            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  tbl_data->key_32b_inst2   = soc_mem_field32_get(
                  unit,
                  IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
                  data,
                  KEY_32B_INST_2f            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  tbl_data->key_32b_inst3   = soc_mem_field32_get(
                  unit,
                  IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
                  data,
                  KEY_32B_INST_3f            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  tbl_data->isa_key_valids_2   = soc_mem_field32_get(
                  unit,
                  IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
                  data,
                  ISA_KEY_VALIDSf );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  tbl_data->isb_key_valids_2   = soc_mem_field32_get(
                  unit,
                  IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
                  data,
                  ISB_KEY_VALIDSf );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  tbl_data->tcam_key_valids_2   = soc_mem_field32_get(
                  unit,
                  IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
                  data,
                  TCAM_KEY_VALIDSf );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  tbl_data->isa_and_mask   = soc_mem_field32_get(
                  unit,
                  IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
                  data,
                  ISA_AND_MASKf );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  tbl_data->isa_or_mask   = soc_mem_field32_get(
                  unit,
                  IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
                  data,
                  ISA_OR_MASKf );
  SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

  tbl_data->isa_lookup_enable   = soc_mem_field32_get(
                  unit,
                  IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
                  data,
                  ISA_LOOKUP_ENABLEf );
  SOC_SAND_CHECK_FUNC_RESULT(res, 54, exit);

  tbl_data->isb_and_mask   = soc_mem_field32_get(
                  unit,
                  IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
                  data,
                  ISB_AND_MASKf );
  SOC_SAND_CHECK_FUNC_RESULT(res, 56, exit);

  tbl_data->isb_or_mask   = soc_mem_field32_get(
                  unit,
                  IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
                  data,
                  ISB_OR_MASKf );
  SOC_SAND_CHECK_FUNC_RESULT(res, 58, exit);

  tbl_data->isb_lookup_enable   = soc_mem_field32_get(
                  unit,
                  IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
                  data,
                  ISB_LOOKUP_ENABLEf );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  tbl_data->tcam_db_profile   = soc_mem_field32_get(
                  unit,
                  IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
                  data,
                  TCAM_DB_PROFILEf );
  SOC_SAND_CHECK_FUNC_RESULT(res, 62, exit);

  if (!SOC_IS_ARADPLUS(unit)) {
      tbl_data->vlan_translation_0_pd_bitmap   = soc_mem_field32_get(
                  unit,
                  IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
                  data,
                  VLAN_TRANSLATION_0_PD_BITMAPf);
      SOC_SAND_CHECK_FUNC_RESULT(res, 64, exit);

      tbl_data->vlan_translation_1_pd_bitmap   = soc_mem_field32_get(
                  unit,
                  IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
                  data,
                  VLAN_TRANSLATION_1_PD_BITMAPf);
      SOC_SAND_CHECK_FUNC_RESULT(res, 66, exit);
  }
  else {
      /* 
       * In ARAD+: 
       * - Skip looking at the 2 LSBs of vlan_translation_0_pd_bitmap: 
       * it seems unused in arad_pp_isem_access.c in any program. Return an error if set in Arad+
       * - Translation table from enable->strength with assumption the order in same strength 
       * is identical to Arad-A0/B0: 
       *   for TCAM low-priority: 0<->0 and 1<-> strength 1
       *   for ISEM-A/B: 0<->0 and 1<->strength 2
       *   for TCAM high-priority: 0<->0 and 1<->strength 3
       */
      soc_field_t 
          strength_fields[2][ARAD_PP_VTT_PD_BITMAP_NOF_FIELDS] = {
              {VLAN_TRANSLATION_0_PD_TCAM_STRENGTHf, VLAN_TRANSLATION_0_PD_TCAM_STRENGTHf, VLAN_TRANSLATION_0_PD_ISA_STRENGTHf, VLAN_TRANSLATION_0_PD_ISB_STRENGTHf},
              {VLAN_TRANSLATION_1_PD_TCAM_STRENGTHf, VLAN_TRANSLATION_1_PD_TCAM_STRENGTHf, VLAN_TRANSLATION_1_PD_ISA_STRENGTHf, VLAN_TRANSLATION_1_PD_ISB_STRENGTHf}
          };
      /* 
       * In case in_use_strength == 0 then use pd_bitmap and convert it to strength (ARAD compatible mode)
       */
      if (tbl_data->vlan_translation_is_use_strength == 0) {
          res = arad_pp_ihp_vtt_flp_key_construction_pd_bitmap_get_unsafe(
                    unit,
                    FALSE, /* is_for_flp */
                    data,
                    IHP_VTT_1ST_LOOKUP_PROGRAM_1m, 
                    strength_fields[0],
                    &(tbl_data->vlan_translation_0_pd_bitmap)
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 64, exit);

          res = arad_pp_ihp_vtt_flp_key_construction_pd_bitmap_get_unsafe(
                    unit,
                    FALSE, /* is_for_flp */
                    data,
                    IHP_VTT_1ST_LOOKUP_PROGRAM_1m, 
                    strength_fields[1],
                    &(tbl_data->vlan_translation_1_pd_bitmap)
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 66, exit);
      } else {
          tbl_data->vlan_translation_0_pd_tcam_strength   = soc_mem_field32_get(
                      unit,
                      IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
                      data,
                      VLAN_TRANSLATION_0_PD_TCAM_STRENGTHf);
          SOC_SAND_CHECK_FUNC_RESULT(res, 64, exit);

          tbl_data->vlan_translation_0_pd_isa_strength   = soc_mem_field32_get(
                      unit,
                      IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
                      data,
                      VLAN_TRANSLATION_0_PD_ISA_STRENGTHf);
          SOC_SAND_CHECK_FUNC_RESULT(res, 66, exit);

          tbl_data->vlan_translation_0_pd_isb_strength   = soc_mem_field32_get(
                      unit,
                      IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
                      data,
                      VLAN_TRANSLATION_0_PD_ISB_STRENGTHf);
          SOC_SAND_CHECK_FUNC_RESULT(res, 68, exit);

          tbl_data->vlan_translation_1_pd_tcam_strength   = soc_mem_field32_get(
                      unit,
                      IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
                      data,
                      VLAN_TRANSLATION_1_PD_TCAM_STRENGTHf);
          SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

          tbl_data->vlan_translation_1_pd_isa_strength   = soc_mem_field32_get(
                      unit,
                      IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
                      data,
                      VLAN_TRANSLATION_1_PD_ISA_STRENGTHf);
          SOC_SAND_CHECK_FUNC_RESULT(res, 72, exit);

          tbl_data->vlan_translation_1_pd_isb_strength   = soc_mem_field32_get(
                      unit,
                      IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
                      data,
                      VLAN_TRANSLATION_1_PD_ISB_STRENGTHf);
          SOC_SAND_CHECK_FUNC_RESULT(res, 74, exit);
      }
  }

  tbl_data->processing_profile   = soc_mem_field32_get(
              unit,
              IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
              data,
              PROCESSING_PROFILEf );
  SOC_SAND_CHECK_FUNC_RESULT(res, 76, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_vtt1st_key_construction1_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_vtt1st_key_construction1_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32    
    data[ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_ENTRY_SIZE);

  soc_mem_field32_set(
          unit,
          IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
          data,
          SOC_IS_JERICHO(unit)? KEY_16B_INST_4f: KEY_16B_INST_2f,
          tbl_data->key_16b_inst2 );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    soc_mem_field32_set(
          unit,
          IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
          data,
          SOC_IS_JERICHO(unit)? KEY_16B_INST_5f: KEY_16B_INST_3f,
          tbl_data->key_16b_inst3 );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

    soc_mem_field32_set(
          unit,
          IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
          data,
          KEY_32B_INST_2f,
          tbl_data->key_32b_inst2 );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

    soc_mem_field32_set(
          unit,
          IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
          data,
          KEY_32B_INST_3f,
          tbl_data->key_32b_inst3 );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

    soc_mem_field32_set(
          unit,
          IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
          data,
          ISA_KEY_VALIDSf,
          tbl_data->isa_key_valids_2 );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  soc_mem_field32_set(
          unit,
          IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
          data,
          ISB_KEY_VALIDSf,
          tbl_data->isb_key_valids_2 );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  soc_mem_field32_set(
          unit,
          IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
          data,
          TCAM_KEY_VALIDSf,
          tbl_data->tcam_key_valids_2 );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

  soc_mem_field32_set(
          unit,
          IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
          data,
          ISA_AND_MASKf,
          tbl_data->isa_and_mask );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  soc_mem_field32_set(
          unit,
          IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
          data,
          ISA_OR_MASKf,
          tbl_data->isa_or_mask );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  soc_mem_field32_set(
          unit,
          IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
          data,
          ISA_LOOKUP_ENABLEf,
          tbl_data->isa_lookup_enable );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  soc_mem_field32_set(
          unit,
          IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
          data,
          ISB_AND_MASKf,
          tbl_data->isb_and_mask );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  soc_mem_field32_set(
          unit,
          IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
          data,
          ISB_OR_MASKf,
          tbl_data->isb_or_mask );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  soc_mem_field32_set(
          unit,
          IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
          data,
          ISB_LOOKUP_ENABLEf,
          tbl_data->isb_lookup_enable );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  soc_mem_field32_set(
          unit,
          IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
          data,
          TCAM_DB_PROFILEf,
          tbl_data->tcam_db_profile );
  SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);


  if (!SOC_IS_ARADPLUS(unit)) {
      soc_mem_field32_set(
              unit,
              IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
              data,
              VLAN_TRANSLATION_0_PD_BITMAPf,
              tbl_data->vlan_translation_0_pd_bitmap );
      SOC_SAND_CHECK_FUNC_RESULT(res, 54, exit);

      soc_mem_field32_set(
              unit,
              IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
              data,
              VLAN_TRANSLATION_1_PD_BITMAPf,
              tbl_data->vlan_translation_1_pd_bitmap );
      SOC_SAND_CHECK_FUNC_RESULT(res, 56, exit);
  }
  else {
      /* 
       * In ARAD+: 
       * - Skip looking at the 2 LSBs of vlan_translation_0_pd_bitmap: 
       * it seems unused in arad_pp_isem_access.c in any program. Return an error if set in Arad+
       * - Translation table from enable->strength with assumption the order in same strength 
       * is identical to Arad-A0/B0: 
       *   for TCAM low-priority: 0<->0 and 1<-> strength 1
       *   for ISEM-A/B: 0<->0 and 1<->strength 2
       *   for TCAM high-priority: 0<->0 and 1<->strength 3
       */
      soc_field_t 
          strength_fields[2][ARAD_PP_VTT_PD_BITMAP_NOF_FIELDS] = {
              {VLAN_TRANSLATION_0_PD_TCAM_STRENGTHf, VLAN_TRANSLATION_0_PD_TCAM_STRENGTHf, VLAN_TRANSLATION_0_PD_ISA_STRENGTHf, VLAN_TRANSLATION_0_PD_ISB_STRENGTHf},
              {VLAN_TRANSLATION_1_PD_TCAM_STRENGTHf, VLAN_TRANSLATION_1_PD_TCAM_STRENGTHf, VLAN_TRANSLATION_1_PD_ISA_STRENGTHf, VLAN_TRANSLATION_1_PD_ISB_STRENGTHf}
          };

      /* 
       * In case in_use_strength == 0 then use pd_bitmap and convert it to strength (ARAD compatible mode)
       */
      if (tbl_data->vlan_translation_is_use_strength == 0) {
          res = arad_pp_ihp_vtt_flp_key_construction_pd_bitmap_set_unsafe(
                  unit,
                  FALSE, /* is_for_flp */
                  data,
                  IHP_VTT_1ST_LOOKUP_PROGRAM_1m, 
                  strength_fields[0],
                  tbl_data->vlan_translation_0_pd_bitmap );
          SOC_SAND_CHECK_FUNC_RESULT(res, 53, exit);

          res = arad_pp_ihp_vtt_flp_key_construction_pd_bitmap_set_unsafe(
                  unit,
                  FALSE, /* is_for_flp */
                  data,
                  IHP_VTT_1ST_LOOKUP_PROGRAM_1m, 
                  strength_fields[1],
                  tbl_data->vlan_translation_1_pd_bitmap );
          SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);
      } else {
          soc_mem_field32_set(
                  unit,
                  IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
                  data,
                  VLAN_TRANSLATION_0_PD_TCAM_STRENGTHf,
                  tbl_data->vlan_translation_0_pd_tcam_strength );
          SOC_SAND_CHECK_FUNC_RESULT(res, 54, exit);

          soc_mem_field32_set(
                  unit,
                  IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
                  data,
                  VLAN_TRANSLATION_0_PD_ISA_STRENGTHf,
                  tbl_data->vlan_translation_0_pd_isa_strength );
          SOC_SAND_CHECK_FUNC_RESULT(res, 56, exit);

          soc_mem_field32_set(
                  unit,
                  IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
                  data,
                  VLAN_TRANSLATION_0_PD_ISB_STRENGTHf,
                  tbl_data->vlan_translation_0_pd_isb_strength );
          SOC_SAND_CHECK_FUNC_RESULT(res, 56, exit);

          soc_mem_field32_set(
                  unit,
                  IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
                  data,
                  VLAN_TRANSLATION_1_PD_TCAM_STRENGTHf,
                  tbl_data->vlan_translation_1_pd_tcam_strength );
          SOC_SAND_CHECK_FUNC_RESULT(res, 58, exit);

          soc_mem_field32_set(
                  unit,
                  IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
                  data,
                  VLAN_TRANSLATION_1_PD_ISA_STRENGTHf,
                  tbl_data->vlan_translation_1_pd_isa_strength );
          SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

          soc_mem_field32_set(
                  unit,
                  IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
                  data,
                  VLAN_TRANSLATION_1_PD_ISB_STRENGTHf,
                  tbl_data->vlan_translation_1_pd_isb_strength );
          SOC_SAND_CHECK_FUNC_RESULT(res, 62, exit);
      }      
  }

  soc_mem_field32_set(
          unit,
          IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
          data,
          PROCESSING_PROFILEf,
          tbl_data->processing_profile );
  SOC_SAND_CHECK_FUNC_RESULT(res, 64, exit);

  res = soc_mem_write(
          unit,
          IHP_VTT_1ST_LOOKUP_PROGRAM_1m,
          MEM_BLOCK_ANY,
          entry_offset,
          data          
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 66, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_vtt1st_key_construction1_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_vtt1st_key_construction_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_INOUT ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_DATA
    data0;
  ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_DATA
    data1;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION_TBL_GET_UNSAFE);

  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION_TBL_DATA, 1);

  res = arad_pp_ihp_vtt1st_key_construction0_tbl_get_unsafe(
          unit,
          entry_offset,
          &data0
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihp_vtt1st_key_construction1_tbl_get_unsafe(
          unit,
          entry_offset,
          &data1
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  tbl_data->key_16b_inst0 = data0.key_16b_inst0;
  tbl_data->key_16b_inst1 = data0.key_16b_inst1;
  tbl_data->key_32b_inst0 = data0.key_32b_inst0;
  tbl_data->key_32b_inst1 = data0.key_32b_inst1;
  tbl_data->isa_key_valids_1 = data0.isa_key_valids_1;
  tbl_data->isb_key_valids_1 = data0.isb_key_valids_1;
  tbl_data->tcam_key_valids_1 = data0.tcam_key_valids_1;
  tbl_data->key_program_variable = data0.key_program_variable;
  tbl_data->key_16b_inst2 = data1.key_16b_inst2;
  tbl_data->key_16b_inst3 = data1.key_16b_inst3;
  tbl_data->key_32b_inst2 = data1.key_32b_inst2;
  tbl_data->key_32b_inst3 = data1.key_32b_inst3;
  tbl_data->isa_key_valids_2 = data1.isa_key_valids_2;
  tbl_data->isb_key_valids_2 = data1.isb_key_valids_2;
  tbl_data->tcam_key_valids_2 = data1.tcam_key_valids_2;
  tbl_data->isa_and_mask = data1.isa_and_mask;
  tbl_data->isa_or_mask = data1.isa_or_mask;
  tbl_data->isa_lookup_enable = data1.isa_lookup_enable;
  tbl_data->isb_and_mask = data1.isb_and_mask;
  tbl_data->isb_or_mask = data1.isb_or_mask;
  tbl_data->isb_lookup_enable = data1.isb_lookup_enable;
  tbl_data->tcam_db_profile = data1.tcam_db_profile;
  tbl_data->vlan_translation_0_pd_bitmap = data1.vlan_translation_0_pd_bitmap;
  tbl_data->vlan_translation_1_pd_bitmap = data1.vlan_translation_1_pd_bitmap;
  tbl_data->processing_profile = data1.processing_profile;

#ifdef BCM_88660_A0
  if (tbl_data->vlan_translation_is_use_strength) {
      tbl_data->vlan_translation_0_pd_tcam_strength = data1.vlan_translation_0_pd_tcam_strength;
      tbl_data->vlan_translation_0_pd_isa_strength = data1.vlan_translation_0_pd_isa_strength; 
      tbl_data->vlan_translation_0_pd_isb_strength = data1.vlan_translation_0_pd_isb_strength; 
      tbl_data->vlan_translation_1_pd_tcam_strength = data1.vlan_translation_1_pd_tcam_strength;
      tbl_data->vlan_translation_1_pd_isa_strength = data1.vlan_translation_1_pd_isa_strength; 
      tbl_data->vlan_translation_1_pd_isb_strength = data1.vlan_translation_1_pd_isb_strength; 
  }
#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_vtt1st_key_construction_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_vtt1st_key_construction_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_DATA
    data0;
  ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_DATA
    data1;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION_TBL_SET_UNSAFE);

  data0.key_16b_inst0 = tbl_data->key_16b_inst0;
  data0.key_16b_inst1 = tbl_data->key_16b_inst1;
  data0.key_32b_inst0 = tbl_data->key_32b_inst0;
  data0.key_32b_inst1 = tbl_data->key_32b_inst1;
  data0.isa_key_valids_1 = tbl_data->isa_key_valids_1;
  data0.isb_key_valids_1 = tbl_data->isb_key_valids_1;
  data0.tcam_key_valids_1 = tbl_data->tcam_key_valids_1;
  data0.key_program_variable = tbl_data->key_program_variable;
  data1.key_16b_inst2 = tbl_data->key_16b_inst2;
  data1.key_16b_inst3 = tbl_data->key_16b_inst3;
  data1.key_32b_inst2 = tbl_data->key_32b_inst2;
  data1.key_32b_inst3 = tbl_data->key_32b_inst3;
  data1.isa_key_valids_2 = tbl_data->isa_key_valids_2;
  data1.isb_key_valids_2 = tbl_data->isb_key_valids_2;
  data1.tcam_key_valids_2 = tbl_data->tcam_key_valids_2;
  data1.isa_and_mask = tbl_data->isa_and_mask;
  data1.isa_or_mask = tbl_data->isa_or_mask;
  data1.isa_lookup_enable = tbl_data->isa_lookup_enable;
  data1.isb_and_mask = tbl_data->isb_and_mask;
  data1.isb_or_mask = tbl_data->isb_or_mask;
  data1.isb_lookup_enable = tbl_data->isb_lookup_enable;
  data1.tcam_db_profile = tbl_data->tcam_db_profile;
  data1.vlan_translation_0_pd_bitmap = tbl_data->vlan_translation_0_pd_bitmap;
  data1.vlan_translation_1_pd_bitmap = tbl_data->vlan_translation_1_pd_bitmap;
  data1.processing_profile = tbl_data->processing_profile;

#ifdef BCM_88660_A0
  data1.vlan_translation_is_use_strength = 0;
  if (tbl_data->vlan_translation_is_use_strength) {
      data1.vlan_translation_is_use_strength = tbl_data->vlan_translation_is_use_strength;
      data1.vlan_translation_0_pd_tcam_strength = tbl_data->vlan_translation_0_pd_tcam_strength;
      data1.vlan_translation_0_pd_isa_strength = tbl_data->vlan_translation_0_pd_isa_strength; 
      data1.vlan_translation_0_pd_isb_strength = tbl_data->vlan_translation_0_pd_isb_strength; 
      data1.vlan_translation_1_pd_tcam_strength = tbl_data->vlan_translation_1_pd_tcam_strength;
      data1.vlan_translation_1_pd_isa_strength = tbl_data->vlan_translation_1_pd_isa_strength; 
      data1.vlan_translation_1_pd_isb_strength = tbl_data->vlan_translation_1_pd_isb_strength; 
  }
#endif

  res = arad_pp_ihp_vtt1st_key_construction0_tbl_set_unsafe(
          unit,
          entry_offset,
          &data0
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_pp_ihp_vtt1st_key_construction1_tbl_set_unsafe(
          unit,
          entry_offset,
          &data1
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_vtt1st_key_construction_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_vtt2nd_key_construction_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32    
    data_tbl_0[ARAD_PP_IHP_VTT2ND_KEY_CONSTRUCTION0_TBL_ENTRY_SIZE],
    data_tbl_1[ARAD_PP_IHP_VTT2ND_KEY_CONSTRUCTION1_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_GET_UNSAFE);

  ARAD_CLEAR(data_tbl_0, uint32, ARAD_PP_IHP_VTT2ND_KEY_CONSTRUCTION0_TBL_ENTRY_SIZE);
  ARAD_CLEAR(data_tbl_1, uint32, ARAD_PP_IHP_VTT2ND_KEY_CONSTRUCTION1_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_DATA, 1);

  /* 
   * ARAD_PP_IHP_VTT2ND_KEY_CONSTRUCTION0
   */
  res = soc_mem_read(
          unit,
          IHP_VTT_2ND_LOOKUP_PROGRAM_0m,
          MEM_BLOCK_ANY,
          entry_offset,
          data_tbl_0          
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->key_16b_inst0 = soc_mem_field32_get(
                  unit,
                  IHP_VTT_2ND_LOOKUP_PROGRAM_0m,
                  data_tbl_0,
                  KEY_16B_INST_0f    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  tbl_data->key_16b_inst1 = soc_mem_field32_get(
                  unit,
                  IHP_VTT_2ND_LOOKUP_PROGRAM_0m,
                  data_tbl_0,
                  KEY_16B_INST_1f    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  tbl_data->key_32b_inst0 = soc_mem_field32_get(
                  unit,
                  IHP_VTT_2ND_LOOKUP_PROGRAM_0m,
                  data_tbl_0,
                  KEY_32B_INST_0f    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  tbl_data->key_32b_inst1 = soc_mem_field32_get(
                  unit,
                  IHP_VTT_2ND_LOOKUP_PROGRAM_0m,
                  data_tbl_0,
                  KEY_32B_INST_1f    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  tbl_data->isa_key_valids_1 = soc_mem_field32_get(
                  unit,
                  IHP_VTT_2ND_LOOKUP_PROGRAM_0m,
                  data_tbl_0,
                  ISA_KEY_VALIDSf    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

  tbl_data->isb_key_valids_1 = soc_mem_field32_get(
                  unit,
                  IHP_VTT_2ND_LOOKUP_PROGRAM_0m,
                  data_tbl_0,
                  ISB_KEY_VALIDSf    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 54, exit);

  tbl_data->tcam_key_valids_1 = soc_mem_field32_get(
                  unit,
                  IHP_VTT_2ND_LOOKUP_PROGRAM_0m,
                  data_tbl_0,
                  TCAM_KEY_VALIDSf    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 56, exit);

  if (SOC_IS_JERICHO(unit)) {
      /* Temp WA to work on Arad microcode */
      tbl_data->isa_key_valids_1 = arad_pp_ihp_vtt_key_construction_key_valids_1_jericho_wa_parse(tbl_data->isa_key_valids_1);
      tbl_data->isb_key_valids_1 = arad_pp_ihp_vtt_key_construction_key_valids_1_jericho_wa_parse(tbl_data->isb_key_valids_1);
      tbl_data->tcam_key_valids_1 = arad_pp_ihp_vtt_key_construction_key_valids_1_jericho_wa_parse(tbl_data->tcam_key_valids_1);
  }

  tbl_data->key_program_variable = soc_mem_field32_get(
                  unit,
                  IHP_VTT_2ND_LOOKUP_PROGRAM_0m,
                  data_tbl_0,
                  KEY_PROGRAM_VARIABLEf    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 58, exit);

  tbl_data->isa_key_initial_from_vt = soc_mem_field32_get(
                  unit,
                  IHP_VTT_2ND_LOOKUP_PROGRAM_0m,
                  data_tbl_0,
                  ISA_KEY_INITIAL_FROM_VTf    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  tbl_data->isb_key_initial_from_vt = soc_mem_field32_get(
                  unit,
                  IHP_VTT_2ND_LOOKUP_PROGRAM_0m,
                  data_tbl_0,
                  ISB_KEY_INITIAL_FROM_VTf    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 62, exit);

  tbl_data->tcam_key_initial_from_vt = soc_mem_field32_get(
                  unit,
                  IHP_VTT_2ND_LOOKUP_PROGRAM_0m,
                  data_tbl_0,
                  TCAM_KEY_INITIAL_FROM_VTf    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 64, exit);

  /* 
   * ARAD_PP_IHP_VTT2ND_KEY_CONSTRUCTION1
   */
  res = soc_mem_read(
          unit,
          IHP_VTT_2ND_LOOKUP_PROGRAM_1m,
          MEM_BLOCK_ANY,
          entry_offset,
          data_tbl_1          
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->key_16b_inst2   = soc_mem_field32_get(
                  unit,
                  IHP_VTT_2ND_LOOKUP_PROGRAM_1m,
                  data_tbl_1,
                  SOC_IS_JERICHO(unit)? KEY_16B_INST_4f: KEY_16B_INST_2f);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  tbl_data->key_16b_inst3   = soc_mem_field32_get(
                  unit,
                  IHP_VTT_2ND_LOOKUP_PROGRAM_1m,
                  data_tbl_1,
                  SOC_IS_JERICHO(unit)? KEY_16B_INST_5f: KEY_16B_INST_3f            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  tbl_data->key_32b_inst2   = soc_mem_field32_get(
                  unit,
                  IHP_VTT_2ND_LOOKUP_PROGRAM_1m,
                  data_tbl_1,
                  KEY_32B_INST_2f            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  tbl_data->key_32b_inst3   = soc_mem_field32_get(
                  unit,
                  IHP_VTT_2ND_LOOKUP_PROGRAM_1m,
                  data_tbl_1,
                  KEY_32B_INST_3f            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  tbl_data->isa_key_valids_2   = soc_mem_field32_get(
                  unit,
                  IHP_VTT_2ND_LOOKUP_PROGRAM_1m,
                  data_tbl_1,
                  ISA_KEY_VALIDSf );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  tbl_data->isb_key_valids_2   = soc_mem_field32_get(
                  unit,
                  IHP_VTT_2ND_LOOKUP_PROGRAM_1m,
                  data_tbl_1,
                  ISB_KEY_VALIDSf );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  tbl_data->tcam_key_valids_2   = soc_mem_field32_get(
                  unit,
                  IHP_VTT_2ND_LOOKUP_PROGRAM_1m,
                  data_tbl_1,
                  TCAM_KEY_VALIDSf );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  tbl_data->isa_and_mask   = soc_mem_field32_get(
                  unit,
                  IHP_VTT_2ND_LOOKUP_PROGRAM_1m,
                  data_tbl_1,
                  ISA_AND_MASKf );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  tbl_data->isa_or_mask   = soc_mem_field32_get(
                  unit,
                  IHP_VTT_2ND_LOOKUP_PROGRAM_1m,
                  data_tbl_1,
                  ISA_OR_MASKf );
  SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

  tbl_data->isa_lookup_enable   = soc_mem_field32_get(
                  unit,
                  IHP_VTT_2ND_LOOKUP_PROGRAM_1m,
                  data_tbl_1,
                  ISA_LOOKUP_ENABLEf );
  SOC_SAND_CHECK_FUNC_RESULT(res, 54, exit);

  tbl_data->isb_and_mask   = soc_mem_field32_get(
                  unit,
                  IHP_VTT_2ND_LOOKUP_PROGRAM_1m,
                  data_tbl_1,
                  ISB_AND_MASKf );
  SOC_SAND_CHECK_FUNC_RESULT(res, 56, exit);

  tbl_data->isb_or_mask   = soc_mem_field32_get(
                  unit,
                  IHP_VTT_2ND_LOOKUP_PROGRAM_1m,
                  data_tbl_1,
                  ISB_OR_MASKf );
  SOC_SAND_CHECK_FUNC_RESULT(res, 58, exit);

  tbl_data->isb_lookup_enable   = soc_mem_field32_get(
                  unit,
                  IHP_VTT_2ND_LOOKUP_PROGRAM_1m,
                  data_tbl_1,
                  ISB_LOOKUP_ENABLEf );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  tbl_data->tcam_db_profile   = soc_mem_field32_get(
                  unit,
                  IHP_VTT_2ND_LOOKUP_PROGRAM_1m,
                  data_tbl_1,
                  TCAM_DB_PROFILEf );
  SOC_SAND_CHECK_FUNC_RESULT(res, 62, exit);

  if (!SOC_IS_ARADPLUS(unit)) {
      tbl_data->termination_0_pd_bitmap   = soc_mem_field32_get(
                  unit,
                  IHP_VTT_2ND_LOOKUP_PROGRAM_1m,
                  data_tbl_1,
                  TERMINATION_0_PD_BITMAPf);
      SOC_SAND_CHECK_FUNC_RESULT(res, 64, exit);

      tbl_data->termination_1_pd_bitmap   = soc_mem_field32_get(
                  unit,
                  IHP_VTT_2ND_LOOKUP_PROGRAM_1m,
                  data_tbl_1,
                  TERMINATION_1_PD_BITMAPf);
      SOC_SAND_CHECK_FUNC_RESULT(res, 66, exit);
  }
  else {
      /* 
       * In ARAD+: 
       * - Skip looking at the 2 LSBs of vlan_translation_0_pd_bitmap: 
       * it seems unused in arad_pp_isem_access.c in any program. Return an error if set in Arad+
       * - Translation table from enable->strength with assumption the order in same strength 
       * is identical to Arad-A0/B0: 
       *   for TCAM low-priority: 0<->0 and 1<-> strength 1
       *   for ISEM-A/B: 0<->0 and 1<->strength 2
       *   for TCAM high-priority: 0<->0 and 1<->strength 3
       */
      soc_field_t 
          strength_fields[2][ARAD_PP_VTT_PD_BITMAP_NOF_FIELDS] = {
              {TERMINATION_0_PD_TCAM_STRENGTHf, TERMINATION_0_PD_TCAM_STRENGTHf, TERMINATION_0_PD_ISA_STRENGTHf, TERMINATION_0_PD_ISB_STRENGTHf},
              {TERMINATION_1_PD_TCAM_STRENGTHf, TERMINATION_1_PD_TCAM_STRENGTHf, TERMINATION_1_PD_ISA_STRENGTHf, TERMINATION_1_PD_ISB_STRENGTHf}
          };

      res = arad_pp_ihp_vtt_flp_key_construction_pd_bitmap_get_unsafe(
                unit,
                FALSE, /* is_for_flp */
                data_tbl_1,
                IHP_VTT_2ND_LOOKUP_PROGRAM_1m, 
                strength_fields[0],
                &(tbl_data->termination_0_pd_bitmap)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 64, exit);

      res = arad_pp_ihp_vtt_flp_key_construction_pd_bitmap_get_unsafe(
                unit,
                FALSE, /* is_for_flp */
                data_tbl_1,
                IHP_VTT_2ND_LOOKUP_PROGRAM_1m, 
                strength_fields[1],
                &(tbl_data->termination_1_pd_bitmap)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 66, exit);
  }

  tbl_data->processing_profile   = soc_mem_field32_get(
              unit,
              IHP_VTT_2ND_LOOKUP_PROGRAM_1m,
              data_tbl_1,
              PROCESSING_PROFILEf );
  SOC_SAND_CHECK_FUNC_RESULT(res, 68, exit);

  tbl_data->processing_code   = soc_mem_field32_get(
              unit,
              IHP_VTT_2ND_LOOKUP_PROGRAM_1m,
              data_tbl_1,
              PROCESSING_CODEf );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  tbl_data->result_to_use_0   = soc_mem_field32_get(
              unit,
              IHP_VTT_2ND_LOOKUP_PROGRAM_1m,
              data_tbl_1,
              RESULT_TO_USE_0f );
  SOC_SAND_CHECK_FUNC_RESULT(res, 72, exit);

  tbl_data->result_to_use_1   = soc_mem_field32_get(
              unit,
              IHP_VTT_2ND_LOOKUP_PROGRAM_1m,
              data_tbl_1,
              RESULT_TO_USE_1f );
  SOC_SAND_CHECK_FUNC_RESULT(res, 74, exit);

  tbl_data->result_to_use_2   = soc_mem_field32_get(
              unit,
              IHP_VTT_2ND_LOOKUP_PROGRAM_1m,
              data_tbl_1,
              RESULT_TO_USE_2f );
  SOC_SAND_CHECK_FUNC_RESULT(res, 76, exit);

 tbl_data->second_stage_parsing   = soc_mem_field32_get(
              unit,
              IHP_VTT_2ND_LOOKUP_PROGRAM_1m,
              data_tbl_1,
              SECOND_STAGE_PARSINGf );
  SOC_SAND_CHECK_FUNC_RESULT(res, 78, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_vtt2nd_key_construction_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_vtt2nd_key_construction_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_DATA  *tbl_data
  )
{
  uint32
      isa_key_valids_1,
      isb_key_valids_1,
      tcam_key_valids_1,
    res = SOC_SAND_OK;
  uint32
    data_tbl_0[ARAD_PP_IHP_VTT2ND_KEY_CONSTRUCTION0_TBL_ENTRY_SIZE],
    data_tbl_1[ARAD_PP_IHP_VTT2ND_KEY_CONSTRUCTION1_TBL_ENTRY_SIZE];
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_SET_UNSAFE);

  ARAD_CLEAR(data_tbl_0, uint32, ARAD_PP_IHP_VTT2ND_KEY_CONSTRUCTION0_TBL_ENTRY_SIZE);
  ARAD_CLEAR(data_tbl_1, uint32, ARAD_PP_IHP_VTT2ND_KEY_CONSTRUCTION1_TBL_ENTRY_SIZE);

  /* 
   * ARAD_PP_IHP_VTT2ND_KEY_CONSTRUCTION0
   */
  soc_mem_field32_set(
          unit,
          IHP_VTT_2ND_LOOKUP_PROGRAM_0m,
          data_tbl_0,
          KEY_16B_INST_0f,
          tbl_data->key_16b_inst0 );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

    soc_mem_field32_set(
          unit,
          IHP_VTT_2ND_LOOKUP_PROGRAM_0m,
          data_tbl_0,
          KEY_16B_INST_1f,
          tbl_data->key_16b_inst1 );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  soc_mem_field32_set(
          unit,
          IHP_VTT_2ND_LOOKUP_PROGRAM_0m,
          data_tbl_0,
          KEY_32B_INST_0f,
          tbl_data->key_32b_inst0 );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

    soc_mem_field32_set(
          unit,
          IHP_VTT_2ND_LOOKUP_PROGRAM_0m,
          data_tbl_0,
          KEY_32B_INST_1f,
          tbl_data->key_32b_inst1 );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  if (SOC_IS_JERICHO(unit)) {
      /* Temp WA to work on Arad microcode */
      isa_key_valids_1 = arad_pp_ihp_vtt_key_construction_key_valids_1_jericho_wa_build(tbl_data->isa_key_valids_1);
      isb_key_valids_1 = arad_pp_ihp_vtt_key_construction_key_valids_1_jericho_wa_build(tbl_data->isb_key_valids_1);
      tcam_key_valids_1 = arad_pp_ihp_vtt_key_construction_key_valids_1_jericho_wa_build(tbl_data->tcam_key_valids_1);
  } else {
      isa_key_valids_1 = tbl_data->isa_key_valids_1;
      isb_key_valids_1 = tbl_data->isb_key_valids_1;
      tcam_key_valids_1 = tbl_data->tcam_key_valids_1;
  }

  soc_mem_field32_set(
          unit,
          IHP_VTT_2ND_LOOKUP_PROGRAM_0m,
          data_tbl_0,
          ISA_KEY_VALIDSf,
          isa_key_valids_1 );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  soc_mem_field32_set(
          unit,
          IHP_VTT_2ND_LOOKUP_PROGRAM_0m,
          data_tbl_0,
          ISB_KEY_VALIDSf,
          isb_key_valids_1 );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  soc_mem_field32_set(
          unit,
          IHP_VTT_2ND_LOOKUP_PROGRAM_0m,
          data_tbl_0,
          TCAM_KEY_VALIDSf,
          tcam_key_valids_1 );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  soc_mem_field32_set(
          unit,
          IHP_VTT_2ND_LOOKUP_PROGRAM_0m,
          data_tbl_0,
          KEY_PROGRAM_VARIABLEf,
          tbl_data->key_program_variable );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  soc_mem_field32_set(
          unit,
          IHP_VTT_2ND_LOOKUP_PROGRAM_0m,
          data_tbl_0,
          ISA_KEY_INITIAL_FROM_VTf,
          tbl_data->isa_key_initial_from_vt );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  soc_mem_field32_set(
          unit,
          IHP_VTT_2ND_LOOKUP_PROGRAM_0m,
          data_tbl_0,
          ISB_KEY_INITIAL_FROM_VTf,
          tbl_data->isb_key_initial_from_vt );
  SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

  soc_mem_field32_set(
          unit,
          IHP_VTT_2ND_LOOKUP_PROGRAM_0m,
          data_tbl_0,
          TCAM_KEY_INITIAL_FROM_VTf,
          tbl_data->tcam_key_initial_from_vt );
  SOC_SAND_CHECK_FUNC_RESULT(res, 54, exit);

  res = soc_mem_write(
          unit,
          IHP_VTT_2ND_LOOKUP_PROGRAM_0m,
          MEM_BLOCK_ANY,
          entry_offset,
          data_tbl_0          
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 55, exit);

  /* 
   * ARAD_PP_IHP_VTT2ND_KEY_CONSTRUCTION1
   */
  soc_mem_field32_set(
          unit,
          IHP_VTT_2ND_LOOKUP_PROGRAM_1m,
          data_tbl_1,
          SOC_IS_JERICHO(unit)? KEY_16B_INST_4f: KEY_16B_INST_2f,
          tbl_data->key_16b_inst2 );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    soc_mem_field32_set(
          unit,
          IHP_VTT_2ND_LOOKUP_PROGRAM_1m,
          data_tbl_1,
          SOC_IS_JERICHO(unit)? KEY_16B_INST_5f: KEY_16B_INST_3f,
          tbl_data->key_16b_inst3 );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

    soc_mem_field32_set(
          unit,
          IHP_VTT_2ND_LOOKUP_PROGRAM_1m,
          data_tbl_1,
          KEY_32B_INST_2f,
          tbl_data->key_32b_inst2 );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

    soc_mem_field32_set(
          unit,
          IHP_VTT_2ND_LOOKUP_PROGRAM_1m,
          data_tbl_1,
          KEY_32B_INST_3f,
          tbl_data->key_32b_inst3 );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

    soc_mem_field32_set(
          unit,
          IHP_VTT_2ND_LOOKUP_PROGRAM_1m,
          data_tbl_1,
          ISA_KEY_VALIDSf,
          tbl_data->isa_key_valids_2 );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  soc_mem_field32_set(
          unit,
          IHP_VTT_2ND_LOOKUP_PROGRAM_1m,
          data_tbl_1,
          ISB_KEY_VALIDSf,
          tbl_data->isb_key_valids_2 );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  soc_mem_field32_set(
          unit,
          IHP_VTT_2ND_LOOKUP_PROGRAM_1m,
          data_tbl_1,
          TCAM_KEY_VALIDSf,
          tbl_data->tcam_key_valids_2 );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

  soc_mem_field32_set(
          unit,
          IHP_VTT_2ND_LOOKUP_PROGRAM_1m,
          data_tbl_1,
          ISA_AND_MASKf,
          tbl_data->isa_and_mask );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  soc_mem_field32_set(
          unit,
          IHP_VTT_2ND_LOOKUP_PROGRAM_1m,
          data_tbl_1,
          ISA_OR_MASKf,
          tbl_data->isa_or_mask );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  soc_mem_field32_set(
          unit,
          IHP_VTT_2ND_LOOKUP_PROGRAM_1m,
          data_tbl_1,
          ISA_LOOKUP_ENABLEf,
          tbl_data->isa_lookup_enable );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  soc_mem_field32_set(
          unit,
          IHP_VTT_2ND_LOOKUP_PROGRAM_1m,
          data_tbl_1,
          ISB_AND_MASKf,
          tbl_data->isb_and_mask );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  soc_mem_field32_set(
          unit,
          IHP_VTT_2ND_LOOKUP_PROGRAM_1m,
          data_tbl_1,
          ISB_OR_MASKf,
          tbl_data->isb_or_mask );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  soc_mem_field32_set(
          unit,
          IHP_VTT_2ND_LOOKUP_PROGRAM_1m,
          data_tbl_1,
          ISB_LOOKUP_ENABLEf,
          tbl_data->isb_lookup_enable );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  soc_mem_field32_set(
          unit,
          IHP_VTT_2ND_LOOKUP_PROGRAM_1m,
          data_tbl_1,
          TCAM_DB_PROFILEf,
          tbl_data->tcam_db_profile );
  SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

  if (!SOC_IS_ARADPLUS(unit)) {
      soc_mem_field32_set(
              unit,
              IHP_VTT_2ND_LOOKUP_PROGRAM_1m,
              data_tbl_1,
              TERMINATION_0_PD_BITMAPf,
              tbl_data->termination_0_pd_bitmap );
      SOC_SAND_CHECK_FUNC_RESULT(res, 54, exit);

      soc_mem_field32_set(
              unit,
              IHP_VTT_2ND_LOOKUP_PROGRAM_1m,
              data_tbl_1,
              TERMINATION_1_PD_BITMAPf,
              tbl_data->termination_1_pd_bitmap );
      SOC_SAND_CHECK_FUNC_RESULT(res, 56, exit);
  }
  else {
      /* 
       * In ARAD+: 
       * - Skip looking at the 2 LSBs of vlan_translation_0_pd_bitmap: 
       * it seems unused in arad_pp_isem_access.c in any program. Return an error if set in Arad+
       * - Translation table from enable->strength with assumption the order in same strength 
       * is identical to Arad-A0/B0: 
       *   for TCAM low-priority: 0<->0 and 1<-> strength 1
       *   for ISEM-A/B: 0<->0 and 1<->strength 2
       *   for TCAM high-priority: 0<->0 and 1<->strength 3
       */
      soc_field_t 
          strength_fields[2][ARAD_PP_VTT_PD_BITMAP_NOF_FIELDS] = {
              {TERMINATION_0_PD_TCAM_STRENGTHf, TERMINATION_0_PD_TCAM_STRENGTHf, TERMINATION_0_PD_ISA_STRENGTHf, TERMINATION_0_PD_ISB_STRENGTHf},
              {TERMINATION_1_PD_TCAM_STRENGTHf, TERMINATION_1_PD_TCAM_STRENGTHf, TERMINATION_1_PD_ISA_STRENGTHf, TERMINATION_1_PD_ISB_STRENGTHf}
          };

      res = arad_pp_ihp_vtt_flp_key_construction_pd_bitmap_set_unsafe(
              unit,
              FALSE, /* is_for_flp */
              data_tbl_1,
              IHP_VTT_2ND_LOOKUP_PROGRAM_1m, 
              strength_fields[0],
              tbl_data->termination_0_pd_bitmap );
      SOC_SAND_CHECK_FUNC_RESULT(res, 53, exit);

      res = arad_pp_ihp_vtt_flp_key_construction_pd_bitmap_set_unsafe(
              unit,
              FALSE, /* is_for_flp */
              data_tbl_1,
              IHP_VTT_2ND_LOOKUP_PROGRAM_1m, 
              strength_fields[1],
              tbl_data->termination_1_pd_bitmap );
      SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);
  }

  soc_mem_field32_set(
          unit,
          IHP_VTT_2ND_LOOKUP_PROGRAM_1m,
          data_tbl_1,
          PROCESSING_PROFILEf,
          tbl_data->processing_profile );
  SOC_SAND_CHECK_FUNC_RESULT(res, 58, exit);

  soc_mem_field32_set(
          unit,
          IHP_VTT_2ND_LOOKUP_PROGRAM_1m,
          data_tbl_1,
          PROCESSING_CODEf,
          tbl_data->processing_code );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  soc_mem_field32_set(
          unit,
          IHP_VTT_2ND_LOOKUP_PROGRAM_1m,
          data_tbl_1,
          RESULT_TO_USE_0f,
          tbl_data->result_to_use_0 );
  SOC_SAND_CHECK_FUNC_RESULT(res, 62, exit);

  soc_mem_field32_set(
          unit,
          IHP_VTT_2ND_LOOKUP_PROGRAM_1m,
          data_tbl_1,
          RESULT_TO_USE_1f,
          tbl_data->result_to_use_1 );
  SOC_SAND_CHECK_FUNC_RESULT(res, 64, exit);

  soc_mem_field32_set(
          unit,
          IHP_VTT_2ND_LOOKUP_PROGRAM_1m,
          data_tbl_1,
          RESULT_TO_USE_2f,
          tbl_data->result_to_use_2 );
  SOC_SAND_CHECK_FUNC_RESULT(res, 66, exit);

  soc_mem_field32_set(
          unit,
          IHP_VTT_2ND_LOOKUP_PROGRAM_1m,
          data_tbl_1,
          SECOND_STAGE_PARSINGf,
          tbl_data->second_stage_parsing );
  SOC_SAND_CHECK_FUNC_RESULT(res, 68, exit);

  res = soc_mem_write(
          unit,
          IHP_VTT_2ND_LOOKUP_PROGRAM_1m,
          MEM_BLOCK_ANY,
          entry_offset,
          data_tbl_1          
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 70, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_vtt2nd_key_construction_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihb_fec_entry_accessed_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHB_FEC_ENTRY_ACCESSED_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHB_FEC_ENTRY_ACCESSED_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_FEC_ENTRY_ACCESSED_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHB_FEC_ENTRY_ACCESSED_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHB_FEC_ENTRY_ACCESSED_TBL_DATA, 1);


  res = READ_IHB_FEC_ENTRY_ACCESSEDm(
          unit,
          ARAD_PP_TBL_FEC_ARRAY_INDEX(entry_offset),
          MEM_BLOCK_ANY,
          ARAD_PP_TBL_FEC_ENTRY_INDEX(entry_offset),
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

          tbl_data->fec_entry_accessed   = soc_mem_field32_get(
                  unit,
                  IHB_FEC_ENTRY_ACCESSEDm,
                  data,
                  FEC_ENTRY_ACCESSEDf            );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_fec_entry_accessed_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihb_fec_entry_accessed_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHB_FEC_ENTRY_ACCESSED_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHB_FEC_ENTRY_ACCESSED_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_FEC_ENTRY_ACCESSED_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHB_FEC_ENTRY_ACCESSED_TBL_ENTRY_SIZE);

   soc_mem_field32_set(
          unit,
          IHB_FEC_ENTRY_ACCESSEDm,
          data,
          FEC_ENTRY_ACCESSEDf,
          tbl_data->fec_entry_accessed );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = WRITE_IHB_FEC_ENTRY_ACCESSEDm(
          unit,
          ARAD_PP_TBL_FEC_ARRAY_INDEX(entry_offset),
          MEM_BLOCK_ANY,
          ARAD_PP_TBL_FEC_ENTRY_INDEX(entry_offset),
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_fec_entry_accessed_tbl_set_unsafe()", entry_offset, 0);
}


uint32
  arad_pp_ihb_path_select_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_IHB_PATH_SELECT_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHB_PATH_SELECT_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_PATH_SELECT_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHB_PATH_SELECT_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHB_PATH_SELECT_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHB_PATH_SELECTm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

   tbl_data->path_select[0]   = soc_mem_field32_get(
                  unit,
                  IHB_PATH_SELECTm,
                  data,
                  PATH_SELECT_0f            );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

   tbl_data->path_select[1]   = soc_mem_field32_get(
                  unit,
                  IHB_PATH_SELECTm,
                  data,
                  PATH_SELECT_1f            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

   tbl_data->path_select[2]   = soc_mem_field32_get(
                  unit,
                  IHB_PATH_SELECTm,
                  data,
                  PATH_SELECT_2f            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

   tbl_data->path_select[3]   = soc_mem_field32_get(
                  unit,
                  IHB_PATH_SELECTm,
                  data,
                  PATH_SELECT_3f            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  tbl_data->path_select[4]   = soc_mem_field32_get(
                  unit,
                  IHB_PATH_SELECTm,
                  data,
                  PATH_SELECT_4f            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

          tbl_data->path_select[5]   = soc_mem_field32_get(
                  unit,
                  IHB_PATH_SELECTm,
                  data,
                  PATH_SELECT_5f            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  tbl_data->path_select[6]   = soc_mem_field32_get(
                  unit,
                  IHB_PATH_SELECTm,
                  data,
                  PATH_SELECT_6f            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

   tbl_data->path_select[7]   = soc_mem_field32_get(
                  unit,
                  IHB_PATH_SELECTm,
                  data,
                  PATH_SELECT_7f            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_path_select_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihb_path_select_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHB_PATH_SELECT_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHB_PATH_SELECT_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_PATH_SELECT_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHB_PATH_SELECT_TBL_ENTRY_SIZE);
        soc_mem_field32_set(
          unit,
          IHB_PATH_SELECTm,
          data,
          PATH_SELECT_0f,
          tbl_data->path_select[0] );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

            soc_mem_field32_set(
          unit,
          IHB_PATH_SELECTm,
          data,
          PATH_SELECT_1f,
          tbl_data->path_select[1] );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

            soc_mem_field32_set(
          unit,
          IHB_PATH_SELECTm,
          data,
          PATH_SELECT_2f,
          tbl_data->path_select[2] );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

            soc_mem_field32_set(
          unit,
          IHB_PATH_SELECTm,
          data,
          PATH_SELECT_3f,
          tbl_data->path_select[3] );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

            soc_mem_field32_set(
          unit,
          IHB_PATH_SELECTm,
          data,
          PATH_SELECT_4f,
          tbl_data->path_select[4] );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

            soc_mem_field32_set(
          unit,
          IHB_PATH_SELECTm,
          data,
          PATH_SELECT_5f,
          tbl_data->path_select[5] );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

            soc_mem_field32_set(
          unit,
          IHB_PATH_SELECTm,
          data,
          PATH_SELECT_6f,
          tbl_data->path_select[6] );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

            soc_mem_field32_set(
          unit,
          IHB_PATH_SELECTm,
          data,
          PATH_SELECT_7f,
          tbl_data->path_select[7] );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  res = soc_mem_write(
          unit,
          IHB_PATH_SELECTm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 38, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_path_select_tbl_set_unsafe()", entry_offset, 0);
}


uint32
  arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[SOC_DPP_DEFS_MAX(IHP_FLP_PROGRAM_SELECTION_CAM_NOF_LONGS)];
  soc_mem_t
    mem;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, SOC_DPP_DEFS_MAX(IHP_FLP_PROGRAM_SELECTION_CAM_NOF_LONGS));
  ARAD_CLEAR(tbl_data, ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA, 1);

  if(SOC_IS_JERICHO(unit)) {
    mem = IHP_FLP_PROGRAM_SELECTION_CAMm;
  } else {
    mem = IHB_FLP_PROGRAM_SELECTION_CAMm;
  }

  res = soc_mem_read(
          unit,
          mem,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->parser_leaf_context = soc_mem_field32_get(unit, mem, data, PARSER_LEAF_CONTEXTf);
  tbl_data->port_profile = soc_mem_field32_get(unit, mem, data, PORT_PROFILEf);
  tbl_data->ptc_profile = soc_mem_field32_get(unit, mem, data, PTC_PROFILEf);
  tbl_data->packet_format_code = soc_mem_field32_get(unit, mem, data, PACKET_FORMAT_CODEf);
  tbl_data->forwarding_header_qualifier = soc_mem_field32_get(unit, mem, data, FORWARDING_HEADER_QUALIFIERf);
  tbl_data->forwarding_code = soc_mem_field32_get(unit, mem, data, FORWARDING_CODEf);
  tbl_data->forwarding_offset_index = soc_mem_field32_get(unit, mem, data, FORWARDING_OFFSET_INDEXf);
  tbl_data->trill_mc = soc_mem_field32_get(unit, mem, data, TRILL_MCf);
  tbl_data->packet_is_compatible_mc = soc_mem_field32_get(unit, mem, data, PACKET_IS_COMPATIBLE_MCf);
  tbl_data->ll_is_arp = soc_mem_field32_get(unit, mem, data, LL_IS_ARPf);
  tbl_data->elk_status = soc_mem_field32_get(unit, mem, data, ELK_STATUSf);
  tbl_data->cos_profile = soc_mem_field32_get(unit, mem, data, COS_PROFILEf);
  tbl_data->service_type = soc_mem_field32_get(unit, mem, data, SERVICE_TYPEf);
  tbl_data->vt_processing_profile = soc_mem_field32_get(unit, mem, data, VT_PROCESSING_PROFILEf);
  tbl_data->vt_lookup_0_found = soc_mem_field32_get(unit, mem, data, VT_LOOKUP_0_FOUNDf);
  tbl_data->vt_lookup_1_found = soc_mem_field32_get(unit, mem, data, VT_LOOKUP_1_FOUNDf);
  tbl_data->tt_processing_profile = soc_mem_field32_get(unit, mem, data, TT_PROCESSING_PROFILEf);
  tbl_data->tt_lookup_0_found = soc_mem_field32_get(unit, mem, data, TT_LOOKUP_0_FOUNDf);
  tbl_data->tt_lookup_1_found = soc_mem_field32_get(unit, mem, data, TT_LOOKUP_1_FOUNDf);
  tbl_data->parser_leaf_context_mask = soc_mem_field32_get(unit, mem, data, PARSER_LEAF_CONTEXT_MASKf);
  tbl_data->port_profile_mask = soc_mem_field32_get(unit, mem, data, PORT_PROFILE_MASKf);
  tbl_data->ptc_profile_mask = soc_mem_field32_get(unit, mem, data, PTC_PROFILE_MASKf);
  tbl_data->packet_format_code_mask = soc_mem_field32_get(unit, mem, data, PACKET_FORMAT_CODE_MASKf);
  tbl_data->forwarding_header_qualifier_mask = soc_mem_field32_get(unit, mem, data, FORWARDING_HEADER_QUALIFIER_MASKf);
  tbl_data->forwarding_code_mask = soc_mem_field32_get(unit, mem, data, FORWARDING_CODE_MASKf);
  tbl_data->forwarding_offset_index_mask = soc_mem_field32_get(unit, mem, data, FORWARDING_OFFSET_INDEX_MASKf);
  tbl_data->trill_mc_mask = soc_mem_field32_get(unit, mem, data, TRILL_MC_MASKf);
  tbl_data->packet_is_compatible_mc_mask = soc_mem_field32_get(unit, mem, data, PACKET_IS_COMPATIBLE_MC_MASKf);
  tbl_data->ll_is_arp_mask = soc_mem_field32_get(unit, mem, data, LL_IS_ARP_MASKf);
  tbl_data->elk_status_mask = soc_mem_field32_get(unit, mem, data, ELK_STATUS_MASKf);
  tbl_data->cos_profile_mask = soc_mem_field32_get(unit, mem, data, COS_PROFILE_MASKf);
  tbl_data->service_type_mask = soc_mem_field32_get(unit, mem, data, SERVICE_TYPE_MASKf);
  tbl_data->vt_processing_profile_mask = soc_mem_field32_get(unit, mem, data, VT_PROCESSING_PROFILE_MASKf);
  tbl_data->vt_lookup_0_found_mask = soc_mem_field32_get(unit, mem, data, VT_LOOKUP_0_FOUND_MASKf);
  tbl_data->vt_lookup_1_found_mask = soc_mem_field32_get(unit, mem, data, VT_LOOKUP_1_FOUND_MASKf);
  tbl_data->tt_processing_profile_mask = soc_mem_field32_get(unit, mem, data, TT_PROCESSING_PROFILE_MASKf);
  tbl_data->tt_lookup_0_found_mask = soc_mem_field32_get(unit, mem, data, TT_LOOKUP_0_FOUND_MASKf);
  tbl_data->tt_lookup_1_found_mask = soc_mem_field32_get(unit, mem, data, TT_LOOKUP_1_FOUND_MASKf);
  tbl_data->program = soc_mem_field32_get(unit, mem, data, PROGRAMf);
  tbl_data->valid = soc_mem_field32_get(unit, mem, data, VALIDf);

  /* In-RIF profile */
  if(SOC_IS_JERICHO(unit)) {
    
    uint32 in_rif_profile, in_rif_profile_mask;
    in_rif_profile = soc_mem_field32_get(unit, mem, data, IN_RIF_PROFILEf);
    in_rif_profile_mask = soc_mem_field32_get(unit, mem, data, IN_RIF_PROFILE_MASKf);
    tbl_data->l_3_vpn_default_routing = SHR_BITGET(&in_rif_profile, ARAD_PP_RIF_PROFILE_ENCODING_L_3_VPN_DEFAULT_ROUTING);
    tbl_data->in_rif_uc_rpf_enable = SHR_BITGET(&in_rif_profile, ARAD_PP_RIF_PROFILE_ENCODING_UC_RPF_ENABLE);
    tbl_data->l_3_vpn_default_routing_mask = SHR_BITGET(&in_rif_profile_mask, ARAD_PP_RIF_PROFILE_ENCODING_L_3_VPN_DEFAULT_ROUTING);
    tbl_data->in_rif_uc_rpf_enable_mask = SHR_BITGET(&in_rif_profile_mask, ARAD_PP_RIF_PROFILE_ENCODING_UC_RPF_ENABLE);
  } else {
    tbl_data->l_3_vpn_default_routing = soc_mem_field32_get(unit, mem, data, L_3_VPN_DEFAULT_ROUTINGf);
    tbl_data->in_rif_uc_rpf_enable = soc_mem_field32_get(unit, mem, data, IN_RIF_UC_RPF_ENABLEf);
    tbl_data->l_3_vpn_default_routing_mask = soc_mem_field32_get(unit, mem, data, L_3_VPN_DEFAULT_ROUTING_MASKf);
    tbl_data->in_rif_uc_rpf_enable_mask = soc_mem_field32_get(unit, mem, data, IN_RIF_UC_RPF_ENABLE_MASKf);
  }

  /* New fields */
  if (SOC_IS_JERICHO(unit)) {
    tbl_data->forwarding_offset_index_ext = soc_mem_field32_get(unit, mem, data, FORWARDING_OFFSET_INDEX_EXTf);
    tbl_data->cpu_trap_code = soc_mem_field32_get(unit, mem, data, CPU_TRAP_CODEf);
    tbl_data->in_lif_profile = soc_mem_field32_get(unit, mem, data, IN_LIF_PROFILEf);
    tbl_data->llvp_incoming_tag_structure = soc_mem_field32_get(unit, mem, data, LLVP_INCOMING_TAG_STRUCTUREf);
    tbl_data->forwarding_plus_1_header_qualifier = soc_mem_field32_get(unit, mem, data, FORWARDING_PLUS_1_HEADER_QUALIFIERf);
    tbl_data->tunnel_termination_code = soc_mem_field32_get(unit, mem, data, TUNNEL_TERMINATION_CODEf);
    tbl_data->qualifier_0 = soc_mem_field32_get(unit, mem, data, QUALIFIER_0f);
    tbl_data->in_lif_data_index = soc_mem_field32_get(unit, mem, data, IN_LIF_DATA_INDEXf);
    tbl_data->in_lif_data_msb = soc_mem_field32_get(unit, mem, data, IN_LIF_DATA_MSBf);
    tbl_data->forwarding_offset_index_ext_mask = soc_mem_field32_get(unit, mem, data, FORWARDING_OFFSET_INDEX_EXT_MASKf);
    tbl_data->cpu_trap_code_mask = soc_mem_field32_get(unit, mem, data, CPU_TRAP_CODE_MASKf);
    tbl_data->in_lif_profile_mask = soc_mem_field32_get(unit, mem, data, IN_LIF_PROFILE_MASKf);
    tbl_data->llvp_incoming_tag_structure_mask = soc_mem_field32_get(unit, mem, data, LLVP_INCOMING_TAG_STRUCTURE_MASKf);
    tbl_data->forwarding_plus_1_header_qualifier_mask = soc_mem_field32_get(unit, mem, data, FORWARDING_PLUS_1_HEADER_QUALIFIER_MASKf);
    tbl_data->tunnel_termination_code_mask = soc_mem_field32_get(unit, mem, data, TUNNEL_TERMINATION_CODE_MASKf);
    tbl_data->qualifier_0_mask = soc_mem_field32_get(unit, mem, data, QUALIFIER_0_MASKf);
    tbl_data->in_lif_data_index_mask = soc_mem_field32_get(unit, mem, data, IN_LIF_DATA_INDEX_MASKf);
    tbl_data->in_lif_data_msb_mask = soc_mem_field32_get(unit, mem, data, IN_LIF_DATA_MSB_MASKf);
  } 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_flp_program_selection_cam_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[SOC_DPP_DEFS_MAX(IHP_FLP_PROGRAM_SELECTION_CAM_NOF_LONGS)];
  soc_mem_t
    mem;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_FLP_PROGRAM_SELECTION_CAM_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, SOC_DPP_DEFS_MAX(IHP_FLP_PROGRAM_SELECTION_CAM_NOF_LONGS));

  if(SOC_IS_JERICHO(unit)) {
    mem = IHP_FLP_PROGRAM_SELECTION_CAMm;
  } else {
    mem = IHB_FLP_PROGRAM_SELECTION_CAMm;
  }

  soc_mem_field32_set(unit, mem, data, PARSER_LEAF_CONTEXTf, tbl_data->parser_leaf_context);
  soc_mem_field32_set(unit, mem, data, PORT_PROFILEf, tbl_data->port_profile);
  soc_mem_field32_set(unit, mem, data, PTC_PROFILEf, tbl_data->ptc_profile);
  soc_mem_field32_set(unit, mem, data, PACKET_FORMAT_CODEf, tbl_data->packet_format_code);
  soc_mem_field32_set(unit, mem, data, FORWARDING_HEADER_QUALIFIERf, tbl_data->forwarding_header_qualifier);
  soc_mem_field32_set(unit, mem, data, FORWARDING_CODEf, tbl_data->forwarding_code);
  soc_mem_field32_set(unit, mem, data, FORWARDING_OFFSET_INDEXf, tbl_data->forwarding_offset_index);
  soc_mem_field32_set(unit, mem, data, TRILL_MCf, tbl_data->trill_mc);
  soc_mem_field32_set(unit, mem, data, PACKET_IS_COMPATIBLE_MCf, tbl_data->packet_is_compatible_mc);
  soc_mem_field32_set(unit, mem, data, LL_IS_ARPf, tbl_data->ll_is_arp);
  soc_mem_field32_set(unit, mem, data, ELK_STATUSf, tbl_data->elk_status);
  soc_mem_field32_set(unit, mem, data, COS_PROFILEf, tbl_data->cos_profile);
  soc_mem_field32_set(unit, mem, data, SERVICE_TYPEf, tbl_data->service_type);
  soc_mem_field32_set(unit, mem, data, VT_PROCESSING_PROFILEf, tbl_data->vt_processing_profile);
  soc_mem_field32_set(unit, mem, data, VT_LOOKUP_0_FOUNDf, tbl_data->vt_lookup_0_found);
  soc_mem_field32_set(unit, mem, data, VT_LOOKUP_1_FOUNDf, tbl_data->vt_lookup_1_found);
  soc_mem_field32_set(unit, mem, data, TT_PROCESSING_PROFILEf, tbl_data->tt_processing_profile);
  soc_mem_field32_set(unit, mem, data, TT_LOOKUP_0_FOUNDf, tbl_data->tt_lookup_0_found);
  soc_mem_field32_set(unit, mem, data, TT_LOOKUP_1_FOUNDf, tbl_data->tt_lookup_1_found);
  soc_mem_field32_set(unit, mem, data, PARSER_LEAF_CONTEXT_MASKf, tbl_data->parser_leaf_context_mask);
  soc_mem_field32_set(unit, mem, data, PORT_PROFILE_MASKf, tbl_data->port_profile_mask);
  soc_mem_field32_set(unit, mem, data, PTC_PROFILE_MASKf, tbl_data->ptc_profile_mask);
  soc_mem_field32_set(unit, mem, data, PACKET_FORMAT_CODE_MASKf, tbl_data->packet_format_code_mask);
  soc_mem_field32_set(unit, mem, data, FORWARDING_HEADER_QUALIFIER_MASKf, tbl_data->forwarding_header_qualifier_mask);
  soc_mem_field32_set(unit, mem, data, FORWARDING_CODE_MASKf, tbl_data->forwarding_code_mask);
  soc_mem_field32_set(unit, mem, data, FORWARDING_OFFSET_INDEX_MASKf, tbl_data->forwarding_offset_index_mask);
  soc_mem_field32_set(unit, mem, data, TRILL_MC_MASKf, tbl_data->trill_mc_mask);
  soc_mem_field32_set(unit, mem, data, PACKET_IS_COMPATIBLE_MC_MASKf, tbl_data->packet_is_compatible_mc_mask);
  soc_mem_field32_set(unit, mem, data, LL_IS_ARP_MASKf, tbl_data->ll_is_arp_mask);
  soc_mem_field32_set(unit, mem, data, ELK_STATUS_MASKf, tbl_data->elk_status_mask);
  soc_mem_field32_set(unit, mem, data, COS_PROFILE_MASKf, tbl_data->cos_profile_mask);
  soc_mem_field32_set(unit, mem, data, SERVICE_TYPE_MASKf, tbl_data->service_type_mask);
  soc_mem_field32_set(unit, mem, data, VT_PROCESSING_PROFILE_MASKf, tbl_data->vt_processing_profile_mask);
  soc_mem_field32_set(unit, mem, data, VT_LOOKUP_0_FOUND_MASKf, tbl_data->vt_lookup_0_found_mask);
  soc_mem_field32_set(unit, mem, data, VT_LOOKUP_1_FOUND_MASKf, tbl_data->vt_lookup_1_found_mask);
  soc_mem_field32_set(unit, mem, data, TT_PROCESSING_PROFILE_MASKf, tbl_data->tt_processing_profile_mask);
  soc_mem_field32_set(unit, mem, data, TT_LOOKUP_0_FOUND_MASKf, tbl_data->tt_lookup_0_found_mask);
  soc_mem_field32_set(unit, mem, data, TT_LOOKUP_1_FOUND_MASKf, tbl_data->tt_lookup_1_found_mask);
  soc_mem_field32_set(unit, mem, data, PROGRAMf, tbl_data->program);
  soc_mem_field32_set(unit, mem, data, VALIDf, tbl_data->valid);

  /* In-RIF profile */
  if(SOC_IS_JERICHO(unit)) {
    
    uint32 in_rif_profile = 0, in_rif_profile_mask = 0;
    /* Set the mask to -1 by default */
    SHR_BITSET_RANGE(&in_rif_profile_mask, 0, soc_mem_field_length(unit, mem, IN_RIF_PROFILE_MASKf));
    /* Copy the relevant bits */
    SHR_BITCOPY_RANGE(&in_rif_profile, ARAD_PP_RIF_PROFILE_ENCODING_L_3_VPN_DEFAULT_ROUTING, &tbl_data->l_3_vpn_default_routing, 0, 1);
    SHR_BITCOPY_RANGE(&in_rif_profile, ARAD_PP_RIF_PROFILE_ENCODING_UC_RPF_ENABLE, &tbl_data->in_rif_uc_rpf_enable, 0, 1);
    SHR_BITCOPY_RANGE(&in_rif_profile_mask, ARAD_PP_RIF_PROFILE_ENCODING_L_3_VPN_DEFAULT_ROUTING, &tbl_data->l_3_vpn_default_routing_mask, 0, 1);
    SHR_BITCOPY_RANGE(&in_rif_profile_mask, ARAD_PP_RIF_PROFILE_ENCODING_UC_RPF_ENABLE, &tbl_data->in_rif_uc_rpf_enable_mask, 0, 1);
    /* Set the HW */
    soc_mem_field32_set(unit, mem, data, IN_RIF_PROFILEf, in_rif_profile);
    soc_mem_field32_set(unit, mem, data, IN_RIF_PROFILE_MASKf, in_rif_profile_mask);
  } else {
    soc_mem_field32_set(unit, mem, data, L_3_VPN_DEFAULT_ROUTINGf, tbl_data->l_3_vpn_default_routing);
    soc_mem_field32_set(unit, mem, data, IN_RIF_UC_RPF_ENABLEf, tbl_data->in_rif_uc_rpf_enable);
    soc_mem_field32_set(unit, mem, data, L_3_VPN_DEFAULT_ROUTING_MASKf, tbl_data->l_3_vpn_default_routing_mask);
    soc_mem_field32_set(unit, mem, data, IN_RIF_UC_RPF_ENABLE_MASKf, tbl_data->in_rif_uc_rpf_enable_mask);
  }

  /* New fields */
  if (SOC_IS_JERICHO(unit)) {
    soc_mem_field32_set(unit, mem, data, FORWARDING_OFFSET_INDEX_EXTf, tbl_data->forwarding_offset_index_ext);
    soc_mem_field32_set(unit, mem, data, CPU_TRAP_CODEf, tbl_data->cpu_trap_code);
    soc_mem_field32_set(unit, mem, data, IN_LIF_PROFILEf, tbl_data->in_lif_profile);
    soc_mem_field32_set(unit, mem, data, LLVP_INCOMING_TAG_STRUCTUREf, tbl_data->llvp_incoming_tag_structure);
    soc_mem_field32_set(unit, mem, data, FORWARDING_PLUS_1_HEADER_QUALIFIERf, tbl_data->forwarding_plus_1_header_qualifier);
    soc_mem_field32_set(unit, mem, data, TUNNEL_TERMINATION_CODEf, tbl_data->tunnel_termination_code);
    soc_mem_field32_set(unit, mem, data, QUALIFIER_0f, tbl_data->qualifier_0);
    soc_mem_field32_set(unit, mem, data, IN_LIF_DATA_INDEXf, tbl_data->in_lif_data_index);
    soc_mem_field32_set(unit, mem, data, IN_LIF_DATA_MSBf, tbl_data->in_lif_data_msb);
    soc_mem_field32_set(unit, mem, data, FORWARDING_OFFSET_INDEX_EXT_MASKf, tbl_data->forwarding_offset_index_ext_mask);
    soc_mem_field32_set(unit, mem, data, CPU_TRAP_CODE_MASKf, tbl_data->cpu_trap_code_mask);
    soc_mem_field32_set(unit, mem, data, IN_LIF_PROFILE_MASKf, tbl_data->in_lif_profile_mask);
    soc_mem_field32_set(unit, mem, data, LLVP_INCOMING_TAG_STRUCTURE_MASKf, tbl_data->llvp_incoming_tag_structure_mask);
    soc_mem_field32_set(unit, mem, data, FORWARDING_PLUS_1_HEADER_QUALIFIER_MASKf, tbl_data->forwarding_plus_1_header_qualifier_mask);
    soc_mem_field32_set(unit, mem, data, TUNNEL_TERMINATION_CODE_MASKf, tbl_data->tunnel_termination_code_mask);
    soc_mem_field32_set(unit, mem, data, QUALIFIER_0_MASKf, tbl_data->qualifier_0_mask);
    soc_mem_field32_set(unit, mem, data, IN_LIF_DATA_INDEX_MASKf, tbl_data->in_lif_data_index_mask);
    soc_mem_field32_set(unit, mem, data, IN_LIF_DATA_MSB_MASKf, tbl_data->in_lif_data_msb_mask);
  } 

  res = soc_mem_write(
          unit,
          mem,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_flp_program_selection_cam_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihb_flp_process_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHB_FLP_PROCESS_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHB_FLP_PROCESS_TBL_ENTRY_SIZE];
  soc_field_t
      strength_fld;
  soc_mem_t
    flp_process_mem;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_FLP_PROCESS_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHB_FLP_PROCESS_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHB_FLP_PROCESS_TBL_DATA, 1);

  if (SOC_IS_JERICHO(unit)) {
      flp_process_mem = IHP_FLP_PROCESSm;
  } else {
      flp_process_mem = IHB_FLP_PROCESSm;
  }

  res = soc_mem_read(
          unit,
          flp_process_mem,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
      /* This field is not present in Arad+, see IHP_ELK_PAYLOAD_FORMATm */
      tbl_data->elk_result_format = soc_mem_field32_get(unit, flp_process_mem, data, ELK_RESULT_FORMATf);
  }

  /* Translation 1x1 in strength between Arad-B0 and Arad+ between present bit and strength value */
  strength_fld = SOC_IS_ARADPLUS(unit)? ELK_FWD_IN_RESULT_A_STRENGTHf: INCLUDE_ELK_FWD_IN_RESULT_Af;
  tbl_data->include_elk_fwd_in_result_a = soc_mem_field32_get(unit, flp_process_mem, data, strength_fld);
  strength_fld = SOC_IS_ARADPLUS(unit)? LEM_2ND_IN_RESULT_A_STRENGTHf: INCLUDE_LEM_2ND_IN_RESULT_Af;
  tbl_data->include_lem_2nd_in_result_a = soc_mem_field32_get(unit, flp_process_mem, data, strength_fld);
  strength_fld = SOC_IS_JERICHO(unit)? TCAM_0_IN_RESULT_A_STRENGTHf: (SOC_IS_ARADPLUS(unit)? TCAM_IN_RESULT_A_STRENGTHf: INCLUDE_TCAM_IN_RESULT_Af);
  tbl_data->include_tcam_in_result_a = soc_mem_field32_get(unit, flp_process_mem, data, strength_fld);
  strength_fld = SOC_IS_ARADPLUS(unit)? LPM_2ND_IN_RESULT_A_STRENGTHf: INCLUDE_LPM_2ND_IN_RESULT_Af;
  tbl_data->include_lpm_2nd_in_result_a = soc_mem_field32_get(unit, flp_process_mem, data, strength_fld);
  strength_fld = SOC_IS_ARADPLUS(unit)? ELK_EXT_IN_RESULT_A_STRENGTHf: INCLUDE_ELK_EXT_IN_RESULT_Af;
  tbl_data->include_elk_ext_in_result_a = soc_mem_field32_get(unit, flp_process_mem, data, strength_fld);
  strength_fld = SOC_IS_ARADPLUS(unit)? LEM_1ST_IN_RESULT_A_STRENGTHf: INCLUDE_LEM_1ST_IN_RESULT_Af;
  tbl_data->include_lem_1st_in_result_a = soc_mem_field32_get(unit, flp_process_mem, data, strength_fld);
  strength_fld = SOC_IS_ARADPLUS(unit)? LPM_1ST_IN_RESULT_A_STRENGTHf: INCLUDE_LPM_1ST_IN_RESULT_Af;
  tbl_data->include_lpm_1st_in_result_a = soc_mem_field32_get(unit, flp_process_mem, data, strength_fld);

  strength_fld = SOC_IS_ARADPLUS(unit)? ELK_FWD_IN_RESULT_B_STRENGTHf: INCLUDE_ELK_FWD_IN_RESULT_Bf;
  tbl_data->include_elk_fwd_in_result_b = soc_mem_field32_get(unit, flp_process_mem, data, strength_fld);
  strength_fld = SOC_IS_ARADPLUS(unit)? LEM_2ND_IN_RESULT_B_STRENGTHf: INCLUDE_LEM_2ND_IN_RESULT_Bf;
  tbl_data->include_lem_2nd_in_result_b = soc_mem_field32_get(unit, flp_process_mem, data, strength_fld);
  strength_fld = SOC_IS_JERICHO(unit)? TCAM_0_IN_RESULT_B_STRENGTHf: (SOC_IS_ARADPLUS(unit)? TCAM_IN_RESULT_B_STRENGTHf: INCLUDE_TCAM_IN_RESULT_Bf);
  tbl_data->include_tcam_in_result_b = soc_mem_field32_get(unit, flp_process_mem, data, strength_fld);
  strength_fld = SOC_IS_ARADPLUS(unit)? LPM_2ND_IN_RESULT_B_STRENGTHf: INCLUDE_LPM_2ND_IN_RESULT_Bf;
  tbl_data->include_lpm_2nd_in_result_b = soc_mem_field32_get(unit, flp_process_mem, data, strength_fld);
  strength_fld = SOC_IS_ARADPLUS(unit)? ELK_EXT_IN_RESULT_B_STRENGTHf: INCLUDE_ELK_EXT_IN_RESULT_Bf;
  tbl_data->include_elk_ext_in_result_b = soc_mem_field32_get(unit, flp_process_mem, data, strength_fld);
  strength_fld = SOC_IS_ARADPLUS(unit)? LEM_1ST_IN_RESULT_B_STRENGTHf: INCLUDE_LEM_1ST_IN_RESULT_Bf;
  tbl_data->include_lem_1st_in_result_b = soc_mem_field32_get(unit, flp_process_mem, data, strength_fld);
  strength_fld = SOC_IS_ARADPLUS(unit)? LPM_1ST_IN_RESULT_B_STRENGTHf: INCLUDE_LPM_1ST_IN_RESULT_Bf;
  tbl_data->include_lpm_1st_in_result_b = soc_mem_field32_get(unit, flp_process_mem, data, strength_fld);

  tbl_data->result_a_format = soc_mem_field32_get(unit, flp_process_mem, data, RESULT_A_FORMATf);
  tbl_data->result_b_format = soc_mem_field32_get(unit, flp_process_mem, data, RESULT_B_FORMATf);
  tbl_data->not_found_trap_code = soc_mem_field32_get(unit, flp_process_mem, data, NOT_FOUND_TRAP_CODEf);
  tbl_data->not_found_trap_strength = soc_mem_field32_get(unit, flp_process_mem, data, NOT_FOUND_TRAP_STRENGTHf);
  tbl_data->not_found_snoop_strength = soc_mem_field32_get(unit, flp_process_mem, data, NOT_FOUND_SNOOP_STRENGTHf);
  tbl_data->sa_lkp_result_select = soc_mem_field32_get(unit, flp_process_mem, data, SA_LKP_RESULT_SELECTf);
  tbl_data->apply_fwd_result_a = soc_mem_field32_get(unit, flp_process_mem, data, APPLY_FWD_RESULT_Af);
  tbl_data->sa_lkp_process_enable = soc_mem_field32_get(unit, flp_process_mem, data, SA_LKP_PROCESS_ENABLEf);
  tbl_data->procedure_ethernet_default = soc_mem_field32_get(unit, flp_process_mem, data, PROCEDURE_ETHERNET_DEFAULTf);
  tbl_data->unknown_address = soc_mem_field32_get(unit, flp_process_mem, data, UNKNOWN_ADDRESSf);
  tbl_data->enable_hair_pin_filter = soc_mem_field32_get(unit, flp_process_mem, data, ENABLE_HAIR_PIN_FILTERf);
  tbl_data->enable_rpf_check = soc_mem_field32_get(unit, flp_process_mem, data, ENABLE_RPF_CHECKf);
  tbl_data->compatible_mc_bridge_fallback = soc_mem_field32_get(unit, flp_process_mem, data, COMPATIBLE_MC_BRIDGE_FALLBACKf);
  tbl_data->enable_lsr_p2p_service = soc_mem_field32_get(unit, flp_process_mem, data, ENABLE_LSR_P2P_SERVICEf);
  tbl_data->learn_enable = soc_mem_field32_get(unit, flp_process_mem, data, LEARN_ENABLEf);
  tbl_data->fwd_processing_profile = soc_mem_field32_get(unit, flp_process_mem, data, FWD_PROCESSING_PROFILEf);
  
  if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
    tbl_data->select_default_result_a = soc_mem_field32_get(unit, flp_process_mem, data, SELECT_DEFAULT_RESULT_Af);
    tbl_data->select_default_result_b = soc_mem_field32_get(unit, flp_process_mem, data, SELECT_DEFAULT_RESULT_Bf);
  }

  /* Jericho new fields */
  if (SOC_IS_JERICHO(unit)) {
    tbl_data->include_tcam_1_in_result_a = soc_mem_field32_get(unit, flp_process_mem, data, TCAM_1_IN_RESULT_A_STRENGTHf);
    tbl_data->include_tcam_1_in_result_b = soc_mem_field32_get(unit, flp_process_mem, data, TCAM_1_IN_RESULT_B_STRENGTHf);
    tbl_data->lpm_1st_lkp_enable_default = soc_mem_field32_get(unit, flp_process_mem, data, LPM_1ST_LKP_ENABLE_DEFAULTf);
    tbl_data->lpm_2nd_lkp_enable_default = soc_mem_field32_get(unit, flp_process_mem, data, LPM_2ND_LKP_ENABLE_DEFAULTf);
    tbl_data->lpm_public_1st_lkp_enable_default = soc_mem_field32_get(unit, flp_process_mem, data, LPM_PUBLIC_1ST_LKP_ENABLE_DEFAULTf);
    tbl_data->lpm_public_2nd_lkp_enable_default = soc_mem_field32_get(unit, flp_process_mem, data, LPM_PUBLIC_2ND_LKP_ENABLE_DEFAULTf);
    tbl_data->elk_fwd_lkp_enable_default = soc_mem_field32_get(unit, flp_process_mem, data, ELK_FWD_LKP_ENABLE_DEFAULTf);
    tbl_data->elk_ext_lkp_enable_default = soc_mem_field32_get(unit, flp_process_mem, data, ELK_EXT_LKP_ENABLE_DEFAULTf);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_flp_process_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihb_flp_process_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHB_FLP_PROCESS_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHB_FLP_PROCESS_TBL_ENTRY_SIZE];
  soc_field_t
      strength_fld;
  soc_mem_t
    flp_process_mem;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_FLP_PROCESS_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHB_FLP_PROCESS_TBL_ENTRY_SIZE);

  if (SOC_IS_JERICHO(unit)) {
      flp_process_mem = IHP_FLP_PROCESSm;
  } else {
      flp_process_mem = IHB_FLP_PROCESSm;
  }

  if (SOC_IS_ARAD_B1_AND_BELOW(unit)) {
      /* This field is not present in Arad+, see IHP_ELK_PAYLOAD_FORMATm */
      soc_mem_field32_set(unit, flp_process_mem, data, ELK_RESULT_FORMATf, tbl_data->elk_result_format);
  }

  /* Translation 1x1 in strength between Arad-B0 and Arad+ between present bit and strength value */
  strength_fld = SOC_IS_ARADPLUS(unit)? ELK_FWD_IN_RESULT_A_STRENGTHf: INCLUDE_ELK_FWD_IN_RESULT_Af;
  soc_mem_field32_set(unit, flp_process_mem, data, strength_fld, tbl_data->include_elk_fwd_in_result_a);
  strength_fld = SOC_IS_ARADPLUS(unit)? LEM_2ND_IN_RESULT_A_STRENGTHf: INCLUDE_LEM_2ND_IN_RESULT_Af;
  soc_mem_field32_set(unit, flp_process_mem, data, strength_fld, tbl_data->include_lem_2nd_in_result_a);
  strength_fld = SOC_IS_JERICHO(unit)? TCAM_0_IN_RESULT_A_STRENGTHf: (SOC_IS_ARADPLUS(unit)? TCAM_IN_RESULT_A_STRENGTHf: INCLUDE_TCAM_IN_RESULT_Af);
  soc_mem_field32_set(unit, flp_process_mem, data, strength_fld, tbl_data->include_tcam_in_result_a);
  strength_fld = SOC_IS_ARADPLUS(unit)? LPM_2ND_IN_RESULT_A_STRENGTHf: INCLUDE_LPM_2ND_IN_RESULT_Af;
  soc_mem_field32_set(unit, flp_process_mem, data, strength_fld, tbl_data->include_lpm_2nd_in_result_a);
  strength_fld = SOC_IS_ARADPLUS(unit)? ELK_EXT_IN_RESULT_A_STRENGTHf: INCLUDE_ELK_EXT_IN_RESULT_Af;
  soc_mem_field32_set(unit, flp_process_mem, data, strength_fld, tbl_data->include_elk_ext_in_result_a);
  strength_fld = SOC_IS_ARADPLUS(unit)? LEM_1ST_IN_RESULT_A_STRENGTHf: INCLUDE_LEM_1ST_IN_RESULT_Af;
  soc_mem_field32_set(unit, flp_process_mem, data, strength_fld, tbl_data->include_lem_1st_in_result_a);
  strength_fld = SOC_IS_ARADPLUS(unit)? LPM_1ST_IN_RESULT_A_STRENGTHf: INCLUDE_LPM_1ST_IN_RESULT_Af;
  soc_mem_field32_set(unit, flp_process_mem, data, strength_fld, tbl_data->include_lpm_1st_in_result_a);

  strength_fld = SOC_IS_ARADPLUS(unit)? ELK_FWD_IN_RESULT_B_STRENGTHf: INCLUDE_ELK_FWD_IN_RESULT_Bf;
  soc_mem_field32_set(unit, flp_process_mem, data, strength_fld, tbl_data->include_elk_fwd_in_result_b);
  strength_fld = SOC_IS_ARADPLUS(unit)? LEM_2ND_IN_RESULT_B_STRENGTHf: INCLUDE_LEM_2ND_IN_RESULT_Bf;
  soc_mem_field32_set(unit, flp_process_mem, data, strength_fld, tbl_data->include_lem_2nd_in_result_b);
  strength_fld = SOC_IS_JERICHO(unit)? TCAM_0_IN_RESULT_B_STRENGTHf: (SOC_IS_ARADPLUS(unit)? TCAM_IN_RESULT_B_STRENGTHf: INCLUDE_TCAM_IN_RESULT_Bf);
  soc_mem_field32_set(unit, flp_process_mem, data, strength_fld, tbl_data->include_tcam_in_result_b);
  strength_fld = SOC_IS_ARADPLUS(unit)? LPM_2ND_IN_RESULT_B_STRENGTHf: INCLUDE_LPM_2ND_IN_RESULT_Bf;
  soc_mem_field32_set(unit, flp_process_mem, data, strength_fld, tbl_data->include_lpm_2nd_in_result_b);
  strength_fld = SOC_IS_ARADPLUS(unit)? ELK_EXT_IN_RESULT_B_STRENGTHf: INCLUDE_ELK_EXT_IN_RESULT_Bf;
  soc_mem_field32_set(unit, flp_process_mem, data, strength_fld, tbl_data->include_elk_ext_in_result_b);
  strength_fld = SOC_IS_ARADPLUS(unit)? LEM_1ST_IN_RESULT_B_STRENGTHf: INCLUDE_LEM_1ST_IN_RESULT_Bf;
  soc_mem_field32_set(unit, flp_process_mem, data, strength_fld, tbl_data->include_lem_1st_in_result_b);
  strength_fld = SOC_IS_ARADPLUS(unit)? LPM_1ST_IN_RESULT_B_STRENGTHf: INCLUDE_LPM_1ST_IN_RESULT_Bf;
  soc_mem_field32_set(unit, flp_process_mem, data, strength_fld, tbl_data->include_lpm_1st_in_result_b);

  soc_mem_field32_set(unit, flp_process_mem, data, RESULT_A_FORMATf, tbl_data->result_a_format);
  soc_mem_field32_set(unit, flp_process_mem, data, RESULT_B_FORMATf, tbl_data->result_b_format);
  soc_mem_field32_set(unit, flp_process_mem, data, NOT_FOUND_TRAP_CODEf, tbl_data->not_found_trap_code);
  soc_mem_field32_set(unit, flp_process_mem, data, NOT_FOUND_TRAP_STRENGTHf, tbl_data->not_found_trap_strength);
  soc_mem_field32_set(unit, flp_process_mem, data, NOT_FOUND_SNOOP_STRENGTHf, tbl_data->not_found_snoop_strength);
  soc_mem_field32_set(unit, flp_process_mem, data, SA_LKP_RESULT_SELECTf, tbl_data->sa_lkp_result_select);
  soc_mem_field32_set(unit, flp_process_mem, data, APPLY_FWD_RESULT_Af, tbl_data->apply_fwd_result_a);
  soc_mem_field32_set(unit, flp_process_mem, data, SA_LKP_PROCESS_ENABLEf, tbl_data->sa_lkp_process_enable);
  soc_mem_field32_set(unit, flp_process_mem, data, PROCEDURE_ETHERNET_DEFAULTf, tbl_data->procedure_ethernet_default);
  soc_mem_field32_set(unit, flp_process_mem, data, UNKNOWN_ADDRESSf, tbl_data->unknown_address);
  soc_mem_field32_set(unit, flp_process_mem, data, ENABLE_HAIR_PIN_FILTERf, tbl_data->enable_hair_pin_filter);
  soc_mem_field32_set(unit, flp_process_mem, data, ENABLE_RPF_CHECKf, tbl_data->enable_rpf_check);
  soc_mem_field32_set(unit, flp_process_mem, data, COMPATIBLE_MC_BRIDGE_FALLBACKf, tbl_data->compatible_mc_bridge_fallback);
  soc_mem_field32_set(unit, flp_process_mem, data, ENABLE_LSR_P2P_SERVICEf, tbl_data->enable_lsr_p2p_service);
  soc_mem_field32_set(unit, flp_process_mem, data, LEARN_ENABLEf, tbl_data->learn_enable);
  soc_mem_field32_set(unit, flp_process_mem, data, FWD_PROCESSING_PROFILEf, tbl_data->fwd_processing_profile);

  if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
    soc_mem_field32_set(unit, flp_process_mem, data, SELECT_DEFAULT_RESULT_Af, tbl_data->select_default_result_a);
    soc_mem_field32_set(unit, flp_process_mem, data, SELECT_DEFAULT_RESULT_Bf, tbl_data->select_default_result_b);
  }


  /* Jericho new fields */
  if (SOC_IS_JERICHO(unit)) {
    soc_mem_field32_set(unit, flp_process_mem, data, TCAM_1_IN_RESULT_A_STRENGTHf, tbl_data->include_tcam_1_in_result_a);
    soc_mem_field32_set(unit, flp_process_mem, data, TCAM_1_IN_RESULT_B_STRENGTHf, tbl_data->include_tcam_1_in_result_b);
    soc_mem_field32_set(unit, flp_process_mem, data, LPM_1ST_LKP_ENABLE_DEFAULTf, tbl_data->lpm_1st_lkp_enable_default);
    soc_mem_field32_set(unit, flp_process_mem, data, LPM_2ND_LKP_ENABLE_DEFAULTf, tbl_data->lpm_2nd_lkp_enable_default);
    soc_mem_field32_set(unit, flp_process_mem, data, LPM_PUBLIC_1ST_LKP_ENABLE_DEFAULTf, tbl_data->lpm_public_1st_lkp_enable_default);
    soc_mem_field32_set(unit, flp_process_mem, data, LPM_PUBLIC_2ND_LKP_ENABLE_DEFAULTf, tbl_data->lpm_public_2nd_lkp_enable_default);
    soc_mem_field32_set(unit, flp_process_mem, data, ELK_FWD_LKP_ENABLE_DEFAULTf, tbl_data->elk_fwd_lkp_enable_default);
    soc_mem_field32_set(unit, flp_process_mem, data, ELK_EXT_LKP_ENABLE_DEFAULTf, tbl_data->elk_ext_lkp_enable_default);
  }

  res = soc_mem_write(
          unit,
          flp_process_mem,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_flp_process_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihb_flp_lookups_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[SOC_DPP_DEFS_MAX(IHP_FLP_LOOKUPS_NOF_LONGS)];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_FLP_LOOKUPS_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, SOC_DPP_DEFS_MAX(IHP_FLP_LOOKUPS_NOF_LONGS));
  ARAD_CLEAR(tbl_data, ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHP_FLP_LOOKUPSm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->elk_lkp_valid = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, ELK_LKP_VALIDf);
  tbl_data->elk_wait_for_reply = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, ELK_WAIT_FOR_REPLYf);
  tbl_data->elk_opcode = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, ELK_OPCODEf);
  tbl_data->elk_key_a_valid_bytes = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, ELK_KEY_A_LSB_VALID_BYTESf);
  tbl_data->elk_key_b_valid_bytes = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, ELK_KEY_B_LSB_VALID_BYTESf);
  tbl_data->elk_key_c_valid_bytes = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, ELK_KEY_C_LSB_VALID_BYTESf);
  tbl_data->elk_packet_data_select = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, ELK_PACKET_DATA_SELECTf);
  tbl_data->lem_1st_lkp_valid = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, LEM_1ST_LKP_VALIDf);
  tbl_data->lem_1st_lkp_key_select = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, LEM_1ST_LKP_KEY_SELECTf);
  tbl_data->lem_1st_lkp_key_type = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, LEM_1ST_LKP_KEY_TYPEf);
  tbl_data->lem_1st_lkp_and_value = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, LEM_1ST_LKP_AND_VALUEf);
  tbl_data->lem_1st_lkp_or_value = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, LEM_1ST_LKP_OR_VALUEf);
  tbl_data->lem_2nd_lkp_valid = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, LEM_2ND_LKP_VALIDf);
  tbl_data->lem_2nd_lkp_key_select = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, LEM_2ND_LKP_KEY_SELECTf);
  tbl_data->lem_2nd_lkp_and_value = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, LEM_2ND_LKP_AND_VALUEf);
  tbl_data->lem_2nd_lkp_or_value = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, LEM_2ND_LKP_OR_VALUEf);
  tbl_data->lpm_1st_lkp_valid = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, LPM_1ST_LKP_VALIDf);
  tbl_data->lpm_1st_lkp_key_select = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, LPM_1ST_LKP_KEY_SELECTf);
  tbl_data->lpm_1st_lkp_and_value = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, LPM_1ST_LKP_AND_VALUEf);
  tbl_data->lpm_1st_lkp_or_value = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, LPM_1ST_LKP_OR_VALUEf);
  tbl_data->lpm_2nd_lkp_valid = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, LPM_2ND_LKP_VALIDf);
  tbl_data->lpm_2nd_lkp_key_select = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, LPM_2ND_LKP_KEY_SELECTf);
  tbl_data->lpm_2nd_lkp_and_value = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, LPM_2ND_LKP_AND_VALUEf);
  tbl_data->lpm_2nd_lkp_or_value = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, LPM_2ND_LKP_OR_VALUEf);
  tbl_data->tcam_lkp_key_select = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, TCAM_LKP_KEY_SELECT_0f);
  tbl_data->tcam_lkp_db_profile = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, TCAM_LKP_DB_PROFILE_0f);
  tbl_data->tcam_traps_lkp_db_profile_0 = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, TCAM_TRAPS_LKP_DB_PROFILE_0f);
  tbl_data->tcam_traps_lkp_db_profile_1 = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, TCAM_TRAPS_LKP_DB_PROFILE_1f);
  tbl_data->enable_tcam_identification_ieee_1588 = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, ENABLE_TCAM_IDENTIFICATION_IEEE_1588f);
  tbl_data->enable_tcam_identification_oam = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, ENABLE_TCAM_IDENTIFICATION_OAMf);
  tbl_data->learn_key_select = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, LEARN_KEY_SELECTf);

  if (SOC_IS_JERICHO(unit)) {
    tbl_data->elk_key_d_lsb_valid_bytes = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, ELK_KEY_D_LSB_VALID_BYTESf);
    tbl_data->elk_key_a_msb_valid_bytes = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, ELK_KEY_A_MSB_VALID_BYTESf);
    tbl_data->elk_key_b_msb_valid_bytes = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, ELK_KEY_B_MSB_VALID_BYTESf);
    tbl_data->elk_key_c_msb_valid_bytes = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, ELK_KEY_C_MSB_VALID_BYTESf);
    tbl_data->elk_key_d_msb_valid_bytes = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, ELK_KEY_D_MSB_VALID_BYTESf);
    tbl_data->lpm_public_1st_lkp_valid = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, LPM_PUBLIC_1ST_LKP_VALIDf);
    tbl_data->lpm_public_1st_lkp_key_select = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, LPM_PUBLIC_1ST_LKP_KEY_SELECTf);
    tbl_data->lpm_public_1st_lkp_key_size = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, LPM_PUBLIC_1ST_LKP_KEY_SIZEf);
    tbl_data->lpm_public_1st_lkp_and_value = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, LPM_PUBLIC_1ST_LKP_AND_VALUEf);
    tbl_data->lpm_public_1st_lkp_or_value = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, LPM_PUBLIC_1ST_LKP_OR_VALUEf);
    tbl_data->lpm_public_2nd_lkp_valid = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, LPM_PUBLIC_2ND_LKP_VALIDf);
    tbl_data->lpm_public_2nd_lkp_key_select = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, LPM_PUBLIC_2ND_LKP_KEY_SELECTf);
    tbl_data->lpm_public_2nd_lkp_key_size = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, LPM_PUBLIC_2ND_LKP_KEY_SIZEf);
    tbl_data->lpm_public_2nd_lkp_and_value = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, LPM_PUBLIC_2ND_LKP_AND_VALUEf);
    tbl_data->lpm_public_2nd_lkp_or_value = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, LPM_PUBLIC_2ND_LKP_OR_VALUEf);
    tbl_data->tcam_lkp_key_select_1 = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, TCAM_LKP_KEY_SELECT_1f);
    tbl_data->tcam_lkp_db_profile_1 = soc_mem_field32_get(unit, IHP_FLP_LOOKUPSm, data, TCAM_LKP_DB_PROFILE_1f);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_flp_lookups_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihb_flp_lookups_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHB_FLP_LOOKUPS_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[SOC_DPP_DEFS_MAX(IHP_FLP_LOOKUPS_NOF_LONGS)];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_FLP_LOOKUPS_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, SOC_DPP_DEFS_MAX(IHP_FLP_LOOKUPS_NOF_LONGS));

  soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, ELK_LKP_VALIDf, tbl_data->elk_lkp_valid);
  soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, ELK_WAIT_FOR_REPLYf, tbl_data->elk_wait_for_reply);
  soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, ELK_OPCODEf, tbl_data->elk_opcode);
  soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, ELK_KEY_A_LSB_VALID_BYTESf, tbl_data->elk_key_a_valid_bytes);
  soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, ELK_KEY_B_LSB_VALID_BYTESf, tbl_data->elk_key_b_valid_bytes);
  soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, ELK_KEY_C_LSB_VALID_BYTESf, tbl_data->elk_key_c_valid_bytes);
  soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, ELK_PACKET_DATA_SELECTf, tbl_data->elk_packet_data_select);
  soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, LEM_1ST_LKP_VALIDf, tbl_data->lem_1st_lkp_valid);
  soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, LEM_1ST_LKP_KEY_SELECTf, tbl_data->lem_1st_lkp_key_select);
  soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, LEM_1ST_LKP_KEY_TYPEf, tbl_data->lem_1st_lkp_key_type);
  soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, LEM_1ST_LKP_AND_VALUEf, tbl_data->lem_1st_lkp_and_value);
  soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, LEM_1ST_LKP_OR_VALUEf, tbl_data->lem_1st_lkp_or_value);
  soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, LEM_2ND_LKP_VALIDf, tbl_data->lem_2nd_lkp_valid);
  soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, LEM_2ND_LKP_KEY_SELECTf, tbl_data->lem_2nd_lkp_key_select);
  soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, LEM_2ND_LKP_AND_VALUEf, tbl_data->lem_2nd_lkp_and_value);
  soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, LEM_2ND_LKP_OR_VALUEf, tbl_data->lem_2nd_lkp_or_value);
  soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, LPM_1ST_LKP_VALIDf, tbl_data->lpm_1st_lkp_valid);
  soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, LPM_1ST_LKP_KEY_SELECTf, tbl_data->lpm_1st_lkp_key_select);
  soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, LPM_1ST_LKP_AND_VALUEf, tbl_data->lpm_1st_lkp_and_value);
  soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, LPM_1ST_LKP_OR_VALUEf, tbl_data->lpm_1st_lkp_or_value);
  soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, LPM_2ND_LKP_VALIDf, tbl_data->lpm_2nd_lkp_valid);
  soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, LPM_2ND_LKP_KEY_SELECTf, tbl_data->lpm_2nd_lkp_key_select);
  soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, LPM_2ND_LKP_AND_VALUEf, tbl_data->lpm_2nd_lkp_and_value);
  soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, LPM_2ND_LKP_OR_VALUEf, tbl_data->lpm_2nd_lkp_or_value);
  soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, TCAM_LKP_KEY_SELECT_0f, tbl_data->tcam_lkp_key_select);
  soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, TCAM_LKP_DB_PROFILE_0f, tbl_data->tcam_lkp_db_profile);
  soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, TCAM_TRAPS_LKP_DB_PROFILE_0f, tbl_data->tcam_traps_lkp_db_profile_0);
  soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, TCAM_TRAPS_LKP_DB_PROFILE_1f, tbl_data->tcam_traps_lkp_db_profile_1);
  soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, ENABLE_TCAM_IDENTIFICATION_IEEE_1588f, tbl_data->enable_tcam_identification_ieee_1588);
  soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, ENABLE_TCAM_IDENTIFICATION_OAMf, tbl_data->enable_tcam_identification_oam);
  soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, LEARN_KEY_SELECTf, tbl_data->learn_key_select);

  if (SOC_IS_JERICHO(unit)) {
    soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, ELK_KEY_D_LSB_VALID_BYTESf, tbl_data->elk_key_d_lsb_valid_bytes);
    soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, ELK_KEY_A_MSB_VALID_BYTESf, tbl_data->elk_key_a_msb_valid_bytes);
    soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, ELK_KEY_B_MSB_VALID_BYTESf, tbl_data->elk_key_b_msb_valid_bytes);
    soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, ELK_KEY_C_MSB_VALID_BYTESf, tbl_data->elk_key_c_msb_valid_bytes);
    soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, ELK_KEY_D_MSB_VALID_BYTESf, tbl_data->elk_key_d_msb_valid_bytes);
    soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, LPM_PUBLIC_1ST_LKP_VALIDf, tbl_data->lpm_public_1st_lkp_valid);
    soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, LPM_PUBLIC_1ST_LKP_KEY_SELECTf, tbl_data->lpm_public_1st_lkp_key_select);
    soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, LPM_PUBLIC_1ST_LKP_KEY_SIZEf, tbl_data->lpm_public_1st_lkp_key_size);
    soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, LPM_PUBLIC_1ST_LKP_AND_VALUEf, tbl_data->lpm_public_1st_lkp_and_value);
    soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, LPM_PUBLIC_1ST_LKP_OR_VALUEf, tbl_data->lpm_public_1st_lkp_or_value);
    soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, LPM_PUBLIC_2ND_LKP_VALIDf, tbl_data->lpm_public_2nd_lkp_valid);
    soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, LPM_PUBLIC_2ND_LKP_KEY_SELECTf, tbl_data->lpm_public_2nd_lkp_key_select);
    soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, LPM_PUBLIC_2ND_LKP_KEY_SIZEf, tbl_data->lpm_public_2nd_lkp_key_size);
    soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, LPM_PUBLIC_2ND_LKP_AND_VALUEf, tbl_data->lpm_public_2nd_lkp_and_value);
    soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, LPM_PUBLIC_2ND_LKP_OR_VALUEf, tbl_data->lpm_public_2nd_lkp_or_value);
    soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, TCAM_LKP_KEY_SELECT_1f, tbl_data->tcam_lkp_key_select_1);
    soc_mem_field32_set(unit, IHP_FLP_LOOKUPSm, data, TCAM_LKP_DB_PROFILE_1f, tbl_data->tcam_lkp_db_profile_1);
  }

  res = soc_mem_write(
          unit,
          IHP_FLP_LOOKUPSm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_flp_lookups_tbl_set_unsafe()", entry_offset, 0);
}

/* 
 * Jericho porting: 
 * - given an instruction, return the correct field 
 * - during Jericho porting, use instruction 0-2 similarly, 
 * and instructions 3-5 as Jericho instructions 4-6
 */
soc_field_t
  arad_pp_ihb_flp_key_field_get(
     SOC_SAND_IN  int      unit,
     SOC_SAND_IN  uint32   instruction_id
  )
{
  switch (instruction_id) {
  case 0:
    return INSTRUCTION_0_16Bf;
    break;
  case 1:
    return INSTRUCTION_1_16Bf;
    break;
  case 2:
    return INSTRUCTION_2_16Bf;
    break;
  case 3:
#if 0 
    return (SOC_IS_JERICHO(unit)? INSTRUCTION_3_16Bf: INSTRUCTION_3_32Bf));
#endif 
    return (SOC_IS_JERICHO(unit)? INSTRUCTION_4_32Bf: INSTRUCTION_3_32Bf);
    break;
  case 4:
#if 0 
    return INSTRUCTION_4_32Bf);
#endif 
    return (SOC_IS_JERICHO(unit)? INSTRUCTION_5_32Bf: INSTRUCTION_4_32Bf);
    break;
  case 5:
#if 0 
    return INSTRUCTION_4_32Bf);
#endif 
    return (SOC_IS_JERICHO(unit)? INSTRUCTION_6_32Bf: INSTRUCTION_5_32Bf);
    break;
  default:
    return INVALIDf;
  }

}

/* 
 * Given an instruction bitmap, return the conofigured HW: 
 * - if Arad, (or Jericho in long-term), return the same value 
 * - if Jericho (short-term, only for porting): 
 *   - if Key-A or Key-B, return 0 on MSB, and same value on LSB
 *   - if Key-C, use bits 7:0 for LSB, and bits 15:8 for MSB
 */
uint32
  arad_pp_ihb_flp_key_inst_valid_bitmap_hw_set(
     SOC_SAND_IN  int      unit,
     SOC_SAND_IN  int      mem_ndx,
     SOC_SAND_IN  uint32   key_id,
     SOC_SAND_IN  uint32   inst_valid_bitmap /* struct value */
  )
{
  uint32 jer_inst_valid_bitmap = 0;

  if (SOC_IS_JERICHO(unit)) {
    jer_inst_valid_bitmap = FLP_INSTR_VALID_BMP_ARAD_TO_JER(inst_valid_bitmap);
    if (key_id < 2) { /* Key A or Key B */
      return (mem_ndx? 0x0: jer_inst_valid_bitmap);
    } else {
      return (mem_ndx? ((jer_inst_valid_bitmap >> 8) & 0xFF): (jer_inst_valid_bitmap & 0xFF));
    }
  } else {
    return inst_valid_bitmap;
  }

}

uint32
  arad_pp_ihb_flp_key_construction_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_ENTRY_SIZE];
  soc_mem_t
    flp_key_construction_mem = SOC_IS_JERICHO(unit)? IHP_FLP_KEY_CONSTRUCTION_LSBm: IHB_FLP_KEY_CONSTRUCTIONm;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          flp_key_construction_mem,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

    tbl_data->instruction_0_16b = soc_mem_field32_get(unit, flp_key_construction_mem, data, arad_pp_ihb_flp_key_field_get(unit, 0));
    tbl_data->instruction_1_16b = soc_mem_field32_get(unit, flp_key_construction_mem, data, arad_pp_ihb_flp_key_field_get(unit, 1));
    tbl_data->instruction_2_16b = soc_mem_field32_get(unit, flp_key_construction_mem, data, arad_pp_ihb_flp_key_field_get(unit, 2));
    tbl_data->instruction_3_32b = soc_mem_field32_get(unit, flp_key_construction_mem, data, arad_pp_ihb_flp_key_field_get(unit, 3));
    tbl_data->instruction_4_32b = soc_mem_field32_get(unit, flp_key_construction_mem, data, arad_pp_ihb_flp_key_field_get(unit, 4));
    tbl_data->instruction_5_32b = soc_mem_field32_get(unit, flp_key_construction_mem, data, arad_pp_ihb_flp_key_field_get(unit, 5));
    tbl_data->key_a_inst_0_to_5_valid = soc_mem_field32_get(unit, flp_key_construction_mem, data, SOC_IS_JERICHO(unit)? KEY_A_INST_0_TO_7_VALIDf: KEY_A_INST_0_TO_5_VALIDf);
    tbl_data->key_b_inst_0_to_5_valid = soc_mem_field32_get(unit, flp_key_construction_mem, data, SOC_IS_JERICHO(unit)? KEY_B_INST_0_TO_7_VALIDf: KEY_B_INST_0_TO_5_VALIDf);
    tbl_data->key_c_inst_0_to_5_valid = soc_mem_field32_get(unit, flp_key_construction_mem, data, SOC_IS_JERICHO(unit)? KEY_C_INST_0_TO_7_VALIDf: KEY_C_INST_0_TO_5_VALIDf);
  
#if 0 
    if (SOC_IS_JERICHO(unit)) {
      tbl_data->instruction_6_32b = soc_mem_field32_get(unit, flp_key_construction_mem, data, INSTRUCTION_6_32Bf);
      tbl_data->instruction_7_32b = soc_mem_field32_get(unit, flp_key_construction_mem, data, INSTRUCTION_7_32Bf);
      tbl_data->key_d_inst_0_to_7_valid = soc_mem_field32_get(unit, flp_key_construction_mem, data, KEY_D_INST_0_TO_7_VALIDf);
    }
#endif 

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_flp_key_construction_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihb_flp_key_construction_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    mem_ndx,
    nof_mems,
    data[ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_ENTRY_SIZE];
  soc_mem_t
    flp_key_construction_mem = SOC_IS_JERICHO(unit)? IHP_FLP_KEY_CONSTRUCTION_LSBm: IHB_FLP_KEY_CONSTRUCTIONm;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHB_FLP_KEY_CONSTRUCTION_TBL_ENTRY_SIZE);

  
  nof_mems = SOC_IS_JERICHO(unit)? 2: 1;
  for (mem_ndx = 0; mem_ndx < nof_mems; mem_ndx++) {
    if (SOC_IS_JERICHO(unit) && (mem_ndx == 1)) {
      flp_key_construction_mem = IHP_FLP_KEY_CONSTRUCTION_MSBm;
    }

    soc_mem_field32_set(unit, flp_key_construction_mem, data, arad_pp_ihb_flp_key_field_get(unit, 0), tbl_data->instruction_0_16b);
    soc_mem_field32_set(unit, flp_key_construction_mem, data, arad_pp_ihb_flp_key_field_get(unit, 1), tbl_data->instruction_1_16b);
    soc_mem_field32_set(unit, flp_key_construction_mem, data, arad_pp_ihb_flp_key_field_get(unit, 2), tbl_data->instruction_2_16b);
    soc_mem_field32_set(unit, flp_key_construction_mem, data, arad_pp_ihb_flp_key_field_get(unit, 3), tbl_data->instruction_3_32b);
    soc_mem_field32_set(unit, flp_key_construction_mem, data, arad_pp_ihb_flp_key_field_get(unit, 4), tbl_data->instruction_4_32b);
    soc_mem_field32_set(unit, flp_key_construction_mem, data, arad_pp_ihb_flp_key_field_get(unit, 5), tbl_data->instruction_5_32b);

    soc_mem_field32_set(unit, flp_key_construction_mem, data, SOC_IS_JERICHO(unit)? KEY_A_INST_0_TO_7_VALIDf: KEY_A_INST_0_TO_5_VALIDf, arad_pp_ihb_flp_key_inst_valid_bitmap_hw_set(unit, mem_ndx, 0, tbl_data->key_a_inst_0_to_5_valid));
    soc_mem_field32_set(unit, flp_key_construction_mem, data, SOC_IS_JERICHO(unit)? KEY_B_INST_0_TO_7_VALIDf: KEY_B_INST_0_TO_5_VALIDf, arad_pp_ihb_flp_key_inst_valid_bitmap_hw_set(unit, mem_ndx, 1, tbl_data->key_b_inst_0_to_5_valid));
    soc_mem_field32_set(unit, flp_key_construction_mem, data, SOC_IS_JERICHO(unit)? KEY_C_INST_0_TO_7_VALIDf: KEY_C_INST_0_TO_5_VALIDf, arad_pp_ihb_flp_key_inst_valid_bitmap_hw_set(unit, mem_ndx, 2, tbl_data->key_c_inst_0_to_5_valid));
    
#if 0 
    if (SOC_IS_JERICHO(unit)) {
      soc_mem_field32_set(unit, flp_key_construction_mem, data, INSTRUCTION_6_32Bf, tbl_data->instruction_6_32b);
      soc_mem_field32_set(unit, flp_key_construction_mem, data, INSTRUCTION_7_32Bf, tbl_data->instruction_7_32b);
      soc_mem_field32_set(unit, flp_key_construction_mem, data, KEY_D_INST_0_TO_7_VALIDf, tbl_data->key_d_inst_0_to_7_valid);
    }
#endif 

    res = soc_mem_write(
            unit,
            flp_key_construction_mem,
            MEM_BLOCK_ANY,
            entry_offset,
            data
          );
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_flp_key_construction_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihb_lpm_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32  bank_id,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHB_LPM_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHB_LPM_TBL_ENTRY_SIZE];
  uint32
    memory;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_LPM_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHB_LPM_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHB_LPM_TBL_DATA, 1);
  
  switch(bank_id) {
  case 0:
      memory = IHB_LPMm;
  break;
  case 1:
      memory = IHB_LPM_2m;
  break;
  case 2:
      memory = IHB_LPM_3m;
  break;
  case 3:
      memory = IHB_LPM_4m;
  break;
  case 4:
      memory = IHB_LPM_5m;
  break;
  case 5:
      memory = IHB_LPM_6m;
  break;
  default:
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_TBL_RANGE_OUT_OF_LIMIT, 20, exit);
  }

  res = soc_mem_read(
          unit,
          memory,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->lpm = soc_mem_field32_get(unit, IHB_LPM_2m, data, LPM_2f);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_lpm_tbl_get_unsafe()", entry_offset, 0);
}
#define LPM_USE_DMA 1

uint32
  arad_pp_ihb_lpm_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32  bank_id,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  uint32   nof_entries,
    SOC_SAND_OUT uint32   *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
#if LPM_USE_DMA
  uint32
      *dma_buff;
#endif
/*  uint32
    data[ARAD_PP_IHB_LPM_TBL_ENTRY_SIZE];*/
  uint32
    memory;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_LPM_TBL_SET_UNSAFE);

  switch(bank_id) {
  case 0:
      memory = IHB_LPMm;
  break;
  case 1:
      memory = IHB_LPM_2m;
  break;
  case 2:
      memory = IHB_LPM_3m;
  break;
  case 3:
      memory = IHB_LPM_4m;
  break;
  case 4:
      memory = IHB_LPM_5m;
  break;
  case 5:
      memory = IHB_LPM_6m;
  break;
  default:
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_TBL_RANGE_OUT_OF_LIMIT, 20, exit);
  }

  if(nof_entries == 1) {
      res = soc_mem_write(
              unit,
              memory,
              MEM_BLOCK_ANY,
              entry_offset,
              tbl_data
            );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
      goto exit;
  }
  /* write chunk of entries */

#if LPM_USE_DMA
  res = arad_pp_sw_db_ipv4_lpm_dma_buff_get(unit,&dma_buff);
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);
  

  /* copy data into DMA buffer */
  sal_memcpy(dma_buff, tbl_data,  nof_entries * sizeof(uint32));

  /* write into the lines, from buffer*/
  res = soc_mem_array_write_range(unit, 0, memory, 0, MEM_BLOCK_ANY, entry_offset, entry_offset+nof_entries-1, dma_buff);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res,50,exit);
#else
{
    uint32 indx = 0;
    for (indx = 0; indx < nof_entries; ++indx) {
        res = soc_mem_write(
                unit,
                memory,
                MEM_BLOCK_ANY,
                entry_offset+indx,
                &tbl_data[indx]
              );
        SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
    }
}
#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_lpm_tbl_set_unsafe()", entry_offset, 0);
}


STATIC uint32
  arad_pp_ihb_tcam_action_mem_and_index_get(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint32              action_tbl_id,
    SOC_SAND_IN  uint8               is_for_hit_bit,
    SOC_SAND_OUT soc_mem_t           *mem_tbl,
    SOC_SAND_OUT uint32              *arr_index
  )
{
  soc_mem_t
      action_tbl_small[] = {IHB_TCAM_ACTION_24m, IHB_TCAM_ACTION_25m, 
          IHB_TCAM_ACTION_26m, IHB_TCAM_ACTION_27m},
      hit_bit_tbl_small[] = {IHB_TCAM_ACTION_HIT_INDICATION_24m, IHB_TCAM_ACTION_HIT_INDICATION_25m, 
              IHB_TCAM_ACTION_HIT_INDICATION_26m, IHB_TCAM_ACTION_HIT_INDICATION_27m},
      action_tbl_small_jericho[] = {PPDB_A_TCAM_ACTION_SMALL_24m, PPDB_A_TCAM_ACTION_SMALL_25m, 
          PPDB_A_TCAM_ACTION_SMALL_26m, PPDB_A_TCAM_ACTION_SMALL_27m, 
          PPDB_A_TCAM_ACTION_SMALL_28m, PPDB_A_TCAM_ACTION_SMALL_29m, 
          PPDB_A_TCAM_ACTION_SMALL_30m, PPDB_A_TCAM_ACTION_SMALL_31m},
      hit_bit_tbl_small_jericho[] = {PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_24m, PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_25m,  
              PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_26m, PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_27m, 
              PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_28m, PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_29m, 
              PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_30m, PPDB_A_TCAM_ACTION_HIT_INDICATION_SMALL_31m};
   uint32
       action_tbl_small_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_TCAM_ACTION_TBL_GET_UNSAFE);

  /* 
   * Get the table name and the good index
   */
  if (action_tbl_id < (2 * SOC_DPP_DEFS_GET(unit, nof_tcam_big_banks))) {
      *mem_tbl = is_for_hit_bit? PPDB_A_TCAM_ACTION_HIT_INDICATIONm: PPDB_A_TCAM_ACTIONm;
      *arr_index = action_tbl_id;
  }
  else if (action_tbl_id < (2 * SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit))) {
      action_tbl_small_id = action_tbl_id - (2 * SOC_DPP_DEFS_GET(unit, nof_tcam_big_banks));
      if(SOC_IS_JERICHO(unit)) {
          *mem_tbl = is_for_hit_bit? hit_bit_tbl_small_jericho[action_tbl_small_id]: action_tbl_small_jericho[action_tbl_small_id];
          *arr_index = 0;
      } else
      {
          *mem_tbl = is_for_hit_bit? hit_bit_tbl_small[action_tbl_small_id]: action_tbl_small[action_tbl_small_id];
          *arr_index = 0;
      }
  }
  else {
      SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT_ERR, 10, exit);
  }         

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_tcam_action_mem_and_index_get()", action_tbl_id, 0);
}


/*
 * Read indirect table tcam_action_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

uint32
  arad_pp_ihb_tcam_action_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              action_tbl_id,
    SOC_SAND_IN  uint32              line,
    SOC_SAND_IN  uint8               is_for_hit_bit,
    SOC_SAND_OUT ARAD_PP_IHB_TCAM_ACTION_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset = line,
    arr_index,
    data[ARAD_PP_IHB_TCAM_ACTION_TBL_ENTRY_SIZE];
  soc_mem_t           
      mem_tbl;
  soc_field_t 
      field;
  uint32
      access_lcl[1];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_TCAM_ACTION_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHB_TCAM_ACTION_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHB_TCAM_ACTION_TBL_DATA, 1);

  /* 
   * Get the table name and the good index
   */
  res = arad_pp_ihb_tcam_action_mem_and_index_get(
            unit,
            action_tbl_id,
            is_for_hit_bit,
            &mem_tbl,
            &arr_index
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
 
  /* 
   * 2 TCAM Action tables per TCAM bank, 1024 lines per table 
   * Take as input the TCAM Bank (and not the TCAM Action table index), 
   * and the Action table logical line, and not the actual HW line 
   */
  if (is_for_hit_bit) {
      offset = ARAD_IHB_TCAM_HIT_INDICATION_TCAM_ENTRY_TO_LINE(line);
  }
  res = soc_mem_array_read(
          unit,
          mem_tbl,
          arr_index,
          MEM_BLOCK_ANY,
          offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);


 LOG_VERBOSE(BSL_LS_SOC_MEM,
             (BSL_META_U(unit,
                         SOC_DPP_MSG("      "
                         "TCAM-Action-read: array-index:%d, line:%d, data:%d \n\r")), arr_index, line, data[0]));

  field = is_for_hit_bit? HITf: ACTIONf;
  tbl_data->action = soc_mem_field32_get(
                        unit,
                        mem_tbl,
                        data,
                        field);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
         
  if (is_for_hit_bit) {
      *access_lcl = tbl_data->action;
      tbl_data->action = SHR_BITGET(access_lcl, ARAD_IHB_TCAM_HIT_INDICATION_TCAM_ENTRY_TO_BIT(line))? 1: 0;
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_tcam_action_tbl_get_unsafe()", line, 0);
}

uint32
  arad_pp_ihb_tcam_action_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              action_tbl_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  uint8               is_for_hit_bit,
    SOC_SAND_IN  ARAD_PP_IHB_TCAM_ACTION_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset=entry_offset,
    arr_index=action_tbl_id,
    data[ARAD_PP_IHB_TCAM_ACTION_TBL_ENTRY_SIZE];
  soc_mem_t           
      mem_tbl;
  soc_field_t 
      field;
  uint32
      action_lcl[1];
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_TCAM_ACTION_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHB_TCAM_ACTION_TBL_ENTRY_SIZE);

  /* 
   * Get the table name and the good index
   */
  res = arad_pp_ihb_tcam_action_mem_and_index_get(
            unit,
            action_tbl_id,
            is_for_hit_bit,
            &mem_tbl,
            &arr_index
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (is_for_hit_bit) {
      offset = ARAD_IHB_TCAM_HIT_INDICATION_TCAM_ENTRY_TO_LINE(entry_offset);
  }

  /* Get the bitmap and set the correct bit */
  if (is_for_hit_bit) {
      res = soc_mem_array_read(
              unit,
              mem_tbl,
              arr_index,
              MEM_BLOCK_ALL,
              offset,
              action_lcl
            );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 23, exit);

      if (tbl_data->action) {
          SHR_BITSET(action_lcl, ARAD_IHB_TCAM_HIT_INDICATION_TCAM_ENTRY_TO_BIT(entry_offset));
      }
      else {
          SHR_BITCLR(action_lcl, ARAD_IHB_TCAM_HIT_INDICATION_TCAM_ENTRY_TO_BIT(entry_offset));
      }
  }
  else {
      *action_lcl = tbl_data->action;
  }

  field = is_for_hit_bit? HITf: ACTIONf;
  soc_mem_field32_set(
          unit,
          mem_tbl,
          data,
          field,
          *action_lcl);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  
  res = soc_mem_array_write(
          unit,
          mem_tbl,
          arr_index,
          MEM_BLOCK_ALL,
          offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

 LOG_VERBOSE(BSL_LS_SOC_MEM,
             (BSL_META_U(unit,
                         SOC_DPP_MSG("      "
                         "TCAM-Action-write: array-index:%d, line:%d, data:%d \n\r")), arr_index, offset, data[0]));


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_tcam_action_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_egq_ingress_vlan_edit_command_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ENTRY_SIZE];
 
 SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA, 1);


  res = soc_mem_read(
          unit,
          EGQ_INGRESS_VLAN_EDITING_COMMAND_IVEC_TABLEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
          );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->tpid_profile_link   = soc_mem_field32_get(
                  unit,
                  EGQ_INGRESS_VLAN_EDITING_COMMAND_IVEC_TABLEm,
                  data,
                  TPID_PROFILEf );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  tbl_data->edit_command_outer_vid_source   = soc_mem_field32_get(
                  unit,
                  EGQ_INGRESS_VLAN_EDITING_COMMAND_IVEC_TABLEm,
                  data,
                  OUTER_VID_SRCf            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  tbl_data->edit_command_outer_pcp_dei_source   = soc_mem_field32_get(
                  unit,
                  EGQ_INGRESS_VLAN_EDITING_COMMAND_IVEC_TABLEm,
                  data,
                  OUTER_PCP_DEI_SRCf        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  tbl_data->edit_command_inner_vid_source   = soc_mem_field32_get(
                  unit,
                  EGQ_INGRESS_VLAN_EDITING_COMMAND_IVEC_TABLEm,
                  data,
                  INNER_VID_SRCf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  tbl_data->edit_command_inner_pcp_dei_source   = soc_mem_field32_get(
                  unit,
                  EGQ_INGRESS_VLAN_EDITING_COMMAND_IVEC_TABLEm,
                  data,
                  INNER_PCP_DEI_SRCf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  tbl_data->edit_command_bytes_to_remove   = soc_mem_field32_get(
                  unit,
                  EGQ_INGRESS_VLAN_EDITING_COMMAND_IVEC_TABLEm,
                  data,
                  TAGS_TO_REMOVEf            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

  tbl_data->outer_tpid_ndx   = soc_mem_field32_get(
                  unit,
                  EGQ_INGRESS_VLAN_EDITING_COMMAND_IVEC_TABLEm,
                  data,
                  OUTER_TPID_INDEXf            );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  tbl_data->inner_tpid_ndx   = soc_mem_field32_get(
                  unit,
                  EGQ_INGRESS_VLAN_EDITING_COMMAND_IVEC_TABLEm,
                  data,
                  INNER_TPID_INDEXf    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_egq_ingress_vlan_edit_command_map_tbl_get_unsafe()", entry_offset, 0);
}


uint32
  arad_pp_egq_ingress_vlan_edit_command_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ENTRY_SIZE);

            soc_mem_field32_set(
          unit,
          EGQ_INGRESS_VLAN_EDITING_COMMAND_IVEC_TABLEm,
          data,
          TPID_PROFILEf,
          tbl_data->tpid_profile_link );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

            soc_mem_field32_set(
          unit,
          EGQ_INGRESS_VLAN_EDITING_COMMAND_IVEC_TABLEm,
          data,
          OUTER_VID_SRCf,
          tbl_data->edit_command_outer_vid_source );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

            soc_mem_field32_set(
          unit,
          EGQ_INGRESS_VLAN_EDITING_COMMAND_IVEC_TABLEm,
          data,
          OUTER_PCP_DEI_SRCf,
          tbl_data->edit_command_outer_pcp_dei_source );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

            soc_mem_field32_set(
          unit,
          EGQ_INGRESS_VLAN_EDITING_COMMAND_IVEC_TABLEm,
          data,
          INNER_VID_SRCf,
          tbl_data->edit_command_inner_vid_source );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

            soc_mem_field32_set(
          unit,
          EGQ_INGRESS_VLAN_EDITING_COMMAND_IVEC_TABLEm,
          data,
          INNER_PCP_DEI_SRCf,
          tbl_data->edit_command_inner_pcp_dei_source );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

            soc_mem_field32_set(
          unit,
          EGQ_INGRESS_VLAN_EDITING_COMMAND_IVEC_TABLEm,
          data,
          TAGS_TO_REMOVEf,
          tbl_data->edit_command_bytes_to_remove );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

      soc_mem_field32_set(
          unit,
          EGQ_INGRESS_VLAN_EDITING_COMMAND_IVEC_TABLEm,
          data,
          OUTER_TPID_INDEXf,
          tbl_data->outer_tpid_ndx );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

            soc_mem_field32_set(
          unit,
          EGQ_INGRESS_VLAN_EDITING_COMMAND_IVEC_TABLEm,
          data,
          INNER_TPID_INDEXf,
          tbl_data->inner_tpid_ndx );
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

  res = soc_mem_write(
          unit,
          EGQ_INGRESS_VLAN_EDITING_COMMAND_IVEC_TABLEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 120, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_egq_ingress_vlan_edit_command_map_tbl_set_unsafe()", entry_offset, 0);
}



uint32
  arad_pp_egq_vsi_membership_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_EGQ_VSI_MEMBERSHIP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
      data[ARAD_PP_EGQ_VSI_MEMBERSHIP_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EGQ_VSI_MEMBERSHIP_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EGQ_VSI_MEMBERSHIP_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_EGQ_VSI_MEMBERSHIP_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          EGQ_VSI_MEMBERSHIPm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
          );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_mem_read(unit, EGQ_VSI_MEMBERSHIPm, MEM_BLOCK_ANY, entry_offset, data); 
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  soc_mem_field_get(unit, EGQ_VSI_MEMBERSHIPm, data, VSI_MEMBERSHIPf, &(tbl_data->vsi_membership[0]));

  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_egq_vsi_membership_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_egq_vsi_membership_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_EGQ_VSI_MEMBERSHIP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EGQ_VSI_MEMBERSHIP_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EGQ_VSI_MEMBERSHIP_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EGQ_VSI_MEMBERSHIP_TBL_ENTRY_SIZE);

  res = soc_mem_read(unit, EGQ_VSI_MEMBERSHIPm, MEM_BLOCK_ANY, entry_offset, data); 
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  soc_mem_field_set(unit, EGQ_VSI_MEMBERSHIPm, data, VSI_MEMBERSHIPf, (uint32*)&(tbl_data->vsi_membership[0]));

  res = soc_mem_write(unit, EGQ_VSI_MEMBERSHIPm, MEM_BLOCK_ALL, entry_offset, data);

  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_egq_vsi_membership_tbl_set_unsafe()", entry_offset, 0);
}
uint32
  arad_pp_epni_tx_tag_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_EPNI_TX_TAG_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EPNI_TX_TAG_TABLE_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EPNI_TX_TAG_TABLE_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EPNI_TX_TAG_TABLE_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_EPNI_TX_TAG_TABLE_TBL_DATA, 1);

  res = soc_mem_read(unit, EPNI_TX_TAG_TABLEm, MEM_BLOCK_ANY, entry_offset, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  soc_mem_field_get(unit, EPNI_TX_TAG_TABLEm, data, TX_TAG_TABLEf, &(tbl_data->entry[0]));
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_epni_tx_tag_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_epni_tx_tag_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_EPNI_TX_TAG_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EPNI_TX_TAG_TABLE_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EPNI_TX_TAG_TABLE_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EPNI_TX_TAG_TABLE_TBL_ENTRY_SIZE);
  
  soc_mem_field_set(unit, EPNI_TX_TAG_TABLEm, data, TX_TAG_TABLEf, (uint32 *)&(tbl_data->entry[0]));
  
  res = soc_mem_write(unit, EPNI_TX_TAG_TABLEm, MEM_BLOCK_ALL, entry_offset, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_epni_tx_tag_table_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_EPNI_STP_TBL_ENTRY_SIZE 9

uint32
  arad_pp_epni_stp_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_EPNI_STP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EPNI_STP_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EPNI_STP_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EPNI_STP_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_EPNI_STP_TBL_DATA, 1);

  res = READ_EPNI_SPANNING_TREE_PROTOCOL_STATE_MEMORY_STPm(unit, MEM_BLOCK_ANY, entry_offset, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
  soc_mem_field_get(unit, EPNI_SPANNING_TREE_PROTOCOL_STATE_MEMORY_STPm, data, STPf, tbl_data->stp);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_epni_stp_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_epni_stp_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_EPNI_STP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EPNI_STP_TBL_ENTRY_SIZE];

 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EPNI_STP_TBL_SET_UNSAFE);
  ARAD_CLEAR(data, uint32, ARAD_PP_EPNI_STP_TBL_ENTRY_SIZE);
  
  soc_mem_field_set(unit, EPNI_SPANNING_TREE_PROTOCOL_STATE_MEMORY_STPm, data, STPf, (uint32*)(tbl_data->stp));
  res = WRITE_EPNI_SPANNING_TREE_PROTOCOL_STATE_MEMORY_STPm(unit, MEM_BLOCK_ALL, entry_offset, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_epni_stp_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_epni_pcp_dei_map_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_EPNI_PCP_DEI_MAP_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EPNI_PCP_DEI_MAP_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EPNI_PCP_DEI_MAP_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EPNI_PCP_DEI_MAP_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_EPNI_PCP_DEI_MAP_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          EPNI_PCP_DEI_TABLEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

soc_mem_field_get(unit, EPNI_PCP_DEI_TABLEm, data, PCP_DEI_MAPf,&tbl_data->pcp_dei_map);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_epni_pcp_dei_map_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_epni_pcp_dei_map_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_EPNI_PCP_DEI_MAP_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EPNI_PCP_DEI_MAP_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EPNI_PCP_DEI_MAP_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EPNI_PCP_DEI_MAP_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, EPNI_PCP_DEI_TABLEm, data, PCP_DEI_MAPf, tbl_data->pcp_dei_map);

  res = soc_mem_write(
          unit,
          EPNI_PCP_DEI_TABLEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_epni_pcp_dei_map_tbl_set_unsafe()", entry_offset, 0);
}



uint32
  arad_pp_epni_egress_edit_cmd_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_EPNI_EGRESS_EDIT_CMD_TBL_DATA  *tbl_data
  )
{
  uint32
    entry,
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EPNI_EGRESS_EDIT_CMD_TBL_ENTRY_SIZE],
    action_cmd_field;    

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EPNI_EGRESS_EDIT_CMD_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EPNI_EGRESS_EDIT_CMD_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_EPNI_EGRESS_EDIT_CMD_TBL_DATA, 1);

 res = soc_mem_read(
          unit,
          EPNI_EVEC_TABLEm,
          MEM_BLOCK_ANY,
          entry_offset/2,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  soc_mem_field_get(unit, EPNI_EVEC_TABLEm, data, EVEC_TABLE_DATAf, &action_cmd_field);

  entry = (entry_offset & 0x1 )*16; 

  soc_sand_bitstream_get_any_field(&action_cmd_field,entry+0,2,&tbl_data->tags_to_remove);
  soc_sand_bitstream_get_any_field(&action_cmd_field,entry+2,2,&tbl_data->inner_pcp_dei_source);
  soc_sand_bitstream_get_any_field(&action_cmd_field,entry+4,2,&tbl_data->outer_pcp_dei_source);
  soc_sand_bitstream_get_any_field(&action_cmd_field,entry+6,3,&tbl_data->inner_vid_source);
  soc_sand_bitstream_get_any_field(&action_cmd_field,entry+9,3,&tbl_data->outer_vid_source);
  soc_sand_bitstream_get_any_field(&action_cmd_field,entry+12,2,&tbl_data->inner_tpid_index);
  soc_sand_bitstream_get_any_field(&action_cmd_field,entry+14,2,&tbl_data->outer_tpid_index);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_epni_egress_edit_cmd_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_epni_egress_edit_cmd_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_EPNI_EGRESS_EDIT_CMD_TBL_DATA  *tbl_data
  )
{
  uint32
    entry,
    tmp,
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EPNI_EGRESS_EDIT_CMD_TBL_ENTRY_SIZE];    
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EPNI_EGRESS_EDIT_CMD_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EPNI_EGRESS_EDIT_CMD_TBL_ENTRY_SIZE);

  res = soc_mem_read(
          unit,
          EPNI_EVEC_TABLEm,
          MEM_BLOCK_ANY,
          entry_offset/2,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  entry = (entry_offset & 0x1) * 16; 

  tmp = ARAD_PP_EPNI_EGRESS_EDIT_CMD_TBL_INFO(
                    tbl_data->outer_tpid_index, tbl_data->inner_tpid_index, \
                    tbl_data->outer_vid_source, tbl_data->inner_vid_source, \
                    tbl_data->outer_pcp_dei_source, tbl_data->inner_pcp_dei_source, \
                    tbl_data->tags_to_remove
                    );

  soc_sand_bitstream_set_any_field(&tmp,entry,ARAD_PP_EPNI_EGRESS_EDIT_CMD_TBL_FLD_SIZE,data);

  /* soc_mem_field_set(unit, EPNI_EVEC_TABLEm, data, EVEC_TABLE_DATAf, &tmp);*/

  res = soc_mem_write(
          unit,
          EPNI_EVEC_TABLEm,
          MEM_BLOCK_ANY,
          entry_offset/2,
          data
        );
  
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_epni_egress_edit_cmd_tbl_set_unsafe()", entry_offset, 0);
}
uint32
  arad_pp_epni_ingress_vlan_edit_command_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT ARAD_PP_EPNI_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EPNI_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EPNI_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EPNI_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_EPNI_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          EPNI_IVEC_TABLEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
    );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

          tbl_data->tpid_profile_link   = soc_mem_field32_get(
                  unit,
                  EPNI_IVEC_TABLEm,
                  data,
                  TPID_PROFILEf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

          tbl_data->edit_command_outer_vid_source   = soc_mem_field32_get(
                  unit,
                  EPNI_IVEC_TABLEm,
                  data,
                  OUTER_VID_SRCf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

          tbl_data->edit_command_outer_pcp_dei_source   = soc_mem_field32_get(
                  unit,
                  EPNI_IVEC_TABLEm,
                  data,
                  OUTER_PCP_DEI_SRCf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

          tbl_data->edit_command_inner_vid_source   = soc_mem_field32_get(
                  unit,
                  EPNI_IVEC_TABLEm,
                  data,
                  INNER_VID_SRCf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

          tbl_data->edit_command_inner_pcp_dei_source   = soc_mem_field32_get(
                  unit,
                  EPNI_IVEC_TABLEm,
                  data,
                  INNER_PCP_DEI_SRCf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

          tbl_data->edit_command_bytes_to_remove   = soc_mem_field32_get(
                  unit,
                  EPNI_IVEC_TABLEm,
                  data,
                  TAGS_TO_REMOVEf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

          tbl_data->outer_tpid_ndx   = soc_mem_field32_get(
                  unit,
                  EPNI_IVEC_TABLEm,
                  data,
                  OUTER_TPID_INDEXf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

          tbl_data->inner_tpid_ndx   = soc_mem_field32_get(
                  unit,
                  EPNI_IVEC_TABLEm,
                  data,
                  INNER_TPID_INDEXf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_epni_ingress_vlan_edit_command_map_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_epni_ingress_vlan_edit_command_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_EPNI_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EPNI_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EPNI_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EPNI_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ENTRY_SIZE);
            soc_mem_field32_set(
          unit,
          EPNI_IVEC_TABLEm,
          data,
          TPID_PROFILEf,
          tbl_data->tpid_profile_link );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

            soc_mem_field32_set(
          unit,
          EPNI_IVEC_TABLEm,
          data,
          OUTER_VID_SRCf,
          tbl_data->edit_command_outer_vid_source );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

            soc_mem_field32_set(
          unit,
          EPNI_IVEC_TABLEm,
          data,
          OUTER_PCP_DEI_SRCf,
          tbl_data->edit_command_outer_pcp_dei_source );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

            soc_mem_field32_set(
          unit,
          EPNI_IVEC_TABLEm,
          data,
          INNER_VID_SRCf,
          tbl_data->edit_command_inner_vid_source );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

            soc_mem_field32_set(
          unit,
          EPNI_IVEC_TABLEm,
          data,
          INNER_PCP_DEI_SRCf,
          tbl_data->edit_command_inner_pcp_dei_source );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

            soc_mem_field32_set(
          unit,
          EPNI_IVEC_TABLEm,
          data,
          TAGS_TO_REMOVEf,
          tbl_data->edit_command_bytes_to_remove );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

            soc_mem_field32_set(
          unit,
          EPNI_IVEC_TABLEm,
          data,
          OUTER_TPID_INDEXf,
          tbl_data->outer_tpid_ndx );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

            soc_mem_field32_set(
          unit,
          EPNI_IVEC_TABLEm,
          data,
          INNER_TPID_INDEXf,
          tbl_data->inner_tpid_ndx );
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

  res = soc_mem_write(
          unit,
          EPNI_IVEC_TABLEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 120, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_epni_ingress_vlan_edit_command_map_tbl_set_unsafe()", entry_offset, 0);
}


#define ARAD_PP_EPNI_EXP_REMARK_TBL_ENTRY_SIZE 2

uint32
  arad_pp_epni_exp_remark_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_EPNI_EXP_REMARK_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EPNI_EXP_REMARK_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EPNI_EXP_REMARK_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EPNI_EXP_REMARK_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_EPNI_EXP_REMARK_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          EPNI_EXP_REMARKm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->exp_remark_data = soc_mem_field32_get(unit, EPNI_EXP_REMARKm, data, EXP_REMARK_DATAf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_epni_exp_remark_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_epni_exp_remark_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_EPNI_EXP_REMARK_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EPNI_EXP_REMARK_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EPNI_EXP_REMARK_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EPNI_EXP_REMARK_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, EPNI_EXP_REMARKm, data, EXP_REMARK_DATAf, tbl_data->exp_remark_data);

  res = soc_mem_write(
          unit,
          EPNI_EXP_REMARKm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_epni_exp_remark_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_EPNI_DSCP_REMARK_TBL_ENTRY_SIZE 2

uint32
  arad_pp_epni_dscp_remark_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_EPNI_DSCP_REMARK_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EPNI_DSCP_REMARK_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EPNI_DSCP_REMARK_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EPNI_DSCP_REMARK_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_EPNI_DSCP_REMARK_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          EPNI_DSCP_REMARKm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->dscp_remark_data = soc_mem_field32_get(unit, EPNI_DSCP_REMARKm, data, DSCP_REMARK_DATAf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_epni_dscp_remark_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_epni_dscp_remark_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_EPNI_DSCP_REMARK_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EPNI_DSCP_REMARK_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EPNI_DSCP_REMARK_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EPNI_DSCP_REMARK_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, EPNI_DSCP_REMARKm, data, DSCP_REMARK_DATAf, tbl_data->dscp_remark_data);

  res = soc_mem_write(
          unit,
          EPNI_DSCP_REMARKm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_epni_dscp_remark_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_EPNI_REMARK_MPLS_TO_EXP_TBL_ENTRY_SIZE 1

uint32
  arad_pp_epni_remark_mpls_to_exp_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_EPNI_REMARK_MPLS_TO_EXP_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EPNI_REMARK_MPLS_TO_EXP_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EPNI_REMARK_MPLS_TO_EXP_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EPNI_REMARK_MPLS_TO_EXP_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_EPNI_REMARK_MPLS_TO_EXP_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          EPNI_REMARK_MPLS_TO_EXPm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->dscp_exp_remark_data = soc_mem_field32_get(unit, EPNI_REMARK_MPLS_TO_EXPm, data, REMARK_MPLS_TO_EXPf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_epni_remark_mpls_to_exp_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_epni_remark_mpls_to_exp_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_EPNI_REMARK_MPLS_TO_EXP_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EPNI_REMARK_MPLS_TO_EXP_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EPNI_REMARK_MPLS_TO_EXP_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EPNI_REMARK_MPLS_TO_EXP_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, EPNI_REMARK_MPLS_TO_EXPm, data, REMARK_MPLS_TO_EXPf, tbl_data->dscp_exp_remark_data);

  res = soc_mem_write(
          unit,
          EPNI_REMARK_MPLS_TO_EXPm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_epni_remark_mpls_to_exp_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_EPNI_REMARK_MPLS_TO_DSCP_TBL_ENTRY_SIZE 1

uint32
  arad_pp_epni_remark_mpls_to_dscp_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_EPNI_REMARK_MPLS_TO_DSCP_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EPNI_REMARK_MPLS_TO_DSCP_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EPNI_REMARK_MPLS_TO_DSCP_TBL_GET_UNSAFE);
    
  ARAD_CLEAR(data, uint32, ARAD_PP_EPNI_REMARK_MPLS_TO_DSCP_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_EPNI_REMARK_MPLS_TO_DSCP_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          EPNI_REMARK_MPLS_TO_DSCPm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->dscp_exp_remark_data = soc_mem_field32_get(unit, EPNI_REMARK_MPLS_TO_DSCPm, data, REMARK_MPLS_TO_DSCPf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_epni_remark_mpls_to_dscp_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_epni_remark_mpls_to_dscp_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_EPNI_REMARK_MPLS_TO_DSCP_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EPNI_REMARK_MPLS_TO_DSCP_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EPNI_REMARK_MPLS_TO_DSCP_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EPNI_REMARK_MPLS_TO_DSCP_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, EPNI_REMARK_MPLS_TO_DSCPm, data, REMARK_MPLS_TO_DSCPf, tbl_data->dscp_exp_remark_data);

  res = soc_mem_write(
          unit,
          EPNI_REMARK_MPLS_TO_DSCPm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_epni_remark_mpls_to_dscp_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_EPNI_REMARK_IPV6_TO_EXP_TBL_ENTRY_SIZE 1

uint32
  arad_pp_epni_remark_ipv6_to_exp_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_EPNI_REMARK_IPV6_TO_EXP_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EPNI_REMARK_IPV6_TO_EXP_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EPNI_REMARK_IPV6_TO_EXP_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EPNI_REMARK_IPV6_TO_EXP_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_EPNI_REMARK_IPV6_TO_EXP_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          EPNI_REMARK_IPV6_TO_EXPm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->dscp_exp_remark_data = soc_mem_field32_get(unit, EPNI_REMARK_IPV6_TO_EXPm, data, EXP_REMARK_DATAf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_epni_remark_ipv6_to_exp_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_epni_remark_ipv6_to_exp_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_EPNI_REMARK_IPV6_TO_EXP_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EPNI_REMARK_IPV6_TO_EXP_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EPNI_REMARK_IPV6_TO_EXP_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EPNI_REMARK_IPV6_TO_EXP_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, EPNI_REMARK_IPV6_TO_EXPm, data, EXP_REMARK_DATAf, tbl_data->dscp_exp_remark_data);

  res = soc_mem_write(
          unit,
          EPNI_REMARK_IPV6_TO_EXPm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_epni_remark_ipv6_to_exp_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_EPNI_REMARK_IPV6_TO_DSCP_TBL_ENTRY_SIZE 2

uint32
  arad_pp_epni_remark_ipv6_to_dscp_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_EPNI_REMARK_IPV6_TO_DSCP_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EPNI_REMARK_IPV6_TO_DSCP_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EPNI_REMARK_IPV6_TO_DSCP_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EPNI_REMARK_IPV6_TO_DSCP_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_EPNI_REMARK_IPV6_TO_DSCP_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          EPNI_REMARK_IPV6_TO_DSCPm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->dscp_exp_remark_data = soc_mem_field32_get(unit, EPNI_REMARK_IPV6_TO_DSCPm, data, EXP_REMARK_DATAf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_epni_remark_ipv6_to_dscp_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_epni_remark_ipv6_to_dscp_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_EPNI_REMARK_IPV6_TO_DSCP_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EPNI_REMARK_IPV6_TO_DSCP_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EPNI_REMARK_IPV6_TO_DSCP_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EPNI_REMARK_IPV6_TO_DSCP_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, EPNI_REMARK_IPV6_TO_DSCPm, data, EXP_REMARK_DATAf, tbl_data->dscp_exp_remark_data);

  res = soc_mem_write(
          unit,
          EPNI_REMARK_IPV6_TO_DSCPm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_epni_remark_ipv6_to_dscp_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_EPNI_REMARK_IPV4_TO_EXP_TBL_ENTRY_SIZE 1

uint32
  arad_pp_epni_remark_ipv4_to_exp_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_EPNI_REMARK_IPV4_TO_EXP_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EPNI_REMARK_IPV4_TO_EXP_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EPNI_REMARK_IPV4_TO_EXP_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EPNI_REMARK_IPV4_TO_EXP_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_EPNI_REMARK_IPV4_TO_EXP_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          EPNI_REMARK_IPV4_TO_EXPm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->dscp_exp_remark_data = soc_mem_field32_get(unit, EPNI_REMARK_IPV4_TO_EXPm, data, EXP_REMARK_DATAf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_epni_remark_ipv4_to_exp_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_epni_remark_ipv4_to_exp_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_EPNI_REMARK_IPV4_TO_EXP_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EPNI_REMARK_IPV4_TO_EXP_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EPNI_REMARK_IPV4_TO_EXP_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EPNI_REMARK_IPV4_TO_EXP_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, EPNI_REMARK_IPV4_TO_EXPm, data, EXP_REMARK_DATAf, tbl_data->dscp_exp_remark_data);

  res = soc_mem_write(
          unit,
          EPNI_REMARK_IPV4_TO_EXPm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_epni_remark_ipv4_to_exp_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_EPNI_REMARK_IPV4_TO_DSCP_TBL_ENTRY_SIZE 2

uint32
  arad_pp_epni_remark_ipv4_to_dscp_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_EPNI_REMARK_IPV4_TO_DSCP_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EPNI_REMARK_IPV4_TO_DSCP_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EPNI_REMARK_IPV4_TO_DSCP_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EPNI_REMARK_IPV4_TO_DSCP_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_EPNI_REMARK_IPV4_TO_DSCP_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          EPNI_REMARK_IPV4_TO_DSCPm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->dscp_exp_remark_data = soc_mem_field32_get(unit, EPNI_REMARK_IPV4_TO_DSCPm, data, EXP_REMARK_DATAf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_epni_remark_ipv4_to_dscp_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_epni_remark_ipv4_to_dscp_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_EPNI_REMARK_IPV4_TO_DSCP_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EPNI_REMARK_IPV4_TO_DSCP_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EPNI_REMARK_IPV4_TO_DSCP_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EPNI_REMARK_IPV4_TO_DSCP_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, EPNI_REMARK_IPV4_TO_DSCPm, data, EXP_REMARK_DATAf, tbl_data->dscp_exp_remark_data);

  res = soc_mem_write(
          unit,
          EPNI_REMARK_IPV4_TO_DSCPm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_epni_remark_ipv4_to_dscp_tbl_set_unsafe()", entry_offset, 0);
}



STATIC uint32
  arad_pp_ihb_tcam_tbl_bank_offset_get(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   uint32                           bank_ndx,
    SOC_SAND_IN   uint32                           hw_offset,
    SOC_SAND_OUT  uint32                           *bank_offset
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_TCAM_TBL_BANK_OFFSET_GET);
    
    if (bank_ndx < SOC_DPP_DEFS_GET(unit, nof_tcam_big_banks)) {
        *bank_offset = (((bank_ndx * SOC_DPP_DEFS_GET(unit, nof_tcam_big_bank_lines)) + hw_offset) * 2);
    }
    else if (bank_ndx < SOC_DPP_DEFS_GET_TCAM_NOF_BANKS(unit)) {
        *bank_offset = (((SOC_DPP_DEFS_GET(unit, nof_tcam_big_banks) * SOC_DPP_DEFS_GET(unit, nof_tcam_big_bank_lines)) + 
               /* Access the HW with 1K and not 256 even if it is over the TCAM table range */ 
               ((bank_ndx - SOC_DPP_DEFS_GET(unit, nof_tcam_big_banks)) * SOC_DPP_DEFS_GET(unit, nof_tcam_big_bank_lines)) + 
                    hw_offset) * 2);
    }
    else {
          SOC_SAND_SET_ERROR_CODE(ARAD_TCAM_BANK_NDX_OUT_OF_RANGE_ERR, 10, exit); /* incorrect bank id */
    }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_pp_ihb_tcam_tbl_bank_offset_get()", 0, 0);
}

uint32
  arad_pp_ihb_tcam_tbl_read_unsafe(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   uint32                           bank_ndx,
    SOC_SAND_IN   uint32                           nof_entries_in_line,
    SOC_SAND_IN   uint32                           entry_offset,
    SOC_SAND_OUT   ARAD_PP_IHB_TCAM_BANK_TBL_DATA    *tcam_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    hw_offset,
    data_index,
    nof_words,
    nof_entries_in_line_w = nof_entries_in_line,
    idx,
    valid_fld;
  soc_reg_above_64_val_t
    mem_val,
    data_out;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_TCAM_TBL_READ_UNSAFE);

  
  sal_memset(tcam_data,0x0, sizeof(ARAD_PP_IHB_TCAM_BANK_TBL_DATA));
 /*
  * in ARAD the only use of the TACM for 164 bits word (one word per line) 82 bits words (2 words per line)
  */
  if (nof_entries_in_line == 0 || nof_entries_in_line > 3)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT_ERR, 10, exit);
  }

  if(nof_entries_in_line == ARAD_PP_IHB_TCAM_HALF_ENTRY_IN_LINE) {
      nof_entries_in_line_w = 1;
  }

 /*
  * in case there is two words per line which word to use.
  */
  if (entry_offset >= nof_entries_in_line_w * ARAD_TCAM_NOF_LINES_PER_BANK(unit, bank_ndx))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT_ERR, 20, exit);
  }
 /*
  * low word or high word
  */
  if (nof_entries_in_line_w == 1)
  {
    hw_offset = entry_offset;
  }
  else
  {
    hw_offset = entry_offset >> 1;
  }

  /* reading the key */
  res = arad_pp_ihb_tcam_tbl_bank_offset_get(unit, bank_ndx, hw_offset, &data_index);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  SOC_REG_ABOVE_64_CLEAR(mem_val);
#ifdef PLISIM /* Preserve logic for simulation */
  if (!SAL_BOOT_PLISIM) 
#endif
{
      soc_mem_field32_set(unit, PPDB_A_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_RDf, 0x1);
      /*those fields should be set to zero. the mem_val is cleared before so it is not needed to zero their value 
        don't eraze this.     
      soc_mem_field32_set(unit, PPDB_A_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_WRf, 0x0);
      soc_mem_field32_set(unit, PPDB_A_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_CMPf, 0x0);
      soc_mem_field32_set(unit, PPDB_A_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_RESERVEDf, 0x0);*/
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, WRITE_PPDB_A_TCAM_BANK_COMMANDm(unit, MEM_BLOCK_ANY, data_index, mem_val));
}


  SOC_SAND_SOC_IF_ERROR_RETURN(res, 55, exit, READ_PPDB_A_TCAM_BANK_COMMANDm(unit, MEM_BLOCK_ANY, data_index, mem_val));
#ifdef PLISIM 
  if (SAL_BOOT_PLISIM) {  
      /* Only in the Simulation */
      valid_fld = soc_mem_field32_get(unit, PPDB_A_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_VALIDf);
  }
  else
#endif
  {
      valid_fld = soc_mem_field32_get(unit, PPDB_A_TCAM_BANK_REPLYm, mem_val, TCAM_CPU_REP_VALIDf);
  }

 LOG_VERBOSE(BSL_LS_SOC_MEM,
             (BSL_META_U(unit,
                         SOC_DPP_MSG("      "
                         "TCAM-read: bank:%d entry:%d, entries in line: %d, "
                         " - Entry is valid: %d\n\r")), bank_ndx, entry_offset, nof_entries_in_line, valid_fld));

  SOC_REG_ABOVE_64_CLEAR(data_out);

  if(nof_entries_in_line_w == 1) {
    if(valid_fld != 0x3 && valid_fld!=0) {
      /* i.e. not all the entry is valid*/
      SOC_SAND_SET_ERROR_CODE(0, 60, exit);
    }
    tcam_data->valid = (valid_fld !=0);

#ifdef PLISIM   
    if (SAL_BOOT_PLISIM) {  
      /* Only in the Simulation */
        soc_mem_field_get(unit, PPDB_A_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_DIf, data_out);
    }
    else
#endif
    {
        soc_mem_field_get(unit, PPDB_A_TCAM_BANK_REPLYm, mem_val, TCAM_CPU_REP_DOUTf, data_out);
    }

    SHR_BITCOPY_RANGE(tcam_data->value, 0, data_out, 2, 80);
    SHR_BITCOPY_RANGE(tcam_data->value, 80, data_out, 84, 80);
  }
  else
  {
#ifdef PLISIM   
    if (SAL_BOOT_PLISIM) {  
        /* Only in the Simulation */
        soc_mem_field_get(unit, PPDB_A_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_DIf, data_out);  
    }
    else
#endif 
    {
        soc_mem_field_get(unit, PPDB_A_TCAM_BANK_REPLYm, mem_val, TCAM_CPU_REP_DOUTf, data_out);
    }
    if(entry_offset % 2 == 0) {

      tcam_data->valid = (valid_fld & 0x1) != 0;
      /* i.e. we should take the lsb 80 bits*/
      SHR_BITCOPY_RANGE(tcam_data->value, 0, data_out, 2, 80); 
    } else {

      tcam_data->valid = (valid_fld & 0x2) != 0;
      /* i.e. we should take the msb 80 bits*/
      SHR_BITCOPY_RANGE(tcam_data->value, 0, data_out, 84, 80);
    }
  }

  /* reading the mask */
  res = arad_pp_ihb_tcam_tbl_bank_offset_get(unit, bank_ndx, hw_offset, &data_index);
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  ++data_index;

  /*SOC_SAND_SOC_IF_ERROR_RETURN(res, 90, exit, READ_PPDB_A_TCAM_BANKm(unit, MEM_BLOCK_ANY, data_index, mem_val));*/
  SOC_REG_ABOVE_64_CLEAR(mem_val);

#ifdef PLISIM /* Preserve logic for simulation */
  if (!SAL_BOOT_PLISIM) 
#endif
{
  soc_mem_field32_set(unit, PPDB_A_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_RDf, 0x1);
  /* those fields should be set to zero. the mem_val is cleared before so it is not needed to zero their value 
  don't eraze this. 
  soc_mem_field32_set(unit, PPDB_A_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_WRf, 0x0);
  soc_mem_field32_set(unit, PPDB_A_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_CMPf, 0x0);
  soc_mem_field32_set(unit, PPDB_A_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_RESERVEDf, 0x0);*/
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 100, exit, WRITE_PPDB_A_TCAM_BANK_COMMANDm(unit, MEM_BLOCK_ANY, data_index, mem_val));
}

  SOC_REG_ABOVE_64_CLEAR(data_out);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 120, exit, READ_PPDB_A_TCAM_BANK_COMMANDm(unit, MEM_BLOCK_ANY, data_index, mem_val));
  SOC_REG_ABOVE_64_CLEAR(data_out);

  if(nof_entries_in_line_w == 1) {
#ifdef PLISIM   
      if (SAL_BOOT_PLISIM) { 
          /* Only in the Simulation */
            soc_mem_field_get(unit, PPDB_A_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_DIf, data_out);
      }
      else
#endif
      {
          soc_mem_field_get(unit, PPDB_A_TCAM_BANK_REPLYm, mem_val, TCAM_CPU_REP_DOUTf, data_out);
      }
      SHR_BITCOPY_RANGE(tcam_data->mask, 0, data_out, 2, 80);
      SHR_BITCOPY_RANGE(tcam_data->mask, 80, data_out, 84, 80);
  }
  else
  {
#ifdef PLISIM   
    if (SAL_BOOT_PLISIM) {
      /* Only in the Simulation */
        soc_mem_field_get(unit, PPDB_A_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_DIf, data_out);
    }
    else 
#endif
    {
        soc_mem_field_get(unit, PPDB_A_TCAM_BANK_REPLYm, mem_val, TCAM_CPU_REP_DOUTf, data_out);
    }

    if(entry_offset % 2 == 0) {
      /* i.e. we should take the lsb 88 bits*/
      SHR_BITCOPY_RANGE(tcam_data->mask, 0, data_out, 2, 80); 
    } else {
      /* i.e. we should take the msb 88 bits*/
      SHR_BITCOPY_RANGE(tcam_data->mask, 0, data_out, 84, 82); 
    }
  }
#if ARAD_PP_TBL_ACC_TCAM_LOW_LEVEL_DEBUG_PRINT
  LOG_INFO(BSL_LS_SOC_MEM,
           (BSL_META_U(unit,
                       "before decode\n\r")));
  arad_tbl_access_buffer_print(tcam_data->value,ARAD_PP_IHB_TCAM_DATA_WIDTH);
  arad_tbl_access_buffer_print(tcam_data->mask,ARAD_PP_IHB_TCAM_DATA_WIDTH);
#endif
  /* convert mask
   * mask = ^read_mask | data; (Arad) 
   * mask = read_mask | data (Jericho) 
   */
  if(nof_entries_in_line_w == 1) {
      nof_words = 5;
  }
  else{
      nof_words = 3;
  }

  for(idx = 0; idx < nof_words; ++idx) {
      if (SOC_IS_JERICHO(unit)) {
          /* Different encoding in Jericho:can be seen only in RTL */
          tcam_data->mask[idx] = tcam_data->mask[idx] | tcam_data->value[idx];
      } else {
          tcam_data->mask[idx] = ~tcam_data->mask[idx] | tcam_data->value[idx];
      }
      
  }

  if(nof_words == 3) {
      SHR_BITCLR_RANGE(tcam_data->mask,80,16);
  }
#if ARAD_PP_TBL_ACC_TCAM_LOW_LEVEL_DEBUG_PRINT
  LOG_INFO(BSL_LS_SOC_MEM,
           (BSL_META_U(unit,
                       "after decode\n\r")));
#endif

  arad_tbl_access_tcam_print(tcam_data);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_tcam_tbl_read_unsafe()",0,0);
}

/* currently support only 80*/
uint32
  arad_pp_ihb_tcam_tbl_read_two_unsafe(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   uint32                           bank_ndx,
    SOC_SAND_IN   uint32                           nof_entries_in_line,
    SOC_SAND_IN   uint32                           entry_offset,
    SOC_SAND_OUT   ARAD_PP_IHB_TCAM_BANK_TBL_DATA  tcam_data[2]
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    hw_offset,
    data_index,
    nof_words,
    nof_entries_in_line_w = nof_entries_in_line,
    idx,
    tcam_idx,
    valid_fld;
  soc_reg_above_64_val_t
    mem_val,
    data_out;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_TCAM_TBL_READ_UNSAFE);

  
  sal_memset(&tcam_data[0],0x0, sizeof(ARAD_PP_IHB_TCAM_BANK_TBL_DATA));
  sal_memset(&tcam_data[1],0x0, sizeof(ARAD_PP_IHB_TCAM_BANK_TBL_DATA));
 /*
  * ONLY 80
  */
  if (nof_entries_in_line != 2)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT_ERR, 10, exit);
  }

 /*
  * in case there is two words per line which word to use.
  */
  if (entry_offset >= nof_entries_in_line_w * ARAD_TCAM_NOF_LINES_PER_BANK(unit, bank_ndx))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT_ERR, 20, exit);
  }

  /* Always 80b key */
  hw_offset = entry_offset >> 1;

  /* raeding the key */
  res = arad_pp_ihb_tcam_tbl_bank_offset_get(unit, bank_ndx, hw_offset, &data_index);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  SOC_REG_ABOVE_64_CLEAR(mem_val);

#ifdef PLISIM
  if (!SAL_BOOT_PLISIM) 
#endif
{
  soc_mem_field32_set(unit, PPDB_A_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_WRf, 0x0);
  soc_mem_field32_set(unit, PPDB_A_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_RDf, 0x1);
  soc_mem_field32_set(unit, PPDB_A_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_CMPf, 0x0);
  soc_mem_field32_set(unit, PPDB_A_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_RESERVEDf, 0x0);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, WRITE_PPDB_A_TCAM_BANK_COMMANDm(unit, MEM_BLOCK_ANY, data_index, mem_val));
}

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 55, exit, READ_PPDB_A_TCAM_BANK_COMMANDm(unit, MEM_BLOCK_ANY, data_index, mem_val));
#ifdef PLISIM   
    if (SAL_BOOT_PLISIM) {
      /* Only in the Simulation */
        valid_fld = soc_mem_field32_get(unit, PPDB_A_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_VALIDf);
    }
    else 
#endif
    {
        valid_fld = soc_mem_field32_get(unit, PPDB_A_TCAM_BANK_REPLYm, mem_val, TCAM_CPU_REP_VALIDf);
    }
 LOG_VERBOSE(BSL_LS_SOC_MEM,
             (BSL_META_U(unit,
                         SOC_DPP_MSG("      "
                         "TCAM-read: bank:%d entry:%d, entries in line: %d, "
                         " - Entry is valid: %d\n\r")), bank_ndx, entry_offset, nof_entries_in_line, valid_fld));

  SOC_REG_ABOVE_64_CLEAR(data_out);

  /* 80b key case */
#ifdef PLISIM   
  if (SAL_BOOT_PLISIM) {
    /* Only in the Simulation */
      soc_mem_field_get(unit, PPDB_A_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_DIf, data_out);
  }
  else 
#endif
  {
      soc_mem_field_get(unit, PPDB_A_TCAM_BANK_REPLYm, mem_val, TCAM_CPU_REP_DOUTf, data_out);
  }

  /* first 80 */
  tcam_data[0].valid = (valid_fld & 0x1) != 0;
  SHR_BITCOPY_RANGE(tcam_data[0].value, 0, data_out, 2, 80); 

  /* second 80 */
  tcam_data[1].valid = (valid_fld & 0x2) != 0;
  SHR_BITCOPY_RANGE(tcam_data[1].value, 0, data_out, 84, 80);

  /* reading the mask */
  res = arad_pp_ihb_tcam_tbl_bank_offset_get(unit, bank_ndx, hw_offset, &data_index);
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  ++data_index;

  /*SOC_SAND_SOC_IF_ERROR_RETURN(res, 90, exit, READ_PPDB_A_TCAM_BANK_COMMANDm(unit, MEM_BLOCK_ANY, data_index, mem_val));*/
  SOC_REG_ABOVE_64_CLEAR(mem_val);

#ifdef PLISIM
  if (!SAL_BOOT_PLISIM) 
#endif
  {
    soc_mem_field32_set(unit, PPDB_A_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_WRf, 0x0);
    soc_mem_field32_set(unit, PPDB_A_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_RDf, 0x1);
    soc_mem_field32_set(unit, PPDB_A_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_CMPf, 0x0);
    soc_mem_field32_set(unit, PPDB_A_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_RESERVEDf, 0x0);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 100, exit, WRITE_PPDB_A_TCAM_BANK_COMMANDm(unit, MEM_BLOCK_ANY, data_index, mem_val));
  }

  SOC_REG_ABOVE_64_CLEAR(data_out);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 120, exit, READ_PPDB_A_TCAM_BANK_COMMANDm(unit, MEM_BLOCK_ANY, data_index, mem_val));
  SOC_REG_ABOVE_64_CLEAR(data_out);

#ifdef PLISIM   
  if (SAL_BOOT_PLISIM) {
    /* Only in the Simulation */
      soc_mem_field_get(unit, PPDB_A_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_DIf, data_out);
  }
  else 
#endif
  {
      soc_mem_field_get(unit, PPDB_A_TCAM_BANK_REPLYm, mem_val, TCAM_CPU_REP_DOUTf, data_out);
  }
  
  /* i.e. we should take the lsb 88 bits*/
  SHR_BITCOPY_RANGE(tcam_data[0].mask, 0, data_out, 2, 80); 
  SHR_BITCOPY_RANGE(tcam_data[1].mask, 0, data_out, 84, 82); 

#if ARAD_PP_TBL_ACC_TCAM_LOW_LEVEL_DEBUG_PRINT
  LOG_INFO(BSL_LS_SOC_MEM,
           (BSL_META_U(unit,
                       "before decode 1\n\r")));
  arad_tbl_access_buffer_print(tcam_data[0].value,ARAD_PP_IHB_TCAM_DATA_WIDTH);
  arad_tbl_access_buffer_print(tcam_data[0].mask,ARAD_PP_IHB_TCAM_DATA_WIDTH);
  LOG_INFO(BSL_LS_SOC_MEM,
           (BSL_META_U(unit,
                       "before decode 2\n\r")));
  arad_tbl_access_buffer_print(tcam_data[1].value,ARAD_PP_IHB_TCAM_DATA_WIDTH);
  arad_tbl_access_buffer_print(tcam_data[1].mask,ARAD_PP_IHB_TCAM_DATA_WIDTH);
#endif
  /* convert mask
   * mask = ^read_mask | data; (Arad) 
   * mask = read_mask | data (Jericho) 
   */
  nof_words = 3;


  for(tcam_idx = 0; tcam_idx < 2; ++tcam_idx) {
      for(idx = 0; idx < nof_words; ++idx) {
          if (SOC_IS_JERICHO(unit)) {
              /* Different encoding in Jericho:can be seen only in RTL */
              tcam_data[tcam_idx].mask[idx] = tcam_data[tcam_idx].mask[idx] | tcam_data[tcam_idx].value[idx];
          } else {
              tcam_data[tcam_idx].mask[idx] = ~tcam_data[tcam_idx].mask[idx] | tcam_data[tcam_idx].value[idx];
          }
      }

      if(nof_words == 3) {
          SHR_BITCLR_RANGE(tcam_data[tcam_idx].mask,80,16);
      }
    #if ARAD_PP_TBL_ACC_TCAM_LOW_LEVEL_DEBUG_PRINT
      LOG_INFO(BSL_LS_SOC_MEM,
               (BSL_META_U(unit,
                           "after decode\n\r")));
    #endif
      arad_tbl_access_tcam_print(&tcam_data[tcam_idx]);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_tcam_tbl_read_unsafe()",0,0);
}



STATIC
  uint32
    arad_pp_ihb_tcam_entries_in_line_to_format(
        SOC_SAND_IN   uint32                           nof_entries_in_line
    )
{
    switch(nof_entries_in_line) {
    case 2:
        return 0;
    case ARAD_PP_IHB_TCAM_HALF_ENTRY_IN_LINE:
        return 3;
    case 1:
    default:
        return 1;
    }
    return 1;
}
    
/*
 * get the TCAM_CPU_CMD_WRf value
 */
uint32
  arad_pp_ihb_tcam_tbl_write_tcam_cpu_cmd_wrf_get(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   uint32                           data_index
  )
{

    if (SOC_IS_JERICHO(unit)){
        return 0x3;
    } else {
        return 0x1;
    }
}
    
/*
 * Write a single line of the TCAM Bank table
 */
STATIC uint32
  arad_pp_ihb_tcam_tbl_write_single_line_mask(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   uint32                           bank_ndx,
    SOC_SAND_IN   uint32                           data_index,
    SOC_SAND_IN   uint32                           tcam_in_data_mask2[ARAD_PP_IHB_TCAM_DATA_WIDTH+1]
  )
{
  uint32
    res = SOC_SAND_OK;
  soc_reg_above_64_val_t
    mem_val,
    fld_val;
    
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_TCAM_TBL_WRITE_UNSAFE);

  SOC_REG_ABOVE_64_CLEAR(mem_val);

  /* In Jericho, TCAM_CPU_CMD_WRf is enlarged to allow independent 80b entry configurations */
  soc_mem_field32_set(unit, PPDB_A_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_WRf, 
                      arad_pp_ihb_tcam_tbl_write_tcam_cpu_cmd_wrf_get(unit, data_index));
  /* those fields should be set to zero. the mem_val is cleared before so it is not needed to zero their value 
  don't erase this. */
  soc_mem_field32_set(unit, PPDB_A_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_RDf, 0x0);
  soc_mem_field32_set(unit, PPDB_A_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_CMPf, 0x0);
  soc_mem_field32_set(unit, PPDB_A_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_RESERVEDf, 0x0);

  SOC_REG_ABOVE_64_CLEAR(fld_val);
  sal_memcpy(fld_val, tcam_in_data_mask2, (ARAD_PP_IHB_TCAM_DATA_WIDTH+1) * sizeof(uint32));
  soc_mem_field_set(unit, PPDB_A_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_DIf, fld_val);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, WRITE_PPDB_A_TCAM_BANK_COMMANDm(unit, MEM_BLOCK_ANY, data_index, mem_val));

#if ARAD_PP_TBL_ACC_TCAM_LOW_LEVEL_DEBUG_PRINT
{
    uint32
        print_indx;
    LOG_INFO(BSL_LS_SOC_MEM,
             (BSL_META_U(unit,
                         "write to address%08x data: \n\r"),data_index));
    for(print_indx = 6;print_indx>0;--print_indx) {
        LOG_INFO(BSL_LS_SOC_MEM,
                 (BSL_META_U(unit,
                             "%08x"), mem_val[print_indx-1]));
    }
    LOG_INFO(BSL_LS_SOC_MEM,
             (BSL_META_U(unit,
                         "\n\r")));
}
#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_tcam_tbl_write_single_line_mask()",0,0);
}
    
/*
 * Write a single line of the TCAM Bank table
 */
STATIC uint32
  arad_pp_ihb_tcam_tbl_write_single_line_value(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   uint32                           bank_ndx,
    SOC_SAND_IN   uint32                           data_index,
    SOC_SAND_IN   uint32                           valid_fld,
    SOC_SAND_IN   uint32                           tcam_in_data_value2[ARAD_PP_IHB_TCAM_DATA_WIDTH+1]
  )
{
  uint32
    res = SOC_SAND_OK;
  soc_reg_above_64_val_t
    mem_val,
    fld_val;
    
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_TCAM_TBL_WRITE_UNSAFE);

  SOC_REG_ABOVE_64_CLEAR(mem_val);

  /* In Jericho, TCAM_CPU_CMD_WRf is enlarged to allow independent 80b entry configurations */
  soc_mem_field32_set(unit, PPDB_A_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_WRf, 
                      arad_pp_ihb_tcam_tbl_write_tcam_cpu_cmd_wrf_get(unit, data_index));
    /* those fields should be set to zero. the mem_val is cleared before so it is not needed to zero their value 
  don't eraze this. */
  soc_mem_field32_set(unit, PPDB_A_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_RDf, 0x0);
  soc_mem_field32_set(unit, PPDB_A_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_CMPf, 0x0);
  soc_mem_field32_set(unit, PPDB_A_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_RESERVEDf, 0x0);

  SOC_REG_ABOVE_64_CLEAR(fld_val);

#if ARAD_PP_TBL_ACC_TCAM_LOW_LEVEL_DEBUG_PRINT
{
    uint32
        print_indx;
    LOG_INFO(BSL_LS_SOC_MEM,
             (BSL_META_U(unit,
                         "write to mem-val without DI %08x data: \n\r"),data_index));
    for(print_indx = 6;print_indx>0;--print_indx) {
        LOG_INFO(BSL_LS_SOC_MEM,
                 (BSL_META_U(unit,
                             "%08x"), mem_val[print_indx-1]));
    }
    LOG_INFO(BSL_LS_SOC_MEM,
             (BSL_META_U(unit,
                         "\n\r")));
    LOG_INFO(BSL_LS_SOC_MEM,
             (BSL_META_U(unit,
                         "tcam_in_data_value2: DI\n\r")));
    for(print_indx = 6;print_indx>0;--print_indx) {
        LOG_INFO(BSL_LS_SOC_MEM,
                 (BSL_META_U(unit,
                             "%08x"), tcam_in_data_value2[print_indx-1]));
    }
    LOG_INFO(BSL_LS_SOC_MEM,
             (BSL_META_U(unit,
                         "\n\r")));
}
#endif

  sal_memcpy(fld_val, tcam_in_data_value2, (ARAD_PP_IHB_TCAM_DATA_WIDTH+1) * sizeof(uint32));
  soc_mem_field_set(unit, PPDB_A_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_DIf, fld_val);
#if ARAD_PP_TBL_ACC_TCAM_LOW_LEVEL_DEBUG_PRINT
{
    uint32
        print_indx;
    LOG_INFO(BSL_LS_SOC_MEM,
             (BSL_META_U(unit,
                         "write to address%08x data: \n\r"),data_index));
    for(print_indx = 6;print_indx>0;--print_indx) {
        LOG_INFO(BSL_LS_SOC_MEM,
                 (BSL_META_U(unit,
                             "%08x"), mem_val[print_indx-1]));
    }
    LOG_INFO(BSL_LS_SOC_MEM,
             (BSL_META_U(unit,
                         "\n\r")));
    LOG_INFO(BSL_LS_SOC_MEM,
             (BSL_META_U(unit,
                         "fld_val\n\r")));
    for(print_indx = 6;print_indx>0;--print_indx) {
        LOG_INFO(BSL_LS_SOC_MEM,
                 (BSL_META_U(unit,
                             "%08x"), fld_val[print_indx-1]));
    }
    LOG_INFO(BSL_LS_SOC_MEM,
             (BSL_META_U(unit,
                         "\n\r")));
}
#endif

  soc_mem_field32_set(unit, PPDB_A_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_VALIDf, valid_fld);
#if ARAD_PP_TBL_ACC_TCAM_LOW_LEVEL_DEBUG_PRINT
{
    uint32
        print_indx;
    LOG_INFO(BSL_LS_SOC_MEM,
             (BSL_META_U(unit,
                         "write to address%08x data: \n\r"),data_index));
    for(print_indx = 6;print_indx>0;--print_indx) {
        LOG_INFO(BSL_LS_SOC_MEM,
                 (BSL_META_U(unit,
                             "%08x"), mem_val[print_indx-1]));
    }
    LOG_INFO(BSL_LS_SOC_MEM,
             (BSL_META_U(unit,
                         "\n\r")));
}
#endif
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, WRITE_PPDB_A_TCAM_BANK_COMMANDm(unit, MEM_BLOCK_ANY, data_index, mem_val));
#if ARAD_PP_TBL_ACC_TCAM_LOW_LEVEL_DEBUG_PRINT
{
    uint32
        print_indx;
    LOG_INFO(BSL_LS_SOC_MEM,
             (BSL_META_U(unit,
                         "write to address%08x data: \n\r"),data_index));
    for(print_indx = 6;print_indx>0;--print_indx) {
        LOG_INFO(BSL_LS_SOC_MEM,
                 (BSL_META_U(unit,
                             "%08x"), mem_val[print_indx-1]));
    }
    LOG_INFO(BSL_LS_SOC_MEM,
             (BSL_META_U(unit,
                         "\n\r")));
}
#endif


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_tcam_tbl_write_single_line_value()",0,0);
}
    


/*
 * Write to Tcam Banks from block IHB,
 * takes semaphore
 */
uint32
  arad_pp_ihb_tcam_tbl_write_unsafe(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   uint32                           bank_ndx,
    SOC_SAND_IN   uint32                           nof_entries_in_line,
    SOC_SAND_IN   uint32                           entry_offset,
    SOC_SAND_IN   ARAD_PP_IHB_TCAM_BANK_TBL_DATA   *tcam_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    hw_offset,
    data_index,
    indx,
    entry_mode,
    old_place=0,
    new_place=0,
    nof_words,
    nof_entries_in_line_w = nof_entries_in_line,
    valid_fld;
  uint32
      tcam_in_data_mask[ARAD_PP_IHB_TCAM_DATA_WIDTH+1],
      tcam_in_data_value[ARAD_PP_IHB_TCAM_DATA_WIDTH+1],
      tcam_in_data_mask2[ARAD_PP_IHB_TCAM_DATA_WIDTH+1],
      tcam_in_data_value2[ARAD_PP_IHB_TCAM_DATA_WIDTH+1];
  ARAD_PP_IHB_TCAM_BANK_TBL_DATA
      read_tcam_data;
  ARAD_PP_IHB_TCAM_BANK_TBL_DATA
      write_tcam_data;
    
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_TCAM_TBL_WRITE_UNSAFE);

 /*
  * in ARAD the only use of the TACM for 164 bits word (one word per line) 82 bits words (2 words per line) 
  *  nof_entries_in_line      width
  *  --------------------------------
  *  1                  160
  *  2                  80
  *  ARAD_PP_IHB_TCAM_HALF_ENTRY_IN_LINE                  320
  */
  if (nof_entries_in_line == 0 || ((nof_entries_in_line > 2) && (nof_entries_in_line !=ARAD_PP_IHB_TCAM_HALF_ENTRY_IN_LINE)))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT_ERR, 10, exit);
  }
  if(nof_entries_in_line == ARAD_PP_IHB_TCAM_HALF_ENTRY_IN_LINE) {
      nof_entries_in_line_w = 1;
  }
 /*
  * in case there are two words per line which word to use.
  */
  if (entry_offset >= nof_entries_in_line_w * ARAD_TCAM_NOF_LINES_PER_BANK(unit, bank_ndx))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT_ERR, 20, exit);
  }
 /*
  * low word or high word
  */
  if (nof_entries_in_line_w == 1)
  {
    hw_offset = entry_offset; /* 160b or 320b */
  }
  else if (nof_entries_in_line_w == 2)
  {
    hw_offset = entry_offset >> 1;
  }
  else{
      SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT_ERR, 21, exit);
  }

 LOG_VERBOSE(BSL_LS_SOC_MEM,
             (BSL_META_U(unit,
                         SOC_DPP_MSG("      "
                         "TCAM-write: bank:%d entry:%d, entries in line: %d\n\r")), bank_ndx, entry_offset, nof_entries_in_line));
  arad_tbl_access_tcam_print(tcam_data);

  
  /* finally we will write, write_tcam_data */
  sal_memset(&write_tcam_data,0x0, sizeof(ARAD_PP_IHB_TCAM_BANK_TBL_DATA));


  /* if mode is two entries in line then read other side */
  if(nof_entries_in_line == 2) {
      valid_fld = 0;

      /* place: 0 or 1 */
      new_place = entry_offset%2;
      old_place = (uint32)(1 - new_place);

      res = 
        arad_pp_ihb_tcam_tbl_read_unsafe(
            unit,
            bank_ndx,
            nof_entries_in_line,
            (entry_offset%2)?entry_offset-1:entry_offset+1, /* my second half */
            &read_tcam_data
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

      /* set old valid bit into valid bits */
      if(read_tcam_data.valid) {
          valid_fld = (1 << old_place);
      }
      /* set new valid bit into valid bits */
      valid_fld |= (tcam_data->valid << new_place);

      /* set old and new mask value parts according to placement */
      SHR_BITCOPY_RANGE(write_tcam_data.mask,80*new_place,tcam_data->mask,0,80);
      SHR_BITCOPY_RANGE(write_tcam_data.value,80*new_place,tcam_data->value,0,80);
      if(read_tcam_data.valid) {
          SHR_BITCOPY_RANGE(write_tcam_data.mask,80*old_place,read_tcam_data.mask,0,80);
          SHR_BITCOPY_RANGE(write_tcam_data.value,80*old_place,read_tcam_data.value,0,80);
      }
      write_tcam_data.valid = valid_fld != 0;
#if ARAD_PP_TBL_ACC_TCAM_LOW_LEVEL_DEBUG_PRINT
  LOG_INFO(BSL_LS_SOC_MEM,
           (BSL_META_U(unit,
                       "full line: \n\r")));
  arad_tbl_access_tcam_print(&write_tcam_data);
#endif
      
  }
  else if (nof_entries_in_line_w == 1){
      sal_memcpy(&write_tcam_data, tcam_data, sizeof(ARAD_PP_IHB_TCAM_BANK_TBL_DATA));
      if(tcam_data->valid) {
          valid_fld = 3;
      }
      else{
          valid_fld = 0;
      }
  }
  else{
      SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT_ERR, 21, exit);/* 320 not implemented yet*/
  }
  entry_mode = arad_pp_ihb_tcam_entries_in_line_to_format(nof_entries_in_line);

 /*
  * calc key and mask 
  * KEY1 (mask) = key | ~mask (Arad) 
  * KEY1 (mask) = ~key & mask (Jericho) 
  * KEY0 (data) = key & mask
  */
  sal_memset(tcam_in_data_mask,0x0, sizeof(uint32)*(ARAD_PP_IHB_TCAM_DATA_WIDTH+1));
  sal_memset(tcam_in_data_value,0x0, sizeof(uint32)*(ARAD_PP_IHB_TCAM_DATA_WIDTH+1));

  /* nof_words = (ARAD_PP_IHB_TCAM_DATA_WIDTH + (nof_entries_in_line -  1)) / nof_entries_in_line;*/
  nof_words = 5;
  for (indx = 0; indx < nof_words; ++indx)
  {
      if (SOC_IS_JERICHO(unit)) {
          /* Different encoding in Jericho:can be seen only in RTL */
          tcam_in_data_mask[indx] = (~(write_tcam_data.value[indx])) & write_tcam_data.mask[indx];
      } else {
          tcam_in_data_mask[indx] = write_tcam_data.value[indx] | ~(write_tcam_data.mask[indx]);
      }
    tcam_in_data_value[indx] = write_tcam_data.value[indx] & write_tcam_data.mask[indx];
  }

#if ARAD_PP_TBL_ACC_TCAM_LOW_LEVEL_DEBUG_PRINT
  LOG_INFO(BSL_LS_SOC_MEM,
           (BSL_META_U(unit,
                       "after encode: \n\r")));
  arad_tbl_access_buffer_print(tcam_in_data_value, ARAD_PP_IHB_TCAM_DATA_WIDTH+1);
  arad_tbl_access_buffer_print(tcam_in_data_mask, ARAD_PP_IHB_TCAM_DATA_WIDTH+1);
#endif

  
  sal_memset(tcam_in_data_mask2,0x0, sizeof(uint32)*(ARAD_PP_IHB_TCAM_DATA_WIDTH+1));
  sal_memset(tcam_in_data_value2,0x0, sizeof(uint32)*(ARAD_PP_IHB_TCAM_DATA_WIDTH+1));

  SHR_BITCOPY_RANGE(tcam_in_data_mask2,2,tcam_in_data_mask,0,80);
  SHR_BITCOPY_RANGE(tcam_in_data_value2,2,tcam_in_data_value,0,80);

  SHR_BITCOPY_RANGE(tcam_in_data_mask2,84,tcam_in_data_mask,80,80);
  SHR_BITCOPY_RANGE(tcam_in_data_value2,84,tcam_in_data_value,80,80);

  /* bits 0-1, and 82-83 for entry type */
  SHR_BITCOPY_RANGE(tcam_in_data_mask2,0,&entry_mode,0,2);
  SHR_BITCOPY_RANGE(tcam_in_data_mask2,82,&entry_mode,0,2);

  SHR_BITCOPY_RANGE(tcam_in_data_value2,0,&entry_mode,0,2);
  SHR_BITCOPY_RANGE(tcam_in_data_value2,82,&entry_mode,0,2);
  
#if ARAD_PP_TBL_ACC_TCAM_LOW_LEVEL_DEBUG_PRINT
  LOG_INFO(BSL_LS_SOC_MEM,
           (BSL_META_U(unit,
                       "after shifting: \n\r")));
  arad_tbl_access_buffer_print(tcam_in_data_value2, ARAD_PP_IHB_TCAM_DATA_WIDTH+1);
  arad_tbl_access_buffer_print(tcam_in_data_mask2, ARAD_PP_IHB_TCAM_DATA_WIDTH+1);
#endif

  /* 
   * In order not to influence the traffic: 
   * - for a valid entry, write the mask first, and then data + valid 
   * - for an invalid entry,  write the data + valid first, and then mask
   */ 
  if (tcam_data->valid) {
      res = arad_pp_ihb_tcam_tbl_bank_offset_get(unit, bank_ndx, hw_offset, &data_index);
      SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);
      ++data_index; /* ++ for mask */
      /* writing the mask first not to influence traffic */
      res = arad_pp_ihb_tcam_tbl_write_single_line_mask(
                unit,
                bank_ndx,
                data_index,
                tcam_in_data_mask2
              );
      SOC_SAND_CHECK_FUNC_RESULT(res, 28, exit);

      /* writing the key */
      res = arad_pp_ihb_tcam_tbl_bank_offset_get(unit, bank_ndx, hw_offset, &data_index);
      SOC_SAND_CHECK_FUNC_RESULT(res, 29, exit);  
      res = arad_pp_ihb_tcam_tbl_write_single_line_value(
                unit,
                bank_ndx,
                data_index,
                valid_fld,
                tcam_in_data_value2
              );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }
  else { /* tcam_data->valid invalid */
      /* writing the key */
      res = arad_pp_ihb_tcam_tbl_bank_offset_get(unit, bank_ndx, hw_offset, &data_index);
      SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);
      res = arad_pp_ihb_tcam_tbl_write_single_line_value(
                unit,
                bank_ndx,
                data_index,
                valid_fld,
                tcam_in_data_value2
              );
      SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

      res = arad_pp_ihb_tcam_tbl_bank_offset_get(unit, bank_ndx, hw_offset, &data_index);
      SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);
      ++data_index; /* ++ for mask */
      /* writing the mask first not to influence traffic */
      res = arad_pp_ihb_tcam_tbl_write_single_line_mask(
                unit,
                bank_ndx,
                data_index,
                tcam_in_data_mask2
              );
      SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_tcam_tbl_write_unsafe()",0,0);
}


/*
 * Write to Tcam Banks from block IHB, use only for 80 bits!
 * takes semaphore
 */
uint32
  arad_pp_ihb_tcam_tbl_write_two_unsafe(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   uint32                           bank_ndx,
    SOC_SAND_IN   uint32                           nof_entries_in_line,
    SOC_SAND_IN   uint32                           entry_offset,
    SOC_SAND_IN   ARAD_PP_IHB_TCAM_BANK_TBL_DATA   tcam_data[2]
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    hw_offset,
    data_index,
    indx,
    entry_mode,
    nof_words,
    nof_entries_in_line_w = nof_entries_in_line,
    valid_fld;
  uint32
      tcam_in_data_mask[ARAD_PP_IHB_TCAM_DATA_WIDTH+1],
      tcam_in_data_value[ARAD_PP_IHB_TCAM_DATA_WIDTH+1],
      tcam_in_data_mask2[ARAD_PP_IHB_TCAM_DATA_WIDTH+1],
      tcam_in_data_value2[ARAD_PP_IHB_TCAM_DATA_WIDTH+1];
  ARAD_PP_IHB_TCAM_BANK_TBL_DATA
      write_tcam_data;
    
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_TCAM_TBL_WRITE_UNSAFE);

 /*
  * in ARAD the only use of the TACM for 164 bits word (one word per line) 82 bits words (2 words per line) 
  *  nof_entries_in_line      width
  *  --------------------------------
  *  1                  160
  *  2                  80
  *  ARAD_PP_IHB_TCAM_HALF_ENTRY_IN_LINE                  320
  */
  if (nof_entries_in_line != 2 )
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT_ERR, 10, exit);
  }

 /*
  * in case there are two words per line which word to use.
  */
  if (entry_offset >= nof_entries_in_line_w * ARAD_TCAM_NOF_LINES_PER_BANK(unit, bank_ndx))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT_ERR, 20, exit);
  }
 /*
  * low word or high word
  */
 hw_offset = entry_offset >> 1;

 LOG_VERBOSE(BSL_LS_SOC_MEM,
             (BSL_META_U(unit,
                         SOC_DPP_MSG("      "
                         "TCAM-write: bank:%d entry:%d, entries in line: %d\n\r")), bank_ndx, entry_offset, nof_entries_in_line));
  arad_tbl_access_tcam_print(tcam_data);

  
  /* finally we will write, write_tcam_data */
  sal_memset(&write_tcam_data,0x0, sizeof(ARAD_PP_IHB_TCAM_BANK_TBL_DATA));


  /* if mode is two entries in line then read other side */

  /* set valid bit into valid bits */
  valid_fld = tcam_data[0].valid | (tcam_data[1].valid << 1);

  /* set old and new mask value parts according to placement */
  SHR_BITCOPY_RANGE(write_tcam_data.mask,0,tcam_data[0].mask,0,80);
  SHR_BITCOPY_RANGE(write_tcam_data.value,0,tcam_data[0].value,0,80);
  SHR_BITCOPY_RANGE(write_tcam_data.mask,80,tcam_data[1].mask,0,80);
  SHR_BITCOPY_RANGE(write_tcam_data.value,80,tcam_data[1].value,0,80);
#if ARAD_PP_TBL_ACC_TCAM_LOW_LEVEL_DEBUG_PRINT
  LOG_INFO(BSL_LS_SOC_MEM,
           (BSL_META_U(unit,
                       "full line: \n\r")));
  arad_tbl_access_tcam_print(&write_tcam_data);
#endif

  entry_mode = arad_pp_ihb_tcam_entries_in_line_to_format(nof_entries_in_line);

 /*
  * calc key and mask 
  * KEY1 (mask) = key | ~mask (Arad) 
  * KEY1 (mask) = ~key & mask (Jericho) 
  * KEY0 (data) = key & mask
  */
  sal_memset(tcam_in_data_mask,0x0, sizeof(uint32)*(ARAD_PP_IHB_TCAM_DATA_WIDTH+1));
  sal_memset(tcam_in_data_value,0x0, sizeof(uint32)*(ARAD_PP_IHB_TCAM_DATA_WIDTH+1));

  /* nof_words = (ARAD_PP_IHB_TCAM_DATA_WIDTH + (nof_entries_in_line -  1)) / nof_entries_in_line;*/
  nof_words = 5;
  for (indx = 0; indx < nof_words; ++indx)
  {
      if (SOC_IS_JERICHO(unit)) {
          /* Different encoding in Jericho:can be seen only in RTL */
          tcam_in_data_mask[indx] = (~(write_tcam_data.value[indx])) & write_tcam_data.mask[indx];
      } else {
          tcam_in_data_mask[indx] = write_tcam_data.value[indx] | ~(write_tcam_data.mask[indx]);
      }
    tcam_in_data_value[indx] = write_tcam_data.value[indx] & write_tcam_data.mask[indx];
  }

#if ARAD_PP_TBL_ACC_TCAM_LOW_LEVEL_DEBUG_PRINT
  LOG_INFO(BSL_LS_SOC_MEM,
           (BSL_META_U(unit,
                       "after encode: \n\r")));
  arad_tbl_access_buffer_print(tcam_in_data_value, ARAD_PP_IHB_TCAM_DATA_WIDTH+1);
  arad_tbl_access_buffer_print(tcam_in_data_mask, ARAD_PP_IHB_TCAM_DATA_WIDTH+1);
#endif

  
  sal_memset(tcam_in_data_mask2,0x0, sizeof(uint32)*(ARAD_PP_IHB_TCAM_DATA_WIDTH+1));
  sal_memset(tcam_in_data_value2,0x0, sizeof(uint32)*(ARAD_PP_IHB_TCAM_DATA_WIDTH+1));

  SHR_BITCOPY_RANGE(tcam_in_data_mask2,2,tcam_in_data_mask,0,80);
  SHR_BITCOPY_RANGE(tcam_in_data_value2,2,tcam_in_data_value,0,80);

  SHR_BITCOPY_RANGE(tcam_in_data_mask2,84,tcam_in_data_mask,80,80);
  SHR_BITCOPY_RANGE(tcam_in_data_value2,84,tcam_in_data_value,80,80);

  /* bits 0-1, and 82-83 for entry type */
  SHR_BITCOPY_RANGE(tcam_in_data_mask2,0,&entry_mode,0,2);
  SHR_BITCOPY_RANGE(tcam_in_data_mask2,82,&entry_mode,0,2);

  SHR_BITCOPY_RANGE(tcam_in_data_value2,0,&entry_mode,0,2);
  SHR_BITCOPY_RANGE(tcam_in_data_value2,82,&entry_mode,0,2);
  
#if ARAD_PP_TBL_ACC_TCAM_LOW_LEVEL_DEBUG_PRINT
  LOG_INFO(BSL_LS_SOC_MEM,
           (BSL_META_U(unit,
                       "after shifting: \n\r")));
  arad_tbl_access_buffer_print(tcam_in_data_value2, ARAD_PP_IHB_TCAM_DATA_WIDTH+1);
  arad_tbl_access_buffer_print(tcam_in_data_mask2, ARAD_PP_IHB_TCAM_DATA_WIDTH+1);
#endif

  /* 
   * This function is mainly used to invalidate entries, 
   * so write the valid bit first 
   */
  /* writing the key */
  res = arad_pp_ihb_tcam_tbl_bank_offset_get(unit, bank_ndx, hw_offset, &data_index);
  SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);
  res = arad_pp_ihb_tcam_tbl_write_single_line_value(
            unit,
            bank_ndx,
            data_index,
            valid_fld,
            tcam_in_data_value2
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = arad_pp_ihb_tcam_tbl_bank_offset_get(unit, bank_ndx, hw_offset, &data_index);
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);
  ++data_index; /* ++ for mask */
  /* writing the mask first not to influence traffic */
  res = arad_pp_ihb_tcam_tbl_write_single_line_mask(
            unit,
            bank_ndx,
            data_index,
            tcam_in_data_mask2
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_tcam_tbl_write_two_unsafe()",0,0);
}

uint32
  arad_pp_ihb_pmf_pass_2_key_update_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHB_PMF_PASS_2_KEY_UPDATE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHB_PMF_PASS_2_KEY_UPDATE_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_PMF_PASS_2_KEY_UPDATE_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHB_PMF_PASS_2_KEY_UPDATE_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHB_PMF_PASS_2_KEY_UPDATE_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHB_PMF_PASS_2_KEY_UPDATEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->action_select = soc_mem_field32_get(unit, IHB_PMF_PASS_2_KEY_UPDATEm, data, ACTION_SELECTf);
  tbl_data->enable_update_key = soc_mem_field32_get(unit, IHB_PMF_PASS_2_KEY_UPDATEm, data, ENABLE_UPDATE_KEYf);
#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit)) {
      tbl_data->key_d_use_compare_result = soc_mem_field32_get(unit, IHB_PMF_PASS_2_KEY_UPDATEm, data, KEY_D_USE_COMPARE_RESULTf);  
      tbl_data->key_d_mask_select = soc_mem_field32_get(unit, IHB_PMF_PASS_2_KEY_UPDATEm, data, KEY_D_MASK_SELECTf);  
      tbl_data->key_d_xor_enable = soc_mem_field32_get(unit, IHB_PMF_PASS_2_KEY_UPDATEm, data, KEY_D_XOR_ENABLEf);  
  }
#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_pmf_pass_2_key_update_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihb_pmf_pass_2_key_update_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHB_PMF_PASS_2_KEY_UPDATE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHB_PMF_PASS_2_KEY_UPDATE_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_PMF_PASS_2_KEY_UPDATE_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHB_PMF_PASS_2_KEY_UPDATE_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHB_PMF_PASS_2_KEY_UPDATEm, data, ACTION_SELECTf, tbl_data->action_select);
  soc_mem_field32_set(unit, IHB_PMF_PASS_2_KEY_UPDATEm, data, ENABLE_UPDATE_KEYf, tbl_data->enable_update_key);
#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit)) {
      soc_mem_field32_set(unit, IHB_PMF_PASS_2_KEY_UPDATEm, data, KEY_D_USE_COMPARE_RESULTf, tbl_data->key_d_use_compare_result);
      soc_mem_field32_set(unit, IHB_PMF_PASS_2_KEY_UPDATEm, data, KEY_D_MASK_SELECTf, tbl_data->key_d_mask_select);
      soc_mem_field32_set(unit, IHB_PMF_PASS_2_KEY_UPDATEm, data, KEY_D_XOR_ENABLEf, tbl_data->key_d_xor_enable);
  }
#endif

  res = soc_mem_write(
          unit,
          IHB_PMF_PASS_2_KEY_UPDATEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_pmf_pass_2_key_update_tbl_set_unsafe()", entry_offset, 0);
}




/*
 * Read to Tcam Banks from block IHB,
 * doesn't take semaphore.
 */
uint32
  arad_pp_ihb_tcam_access_profile_tbl_read_unsafe(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   uint32                           access_profile_id,
    SOC_SAND_OUT  ARAD_PP_IHB_TCAM_ACCESS_PROFILE_TBL_DATA    *data
  )
{
  uint32
    mem_val[2] = {0},
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_TCAM_ACCESS_PROFILE_TBL_READ_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(data);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_PPDB_A_TCAM_ACCESS_PROFILEm(unit, MEM_BLOCK_ANY, access_profile_id, mem_val));
  soc_mem_field_get(unit, PPDB_A_TCAM_ACCESS_PROFILEm, mem_val, ACTION_BITMAPf, &data->action_bitmap);
  soc_mem_field_get(unit, PPDB_A_TCAM_ACCESS_PROFILEm, mem_val, KEY_SIZEf, &data->key_size);
  soc_mem_field_get(unit, PPDB_A_TCAM_ACCESS_PROFILEm, mem_val, IS_DIRECTf, &data->is_direct);
  soc_mem_field_get(unit, PPDB_A_TCAM_ACCESS_PROFILEm, mem_val, ANDf, &data->prefix_and);
  soc_mem_field_get(unit, PPDB_A_TCAM_ACCESS_PROFILEm, mem_val, ORf, &data->prefix_or);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_tcam_access_profile_tbl_read_unsafe()",access_profile_id,0);
}

/*
 * Write to Tcam Banks from block IHB,
 * doesn't take semaphore.
 */
uint32
  arad_pp_ihb_tcam_access_profile_tbl_write_unsafe(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   uint32                           access_profile_id,
    SOC_SAND_IN   ARAD_PP_IHB_TCAM_ACCESS_PROFILE_TBL_DATA     *data
  )
{
  uint32
    mem_val[2] = {0},
    fld_val,
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_TCAM_ACCESS_PROFILE_TBL_WRITE_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(data);

  
  fld_val = data->action_bitmap;
  soc_mem_field_set(unit, PPDB_A_TCAM_ACCESS_PROFILEm, mem_val, ACTION_BITMAPf, &fld_val);
  fld_val = data->key_size;
  soc_mem_field_set(unit, PPDB_A_TCAM_ACCESS_PROFILEm, mem_val, KEY_SIZEf, &fld_val);
  fld_val = data->is_direct;
  soc_mem_field_set(unit, PPDB_A_TCAM_ACCESS_PROFILEm, mem_val, IS_DIRECTf, &fld_val);
  fld_val = data->prefix_and;
  soc_mem_field_set(unit, PPDB_A_TCAM_ACCESS_PROFILEm, mem_val, ANDf, &fld_val);
  fld_val = data->prefix_or;
  soc_mem_field_set(unit, PPDB_A_TCAM_ACCESS_PROFILEm, mem_val, ORf, &fld_val);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, WRITE_PPDB_A_TCAM_ACCESS_PROFILEm(unit, MEM_BLOCK_ANY, access_profile_id, mem_val));
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_tcam_access_profile_tbl_write_unsafe()",0,0);
}

uint32
  arad_pp_ihb_tcam_pd_profile_tbl_read_unsafe(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   uint32                           pd_profile_id,
    SOC_SAND_OUT  ARAD_PP_IHB_TCAM_PD_PROFILE_TBL_DATA    *data
  )
{
  uint32
    mem_val = 0,
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_TCAM_PD_PROFILE_TBL_READ_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(data);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, READ_PPDB_A_TCAM_PD_PROFILEm(unit, MEM_BLOCK_ANY, pd_profile_id, &mem_val));
  soc_mem_field_get(unit, PPDB_A_TCAM_PD_PROFILEm, &mem_val, BITMAPf, &data->bitmap);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_tcam_pd_profile_tbl_read_unsafe()",0,0);
}

uint32
  arad_pp_ihb_tcam_pd_profile_tbl_write_unsafe(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   uint32                           pd_profile_id,
    SOC_SAND_IN   ARAD_PP_IHB_TCAM_PD_PROFILE_TBL_DATA     *data
  )
{
  uint32
    mem_val = 0,
    fld_val,
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_TCAM_PD_PROFILE_TBL_WRITE_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(data);
  
  fld_val = data->bitmap;
  soc_mem_field_set(unit, PPDB_A_TCAM_PD_PROFILEm, &mem_val, BITMAPf, &fld_val);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, WRITE_PPDB_A_TCAM_PD_PROFILEm(unit, MEM_BLOCK_ANY, pd_profile_id, &mem_val));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_tcam_pd_profile_tbl_write_unsafe()",0,0);
}

/*
 * Set the valid bit in the TCAM
 */
uint32
  arad_pp_ihb_tcam_invalid_bit_set_unsafe(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   uint32                           bank_ndx,
    SOC_SAND_IN   uint32                           nof_entries_in_line,
    SOC_SAND_IN   uint32                           entry_offset
  )
{
  uint32
    res = SOC_SAND_OK,
    hw_offset,
    nof_entries_in_line_w = nof_entries_in_line;
  ARAD_PP_IHB_TCAM_BANK_TBL_DATA 
      read_tcam_data[2];
  uint64
    tcam_manager_1;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_TCAM_VALID_BIT_TBL_SET_UNSAFE);

 LOG_VERBOSE(BSL_LS_SOC_MEM,
             (BSL_META_U(unit,
                         SOC_DPP_MSG("      "
                         "TCAM-write-valid: bank:%d entry:%d, entries in line: %d, \n\r")), bank_ndx, entry_offset, nof_entries_in_line));

 /*
  * in ARAD the only use of the TACM for 164 bits word (one word per line) 82 bits words (2 words per line)
  */
  if (nof_entries_in_line == 0 || nof_entries_in_line > 3)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT_ERR, 10, exit);
  }
  if(nof_entries_in_line == ARAD_PP_IHB_TCAM_HALF_ENTRY_IN_LINE) {
      nof_entries_in_line_w = 1;
  }
 /*
  * in case there is two words per line which word to use.
  */
  if (entry_offset >= nof_entries_in_line_w * ARAD_TCAM_NOF_LINES_PER_BANK(unit, bank_ndx))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT_ERR, 20, exit);
  }
 /*
  * low word or high word
  */
  if (nof_entries_in_line_w == 1)
  {
    hw_offset = entry_offset;
  }
  else
  {
    hw_offset = entry_offset >> 1;
  }

  /* for 80 bits, do not use the TCAM manager - it will disable both entries in the same line */
  if(nof_entries_in_line == 2) {
      res = arad_pp_ihb_tcam_tbl_read_two_unsafe(
                unit,
                bank_ndx,
                nof_entries_in_line,
                (entry_offset%2)?entry_offset-1:entry_offset, /* even entry /base */
                read_tcam_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

      read_tcam_data[entry_offset%2].valid = 0;

      res = arad_pp_ihb_tcam_tbl_write_two_unsafe(
                unit,
                bank_ndx,
                nof_entries_in_line,
                (entry_offset%2)?entry_offset-1:entry_offset, /* even entry /base */
                read_tcam_data
            );
          SOC_SAND_CHECK_FUNC_RESULT(res, 24, exit);

      goto exit;
  }
  /* for 160 / 320 bits */
  else {
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_PPDB_A_TCAM_MANAGER_1r(unit, &tcam_manager_1));
      soc_reg64_field32_set(unit, PPDB_A_TCAM_MANAGER_1r, &tcam_manager_1, TCAM_MANAGER_SRCf, hw_offset);
      soc_reg64_field32_set(unit, PPDB_A_TCAM_MANAGER_1r, &tcam_manager_1, TCAM_MANAGER_DSTf, hw_offset);
      soc_reg64_field32_set(unit, PPDB_A_TCAM_MANAGER_1r, &tcam_manager_1, TCAM_MANAGER_RANGEf, 1);
      soc_reg64_field32_set(unit, PPDB_A_TCAM_MANAGER_1r, &tcam_manager_1, TCAM_MANAGER_BANKf, bank_ndx);
      /* Indicate if 320b entry */
      soc_reg64_field32_set(unit, PPDB_A_TCAM_MANAGER_1r, &tcam_manager_1, TCAM_MANAGER_MOVE_COUPLEf, 
                            (nof_entries_in_line == ARAD_PP_IHB_TCAM_HALF_ENTRY_IN_LINE));
      soc_reg64_field32_set(unit, PPDB_A_TCAM_MANAGER_1r, &tcam_manager_1, TCAM_MANAGER_INVALIDATEf, 0x1); /* Always invalidate */
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, WRITE_PPDB_A_TCAM_MANAGER_1r(unit, tcam_manager_1));

      SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, WRITE_PPDB_A_TCAM_MANAGER_0r(unit, 0x1));

      /* Wait trigger to become 0 */
      res = arad_polling(
              unit,
              ARAD_TIMEOUT,
              ARAD_MIN_POLLS,
              PPDB_A_TCAM_MANAGER_0r,
              REG_PORT_ANY,
              0,
              TCAM_MANAGER_TRIGGERf,
              0
            ); 
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_tcam_invalid_bit_set_unsafe()",0,0);
}

/*
 * Get indirect table Tcam Bank A from block IHB,
 * takes semaphore
 */
uint32
  arad_pp_ihb_tcam_tbl_flush_unsafe(
    SOC_SAND_IN   int          unit,
    SOC_SAND_IN   uint32          bank_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_TCAM_TBL_FLUSH_UNSAFE);

 LOG_VERBOSE(BSL_LS_SOC_MEM,
             (BSL_META_U(unit,
                         SOC_DPP_MSG("      "
                         "Invalidate bank: %d \n\r")), bank_ndx));

  /* we should set the bit of the bank index */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, WRITE_PPDB_A_TCAM_RESET_BLOCK_BITMAPr(unit, 1 << bank_ndx));

  /*need sleep */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, WRITE_PPDB_A_TCAM_RESET_BLOCK_BITMAPr(unit, 0));


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_tcam_tbl_flush_unsafe()",0,0);
}

/*
 *  Compare operation for a single TCAM bank. Note that only 72b and 144b are supported by this
 *  operation.
 */
uint32
  arad_pp_ihb_tcam_tbl_compare_unsafe(
    SOC_SAND_IN   int                             unit,
    SOC_SAND_IN   uint32                             bank_ndx,
    SOC_SAND_IN   uint32                             nof_entries_in_line,
    SOC_SAND_IN   ARAD_PP_IHB_TCAM_BANK_TBL_DATA     *compare_data,
    SOC_SAND_OUT  ARAD_PP_IHB_TCAM_COMPARE_DATA      *found_data
  )
{
  uint32
    bank_offset,
    res = SOC_SAND_OK;
  soc_reg_above_64_val_t
    mem_val,
    fld_val;
  uint32
      entry_mode,
      tcam_in_data_value[ARAD_PP_IHB_TCAM_DATA_WIDTH+1];


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_TCAM_TBL_COMPARE_UNSAFE);

 /*
  * in ARAD the only use of the TACM for 164 bits word (one word per line) 82 bits words (2 words per line)
  */
  if (nof_entries_in_line == 0 || nof_entries_in_line > 3)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT_ERR, 10, exit);
  }



  SOC_REG_ABOVE_64_CLEAR(mem_val);

  soc_mem_field32_set(unit, PPDB_A_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_CMPf, 0x1);

  /* 
  those fields should be set to zero. the mem_val is cleared before so it is not needed to zero their value 
  don't eraze this.  
  soc_mem_field32_set(unit, PPDB_A_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_WRf, 0x0);
  soc_mem_field32_set(unit, PPDB_A_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_RDf, 0x0);  
  soc_mem_field32_set(unit, PPDB_A_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_RESERVEDf, 0x0);*/

  SOC_REG_ABOVE_64_CLEAR(fld_val);
  sal_memset(tcam_in_data_value,0x0, sizeof(uint32)*(ARAD_PP_IHB_TCAM_DATA_WIDTH+1));



  /* if 80 bits COPY TO both  */
  if(nof_entries_in_line == 2) {
      SHR_BITCOPY_RANGE(tcam_in_data_value,84,compare_data->value,0,80);
      SHR_BITCOPY_RANGE(tcam_in_data_value,2,compare_data->value,0,80);
  }
  else{
      SHR_BITCOPY_RANGE(tcam_in_data_value,2,compare_data->value,0,80);
      SHR_BITCOPY_RANGE(tcam_in_data_value,84,compare_data->value,80,80);
  }

  /* bits 0-1, and 82-83 for entry type */
  entry_mode = arad_pp_ihb_tcam_entries_in_line_to_format(nof_entries_in_line);


  SHR_BITCOPY_RANGE(tcam_in_data_value,0,&entry_mode,0,2);
  SHR_BITCOPY_RANGE(tcam_in_data_value,82,&entry_mode,0,2);


  sal_memcpy(fld_val, tcam_in_data_value, (ARAD_PP_IHB_TCAM_DATA_WIDTH+1)*sizeof(uint32));

  soc_mem_field_set(unit, PPDB_A_TCAM_BANK_COMMANDm, mem_val, TCAM_CPU_CMD_DIf, fld_val);
  res = arad_pp_ihb_tcam_tbl_bank_offset_get(unit, bank_ndx, 0/* hw_offset */, &bank_offset);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, WRITE_PPDB_A_TCAM_BANK_COMMANDm(unit, 
                                                                   MEM_BLOCK_ANY, 
                                                                   bank_offset, 
                                                                   mem_val));

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, READ_PPDB_A_TCAM_BANK_COMMANDm(unit, 
                                                                  MEM_BLOCK_ANY, 
                                                                  bank_offset,
                                                                  mem_val));
  found_data->found = soc_mem_field32_get(unit, PPDB_A_TCAM_BANK_REPLYm, mem_val, TCAM_CPU_REP_MATCHf);
  if(found_data->found) {
    found_data->address = soc_mem_field32_get(unit, PPDB_A_TCAM_BANK_REPLYm, mem_val, TCAM_CPU_REP_AINDEXf);
    if(nof_entries_in_line == 1) {
        found_data->address /= 2;
    }
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_tcam_tbl_compare_unsafe()",0,0);
}

/*
 * move command by the CPU of the TCAM bank entries.
 * doesn't take semaphore.
 */
uint32
  arad_pp_ihb_tcam_tbl_move_unsafe(
    SOC_SAND_IN   int                           unit,
    SOC_SAND_IN   uint32                           bank_ndx,
    SOC_SAND_IN   uint32                           move_couple,
    SOC_SAND_IN   uint32                           nof_entries_in_line,
    SOC_SAND_IN   uint32                           source_entry_start_ndx,
    SOC_SAND_IN   uint32                           source_entry_end_ndx,
    SOC_SAND_IN   uint32                           dest_entry_start_ndx
  )
{
  uint32
    hw_offset_source,
    hw_offset_dest,
    range,
    res;
  uint64
    tcam_manager_1;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_TCAM_TBL_MOVE_UNSAFE);

 LOG_VERBOSE(BSL_LS_SOC_MEM,
             (BSL_META_U(unit,
                         SOC_DPP_MSG("      "
                         "TCAM-Block-move: bank:%d, entry-source-start:%d, entry-source-end:%d, "
                         "entry-dest-start:%d, is-320b: %d, nof_entries_in_line: %d\n\r")), 
              bank_ndx, source_entry_start_ndx, source_entry_end_ndx,
              dest_entry_start_ndx, move_couple, nof_entries_in_line));

 /*
  * in ARAD the only use of the TACM for 164 bits word (one word per line) 82 bits words (2 words per line)
  */
  if (nof_entries_in_line == 0 || nof_entries_in_line > 2)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT_ERR, 10, exit);
  }
 /*
  * in case there is two words per line which word to use.
  */
  if (source_entry_start_ndx >= nof_entries_in_line * ARAD_TCAM_NOF_LINES_PER_BANK(unit, bank_ndx))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT_ERR, 20, exit);
  }
  if (dest_entry_start_ndx >= nof_entries_in_line * ARAD_TCAM_NOF_LINES_PER_BANK(unit, bank_ndx))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_TBL_RANGE_OUT_OF_LIMIT_ERR, 20, exit);
  }

 /*
  * low word or high word
  */
  if (nof_entries_in_line == 1)
  {
    hw_offset_source = source_entry_start_ndx;
    hw_offset_dest = dest_entry_start_ndx;
    range = source_entry_end_ndx - source_entry_start_ndx + 1;
  }
  else
  {
    hw_offset_source = source_entry_start_ndx >> 1;
    hw_offset_dest = dest_entry_start_ndx >> 1;
    range = (source_entry_end_ndx - source_entry_start_ndx + 2) / 2;
  }

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, READ_PPDB_A_TCAM_MANAGER_1r(unit, &tcam_manager_1));
  soc_reg64_field32_set(unit, PPDB_A_TCAM_MANAGER_1r, &tcam_manager_1, TCAM_MANAGER_SRCf, hw_offset_source);
  soc_reg64_field32_set(unit, PPDB_A_TCAM_MANAGER_1r, &tcam_manager_1, TCAM_MANAGER_DSTf, hw_offset_dest);
  soc_reg64_field32_set(unit, PPDB_A_TCAM_MANAGER_1r, &tcam_manager_1, TCAM_MANAGER_RANGEf, range);
  soc_reg64_field32_set(unit, PPDB_A_TCAM_MANAGER_1r, &tcam_manager_1, TCAM_MANAGER_BANKf, bank_ndx);
  soc_reg64_field32_set(unit, PPDB_A_TCAM_MANAGER_1r, &tcam_manager_1, TCAM_MANAGER_MOVE_COUPLEf, move_couple);
  soc_reg64_field32_set(unit, PPDB_A_TCAM_MANAGER_1r, &tcam_manager_1, TCAM_MANAGER_INVALIDATEf, 0x0);
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 40, exit, WRITE_PPDB_A_TCAM_MANAGER_1r(unit, tcam_manager_1));

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 50, exit, WRITE_PPDB_A_TCAM_MANAGER_0r(unit, 0x1));

  res = arad_polling(
          unit,
          ARAD_TIMEOUT,
          ARAD_MIN_POLLS,
          PPDB_A_TCAM_MANAGER_0r,
          REG_PORT_ANY,
          0,
          TCAM_MANAGER_TRIGGERf,
          0
        ); 
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_tcam_tbl_move_unsafe()",0,0);
}

/* automatically generated code */
#define ARAD_PP_IHP_PINFO_LLR_TBL_ENTRY_SIZE 3

uint32
  arad_pp_ihp_pinfo_llr_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_PINFO_LLR_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_PINFO_LLR_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_PINFO_LLR_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_PINFO_LLR_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_PINFO_LLR_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHP_PINFO_LLRm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->vlan_classification_profile = soc_mem_field32_get(unit, IHP_PINFO_LLRm, data, VLAN_CLASSIFICATION_PROFILEf);
  tbl_data->pcp_profile = soc_mem_field32_get(unit, IHP_PINFO_LLRm, data, PCP_PROFILEf);
  tbl_data->ignore_incoming_pcp = soc_mem_field32_get(unit, IHP_PINFO_LLRm, data, IGNORE_INCOMING_PCPf);
  tbl_data->tc_dp_tos_enable = soc_mem_field32_get(unit, IHP_PINFO_LLRm, data, TC_DP_TOS_ENABLEf);
  tbl_data->tc_dp_tos_index = soc_mem_field32_get(unit, IHP_PINFO_LLRm, data, TC_DP_TOS_INDEXf);
  tbl_data->tc_l_4_protocol_enable = soc_mem_field32_get(unit, IHP_PINFO_LLRm, data, TC_L_4_PROTOCOL_ENABLEf);
  tbl_data->tc_subnet_enable = soc_mem_field32_get(unit, IHP_PINFO_LLRm, data, TC_SUBNET_ENABLEf);
  tbl_data->tc_port_protocol_enable = soc_mem_field32_get(unit, IHP_PINFO_LLRm, data, TC_PORT_PROTOCOL_ENABLEf);
  tbl_data->default_traffic_class = soc_mem_field32_get(unit, IHP_PINFO_LLRm, data, DEFAULT_TRAFFIC_CLASSf);
  tbl_data->ignore_incoming_up = soc_mem_field32_get(unit, IHP_PINFO_LLRm, data, IGNORE_INCOMING_UPf);
  tbl_data->incoming_up_map_profile = soc_mem_field32_get(unit, IHP_PINFO_LLRm, data, INCOMING_UP_MAP_PROFILEf);
  tbl_data->use_dei = soc_mem_field32_get(unit, IHP_PINFO_LLRm, data, USE_DEIf);
  tbl_data->drop_precedence_profile = soc_mem_field32_get(unit, IHP_PINFO_LLRm, data, DROP_PRECEDENCE_PROFILEf);
  tbl_data->sa_lookup_enable = soc_mem_field32_get(unit, IHP_PINFO_LLRm, data, SA_LOOKUP_ENABLEf);
  tbl_data->action_profile_sa_drop_index = soc_mem_field32_get(unit, IHP_PINFO_LLRm, data, ACTION_PROFILE_SA_DROP_INDEXf);
  tbl_data->enable_sa_authentication = soc_mem_field32_get(unit, IHP_PINFO_LLRm, data, ENABLE_SA_AUTHENTICATIONf);
  tbl_data->ignore_incoming_vid = soc_mem_field32_get(unit, IHP_PINFO_LLRm, data, IGNORE_INCOMING_VIDf);
  tbl_data->vid_ipv4_subnet_enable = soc_mem_field32_get(unit, IHP_PINFO_LLRm, data, VID_IPV4_SUBNET_ENABLEf);
  tbl_data->vid_port_protocol_enable = soc_mem_field32_get(unit, IHP_PINFO_LLRm, data, VID_PORT_PROTOCOL_ENABLEf);
  tbl_data->protocol_profile = soc_mem_field32_get(unit, IHP_PINFO_LLRm, data, PROTOCOL_PROFILEf);
  tbl_data->default_initial_vid = soc_mem_field32_get(unit, IHP_PINFO_LLRm, data, DEFAULT_INITIAL_VIDf);
  tbl_data->non_authorized_mode_8021x = soc_mem_field32_get(unit, IHP_PINFO_LLRm, data, NON_AUTHORIZED_MODE_8021Xf);
  tbl_data->reserved_mc_trap_profile = soc_mem_field32_get(unit, IHP_PINFO_LLRm, data, RESERVED_MC_TRAP_PROFILEf);
  tbl_data->enable_arp_trap = soc_mem_field32_get(unit, IHP_PINFO_LLRm, data, ENABLE_ARP_TRAPf);
  tbl_data->enable_igmp_trap = soc_mem_field32_get(unit, IHP_PINFO_LLRm, data, ENABLE_IGMP_TRAPf);
  tbl_data->enable_mld_trap = soc_mem_field32_get(unit, IHP_PINFO_LLRm, data, ENABLE_MLD_TRAPf);
  tbl_data->enable_dhcp_trap = soc_mem_field32_get(unit, IHP_PINFO_LLRm, data, ENABLE_DHCP_TRAPf);
  tbl_data->general_trap_enable = soc_mem_field32_get(unit, IHP_PINFO_LLRm, data, GENERAL_TRAP_ENABLEf);
  tbl_data->default_cpu_trap_code = soc_mem_field32_get(unit, IHP_PINFO_LLRm, data, DEFAULT_CPU_TRAP_CODEf);
  tbl_data->default_action_profile_fwd = soc_mem_field32_get(unit, IHP_PINFO_LLRm, data, DEFAULT_ACTION_PROFILE_FWDf);
  tbl_data->default_action_profile_snp = soc_mem_field32_get(unit, IHP_PINFO_LLRm, data, DEFAULT_ACTION_PROFILE_SNPf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_pinfo_llr_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_pinfo_llr_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_PINFO_LLR_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_PINFO_LLR_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_PINFO_LLR_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_PINFO_LLR_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHP_PINFO_LLRm, data, VLAN_CLASSIFICATION_PROFILEf, tbl_data->vlan_classification_profile);
  soc_mem_field32_set(unit, IHP_PINFO_LLRm, data, PCP_PROFILEf, tbl_data->pcp_profile);
  soc_mem_field32_set(unit, IHP_PINFO_LLRm, data, IGNORE_INCOMING_PCPf, tbl_data->ignore_incoming_pcp);
  soc_mem_field32_set(unit, IHP_PINFO_LLRm, data, TC_DP_TOS_ENABLEf, tbl_data->tc_dp_tos_enable);
  soc_mem_field32_set(unit, IHP_PINFO_LLRm, data, TC_DP_TOS_INDEXf, tbl_data->tc_dp_tos_index);
  soc_mem_field32_set(unit, IHP_PINFO_LLRm, data, TC_L_4_PROTOCOL_ENABLEf, tbl_data->tc_l_4_protocol_enable);
  soc_mem_field32_set(unit, IHP_PINFO_LLRm, data, TC_SUBNET_ENABLEf, tbl_data->tc_subnet_enable);
  soc_mem_field32_set(unit, IHP_PINFO_LLRm, data, TC_PORT_PROTOCOL_ENABLEf, tbl_data->tc_port_protocol_enable);
  soc_mem_field32_set(unit, IHP_PINFO_LLRm, data, DEFAULT_TRAFFIC_CLASSf, tbl_data->default_traffic_class);
  soc_mem_field32_set(unit, IHP_PINFO_LLRm, data, IGNORE_INCOMING_UPf, tbl_data->ignore_incoming_up);
  soc_mem_field32_set(unit, IHP_PINFO_LLRm, data, INCOMING_UP_MAP_PROFILEf, tbl_data->incoming_up_map_profile);
  soc_mem_field32_set(unit, IHP_PINFO_LLRm, data, USE_DEIf, tbl_data->use_dei);
  soc_mem_field32_set(unit, IHP_PINFO_LLRm, data, DROP_PRECEDENCE_PROFILEf, tbl_data->drop_precedence_profile);
  soc_mem_field32_set(unit, IHP_PINFO_LLRm, data, SA_LOOKUP_ENABLEf, tbl_data->sa_lookup_enable);
  soc_mem_field32_set(unit, IHP_PINFO_LLRm, data, ACTION_PROFILE_SA_DROP_INDEXf, tbl_data->action_profile_sa_drop_index);
  soc_mem_field32_set(unit, IHP_PINFO_LLRm, data, ENABLE_SA_AUTHENTICATIONf, tbl_data->enable_sa_authentication);
  soc_mem_field32_set(unit, IHP_PINFO_LLRm, data, IGNORE_INCOMING_VIDf, tbl_data->ignore_incoming_vid);
  soc_mem_field32_set(unit, IHP_PINFO_LLRm, data, VID_IPV4_SUBNET_ENABLEf, tbl_data->vid_ipv4_subnet_enable);
  soc_mem_field32_set(unit, IHP_PINFO_LLRm, data, VID_PORT_PROTOCOL_ENABLEf, tbl_data->vid_port_protocol_enable);
  soc_mem_field32_set(unit, IHP_PINFO_LLRm, data, PROTOCOL_PROFILEf, tbl_data->protocol_profile);
  soc_mem_field32_set(unit, IHP_PINFO_LLRm, data, DEFAULT_INITIAL_VIDf, tbl_data->default_initial_vid);
  soc_mem_field32_set(unit, IHP_PINFO_LLRm, data, NON_AUTHORIZED_MODE_8021Xf, tbl_data->non_authorized_mode_8021x);
  soc_mem_field32_set(unit, IHP_PINFO_LLRm, data, RESERVED_MC_TRAP_PROFILEf, tbl_data->reserved_mc_trap_profile);
  soc_mem_field32_set(unit, IHP_PINFO_LLRm, data, ENABLE_ARP_TRAPf, tbl_data->enable_arp_trap);
  soc_mem_field32_set(unit, IHP_PINFO_LLRm, data, ENABLE_IGMP_TRAPf, tbl_data->enable_igmp_trap);
  soc_mem_field32_set(unit, IHP_PINFO_LLRm, data, ENABLE_MLD_TRAPf, tbl_data->enable_mld_trap);
  soc_mem_field32_set(unit, IHP_PINFO_LLRm, data, ENABLE_DHCP_TRAPf, tbl_data->enable_dhcp_trap);
  soc_mem_field32_set(unit, IHP_PINFO_LLRm, data, GENERAL_TRAP_ENABLEf, tbl_data->general_trap_enable);
  soc_mem_field32_set(unit, IHP_PINFO_LLRm, data, DEFAULT_CPU_TRAP_CODEf, tbl_data->default_cpu_trap_code);
  soc_mem_field32_set(unit, IHP_PINFO_LLRm, data, DEFAULT_ACTION_PROFILE_FWDf, tbl_data->default_action_profile_fwd);
  soc_mem_field32_set(unit, IHP_PINFO_LLRm, data, DEFAULT_ACTION_PROFILE_SNPf, tbl_data->default_action_profile_snp);

  res = soc_mem_write(
          unit,
          IHP_PINFO_LLRm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_pinfo_llr_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHP_LLR_LLVP_TBL_ENTRY_SIZE 1

uint32
  arad_pp_ihp_llr_llvp_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_LLR_LLVP_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_LLR_LLVP_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_LLR_LLVP_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_LLR_LLVP_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_LLR_LLVP_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHP_LLR_LLVPm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->incoming_vid_exist = soc_mem_field32_get(unit, IHP_LLR_LLVPm, data, INCOMING_VID_EXISTf);
  tbl_data->incoming_tag_exist = soc_mem_field32_get(unit, IHP_LLR_LLVPm, data, INCOMING_TAG_EXISTf);
  tbl_data->incoming_tag_index = soc_mem_field32_get(unit, IHP_LLR_LLVPm, data, INCOMING_TAG_INDEXf);
  tbl_data->incoming_s_tag_exist = soc_mem_field32_get(unit, IHP_LLR_LLVPm, data, INCOMING_S_TAG_EXISTf);
  tbl_data->acceptable_frame_type_action = soc_mem_field32_get(unit, IHP_LLR_LLVPm, data, ACCEPTABLE_FRAME_TYPE_ACTIONf);
  tbl_data->outer_comp_index = soc_mem_field32_get(unit, IHP_LLR_LLVPm, data, OUTER_COMP_INDEXf);
  tbl_data->inner_comp_index = soc_mem_field32_get(unit, IHP_LLR_LLVPm, data, INNER_COMP_INDEXf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_llr_llvp_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_llr_llvp_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_LLR_LLVP_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_LLR_LLVP_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_LLR_LLVP_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_LLR_LLVP_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHP_LLR_LLVPm, data, INCOMING_VID_EXISTf, tbl_data->incoming_vid_exist);
  soc_mem_field32_set(unit, IHP_LLR_LLVPm, data, INCOMING_TAG_EXISTf, tbl_data->incoming_tag_exist);
  soc_mem_field32_set(unit, IHP_LLR_LLVPm, data, INCOMING_TAG_INDEXf, tbl_data->incoming_tag_index);
  soc_mem_field32_set(unit, IHP_LLR_LLVPm, data, INCOMING_S_TAG_EXISTf, tbl_data->incoming_s_tag_exist);
  soc_mem_field32_set(unit, IHP_LLR_LLVPm, data, ACCEPTABLE_FRAME_TYPE_ACTIONf, tbl_data->acceptable_frame_type_action);
  soc_mem_field32_set(unit, IHP_LLR_LLVPm, data, OUTER_COMP_INDEXf, tbl_data->outer_comp_index);
  soc_mem_field32_set(unit, IHP_LLR_LLVPm, data, INNER_COMP_INDEXf, tbl_data->inner_comp_index);

  res = soc_mem_write(
          unit,
          IHP_LLR_LLVPm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_llr_llvp_tbl_set_unsafe()", entry_offset, 0);
}


uint32
  arad_pp_ihp_llr_llvp_tbl_fill_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_LLR_LLVP_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_LLR_LLVP_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_LLR_LLVP_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_LLR_LLVP_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHP_LLR_LLVPm, data, INCOMING_VID_EXISTf, tbl_data->incoming_vid_exist);
  soc_mem_field32_set(unit, IHP_LLR_LLVPm, data, INCOMING_TAG_EXISTf, tbl_data->incoming_tag_exist);
  soc_mem_field32_set(unit, IHP_LLR_LLVPm, data, INCOMING_TAG_INDEXf, tbl_data->incoming_tag_index);
  soc_mem_field32_set(unit, IHP_LLR_LLVPm, data, INCOMING_S_TAG_EXISTf, tbl_data->incoming_s_tag_exist);
  soc_mem_field32_set(unit, IHP_LLR_LLVPm, data, ACCEPTABLE_FRAME_TYPE_ACTIONf, tbl_data->acceptable_frame_type_action);
  soc_mem_field32_set(unit, IHP_LLR_LLVPm, data, OUTER_COMP_INDEXf, tbl_data->outer_comp_index);
  soc_mem_field32_set(unit, IHP_LLR_LLVPm, data, INNER_COMP_INDEXf, tbl_data->inner_comp_index);

  res = arad_fill_table_with_entry(unit, IHP_LLR_LLVPm, MEM_BLOCK_ANY, data); /* fill memory with the entry in data */
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_llr_llvp_tbl_fill_unsafe()", entry_offset, 0);
}


#define ARAD_PP_IHP_ECM_STEP_TBL_ENTRY_SIZE 1

uint32
  arad_tbl_access_em_step_set(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   tbl_id,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_ECM_STEP_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_ECM_STEP_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_LLR_LLVP_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_ECM_STEP_TBL_ENTRY_SIZE);

  /* format data to be written */
  soc_mem_field32_set(unit, EDB_ESEM_STEP_TABLEm, data, ESEM_MM_READ_LINEf, tbl_data->read_line);
  soc_mem_field32_set(unit, EDB_ESEM_STEP_TABLEm, data, ESEM_MM_WRITE_LINEf, tbl_data->write_line);
  soc_mem_field32_set(unit, EDB_ESEM_STEP_TABLEm, data, ESEM_MM_WRITE_VALIDf, tbl_data->write_valid);
  soc_mem_field32_set(unit, EDB_ESEM_STEP_TABLEm, data, ESEM_SOURCE_STEPf, tbl_data->step);
  soc_mem_field32_set(unit, EDB_ESEM_STEP_TABLEm, data, ESEM_TABLE_OFFSETf, tbl_data->offset);

  /* write to all ste data same information */
  res = WRITE_EDB_ESEM_STEP_TABLEm(unit,MEM_BLOCK_ALL, entry_offset, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
  res = WRITE_PPDB_A_OEMA_STEP_TABLEm(unit,MEM_BLOCK_ALL, entry_offset, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
  res = WRITE_PPDB_A_OEMB_STEP_TABLEm(unit,MEM_BLOCK_ALL, entry_offset, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
  if (SOC_IS_JERICHO(unit)) {
      res = WRITE_IHB_ISEM_STEP_TABLEm(unit,MEM_BLOCK_ALL, entry_offset, data);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
  } else {
      res = WRITE_IHP_ISA_STEP_TABLEm(unit,MEM_BLOCK_ALL, entry_offset, data);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
      res = WRITE_IHP_ISB_STEP_TABLEm(unit,MEM_BLOCK_ALL, entry_offset, data);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 50, exit);
      res = WRITE_PPDB_B_LARGE_EM_STEP_TABLEm(unit,MEM_BLOCK_ALL, entry_offset, data);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 60, exit);

  }
  res = WRITE_OAMP_REMOTE_MEP_EXACT_MATCH_STEP_TABLEm(unit,MEM_BLOCK_ALL, entry_offset, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 70, exit);

  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_emc_step_tbl_set_unsafe()", entry_offset, 0);
}

/* Jericho Only*/
uint32
  arad_tbl_access_glem_step_set(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   tbl_id,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_ECM_STEP_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_ECM_STEP_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_LLR_LLVP_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_ECM_STEP_TBL_ENTRY_SIZE);

  /* format data to be written */
  soc_mem_field32_set(unit, EDB_GLEM_STEP_TABLEm, data, GLEM_MM_READ_LINEf, tbl_data->read_line);
  soc_mem_field32_set(unit, EDB_GLEM_STEP_TABLEm, data, GLEM_MM_WRITE_LINEf, tbl_data->write_line);
  soc_mem_field32_set(unit, EDB_GLEM_STEP_TABLEm, data, GLEM_MM_WRITE_VALIDf, tbl_data->write_valid);
  soc_mem_field32_set(unit, EDB_GLEM_STEP_TABLEm, data, GLEM_SOURCE_STEPf, tbl_data->step);
  soc_mem_field32_set(unit, EDB_GLEM_STEP_TABLEm, data, GLEM_TABLE_OFFSETf, tbl_data->offset);

  res = WRITE_EDB_GLEM_STEP_TABLEm(unit,MEM_BLOCK_ALL, entry_offset, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  res = WRITE_PPDB_B_LARGE_EM_STEP_TABLEm(unit,MEM_BLOCK_ALL, entry_offset, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 60, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_emc_step_tbl_set_unsafe()", entry_offset, 0);
}


#define ARAD_PP_IHP_LL_MIRROR_PROFILE_TBL_ENTRY_SIZE 1

uint32
  arad_pp_ihp_ll_mirror_profile_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_LL_MIRROR_PROFILE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_LL_MIRROR_PROFILE_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_LL_MIRROR_PROFILE_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_LL_MIRROR_PROFILE_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_LL_MIRROR_PROFILE_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHP_LL_MIRROR_PROFILEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->ll_mirror_profile = soc_mem_field32_get(unit, IHP_LL_MIRROR_PROFILEm, data, LL_MIRROR_PROFILEf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_ll_mirror_profile_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_ll_mirror_profile_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_LL_MIRROR_PROFILE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_LL_MIRROR_PROFILE_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_LL_MIRROR_PROFILE_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_LL_MIRROR_PROFILE_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHP_LL_MIRROR_PROFILEm, data, LL_MIRROR_PROFILEf, tbl_data->ll_mirror_profile);

  res = soc_mem_write(
          unit,
          IHP_LL_MIRROR_PROFILEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_ll_mirror_profile_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHP_SUBNET_CLASSIFY_TBL_ENTRY_SIZE 2

uint32
  arad_pp_ihp_subnet_classify_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_SUBNET_CLASSIFY_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_SUBNET_CLASSIFY_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_SUBNET_CLASSIFY_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_SUBNET_CLASSIFY_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_SUBNET_CLASSIFY_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHP_SUBNET_CLASSIFYm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->ipv4_subnet = soc_mem_field32_get(unit, IHP_SUBNET_CLASSIFYm, data, IPV4_SUBNETf);
  tbl_data->ipv4_subnet_mask = soc_mem_field32_get(unit, IHP_SUBNET_CLASSIFYm, data, IPV4_SUBNET_MASKf);
  tbl_data->ipv4_subnet_vid = soc_mem_field32_get(unit, IHP_SUBNET_CLASSIFYm, data, IPV4_SUBNET_VIDf);
  tbl_data->ipv4_subnet_tc = soc_mem_field32_get(unit, IHP_SUBNET_CLASSIFYm, data, IPV4_SUBNET_TCf);
  tbl_data->ipv4_subnet_tc_valid = soc_mem_field32_get(unit, IHP_SUBNET_CLASSIFYm, data, IPV4_SUBNET_TC_VALIDf);
  tbl_data->ipv4_subnet_valid = soc_mem_field32_get(unit, IHP_SUBNET_CLASSIFYm, data, IPV4_SUBNET_VALIDf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_subnet_classify_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_subnet_classify_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_SUBNET_CLASSIFY_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_SUBNET_CLASSIFY_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_SUBNET_CLASSIFY_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_SUBNET_CLASSIFY_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHP_SUBNET_CLASSIFYm, data, IPV4_SUBNETf, tbl_data->ipv4_subnet);
  soc_mem_field32_set(unit, IHP_SUBNET_CLASSIFYm, data, IPV4_SUBNET_MASKf, tbl_data->ipv4_subnet_mask);
  soc_mem_field32_set(unit, IHP_SUBNET_CLASSIFYm, data, IPV4_SUBNET_VIDf, tbl_data->ipv4_subnet_vid);
  soc_mem_field32_set(unit, IHP_SUBNET_CLASSIFYm, data, IPV4_SUBNET_TCf, tbl_data->ipv4_subnet_tc);
  soc_mem_field32_set(unit, IHP_SUBNET_CLASSIFYm, data, IPV4_SUBNET_TC_VALIDf, tbl_data->ipv4_subnet_tc_valid);
  soc_mem_field32_set(unit, IHP_SUBNET_CLASSIFYm, data, IPV4_SUBNET_VALIDf, tbl_data->ipv4_subnet_valid);

  res = soc_mem_write(
          unit,
          IHP_SUBNET_CLASSIFYm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_subnet_classify_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHP_PORT_PROTOCOL_TBL_ENTRY_SIZE 1

uint32
  arad_pp_ihp_port_protocol_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_PORT_PROTOCOL_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_PORT_PROTOCOL_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_PORT_PROTOCOL_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_PORT_PROTOCOL_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_PORT_PROTOCOL_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHP_PORT_PROTOCOLm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->vid = soc_mem_field32_get(unit, IHP_PORT_PROTOCOLm, data, VIDf);
  tbl_data->vid_valid = soc_mem_field32_get(unit, IHP_PORT_PROTOCOLm, data, VID_VALIDf);
  tbl_data->traffic_class = soc_mem_field32_get(unit, IHP_PORT_PROTOCOLm, data, TRAFFIC_CLASSf);
  tbl_data->traffic_class_valid = soc_mem_field32_get(unit, IHP_PORT_PROTOCOLm, data, TRAFFIC_CLASS_VALIDf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_port_protocol_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_port_protocol_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_PORT_PROTOCOL_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_PORT_PROTOCOL_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_PORT_PROTOCOL_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_PORT_PROTOCOL_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHP_PORT_PROTOCOLm, data, VIDf, tbl_data->vid);
  soc_mem_field32_set(unit, IHP_PORT_PROTOCOLm, data, VID_VALIDf, tbl_data->vid_valid);
  soc_mem_field32_set(unit, IHP_PORT_PROTOCOLm, data, TRAFFIC_CLASSf, tbl_data->traffic_class);
  soc_mem_field32_set(unit, IHP_PORT_PROTOCOLm, data, TRAFFIC_CLASS_VALIDf, tbl_data->traffic_class_valid);

  res = soc_mem_write(
          unit,
          IHP_PORT_PROTOCOLm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_port_protocol_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHP_TOS_2_COS_TBL_ENTRY_SIZE 1

uint32
  arad_pp_ihp_tos_2_cos_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_TOS_2_COS_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_TOS_2_COS_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_TOS_2_COS_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_TOS_2_COS_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_TOS_2_COS_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHP_TOS_2_COSm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->valid = soc_mem_field32_get(unit, IHP_TOS_2_COSm, data, VALIDf);
  tbl_data->tc = soc_mem_field32_get(unit, IHP_TOS_2_COSm, data, TCf);
  tbl_data->dp = soc_mem_field32_get(unit, IHP_TOS_2_COSm, data, DPf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_tos_2_cos_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_tos_2_cos_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_TOS_2_COS_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_TOS_2_COS_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_TOS_2_COS_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_TOS_2_COS_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHP_TOS_2_COSm, data, VALIDf, tbl_data->valid);
  soc_mem_field32_set(unit, IHP_TOS_2_COSm, data, TCf, tbl_data->tc);
  soc_mem_field32_set(unit, IHP_TOS_2_COSm, data, DPf, tbl_data->dp);

  res = soc_mem_write(
          unit,
          IHP_TOS_2_COSm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_tos_2_cos_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHP_RESERVED_MC_TBL_ENTRY_SIZE 1

uint32
  arad_pp_ihp_reserved_mc_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_RESERVED_MC_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_RESERVED_MC_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_RESERVED_MC_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_RESERVED_MC_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_RESERVED_MC_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHP_RESERVED_MCm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->fwd = soc_mem_field32_get(unit, IHP_RESERVED_MCm, data, FWDf);
  tbl_data->snp = soc_mem_field32_get(unit, IHP_RESERVED_MCm, data, SNPf);
  tbl_data->cpu_trap_code = soc_mem_field32_get(unit, IHP_RESERVED_MCm, data, CPU_TRAP_CODEf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_reserved_mc_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_reserved_mc_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_RESERVED_MC_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_RESERVED_MC_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_RESERVED_MC_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_RESERVED_MC_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHP_RESERVED_MCm, data, FWDf, tbl_data->fwd);
  soc_mem_field32_set(unit, IHP_RESERVED_MCm, data, SNPf, tbl_data->snp);
  soc_mem_field32_set(unit, IHP_RESERVED_MCm, data, CPU_TRAP_CODEf, tbl_data->cpu_trap_code);

  res = soc_mem_write(
          unit,
          IHP_RESERVED_MCm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_reserved_mc_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHP_PP_PORT_INFO_TBL_ENTRY_SIZE 1

uint32
  arad_pp_ihp_pp_port_info_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_PP_PORT_INFO_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_PP_PORT_INFO_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_PP_PORT_INFO_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_PP_PORT_INFO_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_PP_PORT_INFO_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHP_PP_PORT_INFOm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->pp_port_outer_header_start = soc_mem_field32_get(unit, IHP_PP_PORT_INFOm, data, PP_PORT_OUTER_HEADER_STARTf);
  tbl_data->tpid_profile = soc_mem_field32_get(unit, IHP_PP_PORT_INFOm, data, TPID_PROFILEf);
  tbl_data->default_parser_program_pointer = soc_mem_field32_get(unit, IHP_PP_PORT_INFOm, data, DEFAULT_PARSER_PROGRAM_POINTERf);
  tbl_data->default_pfq_0 = soc_mem_field32_get(unit, IHP_PP_PORT_INFOm, data, DEFAULT_PFQ_0f);
  tbl_data->parser_custom_macro_select = soc_mem_field32_get(unit, IHP_PP_PORT_INFOm, data, PARSER_CUSTOM_MACRO_SELECTf);
  tbl_data->mpls_next_address_use_specualte = soc_mem_field32_get(unit, IHP_PP_PORT_INFOm, data, MPLS_NEXT_ADDRESS_USE_SPECUALTEf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_pp_port_info_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_pp_port_info_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_PP_PORT_INFO_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_PP_PORT_INFO_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_PP_PORT_INFO_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_PP_PORT_INFO_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHP_PP_PORT_INFOm, data, PP_PORT_OUTER_HEADER_STARTf, tbl_data->pp_port_outer_header_start);
  soc_mem_field32_set(unit, IHP_PP_PORT_INFOm, data, TPID_PROFILEf, tbl_data->tpid_profile);
  soc_mem_field32_set(unit, IHP_PP_PORT_INFOm, data, DEFAULT_PARSER_PROGRAM_POINTERf, tbl_data->default_parser_program_pointer);
  soc_mem_field32_set(unit, IHP_PP_PORT_INFOm, data, DEFAULT_PFQ_0f, tbl_data->default_pfq_0);
  soc_mem_field32_set(unit, IHP_PP_PORT_INFOm, data, PARSER_CUSTOM_MACRO_SELECTf, tbl_data->parser_custom_macro_select);
  soc_mem_field32_set(unit, IHP_PP_PORT_INFOm, data, MPLS_NEXT_ADDRESS_USE_SPECUALTEf, tbl_data->mpls_next_address_use_specualte);

  res = soc_mem_write(
          unit,
          IHP_PP_PORT_INFOm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_pp_port_info_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_ENTRY_SIZE 1

uint32
  arad_pp_ihp_src_system_port_fem_bit_select_table_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->src_system_port_fem_bit_select = soc_mem_field32_get(unit, IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLEm, data, SRC_SYSTEM_PORT_FEM_BIT_SELECTf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_src_system_port_fem_bit_select_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_src_system_port_fem_bit_select_table_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLEm, data, SRC_SYSTEM_PORT_FEM_BIT_SELECTf, tbl_data->src_system_port_fem_bit_select);

  res = soc_mem_write(
          unit,
          IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_src_system_port_fem_bit_select_table_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_ENTRY_SIZE 1

uint32
  arad_pp_ihp_src_system_port_fem_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32              pgm_ndx,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLEm,
          MEM_BLOCK_ANY,
          entry_offset + (pgm_ndx << ARAD_PP_IHP_FEM_SEL_BITS_SIZE_IN_BITS),
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->src_system_port_fem_map_index = soc_mem_field32_get(unit, IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLEm, data, SRC_SYSTEM_PORT_FEM_MAP_INDEXf);
  tbl_data->src_system_port_fem_map_data = soc_mem_field32_get(unit, IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLEm, data, SRC_SYSTEM_PORT_FEM_MAP_DATAf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_src_system_port_fem_map_index_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_src_system_port_fem_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32              pgm_ndx,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLEm, data, SRC_SYSTEM_PORT_FEM_MAP_INDEXf, tbl_data->src_system_port_fem_map_index);
  soc_mem_field32_set(unit, IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLEm, data, SRC_SYSTEM_PORT_FEM_MAP_DATAf, tbl_data->src_system_port_fem_map_data);

  res = soc_mem_write(
          unit,
          IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLEm,
          MEM_BLOCK_ANY,
          entry_offset + (pgm_ndx << ARAD_PP_IHP_FEM_SEL_BITS_SIZE_IN_BITS),
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_src_system_port_fem_map_index_table_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHP_PACKET_FORMAT_TABLE_TBL_ENTRY_SIZE 1

uint32
  arad_pp_ihp_packet_format_table_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_PACKET_FORMAT_TABLE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_PACKET_FORMAT_TABLE_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_PACKET_FORMAT_TABLE_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_PACKET_FORMAT_TABLE_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_PACKET_FORMAT_TABLE_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHP_PACKET_FORMAT_TABLEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->packet_format_code = soc_mem_field32_get(unit, IHP_PACKET_FORMAT_TABLEm, data, PACKET_FORMAT_CODEf);
  tbl_data->parser_leaf_context = soc_mem_field32_get(unit, IHP_PACKET_FORMAT_TABLEm, data, PARSER_LEAF_CONTEXTf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_packet_format_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_packet_format_table_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_PACKET_FORMAT_TABLE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_PACKET_FORMAT_TABLE_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_PACKET_FORMAT_TABLE_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_PACKET_FORMAT_TABLE_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHP_PACKET_FORMAT_TABLEm, data, PACKET_FORMAT_CODEf, tbl_data->packet_format_code);
  soc_mem_field32_set(unit, IHP_PACKET_FORMAT_TABLEm, data, PARSER_LEAF_CONTEXTf, tbl_data->parser_leaf_context);

  res = soc_mem_write(
          unit,
          IHP_PACKET_FORMAT_TABLEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_packet_format_table_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_ENTRY_SIZE 4

uint32
  arad_pp_ihp_parser_custom_macro_parameters_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHP_PARSER_CUSTOM_MACRO_PARAMETERSm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->cstm_word_select = soc_mem_field32_get(unit, IHP_PARSER_CUSTOM_MACRO_PARAMETERSm, data, CSTM_WORD_SELECTf);
  tbl_data->cstm_mask_left = soc_mem_field32_get(unit, IHP_PARSER_CUSTOM_MACRO_PARAMETERSm, data, CSTM_MASK_LEFTf);
  tbl_data->cstm_mask_right = soc_mem_field32_get(unit, IHP_PARSER_CUSTOM_MACRO_PARAMETERSm, data, CSTM_MASK_RIGHTf);
  tbl_data->cstm_condition_select = soc_mem_field32_get(unit, IHP_PARSER_CUSTOM_MACRO_PARAMETERSm, data, CSTM_CONDITION_SELECTf);
  tbl_data->cstm_comparator_mode = soc_mem_field32_get(unit, IHP_PARSER_CUSTOM_MACRO_PARAMETERSm, data, CSTM_COMPARATOR_MODEf);
  tbl_data->cstm_comparator_mask = soc_mem_field32_get(unit, IHP_PARSER_CUSTOM_MACRO_PARAMETERSm, data, CSTM_COMPARATOR_MASKf);
  tbl_data->cstm_shift_sel = soc_mem_field32_get(unit, IHP_PARSER_CUSTOM_MACRO_PARAMETERSm, data, CSTM_SHIFT_SELf);
  tbl_data->cstm_shift_a = soc_mem_field32_get(unit, IHP_PARSER_CUSTOM_MACRO_PARAMETERSm, data, CSTM_SHIFT_Af);
  tbl_data->cstm_shift_b = soc_mem_field32_get(unit, IHP_PARSER_CUSTOM_MACRO_PARAMETERSm, data, CSTM_SHIFT_Bf);
  tbl_data->augment_previous_stage = soc_mem_field32_get(unit, IHP_PARSER_CUSTOM_MACRO_PARAMETERSm, data, AUGMENT_PREVIOUS_STAGEf);
  tbl_data->qualifier_or_mask = soc_mem_field32_get(unit, IHP_PARSER_CUSTOM_MACRO_PARAMETERSm, data, QUALIFIER_OR_MASKf);
  tbl_data->qualifier_and_mask = soc_mem_field32_get(unit, IHP_PARSER_CUSTOM_MACRO_PARAMETERSm, data, QUALIFIER_AND_MASKf);
  tbl_data->header_qualifier_mask = soc_mem_field32_get(unit, IHP_PARSER_CUSTOM_MACRO_PARAMETERSm, data, HEADER_QUALIFIER_MASKf);
  tbl_data->qualifier_shift = soc_mem_field32_get(unit, IHP_PARSER_CUSTOM_MACRO_PARAMETERSm, data, QUALIFIER_SHIFTf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_parser_custom_macro_parameters_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_parser_custom_macro_parameters_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHP_PARSER_CUSTOM_MACRO_PARAMETERSm, data, CSTM_WORD_SELECTf, tbl_data->cstm_word_select);
  soc_mem_field32_set(unit, IHP_PARSER_CUSTOM_MACRO_PARAMETERSm, data, CSTM_MASK_LEFTf, tbl_data->cstm_mask_left);
  soc_mem_field32_set(unit, IHP_PARSER_CUSTOM_MACRO_PARAMETERSm, data, CSTM_MASK_RIGHTf, tbl_data->cstm_mask_right);
  soc_mem_field32_set(unit, IHP_PARSER_CUSTOM_MACRO_PARAMETERSm, data, CSTM_CONDITION_SELECTf, tbl_data->cstm_condition_select);
  soc_mem_field32_set(unit, IHP_PARSER_CUSTOM_MACRO_PARAMETERSm, data, CSTM_COMPARATOR_MODEf, tbl_data->cstm_comparator_mode);
  soc_mem_field32_set(unit, IHP_PARSER_CUSTOM_MACRO_PARAMETERSm, data, CSTM_COMPARATOR_MASKf, tbl_data->cstm_comparator_mask);
  soc_mem_field32_set(unit, IHP_PARSER_CUSTOM_MACRO_PARAMETERSm, data, CSTM_SHIFT_SELf, tbl_data->cstm_shift_sel);
  soc_mem_field32_set(unit, IHP_PARSER_CUSTOM_MACRO_PARAMETERSm, data, CSTM_SHIFT_Af, tbl_data->cstm_shift_a);
  soc_mem_field32_set(unit, IHP_PARSER_CUSTOM_MACRO_PARAMETERSm, data, CSTM_SHIFT_Bf, tbl_data->cstm_shift_b);
  soc_mem_field32_set(unit, IHP_PARSER_CUSTOM_MACRO_PARAMETERSm, data, AUGMENT_PREVIOUS_STAGEf, tbl_data->augment_previous_stage);
  soc_mem_field32_set(unit, IHP_PARSER_CUSTOM_MACRO_PARAMETERSm, data, QUALIFIER_OR_MASKf, tbl_data->qualifier_or_mask);
  soc_mem_field32_set(unit, IHP_PARSER_CUSTOM_MACRO_PARAMETERSm, data, QUALIFIER_AND_MASKf, tbl_data->qualifier_and_mask);
  soc_mem_field32_set(unit, IHP_PARSER_CUSTOM_MACRO_PARAMETERSm, data, HEADER_QUALIFIER_MASKf, tbl_data->header_qualifier_mask);
  soc_mem_field32_set(unit, IHP_PARSER_CUSTOM_MACRO_PARAMETERSm, data, QUALIFIER_SHIFTf, tbl_data->qualifier_shift);

  res = soc_mem_write(
          unit,
          IHP_PARSER_CUSTOM_MACRO_PARAMETERSm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_parser_custom_macro_parameters_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_ENTRY_SIZE 1

uint32
  arad_pp_ihp_parser_eth_protocols_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHP_PARSER_ETH_PROTOCOLSm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->eth_type_protocol = soc_mem_field32_get(unit, IHP_PARSER_ETH_PROTOCOLSm, data, ETH_TYPE_PROTOCOLf);
  tbl_data->eth_sap_protocol = soc_mem_field32_get(unit, IHP_PARSER_ETH_PROTOCOLSm, data, ETH_SAP_PROTOCOLf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_parser_eth_protocols_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_parser_eth_protocols_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHP_PARSER_ETH_PROTOCOLSm, data, ETH_TYPE_PROTOCOLf, tbl_data->eth_type_protocol);
  soc_mem_field32_set(unit, IHP_PARSER_ETH_PROTOCOLSm, data, ETH_SAP_PROTOCOLf, tbl_data->eth_sap_protocol);

  res = soc_mem_write(
          unit,
          IHP_PARSER_ETH_PROTOCOLSm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_parser_eth_protocols_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHP_PARSER_IP_PROTOCOLS_TBL_ENTRY_SIZE 1

uint32
  arad_pp_ihp_parser_ip_protocols_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_PARSER_IP_PROTOCOLS_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_PARSER_IP_PROTOCOLS_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_PARSER_IP_PROTOCOLS_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_PARSER_IP_PROTOCOLS_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_PARSER_IP_PROTOCOLS_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHP_PARSER_IP_PROTOCOLSm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->ip_protocol = soc_mem_field32_get(unit, IHP_PARSER_IP_PROTOCOLSm, data, IP_PROTOCOLf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_parser_ip_protocols_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_parser_ip_protocols_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_PARSER_IP_PROTOCOLS_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_PARSER_IP_PROTOCOLS_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_PARSER_IP_PROTOCOLS_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_PARSER_IP_PROTOCOLS_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHP_PARSER_IP_PROTOCOLSm, data, IP_PROTOCOLf, tbl_data->ip_protocol);

  res = soc_mem_write(
          unit,
          IHP_PARSER_IP_PROTOCOLSm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_parser_ip_protocols_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_ENTRY_SIZE 1

uint32
  arad_pp_ihp_parser_custom_macro_protocols_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHP_PARSER_CUSTOM_MACRO_PROTOCOLSm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->cstm_protocol = soc_mem_field32_get(unit, IHP_PARSER_CUSTOM_MACRO_PROTOCOLSm, data, CSTM_PROTOCOLf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_parser_custom_macro_protocols_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_parser_custom_macro_protocols_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHP_PARSER_CUSTOM_MACRO_PROTOCOLSm, data, CSTM_PROTOCOLf, tbl_data->cstm_protocol);

  res = soc_mem_write(
          unit,
          IHP_PARSER_CUSTOM_MACRO_PROTOCOLSm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_parser_custom_macro_protocols_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHP_BVD_TOPOLOGY_ID_TBL_ENTRY_SIZE 2

uint32
  arad_pp_ihp_bvd_topology_id_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_BVD_TOPOLOGY_ID_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_BVD_TOPOLOGY_ID_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_BVD_TOPOLOGY_ID_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_BVD_TOPOLOGY_ID_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_BVD_TOPOLOGY_ID_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHP_BVD_TOPOLOGY_IDm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->lsb[0] = soc_mem_field32_get(unit, IHP_BVD_TOPOLOGY_IDm, data, LSBf);
  tbl_data->lsb[1] = soc_mem_field32_get(unit, IHP_BVD_TOPOLOGY_IDm, data, LSB_1f);
  tbl_data->lsb[2] = soc_mem_field32_get(unit, IHP_BVD_TOPOLOGY_IDm, data, LSB_2f);
  tbl_data->lsb[3] = soc_mem_field32_get(unit, IHP_BVD_TOPOLOGY_IDm, data, LSB_3f);
  tbl_data->lsb[4] = soc_mem_field32_get(unit, IHP_BVD_TOPOLOGY_IDm, data, LSB_4f);
  tbl_data->lsb[5] = soc_mem_field32_get(unit, IHP_BVD_TOPOLOGY_IDm, data, LSB_5f);
  tbl_data->lsb[6] = soc_mem_field32_get(unit, IHP_BVD_TOPOLOGY_IDm, data, LSB_6f);
  tbl_data->lsb[7] = soc_mem_field32_get(unit, IHP_BVD_TOPOLOGY_IDm, data, LSB_7f);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_bvd_topology_id_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_bvd_topology_id_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_BVD_TOPOLOGY_ID_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_BVD_TOPOLOGY_ID_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_BVD_TOPOLOGY_ID_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_BVD_TOPOLOGY_ID_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHP_BVD_TOPOLOGY_IDm, data, LSBf, tbl_data->lsb[0]);
  soc_mem_field32_set(unit, IHP_BVD_TOPOLOGY_IDm, data, LSB_1f, tbl_data->lsb[1]);
  soc_mem_field32_set(unit, IHP_BVD_TOPOLOGY_IDm, data, LSB_2f, tbl_data->lsb[2]);
  soc_mem_field32_set(unit, IHP_BVD_TOPOLOGY_IDm, data, LSB_3f, tbl_data->lsb[3]);
  soc_mem_field32_set(unit, IHP_BVD_TOPOLOGY_IDm, data, LSB_4f, tbl_data->lsb[4]);
  soc_mem_field32_set(unit, IHP_BVD_TOPOLOGY_IDm, data, LSB_5f, tbl_data->lsb[5]);
  soc_mem_field32_set(unit, IHP_BVD_TOPOLOGY_IDm, data, LSB_6f, tbl_data->lsb[6]);
  soc_mem_field32_set(unit, IHP_BVD_TOPOLOGY_IDm, data, LSB_7f, tbl_data->lsb[7]);

  res = soc_mem_write(
          unit,
          IHP_BVD_TOPOLOGY_IDm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_bvd_topology_id_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHP_BVD_FID_CLASS_TBL_ENTRY_SIZE 1

uint32
  arad_pp_ihp_bvd_fid_class_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_BVD_FID_CLASS_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_BVD_FID_CLASS_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_BVD_FID_CLASS_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_BVD_FID_CLASS_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_BVD_FID_CLASS_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHP_BVD_FID_CLASSm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->fid_class[0] = soc_mem_field32_get(unit, IHP_BVD_FID_CLASSm, data, FID_CLASSf);
  tbl_data->fid_class[1] = soc_mem_field32_get(unit, IHP_BVD_FID_CLASSm, data, FID_CLASS_1f);
  tbl_data->fid_class[2] = soc_mem_field32_get(unit, IHP_BVD_FID_CLASSm, data, FID_CLASS_2f);
  tbl_data->fid_class[3] = soc_mem_field32_get(unit, IHP_BVD_FID_CLASSm, data, FID_CLASS_3f);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_bvd_fid_class_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_bvd_fid_class_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_BVD_FID_CLASS_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_BVD_FID_CLASS_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_BVD_FID_CLASS_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_BVD_FID_CLASS_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHP_BVD_FID_CLASSm, data, FID_CLASSf, tbl_data->fid_class[0]);
  soc_mem_field32_set(unit, IHP_BVD_FID_CLASSm, data, FID_CLASS_1f, tbl_data->fid_class[1]);
  soc_mem_field32_set(unit, IHP_BVD_FID_CLASSm, data, FID_CLASS_2f, tbl_data->fid_class[2]);
  soc_mem_field32_set(unit, IHP_BVD_FID_CLASSm, data, FID_CLASS_3f, tbl_data->fid_class[3]);

  res = soc_mem_write(
          unit,
          IHP_BVD_FID_CLASSm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_bvd_fid_class_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHP_FID_CLASS_2_FID_TBL_ENTRY_SIZE 1

uint32
  arad_pp_ihp_fid_class_2_fid_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_FID_CLASS_2_FID_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_FID_CLASS_2_FID_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_FID_CLASS_2_FID_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_FID_CLASS_2_FID_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_FID_CLASS_2_FID_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHP_FID_CLASS_2_FIDm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->fid = soc_mem_field32_get(unit, IHP_FID_CLASS_2_FIDm, data, FIDf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_fid_class_2_fid_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_fid_class_2_fid_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_FID_CLASS_2_FID_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_FID_CLASS_2_FID_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_FID_CLASS_2_FID_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_FID_CLASS_2_FID_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHP_FID_CLASS_2_FIDm, data, FIDf, tbl_data->fid);

  res = soc_mem_write(
          unit,
          IHP_FID_CLASS_2_FIDm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_fid_class_2_fid_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_ENTRY_SIZE 1

uint32
  arad_pp_ihp_vlan_range_compression_table_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHP_VLAN_RANGE_COMPRESSION_TABLEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->vlan_range_lower_limit = soc_mem_field32_get(unit, IHP_VLAN_RANGE_COMPRESSION_TABLEm, data, VLAN_RANGE_LOWER_LIMITf);
  tbl_data->vlan_range_upper_limit = soc_mem_field32_get(unit, IHP_VLAN_RANGE_COMPRESSION_TABLEm, data, VLAN_RANGE_UPPER_LIMITf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_vlan_range_compression_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_vlan_range_compression_table_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHP_VLAN_RANGE_COMPRESSION_TABLEm, data, VLAN_RANGE_LOWER_LIMITf, tbl_data->vlan_range_lower_limit);
  soc_mem_field32_set(unit, IHP_VLAN_RANGE_COMPRESSION_TABLEm, data, VLAN_RANGE_UPPER_LIMITf, tbl_data->vlan_range_upper_limit);

  res = soc_mem_write(
          unit,
          IHP_VLAN_RANGE_COMPRESSION_TABLEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_vlan_range_compression_table_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_ENTRY_SIZE 3

uint32
  arad_pp_ihp_vtt_in_pp_port_vlan_config_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHP_VTT_IN_PP_PORT_VLAN_CONFIGm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->range_valid_0 = soc_mem_field32_get(unit, IHP_VTT_IN_PP_PORT_VLAN_CONFIGm, data, RANGE_VALID_0f);
  tbl_data->range_valid_1 = soc_mem_field32_get(unit, IHP_VTT_IN_PP_PORT_VLAN_CONFIGm, data, RANGE_VALID_1f);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_vtt_in_pp_port_vlan_config_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_vtt_in_pp_port_vlan_config_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHP_VTT_IN_PP_PORT_VLAN_CONFIGm, data, RANGE_VALID_0f, tbl_data->range_valid_0);
  soc_mem_field32_set(unit, IHP_VTT_IN_PP_PORT_VLAN_CONFIGm, data, RANGE_VALID_1f, tbl_data->range_valid_1);

  res = soc_mem_write(
          unit,
          IHP_VTT_IN_PP_PORT_VLAN_CONFIGm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_vtt_in_pp_port_vlan_config_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_ENTRY_SIZE 1

uint32
  arad_pp_ihp_designated_vlan_table_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHP_DESIGNATED_VLAN_TABLEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->designated_vlan = soc_mem_field32_get(unit, IHP_DESIGNATED_VLAN_TABLEm, data, DESIGNATED_VLANf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_designated_vlan_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_designated_vlan_table_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHP_DESIGNATED_VLAN_TABLEm, data, DESIGNATED_VLANf, tbl_data->designated_vlan);

  res = soc_mem_write(
          unit,
          IHP_DESIGNATED_VLAN_TABLEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_designated_vlan_table_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_ENTRY_SIZE 2

uint32
  arad_pp_ihp_vtt_in_pp_port_config_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHP_VTT_IN_PP_PORT_CONFIGm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->vlan_domain = soc_mem_field32_get(unit, IHP_VTT_IN_PP_PORT_CONFIGm, data, VLAN_DOMAINf);
  tbl_data->vlan_translation_profile = soc_mem_field32_get(unit, IHP_VTT_IN_PP_PORT_CONFIGm, data, VLAN_TRANSLATION_PROFILEf);
  tbl_data->termination_profile = soc_mem_field32_get(unit, IHP_VTT_IN_PP_PORT_CONFIGm, data, TERMINATION_PROFILEf);
  tbl_data->designated_vlan_index = soc_mem_field32_get(unit, IHP_VTT_IN_PP_PORT_CONFIGm, data, DESIGNATED_VLAN_INDEXf);
  tbl_data->default_port_lif = soc_mem_field32_get(unit, IHP_VTT_IN_PP_PORT_CONFIGm, data, DEFAULT_PORT_LIFf);
  tbl_data->all_vlan_member = soc_mem_field32_get(unit, IHP_VTT_IN_PP_PORT_CONFIGm, data, ALL_VLAN_MEMBERf);
  tbl_data->learn_lif = soc_mem_field32_get(unit, IHP_VTT_IN_PP_PORT_CONFIGm, data, LEARN_LIFf);
  tbl_data->use_in_lif = soc_mem_field32_get(unit, IHP_VTT_IN_PP_PORT_CONFIGm, data, USE_IN_LIFf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_vtt_in_pp_port_config_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_vtt_in_pp_port_config_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHP_VTT_IN_PP_PORT_CONFIGm, data, VLAN_DOMAINf, tbl_data->vlan_domain);
  soc_mem_field32_set(unit, IHP_VTT_IN_PP_PORT_CONFIGm, data, VLAN_TRANSLATION_PROFILEf, tbl_data->vlan_translation_profile);
  soc_mem_field32_set(unit, IHP_VTT_IN_PP_PORT_CONFIGm, data, TERMINATION_PROFILEf, tbl_data->termination_profile);
  soc_mem_field32_set(unit, IHP_VTT_IN_PP_PORT_CONFIGm, data, DESIGNATED_VLAN_INDEXf, tbl_data->designated_vlan_index);
  soc_mem_field32_set(unit, IHP_VTT_IN_PP_PORT_CONFIGm, data, DEFAULT_PORT_LIFf, tbl_data->default_port_lif);
  soc_mem_field32_set(unit, IHP_VTT_IN_PP_PORT_CONFIGm, data, ALL_VLAN_MEMBERf, tbl_data->all_vlan_member);
  soc_mem_field32_set(unit, IHP_VTT_IN_PP_PORT_CONFIGm, data, LEARN_LIFf, tbl_data->learn_lif);
  soc_mem_field32_set(unit, IHP_VTT_IN_PP_PORT_CONFIGm, data, USE_IN_LIFf, tbl_data->use_in_lif);

  res = soc_mem_write(
          unit,
          IHP_VTT_IN_PP_PORT_CONFIGm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_vtt_in_pp_port_config_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHP_VTT_LLVP_TBL_ENTRY_SIZE 1

uint32
  arad_pp_ihp_vtt_llvp_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_VTT_LLVP_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_VTT_LLVP_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_VTT_LLVP_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_VTT_LLVP_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_VTT_LLVP_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHP_VTT_LLVPm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->incoming_tag_structure = soc_mem_field32_get(unit, IHP_VTT_LLVPm, data, INCOMING_TAG_STRUCTUREf);
  tbl_data->ivec = soc_mem_field32_get(unit, IHP_VTT_LLVPm, data, IVECf);
  tbl_data->pcp_dei_profile = soc_mem_field32_get(unit, IHP_VTT_LLVPm, data, PCP_DEI_PROFILEf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_vtt_llvp_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_vtt_llvp_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_VTT_LLVP_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_VTT_LLVP_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_VTT_LLVP_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_VTT_LLVP_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHP_VTT_LLVPm, data, INCOMING_TAG_STRUCTUREf, tbl_data->incoming_tag_structure);
  soc_mem_field32_set(unit, IHP_VTT_LLVPm, data, IVECf, tbl_data->ivec);
  soc_mem_field32_set(unit, IHP_VTT_LLVPm, data, PCP_DEI_PROFILEf, tbl_data->pcp_dei_profile);

  res = soc_mem_write(
          unit,
          IHP_VTT_LLVPm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_vtt_llvp_tbl_set_unsafe()", entry_offset, 0);
}



uint32
  arad_pp_ihp_ingress_vlan_edit_command_table_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHP_INGRESS_VLAN_EDIT_COMMAND_TABLEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->ivec = soc_mem_field32_get(unit, IHP_INGRESS_VLAN_EDIT_COMMAND_TABLEm, data, IVECf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_ingress_vlan_edit_command_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_ingress_vlan_edit_command_table_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHP_INGRESS_VLAN_EDIT_COMMAND_TABLEm, data, IVECf, tbl_data->ivec);

  res = soc_mem_write(
          unit,
          IHP_INGRESS_VLAN_EDIT_COMMAND_TABLEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_ingress_vlan_edit_command_table_tbl_set_unsafe()", entry_offset, 0);
}



uint32
  arad_pp_ihp_vlan_edit_pcp_dei_map_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHP_VLAN_EDIT_PCP_DEI_MAPm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->pcp_dei = soc_mem_field32_get(unit, IHP_VLAN_EDIT_PCP_DEI_MAPm, data, PCP_DEIf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_vlan_edit_pcp_dei_map_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_vlan_edit_pcp_dei_map_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHP_VLAN_EDIT_PCP_DEI_MAPm, data, PCP_DEIf, tbl_data->pcp_dei);

  res = soc_mem_write(
          unit,
          IHP_VLAN_EDIT_PCP_DEI_MAPm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_vlan_edit_pcp_dei_map_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_in_rif_config_table_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHP_IN_RIF_CONFIG_TABLEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->cos_profile = soc_mem_field32_get(unit, IHP_IN_RIF_CONFIG_TABLEm, data, SOC_IS_JERICHO(unit)? IN_RIF_COS_PROFILEf: COS_PROFILEf);
  tbl_data->vrf = soc_mem_field32_get(unit, IHP_IN_RIF_CONFIG_TABLEm, data, VRFf);

  if (SOC_IS_JERICHO(unit)) {
    
    uint32 in_rif_profile;
    in_rif_profile = soc_mem_field32_get(unit, IHP_IN_RIF_CONFIG_TABLEm, data, IN_RIF_PROFILEf);
    tbl_data->uc_rpf_enable = SHR_BITGET(&in_rif_profile, ARAD_PP_RIF_PROFILE_ENCODING_UC_RPF_ENABLE);
    tbl_data->enable_routing_mc = SHR_BITGET(&in_rif_profile, ARAD_PP_RIF_PROFILE_ENCODING_ENABLE_ROUTING_MC);
    tbl_data->enable_routing_uc = SHR_BITGET(&in_rif_profile, ARAD_PP_RIF_PROFILE_ENCODING_ENABLE_ROUTING_UC);
    tbl_data->enable_routing_mpls = SHR_BITGET(&in_rif_profile, ARAD_PP_RIF_PROFILE_ENCODING_ENABLE_ROUTING_MPLS);
    tbl_data->l_3_vpn_default_routing = SHR_BITGET(&in_rif_profile, ARAD_PP_RIF_PROFILE_ENCODING_L_3_VPN_DEFAULT_ROUTING);
  } else {
    tbl_data->uc_rpf_enable = soc_mem_field32_get(unit, IHP_IN_RIF_CONFIG_TABLEm, data, UC_RPF_ENABLEf);
    tbl_data->enable_routing_mc = soc_mem_field32_get(unit, IHP_IN_RIF_CONFIG_TABLEm, data, ENABLE_ROUTING_MCf);
    tbl_data->enable_routing_uc = soc_mem_field32_get(unit, IHP_IN_RIF_CONFIG_TABLEm, data, ENABLE_ROUTING_UCf);
    tbl_data->enable_routing_mpls = soc_mem_field32_get(unit, IHP_IN_RIF_CONFIG_TABLEm, data, ENABLE_ROUTING_MPLSf);
    tbl_data->l_3_vpn_default_routing = soc_mem_field32_get(unit, IHP_IN_RIF_CONFIG_TABLEm, data, L_3_VPN_DEFAULT_ROUTINGf);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_in_rif_config_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_in_rif_config_table_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHP_IN_RIF_CONFIG_TABLEm, data, VRFf, tbl_data->vrf);
  soc_mem_field32_set(unit, IHP_IN_RIF_CONFIG_TABLEm, data, SOC_IS_JERICHO(unit)? IN_RIF_COS_PROFILEf: COS_PROFILEf, tbl_data->cos_profile);

  if (SOC_IS_JERICHO(unit)) {
    
    uint32 in_rif_profile = 0;
    SHR_BITCOPY_RANGE(&in_rif_profile, ARAD_PP_RIF_PROFILE_ENCODING_UC_RPF_ENABLE, &tbl_data->uc_rpf_enable, 0, 1);
    SHR_BITCOPY_RANGE(&in_rif_profile, ARAD_PP_RIF_PROFILE_ENCODING_ENABLE_ROUTING_MC, &tbl_data->enable_routing_mc, 0, 1);
    SHR_BITCOPY_RANGE(&in_rif_profile, ARAD_PP_RIF_PROFILE_ENCODING_ENABLE_ROUTING_UC, &tbl_data->enable_routing_uc, 0, 1);
    SHR_BITCOPY_RANGE(&in_rif_profile, ARAD_PP_RIF_PROFILE_ENCODING_ENABLE_ROUTING_MPLS, &tbl_data->enable_routing_mpls, 0, 1);
    SHR_BITCOPY_RANGE(&in_rif_profile, ARAD_PP_RIF_PROFILE_ENCODING_L_3_VPN_DEFAULT_ROUTING, &tbl_data->l_3_vpn_default_routing, 0, 1);
    soc_mem_field32_set(unit, IHP_IN_RIF_CONFIG_TABLEm, data, IN_RIF_PROFILEf, in_rif_profile);
  } else {
    soc_mem_field32_set(unit, IHP_IN_RIF_CONFIG_TABLEm, data, UC_RPF_ENABLEf, tbl_data->uc_rpf_enable);
    soc_mem_field32_set(unit, IHP_IN_RIF_CONFIG_TABLEm, data, ENABLE_ROUTING_MCf, tbl_data->enable_routing_mc);
    soc_mem_field32_set(unit, IHP_IN_RIF_CONFIG_TABLEm, data, ENABLE_ROUTING_UCf, tbl_data->enable_routing_uc);
    soc_mem_field32_set(unit, IHP_IN_RIF_CONFIG_TABLEm, data, ENABLE_ROUTING_MPLSf, tbl_data->enable_routing_mpls);
    soc_mem_field32_set(unit, IHP_IN_RIF_CONFIG_TABLEm, data, L_3_VPN_DEFAULT_ROUTINGf, tbl_data->l_3_vpn_default_routing);
  }

  res = soc_mem_write(
          unit,
          IHP_IN_RIF_CONFIG_TABLEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_in_rif_config_table_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_tc_dp_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_TC_DP_MAP_TABLE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_TC_DP_MAP_TABLE_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_TC_DP_MAP_TABLE_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_TC_DP_MAP_TABLE_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_TC_DP_MAP_TABLE_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHP_TC_DP_MAP_TABLEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->traffic_class_0 = soc_mem_field32_get(unit, IHP_TC_DP_MAP_TABLEm, data, TRAFFIC_CLASS_0f);
  tbl_data->drop_precedence_0 = soc_mem_field32_get(unit, IHP_TC_DP_MAP_TABLEm, data, DROP_PRECEDENCE_0f);
  tbl_data->traffic_class_1 = soc_mem_field32_get(unit, IHP_TC_DP_MAP_TABLEm, data, TRAFFIC_CLASS_1f);
  tbl_data->drop_precedence_1 = soc_mem_field32_get(unit, IHP_TC_DP_MAP_TABLEm, data, DROP_PRECEDENCE_1f);
  tbl_data->traffic_class_2 = soc_mem_field32_get(unit, IHP_TC_DP_MAP_TABLEm, data, TRAFFIC_CLASS_2f);
  tbl_data->drop_precedence_2 = soc_mem_field32_get(unit, IHP_TC_DP_MAP_TABLEm, data, DROP_PRECEDENCE_2f);
  tbl_data->traffic_class_3 = soc_mem_field32_get(unit, IHP_TC_DP_MAP_TABLEm, data, TRAFFIC_CLASS_3f);
  tbl_data->drop_precedence_3 = soc_mem_field32_get(unit, IHP_TC_DP_MAP_TABLEm, data, DROP_PRECEDENCE_3f);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_tc_dp_map_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_tc_dp_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_TC_DP_MAP_TABLE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_TC_DP_MAP_TABLE_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_TC_DP_MAP_TABLE_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_TC_DP_MAP_TABLE_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHP_TC_DP_MAP_TABLEm, data, TRAFFIC_CLASS_0f, tbl_data->traffic_class_0);
  soc_mem_field32_set(unit, IHP_TC_DP_MAP_TABLEm, data, DROP_PRECEDENCE_0f, tbl_data->drop_precedence_0);
  soc_mem_field32_set(unit, IHP_TC_DP_MAP_TABLEm, data, TRAFFIC_CLASS_1f, tbl_data->traffic_class_1);
  soc_mem_field32_set(unit, IHP_TC_DP_MAP_TABLEm, data, DROP_PRECEDENCE_1f, tbl_data->drop_precedence_1);
  soc_mem_field32_set(unit, IHP_TC_DP_MAP_TABLEm, data, TRAFFIC_CLASS_2f, tbl_data->traffic_class_2);
  soc_mem_field32_set(unit, IHP_TC_DP_MAP_TABLEm, data, DROP_PRECEDENCE_2f, tbl_data->drop_precedence_2);
  soc_mem_field32_set(unit, IHP_TC_DP_MAP_TABLEm, data, TRAFFIC_CLASS_3f, tbl_data->traffic_class_3);
  soc_mem_field32_set(unit, IHP_TC_DP_MAP_TABLEm, data, DROP_PRECEDENCE_3f, tbl_data->drop_precedence_3);

  res = soc_mem_write(
          unit,
          IHP_TC_DP_MAP_TABLEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_tc_dp_map_table_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHB_PINFO_FER_TBL_ENTRY_SIZE 1

int
  arad_pp_ihb_pinfo_fer_tbl_get_unsafe(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  uint32     entry_offset,
    SOC_SAND_IN  int        core,
    SOC_SAND_OUT ARAD_PP_IHB_PINFO_FER_TBL_DATA *tbl_data
  )
{
    uint32 data[ARAD_PP_IHB_PINFO_FER_TBL_ENTRY_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    sal_memset(data, 0, sizeof(uint32) * ARAD_PP_IHB_PINFO_FER_TBL_ENTRY_SIZE);
    sal_memset(tbl_data, 0, sizeof(ARAD_PP_IHB_PINFO_FER_TBL_DATA));

    SOCDNX_IF_ERR_EXIT(soc_mem_read(
          unit,
          IHB_PINFO_FERm,
          IHB_BLOCK(unit, core),
          entry_offset,
          data
        ));

    tbl_data->ecmp_lb_key_count = soc_mem_field32_get(unit, IHB_PINFO_FERm, data, ECMP_LB_KEY_COUNTf);
    tbl_data->lag_lb_key_start = soc_mem_field32_get(unit, IHB_PINFO_FERm, data, LAG_LB_KEY_STARTf);
    tbl_data->lag_lb_key_count = soc_mem_field32_get(unit, IHB_PINFO_FERm, data, LAG_LB_KEY_COUNTf);
    tbl_data->lb_profile = soc_mem_field32_get(unit, IHB_PINFO_FERm, data, LB_PROFILEf);
    tbl_data->lb_bos_search = soc_mem_field32_get(unit, IHB_PINFO_FERm, data, LB_BOS_SEARCHf);
    tbl_data->lb_include_bos_hdr = soc_mem_field32_get(unit, IHB_PINFO_FERm, data, LB_INCLUDE_BOS_HDRf);
    tbl_data->icmp_redirect_enable = soc_mem_field32_get(unit, IHB_PINFO_FERm, data, ICMP_REDIRECT_ENABLEf);

exit:
    SOCDNX_FUNC_RETURN;
}

int
  arad_pp_ihb_pinfo_fer_tbl_set_unsafe(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  uint32     entry_offset,
    SOC_SAND_IN  int        core,
    SOC_SAND_IN  ARAD_PP_IHB_PINFO_FER_TBL_DATA *tbl_data
  )
{
    uint32 data[ARAD_PP_IHB_PINFO_FER_TBL_ENTRY_SIZE];

    SOCDNX_INIT_FUNC_DEFS;
 
    sal_memset(data, 0, sizeof(uint32)*ARAD_PP_IHB_PINFO_FER_TBL_ENTRY_SIZE);

    soc_mem_field32_set(unit, IHB_PINFO_FERm, data, ECMP_LB_KEY_COUNTf, tbl_data->ecmp_lb_key_count);
    soc_mem_field32_set(unit, IHB_PINFO_FERm, data, LAG_LB_KEY_STARTf, tbl_data->lag_lb_key_start);
    soc_mem_field32_set(unit, IHB_PINFO_FERm, data, LAG_LB_KEY_COUNTf, tbl_data->lag_lb_key_count);
    soc_mem_field32_set(unit, IHB_PINFO_FERm, data, LB_PROFILEf, tbl_data->lb_profile);
    soc_mem_field32_set(unit, IHB_PINFO_FERm, data, LB_BOS_SEARCHf, tbl_data->lb_bos_search);
    soc_mem_field32_set(unit, IHB_PINFO_FERm, data, LB_INCLUDE_BOS_HDRf, tbl_data->lb_include_bos_hdr);
    soc_mem_field32_set(unit, IHB_PINFO_FERm, data, ICMP_REDIRECT_ENABLEf, tbl_data->icmp_redirect_enable);

    SOCDNX_IF_ERR_EXIT(soc_mem_write(
          unit,
          IHB_PINFO_FERm,
          IHB_BLOCK(unit, core),
          entry_offset,
          data
        ));

exit:
    SOCDNX_FUNC_RETURN;
}

#define ARAD_PP_IHB_LB_PFC_PROFILE_TBL_ENTRY_SIZE 1

uint32
  arad_pp_ihb_lb_pfc_profile_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHB_LB_PFC_PROFILE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHB_LB_PFC_PROFILE_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_LB_PFC_PROFILE_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHB_LB_PFC_PROFILE_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHB_LB_PFC_PROFILE_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHB_LB_PFC_PROFILEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->lb_vector_index_1 = soc_mem_field32_get(unit, IHB_LB_PFC_PROFILEm, data, LB_VECTOR_INDEX_1f);
  tbl_data->lb_vector_index_2 = soc_mem_field32_get(unit, IHB_LB_PFC_PROFILEm, data, LB_VECTOR_INDEX_2f);
  tbl_data->lb_vector_index_3 = soc_mem_field32_get(unit, IHB_LB_PFC_PROFILEm, data, LB_VECTOR_INDEX_3f);
  tbl_data->lb_vector_index_4 = soc_mem_field32_get(unit, IHB_LB_PFC_PROFILEm, data, LB_VECTOR_INDEX_4f);
  tbl_data->lb_vector_index_5 = soc_mem_field32_get(unit, IHB_LB_PFC_PROFILEm, data, LB_VECTOR_INDEX_5f);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_lb_pfc_profile_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihb_lb_pfc_profile_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHB_LB_PFC_PROFILE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHB_LB_PFC_PROFILE_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_LB_PFC_PROFILE_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHB_LB_PFC_PROFILE_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHB_LB_PFC_PROFILEm, data, LB_VECTOR_INDEX_1f, tbl_data->lb_vector_index_1);
  soc_mem_field32_set(unit, IHB_LB_PFC_PROFILEm, data, LB_VECTOR_INDEX_2f, tbl_data->lb_vector_index_2);
  soc_mem_field32_set(unit, IHB_LB_PFC_PROFILEm, data, LB_VECTOR_INDEX_3f, tbl_data->lb_vector_index_3);
  soc_mem_field32_set(unit, IHB_LB_PFC_PROFILEm, data, LB_VECTOR_INDEX_4f, tbl_data->lb_vector_index_4);
  soc_mem_field32_set(unit, IHB_LB_PFC_PROFILEm, data, LB_VECTOR_INDEX_5f, tbl_data->lb_vector_index_5);

  res = soc_mem_write(
          unit,
          IHB_LB_PFC_PROFILEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_lb_pfc_profile_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_ENTRY_SIZE 2

uint32
  arad_pp_ihb_lb_vector_program_map_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHB_LB_VECTOR_PROGRAM_MAPm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  soc_mem_field64_get(unit, IHB_LB_VECTOR_PROGRAM_MAPm, data, CHUNK_BITMAPf, &tbl_data->chunk_bitmap);
  tbl_data->chunk_size = soc_mem_field32_get(unit, IHB_LB_VECTOR_PROGRAM_MAPm, data, CHUNK_SIZEf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_lb_vector_program_map_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihb_lb_vector_program_map_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_ENTRY_SIZE);

  soc_mem_field64_set(unit, IHB_LB_VECTOR_PROGRAM_MAPm, data, CHUNK_BITMAPf, tbl_data->chunk_bitmap);
  soc_mem_field32_set(unit, IHB_LB_VECTOR_PROGRAM_MAPm, data, CHUNK_SIZEf, tbl_data->chunk_size);

  res = soc_mem_write(
          unit,
          IHB_LB_VECTOR_PROGRAM_MAPm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_lb_vector_program_map_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihb_fec_ecmp_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHB_FEC_ECMP_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[SOC_DPP_DEFS_MAX(IHB_FEC_ECMP_NOF_LONGS)];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_FEC_ECMP_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, SOC_DPP_DEFS_MAX(IHB_FEC_ECMP_NOF_LONGS));
  ARAD_CLEAR(tbl_data, ARAD_PP_IHB_FEC_ECMP_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHB_FEC_ECMPm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->group_start_pointer = soc_mem_field32_get(unit, IHB_FEC_ECMPm, data, GROUP_START_POINTERf);
  tbl_data->group_size = soc_mem_field32_get(unit, IHB_FEC_ECMPm, data, GROUP_SIZEf);
  tbl_data->is_protected = soc_mem_field32_get(unit, IHB_FEC_ECMPm, data, PROTECTEDf);
  if (SOC_IS_JERICHO(unit)) {
      tbl_data->rpf_fec_pointer = soc_mem_field32_get(unit, IHB_FEC_RPF_ECMPm, data, RPF_FEC_POINTERf);
  }
  else {
      tbl_data->rpf_fec_pointer = soc_mem_field32_get(unit, IHB_FEC_ECMPm, data, RPF_FEC_POINTERf);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_fec_ecmp_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihb_fec_ecmp_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHB_FEC_ECMP_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[SOC_DPP_DEFS_MAX(IHB_FEC_ECMP_NOF_LONGS)];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_FEC_ECMP_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, SOC_DPP_DEFS_MAX(IHB_FEC_ECMP_NOF_LONGS));

  soc_mem_field32_set(unit, IHB_FEC_ECMPm, data, GROUP_START_POINTERf, tbl_data->group_start_pointer);
  soc_mem_field32_set(unit, IHB_FEC_ECMPm, data, GROUP_SIZEf, tbl_data->group_size);
  soc_mem_field32_set(unit, IHB_FEC_ECMPm, data, PROTECTEDf, tbl_data->is_protected);
  if (SOC_IS_JERICHO(unit)) {
      soc_mem_field32_set(unit, IHB_FEC_RPF_ECMPm, data, RPF_FEC_POINTERf, tbl_data->rpf_fec_pointer);
  }
  else {
      soc_mem_field32_set(unit, IHB_FEC_ECMPm, data, RPF_FEC_POINTERf, tbl_data->rpf_fec_pointer);
  }

  res = soc_mem_write(
          unit,
          IHB_FEC_ECMPm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_fec_ecmp_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHB_FEC_SUPER_ENTRY_TBL_ENTRY_SIZE 5

uint32
  arad_pp_ihb_fec_super_entry_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHB_FEC_SUPER_ENTRY_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHB_FEC_SUPER_ENTRY_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_FEC_SUPER_ENTRY_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHB_FEC_SUPER_ENTRY_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHB_FEC_SUPER_ENTRY_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          SOC_IS_ARADPLUS_AND_BELOW(unit)?IHB_FEC_SUPER_ENTRYm:PPDB_A_FEC_SUPER_ENTRY_BANKm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->protection_pointer = soc_mem_field32_get(unit, SOC_IS_ARADPLUS_AND_BELOW(unit)?IHB_FEC_SUPER_ENTRYm:PPDB_A_FEC_SUPER_ENTRY_BANKm, data, PROTECTION_POINTERf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_fec_super_entry_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihb_fec_super_entry_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHB_FEC_SUPER_ENTRY_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHB_FEC_SUPER_ENTRY_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_FEC_SUPER_ENTRY_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHB_FEC_SUPER_ENTRY_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, SOC_IS_ARADPLUS_AND_BELOW(unit)?IHB_FEC_SUPER_ENTRYm:PPDB_A_FEC_SUPER_ENTRY_BANKm, data, PROTECTION_POINTERf, tbl_data->protection_pointer);

  res = soc_mem_write(
          unit,
          SOC_IS_ARADPLUS_AND_BELOW(unit)?IHB_FEC_SUPER_ENTRYm:PPDB_A_FEC_SUPER_ENTRY_BANKm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_fec_super_entry_tbl_set_unsafe()", entry_offset, 0);
}


uint32
  arad_pp_ihb_fec_super_entry_tbl_fill_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  ARAD_PP_IHB_FEC_SUPER_ENTRY_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHB_FEC_SUPER_ENTRY_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_FEC_SUPER_ENTRY_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHB_FEC_SUPER_ENTRY_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, SOC_IS_ARADPLUS_AND_BELOW(unit)?IHB_FEC_SUPER_ENTRYm:PPDB_A_FEC_SUPER_ENTRY_BANKm, data, PROTECTION_POINTERf, tbl_data->protection_pointer);

   res = arad_fill_table_with_entry(unit, SOC_IS_ARADPLUS_AND_BELOW(unit)?IHB_FEC_SUPER_ENTRYm:PPDB_A_FEC_SUPER_ENTRY_BANKm, MEM_BLOCK_ANY, data); /* fill memory with the entry in data */
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_fec_super_entry_tbl_set_unsafe()", 0, 0);
}

#define ARAD_PP_IHB_FEC_ENTRY_GENERAL_TBL_ENTRY_SIZE 2

uint32
  arad_pp_ihb_fec_entry_general_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHB_FEC_ENTRY_GENERAL_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHB_FEC_ENTRY_GENERAL_TBL_ENTRY_SIZE];
  uint32
    entry_format=0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_FEC_ENTRY_GENERAL_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHB_FEC_ENTRY_GENERAL_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHB_FEC_ENTRY_GENERAL_TBL_DATA, 1);

  if (SOC_IS_JERICHO(unit)) {
      soc_reg_above_64_val_t reg_above_64;
      SOC_REG_ABOVE_64_CLEAR(reg_above_64);
      sal_memset(data, 0, sizeof(data));
      res = READ_PPDB_A_FEC_SUPER_ENTRY_BANKm(
              unit,
              JERICHO_PP_TBL_FEC_ARRAY_INDEX(unit, entry_offset),
              MEM_BLOCK_ANY,
              JERICHO_PP_TBL_FEC_ENTRY_INDEX(unit, entry_offset),
              reg_above_64
            );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
      soc_mem_field_get(unit, PPDB_A_FEC_SUPER_ENTRY_BANKm, reg_above_64, 
                        JERICHO_PP_TBL_FEC_ENTRY_OFFSET(unit, entry_offset) == 0 ? DATA_0f : DATA_1f, data);
  }
  else {
      res = READ_IHB_FEC_ENTRY_GENERALm(
              unit,
              ARAD_PP_TBL_FEC_ARRAY_INDEX(entry_offset),
              MEM_BLOCK_ANY,
              ARAD_PP_TBL_FEC_ENTRY_INDEX(entry_offset),
              data
            );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
  }

  tbl_data->destination = soc_mem_field32_get(unit, SOC_IS_ARADPLUS_AND_BELOW(unit)?IHB_FEC_ENTRY_GENERALm:PPDB_A_FEC_ENTRY_GENERALm, data, DESTINATIONf);
  tbl_data->uc_rpf_mode = soc_mem_field32_get(unit, SOC_IS_ARADPLUS_AND_BELOW(unit)?IHB_FEC_ENTRY_GENERALm:PPDB_A_FEC_ENTRY_GENERALm, data, UC_RPF_MODEf);
  tbl_data->mc_rpf_mode = soc_mem_field32_get(unit, SOC_IS_ARADPLUS_AND_BELOW(unit)?IHB_FEC_ENTRY_GENERALm:PPDB_A_FEC_ENTRY_GENERALm, data, MC_RPF_MODEf);

  SHR_BITCOPY_RANGE(&entry_format,0, data, SOC_IS_ARADPLUS_AND_BELOW(unit)?50:56, 1);
  if(entry_format == 0) { /* format A */
    tbl_data->arp_ptr = soc_mem_field32_get(unit, SOC_IS_ARADPLUS_AND_BELOW(unit)?IHB_FEC_ENTRY_FORMAT_Am:PPDB_A_FEC_ENTRY_FORMAT_Am, data, ARP_PTRf);
    tbl_data->out_lif = soc_mem_field32_get(unit, SOC_IS_ARADPLUS_AND_BELOW(unit)?IHB_FEC_ENTRY_FORMAT_Am:PPDB_A_FEC_ENTRY_FORMAT_Am, data, OUT_LIFf);
    tbl_data->format = ARAD_PP_IHB_FEC_FORMAT_A;
    goto  exit;
  }
  entry_format = 0;
  SHR_BITCOPY_RANGE(&entry_format,0, data,SOC_IS_ARADPLUS_AND_BELOW(unit)?49:55, 2);
  if(entry_format == 3) { /* format NULL */
    tbl_data->format = ARAD_PP_IHB_FEC_FORMAT_NULL;
    goto  exit;
  }
  entry_format = 0;
  SHR_BITCOPY_RANGE(&entry_format,0, data,SOC_IS_ARADPLUS_AND_BELOW(unit)?48:54, 3);
  if(entry_format == 4) { /* format B */
    tbl_data->format = ARAD_PP_IHB_FEC_FORMAT_B;
    tbl_data->out_lif = soc_mem_field32_get(unit, SOC_IS_ARADPLUS_AND_BELOW(unit)?IHB_FEC_ENTRY_FORMAT_Bm:PPDB_A_FEC_ENTRY_FORMAT_Bm, data, OUT_LIFf);
    goto exit;
  }
  if(entry_format == 5) { /* format C */
    tbl_data->format = ARAD_PP_IHB_FEC_FORMAT_C;
    tbl_data->eei = soc_mem_field32_get(unit, SOC_IS_ARADPLUS_AND_BELOW(unit)?IHB_FEC_ENTRY_FORMAT_Cm:PPDB_A_FEC_ENTRY_FORMAT_Cm, data, EEIf);
    goto exit;
  }
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_fec_entry_general_tbl_get_unsafe()", entry_offset, 0);
}


uint32
  arad_pp_ihb_fec_entry_general_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHB_FEC_ENTRY_GENERAL_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHB_FEC_ENTRY_GENERAL_TBL_ENTRY_SIZE];
  uint32
    entry_format;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_FEC_ENTRY_GENERAL_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHB_FEC_ENTRY_GENERAL_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, SOC_IS_ARADPLUS_AND_BELOW(unit)?IHB_FEC_ENTRY_GENERALm:PPDB_A_FEC_ENTRY_GENERALm, data, DESTINATIONf, tbl_data->destination);
  soc_mem_field32_set(unit, SOC_IS_ARADPLUS_AND_BELOW(unit)?IHB_FEC_ENTRY_GENERALm:PPDB_A_FEC_ENTRY_GENERALm, data, UC_RPF_MODEf, tbl_data->uc_rpf_mode);
  soc_mem_field32_set(unit, SOC_IS_ARADPLUS_AND_BELOW(unit)?IHB_FEC_ENTRY_GENERALm:PPDB_A_FEC_ENTRY_GENERALm, data, MC_RPF_MODEf, tbl_data->mc_rpf_mode);
  if(tbl_data->format == ARAD_PP_IHB_FEC_FORMAT_A) {
    entry_format = 0;
    soc_mem_field32_set(unit, SOC_IS_ARADPLUS_AND_BELOW(unit)?IHB_FEC_ENTRY_FORMAT_Am:PPDB_A_FEC_ENTRY_FORMAT_Am, data, ARP_PTRf, tbl_data->arp_ptr);
    soc_mem_field32_set(unit, SOC_IS_ARADPLUS_AND_BELOW(unit)?IHB_FEC_ENTRY_FORMAT_Am:PPDB_A_FEC_ENTRY_FORMAT_Am, data, OUT_LIFf, tbl_data->out_lif);
    if (SOC_IS_JERICHO(unit)) {
        soc_mem_field32_set(unit, IHB_FEC_ENTRY_FORMAT_Am, data, OUT_LIF_IS_NATIVE_VSIf, 1); 
    }
    soc_mem_field32_set(unit, SOC_IS_ARADPLUS_AND_BELOW(unit)?IHB_FEC_ENTRY_FORMAT_Am:PPDB_A_FEC_ENTRY_FORMAT_Am, data, IDENTIFIERf, entry_format);
  }
  else if(tbl_data->format == ARAD_PP_IHB_FEC_FORMAT_NULL) {
    entry_format = 3;
    soc_mem_field32_set(unit, SOC_IS_ARADPLUS_AND_BELOW(unit)?IHB_FEC_ENTRY_FORMAT_NULLm:PPDB_A_FEC_ENTRY_FORMAT_NULLm, data, RESERVEDf, 0);
    soc_mem_field32_set(unit, SOC_IS_ARADPLUS_AND_BELOW(unit)?IHB_FEC_ENTRY_FORMAT_NULLm:PPDB_A_FEC_ENTRY_FORMAT_NULLm, data, IDENTIFIERf, entry_format);
  }
  else if(tbl_data->format == ARAD_PP_IHB_FEC_FORMAT_B) {
    entry_format = 4;
    soc_mem_field32_set(unit, SOC_IS_ARADPLUS_AND_BELOW(unit)?IHB_FEC_ENTRY_FORMAT_Bm:PPDB_A_FEC_ENTRY_FORMAT_Bm, data, OUT_LIFf, tbl_data->out_lif);
    soc_mem_field32_set(unit, SOC_IS_ARADPLUS_AND_BELOW(unit)?IHB_FEC_ENTRY_FORMAT_Bm:PPDB_A_FEC_ENTRY_FORMAT_Bm, data, RESERVEDf, 0);
    soc_mem_field32_set(unit, SOC_IS_ARADPLUS_AND_BELOW(unit)?IHB_FEC_ENTRY_FORMAT_Bm:PPDB_A_FEC_ENTRY_FORMAT_Bm, data, IDENTIFIERf, entry_format);
  }
  else if(tbl_data->format == ARAD_PP_IHB_FEC_FORMAT_C) {
    entry_format = 5;
    soc_mem_field32_set(unit, SOC_IS_ARADPLUS_AND_BELOW(unit)?IHB_FEC_ENTRY_FORMAT_Cm:PPDB_A_FEC_ENTRY_FORMAT_Cm, data, EEIf, tbl_data->eei);
    soc_mem_field32_set(unit, SOC_IS_ARADPLUS_AND_BELOW(unit)?IHB_FEC_ENTRY_FORMAT_Cm:PPDB_A_FEC_ENTRY_FORMAT_Cm, data, RESERVEDf, 0);
    soc_mem_field32_set(unit, SOC_IS_ARADPLUS_AND_BELOW(unit)?IHB_FEC_ENTRY_FORMAT_Cm:PPDB_A_FEC_ENTRY_FORMAT_Cm, data, IDENTIFIERf, entry_format);
  }
  
  if (SOC_IS_JERICHO(unit)) {
      soc_reg_above_64_val_t reg_above_64;
      SOC_REG_ABOVE_64_CLEAR(reg_above_64);

      res = READ_PPDB_A_FEC_SUPER_ENTRY_BANKm(
              unit,
              JERICHO_PP_TBL_FEC_ARRAY_INDEX(unit, entry_offset),
              MEM_BLOCK_ANY,
              JERICHO_PP_TBL_FEC_ENTRY_INDEX(unit, entry_offset),
              reg_above_64
              );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

      soc_mem_field_set(unit, PPDB_A_FEC_SUPER_ENTRY_BANKm, reg_above_64, 
                        JERICHO_PP_TBL_FEC_ENTRY_OFFSET(unit, entry_offset) == 0 ? DATA_0f : DATA_1f, data);
      
      res = WRITE_PPDB_A_FEC_SUPER_ENTRY_BANKm(
              unit,
              JERICHO_PP_TBL_FEC_ARRAY_INDEX(unit, entry_offset),
              MEM_BLOCK_ANY,
              JERICHO_PP_TBL_FEC_ENTRY_INDEX(unit, entry_offset),
              reg_above_64
              );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 31, exit);
  } else {
      res = WRITE_IHB_FEC_ENTRY_GENERALm(
         unit,
         ARAD_PP_TBL_FEC_ARRAY_INDEX(entry_offset),
         MEM_BLOCK_ANY,
         ARAD_PP_TBL_FEC_ENTRY_INDEX(entry_offset),
         data
         );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 31, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_fec_entry_general_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHB_DESTINATION_STATUS_TBL_ENTRY_SIZE 1

uint32
  arad_pp_ihb_destination_status_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHB_DESTINATION_STATUS_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHB_DESTINATION_STATUS_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_DESTINATION_STATUS_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHB_DESTINATION_STATUS_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHB_DESTINATION_STATUS_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHB_DESTINATION_STATUSm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->destination_valid[0] = soc_mem_field32_get(unit, IHB_DESTINATION_STATUSm, data, DESTINATION_VALID_0f);
  tbl_data->destination_valid[1] = soc_mem_field32_get(unit, IHB_DESTINATION_STATUSm, data, DESTINATION_VALID_1f);
  tbl_data->destination_valid[2] = soc_mem_field32_get(unit, IHB_DESTINATION_STATUSm, data, DESTINATION_VALID_2f);
  tbl_data->destination_valid[3] = soc_mem_field32_get(unit, IHB_DESTINATION_STATUSm, data, DESTINATION_VALID_3f);
  tbl_data->destination_valid[4] = soc_mem_field32_get(unit, IHB_DESTINATION_STATUSm, data, DESTINATION_VALID_4f);
  tbl_data->destination_valid[5] = soc_mem_field32_get(unit, IHB_DESTINATION_STATUSm, data, DESTINATION_VALID_5f);
  tbl_data->destination_valid[6] = soc_mem_field32_get(unit, IHB_DESTINATION_STATUSm, data, DESTINATION_VALID_6f);
  tbl_data->destination_valid[7] = soc_mem_field32_get(unit, IHB_DESTINATION_STATUSm, data, DESTINATION_VALID_7f);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_destination_status_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihb_destination_status_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHB_DESTINATION_STATUS_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHB_DESTINATION_STATUS_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_DESTINATION_STATUS_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHB_DESTINATION_STATUS_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHB_DESTINATION_STATUSm, data, DESTINATION_VALID_0f, tbl_data->destination_valid[0]);
  soc_mem_field32_set(unit, IHB_DESTINATION_STATUSm, data, DESTINATION_VALID_1f, tbl_data->destination_valid[1]);
  soc_mem_field32_set(unit, IHB_DESTINATION_STATUSm, data, DESTINATION_VALID_2f, tbl_data->destination_valid[2]);
  soc_mem_field32_set(unit, IHB_DESTINATION_STATUSm, data, DESTINATION_VALID_3f, tbl_data->destination_valid[3]);
  soc_mem_field32_set(unit, IHB_DESTINATION_STATUSm, data, DESTINATION_VALID_4f, tbl_data->destination_valid[4]);
  soc_mem_field32_set(unit, IHB_DESTINATION_STATUSm, data, DESTINATION_VALID_5f, tbl_data->destination_valid[5]);
  soc_mem_field32_set(unit, IHB_DESTINATION_STATUSm, data, DESTINATION_VALID_6f, tbl_data->destination_valid[6]);
  soc_mem_field32_set(unit, IHB_DESTINATION_STATUSm, data, DESTINATION_VALID_7f, tbl_data->destination_valid[7]);

  res = soc_mem_write(
          unit,
          IHB_DESTINATION_STATUSm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_destination_status_tbl_set_unsafe()", entry_offset, 0);
}


uint32
  arad_pp_ihb_destination_status_tbl_fill_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  ARAD_PP_IHB_DESTINATION_STATUS_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHB_DESTINATION_STATUS_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_DESTINATION_STATUS_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHB_DESTINATION_STATUS_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHB_DESTINATION_STATUSm, data, DESTINATION_VALID_0f, tbl_data->destination_valid[0]);
  soc_mem_field32_set(unit, IHB_DESTINATION_STATUSm, data, DESTINATION_VALID_1f, tbl_data->destination_valid[1]);
  soc_mem_field32_set(unit, IHB_DESTINATION_STATUSm, data, DESTINATION_VALID_2f, tbl_data->destination_valid[2]);
  soc_mem_field32_set(unit, IHB_DESTINATION_STATUSm, data, DESTINATION_VALID_3f, tbl_data->destination_valid[3]);
  soc_mem_field32_set(unit, IHB_DESTINATION_STATUSm, data, DESTINATION_VALID_4f, tbl_data->destination_valid[4]);
  soc_mem_field32_set(unit, IHB_DESTINATION_STATUSm, data, DESTINATION_VALID_5f, tbl_data->destination_valid[5]);
  soc_mem_field32_set(unit, IHB_DESTINATION_STATUSm, data, DESTINATION_VALID_6f, tbl_data->destination_valid[6]);
  soc_mem_field32_set(unit, IHB_DESTINATION_STATUSm, data, DESTINATION_VALID_7f, tbl_data->destination_valid[7]);

  res = arad_fill_table_with_entry(unit, IHB_DESTINATION_STATUSm, MEM_BLOCK_ANY, data); /* fill memory with the entry in data */
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_destination_status_tbl_set_unsafe()", 0, 0);
}

#define ARAD_PP_IHB_FWD_ACT_PROFILE_TBL_ENTRY_SIZE 5

uint32
  arad_pp_ihb_fwd_act_profile_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHB_FWD_ACT_PROFILE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHB_FWD_ACT_PROFILE_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_FWD_ACT_PROFILE_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHB_FWD_ACT_PROFILE_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHB_FWD_ACT_PROFILE_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHB_FWD_ACT_PROFILEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->fwd_act_trap = soc_mem_field32_get(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_TRAPf);
  tbl_data->fwd_act_destination = soc_mem_field32_get(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_DESTINATIONf);
  tbl_data->fwd_act_destination_overwrite = soc_mem_field32_get(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_DESTINATION_OVERWRITEf);
  tbl_data->fwd_act_out_lif = soc_mem_field32_get(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_OUT_LIFf);
  tbl_data->fwd_act_out_lif_overwrite = soc_mem_field32_get(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_OUT_LIF_OVERWRITEf);
  tbl_data->fwd_act_eei = soc_mem_field32_get(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_EEIf);
  tbl_data->fwd_act_eei_overwrite = soc_mem_field32_get(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_EEI_OVERWRITEf);
  tbl_data->fwd_act_destination_add_vsi = soc_mem_field32_get(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_DESTINATION_ADD_VSIf);
  tbl_data->fwd_act_destination_add_vsi_shift = soc_mem_field32_get(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_DESTINATION_ADD_VSI_SHIFTf);
  tbl_data->fwd_act_traffic_class = soc_mem_field32_get(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_TRAFFIC_CLASSf);
  tbl_data->fwd_act_traffic_class_overwrite = soc_mem_field32_get(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_TRAFFIC_CLASS_OVERWRITEf);
  tbl_data->fwd_act_drop_precedence = soc_mem_field32_get(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_DROP_PRECEDENCEf);
  tbl_data->fwd_act_drop_precedence_overwrite = soc_mem_field32_get(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_DROP_PRECEDENCE_OVERWRITEf);
  tbl_data->fwd_act_meter_pointer_a_overwrite = soc_mem_field32_get(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_METER_POINTER_A_OVERWRITEf);
  tbl_data->fwd_act_meter_pointer_b_overwrite = soc_mem_field32_get(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_METER_POINTER_B_OVERWRITEf);
  tbl_data->fwd_act_meter_pointer = soc_mem_field32_get(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_METER_POINTERf);
  tbl_data->fwd_act_meter_pointer_update = soc_mem_field32_get(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_METER_POINTER_UPDATEf);
  tbl_data->fwd_act_counter_pointer = soc_mem_field32_get(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_COUNTER_POINTERf);
  tbl_data->fwd_act_counter_pointer_a_overwrite = soc_mem_field32_get(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_COUNTER_POINTER_A_OVERWRITEf);
  tbl_data->fwd_act_counter_pointer_b_overwrite = soc_mem_field32_get(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_COUNTER_POINTER_B_OVERWRITEf);
  tbl_data->fwd_act_counter_pointer_update = soc_mem_field32_get(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_COUNTER_POINTER_UPDATEf);
  tbl_data->fwd_act_drop_precedence_meter_command = soc_mem_field32_get(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_DROP_PRECEDENCE_METER_COMMANDf);
  tbl_data->fwd_act_drop_precedence_meter_command_overwrite = soc_mem_field32_get(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_DROP_PRECEDENCE_METER_COMMAND_OVERWRITEf);
  tbl_data->fwd_act_forwarding_code = soc_mem_field32_get(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_FORWARDING_CODEf);
  tbl_data->fwd_act_forwarding_code_overwrite = soc_mem_field32_get(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_FORWARDING_CODE_OVERWRITEf);
  tbl_data->fwd_act_fwd_offset_index = soc_mem_field32_get(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_FWD_OFFSET_INDEXf);
  tbl_data->fwd_act_fwd_offset_fix = soc_mem_field32_get(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_FWD_OFFSET_FIXf);
  tbl_data->fwd_act_fwd_offset_overwrite = soc_mem_field32_get(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_FWD_OFFSET_OVERWRITEf);
  tbl_data->fwd_act_ethernet_policer_pointer = soc_mem_field32_get(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_ETHERNET_POLICER_POINTERf);
  tbl_data->fwd_act_ethernet_policer_pointer_overwrite = soc_mem_field32_get(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_ETHERNET_POLICER_POINTER_OVERWRITEf);
  tbl_data->fwd_act_learn_disable = soc_mem_field32_get(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_LEARN_DISABLEf);
  tbl_data->fwd_act_bypass_filtering = soc_mem_field32_get(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_BYPASS_FILTERINGf);
  tbl_data->fwd_act_da_type_overwrite = soc_mem_field32_get(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_DA_TYPE_OVERWRITEf);
  tbl_data->fwd_act_da_type = soc_mem_field32_get(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_DA_TYPEf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_fwd_act_profile_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihb_fwd_act_profile_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHB_FWD_ACT_PROFILE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHB_FWD_ACT_PROFILE_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_FWD_ACT_PROFILE_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHB_FWD_ACT_PROFILE_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_TRAPf, tbl_data->fwd_act_trap);
  soc_mem_field32_set(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_DESTINATIONf, tbl_data->fwd_act_destination);
  soc_mem_field32_set(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_DESTINATION_OVERWRITEf, tbl_data->fwd_act_destination_overwrite);
  soc_mem_field32_set(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_OUT_LIFf, tbl_data->fwd_act_out_lif);
  soc_mem_field32_set(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_OUT_LIF_OVERWRITEf, tbl_data->fwd_act_out_lif_overwrite);
  soc_mem_field32_set(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_EEIf, tbl_data->fwd_act_eei);
  soc_mem_field32_set(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_EEI_OVERWRITEf, tbl_data->fwd_act_eei_overwrite);
  soc_mem_field32_set(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_DESTINATION_ADD_VSIf, tbl_data->fwd_act_destination_add_vsi);
  soc_mem_field32_set(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_DESTINATION_ADD_VSI_SHIFTf, tbl_data->fwd_act_destination_add_vsi_shift);
  soc_mem_field32_set(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_TRAFFIC_CLASSf, tbl_data->fwd_act_traffic_class);
  soc_mem_field32_set(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_TRAFFIC_CLASS_OVERWRITEf, tbl_data->fwd_act_traffic_class_overwrite);
  soc_mem_field32_set(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_DROP_PRECEDENCEf, tbl_data->fwd_act_drop_precedence);
  soc_mem_field32_set(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_DROP_PRECEDENCE_OVERWRITEf, tbl_data->fwd_act_drop_precedence_overwrite);
  soc_mem_field32_set(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_METER_POINTER_A_OVERWRITEf, tbl_data->fwd_act_meter_pointer_a_overwrite);
  soc_mem_field32_set(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_METER_POINTER_B_OVERWRITEf, tbl_data->fwd_act_meter_pointer_b_overwrite);
  soc_mem_field32_set(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_METER_POINTERf, tbl_data->fwd_act_meter_pointer);
  soc_mem_field32_set(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_METER_POINTER_UPDATEf, tbl_data->fwd_act_meter_pointer_update);
  soc_mem_field32_set(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_COUNTER_POINTERf, tbl_data->fwd_act_counter_pointer);
  soc_mem_field32_set(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_COUNTER_POINTER_A_OVERWRITEf, tbl_data->fwd_act_counter_pointer_a_overwrite);
  soc_mem_field32_set(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_COUNTER_POINTER_B_OVERWRITEf, tbl_data->fwd_act_counter_pointer_b_overwrite);
  soc_mem_field32_set(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_COUNTER_POINTER_UPDATEf, tbl_data->fwd_act_counter_pointer_update);
  soc_mem_field32_set(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_DROP_PRECEDENCE_METER_COMMANDf, tbl_data->fwd_act_drop_precedence_meter_command);
  soc_mem_field32_set(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_DROP_PRECEDENCE_METER_COMMAND_OVERWRITEf, tbl_data->fwd_act_drop_precedence_meter_command_overwrite);
  soc_mem_field32_set(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_FORWARDING_CODEf, tbl_data->fwd_act_forwarding_code);
  soc_mem_field32_set(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_FORWARDING_CODE_OVERWRITEf, tbl_data->fwd_act_forwarding_code_overwrite);
  soc_mem_field32_set(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_FWD_OFFSET_INDEXf, tbl_data->fwd_act_fwd_offset_index);
  soc_mem_field32_set(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_FWD_OFFSET_FIXf, tbl_data->fwd_act_fwd_offset_fix);
  soc_mem_field32_set(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_FWD_OFFSET_OVERWRITEf, tbl_data->fwd_act_fwd_offset_overwrite);
  soc_mem_field32_set(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_ETHERNET_POLICER_POINTERf, tbl_data->fwd_act_ethernet_policer_pointer);
  soc_mem_field32_set(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_ETHERNET_POLICER_POINTER_OVERWRITEf, tbl_data->fwd_act_ethernet_policer_pointer_overwrite);
  soc_mem_field32_set(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_LEARN_DISABLEf, tbl_data->fwd_act_learn_disable);
  soc_mem_field32_set(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_BYPASS_FILTERINGf, tbl_data->fwd_act_bypass_filtering);
  soc_mem_field32_set(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_DA_TYPE_OVERWRITEf, tbl_data->fwd_act_da_type_overwrite);
  soc_mem_field32_set(unit, IHB_FWD_ACT_PROFILEm, data, FWD_ACT_DA_TYPEf, tbl_data->fwd_act_da_type);

  res = soc_mem_write(
          unit,
          IHB_FWD_ACT_PROFILEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_fwd_act_profile_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHB_SNOOP_ACTION_TBL_ENTRY_SIZE 1

uint32
  arad_pp_ihb_snoop_action_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHB_SNOOP_ACTION_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHB_SNOOP_ACTION_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_SNOOP_ACTION_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHB_SNOOP_ACTION_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHB_SNOOP_ACTION_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHB_SNOOP_ACTIONm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->snoop_action = soc_mem_field32_get(unit, IHB_SNOOP_ACTIONm, data, SNOOP_ACTIONf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_snoop_action_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihb_snoop_action_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHB_SNOOP_ACTION_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHB_SNOOP_ACTION_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_SNOOP_ACTION_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHB_SNOOP_ACTION_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHB_SNOOP_ACTIONm, data, SNOOP_ACTIONf, tbl_data->snoop_action);

  res = soc_mem_write(
          unit,
          IHB_SNOOP_ACTIONm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_snoop_action_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHB_PINFO_FLP_TBL_ENTRY_SIZE 1

uint32
  arad_pp_ihb_pinfo_flp_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHB_PINFO_FLP_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHB_PINFO_FLP_TBL_ENTRY_SIZE];
  soc_mem_t
    pinfo_flp_mem;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_PINFO_FLP_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHB_PINFO_FLP_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHB_PINFO_FLP_TBL_DATA, 1);

  if (SOC_IS_JERICHO(unit)) {
      pinfo_flp_mem = IHP_PINFO_FLP_0m;
  } else {
      pinfo_flp_mem = IHB_PINFO_FLPm;
  }
  res = soc_mem_read(
          unit,
          pinfo_flp_mem,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->program_selection_profile = soc_mem_field32_get(unit, pinfo_flp_mem, data, PROGRAM_SELECTION_PROFILEf);
  tbl_data->enable_pp_inject = soc_mem_field32_get(unit, pinfo_flp_mem, data, ENABLE_PP_INJECTf);
  tbl_data->key_gen_var = soc_mem_field32_get(unit, pinfo_flp_mem, data, KEY_GEN_VARf);

  if (SOC_IS_JERICHO(unit)) {
      pinfo_flp_mem = IHP_PINFO_FLP_1m;
      ARAD_CLEAR(data, uint32, ARAD_PP_IHB_PINFO_FLP_TBL_ENTRY_SIZE);
      res = soc_mem_read(
              unit,
              pinfo_flp_mem,
              MEM_BLOCK_ANY,
              entry_offset,
              data
            );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
  }

  tbl_data->action_profile_sa_drop_index = soc_mem_field32_get(unit, pinfo_flp_mem, data, ACTION_PROFILE_SA_DROP_INDEXf);
  tbl_data->action_profile_sa_not_found_index = soc_mem_field32_get(unit, pinfo_flp_mem, data, ACTION_PROFILE_SA_NOT_FOUND_INDEXf);
  tbl_data->action_profile_da_not_found_index = soc_mem_field32_get(unit, pinfo_flp_mem, data, ACTION_PROFILE_DA_NOT_FOUND_INDEXf);
  tbl_data->enable_unicast_same_interface_filter = soc_mem_field32_get(unit, pinfo_flp_mem, data, ENABLE_UNICAST_SAME_INTERFACE_FILTERf);
  tbl_data->learn_enable = soc_mem_field32_get(unit, pinfo_flp_mem, data, LEARN_ENABLEf);
  tbl_data->port_is_pbp = soc_mem_field32_get(unit, pinfo_flp_mem, data, PORT_IS_PBPf);
  tbl_data->ieee_1588_profile = soc_mem_field32_get(unit, pinfo_flp_mem, data, IEEE_1588_PROFILEf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_pinfo_flp_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihb_pinfo_flp_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHB_PINFO_FLP_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHB_PINFO_FLP_TBL_ENTRY_SIZE];
  soc_mem_t
    pinfo_flp_mem;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_PINFO_FLP_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHB_PINFO_FLP_TBL_ENTRY_SIZE);
  if (SOC_IS_JERICHO(unit)) {
      pinfo_flp_mem = IHP_PINFO_FLP_0m;
  } else {
      pinfo_flp_mem = IHB_PINFO_FLPm;
  }

  soc_mem_field32_set(unit, pinfo_flp_mem, data, PROGRAM_SELECTION_PROFILEf, tbl_data->program_selection_profile);
  soc_mem_field32_set(unit, pinfo_flp_mem, data, ENABLE_PP_INJECTf, tbl_data->enable_pp_inject);
  soc_mem_field32_set(unit, pinfo_flp_mem, data, KEY_GEN_VARf, tbl_data->key_gen_var);

  if (SOC_IS_JERICHO(unit)) {
      res = soc_mem_write(
              unit,
              pinfo_flp_mem,
              MEM_BLOCK_ANY,
              entry_offset,
              data
            );
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
      pinfo_flp_mem = IHP_PINFO_FLP_1m;
      ARAD_CLEAR(data, uint32, ARAD_PP_IHB_PINFO_FLP_TBL_ENTRY_SIZE);
  }

  soc_mem_field32_set(unit, pinfo_flp_mem, data, ACTION_PROFILE_SA_DROP_INDEXf, tbl_data->action_profile_sa_drop_index);
  soc_mem_field32_set(unit, pinfo_flp_mem, data, ACTION_PROFILE_SA_NOT_FOUND_INDEXf, tbl_data->action_profile_sa_not_found_index);
  soc_mem_field32_set(unit, pinfo_flp_mem, data, ACTION_PROFILE_DA_NOT_FOUND_INDEXf, tbl_data->action_profile_da_not_found_index);
  soc_mem_field32_set(unit, pinfo_flp_mem, data, ENABLE_UNICAST_SAME_INTERFACE_FILTERf, tbl_data->enable_unicast_same_interface_filter);
  soc_mem_field32_set(unit, pinfo_flp_mem, data, LEARN_ENABLEf, tbl_data->learn_enable);
  soc_mem_field32_set(unit, pinfo_flp_mem, data, PORT_IS_PBPf, tbl_data->port_is_pbp);
  soc_mem_field32_set(unit, pinfo_flp_mem, data, IEEE_1588_PROFILEf, tbl_data->ieee_1588_profile);

  res = soc_mem_write(
          unit,
          pinfo_flp_mem,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_pinfo_flp_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHB_VRF_CONFIG_TBL_ENTRY_SIZE 2

uint32
  arad_pp_ihb_vrf_config_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHB_VRF_CONFIG_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHB_VRF_CONFIG_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_VRF_CONFIG_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHB_VRF_CONFIG_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHB_VRF_CONFIG_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHB_VRF_CONFIGm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->default_action_uc = soc_mem_field32_get(unit, IHB_VRF_CONFIGm, data, DEFAULT_ACTION_UCf);
  tbl_data->default_action_mc = soc_mem_field32_get(unit, IHB_VRF_CONFIGm, data, DEFAULT_ACTION_MCf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_vrf_config_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihb_vrf_config_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHB_VRF_CONFIG_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHB_VRF_CONFIG_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_VRF_CONFIG_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHB_VRF_CONFIG_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHB_VRF_CONFIGm, data, DEFAULT_ACTION_UCf, tbl_data->default_action_uc);
  soc_mem_field32_set(unit, IHB_VRF_CONFIGm, data, DEFAULT_ACTION_MCf, tbl_data->default_action_mc);

  res = soc_mem_write(
          unit,
          IHB_VRF_CONFIGm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_vrf_config_tbl_set_unsafe()", entry_offset, 0);
}


uint32
  arad_pp_ihb_header_profile_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHB_HEADER_PROFILE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_HEADER_PROFILE_TBL_GET_UNSAFE);

  ARAD_CLEAR(tbl_data, ARAD_PP_IHB_HEADER_PROFILE_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHB_HEADER_PROFILEm,
          MEM_BLOCK_ANY,
          entry_offset,
          &data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->st_vsq_ptr_tc_mode = soc_IHB_HEADER_PROFILEm_field32_get(unit, &data, ST_VSQ_PTR_TC_MODEf);
  tbl_data->build_ftmh = soc_IHB_HEADER_PROFILEm_field32_get(unit, &data, BUILD_FTMHf);
  tbl_data->build_pph = soc_IHB_HEADER_PROFILEm_field32_get(unit, &data, BUILD_PPHf);
  tbl_data->always_add_pph_learn_ext = soc_IHB_HEADER_PROFILEm_field32_get(unit, &data, ALWAYS_ADD_PPH_LEARN_EXTf);
  tbl_data->never_add_pph_learn_ext = soc_IHB_HEADER_PROFILEm_field32_get(unit, &data, NEVER_ADD_PPH_LEARN_EXTf);
  tbl_data->enable_stacking_uc = soc_IHB_HEADER_PROFILEm_field32_get(unit, &data, ENABLE_STACKING_UCf);
  tbl_data->enable_stacking_mc = soc_IHB_HEADER_PROFILEm_field32_get(unit, &data, ENABLE_STACKING_MCf);
  tbl_data->user_header_1_size = soc_IHB_HEADER_PROFILEm_field32_get(unit, &data, USER_HEADER_1_SIZEf);
  tbl_data->user_header_2_size = soc_IHB_HEADER_PROFILEm_field32_get(unit, &data, USER_HEADER_2_SIZEf);
  tbl_data->mc_id_in_user_header_2 = soc_IHB_HEADER_PROFILEm_field32_get(unit, &data, MC_ID_IN_USER_HEADER_2f);
  tbl_data->out_lif_in_user_header_2 = soc_IHB_HEADER_PROFILEm_field32_get(unit, &data, OUT_LIF_IN_USER_HEADER_2f);
  tbl_data->ftmh_reserved_value = soc_IHB_HEADER_PROFILEm_field32_get(unit, &data, FTMH_RESERVED_VALUEf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_header_profile_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihb_header_profile_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHB_HEADER_PROFILE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data = 0;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_HEADER_PROFILE_TBL_SET_UNSAFE);

  soc_IHB_HEADER_PROFILEm_field32_set(unit, &data, ST_VSQ_PTR_TC_MODEf, tbl_data->st_vsq_ptr_tc_mode);
  soc_IHB_HEADER_PROFILEm_field32_set(unit, &data, BUILD_FTMHf, tbl_data->build_ftmh);
  soc_IHB_HEADER_PROFILEm_field32_set(unit, &data, BUILD_PPHf, tbl_data->build_pph);
  soc_IHB_HEADER_PROFILEm_field32_set(unit, &data, ALWAYS_ADD_PPH_LEARN_EXTf, tbl_data->always_add_pph_learn_ext);
  soc_IHB_HEADER_PROFILEm_field32_set(unit, &data, NEVER_ADD_PPH_LEARN_EXTf, tbl_data->never_add_pph_learn_ext);
  soc_IHB_HEADER_PROFILEm_field32_set(unit, &data, ENABLE_STACKING_UCf, tbl_data->enable_stacking_uc);
  soc_IHB_HEADER_PROFILEm_field32_set(unit, &data, ENABLE_STACKING_MCf, tbl_data->enable_stacking_mc);
  soc_IHB_HEADER_PROFILEm_field32_set(unit, &data, USER_HEADER_1_SIZEf, tbl_data->user_header_1_size);
  soc_IHB_HEADER_PROFILEm_field32_set(unit, &data, USER_HEADER_2_SIZEf, tbl_data->user_header_2_size);
  soc_IHB_HEADER_PROFILEm_field32_set(unit, &data, MC_ID_IN_USER_HEADER_2f, tbl_data->mc_id_in_user_header_2);
  soc_IHB_HEADER_PROFILEm_field32_set(unit, &data, OUT_LIF_IN_USER_HEADER_2f, tbl_data->out_lif_in_user_header_2);
  soc_IHB_HEADER_PROFILEm_field32_set(unit, &data, FTMH_RESERVED_VALUEf, tbl_data->ftmh_reserved_value);

  res = soc_mem_write(
          unit,
          IHB_HEADER_PROFILEm,
          MEM_BLOCK_ANY,
          entry_offset,
          &data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_header_profile_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHB_SNP_ACT_PROFILE_TBL_ENTRY_SIZE 1

uint32
  arad_pp_ihb_snp_act_profile_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHB_SNP_ACT_PROFILE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHB_SNP_ACT_PROFILE_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_SNP_ACT_PROFILE_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHB_SNP_ACT_PROFILE_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHB_SNP_ACT_PROFILE_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHB_SNP_ACT_PROFILEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->sampling_probability = soc_mem_field32_get(unit, IHB_SNP_ACT_PROFILEm, data, SAMPLING_PROBABILITYf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_snp_act_profile_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihb_snp_act_profile_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHB_SNP_ACT_PROFILE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHB_SNP_ACT_PROFILE_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_SNP_ACT_PROFILE_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHB_SNP_ACT_PROFILE_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHB_SNP_ACT_PROFILEm, data, SAMPLING_PROBABILITYf, tbl_data->sampling_probability);

  res = soc_mem_write(
          unit,
          IHB_SNP_ACT_PROFILEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_snp_act_profile_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHB_MRR_ACT_PROFILE_TBL_ENTRY_SIZE 1

uint32
  arad_pp_ihb_mrr_act_profile_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHB_MRR_ACT_PROFILE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHB_MRR_ACT_PROFILE_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_MRR_ACT_PROFILE_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHB_MRR_ACT_PROFILE_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHB_MRR_ACT_PROFILE_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHB_MRR_ACT_PROFILEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->sampling_probability = soc_mem_field32_get(unit, IHB_MRR_ACT_PROFILEm, data, SAMPLING_PROBABILITYf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_mrr_act_profile_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihb_mrr_act_profile_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHB_MRR_ACT_PROFILE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHB_MRR_ACT_PROFILE_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHB_MRR_ACT_PROFILE_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHB_MRR_ACT_PROFILE_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHB_MRR_ACT_PROFILEm, data, SAMPLING_PROBABILITYf, tbl_data->sampling_probability);

  res = soc_mem_write(
          unit,
          IHB_MRR_ACT_PROFILEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihb_mrr_act_profile_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_EGQ_PP_PPCT_TBL_ENTRY_SIZE 3

uint32
  arad_pp_egq_pp_ppct_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_EGQ_PP_PPCT_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EGQ_PP_PPCT_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EGQ_PP_PPCT_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EGQ_PP_PPCT_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_EGQ_PP_PPCT_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          EGQ_PACKET_PROCESSING_PER_PORT_CONFIGURATION_TABLE_PP_PPCTm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->pvlan_port_type = soc_mem_field32_get(unit, EGQ_PACKET_PROCESSING_PER_PORT_CONFIGURATION_TABLE_PP_PPCTm, data, PVLAN_PORT_TYPEf);
  tbl_data->unknown_bc_da_action_filter = soc_mem_field32_get(unit, EGQ_PACKET_PROCESSING_PER_PORT_CONFIGURATION_TABLE_PP_PPCTm, data, UNKNOWN_BC_DA_ACTION_FILTERf);
  tbl_data->unknown_mc_da_action_filter = soc_mem_field32_get(unit, EGQ_PACKET_PROCESSING_PER_PORT_CONFIGURATION_TABLE_PP_PPCTm, data, UNKNOWN_MC_DA_ACTION_FILTERf);
  tbl_data->unknown_uc_da_action_filter = soc_mem_field32_get(unit, EGQ_PACKET_PROCESSING_PER_PORT_CONFIGURATION_TABLE_PP_PPCTm, data, UNKNOWN_UC_DA_ACTION_FILTERf);
  tbl_data->acceptable_frame_type_profile = soc_mem_field32_get(unit, EGQ_PACKET_PROCESSING_PER_PORT_CONFIGURATION_TABLE_PP_PPCTm, data, ACCEPTABLE_FRAME_TYPE_PROFILEf);
  tbl_data->egress_vsi_filtering_enable = soc_mem_field32_get(unit, EGQ_PACKET_PROCESSING_PER_PORT_CONFIGURATION_TABLE_PP_PPCTm, data, EGRESS_VSI_FILTERING_ENABLEf);
  tbl_data->disable_filtering = soc_mem_field32_get(unit, EGQ_PACKET_PROCESSING_PER_PORT_CONFIGURATION_TABLE_PP_PPCTm, data, DISABLE_FILTERINGf);
  tbl_data->port_is_pbp = soc_mem_field32_get(unit, EGQ_PACKET_PROCESSING_PER_PORT_CONFIGURATION_TABLE_PP_PPCTm, data, PORT_IS_PBPf);
  tbl_data->pmf_profile = soc_mem_field32_get(unit, EGQ_PACKET_PROCESSING_PER_PORT_CONFIGURATION_TABLE_PP_PPCTm, data, PMF_PROFILEf);
  tbl_data->orientation = soc_mem_field32_get(unit, EGQ_PACKET_PROCESSING_PER_PORT_CONFIGURATION_TABLE_PP_PPCTm, data, ORIENTATIONf);
  tbl_data->mtu = soc_mem_field32_get(unit, EGQ_PACKET_PROCESSING_PER_PORT_CONFIGURATION_TABLE_PP_PPCTm, data, MTUf);
  tbl_data->pmf_data = soc_mem_field32_get(unit, EGQ_PACKET_PROCESSING_PER_PORT_CONFIGURATION_TABLE_PP_PPCTm, data, PMF_DATAf);
 /* tbl_data->enable_same_interface_filters_lifs = soc_mem_field32_get(unit, EGQ_PACKET_PROCESSING_PER_PORT_CONFIGURATION_TABLE_PP_PPCTm, data, ENABLE_SAME_INTERFACE_FILTERS_LIFSf); */
  tbl_data->enable_same_interface_filters_ports = soc_mem_field32_get(unit, EGQ_PACKET_PROCESSING_PER_PORT_CONFIGURATION_TABLE_PP_PPCTm, data, ENABLE_MULTICAST_SAME_INTERFACE_FILTERSf);
  tbl_data->enable_unicast_same_interface_filter = soc_mem_field32_get(unit, EGQ_PACKET_PROCESSING_PER_PORT_CONFIGURATION_TABLE_PP_PPCTm, data, ENABLE_UNICAST_SAME_INTERFACE_FILTERf);
  tbl_data->counter_profile = soc_mem_field32_get(unit, EGQ_PACKET_PROCESSING_PER_PORT_CONFIGURATION_TABLE_PP_PPCTm, data, COUNTER_PROFILEf);
  tbl_data->counter_compensation = soc_mem_field32_get(unit, EGQ_PACKET_PROCESSING_PER_PORT_CONFIGURATION_TABLE_PP_PPCTm, data, COUNTER_COMPENSATIONf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_egq_pp_ppct_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_egq_pp_ppct_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_EGQ_PP_PPCT_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EGQ_PP_PPCT_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EGQ_PP_PPCT_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EGQ_PP_PPCT_TBL_ENTRY_SIZE);


  soc_mem_field32_set(unit, EGQ_PACKET_PROCESSING_PER_PORT_CONFIGURATION_TABLE_PP_PPCTm, data, PVLAN_PORT_TYPEf, tbl_data->pvlan_port_type);
  soc_mem_field32_set(unit, EGQ_PACKET_PROCESSING_PER_PORT_CONFIGURATION_TABLE_PP_PPCTm, data, UNKNOWN_BC_DA_ACTION_FILTERf, tbl_data->unknown_bc_da_action_filter);
  soc_mem_field32_set(unit, EGQ_PACKET_PROCESSING_PER_PORT_CONFIGURATION_TABLE_PP_PPCTm, data, UNKNOWN_MC_DA_ACTION_FILTERf, tbl_data->unknown_mc_da_action_filter);
  soc_mem_field32_set(unit, EGQ_PACKET_PROCESSING_PER_PORT_CONFIGURATION_TABLE_PP_PPCTm, data, UNKNOWN_UC_DA_ACTION_FILTERf, tbl_data->unknown_uc_da_action_filter);
  soc_mem_field32_set(unit, EGQ_PACKET_PROCESSING_PER_PORT_CONFIGURATION_TABLE_PP_PPCTm, data, ACCEPTABLE_FRAME_TYPE_PROFILEf, tbl_data->acceptable_frame_type_profile);
  soc_mem_field32_set(unit, EGQ_PACKET_PROCESSING_PER_PORT_CONFIGURATION_TABLE_PP_PPCTm, data, EGRESS_VSI_FILTERING_ENABLEf, tbl_data->egress_vsi_filtering_enable);
  soc_mem_field32_set(unit, EGQ_PACKET_PROCESSING_PER_PORT_CONFIGURATION_TABLE_PP_PPCTm, data, DISABLE_FILTERINGf, tbl_data->disable_filtering);
  soc_mem_field32_set(unit, EGQ_PACKET_PROCESSING_PER_PORT_CONFIGURATION_TABLE_PP_PPCTm, data, PORT_IS_PBPf, tbl_data->port_is_pbp);
  soc_mem_field32_set(unit, EGQ_PACKET_PROCESSING_PER_PORT_CONFIGURATION_TABLE_PP_PPCTm, data, PMF_PROFILEf, tbl_data->pmf_profile);
  soc_mem_field32_set(unit, EGQ_PACKET_PROCESSING_PER_PORT_CONFIGURATION_TABLE_PP_PPCTm, data, ORIENTATIONf, tbl_data->orientation);
  soc_mem_field32_set(unit, EGQ_PACKET_PROCESSING_PER_PORT_CONFIGURATION_TABLE_PP_PPCTm, data, MTUf, tbl_data->mtu);
  soc_mem_field32_set(unit, EGQ_PACKET_PROCESSING_PER_PORT_CONFIGURATION_TABLE_PP_PPCTm, data, PMF_DATAf, tbl_data->pmf_data);
/*  soc_mem_field32_set(unit, EGQ_PACKET_PROCESSING_PER_PORT_CONFIGURATION_TABLE_PP_PPCTm, data, ENABLE_SAME_INTERFACE_FILTERS_LIFSf, tbl_data->enable_same_interface_filters_lifs); */
  soc_mem_field32_set(unit, EGQ_PACKET_PROCESSING_PER_PORT_CONFIGURATION_TABLE_PP_PPCTm, data, ENABLE_MULTICAST_SAME_INTERFACE_FILTERSf, tbl_data->enable_same_interface_filters_ports);
  soc_mem_field32_set(unit, EGQ_PACKET_PROCESSING_PER_PORT_CONFIGURATION_TABLE_PP_PPCTm, data, ENABLE_UNICAST_SAME_INTERFACE_FILTERf, tbl_data->enable_unicast_same_interface_filter);
  soc_mem_field32_set(unit, EGQ_PACKET_PROCESSING_PER_PORT_CONFIGURATION_TABLE_PP_PPCTm, data, COUNTER_PROFILEf, tbl_data->counter_profile);
  soc_mem_field32_set(unit, EGQ_PACKET_PROCESSING_PER_PORT_CONFIGURATION_TABLE_PP_PPCTm, data, COUNTER_COMPENSATIONf, tbl_data->counter_compensation);

  res = soc_mem_write(
          unit,
          EGQ_PACKET_PROCESSING_PER_PORT_CONFIGURATION_TABLE_PP_PPCTm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_egq_pp_ppct_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_egq_ttl_scope_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   vsi_ndx,
    SOC_SAND_OUT ARAD_PP_EGQ_TTL_SCOPE_TBL_DATA *tbl_data
  )
{
  uint32
      res = SOC_SAND_OK;
  uint64 data, field_64;
  uint32 vsi_offset = vsi_ndx & 0x7;
  soc_reg_above_64_val_t reg_above_64;  
  uint32 vsi_index;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EGQ_TTL_SCOPE_TBL_GET_UNSAFE);
  
  tbl_data->ttl_scope_index = 0;

  COMPILER_64_ZERO(field_64);

  if (SOC_IS_JERICHO(unit)) {
      vsi_index = vsi_ndx / 16;
  } else {
      vsi_index = vsi_ndx / 8;
  }
  

  res = soc_mem_read(
          unit,
          EGQ_TTL_SCOPE_MEMORY_TTL_SCOPEm,
          MEM_BLOCK_ANY,
          vsi_index,
          &data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  soc_mem_field64_get(unit, EGQ_TTL_SCOPE_MEMORY_TTL_SCOPEm, &data, TTL_SCOPEf, &field_64);

  SOC_REG_ABOVE_64_CLEAR(reg_above_64);
  reg_above_64[0] = COMPILER_64_LO(field_64);
  reg_above_64[1] = COMPILER_64_HI(field_64);
  SHR_BITCOPY_RANGE(&tbl_data->ttl_scope_index,0, reg_above_64,  3 * vsi_offset, 3);  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_egq_ttl_scope_tbl_get_unsafe()", vsi_ndx, 0);
}

uint32
  arad_pp_egq_ttl_scope_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   vsi_ndx,
    SOC_SAND_IN  ARAD_PP_EGQ_TTL_SCOPE_TBL_DATA *tbl_data
  )
{
  uint64 field_64;
  uint32 res = SOC_SAND_OK;
  uint64 data;
  uint32 vsi_offset = vsi_ndx & 0x7;
  uint32 vsi_index;
  soc_reg_above_64_val_t reg_above_64;  
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EGQ_TTL_SCOPE_TBL_SET_UNSAFE);

  COMPILER_64_ZERO(field_64);

  if (SOC_IS_JERICHO(unit)) {
      vsi_index = vsi_ndx / 16;
  } else {
      vsi_index = vsi_ndx / 8;
  }

  res = soc_mem_read(
          unit,
          EGQ_TTL_SCOPE_MEMORY_TTL_SCOPEm,
          MEM_BLOCK_ANY,
          vsi_index,
          &data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  soc_mem_field64_get(unit, EGQ_TTL_SCOPE_MEMORY_TTL_SCOPEm, &data, TTL_SCOPEf, &field_64);
  SOC_REG_ABOVE_64_CLEAR(reg_above_64);
  reg_above_64[0] = COMPILER_64_LO(field_64);
  reg_above_64[1] = COMPILER_64_HI(field_64);

  SHR_BITCOPY_RANGE(reg_above_64, 3 * vsi_offset, &(tbl_data->ttl_scope_index),0, 3);
  COMPILER_64_SET(field_64, reg_above_64[1], reg_above_64[0]);
  soc_mem_field64_set(unit, EGQ_TTL_SCOPE_MEMORY_TTL_SCOPEm, &data, TTL_SCOPEf, field_64);

  res = soc_mem_write(
          unit,
          EGQ_TTL_SCOPE_MEMORY_TTL_SCOPEm,
          MEM_BLOCK_ANY,
          vsi_index,
          &data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_egq_ttl_scope_tbl_set_unsafe()", vsi_ndx, 0);
}

#define ARAD_PP_EGQ_AUX_TABLE_TBL_ENTRY_SIZE 2

uint32
  arad_pp_egq_aux_table_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_EGQ_AUX_TABLE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EGQ_AUX_TABLE_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EGQ_AUX_TABLE_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EGQ_AUX_TABLE_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_EGQ_AUX_TABLE_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          EGQ_AUXILIARY_DATA_MEMORY_AUX_TABLEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->aux_table_data = soc_mem_field32_get(unit, EGQ_AUXILIARY_DATA_MEMORY_AUX_TABLEm, data, AUX_TABLE_DATAf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_egq_aux_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_egq_aux_table_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_EGQ_AUX_TABLE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EGQ_AUX_TABLE_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EGQ_AUX_TABLE_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EGQ_AUX_TABLE_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, EGQ_AUXILIARY_DATA_MEMORY_AUX_TABLEm, data, AUX_TABLE_DATAf, tbl_data->aux_table_data);

  res = soc_mem_write(
          unit,
          EGQ_AUXILIARY_DATA_MEMORY_AUX_TABLEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_egq_aux_table_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_EGQ_ACTION_PROFILE_TABLE_TBL_ENTRY_SIZE 2

uint32
  arad_pp_egq_action_profile_table_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_EGQ_ACTION_PROFILE_TABLE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EGQ_ACTION_PROFILE_TABLE_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EGQ_ACTION_PROFILE_TABLE_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EGQ_ACTION_PROFILE_TABLE_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_EGQ_ACTION_PROFILE_TABLE_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          EGQ_ACTION_PROFILE_TABLEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->out_tm_port_valid = soc_mem_field32_get(unit, EGQ_ACTION_PROFILE_TABLEm, data, OUT_TM_PORT_VALIDf);
  tbl_data->out_tm_port = soc_mem_field32_get(unit, EGQ_ACTION_PROFILE_TABLEm, data, OUT_TM_PORTf);
  tbl_data->discard = soc_mem_field32_get(unit, EGQ_ACTION_PROFILE_TABLEm, data, DISCARDf);
  tbl_data->cud_valid = soc_mem_field32_get(unit, EGQ_ACTION_PROFILE_TABLEm, data, ORIGINAL_CUD_VALIDf);
  tbl_data->cud = soc_mem_field32_get(unit, EGQ_ACTION_PROFILE_TABLEm, data, ORIGINAL_CUDf);
  tbl_data->dp_valid = soc_mem_field32_get(unit, EGQ_ACTION_PROFILE_TABLEm, data, DP_VALIDf);
  tbl_data->dp = soc_mem_field32_get(unit, EGQ_ACTION_PROFILE_TABLEm, data, DPf);
  tbl_data->tc_valid = soc_mem_field32_get(unit, EGQ_ACTION_PROFILE_TABLEm, data, TC_VALIDf);
  tbl_data->tc = soc_mem_field32_get(unit, EGQ_ACTION_PROFILE_TABLEm, data, TCf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_egq_action_profile_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_egq_action_profile_table_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_EGQ_ACTION_PROFILE_TABLE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EGQ_ACTION_PROFILE_TABLE_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EGQ_ACTION_PROFILE_TABLE_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EGQ_ACTION_PROFILE_TABLE_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, EGQ_ACTION_PROFILE_TABLEm, data, OUT_TM_PORT_VALIDf, tbl_data->out_tm_port_valid);

  soc_mem_field32_set(unit, EGQ_ACTION_PROFILE_TABLEm, data, OUT_TM_PORTf, tbl_data->out_tm_port);

  soc_mem_field32_set(unit, EGQ_ACTION_PROFILE_TABLEm, data, DISCARDf, tbl_data->discard);
  soc_mem_field32_set(unit, EGQ_ACTION_PROFILE_TABLEm, data, ORIGINAL_CUD_VALIDf, tbl_data->cud_valid);
  soc_mem_field32_set(unit, EGQ_ACTION_PROFILE_TABLEm, data, ORIGINAL_CUDf, tbl_data->cud);
  soc_mem_field32_set(unit, EGQ_ACTION_PROFILE_TABLEm, data, DP_VALIDf, tbl_data->dp_valid);
  soc_mem_field32_set(unit, EGQ_ACTION_PROFILE_TABLEm, data, DPf, tbl_data->dp);
  soc_mem_field32_set(unit, EGQ_ACTION_PROFILE_TABLEm, data, TC_VALIDf, tbl_data->tc_valid);
  soc_mem_field32_set(unit, EGQ_ACTION_PROFILE_TABLEm, data, TCf, tbl_data->tc);

  res = soc_mem_write(
          unit,
          EGQ_ACTION_PROFILE_TABLEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_egq_action_profile_table_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_EPNI_PP_PCT_TBL_ENTRY_SIZE 4

int
  arad_pp_epni_pp_pct_tbl_get_unsafe(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  uint32     pp_port,
    SOC_SAND_IN  int        core,
    SOC_SAND_OUT ARAD_PP_EPNI_PP_PCT_TBL_DATA *tbl_data
  )
{
    uint32 data[ARAD_PP_EPNI_PP_PCT_TBL_ENTRY_SIZE];

    SOCDNX_INIT_FUNC_DEFS;

    sal_memset(data, 0, sizeof(uint32)*ARAD_PP_EPNI_PP_PCT_TBL_ENTRY_SIZE);
    sal_memset(tbl_data, 0, sizeof(ARAD_PP_EPNI_PP_PCT_TBL_DATA));

    SOCDNX_IF_ERR_EXIT(soc_mem_read(
          unit,
          EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm,
          EPNI_BLOCK(unit, core),
          pp_port,
          data
        ));

    tbl_data->acceptable_frame_type_profile = soc_mem_field32_get(unit, EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm, data, ACCEPTABLE_FRAME_TYPE_PROFILEf);
    tbl_data->port_is_pbp = soc_mem_field32_get(unit, EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm, data, PORT_IS_PBPf);
    tbl_data->egress_stp_filter_enable = soc_mem_field32_get(unit, EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm, data, EGRESS_STP_FILTER_ENABLEf);
    tbl_data->egress_vsi_filter_enable = soc_mem_field32_get(unit, EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm, data, EGRESS_VSI_FILTER_ENABLEf);
    tbl_data->disable_filter = soc_mem_field32_get(unit, EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm, data, DISABLE_FILTERf);
    tbl_data->default_sem_result = soc_mem_field32_get(unit, EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm, data, DEFAULT_SEM_RESULTf);
    tbl_data->exp_map_profile = soc_mem_field32_get(unit, EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm, data, EXP_MAP_PROFILEf);
    tbl_data->vlan_domain = soc_mem_field32_get(unit, EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm, data, VLAN_DOMAINf);
    tbl_data->cep_c_vlan_edit = soc_mem_field32_get(unit, EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm, data, CEP_C_VLAN_EDITf);
    tbl_data->llvp_profile = soc_mem_field32_get(unit, EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm, data, LLVP_PROFILEf);
    tbl_data->mpls_eth_type_select = soc_mem_field32_get(unit, EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm, data, MPLS_ETH_TYPE_SELECTf);
    tbl_data->prge_profile = soc_mem_field32_get(unit, EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm, data, PRGE_PROFILEf);
    tbl_data->prge_var = soc_mem_field32_get(unit, EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm, data, PRGE_VARf);
    tbl_data->dst_system_port = soc_mem_field32_get(unit, EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm, data, DST_SYSTEM_PORTf);
    tbl_data->mtu = soc_mem_field32_get(unit, EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm, data, MTUf);
    tbl_data->ieee_1588_mac_enable = soc_mem_field32_get(unit, EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm, data, IEEE_1588_MAC_ENABLEf);
    tbl_data->oam_port_profile = soc_mem_field32_get(unit, EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm, data, OAM_PORT_PROFILEf);
    if (SOC_IS_ARAD_B0_AND_ABOVE(unit)) {
        tbl_data->reserved = soc_mem_field32_get(unit, EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm, data, OSTS_ENABLEf);
    } else {
        tbl_data->reserved = soc_mem_field32_get(unit, EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm, data, RESERVEDf);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

int
  arad_pp_epni_pp_pct_tbl_set_unsafe(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  uint32     pp_port,
    SOC_SAND_IN  int        core,
    SOC_SAND_IN  ARAD_PP_EPNI_PP_PCT_TBL_DATA *tbl_data
  )
{
    uint32 data[ARAD_PP_EPNI_PP_PCT_TBL_ENTRY_SIZE];
 
    SOCDNX_INIT_FUNC_DEFS;

    sal_memset(data, 0, sizeof(uint32)*ARAD_PP_EPNI_PP_PCT_TBL_ENTRY_SIZE);

    soc_mem_field32_set(unit, EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm, data, ACCEPTABLE_FRAME_TYPE_PROFILEf, tbl_data->acceptable_frame_type_profile);
    soc_mem_field32_set(unit, EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm, data, PORT_IS_PBPf, tbl_data->port_is_pbp);
    soc_mem_field32_set(unit, EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm, data, EGRESS_STP_FILTER_ENABLEf, tbl_data->egress_stp_filter_enable);
    soc_mem_field32_set(unit, EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm, data, EGRESS_VSI_FILTER_ENABLEf, tbl_data->egress_vsi_filter_enable);
    soc_mem_field32_set(unit, EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm, data, DISABLE_FILTERf, tbl_data->disable_filter);
    soc_mem_field32_set(unit, EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm, data, DEFAULT_SEM_RESULTf, tbl_data->default_sem_result);
    soc_mem_field32_set(unit, EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm, data, EXP_MAP_PROFILEf, tbl_data->exp_map_profile);
    soc_mem_field32_set(unit, EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm, data, VLAN_DOMAINf, tbl_data->vlan_domain);
    soc_mem_field32_set(unit, EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm, data, CEP_C_VLAN_EDITf, tbl_data->cep_c_vlan_edit);
    soc_mem_field32_set(unit, EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm, data, LLVP_PROFILEf, tbl_data->llvp_profile);
    soc_mem_field32_set(unit, EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm, data, MPLS_ETH_TYPE_SELECTf, tbl_data->mpls_eth_type_select);
    soc_mem_field32_set(unit, EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm, data, PRGE_PROFILEf, tbl_data->prge_profile);
    soc_mem_field32_set(unit, EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm, data, PRGE_VARf, tbl_data->prge_var);
    soc_mem_field32_set(unit, EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm, data, DST_SYSTEM_PORTf, tbl_data->dst_system_port);
    soc_mem_field32_set(unit, EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm, data, MTUf, tbl_data->mtu);
    soc_mem_field32_set(unit, EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm, data, IEEE_1588_MAC_ENABLEf, tbl_data->ieee_1588_mac_enable);
    soc_mem_field32_set(unit, EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm, data, OAM_PORT_PROFILEf, tbl_data->oam_port_profile);
    if (SOC_IS_ARAD_B0_AND_ABOVE(unit)) {
        soc_mem_field32_set(unit, EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm, data, OSTS_ENABLEf, tbl_data->reserved);
    } else {
        soc_mem_field32_set(unit, EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm, data, RESERVEDf, tbl_data->reserved);
    }

    SOCDNX_IF_ERR_EXIT(soc_mem_write(
          unit,
          EPNI_PACKET_PROCESSING_PORT_CONFIGURATION_TABLE_PP_PCTm,
          EPNI_BLOCK(unit, core),
          pp_port,
          data
        ));

exit:
    SOCDNX_FUNC_RETURN;
}

#define ARAD_PP_EPNI_LLVP_TABLE_TBL_ENTRY_SIZE 1

uint32
  arad_pp_epni_llvp_table_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_EPNI_LLVP_TABLE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EPNI_LLVP_TABLE_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EPNI_LLVP_TABLE_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EPNI_LLVP_TABLE_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_EPNI_LLVP_TABLE_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          EPNI_LINK_LAYER_VLAN_PROCESSING_LLVPm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->c_tag_offset = soc_mem_field32_get(unit, EPNI_LINK_LAYER_VLAN_PROCESSING_LLVPm, data, C_TAG_OFFSETf);
  tbl_data->packet_has_c_tag = soc_mem_field32_get(unit, EPNI_LINK_LAYER_VLAN_PROCESSING_LLVPm, data, PACKET_HAS_C_TAGf);
  tbl_data->packet_has_up = soc_mem_field32_get(unit, EPNI_LINK_LAYER_VLAN_PROCESSING_LLVPm, data, PACKET_HAS_UPf);
  tbl_data->packet_has_pcp_dei = soc_mem_field32_get(unit, EPNI_LINK_LAYER_VLAN_PROCESSING_LLVPm, data, PACKET_HAS_PCP_DEIf);
  tbl_data->incoming_tag_format = soc_mem_field32_get(unit, EPNI_LINK_LAYER_VLAN_PROCESSING_LLVPm, data, INCOMING_TAG_FORMATf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_epni_llvp_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_epni_llvp_table_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_EPNI_LLVP_TABLE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EPNI_LLVP_TABLE_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EPNI_LLVP_TABLE_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EPNI_LLVP_TABLE_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, EPNI_LINK_LAYER_VLAN_PROCESSING_LLVPm, data, C_TAG_OFFSETf, tbl_data->c_tag_offset);
  soc_mem_field32_set(unit, EPNI_LINK_LAYER_VLAN_PROCESSING_LLVPm, data, PACKET_HAS_C_TAGf, tbl_data->packet_has_c_tag);
  soc_mem_field32_set(unit, EPNI_LINK_LAYER_VLAN_PROCESSING_LLVPm, data, PACKET_HAS_UPf, tbl_data->packet_has_up);
  soc_mem_field32_set(unit, EPNI_LINK_LAYER_VLAN_PROCESSING_LLVPm, data, PACKET_HAS_PCP_DEIf, tbl_data->packet_has_pcp_dei);
  soc_mem_field32_set(unit, EPNI_LINK_LAYER_VLAN_PROCESSING_LLVPm, data, INCOMING_TAG_FORMATf, tbl_data->incoming_tag_format);

  res = soc_mem_write(
          unit,
          EPNI_LINK_LAYER_VLAN_PROCESSING_LLVPm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_epni_llvp_table_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHP_VSI_LOW_CFG_1_TBL_ENTRY_SIZE 1

uint32
  arad_pp_ihp_vsi_low_cfg_1_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_VSI_LOW_CFG_1_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_VSI_LOW_CFG_1_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_VSI_LOW_CFG_1_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_VSI_LOW_CFG_1_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_VSI_LOW_CFG_1_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHP_VSI_LOW_CFG_1m,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->topology_id = soc_mem_field32_get(unit, IHP_VSI_LOW_CFG_1m, data, TOPOLOGY_IDf);
  tbl_data->my_mac = soc_mem_field32_get(unit, IHP_VSI_LOW_CFG_1m, data, MY_MACf);
  tbl_data->my_mac_valid = soc_mem_field32_get(unit, IHP_VSI_LOW_CFG_1m, data, MY_MAC_VALIDf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_vsi_low_cfg_1_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_vsi_low_cfg_1_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_VSI_LOW_CFG_1_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_VSI_LOW_CFG_1_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_VSI_LOW_CFG_1_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_VSI_LOW_CFG_1_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHP_VSI_LOW_CFG_1m, data, TOPOLOGY_IDf, tbl_data->topology_id);
  soc_mem_field32_set(unit, IHP_VSI_LOW_CFG_1m, data, MY_MACf, tbl_data->my_mac);
  soc_mem_field32_set(unit, IHP_VSI_LOW_CFG_1m, data, MY_MAC_VALIDf, tbl_data->my_mac_valid);

  res = soc_mem_write(
          unit,
          IHP_VSI_LOW_CFG_1m,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_vsi_low_cfg_1_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHP_VSI_LOW_CFG_2_TBL_ENTRY_SIZE 1

uint32
  arad_pp_ihp_vsi_low_cfg_2_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_VSI_LOW_CFG_2_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_VSI_LOW_CFG_2_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_VSI_LOW_CFG_2_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_VSI_LOW_CFG_2_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_VSI_LOW_CFG_2_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHP_VSI_LOW_CFG_2m,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->profile = soc_mem_field32_get(unit, IHP_VSI_LOW_CFG_2m, data, PROFILEf);
  tbl_data->fid_class = soc_mem_field32_get(unit, IHP_VSI_LOW_CFG_2m, data, FID_CLASSf);
  tbl_data->da_not_found_destination = soc_mem_field32_get(unit, IHP_VSI_LOW_CFG_2m, data, DA_NOT_FOUND_DESTINATIONf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_vsi_low_cfg_2_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_vsi_low_cfg_2_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_VSI_LOW_CFG_2_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_VSI_LOW_CFG_2_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_VSI_LOW_CFG_2_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_VSI_LOW_CFG_2_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHP_VSI_LOW_CFG_2m, data, PROFILEf, tbl_data->profile);
  soc_mem_field32_set(unit, IHP_VSI_LOW_CFG_2m, data, FID_CLASSf, tbl_data->fid_class);
  soc_mem_field32_set(unit, IHP_VSI_LOW_CFG_2m, data, DA_NOT_FOUND_DESTINATIONf, tbl_data->da_not_found_destination);

  res = soc_mem_write(
          unit,
          IHP_VSI_LOW_CFG_2m,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_vsi_low_cfg_2_tbl_set_unsafe()", entry_offset, 0);
}


#define ARAD_PP_IHP_VSI_HIGH_MY_MAC_TBL_ENTRY_SIZE 1

uint32
  arad_pp_ihp_vsi_high_my_mac_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_VSI_HIGH_MY_MAC_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_VSI_HIGH_MY_MAC_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_VSI_HIGH_MY_MAC_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_VSI_HIGH_MY_MAC_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_VSI_HIGH_MY_MAC_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHP_VSI_HIGH_MY_MACm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->valid[0] = soc_mem_field32_get(unit, IHP_VSI_HIGH_MY_MACm, data, VALIDf);
  tbl_data->valid[1] = soc_mem_field32_get(unit, IHP_VSI_HIGH_MY_MACm, data, VALID_1f);
  tbl_data->valid[2] = soc_mem_field32_get(unit, IHP_VSI_HIGH_MY_MACm, data, VALID_2f);
  tbl_data->valid[3] = soc_mem_field32_get(unit, IHP_VSI_HIGH_MY_MACm, data, VALID_3f);
  tbl_data->valid[4] = soc_mem_field32_get(unit, IHP_VSI_HIGH_MY_MACm, data, VALID_4f);
  tbl_data->valid[5] = soc_mem_field32_get(unit, IHP_VSI_HIGH_MY_MACm, data, VALID_5f);
  tbl_data->valid[6] = soc_mem_field32_get(unit, IHP_VSI_HIGH_MY_MACm, data, VALID_6f);
  tbl_data->valid[7] = soc_mem_field32_get(unit, IHP_VSI_HIGH_MY_MACm, data, VALID_7f);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_vsi_high_my_mac_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_vsi_high_my_mac_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_VSI_HIGH_MY_MAC_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_VSI_HIGH_MY_MAC_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_VSI_HIGH_MY_MAC_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_VSI_HIGH_MY_MAC_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHP_VSI_HIGH_MY_MACm, data, VALIDf, tbl_data->valid[0]);
  soc_mem_field32_set(unit, IHP_VSI_HIGH_MY_MACm, data, VALID_1f, tbl_data->valid[1]);
  soc_mem_field32_set(unit, IHP_VSI_HIGH_MY_MACm, data, VALID_2f, tbl_data->valid[2]);
  soc_mem_field32_set(unit, IHP_VSI_HIGH_MY_MACm, data, VALID_3f, tbl_data->valid[3]);
  soc_mem_field32_set(unit, IHP_VSI_HIGH_MY_MACm, data, VALID_4f, tbl_data->valid[4]);
  soc_mem_field32_set(unit, IHP_VSI_HIGH_MY_MACm, data, VALID_5f, tbl_data->valid[5]);
  soc_mem_field32_set(unit, IHP_VSI_HIGH_MY_MACm, data, VALID_6f, tbl_data->valid[6]);
  soc_mem_field32_set(unit, IHP_VSI_HIGH_MY_MACm, data, VALID_7f, tbl_data->valid[7]);

  res = soc_mem_write(
          unit,
          IHP_VSI_HIGH_MY_MACm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_vsi_high_my_mac_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHP_VSI_HIGH_PROFILE_TBL_ENTRY_SIZE 1

uint32
  arad_pp_ihp_vsi_high_profile_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_VSI_HIGH_PROFILE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_VSI_HIGH_PROFILE_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_VSI_HIGH_PROFILE_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_VSI_HIGH_PROFILE_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_VSI_HIGH_PROFILE_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHP_VSI_HIGH_PROFILEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->index[0] = soc_mem_field32_get(unit, IHP_VSI_HIGH_PROFILEm, data, INDEXf);
  tbl_data->index[1] = soc_mem_field32_get(unit, IHP_VSI_HIGH_PROFILEm, data, INDEX_1f);
  tbl_data->index[2] = soc_mem_field32_get(unit, IHP_VSI_HIGH_PROFILEm, data, INDEX_2f);
  tbl_data->index[3] = soc_mem_field32_get(unit, IHP_VSI_HIGH_PROFILEm, data, INDEX_3f);
  tbl_data->index[4] = soc_mem_field32_get(unit, IHP_VSI_HIGH_PROFILEm, data, INDEX_4f);
  tbl_data->index[5] = soc_mem_field32_get(unit, IHP_VSI_HIGH_PROFILEm, data, INDEX_5f);
  tbl_data->index[6] = soc_mem_field32_get(unit, IHP_VSI_HIGH_PROFILEm, data, INDEX_6f);
  tbl_data->index[7] = soc_mem_field32_get(unit, IHP_VSI_HIGH_PROFILEm, data, INDEX_7f);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_vsi_high_profile_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_vsi_high_profile_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_VSI_HIGH_PROFILE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_VSI_HIGH_PROFILE_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_VSI_HIGH_PROFILE_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_VSI_HIGH_PROFILE_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHP_VSI_HIGH_PROFILEm, data, INDEXf, tbl_data->index[0]);
  soc_mem_field32_set(unit, IHP_VSI_HIGH_PROFILEm, data, INDEX_1f, tbl_data->index[1]);
  soc_mem_field32_set(unit, IHP_VSI_HIGH_PROFILEm, data, INDEX_2f, tbl_data->index[2]);
  soc_mem_field32_set(unit, IHP_VSI_HIGH_PROFILEm, data, INDEX_3f, tbl_data->index[3]);
  soc_mem_field32_set(unit, IHP_VSI_HIGH_PROFILEm, data, INDEX_4f, tbl_data->index[4]);
  soc_mem_field32_set(unit, IHP_VSI_HIGH_PROFILEm, data, INDEX_5f, tbl_data->index[5]);
  soc_mem_field32_set(unit, IHP_VSI_HIGH_PROFILEm, data, INDEX_6f, tbl_data->index[6]);
  soc_mem_field32_set(unit, IHP_VSI_HIGH_PROFILEm, data, INDEX_7f, tbl_data->index[7]);

  res = soc_mem_write(
          unit,
          IHP_VSI_HIGH_PROFILEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_vsi_high_profile_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHP_VSI_HIGH_DA_NOT_FOUND_DESTINATION_TBL_ENTRY_SIZE 1

uint32
  arad_pp_ihp_vsi_high_da_not_found_destination_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_VSI_HIGH_DA_NOT_FOUND_DESTINATION_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_VSI_HIGH_DA_NOT_FOUND_DESTINATION_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_VSI_HIGH_DA_NOT_FOUND_DESTINATION_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_VSI_HIGH_DA_NOT_FOUND_DESTINATION_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_VSI_HIGH_DA_NOT_FOUND_DESTINATION_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHP_VSI_HIGH_DA_NOT_FOUND_DESTINATIONm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->da_not_found_destination = soc_mem_field32_get(unit, IHP_VSI_HIGH_DA_NOT_FOUND_DESTINATIONm, data, DA_NOT_FOUND_DESTINATIONf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_vsi_high_da_not_found_destination_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_vsi_high_da_not_found_destination_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_VSI_HIGH_DA_NOT_FOUND_DESTINATION_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_VSI_HIGH_DA_NOT_FOUND_DESTINATION_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_VSI_HIGH_DA_NOT_FOUND_DESTINATION_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_VSI_HIGH_DA_NOT_FOUND_DESTINATION_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHP_VSI_HIGH_DA_NOT_FOUND_DESTINATIONm, data, DA_NOT_FOUND_DESTINATIONf, tbl_data->da_not_found_destination);

  res = soc_mem_write(
          unit,
          IHP_VSI_HIGH_DA_NOT_FOUND_DESTINATIONm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_vsi_high_da_not_found_destination_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHP_MACT_FID_COUNTER_DB_TBL_ENTRY_SIZE 1

uint32
  arad_pp_ihp_mact_fid_counter_db_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_MACT_FID_COUNTER_DB_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_MACT_FID_COUNTER_DB_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_MACT_FID_COUNTER_DB_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_MACT_FID_COUNTER_DB_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_MACT_FID_COUNTER_DB_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          PPDB_B_LARGE_EM_FID_COUNTER_DBm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->entry_count = soc_mem_field32_get(unit, PPDB_B_LARGE_EM_FID_COUNTER_DBm, data, ENTRY_COUNTf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_mact_fid_counter_db_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_mact_fid_counter_db_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_MACT_FID_COUNTER_DB_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_MACT_FID_COUNTER_DB_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_MACT_FID_COUNTER_DB_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_MACT_FID_COUNTER_DB_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, PPDB_B_LARGE_EM_FID_COUNTER_DBm, data, ENTRY_COUNTf, tbl_data->entry_count);

  res = soc_mem_write(
          unit,
          PPDB_B_LARGE_EM_FID_COUNTER_DBm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_mact_fid_counter_db_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHP_MACT_FID_PROFILE_DB_TBL_ENTRY_SIZE 1

uint32
  arad_pp_ihp_mact_fid_profile_db_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_MACT_FID_PROFILE_DB_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_MACT_FID_PROFILE_DB_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_MACT_FID_PROFILE_DB_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_MACT_FID_PROFILE_DB_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_MACT_FID_PROFILE_DB_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          PPDB_B_LARGE_EM_FID_PROFILE_DBm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->profile_pointer[0] = soc_mem_field32_get(unit, PPDB_B_LARGE_EM_FID_PROFILE_DBm, data, PROFILE_POINTER_0f);
  tbl_data->profile_pointer[1] = soc_mem_field32_get(unit, PPDB_B_LARGE_EM_FID_PROFILE_DBm, data, PROFILE_POINTER_1f);
  tbl_data->profile_pointer[2] = soc_mem_field32_get(unit, PPDB_B_LARGE_EM_FID_PROFILE_DBm, data, PROFILE_POINTER_2f);
  tbl_data->profile_pointer[3] = soc_mem_field32_get(unit, PPDB_B_LARGE_EM_FID_PROFILE_DBm, data, PROFILE_POINTER_3f);
  tbl_data->profile_pointer[4] = soc_mem_field32_get(unit, PPDB_B_LARGE_EM_FID_PROFILE_DBm, data, PROFILE_POINTER_4f);
  tbl_data->profile_pointer[5] = soc_mem_field32_get(unit, PPDB_B_LARGE_EM_FID_PROFILE_DBm, data, PROFILE_POINTER_5f);
  tbl_data->profile_pointer[6] = soc_mem_field32_get(unit, PPDB_B_LARGE_EM_FID_PROFILE_DBm, data, PROFILE_POINTER_6f);
  tbl_data->profile_pointer[7] = soc_mem_field32_get(unit, PPDB_B_LARGE_EM_FID_PROFILE_DBm, data, PROFILE_POINTER_7f);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_mact_fid_profile_db_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_mact_fid_profile_db_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_MACT_FID_PROFILE_DB_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_MACT_FID_PROFILE_DB_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_MACT_FID_PROFILE_DB_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_MACT_FID_PROFILE_DB_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, PPDB_B_LARGE_EM_FID_PROFILE_DBm, data, PROFILE_POINTER_0f, tbl_data->profile_pointer[0]);
  soc_mem_field32_set(unit, PPDB_B_LARGE_EM_FID_PROFILE_DBm, data, PROFILE_POINTER_1f, tbl_data->profile_pointer[1]);
  soc_mem_field32_set(unit, PPDB_B_LARGE_EM_FID_PROFILE_DBm, data, PROFILE_POINTER_2f, tbl_data->profile_pointer[2]);
  soc_mem_field32_set(unit, PPDB_B_LARGE_EM_FID_PROFILE_DBm, data, PROFILE_POINTER_3f, tbl_data->profile_pointer[3]);
  soc_mem_field32_set(unit, PPDB_B_LARGE_EM_FID_PROFILE_DBm, data, PROFILE_POINTER_4f, tbl_data->profile_pointer[4]);
  soc_mem_field32_set(unit, PPDB_B_LARGE_EM_FID_PROFILE_DBm, data, PROFILE_POINTER_5f, tbl_data->profile_pointer[5]);
  soc_mem_field32_set(unit, PPDB_B_LARGE_EM_FID_PROFILE_DBm, data, PROFILE_POINTER_6f, tbl_data->profile_pointer[6]);
  soc_mem_field32_set(unit, PPDB_B_LARGE_EM_FID_PROFILE_DBm, data, PROFILE_POINTER_7f, tbl_data->profile_pointer[7]);

  res = soc_mem_write(
          unit,
          PPDB_B_LARGE_EM_FID_PROFILE_DBm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_mact_fid_profile_db_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHP_MACT_FID_COUNTER_PROFILE_DB_TBL_ENTRY_SIZE 1

uint32
  arad_pp_ihp_mact_fid_counter_profile_db_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_MACT_FID_COUNTER_PROFILE_DB_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_MACT_FID_COUNTER_PROFILE_DB_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_MACT_FID_COUNTER_PROFILE_DB_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_MACT_FID_COUNTER_PROFILE_DB_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_MACT_FID_COUNTER_PROFILE_DB_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          PPDB_B_LARGE_EM_FID_COUNTER_PROFILE_DBm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->limit = soc_mem_field32_get(unit, PPDB_B_LARGE_EM_FID_COUNTER_PROFILE_DBm, data, LIMITf);
  tbl_data->interrupt_en = soc_mem_field32_get(unit, PPDB_B_LARGE_EM_FID_COUNTER_PROFILE_DBm, data, INTERRUPT_ENf);
  tbl_data->message_en = soc_mem_field32_get(unit, PPDB_B_LARGE_EM_FID_COUNTER_PROFILE_DBm, data, MESSAGE_ENf);
  tbl_data->event_forwarding_profile = soc_mem_field32_get(unit, PPDB_B_LARGE_EM_FID_COUNTER_PROFILE_DBm, data, EVENT_FORWARDING_PROFILEf);
  tbl_data->fid_aging_profile = soc_mem_field32_get(unit, PPDB_B_LARGE_EM_FID_COUNTER_PROFILE_DBm, data, FID_AGING_PROFILEf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_mact_fid_counter_profile_db_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_mact_fid_counter_profile_db_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_MACT_FID_COUNTER_PROFILE_DB_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_MACT_FID_COUNTER_PROFILE_DB_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_MACT_FID_COUNTER_PROFILE_DB_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_MACT_FID_COUNTER_PROFILE_DB_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, PPDB_B_LARGE_EM_FID_COUNTER_PROFILE_DBm, data, LIMITf, tbl_data->limit);
  soc_mem_field32_set(unit, PPDB_B_LARGE_EM_FID_COUNTER_PROFILE_DBm, data, INTERRUPT_ENf, tbl_data->interrupt_en);
  soc_mem_field32_set(unit, PPDB_B_LARGE_EM_FID_COUNTER_PROFILE_DBm, data, MESSAGE_ENf, tbl_data->message_en);
  soc_mem_field32_set(unit, PPDB_B_LARGE_EM_FID_COUNTER_PROFILE_DBm, data, EVENT_FORWARDING_PROFILEf, tbl_data->event_forwarding_profile);
  soc_mem_field32_set(unit, PPDB_B_LARGE_EM_FID_COUNTER_PROFILE_DBm, data, FID_AGING_PROFILEf, tbl_data->fid_aging_profile);

  res = soc_mem_write(
          unit,
          PPDB_B_LARGE_EM_FID_COUNTER_PROFILE_DBm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_mact_fid_counter_profile_db_tbl_set_unsafe()", entry_offset, 0);
}



uint32
  arad_pp_ihp_vlan_port_membership_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_VLAN_PORT_MEMBERSHIP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
      data_0[ARAD_PP_IHP_VLAN_PORT_MEMBERSHIP_TBL_ENTRY_SIZE],
      data_1[ARAD_PP_IHP_VLAN_PORT_MEMBERSHIP_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_VLAN_PORT_MEMBERSHIP_TBL_SET_UNSAFE);

  ARAD_CLEAR(data_0, uint32, ARAD_PP_IHP_VLAN_PORT_MEMBERSHIP_TBL_ENTRY_SIZE);
  ARAD_CLEAR(data_1, uint32, ARAD_PP_IHP_VLAN_PORT_MEMBERSHIP_TBL_ENTRY_SIZE);

  res = soc_mem_array_read(
          unit,
          IHP_VSI_PORT_MEMBERSHIPm,
          0, 
          MEM_BLOCK_ANY,
          entry_offset,
          data_0
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_mem_array_read(
          unit,
          IHP_VSI_PORT_MEMBERSHIPm,
          1, 
          MEM_BLOCK_ANY,
          entry_offset,
          data_1
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

  soc_mem_field_get(
      unit, 
      IHP_VSI_PORT_MEMBERSHIPm, 
      data_0, 
      VLAN_PORT_MEMBER_LINEf, 
      (uint32*) &(tbl_data->vlan_port_member_line[0])
      );
  soc_mem_field_get(
      unit, 
      IHP_VSI_PORT_MEMBERSHIPm, 
      data_1, 
      VLAN_PORT_MEMBER_LINEf, 
      (uint32*) &(tbl_data->vlan_port_member_line[4])
      );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_vlan_port_membership_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_vlan_port_membership_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_VLAN_PORT_MEMBERSHIP_TBL_DATA  *tbl_data
  )
{
  uint32
      res = SOC_SAND_OK;
  uint32
      data_0[ARAD_PP_IHP_VLAN_PORT_MEMBERSHIP_TBL_ENTRY_SIZE],
      data_1[ARAD_PP_IHP_VLAN_PORT_MEMBERSHIP_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_VLAN_PORT_MEMBERSHIP_TBL_SET_UNSAFE);

  ARAD_CLEAR(data_0, uint32, ARAD_PP_IHP_VLAN_PORT_MEMBERSHIP_TBL_ENTRY_SIZE);
  ARAD_CLEAR(data_1, uint32, ARAD_PP_IHP_VLAN_PORT_MEMBERSHIP_TBL_ENTRY_SIZE);

  soc_mem_field_set(
      unit, 
      IHP_VSI_PORT_MEMBERSHIPm, 
      data_0, 
      VLAN_PORT_MEMBER_LINEf, 
      (uint32*) &(tbl_data->vlan_port_member_line[0])
      );

  soc_mem_field_set(
      unit, 
      IHP_VSI_PORT_MEMBERSHIPm, 
      data_1, 
      VLAN_PORT_MEMBER_LINEf, 
      (uint32*) &(tbl_data->vlan_port_member_line[4])
      );

  res = soc_mem_array_write(
          unit,
          IHP_VSI_PORT_MEMBERSHIPm,
          0, 
          MEM_BLOCK_ANY,
          entry_offset,
          data_0
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_mem_array_write(
          unit,
          IHP_VSI_PORT_MEMBERSHIPm,
          1, 
          MEM_BLOCK_ANY,
          entry_offset,
          data_1
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_ihp_vlan_port_membership_tbl_set_unsafe()", entry_offset, 0);
}


#define ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_AC_TBL_ENTRY_SIZE ARAD_PP_IHP_LIF_TABLE_ENTRY_SIZE

/* In Jericho, the out_lif field is split into LSBs and MSBs. These macros are here so the out_lif can be split in the
   set function and merged in the get function. */

#define ARAD_PP_PPDB_B_LIF_TABLE_AC_2_OUT_LIF_OUT_LIF_MSB_MASK    1
#define ARAD_PP_PPDB_B_LIF_TABLE_AC_2_OUT_LIF_OUT_LIF_MSB_SHIFT   17

#define ARAD_PP_PPDB_B_LIF_TABLE_AC_2_OUT_LIF_OUT_LIF_LSB_MASK    0x1ffff
#define ARAD_PP_PPDB_B_LIF_TABLE_AC_2_OUT_LIF_OUT_LIF_LSB_SHIFT   0

#define ARAD_PP_PPDB_B_LIF_TABLE_AC_2_OUT_LIF_OUT_LIF_MSB_GET(_out_lif)  \
    ((_out_lif >> ARAD_PP_PPDB_B_LIF_TABLE_AC_2_OUT_LIF_OUT_LIF_MSB_SHIFT) & ARAD_PP_PPDB_B_LIF_TABLE_AC_2_OUT_LIF_OUT_LIF_MSB_MASK)

#define ARAD_PP_PPDB_B_LIF_TABLE_AC_2_OUT_LIF_OUT_LIF_LSB_GET(_out_lif)  \
    ((_out_lif >> ARAD_PP_PPDB_B_LIF_TABLE_AC_2_OUT_LIF_OUT_LIF_LSB_SHIFT) & ARAD_PP_PPDB_B_LIF_TABLE_AC_2_OUT_LIF_OUT_LIF_LSB_MASK)

#define ARAD_PP_PPDB_B_LIF_TABLE_AC_2_OUT_LIF_OUT_LIF_SET(_out_lif, _out_lif_msb, _out_lif_lsb)  \
    (_out_lif) = (((_out_lif_msb & ARAD_PP_PPDB_B_LIF_TABLE_AC_2_OUT_LIF_OUT_LIF_MSB_MASK) << ARAD_PP_PPDB_B_LIF_TABLE_AC_2_OUT_LIF_OUT_LIF_MSB_SHIFT) \
                | ((out_lif_lsb & ARAD_PP_PPDB_B_LIF_TABLE_AC_2_OUT_LIF_OUT_LIF_LSB_MASK) << ARAD_PP_PPDB_B_LIF_TABLE_AC_2_OUT_LIF_OUT_LIF_LSB_SHIFT))

uint32
  arad_pp_ihp_lif_table_ac_p2p_to_ac_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_AC_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK,
    entry_offset_in_bank,
    bank_id;
  uint32
    data[ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_AC_TBL_ENTRY_SIZE];
  uint32 out_lif_lsb = 0, out_lif_msb = 0;
  soc_mem_t lif_mem_id = PPDB_B_LIF_TABLE_AC_2_OUT_LIFm;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_AC_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_AC_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_AC_TBL_DATA, 1);

  bank_id = LIF_TABLE_ENTRY_OFFSET_TO_BANK_ID(unit, entry_offset);
  entry_offset_in_bank = LIF_TABLE_ENTRY_OFFSET_TO_OFFSET_IN_BANK(unit, entry_offset);
  
  res = soc_mem_array_read(unit, lif_mem_id, bank_id, MEM_BLOCK_ANY, entry_offset_in_bank, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->destination = soc_mem_field32_get(unit, lif_mem_id, data, DESTINATIONf);
  tbl_data->vlan_edit_vid_2 = soc_mem_field32_get(unit, lif_mem_id, data, VLAN_EDIT_VID_2f);
  tbl_data->vlan_edit_vid_1 = soc_mem_field32_get(unit, lif_mem_id, data, VLAN_EDIT_VID_1f);
  tbl_data->vlan_edit_pcp_dei_profile = soc_mem_field32_get(unit, lif_mem_id, data, VLAN_EDIT_PCP_DEI_PROFILEf);
  tbl_data->vlan_edit_profile = soc_mem_field32_get(unit, lif_mem_id, data, VLAN_EDIT_PROFILEf);
  tbl_data->orientation_is_hub = soc_mem_field32_get(unit, lif_mem_id, data, ORIENTATION_IS_HUBf);
  tbl_data->cos_profile = soc_mem_field32_get(unit, lif_mem_id, data, COS_PROFILEf);
  tbl_data->in_lif_profile = soc_mem_field32_get(unit, lif_mem_id, data, IN_LIF_PROFILEf);
  tbl_data->out_lif_valid = soc_mem_field32_get(unit, lif_mem_id, data, OUT_LIF_VALIDf);
  tbl_data->oam_lif_set = soc_mem_field32_get(unit, lif_mem_id, data, OAM_LIF_SETf);
  tbl_data->protection_path = soc_mem_field32_get(unit, lif_mem_id, data, PROTECTION_PATHf);
  tbl_data->protection_pointer = soc_mem_field32_get(unit, lif_mem_id, data, PROTECTION_POINTERf);
  tbl_data->type = soc_mem_field32_get(unit, lif_mem_id, data, TYPEf);

  /* Some fields have different names in Jericho, or don't exist in Jericho, or exist only in Jericho. */
  if (SOC_IS_JERICHO(unit)) {
      tbl_data->sys_in_lif  = soc_mem_field32_get(unit, lif_mem_id, data, SYS_IN_LIFf);
      out_lif_lsb           = soc_mem_field32_get(unit, lif_mem_id, data, OUT_LIF_LSBf);
      out_lif_msb           = soc_mem_field32_get(unit, lif_mem_id, data, OUT_LIF_MSBf);
      ARAD_PP_PPDB_B_LIF_TABLE_AC_2_OUT_LIF_OUT_LIF_SET(tbl_data->out_lif, out_lif_msb, out_lif_lsb);
  } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      tbl_data->use_in_lif = soc_mem_field32_get(unit, IHP_LIF_TABLE_AC_P2P_TO_ACm, data, USE_IN_LIFf);
      tbl_data->out_lif = soc_mem_field32_get(unit, IHP_LIF_TABLE_AC_P2P_TO_ACm, data, OUT_LIFf);
      tbl_data->destination_valid = soc_mem_field32_get(unit, IHP_LIF_TABLE_AC_P2P_TO_ACm, data, DESTINATION_VALIDf);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_lif_table_ac_p2p_to_ac_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_lif_table_ac_p2p_to_ac_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_AC_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK,
    entry_offset_in_bank,
    bank_id;
  uint32
    data[ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_AC_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_AC_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_AC_TBL_ENTRY_SIZE);

  /* The name of the table, and some fields are different in Jericho. Handle them here. */
  if (SOC_IS_JERICHO(unit)) {
      soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_2_OUT_LIFm, data, SYS_IN_LIFf, tbl_data->sys_in_lif);
      soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_2_OUT_LIFm, data, DOUBLE_DATA_ENTRYf, 0 /* Not configurable */);
      soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_2_OUT_LIFm, data, OUT_LIF_LSBf, ARAD_PP_PPDB_B_LIF_TABLE_AC_2_OUT_LIF_OUT_LIF_LSB_GET(tbl_data->out_lif));
      soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_2_OUT_LIFm, data, OUT_LIF_MSBf, ARAD_PP_PPDB_B_LIF_TABLE_AC_2_OUT_LIF_OUT_LIF_MSB_GET(tbl_data->out_lif));
  } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      soc_mem_field32_set(unit, IHP_LIF_TABLE_AC_P2P_TO_ACm, data, USE_IN_LIFf, tbl_data->use_in_lif);
      soc_mem_field32_set(unit, IHP_LIF_TABLE_AC_P2P_TO_ACm, data, DESTINATION_VALIDf, tbl_data->destination_valid);
      soc_mem_field32_set(unit, IHP_LIF_TABLE_AC_P2P_TO_ACm, data, OUT_LIFf, tbl_data->out_lif);
  } 

  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_2_OUT_LIFm, data, DESTINATIONf, tbl_data->destination);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_2_OUT_LIFm, data, VLAN_EDIT_VID_2f, tbl_data->vlan_edit_vid_2);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_2_OUT_LIFm, data, VLAN_EDIT_VID_1f, tbl_data->vlan_edit_vid_1);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_2_OUT_LIFm, data, VLAN_EDIT_PCP_DEI_PROFILEf, tbl_data->vlan_edit_pcp_dei_profile);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_2_OUT_LIFm, data, VLAN_EDIT_PROFILEf, tbl_data->vlan_edit_profile);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_2_OUT_LIFm, data, ORIENTATION_IS_HUBf, tbl_data->orientation_is_hub);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_2_OUT_LIFm, data, COS_PROFILEf, tbl_data->cos_profile);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_2_OUT_LIFm, data, IN_LIF_PROFILEf, tbl_data->in_lif_profile);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_2_OUT_LIFm, data, OUT_LIF_VALIDf, tbl_data->out_lif_valid);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_2_OUT_LIFm, data, OAM_LIF_SETf, tbl_data->oam_lif_set);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_2_OUT_LIFm, data, PROTECTION_PATHf, tbl_data->protection_path);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_2_OUT_LIFm, data, PROTECTION_POINTERf, tbl_data->protection_pointer);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_2_OUT_LIFm, data, TYPEf, tbl_data->type);


  bank_id = LIF_TABLE_ENTRY_OFFSET_TO_BANK_ID(unit, entry_offset);
  entry_offset_in_bank = LIF_TABLE_ENTRY_OFFSET_TO_OFFSET_IN_BANK(unit, entry_offset);

  res = soc_mem_array_write(unit, PPDB_B_LIF_TABLE_AC_2_OUT_LIFm, bank_id, MEM_BLOCK_ANY, entry_offset_in_bank, data);  
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_lif_table_ac_p2p_to_ac_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_PWE_TBL_ENTRY_SIZE ARAD_PP_IHP_LIF_TABLE_ENTRY_SIZE

uint32
  arad_pp_ihp_lif_table_ac_p2p_to_pwe_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_PWE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_PWE_TBL_ENTRY_SIZE],
    bank_id,
    entry_offset_in_bank;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_PWE_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_PWE_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_PWE_TBL_DATA, 1);

  bank_id = LIF_TABLE_ENTRY_OFFSET_TO_BANK_ID(unit, entry_offset);
  entry_offset_in_bank = LIF_TABLE_ENTRY_OFFSET_TO_OFFSET_IN_BANK(unit, entry_offset);


  res = soc_mem_array_read(unit, PPDB_B_LIF_TABLE_AC_2_EEIm, bank_id, MEM_BLOCK_ANY, entry_offset_in_bank, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);
  
  tbl_data->destination = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_AC_2_EEIm, data, DESTINATIONf);
  tbl_data->destination_valid = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_AC_2_EEIm, data, DESTINATION_VALIDf);
  tbl_data->eei_or_out_lif = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_AC_2_EEIm, data, EEI_OR_OUT_LIFf);
  tbl_data->eei_or_out_lif_identifier = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_AC_2_EEIm, data, EEI_OR_OUT_LIF_TYPEf);
  tbl_data->vlan_edit_vid_1 = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_AC_2_EEIm, data, VLAN_EDIT_VID_1f);
  tbl_data->vlan_edit_pcp_dei_profile = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_AC_2_EEIm, data, VLAN_EDIT_PCP_DEI_PROFILEf);
  tbl_data->vlan_edit_profile = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_AC_2_EEIm, data, VLAN_EDIT_PROFILEf);
  tbl_data->orientation_is_hub = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_AC_2_EEIm, data, ORIENTATION_IS_HUBf);
  tbl_data->cos_profile = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_AC_2_EEIm, data, COS_PROFILEf);
  tbl_data->in_lif_profile = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_AC_2_EEIm, data, IN_LIF_PROFILEf);
  tbl_data->vsi_assignment_mode = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_AC_2_EEIm, data, VSI_ASSIGNMENT_MODEf);
  tbl_data->oam_lif_set = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_AC_2_EEIm, data, OAM_LIF_SETf);
  tbl_data->protection_path = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_AC_2_EEIm, data, PROTECTION_PATHf);
  tbl_data->protection_pointer = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_AC_2_EEIm, data, PROTECTION_POINTERf);
  tbl_data->type = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_AC_2_EEIm, data, TYPEf);

    /* Some fields have different names in Jericho, or don't exist in Jericho, or exist only in Jericho. */
  if (SOC_IS_JERICHO(unit)) {
      tbl_data->sys_in_lif =            soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_AC_2_EEIm, data, SYS_IN_LIFf);
  } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      tbl_data->use_in_lif = soc_mem_field32_get(unit, IHP_LIF_TABLE_AC_P2P_TO_PWEm, data, USE_IN_LIFf);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_lif_table_ac_p2p_to_pwe_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_lif_table_ac_p2p_to_pwe_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_PWE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK,
    entry_offset_in_bank,
    bank_id;
  uint32
    data[ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_PWE_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_PWE_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_PWE_TBL_ENTRY_SIZE);

  /* The name of the table, and some fields are different in Jericho. Handle them here. */
  if (SOC_IS_JERICHO(unit)) {
      soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_2_EEIm, data, SYS_IN_LIFf, tbl_data->sys_in_lif);
  } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      soc_mem_field32_set(unit, IHP_LIF_TABLE_AC_P2P_TO_PWEm, data, USE_IN_LIFf, tbl_data->use_in_lif);
  } 


  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_2_EEIm, data, DESTINATIONf, tbl_data->destination);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_2_EEIm, data, DESTINATION_VALIDf, tbl_data->destination_valid);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_2_EEIm, data, EEI_OR_OUT_LIFf, tbl_data->eei_or_out_lif);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_2_EEIm, data, EEI_OR_OUT_LIF_TYPEf, tbl_data->eei_or_out_lif_identifier);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_2_EEIm, data, VLAN_EDIT_VID_1f, tbl_data->vlan_edit_vid_1);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_2_EEIm, data, VLAN_EDIT_PCP_DEI_PROFILEf, tbl_data->vlan_edit_pcp_dei_profile);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_2_EEIm, data, VLAN_EDIT_PROFILEf, tbl_data->vlan_edit_profile);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_2_EEIm, data, ORIENTATION_IS_HUBf, tbl_data->orientation_is_hub);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_2_EEIm, data, COS_PROFILEf, tbl_data->cos_profile);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_2_EEIm, data, IN_LIF_PROFILEf, tbl_data->in_lif_profile);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_2_EEIm, data, VSI_ASSIGNMENT_MODEf, tbl_data->vsi_assignment_mode);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_2_EEIm, data, OAM_LIF_SETf, tbl_data->oam_lif_set);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_2_EEIm, data, PROTECTION_PATHf, tbl_data->protection_path);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_2_EEIm, data, PROTECTION_POINTERf, tbl_data->protection_pointer);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_2_EEIm, data, TYPEf, tbl_data->type);

  bank_id = LIF_TABLE_ENTRY_OFFSET_TO_BANK_ID(unit, entry_offset);
  entry_offset_in_bank = LIF_TABLE_ENTRY_OFFSET_TO_OFFSET_IN_BANK(unit, entry_offset);

  res = soc_mem_array_write(unit, PPDB_B_LIF_TABLE_AC_2_EEIm, bank_id, MEM_BLOCK_ANY, entry_offset_in_bank, data);  
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_lif_table_ac_p2p_to_pwe_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_PBB_TBL_ENTRY_SIZE ARAD_PP_IHP_LIF_TABLE_ENTRY_SIZE

uint32
  arad_pp_ihp_lif_table_ac_p2p_to_pbb_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_PBB_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK,
    entry_offset_in_bank,
    bank_id;
  uint32
    data[ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_PBB_TBL_ENTRY_SIZE];
  soc_mem_t lif_mem = (SOC_IS_JERICHO(unit)) ? PPDB_B_LIF_TABLE_AC_2_EEIm : IHP_LIF_TABLE_AC_P2P_TO_PBBm;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_PBB_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_PBB_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_PBB_TBL_DATA, 1);

  bank_id = LIF_TABLE_ENTRY_OFFSET_TO_BANK_ID(unit, entry_offset);
  entry_offset_in_bank = LIF_TABLE_ENTRY_OFFSET_TO_OFFSET_IN_BANK(unit, entry_offset);

  res = soc_mem_array_read(unit, lif_mem, bank_id, MEM_BLOCK_ANY, entry_offset_in_bank, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->destination = soc_mem_field32_get(unit, lif_mem, data, DESTINATIONf);
  tbl_data->destination_valid = soc_mem_field32_get(unit, lif_mem, data, DESTINATION_VALIDf);
  tbl_data->vlan_edit_vid_1 = soc_mem_field32_get(unit, lif_mem, data, VLAN_EDIT_VID_1f);
  tbl_data->vlan_edit_pcp_dei_profile = soc_mem_field32_get(unit, lif_mem, data, VLAN_EDIT_PCP_DEI_PROFILEf);
  tbl_data->vlan_edit_profile = soc_mem_field32_get(unit, lif_mem, data, VLAN_EDIT_PROFILEf);
  tbl_data->orientation_is_hub = soc_mem_field32_get(unit, lif_mem, data, ORIENTATION_IS_HUBf);
  tbl_data->cos_profile = soc_mem_field32_get(unit, lif_mem, data, COS_PROFILEf);
  tbl_data->in_lif_profile = soc_mem_field32_get(unit, lif_mem, data, IN_LIF_PROFILEf);
  tbl_data->vsi_assignment_mode = soc_mem_field32_get(unit, lif_mem, data, VSI_ASSIGNMENT_MODEf);
  tbl_data->oam_lif_set = soc_mem_field32_get(unit, lif_mem, data, OAM_LIF_SETf);
  tbl_data->protection_path = soc_mem_field32_get(unit, lif_mem, data, PROTECTION_PATHf);
  tbl_data->protection_pointer = soc_mem_field32_get(unit, lif_mem, data, PROTECTION_POINTERf);
  tbl_data->type = soc_mem_field32_get(unit, lif_mem, data, TYPEf);

  if (SOC_IS_JERICHO(unit)) {
      tbl_data->sys_in_lif =            soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_AC_2_EEIm, data, SYS_IN_LIFf);
      tbl_data->eei =                   soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_AC_2_EEIm, data, EEI_OR_OUT_LIFf);
      tbl_data->eei_or_out_lif_type = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_AC_2_EEIm, data, EEI_OR_OUT_LIF_TYPEf);
  } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      tbl_data->use_in_lif = soc_mem_field32_get(unit, IHP_LIF_TABLE_AC_P2P_TO_PBBm, data, USE_IN_LIFf);
      tbl_data->eei = soc_mem_field32_get(unit, IHP_LIF_TABLE_AC_P2P_TO_PBBm, data, EEIf);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_lif_table_ac_p2p_to_pbb_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_lif_table_ac_p2p_to_pbb_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_PBB_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK,
    entry_offset_in_bank,
    bank_id;
  uint32
    data[ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_PBB_TBL_ENTRY_SIZE];
  soc_mem_t lif_mem = (SOC_IS_JERICHO(unit)) ? PPDB_B_LIF_TABLE_AC_2_EEIm : IHP_LIF_TABLE_AC_P2P_TO_PBBm;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_PBB_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_LIF_TABLE_AC_P2P_TO_PBB_TBL_ENTRY_SIZE);

  /* The name of the table, and some fields are different in Jericho. Handle them here. */
  if (SOC_IS_JERICHO(unit)) {
      soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_2_EEIm, data, SYS_IN_LIFf, tbl_data->sys_in_lif);
      soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_2_EEIm, data, DOUBLE_DATA_ENTRYf, 0 /* Not configurable */);
      soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_2_EEIm, data, EEI_OR_OUT_LIF_TYPEf, tbl_data->eei_or_out_lif_type);
  } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      soc_mem_field32_set(unit, IHP_LIF_TABLE_AC_P2P_TO_PBBm, data, USE_IN_LIFf, tbl_data->use_in_lif);
      soc_mem_field32_set(unit, IHP_LIF_TABLE_AC_P2P_TO_PBBm, data, EEIf, tbl_data->eei);
  } 
  
  soc_mem_field32_set(unit, lif_mem, data, DESTINATIONf, tbl_data->destination);
  soc_mem_field32_set(unit, lif_mem, data, DESTINATION_VALIDf, tbl_data->destination_valid);
  soc_mem_field32_set(unit, lif_mem, data, VLAN_EDIT_VID_1f, tbl_data->vlan_edit_vid_1);
  soc_mem_field32_set(unit, lif_mem, data, VLAN_EDIT_PCP_DEI_PROFILEf, tbl_data->vlan_edit_pcp_dei_profile);
  soc_mem_field32_set(unit, lif_mem, data, VLAN_EDIT_PROFILEf, tbl_data->vlan_edit_profile);
  soc_mem_field32_set(unit, lif_mem, data, ORIENTATION_IS_HUBf, tbl_data->orientation_is_hub);
  soc_mem_field32_set(unit, lif_mem, data, COS_PROFILEf, tbl_data->cos_profile);
  soc_mem_field32_set(unit, lif_mem, data, IN_LIF_PROFILEf, tbl_data->in_lif_profile);
  soc_mem_field32_set(unit, lif_mem, data, VSI_ASSIGNMENT_MODEf, tbl_data->vsi_assignment_mode);
  soc_mem_field32_set(unit, lif_mem, data, OAM_LIF_SETf, tbl_data->oam_lif_set);
  soc_mem_field32_set(unit, lif_mem, data, PROTECTION_PATHf, tbl_data->protection_path);
  soc_mem_field32_set(unit, lif_mem, data, PROTECTION_POINTERf, tbl_data->protection_pointer);
  soc_mem_field32_set(unit, lif_mem, data, TYPEf, tbl_data->type);

  bank_id = LIF_TABLE_ENTRY_OFFSET_TO_BANK_ID(unit, entry_offset);
  entry_offset_in_bank = LIF_TABLE_ENTRY_OFFSET_TO_OFFSET_IN_BANK(unit, entry_offset);

  res = soc_mem_array_write(unit, PPDB_B_LIF_TABLE_AC_2_EEIm, bank_id, MEM_BLOCK_ANY, entry_offset_in_bank, data);  
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_lif_table_ac_p2p_to_pbb_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHP_LIF_TABLE_AC_MP_TBL_ENTRY_SIZE ARAD_PP_IHP_LIF_TABLE_ENTRY_SIZE

uint32
  arad_pp_ihp_lif_table_ac_mp_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_LIF_TABLE_AC_MP_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK,
    bank_id,
    entry_offset_in_bank;
  uint32
    data[ARAD_PP_IHP_LIF_TABLE_AC_MP_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_LIF_TABLE_AC_MP_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_LIF_TABLE_AC_MP_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_LIF_TABLE_AC_MP_TBL_DATA, 1);

  bank_id = LIF_TABLE_ENTRY_OFFSET_TO_BANK_ID(unit, entry_offset);
  entry_offset_in_bank = LIF_TABLE_ENTRY_OFFSET_TO_OFFSET_IN_BANK(unit, entry_offset);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 10, exit, soc_mem_array_read(unit, PPDB_B_LIF_TABLE_AC_MPm, bank_id, MEM_BLOCK_ANY, entry_offset_in_bank, data));  

  tbl_data->vsi = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_AC_MPm, data, VSIf);
  tbl_data->learn_info = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_AC_MPm, data, LEARN_INFOf);
  tbl_data->tt_learn_enable = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_AC_MPm, data, TT_LEARN_ENABLEf);
  tbl_data->da_not_found_profile = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_AC_MPm, data, DA_NOT_FOUND_PROFILEf);
  tbl_data->vlan_edit_vid_2 = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_AC_MPm, data, VLAN_EDIT_VID_2f);
  tbl_data->vlan_edit_vid_1 = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_AC_MPm, data, VLAN_EDIT_VID_1f);
  tbl_data->vlan_edit_pcp_dei_profile = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_AC_MPm, data, VLAN_EDIT_PCP_DEI_PROFILEf);
  tbl_data->vlan_edit_profile = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_AC_MPm, data, VLAN_EDIT_PROFILEf);
  tbl_data->orientation_is_hub = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_AC_MPm, data, ORIENTATION_IS_HUBf);
  tbl_data->cos_profile = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_AC_MPm, data, COS_PROFILEf);
  tbl_data->in_lif_profile = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_AC_MPm, data, IN_LIF_PROFILEf);
  tbl_data->vsi_assignment_mode = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_AC_MPm, data, VSI_ASSIGNMENT_MODEf);
  tbl_data->oam_lif_set = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_AC_MPm, data, OAM_LIF_SETf);
  tbl_data->protection_path = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_AC_MPm, data, PROTECTION_PATHf);
  tbl_data->protection_pointer = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_AC_MPm, data, PROTECTION_POINTERf);
  tbl_data->type = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_AC_MPm, data, TYPEf);

  /* Some fields have different names in Jericho, or don't exist in Jericho, or exist only in Jericho. */
  if (SOC_IS_JERICHO(unit)) {
      tbl_data->sys_in_lif =            soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_AC_MPm, data, SYS_IN_LIFf);
  } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      tbl_data->use_in_lif = soc_mem_field32_get(unit, IHP_LIF_TABLE_AC_MPm, data, USE_IN_LIFf);
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_lif_table_ac_mp_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_lif_table_ac_mp_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_LIF_TABLE_AC_MP_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK,
    entry_offset_in_bank,
    bank_id;
  uint32
    data[ARAD_PP_IHP_LIF_TABLE_AC_MP_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_LIF_TABLE_AC_MP_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_LIF_TABLE_AC_MP_TBL_ENTRY_SIZE);

    /* The name of the table, and some fields are different in Jericho. Handle them here. */
  if (SOC_IS_JERICHO(unit)) {
      soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_MPm, data, SYS_IN_LIFf, tbl_data->sys_in_lif);
      soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_MPm, data, DOUBLE_DATA_ENTRYf, 0 /* Not configurable */);
  } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      soc_mem_field32_set(unit, IHP_LIF_TABLE_AC_MPm, data, USE_IN_LIFf, tbl_data->use_in_lif);
  } 


  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_MPm, data, VSIf, tbl_data->vsi);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_MPm, data, LEARN_INFOf, tbl_data->learn_info);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_MPm, data, TT_LEARN_ENABLEf, tbl_data->tt_learn_enable);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_MPm, data, DA_NOT_FOUND_PROFILEf, tbl_data->da_not_found_profile);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_MPm, data, VLAN_EDIT_VID_2f, tbl_data->vlan_edit_vid_2);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_MPm, data, VLAN_EDIT_VID_1f, tbl_data->vlan_edit_vid_1);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_MPm, data, VLAN_EDIT_PCP_DEI_PROFILEf, tbl_data->vlan_edit_pcp_dei_profile);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_MPm, data, VLAN_EDIT_PROFILEf, tbl_data->vlan_edit_profile);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_MPm, data, ORIENTATION_IS_HUBf, tbl_data->orientation_is_hub);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_MPm, data, COS_PROFILEf, tbl_data->cos_profile);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_MPm, data, IN_LIF_PROFILEf, tbl_data->in_lif_profile);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_MPm, data, VSI_ASSIGNMENT_MODEf, tbl_data->vsi_assignment_mode);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_MPm, data, OAM_LIF_SETf, tbl_data->oam_lif_set);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_MPm, data, PROTECTION_PATHf, tbl_data->protection_path);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_MPm, data, PROTECTION_POINTERf, tbl_data->protection_pointer);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_AC_MPm, data, TYPEf, tbl_data->type);

  bank_id = LIF_TABLE_ENTRY_OFFSET_TO_BANK_ID(unit, entry_offset);
  entry_offset_in_bank = LIF_TABLE_ENTRY_OFFSET_TO_OFFSET_IN_BANK(unit, entry_offset);

  res = soc_mem_array_write(unit, PPDB_B_LIF_TABLE_AC_MPm, bank_id, MEM_BLOCK_ANY, entry_offset_in_bank, data);  
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_lif_table_ac_mp_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHP_LIF_TABLE_ISID_P2P_TBL_ENTRY_SIZE ARAD_PP_IHP_LIF_TABLE_ENTRY_SIZE

uint32
  arad_pp_ihp_lif_table_isid_p2p_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_LIF_TABLE_ISID_P2P_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK,
    entry_offset_in_bank,
    bank_id;
  uint32
    data[ARAD_PP_IHP_LIF_TABLE_ISID_P2P_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_LIF_TABLE_ISID_P2P_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_LIF_TABLE_ISID_P2P_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_LIF_TABLE_ISID_P2P_TBL_DATA, 1);

  bank_id = LIF_TABLE_ENTRY_OFFSET_TO_BANK_ID(unit, entry_offset);
  entry_offset_in_bank = LIF_TABLE_ENTRY_OFFSET_TO_OFFSET_IN_BANK(unit, entry_offset);

  res = soc_mem_array_read(unit, PPDB_B_LIF_TABLE_ISID_P2Pm, bank_id, MEM_BLOCK_ANY, entry_offset_in_bank, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->orientation_is_hub = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_ISID_P2Pm, data, ORIENTATION_IS_HUBf);
  tbl_data->eei_or_out_lif_lsb = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_ISID_P2Pm, data, EEI_OR_OUT_LIF_LSBf);
  tbl_data->tpid_profile = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_ISID_P2Pm, data, TPID_PROFILEf);
  tbl_data->cos_profile = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_ISID_P2Pm, data, COS_PROFILEf);
  tbl_data->vsi = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_ISID_P2Pm, data, VSIf);
  tbl_data->destination = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_ISID_P2Pm, data, DESTINATIONf);
  tbl_data->destination_valid = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_ISID_P2Pm, data, DESTINATION_VALIDf);
  tbl_data->eei_or_out_lif_type = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_ISID_P2Pm, data, EEI_OR_OUT_LIF_TYPEf);
  tbl_data->vsi_assignment_mode = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_ISID_P2Pm, data, VSI_ASSIGNMENT_MODEf);
  tbl_data->oam_lif_set = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_ISID_P2Pm, data, OAM_LIF_SETf);
  tbl_data->protection_path = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_ISID_P2Pm, data, PROTECTION_PATHf);
  tbl_data->protection_pointer = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_ISID_P2Pm, data, PROTECTION_POINTERf);
  tbl_data->use_in_lif = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_ISID_P2Pm, data, USE_IN_LIFf);
  tbl_data->service_type_lsb = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_ISID_P2Pm, data, SERVICE_TYPE_LSBf);
  tbl_data->type = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_ISID_P2Pm, data, TYPEf);
  tbl_data->eei_or_out_lif = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_ISID_P2Pm, data, EEI_OR_OUT_LIF_MSBf);

  /* Some fields have different names in Jericho, or don't exist in Jericho, or exist only in Jericho. */
  if (SOC_IS_JERICHO(unit)) {
      tbl_data->sys_in_lif =            soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_ISID_P2Pm, data, SYS_IN_LIFf);
      tbl_data->in_lif_profile =        soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_ISID_P2Pm, data, IN_LIF_PROFILEf);
  } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      tbl_data->use_in_lif = soc_mem_field32_get(unit, IHP_LIF_TABLE_ISID_P2Pm, data, USE_IN_LIFf);
      tbl_data->service_type_lsb = soc_mem_field32_get(unit, IHP_LIF_TABLE_ISID_P2Pm, data, SERVICE_TYPE_LSBf);
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_lif_table_isid_p2p_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_lif_table_isid_p2p_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_LIF_TABLE_ISID_P2P_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK,
    entry_offset_in_bank,
    bank_id;
  uint32
    data[ARAD_PP_IHP_LIF_TABLE_ISID_P2P_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_LIF_TABLE_ISID_P2P_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_LIF_TABLE_ISID_P2P_TBL_ENTRY_SIZE);

  /* The name of the table, and some fields are different in Jericho. Handle them here. */
  if (SOC_IS_JERICHO(unit)) {
      soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_ISID_P2Pm, data, SYS_IN_LIFf, tbl_data->sys_in_lif);
      soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_ISID_P2Pm, data, DOUBLE_DATA_ENTRYf, 0 /* Not configurable */);
      soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_ISID_P2Pm, data, IN_LIF_PROFILEf, tbl_data->in_lif_profile);
  } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      soc_mem_field32_set(unit, IHP_LIF_TABLE_ISID_P2Pm, data, USE_IN_LIFf, tbl_data->use_in_lif);
      soc_mem_field32_set(unit, IHP_LIF_TABLE_ISID_P2Pm, data, SERVICE_TYPE_LSBf, tbl_data->service_type_lsb);
  }

  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_ISID_P2Pm, data, EEI_OR_OUT_LIF_MSBf, tbl_data->eei_or_out_lif);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_ISID_P2Pm, data, ORIENTATION_IS_HUBf, tbl_data->orientation_is_hub);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_ISID_P2Pm, data, EEI_OR_OUT_LIF_LSBf, tbl_data->eei_or_out_lif_lsb);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_ISID_P2Pm, data, TPID_PROFILEf, tbl_data->tpid_profile);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_ISID_P2Pm, data, COS_PROFILEf, tbl_data->cos_profile);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_ISID_P2Pm, data, VSIf, tbl_data->vsi);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_ISID_P2Pm, data, DESTINATIONf, tbl_data->destination);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_ISID_P2Pm, data, DESTINATION_VALIDf, tbl_data->destination_valid);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_ISID_P2Pm, data, EEI_OR_OUT_LIF_TYPEf, tbl_data->eei_or_out_lif_type);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_ISID_P2Pm, data, VSI_ASSIGNMENT_MODEf, tbl_data->vsi_assignment_mode);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_ISID_P2Pm, data, OAM_LIF_SETf, tbl_data->oam_lif_set);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_ISID_P2Pm, data, PROTECTION_PATHf, tbl_data->protection_path);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_ISID_P2Pm, data, PROTECTION_POINTERf, tbl_data->protection_pointer);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_ISID_P2Pm, data, USE_IN_LIFf, tbl_data->use_in_lif);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_ISID_P2Pm, data, SERVICE_TYPE_LSBf, tbl_data->service_type_lsb);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_ISID_P2Pm, data, TYPEf, tbl_data->type);

  bank_id = LIF_TABLE_ENTRY_OFFSET_TO_BANK_ID(unit, entry_offset);
  entry_offset_in_bank = LIF_TABLE_ENTRY_OFFSET_TO_OFFSET_IN_BANK(unit, entry_offset);

  res = soc_mem_array_write(unit, PPDB_B_LIF_TABLE_ISID_P2Pm, bank_id, MEM_BLOCK_ANY, entry_offset_in_bank, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_lif_table_isid_p2p_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHP_LIF_TABLE_ISID_MP_TBL_ENTRY_SIZE ARAD_PP_IHP_LIF_TABLE_ENTRY_SIZE

uint32
  arad_pp_ihp_lif_table_isid_mp_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_LIF_TABLE_ISID_MP_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK,
    entry_offset_in_bank,
    bank_id;
  uint32
    data[ARAD_PP_IHP_LIF_TABLE_ISID_MP_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_LIF_TABLE_ISID_MP_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_LIF_TABLE_ISID_MP_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_LIF_TABLE_ISID_MP_TBL_DATA, 1);

  bank_id = LIF_TABLE_ENTRY_OFFSET_TO_BANK_ID(unit, entry_offset);
  entry_offset_in_bank = LIF_TABLE_ENTRY_OFFSET_TO_OFFSET_IN_BANK(unit, entry_offset);

  res = soc_mem_array_read(unit, PPDB_B_LIF_TABLE_ISID_MPm, bank_id, MEM_BLOCK_ANY, entry_offset_in_bank, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->orientation_is_hub = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_ISID_MPm, data, ORIENTATION_IS_HUBf);
  tbl_data->tpid_profile = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_ISID_MPm, data, TPID_PROFILEf);
  tbl_data->cos_profile = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_ISID_MPm, data, COS_PROFILEf);
  tbl_data->vsi = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_ISID_MPm, data, VSIf);
  tbl_data->da_not_found_profile = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_ISID_MPm, data, DA_NOT_FOUND_PROFILEf);
  tbl_data->destination = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_ISID_MPm, data, DESTINATIONf);
  tbl_data->destination_valid = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_ISID_MPm, data, DESTINATION_VALIDf);
  tbl_data->tt_learn_enable = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_ISID_MPm, data, TT_LEARN_ENABLEf);
  tbl_data->vsi_assignment_mode = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_ISID_MPm, data, VSI_ASSIGNMENT_MODEf);
  tbl_data->oam_lif_set = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_ISID_MPm, data, OAM_LIF_SETf);
  tbl_data->protection_path = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_ISID_MPm, data, PROTECTION_PATHf);
  tbl_data->protection_pointer = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_ISID_MPm, data, PROTECTION_POINTERf);
  tbl_data->type = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_ISID_MPm, data, TYPEf);

  /* Some fields have different names in Jericho, or don't exist in Jericho, or exist only in Jericho. */
  if (SOC_IS_JERICHO(unit)) {
      tbl_data->sys_in_lif =            soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_ISID_MPm, data, SYS_IN_LIFf);
      tbl_data->in_lif_profile =        soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_ISID_MPm, data, IN_LIF_PROFILEf);
  } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      tbl_data->use_in_lif = soc_mem_field32_get(unit, IHP_LIF_TABLE_ISID_MPm, data, USE_IN_LIFf);
      tbl_data->service_type_lsb = soc_mem_field32_get(unit, IHP_LIF_TABLE_ISID_MPm, data, SERVICE_TYPE_LSBf);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_lif_table_isid_mp_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_lif_table_isid_mp_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_LIF_TABLE_ISID_MP_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK,
    entry_offset_in_bank,
    bank_id;
  uint32
    data[ARAD_PP_IHP_LIF_TABLE_ISID_MP_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_LIF_TABLE_ISID_MP_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_LIF_TABLE_ISID_MP_TBL_ENTRY_SIZE);

  /* The name of the table, and some fields are different in Jericho. Handle them here. */
  if (SOC_IS_JERICHO(unit)) {
      soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_ISID_MPm, data, SYS_IN_LIFf, tbl_data->sys_in_lif);
      soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_ISID_MPm, data, DOUBLE_DATA_ENTRYf, 0 /* Not configurable */);
      soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_ISID_MPm, data, IN_LIF_PROFILEf, tbl_data->in_lif_profile );
  } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      soc_mem_field32_set(unit, IHP_LIF_TABLE_ISID_MPm, data, USE_IN_LIFf, tbl_data->use_in_lif);
      soc_mem_field32_set(unit, IHP_LIF_TABLE_ISID_MPm, data, SERVICE_TYPE_LSBf, tbl_data->service_type_lsb);
  } 

  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_ISID_MPm, data, ORIENTATION_IS_HUBf, tbl_data->orientation_is_hub);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_ISID_MPm, data, TPID_PROFILEf, tbl_data->tpid_profile);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_ISID_MPm, data, COS_PROFILEf, tbl_data->cos_profile);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_ISID_MPm, data, VSIf, tbl_data->vsi);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_ISID_MPm, data, DA_NOT_FOUND_PROFILEf, tbl_data->da_not_found_profile);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_ISID_MPm, data, DESTINATIONf, tbl_data->destination);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_ISID_MPm, data, DESTINATION_VALIDf, tbl_data->destination_valid);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_ISID_MPm, data, TT_LEARN_ENABLEf, tbl_data->tt_learn_enable);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_ISID_MPm, data, VSI_ASSIGNMENT_MODEf, tbl_data->vsi_assignment_mode);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_ISID_MPm, data, OAM_LIF_SETf, tbl_data->oam_lif_set);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_ISID_MPm, data, PROTECTION_PATHf, tbl_data->protection_path);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_ISID_MPm, data, PROTECTION_POINTERf, tbl_data->protection_pointer);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_ISID_MPm, data, TYPEf, tbl_data->type);

  bank_id = LIF_TABLE_ENTRY_OFFSET_TO_BANK_ID(unit, entry_offset);
  entry_offset_in_bank = LIF_TABLE_ENTRY_OFFSET_TO_OFFSET_IN_BANK(unit, entry_offset);

  res = soc_mem_array_write(unit, PPDB_B_LIF_TABLE_ISID_MPm, bank_id, MEM_BLOCK_ANY, entry_offset_in_bank, data);  
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_lif_table_isid_mp_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHP_LIF_TABLE_TRILL_TBL_ENTRY_SIZE ARAD_PP_IHP_LIF_TABLE_ENTRY_SIZE

uint32
  arad_pp_ihp_lif_table_trill_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_LIF_TABLE_TRILL_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK,
    entry_offset_in_bank,
    bank_id;
  uint32
    data[ARAD_PP_IHP_LIF_TABLE_TRILL_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_LIF_TABLE_TRILL_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_LIF_TABLE_TRILL_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_LIF_TABLE_TRILL_TBL_DATA, 1);

  bank_id = LIF_TABLE_ENTRY_OFFSET_TO_BANK_ID(unit, entry_offset);
  entry_offset_in_bank = LIF_TABLE_ENTRY_OFFSET_TO_OFFSET_IN_BANK(unit, entry_offset);

  res = soc_mem_array_read(unit, PPDB_B_LIF_TABLE_TRILLm, bank_id, MEM_BLOCK_ANY, entry_offset_in_bank, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->tpid_profile = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_TRILLm, data, TPID_PROFILEf);
  tbl_data->vsi = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_TRILLm, data, VSIf);
  /* Until arad+: learn info is fec only. */
  if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      uint32 learn_info = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_TRILLm, data, LEARN_INFOf);
      COMPILER_64_SET(tbl_data->learn_info, 0,learn_info); 
  } 
  /* for jericho: learn info is fec + eei */
  else {
      soc_mem_field64_get(unit, PPDB_B_LIF_TABLE_TRILLm, data, LEARN_DATAf, &tbl_data->learn_info);
  }
  tbl_data->service_type = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_TRILLm, data, SERVICE_TYPEf);
  tbl_data->tt_learn_enable = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_TRILLm, data, TT_LEARN_ENABLEf);
  tbl_data->vsi_assignment_mode = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_TRILLm, data, VSI_ASSIGNMENT_MODEf);
  tbl_data->oam_lif_set = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_TRILLm, data, OAM_LIF_SETf);
  tbl_data->protection_path = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_TRILLm, data, PROTECTION_PATHf);
  tbl_data->protection_pointer = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_TRILLm, data, PROTECTION_POINTERf);

  if (SOC_IS_JERICHO(unit)) {
      tbl_data->orientation_is_hub = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_TRILLm, data, ORIENTATION_IS_HUBf);
      tbl_data->orientation_is_hub_valid = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_TRILLm, data, ORIENTATION_IS_HUB_VALIDf);
      tbl_data->in_lif_profile = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_TRILLm, data, IN_LIF_PROFILEf);
      tbl_data->in_lif_profile_valid = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_TRILLm, data, IN_LIF_PROFILE_VALIDf);
      tbl_data->sys_in_lif = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_TRILLm, data, SYS_IN_LIFf);    
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_lif_table_trill_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_lif_table_trill_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_LIF_TABLE_TRILL_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK,
    entry_offset_in_bank,
    bank_id;
  uint32
    data[ARAD_PP_IHP_LIF_TABLE_TRILL_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_LIF_TABLE_TRILL_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_LIF_TABLE_TRILL_TBL_ENTRY_SIZE);


    /* The name of the table is different and some fields exist only on Jericho. Set the right table id and handle the fields. */
  if (SOC_IS_JERICHO(unit)) {
      soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_TRILLm, data, ORIENTATION_IS_HUBf, tbl_data->orientation_is_hub);
      soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_TRILLm, data, ORIENTATION_IS_HUB_VALIDf, tbl_data->orientation_is_hub_valid);
      soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_TRILLm, data, IN_LIF_PROFILEf, tbl_data->in_lif_profile);
      soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_TRILLm, data, IN_LIF_PROFILE_VALIDf, tbl_data->in_lif_profile_valid);
      soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_TRILLm, data, SYS_IN_LIFf, tbl_data->sys_in_lif);
      soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_TRILLm, data, DOUBLE_DATA_ENTRYf, 0);
  } 

  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_TRILLm, data, TPID_PROFILEf, tbl_data->tpid_profile);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_TRILLm, data, VSIf, tbl_data->vsi);
  /* Until arad+: learn info is fec only. */
  if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      uint32 learn_info = COMPILER_64_LO(tbl_data->learn_info); 
      soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_TRILLm, data, LEARN_INFOf, learn_info);
  } 
  /* for jericho: learn info is fec + eei */
  else if (SOC_IS_JERICHO(unit)) {
      /* learn info in Jericho supports only forwarding payload format in arad mode \
         Until the code is ready to got the learn info in the right format, don't save learn info */
/*    soc_mem_field64_set(unit, PPDB_B_LIF_TABLE_TRILLm, data, LEARN_DATAf, tbl_data->learn_info);*/
  }
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_TRILLm, data, SERVICE_TYPEf, tbl_data->service_type);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_TRILLm, data, TT_LEARN_ENABLEf, tbl_data->tt_learn_enable);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_TRILLm, data, VSI_ASSIGNMENT_MODEf, tbl_data->vsi_assignment_mode);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_TRILLm, data, OAM_LIF_SETf, tbl_data->oam_lif_set);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_TRILLm, data, PROTECTION_PATHf, tbl_data->protection_path);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_TRILLm, data, PROTECTION_POINTERf, tbl_data->protection_pointer);

  bank_id = LIF_TABLE_ENTRY_OFFSET_TO_BANK_ID(unit, entry_offset);
  entry_offset_in_bank = LIF_TABLE_ENTRY_OFFSET_TO_OFFSET_IN_BANK(unit, entry_offset);

  res = soc_mem_array_write(unit, PPDB_B_LIF_TABLE_TRILLm, bank_id, MEM_BLOCK_ANY, entry_offset_in_bank, data);  
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_lif_table_trill_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHP_LIF_TABLE_IP_TT_TBL_ENTRY_SIZE ARAD_PP_IHP_LIF_TABLE_ENTRY_SIZE

uint32
  arad_pp_ihp_lif_table_ip_tt_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_LIF_TABLE_IP_TT_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK,
    entry_offset_in_bank,
    bank_id;
  uint32
    data[ARAD_PP_IHP_LIF_TABLE_IP_TT_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_LIF_TABLE_IP_TT_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_LIF_TABLE_IP_TT_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_LIF_TABLE_IP_TT_TBL_DATA, 1);

  bank_id = LIF_TABLE_ENTRY_OFFSET_TO_BANK_ID(unit, entry_offset);
  entry_offset_in_bank = LIF_TABLE_ENTRY_OFFSET_TO_OFFSET_IN_BANK(unit, entry_offset);

  res = soc_mem_array_read(unit, PPDB_B_LIF_TABLE_IP_TTm, bank_id, MEM_BLOCK_ANY, entry_offset_in_bank, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->orientation_is_hub = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_IP_TTm, data, ORIENTATION_IS_HUBf);
  tbl_data->model_is_pipe = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_IP_TTm, data, MODEL_IS_PIPEf);
  tbl_data->in_lif_profile = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_IP_TTm, data, IN_LIF_PROFILEf);
  tbl_data->tpid_profile = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_IP_TTm, data, TPID_PROFILEf);
  tbl_data->cos_profile = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_IP_TTm, data, COS_PROFILEf);
  tbl_data->da_not_found_profile = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_IP_TTm, data, DA_NOT_FOUND_PROFILEf);
  tbl_data->vsi = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_IP_TTm, data, VSIf);
  tbl_data->in_rif = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_IP_TTm, data, IN_RIFf);
  tbl_data->in_rif_valid = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_IP_TTm, data, IN_RIF_VALIDf);
  tbl_data->learn_info = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_IP_TTm, data, LEARN_INFOf);
  tbl_data->service_type = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_IP_TTm, data, SERVICE_TYPEf);
  tbl_data->tt_learn_enable = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_IP_TTm, data, TT_LEARN_ENABLEf);
  tbl_data->vsi_assignment_mode = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_IP_TTm, data, VSI_ASSIGNMENT_MODEf);
  tbl_data->oam_lif_set = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_IP_TTm, data, OAM_LIF_SETf);
  tbl_data->protection_path = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_IP_TTm, data, PROTECTION_PATHf);
  tbl_data->protection_pointer = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_IP_TTm, data, PROTECTION_POINTERf);
#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit)) {
      if (SOC_IS_ARADPLUS_AND_BELOW(unit)) { /* Soc is not Jericho and above */
          tbl_data->use_in_lif = soc_mem_field32_get(unit, IHP_LIF_TABLE_IP_TTm, data, USE_IN_LIFf);  
      }
      tbl_data->skip_ethernet = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_IP_TTm, data, SKIP_ETHERNETf);  
  }
 
#endif /* BCM_88660_A0 */

  if (SOC_IS_JERICHO(unit)) {
      tbl_data->sys_in_lif =            soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_IP_TTm, data, SYS_IN_LIFf);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_lif_table_ip_tt_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_lif_table_ip_tt_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_LIF_TABLE_IP_TT_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK,
    entry_offset_in_bank,
    bank_id;
  uint32
    data[ARAD_PP_IHP_LIF_TABLE_IP_TT_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_LIF_TABLE_IP_TT_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_LIF_TABLE_IP_TT_TBL_ENTRY_SIZE);

    /* The name of the table, and some fields are different in Jericho. Handle them here. */
  if (SOC_IS_JERICHO(unit)) {
      soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_IP_TTm, data, SYS_IN_LIFf, tbl_data->sys_in_lif);
      soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_IP_TTm, data, DOUBLE_DATA_ENTRYf, 0 /* Not configurable */);
  } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      if (SOC_IS_ARADPLUS(unit)) {
          soc_mem_field32_set(unit, IHP_LIF_TABLE_IP_TTm, data, USE_IN_LIFf, tbl_data->use_in_lif);
      }
  } 

  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_IP_TTm, data, ORIENTATION_IS_HUBf, tbl_data->orientation_is_hub);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_IP_TTm, data, MODEL_IS_PIPEf, tbl_data->model_is_pipe);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_IP_TTm, data, IN_LIF_PROFILEf, tbl_data->in_lif_profile);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_IP_TTm, data, TPID_PROFILEf, tbl_data->tpid_profile);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_IP_TTm, data, COS_PROFILEf, tbl_data->cos_profile);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_IP_TTm, data, DA_NOT_FOUND_PROFILEf, tbl_data->da_not_found_profile);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_IP_TTm, data, VSIf, tbl_data->vsi);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_IP_TTm, data, IN_RIFf, tbl_data->in_rif);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_IP_TTm, data, IN_RIF_VALIDf, tbl_data->in_rif_valid);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_IP_TTm, data, LEARN_INFOf, tbl_data->learn_info);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_IP_TTm, data, SERVICE_TYPEf, tbl_data->service_type);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_IP_TTm, data, TT_LEARN_ENABLEf, tbl_data->tt_learn_enable);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_IP_TTm, data, VSI_ASSIGNMENT_MODEf, tbl_data->vsi_assignment_mode);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_IP_TTm, data, OAM_LIF_SETf, tbl_data->oam_lif_set);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_IP_TTm, data, PROTECTION_PATHf, tbl_data->protection_path);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_IP_TTm, data, PROTECTION_POINTERf, tbl_data->protection_pointer);
#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit)) {
    /* skip ethernet for roo */
    soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_IP_TTm, data, SKIP_ETHERNETf, tbl_data->skip_ethernet);
  }
#endif /* BCM_88660_A0 */
  

  bank_id = LIF_TABLE_ENTRY_OFFSET_TO_BANK_ID(unit, entry_offset);
  entry_offset_in_bank = LIF_TABLE_ENTRY_OFFSET_TO_OFFSET_IN_BANK(unit, entry_offset);

  res = soc_mem_array_write(unit, PPDB_B_LIF_TABLE_IP_TTm, bank_id, MEM_BLOCK_ANY, entry_offset_in_bank, data);  
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_lif_table_ip_tt_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHP_LIF_TABLE_LABEL_PWE_P2P_TBL_ENTRY_SIZE ARAD_PP_IHP_LIF_TABLE_ENTRY_SIZE

uint32
  arad_pp_ihp_lif_table_label_pwe_p2p_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_LIF_TABLE_LABEL_PWE_P2P_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK,
    bank_id,
    entry_offset_in_bank;
  uint32
    data[ARAD_PP_IHP_LIF_TABLE_LABEL_PWE_P2P_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_LIF_TABLE_LABEL_PWE_P2P_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_LIF_TABLE_LABEL_PWE_P2P_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_LIF_TABLE_LABEL_PWE_P2P_TBL_DATA, 1);

  bank_id = LIF_TABLE_ENTRY_OFFSET_TO_BANK_ID(unit, entry_offset);
  entry_offset_in_bank = LIF_TABLE_ENTRY_OFFSET_TO_OFFSET_IN_BANK(unit, entry_offset);

  res = soc_mem_array_read(unit, PPDB_B_LIF_TABLE_LABEL_PWE_P2Pm, bank_id, MEM_BLOCK_ANY, entry_offset_in_bank, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->orientation_is_hub = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PWE_P2Pm, data, ORIENTATION_IS_HUBf);
  tbl_data->action_profile_index = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PWE_P2Pm, data, ACTION_PROFILE_INDEXf);
  tbl_data->model_is_pipe = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PWE_P2Pm, data, MODEL_IS_PIPEf);
  tbl_data->in_lif_profile = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PWE_P2Pm, data, IN_LIF_PROFILEf);
  tbl_data->tpid_profile = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PWE_P2Pm, data, TPID_PROFILEf);
  tbl_data->cos_profile = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PWE_P2Pm, data, COS_PROFILEf);
  tbl_data->eei_or_out_lif = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PWE_P2Pm, data, EEI_OR_OUT_LIFf);
  tbl_data->destination = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PWE_P2Pm, data, DESTINATIONf);
  tbl_data->eei_or_out_lif_type = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PWE_P2Pm, data, EEI_OR_OUT_LIF_TYPEf);
  tbl_data->vsi_assignment_mode = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PWE_P2Pm, data, VSI_ASSIGNMENT_MODEf);
  tbl_data->oam_lif_set = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PWE_P2Pm, data, OAM_LIF_SETf);
  tbl_data->protection_path = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PWE_P2Pm, data, PROTECTION_PATHf);
  tbl_data->protection_pointer = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PWE_P2Pm, data, PROTECTION_POINTERf);
  tbl_data->type = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PWE_P2Pm, data, TYPEf);
  tbl_data->termination_profile_msb = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PWE_P2Pm, data, TERMINATION_TYPE_MSBf);
  tbl_data->termination_profile = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PWE_P2Pm, data, TERMINATION_TYPE_LSBf);

  /* Some fields have different names in Jericho, or don't exist in Jericho, or exist only in Jericho. */
  if (SOC_IS_JERICHO(unit)) {
        tbl_data->sys_in_lif =            soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PWE_P2Pm, data, SYS_IN_LIFf);
  } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        tbl_data->destination_valid = soc_mem_field32_get(unit, IHP_LIF_TABLE_LABEL_PWE_P2Pm, data, DESTINATION_VALIDf);
        tbl_data->in_lif = soc_mem_field32_get(unit, IHP_LIF_TABLE_LABEL_PWE_P2Pm, data, IN_LIFf);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_lif_table_label_pwe_p2p_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_lif_table_label_pwe_p2p_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_LIF_TABLE_LABEL_PWE_P2P_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK,
    entry_offset_in_bank,
    bank_id;
  uint32
    data[ARAD_PP_IHP_LIF_TABLE_LABEL_PWE_P2P_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_LIF_TABLE_LABEL_PWE_P2P_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_LIF_TABLE_LABEL_PWE_P2P_TBL_ENTRY_SIZE);

  /* The name of the table, and some fields are different in Jericho. Handle them here. */
  if (SOC_IS_JERICHO(unit)) {
      soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PWE_P2Pm, data, SYS_IN_LIFf, tbl_data->sys_in_lif);
      soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PWE_P2Pm, data, DOUBLE_DATA_ENTRYf, 0 /* Not configurable */);
  } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      soc_mem_field32_set(unit, IHP_LIF_TABLE_LABEL_PWE_P2Pm, data, IN_LIFf, tbl_data->in_lif);
      soc_mem_field32_set(unit, IHP_LIF_TABLE_LABEL_PWE_P2Pm, data, DESTINATION_VALIDf, tbl_data->destination_valid);
  } 

  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PWE_P2Pm, data, TERMINATION_TYPE_LSBf, tbl_data->termination_profile);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PWE_P2Pm, data, TERMINATION_TYPE_MSBf, tbl_data->termination_profile_msb);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PWE_P2Pm, data, ORIENTATION_IS_HUBf, tbl_data->orientation_is_hub);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PWE_P2Pm, data, ACTION_PROFILE_INDEXf, tbl_data->action_profile_index);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PWE_P2Pm, data, MODEL_IS_PIPEf, tbl_data->model_is_pipe);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PWE_P2Pm, data, IN_LIF_PROFILEf, tbl_data->in_lif_profile);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PWE_P2Pm, data, TPID_PROFILEf, tbl_data->tpid_profile);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PWE_P2Pm, data, COS_PROFILEf, tbl_data->cos_profile);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PWE_P2Pm, data, EEI_OR_OUT_LIFf, tbl_data->eei_or_out_lif);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PWE_P2Pm, data, DESTINATIONf, tbl_data->destination);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PWE_P2Pm, data, EEI_OR_OUT_LIF_TYPEf, tbl_data->eei_or_out_lif_type);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PWE_P2Pm, data, VSI_ASSIGNMENT_MODEf, tbl_data->vsi_assignment_mode);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PWE_P2Pm, data, OAM_LIF_SETf, tbl_data->oam_lif_set);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PWE_P2Pm, data, PROTECTION_PATHf, tbl_data->protection_path);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PWE_P2Pm, data, PROTECTION_POINTERf, tbl_data->protection_pointer);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PWE_P2Pm, data, TYPEf, tbl_data->type);

  bank_id = LIF_TABLE_ENTRY_OFFSET_TO_BANK_ID(unit, entry_offset);
  entry_offset_in_bank = LIF_TABLE_ENTRY_OFFSET_TO_OFFSET_IN_BANK(unit, entry_offset);

  res = soc_mem_array_write(unit, PPDB_B_LIF_TABLE_LABEL_PWE_P2Pm, bank_id, MEM_BLOCK_ANY, entry_offset_in_bank, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_lif_table_label_pwe_p2p_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHP_LIF_TABLE_LABEL_PWE_MP_TBL_ENTRY_SIZE ARAD_PP_IHP_LIF_TABLE_ENTRY_SIZE

uint32
  arad_pp_ihp_lif_table_label_pwe_mp_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_LIF_TABLE_LABEL_PWE_MP_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK,
    entry_offset_in_bank,
    bank_id;
  uint32
    data[ARAD_PP_IHP_LIF_TABLE_LABEL_PWE_MP_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_LIF_TABLE_LABEL_PWE_MP_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_LIF_TABLE_LABEL_PWE_MP_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_LIF_TABLE_LABEL_PWE_MP_TBL_DATA, 1);

  bank_id = LIF_TABLE_ENTRY_OFFSET_TO_BANK_ID(unit, entry_offset);
  entry_offset_in_bank = LIF_TABLE_ENTRY_OFFSET_TO_OFFSET_IN_BANK(unit, entry_offset);

  res = soc_mem_array_read(unit, PPDB_B_LIF_TABLE_LABEL_PWE_MPm, bank_id, MEM_BLOCK_ANY, entry_offset_in_bank, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->orientation_is_hub = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PWE_MPm, data, ORIENTATION_IS_HUBf);
  tbl_data->action_profile_index = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PWE_MPm, data, ACTION_PROFILE_INDEXf);
  tbl_data->termination_profile = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PWE_MPm, data, TERMINATION_TYPEf);
  tbl_data->in_lif_profile = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PWE_MPm, data, IN_LIF_PROFILEf);
  tbl_data->tpid_profile = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PWE_MPm, data, TPID_PROFILEf);
  tbl_data->cos_profile = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PWE_MPm, data, COS_PROFILEf);
  tbl_data->da_not_found_profile = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PWE_MPm, data, DA_NOT_FOUND_PROFILEf);
  tbl_data->vsi = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PWE_MPm, data, VSIf);
  soc_mem_field64_get(unit, PPDB_B_LIF_TABLE_LABEL_PWE_MPm, data, LEARN_DATAf, &tbl_data->learn_data);
  tbl_data->tt_learn_enable = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PWE_MPm, data, TT_LEARN_ENABLEf);
  tbl_data->vsi_assignment_mode = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PWE_MPm, data, VSI_ASSIGNMENT_MODEf);
  tbl_data->oam_lif_set = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PWE_MPm, data, OAM_LIF_SETf);
  tbl_data->protection_path = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PWE_MPm, data, PROTECTION_PATHf);
  tbl_data->protection_pointer = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PWE_MPm, data, PROTECTION_POINTERf);
  tbl_data->type = soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PWE_MPm, data, TYPEf);

    /* Some fields have different names in Jericho, or don't exist in Jericho, or exist only in Jericho. */
  if (SOC_IS_JERICHO(unit)) {
      tbl_data->sys_in_lif =            soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PWE_MPm, data, SYS_IN_LIFf);
  } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      tbl_data->in_lif_valid =          soc_mem_field32_get(unit, IHP_LIF_TABLE_LABEL_PWE_MPm, data, IN_LIF_VALIDf);
      tbl_data->termination_profile_msb = soc_mem_field32_get(unit, IHP_LIF_TABLE_LABEL_PWE_MPm, data, TERMINATION_PROFILE_MSBf);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_lif_table_label_pwe_mp_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_lif_table_label_pwe_mp_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_LIF_TABLE_LABEL_PWE_MP_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK,
    entry_offset_in_bank,
    bank_id;
  uint32
    data[ARAD_PP_IHP_LIF_TABLE_LABEL_PWE_MP_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_LIF_TABLE_LABEL_PWE_MP_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_LIF_TABLE_LABEL_PWE_MP_TBL_ENTRY_SIZE);

  /* The name of the table, and some fields are different in Jericho. Handle them here. */
  if (SOC_IS_JERICHO(unit)) {
      soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PWE_MPm, data, SYS_IN_LIFf, tbl_data->sys_in_lif);
      soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PWE_MPm, data, DOUBLE_DATA_ENTRYf, 0 /* Not configurable */);
  } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      soc_mem_field32_set(unit, IHP_LIF_TABLE_LABEL_PWE_MPm, data, IN_LIF_VALIDf, tbl_data->in_lif_valid);
      soc_mem_field32_set(unit, IHP_LIF_TABLE_LABEL_PWE_MPm, data, TERMINATION_PROFILE_MSBf, tbl_data->termination_profile_msb);
  } 

  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PWE_MPm, data, TERMINATION_TYPEf, tbl_data->termination_profile);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PWE_MPm, data, ORIENTATION_IS_HUBf, tbl_data->orientation_is_hub);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PWE_MPm, data, ACTION_PROFILE_INDEXf, tbl_data->action_profile_index);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PWE_MPm, data, IN_LIF_PROFILEf, tbl_data->in_lif_profile);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PWE_MPm, data, TPID_PROFILEf, tbl_data->tpid_profile);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PWE_MPm, data, COS_PROFILEf, tbl_data->cos_profile);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PWE_MPm, data, DA_NOT_FOUND_PROFILEf, tbl_data->da_not_found_profile);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PWE_MPm, data, VSIf, tbl_data->vsi);
  soc_mem_field64_set(unit, PPDB_B_LIF_TABLE_LABEL_PWE_MPm, data, LEARN_DATAf, tbl_data->learn_data);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PWE_MPm, data, TT_LEARN_ENABLEf, tbl_data->tt_learn_enable);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PWE_MPm, data, VSI_ASSIGNMENT_MODEf, tbl_data->vsi_assignment_mode);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PWE_MPm, data, OAM_LIF_SETf, tbl_data->oam_lif_set);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PWE_MPm, data, PROTECTION_PATHf, tbl_data->protection_path);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PWE_MPm, data, PROTECTION_POINTERf, tbl_data->protection_pointer);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PWE_MPm, data, TYPEf, tbl_data->type);

  bank_id = LIF_TABLE_ENTRY_OFFSET_TO_BANK_ID(unit, entry_offset);
  entry_offset_in_bank = LIF_TABLE_ENTRY_OFFSET_TO_OFFSET_IN_BANK(unit, entry_offset);

  res = soc_mem_array_write(unit, PPDB_B_LIF_TABLE_LABEL_PWE_MPm, bank_id, MEM_BLOCK_ANY, entry_offset_in_bank, data);  
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_lif_table_label_pwe_mp_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IHP_LIF_TABLE_LABEL_PROTOCOL_OR_LSP_TBL_ENTRY_SIZE ARAD_PP_IHP_LIF_TABLE_ENTRY_SIZE

/* In Jericho, the VSI field is depcrated. Instead, the VSI MSBs are used as the inrif's LSBs. Therefore,
   in the set function, the field needs to be split in two, and in the get function, the LSBs and MSBs
   need to be read separatly and merged. */
#define ARAD_PP_PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSP_IN_RIF_MSB_MASK    0x1fff
#define ARAD_PP_PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSP_IN_RIF_MSB_SHIFT   2

#define ARAD_PP_PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSP_IN_RIF_LSB_MASK    3
#define ARAD_PP_PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSP_IN_RIF_LSB_SHIFT   0

#define ARAD_PP_PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSP_IN_RIF_MSB_GET(_in_rif)  \
    ((_in_rif >> ARAD_PP_PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSP_IN_RIF_MSB_SHIFT) & ARAD_PP_PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSP_IN_RIF_MSB_MASK)

#define ARAD_PP_PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSP_IN_RIF_LSB_GET(_in_rif)  \
    ((_in_rif >> ARAD_PP_PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSP_IN_RIF_LSB_SHIFT) & ARAD_PP_PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSP_IN_RIF_LSB_MASK)

#define ARAD_PP_PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSP_IN_RIF_SET(_in_rif, _in_rif_msb, _in_rif_lsb)  \
    (_in_rif) = (((_in_rif_msb & ARAD_PP_PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSP_IN_RIF_MSB_MASK) << ARAD_PP_PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSP_IN_RIF_MSB_SHIFT) \
                | ((_in_rif_lsb & ARAD_PP_PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSP_IN_RIF_LSB_MASK) << ARAD_PP_PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSP_IN_RIF_LSB_SHIFT))

uint32
  arad_pp_ihp_lif_table_label_protocol_or_lsp_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_LIF_TABLE_LABEL_PROTOCOL_OR_LSP_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK,
    entry_offset_in_bank,
    bank_id;
  uint32
    data[ARAD_PP_IHP_LIF_TABLE_LABEL_PROTOCOL_OR_LSP_TBL_ENTRY_SIZE];
  uint32 in_rif_lsb, in_rif_msb;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_LIF_TABLE_LABEL_PROTOCOL_OR_LSP_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_LIF_TABLE_LABEL_PROTOCOL_OR_LSP_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_LIF_TABLE_LABEL_PROTOCOL_OR_LSP_TBL_DATA, 1);

  bank_id = LIF_TABLE_ENTRY_OFFSET_TO_BANK_ID(unit, entry_offset);
  entry_offset_in_bank = LIF_TABLE_ENTRY_OFFSET_TO_OFFSET_IN_BANK(unit, entry_offset);

  res = soc_mem_array_read(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, bank_id, MEM_BLOCK_ANY, entry_offset_in_bank, data);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->orientation_is_hub =    soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, ORIENTATION_IS_HUBf);
  tbl_data->action_profile_index =  soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, ACTION_PROFILE_INDEXf);
  tbl_data->model_is_pipe =         soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, MODEL_IS_PIPEf);
  tbl_data->in_lif_profile =        soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, IN_LIF_PROFILEf);
  tbl_data->tpid_profile =          soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, TPID_PROFILEf);
  tbl_data->cos_profile =           soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, COS_PROFILEf);
  tbl_data->da_not_found_profile =  soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, DA_NOT_FOUND_PROFILEf);
  tbl_data->service_type =          soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, SERVICE_TYPEf);
  tbl_data->in_rif_valid =          soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, IN_RIF_VALIDf);
  tbl_data->forwarding_code =       soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, FORWARDING_CODEf);
  tbl_data->expect_bos =            soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, EXPECT_BOSf);
  tbl_data->destination =           soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, DESTINATIONf);
  tbl_data->tt_learn_enable =       soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, TT_LEARN_ENABLEf);
  tbl_data->vsi_assignment_mode =   soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, VSI_ASSIGNMENT_MODEf);
  tbl_data->oam_lif_set =           soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, OAM_LIF_SETf);
  tbl_data->protection_path =       soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, PROTECTION_PATHf);
  tbl_data->protection_pointer =    soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, PROTECTION_POINTERf);
  tbl_data->type =                  soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, TYPEf);

  /* Some fields have different names in Jericho, or don't exist in Jericho, or exist only in Jericho. */
  if (SOC_IS_JERICHO(unit)) {
      tbl_data->termination_profile =   soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, TERMINATION_TYPE_LSBf);
      tbl_data->destination_valid =     soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, DESTINATION_VALID_OR_TERMINATION_TYPE_MSBf);
      tbl_data->sys_in_lif =            soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, SYS_IN_LIFf);

      /* Concerning the lines below: In Jericho, the VSI field is depcrated. Instead, the VSI MSBs are used as the inrif's LSBs. Therefore,
         the LSBs and MSBs need to be read separatly and merged. */
      in_rif_msb =                      soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, IN_RIF_MSBf);
      in_rif_lsb =                      soc_mem_field32_get(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, VSI_MSBf);
      ARAD_PP_PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSP_IN_RIF_SET(tbl_data->in_rif, in_rif_msb, in_rif_lsb);
  } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      tbl_data->in_lif_valid =          soc_mem_field32_get(unit, IHP_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, IN_LIF_VALIDf);
      tbl_data->termination_profile =   soc_mem_field32_get(unit, IHP_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, TERMINATION_PROFILEf);
      tbl_data->vsi =                   soc_mem_field32_get(unit, IHP_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, VSIf);
      tbl_data->in_rif =                soc_mem_field32_get(unit, IHP_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, IN_RIFf);
      tbl_data->destination_valid =     soc_mem_field32_get(unit, IHP_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, DESTINATION_VALIDf);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_lif_table_label_protocol_or_lsp_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_lif_table_label_protocol_or_lsp_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_LIF_TABLE_LABEL_PROTOCOL_OR_LSP_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK,
    entry_offset_in_bank,
    bank_id;
  uint32
    data[ARAD_PP_IHP_LIF_TABLE_LABEL_PROTOCOL_OR_LSP_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_LIF_TABLE_LABEL_PROTOCOL_OR_LSP_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_LIF_TABLE_LABEL_PROTOCOL_OR_LSP_TBL_ENTRY_SIZE);


  /* The name of the table, and some fields are different in Jericho. Handle them here. */
  if (SOC_IS_JERICHO(unit)) {
      soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, SYS_IN_LIFf, tbl_data->sys_in_lif);
      soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, DOUBLE_DATA_ENTRYf, 0 /* Not configurable */);
      soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, DESTINATION_VALID_OR_TERMINATION_TYPE_MSBf, tbl_data->destination_valid);
      soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, TERMINATION_TYPE_LSBf, tbl_data->termination_profile);
      soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, IN_RIF_MSBf, ARAD_PP_PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSP_IN_RIF_MSB_GET(tbl_data->in_rif));
      /* Concerning the line below: In Jericho, the VSI field is depcrated. Instead, the VSI MSBs are used as the inrif's LSBs. */
      soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, VSI_MSBf, ARAD_PP_PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSP_IN_RIF_LSB_GET(tbl_data->in_rif)); 
  } else if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
      soc_mem_field32_set(unit, IHP_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, IN_LIF_VALIDf, tbl_data->in_lif_valid);
      soc_mem_field32_set(unit, IHP_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, IN_RIFf, tbl_data->in_rif);
      soc_mem_field32_set(unit, IHP_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, DESTINATION_VALIDf, tbl_data->destination_valid);
      soc_mem_field32_set(unit, IHP_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, TERMINATION_PROFILEf, tbl_data->termination_profile);
      soc_mem_field32_set(unit, IHP_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, VSIf, tbl_data->vsi);
  } 

  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, ORIENTATION_IS_HUBf, tbl_data->orientation_is_hub);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, ACTION_PROFILE_INDEXf, tbl_data->action_profile_index);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, MODEL_IS_PIPEf, tbl_data->model_is_pipe);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, IN_LIF_PROFILEf, tbl_data->in_lif_profile);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, TPID_PROFILEf, tbl_data->tpid_profile);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, COS_PROFILEf, tbl_data->cos_profile);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, DA_NOT_FOUND_PROFILEf, tbl_data->da_not_found_profile);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, SERVICE_TYPEf, tbl_data->service_type);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, IN_RIF_VALIDf, tbl_data->in_rif_valid);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, FORWARDING_CODEf, tbl_data->forwarding_code);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, EXPECT_BOSf, tbl_data->expect_bos);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, DESTINATIONf, tbl_data->destination);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, TT_LEARN_ENABLEf, tbl_data->tt_learn_enable);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, VSI_ASSIGNMENT_MODEf, tbl_data->vsi_assignment_mode);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, OAM_LIF_SETf, tbl_data->oam_lif_set);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, PROTECTION_PATHf, tbl_data->protection_path);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, PROTECTION_POINTERf, tbl_data->protection_pointer);
  soc_mem_field32_set(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, data, TYPEf, tbl_data->type);

  bank_id = LIF_TABLE_ENTRY_OFFSET_TO_BANK_ID(unit, entry_offset);
  entry_offset_in_bank = LIF_TABLE_ENTRY_OFFSET_TO_OFFSET_IN_BANK(unit, entry_offset);

  res = soc_mem_array_write(unit, PPDB_B_LIF_TABLE_LABEL_PROTOCOL_OR_LSPm, bank_id, MEM_BLOCK_ANY, entry_offset_in_bank, data);  
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_lif_table_label_protocol_or_lsp_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_epni_isid_table_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_EPNI_ISID_TABLE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EPNI_ISID_TABLE_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EPNI_ISID_TABLE_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EPNI_ISID_TABLE_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_EPNI_ISID_TABLE_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          EPNI_ISID_TABLEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->isid = soc_mem_field32_get(unit, EPNI_ISID_TABLEm, data, ISIDf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_epni_isid_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_epni_isid_table_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_EPNI_ISID_TABLE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EPNI_ISID_TABLE_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EPNI_ISID_TABLE_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EPNI_ISID_TABLE_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, EPNI_ISID_TABLEm, data, ISIDf, tbl_data->isid);

  res = soc_mem_write(
          unit,
          EPNI_ISID_TABLEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_epni_isid_table_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_termination_profile_table_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_TERMINATION_PROFILE_TABLE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_TERMINATION_PROFILE_TABLE_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_TERMINATION_PROFILE_TABLE_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_TERMINATION_PROFILE_TABLE_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_TERMINATION_PROFILE_TABLE_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHP_TERMINATION_PROFILE_TABLEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->ttl_exp_label_index = soc_mem_field32_get(unit, IHP_TERMINATION_PROFILE_TABLEm, data, TTL_EXP_LABEL_INDEXf);
  tbl_data->reject_ttl_0 = soc_mem_field32_get(unit, IHP_TERMINATION_PROFILE_TABLEm, data, REJECT_TTL_0f);
  tbl_data->reject_ttl_1 = soc_mem_field32_get(unit, IHP_TERMINATION_PROFILE_TABLEm, data, REJECT_TTL_1f);
  tbl_data->has_cw = soc_mem_field32_get(unit, IHP_TERMINATION_PROFILE_TABLEm, data, HAS_CWf);
  tbl_data->labels_to_terminate = soc_mem_field32_get(unit, IHP_TERMINATION_PROFILE_TABLEm, data, LABELS_TO_TERMINATEf);
  tbl_data->skip_ethernet = soc_mem_field32_get(unit, IHP_TERMINATION_PROFILE_TABLEm, data, SKIP_ETHERNETf);

#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit)) {
    tbl_data->check_bos = soc_mem_field32_get(unit, IHP_TERMINATION_PROFILE_TABLEm, data, CHECK_BOSf);
  }
#endif /* BCM_88660_A0 */
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_termination_profile_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_termination_profile_table_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_TERMINATION_PROFILE_TABLE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_TERMINATION_PROFILE_TABLE_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_TERMINATION_PROFILE_TABLE_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_TERMINATION_PROFILE_TABLE_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHP_TERMINATION_PROFILE_TABLEm, data, TTL_EXP_LABEL_INDEXf, tbl_data->ttl_exp_label_index);
  soc_mem_field32_set(unit, IHP_TERMINATION_PROFILE_TABLEm, data, REJECT_TTL_0f, tbl_data->reject_ttl_0);
  soc_mem_field32_set(unit, IHP_TERMINATION_PROFILE_TABLEm, data, REJECT_TTL_1f, tbl_data->reject_ttl_1);
  soc_mem_field32_set(unit, IHP_TERMINATION_PROFILE_TABLEm, data, HAS_CWf, tbl_data->has_cw);
  soc_mem_field32_set(unit, IHP_TERMINATION_PROFILE_TABLEm, data, LABELS_TO_TERMINATEf, tbl_data->labels_to_terminate);
  soc_mem_field32_set(unit, IHP_TERMINATION_PROFILE_TABLEm, data, SKIP_ETHERNETf, tbl_data->skip_ethernet);
        
#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit)) {
    soc_mem_field32_set(unit, IHP_TERMINATION_PROFILE_TABLEm, data, CHECK_BOSf, tbl_data->check_bos);
  }
#endif /* BCM_88660_A0 */
  
  res = soc_mem_write(
          unit,
          IHP_TERMINATION_PROFILE_TABLEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_termination_profile_table_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_action_profile_mpls_value_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IHP_ACTION_PROFILE_MPLS_VALUE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_ACTION_PROFILE_MPLS_VALUE_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_ACTION_PROFILE_MPLS_VALUE_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_ACTION_PROFILE_MPLS_VALUE_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IHP_ACTION_PROFILE_MPLS_VALUE_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IHP_ACTION_PROFILE_MPLS_VALUEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->cpu_trap_code = soc_mem_field32_get(unit, IHP_ACTION_PROFILE_MPLS_VALUEm, data, CPU_TRAP_CODEf);
  tbl_data->cpu_trap_strength = soc_mem_field32_get(unit, IHP_ACTION_PROFILE_MPLS_VALUEm, data, CPU_TRAP_STRENGTHf);
  tbl_data->snoop_strength = soc_mem_field32_get(unit, IHP_ACTION_PROFILE_MPLS_VALUEm, data, SNOOP_STRENGTHf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_action_profile_mpls_value_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_ihp_action_profile_mpls_value_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IHP_ACTION_PROFILE_MPLS_VALUE_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IHP_ACTION_PROFILE_MPLS_VALUE_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IHP_ACTION_PROFILE_MPLS_VALUE_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IHP_ACTION_PROFILE_MPLS_VALUE_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IHP_ACTION_PROFILE_MPLS_VALUEm, data, CPU_TRAP_CODEf, tbl_data->cpu_trap_code);
  soc_mem_field32_set(unit, IHP_ACTION_PROFILE_MPLS_VALUEm, data, CPU_TRAP_STRENGTHf, tbl_data->cpu_trap_strength);
  soc_mem_field32_set(unit, IHP_ACTION_PROFILE_MPLS_VALUEm, data, SNOOP_STRENGTHf, tbl_data->snoop_strength);

  res = soc_mem_write(
          unit,
          IHP_ACTION_PROFILE_MPLS_VALUEm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_ihp_action_profile_mpls_value_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IRR_MCDB_EGRESS_FORMAT_0_TBL_ENTRY_SIZE 2

uint32
  arad_pp_irr_mcdb_egress_format_0_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IRR_MCDB_EGRESS_FORMAT_0_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IRR_MCDB_EGRESS_FORMAT_0_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IRR_MCDB_EGRESS_FORMAT_0_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IRR_MCDB_EGRESS_FORMAT_0_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IRR_MCDB_EGRESS_FORMAT_0_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IRR_MCDB_EGRESS_FORMAT_0m,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->outlif_1 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_0m, data, OUTLIF_1f);
  tbl_data->pp_dsp_1a = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_0m, data, PP_DSP_1Af);
  tbl_data->pp_dsp_1b = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_0m, data, PP_DSP_1Bf);
  tbl_data->link_ptr = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_0m, data, LINK_PTRf);
  tbl_data->entry_format = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_0m, data, ENTRY_FORMATf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_irr_mcdb_egress_format_0_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_irr_mcdb_egress_format_0_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IRR_MCDB_EGRESS_FORMAT_0_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IRR_MCDB_EGRESS_FORMAT_0_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IRR_MCDB_EGRESS_FORMAT_0_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IRR_MCDB_EGRESS_FORMAT_0_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_0m, data, OUTLIF_1f, tbl_data->outlif_1);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_0m, data, PP_DSP_1Af, tbl_data->pp_dsp_1a);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_0m, data, PP_DSP_1Bf, tbl_data->pp_dsp_1b);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_0m, data, LINK_PTRf, tbl_data->link_ptr);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_0m, data, ENTRY_FORMATf, tbl_data->entry_format);

  res = soc_mem_write(
          unit,
          IRR_MCDB_EGRESS_FORMAT_0m,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_irr_mcdb_egress_format_0_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IRR_MCDB_EGRESS_FORMAT_1_TBL_ENTRY_SIZE 2

uint32
  arad_pp_irr_mcdb_egress_format_1_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IRR_MCDB_EGRESS_FORMAT_1_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IRR_MCDB_EGRESS_FORMAT_1_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IRR_MCDB_EGRESS_FORMAT_1_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IRR_MCDB_EGRESS_FORMAT_1_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IRR_MCDB_EGRESS_FORMAT_1_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IRR_MCDB_EGRESS_FORMAT_1m,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->outlif_1 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_1m, data, OUTLIF_1f);
  tbl_data->bmp_ptr = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_1m, data, BMP_PTRf);
  tbl_data->link_ptr = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_1m, data, LINK_PTRf);
  tbl_data->entry_format = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_1m, data, ENTRY_FORMATf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_irr_mcdb_egress_format_1_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_irr_mcdb_egress_format_1_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IRR_MCDB_EGRESS_FORMAT_1_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IRR_MCDB_EGRESS_FORMAT_1_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IRR_MCDB_EGRESS_FORMAT_1_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IRR_MCDB_EGRESS_FORMAT_1_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_1m, data, OUTLIF_1f, tbl_data->outlif_1);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_1m, data, BMP_PTRf, tbl_data->bmp_ptr);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_1m, data, LINK_PTRf, tbl_data->link_ptr);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_1m, data, ENTRY_FORMATf, tbl_data->entry_format);

  res = soc_mem_write(
          unit,
          IRR_MCDB_EGRESS_FORMAT_1m,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_irr_mcdb_egress_format_1_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IRR_MCDB_EGRESS_FORMAT_2_TBL_ENTRY_SIZE 2

uint32
  arad_pp_irr_mcdb_egress_format_2_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IRR_MCDB_EGRESS_FORMAT_2_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IRR_MCDB_EGRESS_FORMAT_2_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IRR_MCDB_EGRESS_FORMAT_2_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IRR_MCDB_EGRESS_FORMAT_2_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IRR_MCDB_EGRESS_FORMAT_2_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IRR_MCDB_EGRESS_FORMAT_2m,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->outlif_1 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_2m, data, OUTLIF_1f);
  tbl_data->outlif_2 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_2m, data, OUTLIF_2f);
  tbl_data->link_ptr = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_2m, data, LINK_PTRf);
  tbl_data->entry_format = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_2m, data, ENTRY_FORMATf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_irr_mcdb_egress_format_2_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_irr_mcdb_egress_format_2_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IRR_MCDB_EGRESS_FORMAT_2_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IRR_MCDB_EGRESS_FORMAT_2_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IRR_MCDB_EGRESS_FORMAT_2_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IRR_MCDB_EGRESS_FORMAT_2_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_2m, data, OUTLIF_1f, tbl_data->outlif_1);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_2m, data, OUTLIF_2f, tbl_data->outlif_2);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_2m, data, LINK_PTRf, tbl_data->link_ptr);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_2m, data, ENTRY_FORMATf, tbl_data->entry_format);

  res = soc_mem_write(
          unit,
          IRR_MCDB_EGRESS_FORMAT_2m,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_irr_mcdb_egress_format_2_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IRR_MCDB_EGRESS_FORMAT_4_TBL_ENTRY_SIZE 2

uint32
  arad_pp_irr_mcdb_egress_format_4_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IRR_MCDB_EGRESS_FORMAT_4_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IRR_MCDB_EGRESS_FORMAT_4_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IRR_MCDB_EGRESS_FORMAT_4_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IRR_MCDB_EGRESS_FORMAT_4_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IRR_MCDB_EGRESS_FORMAT_4_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IRR_MCDB_EGRESS_FORMAT_4m,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->outlif_1 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_4m, data, OUTLIF_1f);
  tbl_data->pp_dsp_1 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_4m, data, PP_DSP_1f);
  tbl_data->outlif_2 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_4m, data, OUTLIF_2f);
  tbl_data->pp_dsp_2 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_4m, data, PP_DSP_2f);
  tbl_data->entry_format = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_4m, data, ENTRY_FORMATf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_irr_mcdb_egress_format_4_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_irr_mcdb_egress_format_4_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IRR_MCDB_EGRESS_FORMAT_4_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IRR_MCDB_EGRESS_FORMAT_4_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IRR_MCDB_EGRESS_FORMAT_4_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IRR_MCDB_EGRESS_FORMAT_4_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_4m, data, OUTLIF_1f, tbl_data->outlif_1);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_4m, data, PP_DSP_1f, tbl_data->pp_dsp_1);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_4m, data, OUTLIF_2f, tbl_data->outlif_2);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_4m, data, PP_DSP_2f, tbl_data->pp_dsp_2);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_4m, data, ENTRY_FORMATf, tbl_data->entry_format);

  res = soc_mem_write(
          unit,
          IRR_MCDB_EGRESS_FORMAT_4m,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_irr_mcdb_egress_format_4_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IRR_MCDB_EGRESS_FORMAT_5_TBL_ENTRY_SIZE 2

uint32
  arad_pp_irr_mcdb_egress_format_5_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IRR_MCDB_EGRESS_FORMAT_5_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IRR_MCDB_EGRESS_FORMAT_5_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IRR_MCDB_EGRESS_FORMAT_5_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IRR_MCDB_EGRESS_FORMAT_5_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IRR_MCDB_EGRESS_FORMAT_5_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IRR_MCDB_EGRESS_FORMAT_5m,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->outlif_1 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_5m, data, OUTLIF_1f);
  tbl_data->pp_dsp_1 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_5m, data, PP_DSP_1f);
  tbl_data->outlif_2 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_5m, data, OUTLIF_2f);
  tbl_data->pp_dsp_2 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_5m, data, PP_DSP_2f);
  tbl_data->entry_format = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_5m, data, ENTRY_FORMATf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_irr_mcdb_egress_format_5_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_irr_mcdb_egress_format_5_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IRR_MCDB_EGRESS_FORMAT_5_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IRR_MCDB_EGRESS_FORMAT_5_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IRR_MCDB_EGRESS_FORMAT_5_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IRR_MCDB_EGRESS_FORMAT_5_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_5m, data, OUTLIF_1f, tbl_data->outlif_1);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_5m, data, PP_DSP_1f, tbl_data->pp_dsp_1);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_5m, data, OUTLIF_2f, tbl_data->outlif_2);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_5m, data, PP_DSP_2f, tbl_data->pp_dsp_2);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_5m, data, ENTRY_FORMATf, tbl_data->entry_format);

  res = soc_mem_write(
          unit,
          IRR_MCDB_EGRESS_FORMAT_5m,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_irr_mcdb_egress_format_5_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IRR_MCDB_EGRESS_FORMAT_6_TBL_ENTRY_SIZE 2

uint32
  arad_pp_irr_mcdb_egress_format_6_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IRR_MCDB_EGRESS_FORMAT_6_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IRR_MCDB_EGRESS_FORMAT_6_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IRR_MCDB_EGRESS_FORMAT_6_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IRR_MCDB_EGRESS_FORMAT_6_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IRR_MCDB_EGRESS_FORMAT_6_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IRR_MCDB_EGRESS_FORMAT_6m,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->outlif_1 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_6m, data, OUTLIF_1f);
  tbl_data->outlif_2 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_6m, data, OUTLIF_2f);
  tbl_data->outlif_3 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_6m, data, OUTLIF_3f);
  tbl_data->entry_format = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_6m, data, ENTRY_FORMATf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_irr_mcdb_egress_format_6_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_irr_mcdb_egress_format_6_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IRR_MCDB_EGRESS_FORMAT_6_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IRR_MCDB_EGRESS_FORMAT_6_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IRR_MCDB_EGRESS_FORMAT_6_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IRR_MCDB_EGRESS_FORMAT_6_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_6m, data, OUTLIF_1f, tbl_data->outlif_1);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_6m, data, OUTLIF_2f, tbl_data->outlif_2);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_6m, data, OUTLIF_3f, tbl_data->outlif_3);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_6m, data, ENTRY_FORMATf, tbl_data->entry_format);

  res = soc_mem_write(
          unit,
          IRR_MCDB_EGRESS_FORMAT_6m,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_irr_mcdb_egress_format_6_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IRR_MCDB_EGRESS_FORMAT_7_TBL_ENTRY_SIZE 2

uint32
  arad_pp_irr_mcdb_egress_format_7_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IRR_MCDB_EGRESS_FORMAT_7_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IRR_MCDB_EGRESS_FORMAT_7_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IRR_MCDB_EGRESS_FORMAT_7_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IRR_MCDB_EGRESS_FORMAT_7_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IRR_MCDB_EGRESS_FORMAT_7_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IRR_MCDB_EGRESS_FORMAT_7m,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->outlif_1 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_7m, data, OUTLIF_1f);
  tbl_data->outlif_2 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_7m, data, OUTLIF_2f);
  tbl_data->outlif_3 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_7m, data, OUTLIF_3f);
  tbl_data->entry_format = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_FORMAT_7m, data, ENTRY_FORMATf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_irr_mcdb_egress_format_7_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_irr_mcdb_egress_format_7_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IRR_MCDB_EGRESS_FORMAT_7_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IRR_MCDB_EGRESS_FORMAT_7_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IRR_MCDB_EGRESS_FORMAT_7_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IRR_MCDB_EGRESS_FORMAT_7_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_7m, data, OUTLIF_1f, tbl_data->outlif_1);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_7m, data, OUTLIF_2f, tbl_data->outlif_2);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_7m, data, OUTLIF_3f, tbl_data->outlif_3);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_FORMAT_7m, data, ENTRY_FORMATf, tbl_data->entry_format);

  res = soc_mem_write(
          unit,
          IRR_MCDB_EGRESS_FORMAT_7m,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_irr_mcdb_egress_format_7_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IRR_MCDB_EGRESS_SPECIAL_FORMAT_TBL_ENTRY_SIZE 2

uint32
  arad_pp_irr_mcdb_egress_special_format_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IRR_MCDB_EGRESS_SPECIAL_FORMAT_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IRR_MCDB_EGRESS_SPECIAL_FORMAT_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IRR_MCDB_EGRESS_SPECIAL_FORMAT_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IRR_MCDB_EGRESS_SPECIAL_FORMAT_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IRR_MCDB_EGRESS_SPECIAL_FORMAT_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IRR_MCDB_EGRESS_SPECIAL_FORMATm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->mc_bmp_1 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_SPECIAL_FORMATm, data, MC_BMP_1f);
  tbl_data->mc_bmp_2 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_SPECIAL_FORMATm, data, MC_BMP_2f);
  tbl_data->mc_bmp_3 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_SPECIAL_FORMATm, data, MC_BMP_3f);
  tbl_data->mc_bmp_4 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_SPECIAL_FORMATm, data, MC_BMP_4f);
  tbl_data->mc_bmp_5 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_SPECIAL_FORMATm, data, MC_BMP_5f);
  tbl_data->mc_bmp_6 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_SPECIAL_FORMATm, data, MC_BMP_6f);
  tbl_data->mc_bmp_7 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_SPECIAL_FORMATm, data, MC_BMP_7f);
  tbl_data->mc_bmp_8 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_SPECIAL_FORMATm, data, MC_BMP_8f);
  tbl_data->reserved = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_SPECIAL_FORMATm, data, RESERVEDf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_irr_mcdb_egress_special_format_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_irr_mcdb_egress_special_format_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IRR_MCDB_EGRESS_SPECIAL_FORMAT_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IRR_MCDB_EGRESS_SPECIAL_FORMAT_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IRR_MCDB_EGRESS_SPECIAL_FORMAT_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IRR_MCDB_EGRESS_SPECIAL_FORMAT_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_SPECIAL_FORMATm, data, MC_BMP_1f, tbl_data->mc_bmp_1);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_SPECIAL_FORMATm, data, MC_BMP_2f, tbl_data->mc_bmp_2);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_SPECIAL_FORMATm, data, MC_BMP_3f, tbl_data->mc_bmp_3);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_SPECIAL_FORMATm, data, MC_BMP_4f, tbl_data->mc_bmp_4);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_SPECIAL_FORMATm, data, MC_BMP_5f, tbl_data->mc_bmp_5);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_SPECIAL_FORMATm, data, MC_BMP_6f, tbl_data->mc_bmp_6);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_SPECIAL_FORMATm, data, MC_BMP_7f, tbl_data->mc_bmp_7);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_SPECIAL_FORMATm, data, MC_BMP_8f, tbl_data->mc_bmp_8);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_SPECIAL_FORMATm, data, RESERVEDf, tbl_data->reserved);

  res = soc_mem_write(
          unit,
          IRR_MCDB_EGRESS_SPECIAL_FORMATm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_irr_mcdb_egress_special_format_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_IRR_MCDB_EGRESS_TDM_FORMAT_TBL_ENTRY_SIZE 2

uint32
  arad_pp_irr_mcdb_egress_tdm_format_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_IRR_MCDB_EGRESS_TDM_FORMAT_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IRR_MCDB_EGRESS_TDM_FORMAT_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IRR_MCDB_EGRESS_TDM_FORMAT_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IRR_MCDB_EGRESS_TDM_FORMAT_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_IRR_MCDB_EGRESS_TDM_FORMAT_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          IRR_MCDB_EGRESS_TDM_FORMATm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->pp_dsp_1 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_TDM_FORMATm, data, PP_DSP_1f);
  tbl_data->pp_dsp_2 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_TDM_FORMATm, data, PP_DSP_2f);
  tbl_data->pp_dsp_3 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_TDM_FORMATm, data, PP_DSP_3f);
  tbl_data->pp_dsp_4 = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_TDM_FORMATm, data, PP_DSP_4f);
  tbl_data->link_ptr = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_TDM_FORMATm, data, LINK_PTRf);
  tbl_data->reserved = soc_mem_field32_get(unit, IRR_MCDB_EGRESS_TDM_FORMATm, data, RESERVEDf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_irr_mcdb_egress_tdm_format_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_irr_mcdb_egress_tdm_format_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_IRR_MCDB_EGRESS_TDM_FORMAT_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_IRR_MCDB_EGRESS_TDM_FORMAT_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_IRR_MCDB_EGRESS_TDM_FORMAT_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_IRR_MCDB_EGRESS_TDM_FORMAT_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_TDM_FORMATm, data, PP_DSP_1f, tbl_data->pp_dsp_1);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_TDM_FORMATm, data, PP_DSP_2f, tbl_data->pp_dsp_2);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_TDM_FORMATm, data, PP_DSP_3f, tbl_data->pp_dsp_3);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_TDM_FORMATm, data, PP_DSP_4f, tbl_data->pp_dsp_4);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_TDM_FORMATm, data, LINK_PTRf, tbl_data->link_ptr);
  soc_mem_field32_set(unit, IRR_MCDB_EGRESS_TDM_FORMATm, data, RESERVEDf, tbl_data->reserved);

  res = soc_mem_write(
          unit,
          IRR_MCDB_EGRESS_TDM_FORMATm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_irr_mcdb_egress_tdm_format_tbl_set_unsafe()", entry_offset, 0);
}

/* Clear table data and mask all fields */
void
  ARAD_PP_EPNI_PRGE_PROGRAM_SELECTION_CAM_TBL_DATA_init(
    ARAD_PP_EPNI_PRGE_PROGRAM_SELECTION_CAM_TBL_DATA   *tbl_data
  )
{
  sal_memset(tbl_data, 0, sizeof(ARAD_PP_EPNI_PRGE_PROGRAM_SELECTION_CAM_TBL_DATA));
  tbl_data->fwd_code_mask                     = 0xf;
  tbl_data->prge_tm_profile_mask              = 0xf;
  tbl_data->prge_pp_profile_mask              = 0xf;
  tbl_data->system_mc_mask                    = 0x1;
  tbl_data->oam_sub_type_mask                 = 0x7;
  tbl_data->ace_variable_mask                 = 0x3;
  tbl_data->value_1_mask                      = 0xff;
  tbl_data->pph_type_mask                     = 0x3;
  tbl_data->fhei_size_mask                    = 0x3;
  tbl_data->eei_valid_mask                    = 0x1;
  tbl_data->learn_ext_exist_mask              = 0x1;
  tbl_data->data_entry_lsbs_mask              = 0x3;
  tbl_data->first_enc_mask                    = 0x1;
  tbl_data->second_enc_mask                   = 0x3;
  tbl_data->link_editor_entry_is_ac_mask      = 0x1;
  tbl_data->data_entry_cnt_mask               = 0x3;
  tbl_data->routing_over_overlay              = 0x0;
  tbl_data->routing_over_overlay_mask         = 0x0;
  tbl_data->first_out_lif_profile_mask        = 0x3f;
  tbl_data->second_out_lif_profile_mask       = 0x3f;
  tbl_data->third_out_lif_profile_mask        = 0x3f;
  tbl_data->forth_out_lif_profile_mask        = 0x3f;
  tbl_data->first_out_lif_profile_valid_mask  = 0x1;
  tbl_data->second_out_lif_profile_valid_mask = 0x1;
  tbl_data->third_out_lif_profile_valid_mask  = 0x1;
  tbl_data->forth_out_lif_profile_valid_mask  = 0x1; 
}

uint32
  arad_pp_epni_prge_program_selection_cam_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_EPNI_PRGE_PROGRAM_SELECTION_CAM_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[SOC_DPP_DEFS_MAX(EPNI_PRGE_PROGRAM_SELECTION_CAM_NOF_LONGS)];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EPNI_PRGE_PROGRAM_SELECTION_CAM_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, SOC_DPP_DEFS_MAX(EPNI_PRGE_PROGRAM_SELECTION_CAM_NOF_LONGS));
  ARAD_CLEAR(tbl_data, ARAD_PP_EPNI_PRGE_PROGRAM_SELECTION_CAM_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          EPNI_PRGE_PROGRAM_SELECTION_CAMm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->fwd_code = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, FWD_CODEf);
  tbl_data->prge_tm_profile = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, PRGE_TM_PROFILEf);
  tbl_data->prge_pp_profile = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, PRGE_PP_PROFILEf);
  tbl_data->system_mc = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, SYSTEM_MCf);
  tbl_data->pph_type = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, PPH_TYPEf);
  tbl_data->fhei_size = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, FHEI_SIZEf);
  tbl_data->eei_valid = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, EEI_VALIDf);
  tbl_data->learn_ext_exist = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, LEARN_EXT_EXISTf);
  tbl_data->value_1 = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, VALUE_1f);
  tbl_data->ace_variable = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, ACE_VARIABLEf);
  tbl_data->oam_sub_type = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, OAM_SUB_TYPEf);
  tbl_data->data_entry_lsbs = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, DATA_ENTRY_LSBSf);
  tbl_data->first_enc = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, FIRST_ENCf);
  tbl_data->second_enc = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, SECOND_ENCf);
  tbl_data->link_editor_entry_is_ac = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, LINK_EDITOR_ENTRY_IS_ACf);
  tbl_data->data_entry_cnt = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, DATA_ENTRY_CNTf);
  tbl_data->fwd_code_mask = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, FWD_CODE_MASKf);
  tbl_data->prge_tm_profile_mask = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, PRGE_TM_PROFILE_MASKf);
  tbl_data->prge_pp_profile_mask = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, PRGE_PP_PROFILE_MASKf);
  tbl_data->system_mc_mask = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, SYSTEM_MC_MASKf);
  tbl_data->pph_type_mask = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, PPH_TYPE_MASKf);
  tbl_data->fhei_size_mask = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, FHEI_SIZE_MASKf);
  tbl_data->eei_valid_mask = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, EEI_VALID_MASKf);
  tbl_data->learn_ext_exist_mask = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, LEARN_EXT_EXIST_MASKf);
  tbl_data->value_1_mask = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, VALUE_1_MASKf);
  tbl_data->ace_variable_mask = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, ACE_VARIABLE_MASKf);
  tbl_data->oam_sub_type_mask = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, OAM_SUB_TYPE_MASKf);
  tbl_data->data_entry_lsbs_mask = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, DATA_ENTRY_LSBS_MASKf);
  tbl_data->first_enc_mask = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, FIRST_ENC_MASKf);
  tbl_data->second_enc_mask = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, SECOND_ENC_MASKf);
  tbl_data->link_editor_entry_is_ac_mask = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, LINK_EDITOR_ENTRY_IS_AC_MASKf);
  tbl_data->data_entry_cnt_mask = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, DATA_ENTRY_CNT_MASKf);
  if (SOC_IS_JERICHO(unit)) {
      tbl_data->routing_over_overlay = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, ROUTING_OVER_OVERLAYf);
      tbl_data->routing_over_overlay_mask = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, ROUTING_OVER_OVERLAY_MASKf);

      tbl_data->first_out_lif_profile = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, FIRST_OUT_LIF_PROFILEf);
      tbl_data->second_out_lif_profile = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, SECOND_OUT_LIF_PROFILEf);
      tbl_data->third_out_lif_profile = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, THIRD_OUT_LIF_PROFILEf);
      tbl_data->forth_out_lif_profile = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, FORTH_OUT_LIF_PROFILEf);

      tbl_data->first_out_lif_profile_valid = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, FIRST_OUT_LIF_PROFILE_VALIDf);
      tbl_data->second_out_lif_profile_valid = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, SECOND_OUT_LIF_PROFILE_VALIDf);
      tbl_data->third_out_lif_profile_valid = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, THIRD_OUT_LIF_PROFILE_VALIDf);
      tbl_data->forth_out_lif_profile_valid = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, FORTH_OUT_LIF_PROFILE_VALIDf);

      tbl_data->first_out_lif_profile_mask = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, FIRST_OUT_LIF_PROFILE_MASKf);
      tbl_data->second_out_lif_profile_mask = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, SECOND_OUT_LIF_PROFILE_MASKf);
      tbl_data->third_out_lif_profile_mask = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, THIRD_OUT_LIF_PROFILE_MASKf);
      tbl_data->forth_out_lif_profile_mask = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, FORTH_OUT_LIF_PROFILE_MASKf);

      tbl_data->first_out_lif_profile_valid_mask = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, FIRST_OUT_LIF_PROFILE_VALID_MASKf);
      tbl_data->second_out_lif_profile_valid_mask = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, SECOND_OUT_LIF_PROFILE_VALID_MASKf);
      tbl_data->third_out_lif_profile_valid_mask = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, THIRD_OUT_LIF_PROFILE_VALID_MASKf);
      tbl_data->forth_out_lif_profile_valid_mask = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, FORTH_OUT_LIF_PROFILE_VALID_MASKf);
  }
  tbl_data->program = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, PROGRAMf); 
  tbl_data->valid = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, VALIDf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_epni_prge_program_selection_cam_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_epni_prge_program_selection_cam_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_EPNI_PRGE_PROGRAM_SELECTION_CAM_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[SOC_DPP_DEFS_MAX(EPNI_PRGE_PROGRAM_SELECTION_CAM_NOF_LONGS)];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EPNI_PRGE_PROGRAM_SELECTION_CAM_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, SOC_DPP_DEFS_MAX(EPNI_PRGE_PROGRAM_SELECTION_CAM_NOF_LONGS));

  soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, FWD_CODEf, tbl_data->fwd_code);
  soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, PRGE_TM_PROFILEf, tbl_data->prge_tm_profile);
  soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, PRGE_PP_PROFILEf, tbl_data->prge_pp_profile);
  soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, SYSTEM_MCf, tbl_data->system_mc);
  soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, PPH_TYPEf, tbl_data->pph_type);
  soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, FHEI_SIZEf, tbl_data->fhei_size);
  soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, EEI_VALIDf, tbl_data->eei_valid);
  soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, LEARN_EXT_EXISTf, tbl_data->learn_ext_exist);
  soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, VALUE_1f, tbl_data->value_1);
  soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, ACE_VARIABLEf, tbl_data->ace_variable);
  soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, OAM_SUB_TYPEf, tbl_data->oam_sub_type);
  soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, DATA_ENTRY_LSBSf, tbl_data->data_entry_lsbs);
  soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, FIRST_ENCf, tbl_data->first_enc);
  soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, SECOND_ENCf, tbl_data->second_enc);
  soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, LINK_EDITOR_ENTRY_IS_ACf, tbl_data->link_editor_entry_is_ac);
  soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, DATA_ENTRY_CNTf, tbl_data->data_entry_cnt);
  soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, FWD_CODE_MASKf, tbl_data->fwd_code_mask);
  soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, PRGE_TM_PROFILE_MASKf, tbl_data->prge_tm_profile_mask);
  soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, PRGE_PP_PROFILE_MASKf, tbl_data->prge_pp_profile_mask);
  soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, SYSTEM_MC_MASKf, tbl_data->system_mc_mask);
  soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, PPH_TYPE_MASKf, tbl_data->pph_type_mask);
  soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, FHEI_SIZE_MASKf, tbl_data->fhei_size_mask);
  soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, EEI_VALID_MASKf, tbl_data->eei_valid_mask);
  soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, LEARN_EXT_EXIST_MASKf, tbl_data->learn_ext_exist_mask);
  soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, VALUE_1_MASKf, tbl_data->value_1_mask);
  soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, ACE_VARIABLE_MASKf, tbl_data->ace_variable_mask);
  soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, OAM_SUB_TYPE_MASKf, tbl_data->oam_sub_type_mask);
  soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, DATA_ENTRY_LSBS_MASKf, tbl_data->data_entry_lsbs_mask);
  soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, FIRST_ENC_MASKf, tbl_data->first_enc_mask);
  soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, SECOND_ENC_MASKf, tbl_data->second_enc_mask);
  soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, LINK_EDITOR_ENTRY_IS_AC_MASKf, tbl_data->link_editor_entry_is_ac_mask);
  soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, DATA_ENTRY_CNT_MASKf, tbl_data->data_entry_cnt_mask);
  if (SOC_IS_JERICHO(unit)) {
      soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, ROUTING_OVER_OVERLAYf, tbl_data->routing_over_overlay);
      soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, ROUTING_OVER_OVERLAY_MASKf, tbl_data->routing_over_overlay_mask);

      soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, FIRST_OUT_LIF_PROFILEf, tbl_data->first_out_lif_profile);
      soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, SECOND_OUT_LIF_PROFILEf, tbl_data->second_out_lif_profile);
      soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, THIRD_OUT_LIF_PROFILEf, tbl_data->third_out_lif_profile);
      soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, FORTH_OUT_LIF_PROFILEf, tbl_data->forth_out_lif_profile);

      soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, FIRST_OUT_LIF_PROFILE_VALIDf, tbl_data->first_out_lif_profile_valid);
      soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, SECOND_OUT_LIF_PROFILE_VALIDf, tbl_data->second_out_lif_profile_valid);
      soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, THIRD_OUT_LIF_PROFILE_VALIDf, tbl_data->third_out_lif_profile_valid);
      soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, FORTH_OUT_LIF_PROFILE_VALIDf, tbl_data->forth_out_lif_profile_valid);

      soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, FIRST_OUT_LIF_PROFILE_MASKf, tbl_data->first_out_lif_profile_mask);
      soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, SECOND_OUT_LIF_PROFILE_MASKf, tbl_data->second_out_lif_profile_mask);
      soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, THIRD_OUT_LIF_PROFILE_MASKf, tbl_data->third_out_lif_profile_mask);
      soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, FORTH_OUT_LIF_PROFILE_MASKf, tbl_data->forth_out_lif_profile_mask);

      soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, FIRST_OUT_LIF_PROFILE_VALID_MASKf, tbl_data->first_out_lif_profile_valid_mask);
      soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, SECOND_OUT_LIF_PROFILE_VALID_MASKf, tbl_data->second_out_lif_profile_valid_mask);
      soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, THIRD_OUT_LIF_PROFILE_VALID_MASKf, tbl_data->third_out_lif_profile_valid_mask);
      soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, FORTH_OUT_LIF_PROFILE_VALID_MASKf, tbl_data->forth_out_lif_profile_valid_mask);
  }
  soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, PROGRAMf, tbl_data->program);
  soc_mem_field32_set(unit, EPNI_PRGE_PROGRAM_SELECTION_CAMm, data, VALIDf, tbl_data->valid);

  res = soc_mem_write(
          unit,
          EPNI_PRGE_PROGRAM_SELECTION_CAMm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_epni_prge_program_selection_cam_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_EPNI_PRGE_PROGRAM_TBL_ENTRY_SIZE 3

uint32
  arad_pp_epni_prge_program_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_EPNI_PRGE_PROGRAM_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EPNI_PRGE_PROGRAM_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EPNI_PRGE_PROGRAM_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EPNI_PRGE_PROGRAM_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_EPNI_PRGE_PROGRAM_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          EPNI_PRGE_PROGRAMm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->program_pointer = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAMm, data, PROGRAM_POINTERf);
  tbl_data->register_src = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAMm, data, REGISTER_SRCf);
  tbl_data->bytes_to_remove = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAMm, data, BYTES_TO_REMOVEf);
  tbl_data->remove_network_header = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAMm, data, REMOVE_NETWORK_HEADERf);
  tbl_data->remove_system_header = soc_mem_field32_get(unit, EPNI_PRGE_PROGRAMm, data, REMOVE_SYSTEM_HEADERf);
  soc_mem_field64_get(unit, EPNI_PRGE_PROGRAMm, data, PROGRAM_VARf, &tbl_data->program_var);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_epni_prge_program_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_epni_prge_program_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_EPNI_PRGE_PROGRAM_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EPNI_PRGE_PROGRAM_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EPNI_PRGE_PROGRAM_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EPNI_PRGE_PROGRAM_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, EPNI_PRGE_PROGRAMm, data, PROGRAM_POINTERf, tbl_data->program_pointer);
  soc_mem_field32_set(unit, EPNI_PRGE_PROGRAMm, data, REGISTER_SRCf, tbl_data->register_src);
  soc_mem_field32_set(unit, EPNI_PRGE_PROGRAMm, data, BYTES_TO_REMOVEf, tbl_data->bytes_to_remove);
  soc_mem_field32_set(unit, EPNI_PRGE_PROGRAMm, data, REMOVE_NETWORK_HEADERf, tbl_data->remove_network_header);
  soc_mem_field32_set(unit, EPNI_PRGE_PROGRAMm, data, REMOVE_SYSTEM_HEADERf, tbl_data->remove_system_header);
  soc_mem_field64_set(unit, EPNI_PRGE_PROGRAMm, data, PROGRAM_VARf, tbl_data->program_var);

  res = soc_mem_write(
          unit,
          EPNI_PRGE_PROGRAMm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_epni_prge_program_tbl_set_unsafe()", entry_offset, 0);
}

#define ARAD_PP_EPNI_PRGE_INSTRUCTION_TBL_ENTRY_SIZE 2

uint32
  arad_pp_epni_prge_instruction_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  soc_mem_t  memory_offset,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_EPNI_PRGE_INSTRUCTION_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EPNI_PRGE_INSTRUCTION_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EPNI_PRGE_INSTRUCTION_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EPNI_PRGE_INSTRUCTION_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_EPNI_PRGE_INSTRUCTION_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          memory_offset,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  tbl_data->src_select = soc_mem_field32_get(unit, memory_offset, data, SRC_SELECTf);
  tbl_data->size_src = soc_mem_field32_get(unit, memory_offset, data, SIZE_SRCf);
  tbl_data->size_base = soc_mem_field32_get(unit, memory_offset, data, SIZE_BASEf);
  tbl_data->offset_src = soc_mem_field32_get(unit, memory_offset, data, OFFSET_SRCf);
  tbl_data->offset_base = soc_mem_field32_get(unit, memory_offset, data, OFFSET_BASEf);
  tbl_data->fem_select = soc_mem_field32_get(unit, memory_offset, data, FEM_SELECTf);
  tbl_data->op_value = soc_mem_field32_get(unit, memory_offset, data, OP_VALUEf);
  tbl_data->op_1_field_select = soc_mem_field32_get(unit, memory_offset, data, OP_1_FIELD_SELECTf);
  tbl_data->op_2_field_select = soc_mem_field32_get(unit, memory_offset, data, OP_2_FIELD_SELECTf);
  tbl_data->op_3_field_select = soc_mem_field32_get(unit, memory_offset, data, OP_3_FIELD_SELECTf);
  tbl_data->alu_action = soc_mem_field32_get(unit, memory_offset, data, ALU_ACTIONf);
  tbl_data->cmp_action = soc_mem_field32_get(unit, memory_offset, data, CMP_ACTIONf);
  tbl_data->dst_select = soc_mem_field32_get(unit, memory_offset, data, DST_SELECTf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_epni_prge_instruction_tbl_get_unsafe()", memory_offset, entry_offset);
}

uint32
  arad_pp_epni_prge_instruction_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  soc_mem_t  memory_offset,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_EPNI_PRGE_INSTRUCTION_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EPNI_PRGE_INSTRUCTION_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EPNI_PRGE_INSTRUCTION_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EPNI_PRGE_INSTRUCTION_TBL_ENTRY_SIZE);

  soc_mem_field32_set(unit, memory_offset, data, SRC_SELECTf, tbl_data->src_select);
  soc_mem_field32_set(unit, memory_offset, data, SIZE_SRCf, tbl_data->size_src);
  soc_mem_field32_set(unit, memory_offset, data, SIZE_BASEf, tbl_data->size_base);
  soc_mem_field32_set(unit, memory_offset, data, OFFSET_SRCf, tbl_data->offset_src);
  soc_mem_field32_set(unit, memory_offset, data, OFFSET_BASEf, tbl_data->offset_base);
  soc_mem_field32_set(unit, memory_offset, data, FEM_SELECTf, tbl_data->fem_select);
  soc_mem_field32_set(unit, memory_offset, data, OP_VALUEf, tbl_data->op_value);
  soc_mem_field32_set(unit, memory_offset, data, OP_1_FIELD_SELECTf, tbl_data->op_1_field_select);
  soc_mem_field32_set(unit, memory_offset, data, OP_2_FIELD_SELECTf, tbl_data->op_2_field_select);
  soc_mem_field32_set(unit, memory_offset, data, OP_3_FIELD_SELECTf, tbl_data->op_3_field_select);
  soc_mem_field32_set(unit, memory_offset, data, ALU_ACTIONf, tbl_data->alu_action);
  soc_mem_field32_set(unit, memory_offset, data, CMP_ACTIONf, tbl_data->cmp_action);
  soc_mem_field32_set(unit, memory_offset, data, DST_SELECTf, tbl_data->dst_select);

  res = soc_mem_write(
          unit,
          memory_offset,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_epni_prge_instruction_tbl_set_unsafe()", memory_offset, entry_offset);
}


#define ARAD_PP_EPNI_PRGE_DATA_TBL_ENTRY_SIZE 5

uint32
  arad_pp_epni_prge_data_tbl_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_OUT ARAD_PP_EPNI_PRGE_DATA_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EPNI_PRGE_DATA_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EPNI_PRGE_DATA_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EPNI_PRGE_DATA_TBL_ENTRY_SIZE);
  ARAD_CLEAR(tbl_data, ARAD_PP_EPNI_PRGE_DATA_TBL_DATA, 1);

  res = soc_mem_read(
          unit,
          EPNI_PRGE_DATAm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  soc_mem_field_get(
      unit, 
      EPNI_PRGE_DATAm, 
      data, 
      PRGE_DATA_ENTRYf, 
      (uint32*) &(tbl_data->prge_data_entry[0])
      );


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_epni_prge_data_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  arad_pp_epni_prge_data_tbl_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   entry_offset,
    SOC_SAND_IN  ARAD_PP_EPNI_PRGE_DATA_TBL_DATA *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_PP_EPNI_PRGE_DATA_TBL_ENTRY_SIZE];
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_EPNI_PRGE_DATA_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_PP_EPNI_PRGE_DATA_TBL_ENTRY_SIZE);

  soc_mem_field_set(
      unit, 
      EPNI_PRGE_DATAm, 
      data, 
      PRGE_DATA_ENTRYf, 
      (uint32*) &(tbl_data->prge_data_entry[0])
      );


  res = soc_mem_write(
          unit,
          EPNI_PRGE_DATAm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_epni_prge_data_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  arad_iqm_cnm_profile_tbl_set_unsafe (
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  uint32                                   profile_ndx,
    SOC_SAND_IN  ARAD_IQM_CNM_PARAMETERS_TABLE_TBL_DATA   *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_IQM_CNM_PARAMETERS_TABLE_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IQM_CNM_PROFILE_TBL_SET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_IQM_CNM_PARAMETERS_TABLE_TBL_ENTRY_SIZE);
  
  soc_mem_field32_set(unit, IQM_CPPRMm, data, CP_FB_MAX_VALf, tbl_data->cp_fb_max_val);
  soc_mem_field32_set(unit, IQM_CPPRMm, data, CP_FIXED_SAMPLE_BASEf, tbl_data->cp_fixed_sample_base);
  soc_mem_field32_set(unit, IQM_CPPRMm, data, CP_QEQf, tbl_data->cp_qeq);
  soc_mem_field32_set(unit, IQM_CPPRMm, data, CP_QUANT_DIVf, tbl_data->cp_quant_div);
  soc_mem_field32_set(unit, IQM_CPPRMm, data, CP_SAMPLE_BASE_0f, tbl_data->cp_sample_base[0]);
  soc_mem_field32_set(unit, IQM_CPPRMm, data, CP_SAMPLE_BASE_1f, tbl_data->cp_sample_base[1]);
  soc_mem_field32_set(unit, IQM_CPPRMm, data, CP_SAMPLE_BASE_2f, tbl_data->cp_sample_base[2]);
  soc_mem_field32_set(unit, IQM_CPPRMm, data, CP_SAMPLE_BASE_3f, tbl_data->cp_sample_base[3]);
  soc_mem_field32_set(unit, IQM_CPPRMm, data, CP_SAMPLE_BASE_4f, tbl_data->cp_sample_base[4]);
  soc_mem_field32_set(unit, IQM_CPPRMm, data, CP_SAMPLE_BASE_5f, tbl_data->cp_sample_base[5]);
  soc_mem_field32_set(unit, IQM_CPPRMm, data, CP_SAMPLE_BASE_6f, tbl_data->cp_sample_base[6]);
  soc_mem_field32_set(unit, IQM_CPPRMm, data, CP_SAMPLE_BASE_7f, tbl_data->cp_sample_base[7]);
  soc_mem_field32_set(unit, IQM_CPPRMm, data, CP_Wf, tbl_data->cp_w);

  res = soc_mem_write(
          unit,
          IQM_CPPRMm,
          MEM_BLOCK_ANY,
          profile_ndx,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_iqm_cnm_profile_tbl_set_unsafe()", profile_ndx, 0);
}

uint32
  arad_iqm_cnm_profile_tbl_get_unsafe (
    SOC_SAND_IN  int                                   unit,
    SOC_SAND_IN  uint32                                   profile_ndx,
    SOC_SAND_OUT ARAD_IQM_CNM_PARAMETERS_TABLE_TBL_DATA   *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data[ARAD_IQM_CNM_PARAMETERS_TABLE_TBL_ENTRY_SIZE];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IQM_CNM_PROFILE_TBL_GET_UNSAFE);

  ARAD_CLEAR(data, uint32, ARAD_IQM_CNM_PARAMETERS_TABLE_TBL_ENTRY_SIZE);
  
  res = soc_mem_read(
          unit,
          IQM_CPPRMm,
          MEM_BLOCK_ANY,
          profile_ndx,
          data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);

  tbl_data->cp_fb_max_val = soc_mem_field32_get(unit, IQM_CPPRMm, data, CP_FB_MAX_VALf);
  tbl_data->cp_fixed_sample_base = soc_mem_field32_get(unit, IQM_CPPRMm, data, CP_FIXED_SAMPLE_BASEf);
  tbl_data->cp_qeq = soc_mem_field32_get(unit, IQM_CPPRMm, data, CP_QEQf);
  tbl_data->cp_quant_div = soc_mem_field32_get(unit, IQM_CPPRMm, data, CP_QUANT_DIVf);
  tbl_data->cp_sample_base[0] = soc_mem_field32_get(unit, IQM_CPPRMm, data, CP_SAMPLE_BASE_0f);
  tbl_data->cp_sample_base[1] = soc_mem_field32_get(unit, IQM_CPPRMm, data, CP_SAMPLE_BASE_1f);
  tbl_data->cp_sample_base[2] = soc_mem_field32_get(unit, IQM_CPPRMm, data, CP_SAMPLE_BASE_2f);
  tbl_data->cp_sample_base[3] = soc_mem_field32_get(unit, IQM_CPPRMm, data, CP_SAMPLE_BASE_3f);
  tbl_data->cp_sample_base[4] = soc_mem_field32_get(unit, IQM_CPPRMm, data, CP_SAMPLE_BASE_4f);
  tbl_data->cp_sample_base[5] = soc_mem_field32_get(unit, IQM_CPPRMm, data, CP_SAMPLE_BASE_5f);
  tbl_data->cp_sample_base[6] = soc_mem_field32_get(unit, IQM_CPPRMm, data, CP_SAMPLE_BASE_6f);
  tbl_data->cp_sample_base[7] = soc_mem_field32_get(unit, IQM_CPPRMm, data, CP_SAMPLE_BASE_7f);
  tbl_data->cp_w = soc_mem_field32_get(unit, IQM_CPPRMm, data, CP_Wf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_iqm_cnm_profile_tbl_get_unsafe()", profile_ndx, 0);
}

uint32
  arad_iqm_cnm_ds_tbl_set_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     entry_offset,
    SOC_SAND_IN  ARAD_IQM_CNM_DS_TBL_DATA   *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data = 0,
    sampling_mode;
  ARAD_CNM_CP_INFO
    cp_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IQM_CNM_DS_TBL_SET_UNSAFE);
  SOC_TMC_CNM_CP_INFO_clear(&cp_info);

  res = arad_cnm_cp_get_unsafe(unit, &cp_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if(cp_info.pkt_gen_mode == SOC_TMC_CNM_GEN_MODE_SAMPLING)
  {
    sampling_mode = 1;
  }
  else
  {
    sampling_mode = 0;
  }

  if(sampling_mode == 0)
  {
    soc_mem_field32_set(unit, IQM_CPDMSm, &data, CP_ENABLEf, tbl_data->cp_enable_1);
    soc_mem_field32_set(unit, IQM_CPDMSm, &data, CP_CLASSf, tbl_data->cp_profile_1);
    soc_mem_field32_set(unit, IQM_CPDMSm, &data, CP_IDf, tbl_data->cp_id);
  }
  else
  {
    /* In sampling mode each entry in the table contains data for 2 CP queues */
    /* Bit 0:     CP Enable 1   */
    /* Bits 1-3:  CP Profile 1  */
    /* Bit 4:     CP Enable 2   */
    /* Bits 5-7:  CP Profile 2  */
    soc_mem_field32_set(unit, IQM_CPDMSm, &data, CP_ENABLEf, tbl_data->cp_enable_1);
    soc_mem_field32_set(unit, IQM_CPDMSm, &data, CP_CLASSf, tbl_data->cp_profile_1);
    SOC_SAND_SET_BIT(data,tbl_data->cp_enable_2,ARAD_IQM_CNM_DS_TBL_DATA_SAMPLING_EN_2_BIT);
    data |= SOC_SAND_SET_BITS_RANGE(tbl_data->cp_profile_2, ARAD_IQM_CNM_DS_TBL_DATA_SAMPLING_PROFILE_MSB, ARAD_IQM_CNM_DS_TBL_DATA_SAMPLING_PROFILE_LSB);
  }
  
  res = soc_mem_write(
          unit,
          IQM_CPDMSm,
          MEM_BLOCK_ANY,
          entry_offset,
          &data
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_iqm_cnm_ds_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  arad_iqm_cnm_ds_tbl_get_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     entry_offset,
    SOC_SAND_OUT ARAD_IQM_CNM_DS_TBL_DATA   *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    data,
    sampling_mode;
  ARAD_CNM_CP_INFO
    cp_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_IQM_CNM_DS_TBL_GET_UNSAFE);
  SOC_TMC_CNM_CP_INFO_clear(&cp_info);

  res = arad_cnm_cp_get_unsafe(unit, &cp_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_mem_read(
        unit,
        IQM_CPDMSm,
        MEM_BLOCK_ANY,
        entry_offset,
        &data
      );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

  if(cp_info.pkt_gen_mode == SOC_TMC_CNM_GEN_MODE_SAMPLING)
  {
    sampling_mode = 1;
  }
  else
  {
    sampling_mode = 0;
  }

  if(sampling_mode == 0)
  {
    soc_mem_field_get(unit, IQM_CPDMSm, &data, CP_ENABLEf, &tbl_data->cp_enable_1);
    soc_mem_field_get(unit, IQM_CPDMSm, &data, CP_CLASSf, &tbl_data->cp_profile_1);
    soc_mem_field_get(unit, IQM_CPDMSm, &data, CP_IDf, &tbl_data->cp_id);
  }
  else
  {
    /* In sampling mode each entry in the table contains data for 2 CP queues */
    /* Bit 0:     CP Enable 1   */
    /* Bits 1-3:  CP Profile 1  */
    /* Bit 4:     CP Enable 2   */
    /* Bits 5-7:  CP Profile 2  */
    soc_mem_field_get(unit, IQM_CPDMSm, &data, CP_ENABLEf, &tbl_data->cp_enable_1);
    soc_mem_field_get(unit, IQM_CPDMSm, &data, CP_CLASSf, &tbl_data->cp_profile_1);
    tbl_data->cp_enable_2 = SOC_SAND_GET_BIT(data, ARAD_IQM_CNM_DS_TBL_DATA_SAMPLING_EN_2_BIT);
    tbl_data->cp_profile_2 = SOC_SAND_GET_BITS_RANGE(data, ARAD_IQM_CNM_DS_TBL_DATA_SAMPLING_PROFILE_MSB, ARAD_IQM_CNM_DS_TBL_DATA_SAMPLING_PROFILE_LSB);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_iqm_cnm_ds_tbl_get_unsafe()", entry_offset, 0);
}

#define ARAD_PP_EGQ_DSP_PTR_MAP_TBL_ENTRY_SIZE 2

int
  arad_pp_egq_dsp_ptr_map_tbl_get_unsafe(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  uint32     tm_port,
    SOC_SAND_IN  int        core_id,
    SOC_SAND_OUT ARAD_PP_EGQ_DSP_PTR_MAP_TBL_DATA *tbl_data
  )
{
    uint32 data[ARAD_PP_EGQ_DSP_PTR_MAP_TBL_ENTRY_SIZE];
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    sal_memset(data, 0, sizeof(uint32)*ARAD_PP_EGQ_DSP_PTR_MAP_TBL_ENTRY_SIZE);
    sal_memset(tbl_data, 0, sizeof(ARAD_PP_EGQ_DSP_PTR_MAP_TBL_DATA));

    rv = READ_EGQ_DSP_PTR_MAPm(unit, EGQ_BLOCK(unit, core_id), tm_port, data);
    SOCDNX_IF_ERR_EXIT(rv);

    tbl_data->out_tm_port = soc_mem_field32_get(unit, EGQ_DSP_PTR_MAPm, data, OUT_TM_PORTf);
    tbl_data->out_pp_port = soc_mem_field32_get(unit, EGQ_DSP_PTR_MAPm, data, OUT_PP_PORTf);
    tbl_data->dst_system_port = soc_mem_field32_get(unit, EGQ_DSP_PTR_MAPm, data, DST_SYSTEM_PORTf);

exit:
    SOCDNX_FUNC_RETURN;
}

int
  arad_pp_egq_dsp_ptr_map_tbl_set_unsafe(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  uint32     tm_port,
    SOC_SAND_IN  int        core_id,
    SOC_SAND_IN  ARAD_PP_EGQ_DSP_PTR_MAP_TBL_DATA *tbl_data
  )
{
    uint32 data[ARAD_PP_EGQ_DSP_PTR_MAP_TBL_ENTRY_SIZE];
    uint32 tm_port_rep, pp_port_rep;
    int rv;
    SOCDNX_INIT_FUNC_DEFS;

    sal_memset(data, 0, sizeof(uint32)*ARAD_PP_EGQ_DSP_PTR_MAP_TBL_ENTRY_SIZE);

    soc_mem_field32_set(unit, EGQ_DSP_PTR_MAPm, data, OUT_TM_PORTf, tbl_data->out_tm_port);
    soc_mem_field32_set(unit, EGQ_DSP_PTR_MAPm, data, OUT_PP_PORTf, tbl_data->out_pp_port);
    soc_mem_field32_set(unit, EGQ_DSP_PTR_MAPm, data, DST_SYSTEM_PORTf, tbl_data->dst_system_port);

    rv = WRITE_EGQ_DSP_PTR_MAPm(unit, EGQ_BLOCK(unit, core_id), tm_port, data);
    SOCDNX_IF_ERR_EXIT(rv);

    if(SOC_IS_JERICHO(unit)) {
        rv = WRITE_EPNI_DSP_PTR_MAPm(unit, EPNI_BLOCK(unit, core_id), tm_port, data);
        SOCDNX_IF_ERR_EXIT(rv);
    }

    if (SOC_DPP_CONFIG(unit)->tm.mc_mode & DPP_MC_EGR_17B_CUDS_127_PORTS_MODE && 
        tbl_data->out_tm_port != ARAD_MAX_FAP_PORT_ID /*setting out tm port as 255 marks it invalid - no need for replication*/ ) 
    {
        if(tm_port > ARAD_NOF_FAP_PORTS_EGR_MC_17BIT_CUD - 1) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Invalid tm port ID when using MC_EGR_17B_CUDS_127_PORTS_MODE")));
        }

        if(tbl_data->out_pp_port > ARAD_NOF_FAP_PORTS_EGR_MC_17BIT_CUD - 1) {
            SOCDNX_EXIT_WITH_ERR(SOC_E_INTERNAL, (_BSL_SOCDNX_MSG_STR( "Invalid pp port ID when using MC_EGR_17B_CUDS_127_PORTS_MODE")));
        }

        tm_port_rep = tm_port + ARAD_FAP_PORTS_OFFSET_EGR_MC_17BIT_CUD;
        pp_port_rep = tbl_data->out_pp_port + ARAD_FAP_PORTS_OFFSET_EGR_MC_17BIT_CUD;
        soc_mem_field32_set(unit, EGQ_DSP_PTR_MAPm, data, OUT_PP_PORTf, pp_port_rep);
        rv = WRITE_EGQ_DSP_PTR_MAPm(unit, EGQ_BLOCK(unit, core_id), tm_port_rep, data);
        SOCDNX_IF_ERR_EXIT(rv);

        if(SOC_IS_JERICHO(unit)) {
            rv = WRITE_EPNI_DSP_PTR_MAPm(unit, EPNI_BLOCK(unit, core_id), tm_port_rep, data);
            SOCDNX_IF_ERR_EXIT(rv);
        }
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/* } */
#undef _ERR_MSG_MODULE_NAME

#include <soc/dpp/SAND/Utils/sand_footer.h>
#endif /* of #if defined(BCM_88650_A0) */


