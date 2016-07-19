/* $Id: ui_pb_api_egr_acl.c,v 1.7 Broadcom SDK $
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
#include <appl/dpp/UserInterface/ui_pure_defi_pb_pp_acc.h>
#include <appl/dpp/UserInterface/ui_pure_defi_pb_api.h> 
 
  
#include <soc/dpp/Petra/PB_TM/pb_egr_acl.h>
#include <appl/dpp/UserInterface/pb/ui_pb_api_egr_acl.h>

#if LINK_PB_LIBRARIES

#ifdef UI_EGR_ACL
/******************************************************************** 
 *  Function handler: dp_values_set (section egr_acl)
 ********************************************************************/
int 
  ui_pb_api_egr_acl_dp_values_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32 
    prm_val_index = 0xFFFFFFFF;  
  SOC_PB_EGR_ACL_DP_VALUES   
    prm_dp_val;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_egr_acl"); 
  soc_sand_proc_name = "soc_pb_egr_acl_dp_values_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_EGR_ACL_DP_VALUES_clear(&prm_dp_val);
 
  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_egr_acl_dp_values_get_unsafe(
          unit,
          &prm_dp_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_egr_acl_dp_values_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_egr_acl_dp_values_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_DP_VALUES_SET_DP_VALUES_SET_DP_VAL_VAL_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_ACL_DP_VALUES_SET_DP_VALUES_SET_DP_VAL_VAL_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_val_index = (uint32)param_val->value.ulong_value;
  } 

  if(prm_val_index != 0xFFFFFFFF)          
  { 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_DP_VALUES_SET_DP_VALUES_SET_DP_VAL_VAL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_ACL_DP_VALUES_SET_DP_VALUES_SET_DP_VAL_VAL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_dp_val.val[ prm_val_index] = (uint32)param_val->value.ulong_value;
  } 

  }   


  /* Call function */
  ret = soc_pb_egr_acl_dp_values_set_unsafe(
          unit,
          &prm_dp_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_egr_acl_dp_values_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_egr_acl_dp_values_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: dp_values_get (section egr_acl)
 ********************************************************************/
int 
  ui_pb_api_egr_acl_dp_values_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_EGR_ACL_DP_VALUES   
    prm_dp_val;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_egr_acl"); 
  soc_sand_proc_name = "soc_pb_egr_acl_dp_values_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_EGR_ACL_DP_VALUES_clear(&prm_dp_val);
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_pb_egr_acl_dp_values_get_unsafe(
          unit,
          &prm_dp_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_egr_acl_dp_values_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_egr_acl_dp_values_get");   
    goto exit; 
  } 

  SOC_PB_EGR_ACL_DP_VALUES_print(&prm_dp_val);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: l4_protocol_code_set (section egr_acl)
 ********************************************************************/
int 
  ui_pb_api_egr_acl_l4_protocol_code_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_code_ndx;
  uint32   
    prm_protocol_code;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_egr_acl"); 
  soc_sand_proc_name = "soc_pb_egr_acl_l4_protocol_code_set"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_L4_PROTOCOL_CODE_SET_L4_PROTOCOL_CODE_SET_CODE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_ACL_L4_PROTOCOL_CODE_SET_L4_PROTOCOL_CODE_SET_CODE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_code_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter code_ndx after l4_protocol_code_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_egr_acl_l4_protocol_code_get_unsafe(
          unit,
          prm_code_ndx,
          &prm_protocol_code
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_egr_acl_l4_protocol_code_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_egr_acl_l4_protocol_code_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_L4_PROTOCOL_CODE_SET_L4_PROTOCOL_CODE_SET_PROTOCOL_CODE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_ACL_L4_PROTOCOL_CODE_SET_L4_PROTOCOL_CODE_SET_PROTOCOL_CODE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_protocol_code = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_egr_acl_l4_protocol_code_set_unsafe(
          unit,
          prm_code_ndx,
          prm_protocol_code
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_egr_acl_l4_protocol_code_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_egr_acl_l4_protocol_code_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: l4_protocol_code_get (section egr_acl)
 ********************************************************************/
int 
  ui_pb_api_egr_acl_l4_protocol_code_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_code_ndx;
  uint32   
    prm_protocol_code;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_egr_acl"); 
  soc_sand_proc_name = "soc_pb_egr_acl_l4_protocol_code_get"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_L4_PROTOCOL_CODE_GET_L4_PROTOCOL_CODE_GET_CODE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_ACL_L4_PROTOCOL_CODE_GET_L4_PROTOCOL_CODE_GET_CODE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_code_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter code_ndx after l4_protocol_code_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_egr_acl_l4_protocol_code_get_unsafe(
          unit,
          prm_code_ndx,
          &prm_protocol_code
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_egr_acl_l4_protocol_code_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_egr_acl_l4_protocol_code_get");   
    goto exit; 
  } 

  soc_sand_os_printf("protocol_code: %u\n\r",prm_protocol_code);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: port_set (section egr_acl)
 ********************************************************************/
int 
  ui_pb_api_egr_acl_port_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_pp_port_ndx;
  SOC_PB_EGR_ACL_PORT_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_egr_acl"); 
  soc_sand_proc_name = "soc_pb_egr_acl_port_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_EGR_ACL_PORT_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_PORT_SET_PORT_SET_PP_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_ACL_PORT_SET_PORT_SET_PP_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_pp_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter pp_port_ndx after port_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_egr_acl_port_get_unsafe(
          unit,
          prm_pp_port_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_egr_acl_port_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_egr_acl_port_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_PORT_SET_PORT_SET_INFO_ACL_DATA_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_ACL_PORT_SET_PORT_SET_INFO_ACL_DATA_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.acl_data = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_PORT_SET_PORT_SET_INFO_PROFILE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_ACL_PORT_SET_PORT_SET_INFO_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.profile = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_egr_acl_port_set_unsafe(
          unit,
          prm_pp_port_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_egr_acl_port_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_egr_acl_port_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: port_get (section egr_acl)
 ********************************************************************/
int 
  ui_pb_api_egr_acl_port_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_pp_port_ndx;
  SOC_PB_EGR_ACL_PORT_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_egr_acl"); 
  soc_sand_proc_name = "soc_pb_egr_acl_port_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_EGR_ACL_PORT_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_PORT_GET_PORT_GET_PP_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_ACL_PORT_GET_PORT_GET_PP_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_pp_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter pp_port_ndx after port_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_egr_acl_port_get_unsafe(
          unit,
          prm_pp_port_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_egr_acl_port_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_egr_acl_port_get");   
    goto exit; 
  } 

  SOC_PB_EGR_ACL_PORT_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: key_profile_map_set (section egr_acl)
 ********************************************************************/
int 
  ui_pb_api_egr_acl_key_profile_map_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_PKT_FRWRD_TYPE   
    prm_fwd_type;
  uint32   
    prm_acl_profile_ndx;
  uint32   
    prm_key_profile;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_egr_acl"); 
  soc_sand_proc_name = "soc_pb_egr_acl_key_profile_map_set"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_KEY_PROFILE_MAP_SET_KEY_PROFILE_MAP_SET_ACL_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_ACL_KEY_PROFILE_MAP_SET_KEY_PROFILE_MAP_SET_ACL_PROFILE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_acl_profile_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter acl_profile_ndx after key_profile_map_set***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_KEY_PROFILE_MAP_SET_KEY_PROFILE_MAP_SET_FWD_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_EGR_ACL_KEY_PROFILE_MAP_SET_KEY_PROFILE_MAP_SET_FWD_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fwd_type = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter fwd_type after key_profile_map_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_egr_acl_key_profile_map_get_unsafe(
          unit,
          prm_fwd_type,
          prm_acl_profile_ndx,
          &prm_key_profile
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_egr_acl_key_profile_map_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_egr_acl_key_profile_map_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_KEY_PROFILE_MAP_SET_KEY_PROFILE_MAP_SET_KEY_PROFILE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_ACL_KEY_PROFILE_MAP_SET_KEY_PROFILE_MAP_SET_KEY_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key_profile = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_egr_acl_key_profile_map_set_unsafe(
          unit,
          prm_fwd_type,
          prm_acl_profile_ndx,
          prm_key_profile
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_egr_acl_key_profile_map_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_egr_acl_key_profile_map_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: key_profile_map_get (section egr_acl)
 ********************************************************************/
int 
  ui_pb_api_egr_acl_key_profile_map_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_PKT_FRWRD_TYPE   
    prm_fwd_type;
  uint32   
    prm_acl_profile_ndx;
  uint32   
    prm_key_profile;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_egr_acl"); 
  soc_sand_proc_name = "soc_pb_egr_acl_key_profile_map_get"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_KEY_PROFILE_MAP_GET_KEY_PROFILE_MAP_GET_ACL_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_ACL_KEY_PROFILE_MAP_GET_KEY_PROFILE_MAP_GET_ACL_PROFILE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_acl_profile_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter acl_profile_ndx after key_profile_map_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_KEY_PROFILE_MAP_GET_KEY_PROFILE_MAP_GET_FWD_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_EGR_ACL_KEY_PROFILE_MAP_GET_KEY_PROFILE_MAP_GET_FWD_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fwd_type = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter fwd_type after key_profile_map_set***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_egr_acl_key_profile_map_get_unsafe(
          unit,
          prm_fwd_type,
          prm_acl_profile_ndx,
          &prm_key_profile
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_egr_acl_key_profile_map_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_egr_acl_key_profile_map_get");   
    goto exit; 
  } 

  soc_sand_os_printf("fwd_type: %s\n\r",SOC_TMC_PKT_FRWRD_TYPE_to_string(prm_fwd_type));

  soc_sand_os_printf("db_id: %u\n\r",prm_key_profile);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
    
/******************************************************************** 
 *  Function handler: db_create (section egr_acl)
 ********************************************************************/
int 
  ui_pb_api_egr_acl_db_create(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_db_ndx;
  SOC_PB_EGR_ACL_DB_TYPE   
    prm_db_type = 0;
  SOC_SAND_SUCCESS_FAILURE   
    prm_success;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_egr_acl"); 
  soc_sand_proc_name = "soc_pb_egr_acl_db_create"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_DB_CREATE_DB_CREATE_DB_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_ACL_DB_CREATE_DB_CREATE_DB_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_db_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter db_ndx after db_create***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_DB_CREATE_DB_CREATE_DB_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_EGR_ACL_DB_CREATE_DB_CREATE_DB_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_db_type = param_val->numeric_equivalent;
  } 


  /* Call function */
  ret = soc_pb_egr_acl_db_create_unsafe(
          unit,
          prm_db_ndx,
          prm_db_ndx,
          prm_db_type,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_egr_acl_db_create - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_egr_acl_db_create");   
    goto exit; 
  } 

  soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: db_get (section egr_acl)
 ********************************************************************/
int 
  ui_pb_api_egr_acl_db_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_db_ndx;
  SOC_PB_EGR_ACL_DB_TYPE   
    prm_db_type;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_egr_acl"); 
  soc_sand_proc_name = "soc_pb_egr_acl_db_get"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_DB_GET_DB_GET_DB_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_ACL_DB_GET_DB_GET_DB_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_db_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter db_ndx after db_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_egr_acl_db_get_unsafe(
          unit,
          prm_db_ndx,
          &prm_db_type
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_egr_acl_db_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_egr_acl_db_get");   
    goto exit; 
  } 

  soc_sand_os_printf("db_type: %s\n\r",SOC_PB_EGR_ACL_DB_TYPE_to_string(prm_db_type));

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: db_destroy (section egr_acl)
 ********************************************************************/
int 
  ui_pb_api_egr_acl_db_destroy(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_db_ndx;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_egr_acl"); 
  soc_sand_proc_name = "soc_pb_egr_acl_db_destroy"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_DB_DESTROY_DB_DESTROY_DB_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_ACL_DB_DESTROY_DB_DESTROY_DB_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_db_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter db_ndx after db_destroy***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_egr_acl_db_destroy_unsafe(
          unit,
          prm_db_ndx,
          prm_db_ndx
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_egr_acl_db_destroy - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_egr_acl_db_destroy");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: entry_add (section egr_acl)
 ********************************************************************/
int 
  ui_pb_api_egr_acl_entry_add(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_db_ndx;
  uint32   
    prm_val_index = 0xFFFFFFFF,
    prm_mask_index = 0xFFFFFFFF,
    prm_entry_ndx;
  SOC_PB_EGR_ACL_ENTRY_INFO   
    prm_info;
  SOC_SAND_SUCCESS_FAILURE   
    prm_success;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_egr_acl"); 
  soc_sand_proc_name = "soc_pb_egr_acl_entry_add"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_EGR_ACL_ENTRY_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_ENTRY_ADD_ENTRY_ADD_DB_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_ACL_ENTRY_ADD_ENTRY_ADD_DB_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_db_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter db_ndx after entry_add***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_ENTRY_ADD_ENTRY_ADD_ENTRY_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_ACL_ENTRY_ADD_ENTRY_ADD_ENTRY_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_entry_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter entry_ndx after entry_add***", TRUE); 
    goto exit; 
  } 

  prm_info.key.size = SOC_PB_TCAM_BANK_ENTRY_SIZE_144_BITS;
  
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_ENTRY_ADD_ENTRY_ADD_INFO_ACTION_VAL_CUD_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_ACL_ENTRY_ADD_ENTRY_ADD_INFO_ACTION_VAL_CUD_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.action_val.cud = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_ENTRY_ADD_ENTRY_ADD_INFO_ACTION_VAL_DP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_ACL_ENTRY_ADD_ENTRY_ADD_INFO_ACTION_VAL_DP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.action_val.dp = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_ENTRY_ADD_ENTRY_ADD_INFO_ACTION_VAL_TC_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_ACL_ENTRY_ADD_ENTRY_ADD_INFO_ACTION_VAL_TC_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.action_val.tc = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_ENTRY_ADD_ENTRY_ADD_INFO_ACTION_VAL_DP_TC_OV_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_ACL_ENTRY_ADD_ENTRY_ADD_INFO_ACTION_VAL_DP_TC_OV_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.action_val.dp_tc_ov = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_ENTRY_ADD_ENTRY_ADD_INFO_ACTION_VAL_OFP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_ACL_ENTRY_ADD_ENTRY_ADD_INFO_ACTION_VAL_OFP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.action_val.ofp = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_ENTRY_ADD_ENTRY_ADD_INFO_ACTION_VAL_OFP_OV_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_ACL_ENTRY_ADD_ENTRY_ADD_INFO_ACTION_VAL_OFP_OV_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.action_val.ofp_ov = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_ENTRY_ADD_ENTRY_ADD_INFO_ACTION_VAL_TRAP_CODE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_ACL_ENTRY_ADD_ENTRY_ADD_INFO_ACTION_VAL_TRAP_CODE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.action_val.trap_code = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_ENTRY_ADD_ENTRY_ADD_INFO_ACTION_VAL_TRAP_EN_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_ACL_ENTRY_ADD_ENTRY_ADD_INFO_ACTION_VAL_TRAP_EN_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.action_val.trap_en = (uint8)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_ENTRY_ADD_ENTRY_ADD_INFO_PRIORITY_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_ACL_ENTRY_ADD_ENTRY_ADD_INFO_PRIORITY_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.priority = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_ENTRY_ADD_ENTRY_ADD_INFO_KEY_FORMAT_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_EGR_ACL_ENTRY_ADD_ENTRY_ADD_INFO_KEY_FORMAT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.key.format.type = SOC_PB_TCAM_KEY_FORMAT_TYPE_EGR_ACL;
    prm_info.key.format.egr_acl = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_ENTRY_ADD_ENTRY_ADD_INFO_KEY_DATA_MASK_INDEX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_ACL_ENTRY_ADD_ENTRY_ADD_INFO_KEY_DATA_MASK_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_mask_index = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_ENTRY_ADD_ENTRY_ADD_INFO_KEY_DATA_MASK_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_ACL_ENTRY_ADD_ENTRY_ADD_INFO_KEY_DATA_MASK_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_info.key.data.mask[ prm_mask_index] = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_ENTRY_ADD_ENTRY_ADD_INFO_KEY_DATA_VAL_INDEX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_ACL_ENTRY_ADD_ENTRY_ADD_INFO_KEY_DATA_VAL_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_val_index = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_ENTRY_ADD_ENTRY_ADD_INFO_KEY_DATA_VAL_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_ACL_ENTRY_ADD_ENTRY_ADD_INFO_KEY_DATA_VAL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_info.key.data.val[ prm_val_index] = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_ENTRY_ADD_ENTRY_ADD_INFO_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_EGR_ACL_ENTRY_ADD_ENTRY_ADD_INFO_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.type = param_val->numeric_equivalent;
  } 


  /* Call function */
  ret = soc_pb_egr_acl_entry_add_unsafe(
          unit,
          prm_db_ndx,
          prm_entry_ndx,
          &prm_info,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_egr_acl_entry_add - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_egr_acl_entry_add");   
    goto exit; 
  } 

  soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success);
  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: entry_get (section egr_acl)
 ********************************************************************/
int 
  ui_pb_api_egr_acl_entry_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_db_ndx;
  uint32   
    prm_entry_ndx;
  SOC_PB_EGR_ACL_ENTRY_INFO   
    prm_info;
  uint8   
    prm_is_found;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_egr_acl"); 
  soc_sand_proc_name = "soc_pb_egr_acl_entry_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_EGR_ACL_ENTRY_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_ENTRY_GET_ENTRY_GET_DB_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_ACL_ENTRY_GET_ENTRY_GET_DB_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_db_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter db_ndx after entry_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_ENTRY_GET_ENTRY_GET_ENTRY_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_ACL_ENTRY_GET_ENTRY_GET_ENTRY_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_entry_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter entry_ndx after entry_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_egr_acl_entry_get_unsafe(
          unit,
          prm_db_ndx,
          prm_entry_ndx,
          &prm_info,
          &prm_is_found
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_egr_acl_entry_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_egr_acl_entry_get");   
    goto exit; 
  } 

  SOC_PB_EGR_ACL_ENTRY_INFO_print(&prm_info);

  soc_sand_os_printf("is_found: %u\n\r",prm_is_found);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: entry_remove (section egr_acl)
 ********************************************************************/
int 
  ui_pb_api_egr_acl_entry_remove(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_db_ndx;
  uint32   
    prm_entry_ndx;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_egr_acl"); 
  soc_sand_proc_name = "soc_pb_egr_acl_entry_remove"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_ENTRY_REMOVE_ENTRY_REMOVE_DB_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_ACL_ENTRY_REMOVE_ENTRY_REMOVE_DB_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_db_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter db_ndx after entry_remove***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_ENTRY_REMOVE_ENTRY_REMOVE_ENTRY_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_ACL_ENTRY_REMOVE_ENTRY_REMOVE_ENTRY_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_entry_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter entry_ndx after entry_remove***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_egr_acl_entry_remove_unsafe(
          unit,
          prm_db_ndx,
          prm_entry_ndx
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_egr_acl_entry_remove - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_egr_acl_entry_remove");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
#endif
#ifdef UI_EGR_ACL/* { egr_acl*/
/******************************************************************** 
 *  Section handler: egr_acl
 ********************************************************************/ 
int 
  ui_pb_api_egr_acl( 
    CURRENT_LINE *current_line 
  ) 
{   
  int
    ret = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_pb_api_egr_acl"); 
 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_DP_VALUES_SET_DP_VALUES_SET_ID,1)) 
  { 
    ret = ui_pb_api_egr_acl_dp_values_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_DP_VALUES_GET_DP_VALUES_GET_ID,1)) 
  { 
    ret = ui_pb_api_egr_acl_dp_values_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_L4_PROTOCOL_CODE_SET_L4_PROTOCOL_CODE_SET_ID,1)) 
  { 
    ret = ui_pb_api_egr_acl_l4_protocol_code_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_L4_PROTOCOL_CODE_GET_L4_PROTOCOL_CODE_GET_ID,1)) 
  { 
    ret = ui_pb_api_egr_acl_l4_protocol_code_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_PORT_SET_PORT_SET_ID,1)) 
  { 
    ret = ui_pb_api_egr_acl_port_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_PORT_GET_PORT_GET_ID,1)) 
  { 
    ret = ui_pb_api_egr_acl_port_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_KEY_PROFILE_MAP_SET_KEY_PROFILE_MAP_SET_ID,1)) 
  { 
    ret = ui_pb_api_egr_acl_key_profile_map_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_KEY_PROFILE_MAP_GET_KEY_PROFILE_MAP_GET_ID,1)) 
  { 
    ret = ui_pb_api_egr_acl_key_profile_map_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_DB_CREATE_DB_CREATE_ID,1)) 
  { 
    ret = ui_pb_api_egr_acl_db_create(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_DB_GET_DB_GET_ID,1)) 
  { 
    ret = ui_pb_api_egr_acl_db_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_DB_DESTROY_DB_DESTROY_ID,1)) 
  { 
    ret = ui_pb_api_egr_acl_db_destroy(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_ENTRY_ADD_ENTRY_ADD_ID,1)) 
  { 
    ret = ui_pb_api_egr_acl_entry_add(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_ENTRY_GET_ENTRY_GET_ID,1)) 
  { 
    ret = ui_pb_api_egr_acl_entry_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_ACL_ENTRY_REMOVE_ENTRY_REMOVE_ID,1)) 
  { 
    ret = ui_pb_api_egr_acl_entry_remove(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after egr_acl***", TRUE); 
  }
  
  ui_ret = ret; 
    
  goto exit; 
exit:        
  return ui_ret; 
}                
          
#endif /* egr_acl */ 


#endif /* LINK_PB_LIBRARIES */ 

