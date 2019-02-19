/* $Id: ui_pb_api_cnt.c,v 1.7 Broadcom SDK $
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
 * General include file for reference design. 
 */ 
#include <appl/diag/dpp/ref_sys.h>  
/*
 *   #include <bcm_app/dpp/../H/usrApp.h> 
 */
/* 
 * Utilities include file. 
 */ 
 
#include <soc/dpp/SAND/Utils/sand_os_interface.h> 
#include <appl/diag/dpp/utils_defi.h> 
#include <appl/dpp/UserInterface/ui_defi.h> 
#include <appl/dpp/UserInterface/ui_pure_defi_pb_pp_acc.h>
#include <appl/dpp/UserInterface/ui_pure_defi_pb_api.h> 
   
#include <soc/dpp/Petra/PB_TM/pb_api_cnt.h>                                                               

#if LINK_PB_LIBRARIES
  
#ifdef UI_CNT
/******************************************************************** 
 *  Function handler: counters_set (section cnt)
 ********************************************************************/
int 
ui_pb_api_cnt_counters_set(
  CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_CNT_PROCESSOR_ID   
    prm_processor_ndx;
  SOC_PB_CNT_COUNTERS_INFO   
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_pb_api_cnt"); 
  soc_sand_proc_name = "soc_pb_cnt_counters_set"; 

  unit = soc_pb_get_default_unit(); 
  SOC_PB_CNT_COUNTERS_INFO_clear(&prm_info);

  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNT_COUNTERS_SET_COUNTERS_SET_PROCESSOR_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_CNT_COUNTERS_SET_COUNTERS_SET_PROCESSOR_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_processor_ndx = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter processor_ndx after counters_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_cnt_counters_get(
    unit,
    prm_processor_ndx,
    &prm_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_cnt_counters_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_cnt_counters_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNT_COUNTERS_SET_COUNTERS_SET_INFO_STAG_LSB_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_CNT_COUNTERS_SET_COUNTERS_SET_INFO_STAG_LSB_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.stag_lsb = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNT_COUNTERS_SET_COUNTERS_SET_INFO_VOQ_CNT_Q_SET_SIZE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_CNT_COUNTERS_SET_COUNTERS_SET_INFO_VOQ_CNT_Q_SET_SIZE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.voq_cnt.q_set_size = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNT_COUNTERS_SET_COUNTERS_SET_INFO_VOQ_CNT_START_Q_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_CNT_COUNTERS_SET_COUNTERS_SET_INFO_VOQ_CNT_START_Q_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.voq_cnt.start_q = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNT_COUNTERS_SET_COUNTERS_SET_INFO_MODE_EG_BASE_VAL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_CNT_COUNTERS_SET_COUNTERS_SET_INFO_MODE_EG_BASE_VAL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.mode_eg.base_val = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNT_COUNTERS_SET_COUNTERS_SET_INFO_MODE_EG_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_CNT_COUNTERS_SET_COUNTERS_SET_INFO_MODE_EG_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.mode_eg.type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNT_COUNTERS_SET_COUNTERS_SET_INFO_MODE_EG_RESOLUTION_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_CNT_COUNTERS_SET_COUNTERS_SET_INFO_MODE_EG_RESOLUTION_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.mode_eg.resolution = (SOC_PB_CNT_MODE_EG_RES)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNT_COUNTERS_SET_COUNTERS_SET_INFO_MODE_ING_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_CNT_COUNTERS_SET_COUNTERS_SET_INFO_MODE_ING_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.mode_ing = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNT_COUNTERS_SET_COUNTERS_SET_INFO_SRC_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_CNT_COUNTERS_SET_COUNTERS_SET_INFO_SRC_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.src_type = param_val->numeric_equivalent;
  } 


  /* Call function */
  ret = soc_pb_cnt_counters_set(
    unit,
    prm_processor_ndx,
    &prm_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_cnt_counters_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_cnt_counters_set");   
    goto exit; 
  } 


  goto exit; 
exit: 
  return ui_ret; 
} 

/******************************************************************** 
 *  Function handler: counters_get (section cnt)
 ********************************************************************/
int 
ui_pb_api_cnt_counters_get(
  CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_CNT_PROCESSOR_ID   
    prm_processor_ndx;
  SOC_PB_CNT_COUNTERS_INFO   
    prm_info;

  UI_MACROS_INIT_FUNCTION("ui_pb_api_cnt"); 
  soc_sand_proc_name = "soc_pb_cnt_counters_get"; 

  unit = soc_pb_get_default_unit(); 
  SOC_PB_CNT_COUNTERS_INFO_clear(&prm_info);

  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNT_COUNTERS_GET_COUNTERS_GET_PROCESSOR_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_CNT_COUNTERS_GET_COUNTERS_GET_PROCESSOR_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_processor_ndx = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter processor_ndx after counters_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_cnt_counters_get(
    unit,
    prm_processor_ndx,
    &prm_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_cnt_counters_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_cnt_counters_get");   
    goto exit; 
  } 

  SOC_PB_CNT_COUNTERS_INFO_print(&prm_info);


  goto exit; 
exit: 
  return ui_ret; 
} 
/******************************************************************** 
 *  Function handler: status_get (section cnt)
 ********************************************************************/
int 
  ui_pb_api_cnt_status_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_CNT_PROCESSOR_ID   
    prm_processor_ndx;
  SOC_PB_CNT_STATUS   
    prm_proc_status;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_cnt"); 
  soc_sand_proc_name = "soc_pb_cnt_status_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_CNT_STATUS_clear(&prm_proc_status);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNT_STATUS_GET_STATUS_GET_PROCESSOR_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_CNT_STATUS_GET_STATUS_GET_PROCESSOR_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_processor_ndx = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter processor_ndx after status_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_cnt_status_get(
          unit,
          prm_processor_ndx,
          &prm_proc_status
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_cnt_status_get - FAIL", TRUE); 
    soc_pb_disp_result(ret, "soc_pb_cnt_status_get");   
    goto exit; 
  } 

  SOC_PB_CNT_STATUS_print(&prm_proc_status);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: algorithmic_read (section cnt)
 ********************************************************************/
int 
  ui_pb_api_cnt_algorithmic_read(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_CNT_PROCESSOR_ID   
    prm_processor_ndx;
  SOC_PB_CNT_RESULT_ARR   
    prm_result_arr;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_cnt"); 
  soc_sand_proc_name = "soc_pb_cnt_algorithmic_read"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_CNT_RESULT_ARR_clear(&prm_result_arr);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNT_ALGORITHMIC_READ_ALGORITHMIC_READ_PROCESSOR_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_CNT_ALGORITHMIC_READ_ALGORITHMIC_READ_PROCESSOR_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_processor_ndx = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter processor_ndx after algorithmic_read***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_cnt_algorithmic_read(
          unit,
          prm_processor_ndx,
          &prm_result_arr
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_cnt_algorithmic_read - FAIL", TRUE); 
    soc_pb_disp_result(ret, "soc_pb_cnt_algorithmic_read");   
    goto exit; 
  } 

  SOC_PB_CNT_RESULT_ARR_print(&prm_result_arr);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: direct_read (section cnt)
 ********************************************************************/
int 
  ui_pb_api_cnt_direct_read(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_CNT_PROCESSOR_ID   
    prm_processor_ndx;
  uint32   
    prm_counter_ndx;
  SOC_PB_CNT_RESULT   
    prm_read_rslt;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_cnt"); 
  soc_sand_proc_name = "soc_pb_cnt_direct_read"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_CNT_RESULT_clear(&prm_read_rslt);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNT_DIRECT_READ_DIRECT_READ_PROCESSOR_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_CNT_DIRECT_READ_DIRECT_READ_PROCESSOR_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_processor_ndx = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter processor_ndx after direct_read***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNT_DIRECT_READ_DIRECT_READ_COUNTER_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_CNT_DIRECT_READ_DIRECT_READ_COUNTER_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_counter_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter counter_ndx after direct_read***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_cnt_direct_read(
          unit,
          prm_processor_ndx,
          prm_counter_ndx,
          &prm_read_rslt
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_cnt_direct_read - FAIL", TRUE); 
    soc_pb_disp_result(ret, "soc_pb_cnt_direct_read");   
    goto exit; 
  } 

  SOC_PB_CNT_RESULT_print(&prm_read_rslt);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: q2cnt_id (section cnt)
 ********************************************************************/
int 
  ui_pb_api_cnt_q2cnt_id(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_queue_ndx;
  uint32   
    prm_counter_ndx;
  SOC_PB_CNT_PROCESSOR_ID         processor_ndx = SOC_PB_CNT_PROCESSOR_ID_A;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_cnt"); 
  soc_sand_proc_name = "soc_pb_cnt_q2cnt_id"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNT_Q2CNT_ID_Q2CNT_ID_QUEUE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_CNT_Q2CNT_ID_Q2CNT_ID_QUEUE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_queue_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter queue_ndx after q2cnt_id***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_cnt_q2cnt_id(
          unit,
          processor_ndx,
          prm_queue_ndx,
          &prm_counter_ndx
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_cnt_q2cnt_id - FAIL", TRUE); 
    soc_pb_disp_result(ret, "soc_pb_cnt_q2cnt_id");   
    goto exit; 
  } 

  soc_sand_os_printf("counter_ndx: %d\n\r",prm_counter_ndx);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: cnt2q_id (section cnt)
 ********************************************************************/
int 
  ui_pb_api_cnt_cnt2q_id(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_counter_ndx;
  uint32   
    prm_queue_ndx;
  SOC_PB_CNT_PROCESSOR_ID         processor_ndx = SOC_PB_CNT_PROCESSOR_ID_A;
  UI_MACROS_INIT_FUNCTION("ui_pb_api_cnt"); 
  soc_sand_proc_name = "soc_pb_cnt_cnt2q_id"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNT_CNT2Q_ID_CNT2Q_ID_COUNTER_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_CNT_CNT2Q_ID_CNT2Q_ID_COUNTER_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_counter_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter counter_ndx after cnt2q_id***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_cnt_cnt2q_id(
          unit,
          processor_ndx,
          prm_counter_ndx,
          &prm_queue_ndx
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_cnt_cnt2q_id - FAIL", TRUE); 
    soc_pb_disp_result(ret, "soc_pb_cnt_cnt2q_id");   
    goto exit; 
  } 

  soc_sand_os_printf("queue_ndx: %d\n\r",prm_queue_ndx);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: meter_hdr_compensation_set (section cnt)
 ********************************************************************/
int 
  ui_pb_api_cnt_meter_hdr_compensation_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_port_ndx;
  int32   
    prm_hdr_compensation;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_cnt"); 
  soc_sand_proc_name = "soc_pb_cnt_meter_hdr_compensation_set"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNT_METER_HDR_COMPENSATION_SET_METER_HDR_COMPENSATION_SET_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_CNT_METER_HDR_COMPENSATION_SET_METER_HDR_COMPENSATION_SET_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter port_ndx after meter_hdr_compensation_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_cnt_meter_hdr_compensation_get(
          unit,
          prm_port_ndx,
          &prm_hdr_compensation
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_cnt_meter_hdr_compensation_get - FAIL", TRUE); 
    soc_pb_disp_result(ret, "soc_pb_cnt_meter_hdr_compensation_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNT_METER_HDR_COMPENSATION_SET_METER_HDR_COMPENSATION_SET_HDR_COMPENSATION_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_CNT_METER_HDR_COMPENSATION_SET_METER_HDR_COMPENSATION_SET_HDR_COMPENSATION_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_hdr_compensation = (int32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_cnt_meter_hdr_compensation_set(
          unit,
          prm_port_ndx,
          prm_hdr_compensation
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_cnt_meter_hdr_compensation_set - FAIL", TRUE); 
    soc_pb_disp_result(ret, "soc_pb_cnt_meter_hdr_compensation_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: meter_hdr_compensation_get (section cnt)
 ********************************************************************/
int 
  ui_pb_api_cnt_meter_hdr_compensation_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_port_ndx;
  int32   
    prm_hdr_compensation;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_cnt"); 
  soc_sand_proc_name = "soc_pb_cnt_meter_hdr_compensation_get"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNT_METER_HDR_COMPENSATION_GET_METER_HDR_COMPENSATION_GET_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_CNT_METER_HDR_COMPENSATION_GET_METER_HDR_COMPENSATION_GET_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter port_ndx after meter_hdr_compensation_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_cnt_meter_hdr_compensation_get(
          unit,
          prm_port_ndx,
          &prm_hdr_compensation
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_cnt_meter_hdr_compensation_get - FAIL", TRUE); 
    soc_pb_disp_result(ret, "soc_pb_cnt_meter_hdr_compensation_get");   
    goto exit; 
  } 

  soc_sand_os_printf("hdr_compensation: %d\n\r",prm_hdr_compensation);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
#endif
#ifdef UI_CNT/* { cnt*/
/******************************************************************** 
 *  Section handler: cnt
 ********************************************************************/ 
int 
  ui_pb_api_cnt( 
    CURRENT_LINE *current_line 
  ) 
{   
  int
    ret = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_pb_api_cnt"); 
 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNT_COUNTERS_SET_COUNTERS_SET_ID,1)) 
  { 
    ret = ui_pb_api_cnt_counters_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNT_COUNTERS_GET_COUNTERS_GET_ID,1)) 
  { 
    ret = ui_pb_api_cnt_counters_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNT_STATUS_GET_STATUS_GET_ID,1)) 
  { 
    ret = ui_pb_api_cnt_status_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNT_ALGORITHMIC_READ_ALGORITHMIC_READ_ID,1)) 
  { 
    ret = ui_pb_api_cnt_algorithmic_read(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNT_DIRECT_READ_DIRECT_READ_ID,1)) 
  { 
    ret = ui_pb_api_cnt_direct_read(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNT_Q2CNT_ID_Q2CNT_ID_ID,1)) 
  { 
    ret = ui_pb_api_cnt_q2cnt_id(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNT_CNT2Q_ID_CNT2Q_ID_ID,1)) 
  { 
    ret = ui_pb_api_cnt_cnt2q_id(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNT_METER_HDR_COMPENSATION_SET_METER_HDR_COMPENSATION_SET_ID,1)) 
  { 
    ret = ui_pb_api_cnt_meter_hdr_compensation_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_CNT_METER_HDR_COMPENSATION_GET_METER_HDR_COMPENSATION_GET_ID,1)) 
  { 
    ret = ui_pb_api_cnt_meter_hdr_compensation_get(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after cnt***", TRUE); 
  }
  
  ui_ret = ret; 
    
  goto exit; 
exit:        
  return ui_ret; 
}                
          
#endif /* cnt */ 


#endif /* LINK_PB_LIBRARIES */ 

