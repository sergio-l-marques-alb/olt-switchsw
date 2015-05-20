/* $Id: ui_pb_api_ingress_traffic_mgmt.c,v 1.7 Broadcom SDK $
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
 
#include <soc/dpp/SAND/Utils/sand_os_interface.h> 
#include <appl/diag/dpp/utils_defi.h> 
#include <appl/dpp/UserInterface/ui_defi.h> 
#include <appl/dpp/UserInterface/ui_pure_defi_pb_pp_acc.h>
#include <appl/dpp/UserInterface/ui_pure_defi_pb_api.h> 
 
  
#include <soc/dpp/Petra/PB_TM/pb_api_ingress_traffic_mgmt.h>
#include <appl/dpp/UserInterface/pb/ui_pb_api_ingress_traffic_mgmt.h>

#if LINK_PB_LIBRARIES

#ifdef UI_INGRESS_TRAFFIC_MGMT
/******************************************************************** 
 *  Function handler: itm_stag_set (section ingress_traffic_mgmt)
 ********************************************************************/
int 
  ui_pb_api_ingress_traffic_mgmt_itm_stag_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_ITM_STAG_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_ingress_traffic_mgmt"); 
  soc_sand_proc_name = "soc_pb_itm_stag_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_ITM_STAG_INFO_clear(&prm_info);
 
  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_itm_stag_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_itm_stag_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_itm_stag_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ITM_STAG_SET_ITM_STAG_SET_INFO_OFFSET_4B_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_ITM_STAG_SET_ITM_STAG_SET_INFO_OFFSET_4B_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.offset_4bits = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ITM_STAG_SET_ITM_STAG_SET_INFO_ENABLE_MODE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_ITM_STAG_SET_ITM_STAG_SET_INFO_ENABLE_MODE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.enable_mode = param_val->numeric_equivalent;
  } 


  /* Call function */
  ret = soc_pb_itm_stag_set(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_itm_stag_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_itm_stag_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: itm_stag_get (section ingress_traffic_mgmt)
 ********************************************************************/
int 
  ui_pb_api_ingress_traffic_mgmt_itm_stag_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_ITM_STAG_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_ingress_traffic_mgmt"); 
  soc_sand_proc_name = "soc_pb_itm_stag_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_ITM_STAG_INFO_clear(&prm_info);
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_pb_itm_stag_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_itm_stag_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_itm_stag_get");   
    goto exit; 
  } 

  SOC_PB_ITM_STAG_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: itm_committed_q_size_set (section ingress_traffic_mgmt)
 ********************************************************************/
int 
  ui_pb_api_ingress_traffic_mgmt_itm_committed_q_size_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_rt_cls4_ndx;
  uint32   
    prm_grnt_bytes;
  uint32   
    prm_exact_grnt_bytes;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_ingress_traffic_mgmt"); 
  soc_sand_proc_name = "soc_pb_itm_committed_q_size_set"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ITM_COMMITTED_Q_SIZE_SET_ITM_COMMITTED_Q_SIZE_SET_RT_CLS4_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_ITM_COMMITTED_Q_SIZE_SET_ITM_COMMITTED_Q_SIZE_SET_RT_CLS4_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_rt_cls4_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter rt_cls4_ndx after itm_committed_q_size_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_itm_committed_q_size_get(
          unit,
          prm_rt_cls4_ndx,
          &prm_grnt_bytes
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_itm_committed_q_size_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_itm_committed_q_size_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ITM_COMMITTED_Q_SIZE_SET_ITM_COMMITTED_Q_SIZE_SET_GRNT_BYTES_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_ITM_COMMITTED_Q_SIZE_SET_ITM_COMMITTED_Q_SIZE_SET_GRNT_BYTES_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_grnt_bytes = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_itm_committed_q_size_set(
          unit,
          prm_rt_cls4_ndx,
          prm_grnt_bytes,
          &prm_exact_grnt_bytes
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_itm_committed_q_size_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_itm_committed_q_size_set");   
    goto exit; 
  } 

  soc_sand_os_printf("exact_grnt_bytes: %u\n\r",prm_exact_grnt_bytes);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: itm_committed_q_size_get (section ingress_traffic_mgmt)
 ********************************************************************/
int 
  ui_pb_api_ingress_traffic_mgmt_itm_committed_q_size_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_rt_cls4_ndx;
  uint32   
    prm_grnt_bytes;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_ingress_traffic_mgmt"); 
  soc_sand_proc_name = "soc_pb_itm_committed_q_size_get"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ITM_COMMITTED_Q_SIZE_GET_ITM_COMMITTED_Q_SIZE_GET_RT_CLS4_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_ITM_COMMITTED_Q_SIZE_GET_ITM_COMMITTED_Q_SIZE_GET_RT_CLS4_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_rt_cls4_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter rt_cls4_ndx after itm_committed_q_size_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_itm_committed_q_size_get(
          unit,
          prm_rt_cls4_ndx,
          &prm_grnt_bytes
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_itm_committed_q_size_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_itm_committed_q_size_get");   
    goto exit; 
  } 

  soc_sand_os_printf("grnt_bytes: %u\n\r",prm_grnt_bytes);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
#endif
#ifdef UI_INGRESS_TRAFFIC_MGMT/* { ingress_traffic_mgmt*/
/******************************************************************** 
 *  Section handler: ingress_traffic_mgmt
 ********************************************************************/ 
int 
  ui_pb_api_ingress_traffic_mgmt( 
    CURRENT_LINE *current_line 
  ) 
{   
  int
    ret = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_pb_api_ingress_traffic_mgmt"); 
 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ITM_STAG_SET_ITM_STAG_SET_ID,1)) 
  { 
    ret = ui_pb_api_ingress_traffic_mgmt_itm_stag_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ITM_STAG_GET_ITM_STAG_GET_ID,1)) 
  { 
    ret = ui_pb_api_ingress_traffic_mgmt_itm_stag_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ITM_COMMITTED_Q_SIZE_SET_ITM_COMMITTED_Q_SIZE_SET_ID,1)) 
  { 
    ret = ui_pb_api_ingress_traffic_mgmt_itm_committed_q_size_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ITM_COMMITTED_Q_SIZE_GET_ITM_COMMITTED_Q_SIZE_GET_ID,1)) 
  { 
    ret = ui_pb_api_ingress_traffic_mgmt_itm_committed_q_size_get(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after ingress_traffic_mgmt***", TRUE); 
  }
  
  ui_ret = ret; 
    
  goto exit; 
exit:        
  return ui_ret; 
}                
          
#endif /* ingress_traffic_mgmt */ 


#endif /* LINK_PB_LIBRARIES */ 

