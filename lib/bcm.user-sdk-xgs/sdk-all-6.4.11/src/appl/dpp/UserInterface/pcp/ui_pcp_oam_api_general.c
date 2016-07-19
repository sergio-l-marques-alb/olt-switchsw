/* $Id: ui_pcp_oam_api_general.c,v 1.6 Broadcom SDK $
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
#include <soc/dpp/SAND/SAND_FM/sand_user_callback.h>
#include <appl/diag/dpp/utils_defi.h> 
#include <appl/dpp/UserInterface/ui_defi.h> 
 
#include <appl/dpp/UserInterface/ui_pure_defi_pcp_api.h> 
 
  
#include <soc/dpp/PCP/pcp_oam_api_general.h>
#include <appl/dpp/UserInterface/pcp/ui_pcp_oam_api_general.h>

#if LINK_PCP_LIBRARIES

#ifdef UI_GENERAL
/******************************************************************** 
 *  Function handler: info_set (section general)
 ********************************************************************/
int 
  ui_pcp_oam_api_general_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  PCP_OAM_GENERAL_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_general"); 
  soc_sand_proc_name = "pcp_oam_general_info_set"; 
 
  unit = pcp_get_default_unit(); 
  PCP_OAM_GENERAL_INFO_clear(&prm_info);
 
  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = pcp_oam_general_info_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_general_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_general_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_GENERAL_INFO_SET_INFO_SET_INFO_CPU_DP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_GENERAL_INFO_SET_INFO_SET_INFO_CPU_DP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.cpu_dp = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_GENERAL_INFO_SET_INFO_SET_INFO_CPU_TC_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_GENERAL_INFO_SET_INFO_SET_INFO_CPU_TC_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.cpu_tc = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_GENERAL_INFO_SET_INFO_SET_INFO_CPU_DST_SYS_PORT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_GENERAL_INFO_SET_INFO_SET_INFO_CPU_DST_SYS_PORT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.cpu_dst_sys_port = (uint32)param_val->value.ulong_value;
  } 

  /* Call function */
  ret = pcp_oam_general_info_set(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_general_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_general_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: info_get (section general)
 ********************************************************************/
int 
  ui_pcp_oam_api_general_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  PCP_OAM_GENERAL_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_general"); 
  soc_sand_proc_name = "pcp_oam_general_info_get"; 
 
  unit = pcp_get_default_unit(); 
  PCP_OAM_GENERAL_INFO_clear(&prm_info);
 
  /* Get parameters */ 

  /* Call function */
  ret = pcp_oam_general_info_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_general_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_general_info_get");   
    goto exit; 
  } 

  PCP_OAM_GENERAL_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: oam_callback_function_register (section general)
 ********************************************************************/
int 
  ui_pcp_oam_api_general_oam_callback_function_register(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_SAND_USER_CALLBACK   
    prm_user_callback_struct;
  uint32   
    prm_callback_id;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_general"); 
  soc_sand_proc_name = "pcp_oam_callback_function_register"; 
 
  unit = pcp_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_CALLBACK_FUNCTION_REGISTER_OAM_CALLBACK_FUNCTION_REGISTER_USER_CALLBACK_STRUCT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_OAM_CALLBACK_FUNCTION_REGISTER_OAM_CALLBACK_FUNCTION_REGISTER_USER_CALLBACK_STRUCT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    /* prm_user_callback_struct = (SOC_SAND_USER_CALLBACK)param_val->value.ulong_value; */
  } 


  /* Call function */
  ret = pcp_oam_callback_function_register(
          unit,
          &prm_user_callback_struct,
          &prm_callback_id
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_callback_function_register - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_callback_function_register");   
    goto exit; 
  } 

  cli_out("callback_id: %u\n\r",prm_callback_id);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: oam_interrupt_handler (section general)
 ********************************************************************/
int 
  ui_pcp_oam_api_general_oam_interrupt_handler(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_general"); 
  soc_sand_proc_name = "pcp_oam_interrupt_handler"; 
 
  unit = pcp_get_default_unit(); 
 
  /* Get parameters */ 

  /* Call function */
  ret = pcp_oam_interrupt_handler(
          unit
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_interrupt_handler - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_interrupt_handler");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: oam_msg_info_get (section general)
 ********************************************************************/
int 
  ui_pcp_oam_api_general_oam_msg_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  PCP_OAM_MSG_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_general"); 
  soc_sand_proc_name = "pcp_oam_msg_info_get"; 
 
  unit = pcp_get_default_unit(); 
  PCP_OAM_MSG_INFO_clear(&prm_info);
 
  /* Get parameters */ 

  /* Call function */
  ret = pcp_oam_msg_info_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_oam_msg_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_oam_msg_info_get");   
    goto exit; 
  } 

  PCP_OAM_MSG_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
#endif
#ifdef UI_GENERAL/* { general*/
/******************************************************************** 
 *  Section handler: general
 ********************************************************************/ 
int 
  ui_pcp_oam_api_general( 
    CURRENT_LINE *current_line 
  ) 
{   
  int
    ret = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_pcp_oam_api_general"); 
 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_GENERAL_INFO_SET_INFO_SET_ID,1)) 
  { 
    ret = ui_pcp_oam_api_general_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_GENERAL_INFO_GET_INFO_GET_ID,1)) 
  { 
    ret = ui_pcp_oam_api_general_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_CALLBACK_FUNCTION_REGISTER_OAM_CALLBACK_FUNCTION_REGISTER_ID,1)) 
  { 
    ret = ui_pcp_oam_api_general_oam_callback_function_register(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_INTERRUPT_HANDLER_OAM_INTERRUPT_HANDLER_ID,1)) 
  { 
    ret = ui_pcp_oam_api_general_oam_interrupt_handler(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_OAM_MSG_INFO_GET_OAM_MSG_INFO_GET_ID,1)) 
  { 
    ret = ui_pcp_oam_api_general_oam_msg_info_get(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after general***", TRUE); 
  }
  
  ui_ret = ret; 
    
  goto exit; 
exit:        
  return ui_ret; 
}                
          
#endif /* general */ 


#endif /* LINK_PCP_LIBRARIES */ 

