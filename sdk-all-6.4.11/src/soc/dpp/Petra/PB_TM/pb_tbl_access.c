/* $Id: pb_tbl_access.c,v 1.9 Broadcom SDK $
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
                                                                    
#include <soc/dpp/Petra/PB_TM/pb_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_chip_tbls.h>
#include <soc/dpp/Petra/PB_TM/pb_chip_regs.h>
#include <soc/dpp/Petra/PB_TM/pb_api_framework.h>
#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_bitstream.h>
#include <soc/dpp/Petra/PB_TM/pb_api_general.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_IRE_NIF_CTXT_MAP_TBL_ENTRY_SIZE                                               1
#define SOC_PB_IRE_NIF_PORT_TO_CTXT_BIT_MAP_TBL_ENTRY_SIZE                                   3
#define SOC_PB_IRE_RCY_CTXT_MAP_TBL_ENTRY_SIZE                                               1
#define SOC_PB_IRE_TDM_CONFIG_TBL_ENTRY_SIZE                                                 3
#define SOC_PB_IDR_CONTEXT_MRU_TBL_ENTRY_SIZE                                                2
#define SOC_PB_IDR_ETHERNET_METER_PROFILES_TBL_ENTRY_SIZE                                    2
#define SOC_PB_IDR_ETHERNET_METER_STATUS_TBL_ENTRY_SIZE                                      3
#define SOC_PB_IDR_ETHERNET_METER_CONFIG_TBL_ENTRY_SIZE                                      1
#define SOC_PB_IDR_GLOBAL_METER_PROFILES_TBL_ENTRY_SIZE                                      2
#define SOC_PB_IDR_GLOBAL_METER_STATUS_TBL_ENTRY_SIZE                                        2
#define SOC_PB_IRR_MCDB_EGRESS_FORMAT_B_TBL_ENTRY_SIZE                                       3
#define SOC_PB_IRR_MCDB_EGRESS_FORMAT_C_TBL_ENTRY_SIZE                                       3
#define SOC_PB_IRR_SNOOP_MIRROR_TABLE0_TBL_ENTRY_SIZE                                        2
#define SOC_PB_IRR_SNOOP_MIRROR_TABLE1_TBL_ENTRY_SIZE                                        3
#define SOC_PB_IRR_FREE_PCB_MEMORY_TBL_ENTRY_SIZE                                            1
#define SOC_PB_IRR_PCB_LINK_TABLE_TBL_ENTRY_SIZE                                             1
#define SOC_PB_IRR_IS_FREE_PCB_MEMORY_TBL_ENTRY_SIZE                                         1
#define SOC_PB_IRR_IS_PCB_LINK_TABLE_TBL_ENTRY_SIZE                                          1
#define SOC_PB_IRR_RPF_MEMORY_TBL_ENTRY_SIZE                                                 1
#define SOC_PB_IRR_MCR_MEMORY_TBL_ENTRY_SIZE                                                 3
#define SOC_PB_IRR_ISF_MEMORY_TBL_ENTRY_SIZE                                                 2
#define SOC_PB_IRR_DESTINATION_TABLE_TBL_ENTRY_SIZE                                          1
#define SOC_PB_IRR_LAG_TO_LAG_RANGE_TBL_ENTRY_SIZE                                           1
#define SOC_PB_IRR_LAG_MAPPING_TBL_ENTRY_SIZE                                                1
#define SOC_PB_IRR_LAG_NEXT_MEMBER_TBL_ENTRY_SIZE                                            1
#define SOC_PB_IRR_SMOOTH_DIVISION_TBL_ENTRY_SIZE                                            1
#define SOC_PB_IRR_TRAFFIC_CLASS_MAPPING_TBL_ENTRY_SIZE                                      1
#define SOC_PB_IHP_PORT_MINE_TABLE_LAG_PORT_TBL_ENTRY_SIZE                                   1
#define SOC_PB_IHP_TM_PORT_PP_PORT_CONFIG_TBL_ENTRY_SIZE                                     1
#define SOC_PB_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_ENTRY_SIZE                                    2
#define SOC_PB_IHP_PP_PORT_VALUES_TBL_ENTRY_SIZE                                             1
#define SOC_PB_IQM_CNM_DESCRIPTOR_STATIC_TBL_ENTRY_SIZE                                      1
#define SOC_PB_IQM_CNM_DESCRIPTOR_DYNAMIC_TBL_ENTRY_SIZE                                     1
#define SOC_PB_IQM_CNM_PARAMETERS_TABLE_TBL_ENTRY_SIZE                                       5
#define SOC_PB_IQM_METER_PROCESSOR_RESULT_RESOLVE_TABLE_STATIC_TBL_ENTRY_SIZE                1
#define SOC_PB_IQM_PRFSEL_TBL_ENTRY_SIZE                                                     1
#define SOC_PB_IQM_PRFCFG_TBL_ENTRY_SIZE                                                     2
#define SOC_PB_IQM_NORMAL_DYNAMICA_TBL_ENTRY_SIZE                                            2
#define SOC_PB_IQM_NORMAL_DYNAMICB_TBL_ENTRY_SIZE                                            2
#define SOC_PB_IQM_HIGH_DYNAMICA_TBL_ENTRY_SIZE                                              2
#define SOC_PB_IQM_HIGH_DYNAMICB_TBL_ENTRY_SIZE                                              2
#define SOC_PB_IQM_CNTS_MEMA_TBL_ENTRY_SIZE                                                  2
#define SOC_PB_IQM_CNTS_MEMB_TBL_ENTRY_SIZE                                                  2
#define SOC_PB_IQM_OVTH_MEMA_TBL_ENTRY_SIZE                                                  2
#define SOC_PB_IQM_OVTH_MEMB_TBL_ENTRY_SIZE                                                  2
#define SOC_PB_IPT_SOP_MMU_TBL_ENTRY_SIZE                                                    3
#define SOC_PB_IPT_EGQ_TXQ_RD_ADDR_TBL_ENTRY_SIZE                                            1
#define SOC_PB_IPT_FDT_TXQ_WR_ADDR_TBL_ENTRY_SIZE                                            2
#define SOC_PB_IPT_FDT_TXQ_RD_ADDR_TBL_ENTRY_SIZE                                            2
#define SOC_PB_IPT_GCI_BACKOFF_MASK_TBL_ENTRY_SIZE                                           2
#define SOC_PB_FDT_IPT_CONTRO_L_FIFO_TBL_ENTRY_SIZE                                          2
#define SOC_PB_EGQ_QM_TBL_ENTRY_SIZE                                                         2
#define SOC_PB_EGQ_QSM_TBL_ENTRY_SIZE                                                        1
#define SOC_PB_EGQ_DCM_TBL_ENTRY_SIZE                                                        1
#define SOC_PB_EGQ_DWM_NEW_TBL_ENTRY_SIZE                                                    1
#define SOC_PB_EGQ_PCT_TBL_ENTRY_SIZE                                                        2
#define SOC_PB_EGQ_TC_DP_MAP_TBL_ENTRY_SIZE                                                  1
#define SOC_PB_EGQ_FQP_NIF_PORT_MUX_TBL_ENTRY_SIZE                                           1
#define SOC_PB_EGQ_KEY_PROFILE_MAP_INDEX_TBL_ENTRY_SIZE                                      1
#define SOC_PB_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_ENTRY_SIZE                                1
#define SOC_PB_EGQ_PARSER_LAST_SYS_RECORD_TBL_ENTRY_SIZE                                     8
#define SOC_PB_EGQ_PARSER_LAST_NWK_RECORD1_TBL_ENTRY_SIZE                                    8
#define SOC_PB_EGQ_PARSER_LAST_NWK_RECORD2_TBL_ENTRY_SIZE                                    2
#define SOC_PB_EGQ_ERPP_DEBUG_TBL_ENTRY_SIZE                                                 8
#define SOC_PB_IHB_PINFO_LBP_TBL_ENTRY_SIZE                                                  1
#define SOC_PB_IHB_HEADER_PROFILE_TBL_ENTRY_SIZE                                             1
#define SOC_PB_IHB_PINFO_PMF_TBL_ENTRY_SIZE                                                  1
#define SOC_PB_IHB_PACKET_FORMAT_CODE_PROFILE_TBL_ENTRY_SIZE                                 1
#define SOC_PB_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_ENTRY_SIZE                               2
#define SOC_PB_IHB_DIRECT_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE                  1
#define SOC_PB_IHB_DIRECT_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE                  1
#define SOC_PB_IHB_FEM0_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE                 1
#define SOC_PB_IHB_FEM0_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE                     1
#define SOC_PB_IHB_FEM0_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE                           1
#define SOC_PB_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE                                 2
#define SOC_PB_IHB_FEM1_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE                 1
#define SOC_PB_IHB_FEM1_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE                     1
#define SOC_PB_IHB_FEM1_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE                           1
#define SOC_PB_IHB_FEM1_4B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE                                 2
#define SOC_PB_IHB_FEM2_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE                1
#define SOC_PB_IHB_FEM2_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE                    1
#define SOC_PB_IHB_FEM2_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE                          1
#define SOC_PB_IHB_FEM2_14B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE                                4
#define SOC_PB_IHB_FEM2_14B_1ST_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE                             1
#define SOC_PB_IHB_FEM3_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE                1
#define SOC_PB_IHB_FEM3_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE                    1
#define SOC_PB_IHB_FEM3_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE                          1
#define SOC_PB_IHB_FEM3_14B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE                                4
#define SOC_PB_IHB_FEM3_14B_1ST_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE                             1
#define SOC_PB_IHB_FEM4_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE                1
#define SOC_PB_IHB_FEM4_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE                    1
#define SOC_PB_IHB_FEM4_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE                          1
#define SOC_PB_IHB_FEM4_14B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE                                4
#define SOC_PB_IHB_FEM4_14B_1ST_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE                             1
#define SOC_PB_IHB_FEM5_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE                1
#define SOC_PB_IHB_FEM5_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE                    1
#define SOC_PB_IHB_FEM5_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE                          1
#define SOC_PB_IHB_FEM5_17B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE                                5
#define SOC_PB_IHB_FEM5_17B_1ST_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE                             1
#define SOC_PB_IHB_FEM6_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE                1
#define SOC_PB_IHB_FEM6_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE                    1
#define SOC_PB_IHB_FEM6_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE                          1
#define SOC_PB_IHB_FEM6_17B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE                                5
#define SOC_PB_IHB_FEM6_17B_1ST_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE                             1
#define SOC_PB_IHB_FEM7_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE                1
#define SOC_PB_IHB_FEM7_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE                    1
#define SOC_PB_IHB_FEM7_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE                          1
#define SOC_PB_IHB_FEM7_17B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE                                5
#define SOC_PB_IHB_FEM7_17B_1ST_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE                             1
#define SOC_PB_IHB_FEM0_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE                 1
#define SOC_PB_IHB_FEM0_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE                     1
#define SOC_PB_IHB_FEM0_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE                           1
#define SOC_PB_IHB_FEM0_4B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE                                 2
#define SOC_PB_IHB_FEM1_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE                 1
#define SOC_PB_IHB_FEM1_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE                     1
#define SOC_PB_IHB_FEM1_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE                           1
#define SOC_PB_IHB_FEM1_4B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE                                 2
#define SOC_PB_IHB_FEM2_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE                1
#define SOC_PB_IHB_FEM2_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE                    1
#define SOC_PB_IHB_FEM2_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE                          1
#define SOC_PB_IHB_FEM2_14B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE                                4
#define SOC_PB_IHB_FEM2_14B_2ND_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE                             1
#define SOC_PB_IHB_FEM3_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE                1
#define SOC_PB_IHB_FEM3_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE                    1
#define SOC_PB_IHB_FEM3_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE                          1
#define SOC_PB_IHB_FEM3_14B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE                                4
#define SOC_PB_IHB_FEM3_14B_2ND_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE                             1
#define SOC_PB_IHB_FEM4_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE                1
#define SOC_PB_IHB_FEM4_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE                    1
#define SOC_PB_IHB_FEM4_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE                          1
#define SOC_PB_IHB_FEM4_14B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE                                4
#define SOC_PB_IHB_FEM4_14B_2ND_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE                             1
#define SOC_PB_IHB_FEM5_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE                1
#define SOC_PB_IHB_FEM5_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE                    1
#define SOC_PB_IHB_FEM5_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE                          1
#define SOC_PB_IHB_FEM5_17B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE                                5
#define SOC_PB_IHB_FEM5_17B_2ND_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE                             1
#define SOC_PB_IHB_FEM6_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE                1
#define SOC_PB_IHB_FEM6_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE                    1
#define SOC_PB_IHB_FEM6_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE                          1
#define SOC_PB_IHB_FEM6_17B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE                                5
#define SOC_PB_IHB_FEM6_17B_2ND_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE                             1
#define SOC_PB_IHB_FEM7_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE                1
#define SOC_PB_IHB_FEM7_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE                    1
#define SOC_PB_IHB_FEM7_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE                          1
#define SOC_PB_IHB_FEM7_17B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE                                5
#define SOC_PB_IHB_FEM7_17B_2ND_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE                             1
#define SOC_PB_IHB_PINFO_LBP_TBL_ENTRY_SIZE                                                  1
#define SOC_PB_IHB_HEADER_PROFILE_TBL_ENTRY_SIZE                                             1
#define SOC_PB_EPNI_COPY_ENGINE0_PROGRAM_TBL_ENTRY_SIZE                                      2
#define SOC_PB_EPNI_LFEM0_FIELD_SELECT_MAP_TBL_ENTRY_SIZE                                    3
#define SOC_PB_CFC_RCL2_OFP_TBL_ENTRY_SIZE                                                   1
#define SOC_PB_CFC_NIFCLSB2_OFP_TBL_ENTRY_SIZE                                               2
#define SOC_PB_CFC_CALRX_TBL_ENTRY_SIZE                                                      1
#define SOC_PB_CFC_OOB_SCH_MAP_TBL_ENTRY_SIZE                                                1
#define SOC_PB_CFC_CALTX_TBL_ENTRY_SIZE                                                      1
#define SOC_PB_CFC_CALRX_TBL_ENTRY_SIZE                                                1
#define SOC_PB_CFC_OOB_SCH_MAP_TBL_ENTRY_SIZE                                              1
#define SOC_PB_CFC_OOB_SCH_MAP_TBL_ENTRY_SIZE                                              1
#define SOC_PB_CFC_CALTX_TBL_ENTRY_SIZE                                                1

/* } */
/*************
 *  MACROS   *
 *************/
/* { */

#define SOC_PB_TBL_SIZE_CHECK                                                            \
  if (offset >= tbl->addr.base + tbl->addr.size)                                     \
  {                                                                                  \
    SOC_SAND_SET_ERROR_CODE(SOC_PB_TBL_RANGE_OUT_OF_LIMIT, 1001, exit);                      \
  }                                                                                  \

#define SOC_PB_CLEAR(var_ptr, type, count)                                    \
  {                                                                       \
    if (var_ptr == NULL)                                                  \
    {                                                                     \
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, SOC_SAND_NULL_POINTER_ERR, exit); \
    }                                                                     \
    res = SOC_SAND_OK; sal_memset(                                                 \
            var_ptr,                                                      \
            0x0,                                                          \
            (count) * sizeof(type)                                        \
          );                                                              \
    SOC_SAND_CHECK_FUNC_RESULT(res, SOC_PB_GEN_NUM_CLEAR_ERR, exit);              \
  }

#define SOC_PB_FEM_TBL_ACCESS(fun_suffix)																									\
  switch(cycle_ndx)                                                                   \
  {                                                                                   \
  case 0:                                                                             \
    switch(fem_ndx)                                                                   \
    {                                                                                 \
    case 0:                                                                           \
      res = soc_pb_ihb_fem0_4b_1st_pass_##fun_suffix (unit, entry_offset, tbl_data); \
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);                                          \
      break;                                                                          \
    case 1:                                                                           \
      res = soc_pb_ihb_fem1_4b_1st_pass_##fun_suffix (unit, entry_offset, tbl_data); \
      SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);                                          \
      break;                                                                          \
    case 2:                                                                           \
      res = soc_pb_ihb_fem2_14b_1st_pass_##fun_suffix (unit, entry_offset, tbl_data);\
      SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);                                          \
      break;                                                                          \
    case 3:                                                                           \
      res = soc_pb_ihb_fem3_14b_1st_pass_##fun_suffix (unit, entry_offset, tbl_data);\
      SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);                                          \
      break;                                                                          \
    case 4:                                                                           \
      res = soc_pb_ihb_fem4_14b_1st_pass_##fun_suffix (unit, entry_offset, tbl_data);\
      SOC_SAND_CHECK_FUNC_RESULT(res, 18, exit);                                          \
      break;                                                                          \
    case 5:                                                                           \
      res = soc_pb_ihb_fem5_17b_1st_pass_##fun_suffix (unit, entry_offset, tbl_data);\
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);                                          \
      break;                                                                          \
    case 6:                                                                           \
      res = soc_pb_ihb_fem6_17b_1st_pass_##fun_suffix (unit, entry_offset, tbl_data);\
      SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);                                          \
      break;                                                                          \
    case 7:                                                                           \
      res = soc_pb_ihb_fem7_17b_1st_pass_##fun_suffix (unit, entry_offset, tbl_data);\
      SOC_SAND_CHECK_FUNC_RESULT(res, 24, exit);                                          \
      break;                                                                          \
                                                                                      \
    default:                                                                          \
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_FEM_ID_OUT_OF_RANGE_ERR, 20, exit);        \
    }                                                                                 \
    break;                                                                            \
                                                                                      \
  case 1:                                                                             \
    switch(fem_ndx)                                                                   \
    {                                                                                 \
    case 0:                                                                           \
      res = soc_pb_ihb_fem0_4b_2nd_pass_##fun_suffix (unit, entry_offset, tbl_data); \
      SOC_SAND_CHECK_FUNC_RESULT(res, 26, exit);                                          \
      break;                                                                          \
    case 1:                                                                           \
      res = soc_pb_ihb_fem1_4b_2nd_pass_##fun_suffix (unit, entry_offset, tbl_data); \
      SOC_SAND_CHECK_FUNC_RESULT(res, 28, exit);                                          \
      break;                                                                          \
    case 2:                                                                           \
      res = soc_pb_ihb_fem2_14b_2nd_pass_##fun_suffix (unit, entry_offset, tbl_data);\
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);                                          \
      break;                                                                          \
    case 3:                                                                           \
      res = soc_pb_ihb_fem3_14b_2nd_pass_##fun_suffix (unit, entry_offset, tbl_data);\
      SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);                                          \
      break;                                                                          \
    case 4:                                                                           \
      res = soc_pb_ihb_fem4_14b_2nd_pass_##fun_suffix (unit, entry_offset, tbl_data);\
      SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);                                          \
      break;                                                                          \
    case 5:                                                                           \
      res = soc_pb_ihb_fem5_17b_2nd_pass_##fun_suffix (unit, entry_offset, tbl_data);\
      SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);                                          \
      break;                                                                          \
    case 6:                                                                           \
      res = soc_pb_ihb_fem6_17b_2nd_pass_##fun_suffix (unit, entry_offset, tbl_data);\
      SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);                                          \
      break;                                                                          \
    case 7:                                                                           \
      res = soc_pb_ihb_fem7_17b_2nd_pass_##fun_suffix (unit, entry_offset, tbl_data);\
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);                                          \
      break;                                                                          \
                                                                                      \
    default:                                                                          \
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_FEM_ID_OUT_OF_RANGE_ERR, 42, exit);        \
    }                                                                                 \
    break;                                                                            \
                                                                                      \
  default:                                                                            \
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_LOW_LEVEL_CYCLE_NDX_OUT_OF_RANGE_ERR, 44, exit);       \
  }

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

/* } */
/*************
 * FUNCTIONS *
 *************/

/*
 *  Set number of repetitions for table write for MMU block
 *  Each write command is executed nof_reps times.
 *  The address is advanced by one for each write command.
 *  If set to 0 only one operation is performed.
 */
uint32
  soc_pb_mmu_set_reps_for_tbl_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              nof_reps
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MMU_SET_REPS_FOR_TBL_UNSAFE);

  regs = soc_petra_regs();

  SOC_PB_FLD_SET(regs->mmu.indirect_command_reg.indirect_command_count, nof_reps + 1, 10, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_mmu_set_reps_for_tbl_unsafe()", nof_reps, 0);
}

/*
 *  Set number of repetitions for table write for FDT block
 *  Each write command is executed nof_reps times.
 *  The address is advanced by one for each write command.
 *  If set to 0 only one operation is performed.
 */
uint32
  soc_pb_fdt_set_reps_for_tbl_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              nof_reps
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FDT_SET_REPS_FOR_TBL_UNSAFE);

/*
  SOC_PB_FLD_SET(regs->fdt.indirect_command_reg.indirect_command_count, nof_reps + 1, 10, exit);
*/
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fdt_set_reps_for_tbl_unsafe()", nof_reps, 0);
}

/*
 *  Set number of repetitions for table write for IHB block
 *  Each write command is executed nof_reps times.
 *  The address is advanced by one for each write command.
 *  If set to 0 only one operation is performed.
 */
uint32
  soc_pb_ihb_set_reps_for_tbl_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              nof_reps
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_SET_REPS_FOR_TBL_UNSAFE);

  regs = soc_petra_regs();

  SOC_PB_FLD_SET(regs->ihb.indirect_command_reg.indirect_command_count, nof_reps + 1, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_set_reps_for_tbl_unsafe()", nof_reps, 0);
}


/*
 * Read indirect table nif_ctxt_map_tbl from block IRE,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ire_nif_ctxt_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IRE_NIF_CTXT_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IRE_NIF_CTXT_MAP_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IRE_NIF_CTXT_MAP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IRE_NIF_CTXT_MAP_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IRE_NIF_CTXT_MAP_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IRE_NIF_CTXT_MAP_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ire.nif_ctxt_map_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IRE_ID,
          SOC_PB_IRE_NIF_CTXT_MAP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->fap_port),
          &(tbl_data->fap_port)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->parity),
          &(tbl_data->parity)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ire_nif_ctxt_map_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table nif_ctxt_map_tbl from block IRE,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ire_nif_ctxt_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IRE_NIF_CTXT_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IRE_NIF_CTXT_MAP_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IRE_NIF_CTXT_MAP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IRE_NIF_CTXT_MAP_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IRE_NIF_CTXT_MAP_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ire.nif_ctxt_map_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->fap_port),
          &(tbl->fap_port),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IRE_ID,
          SOC_PB_IRE_NIF_CTXT_MAP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ire_nif_ctxt_map_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table nif_port_to_ctxt_bit_map_tbl from block IRE,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ire_nif_port_to_ctxt_bit_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IRE_NIF_PORT_TO_CTXT_BIT_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IRE_NIF_PORT_TO_CTXT_BIT_MAP_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IRE_NIF_PORT_TO_CTXT_BIT_MAP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IRE_NIF_PORT_TO_CTXT_BIT_MAP_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IRE_NIF_PORT_TO_CTXT_BIT_MAP_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IRE_NIF_PORT_TO_CTXT_BIT_MAP_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ire.nif_port_to_ctxt_bit_map_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IRE_ID,
          SOC_PB_IRE_NIF_PORT_TO_CTXT_BIT_MAP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->contexts_bit_mapping),
          (tbl_data->contexts_bit_mapping)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->parity),
          &(tbl_data->parity)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ire_nif_port_to_ctxt_bit_map_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table nif_port_to_ctxt_bit_map_tbl from block IRE,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ire_nif_port_to_ctxt_bit_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IRE_NIF_PORT_TO_CTXT_BIT_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IRE_NIF_PORT_TO_CTXT_BIT_MAP_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IRE_NIF_PORT_TO_CTXT_BIT_MAP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IRE_NIF_PORT_TO_CTXT_BIT_MAP_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IRE_NIF_PORT_TO_CTXT_BIT_MAP_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ire.nif_port_to_ctxt_bit_map_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          (tbl_data->contexts_bit_mapping),
          &(tbl->contexts_bit_mapping),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IRE_ID,
          SOC_PB_IRE_NIF_PORT_TO_CTXT_BIT_MAP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ire_nif_port_to_ctxt_bit_map_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table rcy_ctxt_map_tbl from block IRE,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ire_rcy_ctxt_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IRE_RCY_CTXT_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IRE_RCY_CTXT_MAP_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IRE_RCY_CTXT_MAP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IRE_RCY_CTXT_MAP_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IRE_RCY_CTXT_MAP_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IRE_RCY_CTXT_MAP_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ire.rcy_ctxt_map_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IRE_ID,
          SOC_PB_IRE_RCY_CTXT_MAP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->fap_port),
          &(tbl_data->fap_port)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->parity),
          &(tbl_data->parity)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ire_rcy_ctxt_map_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table rcy_ctxt_map_tbl from block IRE,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ire_rcy_ctxt_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IRE_RCY_CTXT_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IRE_RCY_CTXT_MAP_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IRE_RCY_CTXT_MAP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IRE_RCY_CTXT_MAP_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IRE_RCY_CTXT_MAP_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ire.rcy_ctxt_map_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->fap_port),
          &(tbl->fap_port),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IRE_ID,
          SOC_PB_IRE_RCY_CTXT_MAP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ire_rcy_ctxt_map_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table tdm_config_tbl from block IRE,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ire_tdm_config_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IRE_TDM_CONFIG_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IRE_TDM_CONFIG_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IRE_TDM_CONFIG_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IRE_TDM_CONFIG_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IRE_TDM_CONFIG_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IRE_TDM_CONFIG_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ire.tdm_config_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IRE_ID,
          SOC_PB_IRE_TDM_CONFIG_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->mode),
          &(tbl_data->mode)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cpu),
          &(tbl_data->cpu)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->header),
          (tbl_data->header)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->parity),
          &(tbl_data->parity)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ire_tdm_config_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table tdm_config_tbl from block IRE,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ire_tdm_config_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IRE_TDM_CONFIG_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IRE_TDM_CONFIG_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IRE_TDM_CONFIG_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IRE_TDM_CONFIG_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IRE_TDM_CONFIG_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ire.tdm_config_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->mode),
          &(tbl->mode),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cpu),
          &(tbl->cpu),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          (tbl_data->header),
          &(tbl->header),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);


  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IRE_ID,
          SOC_PB_IRE_TDM_CONFIG_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ire_tdm_config_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table context_mru_tbl from block IDR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_idr_context_mru_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IDR_CONTEXT_MRU_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IDR_CONTEXT_MRU_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IDR_CONTEXT_MRU_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IDR_CONTEXT_MRU_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IDR_CONTEXT_MRU_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IDR_CONTEXT_MRU_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->idr.context_mru_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IDR_ID,
          SOC_PB_IDR_CONTEXT_MRU_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->org_size),
          &(tbl_data->org_size)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->size),
          &(tbl_data->size)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->parity),
          &(tbl_data->parity)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_idr_context_mru_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table context_mru_tbl from block IDR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_idr_context_mru_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IDR_CONTEXT_MRU_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IDR_CONTEXT_MRU_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IDR_CONTEXT_MRU_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IDR_CONTEXT_MRU_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IDR_CONTEXT_MRU_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->idr.context_mru_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->org_size),
          &(tbl->org_size),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->size),
          &(tbl->size),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);


  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IDR_ID,
          SOC_PB_IDR_CONTEXT_MRU_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_idr_context_mru_tbl_set_unsafe()", entry_offset, 0);
}


/*
 * Read indirect table ethernet_meter_profiles_tbl from block IDR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_idr_ethernet_meter_profiles_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IDR_ETHERNET_METER_PROFILES_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IDR_ETHERNET_METER_PROFILES_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IDR_ETHERNET_METER_PROFILES_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IDR_ETHERNET_METER_PROFILES_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IDR_ETHERNET_METER_PROFILES_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IDR_ETHERNET_METER_PROFILES_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->idr.ethernet_meter_profiles_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IDR_ID,
          SOC_PB_IDR_ETHERNET_METER_PROFILES_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->rate_mantissa),
          &(tbl_data->rate_mantissa)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->rate_exp),
          &(tbl_data->rate_exp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->burst_mantissa),
          &(tbl_data->burst_mantissa)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->burst_exp),
          &(tbl_data->burst_exp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->packet_mode),
          &(tbl_data->packet_mode)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->parity),
          &(tbl_data->parity)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_idr_ethernet_meter_profiles_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table ethernet_meter_profiles_tbl from block IDR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_idr_ethernet_meter_profiles_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IDR_ETHERNET_METER_PROFILES_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IDR_ETHERNET_METER_PROFILES_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IDR_ETHERNET_METER_PROFILES_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IDR_ETHERNET_METER_PROFILES_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IDR_ETHERNET_METER_PROFILES_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->idr.ethernet_meter_profiles_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->rate_mantissa),
          &(tbl->rate_mantissa),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->rate_exp),
          &(tbl->rate_exp),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->burst_mantissa),
          &(tbl->burst_mantissa),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->burst_exp),
          &(tbl->burst_exp),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->packet_mode),
          &(tbl->packet_mode),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IDR_ID,
          SOC_PB_IDR_ETHERNET_METER_PROFILES_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_idr_ethernet_meter_profiles_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_idr_ethernet_meter_status_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IDR_ETHERNET_METER_STATUS_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IDR_ETHERNET_METER_STATUS_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IDR_ETHERNET_METER_STATUS_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IDR_ETHERNET_METER_STATUS_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IDR_ETHERNET_METER_STATUS_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IDR_ETHERNET_METER_STATUS_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->idr.ethernet_meter_status_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IDR_ID,
          SOC_PB_IDR_ETHERNET_METER_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->enable),
          &(tbl_data->enable)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->time_stamp),
          &(tbl_data->time_stamp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->level),
          &(tbl_data->level)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->reserved),
          &(tbl_data->reserved)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);
  
  res = soc_petra_field_in_place_get(
          data,
          &(tbl->parity),
          &(tbl_data->parity)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);



exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_idr_ethernet_meter_status_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_idr_ethernet_meter_status_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IDR_ETHERNET_METER_STATUS_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IDR_ETHERNET_METER_STATUS_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IDR_ETHERNET_METER_STATUS_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IDR_ETHERNET_METER_STATUS_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IDR_ETHERNET_METER_STATUS_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->idr.ethernet_meter_status_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->enable),
          &(tbl->enable),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->time_stamp),
          &(tbl->time_stamp),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->level),
          &(tbl->level),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->reserved),
          &(tbl->reserved),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IDR_ID,
          SOC_PB_IDR_ETHERNET_METER_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_idr_ethernet_meter_status_tbl_set_unsafe()", entry_offset, 0);
}


/*
 * Read indirect table ethernet_meter_config_tbl from block IDR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_idr_ethernet_meter_config_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IDR_ETHERNET_METER_CONFIG_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IDR_ETHERNET_METER_CONFIG_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IDR_ETHERNET_METER_CONFIG_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IDR_ETHERNET_METER_CONFIG_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IDR_ETHERNET_METER_CONFIG_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IDR_ETHERNET_METER_CONFIG_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->idr.ethernet_meter_config_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IDR_ID,
          SOC_PB_IDR_ETHERNET_METER_CONFIG_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->profile),
          &(tbl_data->profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->global_meter_ptr),
          &(tbl_data->global_meter_ptr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->parity),
          &(tbl_data->parity)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_idr_ethernet_meter_config_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table ethernet_meter_config_tbl from block IDR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_idr_ethernet_meter_config_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IDR_ETHERNET_METER_CONFIG_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IDR_ETHERNET_METER_CONFIG_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IDR_ETHERNET_METER_CONFIG_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IDR_ETHERNET_METER_CONFIG_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IDR_ETHERNET_METER_CONFIG_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->idr.ethernet_meter_config_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->profile),
          &(tbl->profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->global_meter_ptr),
          &(tbl->global_meter_ptr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IDR_ID,
          SOC_PB_IDR_ETHERNET_METER_CONFIG_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_idr_ethernet_meter_config_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table global_meter_profiles_tbl from block IDR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_idr_global_meter_profiles_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IDR_GLOBAL_METER_PROFILES_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IDR_GLOBAL_METER_PROFILES_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IDR_GLOBAL_METER_PROFILES_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IDR_GLOBAL_METER_PROFILES_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IDR_GLOBAL_METER_PROFILES_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IDR_GLOBAL_METER_PROFILES_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->idr.global_meter_profiles_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IDR_ID,
          SOC_PB_IDR_GLOBAL_METER_PROFILES_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->rate_mantissa),
          &(tbl_data->rate_mantissa)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->rate_exp),
          &(tbl_data->rate_exp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->burst_mantissa),
          &(tbl_data->burst_mantissa)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->burst_exp),
          &(tbl_data->burst_exp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->packet_mode),
          &(tbl_data->packet_mode)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->enable),
          &(tbl_data->enable)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_idr_global_meter_profiles_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table global_meter_profiles_tbl from block IDR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_idr_global_meter_profiles_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IDR_GLOBAL_METER_PROFILES_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IDR_GLOBAL_METER_PROFILES_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IDR_GLOBAL_METER_PROFILES_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IDR_GLOBAL_METER_PROFILES_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IDR_GLOBAL_METER_PROFILES_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->idr.global_meter_profiles_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->rate_mantissa),
          &(tbl->rate_mantissa),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->rate_exp),
          &(tbl->rate_exp),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->burst_mantissa),
          &(tbl->burst_mantissa),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->burst_exp),
          &(tbl->burst_exp),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->packet_mode),
          &(tbl->packet_mode),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->enable),
          &(tbl->enable),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IDR_ID,
          SOC_PB_IDR_GLOBAL_METER_PROFILES_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_idr_global_meter_profiles_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table global_meter_status_tbl from block IDR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_idr_global_meter_status_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IDR_GLOBAL_METER_STATUS_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IDR_GLOBAL_METER_STATUS_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IDR_GLOBAL_METER_STATUS_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IDR_GLOBAL_METER_STATUS_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IDR_GLOBAL_METER_STATUS_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IDR_GLOBAL_METER_STATUS_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->idr.global_meter_status_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IDR_ID,
          SOC_PB_IDR_GLOBAL_METER_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->time_stamp),
          &(tbl_data->time_stamp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->level),
          &(tbl_data->level)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_idr_global_meter_status_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table global_meter_status_tbl from block IDR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_idr_global_meter_status_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IDR_GLOBAL_METER_STATUS_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IDR_GLOBAL_METER_STATUS_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IDR_GLOBAL_METER_STATUS_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IDR_GLOBAL_METER_STATUS_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IDR_GLOBAL_METER_STATUS_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->idr.global_meter_status_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->time_stamp),
          &(tbl->time_stamp),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->level),
          &(tbl->level),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IDR_ID,
          SOC_PB_IDR_GLOBAL_METER_STATUS_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_idr_global_meter_status_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_irr_mcdb_egress_format_b_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IRR_MCDB_EGRESS_FORMAT_B_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IRR_MCDB_EGRESS_FORMAT_B_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IRR_MCDB_EGRESS_FORMAT_B_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IRR_MCDB_EGRESS_FORMAT_B_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IRR_MCDB_EGRESS_FORMAT_B_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IRR_MCDB_EGRESS_FORMAT_B_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->irr.mcdb_egress_format_b_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IRR_ID,
          SOC_PB_IRR_MCDB_EGRESS_FORMAT_B_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->multicast_bitmap_ptr),
          &(tbl_data->multicast_bitmap_ptr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->out_lif1),
          &(tbl_data->out_lif[0])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->out_port),
          &(tbl_data->out_port)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->out_lif2),
          &(tbl_data->out_lif[1])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->format_select),
          &(tbl_data->format_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->link_ptr),
          &(tbl_data->link_ptr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_irr_mcdb_egress_format_b_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_irr_mcdb_egress_format_b_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IRR_MCDB_EGRESS_FORMAT_B_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IRR_MCDB_EGRESS_FORMAT_B_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IRR_MCDB_EGRESS_FORMAT_B_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IRR_MCDB_EGRESS_FORMAT_B_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IRR_MCDB_EGRESS_FORMAT_B_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->irr.mcdb_egress_format_b_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->multicast_bitmap_ptr),
          &(tbl->multicast_bitmap_ptr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->out_lif[1]),
          &(tbl->out_lif2),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->out_port),
          &(tbl->out_port),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->out_lif[0]),
          &(tbl->out_lif1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->format_select),
          &(tbl->format_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->link_ptr),
          &(tbl->link_ptr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IRR_ID,
          SOC_PB_IRR_MCDB_EGRESS_FORMAT_B_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_irr_mcdb_egress_format_b_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_irr_mcdb_egress_format_c_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IRR_MCDB_EGRESS_FORMAT_C_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IRR_MCDB_EGRESS_FORMAT_C_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IRR_MCDB_EGRESS_FORMAT_C_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IRR_MCDB_EGRESS_FORMAT_C_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IRR_MCDB_EGRESS_FORMAT_C_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IRR_MCDB_EGRESS_FORMAT_C_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->irr.mcdb_egress_format_c_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IRR_ID,
          SOC_PB_IRR_MCDB_EGRESS_FORMAT_C_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->multicast_bitmap_ptr2),
          &(tbl_data->multicast_bitmap_ptr[1])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->out_lif2),
          &(tbl_data->out_lif[1])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->multicast_bitmap_ptr1),
          &(tbl_data->multicast_bitmap_ptr[0])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->out_lif1),
          &(tbl_data->out_lif[0])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->format_select),
          &(tbl_data->format_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->link_ptr),
          &(tbl_data->link_ptr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_irr_mcdb_egress_format_c_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_irr_mcdb_egress_format_c_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IRR_MCDB_EGRESS_FORMAT_C_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IRR_MCDB_EGRESS_FORMAT_C_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IRR_MCDB_EGRESS_FORMAT_C_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IRR_MCDB_EGRESS_FORMAT_C_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IRR_MCDB_EGRESS_FORMAT_C_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->irr.mcdb_egress_format_c_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->multicast_bitmap_ptr[1]),
          &(tbl->multicast_bitmap_ptr2),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->out_lif[1]),
          &(tbl->out_lif2),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->multicast_bitmap_ptr[0]),
          &(tbl->multicast_bitmap_ptr1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->out_lif[0]),
          &(tbl->out_lif1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->format_select),
          &(tbl->format_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->link_ptr),
          &(tbl->link_ptr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IRR_ID,
          SOC_PB_IRR_MCDB_EGRESS_FORMAT_C_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_irr_mcdb_egress_format_c_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table snoop_mirror_table0_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_irr_snoop_mirror_table0_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IRR_SNOOP_MIRROR_TABLE0_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IRR_SNOOP_MIRROR_TABLE0_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IRR_SNOOP_MIRROR_TABLE0_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IRR_SNOOP_MIRROR_TABLE0_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IRR_SNOOP_MIRROR_TABLE0_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IRR_SNOOP_MIRROR_TABLE0_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->irr.snoop_mirror_table0_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IRR_ID,
          SOC_PB_IRR_SNOOP_MIRROR_TABLE0_TBL_ENTRY_SIZE * sizeof(uint32)
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
          &(tbl->tc),
          &(tbl_data->tc)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->tc_ow),
          &(tbl_data->tc_ow)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->dp),
          &(tbl_data->dp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->dp_ow),
          &(tbl_data->dp_ow)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->parity),
          &(tbl_data->parity)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_irr_snoop_mirror_table0_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table snoop_mirror_table0_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_irr_snoop_mirror_table0_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IRR_SNOOP_MIRROR_TABLE0_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IRR_SNOOP_MIRROR_TABLE0_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IRR_SNOOP_MIRROR_TABLE0_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IRR_SNOOP_MIRROR_TABLE0_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IRR_SNOOP_MIRROR_TABLE0_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->irr.snoop_mirror_table0_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->destination),
          &(tbl->destination),
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
          &(tbl_data->tc_ow),
          &(tbl->tc_ow),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->dp),
          &(tbl->dp),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->dp_ow),
          &(tbl->dp_ow),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IRR_ID,
          SOC_PB_IRR_SNOOP_MIRROR_TABLE0_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_irr_snoop_mirror_table0_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table snoop_mirror_table1_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_irr_snoop_mirror_table1_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IRR_SNOOP_MIRROR_TABLE1_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IRR_SNOOP_MIRROR_TABLE1_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IRR_SNOOP_MIRROR_TABLE1_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IRR_SNOOP_MIRROR_TABLE1_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IRR_SNOOP_MIRROR_TABLE1_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IRR_SNOOP_MIRROR_TABLE1_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->irr.snoop_mirror_table1_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IRR_ID,
          SOC_PB_IRR_SNOOP_MIRROR_TABLE1_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->meter_ptr0),
          &(tbl_data->meter_ptr0)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->meter_ptr0_ow),
          &(tbl_data->meter_ptr0_ow)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->meter_ptr1),
          &(tbl_data->meter_ptr1)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->meter_ptr1_ow),
          &(tbl_data->meter_ptr1_ow)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->counter_ptr0),
          &(tbl_data->counter_ptr0)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->counter_ptr0_ow),
          &(tbl_data->counter_ptr0_ow)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->counter_ptr1),
          &(tbl_data->counter_ptr1)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->counter_ptr1_ow),
          &(tbl_data->counter_ptr1_ow)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->dp_cmd),
          &(tbl_data->dp_cmd)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->dp_cmd_ow),
          &(tbl_data->dp_cmd_ow)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->parity),
          &(tbl_data->parity)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_irr_snoop_mirror_table1_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table snoop_mirror_table1_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_irr_snoop_mirror_table1_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IRR_SNOOP_MIRROR_TABLE1_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IRR_SNOOP_MIRROR_TABLE1_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IRR_SNOOP_MIRROR_TABLE1_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IRR_SNOOP_MIRROR_TABLE1_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IRR_SNOOP_MIRROR_TABLE1_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->irr.snoop_mirror_table1_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->meter_ptr0),
          &(tbl->meter_ptr0),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->meter_ptr0_ow),
          &(tbl->meter_ptr0_ow),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->meter_ptr1),
          &(tbl->meter_ptr1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->meter_ptr1_ow),
          &(tbl->meter_ptr1_ow),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->counter_ptr0),
          &(tbl->counter_ptr0),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->counter_ptr0_ow),
          &(tbl->counter_ptr0_ow),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->counter_ptr1),
          &(tbl->counter_ptr1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->counter_ptr1_ow),
          &(tbl->counter_ptr1_ow),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->dp_cmd),
          &(tbl->dp_cmd),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->dp_cmd_ow),
          &(tbl->dp_cmd_ow),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 39, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IRR_ID,
          SOC_PB_IRR_SNOOP_MIRROR_TABLE1_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_irr_snoop_mirror_table1_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_irr_free_pcb_memory_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IRR_FREE_PCB_MEMORY_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IRR_FREE_PCB_MEMORY_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IRR_FREE_PCB_MEMORY_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IRR_FREE_PCB_MEMORY_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IRR_FREE_PCB_MEMORY_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IRR_FREE_PCB_MEMORY_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->irr.free_pcb_memory_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IRR_ID,
          SOC_PB_IRR_FREE_PCB_MEMORY_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ecc),
          &(tbl_data->ecc)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->pcp),
          &(tbl_data->pcp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_irr_free_pcb_memory_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_irr_free_pcb_memory_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IRR_FREE_PCB_MEMORY_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IRR_FREE_PCB_MEMORY_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IRR_FREE_PCB_MEMORY_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IRR_FREE_PCB_MEMORY_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IRR_FREE_PCB_MEMORY_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->irr.free_pcb_memory_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->ecc),
          &(tbl->ecc),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->pcp),
          &(tbl->pcp),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IRR_ID,
          SOC_PB_IRR_FREE_PCB_MEMORY_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_irr_free_pcb_memory_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_irr_pcb_link_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IRR_PCB_LINK_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IRR_PCB_LINK_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IRR_PCB_LINK_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IRR_PCB_LINK_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IRR_PCB_LINK_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IRR_PCB_LINK_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->irr.pcb_link_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IRR_ID,
          SOC_PB_IRR_PCB_LINK_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ecc),
          &(tbl_data->ecc)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->pcp),
          &(tbl_data->pcp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_irr_pcb_link_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_irr_pcb_link_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IRR_PCB_LINK_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IRR_PCB_LINK_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IRR_PCB_LINK_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IRR_PCB_LINK_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IRR_PCB_LINK_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->irr.pcb_link_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->ecc),
          &(tbl->ecc),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->pcp),
          &(tbl->pcp),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IRR_ID,
          SOC_PB_IRR_PCB_LINK_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_irr_pcb_link_table_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_irr_is_free_pcb_memory_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IRR_IS_FREE_PCB_MEMORY_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IRR_IS_FREE_PCB_MEMORY_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IRR_IS_FREE_PCB_MEMORY_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IRR_IS_FREE_PCB_MEMORY_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IRR_IS_FREE_PCB_MEMORY_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IRR_IS_FREE_PCB_MEMORY_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->irr.is_free_pcb_memory_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IRR_ID,
          SOC_PB_IRR_IS_FREE_PCB_MEMORY_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ecc),
          &(tbl_data->ecc)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->is_pcp),
          &(tbl_data->is_pcp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_irr_is_free_pcb_memory_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_irr_is_free_pcb_memory_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IRR_IS_FREE_PCB_MEMORY_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IRR_IS_FREE_PCB_MEMORY_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IRR_IS_FREE_PCB_MEMORY_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IRR_IS_FREE_PCB_MEMORY_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IRR_IS_FREE_PCB_MEMORY_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->irr.is_free_pcb_memory_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->ecc),
          &(tbl->ecc),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->is_pcp),
          &(tbl->is_pcp),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IRR_ID,
          SOC_PB_IRR_IS_FREE_PCB_MEMORY_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_irr_is_free_pcb_memory_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_irr_is_pcb_link_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IRR_IS_PCB_LINK_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IRR_IS_PCB_LINK_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IRR_IS_PCB_LINK_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IRR_IS_PCB_LINK_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IRR_IS_PCB_LINK_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IRR_IS_PCB_LINK_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->irr.is_pcb_link_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IRR_ID,
          SOC_PB_IRR_IS_PCB_LINK_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ecc),
          &(tbl_data->ecc)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->is_pcp),
          &(tbl_data->is_pcp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_irr_is_pcb_link_table_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_irr_is_pcb_link_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IRR_IS_PCB_LINK_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IRR_IS_PCB_LINK_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IRR_IS_PCB_LINK_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IRR_IS_PCB_LINK_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IRR_IS_PCB_LINK_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->irr.is_pcb_link_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->ecc),
          &(tbl->ecc),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->is_pcp),
          &(tbl->is_pcp),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IRR_ID,
          SOC_PB_IRR_IS_PCB_LINK_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_irr_is_pcb_link_table_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_irr_rpf_memory_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IRR_RPF_MEMORY_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IRR_RPF_MEMORY_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IRR_RPF_MEMORY_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IRR_RPF_MEMORY_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IRR_RPF_MEMORY_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IRR_RPF_MEMORY_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->irr.rpf_memory_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IRR_ID,
          SOC_PB_IRR_RPF_MEMORY_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ecc),
          &(tbl_data->ecc)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->pointer),
          &(tbl_data->pointer)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_irr_rpf_memory_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_irr_rpf_memory_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IRR_RPF_MEMORY_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IRR_RPF_MEMORY_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IRR_RPF_MEMORY_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IRR_RPF_MEMORY_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IRR_RPF_MEMORY_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->irr.rpf_memory_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->ecc),
          &(tbl->ecc),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->pointer),
          &(tbl->pointer),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IRR_ID,
          SOC_PB_IRR_RPF_MEMORY_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_irr_rpf_memory_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_irr_mcr_memory_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IRR_MCR_MEMORY_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IRR_MCR_MEMORY_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IRR_MCR_MEMORY_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IRR_MCR_MEMORY_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IRR_MCR_MEMORY_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IRR_MCR_MEMORY_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->irr.mcr_memory_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IRR_ID,
          SOC_PB_IRR_MCR_MEMORY_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->descriptor),
          (tbl_data->descriptor)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ecc),
          &(tbl_data->ecc)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_irr_mcr_memory_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_irr_mcr_memory_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IRR_MCR_MEMORY_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IRR_MCR_MEMORY_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IRR_MCR_MEMORY_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IRR_MCR_MEMORY_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IRR_MCR_MEMORY_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->irr.mcr_memory_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          (tbl_data->descriptor),
          &(tbl->descriptor),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->ecc),
          &(tbl->ecc),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IRR_ID,
          SOC_PB_IRR_MCR_MEMORY_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_irr_mcr_memory_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_irr_isf_memory_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IRR_ISF_MEMORY_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IRR_ISF_MEMORY_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IRR_ISF_MEMORY_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IRR_ISF_MEMORY_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IRR_ISF_MEMORY_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IRR_ISF_MEMORY_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->irr.isf_memory_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IRR_ID,
          SOC_PB_IRR_ISF_MEMORY_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->descriptor),
          (tbl_data->descriptor)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ecc),
          &(tbl_data->ecc)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_irr_isf_memory_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_irr_isf_memory_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IRR_ISF_MEMORY_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IRR_ISF_MEMORY_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IRR_ISF_MEMORY_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IRR_ISF_MEMORY_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IRR_ISF_MEMORY_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->irr.isf_memory_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          (tbl_data->descriptor),
          &(tbl->descriptor),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->ecc),
          &(tbl->ecc),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IRR_ID,
          SOC_PB_IRR_ISF_MEMORY_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_irr_isf_memory_tbl_set_unsafe()", entry_offset, 0);
}
/*
 * Read indirect table destination_table_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_irr_destination_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IRR_DESTINATION_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IRR_DESTINATION_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IRR_DESTINATION_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IRR_DESTINATION_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IRR_DESTINATION_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IRR_DESTINATION_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->irr.destination_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IRR_ID,
          SOC_PB_IRR_DESTINATION_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->queue_number),
          &(tbl_data->queue_number)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->queue_valid),
          &(tbl_data->queue_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->tc_profile),
          &(tbl_data->tc_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->parity),
          &(tbl_data->parity)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_irr_destination_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table destination_table_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_irr_destination_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IRR_DESTINATION_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IRR_DESTINATION_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IRR_DESTINATION_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IRR_DESTINATION_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IRR_DESTINATION_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->irr.destination_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->queue_number),
          &(tbl->queue_number),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->queue_valid),
          &(tbl->queue_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->tc_profile),
          &(tbl->tc_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IRR_ID,
          SOC_PB_IRR_DESTINATION_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_irr_destination_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table lag_to_lag_range_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_irr_lag_to_lag_range_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IRR_LAG_TO_LAG_RANGE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IRR_LAG_TO_LAG_RANGE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IRR_LAG_TO_LAG_RANGE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IRR_LAG_TO_LAG_RANGE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IRR_LAG_TO_LAG_RANGE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IRR_LAG_TO_LAG_RANGE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->irr.lag_to_lag_range_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IRR_ID,
          SOC_PB_IRR_LAG_TO_LAG_RANGE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->range),
          &(tbl_data->range)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->mode),
          &(tbl_data->mode)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->parity),
          &(tbl_data->parity)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_irr_lag_to_lag_range_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table lag_to_lag_range_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_irr_lag_to_lag_range_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IRR_LAG_TO_LAG_RANGE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IRR_LAG_TO_LAG_RANGE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IRR_LAG_TO_LAG_RANGE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IRR_LAG_TO_LAG_RANGE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IRR_LAG_TO_LAG_RANGE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->irr.lag_to_lag_range_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->range),
          &(tbl->range),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->mode),
          &(tbl->mode),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IRR_ID,
          SOC_PB_IRR_LAG_TO_LAG_RANGE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_irr_lag_to_lag_range_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table lag_mapping_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_irr_lag_mapping_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN   uint32              lag_ndx,
    SOC_SAND_IN   uint32              port_ndx,
    SOC_SAND_OUT SOC_PB_IRR_LAG_MAPPING_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    entry_offset=0,
    offset  = 0,
    data[SOC_PB_IRR_LAG_MAPPING_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IRR_LAG_MAPPING_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IRR_LAG_MAPPING_TBL_GET_UNSAFE);

  entry_offset =
    (lag_ndx << SOC_PETRA_IRR_GLAG_MAPPING_LAG_PORT_INDEX_NOF_BITS) | port_ndx;

  SOC_PB_CLEAR(data, uint32, SOC_PB_IRR_LAG_MAPPING_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IRR_LAG_MAPPING_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->irr.lag_mapping_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IRR_ID,
          SOC_PB_IRR_LAG_MAPPING_TBL_ENTRY_SIZE * sizeof(uint32)
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
          &(tbl->parity),
          &(tbl_data->parity)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_irr_lag_mapping_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table lag_mapping_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_irr_lag_mapping_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN   uint32              lag_ndx,
    SOC_SAND_IN   uint32              port_ndx,
    SOC_SAND_IN  SOC_PB_IRR_LAG_MAPPING_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    entry_offset=0,
    offset  = 0,
    data[SOC_PB_IRR_LAG_MAPPING_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IRR_LAG_MAPPING_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IRR_LAG_MAPPING_TBL_SET_UNSAFE);

  entry_offset =
    (lag_ndx << SOC_PETRA_IRR_GLAG_MAPPING_LAG_PORT_INDEX_NOF_BITS) | port_ndx;

  SOC_PB_CLEAR(data, uint32, SOC_PB_IRR_LAG_MAPPING_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->irr.lag_mapping_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->destination),
          &(tbl->destination),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IRR_ID,
          SOC_PB_IRR_LAG_MAPPING_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_irr_lag_mapping_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table lag_next_member_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_irr_lag_next_member_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IRR_LAG_NEXT_MEMBER_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IRR_LAG_NEXT_MEMBER_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IRR_LAG_NEXT_MEMBER_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IRR_LAG_NEXT_MEMBER_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IRR_LAG_NEXT_MEMBER_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IRR_LAG_NEXT_MEMBER_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->irr.lag_next_member_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IRR_ID,
          SOC_PB_IRR_LAG_NEXT_MEMBER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->offset),
          &(tbl_data->offset)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->parity),
          &(tbl_data->parity)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_irr_lag_next_member_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table lag_next_member_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_irr_lag_next_member_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IRR_LAG_NEXT_MEMBER_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IRR_LAG_NEXT_MEMBER_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IRR_LAG_NEXT_MEMBER_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IRR_LAG_NEXT_MEMBER_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IRR_LAG_NEXT_MEMBER_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->irr.lag_next_member_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->offset),
          &(tbl->offset),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IRR_ID,
          SOC_PB_IRR_LAG_NEXT_MEMBER_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_irr_lag_next_member_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table smooth_division_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_irr_smooth_division_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN   uint32              lag_size,
    SOC_SAND_IN   uint32              hash_val,
    SOC_SAND_OUT SOC_PB_IRR_SMOOTH_DIVISION_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    entry_offset,
    data[SOC_PB_IRR_SMOOTH_DIVISION_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IRR_SMOOTH_DIVISION_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IRR_SMOOTH_DIVISION_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IRR_SMOOTH_DIVISION_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IRR_SMOOTH_DIVISION_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->irr.smooth_division_tbl);

  entry_offset = lag_size << SOC_PB_IRR_GLAG_DEVISION_HASH_NOF_BITS;
  entry_offset |= hash_val;

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IRR_ID,
          SOC_PB_IRR_SMOOTH_DIVISION_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->member),
          &(tbl_data->member)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->parity),
          &(tbl_data->parity)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_irr_smooth_division_tbl_get_unsafe()", lag_size, hash_val);
}

/*
 * Write indirect table smooth_division_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_irr_smooth_division_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN   uint32              lag_size,
    SOC_SAND_IN   uint32              hash_val,
    SOC_SAND_IN  SOC_PB_IRR_SMOOTH_DIVISION_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    entry_offset,
    data[SOC_PB_IRR_SMOOTH_DIVISION_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IRR_SMOOTH_DIVISION_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IRR_SMOOTH_DIVISION_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IRR_SMOOTH_DIVISION_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->irr.smooth_division_tbl);

  entry_offset = lag_size << SOC_PB_IRR_GLAG_DEVISION_HASH_NOF_BITS;
  entry_offset |= hash_val;

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->member),
          &(tbl->member),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IRR_ID,
          SOC_PB_IRR_SMOOTH_DIVISION_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_irr_smooth_division_tbl_set_unsafe()", lag_size, hash_val);
}

/*
 * Read indirect table traffic_class_mapping_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_irr_traffic_class_mapping_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IRR_TRAFFIC_CLASS_MAPPING_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IRR_TRAFFIC_CLASS_MAPPING_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IRR_TRAFFIC_CLASS_MAPPING_TBL
    *tbl;
  int32
    tc_ndx = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IRR_TRAFFIC_CLASS_MAPPING_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IRR_TRAFFIC_CLASS_MAPPING_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IRR_TRAFFIC_CLASS_MAPPING_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->irr.traffic_class_mapping_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IRR_ID,
          SOC_PB_IRR_TRAFFIC_CLASS_MAPPING_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  for (tc_ndx = 0; tc_ndx < SOC_PETRA_NOF_TRAFFIC_CLASSES; ++tc_ndx)
  {
    res = soc_petra_field_in_place_get(
            data,
            &(tbl->traffic_class_mapping[tc_ndx]),
            &(tbl_data->traffic_class_mapping[tc_ndx])
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_irr_traffic_class_mapping_tbl_get_unsafe()", entry_offset, tc_ndx);
}

/*
 * Write indirect table traffic_class_mapping_tbl from block IRR,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_irr_traffic_class_mapping_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IRR_TRAFFIC_CLASS_MAPPING_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IRR_TRAFFIC_CLASS_MAPPING_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IRR_TRAFFIC_CLASS_MAPPING_TBL
    *tbl;
  int32
    tc_ndx = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IRR_TRAFFIC_CLASS_MAPPING_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IRR_TRAFFIC_CLASS_MAPPING_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->irr.traffic_class_mapping_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  for (tc_ndx = 0; tc_ndx < SOC_PETRA_NOF_TRAFFIC_CLASSES; ++tc_ndx)
  {
    res = soc_petra_field_in_place_set(
            &(tbl_data->traffic_class_mapping[tc_ndx]),
            &(tbl->traffic_class_mapping[tc_ndx]),
            data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IRR_ID,
          SOC_PB_IRR_TRAFFIC_CLASS_MAPPING_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_irr_traffic_class_mapping_tbl_set_unsafe()", entry_offset, tc_ndx);
}

uint32
  soc_pb_ihp_port_mine_table_lag_port_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHP_PORT_MINE_TABLE_LAG_PORT_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHP_PORT_MINE_TABLE_LAG_PORT_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHP_PORT_MINE_TABLE_LAG_PORT_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHP_PORT_MINE_TABLE_LAG_PORT_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHP_PORT_MINE_TABLE_LAG_PORT_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHP_PORT_MINE_TABLE_LAG_PORT_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.port_mine_table_lag_port_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHP_ID,
          SOC_PB_IHP_PORT_MINE_TABLE_LAG_PORT_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->lag_port_mine0),
          &(tbl_data->lag_port_mine0)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->lag_port_mine1),
          &(tbl_data->lag_port_mine1)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->lag_port_mine2),
          &(tbl_data->lag_port_mine2)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->lag_port_mine3),
          &(tbl_data->lag_port_mine3)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihp_port_mine_table_lag_port_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_ihp_port_mine_table_lag_port_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHP_PORT_MINE_TABLE_LAG_PORT_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHP_PORT_MINE_TABLE_LAG_PORT_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHP_PORT_MINE_TABLE_LAG_PORT_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHP_PORT_MINE_TABLE_LAG_PORT_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHP_PORT_MINE_TABLE_LAG_PORT_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.port_mine_table_lag_port_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->lag_port_mine0),
          &(tbl->lag_port_mine0),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->lag_port_mine1),
          &(tbl->lag_port_mine1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->lag_port_mine2),
          &(tbl->lag_port_mine2),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->lag_port_mine3),
          &(tbl->lag_port_mine3),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHP_ID,
          SOC_PB_IHP_PORT_MINE_TABLE_LAG_PORT_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihp_port_mine_table_lag_port_tbl_set_unsafe()", entry_offset, 0);
}
/*
 * Read indirect table tm_port_pp_port_config_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihp_tm_port_pp_port_config_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHP_TM_PORT_PP_PORT_CONFIG_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHP_TM_PORT_PP_PORT_CONFIG_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHP_TM_PORT_PP_PORT_CONFIG_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHP_TM_PORT_PP_PORT_CONFIG_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHP_TM_PORT_PP_PORT_CONFIG_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHP_TM_PORT_PP_PORT_CONFIG_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.tm_port_pp_port_config_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHP_ID,
          SOC_PB_IHP_TM_PORT_PP_PORT_CONFIG_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->pp_port_offset1),
          &(tbl_data->pp_port_offset1)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->pp_port_offset2),
          &(tbl_data->pp_port_offset2)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->pp_port_profile),
          &(tbl_data->pp_port_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->pp_port_use_offset_directly),
          &(tbl_data->pp_port_use_offset_directly)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihp_tm_port_pp_port_config_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table tm_port_pp_port_config_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihp_tm_port_pp_port_config_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHP_TM_PORT_PP_PORT_CONFIG_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHP_TM_PORT_PP_PORT_CONFIG_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHP_TM_PORT_PP_PORT_CONFIG_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHP_TM_PORT_PP_PORT_CONFIG_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHP_TM_PORT_PP_PORT_CONFIG_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.tm_port_pp_port_config_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->pp_port_offset1),
          &(tbl->pp_port_offset1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->pp_port_offset2),
          &(tbl->pp_port_offset2),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->pp_port_profile),
          &(tbl->pp_port_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->pp_port_use_offset_directly),
          &(tbl->pp_port_use_offset_directly),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHP_ID,
          SOC_PB_IHP_TM_PORT_PP_PORT_CONFIG_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihp_tm_port_pp_port_config_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table tm_port_sys_port_config_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihp_tm_port_sys_port_config_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHP_TM_PORT_SYS_PORT_CONFIG_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.tm_port_sys_port_config_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHP_ID,
          SOC_PB_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->system_port_offset1),
          &(tbl_data->system_port_offset1)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->system_port_offset2),
          &(tbl_data->system_port_offset2)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->system_port_value),
          &(tbl_data->system_port_value)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->system_port_profile),
          &(tbl_data->system_port_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->system_port_value_to_use),
          &(tbl_data->system_port_value_to_use)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihp_tm_port_sys_port_config_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table tm_port_sys_port_config_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihp_tm_port_sys_port_config_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHP_TM_PORT_SYS_PORT_CONFIG_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.tm_port_sys_port_config_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->system_port_offset1),
          &(tbl->system_port_offset1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->system_port_offset2),
          &(tbl->system_port_offset2),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->system_port_value),
          &(tbl->system_port_value),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->system_port_profile),
          &(tbl->system_port_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->system_port_value_to_use),
          &(tbl->system_port_value_to_use),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHP_ID,
          SOC_PB_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihp_tm_port_sys_port_config_tbl_set_unsafe()", entry_offset, 0);
}


/*
 * Read indirect table pp_port_values_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihp_pp_port_values_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHP_PP_PORT_VALUES_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHP_PP_PORT_VALUES_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHP_PP_PORT_VALUES_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHP_PP_PORT_VALUES_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHP_PP_PORT_VALUES_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHP_PP_PORT_VALUES_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.pp_port_values_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHP_ID,
          SOC_PB_IHP_PP_PORT_VALUES_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->pp_port_system_port_value),
          &(tbl_data->pp_port_system_port_value)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihp_pp_port_values_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table pp_port_values_tbl from block IHP,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihp_pp_port_values_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHP_PP_PORT_VALUES_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHP_PP_PORT_VALUES_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHP_PP_PORT_VALUES_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHP_PP_PORT_VALUES_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHP_PP_PORT_VALUES_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihp.pp_port_values_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->pp_port_system_port_value),
          &(tbl->pp_port_system_port_value),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHP_ID,
          SOC_PB_IHP_PP_PORT_VALUES_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihp_pp_port_values_tbl_set_unsafe()", entry_offset, 0);
}



/*
 * Read indirect table cnm_descriptor_static_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_cnm_descriptor_static_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IQM_CNM_DESCRIPTOR_STATIC_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IQM_CNM_DESCRIPTOR_STATIC_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IQM_CNM_DESCRIPTOR_STATIC_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IQM_CNM_DESCRIPTOR_STATIC_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IQM_CNM_DESCRIPTOR_STATIC_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IQM_CNM_DESCRIPTOR_STATIC_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->iqm.cnm_descriptor_static_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IQM_ID,
          SOC_PB_IQM_CNM_DESCRIPTOR_STATIC_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cp_enable),
          &(tbl_data->cp_enable)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cp_class),
          &(tbl_data->cp_class)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cp_id),
          &(tbl_data->cp_id)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_iqm_cnm_descriptor_static_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table cnm_descriptor_static_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_cnm_descriptor_static_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IQM_CNM_DESCRIPTOR_STATIC_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IQM_CNM_DESCRIPTOR_STATIC_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IQM_CNM_DESCRIPTOR_STATIC_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IQM_CNM_DESCRIPTOR_STATIC_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IQM_CNM_DESCRIPTOR_STATIC_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->iqm.cnm_descriptor_static_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->cp_enable),
          &(tbl->cp_enable),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cp_class),
          &(tbl->cp_class),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cp_id),
          &(tbl->cp_id),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IQM_ID,
          SOC_PB_IQM_CNM_DESCRIPTOR_STATIC_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_iqm_cnm_descriptor_static_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table cnm_descriptor_dynamic_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_cnm_descriptor_dynamic_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IQM_CNM_DESCRIPTOR_DYNAMIC_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IQM_CNM_DESCRIPTOR_DYNAMIC_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IQM_CNM_DESCRIPTOR_DYNAMIC_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IQM_CNM_DESCRIPTOR_DYNAMIC_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IQM_CNM_DESCRIPTOR_DYNAMIC_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IQM_CNM_DESCRIPTOR_DYNAMIC_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->iqm.cnm_descriptor_dynamic_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IQM_ID,
          SOC_PB_IQM_CNM_DESCRIPTOR_DYNAMIC_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cp_enqued1),
          &(tbl_data->cp_enqued1)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cp_qsize_old),
          &(tbl_data->cp_qsize_old)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_iqm_cnm_descriptor_dynamic_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table cnm_descriptor_dynamic_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_cnm_descriptor_dynamic_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IQM_CNM_DESCRIPTOR_DYNAMIC_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IQM_CNM_DESCRIPTOR_DYNAMIC_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IQM_CNM_DESCRIPTOR_DYNAMIC_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IQM_CNM_DESCRIPTOR_DYNAMIC_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IQM_CNM_DESCRIPTOR_DYNAMIC_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->iqm.cnm_descriptor_dynamic_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->cp_enqued1),
          &(tbl->cp_enqued1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cp_qsize_old),
          &(tbl->cp_qsize_old),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IQM_ID,
          SOC_PB_IQM_CNM_DESCRIPTOR_DYNAMIC_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_iqm_cnm_descriptor_dynamic_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table cnm_parameters_table_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_cnm_parameters_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IQM_CNM_PARAMETERS_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    ind,
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IQM_CNM_PARAMETERS_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IQM_CNM_PARAMETERS_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IQM_CNM_PARAMETERS_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IQM_CNM_PARAMETERS_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IQM_CNM_PARAMETERS_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->iqm.cnm_parameters_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IQM_ID,
          SOC_PB_IQM_CNM_PARAMETERS_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cp_qeq),
          &(tbl_data->cp_qeq)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cp_w),
          &(tbl_data->cp_w)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cp_fb_max_val),
          &(tbl_data->cp_fb_max_val)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cp_quant_div),
          &(tbl_data->cp_quant_div)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  for (ind =0 ; ind < SOC_PB_CNM_CP_SAMPLE_BASE_SIZE; ind++)
  {
    res = soc_petra_field_in_place_get(
      data,
      &(tbl->cp_sample_base[ind]),
      &(tbl_data->cp_sample_base[ind])
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);
  }
    

 


  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cp_fixed_sample_base),
          &(tbl_data->cp_fixed_sample_base)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_iqm_cnm_parameters_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table cnm_parameters_table_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_cnm_parameters_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IQM_CNM_PARAMETERS_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    ind = 0,
    offset  = 0,
    data[SOC_PB_IQM_CNM_PARAMETERS_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IQM_CNM_PARAMETERS_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IQM_CNM_PARAMETERS_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IQM_CNM_PARAMETERS_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->iqm.cnm_parameters_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->cp_qeq),
          &(tbl->cp_qeq),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cp_w),
          &(tbl->cp_w),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cp_fb_max_val),
          &(tbl->cp_fb_max_val),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cp_quant_div),
          &(tbl->cp_quant_div),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  for (ind = 0; ind < SOC_PB_CNM_CP_SAMPLE_BASE_SIZE ; ind++)
  {
  
    res = soc_petra_field_in_place_set(
          &(tbl_data->cp_sample_base[ind]),
          &(tbl->cp_sample_base[ind]),
          data
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);
  }
 

  res = soc_petra_field_in_place_set(
          &(tbl_data->cp_fixed_sample_base),
          &(tbl->cp_fixed_sample_base),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IQM_ID,
          SOC_PB_IQM_CNM_PARAMETERS_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_iqm_cnm_parameters_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table meter_processor_result_resolve_table_static_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_meter_processor_result_resolve_table_static_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IQM_METER_PROCESSOR_RESULT_RESOLVE_TABLE_STATIC_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IQM_METER_PROCESSOR_RESULT_RESOLVE_TABLE_STATIC_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IQM_METER_PROCESSOR_RESULT_RESOLVE_TABLE_STATIC_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IQM_METER_PROCESSOR_RESULT_RESOLVE_TABLE_STATIC_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IQM_METER_PROCESSOR_RESULT_RESOLVE_TABLE_STATIC_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IQM_METER_PROCESSOR_RESULT_RESOLVE_TABLE_STATIC_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->iqm.meter_processor_result_resolve_table_static_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IQM_ID,
          SOC_PB_IQM_METER_PROCESSOR_RESULT_RESOLVE_TABLE_STATIC_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->iqm_dp),
          &(tbl_data->iqm_dp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->etm_de),
          &(tbl_data->etm_de)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_iqm_meter_processor_result_resolve_table_static_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table meter_processor_result_resolve_table_static_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_meter_processor_result_resolve_table_static_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IQM_METER_PROCESSOR_RESULT_RESOLVE_TABLE_STATIC_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IQM_METER_PROCESSOR_RESULT_RESOLVE_TABLE_STATIC_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IQM_METER_PROCESSOR_RESULT_RESOLVE_TABLE_STATIC_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IQM_METER_PROCESSOR_RESULT_RESOLVE_TABLE_STATIC_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IQM_METER_PROCESSOR_RESULT_RESOLVE_TABLE_STATIC_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->iqm.meter_processor_result_resolve_table_static_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->iqm_dp),
          &(tbl->iqm_dp),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->etm_de),
          &(tbl->etm_de),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IQM_ID,
          SOC_PB_IQM_METER_PROCESSOR_RESULT_RESOLVE_TABLE_STATIC_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_iqm_meter_processor_result_resolve_table_static_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table prfsel_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_prfsel_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              tbl_index,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IQM_PRFSEL_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IQM_PRFSEL_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IQM_PRFSEL_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IQM_PRFSEL_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IQM_PRFSEL_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IQM_PRFSEL_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (tbl_index >= SOC_PB_IQM_PRFSEL_NOF_TBLS)
{
    SOC_SAND_SET_ERROR_CODE(SOC_PB_IQM_PRFSEL_TBL_INDEX_OUT_OF_RANGE_ERR, 10, exit);
}

  tbl = &(tables->iqm.prfsel_tbl[tbl_index]);
  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IQM_ID,
          SOC_PB_IQM_PRFSEL_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->profile),
          &(tbl_data->profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_iqm_prfsel_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table prfsel_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_prfsel_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              tbl_index,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IQM_PRFSEL_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IQM_PRFSEL_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IQM_PRFSEL_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IQM_PRFSEL_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IQM_PRFSEL_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (tbl_index >= SOC_PB_IQM_PRFSEL_NOF_TBLS)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_IQM_PRFSEL_TBL_INDEX_OUT_OF_RANGE_ERR, 10, exit);
  }

  tbl = &(tables->iqm.prfsel_tbl[tbl_index]);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->profile),
          &(tbl->profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IQM_ID,
          SOC_PB_IQM_PRFSEL_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_iqm_prfsel_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table prfcfg_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_prfcfg_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              tbl_index,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IQM_PRFCFG_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IQM_PRFCFG_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IQM_PRFCFG_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IQM_PRFCFG_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IQM_PRFCFG_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IQM_PRFCFG_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (tbl_index >= SOC_PB_IQM_PRFCFG_NOF_TBLS)
{
    SOC_SAND_SET_ERROR_CODE(SOC_PB_IQM_PRFCFG_TBL_INDEX_OUT_OF_RANGE_ERR, 10, exit);
}

  tbl = &(tables->iqm.prfcfg_tbl[tbl_index]);
  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IQM_ID,
          SOC_PB_IQM_PRFCFG_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->color_aware),
          &(tbl_data->color_aware)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->coupling_flag),
          &(tbl_data->coupling_flag)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->fairness_mode),
          &(tbl_data->fairness_mode)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cir_mantissa_64),
          &(tbl_data->cir_mantissa_64)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cir_reverse_exponent),
          &(tbl_data->cir_reverse_exponent)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->reset_cir),
          &(tbl_data->reset_cir)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cbs_mantissa_64),
          &(tbl_data->cbs_mantissa_64)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cbs_exponent),
          &(tbl_data->cbs_exponent)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->eir_mantissa_64),
          &(tbl_data->eir_mantissa_64)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->eir_reverse_exponent),
          &(tbl_data->eir_reverse_exponent)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->reset_eir),
          &(tbl_data->reset_eir)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ebs_mantissa_64),
          &(tbl_data->ebs_mantissa_64)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 51, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ebs_exponent),
          &(tbl_data->ebs_exponent)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_iqm_prfcfg_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table prfcfg_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_prfcfg_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              tbl_index,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IQM_PRFCFG_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IQM_PRFCFG_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IQM_PRFCFG_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IQM_PRFCFG_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IQM_PRFCFG_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if (tbl_index >= SOC_PB_IQM_PRFCFG_NOF_TBLS)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_IQM_PRFSEL_TBL_INDEX_OUT_OF_RANGE_ERR, 10, exit);
  }

  tbl = &(tables->iqm.prfcfg_tbl[tbl_index]);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->color_aware),
          &(tbl->color_aware),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->coupling_flag),
          &(tbl->coupling_flag),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->fairness_mode),
          &(tbl->fairness_mode),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cir_mantissa_64),
          &(tbl->cir_mantissa_64),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cir_reverse_exponent),
          &(tbl->cir_reverse_exponent),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->reset_cir),
          &(tbl->reset_cir),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cbs_mantissa_64),
          &(tbl->cbs_mantissa_64),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cbs_exponent),
          &(tbl->cbs_exponent),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->eir_mantissa_64),
          &(tbl->eir_mantissa_64),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->eir_reverse_exponent),
          &(tbl->eir_reverse_exponent),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 39, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->reset_eir),
          &(tbl->reset_eir),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->ebs_mantissa_64),
          &(tbl->ebs_mantissa_64),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->ebs_exponent),
          &(tbl->ebs_exponent),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IQM_ID,
          SOC_PB_IQM_PRFCFG_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_iqm_prfcfg_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table normal_dynamica_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_normal_dynamica_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IQM_NORMAL_DYNAMICA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IQM_NORMAL_DYNAMICA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IQM_NORMAL_DYNAMICA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IQM_NORMAL_DYNAMICA_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IQM_NORMAL_DYNAMICA_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IQM_NORMAL_DYNAMICA_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->iqm.normal_dynamica_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IQM_ID,
          SOC_PB_IQM_NORMAL_DYNAMICA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->time_stamp),
          &(tbl_data->time_stamp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cbl),
          &(tbl_data->cbl)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ebl),
          &(tbl_data->ebl)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_iqm_normal_dynamica_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table normal_dynamica_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_normal_dynamica_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IQM_NORMAL_DYNAMICA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IQM_NORMAL_DYNAMICA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IQM_NORMAL_DYNAMICA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IQM_NORMAL_DYNAMICA_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IQM_NORMAL_DYNAMICA_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->iqm.normal_dynamica_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->time_stamp),
          &(tbl->time_stamp),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cbl),
          &(tbl->cbl),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->ebl),
          &(tbl->ebl),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IQM_ID,
          SOC_PB_IQM_NORMAL_DYNAMICA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_iqm_normal_dynamica_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table normal_dynamicb_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_normal_dynamicb_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IQM_NORMAL_DYNAMICB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IQM_NORMAL_DYNAMICB_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IQM_NORMAL_DYNAMICB_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IQM_NORMAL_DYNAMICB_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IQM_NORMAL_DYNAMICB_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IQM_NORMAL_DYNAMICB_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->iqm.normal_dynamicb_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IQM_ID,
          SOC_PB_IQM_NORMAL_DYNAMICB_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->time_stamp),
          &(tbl_data->time_stamp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cbl),
          &(tbl_data->cbl)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ebl),
          &(tbl_data->ebl)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_iqm_normal_dynamicb_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table normal_dynamicb_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_normal_dynamicb_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IQM_NORMAL_DYNAMICB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IQM_NORMAL_DYNAMICB_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IQM_NORMAL_DYNAMICB_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IQM_NORMAL_DYNAMICB_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IQM_NORMAL_DYNAMICB_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->iqm.normal_dynamicb_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->time_stamp),
          &(tbl->time_stamp),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cbl),
          &(tbl->cbl),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->ebl),
          &(tbl->ebl),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IQM_ID,
          SOC_PB_IQM_NORMAL_DYNAMICB_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_iqm_normal_dynamicb_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table high_dynamica_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_high_dynamica_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IQM_HIGH_DYNAMICA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IQM_HIGH_DYNAMICA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IQM_HIGH_DYNAMICA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IQM_HIGH_DYNAMICA_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IQM_HIGH_DYNAMICA_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IQM_HIGH_DYNAMICA_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->iqm.high_dynamica_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IQM_ID,
          SOC_PB_IQM_HIGH_DYNAMICA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->time_stamp),
          &(tbl_data->time_stamp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cbl),
          &(tbl_data->cbl)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ebl),
          &(tbl_data->ebl)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_iqm_high_dynamica_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table high_dynamica_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_high_dynamica_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IQM_HIGH_DYNAMICA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IQM_HIGH_DYNAMICA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IQM_HIGH_DYNAMICA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IQM_HIGH_DYNAMICA_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IQM_HIGH_DYNAMICA_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->iqm.high_dynamica_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->time_stamp),
          &(tbl->time_stamp),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cbl),
          &(tbl->cbl),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->ebl),
          &(tbl->ebl),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IQM_ID,
          SOC_PB_IQM_HIGH_DYNAMICA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_iqm_high_dynamica_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table high_dynamicb_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_high_dynamicb_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IQM_HIGH_DYNAMICB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IQM_HIGH_DYNAMICB_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IQM_HIGH_DYNAMICB_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IQM_HIGH_DYNAMICB_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IQM_HIGH_DYNAMICB_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IQM_HIGH_DYNAMICB_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->iqm.high_dynamicb_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IQM_ID,
          SOC_PB_IQM_HIGH_DYNAMICB_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->time_stamp),
          &(tbl_data->time_stamp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cbl),
          &(tbl_data->cbl)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ebl),
          &(tbl_data->ebl)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_iqm_high_dynamicb_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table high_dynamicb_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_high_dynamicb_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IQM_HIGH_DYNAMICB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IQM_HIGH_DYNAMICB_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IQM_HIGH_DYNAMICB_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IQM_HIGH_DYNAMICB_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IQM_HIGH_DYNAMICB_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->iqm.high_dynamicb_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->time_stamp),
          &(tbl->time_stamp),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cbl),
          &(tbl->cbl),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->ebl),
          &(tbl->ebl),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IQM_ID,
          SOC_PB_IQM_HIGH_DYNAMICB_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_iqm_high_dynamicb_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table cnts_mema_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_cnts_mem_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_PB_CNT_PROCESSOR_ID   proc_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IQM_CNTS_MEM_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IQM_CNTS_MEMA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IQM_CNTS_MEM_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IQM_CNTS_MEMA_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IQM_CNTS_MEMA_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->iqm.cnts_mem_tbl[proc_id]);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->packets_counter),
          &(tbl->packets_counter),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->octets_counter),
          &(tbl->octets_counter),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IQM_ID,
          SOC_PB_IQM_CNTS_MEMA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_iqm_cnts_mema_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table cnts_memb_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_cnts_mem_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_PB_CNT_PROCESSOR_ID   proc_id,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IQM_CNTS_MEM_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IQM_CNTS_MEMB_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IQM_CNTS_MEM_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IQM_CNTS_MEMB_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IQM_CNTS_MEMB_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IQM_CNTS_MEM_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->iqm.cnts_mem_tbl[proc_id]);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IQM_ID,
          SOC_PB_IQM_CNTS_MEMB_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->packets_counter),
          &(tbl_data->packets_counter)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->octets_counter),
          &(tbl_data->octets_counter)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_iqm_cnts_mem_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table ovth_mema_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_ovth_mema_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IQM_OVTH_MEMA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IQM_OVTH_MEMA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IQM_OVTH_MEMA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IQM_OVTH_MEMA_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IQM_OVTH_MEMA_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IQM_OVTH_MEMA_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->iqm.ovth_mema_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IQM_ID,
          SOC_PB_IQM_OVTH_MEMA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ovth_counter_bits),
          (tbl_data->ovth_counter_bits)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_iqm_ovth_mema_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table ovth_mema_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_ovth_mema_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IQM_OVTH_MEMA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IQM_OVTH_MEMA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IQM_OVTH_MEMA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IQM_OVTH_MEMA_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IQM_OVTH_MEMA_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->iqm.ovth_mema_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          (tbl_data->ovth_counter_bits),
          &(tbl->ovth_counter_bits),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IQM_ID,
          SOC_PB_IQM_OVTH_MEMA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_iqm_ovth_mema_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table ovth_memb_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_ovth_memb_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IQM_OVTH_MEMB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IQM_OVTH_MEMB_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IQM_OVTH_MEMB_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IQM_OVTH_MEMB_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IQM_OVTH_MEMB_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IQM_OVTH_MEMB_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->iqm.ovth_memb_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IQM_ID,
          SOC_PB_IQM_OVTH_MEMB_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ovth_counter_bits),
          (tbl_data->ovth_counter_bits)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_iqm_ovth_memb_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table ovth_memb_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_iqm_ovth_memb_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IQM_OVTH_MEMB_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IQM_OVTH_MEMB_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IQM_OVTH_MEMB_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IQM_OVTH_MEMB_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IQM_OVTH_MEMB_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->iqm.ovth_memb_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          (tbl_data->ovth_counter_bits),
          &(tbl->ovth_counter_bits),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IQM_ID,
          SOC_PB_IQM_OVTH_MEMB_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_iqm_ovth_memb_tbl_set_unsafe()", entry_offset, 0);
}


/*
 * Read indirect table sop_mmu_tbl from block IPT,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ipt_sop_mmu_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IPT_SOP_MMU_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IPT_SOP_MMU_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IPT_SOP_MMU_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IPT_SOP_MMU_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IPT_SOP_MMU_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IPT_SOP_MMU_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ipt.sop_mmu_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IPT_ID,
          SOC_PB_IPT_SOP_MMU_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->sop_mmu),
          (tbl_data->sop_mmu)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ipt_sop_mmu_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table sop_mmu_tbl from block IPT,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ipt_sop_mmu_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IPT_SOP_MMU_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IPT_SOP_MMU_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IPT_SOP_MMU_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IPT_SOP_MMU_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IPT_SOP_MMU_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ipt.sop_mmu_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          (tbl_data->sop_mmu),
          &(tbl->sop_mmu),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IPT_ID,
          SOC_PB_IPT_SOP_MMU_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ipt_sop_mmu_tbl_set_unsafe()", entry_offset, 0);
}
uint32
  soc_pb_ipt_egq_txq_rd_addr_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IPT_EGQ_TXQ_RD_ADDR_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IPT_EGQ_TXQ_RD_ADDR_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IPT_EGQ_TXQ_RD_ADDR_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IPT_EGQ_TXQ_RD_ADDR_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IPT_EGQ_TXQ_RD_ADDR_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IPT_EGQ_TXQ_RD_ADDR_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ipt.egq_txq_rd_addr_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IPT_ID,
          SOC_PB_IPT_EGQ_TXQ_RD_ADDR_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->egq_txq_rd_addr),
          &(tbl_data->egq_txq_rd_addr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ipt_egq_txq_rd_addr_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_ipt_egq_txq_rd_addr_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IPT_EGQ_TXQ_RD_ADDR_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IPT_EGQ_TXQ_RD_ADDR_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IPT_EGQ_TXQ_RD_ADDR_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IPT_EGQ_TXQ_RD_ADDR_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IPT_EGQ_TXQ_RD_ADDR_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ipt.egq_txq_rd_addr_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->egq_txq_rd_addr),
          &(tbl->egq_txq_rd_addr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IPT_ID,
          SOC_PB_IPT_EGQ_TXQ_RD_ADDR_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ipt_egq_txq_rd_addr_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_ipt_fdt_txq_wr_addr_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IPT_FDT_TXQ_WR_ADDR_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IPT_FDT_TXQ_WR_ADDR_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IPT_FDT_TXQ_WR_ADDR_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IPT_FDT_TXQ_WR_ADDR_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IPT_FDT_TXQ_WR_ADDR_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IPT_FDT_TXQ_WR_ADDR_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ipt.fdt_txq_wr_addr_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IPT_ID,
          SOC_PB_IPT_FDT_TXQ_WR_ADDR_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->dtq0_wr_addr),
          &(tbl_data->dtq0_wr_addr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->dtq1_wr_addr),
          &(tbl_data->dtq1_wr_addr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->dtq2_wr_addr),
          &(tbl_data->dtq2_wr_addr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->dtq3_wr_addr),
          &(tbl_data->dtq3_wr_addr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->dtq4_wr_addr),
          &(tbl_data->dtq4_wr_addr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->dtq5_wr_addr),
          &(tbl_data->dtq5_wr_addr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->dtq6_wr_addr),
          &(tbl_data->dtq6_wr_addr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ipt_fdt_txq_wr_addr_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_ipt_fdt_txq_wr_addr_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IPT_FDT_TXQ_WR_ADDR_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IPT_FDT_TXQ_WR_ADDR_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IPT_FDT_TXQ_WR_ADDR_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IPT_FDT_TXQ_WR_ADDR_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IPT_FDT_TXQ_WR_ADDR_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ipt.fdt_txq_wr_addr_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->dtq0_wr_addr),
          &(tbl->dtq0_wr_addr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->dtq1_wr_addr),
          &(tbl->dtq1_wr_addr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->dtq2_wr_addr),
          &(tbl->dtq2_wr_addr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->dtq3_wr_addr),
          &(tbl->dtq3_wr_addr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->dtq4_wr_addr),
          &(tbl->dtq4_wr_addr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->dtq5_wr_addr),
          &(tbl->dtq5_wr_addr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->dtq6_wr_addr),
          &(tbl->dtq6_wr_addr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IPT_ID,
          SOC_PB_IPT_FDT_TXQ_WR_ADDR_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ipt_fdt_txq_wr_addr_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_ipt_fdt_txq_rd_addr_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IPT_FDT_TXQ_RD_ADDR_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IPT_FDT_TXQ_RD_ADDR_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IPT_FDT_TXQ_RD_ADDR_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IPT_FDT_TXQ_RD_ADDR_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IPT_FDT_TXQ_RD_ADDR_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IPT_FDT_TXQ_RD_ADDR_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ipt.fdt_txq_rd_addr_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IPT_ID,
          SOC_PB_IPT_FDT_TXQ_RD_ADDR_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->dtq0_rd_addr),
          &(tbl_data->dtq0_rd_addr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->dtq1_rd_addr),
          &(tbl_data->dtq1_rd_addr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->dtq2_rd_addr),
          &(tbl_data->dtq2_rd_addr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->dtq3_rd_addr),
          &(tbl_data->dtq3_rd_addr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->dtq4_rd_addr),
          &(tbl_data->dtq4_rd_addr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->dtq5_rd_addr),
          &(tbl_data->dtq5_rd_addr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->dtq6_rd_addr),
          &(tbl_data->dtq6_rd_addr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ipt_fdt_txq_rd_addr_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_ipt_fdt_txq_rd_addr_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IPT_FDT_TXQ_RD_ADDR_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IPT_FDT_TXQ_RD_ADDR_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IPT_FDT_TXQ_RD_ADDR_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IPT_FDT_TXQ_RD_ADDR_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IPT_FDT_TXQ_RD_ADDR_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ipt.fdt_txq_rd_addr_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->dtq0_rd_addr),
          &(tbl->dtq0_rd_addr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->dtq1_rd_addr),
          &(tbl->dtq1_rd_addr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->dtq2_rd_addr),
          &(tbl->dtq2_rd_addr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->dtq3_rd_addr),
          &(tbl->dtq3_rd_addr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->dtq4_rd_addr),
          &(tbl->dtq4_rd_addr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->dtq5_rd_addr),
          &(tbl->dtq5_rd_addr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->dtq6_rd_addr),
          &(tbl->dtq6_rd_addr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IPT_ID,
          SOC_PB_IPT_FDT_TXQ_RD_ADDR_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ipt_fdt_txq_rd_addr_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_ipt_gci_backoff_mask_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IPT_GCI_BACKOFF_MASK_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IPT_GCI_BACKOFF_MASK_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IPT_GCI_BACKOFF_MASK_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IPT_GCI_BACKOFF_MASK_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IPT_GCI_BACKOFF_MASK_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IPT_GCI_BACKOFF_MASK_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ipt.gci_backoff_mask_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IPT_ID,
          SOC_PB_IPT_GCI_BACKOFF_MASK_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->mask0),
          &(tbl_data->mask0)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->mask1),
          &(tbl_data->mask1)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->mask2),
          &(tbl_data->mask2)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->mask3),
          &(tbl_data->mask3)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ipt_gci_backoff_mask_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_ipt_gci_backoff_mask_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IPT_GCI_BACKOFF_MASK_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IPT_GCI_BACKOFF_MASK_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IPT_GCI_BACKOFF_MASK_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IPT_GCI_BACKOFF_MASK_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IPT_GCI_BACKOFF_MASK_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ipt.gci_backoff_mask_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->mask0),
          &(tbl->mask0),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->mask1),
          &(tbl->mask1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->mask2),
          &(tbl->mask2),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->mask3),
          &(tbl->mask3),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IPT_ID,
          SOC_PB_IPT_GCI_BACKOFF_MASK_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ipt_gci_backoff_mask_tbl_set_unsafe()", entry_offset, 0);
}


/*
 * Read indirect table ipt_contro_l_fifo_tbl from block FDT,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_fdt_ipt_contro_l_fifo_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_FDT_IPT_CONTRO_L_FIFO_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_FDT_IPT_CONTRO_L_FIFO_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_FDT_IPT_CONTRO_L_FIFO_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_FDT_IPT_CONTRO_L_FIFO_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_FDT_IPT_CONTRO_L_FIFO_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_FDT_IPT_CONTRO_L_FIFO_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->fdt.ipt_contro_l_fifo_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_FDT_ID,
          SOC_PB_FDT_IPT_CONTRO_L_FIFO_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ipt_contro_l_fifo),
          (tbl_data->ipt_contro_l_fifo)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_fdt_ipt_contro_l_fifo_tbl_get_unsafe()", entry_offset, 0);
}


/*
 * Read indirect table qm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_egq_qm_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_EGQ_QM_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_EGQ_QM_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_EGQ_QM_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGQ_QM_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_EGQ_QM_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_EGQ_QM_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->egq.qm_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_EGQ_ID,
          SOC_PB_EGQ_QM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->read_pointer),
          &(tbl_data->read_pointer)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->packet_size256to_eop),
          &(tbl_data->packet_size256to_eop)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->counter_decreament),
          &(tbl_data->counter_decreament)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->copy_data),
          &(tbl_data->copy_data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->last_seg_size),
          &(tbl_data->last_seg_size)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->before_last_seg_size),
          &(tbl_data->before_last_seg_size)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->pqp_qnum),
          &(tbl_data->pqp_qnum)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->pqp_oc768_qnum),
          &(tbl_data->pqp_oc768_qnum)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egq_qm_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table qm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_egq_qm_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_EGQ_QM_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_EGQ_QM_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_EGQ_QM_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGQ_QM_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_EGQ_QM_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->egq.qm_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->read_pointer),
          &(tbl->read_pointer),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->packet_size256to_eop),
          &(tbl->packet_size256to_eop),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->counter_decreament),
          &(tbl->counter_decreament),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->copy_data),
          &(tbl->copy_data),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->last_seg_size),
          &(tbl->last_seg_size),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->before_last_seg_size),
          &(tbl->before_last_seg_size),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->pqp_qnum),
          &(tbl->pqp_qnum),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->pqp_oc768_qnum),
          &(tbl->pqp_oc768_qnum),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_EGQ_ID,
          SOC_PB_EGQ_QM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egq_qm_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table qsm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_egq_qsm_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_EGQ_QSM_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_EGQ_QSM_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_EGQ_QSM_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGQ_QSM_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_EGQ_QSM_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_EGQ_QSM_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->egq.qsm_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_EGQ_ID,
          SOC_PB_EGQ_QSM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->qsm),
          &(tbl_data->qsm)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egq_qsm_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table qsm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_egq_qsm_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_EGQ_QSM_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_EGQ_QSM_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_EGQ_QSM_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGQ_QSM_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_EGQ_QSM_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->egq.qsm_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->qsm),
          &(tbl->qsm),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_EGQ_ID,
          SOC_PB_EGQ_QSM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egq_qsm_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table dcm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_egq_dcm_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_EGQ_DCM_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_EGQ_DCM_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_EGQ_DCM_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGQ_DCM_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_EGQ_DCM_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_EGQ_DCM_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->egq.dcm_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_EGQ_ID,
          SOC_PB_EGQ_DCM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->dcm),
          &(tbl_data->dcm)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egq_dcm_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table dcm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_egq_dcm_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_EGQ_DCM_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_EGQ_DCM_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_EGQ_DCM_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGQ_DCM_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_EGQ_DCM_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->egq.dcm_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->dcm),
          &(tbl->dcm),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_EGQ_ID,
          SOC_PB_EGQ_DCM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egq_dcm_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table dwm_new_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_egq_dwm_new_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_EGQ_DWM_NEW_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_EGQ_DWM_NEW_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_EGQ_DWM_NEW_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGQ_DWM_NEW_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_EGQ_DWM_NEW_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_EGQ_DWM_NEW_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->egq.dwm_new_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_EGQ_ID,
          SOC_PB_EGQ_DWM_NEW_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->mc_high_queue_weight),
          &(tbl_data->mc_high_queue_weight)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->uc_high_queue_weight),
          &(tbl_data->uc_high_queue_weight)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egq_dwm_new_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table dwm_new_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_egq_dwm_new_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_EGQ_DWM_NEW_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_EGQ_DWM_NEW_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_EGQ_DWM_NEW_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGQ_DWM_NEW_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_EGQ_DWM_NEW_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->egq.dwm_new_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->mc_high_queue_weight),
          &(tbl->mc_high_queue_weight),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->uc_high_queue_weight),
          &(tbl->uc_high_queue_weight),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_EGQ_ID,
          SOC_PB_EGQ_DWM_NEW_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egq_dwm_new_tbl_set_unsafe()", entry_offset, 0);
}


/*
 * Read indirect table pct_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_egq_pct_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_EGQ_PCT_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_EGQ_PCT_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_EGQ_PCT_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGQ_PCT_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_EGQ_PCT_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_EGQ_PCT_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->egq.pct_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_EGQ_ID,
          SOC_PB_EGQ_PCT_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->prog_editor_value),
          &(tbl_data->prog_editor_value)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->out_pp_port),
          &(tbl_data->out_pp_port)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->outbound_mirr),
          &(tbl_data->outbound_mirr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->dest_port),
          &(tbl_data->dest_port)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->port_profile),
          &(tbl_data->port_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->port_ch_num),
          &(tbl_data->port_ch_num)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->port_type),
          &(tbl_data->port_type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->cr_adjust_type),
          &(tbl_data->cr_adjust_type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egq_pct_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table pct_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_egq_pct_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_EGQ_PCT_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_EGQ_PCT_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_EGQ_PCT_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGQ_PCT_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_EGQ_PCT_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->egq.pct_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->prog_editor_value),
          &(tbl->prog_editor_value),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->out_pp_port),
          &(tbl->out_pp_port),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->outbound_mirr),
          &(tbl->outbound_mirr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->dest_port),
          &(tbl->dest_port),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->port_profile),
          &(tbl->port_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->port_ch_num),
          &(tbl->port_ch_num),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->port_type),
          &(tbl->port_type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->cr_adjust_type),
          &(tbl->cr_adjust_type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_EGQ_ID,
          SOC_PB_EGQ_PCT_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egq_pct_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table tc_dp_map_table_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */STATIC
  uint32
    soc_pb_egq_tc_dp_map_table_entry_translate_unsafe(
      SOC_SAND_IN  int             unit,
      SOC_SAND_IN  SOC_PB_EGQ_TC_DP_MAP_TBL_ENTRY       *entry,
      SOC_SAND_OUT uint32                         *entry_offset
    )
{
  uint32
    offset = 0;

  offset |= SOC_SAND_SET_BITS_RANGE(entry->dp, 1, 0);
  offset |= SOC_SAND_SET_BITS_RANGE(entry->tc, 4, 2);
  offset |= SOC_SAND_SET_BITS_RANGE(entry->is_syst_mc, 5, 5);
  offset |= SOC_SAND_SET_BITS_RANGE(entry->is_egr_mc, 6, 6);
  offset |= SOC_SAND_SET_BITS_RANGE(entry->map_profile, 8, 7);

  *entry_offset = offset;

  return 0;
}

void
  SOC_PB_EGQ_TC_DP_MAP_TBL_ENTRY_clear(
    SOC_SAND_OUT SOC_PB_EGQ_TC_DP_MAP_TBL_ENTRY       *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_EGQ_TC_DP_MAP_TBL_ENTRY));
  info->tc = 0;
  info->dp = 0;
  info->is_syst_mc = 0;
  info->is_egr_mc = 0;
  info->map_profile = 0;

  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

uint32
  soc_pb_egq_tc_dp_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_PB_EGQ_TC_DP_MAP_TBL_ENTRY       *entry,
    SOC_SAND_OUT SOC_PB_EGQ_TC_DP_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    entry_offset = 0,
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_EGQ_TC_DP_MAP_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_EGQ_TC_DP_MAP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGQ_TC_DP_MAP_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_EGQ_TC_DP_MAP_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_EGQ_TC_DP_MAP_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->egq.tc_dp_map_tbl);

  soc_pb_egq_tc_dp_map_table_entry_translate_unsafe(
    unit,
    entry,
    &entry_offset
  );

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_EGQ_ID,
          SOC_PB_EGQ_TC_DP_MAP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->tc),
          &(tbl_data->tc)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->dp),
          &(tbl_data->dp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egq_tc_dp_map_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table tc_dp_map_table_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_egq_tc_dp_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_PB_EGQ_TC_DP_MAP_TBL_ENTRY       *entry,
    SOC_SAND_IN  SOC_PB_EGQ_TC_DP_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    entry_offset = 0,
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_EGQ_TC_DP_MAP_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_EGQ_TC_DP_MAP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGQ_TC_DP_MAP_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_EGQ_TC_DP_MAP_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->egq.tc_dp_map_tbl);

  soc_pb_egq_tc_dp_map_table_entry_translate_unsafe(
    unit,
    entry,
    &entry_offset
  );

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->tc),
          &(tbl->tc),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->dp),
          &(tbl->dp),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_EGQ_ID,
          SOC_PB_EGQ_TC_DP_MAP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egq_tc_dp_map_tbl_set_unsafe()", entry_offset, 0);
}


/*
 * Read indirect table fqp_nif_port_mux_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_egq_fqp_nif_port_mux_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_EGQ_FQP_NIF_PORT_MUX_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_EGQ_FQP_NIF_PORT_MUX_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_EGQ_FQP_NIF_PORT_MUX_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGQ_FQP_NIF_PORT_MUX_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_EGQ_FQP_NIF_PORT_MUX_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_EGQ_FQP_NIF_PORT_MUX_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->egq.fqp_nif_port_mux_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_EGQ_ID,
          SOC_PB_EGQ_FQP_NIF_PORT_MUX_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->fqp_nif_port_mux),
          &(tbl_data->fqp_nif_port_mux)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egq_fqp_nif_port_mux_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fqp_nif_port_mux_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_egq_fqp_nif_port_mux_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_EGQ_FQP_NIF_PORT_MUX_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_EGQ_FQP_NIF_PORT_MUX_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_EGQ_FQP_NIF_PORT_MUX_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGQ_FQP_NIF_PORT_MUX_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_EGQ_FQP_NIF_PORT_MUX_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->egq.fqp_nif_port_mux_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->fqp_nif_port_mux),
          &(tbl->fqp_nif_port_mux),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_EGQ_ID,
          SOC_PB_EGQ_FQP_NIF_PORT_MUX_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egq_fqp_nif_port_mux_tbl_set_unsafe()", entry_offset, 0);
}
uint32
  soc_pb_egq_key_profile_map_index_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              fwd_type_ndx,
    SOC_SAND_IN  uint32              acl_profile_ndx,
    SOC_SAND_OUT SOC_PB_EGQ_KEY_PROFILE_MAP_INDEX_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_EGQ_KEY_PROFILE_MAP_INDEX_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_EGQ_KEY_PROFILE_MAP_INDEX_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGQ_KEY_PROFILE_MAP_INDEX_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_EGQ_KEY_PROFILE_MAP_INDEX_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_EGQ_KEY_PROFILE_MAP_INDEX_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->egq.key_profile_map_index_tbl);

  offset = tbl->addr.base + (fwd_type_ndx << 2) + acl_profile_ndx;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_EGQ_ID,
          SOC_PB_EGQ_KEY_PROFILE_MAP_INDEX_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->key_profile_map_index),
          &(tbl_data->key_profile_map_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egq_key_profile_map_index_tbl_get_unsafe()", fwd_type_ndx, acl_profile_ndx);
}

/*
 * Write indirect table key_profile_map_index_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_egq_key_profile_map_index_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              fwd_type_ndx,
    SOC_SAND_IN  uint32              acl_profile_ndx,
    SOC_SAND_IN  SOC_PB_EGQ_KEY_PROFILE_MAP_INDEX_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_EGQ_KEY_PROFILE_MAP_INDEX_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_EGQ_KEY_PROFILE_MAP_INDEX_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGQ_KEY_PROFILE_MAP_INDEX_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_EGQ_KEY_PROFILE_MAP_INDEX_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->egq.key_profile_map_index_tbl);

  offset =  tbl->addr.base + (fwd_type_ndx << 2) + acl_profile_ndx;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->key_profile_map_index),
          &(tbl->key_profile_map_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_EGQ_ID,
          SOC_PB_EGQ_KEY_PROFILE_MAP_INDEX_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egq_key_profile_map_index_tbl_set_unsafe()", fwd_type_ndx, acl_profile_ndx);
}

/*
 * Read indirect table tcam_key_resolution_profile_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_egq_tcam_key_resolution_profile_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->egq.tcam_key_resolution_profile_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_EGQ_ID,
          SOC_PB_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->key_select),
          &(tbl_data->key_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->key_and_value),
          &(tbl_data->key_and_value)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->key_or_value),
          &(tbl_data->key_or_value)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->tcam_profile),
          &(tbl_data->tcam_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egq_tcam_key_resolution_profile_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table tcam_key_resolution_profile_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_egq_tcam_key_resolution_profile_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->egq.tcam_key_resolution_profile_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->key_select),
          &(tbl->key_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->key_and_value),
          &(tbl->key_and_value),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->key_or_value),
          &(tbl->key_or_value),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->tcam_profile),
          &(tbl->tcam_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_EGQ_ID,
          SOC_PB_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egq_tcam_key_resolution_profile_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_egq_parser_last_sys_record_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_EGQ_PARSER_LAST_SYS_RECORD_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_EGQ_PARSER_LAST_SYS_RECORD_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_EGQ_PARSER_LAST_SYS_RECORD_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGQ_PARSER_LAST_SYS_RECORD_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_EGQ_PARSER_LAST_SYS_RECORD_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_EGQ_PARSER_LAST_SYS_RECORD_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->egq.parser_last_sys_record_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_EGQ_ID,
          SOC_PB_EGQ_PARSER_LAST_SYS_RECORD_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->parser_last_sys_record),
          (tbl_data->parser_last_sys_record)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egq_parser_last_sys_record_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_egq_parser_last_sys_record_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_EGQ_PARSER_LAST_SYS_RECORD_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_EGQ_PARSER_LAST_SYS_RECORD_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_EGQ_PARSER_LAST_SYS_RECORD_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGQ_PARSER_LAST_SYS_RECORD_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_EGQ_PARSER_LAST_SYS_RECORD_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->egq.parser_last_sys_record_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          (tbl_data->parser_last_sys_record),
          &(tbl->parser_last_sys_record),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_EGQ_ID,
          SOC_PB_EGQ_PARSER_LAST_SYS_RECORD_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egq_parser_last_sys_record_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_egq_parser_last_nwk_record1_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_EGQ_PARSER_LAST_NWK_RECORD1_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_EGQ_PARSER_LAST_NWK_RECORD1_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_EGQ_PARSER_LAST_NWK_RECORD1_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGQ_PARSER_LAST_NWK_RECORD1_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_EGQ_PARSER_LAST_NWK_RECORD1_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_EGQ_PARSER_LAST_NWK_RECORD1_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->egq.parser_last_nwk_record1_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_EGQ_ID,
          SOC_PB_EGQ_PARSER_LAST_NWK_RECORD1_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->parser_last_nwk_record1),
          (tbl_data->parser_last_nwk_record1)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egq_parser_last_nwk_record1_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_egq_parser_last_nwk_record1_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_EGQ_PARSER_LAST_NWK_RECORD1_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_EGQ_PARSER_LAST_NWK_RECORD1_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_EGQ_PARSER_LAST_NWK_RECORD1_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGQ_PARSER_LAST_NWK_RECORD1_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_EGQ_PARSER_LAST_NWK_RECORD1_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->egq.parser_last_nwk_record1_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          (tbl_data->parser_last_nwk_record1),
          &(tbl->parser_last_nwk_record1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_EGQ_ID,
          SOC_PB_EGQ_PARSER_LAST_NWK_RECORD1_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egq_parser_last_nwk_record1_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_egq_parser_last_nwk_record2_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_EGQ_PARSER_LAST_NWK_RECORD2_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_EGQ_PARSER_LAST_NWK_RECORD2_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_EGQ_PARSER_LAST_NWK_RECORD2_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGQ_PARSER_LAST_NWK_RECORD2_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_EGQ_PARSER_LAST_NWK_RECORD2_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_EGQ_PARSER_LAST_NWK_RECORD2_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->egq.parser_last_nwk_record2_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_EGQ_ID,
          SOC_PB_EGQ_PARSER_LAST_NWK_RECORD2_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->parser_last_nwk_record2),
          (tbl_data->parser_last_nwk_record2)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egq_parser_last_nwk_record2_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_egq_parser_last_nwk_record2_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_EGQ_PARSER_LAST_NWK_RECORD2_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_EGQ_PARSER_LAST_NWK_RECORD2_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_EGQ_PARSER_LAST_NWK_RECORD2_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGQ_PARSER_LAST_NWK_RECORD2_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_EGQ_PARSER_LAST_NWK_RECORD2_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->egq.parser_last_nwk_record2_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          (tbl_data->parser_last_nwk_record2),
          &(tbl->parser_last_nwk_record2),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_EGQ_ID,
          SOC_PB_EGQ_PARSER_LAST_NWK_RECORD2_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egq_parser_last_nwk_record2_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_egq_erpp_debug_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_EGQ_ERPP_DEBUG_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_EGQ_ERPP_DEBUG_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_EGQ_ERPP_DEBUG_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGQ_ERPP_DEBUG_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_EGQ_ERPP_DEBUG_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_EGQ_ERPP_DEBUG_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->egq.erpp_debug_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_EGQ_ID,
          SOC_PB_EGQ_ERPP_DEBUG_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->erpp_debug),
          (tbl_data->erpp_debug)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egq_erpp_debug_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_egq_erpp_debug_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_EGQ_ERPP_DEBUG_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_EGQ_ERPP_DEBUG_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_EGQ_ERPP_DEBUG_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EGQ_ERPP_DEBUG_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_EGQ_ERPP_DEBUG_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->egq.erpp_debug_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          (tbl_data->erpp_debug),
          &(tbl->erpp_debug),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_EGQ_ID,
          SOC_PB_EGQ_ERPP_DEBUG_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_egq_erpp_debug_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table pinfo_pmf_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_pinfo_lbp_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_PINFO_LBP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_PINFO_LBP_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_PINFO_LBP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_PINFO_LBP_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_PINFO_LBP_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_PINFO_LBP_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.pinfo_lbp_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_PINFO_LBP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->counter_compension),
          &(tbl_data->counter_compension)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_pinfo_lbp_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_ihb_pinfo_lbp_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_PINFO_LBP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_PINFO_LBP_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_PINFO_LBP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_PINFO_LBP_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_PINFO_LBP_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.pinfo_lbp_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->counter_compension),
          &(tbl->counter_compension),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_PINFO_LBP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_pinfo_lbp_tbl_set_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_ihb_pinfo_pmf_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_PINFO_PMF_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_PINFO_PMF_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_PINFO_PMF_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_PINFO_PMF_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_PINFO_PMF_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_PINFO_PMF_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.pinfo_pmf_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_PINFO_PMF_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->port_pmf_profile),
          &(tbl_data->port_pmf_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_pinfo_pmf_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table pinfo_pmf_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_pinfo_pmf_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_PINFO_PMF_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_PINFO_PMF_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_PINFO_PMF_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_PINFO_PMF_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_PINFO_PMF_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.pinfo_pmf_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->port_pmf_profile),
          &(tbl->port_pmf_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_PINFO_PMF_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_pinfo_pmf_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table packet_format_code_profile_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_packet_format_code_profile_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_PACKET_FORMAT_CODE_PROFILE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_PACKET_FORMAT_CODE_PROFILE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_PACKET_FORMAT_CODE_PROFILE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_PACKET_FORMAT_CODE_PROFILE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_PACKET_FORMAT_CODE_PROFILE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_PACKET_FORMAT_CODE_PROFILE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.packet_format_code_profile_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_PACKET_FORMAT_CODE_PROFILE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->packet_format_code_profile),
          &(tbl_data->packet_format_code_profile)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_packet_format_code_profile_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table packet_format_code_profile_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_packet_format_code_profile_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_PACKET_FORMAT_CODE_PROFILE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_PACKET_FORMAT_CODE_PROFILE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_PACKET_FORMAT_CODE_PROFILE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_PACKET_FORMAT_CODE_PROFILE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_PACKET_FORMAT_CODE_PROFILE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.packet_format_code_profile_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->packet_format_code_profile),
          &(tbl->packet_format_code_profile),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_PACKET_FORMAT_CODE_PROFILE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_packet_format_code_profile_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table src_dest_port_for_l3_acl_key_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_src_dest_port_for_l3_acl_key_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.src_dest_port_for_l3_acl_key_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->source_port_min),
          &(tbl_data->source_port_min)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->source_port_max),
          &(tbl_data->source_port_max)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->destination_port_min),
          &(tbl_data->destination_port_min)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->destination_port_max),
          &(tbl_data->destination_port_max)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_src_dest_port_for_l3_acl_key_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table src_dest_port_for_l3_acl_key_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_src_dest_port_for_l3_acl_key_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.src_dest_port_for_l3_acl_key_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->source_port_min),
          &(tbl->source_port_min),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->source_port_max),
          &(tbl->source_port_max),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->destination_port_min),
          &(tbl->destination_port_min),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->destination_port_max),
          &(tbl->destination_port_max),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_src_dest_port_for_l3_acl_key_tbl_set_unsafe()", entry_offset, 0);
}

/*
 *	Generic access to the FEM table
 */
uint32
  soc_pb_ihb_fem_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              fem_ndx,
    SOC_SAND_IN  uint32              cycle_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_GET_UNSAFE);

  SOC_PB_FEM_TBL_ACCESS(key_profile_resolved_data_tbl_get_unsafe);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem_key_profile_resolved_data_tbl_get_unsafe()", 0, 0);
}

uint32
  soc_pb_ihb_fem_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              fem_ndx,
    SOC_SAND_IN  uint32              cycle_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_SET_UNSAFE);

  SOC_PB_FEM_TBL_ACCESS(key_profile_resolved_data_tbl_set_unsafe);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem_key_profile_resolved_data_tbl_set_unsafe()", 0, 0);
}

uint32
  soc_pb_ihb_fem_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              fem_ndx,
    SOC_SAND_IN  uint32              cycle_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_GET_UNSAFE);

  SOC_PB_FEM_TBL_ACCESS(program_resolved_data_tbl_get_unsafe);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem_program_resolved_data_tbl_get_unsafe()", 0, 0);
}

uint32
  soc_pb_ihb_fem_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              fem_ndx,
    SOC_SAND_IN  uint32              cycle_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_SET_UNSAFE);

  SOC_PB_FEM_TBL_ACCESS(program_resolved_data_tbl_set_unsafe);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem_program_resolved_data_tbl_set_unsafe()", 0, 0);
}

uint32
  soc_pb_ihb_fem_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              fem_ndx,
    SOC_SAND_IN  uint32              cycle_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_GET_UNSAFE);

  SOC_PB_FEM_TBL_ACCESS(map_index_table_tbl_get_unsafe);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem_map_index_table_tbl_get_unsafe()", 0, 0);
}

uint32
  soc_pb_ihb_fem_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              fem_ndx,
    SOC_SAND_IN  uint32              cycle_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_SET_UNSAFE);

  SOC_PB_FEM_TBL_ACCESS(map_index_table_tbl_set_unsafe);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem_map_index_table_tbl_set_unsafe()", 0, 0);
}

uint32
  soc_pb_ihb_fem_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              fem_ndx,
    SOC_SAND_IN  uint32              cycle_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM_MAP_TABLE_TBL_GET_UNSAFE);

  SOC_PB_FEM_TBL_ACCESS(map_table_tbl_get_unsafe);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem_map_table_tbl_get_unsafe()", 0, 0);
}

uint32
  soc_pb_ihb_fem_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              fem_ndx,
    SOC_SAND_IN  uint32              cycle_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM_MAP_TABLE_TBL_SET_UNSAFE);

  SOC_PB_FEM_TBL_ACCESS(map_table_tbl_set_unsafe);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem_map_table_tbl_set_unsafe()", 0, 0);
}

uint32
  soc_pb_ihb_fem_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              fem_ndx,
    SOC_SAND_IN  uint32              cycle_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_GET_UNSAFE);

  SOC_PB_FEM_TBL_ACCESS(offset_table_tbl_get_unsafe);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem_offset_table_tbl_get_unsafe()", 0, 0);
}

uint32
  soc_pb_ihb_fem_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              fem_ndx,
    SOC_SAND_IN  uint32              cycle_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_SET_UNSAFE);

  SOC_PB_FEM_TBL_ACCESS(offset_table_tbl_set_unsafe);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem_offset_table_tbl_set_unsafe()", 0, 0);
}


/*
 *	Symbolic function for non existing tables. Return error
 */
uint32
  soc_pb_ihb_fem0_4b_1st_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM0_4B_1ST_PASS_OFFSET_TABLE_TBL_GET_UNSAFE);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_OFFSET_FEM_INVALID_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem0_4b_1st_pass_offset_table_tbl_get_unsafe()", 0, 0);
}
uint32
  soc_pb_ihb_fem1_4b_1st_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM1_4B_1ST_PASS_OFFSET_TABLE_TBL_GET_UNSAFE);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_OFFSET_FEM_INVALID_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem1_4b_1st_pass_offset_table_tbl_get_unsafe()", 0, 0);
}
uint32
  soc_pb_ihb_fem0_4b_2nd_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM0_4B_2ND_PASS_OFFSET_TABLE_TBL_GET_UNSAFE);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_OFFSET_FEM_INVALID_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem0_4b_2nd_pass_offset_table_tbl_get_unsafe()", 0, 0);
}
uint32
  soc_pb_ihb_fem1_4b_2nd_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM1_4B_2ND_PASS_OFFSET_TABLE_TBL_GET_UNSAFE);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_OFFSET_FEM_INVALID_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem1_4b_2nd_pass_offset_table_tbl_get_unsafe()", 0, 0);
}
uint32
  soc_pb_ihb_fem0_4b_1st_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM0_4B_1ST_PASS_OFFSET_TABLE_TBL_SET_UNSAFE);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_OFFSET_FEM_INVALID_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem0_4b_1st_pass_offset_table_tbl_set_unsafe()", 0, 0);
}
uint32
  soc_pb_ihb_fem1_4b_1st_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM1_4B_1ST_PASS_OFFSET_TABLE_TBL_SET_UNSAFE);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_OFFSET_FEM_INVALID_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem1_4b_1st_pass_offset_table_tbl_set_unsafe()", 0, 0);
}

uint32
  soc_pb_ihb_fem0_4b_2nd_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM0_4B_2ND_PASS_OFFSET_TABLE_TBL_SET_UNSAFE);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_OFFSET_FEM_INVALID_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem0_4b_2nd_pass_offset_table_tbl_set_unsafe()", 0, 0);
}

uint32
  soc_pb_ihb_fem1_4b_2nd_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM1_4B_2ND_PASS_OFFSET_TABLE_TBL_SET_UNSAFE);

  SOC_SAND_SET_ERROR_CODE(SOC_PB_PMF_OFFSET_FEM_INVALID_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem1_4b_2nd_pass_offset_table_tbl_set_unsafe()", 0, 0);
}


/*
 * Read indirect table direct_1st_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_direct_1st_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_DIRECT_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_DIRECT_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_DIRECT_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_DIRECT_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_DIRECT_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_DIRECT_KEY_PROFILE_RESOLVED_DATA_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.direct_1st_pass_key_profile_resolved_data_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_DIRECT_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->direct_key_select),
          &(tbl_data->direct_key_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->direct_db_and_value),
          &(tbl_data->direct_db_and_value)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->direct_db_or_value),
          &(tbl_data->direct_db_or_value)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_direct_1st_pass_key_profile_resolved_data_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table direct_1st_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_direct_1st_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_DIRECT_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_DIRECT_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_DIRECT_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_DIRECT_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_DIRECT_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.direct_1st_pass_key_profile_resolved_data_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->direct_key_select),
          &(tbl->direct_key_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->direct_db_and_value),
          &(tbl->direct_db_and_value),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->direct_db_or_value),
          &(tbl->direct_db_or_value),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_DIRECT_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_direct_1st_pass_key_profile_resolved_data_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table direct_2nd_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_direct_2nd_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_DIRECT_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_DIRECT_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_DIRECT_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_DIRECT_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_DIRECT_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_DIRECT_KEY_PROFILE_RESOLVED_DATA_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.direct_2nd_pass_key_profile_resolved_data_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_DIRECT_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->direct_key_select),
          &(tbl_data->direct_key_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->direct_db_and_value),
          &(tbl_data->direct_db_and_value)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->direct_db_or_value),
          &(tbl_data->direct_db_or_value)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_direct_2nd_pass_key_profile_resolved_data_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table direct_2nd_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_direct_2nd_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_DIRECT_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_DIRECT_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_DIRECT_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_DIRECT_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_DIRECT_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.direct_2nd_pass_key_profile_resolved_data_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->direct_key_select),
          &(tbl->direct_key_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->direct_db_and_value),
          &(tbl->direct_db_and_value),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->direct_db_or_value),
          &(tbl->direct_db_or_value),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_DIRECT_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_direct_2nd_pass_key_profile_resolved_data_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem0_4b_1st_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem0_4b_1st_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM0_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM0_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM0_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM0_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem0_4b_1st_pass_key_profile_resolved_data_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM0_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->program),
          &(tbl_data->program)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->key_select),
          &(tbl_data->key_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem0_4b_1st_pass_key_profile_resolved_data_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem0_4b_1st_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem0_4b_1st_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM0_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM0_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM0_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM0_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem0_4b_1st_pass_key_profile_resolved_data_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->program),
          &(tbl->program),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->key_select),
          &(tbl->key_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM0_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem0_4b_1st_pass_key_profile_resolved_data_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem0_4b_1st_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem0_4b_1st_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM0_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM0_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM0_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM0_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem0_4b_1st_pass_program_resolved_data_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM0_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->bit_select),
          &(tbl_data->bit_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem0_4b_1st_pass_program_resolved_data_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem0_4b_1st_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem0_4b_1st_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM0_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM0_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM0_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM0_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem0_4b_1st_pass_program_resolved_data_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->bit_select),
          &(tbl->bit_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM0_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem0_4b_1st_pass_program_resolved_data_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem0_4b_1st_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem0_4b_1st_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM0_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM0_4B_1ST_PASS_MAP_INDEX_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM0_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM0_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem0_4b_1st_pass_map_index_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM0_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->map_index),
          &(tbl_data->map_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->map_data),
          &(tbl_data->map_data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem0_4b_1st_pass_map_index_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem0_4b_1st_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem0_4b_1st_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM0_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM0_4B_1ST_PASS_MAP_INDEX_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM0_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM0_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem0_4b_1st_pass_map_index_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->map_index),
          &(tbl->map_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->map_data),
          &(tbl->map_data),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM0_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem0_4b_1st_pass_map_index_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem0_4b_1st_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem0_4b_1st_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem0_4b_1st_pass_map_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->action_type),
          &(tbl_data->action_type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map0),
          &(tbl_data->field_select_map[0])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map1),
          &(tbl_data->field_select_map[1])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map2),
          &(tbl_data->field_select_map[2])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map3),
          &(tbl_data->field_select_map[3])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem0_4b_1st_pass_map_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem0_4b_1st_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem0_4b_1st_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem0_4b_1st_pass_map_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->action_type),
          &(tbl->action_type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[0]),
          &(tbl->field_select_map0),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[1]),
          &(tbl->field_select_map1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[2]),
          &(tbl->field_select_map2),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[3]),
          &(tbl->field_select_map3),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem0_4b_1st_pass_map_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem1_4b_1st_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem1_4b_1st_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM1_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM1_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM1_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM1_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem1_4b_1st_pass_key_profile_resolved_data_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM1_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->program),
          &(tbl_data->program)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->key_select),
          &(tbl_data->key_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem1_4b_1st_pass_key_profile_resolved_data_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem1_4b_1st_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem1_4b_1st_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM1_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM1_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM1_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM1_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem1_4b_1st_pass_key_profile_resolved_data_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->program),
          &(tbl->program),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->key_select),
          &(tbl->key_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM1_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem1_4b_1st_pass_key_profile_resolved_data_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem1_4b_1st_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem1_4b_1st_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM1_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM1_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM1_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM1_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem1_4b_1st_pass_program_resolved_data_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM1_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->bit_select),
          &(tbl_data->bit_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem1_4b_1st_pass_program_resolved_data_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem1_4b_1st_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem1_4b_1st_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM1_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM1_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM1_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM1_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem1_4b_1st_pass_program_resolved_data_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->bit_select),
          &(tbl->bit_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM1_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem1_4b_1st_pass_program_resolved_data_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem1_4b_1st_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem1_4b_1st_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM1_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM1_4B_1ST_PASS_MAP_INDEX_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM1_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM1_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem1_4b_1st_pass_map_index_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM1_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->map_index),
          &(tbl_data->map_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->map_data),
          &(tbl_data->map_data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem1_4b_1st_pass_map_index_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem1_4b_1st_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem1_4b_1st_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM1_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM1_4B_1ST_PASS_MAP_INDEX_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM1_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM1_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem1_4b_1st_pass_map_index_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->map_index),
          &(tbl->map_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->map_data),
          &(tbl->map_data),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM1_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem1_4b_1st_pass_map_index_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem1_4b_1st_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem1_4b_1st_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM1_4B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM1_4B_1ST_PASS_MAP_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM1_4B_1ST_PASS_MAP_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM1_4B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem1_4b_1st_pass_map_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM1_4B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->action_type),
          &(tbl_data->action_type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map0),
          &(tbl_data->field_select_map[0])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map1),
          &(tbl_data->field_select_map[1])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map2),
          &(tbl_data->field_select_map[2])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map3),
          &(tbl_data->field_select_map[3])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem1_4b_1st_pass_map_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem1_4b_1st_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem1_4b_1st_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM1_4B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM1_4B_1ST_PASS_MAP_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM1_4B_1ST_PASS_MAP_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM1_4B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem1_4b_1st_pass_map_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->action_type),
          &(tbl->action_type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[0]),
          &(tbl->field_select_map0),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[1]),
          &(tbl->field_select_map1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[2]),
          &(tbl->field_select_map2),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[3]),
          &(tbl->field_select_map3),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM1_4B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem1_4b_1st_pass_map_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem2_14b_1st_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem2_14b_1st_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM2_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM2_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM2_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM2_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem2_14b_1st_pass_key_profile_resolved_data_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM2_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->program),
          &(tbl_data->program)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->key_select),
          &(tbl_data->key_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem2_14b_1st_pass_key_profile_resolved_data_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem2_14b_1st_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem2_14b_1st_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM2_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM2_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM2_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM2_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem2_14b_1st_pass_key_profile_resolved_data_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->program),
          &(tbl->program),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->key_select),
          &(tbl->key_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM2_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem2_14b_1st_pass_key_profile_resolved_data_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem2_14b_1st_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem2_14b_1st_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM2_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM2_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM2_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM2_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem2_14b_1st_pass_program_resolved_data_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM2_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->bit_select),
          &(tbl_data->bit_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem2_14b_1st_pass_program_resolved_data_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem2_14b_1st_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem2_14b_1st_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM2_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM2_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM2_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM2_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem2_14b_1st_pass_program_resolved_data_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->bit_select),
          &(tbl->bit_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM2_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem2_14b_1st_pass_program_resolved_data_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem2_14b_1st_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem2_14b_1st_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM2_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM2_14B_1ST_PASS_MAP_INDEX_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM2_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM2_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem2_14b_1st_pass_map_index_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM2_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->map_index),
          &(tbl_data->map_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->map_data),
          &(tbl_data->map_data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem2_14b_1st_pass_map_index_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem2_14b_1st_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem2_14b_1st_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM2_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM2_14B_1ST_PASS_MAP_INDEX_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM2_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM2_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem2_14b_1st_pass_map_index_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->map_index),
          &(tbl->map_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->map_data),
          &(tbl->map_data),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM2_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem2_14b_1st_pass_map_index_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem2_14b_1st_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem2_14b_1st_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM2_14B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM2_14B_1ST_PASS_MAP_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM2_14B_1ST_PASS_MAP_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM2_14B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem2_14b_1st_pass_map_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM2_14B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->action_type),
          &(tbl_data->action_type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map0),
          &(tbl_data->field_select_map[0])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map1),
          &(tbl_data->field_select_map[1])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map2),
          &(tbl_data->field_select_map[2])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map3),
          &(tbl_data->field_select_map[3])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map4),
          &(tbl_data->field_select_map[4])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map5),
          &(tbl_data->field_select_map[5])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map6),
          &(tbl_data->field_select_map[6])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map7),
          &(tbl_data->field_select_map[7])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map8),
          &(tbl_data->field_select_map[8])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map9),
          &(tbl_data->field_select_map[9])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map10),
          &(tbl_data->field_select_map[10])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 51, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map11),
          &(tbl_data->field_select_map[11])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map12),
          &(tbl_data->field_select_map[12])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 53, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map13),
          &(tbl_data->field_select_map[13])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 54, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->offset_index),
          &(tbl_data->offset_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem2_14b_1st_pass_map_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem2_14b_1st_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem2_14b_1st_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM2_14B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM2_14B_1ST_PASS_MAP_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM2_14B_1ST_PASS_MAP_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM2_14B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem2_14b_1st_pass_map_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->action_type),
          &(tbl->action_type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[0]),
          &(tbl->field_select_map0),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[1]),
          &(tbl->field_select_map1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[2]),
          &(tbl->field_select_map2),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[3]),
          &(tbl->field_select_map3),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[4]),
          &(tbl->field_select_map4),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[5]),
          &(tbl->field_select_map5),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[6]),
          &(tbl->field_select_map6),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[7]),
          &(tbl->field_select_map7),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[8]),
          &(tbl->field_select_map8),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 39, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[9]),
          &(tbl->field_select_map9),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[10]),
          &(tbl->field_select_map10),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[11]),
          &(tbl->field_select_map11),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[12]),
          &(tbl->field_select_map12),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[13]),
          &(tbl->field_select_map13),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->offset_index),
          &(tbl->offset_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM2_14B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem2_14b_1st_pass_map_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem2_14b_1st_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem2_14b_1st_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM2_14B_1ST_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM2_14B_1ST_PASS_OFFSET_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM2_14B_1ST_PASS_OFFSET_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM2_14B_1ST_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem2_14b_1st_pass_offset_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM2_14B_1ST_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->offset),
          &(tbl_data->offset)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem2_14b_1st_pass_offset_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem2_14b_1st_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem2_14b_1st_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM2_14B_1ST_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM2_14B_1ST_PASS_OFFSET_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM2_14B_1ST_PASS_OFFSET_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM2_14B_1ST_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem2_14b_1st_pass_offset_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->offset),
          &(tbl->offset),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM2_14B_1ST_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem2_14b_1st_pass_offset_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem3_14b_1st_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem3_14b_1st_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM3_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM3_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM3_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM3_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem3_14b_1st_pass_key_profile_resolved_data_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM3_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->program),
          &(tbl_data->program)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->key_select),
          &(tbl_data->key_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem3_14b_1st_pass_key_profile_resolved_data_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem3_14b_1st_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem3_14b_1st_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM3_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM3_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM3_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM3_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem3_14b_1st_pass_key_profile_resolved_data_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->program),
          &(tbl->program),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->key_select),
          &(tbl->key_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM3_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem3_14b_1st_pass_key_profile_resolved_data_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem3_14b_1st_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem3_14b_1st_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM3_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM3_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM3_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM3_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem3_14b_1st_pass_program_resolved_data_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM3_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->bit_select),
          &(tbl_data->bit_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem3_14b_1st_pass_program_resolved_data_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem3_14b_1st_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem3_14b_1st_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM3_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM3_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM3_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM3_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem3_14b_1st_pass_program_resolved_data_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->bit_select),
          &(tbl->bit_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM3_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem3_14b_1st_pass_program_resolved_data_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem3_14b_1st_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem3_14b_1st_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM3_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM3_14B_1ST_PASS_MAP_INDEX_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM3_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM3_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem3_14b_1st_pass_map_index_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM3_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->map_index),
          &(tbl_data->map_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->map_data),
          &(tbl_data->map_data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem3_14b_1st_pass_map_index_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem3_14b_1st_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem3_14b_1st_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM3_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM3_14B_1ST_PASS_MAP_INDEX_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM3_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM3_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem3_14b_1st_pass_map_index_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->map_index),
          &(tbl->map_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->map_data),
          &(tbl->map_data),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM3_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem3_14b_1st_pass_map_index_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem3_14b_1st_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem3_14b_1st_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM3_14B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM3_14B_1ST_PASS_MAP_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM3_14B_1ST_PASS_MAP_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM3_14B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem3_14b_1st_pass_map_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM3_14B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->action_type),
          &(tbl_data->action_type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map0),
          &(tbl_data->field_select_map[0])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map1),
          &(tbl_data->field_select_map[1])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map2),
          &(tbl_data->field_select_map[2])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map3),
          &(tbl_data->field_select_map[3])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map4),
          &(tbl_data->field_select_map[4])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map5),
          &(tbl_data->field_select_map[5])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map6),
          &(tbl_data->field_select_map[6])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map7),
          &(tbl_data->field_select_map[7])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map8),
          &(tbl_data->field_select_map[8])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map9),
          &(tbl_data->field_select_map[9])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map10),
          &(tbl_data->field_select_map[10])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 51, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map11),
          &(tbl_data->field_select_map[11])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map12),
          &(tbl_data->field_select_map[12])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 53, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map13),
          &(tbl_data->field_select_map[13])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 54, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->offset_index),
          &(tbl_data->offset_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem3_14b_1st_pass_map_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem3_14b_1st_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem3_14b_1st_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM3_14B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM3_14B_1ST_PASS_MAP_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM3_14B_1ST_PASS_MAP_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM3_14B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem3_14b_1st_pass_map_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->action_type),
          &(tbl->action_type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[0]),
          &(tbl->field_select_map0),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[1]),
          &(tbl->field_select_map1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[2]),
          &(tbl->field_select_map2),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[3]),
          &(tbl->field_select_map3),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[4]),
          &(tbl->field_select_map4),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[5]),
          &(tbl->field_select_map5),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[6]),
          &(tbl->field_select_map6),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[7]),
          &(tbl->field_select_map7),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[8]),
          &(tbl->field_select_map8),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 39, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[9]),
          &(tbl->field_select_map9),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[10]),
          &(tbl->field_select_map10),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[11]),
          &(tbl->field_select_map11),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[12]),
          &(tbl->field_select_map12),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[13]),
          &(tbl->field_select_map13),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->offset_index),
          &(tbl->offset_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM3_14B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem3_14b_1st_pass_map_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem3_14b_1st_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem3_14b_1st_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM3_14B_1ST_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM3_14B_1ST_PASS_OFFSET_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM3_14B_1ST_PASS_OFFSET_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM3_14B_1ST_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem3_14b_1st_pass_offset_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM3_14B_1ST_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->offset),
          &(tbl_data->offset)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem3_14b_1st_pass_offset_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem3_14b_1st_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem3_14b_1st_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM3_14B_1ST_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM3_14B_1ST_PASS_OFFSET_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM3_14B_1ST_PASS_OFFSET_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM3_14B_1ST_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem3_14b_1st_pass_offset_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->offset),
          &(tbl->offset),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM3_14B_1ST_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem3_14b_1st_pass_offset_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem4_14b_1st_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem4_14b_1st_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM4_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM4_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM4_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM4_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem4_14b_1st_pass_key_profile_resolved_data_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM4_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->program),
          &(tbl_data->program)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->key_select),
          &(tbl_data->key_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem4_14b_1st_pass_key_profile_resolved_data_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem4_14b_1st_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem4_14b_1st_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM4_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM4_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM4_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM4_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem4_14b_1st_pass_key_profile_resolved_data_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->program),
          &(tbl->program),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->key_select),
          &(tbl->key_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM4_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem4_14b_1st_pass_key_profile_resolved_data_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem4_14b_1st_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem4_14b_1st_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM4_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM4_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM4_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM4_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem4_14b_1st_pass_program_resolved_data_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM4_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->bit_select),
          &(tbl_data->bit_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem4_14b_1st_pass_program_resolved_data_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem4_14b_1st_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem4_14b_1st_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM4_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM4_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM4_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM4_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem4_14b_1st_pass_program_resolved_data_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->bit_select),
          &(tbl->bit_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM4_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem4_14b_1st_pass_program_resolved_data_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem4_14b_1st_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem4_14b_1st_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM4_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM4_14B_1ST_PASS_MAP_INDEX_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM4_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM4_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem4_14b_1st_pass_map_index_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM4_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->map_index),
          &(tbl_data->map_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->map_data),
          &(tbl_data->map_data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem4_14b_1st_pass_map_index_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem4_14b_1st_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem4_14b_1st_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM4_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM4_14B_1ST_PASS_MAP_INDEX_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM4_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM4_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem4_14b_1st_pass_map_index_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->map_index),
          &(tbl->map_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->map_data),
          &(tbl->map_data),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM4_14B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem4_14b_1st_pass_map_index_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem4_14b_1st_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem4_14b_1st_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM4_14B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM4_14B_1ST_PASS_MAP_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM4_14B_1ST_PASS_MAP_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM4_14B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem4_14b_1st_pass_map_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM4_14B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->action_type),
          &(tbl_data->action_type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map0),
          &(tbl_data->field_select_map[0])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map1),
          &(tbl_data->field_select_map[1])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map2),
          &(tbl_data->field_select_map[2])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map3),
          &(tbl_data->field_select_map[3])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map4),
          &(tbl_data->field_select_map[4])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map5),
          &(tbl_data->field_select_map[5])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map6),
          &(tbl_data->field_select_map[6])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map7),
          &(tbl_data->field_select_map[7])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map8),
          &(tbl_data->field_select_map[8])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map9),
          &(tbl_data->field_select_map[9])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map10),
          &(tbl_data->field_select_map[10])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 51, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map11),
          &(tbl_data->field_select_map[11])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map12),
          &(tbl_data->field_select_map[12])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 53, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map13),
          &(tbl_data->field_select_map[13])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 54, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->offset_index),
          &(tbl_data->offset_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem4_14b_1st_pass_map_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem4_14b_1st_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem4_14b_1st_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM4_14B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM4_14B_1ST_PASS_MAP_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM4_14B_1ST_PASS_MAP_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM4_14B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem4_14b_1st_pass_map_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->action_type),
          &(tbl->action_type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[0]),
          &(tbl->field_select_map0),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[1]),
          &(tbl->field_select_map1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[2]),
          &(tbl->field_select_map2),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[3]),
          &(tbl->field_select_map3),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[4]),
          &(tbl->field_select_map4),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[5]),
          &(tbl->field_select_map5),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[6]),
          &(tbl->field_select_map6),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[7]),
          &(tbl->field_select_map7),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[8]),
          &(tbl->field_select_map8),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 39, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[9]),
          &(tbl->field_select_map9),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[10]),
          &(tbl->field_select_map10),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[11]),
          &(tbl->field_select_map11),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[12]),
          &(tbl->field_select_map12),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[13]),
          &(tbl->field_select_map13),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->offset_index),
          &(tbl->offset_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM4_14B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem4_14b_1st_pass_map_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem4_14b_1st_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem4_14b_1st_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM4_14B_1ST_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM4_14B_1ST_PASS_OFFSET_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM4_14B_1ST_PASS_OFFSET_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM4_14B_1ST_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem4_14b_1st_pass_offset_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM4_14B_1ST_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->offset),
          &(tbl_data->offset)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem4_14b_1st_pass_offset_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem4_14b_1st_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem4_14b_1st_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM4_14B_1ST_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM4_14B_1ST_PASS_OFFSET_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM4_14B_1ST_PASS_OFFSET_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM4_14B_1ST_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem4_14b_1st_pass_offset_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->offset),
          &(tbl->offset),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM4_14B_1ST_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem4_14b_1st_pass_offset_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem5_17b_1st_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem5_17b_1st_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM5_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM5_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM5_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM5_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem5_17b_1st_pass_key_profile_resolved_data_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM5_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->program),
          &(tbl_data->program)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->key_select),
          &(tbl_data->key_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem5_17b_1st_pass_key_profile_resolved_data_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem5_17b_1st_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem5_17b_1st_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM5_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM5_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM5_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM5_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem5_17b_1st_pass_key_profile_resolved_data_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->program),
          &(tbl->program),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->key_select),
          &(tbl->key_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM5_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem5_17b_1st_pass_key_profile_resolved_data_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem5_17b_1st_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem5_17b_1st_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM5_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM5_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM5_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM5_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem5_17b_1st_pass_program_resolved_data_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM5_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->bit_select),
          &(tbl_data->bit_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem5_17b_1st_pass_program_resolved_data_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem5_17b_1st_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem5_17b_1st_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM5_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM5_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM5_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM5_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem5_17b_1st_pass_program_resolved_data_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->bit_select),
          &(tbl->bit_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM5_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem5_17b_1st_pass_program_resolved_data_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem5_17b_1st_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem5_17b_1st_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM5_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM5_17B_1ST_PASS_MAP_INDEX_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM5_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM5_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem5_17b_1st_pass_map_index_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM5_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->map_index),
          &(tbl_data->map_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->map_data),
          &(tbl_data->map_data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem5_17b_1st_pass_map_index_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem5_17b_1st_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem5_17b_1st_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM5_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM5_17B_1ST_PASS_MAP_INDEX_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM5_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM5_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem5_17b_1st_pass_map_index_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->map_index),
          &(tbl->map_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->map_data),
          &(tbl->map_data),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM5_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem5_17b_1st_pass_map_index_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem5_17b_1st_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem5_17b_1st_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM5_17B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM5_17B_1ST_PASS_MAP_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM5_17B_1ST_PASS_MAP_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM5_17B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem5_17b_1st_pass_map_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM5_17B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->action_type),
          &(tbl_data->action_type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map0),
          &(tbl_data->field_select_map[0])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map1),
          &(tbl_data->field_select_map[1])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map2),
          &(tbl_data->field_select_map[2])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map3),
          &(tbl_data->field_select_map[3])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map4),
          &(tbl_data->field_select_map[4])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map5),
          &(tbl_data->field_select_map[5])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map6),
          &(tbl_data->field_select_map[6])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map7),
          &(tbl_data->field_select_map[7])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map8),
          &(tbl_data->field_select_map[8])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map9),
          &(tbl_data->field_select_map[9])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map10),
          &(tbl_data->field_select_map[10])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 51, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map11),
          &(tbl_data->field_select_map[11])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map12),
          &(tbl_data->field_select_map[12])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 53, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map13),
          &(tbl_data->field_select_map[13])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 54, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map14),
          &(tbl_data->field_select_map[14])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map15),
          &(tbl_data->field_select_map[15])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 56, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map16),
          &(tbl_data->field_select_map[16])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 57, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->offset_index),
          &(tbl_data->offset_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 58, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem5_17b_1st_pass_map_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem5_17b_1st_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem5_17b_1st_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM5_17B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM5_17B_1ST_PASS_MAP_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM5_17B_1ST_PASS_MAP_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM5_17B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem5_17b_1st_pass_map_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->action_type),
          &(tbl->action_type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[0]),
          &(tbl->field_select_map0),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[1]),
          &(tbl->field_select_map1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[2]),
          &(tbl->field_select_map2),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[3]),
          &(tbl->field_select_map3),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[4]),
          &(tbl->field_select_map4),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[5]),
          &(tbl->field_select_map5),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[6]),
          &(tbl->field_select_map6),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[7]),
          &(tbl->field_select_map7),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[8]),
          &(tbl->field_select_map8),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 39, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[9]),
          &(tbl->field_select_map9),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[10]),
          &(tbl->field_select_map10),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[11]),
          &(tbl->field_select_map11),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[12]),
          &(tbl->field_select_map12),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[13]),
          &(tbl->field_select_map13),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[14]),
          &(tbl->field_select_map14),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[15]),
          &(tbl->field_select_map15),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[16]),
          &(tbl->field_select_map16),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->offset_index),
          &(tbl->offset_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM5_17B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem5_17b_1st_pass_map_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem5_17b_1st_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem5_17b_1st_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM5_17B_1ST_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM5_17B_1ST_PASS_OFFSET_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM5_17B_1ST_PASS_OFFSET_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM5_17B_1ST_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem5_17b_1st_pass_offset_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM5_17B_1ST_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->offset),
          &(tbl_data->offset)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem5_17b_1st_pass_offset_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem5_17b_1st_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem5_17b_1st_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM5_17B_1ST_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM5_17B_1ST_PASS_OFFSET_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM5_17B_1ST_PASS_OFFSET_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM5_17B_1ST_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem5_17b_1st_pass_offset_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->offset),
          &(tbl->offset),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM5_17B_1ST_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem5_17b_1st_pass_offset_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem6_17b_1st_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem6_17b_1st_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM6_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM6_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM6_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM6_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem6_17b_1st_pass_key_profile_resolved_data_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM6_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->program),
          &(tbl_data->program)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->key_select),
          &(tbl_data->key_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem6_17b_1st_pass_key_profile_resolved_data_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem6_17b_1st_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem6_17b_1st_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM6_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM6_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM6_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM6_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem6_17b_1st_pass_key_profile_resolved_data_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->program),
          &(tbl->program),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->key_select),
          &(tbl->key_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM6_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem6_17b_1st_pass_key_profile_resolved_data_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem6_17b_1st_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem6_17b_1st_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM6_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM6_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM6_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM6_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem6_17b_1st_pass_program_resolved_data_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM6_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->bit_select),
          &(tbl_data->bit_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem6_17b_1st_pass_program_resolved_data_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem6_17b_1st_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem6_17b_1st_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM6_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM6_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM6_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM6_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem6_17b_1st_pass_program_resolved_data_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->bit_select),
          &(tbl->bit_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM6_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem6_17b_1st_pass_program_resolved_data_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem6_17b_1st_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem6_17b_1st_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM6_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM6_17B_1ST_PASS_MAP_INDEX_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM6_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM6_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem6_17b_1st_pass_map_index_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM6_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->map_index),
          &(tbl_data->map_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->map_data),
          &(tbl_data->map_data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem6_17b_1st_pass_map_index_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem6_17b_1st_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem6_17b_1st_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM6_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM6_17B_1ST_PASS_MAP_INDEX_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM6_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM6_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem6_17b_1st_pass_map_index_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->map_index),
          &(tbl->map_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->map_data),
          &(tbl->map_data),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM6_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem6_17b_1st_pass_map_index_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem6_17b_1st_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem6_17b_1st_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM6_17B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM6_17B_1ST_PASS_MAP_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM6_17B_1ST_PASS_MAP_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM6_17B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem6_17b_1st_pass_map_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM6_17B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->action_type),
          &(tbl_data->action_type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map0),
          &(tbl_data->field_select_map[0])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map1),
          &(tbl_data->field_select_map[1])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map2),
          &(tbl_data->field_select_map[2])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map3),
          &(tbl_data->field_select_map[3])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map4),
          &(tbl_data->field_select_map[4])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map5),
          &(tbl_data->field_select_map[5])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map6),
          &(tbl_data->field_select_map[6])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map7),
          &(tbl_data->field_select_map[7])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map8),
          &(tbl_data->field_select_map[8])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map9),
          &(tbl_data->field_select_map[9])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map10),
          &(tbl_data->field_select_map[10])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 51, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map11),
          &(tbl_data->field_select_map[11])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map12),
          &(tbl_data->field_select_map[12])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 53, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map13),
          &(tbl_data->field_select_map[13])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 54, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map14),
          &(tbl_data->field_select_map[14])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map15),
          &(tbl_data->field_select_map[15])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 56, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map16),
          &(tbl_data->field_select_map[16])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 57, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->offset_index),
          &(tbl_data->offset_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 58, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem6_17b_1st_pass_map_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem6_17b_1st_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem6_17b_1st_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM6_17B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM6_17B_1ST_PASS_MAP_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM6_17B_1ST_PASS_MAP_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM6_17B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem6_17b_1st_pass_map_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->action_type),
          &(tbl->action_type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[0]),
          &(tbl->field_select_map0),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[1]),
          &(tbl->field_select_map1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[2]),
          &(tbl->field_select_map2),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[3]),
          &(tbl->field_select_map3),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[4]),
          &(tbl->field_select_map4),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[5]),
          &(tbl->field_select_map5),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[6]),
          &(tbl->field_select_map6),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[7]),
          &(tbl->field_select_map7),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[8]),
          &(tbl->field_select_map8),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 39, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[9]),
          &(tbl->field_select_map9),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[10]),
          &(tbl->field_select_map10),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[11]),
          &(tbl->field_select_map11),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[12]),
          &(tbl->field_select_map12),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[13]),
          &(tbl->field_select_map13),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[14]),
          &(tbl->field_select_map14),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[15]),
          &(tbl->field_select_map15),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[16]),
          &(tbl->field_select_map16),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->offset_index),
          &(tbl->offset_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM6_17B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem6_17b_1st_pass_map_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem6_17b_1st_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem6_17b_1st_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM6_17B_1ST_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM6_17B_1ST_PASS_OFFSET_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM6_17B_1ST_PASS_OFFSET_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM6_17B_1ST_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem6_17b_1st_pass_offset_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM6_17B_1ST_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->offset),
          &(tbl_data->offset)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem6_17b_1st_pass_offset_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem6_17b_1st_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem6_17b_1st_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM6_17B_1ST_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM6_17B_1ST_PASS_OFFSET_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM6_17B_1ST_PASS_OFFSET_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM6_17B_1ST_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem6_17b_1st_pass_offset_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->offset),
          &(tbl->offset),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM6_17B_1ST_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem6_17b_1st_pass_offset_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem7_17b_1st_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem7_17b_1st_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM7_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM7_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM7_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM7_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem7_17b_1st_pass_key_profile_resolved_data_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM7_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->program),
          &(tbl_data->program)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->key_select),
          &(tbl_data->key_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem7_17b_1st_pass_key_profile_resolved_data_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem7_17b_1st_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem7_17b_1st_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM7_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM7_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM7_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM7_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem7_17b_1st_pass_key_profile_resolved_data_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->program),
          &(tbl->program),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->key_select),
          &(tbl->key_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM7_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem7_17b_1st_pass_key_profile_resolved_data_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem7_17b_1st_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem7_17b_1st_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM7_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM7_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM7_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM7_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem7_17b_1st_pass_program_resolved_data_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM7_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->bit_select),
          &(tbl_data->bit_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem7_17b_1st_pass_program_resolved_data_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem7_17b_1st_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem7_17b_1st_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM7_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM7_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM7_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM7_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem7_17b_1st_pass_program_resolved_data_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->bit_select),
          &(tbl->bit_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM7_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem7_17b_1st_pass_program_resolved_data_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem7_17b_1st_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem7_17b_1st_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM7_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM7_17B_1ST_PASS_MAP_INDEX_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM7_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM7_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem7_17b_1st_pass_map_index_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM7_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->map_index),
          &(tbl_data->map_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->map_data),
          &(tbl_data->map_data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem7_17b_1st_pass_map_index_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem7_17b_1st_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem7_17b_1st_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM7_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM7_17B_1ST_PASS_MAP_INDEX_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM7_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM7_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem7_17b_1st_pass_map_index_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->map_index),
          &(tbl->map_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->map_data),
          &(tbl->map_data),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM7_17B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem7_17b_1st_pass_map_index_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem7_17b_1st_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem7_17b_1st_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM7_17B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM7_17B_1ST_PASS_MAP_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM7_17B_1ST_PASS_MAP_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM7_17B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem7_17b_1st_pass_map_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM7_17B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->action_type),
          &(tbl_data->action_type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map0),
          &(tbl_data->field_select_map[0])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map1),
          &(tbl_data->field_select_map[1])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map2),
          &(tbl_data->field_select_map[2])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map3),
          &(tbl_data->field_select_map[3])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map4),
          &(tbl_data->field_select_map[4])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map5),
          &(tbl_data->field_select_map[5])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map6),
          &(tbl_data->field_select_map[6])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map7),
          &(tbl_data->field_select_map[7])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map8),
          &(tbl_data->field_select_map[8])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map9),
          &(tbl_data->field_select_map[9])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map10),
          &(tbl_data->field_select_map[10])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 51, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map11),
          &(tbl_data->field_select_map[11])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map12),
          &(tbl_data->field_select_map[12])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 53, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map13),
          &(tbl_data->field_select_map[13])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 54, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map14),
          &(tbl_data->field_select_map[14])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map15),
          &(tbl_data->field_select_map[15])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 56, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map16),
          &(tbl_data->field_select_map[16])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 57, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->offset_index),
          &(tbl_data->offset_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 58, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem7_17b_1st_pass_map_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem7_17b_1st_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem7_17b_1st_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM7_17B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM7_17B_1ST_PASS_MAP_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM7_17B_1ST_PASS_MAP_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM7_17B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem7_17b_1st_pass_map_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->action_type),
          &(tbl->action_type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[0]),
          &(tbl->field_select_map0),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[1]),
          &(tbl->field_select_map1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[2]),
          &(tbl->field_select_map2),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[3]),
          &(tbl->field_select_map3),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[4]),
          &(tbl->field_select_map4),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[5]),
          &(tbl->field_select_map5),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[6]),
          &(tbl->field_select_map6),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[7]),
          &(tbl->field_select_map7),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[8]),
          &(tbl->field_select_map8),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 39, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[9]),
          &(tbl->field_select_map9),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[10]),
          &(tbl->field_select_map10),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[11]),
          &(tbl->field_select_map11),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[12]),
          &(tbl->field_select_map12),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[13]),
          &(tbl->field_select_map13),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[14]),
          &(tbl->field_select_map14),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[15]),
          &(tbl->field_select_map15),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[16]),
          &(tbl->field_select_map16),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->offset_index),
          &(tbl->offset_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM7_17B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem7_17b_1st_pass_map_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem7_17b_1st_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem7_17b_1st_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM7_17B_1ST_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM7_17B_1ST_PASS_OFFSET_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM7_17B_1ST_PASS_OFFSET_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM7_17B_1ST_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem7_17b_1st_pass_offset_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM7_17B_1ST_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->offset),
          &(tbl_data->offset)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem7_17b_1st_pass_offset_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem7_17b_1st_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem7_17b_1st_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM7_17B_1ST_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM7_17B_1ST_PASS_OFFSET_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM7_17B_1ST_PASS_OFFSET_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM7_17B_1ST_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem7_17b_1st_pass_offset_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->offset),
          &(tbl->offset),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM7_17B_1ST_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem7_17b_1st_pass_offset_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem0_4b_2nd_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem0_4b_2nd_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM0_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM0_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM0_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM0_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem0_4b_2nd_pass_key_profile_resolved_data_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM0_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->program),
          &(tbl_data->program)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->key_select),
          &(tbl_data->key_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem0_4b_2nd_pass_key_profile_resolved_data_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem0_4b_2nd_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem0_4b_2nd_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM0_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM0_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM0_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM0_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem0_4b_2nd_pass_key_profile_resolved_data_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->program),
          &(tbl->program),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->key_select),
          &(tbl->key_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM0_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem0_4b_2nd_pass_key_profile_resolved_data_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem0_4b_2nd_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem0_4b_2nd_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM0_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM0_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM0_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM0_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem0_4b_2nd_pass_program_resolved_data_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM0_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->bit_select),
          &(tbl_data->bit_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem0_4b_2nd_pass_program_resolved_data_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem0_4b_2nd_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem0_4b_2nd_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM0_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM0_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM0_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM0_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem0_4b_2nd_pass_program_resolved_data_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->bit_select),
          &(tbl->bit_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM0_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem0_4b_2nd_pass_program_resolved_data_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem0_4b_2nd_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem0_4b_2nd_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM0_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM0_4B_2ND_PASS_MAP_INDEX_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM0_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM0_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem0_4b_2nd_pass_map_index_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM0_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->map_index),
          &(tbl_data->map_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->map_data),
          &(tbl_data->map_data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem0_4b_2nd_pass_map_index_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem0_4b_2nd_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem0_4b_2nd_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM0_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM0_4B_2ND_PASS_MAP_INDEX_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM0_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM0_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem0_4b_2nd_pass_map_index_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->map_index),
          &(tbl->map_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->map_data),
          &(tbl->map_data),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM0_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem0_4b_2nd_pass_map_index_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem0_4b_2nd_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem0_4b_2nd_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM0_4B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM0_4B_2ND_PASS_MAP_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM0_4B_2ND_PASS_MAP_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM0_4B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem0_4b_2nd_pass_map_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM0_4B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->action_type),
          &(tbl_data->action_type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map0),
          &(tbl_data->field_select_map[0])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map1),
          &(tbl_data->field_select_map[1])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map2),
          &(tbl_data->field_select_map[2])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map3),
          &(tbl_data->field_select_map[3])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem0_4b_2nd_pass_map_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem0_4b_2nd_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem0_4b_2nd_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM0_4B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM0_4B_2ND_PASS_MAP_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM0_4B_2ND_PASS_MAP_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM0_4B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem0_4b_2nd_pass_map_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->action_type),
          &(tbl->action_type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[0]),
          &(tbl->field_select_map0),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[1]),
          &(tbl->field_select_map1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[2]),
          &(tbl->field_select_map2),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[3]),
          &(tbl->field_select_map3),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM0_4B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem0_4b_2nd_pass_map_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem1_4b_2nd_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem1_4b_2nd_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM1_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM1_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM1_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM1_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem1_4b_2nd_pass_key_profile_resolved_data_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM1_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->program),
          &(tbl_data->program)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->key_select),
          &(tbl_data->key_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem1_4b_2nd_pass_key_profile_resolved_data_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem1_4b_2nd_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem1_4b_2nd_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM1_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM1_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM1_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM1_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem1_4b_2nd_pass_key_profile_resolved_data_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->program),
          &(tbl->program),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->key_select),
          &(tbl->key_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM1_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem1_4b_2nd_pass_key_profile_resolved_data_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem1_4b_2nd_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem1_4b_2nd_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM1_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM1_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM1_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM1_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem1_4b_2nd_pass_program_resolved_data_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM1_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->bit_select),
          &(tbl_data->bit_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem1_4b_2nd_pass_program_resolved_data_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem1_4b_2nd_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem1_4b_2nd_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM1_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM1_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM1_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM1_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem1_4b_2nd_pass_program_resolved_data_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->bit_select),
          &(tbl->bit_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM1_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem1_4b_2nd_pass_program_resolved_data_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem1_4b_2nd_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem1_4b_2nd_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM1_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM1_4B_2ND_PASS_MAP_INDEX_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM1_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM1_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem1_4b_2nd_pass_map_index_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM1_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->map_index),
          &(tbl_data->map_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->map_data),
          &(tbl_data->map_data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem1_4b_2nd_pass_map_index_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem1_4b_2nd_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem1_4b_2nd_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM1_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM1_4B_2ND_PASS_MAP_INDEX_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM1_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM1_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem1_4b_2nd_pass_map_index_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->map_index),
          &(tbl->map_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->map_data),
          &(tbl->map_data),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM1_4B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem1_4b_2nd_pass_map_index_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem1_4b_2nd_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem1_4b_2nd_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM1_4B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM1_4B_2ND_PASS_MAP_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM1_4B_2ND_PASS_MAP_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM1_4B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem1_4b_2nd_pass_map_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM1_4B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->action_type),
          &(tbl_data->action_type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map0),
          &(tbl_data->field_select_map[0])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map1),
          &(tbl_data->field_select_map[1])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map2),
          &(tbl_data->field_select_map[2])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map3),
          &(tbl_data->field_select_map[3])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem1_4b_2nd_pass_map_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem1_4b_2nd_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem1_4b_2nd_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM1_4B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM1_4B_2ND_PASS_MAP_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM1_4B_2ND_PASS_MAP_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM1_4B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem1_4b_2nd_pass_map_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->action_type),
          &(tbl->action_type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[0]),
          &(tbl->field_select_map0),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[1]),
          &(tbl->field_select_map1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[2]),
          &(tbl->field_select_map2),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[3]),
          &(tbl->field_select_map3),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM1_4B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem1_4b_2nd_pass_map_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem2_14b_2nd_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem2_14b_2nd_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM2_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM2_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM2_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM2_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem2_14b_2nd_pass_key_profile_resolved_data_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM2_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->program),
          &(tbl_data->program)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->key_select),
          &(tbl_data->key_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem2_14b_2nd_pass_key_profile_resolved_data_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem2_14b_2nd_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem2_14b_2nd_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM2_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM2_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM2_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM2_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem2_14b_2nd_pass_key_profile_resolved_data_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->program),
          &(tbl->program),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->key_select),
          &(tbl->key_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM2_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem2_14b_2nd_pass_key_profile_resolved_data_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem2_14b_2nd_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem2_14b_2nd_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM2_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM2_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM2_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM2_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem2_14b_2nd_pass_program_resolved_data_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM2_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->bit_select),
          &(tbl_data->bit_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem2_14b_2nd_pass_program_resolved_data_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem2_14b_2nd_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem2_14b_2nd_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM2_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM2_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM2_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM2_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem2_14b_2nd_pass_program_resolved_data_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->bit_select),
          &(tbl->bit_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM2_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem2_14b_2nd_pass_program_resolved_data_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem2_14b_2nd_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem2_14b_2nd_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM2_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM2_14B_2ND_PASS_MAP_INDEX_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM2_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM2_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem2_14b_2nd_pass_map_index_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM2_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->map_index),
          &(tbl_data->map_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->map_data),
          &(tbl_data->map_data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem2_14b_2nd_pass_map_index_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem2_14b_2nd_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem2_14b_2nd_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM2_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM2_14B_2ND_PASS_MAP_INDEX_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM2_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM2_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem2_14b_2nd_pass_map_index_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->map_index),
          &(tbl->map_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->map_data),
          &(tbl->map_data),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM2_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem2_14b_2nd_pass_map_index_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem2_14b_2nd_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem2_14b_2nd_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM2_14B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM2_14B_2ND_PASS_MAP_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM2_14B_2ND_PASS_MAP_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM2_14B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem2_14b_2nd_pass_map_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM2_14B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->action_type),
          &(tbl_data->action_type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map0),
          &(tbl_data->field_select_map[0])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map1),
          &(tbl_data->field_select_map[1])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map2),
          &(tbl_data->field_select_map[2])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map3),
          &(tbl_data->field_select_map[3])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map4),
          &(tbl_data->field_select_map[4])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map5),
          &(tbl_data->field_select_map[5])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map6),
          &(tbl_data->field_select_map[6])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map7),
          &(tbl_data->field_select_map[7])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map8),
          &(tbl_data->field_select_map[8])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map9),
          &(tbl_data->field_select_map[9])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map10),
          &(tbl_data->field_select_map[10])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 51, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map11),
          &(tbl_data->field_select_map[11])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map12),
          &(tbl_data->field_select_map[12])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 53, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map13),
          &(tbl_data->field_select_map[13])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 54, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->offset_index),
          &(tbl_data->offset_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem2_14b_2nd_pass_map_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem2_14b_2nd_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem2_14b_2nd_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM2_14B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM2_14B_2ND_PASS_MAP_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM2_14B_2ND_PASS_MAP_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM2_14B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem2_14b_2nd_pass_map_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->action_type),
          &(tbl->action_type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[0]),
          &(tbl->field_select_map0),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[1]),
          &(tbl->field_select_map1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[2]),
          &(tbl->field_select_map2),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[3]),
          &(tbl->field_select_map3),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[4]),
          &(tbl->field_select_map4),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[5]),
          &(tbl->field_select_map5),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[6]),
          &(tbl->field_select_map6),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[7]),
          &(tbl->field_select_map7),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[8]),
          &(tbl->field_select_map8),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 39, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[9]),
          &(tbl->field_select_map9),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[10]),
          &(tbl->field_select_map10),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[11]),
          &(tbl->field_select_map11),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[12]),
          &(tbl->field_select_map12),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[13]),
          &(tbl->field_select_map13),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->offset_index),
          &(tbl->offset_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM2_14B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem2_14b_2nd_pass_map_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem2_14b_2nd_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem2_14b_2nd_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM2_14B_2ND_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM2_14B_2ND_PASS_OFFSET_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM2_14B_2ND_PASS_OFFSET_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM2_14B_2ND_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem2_14b_2nd_pass_offset_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM2_14B_2ND_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->offset),
          &(tbl_data->offset)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem2_14b_2nd_pass_offset_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem2_14b_2nd_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem2_14b_2nd_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM2_14B_2ND_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM2_14B_2ND_PASS_OFFSET_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM2_14B_2ND_PASS_OFFSET_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM2_14B_2ND_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem2_14b_2nd_pass_offset_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->offset),
          &(tbl->offset),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM2_14B_2ND_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem2_14b_2nd_pass_offset_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem3_14b_2nd_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem3_14b_2nd_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM3_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM3_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM3_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM3_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem3_14b_2nd_pass_key_profile_resolved_data_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM3_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->program),
          &(tbl_data->program)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->key_select),
          &(tbl_data->key_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem3_14b_2nd_pass_key_profile_resolved_data_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem3_14b_2nd_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem3_14b_2nd_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM3_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM3_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM3_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM3_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem3_14b_2nd_pass_key_profile_resolved_data_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->program),
          &(tbl->program),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->key_select),
          &(tbl->key_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM3_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem3_14b_2nd_pass_key_profile_resolved_data_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem3_14b_2nd_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem3_14b_2nd_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM3_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM3_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM3_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM3_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem3_14b_2nd_pass_program_resolved_data_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM3_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->bit_select),
          &(tbl_data->bit_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem3_14b_2nd_pass_program_resolved_data_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem3_14b_2nd_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem3_14b_2nd_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM3_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM3_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM3_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM3_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem3_14b_2nd_pass_program_resolved_data_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->bit_select),
          &(tbl->bit_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM3_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem3_14b_2nd_pass_program_resolved_data_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem3_14b_2nd_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem3_14b_2nd_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM3_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM3_14B_2ND_PASS_MAP_INDEX_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM3_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM3_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem3_14b_2nd_pass_map_index_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM3_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->map_index),
          &(tbl_data->map_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->map_data),
          &(tbl_data->map_data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem3_14b_2nd_pass_map_index_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem3_14b_2nd_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem3_14b_2nd_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM3_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM3_14B_2ND_PASS_MAP_INDEX_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM3_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM3_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem3_14b_2nd_pass_map_index_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->map_index),
          &(tbl->map_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->map_data),
          &(tbl->map_data),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM3_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem3_14b_2nd_pass_map_index_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem3_14b_2nd_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem3_14b_2nd_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM3_14B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM3_14B_2ND_PASS_MAP_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM3_14B_2ND_PASS_MAP_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM3_14B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem3_14b_2nd_pass_map_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM3_14B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->action_type),
          &(tbl_data->action_type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map0),
          &(tbl_data->field_select_map[0])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map1),
          &(tbl_data->field_select_map[1])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map2),
          &(tbl_data->field_select_map[2])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map3),
          &(tbl_data->field_select_map[3])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map4),
          &(tbl_data->field_select_map[4])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map5),
          &(tbl_data->field_select_map[5])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map6),
          &(tbl_data->field_select_map[6])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map7),
          &(tbl_data->field_select_map[7])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map8),
          &(tbl_data->field_select_map[8])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map9),
          &(tbl_data->field_select_map[9])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map10),
          &(tbl_data->field_select_map[10])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 51, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map11),
          &(tbl_data->field_select_map[11])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map12),
          &(tbl_data->field_select_map[12])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 53, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map13),
          &(tbl_data->field_select_map[13])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 54, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->offset_index),
          &(tbl_data->offset_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem3_14b_2nd_pass_map_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem3_14b_2nd_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem3_14b_2nd_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM3_14B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM3_14B_2ND_PASS_MAP_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM3_14B_2ND_PASS_MAP_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM3_14B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem3_14b_2nd_pass_map_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->action_type),
          &(tbl->action_type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[0]),
          &(tbl->field_select_map0),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[1]),
          &(tbl->field_select_map1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[2]),
          &(tbl->field_select_map2),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[3]),
          &(tbl->field_select_map3),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[4]),
          &(tbl->field_select_map4),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[5]),
          &(tbl->field_select_map5),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[6]),
          &(tbl->field_select_map6),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[7]),
          &(tbl->field_select_map7),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[8]),
          &(tbl->field_select_map8),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 39, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[9]),
          &(tbl->field_select_map9),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[10]),
          &(tbl->field_select_map10),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[11]),
          &(tbl->field_select_map11),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[12]),
          &(tbl->field_select_map12),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[13]),
          &(tbl->field_select_map13),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->offset_index),
          &(tbl->offset_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM3_14B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem3_14b_2nd_pass_map_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem3_14b_2nd_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem3_14b_2nd_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM3_14B_2ND_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM3_14B_2ND_PASS_OFFSET_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM3_14B_2ND_PASS_OFFSET_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM3_14B_2ND_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem3_14b_2nd_pass_offset_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM3_14B_2ND_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->offset),
          &(tbl_data->offset)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem3_14b_2nd_pass_offset_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem3_14b_2nd_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem3_14b_2nd_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM3_14B_2ND_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM3_14B_2ND_PASS_OFFSET_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM3_14B_2ND_PASS_OFFSET_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM3_14B_2ND_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem3_14b_2nd_pass_offset_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->offset),
          &(tbl->offset),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM3_14B_2ND_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem3_14b_2nd_pass_offset_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem4_14b_2nd_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem4_14b_2nd_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM4_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM4_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM4_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM4_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem4_14b_2nd_pass_key_profile_resolved_data_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM4_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->program),
          &(tbl_data->program)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->key_select),
          &(tbl_data->key_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem4_14b_2nd_pass_key_profile_resolved_data_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem4_14b_2nd_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem4_14b_2nd_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM4_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM4_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM4_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM4_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem4_14b_2nd_pass_key_profile_resolved_data_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->program),
          &(tbl->program),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->key_select),
          &(tbl->key_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM4_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem4_14b_2nd_pass_key_profile_resolved_data_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem4_14b_2nd_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem4_14b_2nd_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM4_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM4_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM4_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM4_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem4_14b_2nd_pass_program_resolved_data_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM4_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->bit_select),
          &(tbl_data->bit_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem4_14b_2nd_pass_program_resolved_data_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem4_14b_2nd_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem4_14b_2nd_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM4_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM4_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM4_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM4_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem4_14b_2nd_pass_program_resolved_data_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->bit_select),
          &(tbl->bit_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM4_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem4_14b_2nd_pass_program_resolved_data_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem4_14b_2nd_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem4_14b_2nd_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM4_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM4_14B_2ND_PASS_MAP_INDEX_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM4_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM4_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem4_14b_2nd_pass_map_index_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM4_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->map_index),
          &(tbl_data->map_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->map_data),
          &(tbl_data->map_data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem4_14b_2nd_pass_map_index_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem4_14b_2nd_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem4_14b_2nd_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM4_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM4_14B_2ND_PASS_MAP_INDEX_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM4_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM4_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem4_14b_2nd_pass_map_index_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->map_index),
          &(tbl->map_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->map_data),
          &(tbl->map_data),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM4_14B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem4_14b_2nd_pass_map_index_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem4_14b_2nd_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem4_14b_2nd_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM4_14B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM4_14B_2ND_PASS_MAP_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM4_14B_2ND_PASS_MAP_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM4_14B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem4_14b_2nd_pass_map_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM4_14B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->action_type),
          &(tbl_data->action_type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map0),
          &(tbl_data->field_select_map[0])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map1),
          &(tbl_data->field_select_map[1])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map2),
          &(tbl_data->field_select_map[2])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map3),
          &(tbl_data->field_select_map[3])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map4),
          &(tbl_data->field_select_map[4])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map5),
          &(tbl_data->field_select_map[5])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map6),
          &(tbl_data->field_select_map[6])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map7),
          &(tbl_data->field_select_map[7])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map8),
          &(tbl_data->field_select_map[8])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map9),
          &(tbl_data->field_select_map[9])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map10),
          &(tbl_data->field_select_map[10])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 51, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map11),
          &(tbl_data->field_select_map[11])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map12),
          &(tbl_data->field_select_map[12])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 53, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map13),
          &(tbl_data->field_select_map[13])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 54, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->offset_index),
          &(tbl_data->offset_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem4_14b_2nd_pass_map_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem4_14b_2nd_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem4_14b_2nd_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM4_14B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM4_14B_2ND_PASS_MAP_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM4_14B_2ND_PASS_MAP_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM4_14B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem4_14b_2nd_pass_map_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->action_type),
          &(tbl->action_type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[0]),
          &(tbl->field_select_map0),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[1]),
          &(tbl->field_select_map1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[2]),
          &(tbl->field_select_map2),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[3]),
          &(tbl->field_select_map3),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[4]),
          &(tbl->field_select_map4),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[5]),
          &(tbl->field_select_map5),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[6]),
          &(tbl->field_select_map6),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[7]),
          &(tbl->field_select_map7),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[8]),
          &(tbl->field_select_map8),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 39, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[9]),
          &(tbl->field_select_map9),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[10]),
          &(tbl->field_select_map10),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[11]),
          &(tbl->field_select_map11),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[12]),
          &(tbl->field_select_map12),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[13]),
          &(tbl->field_select_map13),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->offset_index),
          &(tbl->offset_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM4_14B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem4_14b_2nd_pass_map_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem4_14b_2nd_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem4_14b_2nd_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM4_14B_2ND_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM4_14B_2ND_PASS_OFFSET_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM4_14B_2ND_PASS_OFFSET_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM4_14B_2ND_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem4_14b_2nd_pass_offset_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM4_14B_2ND_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->offset),
          &(tbl_data->offset)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem4_14b_2nd_pass_offset_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem4_14b_2nd_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem4_14b_2nd_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM4_14B_2ND_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM4_14B_2ND_PASS_OFFSET_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM4_14B_2ND_PASS_OFFSET_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM4_14B_2ND_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem4_14b_2nd_pass_offset_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->offset),
          &(tbl->offset),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM4_14B_2ND_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem4_14b_2nd_pass_offset_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem5_17b_2nd_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem5_17b_2nd_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM5_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM5_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM5_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM5_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem5_17b_2nd_pass_key_profile_resolved_data_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM5_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->program),
          &(tbl_data->program)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->key_select),
          &(tbl_data->key_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem5_17b_2nd_pass_key_profile_resolved_data_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem5_17b_2nd_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem5_17b_2nd_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM5_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM5_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM5_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM5_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem5_17b_2nd_pass_key_profile_resolved_data_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->program),
          &(tbl->program),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->key_select),
          &(tbl->key_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM5_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem5_17b_2nd_pass_key_profile_resolved_data_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem5_17b_2nd_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem5_17b_2nd_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM5_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM5_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM5_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM5_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem5_17b_2nd_pass_program_resolved_data_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM5_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->bit_select),
          &(tbl_data->bit_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem5_17b_2nd_pass_program_resolved_data_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem5_17b_2nd_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem5_17b_2nd_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM5_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM5_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM5_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM5_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem5_17b_2nd_pass_program_resolved_data_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->bit_select),
          &(tbl->bit_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM5_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem5_17b_2nd_pass_program_resolved_data_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem5_17b_2nd_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem5_17b_2nd_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM5_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM5_17B_2ND_PASS_MAP_INDEX_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM5_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM5_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem5_17b_2nd_pass_map_index_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM5_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->map_index),
          &(tbl_data->map_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->map_data),
          &(tbl_data->map_data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem5_17b_2nd_pass_map_index_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem5_17b_2nd_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem5_17b_2nd_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM5_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM5_17B_2ND_PASS_MAP_INDEX_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM5_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM5_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem5_17b_2nd_pass_map_index_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->map_index),
          &(tbl->map_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->map_data),
          &(tbl->map_data),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM5_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem5_17b_2nd_pass_map_index_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem5_17b_2nd_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem5_17b_2nd_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM5_17B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM5_17B_2ND_PASS_MAP_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM5_17B_2ND_PASS_MAP_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM5_17B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem5_17b_2nd_pass_map_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM5_17B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->action_type),
          &(tbl_data->action_type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map0),
          &(tbl_data->field_select_map[0])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map1),
          &(tbl_data->field_select_map[1])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map2),
          &(tbl_data->field_select_map[2])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map3),
          &(tbl_data->field_select_map[3])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map4),
          &(tbl_data->field_select_map[4])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map5),
          &(tbl_data->field_select_map[5])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map6),
          &(tbl_data->field_select_map[6])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map7),
          &(tbl_data->field_select_map[7])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map8),
          &(tbl_data->field_select_map[8])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map9),
          &(tbl_data->field_select_map[9])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map10),
          &(tbl_data->field_select_map[10])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 51, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map11),
          &(tbl_data->field_select_map[11])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map12),
          &(tbl_data->field_select_map[12])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 53, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map13),
          &(tbl_data->field_select_map[13])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 54, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map14),
          &(tbl_data->field_select_map[14])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map15),
          &(tbl_data->field_select_map[15])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 56, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map16),
          &(tbl_data->field_select_map[16])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 57, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->offset_index),
          &(tbl_data->offset_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 58, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem5_17b_2nd_pass_map_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem5_17b_2nd_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem5_17b_2nd_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM5_17B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM5_17B_2ND_PASS_MAP_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM5_17B_2ND_PASS_MAP_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM5_17B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem5_17b_2nd_pass_map_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->action_type),
          &(tbl->action_type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[0]),
          &(tbl->field_select_map0),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[1]),
          &(tbl->field_select_map1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[2]),
          &(tbl->field_select_map2),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[3]),
          &(tbl->field_select_map3),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[4]),
          &(tbl->field_select_map4),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[5]),
          &(tbl->field_select_map5),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[6]),
          &(tbl->field_select_map6),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[7]),
          &(tbl->field_select_map7),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[8]),
          &(tbl->field_select_map8),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 39, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[9]),
          &(tbl->field_select_map9),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[10]),
          &(tbl->field_select_map10),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[11]),
          &(tbl->field_select_map11),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[12]),
          &(tbl->field_select_map12),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[13]),
          &(tbl->field_select_map13),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[14]),
          &(tbl->field_select_map14),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[15]),
          &(tbl->field_select_map15),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[16]),
          &(tbl->field_select_map16),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->offset_index),
          &(tbl->offset_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM5_17B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem5_17b_2nd_pass_map_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem5_17b_2nd_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem5_17b_2nd_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM5_17B_2ND_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM5_17B_2ND_PASS_OFFSET_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM5_17B_2ND_PASS_OFFSET_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM5_17B_2ND_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem5_17b_2nd_pass_offset_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM5_17B_2ND_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->offset),
          &(tbl_data->offset)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem5_17b_2nd_pass_offset_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem5_17b_2nd_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem5_17b_2nd_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM5_17B_2ND_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM5_17B_2ND_PASS_OFFSET_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM5_17B_2ND_PASS_OFFSET_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM5_17B_2ND_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem5_17b_2nd_pass_offset_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->offset),
          &(tbl->offset),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM5_17B_2ND_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem5_17b_2nd_pass_offset_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem6_17b_2nd_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem6_17b_2nd_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM6_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM6_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM6_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM6_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem6_17b_2nd_pass_key_profile_resolved_data_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM6_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->program),
          &(tbl_data->program)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->key_select),
          &(tbl_data->key_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem6_17b_2nd_pass_key_profile_resolved_data_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem6_17b_2nd_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem6_17b_2nd_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM6_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM6_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM6_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM6_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem6_17b_2nd_pass_key_profile_resolved_data_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->program),
          &(tbl->program),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->key_select),
          &(tbl->key_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM6_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem6_17b_2nd_pass_key_profile_resolved_data_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem6_17b_2nd_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem6_17b_2nd_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM6_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM6_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM6_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM6_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem6_17b_2nd_pass_program_resolved_data_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM6_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->bit_select),
          &(tbl_data->bit_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem6_17b_2nd_pass_program_resolved_data_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem6_17b_2nd_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem6_17b_2nd_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM6_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM6_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM6_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM6_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem6_17b_2nd_pass_program_resolved_data_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->bit_select),
          &(tbl->bit_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM6_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem6_17b_2nd_pass_program_resolved_data_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem6_17b_2nd_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem6_17b_2nd_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM6_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM6_17B_2ND_PASS_MAP_INDEX_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM6_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM6_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem6_17b_2nd_pass_map_index_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM6_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->map_index),
          &(tbl_data->map_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->map_data),
          &(tbl_data->map_data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem6_17b_2nd_pass_map_index_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem6_17b_2nd_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem6_17b_2nd_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM6_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM6_17B_2ND_PASS_MAP_INDEX_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM6_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM6_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem6_17b_2nd_pass_map_index_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->map_index),
          &(tbl->map_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->map_data),
          &(tbl->map_data),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM6_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem6_17b_2nd_pass_map_index_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem6_17b_2nd_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem6_17b_2nd_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM6_17B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM6_17B_2ND_PASS_MAP_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM6_17B_2ND_PASS_MAP_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM6_17B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem6_17b_2nd_pass_map_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM6_17B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->action_type),
          &(tbl_data->action_type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map0),
          &(tbl_data->field_select_map[0])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map1),
          &(tbl_data->field_select_map[1])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map2),
          &(tbl_data->field_select_map[2])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map3),
          &(tbl_data->field_select_map[3])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map4),
          &(tbl_data->field_select_map[4])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map5),
          &(tbl_data->field_select_map[5])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map6),
          &(tbl_data->field_select_map[6])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map7),
          &(tbl_data->field_select_map[7])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map8),
          &(tbl_data->field_select_map[8])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map9),
          &(tbl_data->field_select_map[9])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map10),
          &(tbl_data->field_select_map[10])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 51, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map11),
          &(tbl_data->field_select_map[11])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map12),
          &(tbl_data->field_select_map[12])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 53, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map13),
          &(tbl_data->field_select_map[13])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 54, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map14),
          &(tbl_data->field_select_map[14])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map15),
          &(tbl_data->field_select_map[15])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 56, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map16),
          &(tbl_data->field_select_map[16])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 57, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->offset_index),
          &(tbl_data->offset_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 58, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem6_17b_2nd_pass_map_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem6_17b_2nd_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem6_17b_2nd_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM6_17B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM6_17B_2ND_PASS_MAP_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM6_17B_2ND_PASS_MAP_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM6_17B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem6_17b_2nd_pass_map_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->action_type),
          &(tbl->action_type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[0]),
          &(tbl->field_select_map0),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[1]),
          &(tbl->field_select_map1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[2]),
          &(tbl->field_select_map2),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[3]),
          &(tbl->field_select_map3),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[4]),
          &(tbl->field_select_map4),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[5]),
          &(tbl->field_select_map5),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[6]),
          &(tbl->field_select_map6),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[7]),
          &(tbl->field_select_map7),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[8]),
          &(tbl->field_select_map8),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 39, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[9]),
          &(tbl->field_select_map9),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[10]),
          &(tbl->field_select_map10),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[11]),
          &(tbl->field_select_map11),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[12]),
          &(tbl->field_select_map12),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[13]),
          &(tbl->field_select_map13),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[14]),
          &(tbl->field_select_map14),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[15]),
          &(tbl->field_select_map15),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[16]),
          &(tbl->field_select_map16),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->offset_index),
          &(tbl->offset_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM6_17B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem6_17b_2nd_pass_map_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem6_17b_2nd_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem6_17b_2nd_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM6_17B_2ND_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM6_17B_2ND_PASS_OFFSET_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM6_17B_2ND_PASS_OFFSET_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM6_17B_2ND_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem6_17b_2nd_pass_offset_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM6_17B_2ND_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->offset),
          &(tbl_data->offset)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem6_17b_2nd_pass_offset_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem6_17b_2nd_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem6_17b_2nd_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM6_17B_2ND_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM6_17B_2ND_PASS_OFFSET_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM6_17B_2ND_PASS_OFFSET_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM6_17B_2ND_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem6_17b_2nd_pass_offset_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->offset),
          &(tbl->offset),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM6_17B_2ND_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem6_17b_2nd_pass_offset_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem7_17b_2nd_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem7_17b_2nd_pass_key_profile_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM7_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM7_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM7_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM7_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem7_17b_2nd_pass_key_profile_resolved_data_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM7_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->program),
          &(tbl_data->program)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->key_select),
          &(tbl_data->key_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem7_17b_2nd_pass_key_profile_resolved_data_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem7_17b_2nd_pass_key_profile_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem7_17b_2nd_pass_key_profile_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM7_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM7_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM7_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM7_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem7_17b_2nd_pass_key_profile_resolved_data_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->program),
          &(tbl->program),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->key_select),
          &(tbl->key_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM7_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem7_17b_2nd_pass_key_profile_resolved_data_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem7_17b_2nd_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem7_17b_2nd_pass_program_resolved_data_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM7_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM7_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM7_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM7_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem7_17b_2nd_pass_program_resolved_data_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM7_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->bit_select),
          &(tbl_data->bit_select)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem7_17b_2nd_pass_program_resolved_data_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem7_17b_2nd_pass_program_resolved_data_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem7_17b_2nd_pass_program_resolved_data_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM7_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM7_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM7_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM7_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem7_17b_2nd_pass_program_resolved_data_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->bit_select),
          &(tbl->bit_select),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM7_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem7_17b_2nd_pass_program_resolved_data_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem7_17b_2nd_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem7_17b_2nd_pass_map_index_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM7_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM7_17B_2ND_PASS_MAP_INDEX_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM7_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM7_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem7_17b_2nd_pass_map_index_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM7_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->map_index),
          &(tbl_data->map_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->map_data),
          &(tbl_data->map_data)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem7_17b_2nd_pass_map_index_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem7_17b_2nd_pass_map_index_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem7_17b_2nd_pass_map_index_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM7_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM7_17B_2ND_PASS_MAP_INDEX_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM7_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM7_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem7_17b_2nd_pass_map_index_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->map_index),
          &(tbl->map_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->map_data),
          &(tbl->map_data),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM7_17B_2ND_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem7_17b_2nd_pass_map_index_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem7_17b_2nd_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem7_17b_2nd_pass_map_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM7_17B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM7_17B_2ND_PASS_MAP_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM7_17B_2ND_PASS_MAP_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM7_17B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem7_17b_2nd_pass_map_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM7_17B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->action_type),
          &(tbl_data->action_type)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map0),
          &(tbl_data->field_select_map[0])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map1),
          &(tbl_data->field_select_map[1])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map2),
          &(tbl_data->field_select_map[2])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map3),
          &(tbl_data->field_select_map[3])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map4),
          &(tbl_data->field_select_map[4])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map5),
          &(tbl_data->field_select_map[5])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map6),
          &(tbl_data->field_select_map[6])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map7),
          &(tbl_data->field_select_map[7])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map8),
          &(tbl_data->field_select_map[8])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map9),
          &(tbl_data->field_select_map[9])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map10),
          &(tbl_data->field_select_map[10])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 51, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map11),
          &(tbl_data->field_select_map[11])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map12),
          &(tbl_data->field_select_map[12])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 53, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map13),
          &(tbl_data->field_select_map[13])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 54, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map14),
          &(tbl_data->field_select_map[14])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map15),
          &(tbl_data->field_select_map[15])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 56, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->field_select_map16),
          &(tbl_data->field_select_map[16])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 57, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->offset_index),
          &(tbl_data->offset_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 58, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem7_17b_2nd_pass_map_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem7_17b_2nd_pass_map_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem7_17b_2nd_pass_map_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM7_17B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM7_17B_2ND_PASS_MAP_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM7_17B_2ND_PASS_MAP_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM7_17B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem7_17b_2nd_pass_map_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->action_type),
          &(tbl->action_type),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[0]),
          &(tbl->field_select_map0),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[1]),
          &(tbl->field_select_map1),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[2]),
          &(tbl->field_select_map2),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[3]),
          &(tbl->field_select_map3),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[4]),
          &(tbl->field_select_map4),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[5]),
          &(tbl->field_select_map5),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[6]),
          &(tbl->field_select_map6),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[7]),
          &(tbl->field_select_map7),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[8]),
          &(tbl->field_select_map8),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 39, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[9]),
          &(tbl->field_select_map9),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[10]),
          &(tbl->field_select_map10),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[11]),
          &(tbl->field_select_map11),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[12]),
          &(tbl->field_select_map12),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[13]),
          &(tbl->field_select_map13),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[14]),
          &(tbl->field_select_map14),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[15]),
          &(tbl->field_select_map15),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->field_select_map[16]),
          &(tbl->field_select_map16),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->offset_index),
          &(tbl->offset_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM7_17B_2ND_PASS_MAP_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem7_17b_2nd_pass_map_table_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table fem7_17b_2nd_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem7_17b_2nd_pass_offset_table_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM7_17B_2ND_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM7_17B_2ND_PASS_OFFSET_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM7_17B_2ND_PASS_OFFSET_TABLE_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM7_17B_2ND_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem7_17b_2nd_pass_offset_table_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM7_17B_2ND_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->offset),
          &(tbl_data->offset)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem7_17b_2nd_pass_offset_table_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table fem7_17b_2nd_pass_offset_table_tbl from block IHB,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_ihb_fem7_17b_2nd_pass_offset_table_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_IHB_FEM7_17B_2ND_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_IHB_FEM7_17B_2ND_PASS_OFFSET_TABLE_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_IHB_FEM7_17B_2ND_PASS_OFFSET_TABLE_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_IHB_FEM7_17B_2ND_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->ihb.fem7_17b_2nd_pass_offset_table_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->offset),
          &(tbl->offset),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_IHB_ID,
          SOC_PB_IHB_FEM7_17B_2ND_PASS_OFFSET_TABLE_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_ihb_fem7_17b_2nd_pass_offset_table_tbl_set_unsafe()", entry_offset, 0);
}
/*
 * Read indirect table copy_engine0_program_tbl from block EPNI,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_epni_copy_engine_program_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              tbl_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_EPNI_COPY_ENGINE_PROGRAM_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_EPNI_COPY_ENGINE0_PROGRAM_TBL_ENTRY_SIZE],
    inst_idx;
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_EPNI_COPY_ENGINE_PROGRAM_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EPNI_COPY_ENGINE0_PROGRAM_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_EPNI_COPY_ENGINE0_PROGRAM_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_EPNI_COPY_ENGINE_PROGRAM_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->epni.copy_engine_program_tbl[tbl_ndx]);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_EPNI_ID,
          SOC_PB_EPNI_COPY_ENGINE0_PROGRAM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  for (inst_idx = 0; inst_idx < 2; ++inst_idx)
  {
    res = soc_petra_field_in_place_get(
            data,
            &(tbl->inst_valid[inst_idx]),
            &(tbl_data->inst_valid[inst_idx])
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    res = soc_petra_field_in_place_get(
            data,
            &(tbl->inst_source_select[inst_idx]),
            &(tbl_data->inst_source_select[inst_idx])
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

    res = soc_petra_field_in_place_get(
            data,
            &(tbl->inst_header_offset_select[inst_idx]),
            &(tbl_data->inst_header_offset_select[inst_idx])
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

    res = soc_petra_field_in_place_get(
            data,
            &(tbl->inst_niblle_field_offset[inst_idx]),
            &(tbl_data->inst_niblle_field_offset[inst_idx])
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

    res = soc_petra_field_in_place_get(
            data,
            &(tbl->inst_bit_count[inst_idx]),
            &(tbl_data->inst_bit_count[inst_idx])
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

    res = soc_petra_field_in_place_get(
            data,
            &(tbl->inst_lfem_program[inst_idx]),
            &(tbl_data->inst_lfem_program[inst_idx])
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_epni_copy_engine0_program_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table copy_engine0_program_tbl from block EPNI,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_epni_copy_engine_program_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              tbl_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_EPNI_COPY_ENGINE_PROGRAM_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_EPNI_COPY_ENGINE0_PROGRAM_TBL_ENTRY_SIZE],
    inst_ndx;
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_EPNI_COPY_ENGINE_PROGRAM_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EPNI_COPY_ENGINE0_PROGRAM_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_EPNI_COPY_ENGINE0_PROGRAM_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->epni.copy_engine_program_tbl[tbl_ndx]);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  for (inst_ndx = 0; inst_ndx < 2; ++inst_ndx)
  {
    res = soc_petra_field_in_place_set(
            &(tbl_data->inst_valid[inst_ndx]),
            &(tbl->inst_valid[inst_ndx]),
            data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    res = soc_petra_field_in_place_set(
            &(tbl_data->inst_source_select[inst_ndx]),
            &(tbl->inst_source_select[inst_ndx]),
            data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

    res = soc_petra_field_in_place_set(
            &(tbl_data->inst_header_offset_select[inst_ndx]),
            &(tbl->inst_header_offset_select[inst_ndx]),
            data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

    res = soc_petra_field_in_place_set(
            &(tbl_data->inst_niblle_field_offset[inst_ndx]),
            &(tbl->inst_niblle_field_offset[inst_ndx]),
            data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

    res = soc_petra_field_in_place_set(
            &(tbl_data->inst_bit_count[inst_ndx]),
            &(tbl->inst_bit_count[inst_ndx]),
            data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

    res = soc_petra_field_in_place_set(
            &(tbl_data->inst_lfem_program[inst_ndx]),
            &(tbl->inst_lfem_program[inst_ndx]),
            data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);
  }

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_EPNI_ID,
          SOC_PB_EPNI_COPY_ENGINE0_PROGRAM_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_epni_copy_engine0_program_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table copy_engine1_program_tbl from block EPNI,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_epni_lfem_field_select_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              tbl_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_EPNI_LFEM0_FIELD_SELECT_MAP_TBL_ENTRY_SIZE],
    fld_idx;
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EPNI_LFEM0_FIELD_SELECT_MAP_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_EPNI_LFEM0_FIELD_SELECT_MAP_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->epni.lfem_field_select_map_tbl[tbl_ndx]);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_EPNI_ID,
          SOC_PB_EPNI_LFEM0_FIELD_SELECT_MAP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  for (fld_idx = 0; fld_idx < SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_NOF_FLDS; ++fld_idx)
  {
    res = soc_petra_field_in_place_get(
            data,
            &(tbl->lfem_field_select_map[fld_idx]),
            &(tbl_data->lfem_field_select_map[fld_idx])
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_epni_lfem0_field_select_map_tbl_get_unsafe()", entry_offset, 0);
}

uint32
  soc_pb_epni_lfem_field_select_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              tbl_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_EPNI_LFEM0_FIELD_SELECT_MAP_TBL_ENTRY_SIZE],
    fld_idx;
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_EPNI_LFEM0_FIELD_SELECT_MAP_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_EPNI_LFEM0_FIELD_SELECT_MAP_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->epni.lfem_field_select_map_tbl[tbl_ndx]);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  for (fld_idx = 0; fld_idx < SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_NOF_FLDS; ++fld_idx)
  {
    res = soc_petra_field_in_place_set(
            &(tbl_data->lfem_field_select_map[fld_idx]),
            &(tbl->lfem_field_select_map[fld_idx]),
            data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_EPNI_ID,
          SOC_PB_EPNI_LFEM0_FIELD_SELECT_MAP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_epni_lfem0_field_select_map_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table rcl2_ofp_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_cfc_rcl2_ofp_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_CFC_RCL2_OFP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_CFC_RCL2_OFP_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_CFC_RCL2_OFP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CFC_RCL2_OFP_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_CFC_RCL2_OFP_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_CFC_RCL2_OFP_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->cfc.rcl2_ofp_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_CFC_ID,
          SOC_PB_CFC_RCL2_OFP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ofp_num),
          &(tbl_data->ofp_num)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->sch_hp_valid),
          &(tbl_data->sch_hp_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->sch_lp_valid),
          &(tbl_data->sch_lp_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->egq_hp_valid),
          &(tbl_data->egq_hp_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->egq_lp_valid),
          &(tbl_data->egq_lp_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cfc_rcl2_ofp_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table rcl2_ofp_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_cfc_rcl2_ofp_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_CFC_RCL2_OFP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_CFC_RCL2_OFP_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_CFC_RCL2_OFP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CFC_RCL2_OFP_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_CFC_RCL2_OFP_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->cfc.rcl2_ofp_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->ofp_num),
          &(tbl->ofp_num),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->sch_hp_valid),
          &(tbl->sch_hp_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->sch_lp_valid),
          &(tbl->sch_lp_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->egq_hp_valid),
          &(tbl->egq_hp_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->egq_lp_valid),
          &(tbl->egq_lp_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_CFC_ID,
          SOC_PB_CFC_RCL2_OFP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cfc_rcl2_ofp_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table nifclsb2_ofp_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_cfc_nifclsb2_ofp_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_CFC_NIFCLSB2_OFP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_CFC_NIFCLSB2_OFP_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_CFC_NIFCLSB2_OFP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CFC_NIFCLSB2_OFP_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_CFC_NIFCLSB2_OFP_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_CFC_NIFCLSB2_OFP_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->cfc.nifclsb2_ofp_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_CFC_ID,
          SOC_PB_CFC_NIFCLSB2_OFP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ofp_num[0]),
          &(tbl_data->data[0].ofp_num)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ofp_num[1]),
          &(tbl_data->data[1].ofp_num)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ofp_num[2]),
          &(tbl_data->data[2].ofp_num)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ofp_num[3]),
          &(tbl_data->data[3].ofp_num)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->sch_ofp_hp_valid[0]),
          &(tbl_data->data[0].sch.hp_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->sch_ofp_hp_valid[1]),
          &(tbl_data->data[1].sch.hp_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->sch_ofp_hp_valid[2]),
          &(tbl_data->data[2].sch.hp_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->sch_ofp_hp_valid[3]),
          &(tbl_data->data[3].sch.hp_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->sch_ofp_lp_valid[0]),
          &(tbl_data->data[0].sch.lp_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->sch_ofp_lp_valid[1]),
          &(tbl_data->data[1].sch.lp_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->sch_ofp_lp_valid[2]),
          &(tbl_data->data[2].sch.lp_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->sch_ofp_lp_valid[3]),
          &(tbl_data->data[3].sch.lp_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 51, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->egq_ofp_hp_valid[0]),
          &(tbl_data->data[0].egq.hp_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->egq_ofp_hp_valid[1]),
          &(tbl_data->data[1].egq.hp_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 53, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->egq_ofp_hp_valid[2]),
          &(tbl_data->data[2].egq.hp_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 54, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->egq_ofp_hp_valid[3]),
          &(tbl_data->data[3].egq.hp_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->egq_ofp_lp_valid[0]),
          &(tbl_data->data[0].egq.lp_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 56, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->egq_ofp_lp_valid[1]),
          &(tbl_data->data[1].egq.lp_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 57, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->egq_ofp_lp_valid[2]),
          &(tbl_data->data[2].egq.lp_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 58, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->egq_ofp_lp_valid[3]),
          &(tbl_data->data[3].egq.lp_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 59, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cfc_nifclsb2_ofp_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table nifclsb2_ofp_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_cfc_nifclsb2_ofp_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_CFC_NIFCLSB2_OFP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_CFC_NIFCLSB2_OFP_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_CFC_NIFCLSB2_OFP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CFC_NIFCLSB2_OFP_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_CFC_NIFCLSB2_OFP_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->cfc.nifclsb2_ofp_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->data[0].ofp_num),
          &(tbl->ofp_num[0]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->data[1].ofp_num),
          &(tbl->ofp_num[1]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->data[2].ofp_num),
          &(tbl->ofp_num[2]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->data[3].ofp_num),
          &(tbl->ofp_num[3]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->data[0].sch.hp_valid),
          &(tbl->sch_ofp_hp_valid[0]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->data[1].sch.hp_valid),
          &(tbl->sch_ofp_hp_valid[1]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->data[2].sch.hp_valid),
          &(tbl->sch_ofp_hp_valid[2]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->data[3].sch.hp_valid),
          &(tbl->sch_ofp_hp_valid[3]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->data[0].sch.lp_valid),
          &(tbl->sch_ofp_lp_valid[0]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->data[1].sch.lp_valid),
          &(tbl->sch_ofp_lp_valid[1]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 39, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->data[2].sch.lp_valid),
          &(tbl->sch_ofp_lp_valid[2]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->data[3].sch.lp_valid),
          &(tbl->sch_ofp_lp_valid[3]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->data[0].egq.hp_valid),
          &(tbl->egq_ofp_hp_valid[0]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->data[1].egq.hp_valid),
          &(tbl->egq_ofp_hp_valid[1]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 43, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->data[2].egq.hp_valid),
          &(tbl->egq_ofp_hp_valid[2]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->data[3].egq.hp_valid),
          &(tbl->egq_ofp_hp_valid[3]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->data[0].egq.lp_valid),
          &(tbl->egq_ofp_lp_valid[0]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->data[1].egq.lp_valid),
          &(tbl->egq_ofp_lp_valid[1]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 47, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->data[2].egq.lp_valid),
          &(tbl->egq_ofp_lp_valid[2]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->data[3].egq.lp_valid),
          &(tbl->egq_ofp_lp_valid[3]),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 49, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_CFC_ID,
          SOC_PB_CFC_NIFCLSB2_OFP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cfc_nifclsb2_ofp_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table calrx_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_cfc_oob_calrx_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID          if_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_CFC_CALRX_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_CFC_CALRX_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_CFC_CALRX_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CFC_CALRX_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_CFC_CALRX_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_CFC_CALRX_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = (if_ndx == SOC_TMC_FC_OOB_ID_A) ? &(tables->cfc.calrxa_tbl) : &(tables->cfc.calrxb_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_CFC_ID,
          SOC_PB_CFC_CALRX_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->fc_index),
          &(tbl_data->fc_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->fc_dest_sel),
          &(tbl_data->fc_dest_sel)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cfc_oob_calrx_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table calrx_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_cfc_oob_calrx_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID          if_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_CFC_CALRX_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_CFC_CALRX_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_CFC_CALRX_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CFC_CALRX_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_CFC_CALRX_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = (if_ndx == SOC_TMC_FC_OOB_ID_A) ? &(tables->cfc.calrxa_tbl) : &(tables->cfc.calrxb_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->fc_index),
          &(tbl->fc_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->fc_dest_sel),
          &(tbl->fc_dest_sel),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_CFC_ID,
          SOC_PB_CFC_CALRX_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cfc_oob_calrx_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table oob0_sch_map_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_cfc_oob_sch_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID          if_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_CFC_OOB_SCH_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_CFC_OOB_SCH_MAP_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_CFC_OOB_SCH_MAP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CFC_OOB_SCH_MAP_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_CFC_OOB_SCH_MAP_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_CFC_OOB_SCH_MAP_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = (if_ndx == SOC_TMC_FC_OOB_ID_A) ? &(tables->cfc.oob0_sch_map_tbl) : &(tables->cfc.oob1_sch_map_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_CFC_ID,
          SOC_PB_CFC_OOB_SCH_MAP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ofp_hr),
          &(tbl_data->ofp_hr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->lp_ofp_valid),
          &(tbl_data->lp_ofp_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->hp_ofp_valid),
          &(tbl_data->hp_ofp_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cfc_oob_sch_map_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table oob0_sch_map_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_cfc_oob_sch_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID          if_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_CFC_OOB_SCH_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_CFC_OOB_SCH_MAP_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_CFC_OOB_SCH_MAP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CFC_OOB_SCH_MAP_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_CFC_OOB_SCH_MAP_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = (if_ndx == SOC_TMC_FC_OOB_ID_A) ? &(tables->cfc.oob0_sch_map_tbl) : &(tables->cfc.oob1_sch_map_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->ofp_hr),
          &(tbl->ofp_hr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->lp_ofp_valid),
          &(tbl->lp_ofp_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->hp_ofp_valid),
          &(tbl->hp_ofp_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_CFC_ID,
          SOC_PB_CFC_OOB_SCH_MAP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cfc_oob_sch_map_tbl_set_unsafe()", entry_offset, 0);
}


/*
 * Read indirect table caltx_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_cfc_oob_caltx_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_CFC_CALTX_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_CFC_CALTX_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_CFC_CALTX_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CFC_CALTX_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_CFC_CALTX_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_CFC_CALTX_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->cfc.caltx_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_CFC_ID,
          SOC_PB_CFC_CALTX_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->fc_index),
          &(tbl_data->fc_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->fc_source_sel),
          &(tbl_data->fc_source_sel)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cfc_oob_caltx_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table caltx_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_cfc_oob_caltx_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_CFC_CALTX_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_CFC_CALTX_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_CFC_CALTX_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CFC_CALTX_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_CFC_CALTX_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = &(tables->cfc.caltx_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->fc_index),
          &(tbl->fc_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->fc_source_sel),
          &(tbl->fc_source_sel),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_CFC_ID,
          SOC_PB_CFC_CALTX_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cfc_oob_caltx_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table ilkn_calrx_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_cfc_ilkn_calrx_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID          if_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_CFC_CALRX_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_CFC_CALRX_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_CFC_CALRX_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CFC_CALRX_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_CFC_CALRX_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_CFC_CALRX_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = (if_ndx == SOC_TMC_FC_OOB_ID_A) ? &(tables->cfc.ilkn0_calrx_tbl) : &(tables->cfc.ilkn1_calrx_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_CFC_ID,
          SOC_PB_CFC_CALRX_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->fc_index),
          &(tbl_data->fc_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->fc_dest_sel),
          &(tbl_data->fc_dest_sel)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cfc_ilkn_calrx_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table ilkn_calrx_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_cfc_ilkn_calrx_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID          if_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_CFC_CALRX_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_CFC_CALRX_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_CFC_CALRX_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CFC_CALRX_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_CFC_CALRX_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = (if_ndx == SOC_TMC_FC_OOB_ID_A) ? &(tables->cfc.ilkn0_calrx_tbl) : &(tables->cfc.ilkn1_calrx_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->fc_index),
          &(tbl->fc_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->fc_dest_sel),
          &(tbl->fc_dest_sel),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_CFC_ID,
          SOC_PB_CFC_CALRX_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cfc_ilkn_calrx_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table ilkn_sch_map_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_cfc_ilkn_sch_map_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID          if_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_CFC_OOB_SCH_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_CFC_OOB_SCH_MAP_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_CFC_OOB_SCH_MAP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CFC_OOB_SCH_MAP_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_CFC_OOB_SCH_MAP_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_CFC_OOB_SCH_MAP_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = (if_ndx == SOC_TMC_FC_OOB_ID_A) ? &(tables->cfc.ilkn0_sch_map_tbl) : &(tables->cfc.ilkn1_sch_map_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_CFC_ID,
          SOC_PB_CFC_OOB_SCH_MAP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->ofp_hr),
          &(tbl_data->ofp_hr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->lp_ofp_valid),
          &(tbl_data->lp_ofp_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->hp_ofp_valid),
          &(tbl_data->hp_ofp_valid)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cfc_ilkn_sch_map_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table ilkn_sch_map_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_cfc_ilkn_sch_map_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID          if_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_CFC_OOB_SCH_MAP_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_CFC_OOB_SCH_MAP_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_CFC_OOB_SCH_MAP_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CFC_OOB_SCH_MAP_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_CFC_OOB_SCH_MAP_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = (if_ndx == SOC_TMC_FC_OOB_ID_A) ? &(tables->cfc.ilkn0_sch_map_tbl) : &(tables->cfc.ilkn1_sch_map_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->ofp_hr),
          &(tbl->ofp_hr),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->lp_ofp_valid),
          &(tbl->lp_ofp_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->hp_ofp_valid),
          &(tbl->hp_ofp_valid),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_CFC_ID,
          SOC_PB_CFC_OOB_SCH_MAP_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cfc_ilkn_sch_map_tbl_set_unsafe()", entry_offset, 0);
}

/*
 * Read indirect table ilkn_caltx_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_cfc_ilkn_caltx_tbl_get_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID          if_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_OUT SOC_PB_CFC_CALTX_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_CFC_CALTX_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_CFC_CALTX_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CFC_CALTX_TBL_GET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_CFC_CALTX_TBL_ENTRY_SIZE);
  SOC_PB_CLEAR(tbl_data, SOC_PB_CFC_CALTX_TBL_DATA, 1);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = (if_ndx == SOC_TMC_FC_OOB_ID_A) ? &(tables->cfc.ilkn0_caltx_tbl) : &(tables->cfc.ilkn1_caltx_tbl);

  offset = tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_sand_tbl_read_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_CFC_ID,
          SOC_PB_CFC_CALTX_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->fc_index),
          &(tbl_data->fc_index)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_field_in_place_get(
          data,
          &(tbl->fc_source_sel),
          &(tbl_data->fc_source_sel)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 41, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cfc_ilkn_caltx_tbl_get_unsafe()", entry_offset, 0);
}

/*
 * Write indirect table ilkn_caltx_tbl from block CFC,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
uint32
  soc_pb_cfc_ilkn_caltx_tbl_set_unsafe(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  SOC_PB_FC_OOB_ID          if_ndx,
    SOC_SAND_IN  uint32              entry_offset,
    SOC_SAND_IN  SOC_PB_CFC_CALTX_TBL_DATA  *tbl_data
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    offset  = 0,
    data[SOC_PB_CFC_CALTX_TBL_ENTRY_SIZE];
  SOC_PB_TBLS
    *tables = NULL;
  SOC_PB_CFC_CALTX_TBL
    *tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CFC_CALTX_TBL_SET_UNSAFE);

  SOC_PB_CLEAR(data, uint32, SOC_PB_CFC_CALTX_TBL_ENTRY_SIZE);

  res = soc_pb_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  tbl = (if_ndx == SOC_TMC_FC_OOB_ID_A) ? &(tables->cfc.ilkn0_caltx_tbl) : &(tables->cfc.ilkn1_caltx_tbl);

  offset =  tbl->addr.base + entry_offset;

  SOC_PB_TBL_SIZE_CHECK;

  res = soc_petra_field_in_place_set(
          &(tbl_data->fc_index),
          &(tbl->fc_index),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_field_in_place_set(
          &(tbl_data->fc_source_sel),
          &(tbl->fc_source_sel),
          data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

  res = soc_sand_tbl_write_unsafe(
          unit,
          data,
          offset,
          sizeof(data),
          SOC_PB_CFC_ID,
          SOC_PB_CFC_CALTX_TBL_ENTRY_SIZE * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cfc_ilkn_caltx_tbl_set_unsafe()", entry_offset, 0);
}


/* } */
#include <soc/dpp/SAND/Utils/sand_footer.h>
                                                                    
