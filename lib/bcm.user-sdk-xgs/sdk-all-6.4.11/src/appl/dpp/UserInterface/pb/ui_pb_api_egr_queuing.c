/* $Id: ui_pb_api_egr_queuing.c,v 1.6 Broadcom SDK $
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
 
#include <soc/dpp/Petra/PB_TM/pb_framework.h> 
#include <soc/dpp/Petra/PB_TM/pb_api_egr_queuing.h>
#include <appl/dpp/UserInterface/pb/ui_pb_api_egr_queuing.h>

#if LINK_PB_LIBRARIES

#ifdef UI_EGR_QUEUING
/******************************************************************** 
 *  Function handler: egr_q_prio_set (section egr_queuing)
 ********************************************************************/
int 
  ui_pb_api_egr_queuing_egr_q_prio_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PETRA_EGR_Q_PRIO_MAPPING_TYPE   
    prm_map_type_ndx;
  uint32   
    prm_tc_ndx;
  uint32   
    prm_dp_ndx;
  uint32   
    prm_map_profile_ndx;
  SOC_PB_EGR_Q_PRIORITY   
    prm_priority;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_egr_queuing"); 
  soc_sand_proc_name = "soc_pb_egr_q_prio_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_EGR_Q_PRIORITY_clear(&prm_priority);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_Q_PRIO_SET_EGR_Q_PRIO_SET_MAP_TYPE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_Q_PRIO_SET_EGR_Q_PRIO_SET_MAP_TYPE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_map_type_ndx = (SOC_PETRA_EGR_Q_PRIO_MAPPING_TYPE)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter map_type_ndx after egr_q_prio_set***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_Q_PRIO_SET_EGR_Q_PRIO_SET_TC_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_Q_PRIO_SET_EGR_Q_PRIO_SET_TC_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_tc_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter tc_ndx after egr_q_prio_set***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_Q_PRIO_SET_EGR_Q_PRIO_SET_DP_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_Q_PRIO_SET_EGR_Q_PRIO_SET_DP_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_dp_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter dp_ndx after egr_q_prio_set***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_Q_PRIO_SET_EGR_Q_PRIO_SET_MAP_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_Q_PRIO_SET_EGR_Q_PRIO_SET_MAP_PROFILE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_map_profile_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter map_profile_ndx after egr_q_prio_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_egr_q_prio_get(
          unit,
          prm_map_type_ndx,
          prm_tc_ndx,
          prm_dp_ndx,
          prm_map_profile_ndx,
          &prm_priority
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_egr_q_prio_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_egr_q_prio_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_Q_PRIO_SET_EGR_Q_PRIO_SET_PRIORITY_DP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_Q_PRIO_SET_EGR_Q_PRIO_SET_PRIORITY_DP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_priority.dp = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_Q_PRIO_SET_EGR_Q_PRIO_SET_PRIORITY_TC_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_Q_PRIO_SET_EGR_Q_PRIO_SET_PRIORITY_TC_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_priority.tc = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_egr_q_prio_set(
          unit,
          prm_map_type_ndx,
          prm_tc_ndx,
          prm_dp_ndx,
          prm_map_profile_ndx,
          &prm_priority
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_egr_q_prio_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_egr_q_prio_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: egr_q_prio_get (section egr_queuing)
 ********************************************************************/
int 
  ui_pb_api_egr_queuing_egr_q_prio_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PETRA_EGR_Q_PRIO_MAPPING_TYPE   
    prm_map_type_ndx;
  uint32   
    prm_tc_ndx;
  uint32   
    prm_dp_ndx;
  uint32   
    prm_map_profile_ndx;
  SOC_PB_EGR_Q_PRIORITY   
    prm_priority;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_egr_queuing"); 
  soc_sand_proc_name = "soc_pb_egr_q_prio_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_EGR_Q_PRIORITY_clear(&prm_priority);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_Q_PRIO_GET_EGR_Q_PRIO_GET_MAP_TYPE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_Q_PRIO_GET_EGR_Q_PRIO_GET_MAP_TYPE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_map_type_ndx = (SOC_PETRA_EGR_Q_PRIO_MAPPING_TYPE)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter map_type_ndx after egr_q_prio_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_Q_PRIO_GET_EGR_Q_PRIO_GET_TC_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_Q_PRIO_GET_EGR_Q_PRIO_GET_TC_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_tc_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter tc_ndx after egr_q_prio_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_Q_PRIO_GET_EGR_Q_PRIO_GET_DP_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_Q_PRIO_GET_EGR_Q_PRIO_GET_DP_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_dp_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter dp_ndx after egr_q_prio_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_Q_PRIO_GET_EGR_Q_PRIO_GET_MAP_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_Q_PRIO_GET_EGR_Q_PRIO_GET_MAP_PROFILE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_map_profile_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter map_profile_ndx after egr_q_prio_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_egr_q_prio_get(
          unit,
          prm_map_type_ndx,
          prm_tc_ndx,
          prm_dp_ndx,
          prm_map_profile_ndx,
          &prm_priority
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_egr_q_prio_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_egr_q_prio_get");   
    goto exit; 
  } 
  
  SOC_PB_EGR_Q_PRIORITY_print(&prm_priority);

  
  goto exit; 
exit: 
  return ui_ret; 
} 


/******************************************************************** 
 *  Function handler: egr_q_nif_cal_set (section egr_queuing)
 ********************************************************************/
int 
  ui_pb_api_egr_queuing_egr_q_nif_cal_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_egr_queuing"); 
  soc_sand_proc_name = "soc_pb_egr_q_nif_cal_set"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_egr_q_nif_cal_get(
          unit
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_egr_q_nif_cal_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_egr_q_nif_cal_get");   
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_egr_q_nif_cal_set(
          unit
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_egr_q_nif_cal_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_egr_q_nif_cal_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: egr_q_nif_cal_get (section egr_queuing)
 ********************************************************************/
int 
  ui_pb_api_egr_queuing_egr_q_nif_cal_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_egr_queuing"); 
  soc_sand_proc_name = "soc_pb_egr_q_nif_cal_get"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_pb_egr_q_nif_cal_get(
          unit
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_egr_q_nif_cal_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_egr_q_nif_cal_get");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
    
/******************************************************************** 
 *  Function handler: egr_q_profile_map_set (section egr_queuing)
 ********************************************************************/
int 
  ui_pb_api_egr_queuing_egr_q_profile_map_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PETRA_FAP_PORT_ID   
    prm_ofp_ndx;
  uint32   
    prm_map_profile_ndx;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_egr_queuing"); 
  soc_sand_proc_name = "soc_pb_egr_q_profile_map_set"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_Q_PROFILE_MAP_SET_EGR_Q_PROFILE_MAP_SET_OFP_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_Q_PROFILE_MAP_SET_EGR_Q_PROFILE_MAP_SET_OFP_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_ofp_ndx = (SOC_PETRA_FAP_PORT_ID)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter ofp_ndx after egr_q_profile_map_set***", TRUE); 
    goto exit; 
  } 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_egr_q_profile_map_get(
    unit,
    prm_ofp_ndx,
    &prm_map_profile_ndx
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_egr_q_profile_map_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_egr_q_profile_map_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_Q_PROFILE_MAP_SET_EGR_Q_PROFILE_MAP_SET_MAP_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_Q_PROFILE_MAP_SET_EGR_Q_PROFILE_MAP_SET_MAP_PROFILE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_map_profile_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter map_profile_ndx after egr_q_profile_map_set***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_egr_q_profile_map_set(
          unit,
          prm_ofp_ndx,
          prm_map_profile_ndx
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_egr_q_profile_map_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_egr_q_profile_map_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: egr_q_profile_map_get (section egr_queuing)
 ********************************************************************/

int 
  ui_pb_api_egr_queuing_egr_q_cal_get_and_print(
    CURRENT_LINE *current_line 
    ) 
{   
  uint32 
    ret = 0;   
 
  UI_MACROS_INIT_FUNCTION("ui_pb_api_egr_queuing"); 
  soc_sand_proc_name = "soc_pb_egr_q_profile_map_get"; 

  unit = soc_pb_get_default_unit(); 

  /* Get parameters */ 
 

  /* Call function */
  soc_pb_egr_q_nif_cal_get_and_print(
    unit
      );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_egr_q_nif_cal_get_and_print - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_egr_q_nif_cal_get_and_print");   
    goto exit; 
  } 




  goto exit; 
exit: 
  return ui_ret; 
} 

int ui_pb_api_egr_q_nif_cal_set(
  CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret = 0;   

  UI_MACROS_INIT_FUNCTION("ui_pb_api_egr_q_nif_cal_set"); 
  soc_sand_proc_name = "ui_pb_api_egr_q_nif_cal_set"; 

  unit = soc_pb_get_default_unit(); 

  /* Get parameters */ 


  /* Call function */
   ret = soc_pb_egr_q_nif_cal_get(
    unit
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_egr_q_nif_cal_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_egr_q_nif_cal_set");   
    goto exit; 
  } 




  goto exit; 
exit: 
  return ui_ret; 
} 


int 
  ui_pb_api_egr_queuing_egr_q_profile_map_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PETRA_FAP_PORT_ID   
    prm_ofp_ndx;
  uint32   
    prm_map_profile_ndx;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_egr_queuing"); 
  soc_sand_proc_name = "soc_pb_egr_q_profile_map_get"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_Q_PROFILE_MAP_GET_EGR_Q_PROFILE_MAP_GET_OFP_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_EGR_Q_PROFILE_MAP_GET_EGR_Q_PROFILE_MAP_GET_OFP_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_ofp_ndx = (SOC_PETRA_FAP_PORT_ID)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter ofp_ndx after egr_q_profile_map_get***", TRUE); 
    goto exit; 
  } 

  /* Call function */
  ret = soc_pb_egr_q_profile_map_get(
          unit,
          prm_ofp_ndx,
          &prm_map_profile_ndx
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_egr_q_profile_map_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_egr_q_profile_map_get");   
    goto exit; 
  } 
  
  soc_sand_os_printf("map_profile_ndx: %d\n\r",prm_map_profile_ndx);
  
  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
#endif
#ifdef UI_EGR_QUEUING/* { egr_queuing*/
/******************************************************************** 
 *  Section handler: egr_queuing
 ********************************************************************/ 
int 
  ui_pb_api_egr_queuing( 
    CURRENT_LINE *current_line 
  ) 
{   
  int
    ret = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_pb_api_egr_queuing"); 
 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_Q_PRIO_SET_EGR_Q_PRIO_SET_ID,1)) 
  { 
    ret = ui_pb_api_egr_queuing_egr_q_prio_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_Q_PRIO_GET_EGR_Q_PRIO_GET_ID,1)) 
  { 
    ret = ui_pb_api_egr_queuing_egr_q_prio_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_Q_PROFILE_MAP_SET_EGR_Q_PROFILE_MAP_SET_ID,1)) 
  { 
    ret = ui_pb_api_egr_queuing_egr_q_profile_map_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_Q_PROFILE_MAP_GET_EGR_Q_PROFILE_MAP_GET_ID,1)) 
  { 
    ret = ui_pb_api_egr_queuing_egr_q_profile_map_get(current_line); 
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_API_EGR_Q_CAL_GET_AND_PRINT_ID,1)) 
  { 
    ret = ui_pb_api_egr_queuing_egr_q_cal_get_and_print(current_line); 
  }  
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_API_EGR_Q_CAL_SET_ID,1)) 
  { 
    ret = ui_pb_api_egr_q_nif_cal_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_Q_NIF_CAL_SET_EGR_Q_NIF_CAL_SET_ID,1)) 
  { 
    ret = ui_pb_api_egr_queuing_egr_q_nif_cal_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_EGR_Q_NIF_CAL_GET_EGR_Q_NIF_CAL_GET_ID,1)) 
  { 
    ret = ui_pb_api_egr_queuing_egr_q_nif_cal_get(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after egr_queuing***", TRUE); 
  }
  
  ui_ret = ret; 
    
  goto exit; 
exit:        
  return ui_ret; 
}                
          
#endif /* egr_queuing */ 


#endif /* LINK_PB_LIBRARIES */ 

