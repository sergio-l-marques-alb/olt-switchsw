/* $Id: ui_pb_api_cnm.c,v 1.8 Broadcom SDK $
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
/* 
 * Utilities include file. 
 */ 
 
#include <soc/dpp/SAND/Utils/sand_os_interface.h> 
#include <appl/diag/dpp/utils_defi.h> 
#include <appl/dpp/UserInterface/ui_defi.h> 
 
#include <appl/dpp/UserInterface/ui_pure_defi_pb_api.h> 
#include <appl/dpp/UserInterface/ui_pure_defi_pb_pp_acc.h>
  
#include <soc/dpp/Petra/PB_TM/pb_api_cnm.h>
#include <appl/dpp/UserInterface/pb/ui_pb_api_cnm.h>

#if LINK_PB_LIBRARIES

#ifdef UI_CNM
/******************************************************************** 
 *  Function handler: cp_set (section cnm)
 ********************************************************************/
int 
  ui_pb_api_cnm_cp_set(
    CURRENT_LINE *current_line, 
    uint8 is_petra_b 
  ) 
{   
  uint32
    indx, 
    ret;   
  SOC_PB_CNM_CP_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_cnm"); 
  soc_sand_proc_name = "soc_pb_cnm_cp_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_CNM_CP_INFO_clear(&prm_info);
 
  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_cnm_cp_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_cnm_cp_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_cnm_cp_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_CP_SET_CP_SET_INFO_IS_CP_ENABLED_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_CNM_CP_SET_CP_SET_INFO_IS_CP_ENABLED_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.is_cp_enabled = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_CP_SET_CP_SET_INFO_PCKT_CP_ID_4_MSB_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_CNM_CP_SET_CP_SET_INFO_PCKT_CP_ID_4_MSB_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.pckt.cp_id_4_msb = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_CP_SET_CP_SET_INFO_PCKT_TC_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_CNM_CP_SET_CP_SET_INFO_PCKT_TC_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.pckt.tc = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_CP_SET_CP_SET_INFO_PCKT_TC_ID_FTMH_OTM_PORT,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_CNM_CP_SET_CP_SET_INFO_PCKT_TC_ID_FTMH_OTM_PORT);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.pckt.dest_tm_port = (uint32)param_val->value.ulong_value;
  } 

  if (is_petra_b)
  {
    prm_info.pkt_gen_mode = SOC_PB_CNM_GEN_MODE_PETRA_B_PP;
  }
  else 
  {
    if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_CP_SET_CP_SET_INFO_PKT_GEN_MODE_ID,1)) 
    { 
      UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_CNM_CP_SET_CP_SET_INFO_PKT_GEN_MODE_ID); 
      UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
      prm_info.pkt_gen_mode = param_val->numeric_equivalent;
    }
  } 

  if(prm_info.pkt_gen_mode == SOC_PB_CNM_GEN_MODE_PETRA_B_PP)
  {

  /*   Begin of initialization of the CP in the soc_petra_B pp mode                    */ 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_CP_SET_CP_SET_INFO_PP_ETHER_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_CNM_CP_SET_CP_SET_INFO_PP_ETHER_TYPE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.pp.ether_type = (uint32)param_val->value.ulong_value;
  } 




  
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_CP_SET_CP_SET_INFO_PP_PDU_CP_ID_6_MSB_ID,1)) 
  { 
    for (indx = 0; indx < SOC_PB_CNM_NOF_UINT32S_IN_CP_ID;  ++indx, ui_ret = 0)
    {
   
      UI_MACROS_GET_NUMMERIC_VAL_WITH_INDEX(PARAM_PB_CNM_CP_SET_CP_SET_INFO_PP_PDU_CP_ID_6_MSB_ID, indx + 1);
     
    prm_info.pp.pdu.cp_id_6_msb[indx] = (uint32)param_val->value.ulong_value;
    }
  } 

   

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_CP_SET_CP_SET_INFO_PP_PDU_RES_V_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_CNM_CP_SET_CP_SET_INFO_PP_PDU_RES_V_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.pp.pdu.res_v = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_CP_SET_CP_SET_INFO_PP_PDU_VERSION_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_CNM_CP_SET_CP_SET_INFO_PP_PDU_VERSION_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.pp.pdu.version = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_CP_SET_CP_SET_INFO_PP_PDU_ETHER_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_CNM_CP_SET_CP_SET_INFO_PP_PDU_ETHER_TYPE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.pp.pdu.ether_type = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_CP_SET_CP_SET_INFO_PP_PPH_ING_VLAN_EDIT_CMD_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_CNM_CP_SET_CP_SET_INFO_PP_PPH_ING_VLAN_EDIT_CMD_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.pp.pph.ing_vlan_edit_cmd = (uint32)param_val->value.ulong_value;
  } 

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_PB_CNM_CP_SET_CP_SET_INFO_PP_MAC_SA_ID,1,  
         &param_val,VAL_TEXT,err_msg)) 
  { 
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_info.pp.mac_sa));
  }

  } /*   End of initialization of the CP in the soc_petra_B pp mode                    */ 

  /* Call function */
  ret = soc_pb_cnm_cp_set(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_cnm_cp_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_cnm_cp_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: cp_get (section cnm)
 ********************************************************************/
int 
  ui_pb_api_cnm_cp_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_CNM_CP_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_cnm"); 
  soc_sand_proc_name = "soc_pb_cnm_cp_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_CNM_CP_INFO_clear(&prm_info);
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_pb_cnm_cp_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_cnm_cp_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_cnm_cp_get");   
    goto exit; 
  } 

  SOC_PB_CNM_CP_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: q_mapping_set (section cnm)
 ********************************************************************/
int 
  ui_pb_api_cnm_q_mapping_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_CNM_Q_MAPPING_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_cnm"); 
  soc_sand_proc_name = "soc_pb_cnm_q_mapping_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_CNM_Q_MAPPING_INFO_clear(&prm_info);
 
  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_cnm_q_mapping_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_cnm_q_mapping_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_cnm_q_mapping_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_Q_MAPPING_SET_Q_MAPPING_SET_INFO_NOF_QUEUES_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_CNM_Q_MAPPING_SET_Q_MAPPING_SET_INFO_NOF_QUEUES_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.nof_queues = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_Q_MAPPING_SET_Q_MAPPING_SET_INFO_Q_SET_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_CNM_Q_MAPPING_SET_Q_MAPPING_SET_INFO_Q_SET_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.q_set = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_Q_MAPPING_SET_Q_MAPPING_SET_INFO_Q_BASE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_CNM_Q_MAPPING_SET_Q_MAPPING_SET_INFO_Q_BASE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.q_base = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_cnm_q_mapping_set(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_cnm_q_mapping_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_cnm_q_mapping_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: q_mapping_get (section cnm)
 ********************************************************************/
int 
  ui_pb_api_cnm_q_mapping_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_CNM_Q_MAPPING_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_cnm"); 
  soc_sand_proc_name = "soc_pb_cnm_q_mapping_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_CNM_Q_MAPPING_INFO_clear(&prm_info);
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_pb_cnm_q_mapping_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_cnm_q_mapping_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_cnm_q_mapping_get");   
    goto exit; 
  } 

  SOC_PB_CNM_Q_MAPPING_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: congestion_test_set (section cnm)
 ********************************************************************/
int 
  ui_pb_api_cnm_congestion_test_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_CNM_CONGESTION_TEST_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_cnm"); 
  soc_sand_proc_name = "soc_pb_cnm_congestion_test_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_CNM_CONGESTION_TEST_INFO_clear(&prm_info);
 
  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_cnm_congestion_test_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_cnm_congestion_test_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_cnm_congestion_test_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_CONGESTION_TEST_SET_CONGESTION_TEST_SET_INFO_IS_MIRR_ALSO_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_CNM_CONGESTION_TEST_SET_CONGESTION_TEST_SET_INFO_IS_MIRR_ALSO_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.is_mirr_also = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_CONGESTION_TEST_SET_CONGESTION_TEST_SET_INFO_IS_SNOOP_ALSO_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_CNM_CONGESTION_TEST_SET_CONGESTION_TEST_SET_INFO_IS_SNOOP_ALSO_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.is_snoop_also = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_CONGESTION_TEST_SET_CONGESTION_TEST_SET_INFO_IS_INGR_REP_ALSO_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_CNM_CONGESTION_TEST_SET_CONGESTION_TEST_SET_INFO_IS_INGR_REP_ALSO_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.is_ingr_rep_also = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_CONGESTION_TEST_SET_CONGESTION_TEST_SET_INFO_IS_MC_ALSO_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_CNM_CONGESTION_TEST_SET_CONGESTION_TEST_SET_INFO_IS_MC_ALSO_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.is_mc_also = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_cnm_congestion_test_set(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_cnm_congestion_test_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_cnm_congestion_test_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: congestion_test_get (section cnm)
 ********************************************************************/
int 
  ui_pb_api_cnm_congestion_test_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_CNM_CONGESTION_TEST_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_cnm"); 
  soc_sand_proc_name = "soc_pb_cnm_congestion_test_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_CNM_CONGESTION_TEST_INFO_clear(&prm_info);
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_pb_cnm_congestion_test_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_cnm_congestion_test_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_cnm_congestion_test_get");   
    goto exit; 
  } 

  SOC_PB_CNM_CONGESTION_TEST_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: cp_profile_set (section cnm)
 ********************************************************************/
int 
  ui_pb_api_cnm_cp_profile_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_profile_ndx;
  uint32 
    prm_sampling_base_index = 0xFFFFFFFF;  
  SOC_PB_CNM_CP_PROFILE_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_cnm"); 
  soc_sand_proc_name = "soc_pb_cnm_cp_profile_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_CNM_CP_PROFILE_INFO_clear(&prm_info);
 
  /* Get parameters */                                 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_CP_PROFILE_SET_CNM_CP_PROFILE_SET_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_CNM_CP_PROFILE_SET_CNM_CP_PROFILE_SET_PROFILE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_profile_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter profile_ndx after cp_profile_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_cnm_cp_profile_get(
          unit,
          prm_profile_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_cnm_cp_profile_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_cnm_cp_profile_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_CP_PROFILE_SET_CNM_CP_PROFILE_SET_INFO_IS_SAMPLING_TH_RANDOM_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_CNM_CP_PROFILE_SET_CNM_CP_PROFILE_SET_INFO_IS_SAMPLING_TH_RANDOM_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.is_sampling_th_random = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_CP_PROFILE_SET_CNM_CP_PROFILE_SET_INFO_QUANT_DIV_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_CNM_CP_PROFILE_SET_CNM_CP_PROFILE_SET_INFO_QUANT_DIV_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.quant_div = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_CP_PROFILE_SET_CNM_CP_PROFILE_SET_INFO_MAX_NEG_FB_VALUE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_CNM_CP_PROFILE_SET_CNM_CP_PROFILE_SET_INFO_MAX_NEG_FB_VALUE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.max_neg_fb_value = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_CP_PROFILE_SET_CNM_CP_PROFILE_SET_INFO_SAMPLING_BASE_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_CNM_CP_PROFILE_SET_CNM_CP_PROFILE_SET_INFO_SAMPLING_BASE_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_sampling_base_index = (uint32)param_val->value.ulong_value;
  } 

  if(prm_sampling_base_index != 0xFFFFFFFF)          
  { 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_CP_PROFILE_SET_CNM_CP_PROFILE_SET_INFO_SAMPLING_BASE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_CNM_CP_PROFILE_SET_CNM_CP_PROFILE_SET_INFO_SAMPLING_BASE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.sampling_base[ prm_sampling_base_index] = (uint32)param_val->value.ulong_value;
  } 

  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_CP_PROFILE_SET_CNM_CP_PROFILE_SET_INFO_CPW_POWER_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_CNM_CP_PROFILE_SET_CNM_CP_PROFILE_SET_INFO_CPW_POWER_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.cpw_power = (int32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_CP_PROFILE_SET_CNM_CP_PROFILE_SET_INFO_Q_EQ_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_CNM_CP_PROFILE_SET_CNM_CP_PROFILE_SET_INFO_Q_EQ_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.q_eq = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_cnm_cp_profile_set(
          unit,
          prm_profile_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_cnm_cp_profile_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_cnm_cp_profile_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: cp_profile_get (section cnm)
 ********************************************************************/
int 
  ui_pb_api_cnm_cp_profile_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_profile_ndx;
  SOC_PB_CNM_CP_PROFILE_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_cnm"); 
  soc_sand_proc_name = "soc_pb_cnm_cp_profile_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_CNM_CP_PROFILE_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_CP_PROFILE_GET_CNM_CP_PROFILE_GET_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_CNM_CP_PROFILE_GET_CNM_CP_PROFILE_GET_PROFILE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_profile_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter profile_ndx after cp_profile_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_cnm_cp_profile_get(
          unit,
          prm_profile_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_cnm_cp_profile_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_cnm_cp_profile_get");   
    goto exit; 
  } 

  SOC_PB_CNM_CP_PROFILE_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: sampling_profile_set (section cnm)
 ********************************************************************/
int 
  ui_pb_api_cnm_sampling_profile_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_profile_ndx;
  uint32   
    prm_sampling_rate;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_cnm"); 
  soc_sand_proc_name = "soc_pb_cnm_sampling_profile_set"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_SAMPLING_PROFILE_SET_CNM_SAMPLING_PROFILE_SET_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_CNM_SAMPLING_PROFILE_SET_CNM_SAMPLING_PROFILE_SET_PROFILE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_profile_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter profile_ndx after sampling_profile_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_cnm_sampling_profile_get(
          unit,
          prm_profile_ndx,
          &prm_sampling_rate
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_cnm_sampling_profile_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_cnm_sampling_profile_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_SAMPLING_PROFILE_SET_CNM_SAMPLING_PROFILE_SET_SAMPLING_RATE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_CNM_SAMPLING_PROFILE_SET_CNM_SAMPLING_PROFILE_SET_SAMPLING_RATE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_sampling_rate = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_cnm_sampling_profile_set(
          unit,
          prm_profile_ndx,
          prm_sampling_rate
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_cnm_sampling_profile_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_cnm_sampling_profile_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: sampling_profile_get (section cnm)
 ********************************************************************/
int 
  ui_pb_api_cnm_sampling_profile_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_profile_ndx;
  uint32   
    prm_sampling_rate;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_cnm"); 
  soc_sand_proc_name = "soc_pb_cnm_sampling_profile_get"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_SAMPLING_PROFILE_GET_CNM_SAMPLING_PROFILE_GET_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_CNM_SAMPLING_PROFILE_GET_CNM_SAMPLING_PROFILE_GET_PROFILE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_profile_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter profile_ndx after sampling_profile_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_cnm_sampling_profile_get(
          unit,
          prm_profile_ndx,
          &prm_sampling_rate
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_cnm_sampling_profile_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_cnm_sampling_profile_get");   
    goto exit; 
  } 

  soc_sand_os_printf("sampling_rate: %u\n\r",prm_sampling_rate);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: cpq_pp_set (section cnm)
 ********************************************************************/
int 
  ui_pb_api_cnm_cpq_pp_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_cpq_ndx;
  SOC_PB_CNM_CPQ_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_cnm"); 
  soc_sand_proc_name = "soc_pb_cnm_cpq_pp_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_CNM_CPQ_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_CPQ_PP_SET_CNM_CPQ_PP_SET_CPQ_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_CNM_CPQ_PP_SET_CNM_CPQ_PP_SET_CPQ_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_cpq_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter cpq_ndx after cpq_pp_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_cnm_cpq_pp_get(
          unit,
          prm_cpq_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_cnm_cpq_pp_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_cnm_cpq_pp_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_CPQ_PP_SET_CNM_CPQ_PP_SET_INFO_PROFILE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_CNM_CPQ_PP_SET_CNM_CPQ_PP_SET_INFO_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.profile = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_CPQ_PP_SET_CNM_CPQ_PP_SET_INFO_IS_CP_ENABLED_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_CNM_CPQ_PP_SET_CNM_CPQ_PP_SET_INFO_IS_CP_ENABLED_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.is_cp_enabled = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_cnm_cpq_pp_set(
          unit,
          prm_cpq_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_cnm_cpq_pp_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_cnm_cpq_pp_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: cpq_pp_get (section cnm)
 ********************************************************************/
int 
  ui_pb_api_cnm_cpq_pp_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_cpq_ndx;
  SOC_PB_CNM_CPQ_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_cnm"); 
  soc_sand_proc_name = "soc_pb_cnm_cpq_pp_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_CNM_CPQ_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_CPQ_PP_GET_CNM_CPQ_PP_GET_CPQ_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_CNM_CPQ_PP_GET_CNM_CPQ_PP_GET_CPQ_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_cpq_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter cpq_ndx after cpq_pp_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_cnm_cpq_pp_get(
          unit,
          prm_cpq_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_cnm_cpq_pp_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_cnm_cpq_pp_get");   
    goto exit; 
  } 

  SOC_PB_CNM_CPQ_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: cpq_sampling_set (section cnm)
 ********************************************************************/
int 
  ui_pb_api_cnm_cpq_sampling_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_cpq_ndx;
  SOC_PB_CNM_CPQ_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_cnm"); 
  soc_sand_proc_name = "soc_pb_cnm_cpq_sampling_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_CNM_CPQ_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_CPQ_SAMPLING_SET_CNM_CPQ_SAMPLING_SET_CPQ_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_CNM_CPQ_SAMPLING_SET_CNM_CPQ_SAMPLING_SET_CPQ_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_cpq_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter cpq_ndx after cpq_sampling_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_cnm_cpq_sampling_get(
          unit,
          prm_cpq_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_cnm_cpq_sampling_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_cnm_cpq_sampling_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_CPQ_SAMPLING_SET_CNM_CPQ_SAMPLING_SET_INFO_PROFILE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_CNM_CPQ_SAMPLING_SET_CNM_CPQ_SAMPLING_SET_INFO_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.profile = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_CPQ_SAMPLING_SET_CNM_CPQ_SAMPLING_SET_INFO_IS_CP_ENABLED_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_CNM_CPQ_SAMPLING_SET_CNM_CPQ_SAMPLING_SET_INFO_IS_CP_ENABLED_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.is_cp_enabled = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_cnm_cpq_sampling_set(
          unit,
          prm_cpq_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_cnm_cpq_sampling_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_cnm_cpq_sampling_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: cpq_sampling_get (section cnm)
 ********************************************************************/
int 
  ui_pb_api_cnm_cpq_sampling_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_cpq_ndx;
  SOC_PB_CNM_CPQ_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_cnm"); 
  soc_sand_proc_name = "soc_pb_cnm_cpq_sampling_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_CNM_CPQ_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_CPQ_SAMPLING_GET_CNM_CPQ_SAMPLING_GET_CPQ_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_CNM_CPQ_SAMPLING_GET_CNM_CPQ_SAMPLING_GET_CPQ_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_cpq_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter cpq_ndx after cpq_sampling_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_cnm_cpq_sampling_get(
          unit,
          prm_cpq_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_cnm_cpq_sampling_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_cnm_cpq_sampling_get");   
    goto exit; 
  } 

  SOC_PB_CNM_CPQ_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: intercept_timer_set (section cnm)
 ********************************************************************/
int 
  ui_pb_api_cnm_intercept_timer_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_delay;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_cnm"); 
  soc_sand_proc_name = "soc_pb_cnm_intercept_timer_set"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_cnm_intercept_timer_get(
          unit,
          &prm_delay
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_cnm_intercept_timer_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_cnm_intercept_timer_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_INTERCEPT_TIMER_SET_CNM_INTERCEPT_TIMER_SET_DELAY_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_CNM_INTERCEPT_TIMER_SET_CNM_INTERCEPT_TIMER_SET_DELAY_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_delay = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_cnm_intercept_timer_set(
          unit,
          prm_delay
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_cnm_intercept_timer_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_cnm_intercept_timer_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: intercept_timer_get (section cnm)
 ********************************************************************/
int 
  ui_pb_api_cnm_intercept_timer_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_delay;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_cnm"); 
  soc_sand_proc_name = "soc_pb_cnm_intercept_timer_get"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_pb_cnm_intercept_timer_get(
          unit,
          &prm_delay
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_cnm_intercept_timer_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_cnm_intercept_timer_get");   
    goto exit; 
  } 

  soc_sand_os_printf("delay: %u\n\r",prm_delay);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
#endif
#ifdef UI_CNM/* { cnm*/
/******************************************************************** 
 *  Section handler: cnm
 ********************************************************************/ 
int 
  ui_pb_api_cnm( 
    CURRENT_LINE *current_line 
  ) 
{   
  int
    ret = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_pb_api_cnm"); 
 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_CP_SET_CP_SET_ID_EXT_PP_OR_SAMPLING,1)) 
  { 
    ret = ui_pb_api_cnm_cp_set(current_line,0); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_CP_SET_CP_SET_ID_PETRA_B_PP,1)) 
  { 
    ret = ui_pb_api_cnm_cp_set(current_line,1); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_CP_GET_CNM_CP_GET_ID,1)) 
  { 
    ret = ui_pb_api_cnm_cp_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_Q_MAPPING_SET_CNM_Q_MAPPING_SET_ID,1)) 
  { 
    ret = ui_pb_api_cnm_q_mapping_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_Q_MAPPING_GET_CNM_Q_MAPPING_GET_ID,1)) 
  { 
    ret = ui_pb_api_cnm_q_mapping_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_CONGESTION_TEST_SET_CNM_CONGESTION_TEST_SET_ID,1)) 
  { 
    ret = ui_pb_api_cnm_congestion_test_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_CONGESTION_TEST_GET_CNM_CONGESTION_TEST_GET_ID,1)) 
  { 
    ret = ui_pb_api_cnm_congestion_test_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_CP_PROFILE_SET_CNM_CP_PROFILE_SET_ID,1)) 
  { 
    ret = ui_pb_api_cnm_cp_profile_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_CP_PROFILE_GET_CNM_CP_PROFILE_GET_ID,1)) 
  { 
    ret = ui_pb_api_cnm_cp_profile_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_SAMPLING_PROFILE_SET_CNM_SAMPLING_PROFILE_SET_ID,1)) 
  { 
    ret = ui_pb_api_cnm_sampling_profile_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_SAMPLING_PROFILE_GET_CNM_SAMPLING_PROFILE_GET_ID,1)) 
  { 
    ret = ui_pb_api_cnm_sampling_profile_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_CPQ_PP_SET_CNM_CPQ_PP_SET_ID,1)) 
  { 
    ret = ui_pb_api_cnm_cpq_pp_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_CPQ_PP_GET_CNM_CPQ_PP_GET_ID,1)) 
  { 
    ret = ui_pb_api_cnm_cpq_pp_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_CPQ_SAMPLING_SET_CNM_CPQ_SAMPLING_SET_ID,1)) 
  { 
    ret = ui_pb_api_cnm_cpq_sampling_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_CPQ_SAMPLING_GET_CNM_CPQ_SAMPLING_GET_ID,1)) 
  { 
    ret = ui_pb_api_cnm_cpq_sampling_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_INTERCEPT_TIMER_SET_CNM_INTERCEPT_TIMER_SET_ID,1)) 
  { 
    ret = ui_pb_api_cnm_intercept_timer_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNM_INTERCEPT_TIMER_GET_CNM_INTERCEPT_TIMER_GET_ID,1)) 
  { 
    ret = ui_pb_api_cnm_intercept_timer_get(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after cnm***", TRUE); 
  }
  
  ui_ret = ret; 
    
  goto exit; 
exit:        
  return ui_ret; 
}                
          
#endif /* cnm */ 


#endif /* LINK_PB_LIBRARIES */ 

