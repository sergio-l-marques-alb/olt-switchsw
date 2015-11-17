/* $Id: pb_pp_init_tbl.c,v 1.7 Broadcom SDK $
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

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_TM/pb_pp_init_tbl.h>
#include <soc/dpp/Petra/PB_TM/pb_api_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_api_general.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level_pgm.h>


/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/* } */
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
 * GLOBALS   *
 *************/
/* { */


/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

STATIC uint32
  soc_pb_pp_mgmt_eci_tbls_init_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MGMT_ECI_TBLS_INIT);

  SOC_PB_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_mgmt_eci_tbls_init_unsafe()",0,0);
}

uint32
  soc_pb_pp_mgmt_ihp_tbls_init_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MGMT_IHP_TBLS_INIT);
  
  do
  {
    SOC_PB_PP_IHP_LLR_LLVP_TBL_DATA
      ihp_llr_llvp_tbl_data;

    SOC_PETRA_CLEAR(&ihp_llr_llvp_tbl_data, SOC_PB_PP_IHP_LLR_LLVP_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_llr_llvp_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_llr_llvp_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_LL_MIRROR_PROFILE_TBL_DATA
      ihp_ll_mirror_profile_tbl_data;

    SOC_PETRA_CLEAR(&ihp_ll_mirror_profile_tbl_data, SOC_PB_PP_IHP_LL_MIRROR_PROFILE_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_ll_mirror_profile_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_ll_mirror_profile_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_SUBNET_CLASSIFY_TBL_DATA
      ihp_subnet_classify_tbl_data;

    SOC_PETRA_CLEAR(&ihp_subnet_classify_tbl_data, SOC_PB_PP_IHP_SUBNET_CLASSIFY_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_subnet_classify_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_subnet_classify_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_PORT_PROTOCOL_TBL_DATA
      ihp_port_protocol_tbl_data;

    SOC_PETRA_CLEAR(&ihp_port_protocol_tbl_data, SOC_PB_PP_IHP_PORT_PROTOCOL_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_port_protocol_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_port_protocol_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_TOS_2_COS_TBL_DATA
      ihp_tos_2_cos_tbl_data;

    SOC_PETRA_CLEAR(&ihp_tos_2_cos_tbl_data, SOC_PB_PP_IHP_TOS_2_COS_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_tos_2_cos_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_tos_2_cos_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_RESERVED_MC_TBL_DATA
      ihp_reserved_mc_tbl_data;

    SOC_PETRA_CLEAR(&ihp_reserved_mc_tbl_data, SOC_PB_PP_IHP_RESERVED_MC_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_reserved_mc_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_reserved_mc_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_FLUSH_DB_TBL_DATA
      ihp_flush_db_tbl_data;

    SOC_PETRA_CLEAR(&ihp_flush_db_tbl_data, SOC_PB_PP_IHP_FLUSH_DB_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_flush_db_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_flush_db_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_DB_TBL_DATA
      ihp_large_em_fid_counter_db_tbl_data;

    SOC_PETRA_CLEAR(&ihp_large_em_fid_counter_db_tbl_data, SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_DB_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_large_em_fid_counter_db_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_large_em_fid_counter_db_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_PROFILE_DB_TBL_DATA
      ihp_large_em_fid_counter_profile_db_tbl_data;

    SOC_PETRA_CLEAR(&ihp_large_em_fid_counter_profile_db_tbl_data, SOC_PB_PP_IHP_LARGE_EM_FID_COUNTER_PROFILE_DB_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_large_em_fid_counter_profile_db_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_large_em_fid_counter_profile_db_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 190, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_LARGE_EM_AGING_CONFIGURATION_TABLE_TBL_DATA
      ihp_large_em_aging_configuration_table_tbl_data;

    SOC_PETRA_CLEAR(&ihp_large_em_aging_configuration_table_tbl_data, SOC_PB_PP_IHP_LARGE_EM_AGING_CONFIGURATION_TABLE_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_large_em_aging_configuration_table_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_large_em_aging_configuration_table_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_TM_PORT_PP_CONTEXT_CONFIG_TBL_DATA
      ihp_tm_port_pp_context_config_tbl_data;

    SOC_PETRA_CLEAR(&ihp_tm_port_pp_context_config_tbl_data, SOC_PB_PP_IHP_TM_PORT_PP_CONTEXT_CONFIG_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_tm_port_pp_context_config_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_tm_port_pp_context_config_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 230, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_PP_PORT_INFO_TBL_DATA
      ihp_pp_port_info_tbl_data;

    SOC_PETRA_CLEAR(&ihp_pp_port_info_tbl_data, SOC_PB_PP_IHP_PP_PORT_INFO_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_pp_port_info_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_pp_port_info_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 240, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_PP_PORT_VALUES_TBL_DATA
      ihp_pp_port_values_tbl_data;

    SOC_PETRA_CLEAR(&ihp_pp_port_values_tbl_data, SOC_PB_PP_IHP_PP_PORT_VALUES_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_pp_port_values_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_pp_port_values_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 250, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_PP_PORT_FEM_BIT_SELECT_TABLE_TBL_DATA
      ihp_pp_port_fem_bit_select_table_tbl_data;

    SOC_PETRA_CLEAR(&ihp_pp_port_fem_bit_select_table_tbl_data, SOC_PB_PP_IHP_PP_PORT_FEM_BIT_SELECT_TABLE_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_pp_port_fem_bit_select_table_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_pp_port_fem_bit_select_table_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 260, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_PP_PORT_FEM_MAP_INDEX_TABLE_TBL_DATA
      ihp_pp_port_fem_map_index_table_tbl_data;

    SOC_PETRA_CLEAR(&ihp_pp_port_fem_map_index_table_tbl_data, SOC_PB_PP_IHP_PP_PORT_FEM_MAP_INDEX_TABLE_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_pp_port_fem_map_index_table_tbl_set_unsafe(
            unit,
            0,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_pp_port_fem_map_index_table_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 270, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_PP_PORT_FEM_FIELD_SELECT_MAP_TBL_DATA
      ihp_pp_port_fem_field_select_map_tbl_data;

    SOC_PETRA_CLEAR(&ihp_pp_port_fem_field_select_map_tbl_data, SOC_PB_PP_IHP_PP_PORT_FEM_FIELD_SELECT_MAP_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_pp_port_fem_field_select_map_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_pp_port_fem_field_select_map_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 280, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_DATA
      ihp_src_system_port_fem_bit_select_table_tbl_data;

    SOC_PETRA_CLEAR(&ihp_src_system_port_fem_bit_select_table_tbl_data, SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_BIT_SELECT_TABLE_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_src_system_port_fem_bit_select_table_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_src_system_port_fem_bit_select_table_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 290, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_DATA
      ihp_src_system_port_fem_map_index_table_tbl_data;

    SOC_PETRA_CLEAR(&ihp_src_system_port_fem_map_index_table_tbl_data, SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_MAP_INDEX_TABLE_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_src_system_port_fem_map_index_table_tbl_set_unsafe(
            unit,
            0,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_src_system_port_fem_map_index_table_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 300, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_DATA
      ihp_src_system_port_fem_field_select_map_tbl_data;

    SOC_PETRA_CLEAR(&ihp_src_system_port_fem_field_select_map_tbl_data, SOC_PB_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_src_system_port_fem_field_select_map_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_src_system_port_fem_field_select_map_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 310, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_PP_CONTEXT_FEM_BIT_SELECT_TABLE_TBL_DATA
      ihp_pp_context_fem_bit_select_table_tbl_data;

    SOC_PETRA_CLEAR(&ihp_pp_context_fem_bit_select_table_tbl_data, SOC_PB_PP_IHP_PP_CONTEXT_FEM_BIT_SELECT_TABLE_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_pp_context_fem_bit_select_table_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_pp_context_fem_bit_select_table_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 320, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_PP_CONTEXT_FEM_MAP_INDEX_TABLE_TBL_DATA
      ihp_pp_context_fem_map_index_table_tbl_data;

    SOC_PETRA_CLEAR(&ihp_pp_context_fem_map_index_table_tbl_data, SOC_PB_PP_IHP_PP_CONTEXT_FEM_MAP_INDEX_TABLE_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_pp_context_fem_map_index_table_tbl_set_unsafe(
            unit,
            0,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_pp_context_fem_map_index_table_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 330, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_PP_CONTEXT_FEM_FIELD_SELECT_MAP_TBL_DATA
      ihp_pp_context_fem_field_select_map_tbl_data;

    SOC_PETRA_CLEAR(&ihp_pp_context_fem_field_select_map_tbl_data, SOC_PB_PP_IHP_PP_CONTEXT_FEM_FIELD_SELECT_MAP_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_pp_context_fem_field_select_map_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_pp_context_fem_field_select_map_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 340, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_PARSER_PROGRAM1_TBL_DATA
      ihp_parser_program1_tbl_data;

    SOC_PETRA_CLEAR(&ihp_parser_program1_tbl_data, SOC_PB_PP_IHP_PARSER_PROGRAM1_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_parser_program1_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_parser_program1_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 350, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_PARSER_PROGRAM2_TBL_DATA
      ihp_parser_program2_tbl_data;

    SOC_PETRA_CLEAR(&ihp_parser_program2_tbl_data, SOC_PB_PP_IHP_PARSER_PROGRAM2_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_parser_program2_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_parser_program2_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 360, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_PACKET_FORMAT_TABLE_TBL_DATA
      ihp_packet_format_table_tbl_data;

    SOC_PETRA_CLEAR(&ihp_packet_format_table_tbl_data, SOC_PB_PP_IHP_PACKET_FORMAT_TABLE_TBL_DATA, 1);
    ihp_packet_format_table_tbl_data.packet_format_code = 0x3f;
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_packet_format_table_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_packet_format_table_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 370, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_DATA
      ihp_parser_custom_macro_parameters_tbl_data;

    SOC_PETRA_CLEAR(&ihp_parser_custom_macro_parameters_tbl_data, SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PARAMETERS_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_parser_custom_macro_parameters_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_parser_custom_macro_parameters_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 380, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_DATA
      ihp_parser_eth_protocols_tbl_data;

    SOC_PETRA_CLEAR(&ihp_parser_eth_protocols_tbl_data, SOC_PB_PP_IHP_PARSER_ETH_PROTOCOLS_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_parser_eth_protocols_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_parser_eth_protocols_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 390, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_PARSER_IP_PROTOCOLS_TBL_DATA
      ihp_parser_ip_protocols_tbl_data;

    SOC_PETRA_CLEAR(&ihp_parser_ip_protocols_tbl_data, SOC_PB_PP_IHP_PARSER_IP_PROTOCOLS_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_parser_ip_protocols_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_parser_ip_protocols_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 400, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_DATA
      ihp_parser_custom_macro_protocols_tbl_data;

    SOC_PETRA_CLEAR(&ihp_parser_custom_macro_protocols_tbl_data, SOC_PB_PP_IHP_PARSER_CUSTOM_MACRO_PROTOCOLS_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_parser_custom_macro_protocols_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_parser_custom_macro_protocols_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 410, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_VSI_ISID_TBL_DATA
      ihp_vsi_isid_tbl_data;

    SOC_PETRA_CLEAR(&ihp_vsi_isid_tbl_data, SOC_PB_PP_IHP_VSI_ISID_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_vsi_isid_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_vsi_isid_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 430, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_VSI_MY_MAC_TBL_DATA
      ihp_vsi_my_mac_tbl_data;

    SOC_PETRA_CLEAR(&ihp_vsi_my_mac_tbl_data, SOC_PB_PP_IHP_VSI_MY_MAC_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_vsi_my_mac_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_vsi_my_mac_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 440, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_VSI_TOPOLOGY_ID_TBL_DATA
      ihp_vsi_topology_id_tbl_data;

    SOC_PETRA_CLEAR(&ihp_vsi_topology_id_tbl_data, SOC_PB_PP_IHP_VSI_TOPOLOGY_ID_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_vsi_topology_id_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_vsi_topology_id_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 450, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_VSI_FID_CLASS_TBL_DATA
      ihp_vsi_fid_class_tbl_data;

    SOC_PETRA_CLEAR(&ihp_vsi_fid_class_tbl_data, SOC_PB_PP_IHP_VSI_FID_CLASS_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_vsi_fid_class_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_vsi_fid_class_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 460, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_VSI_DA_NOT_FOUND_TBL_DATA
      ihp_vsi_da_not_found_tbl_data;

    SOC_PETRA_CLEAR(&ihp_vsi_da_not_found_tbl_data, SOC_PB_PP_IHP_VSI_DA_NOT_FOUND_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_vsi_da_not_found_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_vsi_da_not_found_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 470, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_BVD_TOPOLOGY_ID_TBL_DATA
      ihp_bvd_topology_id_tbl_data;

    SOC_PETRA_CLEAR(&ihp_bvd_topology_id_tbl_data, SOC_PB_PP_IHP_BVD_TOPOLOGY_ID_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_bvd_topology_id_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_bvd_topology_id_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 480, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_BVD_FID_CLASS_TBL_DATA
      ihp_bvd_fid_class_tbl_data;

    SOC_PETRA_CLEAR(&ihp_bvd_fid_class_tbl_data, SOC_PB_PP_IHP_BVD_FID_CLASS_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_bvd_fid_class_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_bvd_fid_class_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 490, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_BVD_DA_NOT_FOUND_TBL_DATA
      ihp_bvd_da_not_found_tbl_data;

    SOC_PETRA_CLEAR(&ihp_bvd_da_not_found_tbl_data, SOC_PB_PP_IHP_BVD_DA_NOT_FOUND_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_bvd_da_not_found_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_bvd_da_not_found_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 500, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_FID_CLASS_2_FID_TBL_DATA
      ihp_fid_class_2_fid_tbl_data;

    SOC_PETRA_CLEAR(&ihp_fid_class_2_fid_tbl_data, SOC_PB_PP_IHP_FID_CLASS_2_FID_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_fid_class_2_fid_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_fid_class_2_fid_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 510, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_DATA
      ihp_vlan_range_compression_table_tbl_data;

    SOC_PETRA_CLEAR(&ihp_vlan_range_compression_table_tbl_data, SOC_PB_PP_IHP_VLAN_RANGE_COMPRESSION_TABLE_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_vlan_range_compression_table_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_vlan_range_compression_table_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 520, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_DATA
      ihp_vtt_in_pp_port_vlan_config_tbl_data;

    SOC_PETRA_CLEAR(&ihp_vtt_in_pp_port_vlan_config_tbl_data, SOC_PB_PP_IHP_VTT_IN_PP_PORT_VLAN_CONFIG_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_vtt_in_pp_port_vlan_config_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_vtt_in_pp_port_vlan_config_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 530, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_DATA
      ihp_designated_vlan_table_tbl_data;

    SOC_PETRA_CLEAR(&ihp_designated_vlan_table_tbl_data, SOC_PB_PP_IHP_DESIGNATED_VLAN_TABLE_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_designated_vlan_table_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_designated_vlan_table_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 540, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_VSI_PORT_MEMBERSHIP_TBL_DATA
      ihp_vsi_port_membership_tbl_data;

    SOC_PETRA_CLEAR(&ihp_vsi_port_membership_tbl_data, SOC_PB_PP_IHP_VSI_PORT_MEMBERSHIP_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_vsi_port_membership_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_vsi_port_membership_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 550, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_DATA
      ihp_vtt_in_pp_port_config_tbl_data;

    SOC_PETRA_CLEAR(&ihp_vtt_in_pp_port_config_tbl_data, SOC_PB_PP_IHP_VTT_IN_PP_PORT_CONFIG_TBL_DATA, 1);
    /* no opcode by default */
    ihp_vtt_in_pp_port_config_tbl_data.default_sem_opcode = 3;
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_vtt_in_pp_port_config_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_vtt_in_pp_port_config_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 560, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_SEM_RESULT_TABLE_TBL_DATA
      ihp_sem_result_table_tbl_data;

    SOC_PETRA_CLEAR(&ihp_sem_result_table_tbl_data, SOC_PB_PP_IHP_SEM_RESULT_TABLE_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_sem_result_table_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_sem_result_table_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 570, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_DATA
      ihp_sem_opcode_ip_offsets_tbl_data;

    SOC_PETRA_CLEAR(&ihp_sem_opcode_ip_offsets_tbl_data, SOC_PB_PP_IHP_SEM_OPCODE_IP_OFFSETS_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_sem_opcode_ip_offsets_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_sem_opcode_ip_offsets_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 700, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_DATA
      ihp_sem_opcode_tc_dp_offsets_tbl_data;

    SOC_PETRA_CLEAR(&ihp_sem_opcode_tc_dp_offsets_tbl_data, SOC_PB_PP_IHP_SEM_OPCODE_TC_DP_OFFSETS_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_sem_opcode_tc_dp_offsets_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_sem_opcode_tc_dp_offsets_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 710, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_DATA
      ihp_sem_opcode_pcp_dei_offsets_tbl_data;

    SOC_PETRA_CLEAR(&ihp_sem_opcode_pcp_dei_offsets_tbl_data, SOC_PB_PP_IHP_SEM_OPCODE_PCP_DEI_OFFSETS_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_sem_opcode_pcp_dei_offsets_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_sem_opcode_pcp_dei_offsets_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 720, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_STP_TABLE_TBL_DATA
      ihp_stp_table_tbl_data;

    SOC_PETRA_CLEAR(&ihp_stp_table_tbl_data, SOC_PB_PP_IHP_STP_TABLE_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_stp_table_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_stp_table_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 730, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_VRID_MY_MAC_MAP_TBL_DATA
      ihp_vrid_my_mac_map_tbl_data;

    SOC_PETRA_CLEAR(&ihp_vrid_my_mac_map_tbl_data, SOC_PB_PP_IHP_VRID_MY_MAC_MAP_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_vrid_my_mac_map_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_vrid_my_mac_map_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 740, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_MPLS_LABEL_RANGE_ENCOUNTERED_TBL_DATA
      ihp_mpls_label_range_encountered_tbl_data;

    SOC_PETRA_CLEAR(&ihp_mpls_label_range_encountered_tbl_data, SOC_PB_PP_IHP_MPLS_LABEL_RANGE_ENCOUNTERED_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_mpls_label_range_encountered_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_mpls_label_range_encountered_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 750, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_MPLS_TUNNEL_TERMINATION_VALID_TBL_DATA
      ihp_mpls_tunnel_termination_valid_tbl_data;

    SOC_PETRA_CLEAR(&ihp_mpls_tunnel_termination_valid_tbl_data, SOC_PB_PP_IHP_MPLS_TUNNEL_TERMINATION_VALID_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_mpls_tunnel_termination_valid_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_mpls_tunnel_termination_valid_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 760, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_IP_OVER_MPLS_EXP_MAPPING_TBL_DATA
      ihp_ip_over_mpls_exp_mapping_tbl_data;

    SOC_PETRA_CLEAR(&ihp_ip_over_mpls_exp_mapping_tbl_data, SOC_PB_PP_IHP_IP_OVER_MPLS_EXP_MAPPING_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_ip_over_mpls_exp_mapping_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_ip_over_mpls_exp_mapping_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 770, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_VTT_LLVP_TBL_DATA
      ihp_vtt_llvp_tbl_data;

    SOC_PETRA_CLEAR(&ihp_vtt_llvp_tbl_data, SOC_PB_PP_IHP_VTT_LLVP_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_vtt_llvp_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_vtt_llvp_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 780, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_LLVP_PROG_SEL_TBL_DATA
      ihp_llvp_prog_sel_tbl_data;

    SOC_PETRA_CLEAR(&ihp_llvp_prog_sel_tbl_data, SOC_PB_PP_IHP_LLVP_PROG_SEL_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_llvp_prog_sel_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_llvp_prog_sel_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 790, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_DATA
      ihp_vtt1st_key_construction0_tbl_data;

    SOC_PETRA_CLEAR(&ihp_vtt1st_key_construction0_tbl_data, SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_vtt1st_key_construction0_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_vtt1st_key_construction0_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 800, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_DATA
      ihp_vtt1st_key_construction1_tbl_data;

    SOC_PETRA_CLEAR(&ihp_vtt1st_key_construction1_tbl_data, SOC_PB_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_vtt1st_key_construction1_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_vtt1st_key_construction1_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 810, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_DATA
      ihp_vtt2nd_key_construction_tbl_data;

    SOC_PETRA_CLEAR(&ihp_vtt2nd_key_construction_tbl_data, SOC_PB_PP_IHP_VTT2ND_KEY_CONSTRUCTION_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_vtt2nd_key_construction_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_vtt2nd_key_construction_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 820, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_DATA
      ihp_ingress_vlan_edit_command_table_tbl_data;

    SOC_PETRA_CLEAR(&ihp_ingress_vlan_edit_command_table_tbl_data, SOC_PB_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_ingress_vlan_edit_command_table_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_ingress_vlan_edit_command_table_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 830, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_DATA
      ihp_vlan_edit_pcp_dei_map_tbl_data;

    SOC_PETRA_CLEAR(&ihp_vlan_edit_pcp_dei_map_tbl_data, SOC_PB_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_vlan_edit_pcp_dei_map_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_vlan_edit_pcp_dei_map_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 840, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_PBB_CFM_TABLE_TBL_DATA
      ihp_pbb_cfm_table_tbl_data;

    SOC_PETRA_CLEAR(&ihp_pbb_cfm_table_tbl_data, SOC_PB_PP_IHP_PBB_CFM_TABLE_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_pbb_cfm_table_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_pbb_cfm_table_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 850, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_SEM_RESULT_ACCESSED_TBL_DATA
      ihp_sem_result_accessed_tbl_data;

    SOC_PETRA_CLEAR(&ihp_sem_result_accessed_tbl_data, SOC_PB_PP_IHP_SEM_RESULT_ACCESSED_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_sem_result_accessed_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_sem_result_accessed_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 860, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_DATA
      ihp_in_rif_config_table_tbl_data;

    SOC_PETRA_CLEAR(&ihp_in_rif_config_table_tbl_data, SOC_PB_PP_IHP_IN_RIF_CONFIG_TABLE_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_in_rif_config_table_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_in_rif_config_table_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 870, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_DATA
      ihp_tc_dp_map_table_tbl_data;

    SOC_PETRA_CLEAR(&ihp_tc_dp_map_table_tbl_data, SOC_PB_PP_IHP_TC_DP_MAP_TABLE_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_tc_dp_map_table_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_tc_dp_map_table_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 880, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHP_PORT_MINE_TABLE_PHYSICAL_PORT_TBL_DATA
      ihp_port_mine_table_physical_port_tbl_data;

    SOC_PETRA_CLEAR(&ihp_port_mine_table_physical_port_tbl_data, SOC_PB_PP_IHP_PORT_MINE_TABLE_PHYSICAL_PORT_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_port_mine_table_physical_port_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_port_mine_table_physical_port_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 890, exit);
  } while(0);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_mgmt_ihp_tbls_init_unsafe()",0,0);
}

uint32
  soc_pb_pp_mgmt_ihb_tbls_init_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    map_tbl_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MGMT_IHB_TBLS_INIT);

  do
  {
    SOC_PB_PP_IHB_PINFO_FER_TBL_DATA
      ihb_pinfo_fer_tbl_data;

    SOC_PETRA_CLEAR(&ihb_pinfo_fer_tbl_data, SOC_PB_PP_IHB_PINFO_FER_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihb_pinfo_fer_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihb_pinfo_fer_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHB_ECMP_GROUP_SIZE_TBL_DATA
      ihb_ecmp_group_size_tbl_data;

    SOC_PETRA_CLEAR(&ihb_ecmp_group_size_tbl_data, SOC_PB_PP_IHB_ECMP_GROUP_SIZE_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihb_ecmp_group_size_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihb_ecmp_group_size_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHB_LB_PFC_PROFILE_TBL_DATA
      ihb_lb_pfc_profile_tbl_data;

    SOC_PETRA_CLEAR(&ihb_lb_pfc_profile_tbl_data, SOC_PB_PP_IHB_LB_PFC_PROFILE_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihb_lb_pfc_profile_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihb_lb_pfc_profile_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_DATA
      ihb_lb_vector_program_map_tbl_data;

    SOC_PETRA_CLEAR(&ihb_lb_vector_program_map_tbl_data, SOC_PB_PP_IHB_LB_VECTOR_PROGRAM_MAP_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihb_lb_vector_program_map_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihb_lb_vector_program_map_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHB_FEC_SUPER_ENTRY_TBL_DATA
      ihb_fec_super_entry_tbl_data;

    SOC_PETRA_CLEAR(&ihb_fec_super_entry_tbl_data, SOC_PB_PP_IHB_FEC_SUPER_ENTRY_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihb_fec_super_entry_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihb_fec_super_entry_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHB_FEC_ENTRY_TBL_DATA
      ihb_fec_entry_tbl_data;

    SOC_PETRA_CLEAR(&ihb_fec_entry_tbl_data, SOC_PB_PP_IHB_FEC_ENTRY_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihb_fec_entry_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihb_fec_entry_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  } while(0);


  do
  {
    SOC_PB_PP_IHB_PATH_SELECT_TBL_DATA
      ihb_path_select_tbl_data;

    SOC_PETRA_CLEAR(&ihb_path_select_tbl_data, SOC_PB_PP_IHB_PATH_SELECT_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihb_path_select_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihb_path_select_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHB_DESTINATION_STATUS_TBL_DATA
      ihb_destination_status_tbl_data;

    SOC_PETRA_CLEAR(&ihb_destination_status_tbl_data, SOC_PB_PP_IHB_DESTINATION_STATUS_TBL_DATA, 1);
    /* all destination are valid */
    ihb_destination_status_tbl_data.destination_valid = 0xff;
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihb_destination_status_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihb_destination_status_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_DATA
      ihb_fwd_act_profile_tbl_data;

    SOC_PETRA_CLEAR(&ihb_fwd_act_profile_tbl_data, SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihb_fwd_act_profile_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihb_fwd_act_profile_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHB_SNOOP_ACTION_TBL_DATA
      ihb_snoop_action_tbl_data;

    SOC_PETRA_CLEAR(&ihb_snoop_action_tbl_data, SOC_PB_PP_IHB_SNOOP_ACTION_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihb_snoop_action_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihb_snoop_action_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHB_PINFO_FLP_TBL_DATA
      ihb_pinfo_flp_tbl_data;

    SOC_PETRA_CLEAR(&ihb_pinfo_flp_tbl_data, SOC_PB_PP_IHB_PINFO_FLP_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihb_pinfo_flp_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihb_pinfo_flp_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHB_PROGRAM_TRANSLATION_MAP_TBL_DATA
      ihb_program_translation_map_tbl_data;

    SOC_PETRA_CLEAR(&ihb_program_translation_map_tbl_data, SOC_PB_PP_IHB_PROGRAM_TRANSLATION_MAP_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihb_program_translation_map_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihb_program_translation_map_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL_DATA
      ihb_flp_key_program_map_tbl_data;

    SOC_PETRA_CLEAR(&ihb_flp_key_program_map_tbl_data, SOC_PB_PP_IHB_FLP_KEY_PROGRAM_MAP_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihb_flp_key_program_map_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihb_flp_key_program_map_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHB_UNKNOWN_DA_ACTION_PROFILES_TBL_DATA
      ihb_unknown_da_action_profiles_tbl_data;

    SOC_PETRA_CLEAR(&ihb_unknown_da_action_profiles_tbl_data, SOC_PB_PP_IHB_UNKNOWN_DA_ACTION_PROFILES_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihb_unknown_da_action_profiles_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihb_unknown_da_action_profiles_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 190, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHB_VRF_CONFIG_TBL_DATA
      ihb_vrf_config_tbl_data;

    SOC_PETRA_CLEAR(&ihb_vrf_config_tbl_data, SOC_PB_PP_IHB_VRF_CONFIG_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihb_vrf_config_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihb_vrf_config_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHB_HEADER_PROFILE_TBL_DATA
      ihb_header_profile_tbl_data;

    SOC_PETRA_CLEAR(&ihb_header_profile_tbl_data, SOC_PB_PP_IHB_HEADER_PROFILE_TBL_DATA, 1);
    /*
     * Set the default configuration for Raw (or TM) Ports
     */
    ihb_header_profile_tbl_data.header_profile_build_ftmh = 0x1;
    ihb_header_profile_tbl_data.header_profile_learn_disable = 0x1;
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihb_header_profile_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihb_header_profile_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 210, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHB_SNP_ACT_PROFILE_TBL_DATA
      ihb_snp_act_profile_tbl_data;

    SOC_PETRA_CLEAR(&ihb_snp_act_profile_tbl_data, SOC_PB_PP_IHB_SNP_ACT_PROFILE_TBL_DATA, 1);
    /* all snoop probabality are 100% */
    ihb_snp_act_profile_tbl_data.snp_act_snp_sampling_probability = 1023;
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihb_snp_act_profile_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihb_snp_act_profile_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 220, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHB_MRR_ACT_PROFILE_TBL_DATA
      ihb_mrr_act_profile_tbl_data;

    SOC_PETRA_CLEAR(&ihb_mrr_act_profile_tbl_data, SOC_PB_PP_IHB_MRR_ACT_PROFILE_TBL_DATA, 1);
    /* all mirror probabality are 100% */
    ihb_mrr_act_profile_tbl_data.mrr_act_mrr_sampling_probability = 1023;
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihb_mrr_act_profile_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihb_mrr_act_profile_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 230, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHB_LPM_TBL_DATA
      ihb_lpm_tbl_data;

    SOC_PETRA_CLEAR(&ihb_lpm_tbl_data, SOC_PB_PP_IHB_LPM_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihb_lpm_tbl_set_unsafe(
            unit,
            1,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihb_lpm_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 240, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHB_LPM_TBL_DATA
      ihb_lpm_tbl_data;

    SOC_PETRA_CLEAR(&ihb_lpm_tbl_data, SOC_PB_PP_IHB_LPM_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihb_lpm_tbl_set_unsafe(
            unit,
            2,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihb_lpm_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 250, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHB_LPM_TBL_DATA
      ihb_lpm_tbl_data;

    SOC_PETRA_CLEAR(&ihb_lpm_tbl_data, SOC_PB_PP_IHB_LPM_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihb_lpm_tbl_set_unsafe(
            unit,
            3,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihb_lpm_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 260, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHB_LPM_TBL_DATA
      ihb_lpm_tbl_data;

    SOC_PETRA_CLEAR(&ihb_lpm_tbl_data, SOC_PB_PP_IHB_LPM_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihb_lpm_tbl_set_unsafe(
            unit,
            4,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihb_lpm_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 270, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHB_LPM_TBL_DATA
      ihb_lpm_tbl_data;

    SOC_PETRA_CLEAR(&ihb_lpm_tbl_data, SOC_PB_PP_IHB_LPM_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihb_lpm_tbl_set_unsafe(
            unit,
            5,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihb_lpm_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 280, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHB_LPM_TBL_DATA
      ihb_lpm_tbl_data;

    SOC_PETRA_CLEAR(&ihb_lpm_tbl_data, SOC_PB_PP_IHB_LPM_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihb_lpm_tbl_set_unsafe(
            unit,
            6,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihb_lpm_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 290, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHB_PINFO_PMF_KEY_GEN_VAR_TBL_DATA
      ihb_pinfo_pmf_key_gen_var_tbl_data;

    SOC_PETRA_CLEAR(&ihb_pinfo_pmf_key_gen_var_tbl_data, SOC_PB_PP_IHB_PINFO_PMF_KEY_GEN_VAR_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihb_pinfo_pmf_key_gen_var_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihb_pinfo_pmf_key_gen_var_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 300, exit);
  } while(0);

  for (map_tbl_ndx = 0; map_tbl_ndx < SOC_PB_NOF_PMF_PGM_SEL_TYPES; ++map_tbl_ndx)
  {
    do
    {
      SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA
        ihb_program_selection_map_tbl_data;

      SOC_PETRA_CLEAR(&ihb_program_selection_map_tbl_data, SOC_PB_PP_IHB_PROGRAM_SELECTION_MAP_TBL_DATA, 1);
      soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
      /*
       *	Init all the PMF program selection table to 0x7FFFFFFF
       */
      ihb_program_selection_map_tbl_data.program_selection_map = 0x7FFFFFFF;
      res = soc_pb_pp_ihb_program_selection_map_tbl_set_unsafe(
              unit,
              map_tbl_ndx,
              SOC_PB_PP_FIRST_TBL_ENTRY,
              &ihb_program_selection_map_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 310, exit);
    } while(0);
  }

  do
  {
    SOC_PB_PP_IHB_PROGRAM_VARS_TABLE_TBL_DATA
      ihb_program_vars_table_tbl_data;

    SOC_PETRA_CLEAR(&ihb_program_vars_table_tbl_data, SOC_PB_PP_IHB_PROGRAM_VARS_TABLE_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihb_program_vars_table_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihb_program_vars_table_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 320, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHB_PROGRAM_INSTRUCTION_TABLE_TBL_DATA
      ihb_program_instruction_table_tbl_data;

    SOC_PETRA_CLEAR(&ihb_program_instruction_table_tbl_data, SOC_PB_PP_IHB_PROGRAM_INSTRUCTION_TABLE_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihb_program_instruction_table_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihb_program_instruction_table_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 330, exit);
  } while(0);

  for (map_tbl_ndx = 0; map_tbl_ndx < SOC_PB_PP_IHB_PROGRAM_INSTRUCTION_TABLE_NOF_TBLS; ++map_tbl_ndx)
  {
    do
    {
      SOC_PB_PP_IHB_KEY_AB_PROGRAM_INSTRUCTION_TABLE_TBL_DATA
        ihb_key_ab_program_instruction_table_tbl_data;

      SOC_PETRA_CLEAR(&ihb_key_ab_program_instruction_table_tbl_data, SOC_PB_PP_IHB_KEY_AB_PROGRAM_INSTRUCTION_TABLE_TBL_DATA, 1);
      soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
      res = soc_pb_pp_ihb_key_a_program_instruction_table_tbl_set_unsafe(
              unit,
              map_tbl_ndx,
              SOC_PB_PP_FIRST_TBL_ENTRY,
              &ihb_key_ab_program_instruction_table_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 340, exit);
      soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
      res = soc_pb_pp_ihb_key_b_program_instruction_table_tbl_set_unsafe(
              unit,
              map_tbl_ndx,
              SOC_PB_PP_FIRST_TBL_ENTRY,
              &ihb_key_ab_program_instruction_table_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 350, exit);
    } while(0);
  }

  do
  {
    SOC_PB_PP_IHB_TCAM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA
      ihb_tcam_1st_pass_key_profile_resolved_data_tbl_data;

    SOC_PETRA_CLEAR(&ihb_tcam_1st_pass_key_profile_resolved_data_tbl_data, SOC_PB_PP_IHB_TCAM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihb_tcam_1st_pass_key_profile_resolved_data_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihb_tcam_1st_pass_key_profile_resolved_data_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 360, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHB_TCAM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA
      ihb_tcam_2nd_pass_key_profile_resolved_data_tbl_data;

    SOC_PETRA_CLEAR(&ihb_tcam_2nd_pass_key_profile_resolved_data_tbl_data, SOC_PB_PP_IHB_TCAM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihb_tcam_2nd_pass_key_profile_resolved_data_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihb_tcam_2nd_pass_key_profile_resolved_data_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 370, exit);
  } while(0);

  do
  {
    SOC_PB_PP_IHB_DIRECT_ACTION_TABLE_TBL_DATA
      ihb_direct_action_table_tbl_data;

    SOC_PETRA_CLEAR(&ihb_direct_action_table_tbl_data, SOC_PB_PP_IHB_DIRECT_ACTION_TABLE_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihb_direct_action_table_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihb_direct_action_table_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 380, exit);
  } while(0);
/* $Id: pb_pp_init_tbl.c,v 1.7 Broadcom SDK $
  do
  {
    SOC_PB_PP_IHB_TCAM_TBL_SET_DATA
      ihb_tcam_bank_tbl_data;

    SOC_PETRA_CLEAR(&ihb_tcam_bank_tbl_data, SOC_PB_PP_IHB_TCAM_TBL_SET_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihb_tcam_bank_tbl_set_unsafe(
            unit,
            0,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihb_tcam_bank_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 390, exit);
  } while(0);
*/
  do
  {
    SOC_PB_PP_IHB_TCAM_ACTION_TBL_DATA
      ihb_tcam_action_tbl_data;

    SOC_PETRA_CLEAR(&ihb_tcam_action_tbl_data, SOC_PB_PP_IHB_TCAM_ACTION_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihb_tcam_action_tbl_set_unsafe(
            unit,
            0,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihb_tcam_action_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 400, exit);
  } while(0);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_mgmt_ihb_tbls_init_unsafe()",0,0);
}


STATIC uint32
  soc_pb_pp_mgmt_olp_tbls_init_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MGMT_OLP_TBLS_INIT);

  SOC_PB_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_mgmt_olp_tbls_init_unsafe()",0,0);
}

uint32
  soc_pb_pp_mgmt_egq_tbls_init_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MGMT_EGQ_TBLS_INIT);
  
  do
  {
    SOC_PB_PP_EGQ_PP_PPCT_TBL_DATA
      egq_pp_ppct_tbl_data;

    SOC_PETRA_CLEAR(&egq_pp_ppct_tbl_data, SOC_PB_PP_EGQ_PP_PPCT_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_egq_pp_ppct_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &egq_pp_ppct_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  } while(0);

  do
  {
    SOC_PB_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA
      egq_ingress_vlan_edit_command_map_tbl_data;

    SOC_PETRA_CLEAR(&egq_ingress_vlan_edit_command_map_tbl_data, SOC_PB_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_egq_ingress_vlan_edit_command_map_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &egq_ingress_vlan_edit_command_map_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  } while(0);

  do
  {
    SOC_PB_PP_EGQ_VSI_MEMBERSHIP_TBL_DATA
      egq_vsi_membership_tbl_data;

    SOC_PETRA_CLEAR(&egq_vsi_membership_tbl_data, SOC_PB_PP_EGQ_VSI_MEMBERSHIP_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_egq_vsi_membership_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &egq_vsi_membership_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  } while(0);

  do
  {
    SOC_PB_PP_EGQ_TTL_SCOPE_TBL_DATA
      egq_ttl_scope_tbl_data;

    SOC_PETRA_CLEAR(&egq_ttl_scope_tbl_data, SOC_PB_PP_EGQ_TTL_SCOPE_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_egq_ttl_scope_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &egq_ttl_scope_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  } while(0);

  do
  {
    SOC_PB_PP_EGQ_AUX_TABLE_TBL_DATA
      egq_aux_table_tbl_data;

    SOC_PETRA_CLEAR(&egq_aux_table_tbl_data, SOC_PB_PP_EGQ_AUX_TABLE_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_egq_aux_table_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &egq_aux_table_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  } while(0);

  do
  {
    SOC_PB_PP_EGQ_EEP_ORIENTATION_TBL_DATA
      egq_eep_orientation_tbl_data;

    SOC_PETRA_CLEAR(&egq_eep_orientation_tbl_data, SOC_PB_PP_EGQ_EEP_ORIENTATION_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_egq_eep_orientation_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &egq_eep_orientation_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  } while(0);

  do
  {
    SOC_PB_PP_EGQ_ACTION_PROFILE_TABLE_TBL_DATA
      egq_action_profile_table_tbl_data;

    SOC_PETRA_CLEAR(&egq_action_profile_table_tbl_data, SOC_PB_PP_EGQ_ACTION_PROFILE_TABLE_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_egq_action_profile_table_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &egq_action_profile_table_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
  } while(0);

  do
  {
    SOC_PB_PP_EGQ_CFM_TRAP_TBL_DATA
      egq_cfm_trap_tbl_data;

    SOC_PETRA_CLEAR(&egq_cfm_trap_tbl_data, SOC_PB_PP_EGQ_CFM_TRAP_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_egq_cfm_trap_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &egq_cfm_trap_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 85, exit);
  } while(0);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_mgmt_egq_tbls_init_unsafe()",0,0);
}

uint32
  soc_pb_pp_mgmt_epni_tbls_init_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MGMT_EPNI_TBLS_INIT);

  do
  {
    SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_MPLS_TUNNEL_FORMAT_TBL_DATA
      epni_egress_encapsulation_entry_mpls_tunnel_format_tbl_data;

    SOC_PETRA_CLEAR(&epni_egress_encapsulation_entry_mpls_tunnel_format_tbl_data, SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_MPLS_TUNNEL_FORMAT_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_epni_egress_encapsulation_entry_mpls_tunnel_format_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &epni_egress_encapsulation_entry_mpls_tunnel_format_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  } while(0);

  do
  {
    SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_IP_TUNNEL_FORMAT_TBL_DATA
      epni_egress_encapsulation_entry_ip_tunnel_format_tbl_data;

    SOC_PETRA_CLEAR(&epni_egress_encapsulation_entry_ip_tunnel_format_tbl_data, SOC_PB_PP_EPNI_EGRESS_ENCAPSULATION_ENTRY_IP_TUNNEL_FORMAT_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_epni_egress_encapsulation_entry_ip_tunnel_format_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &epni_egress_encapsulation_entry_ip_tunnel_format_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  } while(0);

  do
  {
    SOC_PB_PP_EPNI_ENCAPSULATION_MEMORY_LINK_LAYER_ENTRY_FORMAT_TBL_DATA
      epni_encapsulation_memory_link_layer_entry_format_tbl_data;

    SOC_PETRA_CLEAR(&epni_encapsulation_memory_link_layer_entry_format_tbl_data, SOC_PB_PP_EPNI_ENCAPSULATION_MEMORY_LINK_LAYER_ENTRY_FORMAT_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_epni_encapsulation_memory_link_layer_entry_format_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &epni_encapsulation_memory_link_layer_entry_format_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  } while(0);

  do
  {
    SOC_PB_PP_EPNI_TX_TAG_TABLE_TBL_DATA
      epni_tx_tag_table_tbl_data;

    SOC_PETRA_CLEAR(&epni_tx_tag_table_tbl_data, SOC_PB_PP_EPNI_TX_TAG_TABLE_TBL_DATA, 1);
    /* by default don't remove tag set by EVE*/
    epni_tx_tag_table_tbl_data.entry[0] = 0xffffffff;
    epni_tx_tag_table_tbl_data.entry[1] = 0xffffffff;
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_epni_tx_tag_table_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &epni_tx_tag_table_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  } while(0);

  do
  {
    SOC_PB_PP_EPNI_STP_TBL_DATA
      epni_stp_tbl_data;

    SOC_PETRA_CLEAR(&epni_stp_tbl_data, SOC_PB_PP_EPNI_STP_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_epni_stp_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &epni_stp_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  } while(0);

  do
  {
    SOC_PB_PP_EPNI_SMALL_EM_RESULT_MEMORY_TBL_DATA
      epni_small_em_result_memory_tbl_data;

    SOC_PETRA_CLEAR(&epni_small_em_result_memory_tbl_data, SOC_PB_PP_EPNI_SMALL_EM_RESULT_MEMORY_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_epni_small_em_result_memory_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &epni_small_em_result_memory_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
  } while(0);

  do
  {
    SOC_PB_PP_EPNI_PCP_DEI_TABLE_TBL_DATA
      epni_pcp_dei_table_tbl_data;

    SOC_PETRA_CLEAR(&epni_pcp_dei_table_tbl_data, SOC_PB_PP_EPNI_PCP_DEI_TABLE_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_epni_pcp_dei_table_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &epni_pcp_dei_table_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
  } while(0);

  do
  {
    SOC_PB_PP_EPNI_PP_PCT_TBL_DATA
      epni_pp_pct_tbl_data;

    SOC_PETRA_CLEAR(&epni_pp_pct_tbl_data, SOC_PB_PP_EPNI_PP_PCT_TBL_DATA, 1);
    /* disable Filter in EPNI PPCT  */
    epni_pp_pct_tbl_data.disable_filter = 1;
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_epni_pp_pct_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &epni_pp_pct_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
  } while(0);

  do
  {
    SOC_PB_PP_EPNI_LLVP_TABLE_TBL_DATA
      epni_llvp_table_tbl_data;

    SOC_PETRA_CLEAR(&epni_llvp_table_tbl_data, SOC_PB_PP_EPNI_LLVP_TABLE_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_epni_llvp_table_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &epni_llvp_table_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
  } while(0);

  do
  {
    SOC_PB_PP_EPNI_EGRESS_EDIT_CMD_TBL_DATA
      epni_egress_edit_cmd_tbl_data;

    SOC_PETRA_CLEAR(&epni_egress_edit_cmd_tbl_data, SOC_PB_PP_EPNI_EGRESS_EDIT_CMD_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_epni_egress_edit_cmd_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &epni_egress_edit_cmd_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
  } while(0);

  do
  {
    SOC_PB_PP_EPNI_PROGRAM_VARS_TBL_DATA
      epni_program_vars_tbl_data;

    SOC_PETRA_CLEAR(&epni_program_vars_tbl_data, SOC_PB_PP_EPNI_PROGRAM_VARS_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_epni_program_vars_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &epni_program_vars_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);
  } while(0);

  do
  {
    SOC_PB_PP_EPNI_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA
      epni_ingress_vlan_edit_command_map_tbl_data;

    SOC_PETRA_CLEAR(&epni_ingress_vlan_edit_command_map_tbl_data, SOC_PB_PP_EPNI_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_epni_ingress_vlan_edit_command_map_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &epni_ingress_vlan_edit_command_map_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);
  } while(0);

  do
  {
    SOC_PB_PP_EPNI_IP_EXP_MAP_TBL_DATA
      epni_ip_exp_map_tbl_data;

    SOC_PETRA_CLEAR(&epni_ip_exp_map_tbl_data, SOC_PB_PP_EPNI_IP_EXP_MAP_TBL_DATA, 1);
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_epni_ip_exp_map_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &epni_ip_exp_map_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);
  } while(0);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_mgmt_epni_tbls_init_unsafe()",0,0);
}


uint32
  soc_pb_pp_mgmt_tbls_init_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MGMT_TBLS_INIT);

  res = soc_pb_pp_mgmt_eci_tbls_init_unsafe(unit, silent);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_pb_pp_mgmt_ihp_tbls_init_unsafe(unit, silent);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_pb_pp_mgmt_ihb_tbls_init_unsafe(unit, silent);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_mgmt_olp_tbls_init_unsafe(unit, silent);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_pb_pp_mgmt_egq_tbls_init_unsafe(unit, silent);
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_pb_pp_mgmt_epni_tbls_init_unsafe(unit, silent);
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  res = soc_pb_pp_tbl_access_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_mgmt_tbls_init_unsafe()",0,0);
}


/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

