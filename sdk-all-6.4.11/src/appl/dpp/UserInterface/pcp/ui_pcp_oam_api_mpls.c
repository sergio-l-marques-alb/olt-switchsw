/* $Id: ui_pcp_oam_api_mpls.c,v 1.5 Broadcom SDK $
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
#include <soc/dpp/SAND/SAND_FM/sand_pp_general.h>
#include <appl/diag/dpp/utils_defi.h> 
#include <appl/dpp/UserInterface/ui_defi.h> 
 
#include <appl/dpp/UserInterface/ui_pure_defi_pcp_api.h> 
 
  
#include <soc/dpp/PCP/pcp_oam_api_mpls.h>
#include <appl/dpp/UserInterface/pcp/ui_pcp_oam_api_mpls.h>

#if LINK_PCP_LIBRARIES

#ifdef UI_MPLS
/******************************************************************** 
 *  Function handler: ttl_mapping_info_set (section mpls)
 ********************************************************************/
int 
  ui_pcp_oam_api_mpls_ttl_mapping_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  PCP_OAM_MPLS_TTL_ID   
    prm_ttl_ndx;
  PCP_OAM_MPLS_TTL_MAPPING_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_mpls"); 
  soc_sand_proc_name = "pcp_oam_mpls_ttl_mapping_info_set"; 
 
  unit = pcp_get_default_unit(); 
  PCP_OAM_MPLS_TTL_MAPPING_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_TTL_MAPPING_INFO_SET_TTL_MAPPING_INFO_SET_TTL_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_MPLS_TTL_MAPPING_INFO_SET_TTL_MAPPING_INFO_SET_TTL_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_ttl_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter ttl_ndx after ttl_mapping_info_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = pcp_oam_mpls_ttl_mapping_info_get(
          unit,
          prm_ttl_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_mpls_ttl_mapping_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_mpls_ttl_mapping_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_TTL_MAPPING_INFO_SET_TTL_MAPPING_INFO_SET_INFO_TTL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_MPLS_TTL_MAPPING_INFO_SET_TTL_MAPPING_INFO_SET_INFO_TTL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.ttl = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = pcp_oam_mpls_ttl_mapping_info_set(
          unit,
          prm_ttl_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_mpls_ttl_mapping_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_mpls_ttl_mapping_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: ttl_mapping_info_get (section mpls)
 ********************************************************************/
int 
  ui_pcp_oam_api_mpls_ttl_mapping_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  PCP_OAM_MPLS_TTL_ID   
    prm_ttl_ndx;
  PCP_OAM_MPLS_TTL_MAPPING_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_mpls"); 
  soc_sand_proc_name = "pcp_oam_mpls_ttl_mapping_info_get"; 
 
  unit = pcp_get_default_unit(); 
  PCP_OAM_MPLS_TTL_MAPPING_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_TTL_MAPPING_INFO_GET_TTL_MAPPING_INFO_GET_TTL_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_MPLS_TTL_MAPPING_INFO_GET_TTL_MAPPING_INFO_GET_TTL_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_ttl_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter ttl_ndx after ttl_mapping_info_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = pcp_oam_mpls_ttl_mapping_info_get(
          unit,
          prm_ttl_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_mpls_ttl_mapping_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_mpls_ttl_mapping_info_get");   
    goto exit; 
  } 

  PCP_OAM_MPLS_TTL_MAPPING_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: ip_mapping_info_set (section mpls)
 ********************************************************************/
int 
  ui_pcp_oam_api_mpls_ip_mapping_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  PCP_OAM_MPLS_IP_ID   
    prm_ip_ndx;
  uint8   
    prm_is_ipv6;
  PCP_OAM_MPLS_IP_MAPPING_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_mpls"); 
  soc_sand_proc_name = "pcp_oam_mpls_ip_mapping_info_set"; 
 
  unit = pcp_get_default_unit(); 
  PCP_OAM_MPLS_IP_MAPPING_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_IP_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_MPLS_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_IP_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_ip_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter ip_ndx after ip_mapping_info_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = pcp_oam_mpls_ip_mapping_info_get(
          unit,
          prm_ip_ndx,
          &prm_is_ipv6,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_mpls_ip_mapping_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_mpls_ip_mapping_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_IS_IPV6_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_MPLS_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_IS_IPV6_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_is_ipv6 = (uint8)param_val->value.ulong_value;
  } 

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_PCP_OAM_MPLS_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_INFO_IPV6_IP_ID,1,  
         &param_val,VAL_TEXT,err_msg)) 
  { 
    soc_sand_pp_ipv6_address_string_parse(param_val->value.val_text, &(prm_info.ipv6.ip));
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_INFO_IPV4_IP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_MPLS_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_INFO_IPV4_IP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    /* prm_info.ipv4.ip = (SOC_SAND_PP_IPV4_ADDRESS)param_val->value.ulong_value; */
    soc_sand_pp_ipv4_address_string_parse(param_val->value.val_text, &(prm_info.ipv4.ip));
  } 


  /* Call function */
  ret = pcp_oam_mpls_ip_mapping_info_set(
          unit,
          prm_ip_ndx,
          prm_is_ipv6,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_mpls_ip_mapping_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_mpls_ip_mapping_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: ip_mapping_info_get (section mpls)
 ********************************************************************/
int 
  ui_pcp_oam_api_mpls_ip_mapping_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  PCP_OAM_MPLS_IP_ID   
    prm_ip_ndx;
  uint8   
    prm_is_ipv6;
  PCP_OAM_MPLS_IP_MAPPING_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_mpls"); 
  soc_sand_proc_name = "pcp_oam_mpls_ip_mapping_info_get"; 
 
  unit = pcp_get_default_unit(); 
  PCP_OAM_MPLS_IP_MAPPING_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_IP_MAPPING_INFO_GET_IP_MAPPING_INFO_GET_IP_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_MPLS_IP_MAPPING_INFO_GET_IP_MAPPING_INFO_GET_IP_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_ip_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter ip_ndx after ip_mapping_info_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = pcp_oam_mpls_ip_mapping_info_get(
          unit,
          prm_ip_ndx,
          &prm_is_ipv6,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_mpls_ip_mapping_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_mpls_ip_mapping_info_get");   
    goto exit; 
  } 

  cli_out("is_ipv6: %u\n\r",prm_is_ipv6);

  PCP_OAM_MPLS_IP_MAPPING_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: label_ranges_set (section mpls)
 ********************************************************************/
int 
  ui_pcp_oam_api_mpls_label_ranges_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  PCP_OAM_MPLS_LABEL_RANGE   
    prm_tnl_range;
  PCP_OAM_MPLS_LABEL_RANGE   
    prm_pwe_range;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_mpls"); 
  soc_sand_proc_name = "pcp_oam_mpls_label_ranges_set"; 
 
  unit = pcp_get_default_unit(); 
  PCP_OAM_MPLS_LABEL_RANGE_clear(&prm_tnl_range);
  PCP_OAM_MPLS_LABEL_RANGE_clear(&prm_pwe_range);
 
  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = pcp_oam_mpls_label_ranges_get(
          unit,
          &prm_tnl_range,
          &prm_pwe_range
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_mpls_label_ranges_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_mpls_label_ranges_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_TNL_RANGE_END_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_TNL_RANGE_END_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tnl_range.end = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_TNL_RANGE_START_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_TNL_RANGE_START_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tnl_range.start = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_TNL_RANGE_BASE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_TNL_RANGE_BASE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tnl_range.base = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_PWE_RANGE_END_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_PWE_RANGE_END_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_pwe_range.end = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_PWE_RANGE_START_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_PWE_RANGE_START_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_pwe_range.start = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_PWE_RANGE_BASE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_PWE_RANGE_BASE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_pwe_range.base = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = pcp_oam_mpls_label_ranges_set(
          unit,
          &prm_tnl_range,
          &prm_pwe_range
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_mpls_label_ranges_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_mpls_label_ranges_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: label_ranges_get (section mpls)
 ********************************************************************/
int 
  ui_pcp_oam_api_mpls_label_ranges_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  PCP_OAM_MPLS_LABEL_RANGE   
    prm_tnl_range;
  PCP_OAM_MPLS_LABEL_RANGE   
    prm_pwe_range;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_mpls"); 
  soc_sand_proc_name = "pcp_oam_mpls_label_ranges_get"; 
 
  unit = pcp_get_default_unit(); 
  PCP_OAM_MPLS_LABEL_RANGE_clear(&prm_tnl_range);
  PCP_OAM_MPLS_LABEL_RANGE_clear(&prm_pwe_range);
 
  /* Get parameters */ 

  /* Call function */
  ret = pcp_oam_mpls_label_ranges_get(
          unit,
          &prm_tnl_range,
          &prm_pwe_range
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_mpls_label_ranges_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_mpls_label_ranges_get");   
    goto exit; 
  } 

  PCP_OAM_MPLS_LABEL_RANGE_print(&prm_tnl_range);

  PCP_OAM_MPLS_LABEL_RANGE_print(&prm_pwe_range);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: configurable_ffd_rate_set (section mpls)
 ********************************************************************/
int 
  ui_pcp_oam_api_mpls_configurable_ffd_rate_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  PCP_OAM_MPLS_CC_PKT_TYPE   
    prm_rate_ndx;
  uint32   
    prm_interval;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_mpls"); 
  soc_sand_proc_name = "pcp_oam_mpls_configurable_ffd_rate_set"; 
 
  unit = pcp_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_CONFIGURABLE_FFD_RATE_SET_CONFIGURABLE_FFD_RATE_SET_RATE_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PCP_OAM_MPLS_CONFIGURABLE_FFD_RATE_SET_CONFIGURABLE_FFD_RATE_SET_RATE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rate_ndx = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter rate_ndx after configurable_ffd_rate_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = pcp_oam_mpls_configurable_ffd_rate_get(
          unit,
          prm_rate_ndx,
          &prm_interval
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_mpls_configurable_ffd_rate_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_mpls_configurable_ffd_rate_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_CONFIGURABLE_FFD_RATE_SET_CONFIGURABLE_FFD_RATE_SET_INTERVAL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_MPLS_CONFIGURABLE_FFD_RATE_SET_CONFIGURABLE_FFD_RATE_SET_INTERVAL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_interval = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = pcp_oam_mpls_configurable_ffd_rate_set(
          unit,
          prm_rate_ndx,
          prm_interval
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_mpls_configurable_ffd_rate_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_mpls_configurable_ffd_rate_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: configurable_ffd_rate_get (section mpls)
 ********************************************************************/
int 
  ui_pcp_oam_api_mpls_configurable_ffd_rate_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  PCP_OAM_MPLS_CC_PKT_TYPE   
    prm_rate_ndx;
  uint32   
    prm_interval;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_mpls"); 
  soc_sand_proc_name = "pcp_oam_mpls_configurable_ffd_rate_get"; 
 
  unit = pcp_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_CONFIGURABLE_FFD_RATE_GET_CONFIGURABLE_FFD_RATE_GET_RATE_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PCP_OAM_MPLS_CONFIGURABLE_FFD_RATE_GET_CONFIGURABLE_FFD_RATE_GET_RATE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rate_ndx = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter rate_ndx after configurable_ffd_rate_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = pcp_oam_mpls_configurable_ffd_rate_get(
          unit,
          prm_rate_ndx,
          &prm_interval
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_mpls_configurable_ffd_rate_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_mpls_configurable_ffd_rate_get");   
    goto exit; 
  } 

  cli_out("interval: %d\n\r",prm_interval);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: lsp_tx_info_set (section mpls)
 ********************************************************************/
int 
  ui_pcp_oam_api_mpls_lsp_tx_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_mot_ndx;
  PCP_OAM_MPLS_LSP_TX_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_mpls"); 
  soc_sand_proc_name = "pcp_oam_mpls_lsp_tx_info_set"; 
 
  unit = pcp_get_default_unit(); 
  PCP_OAM_MPLS_LSP_TX_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_MOT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_MOT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_mot_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter mot_ndx after lsp_tx_info_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = pcp_oam_mpls_lsp_tx_info_get(
          unit,
          prm_mot_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_mpls_lsp_tx_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_mpls_lsp_tx_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_DEFECT_INFO_FDI_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_DEFECT_INFO_FDI_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.defect_info.fdi_enable = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_DEFECT_INFO_BDI_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_DEFECT_INFO_BDI_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.defect_info.bdi_enable = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_DEFECT_INFO_DEFECT_LOCATION_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_DEFECT_INFO_DEFECT_LOCATION_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.defect_info.defect_location = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_DEFECT_INFO_DEFECT_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_DEFECT_INFO_DEFECT_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.defect_info.defect_type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_PACKET_INFO_LSP_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_PACKET_INFO_LSP_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.packet_info.lsp_id = (uint16)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_PACKET_INFO_LSR_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_PACKET_INFO_LSR_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.packet_info.lsr_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_PACKET_INFO_EEP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_PACKET_INFO_EEP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.packet_info.eep = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_PACKET_INFO_TTL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_PACKET_INFO_TTL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.packet_info.ttl = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_PACKET_INFO_COS_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_PACKET_INFO_COS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.packet_info.cos = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_PACKET_INFO_SYSTEM_PORT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_PACKET_INFO_SYSTEM_PORT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.packet_info.system_port = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_VALID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_INFO_VALID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.valid = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = pcp_oam_mpls_lsp_tx_info_set(
          unit,
          prm_mot_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_mpls_lsp_tx_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_mpls_lsp_tx_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: lsp_tx_info_get (section mpls)
 ********************************************************************/
int 
  ui_pcp_oam_api_mpls_lsp_tx_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_mot_ndx;
  PCP_OAM_MPLS_LSP_TX_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_mpls"); 
  soc_sand_proc_name = "pcp_oam_mpls_lsp_tx_info_get"; 
 
  unit = pcp_get_default_unit(); 
  PCP_OAM_MPLS_LSP_TX_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_LSP_TX_INFO_GET_LSP_TX_INFO_GET_MOT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_MPLS_LSP_TX_INFO_GET_LSP_TX_INFO_GET_MOT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_mot_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter mot_ndx after lsp_tx_info_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = pcp_oam_mpls_lsp_tx_info_get(
          unit,
          prm_mot_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_mpls_lsp_tx_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_mpls_lsp_tx_info_get");   
    goto exit; 
  } 

  PCP_OAM_MPLS_LSP_TX_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: lsp_rx_info_set (section mpls)
 ********************************************************************/
int 
  ui_pcp_oam_api_mpls_lsp_rx_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_mor_ndx;
  PCP_OAM_MPLS_LSP_RX_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_mpls"); 
  soc_sand_proc_name = "pcp_oam_mpls_lsp_rx_info_set"; 
 
  unit = pcp_get_default_unit(); 
  PCP_OAM_MPLS_LSP_RX_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_LSP_RX_INFO_SET_LSP_RX_INFO_SET_MOR_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_MPLS_LSP_RX_INFO_SET_LSP_RX_INFO_SET_MOR_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_mor_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter mor_ndx after lsp_rx_info_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = pcp_oam_mpls_lsp_rx_info_get(
          unit,
          prm_mor_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_mpls_lsp_rx_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_mpls_lsp_rx_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_LSP_RX_INFO_SET_LSP_RX_INFO_SET_INFO_BDI_MSG_TYPE_TO_FILTER_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_MPLS_LSP_RX_INFO_SET_LSP_RX_INFO_SET_INFO_BDI_MSG_TYPE_TO_FILTER_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.bdi_msg_type_to_filter = (PCP_OAM_MPLS_LSP_RX_DEFECT_FILTER_TYPE)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_LSP_RX_INFO_SET_LSP_RX_INFO_SET_INFO_FDI_MSG_TYPE_TO_FILTER_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_MPLS_LSP_RX_INFO_SET_LSP_RX_INFO_SET_INFO_FDI_MSG_TYPE_TO_FILTER_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.fdi_msg_type_to_filter = (PCP_OAM_MPLS_LSP_RX_DEFECT_FILTER_TYPE)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_LSP_RX_INFO_SET_LSP_RX_INFO_SET_INFO_IS_FFD_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_MPLS_LSP_RX_INFO_SET_LSP_RX_INFO_SET_INFO_IS_FFD_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.is_ffd = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_LSP_RX_INFO_SET_LSP_RX_INFO_SET_INFO_VALID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_MPLS_LSP_RX_INFO_SET_LSP_RX_INFO_SET_INFO_VALID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.valid = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = pcp_oam_mpls_lsp_rx_info_set(
          unit,
          prm_mor_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_mpls_lsp_rx_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_mpls_lsp_rx_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: lsp_rx_info_get (section mpls)
 ********************************************************************/
int 
  ui_pcp_oam_api_mpls_lsp_rx_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_mor_ndx;
  PCP_OAM_MPLS_LSP_RX_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_mpls"); 
  soc_sand_proc_name = "pcp_oam_mpls_lsp_rx_info_get"; 
 
  unit = pcp_get_default_unit(); 
  PCP_OAM_MPLS_LSP_RX_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_LSP_RX_INFO_GET_LSP_RX_INFO_GET_MOR_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_MPLS_LSP_RX_INFO_GET_LSP_RX_INFO_GET_MOR_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_mor_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter mor_ndx after lsp_rx_info_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = pcp_oam_mpls_lsp_rx_info_get(
          unit,
          prm_mor_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_mpls_lsp_rx_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_mpls_lsp_rx_info_get");   
    goto exit; 
  } 

  PCP_OAM_MPLS_LSP_RX_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
#endif
#ifdef UI_MPLS/* { mpls*/
/******************************************************************** 
 *  Section handler: mpls
 ********************************************************************/ 
int 
  ui_pcp_oam_api_mpls( 
    CURRENT_LINE *current_line 
  ) 
{   
  int
    ret = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_mpls"); 
 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_TTL_MAPPING_INFO_SET_TTL_MAPPING_INFO_SET_ID,1)) 
  { 
    ret = ui_pcp_oam_api_mpls_ttl_mapping_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_TTL_MAPPING_INFO_GET_TTL_MAPPING_INFO_GET_ID,1)) 
  { 
    ret = ui_pcp_oam_api_mpls_ttl_mapping_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_ID,1)) 
  { 
    ret = ui_pcp_oam_api_mpls_ip_mapping_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_IP_MAPPING_INFO_GET_IP_MAPPING_INFO_GET_ID,1)) 
  { 
    ret = ui_pcp_oam_api_mpls_ip_mapping_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_LABEL_RANGES_SET_LABEL_RANGES_SET_ID,1)) 
  { 
    ret = ui_pcp_oam_api_mpls_label_ranges_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_LABEL_RANGES_GET_LABEL_RANGES_GET_ID,1)) 
  { 
    ret = ui_pcp_oam_api_mpls_label_ranges_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_CONFIGURABLE_FFD_RATE_SET_CONFIGURABLE_FFD_RATE_SET_ID,1)) 
  { 
    ret = ui_pcp_oam_api_mpls_configurable_ffd_rate_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_CONFIGURABLE_FFD_RATE_GET_CONFIGURABLE_FFD_RATE_GET_ID,1)) 
  { 
    ret = ui_pcp_oam_api_mpls_configurable_ffd_rate_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_LSP_TX_INFO_SET_LSP_TX_INFO_SET_ID,1)) 
  { 
    ret = ui_pcp_oam_api_mpls_lsp_tx_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_LSP_TX_INFO_GET_LSP_TX_INFO_GET_ID,1)) 
  { 
    ret = ui_pcp_oam_api_mpls_lsp_tx_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_LSP_RX_INFO_SET_LSP_RX_INFO_SET_ID,1)) 
  { 
    ret = ui_pcp_oam_api_mpls_lsp_rx_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MPLS_LSP_RX_INFO_GET_LSP_RX_INFO_GET_ID,1)) 
  { 
    ret = ui_pcp_oam_api_mpls_lsp_rx_info_get(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after mpls***", TRUE); 
  }
  
  ui_ret = ret; 
    
  goto exit; 
exit:        
  return ui_ret; 
}                
          
#endif /* mpls */ 


#endif /* LINK_PCP_LIBRARIES */ 

