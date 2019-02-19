/* $Id: ui_pb_api_stack.c,v 1.6 Broadcom SDK $
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
 
  
#include <soc/dpp/Petra/PB_TM/pb_api_stack.h>
#include <appl/dpp/UserInterface/pb/ui_pb_api_stack.h>

#if LINK_PB_LIBRARIES

#ifdef UI_STACK
/******************************************************************** 
 *  Function handler: global_info_set (section stack)
 ********************************************************************/
int 
  ui_pb_api_stack_global_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_STACK_GLBL_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_stack"); 
  soc_sand_proc_name = "soc_pb_stack_global_info_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_STACK_GLBL_INFO_clear(&prm_info);
 
  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_stack_global_info_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_stack_global_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_stack_global_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_STACK_GLOBAL_INFO_SET_GLOBAL_INFO_SET_INFO_MY_TM_DOMAIN_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_STACK_GLOBAL_INFO_SET_GLOBAL_INFO_SET_INFO_MY_TM_DOMAIN_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.my_tm_domain = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_STACK_GLOBAL_INFO_SET_GLOBAL_INFO_SET_INFO_MAX_NOF_TM_DOMAINS_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_STACK_GLOBAL_INFO_SET_GLOBAL_INFO_SET_INFO_MAX_NOF_TM_DOMAINS_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.max_nof_tm_domains = param_val->numeric_equivalent;
  } 


  /* Call function */
  ret = soc_pb_stack_global_info_set(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_stack_global_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_stack_global_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: global_info_get (section stack)
 ********************************************************************/
int 
  ui_pb_api_stack_global_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_STACK_GLBL_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_stack"); 
  soc_sand_proc_name = "soc_pb_stack_global_info_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_STACK_GLBL_INFO_clear(&prm_info);
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_pb_stack_global_info_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_stack_global_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_stack_global_info_get");   
    goto exit; 
  } 

  SOC_PB_STACK_GLBL_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: port_distribution_info_set (section stack)
 ********************************************************************/
int 
  ui_pb_api_stack_port_distribution_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_local_stack_port_ndx;
  uint32 
    prm_prun_bmp_index = 0;  
  SOC_PB_STACK_PORT_DISTR_INFO   
    prm_distribution_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_stack"); 
  soc_sand_proc_name = "soc_pb_stack_port_distribution_info_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_STACK_PORT_DISTR_INFO_clear(&prm_distribution_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_STACK_PORT_DISTRIBUTION_INFO_SET_PORT_DISTRIBUTION_INFO_SET_LOCAL_STACK_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_STACK_PORT_DISTRIBUTION_INFO_SET_PORT_DISTRIBUTION_INFO_SET_LOCAL_STACK_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_local_stack_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter local_stack_port_ndx after port_distribution_info_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_stack_port_distribution_info_get(
          unit,
          prm_local_stack_port_ndx,
          &prm_distribution_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_stack_port_distribution_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_stack_port_distribution_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_STACK_PORT_DISTRIBUTION_INFO_SET_PORT_DISTRIBUTION_INFO_SET_DISTRIBUTION_INFO_PRUN_BMP_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_STACK_PORT_DISTRIBUTION_INFO_SET_PORT_DISTRIBUTION_INFO_SET_DISTRIBUTION_INFO_PRUN_BMP_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_prun_bmp_index = (uint32)param_val->value.ulong_value;
  } 

  if(prm_prun_bmp_index != 0xFFFFFFFF)          
  { 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_STACK_PORT_DISTRIBUTION_INFO_SET_PORT_DISTRIBUTION_INFO_SET_DISTRIBUTION_INFO_PRUN_BMP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_STACK_PORT_DISTRIBUTION_INFO_SET_PORT_DISTRIBUTION_INFO_SET_DISTRIBUTION_INFO_PRUN_BMP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_distribution_info.prun_bmp[ prm_prun_bmp_index] = (uint32)param_val->value.ulong_value;
  } 

  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_STACK_PORT_DISTRIBUTION_INFO_SET_PORT_DISTRIBUTION_INFO_SET_DISTRIBUTION_INFO_PEER_TM_DOMAIN_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_STACK_PORT_DISTRIBUTION_INFO_SET_PORT_DISTRIBUTION_INFO_SET_DISTRIBUTION_INFO_PEER_TM_DOMAIN_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_distribution_info.peer_tm_domain = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_stack_port_distribution_info_set(
          unit,
          prm_local_stack_port_ndx,
          &prm_distribution_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_stack_port_distribution_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_stack_port_distribution_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: port_distribution_info_get (section stack)
 ********************************************************************/
int 
  ui_pb_api_stack_port_distribution_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_local_stack_port_ndx;
  SOC_PB_STACK_PORT_DISTR_INFO   
    prm_distribution_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_stack"); 
  soc_sand_proc_name = "soc_pb_stack_port_distribution_info_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_STACK_PORT_DISTR_INFO_clear(&prm_distribution_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_STACK_PORT_DISTRIBUTION_INFO_GET_PORT_DISTRIBUTION_INFO_GET_LOCAL_STACK_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_STACK_PORT_DISTRIBUTION_INFO_GET_PORT_DISTRIBUTION_INFO_GET_LOCAL_STACK_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_local_stack_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter local_stack_port_ndx after port_distribution_info_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_stack_port_distribution_info_get(
          unit,
          prm_local_stack_port_ndx,
          &prm_distribution_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_stack_port_distribution_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_stack_port_distribution_info_get");   
    goto exit; 
  } 

  SOC_PB_STACK_PORT_DISTR_INFO_print(&prm_distribution_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
#endif
#ifdef UI_STACK/* { stack*/
/******************************************************************** 
 *  Section handler: stack
 ********************************************************************/ 
int 
  ui_pb_api_stack( 
    CURRENT_LINE *current_line 
  ) 
{   
  int
    ret = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_pb_api_stack"); 
 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_STACK_GLOBAL_INFO_SET_GLOBAL_INFO_SET_ID,1)) 
  { 
    ret = ui_pb_api_stack_global_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_STACK_GLOBAL_INFO_GET_GLOBAL_INFO_GET_ID,1)) 
  { 
    ret = ui_pb_api_stack_global_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_STACK_PORT_DISTRIBUTION_INFO_SET_PORT_DISTRIBUTION_INFO_SET_ID,1)) 
  { 
    ret = ui_pb_api_stack_port_distribution_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_STACK_PORT_DISTRIBUTION_INFO_GET_PORT_DISTRIBUTION_INFO_GET_ID,1)) 
  { 
    ret = ui_pb_api_stack_port_distribution_info_get(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after stack***", TRUE); 
  }
  
  ui_ret = ret; 
    
  goto exit; 
exit:        
  return ui_ret; 
}                
          
#endif /* stack */ 


#endif /* LINK_PB_LIBRARIES */ 

