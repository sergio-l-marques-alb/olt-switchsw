/* $Id: ui_ppd_api_counting.c,v 1.5 Broadcom SDK $
 * $Copyright: Copyright 2015 Broadcom Corporation.
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





#include <soc/dpp/PPD/ppd_api_counting.h>                                                               
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_counting.h>
 
#if LINK_PPD_LIBRARIES


#ifdef UI_COUNTING
/******************************************************************** 
*  Function handler: cnt_counter_type_set (section counting)
********************************************************************/
int 
ui_ppd_api_counting_cnt_counter_type_set(
  CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPD_CNT_COUNTER_ID   
    prm_counter_key;
  SOC_PPD_CNT_COUNTER_TYPE   
    prm_counter_type;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_counting"); 
  soc_sand_proc_name = "soc_ppd_cnt_counter_type_set"; 

  unit = soc_ppd_get_default_unit(); 
  SOC_PPD_CNT_COUNTER_ID_clear(&prm_counter_key);

  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_CNT_COUNTER_TYPE_SET_CNT_COUNTER_TYPE_SET_COUNTER_KEY_ID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_CNT_COUNTER_TYPE_SET_CNT_COUNTER_TYPE_SET_COUNTER_KEY_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_counter_key.id = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_CNT_COUNTER_TYPE_SET_CNT_COUNTER_TYPE_SET_COUNTER_KEY_GROUP_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_CNT_COUNTER_TYPE_SET_CNT_COUNTER_TYPE_SET_COUNTER_KEY_GROUP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_counter_key.group = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_CNT_COUNTER_TYPE_SET_CNT_COUNTER_TYPE_SET_COUNTER_KEY_STAGE_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_CNT_COUNTER_TYPE_SET_CNT_COUNTER_TYPE_SET_COUNTER_KEY_STAGE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_counter_key.stage = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter counter_key after cnt_counter_type_set***", TRUE); 
    goto exit; 
  } 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_cnt_counter_type_get(
    unit,
    &prm_counter_key,
    &prm_counter_type
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_cnt_counter_type_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_cnt_counter_type_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_CNT_COUNTER_TYPE_SET_CNT_COUNTER_TYPE_SET_COUNTER_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_CNT_COUNTER_TYPE_SET_CNT_COUNTER_TYPE_SET_COUNTER_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_counter_type = param_val->numeric_equivalent;
  } 

  /* Call function */  
  ret = soc_ppd_cnt_counter_type_set(
    unit,
    &prm_counter_key,
    prm_counter_type
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_cnt_counter_type_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_cnt_counter_type_set");   
    goto exit; 
  } 


  goto exit; 
exit: 
  return ui_ret; 
} 

/******************************************************************** 
*  Function handler: cnt_counter_type_get (section counting)
********************************************************************/
int 
ui_ppd_api_counting_cnt_counter_type_get(
  CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPD_CNT_COUNTER_ID   
    prm_counter_key;
  SOC_PPD_CNT_COUNTER_TYPE   
    prm_counter_type;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_counting"); 
  soc_sand_proc_name = "soc_ppd_cnt_counter_type_get"; 

  unit = soc_ppd_get_default_unit(); 
  SOC_PPD_CNT_COUNTER_ID_clear(&prm_counter_key);

  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_CNT_COUNTER_TYPE_GET_CNT_COUNTER_TYPE_GET_COUNTER_KEY_ID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_CNT_COUNTER_TYPE_GET_CNT_COUNTER_TYPE_GET_COUNTER_KEY_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_counter_key.id = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_CNT_COUNTER_TYPE_GET_CNT_COUNTER_TYPE_GET_COUNTER_KEY_GROUP_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_CNT_COUNTER_TYPE_GET_CNT_COUNTER_TYPE_GET_COUNTER_KEY_GROUP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_counter_key.group = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_CNT_COUNTER_TYPE_GET_CNT_COUNTER_TYPE_GET_COUNTER_KEY_STAGE_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_CNT_COUNTER_TYPE_GET_CNT_COUNTER_TYPE_GET_COUNTER_KEY_STAGE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_counter_key.stage = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter counter_key after cnt_counter_type_get***", TRUE); 
    goto exit; 
  } 

  /* Call function */  
  ret = soc_ppd_cnt_counter_type_get(
    unit,
    &prm_counter_key,
    &prm_counter_type
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_cnt_counter_type_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_cnt_counter_type_get");   
    goto exit; 
  } 

  cli_out("counter_type: %s\n",SOC_PPD_CNT_COUNTER_TYPE_to_string(prm_counter_type));


  goto exit; 
exit: 
  return ui_ret; 
} 

/******************************************************************** 
*  Function handler: cnt_counter_map_set (section counting)
********************************************************************/
int 
ui_ppd_api_counting_cnt_counter_map_set(
                                        CURRENT_LINE *current_line 
                                        ) 
{   
  uint32 
    ret;   
  SOC_PPD_CNT_KEY   
    prm_count_key;
  SOC_PPD_CNT_COUNTER_ID   
    prm_counter_id;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_counting"); 
  soc_sand_proc_name = "soc_ppd_cnt_counter_map_set"; 

  unit = soc_ppd_get_default_unit(); 
  SOC_PPD_CNT_KEY_clear(&prm_count_key);
  SOC_PPD_CNT_COUNTER_ID_clear(&prm_counter_id);

  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_CNT_COUNTER_MAP_SET_CNT_COUNTER_MAP_SET_COUNT_KEY_ID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_CNT_COUNTER_MAP_SET_CNT_COUNTER_MAP_SET_COUNT_KEY_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_count_key.id = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_CNT_COUNTER_MAP_SET_CNT_COUNTER_MAP_SET_COUNT_KEY_TYPE_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_CNT_COUNTER_MAP_SET_CNT_COUNTER_MAP_SET_COUNT_KEY_TYPE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_count_key.type = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter count_key after cnt_counter_map_set***", TRUE); 
    goto exit; 
  } 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_cnt_counter_map_get(
    unit,
    &prm_count_key,
    &prm_counter_id
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_cnt_counter_map_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_cnt_counter_map_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_CNT_COUNTER_MAP_SET_CNT_COUNTER_MAP_SET_COUNTER_ID_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_CNT_COUNTER_MAP_SET_CNT_COUNTER_MAP_SET_COUNTER_ID_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_counter_id.id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_CNT_COUNTER_MAP_SET_CNT_COUNTER_MAP_SET_COUNTER_ID_GROUP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_CNT_COUNTER_MAP_SET_CNT_COUNTER_MAP_SET_COUNTER_ID_GROUP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_counter_id.group = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_CNT_COUNTER_MAP_SET_CNT_COUNTER_MAP_SET_COUNTER_ID_STAGE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_CNT_COUNTER_MAP_SET_CNT_COUNTER_MAP_SET_COUNTER_ID_STAGE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_counter_id.stage = param_val->numeric_equivalent;
  } 

  /* Call function */  
  ret = soc_ppd_cnt_counter_map_set(
    unit,
    &prm_count_key,
    &prm_counter_id
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_cnt_counter_map_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_cnt_counter_map_set");   
    goto exit; 
  } 


  goto exit; 
exit: 
  return ui_ret; 
} 

/******************************************************************** 
*  Function handler: cnt_counter_map_get (section counting)
********************************************************************/
int 
ui_ppd_api_counting_cnt_counter_map_get(
                                        CURRENT_LINE *current_line 
                                        ) 
{   
  uint32 
    ret;   
  SOC_PPD_CNT_KEY   
    prm_count_key;
  SOC_PPD_CNT_COUNTER_ID   
    prm_counter_id;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_counting"); 
  soc_sand_proc_name = "soc_ppd_cnt_counter_map_get"; 

  unit = soc_ppd_get_default_unit(); 
  SOC_PPD_CNT_KEY_clear(&prm_count_key);
  SOC_PPD_CNT_COUNTER_ID_clear(&prm_counter_id);

  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_CNT_COUNTER_MAP_GET_CNT_COUNTER_MAP_GET_COUNT_KEY_ID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_CNT_COUNTER_MAP_GET_CNT_COUNTER_MAP_GET_COUNT_KEY_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_count_key.id = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_CNT_COUNTER_MAP_GET_CNT_COUNTER_MAP_GET_COUNT_KEY_TYPE_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_CNT_COUNTER_MAP_GET_CNT_COUNTER_MAP_GET_COUNT_KEY_TYPE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_count_key.type = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter count_key after cnt_counter_map_get***", TRUE); 
    goto exit; 
  } 

  /* Call function */  
  ret = soc_ppd_cnt_counter_map_get(
    unit,
    &prm_count_key,
    &prm_counter_id
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_cnt_counter_map_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_cnt_counter_map_get");   
    goto exit; 
  } 

  SOC_PPD_CNT_COUNTER_ID_print(&prm_counter_id);


  goto exit; 
exit: 
  return ui_ret; 
} 

/******************************************************************** 
*  Function handler: cnt_counter_ins_range_map_set (section counting)
********************************************************************/
int 
ui_ppd_api_counting_cnt_counter_ins_range_map_set(
  CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPD_CNT_TYPE   
    prm_countered_type_ndx;
  uint32   
    prm_first_countered_ndx;
  SOC_PPD_CNT_COUNTER_ID   
    prm_first_counter_ins_id;
  uint32   
    prm_nof_counter_ins;
  SOC_SAND_SUCCESS_FAILURE   
    prm_success;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_counting"); 
  soc_sand_proc_name = "soc_ppd_cnt_counter_ins_range_map_set"; 

  unit = soc_ppd_get_default_unit(); 
  SOC_PPD_CNT_COUNTER_ID_clear(&prm_first_counter_ins_id);

  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_CNT_COUNTER_INS_RANGE_MAP_SET_CNT_COUNTER_INS_RANGE_MAP_SET_COUNTERED_TYPE_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_CNT_COUNTER_INS_RANGE_MAP_SET_CNT_COUNTER_INS_RANGE_MAP_SET_COUNTERED_TYPE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_countered_type_ndx = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter countered_type_ndx after cnt_counter_ins_range_map_set***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_CNT_COUNTER_INS_RANGE_MAP_SET_CNT_COUNTER_INS_RANGE_MAP_SET_FIRST_COUNTERED_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_CNT_COUNTER_INS_RANGE_MAP_SET_CNT_COUNTER_INS_RANGE_MAP_SET_FIRST_COUNTERED_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_first_countered_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter first_countered_ndx after cnt_counter_ins_range_map_set***", TRUE); 
    goto exit; 
  } 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_cnt_counter_ins_range_map_get(
    unit,
    prm_countered_type_ndx,
    prm_first_countered_ndx,
    &prm_first_counter_ins_id,
    &prm_nof_counter_ins
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_cnt_counter_ins_range_map_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_cnt_counter_ins_range_map_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_CNT_COUNTER_INS_RANGE_MAP_SET_CNT_COUNTER_INS_RANGE_MAP_SET_FIRST_COUNTER_INS_ID_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_CNT_COUNTER_INS_RANGE_MAP_SET_CNT_COUNTER_INS_RANGE_MAP_SET_FIRST_COUNTER_INS_ID_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_first_counter_ins_id.id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_CNT_COUNTER_INS_RANGE_MAP_SET_CNT_COUNTER_INS_RANGE_MAP_SET_FIRST_COUNTER_INS_ID_GROUP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_CNT_COUNTER_INS_RANGE_MAP_SET_CNT_COUNTER_INS_RANGE_MAP_SET_FIRST_COUNTER_INS_ID_GROUP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_first_counter_ins_id.group = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_CNT_COUNTER_INS_RANGE_MAP_SET_CNT_COUNTER_INS_RANGE_MAP_SET_FIRST_COUNTER_INS_ID_STAGE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_CNT_COUNTER_INS_RANGE_MAP_SET_CNT_COUNTER_INS_RANGE_MAP_SET_FIRST_COUNTER_INS_ID_STAGE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_first_counter_ins_id.stage = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_CNT_COUNTER_INS_RANGE_MAP_SET_CNT_COUNTER_INS_RANGE_MAP_SET_NOF_COUNTER_INS_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_CNT_COUNTER_INS_RANGE_MAP_SET_CNT_COUNTER_INS_RANGE_MAP_SET_NOF_COUNTER_INS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_nof_counter_ins = (uint32)param_val->value.ulong_value;
  } 

  /* Call function */  
  ret = soc_ppd_cnt_counter_ins_range_map_set(
    unit,
    prm_countered_type_ndx,
    prm_first_countered_ndx,
    &prm_first_counter_ins_id,
    prm_nof_counter_ins,
    &prm_success
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_cnt_counter_ins_range_map_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_cnt_counter_ins_range_map_set");   
    goto exit; 
  } 

  cli_out("success: %s\n",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));


  goto exit; 
exit: 
  return ui_ret; 
} 

/******************************************************************** 
*  Function handler: cnt_counter_ins_range_map_get (section counting)
********************************************************************/
int 
ui_ppd_api_counting_cnt_counter_ins_range_map_get(
  CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPD_CNT_TYPE   
    prm_countered_type_ndx;
  uint32   
    prm_first_countered_ndx;
  SOC_PPD_CNT_COUNTER_ID   
    prm_first_counter_ins_id;
  uint32   
    prm_nof_counter_ins;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_counting"); 
  soc_sand_proc_name = "soc_ppd_cnt_counter_ins_range_map_get"; 

  unit = soc_ppd_get_default_unit(); 
  SOC_PPD_CNT_COUNTER_ID_clear(&prm_first_counter_ins_id);

  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_CNT_COUNTER_INS_RANGE_MAP_GET_CNT_COUNTER_INS_RANGE_MAP_GET_COUNTERED_TYPE_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_CNT_COUNTER_INS_RANGE_MAP_GET_CNT_COUNTER_INS_RANGE_MAP_GET_COUNTERED_TYPE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_countered_type_ndx = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter countered_type_ndx after cnt_counter_ins_range_map_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_CNT_COUNTER_INS_RANGE_MAP_GET_CNT_COUNTER_INS_RANGE_MAP_GET_FIRST_COUNTERED_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_CNT_COUNTER_INS_RANGE_MAP_GET_CNT_COUNTER_INS_RANGE_MAP_GET_FIRST_COUNTERED_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_first_countered_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter first_countered_ndx after cnt_counter_ins_range_map_get***", TRUE); 
    goto exit; 
  } 

  /* Call function */  
  ret = soc_ppd_cnt_counter_ins_range_map_get(
    unit,
    prm_countered_type_ndx,
    prm_first_countered_ndx,
    &prm_first_counter_ins_id,
    &prm_nof_counter_ins
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_cnt_counter_ins_range_map_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_cnt_counter_ins_range_map_get");   
    goto exit; 
  } 

  SOC_PPD_CNT_COUNTER_ID_print(&prm_first_counter_ins_id);

  cli_out("nof_counter_ins: %lu\n",prm_nof_counter_ins);


  goto exit; 
exit: 
  return ui_ret; 
} 

/******************************************************************** 
*  Function handler: cnt_counter_value_get (section counting)
********************************************************************/
int 
ui_ppd_api_counting_cnt_counter_value_get(
  CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPD_CNT_COUNTER_ID   
    prm_counter_key;
  uint8   
    prm_reset;
  SOC_PPD_CNT_RESULT   
    prm_val;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_counting"); 
  soc_sand_proc_name = "soc_ppd_cnt_counter_value_get"; 

  unit = soc_ppd_get_default_unit(); 
  SOC_PPD_CNT_COUNTER_ID_clear(&prm_counter_key);
  SOC_PPD_CNT_RESULT_clear(&prm_val);

  prm_reset = TRUE;

  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_CNT_COUNTER_VALUE_GET_CNT_COUNTER_VALUE_GET_COUNTER_KEY_ID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_CNT_COUNTER_VALUE_GET_CNT_COUNTER_VALUE_GET_COUNTER_KEY_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_counter_key.id = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_CNT_COUNTER_VALUE_GET_CNT_COUNTER_VALUE_GET_COUNTER_KEY_GROUP_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_CNT_COUNTER_VALUE_GET_CNT_COUNTER_VALUE_GET_COUNTER_KEY_GROUP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_counter_key.group = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_CNT_COUNTER_VALUE_GET_CNT_COUNTER_VALUE_GET_COUNTER_KEY_STAGE_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_CNT_COUNTER_VALUE_GET_CNT_COUNTER_VALUE_GET_COUNTER_KEY_STAGE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_counter_key.stage = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter counter_key after cnt_counter_value_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_CNT_COUNTER_VALUE_GET_CNT_COUNTER_VALUE_GET_RESET_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_CNT_COUNTER_VALUE_GET_CNT_COUNTER_VALUE_GET_RESET_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_reset = (uint8)param_val->value.ulong_value;
  } 

  /* Call function */  
  ret = soc_ppd_cnt_counter_value_get(
    unit,
    &prm_counter_key,
    prm_reset,
    &prm_val
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_cnt_counter_value_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_cnt_counter_value_get");   
    goto exit; 
  } 

  SOC_PPD_CNT_RESULT_print(&prm_val);


  goto exit; 
exit: 
  return ui_ret; 
} 

/******************************************************************** 
*  Section handler: counting
********************************************************************/ 
int 
  ui_ppd_api_counting( 
    CURRENT_LINE *current_line 
    ) 
{   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_counting"); 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_CNT_COUNTER_TYPE_SET_CNT_COUNTER_TYPE_SET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_counting_cnt_counter_type_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_CNT_COUNTER_TYPE_GET_CNT_COUNTER_TYPE_GET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_counting_cnt_counter_type_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_CNT_COUNTER_MAP_SET_CNT_COUNTER_MAP_SET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_counting_cnt_counter_map_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_CNT_COUNTER_MAP_GET_CNT_COUNTER_MAP_GET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_counting_cnt_counter_map_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_CNT_COUNTER_INS_RANGE_MAP_SET_CNT_COUNTER_INS_RANGE_MAP_SET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_counting_cnt_counter_ins_range_map_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_CNT_COUNTER_INS_RANGE_MAP_GET_CNT_COUNTER_INS_RANGE_MAP_GET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_counting_cnt_counter_ins_range_map_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_CNT_COUNTER_VALUE_GET_CNT_COUNTER_VALUE_GET_ID,1)) 
  { 
    ui_ret = ui_ppd_api_counting_cnt_counter_value_get(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after counting***", TRUE); 
  } 

  goto exit; 
exit:        
  return ui_ret; 
}

#endif

#endif /* LINK_PPD_LIBRARIES */
