/* $Id: ui_pcp_oam_api_eth.c,v 1.8 Broadcom SDK $
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
#include <shared/bsl.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h> 
#include <appl/diag/dpp/utils_defi.h> 
#include <appl/dpp/UserInterface/ui_defi.h> 
 
#include <appl/dpp/UserInterface/ui_pure_defi_pcp_api.h> 
 
  
#include <soc/dpp/PCP/pcp_oam_api_eth.h>
#include <appl/dpp/UserInterface/pcp/ui_pcp_oam_api_eth.h>

#if LINK_PCP_LIBRARIES

#ifdef UI_ETH
/******************************************************************** 
 *  Function handler: general_info_set (section eth)
 ********************************************************************/
int 
  ui_pcp_oam_api_eth_general_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  PCP_OAM_ETH_GENERAL_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_eth"); 
  soc_sand_proc_name = "pcp_oam_eth_general_info_set"; 
 
  unit = pcp_get_default_unit(); 
  PCP_OAM_ETH_GENERAL_INFO_clear(&prm_info);
 
  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = pcp_oam_eth_general_info_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_eth_general_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_eth_general_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_GENERAL_INFO_SET_GENERAL_INFO_SET_INFO_CUSTOM_HEADER_SIZE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_GENERAL_INFO_SET_GENERAL_INFO_SET_INFO_CUSTOM_HEADER_SIZE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.custom_header_size = (uint8)param_val->value.ulong_value;
  } 

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_PCP_OAM_ETH_GENERAL_INFO_SET_GENERAL_INFO_SET_INFO_MEP_SA_MSB_ID,1,  
         &param_val,VAL_TEXT,err_msg)) 
  { 
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_info.mep_sa_msb));
  } 

  /* Call function */
  ret = pcp_oam_eth_general_info_set(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_eth_general_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_eth_general_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: general_info_get (section eth)
 ********************************************************************/
int 
  ui_pcp_oam_api_eth_general_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  PCP_OAM_ETH_GENERAL_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_eth"); 
  soc_sand_proc_name = "pcp_oam_eth_general_info_get"; 
 
  unit = pcp_get_default_unit(); 
  PCP_OAM_ETH_GENERAL_INFO_clear(&prm_info);
 
  /* Get parameters */ 

  /* Call function */
  ret = pcp_oam_eth_general_info_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_eth_general_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_eth_general_info_get");   
    goto exit; 
  } 

  PCP_OAM_ETH_GENERAL_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: cos_mapping_info_set (section eth)
 ********************************************************************/
int 
  ui_pcp_oam_api_eth_cos_mapping_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  PCP_OAM_ETH_COS_PROFILE_ID   
    prm_cos_ndx;
  uint32   
    prm_prio_ndx;
  PCP_OAM_ETH_COS_MAPPING_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_eth"); 
  soc_sand_proc_name = "pcp_oam_eth_cos_mapping_info_set"; 
 
  unit = pcp_get_default_unit(); 
  PCP_OAM_ETH_COS_MAPPING_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_COS_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_COS_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_cos_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter cos_ndx after cos_mapping_info_set***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_PRIO_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_PRIO_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_prio_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter prio_ndx after cos_mapping_info_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = pcp_oam_eth_cos_mapping_info_get(
          unit,
          prm_cos_ndx,
          prm_prio_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_eth_cos_mapping_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_eth_cos_mapping_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_INFO_DP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_INFO_DP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.dp = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_INFO_TC_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_INFO_TC_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.tc = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = pcp_oam_eth_cos_mapping_info_set(
          unit,
          prm_cos_ndx,
          prm_prio_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_eth_cos_mapping_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_eth_cos_mapping_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: cos_mapping_info_get (section eth)
 ********************************************************************/
int 
  ui_pcp_oam_api_eth_cos_mapping_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  PCP_OAM_ETH_COS_PROFILE_ID   
    prm_cos_ndx;
  uint32   
    prm_prio_ndx;
  PCP_OAM_ETH_COS_MAPPING_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_eth"); 
  soc_sand_proc_name = "pcp_oam_eth_cos_mapping_info_get"; 
 
  unit = pcp_get_default_unit(); 
  PCP_OAM_ETH_COS_MAPPING_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_COS_MAPPING_INFO_GET_COS_MAPPING_INFO_GET_COS_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_COS_MAPPING_INFO_GET_COS_MAPPING_INFO_GET_COS_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_cos_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter cos_ndx after cos_mapping_info_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_COS_MAPPING_INFO_GET_COS_MAPPING_INFO_GET_PRIO_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_COS_MAPPING_INFO_GET_COS_MAPPING_INFO_GET_PRIO_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_prio_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter prio_ndx after cos_mapping_info_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = pcp_oam_eth_cos_mapping_info_get(
          unit,
          prm_cos_ndx,
          prm_prio_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_eth_cos_mapping_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_eth_cos_mapping_info_get");   
    goto exit; 
  } 

  PCP_OAM_ETH_COS_MAPPING_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: oam_mp_info_set (section eth)
 ********************************************************************/
int 
  ui_pcp_oam_api_eth_oam_mp_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  PCP_OAM_ETH_MP_KEY   
    prm_mp_key;
  PCP_OAM_ETH_MP_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_eth"); 
  soc_sand_proc_name = "pcp_oam_mp_info_set"; 
 
  unit = pcp_get_default_unit(); 
  PCP_OAM_ETH_MP_KEY_clear(&prm_mp_key);
  PCP_OAM_ETH_MP_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MP_INFO_SET_OAM_MP_INFO_SET_MP_KEY_DIRECTION_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PCP_OAM_MP_INFO_SET_OAM_MP_INFO_SET_MP_KEY_DIRECTION_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_mp_key.direction = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MP_INFO_SET_OAM_MP_INFO_SET_MP_KEY_LEVEL_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_MP_INFO_SET_OAM_MP_INFO_SET_MP_KEY_LEVEL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_mp_key.level = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MP_INFO_SET_OAM_MP_INFO_SET_MP_KEY_AC_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_MP_INFO_SET_OAM_MP_INFO_SET_MP_KEY_AC_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_mp_key.ac_ndx = (SOC_PPD_LIF_ID)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter mp_key after oam_mp_info_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = pcp_oam_mp_info_get(
          unit,
          &prm_mp_key,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_mp_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_mp_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MP_INFO_SET_OAM_MP_INFO_SET_INFO_ACC_MEP_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_MP_INFO_SET_OAM_MP_INFO_SET_INFO_ACC_MEP_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.acc_mep_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MP_INFO_SET_OAM_MP_INFO_SET_INFO_VALID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_MP_INFO_SET_OAM_MP_INFO_SET_INFO_VALID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.valid = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = pcp_oam_mp_info_set(
          unit,
          &prm_mp_key,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_mp_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_mp_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: oam_mp_info_get (section eth)
 ********************************************************************/
int 
  ui_pcp_oam_api_eth_oam_mp_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  PCP_OAM_ETH_MP_KEY   
    prm_mp_key;
  PCP_OAM_ETH_MP_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_eth"); 
  soc_sand_proc_name = "pcp_oam_mp_info_get"; 
 
  unit = pcp_get_default_unit(); 
  PCP_OAM_ETH_MP_KEY_clear(&prm_mp_key);
  PCP_OAM_ETH_MP_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MP_INFO_GET_OAM_MP_INFO_GET_MP_KEY_DIRECTION_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PCP_OAM_MP_INFO_GET_OAM_MP_INFO_GET_MP_KEY_DIRECTION_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_mp_key.direction = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MP_INFO_GET_OAM_MP_INFO_GET_MP_KEY_LEVEL_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_MP_INFO_GET_OAM_MP_INFO_GET_MP_KEY_LEVEL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_mp_key.level = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MP_INFO_GET_OAM_MP_INFO_GET_MP_KEY_AC_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_MP_INFO_GET_OAM_MP_INFO_GET_MP_KEY_AC_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_mp_key.ac_ndx = (SOC_PPD_LIF_ID)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter mp_key after oam_mp_info_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = pcp_oam_mp_info_get(
          unit,
          &prm_mp_key,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_mp_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_mp_info_get");   
    goto exit; 
  } 

  PCP_OAM_ETH_MP_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: acc_mep_info_set (section eth)
 ********************************************************************/
int 
  ui_pcp_oam_api_eth_acc_mep_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  PCP_OAM_ETH_ACC_MEP_ID   
    prm_acc_mep_ndx;
  PCP_OAM_ETH_ACC_MEP_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_eth"); 
  soc_sand_proc_name = "pcp_oam_eth_acc_mep_info_set"; 
 
  unit = pcp_get_default_unit(); 
  PCP_OAM_ETH_ACC_MEP_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_ACC_MEP_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_ACC_MEP_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_acc_mep_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter acc_mep_ndx after acc_mep_info_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = pcp_oam_eth_acc_mep_info_get(
          unit,
          prm_acc_mep_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_eth_acc_mep_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_eth_acc_mep_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_PEFORMANCE_MONITORING_INFO_DLM_INFO_DLM_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_PEFORMANCE_MONITORING_INFO_DLM_INFO_DLM_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.peformance_monitoring_info.dlm_info.dlm_index = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_PEFORMANCE_MONITORING_INFO_DLM_INFO_DLM_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_PEFORMANCE_MONITORING_INFO_DLM_INFO_DLM_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.peformance_monitoring_info.dlm_info.dlm_enable = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_PEFORMANCE_MONITORING_INFO_LMM_INFO_IS_DUAL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_PEFORMANCE_MONITORING_INFO_LMM_INFO_IS_DUAL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.peformance_monitoring_info.lmm_info.is_dual = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_PEFORMANCE_MONITORING_INFO_LMM_INFO_LMM_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_PEFORMANCE_MONITORING_INFO_LMM_INFO_LMM_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.peformance_monitoring_info.lmm_info.lmm_index = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_PEFORMANCE_MONITORING_INFO_LMM_INFO_LMM_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_PEFORMANCE_MONITORING_INFO_LMM_INFO_LMM_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.peformance_monitoring_info.lmm_info.lmm_enable = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_PEFORMANCE_MONITORING_INFO_LMM_INFO_LMR_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_PEFORMANCE_MONITORING_INFO_LMM_INFO_LMR_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.peformance_monitoring_info.lmm_info.lmr_enable = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_PEFORMANCE_MONITORING_INFO_CCM_INFO_CCM_PRIORITY_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_PEFORMANCE_MONITORING_INFO_CCM_INFO_CCM_PRIORITY_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.peformance_monitoring_info.ccm_info.ccm_priority = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_PEFORMANCE_MONITORING_INFO_CCM_INFO_CCM_INTERVAL_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_PEFORMANCE_MONITORING_INFO_CCM_INFO_CCM_INTERVAL_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.peformance_monitoring_info.ccm_info.ccm_interval = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FRWD_INFO_DOWN_MEP_COS_PROFILE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FRWD_INFO_DOWN_MEP_COS_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.frwd_info.down_mep.cos_profile = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FRWD_INFO_DOWN_MEP_AC_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FRWD_INFO_DOWN_MEP_AC_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.frwd_info.down_mep.ac = (uint32)param_val->value.ulong_value;
  } 
/*
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FRWD_INFO_DOWN_MEP_DESTINATION_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FRWD_INFO_DOWN_MEP_DESTINATION_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
	  prm_info.frwd_info.up_mep.destination = (SOC_SAND_PP_DESTINATION_ID)param_val->value.ulong_value;
  }
*/
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FRWD_INFO_DOWN_MEP_DEST_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FRWD_INFO_DOWN_MEP_DEST_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.frwd_info.down_mep.destination.id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FRWD_INFO_DOWN_MEP_DEST_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FRWD_INFO_DOWN_MEP_DEST_TYPE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.frwd_info.down_mep.destination.type = (SOC_TMC_DEST_TYPE)param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FRWD_INFO_DOWN_MEP_DN_VID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FRWD_INFO_DOWN_MEP_DN_VID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.frwd_info.down_mep.dn_vid = (uint32)param_val->value.ulong_value;
  } 

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FRWD_INFO_DOWN_MEP_DN_SA_LSB_ID,1,  
         &param_val,VAL_TEXT,err_msg)) 
  { 
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_info.frwd_info.down_mep.dn_sa_lsb));
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FRWD_INFO_UP_MEP_UP_VID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FRWD_INFO_UP_MEP_UP_VID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.frwd_info.up_mep.up_vid = (uint32)param_val->value.ulong_value;
  } 

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_FRWD_INFO_UP_MEP_UP_SA_LSB_ID,1,  
         &param_val,VAL_TEXT,err_msg)) 
  { 
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_info.frwd_info.up_mep.up_sa_lsb));
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_DIRECTION_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_DIRECTION_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.direction = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_MD_LEVEL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_MD_LEVEL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.md_level = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_MEP_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_MEP_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.mep_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_MA_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_INFO_MA_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.ma_id = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = pcp_oam_eth_acc_mep_info_set(
          unit,
          prm_acc_mep_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_eth_acc_mep_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_eth_acc_mep_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: acc_mep_info_get (section eth)
 ********************************************************************/
int 
  ui_pcp_oam_api_eth_acc_mep_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  PCP_OAM_ETH_ACC_MEP_ID   
    prm_acc_mep_ndx;
  PCP_OAM_ETH_ACC_MEP_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_eth"); 
  soc_sand_proc_name = "pcp_oam_eth_acc_mep_info_get"; 
 
  unit = pcp_get_default_unit(); 
  PCP_OAM_ETH_ACC_MEP_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_MEP_INFO_GET_ACC_MEP_INFO_GET_ACC_MEP_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_ACC_MEP_INFO_GET_ACC_MEP_INFO_GET_ACC_MEP_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_acc_mep_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter acc_mep_ndx after acc_mep_info_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = pcp_oam_eth_acc_mep_info_get(
          unit,
          prm_acc_mep_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_eth_acc_mep_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_eth_acc_mep_info_get");   
    goto exit; 
  } 

  PCP_OAM_ETH_ACC_MEP_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: acc_mep_custom_header_set (section eth)
 ********************************************************************/
int 
  ui_pcp_oam_api_eth_acc_mep_custom_header_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  PCP_OAM_ETH_ACC_MEP_ID   
    prm_acc_mep_ndx;
  uint32   
    prm_header;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_eth"); 
  soc_sand_proc_name = "pcp_oam_eth_acc_mep_custom_header_set"; 
 
  unit = pcp_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_MEP_CUSTOM_HEADER_SET_ACC_MEP_CUSTOM_HEADER_SET_ACC_MEP_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_ACC_MEP_CUSTOM_HEADER_SET_ACC_MEP_CUSTOM_HEADER_SET_ACC_MEP_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_acc_mep_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter acc_mep_ndx after acc_mep_custom_header_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = pcp_oam_eth_acc_mep_custom_header_get(
          unit,
          prm_acc_mep_ndx,
          &prm_header
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_eth_acc_mep_custom_header_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_eth_acc_mep_custom_header_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_MEP_CUSTOM_HEADER_SET_ACC_MEP_CUSTOM_HEADER_SET_HEADER_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_ACC_MEP_CUSTOM_HEADER_SET_ACC_MEP_CUSTOM_HEADER_SET_HEADER_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_header = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = pcp_oam_eth_acc_mep_custom_header_set(
          unit,
          prm_acc_mep_ndx,
          prm_header
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_eth_acc_mep_custom_header_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_eth_acc_mep_custom_header_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: acc_mep_custom_header_get (section eth)
 ********************************************************************/
int 
  ui_pcp_oam_api_eth_acc_mep_custom_header_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  PCP_OAM_ETH_ACC_MEP_ID   
    prm_acc_mep_ndx;
  uint32   
    prm_header;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_eth"); 
  soc_sand_proc_name = "pcp_oam_eth_acc_mep_custom_header_get"; 
 
  unit = pcp_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_MEP_CUSTOM_HEADER_GET_ACC_MEP_CUSTOM_HEADER_GET_ACC_MEP_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_ACC_MEP_CUSTOM_HEADER_GET_ACC_MEP_CUSTOM_HEADER_GET_ACC_MEP_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_acc_mep_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter acc_mep_ndx after acc_mep_custom_header_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = pcp_oam_eth_acc_mep_custom_header_get(
          unit,
          prm_acc_mep_ndx,
          &prm_header
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_eth_acc_mep_custom_header_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_eth_acc_mep_custom_header_get");   
    goto exit; 
  } 

  cli_out("header: %u\n\r",prm_header);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: acc_lmm_set (section eth)
 ********************************************************************/
int 
  ui_pcp_oam_api_eth_acc_lmm_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  PCP_OAM_ETH_ACC_LMM_ID   
    prm_acc_lmm_ndx;
  PCP_OAM_ETH_ACC_LMM_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_eth"); 
  soc_sand_proc_name = "pcp_oam_eth_acc_lmm_set"; 
 
  unit = pcp_get_default_unit(); 
  PCP_OAM_ETH_ACC_LMM_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_LMM_SET_ACC_LMM_SET_ACC_LMM_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_ACC_LMM_SET_ACC_LMM_SET_ACC_LMM_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_acc_lmm_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter acc_lmm_ndx after acc_lmm_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = pcp_oam_eth_acc_lmm_get(
          unit,
          prm_acc_lmm_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_eth_acc_lmm_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_eth_acc_lmm_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_LMM_SET_ACC_LMM_SET_INFO_PRIORITY_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_ACC_LMM_SET_ACC_LMM_SET_INFO_PRIORITY_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.priority = (uint8)param_val->value.ulong_value;
  } 

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_PCP_OAM_ETH_ACC_LMM_SET_ACC_LMM_SET_INFO_DA_ID,1,  
         &param_val,VAL_TEXT,err_msg)) 
  { 
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_info.da));
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_LMM_SET_ACC_LMM_SET_INFO_INTERVAL_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PCP_OAM_ETH_ACC_LMM_SET_ACC_LMM_SET_INFO_INTERVAL_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.interval = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_LMM_SET_ACC_LMM_SET_INFO_RX_COUNTER_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_ACC_LMM_SET_ACC_LMM_SET_INFO_RX_COUNTER_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.rx_counter = param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_LMM_SET_ACC_LMM_SET_INFO_TX_COUNTER_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_ACC_LMM_SET_ACC_LMM_SET_INFO_TX_COUNTER_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.tx_counter = param_val->value.ulong_value;
  } 

  /* Call function */
  ret = pcp_oam_eth_acc_lmm_set(
          unit,
          prm_acc_lmm_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_eth_acc_lmm_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_eth_acc_lmm_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: acc_lmm_get (section eth)
 ********************************************************************/
int 
  ui_pcp_oam_api_eth_acc_lmm_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  PCP_OAM_ETH_ACC_LMM_ID   
    prm_acc_lmm_ndx;
  PCP_OAM_ETH_ACC_LMM_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_eth"); 
  soc_sand_proc_name = "pcp_oam_eth_acc_lmm_get"; 
 
  unit = pcp_get_default_unit(); 
  PCP_OAM_ETH_ACC_LMM_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_LMM_GET_ACC_LMM_GET_ACC_LMM_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_ACC_LMM_GET_ACC_LMM_GET_ACC_LMM_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_acc_lmm_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter acc_lmm_ndx after acc_lmm_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = pcp_oam_eth_acc_lmm_get(
          unit,
          prm_acc_lmm_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_eth_acc_lmm_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_eth_acc_lmm_get");   
    goto exit; 
  } 

  PCP_OAM_ETH_ACC_LMM_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: acc_dlm_set (section eth)
 ********************************************************************/
int 
  ui_pcp_oam_api_eth_acc_dlm_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  PCP_OAM_ETH_ACC_DLM_ID   
    prm_acc_dlm_ndx;
  PCP_OAM_ETH_ACC_DLM_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_eth"); 
  soc_sand_proc_name = "pcp_oam_eth_acc_dlm_set"; 
 
  unit = pcp_get_default_unit(); 
  PCP_OAM_ETH_ACC_DLM_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_DLM_SET_ACC_DLM_SET_ACC_DLM_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_ACC_DLM_SET_ACC_DLM_SET_ACC_DLM_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_acc_dlm_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter acc_dlm_ndx after acc_dlm_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = pcp_oam_eth_acc_dlm_get(
          unit,
          prm_acc_dlm_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_eth_acc_dlm_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_eth_acc_dlm_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_DLM_SET_ACC_DLM_SET_INFO_PRIORITY_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_ACC_DLM_SET_ACC_DLM_SET_INFO_PRIORITY_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.priority = (uint8)param_val->value.ulong_value;
  } 

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_PCP_OAM_ETH_ACC_DLM_SET_ACC_DLM_SET_INFO_DA_ID,1,  
         &param_val,VAL_TEXT,err_msg)) 
  { 
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_info.da));
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_DLM_SET_ACC_DLM_SET_INFO_INTERVAL_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PCP_OAM_ETH_ACC_DLM_SET_ACC_DLM_SET_INFO_INTERVAL_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.interval = param_val->numeric_equivalent;
  } 


  /* Call function */
  ret = pcp_oam_eth_acc_dlm_set(
          unit,
          prm_acc_dlm_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_eth_acc_dlm_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_eth_acc_dlm_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: acc_dlm_get (section eth)
 ********************************************************************/
int 
  ui_pcp_oam_api_eth_acc_dlm_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  PCP_OAM_ETH_ACC_DLM_ID   
    prm_acc_dlm_ndx;
  PCP_OAM_ETH_ACC_DLM_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_eth"); 
  soc_sand_proc_name = "pcp_oam_eth_acc_dlm_get"; 
 
  unit = pcp_get_default_unit(); 
  PCP_OAM_ETH_ACC_DLM_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_DLM_GET_ACC_DLM_GET_ACC_DLM_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_ACC_DLM_GET_ACC_DLM_GET_ACC_DLM_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_acc_dlm_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter acc_dlm_ndx after acc_dlm_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = pcp_oam_eth_acc_dlm_get(
          unit,
          prm_acc_dlm_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_eth_acc_dlm_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_eth_acc_dlm_get");   
    goto exit; 
  } 

  PCP_OAM_ETH_ACC_DLM_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: rmep_add (section eth)
 ********************************************************************/
int 
  ui_pcp_oam_api_eth_rmep_add(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  PCP_OAM_ETH_RMEP_KEY   
    prm_rmep_key;
  uint32   
    prm_rmep = 0;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_eth"); 
  soc_sand_proc_name = "pcp_oam_eth_rmep_add"; 
 
  unit = pcp_get_default_unit(); 
  PCP_OAM_ETH_RMEP_KEY_clear(&prm_rmep_key);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_RMEP_ADD_RMEP_ADD_RMEP_KEY_RMEP_ID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_RMEP_ADD_RMEP_ADD_RMEP_KEY_RMEP_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_rmep_key.rmep_id = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_RMEP_ADD_RMEP_ADD_RMEP_KEY_ACC_MEP_ID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_RMEP_ADD_RMEP_ADD_RMEP_KEY_ACC_MEP_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_rmep_key.acc_mep_id = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter rmep_key after rmep_add***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_RMEP_ADD_RMEP_ADD_RMEP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_RMEP_ADD_RMEP_ADD_RMEP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rmep = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = pcp_oam_eth_rmep_add(
          unit,
          &prm_rmep_key,
          prm_rmep
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_eth_rmep_add - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_eth_rmep_add");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: rmep_remove (section eth)
 ********************************************************************/
int 
  ui_pcp_oam_api_eth_rmep_remove(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  PCP_OAM_ETH_RMEP_KEY   
    prm_rmep_key;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_eth"); 
  soc_sand_proc_name = "pcp_oam_eth_rmep_remove"; 
 
  unit = pcp_get_default_unit(); 
  PCP_OAM_ETH_RMEP_KEY_clear(&prm_rmep_key);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_RMEP_REMOVE_RMEP_REMOVE_RMEP_KEY_RMEP_ID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_RMEP_REMOVE_RMEP_REMOVE_RMEP_KEY_RMEP_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_rmep_key.rmep_id = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_RMEP_REMOVE_RMEP_REMOVE_RMEP_KEY_ACC_MEP_ID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_RMEP_REMOVE_RMEP_REMOVE_RMEP_KEY_ACC_MEP_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_rmep_key.acc_mep_id = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter rmep_key after rmep_remove***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = pcp_oam_eth_rmep_remove(
          unit,
          &prm_rmep_key
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_eth_rmep_remove - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_eth_rmep_remove");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: rmep_info_set (section eth)
 ********************************************************************/
int 
  ui_pcp_oam_api_eth_rmep_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_rmep_ndx;
  PCP_OAM_ETH_RMEP_INFO   
    prm_rmep_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_eth"); 
  soc_sand_proc_name = "pcp_oam_eth_rmep_info_set"; 
 
  unit = pcp_get_default_unit(); 
  PCP_OAM_ETH_RMEP_INFO_clear(&prm_rmep_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_RMEP_INFO_SET_RMEP_INFO_SET_RMEP_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_RMEP_INFO_SET_RMEP_INFO_SET_RMEP_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_rmep_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter rmep_ndx after rmep_info_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = pcp_oam_eth_rmep_info_get(
          unit,
          prm_rmep_ndx,
          &prm_rmep_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_eth_rmep_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_eth_rmep_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_RMEP_INFO_SET_RMEP_INFO_SET_RMEP_INFO_LOCAL_DEFECT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_RMEP_INFO_SET_RMEP_INFO_SET_RMEP_INFO_LOCAL_DEFECT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rmep_info.local_defect = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_RMEP_INFO_SET_RMEP_INFO_SET_RMEP_INFO_RDI_RECEIVED_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_RMEP_INFO_SET_RMEP_INFO_SET_RMEP_INFO_RDI_RECEIVED_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rmep_info.rdi_received = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_RMEP_INFO_SET_RMEP_INFO_SET_RMEP_INFO_INTERVAL_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PCP_OAM_ETH_RMEP_INFO_SET_RMEP_INFO_SET_RMEP_INFO_INTERVAL_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rmep_info.interval = param_val->numeric_equivalent;
  } 


  /* Call function */
  ret = pcp_oam_eth_rmep_info_set(
          unit,
          prm_rmep_ndx,
          &prm_rmep_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_eth_rmep_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_eth_rmep_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: rmep_info_get (section eth)
 ********************************************************************/
int 
  ui_pcp_oam_api_eth_rmep_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_rmep_ndx;
  PCP_OAM_ETH_RMEP_INFO   
    prm_rmep_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_eth"); 
  soc_sand_proc_name = "pcp_oam_eth_rmep_info_get"; 
 
  unit = pcp_get_default_unit(); 
  PCP_OAM_ETH_RMEP_INFO_clear(&prm_rmep_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_RMEP_INFO_GET_RMEP_INFO_GET_RMEP_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_RMEP_INFO_GET_RMEP_INFO_GET_RMEP_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_rmep_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter rmep_ndx after rmep_info_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = pcp_oam_eth_rmep_info_get(
          unit,
          prm_rmep_ndx,
          &prm_rmep_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_eth_rmep_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_eth_rmep_info_get");   
    goto exit; 
  } 

  PCP_OAM_ETH_RMEP_INFO_print(&prm_rmep_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: acc_mep_tx_rdi_set (section eth)
 ********************************************************************/
int 
  ui_pcp_oam_api_eth_acc_mep_tx_rdi_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  PCP_OAM_ETH_ACC_MEP_ID   
    prm_acc_mep_ndx;
  uint8   
    prm_rdi;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_eth"); 
  soc_sand_proc_name = "pcp_oam_eth_acc_mep_tx_rdi_set"; 
 
  unit = pcp_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_MEP_TX_RDI_SET_ACC_MEP_TX_RDI_SET_ACC_MEP_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_ACC_MEP_TX_RDI_SET_ACC_MEP_TX_RDI_SET_ACC_MEP_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_acc_mep_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter acc_mep_ndx after acc_mep_tx_rdi_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = pcp_oam_eth_acc_mep_tx_rdi_get(
          unit,
          prm_acc_mep_ndx,
          &prm_rdi
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_eth_acc_mep_tx_rdi_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_eth_acc_mep_tx_rdi_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_MEP_TX_RDI_SET_ACC_MEP_TX_RDI_SET_RDI_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_ACC_MEP_TX_RDI_SET_ACC_MEP_TX_RDI_SET_RDI_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rdi = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = pcp_oam_eth_acc_mep_tx_rdi_set(
          unit,
          prm_acc_mep_ndx,
          prm_rdi
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_eth_acc_mep_tx_rdi_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_eth_acc_mep_tx_rdi_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: acc_mep_tx_rdi_get (section eth)
 ********************************************************************/
int 
  ui_pcp_oam_api_eth_acc_mep_tx_rdi_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  PCP_OAM_ETH_ACC_MEP_ID   
    prm_acc_mep_ndx;
  uint8   
    prm_rdi;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_eth"); 
  soc_sand_proc_name = "pcp_oam_eth_acc_mep_tx_rdi_get"; 
 
  unit = pcp_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_MEP_TX_RDI_GET_ACC_MEP_TX_RDI_GET_ACC_MEP_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_ACC_MEP_TX_RDI_GET_ACC_MEP_TX_RDI_GET_ACC_MEP_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_acc_mep_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter acc_mep_ndx after acc_mep_tx_rdi_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = pcp_oam_eth_acc_mep_tx_rdi_get(
          unit,
          prm_acc_mep_ndx,
          &prm_rdi
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_eth_acc_mep_tx_rdi_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_eth_acc_mep_tx_rdi_get");   
    goto exit; 
  } 

  cli_out("rdi: %u\n\r",prm_rdi);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: mep_delay_measurement_get (section eth)
 ********************************************************************/
int 
  ui_pcp_oam_api_eth_mep_delay_measurement_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  PCP_OAM_ETH_ACC_DLM_ID   
    prm_acc_dlm_ndx;
  uint32   
    prm_dm;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_eth"); 
  soc_sand_proc_name = "pcp_oam_eth_mep_delay_measurement_get"; 
 
  unit = pcp_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_MEP_DELAY_MEASUREMENT_GET_MEP_DELAY_MEASUREMENT_GET_ACC_DLM_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_MEP_DELAY_MEASUREMENT_GET_MEP_DELAY_MEASUREMENT_GET_ACC_DLM_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_acc_dlm_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter acc_dlm_ndx after mep_delay_measurement_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = pcp_oam_eth_mep_delay_measurement_get(
          unit,
          prm_acc_dlm_ndx,
          &prm_dm
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_eth_mep_delay_measurement_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_eth_mep_delay_measurement_get");   
    goto exit; 
  } 

  cli_out("dm: %u\n\r",prm_dm);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: mep_loss_measurement_get (section eth)
 ********************************************************************/
int 
  ui_pcp_oam_api_eth_mep_loss_measurement_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  PCP_OAM_ETH_ACC_LMM_ID   
    prm_acc_lmm_ndx;
  PCP_OAM_ETH_ACC_MEP_LM_CNT_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_eth"); 
  soc_sand_proc_name = "pcp_oam_eth_mep_loss_measurement_get"; 
 
  unit = pcp_get_default_unit(); 
  PCP_OAM_ETH_ACC_MEP_LM_CNT_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_MEP_LOSS_MEASUREMENT_GET_MEP_LOSS_MEASUREMENT_GET_ACC_LMM_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_ETH_MEP_LOSS_MEASUREMENT_GET_MEP_LOSS_MEASUREMENT_GET_ACC_LMM_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_acc_lmm_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter acc_lmm_ndx after mep_loss_measurement_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = pcp_oam_eth_mep_loss_measurement_get(
          unit,
          prm_acc_lmm_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_eth_mep_loss_measurement_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_eth_mep_loss_measurement_get");   
    goto exit; 
  } 

  PCP_OAM_ETH_ACC_MEP_LM_CNT_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
#endif
#ifdef UI_ETH/* { eth*/
/******************************************************************** 
 *  Section handler: eth
 ********************************************************************/ 
int 
  ui_pcp_oam_api_eth( 
    CURRENT_LINE *current_line 
  ) 
{   
  int
    ret = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_eth"); 
 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_GENERAL_INFO_SET_GENERAL_INFO_SET_ID,1)) 
  { 
    ret = ui_pcp_oam_api_eth_general_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_GENERAL_INFO_GET_GENERAL_INFO_GET_ID,1)) 
  { 
    ret = ui_pcp_oam_api_eth_general_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_COS_MAPPING_INFO_SET_COS_MAPPING_INFO_SET_ID,1)) 
  { 
    ret = ui_pcp_oam_api_eth_cos_mapping_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_COS_MAPPING_INFO_GET_COS_MAPPING_INFO_GET_ID,1)) 
  { 
    ret = ui_pcp_oam_api_eth_cos_mapping_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MP_INFO_SET_OAM_MP_INFO_SET_ID,1)) 
  { 
    ret = ui_pcp_oam_api_eth_oam_mp_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MP_INFO_GET_OAM_MP_INFO_GET_ID,1)) 
  { 
    ret = ui_pcp_oam_api_eth_oam_mp_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_MEP_INFO_SET_ACC_MEP_INFO_SET_ID,1)) 
  { 
    ret = ui_pcp_oam_api_eth_acc_mep_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_MEP_INFO_GET_ACC_MEP_INFO_GET_ID,1)) 
  { 
    ret = ui_pcp_oam_api_eth_acc_mep_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_MEP_CUSTOM_HEADER_SET_ACC_MEP_CUSTOM_HEADER_SET_ID,1)) 
  { 
    ret = ui_pcp_oam_api_eth_acc_mep_custom_header_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_MEP_CUSTOM_HEADER_GET_ACC_MEP_CUSTOM_HEADER_GET_ID,1)) 
  { 
    ret = ui_pcp_oam_api_eth_acc_mep_custom_header_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_LMM_SET_ACC_LMM_SET_ID,1)) 
  { 
    ret = ui_pcp_oam_api_eth_acc_lmm_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_LMM_GET_ACC_LMM_GET_ID,1)) 
  { 
    ret = ui_pcp_oam_api_eth_acc_lmm_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_DLM_SET_ACC_DLM_SET_ID,1)) 
  { 
    ret = ui_pcp_oam_api_eth_acc_dlm_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_DLM_GET_ACC_DLM_GET_ID,1)) 
  { 
    ret = ui_pcp_oam_api_eth_acc_dlm_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_RMEP_ADD_RMEP_ADD_ID,1)) 
  { 
    ret = ui_pcp_oam_api_eth_rmep_add(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_RMEP_REMOVE_RMEP_REMOVE_ID,1)) 
  { 
    ret = ui_pcp_oam_api_eth_rmep_remove(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_RMEP_INFO_SET_RMEP_INFO_SET_ID,1)) 
  { 
    ret = ui_pcp_oam_api_eth_rmep_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_RMEP_INFO_GET_RMEP_INFO_GET_ID,1)) 
  { 
    ret = ui_pcp_oam_api_eth_rmep_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_MEP_TX_RDI_SET_ACC_MEP_TX_RDI_SET_ID,1)) 
  { 
    ret = ui_pcp_oam_api_eth_acc_mep_tx_rdi_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_ACC_MEP_TX_RDI_GET_ACC_MEP_TX_RDI_GET_ID,1)) 
  { 
    ret = ui_pcp_oam_api_eth_acc_mep_tx_rdi_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_MEP_DELAY_MEASUREMENT_GET_MEP_DELAY_MEASUREMENT_GET_ID,1)) 
  { 
    ret = ui_pcp_oam_api_eth_mep_delay_measurement_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_ETH_MEP_LOSS_MEASUREMENT_GET_MEP_LOSS_MEASUREMENT_GET_ID,1)) 
  { 
    ret = ui_pcp_oam_api_eth_mep_loss_measurement_get(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after eth***", TRUE); 
  }
  
  ui_ret = ret; 
    
  goto exit; 
exit:        
  return ui_ret; 
}                
          
#endif /* eth */ 


#endif /* LINK_PCP_LIBRARIES */ 

