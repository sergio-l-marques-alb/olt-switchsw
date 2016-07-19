/* $Id: ui_pcp_oam_api_bfd.c,v 1.5 Broadcom SDK $
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
 
  
#include <soc/dpp/PCP/pcp_oam_api_bfd.h>
#include <appl/dpp/UserInterface/pcp/ui_pcp_oam_api_bfd.h>

#if LINK_PCP_LIBRARIES

#ifdef UI_BFD
/******************************************************************** 
 *  Function handler: ttl_mapping_info_set (section bfd)
 ********************************************************************/
int 
  ui_pcp_oam_api_bfd_ttl_mapping_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  PCP_OAM_BFD_TTL_ID   
    prm_ttl_ndx;
  PCP_OAM_BFD_TTL_MAPPING_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_bfd"); 
  soc_sand_proc_name = "pcp_oam_bfd_ttl_mapping_info_set"; 
 
  unit = pcp_get_default_unit(); 
  PCP_OAM_BFD_TTL_MAPPING_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_TTL_MAPPING_INFO_SET_TTL_MAPPING_INFO_SET_TTL_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_BFD_TTL_MAPPING_INFO_SET_TTL_MAPPING_INFO_SET_TTL_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_ttl_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter ttl_ndx after ttl_mapping_info_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = pcp_oam_bfd_ttl_mapping_info_get(
          unit,
          prm_ttl_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_bfd_ttl_mapping_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_bfd_ttl_mapping_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_TTL_MAPPING_INFO_SET_TTL_MAPPING_INFO_SET_INFO_PWE_TTL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_BFD_TTL_MAPPING_INFO_SET_TTL_MAPPING_INFO_SET_INFO_PWE_TTL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.pwe_ttl = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_TTL_MAPPING_INFO_SET_TTL_MAPPING_INFO_SET_INFO_TNL_TTL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_BFD_TTL_MAPPING_INFO_SET_TTL_MAPPING_INFO_SET_INFO_TNL_TTL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.tnl_ttl = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = pcp_oam_bfd_ttl_mapping_info_set(
          unit,
          prm_ttl_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_bfd_ttl_mapping_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_bfd_ttl_mapping_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: ttl_mapping_info_get (section bfd)
 ********************************************************************/
int 
  ui_pcp_oam_api_bfd_ttl_mapping_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  PCP_OAM_BFD_TTL_ID   
    prm_ttl_ndx;
  PCP_OAM_BFD_TTL_MAPPING_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_bfd"); 
  soc_sand_proc_name = "pcp_oam_bfd_ttl_mapping_info_get"; 
 
  unit = pcp_get_default_unit(); 
  PCP_OAM_BFD_TTL_MAPPING_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_TTL_MAPPING_INFO_GET_TTL_MAPPING_INFO_GET_TTL_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_BFD_TTL_MAPPING_INFO_GET_TTL_MAPPING_INFO_GET_TTL_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_ttl_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter ttl_ndx after ttl_mapping_info_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = pcp_oam_bfd_ttl_mapping_info_get(
          unit,
          prm_ttl_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_bfd_ttl_mapping_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_bfd_ttl_mapping_info_get");   
    goto exit; 
  } 

  PCP_OAM_BFD_TTL_MAPPING_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: ip_mapping_info_set (section bfd)
 ********************************************************************/
int 
  ui_pcp_oam_api_bfd_ip_mapping_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  PCP_OAM_BFD_IP_ID   
    prm_ip_ndx;
  PCP_OAM_BFD_IP_MAPPING_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_bfd"); 
  soc_sand_proc_name = "pcp_oam_bfd_ip_mapping_info_set"; 
 
  unit = pcp_get_default_unit(); 
  PCP_OAM_BFD_IP_MAPPING_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_IP_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_BFD_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_IP_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_ip_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter ip_ndx after ip_mapping_info_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = pcp_oam_bfd_ip_mapping_info_get(
          unit,
          prm_ip_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_bfd_ip_mapping_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_bfd_ip_mapping_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_INFO_IP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_BFD_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_INFO_IP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    /* prm_info.ip = (SOC_SAND_PP_IPV4_ADDRESS)param_val->value.ulong_value; */
    soc_sand_pp_ipv4_address_string_parse(param_val->value.val_text, &(prm_info.ip));
  } 


  /* Call function */
  ret = pcp_oam_bfd_ip_mapping_info_set(
          unit,
          prm_ip_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_bfd_ip_mapping_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_bfd_ip_mapping_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: ip_mapping_info_get (section bfd)
 ********************************************************************/
int 
  ui_pcp_oam_api_bfd_ip_mapping_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  PCP_OAM_BFD_IP_ID   
    prm_ip_ndx;
  PCP_OAM_BFD_IP_MAPPING_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_bfd"); 
  soc_sand_proc_name = "pcp_oam_bfd_ip_mapping_info_get"; 
 
  unit = pcp_get_default_unit(); 
  PCP_OAM_BFD_IP_MAPPING_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_IP_MAPPING_INFO_GET_IP_MAPPING_INFO_GET_IP_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_BFD_IP_MAPPING_INFO_GET_IP_MAPPING_INFO_GET_IP_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_ip_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter ip_ndx after ip_mapping_info_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = pcp_oam_bfd_ip_mapping_info_get(
          unit,
          prm_ip_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_bfd_ip_mapping_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_bfd_ip_mapping_info_get");   
    goto exit; 
  } 

  PCP_OAM_BFD_IP_MAPPING_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: my_discriminator_range_set (section bfd)
 ********************************************************************/
int 
  ui_pcp_oam_api_bfd_my_discriminator_range_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  PCP_OAM_BFD_DISCRIMINATOR_RANGE   
    prm_disc_range;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_bfd"); 
  soc_sand_proc_name = "pcp_oam_bfd_my_discriminator_range_set"; 
 
  unit = pcp_get_default_unit(); 
  PCP_OAM_BFD_DISCRIMINATOR_RANGE_clear(&prm_disc_range);
 
  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = pcp_oam_bfd_my_discriminator_range_get(
          unit,
          &prm_disc_range
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_bfd_my_discriminator_range_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_bfd_my_discriminator_range_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_MY_DISCRIMINATOR_RANGE_SET_MY_DISCRIMINATOR_RANGE_SET_DISC_RANGE_END_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_BFD_MY_DISCRIMINATOR_RANGE_SET_MY_DISCRIMINATOR_RANGE_SET_DISC_RANGE_END_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_disc_range.end = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_MY_DISCRIMINATOR_RANGE_SET_MY_DISCRIMINATOR_RANGE_SET_DISC_RANGE_START_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_BFD_MY_DISCRIMINATOR_RANGE_SET_MY_DISCRIMINATOR_RANGE_SET_DISC_RANGE_START_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_disc_range.start = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_MY_DISCRIMINATOR_RANGE_SET_MY_DISCRIMINATOR_RANGE_SET_DISC_RANGE_BASE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_BFD_MY_DISCRIMINATOR_RANGE_SET_MY_DISCRIMINATOR_RANGE_SET_DISC_RANGE_BASE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_disc_range.base = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = pcp_oam_bfd_my_discriminator_range_set(
          unit,
          &prm_disc_range
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_bfd_my_discriminator_range_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_bfd_my_discriminator_range_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: my_discriminator_range_get (section bfd)
 ********************************************************************/
int 
  ui_pcp_oam_api_bfd_my_discriminator_range_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  PCP_OAM_BFD_DISCRIMINATOR_RANGE   
    prm_disc_range;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_bfd"); 
  soc_sand_proc_name = "pcp_oam_bfd_my_discriminator_range_get"; 
 
  unit = pcp_get_default_unit(); 
  PCP_OAM_BFD_DISCRIMINATOR_RANGE_clear(&prm_disc_range);
 
  /* Get parameters */ 

  /* Call function */
  ret = pcp_oam_bfd_my_discriminator_range_get(
          unit,
          &prm_disc_range
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_bfd_my_discriminator_range_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_bfd_my_discriminator_range_get");   
    goto exit; 
  } 

  PCP_OAM_BFD_DISCRIMINATOR_RANGE_print(&prm_disc_range);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: tx_info_set (section bfd)
 ********************************************************************/
int 
  ui_pcp_oam_api_bfd_tx_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_my_discriminator_ndx;
  PCP_OAM_BFD_TX_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_bfd"); 
  soc_sand_proc_name = "pcp_oam_bfd_tx_info_set"; 
 
  unit = pcp_get_default_unit(); 
  PCP_OAM_BFD_TX_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_MY_DISCRIMINATOR_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_MY_DISCRIMINATOR_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_my_discriminator_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter my_discriminator_ndx after tx_info_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = pcp_oam_bfd_tx_info_get(
          unit,
          prm_my_discriminator_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_bfd_tx_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_bfd_tx_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_IP_INFO_TTL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_IP_INFO_TTL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.fw_info.ip_info.ttl = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_IP_INFO_COS_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_IP_INFO_COS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.fw_info.ip_info.cos = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_IP_INFO_DST_IP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_IP_INFO_DST_IP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    /* prm_info.fw_info.ip_info.dst_ip = (SOC_SAND_PP_IPV4_ADDRESS)param_val->value.ulong_value; */
    soc_sand_pp_ipv4_address_string_parse(param_val->value.val_text, &(prm_info.fw_info.ip_info.dst_ip));
    
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_IP_INFO_SRC_IP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_IP_INFO_SRC_IP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.fw_info.ip_info.src_ip = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_WITH_IP_INFO_TTL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_WITH_IP_INFO_TTL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.fw_info.mpls_pwe_with_ip_info.ttl = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_WITH_IP_INFO_COS_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_WITH_IP_INFO_COS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.fw_info.mpls_pwe_with_ip_info.cos = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_WITH_IP_INFO_PWE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_WITH_IP_INFO_PWE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.fw_info.mpls_pwe_with_ip_info.pwe = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_WITH_IP_INFO_EEP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_WITH_IP_INFO_EEP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.fw_info.mpls_pwe_with_ip_info.eep = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_WITH_IP_INFO_SRC_IP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_WITH_IP_INFO_SRC_IP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.fw_info.mpls_pwe_with_ip_info.src_ip = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_INFO_TTL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_INFO_TTL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.fw_info.mpls_pwe_info.ttl = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_INFO_COS_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_INFO_COS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.fw_info.mpls_pwe_info.cos = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_INFO_PWE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_INFO_PWE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.fw_info.mpls_pwe_info.pwe = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_INFO_EEP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_PWE_INFO_EEP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.fw_info.mpls_pwe_info.eep = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_TUNNEL_INFO_TTL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_TUNNEL_INFO_TTL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.fw_info.mpls_tunnel_info.ttl = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_TUNNEL_INFO_COS_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_TUNNEL_INFO_COS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.fw_info.mpls_tunnel_info.cos = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_TUNNEL_INFO_EEP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_FW_INFO_MPLS_TUNNEL_INFO_EEP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.fw_info.mpls_tunnel_info.eep = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_NG_INFO_MIN_TX_INTERVAL_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_NG_INFO_MIN_TX_INTERVAL_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.ng_info.min_tx_interval = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_NG_INFO_MIN_RX_INTERVAL_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_NG_INFO_MIN_RX_INTERVAL_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.ng_info.min_rx_interval = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_NG_INFO_DETECT_MULT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_NG_INFO_DETECT_MULT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.ng_info.detect_mult = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_BFD_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_BFD_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.bfd_type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_DISCRIMINATOR_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_DISCRIMINATOR_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.discriminator = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_TX_RATE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_TX_RATE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.tx_rate = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_VALID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_INFO_VALID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.valid = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = pcp_oam_bfd_tx_info_set(
          unit,
          prm_my_discriminator_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_bfd_tx_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_bfd_tx_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: tx_info_get (section bfd)
 ********************************************************************/
int 
  ui_pcp_oam_api_bfd_tx_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_my_discriminator_ndx;
  PCP_OAM_BFD_TX_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_bfd"); 
  soc_sand_proc_name = "pcp_oam_bfd_tx_info_get"; 
 
  unit = pcp_get_default_unit(); 
  PCP_OAM_BFD_TX_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_TX_INFO_GET_TX_INFO_GET_MY_DISCRIMINATOR_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_BFD_TX_INFO_GET_TX_INFO_GET_MY_DISCRIMINATOR_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_my_discriminator_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter my_discriminator_ndx after tx_info_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = pcp_oam_bfd_tx_info_get(
          unit,
          prm_my_discriminator_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_bfd_tx_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_bfd_tx_info_get");   
    goto exit; 
  } 

  PCP_OAM_BFD_TX_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: rx_info_set (section bfd)
 ********************************************************************/
int 
  ui_pcp_oam_api_bfd_rx_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_my_discriminator_ndx;
  PCP_OAM_BFD_RX_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_bfd"); 
  soc_sand_proc_name = "pcp_oam_bfd_rx_info_set"; 
 
  unit = pcp_get_default_unit(); 
  PCP_OAM_BFD_RX_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_RX_INFO_SET_RX_INFO_SET_MY_DISCRIMINATOR_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_BFD_RX_INFO_SET_RX_INFO_SET_MY_DISCRIMINATOR_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_my_discriminator_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter my_discriminator_ndx after rx_info_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = pcp_oam_bfd_rx_info_get(
          unit,
          prm_my_discriminator_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_bfd_rx_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_bfd_rx_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_RX_INFO_SET_RX_INFO_SET_INFO_DEFECT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_BFD_RX_INFO_SET_RX_INFO_SET_INFO_DEFECT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.defect = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_RX_INFO_SET_RX_INFO_SET_INFO_LIFE_TIME_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_BFD_RX_INFO_SET_RX_INFO_SET_INFO_LIFE_TIME_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.life_time = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = pcp_oam_bfd_rx_info_set(
          unit,
          prm_my_discriminator_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_bfd_rx_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_bfd_rx_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: rx_info_get (section bfd)
 ********************************************************************/
int 
  ui_pcp_oam_api_bfd_rx_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_my_discriminator_ndx;
  PCP_OAM_BFD_RX_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_bfd"); 
  soc_sand_proc_name = "pcp_oam_bfd_rx_info_get"; 
 
  unit = pcp_get_default_unit(); 
  PCP_OAM_BFD_RX_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_RX_INFO_GET_RX_INFO_GET_MY_DISCRIMINATOR_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_BFD_RX_INFO_GET_RX_INFO_GET_MY_DISCRIMINATOR_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_my_discriminator_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter my_discriminator_ndx after rx_info_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = pcp_oam_bfd_rx_info_get(
          unit,
          prm_my_discriminator_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_bfd_rx_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_bfd_rx_info_get");   
    goto exit; 
  } 

  PCP_OAM_BFD_RX_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
#endif
#ifdef UI_BFD/* { bfd*/
/******************************************************************** 
 *  Section handler: bfd
 ********************************************************************/ 
int 
  ui_pcp_oam_api_bfd( 
    CURRENT_LINE *current_line 
  ) 
{   
  int
    ret = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_bfd"); 
 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_TTL_MAPPING_INFO_SET_TTL_MAPPING_INFO_SET_ID,1)) 
  { 
    ret = ui_pcp_oam_api_bfd_ttl_mapping_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_TTL_MAPPING_INFO_GET_TTL_MAPPING_INFO_GET_ID,1)) 
  { 
    ret = ui_pcp_oam_api_bfd_ttl_mapping_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_IP_MAPPING_INFO_SET_IP_MAPPING_INFO_SET_ID,1)) 
  { 
    ret = ui_pcp_oam_api_bfd_ip_mapping_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_IP_MAPPING_INFO_GET_IP_MAPPING_INFO_GET_ID,1)) 
  { 
    ret = ui_pcp_oam_api_bfd_ip_mapping_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_MY_DISCRIMINATOR_RANGE_SET_MY_DISCRIMINATOR_RANGE_SET_ID,1)) 
  { 
    ret = ui_pcp_oam_api_bfd_my_discriminator_range_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_MY_DISCRIMINATOR_RANGE_GET_MY_DISCRIMINATOR_RANGE_GET_ID,1)) 
  { 
    ret = ui_pcp_oam_api_bfd_my_discriminator_range_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_TX_INFO_SET_TX_INFO_SET_ID,1)) 
  { 
    ret = ui_pcp_oam_api_bfd_tx_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_TX_INFO_GET_TX_INFO_GET_ID,1)) 
  { 
    ret = ui_pcp_oam_api_bfd_tx_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_RX_INFO_SET_RX_INFO_SET_ID,1)) 
  { 
    ret = ui_pcp_oam_api_bfd_rx_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_BFD_RX_INFO_GET_RX_INFO_GET_ID,1)) 
  { 
    ret = ui_pcp_oam_api_bfd_rx_info_get(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after bfd***", TRUE); 
  }
  
  ui_ret = ret; 
    
  goto exit; 
exit:        
  return ui_ret; 
}                
          
#endif /* bfd */ 


#endif /* LINK_PCP_LIBRARIES */ 

