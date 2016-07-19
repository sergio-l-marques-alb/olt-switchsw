/* $Id: ui_pb_api_flow_control.c,v 1.7 Broadcom SDK $
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
 
  
#include <soc/dpp/Petra/PB_TM/pb_api_flow_control.h>
#include <appl/dpp/UserInterface/pb/ui_pb_api_flow_control.h>

#if LINK_PB_LIBRARIES

#ifdef UI_FLOW_CONTROL
/******************************************************************** 
 *  Function handler: fc_gen_inbnd_set, Class Based Flow Control mode (section flow_control)
 ********************************************************************/
int 
  ui_pb_api_flow_control_fc_gen_inbnd_set_cb(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32
    prm_nif_ndx;
  SOC_PB_NIF_TYPE
    prm_nif_type;
  SOC_PETRA_INTERFACE_ID 
    prm_nif_id;
  SOC_PB_FC_GEN_INBND_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_flow_control"); 
  soc_sand_proc_name = "soc_pb_fc_gen_inbnd_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_FC_GEN_INBND_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_GEN_INBND_SET_FC_GEN_INBND_SET_NIF_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_FC_GEN_INBND_SET_FC_GEN_INBND_SET_NIF_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_nif_type = param_val->numeric_equivalent;
  }
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter type after on_off_set***", TRUE); 
    goto exit; 
  } 
   
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_GEN_INBND_SET_FC_GEN_INBND_SET_NIF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_GEN_INBND_SET_FC_GEN_INBND_SET_NIF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_nif_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter nif_ndx after fc_gen_inbnd_set***", TRUE); 
    goto exit; 
  } 
  
  /* Change format to SOC_PETRA_INTERFACE_ID */
  prm_nif_id = soc_pb_nif_type2id(prm_nif_type,prm_nif_ndx);

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_fc_gen_inbnd_get(
          unit,
          prm_nif_id,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_gen_inbnd_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_gen_inbnd_get");   
    goto exit; 
  } 

  prm_info.mode = SOC_PB_FC_INBND_MODE_CB;

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_GEN_INBND_SET_FC_GEN_INBND_SET_INFO_CB_NIF_CLS_BITMAP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_GEN_INBND_SET_FC_GEN_INBND_SET_INFO_CB_NIF_CLS_BITMAP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.cb.nif_cls_bitmap = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_GEN_INBND_SET_FC_GEN_INBND_SET_INFO_CB_CNM_INTERCEPT_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_GEN_INBND_SET_FC_GEN_INBND_SET_INFO_CB_CNM_INTERCEPT_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.cb.cnm_intercept_enable = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_GEN_INBND_SET_FC_GEN_INBND_SET_INFO_CB_GLBL_RCS_LOW_FC_CLS_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_GEN_INBND_SET_FC_GEN_INBND_SET_INFO_CB_GLBL_RCS_LOW_FC_CLS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.cb.glbl_rcs_low = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_GEN_INBND_SET_FC_GEN_INBND_SET_INFO_CB_INHERIT_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_FC_GEN_INBND_SET_FC_GEN_INBND_SET_INFO_CB_INHERIT_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.cb.inherit = param_val->numeric_equivalent;
  }  

  /* Call function */
  ret = soc_pb_fc_gen_inbnd_set(
          unit,
          prm_nif_id,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_gen_inbnd_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_gen_inbnd_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 

/******************************************************************** 
 *  Function handler: fc_gen_inbnd_set, Link-Level FC Mode (section flow_control)
 ********************************************************************/
int 
  ui_pb_api_flow_control_fc_gen_inbnd_set_ll(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32
    prm_nif_ndx;
  SOC_PB_NIF_TYPE
    prm_nif_type;
  SOC_PETRA_INTERFACE_ID 
    prm_nif_id;
  SOC_PB_FC_GEN_INBND_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_flow_control"); 
  soc_sand_proc_name = "soc_pb_fc_gen_inbnd_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_FC_GEN_INBND_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_GEN_INBND_SET_FC_GEN_INBND_SET_NIF_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_FC_GEN_INBND_SET_FC_GEN_INBND_SET_NIF_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_nif_type = param_val->numeric_equivalent;
  }
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter type after on_off_set***", TRUE); 
    goto exit; 
  } 
   
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_GEN_INBND_SET_FC_GEN_INBND_SET_NIF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_GEN_INBND_SET_FC_GEN_INBND_SET_NIF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_nif_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter nif_ndx after fc_gen_inbnd_set***", TRUE); 
    goto exit; 
  } 
  
  /* Change format to SOC_PETRA_INTERFACE_ID */
  prm_nif_id = soc_pb_nif_type2id(prm_nif_type,prm_nif_ndx);

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_fc_gen_inbnd_get(
          unit,
          prm_nif_id,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_gen_inbnd_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_gen_inbnd_get");   
    goto exit; 
  } 

  prm_info.mode = SOC_PB_FC_INBND_MODE_LL;

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_GEN_INBND_SET_FC_GEN_INBND_SET_INFO_LL_CNM_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_GEN_INBND_SET_FC_GEN_INBND_SET_INFO_LL_CNM_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.ll.cnm_enable = (uint8)param_val->value.ulong_value;
  }   

  /* Call function */
  ret = soc_pb_fc_gen_inbnd_set(
          unit,
          prm_nif_id,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_gen_inbnd_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_gen_inbnd_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: fc_gen_inbnd_get (section flow_control)
 ********************************************************************/
int 
  ui_pb_api_flow_control_fc_gen_inbnd_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32
    prm_nif_ndx;
  SOC_PB_NIF_TYPE
    prm_nif_type;
  SOC_PETRA_INTERFACE_ID 
    prm_nif_id;
  SOC_PB_FC_GEN_INBND_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_flow_control"); 
  soc_sand_proc_name = "soc_pb_fc_gen_inbnd_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_FC_GEN_INBND_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_GEN_INBND_GET_FC_GEN_INBND_GET_NIF_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_FC_GEN_INBND_GET_FC_GEN_INBND_GET_NIF_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_nif_type = param_val->numeric_equivalent;
  }
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter nif_ndx after fc_gen_inbnd_set***", TRUE); 
    goto exit; 
  } 
   
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_GEN_INBND_GET_FC_GEN_INBND_GET_NIF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_GEN_INBND_GET_FC_GEN_INBND_GET_NIF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_nif_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter nif_ndx after fc_gen_inbnd_get***", TRUE); 
    goto exit; 
  } 

  /* Change format to SOC_PETRA_INTERFACE_ID */
  prm_nif_id = soc_pb_nif_type2id(prm_nif_type,prm_nif_ndx);
  
  /* Call function */
  ret = soc_pb_fc_gen_inbnd_get(
          unit,
          prm_nif_id,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_gen_inbnd_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_gen_inbnd_get");   
    goto exit; 
  } 

  SOC_PB_FC_GEN_INBND_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: fc_gen_inbnd_glb_hp_set (section flow_control)
 ********************************************************************/
int 
  ui_pb_api_flow_control_fc_gen_inbnd_glb_hp_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_FC_INGR_GEN_GLB_HP_MODE   
    prm_fc_mode;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_flow_control"); 
  soc_sand_proc_name = "soc_pb_fc_gen_inbnd_glb_hp_set"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_fc_gen_inbnd_glb_hp_get(
          unit,
          &prm_fc_mode
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_gen_inbnd_glb_hp_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_gen_inbnd_glb_hp_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_GEN_INBND_GLB_HP_SET_FC_GEN_INBND_GLB_HP_SET_FC_MODE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_FC_GEN_INBND_GLB_HP_SET_FC_GEN_INBND_GLB_HP_SET_FC_MODE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fc_mode = param_val->numeric_equivalent;
  } 

  /* Call function */
  ret = soc_pb_fc_gen_inbnd_glb_hp_set(
          unit,
          prm_fc_mode
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_gen_inbnd_glb_hp_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_gen_inbnd_glb_hp_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: fc_gen_inbnd_glb_hp_get (section flow_control)
 ********************************************************************/
int 
  ui_pb_api_flow_control_fc_gen_inbnd_glb_hp_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_FC_INGR_GEN_GLB_HP_MODE   
    prm_fc_mode;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_flow_control"); 
  soc_sand_proc_name = "soc_pb_fc_gen_inbnd_glb_hp_get"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_pb_fc_gen_inbnd_glb_hp_get(
          unit,
          &prm_fc_mode
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_gen_inbnd_glb_hp_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_gen_inbnd_glb_hp_get");   
    goto exit; 
  } 

  soc_sand_os_printf("fc_mode: %s\n\r",SOC_PB_FC_INGR_GEN_GLB_HP_MODE_to_string(prm_fc_mode));

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: fc_gen_inbnd_cb_cnm_map_set (section flow_control)
 ********************************************************************/
int 
  ui_pb_api_flow_control_fc_gen_inbnd_cb_cnm_map_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_cpid_tc_ndx;
  uint8
    prm_ll_enable;
  uint32   
    prm_fc_class;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_flow_control"); 
  soc_sand_proc_name = "soc_pb_fc_gen_inbnd_cnm_map_set"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_GEN_INBND_CNM_MAP_SET_FC_GEN_INBND_CNM_MAP_SET_CPID_TC_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_GEN_INBND_CNM_MAP_SET_FC_GEN_INBND_CNM_MAP_SET_CPID_TC_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_cpid_tc_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter cpid_tc_ndx after fc_gen_inbnd_cb_cnm_map_set***", TRUE); 
    goto exit; 
  } 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_fc_gen_inbnd_cnm_map_get(
          unit,
          prm_cpid_tc_ndx,
          &prm_ll_enable,
          &prm_fc_class
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_gen_inbnd_cnm_map_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_gen_inbnd_cnm_map_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_GEN_INBND_CNM_MAP_SET_FC_GEN_INBND_CNM_MAP_SET_LL_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_GEN_INBND_CNM_MAP_SET_FC_GEN_INBND_CNM_MAP_SET_LL_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_ll_enable = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_GEN_INBND_CNM_MAP_SET_FC_GEN_INBND_CNM_MAP_SET_FC_CLASS_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_GEN_INBND_CNM_MAP_SET_FC_GEN_INBND_CNM_MAP_SET_FC_CLASS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fc_class = (uint32)param_val->value.ulong_value;
  } 

  /* Call function */
  ret = soc_pb_fc_gen_inbnd_cnm_map_set(
          unit,
          prm_cpid_tc_ndx,
          prm_ll_enable,
          prm_fc_class
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_gen_inbnd_cnm_map_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_gen_inbnd_cnm_map_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: fc_gen_inbnd_cb_cnm_map_get (section flow_control)
 ********************************************************************/
int 
  ui_pb_api_flow_control_fc_gen_inbnd_cb_cnm_map_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_cpid_tc_ndx;
  uint32   
    prm_fc_class;
  uint8
    prm_ll_enable;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_flow_control"); 
  soc_sand_proc_name = "soc_pb_fc_gen_inbnd_cnm_map_get"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_GEN_INBND_CNM_MAP_GET_FC_GEN_INBND_CNM_MAP_GET_CPID_TC_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_GEN_INBND_CNM_MAP_GET_FC_GEN_INBND_CNM_MAP_GET_CPID_TC_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_cpid_tc_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter cpid_tc_ndx after fc_gen_inbnd_cb_cnm_map_get***", TRUE); 
    goto exit; 
  } 

  /* Call function */
  ret = soc_pb_fc_gen_inbnd_cnm_map_get(
          unit,
          prm_cpid_tc_ndx,
          &prm_ll_enable,
          &prm_fc_class
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_gen_inbnd_cnm_map_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_gen_inbnd_cnm_map_get");   
    goto exit; 
  } 

  soc_sand_os_printf("fc_class: %d\n\r",prm_fc_class);
  soc_sand_os_printf("prm_ll_enable: %d\n\r",prm_ll_enable);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: fc_rec_inbnd_set (section flow_control)
 ********************************************************************/
int 
  ui_pb_api_flow_control_fc_rec_inbnd_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32
    prm_nif_ndx;
  SOC_PB_NIF_TYPE
    prm_nif_type;
  SOC_PETRA_INTERFACE_ID 
    prm_nif_id;
  SOC_PB_FC_REC_INBND_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_flow_control"); 
  soc_sand_proc_name = "soc_pb_fc_rec_inbnd_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_FC_REC_INBND_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_REC_INBND_SET_FC_REC_INBND_SET_NIF_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_FC_REC_INBND_SET_FC_REC_INBND_SET_NIF_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_nif_type = param_val->numeric_equivalent;
  }
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter type after on_off_set***", TRUE); 
    goto exit; 
  } 
   
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_REC_INBND_SET_FC_REC_INBND_SET_NIF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_REC_INBND_SET_FC_REC_INBND_SET_NIF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_nif_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter nif_ndx after fc_rec_inbnd_set***", TRUE); 
    goto exit; 
  } 
  
  /* Change format to SOC_PETRA_INTERFACE_ID */
  prm_nif_id = soc_pb_nif_type2id(prm_nif_type,prm_nif_ndx);

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_fc_rec_inbnd_get(
          unit,
          prm_nif_id,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_rec_inbnd_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_rec_inbnd_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_REC_INBND_SET_FC_REC_INBND_SET_INFO_CB_SCH_HR_BITMAP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_REC_INBND_SET_FC_REC_INBND_SET_INFO_CB_SCH_HR_BITMAP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.cb.sch_hr_bitmap = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_REC_INBND_SET_FC_REC_INBND_SET_INFO_CB_INHERIT_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_FC_REC_INBND_SET_FC_REC_INBND_SET_INFO_CB_INHERIT_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.cb.inherit = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_REC_INBND_SET_FC_REC_INBND_SET_INFO_MODE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_FC_REC_INBND_SET_FC_REC_INBND_SET_INFO_MODE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.mode = param_val->numeric_equivalent;
  } 

  /* Call function */
  ret = soc_pb_fc_rec_inbnd_set(
          unit,
          prm_nif_id,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_rec_inbnd_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_rec_inbnd_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: fc_rec_inbnd_get (section flow_control)
 ********************************************************************/
int 
  ui_pb_api_flow_control_fc_rec_inbnd_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32
    prm_nif_ndx;
  SOC_PB_NIF_TYPE
    prm_nif_type;
  SOC_PETRA_INTERFACE_ID 
    prm_nif_id;
  SOC_PB_FC_REC_INBND_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_flow_control"); 
  soc_sand_proc_name = "soc_pb_fc_rec_inbnd_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_FC_REC_INBND_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_REC_INBND_GET_FC_REC_INBND_GET_NIF_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_FC_REC_INBND_GET_FC_REC_INBND_GET_NIF_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_nif_type = param_val->numeric_equivalent;
  }
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter type after on_off_set***", TRUE); 
    goto exit; 
  } 
   
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_REC_INBND_GET_FC_REC_INBND_GET_NIF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_REC_INBND_GET_FC_REC_INBND_GET_NIF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_nif_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter nif_ndx after fc_rec_inbnd_get***", TRUE); 
    goto exit; 
  } 
  
  /* Change format to SOC_PETRA_INTERFACE_ID */
  prm_nif_id = soc_pb_nif_type2id(prm_nif_type,prm_nif_ndx);

  /* Call function */
  ret = soc_pb_fc_rec_inbnd_get(
          unit,
          prm_nif_id,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_rec_inbnd_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_rec_inbnd_get");   
    goto exit; 
  } 

  SOC_PB_FC_REC_INBND_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: fc_rec_inbnd_ofp_map_set (section flow_control)
 ********************************************************************/
int 
  ui_pb_api_flow_control_fc_rec_inbnd_ofp_map_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32
    prm_nif_ndx;
  SOC_PB_NIF_TYPE
    prm_nif_type;
  SOC_PETRA_INTERFACE_ID 
    prm_nif_id;
  uint32   
    prm_fc_cls_ndx;
  SOC_PB_FC_REC_OFP_MAP_INFO   
    prm_info; 
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_flow_control"); 
  soc_sand_proc_name = "soc_pb_fc_rec_inbnd_ofp_map_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_FC_REC_OFP_MAP_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_REC_INBND_OFP_MAP_SET_FC_REC_INBND_OFP_MAP_SET_NIF_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_FC_REC_INBND_OFP_MAP_SET_FC_REC_INBND_OFP_MAP_SET_NIF_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_nif_type = param_val->numeric_equivalent;
  }
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter type after on_off_set***", TRUE); 
    goto exit; 
  } 
   
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_REC_INBND_OFP_MAP_SET_FC_REC_INBND_OFP_MAP_SET_NIF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_REC_INBND_OFP_MAP_SET_FC_REC_INBND_OFP_MAP_SET_NIF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_nif_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter nif_ndx after fc_rec_inbnd_ofp_map_set***", TRUE); 
    goto exit; 
  } 
  
  /* Change format to SOC_PETRA_INTERFACE_ID */
  prm_nif_id = soc_pb_nif_type2id(prm_nif_type,prm_nif_ndx);

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_REC_INBND_OFP_MAP_SET_FC_REC_INBND_OFP_MAP_SET_FC_CLS_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_REC_INBND_OFP_MAP_SET_FC_REC_INBND_OFP_MAP_SET_FC_CLS_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_fc_cls_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter fc_cls_ndx after fc_rec_inbnd_ofp_map_set***", TRUE); 
    goto exit; 
  } 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_fc_rec_inbnd_ofp_map_get(
          unit,
          prm_nif_id,
          prm_fc_cls_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_rec_inbnd_ofp_map_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_rec_inbnd_ofp_map_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_REC_INBND_OFP_MAP_SET_FC_REC_INBND_OFP_MAP_SET_INFO_PRIO_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_FC_REC_INBND_OFP_MAP_SET_FC_REC_INBND_OFP_MAP_SET_INFO_PRIO_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.priority = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_REC_INBND_OFP_MAP_SET_FC_REC_INBND_OFP_MAP_SET_INFO_OFP_NDX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_REC_INBND_OFP_MAP_SET_FC_REC_INBND_OFP_MAP_SET_INFO_OFP_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.ofp_ndx = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_REC_INBND_OFP_MAP_SET_FC_REC_INBND_OFP_MAP_SET_INFO_REACT_POINT_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_FC_REC_INBND_OFP_MAP_SET_FC_REC_INBND_OFP_MAP_SET_INFO_REACT_POINT_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.react_point = param_val->numeric_equivalent;
  } 

  /* Call function */
  ret = soc_pb_fc_rec_inbnd_ofp_map_set(
          unit,
          prm_nif_id,
          prm_fc_cls_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_rec_inbnd_ofp_map_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_rec_inbnd_ofp_map_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: fc_rec_inbnd_ofp_map_get (section flow_control)
 ********************************************************************/
int 
  ui_pb_api_flow_control_fc_rec_inbnd_ofp_map_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32
    prm_nif_ndx;
  SOC_PB_NIF_TYPE
    prm_nif_type;
  SOC_PETRA_INTERFACE_ID 
    prm_nif_id;
  uint32   
    prm_fc_cls_ndx;
  SOC_PB_FC_REC_OFP_MAP_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_flow_control"); 
  soc_sand_proc_name = "soc_pb_fc_rec_inbnd_ofp_map_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_FC_REC_OFP_MAP_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_REC_INBND_OFP_MAP_GET_FC_REC_INBND_OFP_MAP_GET_NIF_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_FC_REC_INBND_OFP_MAP_GET_FC_REC_INBND_OFP_MAP_GET_NIF_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_nif_type = param_val->numeric_equivalent;
  }
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter type after on_off_set***", TRUE); 
    goto exit; 
  } 
   
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_REC_INBND_OFP_MAP_GET_FC_REC_INBND_OFP_MAP_GET_NIF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_REC_INBND_OFP_MAP_GET_FC_REC_INBND_OFP_MAP_GET_NIF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_nif_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter nif_ndx after fc_rec_inbnd_ofp_map_get***", TRUE); 
    goto exit; 
  } 
  
  /* Change format to SOC_PETRA_INTERFACE_ID */
  prm_nif_id = soc_pb_nif_type2id(prm_nif_type,prm_nif_ndx);

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_REC_INBND_OFP_MAP_GET_FC_REC_INBND_OFP_MAP_GET_FC_CLS_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_REC_INBND_OFP_MAP_GET_FC_REC_INBND_OFP_MAP_GET_FC_CLS_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_fc_cls_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter fc_cls_ndx after fc_rec_inbnd_ofp_map_get***", TRUE); 
    goto exit; 
  } 

  /* Call function */
  ret = soc_pb_fc_rec_inbnd_ofp_map_get(
          unit,
          prm_nif_id,
          prm_fc_cls_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_rec_inbnd_ofp_map_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_rec_inbnd_ofp_map_get");   
    goto exit; 
  } 

  SOC_PB_FC_REC_OFP_MAP_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: fc_rcy_ofp_set (section flow_control)
 ********************************************************************/
int 
  ui_pb_api_flow_control_fc_rcy_ofp_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_trgr_ndx;
  SOC_PB_FC_RCY_OFP_HANDLER   
    prm_react_point_ndx;
  SOC_PB_FC_OFP_PRIORITY   
    prm_prio_ndx;
  uint32 
    prm_ofp_bitmap_index = 0xFFFFFFFF;  
  SOC_PB_FC_RCY_PORT_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_flow_control"); 
  soc_sand_proc_name = "soc_pb_fc_rcy_ofp_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_FC_RCY_PORT_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_RCY_OFP_SET_FC_RCY_OFP_SET_TRGR_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_RCY_OFP_SET_FC_RCY_OFP_SET_TRGR_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_trgr_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter trgr_ndx after fc_rcy_ofp_set***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_RCY_OFP_SET_FC_RCY_OFP_SET_REACT_POINT_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_FC_RCY_OFP_SET_FC_RCY_OFP_SET_REACT_POINT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_react_point_ndx = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter react_point_ndx after fc_rcy_ofp_set***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_RCY_OFP_SET_FC_RCY_OFP_SET_PRIO_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_FC_RCY_OFP_SET_FC_RCY_OFP_SET_PRIO_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_prio_ndx = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter prio_ndx after fc_rcy_ofp_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_fc_rcy_ofp_get(
          unit,
          prm_trgr_ndx,
          prm_react_point_ndx,
          prm_prio_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_rcy_ofp_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_rcy_ofp_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_RCY_OFP_SET_FC_RCY_OFP_SET_INFO_OFP_BITMAP_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_RCY_OFP_SET_FC_RCY_OFP_SET_INFO_OFP_BITMAP_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_ofp_bitmap_index = (uint32)param_val->value.ulong_value;
  } 

  if(prm_ofp_bitmap_index != 0xFFFFFFFF)          
  { 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_RCY_OFP_SET_FC_RCY_OFP_SET_INFO_OFP_BITMAP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_RCY_OFP_SET_FC_RCY_OFP_SET_INFO_OFP_BITMAP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.ofp_bitmap[ prm_ofp_bitmap_index] = (uint32)param_val->value.ulong_value;
  } 

  }   


  /* Call function */
  ret = soc_pb_fc_rcy_ofp_set(
          unit,
          prm_trgr_ndx,
          prm_react_point_ndx,
          prm_prio_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_rcy_ofp_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_rcy_ofp_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: fc_rcy_ofp_get (section flow_control)
 ********************************************************************/
int 
  ui_pb_api_flow_control_fc_rcy_ofp_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_trgr_ndx;
  SOC_PB_FC_RCY_OFP_HANDLER   
    prm_react_point_ndx;
  SOC_PB_FC_OFP_PRIORITY   
    prm_prio_ndx;
  SOC_PB_FC_RCY_PORT_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_flow_control"); 
  soc_sand_proc_name = "soc_pb_fc_rcy_ofp_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_FC_RCY_PORT_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_RCY_OFP_GET_FC_RCY_OFP_GET_TRGR_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_RCY_OFP_GET_FC_RCY_OFP_GET_TRGR_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_trgr_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter trgr_ndx after fc_rcy_ofp_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_RCY_OFP_GET_FC_RCY_OFP_GET_REACT_POINT_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_FC_RCY_OFP_GET_FC_RCY_OFP_GET_REACT_POINT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_react_point_ndx = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter react_point_ndx after fc_rcy_ofp_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_RCY_OFP_GET_FC_RCY_OFP_GET_PRIO_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_FC_RCY_OFP_GET_FC_RCY_OFP_GET_PRIO_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_prio_ndx = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter prio_ndx after fc_rcy_ofp_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_fc_rcy_ofp_get(
          unit,
          prm_trgr_ndx,
          prm_react_point_ndx,
          prm_prio_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_rcy_ofp_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_rcy_ofp_get");   
    goto exit; 
  } 

  SOC_PB_FC_RCY_PORT_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: fc_rcy_hr_enable_set (section flow_control)
 ********************************************************************/
int 
  ui_pb_api_flow_control_fc_rcy_hr_enable_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint8   
    prm_enable;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_flow_control"); 
  soc_sand_proc_name = "soc_pb_fc_rcy_hr_enable_set"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_fc_rcy_hr_enable_get(
          unit,
          &prm_enable
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_rcy_hr_enable_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_rcy_hr_enable_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_RCY_HR_ENABLE_SET_FC_RCY_HR_ENABLE_SET_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_RCY_HR_ENABLE_SET_FC_RCY_HR_ENABLE_SET_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_enable = (uint8)param_val->value.ulong_value;
  } 

  /* Call function */
  ret = soc_pb_fc_rcy_hr_enable_set(
          unit,
          prm_enable
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_rcy_hr_enable_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_rcy_hr_enable_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: fc_rcy_hr_enable_get (section flow_control)
 ********************************************************************/
int 
  ui_pb_api_flow_control_fc_rcy_hr_enable_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint8   
    prm_enable;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_flow_control"); 
  soc_sand_proc_name = "soc_pb_fc_rcy_hr_enable_get"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_pb_fc_rcy_hr_enable_get(
          unit,
          &prm_enable
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_rcy_hr_enable_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_rcy_hr_enable_get");   
    goto exit; 
  } 

  soc_sand_os_printf("enable: %u\n\r",prm_enable);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: fc_gen_cal_set (section flow_control)
 ********************************************************************/
int 
  ui_pb_api_flow_control_fc_gen_cal_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_FC_CAL_MODE   
    prm_cal_mode_ndx;
  uint32   
    prm_if_ndx;
  SOC_PB_FC_CAL_IF_INFO   
    prm_cal_conf;
  SOC_PB_FC_GEN_CALENDAR   
    prm_cal_buff[UI_PB_FC_CAL_MAX_LEN];
  uint32
    prm_entry_ndx_min, prm_entry_ndx_max, entry_i, nof_entries;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_flow_control"); 
  soc_sand_proc_name = "soc_pb_fc_gen_cal_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_FC_CAL_IF_INFO_clear(&prm_cal_conf);
  for(entry_i = 0; entry_i < UI_PB_FC_CAL_MAX_LEN; entry_i++)
  {
    SOC_PB_FC_GEN_CALENDAR_clear(&(prm_cal_buff[entry_i]));
  }
  prm_cal_conf.cal_reps = 1;
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_GEN_CAL_SET_FC_GEN_CAL_SET_CAL_MODE_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_FC_GEN_CAL_SET_FC_GEN_CAL_SET_CAL_MODE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_cal_mode_ndx = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter cal_mode_ndx after fc_gen_cal_set***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_GEN_CAL_SET_FC_GEN_CAL_SET_IF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_GEN_CAL_SET_FC_GEN_CAL_SET_IF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_if_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter if_ndx after fc_gen_cal_set***", TRUE); 
    goto exit; 
  } 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_fc_gen_cal_get(
          unit,
          prm_cal_mode_ndx,
          prm_if_ndx,
          &prm_cal_conf,
          prm_cal_buff
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_gen_cal_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_gen_cal_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_GEN_CAL_SET_FC_GEN_CAL_SET_CAL_CONF_CAL_REPS_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_GEN_CAL_SET_FC_GEN_CAL_SET_CAL_CONF_CAL_REPS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_cal_conf.cal_reps = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_GEN_CAL_SET_FC_GEN_CAL_SET_CAL_CONF_CAL_LEN_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_GEN_CAL_SET_FC_GEN_CAL_SET_CAL_CONF_CAL_LEN_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_cal_conf.cal_len = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_GEN_CAL_SET_FC_GEN_CAL_SET_CAL_CONF_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_GEN_CAL_SET_FC_GEN_CAL_SET_CAL_CONF_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_cal_conf.enable = (uint8)param_val->value.ulong_value;
  } 
  if(prm_cal_conf.enable)
  {
    if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_GEN_CAL_ENTRY_ID,1))
    {
      UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_PB_FC_GEN_CAL_ENTRY_ID, 1);
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;
      UI_MACROS_LOAD_LONG_VAL(prm_entry_ndx_min);

      prm_entry_ndx_max = prm_entry_ndx_min;
      UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_PB_FC_GEN_CAL_ENTRY_ID, 2);

      if (ui_ret)
      {
        ui_ret = 0;
      }
      else
      {
        UI_MACROS_CHECK_GET_VAL_OF_ERROR;
        UI_MACROS_LOAD_LONG_VAL(prm_entry_ndx_max);
      }
    }
    else
    {
      send_string_to_screen(" *** SW error - expecting parameter entry_ndx after fc_gen_cal_set***", TRUE);
      goto exit;
    }

    nof_entries = (prm_entry_ndx_min==UI_PB_FC_CAL_MAX_LEN)?((uint32)UI_PB_FC_CAL_MAX_LEN):(prm_entry_ndx_max-prm_entry_ndx_min+1);
    for(entry_i = (prm_entry_ndx_min==UI_PB_FC_CAL_MAX_LEN)?0:prm_entry_ndx_min;
      entry_i < ((prm_entry_ndx_min==UI_PB_FC_CAL_MAX_LEN)?UI_PB_FC_CAL_MAX_LEN:(prm_entry_ndx_min+nof_entries));
      entry_i++)
    {
      if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_GEN_CAL_SET_FC_GEN_CAL_SET_CAL_BUFF_ID_ID,1)) 
      { 
        UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_GEN_CAL_SET_FC_GEN_CAL_SET_CAL_BUFF_ID_ID);  
        UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
        prm_cal_buff[entry_i].id = (uint32)param_val->value.ulong_value;
      } 

      if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_GEN_CAL_SET_FC_GEN_CAL_SET_CAL_BUFF_SOURCE_ID,1)) 
      { 
        UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_FC_GEN_CAL_SET_FC_GEN_CAL_SET_CAL_BUFF_SOURCE_ID); 
        UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
        prm_cal_buff[entry_i].source = param_val->numeric_equivalent;
      } 
    }
  }
  /* Call function */
  ret = soc_pb_fc_gen_cal_set(
          unit,
          prm_cal_mode_ndx,
          prm_if_ndx,
          &prm_cal_conf,
          prm_cal_buff
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_gen_cal_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_gen_cal_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: fc_gen_cal_get (section flow_control)
 ********************************************************************/
int 
  ui_pb_api_flow_control_fc_gen_cal_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_FC_CAL_MODE   
    prm_cal_mode_ndx;
  uint32   
    prm_if_ndx,
    entry_i;
  SOC_PB_FC_CAL_IF_INFO   
    prm_cal_conf;
  SOC_PB_FC_GEN_CALENDAR   
    prm_cal_buff[UI_PB_FC_CAL_MAX_LEN];
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_flow_control"); 
  soc_sand_proc_name = "soc_pb_fc_gen_cal_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_FC_CAL_IF_INFO_clear(&prm_cal_conf);
  for(entry_i = 0; entry_i < UI_PB_FC_CAL_MAX_LEN; entry_i++)
  {
    SOC_PB_FC_GEN_CALENDAR_clear(&(prm_cal_buff[entry_i]));
  }

  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_GEN_CAL_GET_FC_GEN_CAL_GET_CAL_MODE_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_FC_GEN_CAL_GET_FC_GEN_CAL_GET_CAL_MODE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_cal_mode_ndx = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter cal_mode_ndx after fc_gen_cal_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_GEN_CAL_GET_FC_GEN_CAL_GET_IF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_GEN_CAL_GET_FC_GEN_CAL_GET_IF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_if_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter if_ndx after fc_gen_cal_get***", TRUE); 
    goto exit; 
  } 

  /* Call function */
  ret = soc_pb_fc_gen_cal_get(
          unit,
          prm_cal_mode_ndx,
          prm_if_ndx,
          &prm_cal_conf,
          prm_cal_buff
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_gen_cal_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_gen_cal_get");   
    goto exit; 
  } 

  SOC_PB_FC_CAL_IF_INFO_print(&prm_cal_conf);

  for(entry_i = 0; entry_i < prm_cal_conf.cal_len; entry_i++)
  {
    soc_sand_os_printf("%u: ", entry_i);
    SOC_PB_FC_GEN_CALENDAR_print(&(prm_cal_buff[entry_i]));
  }
    
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: fc_rec_cal_set (section flow_control)
 ********************************************************************/
int 
  ui_pb_api_flow_control_fc_rec_cal_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret,
    i;   
  SOC_PB_FC_CAL_MODE   
    prm_cal_mode_ndx;
  uint32   
    prm_if_ndx,
	prm_entry_ndx_min, prm_entry_ndx_max, entry_i, nof_entries;
  SOC_PB_FC_CAL_IF_INFO   
    prm_cal_conf;
  SOC_PB_FC_REC_CALENDAR   
    prm_cal_buff[512];
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_flow_control"); 
  soc_sand_proc_name = "soc_pb_fc_rec_cal_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_FC_CAL_IF_INFO_clear(&prm_cal_conf);
  
  for (i = 0; i < sizeof(prm_cal_buff) / sizeof(prm_cal_buff[0]); ++i)
  {
    SOC_PB_FC_REC_CALENDAR_clear(&prm_cal_buff[i]);
  }
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_REC_CAL_SET_FC_REC_CAL_SET_CAL_MODE_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_FC_REC_CAL_SET_FC_REC_CAL_SET_CAL_MODE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_cal_mode_ndx = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter cal_mode_ndx after fc_rec_cal_set***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_REC_CAL_SET_FC_REC_CAL_SET_IF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_REC_CAL_SET_FC_REC_CAL_SET_IF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_if_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter if_ndx after fc_rec_cal_set***", TRUE); 
    goto exit; 
  } 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_fc_rec_cal_get(
          unit,
          prm_cal_mode_ndx,
          prm_if_ndx,
          &prm_cal_conf,
          prm_cal_buff
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_rec_cal_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_rec_cal_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_REC_CAL_SET_FC_REC_CAL_SET_CAL_CONF_CAL_REPS_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_REC_CAL_SET_FC_REC_CAL_SET_CAL_CONF_CAL_REPS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_cal_conf.cal_reps = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_REC_CAL_SET_FC_REC_CAL_SET_CAL_CONF_CAL_LEN_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_REC_CAL_SET_FC_REC_CAL_SET_CAL_CONF_CAL_LEN_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_cal_conf.cal_len = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_REC_CAL_SET_FC_REC_CAL_SET_CAL_CONF_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_REC_CAL_SET_FC_REC_CAL_SET_CAL_CONF_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_cal_conf.enable = (uint8)param_val->value.ulong_value;
  } 

  if(prm_cal_conf.enable)
  {
	  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_REC_CAL_ENTRY_ID,1))
	  {
		  UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_PB_FC_REC_CAL_ENTRY_ID, 1);
		  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
		  UI_MACROS_LOAD_LONG_VAL(prm_entry_ndx_min);

		  prm_entry_ndx_max = prm_entry_ndx_min;
		  UI_MACROS_GET_NUM_SYM_VAL_WITH_INDEX(PARAM_PB_FC_REC_CAL_ENTRY_ID, 2);

		  if (ui_ret)
		  {
			  ui_ret = 0;
		  }
		  else
		  {
			  UI_MACROS_CHECK_GET_VAL_OF_ERROR;
			  UI_MACROS_LOAD_LONG_VAL(prm_entry_ndx_max);
		  }
	  }
	  else
	  {
		  send_string_to_screen(" *** SW error - expecting parameter entry_ndx after fc_gen_cal_set***", TRUE);
		  goto exit;
	  }

	  nof_entries = (prm_entry_ndx_min==UI_PB_FC_CAL_MAX_LEN)?((uint32)UI_PB_FC_CAL_MAX_LEN):(prm_entry_ndx_max-prm_entry_ndx_min+1);
	  for(entry_i = (prm_entry_ndx_min==UI_PB_FC_CAL_MAX_LEN)?0:prm_entry_ndx_min;
		  entry_i < ((prm_entry_ndx_min==UI_PB_FC_CAL_MAX_LEN)?UI_PB_FC_CAL_MAX_LEN:(prm_entry_ndx_min+nof_entries));
		  entry_i++)
	  {
		  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_REC_CAL_SET_FC_REC_CAL_SET_CAL_BUFF_ID_ID,1)) 
		  { 
			  UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_REC_CAL_SET_FC_REC_CAL_SET_CAL_BUFF_ID_ID);  
			  UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
			  prm_cal_buff[entry_i].id = (uint32)param_val->value.ulong_value;
		  } 

		  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_REC_CAL_SET_FC_REC_CAL_SET_CAL_BUFF_DESTINATION_ID,1)) 
		  { 
			  UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_FC_REC_CAL_SET_FC_REC_CAL_SET_CAL_BUFF_DESTINATION_ID); 
			  UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
			  prm_cal_buff[entry_i].destination = param_val->numeric_equivalent;
		  } 
	  }
  }

  /* Call function */
  ret = soc_pb_fc_rec_cal_set(
          unit,
          prm_cal_mode_ndx,
          prm_if_ndx,
          &prm_cal_conf,
          prm_cal_buff
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_rec_cal_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_rec_cal_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: fc_rec_cal_get (section flow_control)
 ********************************************************************/
int 
  ui_pb_api_flow_control_fc_rec_cal_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret,
	entry_i;   
  SOC_PB_FC_CAL_MODE   
    prm_cal_mode_ndx;
  uint32   
    prm_if_ndx;
  SOC_PB_FC_CAL_IF_INFO   
    prm_cal_conf;
  SOC_PB_FC_REC_CALENDAR   
    prm_cal_buff[UI_PB_FC_CAL_MAX_LEN];
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_flow_control"); 
  soc_sand_proc_name = "soc_pb_fc_rec_cal_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_FC_CAL_IF_INFO_clear(&prm_cal_conf);
  for(entry_i = 0; entry_i < UI_PB_FC_CAL_MAX_LEN; entry_i++)
  {
	  SOC_PB_FC_REC_CALENDAR_clear(&(prm_cal_buff[entry_i]));
  }
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_REC_CAL_GET_FC_REC_CAL_GET_CAL_MODE_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_FC_REC_CAL_GET_FC_REC_CAL_GET_CAL_MODE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_cal_mode_ndx = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter cal_mode_ndx after fc_rec_cal_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_REC_CAL_GET_FC_REC_CAL_GET_IF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_REC_CAL_GET_FC_REC_CAL_GET_IF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_if_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter if_ndx after fc_rec_cal_get***", TRUE); 
    goto exit; 
  } 

  /* Call function */
  ret = soc_pb_fc_rec_cal_get(
          unit,
          prm_cal_mode_ndx,
          prm_if_ndx,
          &prm_cal_conf,
          prm_cal_buff
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_rec_cal_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_rec_cal_get");   
    goto exit; 
  } 

  SOC_PB_FC_CAL_IF_INFO_print(&prm_cal_conf);

  for(entry_i = 0; entry_i < prm_cal_conf.cal_len; entry_i++)
  {
	  soc_sand_os_printf("%u: ", entry_i);
	  SOC_PB_FC_REC_CALENDAR_print(&(prm_cal_buff[entry_i]));
  }


  
  goto exit; 
exit: 
  return ui_ret; 
} 

/******************************************************************** 
 *  Function handler: fc_oob_phy_params_set (section flow_control)
 ********************************************************************/
int 
  ui_pb_api_flow_control_fc_oob_phy_params_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;
  uint32   
    prm_if_ndx;
  SOC_PB_FC_PHY_PARAMS_INFO   
    prm_phy_params_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_flow_control"); 
  soc_sand_proc_name = "soc_pb_fc_oob_phy_params_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_FC_PHY_PARAMS_INFO_clear(&prm_phy_params_info);
  
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_OOB_PHY_PARAMS_SET_FC_OOB_PHY_PARAMS_SET_IF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_OOB_PHY_PARAMS_SET_FC_OOB_PHY_PARAMS_SET_IF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_if_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter if_ndx after fc_oob_phy_params_set***", TRUE); 
    goto exit; 
  } 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_fc_oob_phy_params_get(
          unit,
          prm_if_ndx,
          &prm_phy_params_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_oob_phy_params_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_oob_phy_params_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_OOB_PHY_PARAMS_SET_FC_OOB_PHY_PARAMS_SET_IS_ON_IF_OOF_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_OOB_PHY_PARAMS_SET_FC_OOB_PHY_PARAMS_SET_IS_ON_IF_OOF_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_phy_params_info.is_on_if_oof = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_OOB_PHY_PARAMS_SET_FC_OOB_PHY_PARAMS_SET_IS_SAMPLED_RISING_EDGE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_OOB_PHY_PARAMS_SET_FC_OOB_PHY_PARAMS_SET_IS_SAMPLED_RISING_EDGE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_phy_params_info.is_sampled_rising_edge = (uint8)param_val->value.ulong_value;
  } 

  /* Call function */
  ret = soc_pb_fc_oob_phy_params_set(
          unit,
          prm_if_ndx,
          &prm_phy_params_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_oob_phy_params_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_oob_phy_params_set");   
    goto exit; 
  } 
  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: fc_oob_phy_params_get (section flow_control)
 ********************************************************************/
int 
  ui_pb_api_flow_control_fc_oob_phy_params_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_if_ndx;
  SOC_PB_FC_PHY_PARAMS_INFO   
    prm_phy_params_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_flow_control"); 
  soc_sand_proc_name = "soc_pb_fc_oob_phy_params_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_FC_PHY_PARAMS_INFO_clear(&prm_phy_params_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_OOB_PHY_PARAMS_GET_FC_OOB_PHY_PARAMS_GET_IF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_OOB_PHY_PARAMS_GET_FC_OOB_PHY_PARAMS_GET_IF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_if_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter if_ndx after fc_oob_phy_params_get***", TRUE); 
    goto exit; 
  } 

  /* Call function */
  ret = soc_pb_fc_oob_phy_params_get(
          unit,
          prm_if_ndx,
          &prm_phy_params_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_oob_phy_params_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_oob_phy_params_get");   
    goto exit; 
  } 

  SOC_PB_FC_PHY_PARAMS_INFO_print(&prm_phy_params_info);
  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: fc_egr_rec_oob_stat_get (section flow_control)
 ********************************************************************/
int 
  ui_pb_api_flow_control_fc_egr_rec_oob_stat_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_FC_OOB_ID   
    prm_oob_ndx;
  SOC_PB_FC_CAL_REC_STAT   
    prm_status;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_flow_control"); 
  soc_sand_proc_name = "soc_pb_fc_egr_rec_oob_stat_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_FC_CAL_REC_STAT_clear(&prm_status);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_EGR_REC_OOB_STAT_GET_FC_EGR_REC_OOB_STAT_GET_OOB_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_FC_EGR_REC_OOB_STAT_GET_FC_EGR_REC_OOB_STAT_GET_OOB_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_oob_ndx = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter oob_ndx after fc_egr_rec_oob_stat_get***", TRUE); 
    goto exit; 
  } 

  /* Call function */
  ret = soc_pb_fc_egr_rec_oob_stat_get(
          unit,
          prm_oob_ndx,
          &prm_status
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_egr_rec_oob_stat_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_egr_rec_oob_stat_get");   
    goto exit; 
  } 

  SOC_PB_FC_CAL_REC_STAT_print(&prm_status);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: fc_ilkn_llfc_set (section flow_control)
 ********************************************************************/
int 
  ui_pb_api_flow_control_fc_ilkn_llfc_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_NIF_ILKN_ID   
    prm_ilkn_ndx;
  SOC_PB_FC_ILKN_LLFC_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_flow_control"); 
  soc_sand_proc_name = "soc_pb_fc_ilkn_llfc_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_FC_ILKN_LLFC_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_ILKN_LLFC_SET_FC_ILKN_LLFC_SET_ILKN_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_FC_ILKN_LLFC_SET_FC_ILKN_LLFC_SET_ILKN_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_ilkn_ndx = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter ilkn_ndx after fc_ilkn_llfc_set***", TRUE); 
    goto exit; 
  } 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_fc_ilkn_llfc_get(
          unit,
          prm_ilkn_ndx,
          &prm_info,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_ilkn_llfc_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_ilkn_llfc_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_ILKN_LLFC_SET_FC_ILKN_LLFC_SET_INFO_CAL_CHANNEL_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_FC_ILKN_LLFC_SET_FC_ILKN_LLFC_SET_INFO_CAL_CHANNEL_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.cal_channel = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_ILKN_LLFC_SET_FC_ILKN_LLFC_SET_INFO_MULTI_USE_MASK_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_ILKN_LLFC_SET_FC_ILKN_LLFC_SET_INFO_MULTI_USE_MASK_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.multi_use_mask = (uint8)param_val->value.ulong_value;
  } 

  /* Call function */
  ret = soc_pb_fc_ilkn_llfc_set(
          unit,
          prm_ilkn_ndx,
          SOC_PETRA_FC_DIRECTION_REC,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_ilkn_llfc_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_ilkn_llfc_set");   
    goto exit; 
  } 

  /* Call function */
  ret = soc_pb_fc_ilkn_llfc_set(
          unit,
          prm_ilkn_ndx,
          SOC_PETRA_FC_DIRECTION_GEN,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_ilkn_llfc_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_ilkn_llfc_set");   
    goto exit; 
  } 

  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: fc_ilkn_llfc_get (section flow_control)
 ********************************************************************/
int 
  ui_pb_api_flow_control_fc_ilkn_llfc_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_NIF_ILKN_ID   
    prm_ilkn_ndx;
  SOC_PB_FC_ILKN_LLFC_INFO   
    prm_rec_info,
    prm_gen_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_flow_control"); 
  soc_sand_proc_name = "soc_pb_fc_ilkn_llfc_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_FC_ILKN_LLFC_INFO_clear(&prm_rec_info);
  SOC_PB_FC_ILKN_LLFC_INFO_clear(&prm_gen_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_ILKN_LLFC_GET_FC_ILKN_LLFC_GET_ILKN_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_FC_ILKN_LLFC_GET_FC_ILKN_LLFC_GET_ILKN_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_ilkn_ndx = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter ilkn_ndx after fc_ilkn_llfc_get***", TRUE); 
    goto exit; 
  } 

  /* Call function */
  ret = soc_pb_fc_ilkn_llfc_get(
          unit,
          prm_ilkn_ndx,
          &prm_rec_info,
          &prm_gen_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_ilkn_llfc_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_ilkn_llfc_get");   
    goto exit; 
  } 

  send_string_to_screen("Receive LLFC:", TRUE);
  SOC_PB_FC_ILKN_LLFC_INFO_print(&prm_rec_info);
  send_string_to_screen("\r\nGenerate LLFC:", TRUE);
  SOC_PB_FC_ILKN_LLFC_INFO_print(&prm_gen_info);
  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: fc_nif_oversubscr_scheme_set (section flow_control)
 ********************************************************************/
int 
  ui_pb_api_flow_control_fc_nif_oversubscr_scheme_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_FC_NIF_OVERSUBSCR_SCHEME   
    prm_scheme;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_flow_control"); 
  soc_sand_proc_name = "soc_pb_fc_nif_oversubscr_scheme_set"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_fc_nif_oversubscr_scheme_get(
          unit,
          &prm_scheme
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_nif_oversubscr_scheme_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_nif_oversubscr_scheme_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_NIF_OVERSUBSCR_SCHEME_SET_FC_NIF_OVERSUBSCR_SCHEME_SET_SCHEME_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_FC_NIF_OVERSUBSCR_SCHEME_SET_FC_NIF_OVERSUBSCR_SCHEME_SET_SCHEME_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_scheme = param_val->numeric_equivalent;
  } 

  /* Call function */
  ret = soc_pb_fc_nif_oversubscr_scheme_set(
          unit,
          prm_scheme
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_nif_oversubscr_scheme_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_nif_oversubscr_scheme_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: fc_nif_oversubscr_scheme_get (section flow_control)
 ********************************************************************/
int 
  ui_pb_api_flow_control_fc_nif_oversubscr_scheme_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_FC_NIF_OVERSUBSCR_SCHEME   
    prm_scheme;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_flow_control"); 
  soc_sand_proc_name = "soc_pb_fc_nif_oversubscr_scheme_get"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_pb_fc_nif_oversubscr_scheme_get(
          unit,
          &prm_scheme
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_nif_oversubscr_scheme_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_nif_oversubscr_scheme_get");   
    goto exit; 
  } 

  soc_sand_os_printf("scheme: %s\n\r",SOC_PB_FC_NIF_OVERSUBSCR_SCHEME_to_string(prm_scheme));

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: fc_nif_pause_quanta_set (section flow_control)
 ********************************************************************/
int 
  ui_pb_api_flow_control_fc_nif_pause_quanta_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_mal_ndx;
  uint32   
    prm_pause_quanta;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_flow_control"); 
  soc_sand_proc_name = "soc_pb_fc_nif_pause_quanta_set"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_NIF_PAUSE_QUANTA_SET_FC_NIF_PAUSE_QUANTA_SET_MAL_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_NIF_PAUSE_QUANTA_SET_FC_NIF_PAUSE_QUANTA_SET_MAL_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_mal_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter mal_ndx after fc_nif_pause_quanta_set***", TRUE); 
    goto exit; 
  } 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_fc_nif_pause_quanta_get(
          unit,
          prm_mal_ndx,
          &prm_pause_quanta
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_nif_pause_quanta_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_nif_pause_quanta_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_NIF_PAUSE_QUANTA_SET_FC_NIF_PAUSE_QUANTA_SET_PAUSE_QUANTA_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_NIF_PAUSE_QUANTA_SET_FC_NIF_PAUSE_QUANTA_SET_PAUSE_QUANTA_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_pause_quanta = (uint32)param_val->value.ulong_value;
  } 

  /* Call function */
  ret = soc_pb_fc_nif_pause_quanta_set(
          unit,
          prm_mal_ndx,
          prm_pause_quanta
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_nif_pause_quanta_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_nif_pause_quanta_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: fc_nif_pause_quanta_get (section flow_control)
 ********************************************************************/
int 
  ui_pb_api_flow_control_fc_nif_pause_quanta_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_mal_ndx;
  uint32   
    prm_pause_quanta;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_flow_control"); 
  soc_sand_proc_name = "soc_pb_fc_nif_pause_quanta_get"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_NIF_PAUSE_QUANTA_GET_FC_NIF_PAUSE_QUANTA_GET_MAL_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_NIF_PAUSE_QUANTA_GET_FC_NIF_PAUSE_QUANTA_GET_MAL_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_mal_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter mal_ndx after fc_nif_pause_quanta_get***", TRUE); 
    goto exit; 
  } 

  /* Call function */
  ret = soc_pb_fc_nif_pause_quanta_get(
          unit,
          prm_mal_ndx,
          &prm_pause_quanta
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_nif_pause_quanta_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_nif_pause_quanta_get");   
    goto exit; 
  } 

  soc_sand_os_printf("pause_quanta: %u\n\r",prm_pause_quanta);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: fc_nif_pause_frame_src_addr_set (section flow_control)
 ********************************************************************/
int 
  ui_pb_api_flow_control_fc_nif_pause_frame_src_addr_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_mal_ndx;
  SOC_SAND_PP_MAC_ADDRESS   
    prm_mac_addr;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_flow_control"); 
  soc_sand_proc_name = "soc_pb_fc_nif_pause_frame_src_addr_set"; 
 
  unit = soc_pb_get_default_unit(); 
  soc_sand_SAND_PP_MAC_ADDRESS_clear(&prm_mac_addr);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_NIF_PAUSE_FRAME_SRC_ADDR_SET_FC_NIF_PAUSE_FRAME_SRC_ADDR_SET_MAL_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_NIF_PAUSE_FRAME_SRC_ADDR_SET_FC_NIF_PAUSE_FRAME_SRC_ADDR_SET_MAL_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_mal_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter mal_ndx after fc_nif_pause_frame_src_addr_set***", TRUE); 
    goto exit; 
  } 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_fc_nif_pause_frame_src_addr_get(
          unit,
          prm_mal_ndx,
          &prm_mac_addr
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_nif_pause_frame_src_addr_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_nif_pause_frame_src_addr_get");   
    goto exit; 
  } 

  if (!get_val_of(
         current_line,(int *)&match_index,PARAM_PB_FC_NIF_PAUSE_FRAME_SRC_ADDR_SET_FC_NIF_PAUSE_FRAME_SRC_ADDR_SET_MAC_ADDR_ID,1,  
         &param_val,VAL_TEXT,err_msg)) 
  { 
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_mac_addr));
  } 

  /* Call function */
  ret = soc_pb_fc_nif_pause_frame_src_addr_set(
          unit,
          prm_mal_ndx,
          &prm_mac_addr
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_nif_pause_frame_src_addr_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_nif_pause_frame_src_addr_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: fc_nif_pause_frame_src_addr_get (section flow_control)
 ********************************************************************/
int 
  ui_pb_api_flow_control_fc_nif_pause_frame_src_addr_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_mal_ndx;
  SOC_SAND_PP_MAC_ADDRESS   
    prm_mac_addr;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_flow_control"); 
  soc_sand_proc_name = "soc_pb_fc_nif_pause_frame_src_addr_get"; 
 
  unit = soc_pb_get_default_unit(); 
  soc_sand_SAND_PP_MAC_ADDRESS_clear(&prm_mac_addr);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_NIF_PAUSE_FRAME_SRC_ADDR_GET_FC_NIF_PAUSE_FRAME_SRC_ADDR_GET_MAL_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_NIF_PAUSE_FRAME_SRC_ADDR_GET_FC_NIF_PAUSE_FRAME_SRC_ADDR_GET_MAL_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_mal_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter mal_ndx after fc_nif_pause_frame_src_addr_get***", TRUE); 
    goto exit; 
  } 

  /* Call function */
  ret = soc_pb_fc_nif_pause_frame_src_addr_get(
          unit,
          prm_mal_ndx,
          &prm_mac_addr
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_nif_pause_frame_src_addr_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_nif_pause_frame_src_addr_get");   
    goto exit; 
  } 

  soc_sand_SAND_PP_MAC_ADDRESS_print(&prm_mac_addr);

  
  goto exit; 
exit: 
  return ui_ret; 
}  
 
/******************************************************************** 
 *  Function handler: fc_vsq_by_incoming_nif_set (section flow_control)
 ********************************************************************/
int 
  ui_pb_api_flow_control_fc_vsq_by_incoming_nif_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint8   
    prm_enable;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_flow_control"); 
  soc_sand_proc_name = "soc_pb_fc_vsq_by_incoming_nif_set"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_fc_vsq_by_incoming_nif_get(
          unit,
          &prm_enable
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_vsq_by_incoming_nif_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_vsq_by_incoming_nif_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_VSQ_BY_INCOMING_NIF_SET_FC_VSQ_BY_INCOMING_NIF_SET_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_FC_VSQ_BY_INCOMING_NIF_SET_FC_VSQ_BY_INCOMING_NIF_SET_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_enable = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_fc_vsq_by_incoming_nif_set(
          unit,
          prm_enable
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_vsq_by_incoming_nif_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_vsq_by_incoming_nif_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: fc_vsq_by_incoming_nif_get (section flow_control)
 ********************************************************************/
int 
  ui_pb_api_flow_control_fc_vsq_by_incoming_nif_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint8   
    prm_enable;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_flow_control"); 
  soc_sand_proc_name = "soc_pb_fc_vsq_by_incoming_nif_get"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_pb_fc_vsq_by_incoming_nif_get(
          unit,
          &prm_enable
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_fc_vsq_by_incoming_nif_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_fc_vsq_by_incoming_nif_get");   
    goto exit; 
  } 

  soc_sand_os_printf("enable: %u\n\r",prm_enable);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
#endif
#ifdef UI_FLOW_CONTROL/* { flow_control*/
/******************************************************************** 
 *  Section handler: flow_control
 ********************************************************************/ 
int 
  ui_pb_api_flow_control( 
    CURRENT_LINE *current_line 
  ) 
{   
  int
    ret = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_pb_api_flow_control"); 
 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_GEN_INBND_SET_FC_GEN_INBND_SET_CB_ID,1)) 
  { 
    ret = ui_pb_api_flow_control_fc_gen_inbnd_set_cb(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_GEN_INBND_SET_FC_GEN_INBND_SET_LL_ID,1)) 
  { 
    ret = ui_pb_api_flow_control_fc_gen_inbnd_set_ll(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_GEN_INBND_GET_FC_GEN_INBND_GET_ID,1)) 
  { 
    ret = ui_pb_api_flow_control_fc_gen_inbnd_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_GEN_INBND_GLB_HP_SET_FC_GEN_INBND_GLB_HP_SET_ID,1)) 
  { 
    ret = ui_pb_api_flow_control_fc_gen_inbnd_glb_hp_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_GEN_INBND_GLB_HP_GET_FC_GEN_INBND_GLB_HP_GET_ID,1)) 
  { 
    ret = ui_pb_api_flow_control_fc_gen_inbnd_glb_hp_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_GEN_INBND_CNM_MAP_SET_FC_GEN_INBND_CNM_MAP_SET_ID,1)) 
  { 
    ret = ui_pb_api_flow_control_fc_gen_inbnd_cb_cnm_map_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_GEN_INBND_CNM_MAP_GET_FC_GEN_INBND_CNM_MAP_GET_ID,1)) 
  { 
    ret = ui_pb_api_flow_control_fc_gen_inbnd_cb_cnm_map_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_REC_INBND_SET_FC_REC_INBND_SET_ID,1)) 
  { 
    ret = ui_pb_api_flow_control_fc_rec_inbnd_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_REC_INBND_GET_FC_REC_INBND_GET_ID,1)) 
  { 
    ret = ui_pb_api_flow_control_fc_rec_inbnd_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_REC_INBND_OFP_MAP_SET_FC_REC_INBND_OFP_MAP_SET_ID,1)) 
  { 
    ret = ui_pb_api_flow_control_fc_rec_inbnd_ofp_map_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_REC_INBND_OFP_MAP_GET_FC_REC_INBND_OFP_MAP_GET_ID,1)) 
  { 
    ret = ui_pb_api_flow_control_fc_rec_inbnd_ofp_map_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_RCY_OFP_SET_FC_RCY_OFP_SET_ID,1)) 
  { 
    ret = ui_pb_api_flow_control_fc_rcy_ofp_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_RCY_OFP_GET_FC_RCY_OFP_GET_ID,1)) 
  { 
    ret = ui_pb_api_flow_control_fc_rcy_ofp_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_RCY_HR_ENABLE_SET_FC_RCY_HR_ENABLE_SET_ID,1)) 
  { 
    ret = ui_pb_api_flow_control_fc_rcy_hr_enable_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_RCY_HR_ENABLE_GET_FC_RCY_HR_ENABLE_GET_ID,1)) 
  { 
    ret = ui_pb_api_flow_control_fc_rcy_hr_enable_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_GEN_CAL_SET_FC_GEN_CAL_SET_ID,1)) 
  { 
    ret = ui_pb_api_flow_control_fc_gen_cal_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_GEN_CAL_GET_FC_GEN_CAL_GET_ID,1)) 
  { 
    ret = ui_pb_api_flow_control_fc_gen_cal_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_REC_CAL_SET_FC_REC_CAL_SET_ID,1)) 
  { 
    ret = ui_pb_api_flow_control_fc_rec_cal_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_REC_CAL_GET_FC_REC_CAL_GET_ID,1)) 
  { 
    ret = ui_pb_api_flow_control_fc_rec_cal_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_OOB_PHY_PARAMS_SET_FC_OOB_PHY_PARAMS_SET_ID,1)) 
  { 
    ret = ui_pb_api_flow_control_fc_oob_phy_params_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_OOB_PHY_PARAMS_GET_FC_OOB_PHY_PARAMS_GET_ID,1)) 
  { 
    ret = ui_pb_api_flow_control_fc_oob_phy_params_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_EGR_REC_OOB_STAT_GET_FC_EGR_REC_OOB_STAT_GET_ID,1)) 
  { 
    ret = ui_pb_api_flow_control_fc_egr_rec_oob_stat_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_ILKN_LLFC_SET_FC_ILKN_LLFC_SET_ID,1)) 
  { 
    ret = ui_pb_api_flow_control_fc_ilkn_llfc_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_ILKN_LLFC_GET_FC_ILKN_LLFC_GET_ID,1)) 
  { 
    ret = ui_pb_api_flow_control_fc_ilkn_llfc_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_NIF_OVERSUBSCR_SCHEME_SET_FC_NIF_OVERSUBSCR_SCHEME_SET_ID,1)) 
  { 
    ret = ui_pb_api_flow_control_fc_nif_oversubscr_scheme_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_NIF_OVERSUBSCR_SCHEME_GET_FC_NIF_OVERSUBSCR_SCHEME_GET_ID,1)) 
  { 
    ret = ui_pb_api_flow_control_fc_nif_oversubscr_scheme_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_NIF_PAUSE_QUANTA_SET_FC_NIF_PAUSE_QUANTA_SET_ID,1)) 
  { 
    ret = ui_pb_api_flow_control_fc_nif_pause_quanta_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_NIF_PAUSE_QUANTA_GET_FC_NIF_PAUSE_QUANTA_GET_ID,1)) 
  { 
    ret = ui_pb_api_flow_control_fc_nif_pause_quanta_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_NIF_PAUSE_FRAME_SRC_ADDR_SET_FC_NIF_PAUSE_FRAME_SRC_ADDR_SET_ID,1)) 
  { 
    ret = ui_pb_api_flow_control_fc_nif_pause_frame_src_addr_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_NIF_PAUSE_FRAME_SRC_ADDR_GET_FC_NIF_PAUSE_FRAME_SRC_ADDR_GET_ID,1)) 
  { 
    ret = ui_pb_api_flow_control_fc_nif_pause_frame_src_addr_get(current_line); 
  }   
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_VSQ_BY_INCOMING_NIF_SET_FC_VSQ_BY_INCOMING_NIF_SET_ID,1)) 
  { 
    ret = ui_pb_api_flow_control_fc_vsq_by_incoming_nif_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_FC_VSQ_BY_INCOMING_NIF_GET_FC_VSQ_BY_INCOMING_NIF_GET_ID,1)) 
  { 
    ret = ui_pb_api_flow_control_fc_vsq_by_incoming_nif_get(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after flow_control***", TRUE); 
  }
  
  ui_ret = ret; 
    
  goto exit; 
exit:        
  return ui_ret; 
}                
          
#endif /* flow_control */ 

#endif /* LINK_PB_LIBRARIES */ 

