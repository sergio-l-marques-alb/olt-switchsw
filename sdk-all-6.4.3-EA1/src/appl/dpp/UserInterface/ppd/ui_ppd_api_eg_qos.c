/* $Id: ui_ppd_api_eg_qos.c,v 1.11 Broadcom SDK $
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
/* 
 * Utilities include file. 
 */ 
#include <shared/bsl.h>  
#include <soc/dpp/SAND/Utils/sand_os_interface.h> 
#include <appl/diag/dpp/utils_defi.h> 
#include <appl/dpp/UserInterface/ui_defi.h> 
 
#include <appl/dpp/UserInterface/ui_pure_defi_ppd_api.h> 

  
#include <soc/dpp/PPD/ppd_api_eg_qos.h>                                                               
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_eg_qos.h>

#if LINK_PPD_LIBRARIES

#ifdef UI_EG_QOS
/******************************************************************** 
 *  Function handler: port_info_set (section eg_qos)
 ********************************************************************/
int 
  ui_ppd_api_eg_qos_port_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPD_PORT   
    prm_local_port_ndx;
  SOC_PPD_EG_QOS_PORT_INFO   
    prm_port_qos_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_qos"); 
  soc_sand_proc_name = "soc_ppd_eg_qos_port_info_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPD_EG_QOS_PORT_INFO_clear(&prm_port_qos_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_QOS_PORT_INFO_SET_PORT_INFO_SET_LOCAL_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_QOS_PORT_INFO_SET_PORT_INFO_SET_LOCAL_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_local_port_ndx = (SOC_PPD_PORT)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter local_port_ndx after port_info_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_eg_qos_port_info_get(
          unit,
          prm_local_port_ndx,
          &prm_port_qos_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_qos_port_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_qos_port_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_QOS_PORT_INFO_SET_PORT_INFO_SET_PORT_QOS_INFO_EXP_MAP_PROFILE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_QOS_PORT_INFO_SET_PORT_INFO_SET_PORT_QOS_INFO_EXP_MAP_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_qos_info.exp_map_profile = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_eg_qos_port_info_set(
          unit,
          prm_local_port_ndx,
          &prm_port_qos_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_qos_port_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_qos_port_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: port_info_get (section eg_qos)
 ********************************************************************/
int 
  ui_ppd_api_eg_qos_port_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPD_PORT   
    prm_local_port_ndx;
  SOC_PPD_EG_QOS_PORT_INFO   
    prm_port_qos_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_qos"); 
  soc_sand_proc_name = "soc_ppd_eg_qos_port_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPD_EG_QOS_PORT_INFO_clear(&prm_port_qos_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_QOS_PORT_INFO_GET_PORT_INFO_GET_LOCAL_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_QOS_PORT_INFO_GET_PORT_INFO_GET_LOCAL_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_local_port_ndx = (SOC_PPD_PORT)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter local_port_ndx after port_info_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_eg_qos_port_info_get(
          unit,
          prm_local_port_ndx,
          &prm_port_qos_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_qos_port_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_qos_port_info_get");   
    goto exit; 
  } 

  SOC_PPD_EG_QOS_PORT_INFO_print(&prm_port_qos_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: params_php_remark_set (section eg_qos)
 ********************************************************************/
int 
  ui_ppd_api_eg_qos_params_php_remark_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPD_EG_QOS_PHP_REMARK_KEY   
    prm_php_key;
  uint32   
    prm_dscp_exp;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_qos"); 
  soc_sand_proc_name = "soc_ppd_eg_qos_params_php_remark_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPD_EG_QOS_PHP_REMARK_KEY_clear(&prm_php_key);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_QOS_PARAMS_PHP_REMARK_SET_PARAMS_PHP_REMARK_SET_PHP_KEY_EXP_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_QOS_PARAMS_PHP_REMARK_SET_PARAMS_PHP_REMARK_SET_PHP_KEY_EXP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_php_key.exp = (SOC_SAND_PP_MPLS_EXP)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_QOS_PARAMS_PHP_REMARK_SET_PARAMS_PHP_REMARK_SET_PHP_KEY_PHP_TYPE_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_EG_QOS_PARAMS_PHP_REMARK_SET_PARAMS_PHP_REMARK_SET_PHP_KEY_PHP_TYPE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_php_key.php_type = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_QOS_PARAMS_PHP_REMARK_SET_PARAMS_PHP_REMARK_SET_PHP_KEY_EXP_MAP_PROFILE_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_QOS_PARAMS_PHP_REMARK_SET_PARAMS_PHP_REMARK_SET_PHP_KEY_EXP_MAP_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_php_key.exp_map_profile = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter php_key after params_php_remark_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_eg_qos_params_php_remark_get(
          unit,
          &prm_php_key,
          &prm_dscp_exp
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_qos_params_php_remark_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_qos_params_php_remark_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_QOS_PARAMS_PHP_REMARK_SET_PARAMS_PHP_REMARK_SET_DSCP_EXP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_QOS_PARAMS_PHP_REMARK_SET_PARAMS_PHP_REMARK_SET_DSCP_EXP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_dscp_exp = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_eg_qos_params_php_remark_set(
          unit,
          &prm_php_key,
          prm_dscp_exp
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_qos_params_php_remark_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_qos_params_php_remark_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: params_php_remark_get (section eg_qos)
 ********************************************************************/
int 
  ui_ppd_api_eg_qos_params_php_remark_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPD_EG_QOS_PHP_REMARK_KEY   
    prm_php_key;
  uint32   
    prm_dscp_exp;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_qos"); 
  soc_sand_proc_name = "soc_ppd_eg_qos_params_php_remark_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPD_EG_QOS_PHP_REMARK_KEY_clear(&prm_php_key);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_QOS_PARAMS_PHP_REMARK_GET_PARAMS_PHP_REMARK_GET_PHP_KEY_EXP_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_QOS_PARAMS_PHP_REMARK_GET_PARAMS_PHP_REMARK_GET_PHP_KEY_EXP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_php_key.exp = (SOC_SAND_PP_MPLS_EXP)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_QOS_PARAMS_PHP_REMARK_GET_PARAMS_PHP_REMARK_GET_PHP_KEY_PHP_TYPE_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_EG_QOS_PARAMS_PHP_REMARK_GET_PARAMS_PHP_REMARK_GET_PHP_KEY_PHP_TYPE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_php_key.php_type = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_QOS_PARAMS_PHP_REMARK_GET_PARAMS_PHP_REMARK_GET_PHP_KEY_EXP_MAP_PROFILE_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_QOS_PARAMS_PHP_REMARK_GET_PARAMS_PHP_REMARK_GET_PHP_KEY_EXP_MAP_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_php_key.exp_map_profile = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter php_key after params_php_remark_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_eg_qos_params_php_remark_get(
          unit,
          &prm_php_key,
          &prm_dscp_exp
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_qos_params_php_remark_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_qos_params_php_remark_get");   
    goto exit; 
  } 

  cli_out("dscp_exp: %u\n\r",prm_dscp_exp);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: params_remark_set (section eg_qos)
 ********************************************************************/
int 
  ui_ppd_api_eg_qos_params_remark_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPD_EG_QOS_MAP_KEY   
    prm_in_qos_key;
  SOC_PPD_EG_QOS_PARAMS   
    prm_out_qos_params;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_qos"); 
  soc_sand_proc_name = "soc_ppd_eg_qos_params_remark_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPD_EG_QOS_MAP_KEY_clear(&prm_in_qos_key);
  SOC_PPD_EG_QOS_PARAMS_clear(&prm_out_qos_params);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_QOS_PARAMS_REMARK_SET_PARAMS_REMARK_SET_IN_QOS_KEY_DP_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_QOS_PARAMS_REMARK_SET_PARAMS_REMARK_SET_IN_QOS_KEY_DP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_in_qos_key.dp = (SOC_SAND_PP_DP)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_QOS_PARAMS_REMARK_SET_PARAMS_REMARK_SET_IN_QOS_KEY_IN_DSCP_EXP_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_QOS_PARAMS_REMARK_SET_PARAMS_REMARK_SET_IN_QOS_KEY_IN_DSCP_EXP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_in_qos_key.in_dscp_exp = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter in_qos_key after params_remark_set***", TRUE); 
    goto exit; 
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_QOS_PARAMS_REMARK_SET_PARAMS_REMARK_SET_IN_QOS_KEY_REMARK_PROFILE_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_QOS_PARAMS_REMARK_SET_PARAMS_REMARK_SET_IN_QOS_KEY_REMARK_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_in_qos_key.remark_profile = (uint32)param_val->value.ulong_value;
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_eg_qos_params_remark_get(
          unit,
          &prm_in_qos_key,
          &prm_out_qos_params
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_qos_params_remark_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_qos_params_remark_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_QOS_PARAMS_REMARK_SET_PARAMS_REMARK_SET_OUT_QOS_PARAMS_MPLS_EXP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_QOS_PARAMS_REMARK_SET_PARAMS_REMARK_SET_OUT_QOS_PARAMS_MPLS_EXP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_out_qos_params.mpls_exp = (SOC_SAND_PP_MPLS_EXP)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_QOS_PARAMS_REMARK_SET_PARAMS_REMARK_SET_OUT_QOS_PARAMS_IPV6_TC_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_QOS_PARAMS_REMARK_SET_PARAMS_REMARK_SET_OUT_QOS_PARAMS_IPV6_TC_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_out_qos_params.ipv6_tc = (SOC_SAND_PP_IPV6_TC)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_QOS_PARAMS_REMARK_SET_PARAMS_REMARK_SET_OUT_QOS_PARAMS_IPV4_TOS_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_QOS_PARAMS_REMARK_SET_PARAMS_REMARK_SET_OUT_QOS_PARAMS_IPV4_TOS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_out_qos_params.ipv4_tos = (SOC_SAND_PP_IPV4_TOS)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_eg_qos_params_remark_set(
          unit,
          &prm_in_qos_key,
          &prm_out_qos_params
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_qos_params_remark_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_qos_params_remark_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: params_remark_get (section eg_qos)
 ********************************************************************/
int 
  ui_ppd_api_eg_qos_params_remark_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPD_EG_QOS_MAP_KEY   
    prm_in_qos_key;
  SOC_PPD_EG_QOS_PARAMS   
    prm_out_qos_params;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_qos"); 
  soc_sand_proc_name = "soc_ppd_eg_qos_params_remark_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPD_EG_QOS_MAP_KEY_clear(&prm_in_qos_key);
  SOC_PPD_EG_QOS_PARAMS_clear(&prm_out_qos_params);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_QOS_PARAMS_REMARK_GET_PARAMS_REMARK_GET_IN_QOS_KEY_DP_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_QOS_PARAMS_REMARK_GET_PARAMS_REMARK_GET_IN_QOS_KEY_DP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_in_qos_key.dp = (SOC_SAND_PP_DP)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_QOS_PARAMS_REMARK_GET_PARAMS_REMARK_GET_IN_QOS_KEY_IN_DSCP_EXP_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_QOS_PARAMS_REMARK_GET_PARAMS_REMARK_GET_IN_QOS_KEY_IN_DSCP_EXP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_in_qos_key.in_dscp_exp = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter in_qos_key after params_remark_get***", TRUE); 
    goto exit; 
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_QOS_PARAMS_REMARK_GET_PARAMS_REMARK_GET_IN_QOS_KEY_REMARK_PROFILE_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_QOS_PARAMS_REMARK_GET_PARAMS_REMARK_GET_IN_QOS_KEY_REMARK_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_in_qos_key.remark_profile = (uint32)param_val->value.ulong_value;
  } 
  

  /* Call function */
  ret = soc_ppd_eg_qos_params_remark_get(
          unit,
          &prm_in_qos_key,
          &prm_out_qos_params
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_qos_params_remark_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_qos_params_remark_get");   
    goto exit; 
  } 

  SOC_PPD_EG_QOS_PARAMS_print(&prm_out_qos_params);

  
  goto exit; 
exit: 
  return ui_ret; 
} 

/******************************************************************** 
 *  Function handler: encap_params_remark_set (section eg_qos)
 ********************************************************************/
int 
  ui_ppd_api_eg_encap_qos_params_remark_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPD_EG_ENCAP_QOS_MAP_KEY   
    prm_in_encap_qos_key;
  SOC_PPD_EG_ENCAP_QOS_PARAMS   
    prm_out_encap_qos_params;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_qos"); 
  soc_sand_proc_name = "soc_ppd_eg_qos_params_remark_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPD_EG_ENCAP_QOS_MAP_KEY_clear(&prm_in_encap_qos_key);
  SOC_PPD_EG_ENCAP_QOS_PARAMS_clear(&prm_out_encap_qos_params);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_QOS_PARAMS_REMARK_SET_PARAMS_REMARK_SET_IN_ENCAP_QOS_KEY_PKT_HDR_TYPE_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_EG_ENCAP_QOS_PARAMS_REMARK_SET_PARAMS_REMARK_SET_IN_ENCAP_QOS_KEY_PKT_HDR_TYPE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_in_encap_qos_key.pkt_hdr_type = (SOC_PPD_PKT_HDR_TYPE)param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_QOS_PARAMS_REMARK_SET_PARAMS_REMARK_SET_IN_ENCAP_QOS_KEY_IN_DSCP_EXP_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_QOS_PARAMS_REMARK_SET_PARAMS_REMARK_SET_IN_ENCAP_QOS_KEY_IN_DSCP_EXP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_in_encap_qos_key.in_dscp_exp = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter in_qos_key after params_remark_set***", TRUE); 
    goto exit; 
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_QOS_PARAMS_REMARK_SET_PARAMS_REMARK_SET_IN_ENCAP_QOS_KEY_REMARK_PROFILE_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_QOS_PARAMS_REMARK_SET_PARAMS_REMARK_SET_IN_ENCAP_QOS_KEY_REMARK_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_in_encap_qos_key.remark_profile = (uint32)param_val->value.ulong_value;
  } 
  

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_eg_encap_qos_params_remark_get(
          unit,
          &prm_in_encap_qos_key,
          &prm_out_encap_qos_params
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_encap_qos_params_remark_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_enca_qos_params_remark_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_QOS_PARAMS_REMARK_SET_PARAMS_REMARK_SET_OUT_ENCAP_QOS_PARAMS_MPLS_EXP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_QOS_PARAMS_REMARK_SET_PARAMS_REMARK_SET_OUT_ENCAP_QOS_PARAMS_MPLS_EXP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_out_encap_qos_params.mpls_exp = (SOC_SAND_PP_MPLS_EXP)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_QOS_PARAMS_REMARK_SET_PARAMS_REMARK_SET_OUT_ENCAP_QOS_PARAMS_IPV4_TOS_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_QOS_PARAMS_REMARK_SET_PARAMS_REMARK_SET_OUT_ENCAP_QOS_PARAMS_IPV4_TOS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_out_encap_qos_params.ip_dscp = (SOC_SAND_PP_IPV4_TOS)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_eg_encap_qos_params_remark_set(
          unit,
          &prm_in_encap_qos_key,
          &prm_out_encap_qos_params
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_encap_qos_params_remark_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_encap_qos_params_remark_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: encap_params_remark_get (section eg_qos)
 ********************************************************************/
int 
  ui_ppd_api_eg_encap_qos_params_remark_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPD_EG_ENCAP_QOS_MAP_KEY   
    prm_in_encap_qos_key;
  SOC_PPD_EG_ENCAP_QOS_PARAMS   
    prm_out_encap_qos_params;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_qos"); 
  soc_sand_proc_name = "soc_ppd_eg_encap_qos_params_remark_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPD_EG_ENCAP_QOS_MAP_KEY_clear(&prm_in_encap_qos_key);
  SOC_PPD_EG_ENCAP_QOS_PARAMS_clear(&prm_out_encap_qos_params);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_QOS_PARAMS_REMARK_GET_PARAMS_REMARK_GET_IN_ENCAP_QOS_KEY_REMARK_PROFILE_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_QOS_PARAMS_REMARK_GET_PARAMS_REMARK_GET_IN_ENCAP_QOS_KEY_REMARK_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_in_encap_qos_key.remark_profile = (SOC_SAND_PP_DP)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_QOS_PARAMS_REMARK_GET_PARAMS_REMARK_GET_IN_ENCAP_QOS_KEY_IN_DSCP_EXP_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_ENCAP_QOS_PARAMS_REMARK_GET_PARAMS_REMARK_GET_IN_ENCAP_QOS_KEY_IN_DSCP_EXP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_in_encap_qos_key.in_dscp_exp = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter in_encap_qos_key after encap_params_remark_get***", TRUE); 
    goto exit; 
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_QOS_PARAMS_REMARK_GET_PARAMS_REMARK_GET_IN_ENCAP_QOS_KEY_PKT_HDR_TYPE_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_EG_ENCAP_QOS_PARAMS_REMARK_GET_PARAMS_REMARK_GET_IN_ENCAP_QOS_KEY_PKT_HDR_TYPE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_in_encap_qos_key.pkt_hdr_type = (SOC_PPD_PKT_HDR_TYPE)param_val->numeric_equivalent; 
  } 


  /* Call function */
  ret = soc_ppd_eg_encap_qos_params_remark_get(
          unit,
          &prm_in_encap_qos_key,
          &prm_out_encap_qos_params
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_encap_qos_params_remark_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_encap_qos_params_remark_get");   
    goto exit; 
  } 

  SOC_PPD_EG_ENCAP_QOS_PARAMS_print(&prm_out_encap_qos_params);

  
  goto exit; 
exit: 
  return ui_ret; 
} 

 
#endif
#ifdef UI_EG_QOS/* { eg_qos*/
/******************************************************************** 
 *  Section handler: eg_qos
 ********************************************************************/ 
int 
  ui_ppd_api_eg_qos( 
    CURRENT_LINE *current_line 
  ) 
{   
  int
    ret = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_qos"); 
 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_QOS_PORT_INFO_SET_PORT_INFO_SET_ID,1)) 
  { 
    ret = ui_ppd_api_eg_qos_port_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_QOS_PORT_INFO_GET_PORT_INFO_GET_ID,1)) 
  { 
    ret = ui_ppd_api_eg_qos_port_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_QOS_PARAMS_PHP_REMARK_SET_PARAMS_PHP_REMARK_SET_ID,1)) 
  { 
    ret = ui_ppd_api_eg_qos_params_php_remark_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_QOS_PARAMS_PHP_REMARK_GET_PARAMS_PHP_REMARK_GET_ID,1)) 
  { 
    ret = ui_ppd_api_eg_qos_params_php_remark_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_QOS_PARAMS_REMARK_SET_PARAMS_REMARK_SET_ID,1)) 
  { 
    ret = ui_ppd_api_eg_qos_params_remark_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_QOS_PARAMS_REMARK_GET_PARAMS_REMARK_GET_ID,1)) 
  { 
    ret = ui_ppd_api_eg_qos_params_remark_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_QOS_PARAMS_REMARK_SET_PARAMS_REMARK_SET_ID,1)) 
  { 
    ret = ui_ppd_api_eg_encap_qos_params_remark_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_ENCAP_QOS_PARAMS_REMARK_GET_PARAMS_REMARK_GET_ID,1)) 
  { 
    ret = ui_ppd_api_eg_encap_qos_params_remark_get(current_line);
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after eg_qos***", TRUE); 
  } 
  
  ui_ret = ret;
    
  goto exit; 
exit:        
  return ui_ret; 
}                
          
#endif /* eg_qos */ 


#endif /* LINK_PPD_LIBRARIES */ 

