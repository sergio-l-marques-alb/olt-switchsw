/* $Id: ui_pb_api_pmf_low_level_diag.c,v 1.7 Broadcom SDK $
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
 
  
#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level_diag.h>
#include <appl/dpp/UserInterface/pb/ui_pb_api_pmf_low_level_diag.h>

#if LINK_PB_LIBRARIES

#ifdef UI_PMF_LOW_LEVEL_DIAG
/******************************************************************** 
 *  Function handler: pmf_diag_force_prog_set (section pmf_low_level_diag)
 ********************************************************************/
int 
  ui_pb_api_pmf_low_level_diag_pmf_diag_force_prog_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_prog_id;
  uint8   
    prm_enable;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_pmf_low_level_diag"); 
  soc_sand_proc_name = "soc_pb_pmf_diag_force_prog_set"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_pmf_diag_force_prog_get(
          unit,
          &prm_prog_id,
          &prm_enable
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_pmf_diag_force_prog_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_pmf_diag_force_prog_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DIAG_FORCE_PROG_SET_PMF_DIAG_FORCE_PROG_SET_PROG_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_DIAG_FORCE_PROG_SET_PMF_DIAG_FORCE_PROG_SET_PROG_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_prog_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DIAG_FORCE_PROG_SET_PMF_DIAG_FORCE_PROG_SET_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_DIAG_FORCE_PROG_SET_PMF_DIAG_FORCE_PROG_SET_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_enable = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_pmf_diag_force_prog_set(
          unit,
          prm_prog_id,
          prm_enable
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_pmf_diag_force_prog_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_pmf_diag_force_prog_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pmf_diag_force_prog_get (section pmf_low_level_diag)
 ********************************************************************/
int 
  ui_pb_api_pmf_low_level_diag_pmf_diag_force_prog_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_prog_id;
  uint8   
    prm_enable;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_pmf_low_level_diag"); 
  soc_sand_proc_name = "soc_pb_pmf_diag_force_prog_get"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_pb_pmf_diag_force_prog_get(
          unit,
          &prm_prog_id,
          &prm_enable
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_pmf_diag_force_prog_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_pmf_diag_force_prog_get");   
    goto exit; 
  } 

  soc_sand_os_printf("prog_id: %u\n\r",prm_prog_id);

  soc_sand_os_printf("enable: %u\n\r",prm_enable);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pmf_diag_selected_progs_get (section pmf_low_level_diag)
 ********************************************************************/
int 
  ui_pb_api_pmf_low_level_diag_pmf_diag_selected_progs_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_progs_bmp;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_pmf_low_level_diag"); 
  soc_sand_proc_name = "soc_pb_pmf_diag_selected_progs_get"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_pb_pmf_diag_selected_progs_get(
          unit,
          &prm_progs_bmp
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_pmf_diag_selected_progs_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_pmf_diag_selected_progs_get");   
    goto exit; 
  } 

  soc_sand_os_printf("progs_bmp: %u\n\r",prm_progs_bmp);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pmf_diag_force_action_set (section pmf_low_level_diag)
 ********************************************************************/
int 
  ui_pb_api_pmf_low_level_diag_pmf_diag_force_action_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32 
    prm_actions_index = 0;  
  SOC_PB_PMF_DIAG_ACTION_INFO   
    prm_action_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_pmf_low_level_diag"); 
  soc_sand_proc_name = "soc_pb_pmf_diag_force_action_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_PMF_DIAG_ACTION_INFO_clear(&prm_action_info);
 
  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_pmf_diag_force_action_get(
          unit,
          &prm_action_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_pmf_diag_force_action_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_pmf_diag_force_action_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DIAG_FORCE_ACTION_SET_PMF_DIAG_FORCE_ACTION_SET_ACTION_INFO_STRENGTH_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_DIAG_FORCE_ACTION_SET_PMF_DIAG_FORCE_ACTION_SET_ACTION_INFO_STRENGTH_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_action_info.strength = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DIAG_FORCE_ACTION_SET_PMF_DIAG_FORCE_ACTION_SET_ACTION_INFO_ACTIONS_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_DIAG_FORCE_ACTION_SET_PMF_DIAG_FORCE_ACTION_SET_ACTION_INFO_ACTIONS_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_actions_index = (uint32)param_val->value.ulong_value;
  } 

  if(prm_actions_index != 0xFFFFFFFF)          
  { 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DIAG_FORCE_ACTION_SET_PMF_DIAG_FORCE_ACTION_SET_ACTION_INFO_ACTIONS_IS_VALID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_DIAG_FORCE_ACTION_SET_PMF_DIAG_FORCE_ACTION_SET_ACTION_INFO_ACTIONS_IS_VALID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_action_info.actions[ prm_actions_index].is_valid = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DIAG_FORCE_ACTION_SET_PMF_DIAG_FORCE_ACTION_SET_ACTION_INFO_ACTIONS_VALUE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_DIAG_FORCE_ACTION_SET_PMF_DIAG_FORCE_ACTION_SET_ACTION_INFO_ACTIONS_VALUE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_action_info.actions[ prm_actions_index].value = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DIAG_FORCE_ACTION_SET_PMF_DIAG_FORCE_ACTION_SET_ACTION_INFO_ACTIONS_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PMF_DIAG_FORCE_ACTION_SET_PMF_DIAG_FORCE_ACTION_SET_ACTION_INFO_ACTIONS_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_action_info.actions[ prm_actions_index].type = param_val->numeric_equivalent;
  } 

  }   


  /* Call function */
  ret = soc_pb_pmf_diag_force_action_set(
          unit,
          &prm_action_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_pmf_diag_force_action_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_pmf_diag_force_action_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pmf_diag_force_action_get (section pmf_low_level_diag)
 ********************************************************************/
int 
  ui_pb_api_pmf_low_level_diag_pmf_diag_force_action_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_PMF_DIAG_ACTION_INFO   
    prm_action_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_pmf_low_level_diag"); 
  soc_sand_proc_name = "soc_pb_pmf_diag_force_action_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_PMF_DIAG_ACTION_INFO_clear(&prm_action_info);
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_pb_pmf_diag_force_action_get(
          unit,
          &prm_action_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_pmf_diag_force_action_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_pmf_diag_force_action_get");   
    goto exit; 
  } 

  SOC_PB_PMF_DIAG_ACTION_INFO_print(&prm_action_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pmf_diag_built_keys_get (section pmf_low_level_diag)
 ********************************************************************/
int 
  ui_pb_api_pmf_low_level_diag_pmf_diag_built_keys_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_PMF_DIAG_KEYS_INFO   
    prm_built_keys;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_pmf_low_level_diag"); 
  soc_sand_proc_name = "soc_pb_pmf_diag_built_keys_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_PMF_DIAG_KEYS_INFO_clear(&prm_built_keys);
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_pb_pmf_diag_built_keys_get(
          unit,
          &prm_built_keys
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_pmf_diag_built_keys_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_pmf_diag_built_keys_get");   
    goto exit; 
  } 

  SOC_PB_PMF_DIAG_KEYS_INFO_print(&prm_built_keys);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pmf_diag_fem_freeze_set (section pmf_low_level_diag)
 ********************************************************************/
int 
  ui_pb_api_pmf_low_level_diag_pmf_diag_fem_freeze_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_PMF_DIAG_FEM_FREEZE_INFO   
    prm_freeze_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_pmf_low_level_diag"); 
  soc_sand_proc_name = "soc_pb_pmf_diag_fem_freeze_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_PMF_DIAG_FEM_FREEZE_INFO_clear(&prm_freeze_info);
 
  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_pmf_diag_fem_freeze_get(
          unit,
          &prm_freeze_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_pmf_diag_fem_freeze_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_pmf_diag_fem_freeze_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DIAG_FEM_FREEZE_SET_PMF_DIAG_FEM_FREEZE_SET_FREEZE_INFO_IS_PRG_VALID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_DIAG_FEM_FREEZE_SET_PMF_DIAG_FEM_FREEZE_SET_FREEZE_INFO_IS_PRG_VALID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_freeze_info.is_prg_valid = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DIAG_FEM_FREEZE_SET_PMF_DIAG_FEM_FREEZE_SET_FREEZE_INFO_PRG_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_DIAG_FEM_FREEZE_SET_PMF_DIAG_FEM_FREEZE_SET_FREEZE_INFO_PRG_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_freeze_info.prg = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DIAG_FEM_FREEZE_SET_PMF_DIAG_FEM_FREEZE_SET_FREEZE_INFO_PASS_NUM_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_DIAG_FEM_FREEZE_SET_PMF_DIAG_FEM_FREEZE_SET_FREEZE_INFO_PASS_NUM_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_freeze_info.pass_num = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DIAG_FEM_FREEZE_SET_PMF_DIAG_FEM_FREEZE_SET_FREEZE_INFO_FREEZE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_DIAG_FEM_FREEZE_SET_PMF_DIAG_FEM_FREEZE_SET_FREEZE_INFO_FREEZE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_freeze_info.freeze = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_pmf_diag_fem_freeze_set(
          unit,
          &prm_freeze_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_pmf_diag_fem_freeze_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_pmf_diag_fem_freeze_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pmf_diag_fem_freeze_get (section pmf_low_level_diag)
 ********************************************************************/
int 
  ui_pb_api_pmf_low_level_diag_pmf_diag_fem_freeze_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_PMF_DIAG_FEM_FREEZE_INFO   
    prm_freeze_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_pmf_low_level_diag"); 
  soc_sand_proc_name = "soc_pb_pmf_diag_fem_freeze_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_PMF_DIAG_FEM_FREEZE_INFO_clear(&prm_freeze_info);
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_pb_pmf_diag_fem_freeze_get(
          unit,
          &prm_freeze_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_pmf_diag_fem_freeze_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_pmf_diag_fem_freeze_get");   
    goto exit; 
  } 

  SOC_PB_PMF_DIAG_FEM_FREEZE_INFO_print(&prm_freeze_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pmf_diag_fems_info_get (section pmf_low_level_diag)
 ********************************************************************/
int 
  ui_pb_api_pmf_low_level_diag_pmf_diag_fems_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32
    ind;
  uint8   
    prm_release=0;
  uint8   
    prm_only_valid = TRUE;
  SOC_PB_PMF_DIAG_FEM_INFO   
    prm_fems_info[8];
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_pmf_low_level_diag"); 
  soc_sand_proc_name = "soc_pb_pmf_diag_fems_info_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_PMF_DIAG_FEM_INFO_clear(prm_fems_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DIAG_FEMS_INFO_GET_PMF_DIAG_FEMS_INFO_GET_RELEASE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_DIAG_FEMS_INFO_GET_PMF_DIAG_FEMS_INFO_GET_RELEASE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_release = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DIAG_FEMS_INFO_GET_PMF_DIAG_FEMS_INFO_GET_ONLY_VALID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_DIAG_FEMS_INFO_GET_PMF_DIAG_FEMS_INFO_GET_ONLY_VALID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_only_valid = (uint8)param_val->value.ulong_value;
  } 
  
  /* Call function */
  ret = soc_pb_pmf_diag_fems_info_get(
          unit,
          prm_release,
          prm_fems_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_pmf_diag_fems_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_pmf_diag_fems_info_get");   
    goto exit; 
  } 

  for (ind = 0; ind < 8; ++ind)
  {
    if (prm_only_valid && !prm_fems_info[ind].out_action.is_valid)
    {
      continue;
    }
    soc_sand_os_printf("\n\rFEM %u:\n\r",ind);
    SOC_PB_PMF_DIAG_FEM_INFO_print(&prm_fems_info[ind]);
  }
  

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
#endif
#ifdef UI_PMF_LOW_LEVEL_DIAG/* { pmf_low_level_diag*/
/******************************************************************** 
 *  Section handler: pmf_low_level_diag
 ********************************************************************/ 
int 
  ui_pb_api_pmf_low_level_diag( 
    CURRENT_LINE *current_line 
  ) 
{   
  int
    ret = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_pb_api_pmf_low_level_diag"); 
 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DIAG_FORCE_PROG_SET_PMF_DIAG_FORCE_PROG_SET_ID,1)) 
  { 
    ret = ui_pb_api_pmf_low_level_diag_pmf_diag_force_prog_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DIAG_FORCE_PROG_GET_PMF_DIAG_FORCE_PROG_GET_ID,1)) 
  { 
    ret = ui_pb_api_pmf_low_level_diag_pmf_diag_force_prog_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DIAG_SELECTED_PROGS_GET_PMF_DIAG_SELECTED_PROGS_GET_ID,1)) 
  { 
    ret = ui_pb_api_pmf_low_level_diag_pmf_diag_selected_progs_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DIAG_FORCE_ACTION_SET_PMF_DIAG_FORCE_ACTION_SET_ID,1)) 
  { 
    ret = ui_pb_api_pmf_low_level_diag_pmf_diag_force_action_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DIAG_FORCE_ACTION_GET_PMF_DIAG_FORCE_ACTION_GET_ID,1)) 
  { 
    ret = ui_pb_api_pmf_low_level_diag_pmf_diag_force_action_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DIAG_BUILT_KEYS_GET_PMF_DIAG_BUILT_KEYS_GET_ID,1)) 
  { 
    ret = ui_pb_api_pmf_low_level_diag_pmf_diag_built_keys_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DIAG_FEM_FREEZE_SET_PMF_DIAG_FEM_FREEZE_SET_ID,1)) 
  { 
    ret = ui_pb_api_pmf_low_level_diag_pmf_diag_fem_freeze_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DIAG_FEM_FREEZE_GET_PMF_DIAG_FEM_FREEZE_GET_ID,1)) 
  { 
    ret = ui_pb_api_pmf_low_level_diag_pmf_diag_fem_freeze_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DIAG_FEMS_INFO_GET_PMF_DIAG_FEMS_INFO_GET_ID,1)) 
  { 
    ret = ui_pb_api_pmf_low_level_diag_pmf_diag_fems_info_get(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after pmf_low_level_diag***", TRUE); 
  }
  
  ui_ret = ret; 
    
  goto exit; 
exit:        
  return ui_ret; 
}                
          
#endif /* pmf_low_level_diag */ 


#endif /* LINK_PB_LIBRARIES */ 

