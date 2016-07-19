/* $Id: ui_pb_api_action_cmd.c,v 1.6 Broadcom SDK $
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
  
#include <soc/dpp/Petra/PB_TM/pb_api_action_cmd.h>
#include <appl/dpp/UserInterface/pb/ui_pb_api_action_cmd.h>

#if LINK_PB_LIBRARIES

#ifdef UI_ACTION_CMD
/******************************************************************** 
 *  Function handler: snoop_set (section action_cmd)
 ********************************************************************/
int 
  ui_pb_api_action_cmd_snoop_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_action_ndx;
  SOC_PB_ACTION_CMD_SNOOP_MIRROR_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_action_cmd"); 
  soc_sand_proc_name = "soc_pb_action_cmd_snoop_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_ACTION_CMD_SNOOP_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ACTION_CMD_SNOOP_SET_SNOOP_SET_ACTION_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_ACTION_CMD_SNOOP_SET_SNOOP_SET_ACTION_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_action_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter action_ndx after snoop_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_action_cmd_snoop_get(
          unit,
          prm_action_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_action_cmd_snoop_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_action_cmd_snoop_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ACTION_CMD_SNOOP_SET_SNOOP_SET_INFO_SNOOP_SIZE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_ACTION_CMD_SNOOP_SET_SNOOP_SET_INFO_SNOOP_SIZE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.size = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ACTION_CMD_SNOOP_SET_SNOOP_SET_INFO_SNOOP_PROB_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_ACTION_CMD_SNOOP_SET_SNOOP_SET_INFO_SNOOP_PROB_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.prob = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ACTION_CMD_SNOOP_SET_SNOOP_SET_INFO_CMD_IS_ING_MC_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_ACTION_CMD_SNOOP_SET_SNOOP_SET_INFO_CMD_IS_ING_MC_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.cmd.is_ing_mc = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ACTION_CMD_SNOOP_SET_SNOOP_SET_INFO_CMD_COUNTER_PTR_2_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_ACTION_CMD_SNOOP_SET_SNOOP_SET_INFO_CMD_COUNTER_PTR_2_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.cmd.counter_ptr_2.enable = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ACTION_CMD_SNOOP_SET_SNOOP_SET_INFO_CMD_COUNTER_PTR_2_VALUE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_ACTION_CMD_SNOOP_SET_SNOOP_SET_INFO_CMD_COUNTER_PTR_2_VALUE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.cmd.counter_ptr_2.value = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ACTION_CMD_SNOOP_SET_SNOOP_SET_INFO_CMD_COUNTER_PTR_1_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_ACTION_CMD_SNOOP_SET_SNOOP_SET_INFO_CMD_COUNTER_PTR_1_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.cmd.counter_ptr_1.enable = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ACTION_CMD_SNOOP_SET_SNOOP_SET_INFO_CMD_COUNTER_PTR_1_VALUE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_ACTION_CMD_SNOOP_SET_SNOOP_SET_INFO_CMD_COUNTER_PTR_1_VALUE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.cmd.counter_ptr_1.value = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ACTION_CMD_SNOOP_SET_SNOOP_SET_INFO_CMD_METER_DP_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_ACTION_CMD_SNOOP_SET_SNOOP_SET_INFO_CMD_METER_DP_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.cmd.meter_dp.enable = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ACTION_CMD_SNOOP_SET_SNOOP_SET_INFO_CMD_METER_DP_VALUE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_ACTION_CMD_SNOOP_SET_SNOOP_SET_INFO_CMD_METER_DP_VALUE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.cmd.meter_dp.value = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ACTION_CMD_SNOOP_SET_SNOOP_SET_INFO_CMD_METER_PTR_UP_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_ACTION_CMD_SNOOP_SET_SNOOP_SET_INFO_CMD_METER_PTR_UP_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.cmd.meter_ptr_up.enable = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ACTION_CMD_SNOOP_SET_SNOOP_SET_INFO_CMD_METER_PTR_UP_VALUE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_ACTION_CMD_SNOOP_SET_SNOOP_SET_INFO_CMD_METER_PTR_UP_VALUE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.cmd.meter_ptr_up.value = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ACTION_CMD_SNOOP_SET_SNOOP_SET_INFO_CMD_METER_PTR_LOW_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_ACTION_CMD_SNOOP_SET_SNOOP_SET_INFO_CMD_METER_PTR_LOW_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.cmd.meter_ptr_low.enable = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ACTION_CMD_SNOOP_SET_SNOOP_SET_INFO_CMD_METER_PTR_LOW_VALUE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_ACTION_CMD_SNOOP_SET_SNOOP_SET_INFO_CMD_METER_PTR_LOW_VALUE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.cmd.meter_ptr_low.value = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ACTION_CMD_SNOOP_SET_SNOOP_SET_INFO_CMD_DP_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_ACTION_CMD_SNOOP_SET_SNOOP_SET_INFO_CMD_DP_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.cmd.dp.enable = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ACTION_CMD_SNOOP_SET_SNOOP_SET_INFO_CMD_DP_VALUE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_ACTION_CMD_SNOOP_SET_SNOOP_SET_INFO_CMD_DP_VALUE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.cmd.dp.value = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ACTION_CMD_SNOOP_SET_SNOOP_SET_INFO_CMD_TC_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_ACTION_CMD_SNOOP_SET_SNOOP_SET_INFO_CMD_TC_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.cmd.tc.enable = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ACTION_CMD_SNOOP_SET_SNOOP_SET_INFO_CMD_TC_VALUE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_ACTION_CMD_SNOOP_SET_SNOOP_SET_INFO_CMD_TC_VALUE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.cmd.tc.value = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ACTION_CMD_SNOOP_SET_SNOOP_SET_INFO_CMD_DEST_ID_DEST_VAL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_ACTION_CMD_SNOOP_SET_SNOOP_SET_INFO_CMD_DEST_ID_DEST_VAL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.cmd.dest_id.id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ACTION_CMD_SNOOP_SET_SNOOP_SET_INFO_CMD_DEST_ID_DEST_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_ACTION_CMD_SNOOP_SET_SNOOP_SET_INFO_CMD_DEST_ID_DEST_TYPE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.cmd.dest_id.type = (SOC_TMC_DEST_TYPE)param_val->numeric_equivalent;
  }


  /* Call function */
  ret = soc_pb_action_cmd_snoop_set(
          unit,
          prm_action_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_action_cmd_snoop_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_action_cmd_snoop_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: snoop_get (section action_cmd)
 ********************************************************************/
int 
  ui_pb_api_action_cmd_snoop_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_action_ndx;
  SOC_PB_ACTION_CMD_SNOOP_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_action_cmd"); 
  soc_sand_proc_name = "soc_pb_action_cmd_snoop_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_ACTION_CMD_SNOOP_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ACTION_CMD_SNOOP_GET_SNOOP_GET_ACTION_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_ACTION_CMD_SNOOP_GET_SNOOP_GET_ACTION_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_action_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter action_ndx after snoop_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_action_cmd_snoop_get(
          unit,
          prm_action_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_action_cmd_snoop_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_action_cmd_snoop_get");   
    goto exit; 
  } 

  SOC_PB_ACTION_CMD_SNOOP_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: mirror_set (section action_cmd)
 ********************************************************************/
int 
  ui_pb_api_action_cmd_mirror_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_action_ndx;
  SOC_PB_ACTION_CMD_MIRROR_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_action_cmd"); 
  soc_sand_proc_name = "soc_pb_action_cmd_mirror_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_ACTION_CMD_MIRROR_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ACTION_CMD_MIRROR_SET_MIRROR_SET_ACTION_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_ACTION_CMD_MIRROR_SET_MIRROR_SET_ACTION_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_action_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter action_ndx after mirror_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_action_cmd_mirror_get(
          unit,
          prm_action_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_action_cmd_mirror_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_action_cmd_mirror_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ACTION_CMD_MIRROR_SET_MIRROR_SET_INFO_MIRROR_PROB_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_ACTION_CMD_MIRROR_SET_MIRROR_SET_INFO_MIRROR_PROB_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.prob = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ACTION_CMD_MIRROR_SET_MIRROR_SET_INFO_CMD_IS_ING_MC_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_ACTION_CMD_MIRROR_SET_MIRROR_SET_INFO_CMD_IS_ING_MC_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.cmd.is_ing_mc = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ACTION_CMD_MIRROR_SET_MIRROR_SET_INFO_CMD_COUNTER_PTR_2_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_ACTION_CMD_MIRROR_SET_MIRROR_SET_INFO_CMD_COUNTER_PTR_2_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.cmd.counter_ptr_2.enable = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ACTION_CMD_MIRROR_SET_MIRROR_SET_INFO_CMD_COUNTER_PTR_2_VALUE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_ACTION_CMD_MIRROR_SET_MIRROR_SET_INFO_CMD_COUNTER_PTR_2_VALUE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.cmd.counter_ptr_2.value = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ACTION_CMD_MIRROR_SET_MIRROR_SET_INFO_CMD_COUNTER_PTR_1_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_ACTION_CMD_MIRROR_SET_MIRROR_SET_INFO_CMD_COUNTER_PTR_1_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.cmd.counter_ptr_1.enable = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ACTION_CMD_MIRROR_SET_MIRROR_SET_INFO_CMD_COUNTER_PTR_1_VALUE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_ACTION_CMD_MIRROR_SET_MIRROR_SET_INFO_CMD_COUNTER_PTR_1_VALUE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.cmd.counter_ptr_1.value = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ACTION_CMD_MIRROR_SET_MIRROR_SET_INFO_CMD_METER_DP_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_ACTION_CMD_MIRROR_SET_MIRROR_SET_INFO_CMD_METER_DP_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.cmd.meter_dp.enable = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ACTION_CMD_MIRROR_SET_MIRROR_SET_INFO_CMD_METER_DP_VALUE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_ACTION_CMD_MIRROR_SET_MIRROR_SET_INFO_CMD_METER_DP_VALUE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.cmd.meter_dp.value = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ACTION_CMD_MIRROR_SET_MIRROR_SET_INFO_CMD_METER_PTR_UP_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_ACTION_CMD_MIRROR_SET_MIRROR_SET_INFO_CMD_METER_PTR_UP_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.cmd.meter_ptr_up.enable = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ACTION_CMD_MIRROR_SET_MIRROR_SET_INFO_CMD_METER_PTR_UP_VALUE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_ACTION_CMD_MIRROR_SET_MIRROR_SET_INFO_CMD_METER_PTR_UP_VALUE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.cmd.meter_ptr_up.value = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ACTION_CMD_MIRROR_SET_MIRROR_SET_INFO_CMD_METER_PTR_LOW_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_ACTION_CMD_MIRROR_SET_MIRROR_SET_INFO_CMD_METER_PTR_LOW_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.cmd.meter_ptr_low.enable = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ACTION_CMD_MIRROR_SET_MIRROR_SET_INFO_CMD_METER_PTR_LOW_VALUE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_ACTION_CMD_MIRROR_SET_MIRROR_SET_INFO_CMD_METER_PTR_LOW_VALUE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.cmd.meter_ptr_low.value = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ACTION_CMD_MIRROR_SET_MIRROR_SET_INFO_CMD_DP_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_ACTION_CMD_MIRROR_SET_MIRROR_SET_INFO_CMD_DP_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.cmd.dp.enable = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ACTION_CMD_MIRROR_SET_MIRROR_SET_INFO_CMD_DP_VALUE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_ACTION_CMD_MIRROR_SET_MIRROR_SET_INFO_CMD_DP_VALUE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.cmd.dp.value = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ACTION_CMD_MIRROR_SET_MIRROR_SET_INFO_CMD_TC_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_ACTION_CMD_MIRROR_SET_MIRROR_SET_INFO_CMD_TC_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.cmd.tc.enable = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ACTION_CMD_MIRROR_SET_MIRROR_SET_INFO_CMD_TC_VALUE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_ACTION_CMD_MIRROR_SET_MIRROR_SET_INFO_CMD_TC_VALUE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.cmd.tc.value = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ACTION_CMD_MIRROR_SET_MIRROR_SET_INFO_CMD_DEST_ID_DEST_VAL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_ACTION_CMD_MIRROR_SET_MIRROR_SET_INFO_CMD_DEST_ID_DEST_VAL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.cmd.dest_id.id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ACTION_CMD_MIRROR_SET_MIRROR_SET_INFO_CMD_DEST_ID_DEST_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_ACTION_CMD_MIRROR_SET_MIRROR_SET_INFO_CMD_DEST_ID_DEST_TYPE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.cmd.dest_id.type = (SOC_TMC_DEST_TYPE)param_val->numeric_equivalent;
  }




  /* Call function */
  ret = soc_pb_action_cmd_mirror_set(
          unit,
          prm_action_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_action_cmd_mirror_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_action_cmd_mirror_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: mirror_get (section action_cmd)
 ********************************************************************/
int 
  ui_pb_api_action_cmd_mirror_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_action_ndx;
  SOC_PB_ACTION_CMD_MIRROR_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_action_cmd"); 
  soc_sand_proc_name = "soc_pb_action_cmd_mirror_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_ACTION_CMD_MIRROR_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ACTION_CMD_MIRROR_GET_MIRROR_GET_ACTION_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_ACTION_CMD_MIRROR_GET_MIRROR_GET_ACTION_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_action_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter action_ndx after mirror_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_action_cmd_mirror_get(
          unit,
          prm_action_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_action_cmd_mirror_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_action_cmd_mirror_get");   
    goto exit; 
  } 

  SOC_PB_ACTION_CMD_MIRROR_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
#endif
#ifdef UI_ACTION_CMD/* { action_cmd*/
/******************************************************************** 
 *  Section handler: action_cmd
 ********************************************************************/ 
int 
  ui_pb_api_action_cmd( 
    CURRENT_LINE *current_line 
  ) 
{   
  int
    ret = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_pb_api_action_cmd"); 
 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ACTION_CMD_SNOOP_SET_SNOOP_SET_ID,1)) 
  { 
    ret = ui_pb_api_action_cmd_snoop_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ACTION_CMD_SNOOP_GET_SNOOP_GET_ID,1)) 
  { 
    ret = ui_pb_api_action_cmd_snoop_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ACTION_CMD_MIRROR_SET_MIRROR_SET_ID,1)) 
  { 
    ret = ui_pb_api_action_cmd_mirror_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_ACTION_CMD_MIRROR_GET_MIRROR_GET_ID,1)) 
  { 
    ret = ui_pb_api_action_cmd_mirror_get(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after action_cmd***", TRUE); 
  }
  
  ui_ret = ret; 
    
  goto exit; 
exit:        
  return ui_ret; 
}                
          
#endif /* action_cmd */ 


#endif /* LINK_PB_LIBRARIES */ 

