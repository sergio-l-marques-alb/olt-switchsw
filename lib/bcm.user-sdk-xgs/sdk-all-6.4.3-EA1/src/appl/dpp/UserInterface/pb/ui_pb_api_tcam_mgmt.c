/* $Id: ui_pb_api_tcam_mgmt.c,v 1.6 Broadcom SDK $
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
 
  
#include <soc/dpp/Petra/PB_TM/pb_tcam_mgmt.h>
#include <appl/dpp/UserInterface/pb/ui_pb_api_tcam_mgmt.h>

#if LINK_PB_LIBRARIES

#ifdef UI_TCAM_MGMT
/******************************************************************** 
 *  Function handler: tcam_access_profile_create (section tcam_mgmt)
 ********************************************************************/
int 
  ui_pb_api_tcam_mgmt_tcam_access_profile_create(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_profile_id = 0;
  uint32   
    prm_tcam_db_id = 0;
  SOC_PB_TCAM_ACCESS_CYCLE   
    prm_cycle = 0;
  uint8   
    prm_uniform_prefix = 0;
  uint32   
    prm_min_banks = 0;
  SOC_PB_TCAM_MGMT_SIGNAL   
    prm_callback = 0;
  uint32   
    prm_user_data = 0;
  SOC_SAND_SUCCESS_FAILURE   
    prm_success;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_tcam_mgmt"); 
  soc_sand_proc_name = "soc_pb_tcam_access_profile_create"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_ACCESS_PROFILE_CREATE_TCAM_ACCESS_PROFILE_CREATE_PROFILE_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_ACCESS_PROFILE_CREATE_TCAM_ACCESS_PROFILE_CREATE_PROFILE_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_profile_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_ACCESS_PROFILE_CREATE_TCAM_ACCESS_PROFILE_CREATE_TCAM_DB_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_ACCESS_PROFILE_CREATE_TCAM_ACCESS_PROFILE_CREATE_TCAM_DB_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tcam_db_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_ACCESS_PROFILE_CREATE_TCAM_ACCESS_PROFILE_CREATE_CYCLE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_TCAM_ACCESS_PROFILE_CREATE_TCAM_ACCESS_PROFILE_CREATE_CYCLE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_cycle = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_ACCESS_PROFILE_CREATE_TCAM_ACCESS_PROFILE_CREATE_UNIFORM_PREFIX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_ACCESS_PROFILE_CREATE_TCAM_ACCESS_PROFILE_CREATE_UNIFORM_PREFIX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_uniform_prefix = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_ACCESS_PROFILE_CREATE_TCAM_ACCESS_PROFILE_CREATE_MIN_BANKS_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_ACCESS_PROFILE_CREATE_TCAM_ACCESS_PROFILE_CREATE_MIN_BANKS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_min_banks = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_ACCESS_PROFILE_CREATE_TCAM_ACCESS_PROFILE_CREATE_CALLBACK_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_ACCESS_PROFILE_CREATE_TCAM_ACCESS_PROFILE_CREATE_CALLBACK_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_callback = (SOC_PB_TCAM_MGMT_SIGNAL)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_ACCESS_PROFILE_CREATE_TCAM_ACCESS_PROFILE_CREATE_USER_DATA_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_ACCESS_PROFILE_CREATE_TCAM_ACCESS_PROFILE_CREATE_USER_DATA_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_user_data = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_tcam_access_profile_create_unsafe(
          unit,
          prm_profile_id,
          prm_tcam_db_id,
          prm_cycle,
          prm_uniform_prefix,
          prm_min_banks,
          prm_callback,
          prm_user_data,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_tcam_access_profile_create - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_tcam_access_profile_create");   
    goto exit; 
  } 



  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: tcam_managed_db_entry_add (section tcam_mgmt)
 ********************************************************************/
int 
  ui_pb_api_tcam_mgmt_tcam_managed_db_entry_add(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_profile_id = 0;
  uint32   
    prm_entry_id = 0;
  uint16   
    prm_priority = 0;
  uint32 
    prm_mask_index = 0xFFFFFFFF;  
  uint32 
    prm_value_index = 0xFFFFFFFF;  
  SOC_PB_TCAM_ENTRY   
    prm_entry;
  uint32   
    prm_action = 0;
  SOC_SAND_SUCCESS_FAILURE   
    prm_success;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_tcam_mgmt"); 
  soc_sand_proc_name = "soc_pb_tcam_managed_db_entry_add"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_TCAM_ENTRY_clear(&prm_entry);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_MANAGED_DB_ENTRY_ADD_TCAM_MANAGED_DB_ENTRY_ADD_PROFILE_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_MANAGED_DB_ENTRY_ADD_TCAM_MANAGED_DB_ENTRY_ADD_PROFILE_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_profile_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_MANAGED_DB_ENTRY_ADD_TCAM_MANAGED_DB_ENTRY_ADD_ENTRY_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_MANAGED_DB_ENTRY_ADD_TCAM_MANAGED_DB_ENTRY_ADD_ENTRY_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_entry_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_MANAGED_DB_ENTRY_ADD_TCAM_MANAGED_DB_ENTRY_ADD_PRIORITY_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_MANAGED_DB_ENTRY_ADD_TCAM_MANAGED_DB_ENTRY_ADD_PRIORITY_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_priority = (uint16)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_MANAGED_DB_ENTRY_ADD_TCAM_MANAGED_DB_ENTRY_ADD_ENTRY_MASK_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_MANAGED_DB_ENTRY_ADD_TCAM_MANAGED_DB_ENTRY_ADD_ENTRY_MASK_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_mask_index = (uint32)param_val->value.ulong_value;
  } 

  if(prm_mask_index != 0xFFFFFFFF)          
  { 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_MANAGED_DB_ENTRY_ADD_TCAM_MANAGED_DB_ENTRY_ADD_ENTRY_MASK_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_MANAGED_DB_ENTRY_ADD_TCAM_MANAGED_DB_ENTRY_ADD_ENTRY_MASK_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_entry.mask[ prm_mask_index] = (uint32)param_val->value.ulong_value;
  } 

  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_MANAGED_DB_ENTRY_ADD_TCAM_MANAGED_DB_ENTRY_ADD_ENTRY_VALUE_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_MANAGED_DB_ENTRY_ADD_TCAM_MANAGED_DB_ENTRY_ADD_ENTRY_VALUE_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_value_index = (uint32)param_val->value.ulong_value;
  } 

  if(prm_value_index != 0xFFFFFFFF)          
  { 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_MANAGED_DB_ENTRY_ADD_TCAM_MANAGED_DB_ENTRY_ADD_ENTRY_VALUE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_MANAGED_DB_ENTRY_ADD_TCAM_MANAGED_DB_ENTRY_ADD_ENTRY_VALUE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_entry.value[ prm_value_index] = (uint32)param_val->value.ulong_value;
  } 

  }   

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_MANAGED_DB_ENTRY_ADD_TCAM_MANAGED_DB_ENTRY_ADD_ACTION_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_MANAGED_DB_ENTRY_ADD_TCAM_MANAGED_DB_ENTRY_ADD_ACTION_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_action = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_tcam_managed_db_entry_add_unsafe(
          unit,
          prm_profile_id,
          prm_entry_id,
          prm_priority,
          &prm_entry,
          prm_action,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_tcam_managed_db_entry_add - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_tcam_managed_db_entry_add");   
    goto exit; 
  } 



  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: tcam_managed_db_entry_remove (section tcam_mgmt)
 ********************************************************************/
int 
  ui_pb_api_tcam_mgmt_tcam_managed_db_entry_remove(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_profile_id = 0;
  uint32   
    prm_entry_id = 0;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_tcam_mgmt"); 
  soc_sand_proc_name = "soc_pb_tcam_managed_db_entry_remove"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_MANAGED_DB_ENTRY_REMOVE_TCAM_MANAGED_DB_ENTRY_REMOVE_PROFILE_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_MANAGED_DB_ENTRY_REMOVE_TCAM_MANAGED_DB_ENTRY_REMOVE_PROFILE_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_profile_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_MANAGED_DB_ENTRY_REMOVE_TCAM_MANAGED_DB_ENTRY_REMOVE_ENTRY_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_MANAGED_DB_ENTRY_REMOVE_TCAM_MANAGED_DB_ENTRY_REMOVE_ENTRY_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_entry_id = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_tcam_managed_db_entry_remove_unsafe(
          unit,
          prm_profile_id,
          prm_entry_id
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_tcam_managed_db_entry_remove - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_tcam_managed_db_entry_remove");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: tcam_managed_bank_accessed_db_get (section tcam_mgmt)
 ********************************************************************/
int 
  ui_pb_api_tcam_mgmt_tcam_managed_bank_accessed_db_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_bank_id = 0;
  SOC_PB_TCAM_ACCESS_CYCLE   
    prm_cycle = 0;
  uint32   
    prm_tcam_db_id;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_tcam_mgmt"); 
  soc_sand_proc_name = "soc_pb_tcam_managed_bank_accessed_db_get"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_MANAGED_BANK_ACCESSED_DB_GET_TCAM_MANAGED_BANK_ACCESSED_DB_GET_BANK_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_MANAGED_BANK_ACCESSED_DB_GET_TCAM_MANAGED_BANK_ACCESSED_DB_GET_BANK_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_bank_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_MANAGED_BANK_ACCESSED_DB_GET_TCAM_MANAGED_BANK_ACCESSED_DB_GET_CYCLE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_TCAM_MANAGED_BANK_ACCESSED_DB_GET_TCAM_MANAGED_BANK_ACCESSED_DB_GET_CYCLE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_cycle = param_val->numeric_equivalent;
  } 


  /* Call function */
  ret = soc_pb_tcam_managed_bank_accessed_db_get_unsafe(
          unit,
          prm_bank_id,
          prm_cycle,
          &prm_tcam_db_id
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_tcam_managed_bank_accessed_db_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_tcam_managed_bank_accessed_db_get");   
    goto exit; 
  } 

  soc_sand_os_printf("tcam_db_id: %d\n\r",prm_tcam_db_id);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: tcam_access_profile_access_device_get (section tcam_mgmt)
 ********************************************************************/
int 
  ui_pb_api_tcam_mgmt_tcam_access_profile_access_device_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_profile_id = 0;
  SOC_PB_TCAM_ACCESS_DEVICE   
    prm_access_device;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_tcam_mgmt"); 
  soc_sand_proc_name = "soc_pb_tcam_access_profile_access_device_get"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_ACCESS_PROFILE_ACCESS_DEVICE_GET_TCAM_ACCESS_PROFILE_ACCESS_DEVICE_GET_PROFILE_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_ACCESS_PROFILE_ACCESS_DEVICE_GET_TCAM_ACCESS_PROFILE_ACCESS_DEVICE_GET_PROFILE_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_profile_id = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_tcam_access_profile_access_device_get_unsafe(
          unit,
          prm_profile_id,
          &prm_access_device
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_tcam_access_profile_access_device_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_tcam_access_profile_access_device_get");   
    goto exit; 
  } 

  soc_sand_os_printf("access_device: %s\n\r",SOC_PB_TCAM_ACCESS_DEVICE_to_string(prm_access_device));

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
#endif
#ifdef UI_TCAM_MGMT/* { tcam_mgmt*/
/******************************************************************** 
 *  Section handler: tcam_mgmt
 ********************************************************************/ 
int 
  ui_pb_api_tcam_mgmt( 
    CURRENT_LINE *current_line 
  ) 
{   
  int
    ret = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_pb_api_tcam_mgmt"); 
 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_ACCESS_PROFILE_CREATE_TCAM_ACCESS_PROFILE_CREATE_ID,1)) 
  { 
    ret = ui_pb_api_tcam_mgmt_tcam_access_profile_create(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_MANAGED_DB_ENTRY_ADD_TCAM_MANAGED_DB_ENTRY_ADD_ID,1)) 
  { 
    ret = ui_pb_api_tcam_mgmt_tcam_managed_db_entry_add(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_MANAGED_DB_ENTRY_REMOVE_TCAM_MANAGED_DB_ENTRY_REMOVE_ID,1)) 
  { 
    ret = ui_pb_api_tcam_mgmt_tcam_managed_db_entry_remove(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_MANAGED_BANK_ACCESSED_DB_GET_TCAM_MANAGED_BANK_ACCESSED_DB_GET_ID,1)) 
  { 
    ret = ui_pb_api_tcam_mgmt_tcam_managed_bank_accessed_db_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_ACCESS_PROFILE_ACCESS_DEVICE_GET_TCAM_ACCESS_PROFILE_ACCESS_DEVICE_GET_ID,1)) 
  { 
    ret = ui_pb_api_tcam_mgmt_tcam_access_profile_access_device_get(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after tcam_mgmt***", TRUE); 
  }
  
  ui_ret = ret; 
    
  goto exit; 
exit:        
  return ui_ret; 
}                
          
#endif /* tcam_mgmt */ 


#endif /* LINK_PB_LIBRARIES */ 

