/* $Id: ui_pb_api_ports.c,v 1.6 Broadcom SDK $
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
 
#include <soc/dpp/SAND/Utils/sand_os_interface.h> 
#include <appl/diag/dpp/utils_defi.h> 
#include <appl/dpp/UserInterface/ui_defi.h> 
#include <appl/dpp/UserInterface/ui_pure_defi_pb_pp_acc.h>
#include <appl/dpp/UserInterface/ui_pure_defi_pb_api.h> 
 
  
#include <soc/dpp/Petra/PB_TM/pb_api_ports.h>  

#if LINK_PB_LIBRARIES
   
#ifdef UI_PORTS
/******************************************************************** 
 *  Function handler: port_pp_port_add (section ports)
 ********************************************************************/
int 
  ui_pb_api_ports_port_pp_port_add(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_pp_port_ndx;
  SOC_PB_PORT_PP_PORT_INFO   
    prm_info;
  SOC_SAND_SUCCESS_FAILURE   
    prm_success;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_ports"); 
  soc_sand_proc_name = "soc_pb_port_pp_port_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_PORT_PP_PORT_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PORT_TM_PROFILE_ADD_PORT_TM_PROFILE_ADD_PP_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PORT_TM_PROFILE_ADD_PORT_TM_PROFILE_ADD_PP_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_pp_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter pp_port_ndx after port_pp_port_add***", TRUE); 
    goto exit; 
  } 
  ret = soc_pb_port_pp_port_get(
          unit,
          SOC_CORE_DEFAULT,
          prm_pp_port_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_port_pp_port_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_port_pp_port_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PORT_TM_PROFILE_ADD_PORT_TM_PROFILE_ADD_INFO_IS_PROFILE_FOR_RAW_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PORT_TM_PROFILE_ADD_PORT_TM_PROFILE_ADD_INFO_IS_PROFILE_FOR_RAW_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.header_type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PORT_TM_PROFILE_ADD_PORT_TM_PROFILE_ADD_INFO_IS_SNOOP_ENABLED_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PORT_TM_PROFILE_ADD_PORT_TM_PROFILE_ADD_INFO_IS_SNOOP_ENABLED_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.is_snoop_enabled = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PORT_TM_PROFILE_ADD_PORT_TM_PROFILE_ADD_INFO_IS_ING_SHAPING_ENABLED_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PORT_TM_PROFILE_ADD_PORT_TM_PROFILE_ADD_INFO_IS_ING_SHAPING_ENABLED_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.is_tm_ing_shaping_enabled = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PORT_TM_PROFILE_ADD_PORT_TM_PROFILE_ADD_INFO_IS_TM_PPH_PRESENT_ENABLED_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PORT_TM_PROFILE_ADD_PORT_TM_PROFILE_ADD_INFO_IS_TM_PPH_PRESENT_ENABLED_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.is_tm_pph_present_enabled = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PORT_TM_PROFILE_ADD_PORT_TM_PROFILE_ADD_INFO_IS_TM_SRC_SYST_PORT_EXT_PRESENT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PORT_TM_PROFILE_ADD_PORT_TM_PROFILE_ADD_INFO_IS_TM_SRC_SYST_PORT_EXT_PRESENT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.is_tm_src_syst_port_ext_present = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PORT_TM_PROFILE_ADD_PORT_TM_PROFILE_ADD_INFO_MIRROR_PROFILE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PORT_TM_PROFILE_ADD_PORT_TM_PROFILE_ADD_INFO_MIRROR_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.mirror_profile = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PORT_TM_PROFILE_ADD_PORT_TM_PROFILE_ADD_INFO_FC_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PORT_TM_PROFILE_ADD_PORT_TM_PROFILE_ADD_INFO_FC_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.fc_type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PORT_TM_PROFILE_ADD_PORT_TM_PROFILE_ADD_INFO_IS_FAT_PIPE_ENABLED_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PORT_TM_PROFILE_ADD_PORT_TM_PROFILE_ADD_INFO_IS_FAT_PIPE_ENABLED_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.first_header_size = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PORT_TM_PROFILE_ADD_PORT_TM_PROFILE_ADD_INFO_IS_STAG_ENABLED_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PORT_TM_PROFILE_ADD_PORT_TM_PROFILE_ADD_INFO_IS_STAG_ENABLED_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.is_stag_enabled = (uint8)param_val->value.ulong_value;
  } 

  /* Call function */
  ret = soc_pb_port_pp_port_set(
          unit,
          prm_pp_port_ndx,
          &prm_info,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_port_pp_port_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_port_pp_port_set");   
    goto exit; 
  } 

  soc_sand_os_printf("success: %s\n\r",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));
  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: port_pp_port_get (section ports)
 ********************************************************************/
int 
  ui_pb_api_ports_port_pp_port_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_pp_port_ndx;
  SOC_PB_PORT_PP_PORT_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_ports"); 
  soc_sand_proc_name = "soc_pb_port_pp_port_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_PORT_PP_PORT_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PORT_TM_PROFILE_GET_PORT_TM_PROFILE_GET_PP_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PORT_TM_PROFILE_GET_PORT_TM_PROFILE_GET_PP_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_pp_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter pp_port_ndx after port_pp_port_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_port_pp_port_get(
          unit,
          SOC_CORE_DEFAULT,
          prm_pp_port_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_port_pp_port_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_port_pp_port_get");   
    goto exit; 
  } 

  SOC_PB_PORT_PP_PORT_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: port_to_pp_port_map_set (section ports)
 ********************************************************************/
int 
  ui_pb_api_ports_port_to_pp_port_map_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_port_ndx;
  uint32   
    pp_port = 0;
  SOC_PETRA_PORT_DIRECTION
    direction = SOC_PETRA_PORT_DIRECTION_BOTH;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_ports"); 
  soc_sand_proc_name = "soc_pb_port_to_pp_port_map_set"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PORT_TO_PP_PORT_MAP_SET_PORT_TO_PP_PORT_MAP_SET_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PORT_TO_PP_PORT_MAP_SET_PORT_TO_PP_PORT_MAP_SET_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter port_ndx after port_to_pp_port_map_set***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PORT_TO_PP_PORT_MAP_SET_PORT_TO_PP_PORT_MAP_SET_DIRECTION_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PORT_TO_PP_PORT_MAP_SET_PORT_TO_PP_PORT_MAP_SET_DIRECTION_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    direction = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter direction after port_to_pp_port_map_set***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PORT_TO_PP_PORT_MAP_SET_PORT_TO_PP_PORT_MAP_SET_INFO_PROFILE_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PORT_TO_PP_PORT_MAP_SET_PORT_TO_PP_PORT_MAP_SET_INFO_PROFILE_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    pp_port = (uint32)param_val->value.ulong_value;
  } 

  /* Call function */
  ret = soc_pb_port_to_pp_port_map_set(
          unit,
          prm_port_ndx,
          direction,
          pp_port
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_port_to_pp_port_map_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_port_to_pp_port_map_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: port_to_pp_port_map_get (section ports)
 ********************************************************************/
int 
  ui_pb_api_ports_port_to_pp_port_map_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_port_ndx,
    pp_port_in,
    pp_port_out;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_ports"); 
  soc_sand_proc_name = "soc_pb_port_to_pp_port_map_get"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PORT_TO_PP_PORT_MAP_GET_PORT_TO_PP_PORT_MAP_GET_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PORT_TO_PP_PORT_MAP_GET_PORT_TO_PP_PORT_MAP_GET_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter port_ndx after port_to_pp_port_map_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_port_to_pp_port_map_get(
          unit,
          prm_port_ndx,
          &pp_port_in,
          &pp_port_out
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_port_to_pp_port_map_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_port_to_pp_port_map_get");   
    goto exit; 
  } 

  soc_sand_os_printf("pp_port_in: %d\n\r",pp_port_in);
  soc_sand_os_printf("pp_port_out: %d\n\r",pp_port_out);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: port_forwarding_header_set (section ports)
 ********************************************************************/
int 
  ui_pb_api_ports_port_forwarding_header_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_pp_port_ndx;
  SOC_PB_PORTS_FORWARDING_HEADER_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_ports"); 
  soc_sand_proc_name = "soc_pb_port_forwarding_header_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_PORTS_FORWARDING_HEADER_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PORT_FORWARDING_HEADER_SET_PORT_FORWARDING_HEADER_SET_PP_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PORT_FORWARDING_HEADER_SET_PORT_FORWARDING_HEADER_SET_PP_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_pp_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter pp_port_ndx after port_forwarding_header_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_port_forwarding_header_get(
          unit,
          prm_pp_port_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_port_forwarding_header_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_port_forwarding_header_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PORT_FORWARDING_HEADER_SET_PORT_FORWARDING_HEADER_SET_INFO_COUNTER_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PORT_FORWARDING_HEADER_SET_PORT_FORWARDING_HEADER_SET_INFO_COUNTER_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.counter.id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PORT_FORWARDING_HEADER_SET_PORT_FORWARDING_HEADER_SET_INFO_COUNTER_PROCESSOR_ID_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PORT_FORWARDING_HEADER_SET_PORT_FORWARDING_HEADER_SET_INFO_COUNTER_PROCESSOR_ID_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.counter.processor_id = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PORT_FORWARDING_HEADER_SET_PORT_FORWARDING_HEADER_SET_INFO_SNOOP_CMD_NDX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PORT_FORWARDING_HEADER_SET_PORT_FORWARDING_HEADER_SET_INFO_SNOOP_CMD_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.snoop_cmd_ndx = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PORT_FORWARDING_HEADER_SET_PORT_FORWARDING_HEADER_SET_INFO_DP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PORT_FORWARDING_HEADER_SET_PORT_FORWARDING_HEADER_SET_INFO_DP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.dp = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PORT_FORWARDING_HEADER_SET_PORT_FORWARDING_HEADER_SET_INFO_TR_CLS_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PORT_FORWARDING_HEADER_SET_PORT_FORWARDING_HEADER_SET_INFO_TR_CLS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.tr_cls = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PORT_FORWARDING_HEADER_SET_PORT_FORWARDING_HEADER_SET_INFO_DESTINATION_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PORT_FORWARDING_HEADER_SET_PORT_FORWARDING_HEADER_SET_INFO_DESTINATION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.destination.id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PORT_FORWARDING_HEADER_SET_PORT_FORWARDING_HEADER_SET_INFO_DESTINATION_ID_TYPE,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PORT_FORWARDING_HEADER_SET_PORT_FORWARDING_HEADER_SET_INFO_DESTINATION_ID_TYPE);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.destination.type = param_val->numeric_equivalent;
  }



  /* Call function */
  ret = soc_pb_port_forwarding_header_set(
          unit,
          prm_pp_port_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_port_forwarding_header_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_port_forwarding_header_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: port_forwarding_header_get (section ports)
 ********************************************************************/
int 
  ui_pb_api_ports_port_forwarding_header_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_pp_port_ndx;
  SOC_PB_PORTS_FORWARDING_HEADER_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_ports"); 
  soc_sand_proc_name = "soc_pb_port_forwarding_header_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_PORTS_FORWARDING_HEADER_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PORT_FORWARDING_HEADER_GET_PORT_FORWARDING_HEADER_GET_PP_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PORT_FORWARDING_HEADER_GET_PORT_FORWARDING_HEADER_GET_PP_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_pp_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter pp_port_ndx after port_forwarding_header_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_port_forwarding_header_get(
          unit,
          prm_pp_port_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_port_forwarding_header_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_port_forwarding_header_get");   
    goto exit; 
  } 

  SOC_PB_PORTS_FORWARDING_HEADER_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
#endif
#ifdef UI_PORTS/* { ports*/
/******************************************************************** 
 *  Section handler: ports
 ********************************************************************/ 
int 
  ui_pb_api_ports( 
    CURRENT_LINE *current_line 
  ) 
{   
  int
    ret = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_pb_api_ports"); 
 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PORT_TM_PROFILE_ADD_PORT_TM_PROFILE_ADD_ID,1)) 
  { 
    ret = ui_pb_api_ports_port_pp_port_add(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PORT_TM_PROFILE_GET_PORT_TM_PROFILE_GET_ID,1)) 
  { 
    ret = ui_pb_api_ports_port_pp_port_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PORT_TO_PP_PORT_MAP_SET_PORT_TO_PP_PORT_MAP_SET_ID,1)) 
  { 
    ret = ui_pb_api_ports_port_to_pp_port_map_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PORT_TO_PP_PORT_MAP_GET_PORT_TO_PP_PORT_MAP_GET_ID,1)) 
  { 
    ret = ui_pb_api_ports_port_to_pp_port_map_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PORT_FORWARDING_HEADER_SET_PORT_FORWARDING_HEADER_SET_ID,1)) 
  { 
    ret = ui_pb_api_ports_port_forwarding_header_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PORT_FORWARDING_HEADER_GET_PORT_FORWARDING_HEADER_GET_ID,1)) 
  { 
    ret = ui_pb_api_ports_port_forwarding_header_get(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after ports***", TRUE); 
  }
  
  ui_ret = ret; 
    
  goto exit; 
exit:        
  return ui_ret; 
}                
          
#endif /* ports */ 


#endif /* LINK_PB_LIBRARIES */ 

