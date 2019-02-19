/* $Id: ui_pb_api_tcam.c,v 1.7 Broadcom SDK $
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
 
  
#include <soc/dpp/Petra/PB_TM/pb_tcam.h>
#include <appl/dpp/UserInterface/pb/ui_pb_api_tcam.h>

#if LINK_PB_LIBRARIES

#ifdef UI_TCAM
/******************************************************************** 
 *  Function handler: bank_init (section tcam)
 ********************************************************************/
int 
  ui_pb_api_tcam_bank_init(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_bank_id = 0;
  SOC_PB_TCAM_BANK_ENTRY_SIZE   
    prm_entry_size = 0;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_tcam"); 
  soc_sand_proc_name = "soc_pb_tcam_bank_init"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_BANK_INIT_BANK_INIT_BANK_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_BANK_INIT_BANK_INIT_BANK_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_bank_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_BANK_INIT_BANK_INIT_ENTRY_SIZE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_TCAM_BANK_INIT_BANK_INIT_ENTRY_SIZE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_entry_size = param_val->numeric_equivalent;
  } 


  /* Call function */
  ret = soc_pb_tcam_bank_init_unsafe(
          unit,
          prm_bank_id,
          prm_entry_size
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_tcam_bank_init - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_tcam_bank_init");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: db_create (section tcam)
 ********************************************************************/
int 
  ui_pb_api_tcam_db_create(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_tcam_db_id = 0;
  SOC_PB_TCAM_BANK_ENTRY_SIZE   
    prm_entry_size = 0;
  uint32   
    prm_prefix_size = 0;
  SOC_PB_TCAM_DB_PRIO_MODE   
    prm_prio_mode = 0;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_tcam"); 
  soc_sand_proc_name = "soc_pb_tcam_db_create"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_DB_CREATE_DB_CREATE_TCAM_DB_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_DB_CREATE_DB_CREATE_TCAM_DB_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tcam_db_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_DB_CREATE_DB_CREATE_ENTRY_SIZE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_TCAM_DB_CREATE_DB_CREATE_ENTRY_SIZE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_entry_size = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_DB_CREATE_DB_CREATE_PREFIX_SIZE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_DB_CREATE_DB_CREATE_PREFIX_SIZE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_prefix_size = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_DB_CREATE_DB_CREATE_PRIO_MODE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_TCAM_DB_CREATE_DB_CREATE_PRIO_MODE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_prio_mode = param_val->numeric_equivalent;
  } 


  /* Call function */
  ret = soc_pb_tcam_db_create_unsafe(
          unit,
          prm_tcam_db_id,
          prm_entry_size,
          prm_prefix_size,
          prm_prio_mode
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_tcam_db_create - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_tcam_db_create");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: db_destroy (section tcam)
 ********************************************************************/
int 
  ui_pb_api_tcam_db_destroy(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_tcam_db_id = 0;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_tcam"); 
  soc_sand_proc_name = "soc_pb_tcam_db_destroy"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_DB_DESTROY_DB_DESTROY_TCAM_DB_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_DB_DESTROY_DB_DESTROY_TCAM_DB_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tcam_db_id = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_tcam_db_destroy_unsafe(
          unit,
          prm_tcam_db_id
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_tcam_db_destroy - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_tcam_db_destroy");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: db_bank_add (section tcam)
 ********************************************************************/
int 
  ui_pb_api_tcam_db_bank_add(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_tcam_db_id = 0;
  uint32   
    prm_bank_id = 0;
  SOC_PB_TCAM_PREFIX   
    prm_prefix;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_tcam"); 
  soc_sand_proc_name = "soc_pb_tcam_db_bank_add"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_TCAM_PREFIX_clear(&prm_prefix);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_DB_BANK_ADD_DB_BANK_ADD_TCAM_DB_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_DB_BANK_ADD_DB_BANK_ADD_TCAM_DB_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tcam_db_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_DB_BANK_ADD_DB_BANK_ADD_BANK_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_DB_BANK_ADD_DB_BANK_ADD_BANK_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_bank_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_DB_BANK_ADD_DB_BANK_ADD_PREFIX_LENGTH_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_DB_BANK_ADD_DB_BANK_ADD_PREFIX_LENGTH_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_prefix.length = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_DB_BANK_ADD_DB_BANK_ADD_PREFIX_BITS_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_DB_BANK_ADD_DB_BANK_ADD_PREFIX_BITS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_prefix.bits = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_tcam_db_bank_add_unsafe(
          unit,
          prm_tcam_db_id,
          prm_bank_id,
          &prm_prefix
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_tcam_db_bank_add - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_tcam_db_bank_add");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: db_bank_remove (section tcam)
 ********************************************************************/
int 
  ui_pb_api_tcam_db_bank_remove(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_tcam_db_id = 0;
  uint32   
    prm_bank_id = 0;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_tcam"); 
  soc_sand_proc_name = "soc_pb_tcam_db_bank_remove"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_DB_BANK_REMOVE_DB_BANK_REMOVE_TCAM_DB_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_DB_BANK_REMOVE_DB_BANK_REMOVE_TCAM_DB_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tcam_db_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_DB_BANK_REMOVE_DB_BANK_REMOVE_BANK_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_DB_BANK_REMOVE_DB_BANK_REMOVE_BANK_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_bank_id = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_tcam_db_bank_remove_unsafe(
          unit,
          prm_tcam_db_id,
          prm_bank_id
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_tcam_db_bank_remove - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_tcam_db_bank_remove");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: db_nof_banks_get (section tcam)
 ********************************************************************/
int 
  ui_pb_api_tcam_db_nof_banks_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_tcam_db_id = 0;
  uint32   
    prm_nof_banks = 0;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_tcam"); 
  soc_sand_proc_name = "soc_pb_tcam_db_nof_banks_get"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_DB_NOF_BANKS_GET_DB_NOF_BANKS_GET_TCAM_DB_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_DB_NOF_BANKS_GET_DB_NOF_BANKS_GET_TCAM_DB_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tcam_db_id = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_tcam_db_nof_banks_get_unsafe(
          unit,
          prm_tcam_db_id,
          &prm_nof_banks
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_tcam_db_nof_banks_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_tcam_db_nof_banks_get");   
    goto exit; 
  } 

  soc_sand_os_printf("nof_banks: %d\n\r",prm_nof_banks);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: db_bank_prefix_get (section tcam)
 ********************************************************************/
int 
  ui_pb_api_tcam_db_bank_prefix_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_tcam_db_id = 0;
  uint32   
    prm_bank_id = 0;
  SOC_PB_TCAM_PREFIX   
    prm_prefix;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_tcam"); 
  soc_sand_proc_name = "soc_pb_tcam_db_bank_prefix_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_TCAM_PREFIX_clear(&prm_prefix);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_DB_BANK_PREFIX_GET_DB_BANK_PREFIX_GET_TCAM_DB_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_DB_BANK_PREFIX_GET_DB_BANK_PREFIX_GET_TCAM_DB_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tcam_db_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_DB_BANK_PREFIX_GET_DB_BANK_PREFIX_GET_BANK_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_DB_BANK_PREFIX_GET_DB_BANK_PREFIX_GET_BANK_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_bank_id = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_tcam_db_bank_prefix_get_unsafe(
          unit,
          prm_tcam_db_id,
          prm_bank_id,
          &prm_prefix
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_tcam_db_bank_prefix_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_tcam_db_bank_prefix_get");   
    goto exit; 
  } 

  SOC_PB_TCAM_PREFIX_print(&prm_prefix);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: db_entry_size_get (section tcam)
 ********************************************************************/
int 
  ui_pb_api_tcam_db_entry_size_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_tcam_db_id = 0;
  SOC_PB_TCAM_BANK_ENTRY_SIZE   
    prm_entry_size = 0;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_tcam"); 
  soc_sand_proc_name = "soc_pb_tcam_db_entry_size_get"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_DB_ENTRY_SIZE_GET_DB_ENTRY_SIZE_GET_TCAM_DB_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_DB_ENTRY_SIZE_GET_DB_ENTRY_SIZE_GET_TCAM_DB_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tcam_db_id = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_tcam_db_entry_size_get_unsafe(
          unit,
          prm_tcam_db_id,
          &prm_entry_size
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_tcam_db_entry_size_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_tcam_db_entry_size_get");   
    goto exit; 
  } 

  soc_sand_os_printf("entry_size: %s\n\r",SOC_PB_TCAM_BANK_ENTRY_SIZE_to_string(prm_entry_size));

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: db_entry_add (section tcam)
 ********************************************************************/
int 
  ui_pb_api_tcam_db_entry_add(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_tcam_db_id = 0;
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
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_tcam"); 
  soc_sand_proc_name = "soc_pb_tcam_db_entry_add"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_TCAM_ENTRY_clear(&prm_entry);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_DB_ENTRY_ADD_DB_ENTRY_ADD_TCAM_DB_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_DB_ENTRY_ADD_DB_ENTRY_ADD_TCAM_DB_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tcam_db_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_DB_ENTRY_ADD_DB_ENTRY_ADD_ENTRY_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_DB_ENTRY_ADD_DB_ENTRY_ADD_ENTRY_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_entry_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_DB_ENTRY_ADD_DB_ENTRY_ADD_PRIORITY_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_DB_ENTRY_ADD_DB_ENTRY_ADD_PRIORITY_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_priority = (uint16)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_DB_ENTRY_ADD_DB_ENTRY_ADD_ENTRY_MASK_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_DB_ENTRY_ADD_DB_ENTRY_ADD_ENTRY_MASK_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_mask_index = (uint32)param_val->value.ulong_value;
  } 

  if(prm_mask_index != 0xFFFFFFFF)          
  { 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_DB_ENTRY_ADD_DB_ENTRY_ADD_ENTRY_MASK_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_DB_ENTRY_ADD_DB_ENTRY_ADD_ENTRY_MASK_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_entry.mask[ prm_mask_index] = (uint32)param_val->value.ulong_value;
  } 

  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_DB_ENTRY_ADD_DB_ENTRY_ADD_ENTRY_VALUE_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_DB_ENTRY_ADD_DB_ENTRY_ADD_ENTRY_VALUE_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_value_index = (uint32)param_val->value.ulong_value;
  } 

  if(prm_value_index != 0xFFFFFFFF)          
  { 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_DB_ENTRY_ADD_DB_ENTRY_ADD_ENTRY_VALUE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_DB_ENTRY_ADD_DB_ENTRY_ADD_ENTRY_VALUE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_entry.value[ prm_value_index] = (uint32)param_val->value.ulong_value;
  } 

  }   

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_DB_ENTRY_ADD_DB_ENTRY_ADD_ACTION_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_DB_ENTRY_ADD_DB_ENTRY_ADD_ACTION_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_action = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_tcam_db_entry_add_unsafe(
          unit,
          prm_tcam_db_id,
          prm_entry_id,
          prm_priority,
          &prm_entry,
          prm_action,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_tcam_db_entry_add - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_tcam_db_entry_add");   
    goto exit; 
  } 



  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: db_entry_get (section tcam)
 ********************************************************************/
int 
  ui_pb_api_tcam_db_entry_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_tcam_db_id = 0;
  uint32   
    prm_entry_id = 0;
  uint16   
    prm_priority = 0;
  SOC_PB_TCAM_ENTRY   
    prm_entry;
  uint32   
    prm_action = 0;
  uint8
    prm_found;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_tcam"); 
  soc_sand_proc_name = "soc_pb_tcam_db_entry_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_TCAM_ENTRY_clear(&prm_entry);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_DB_ENTRY_GET_DB_ENTRY_GET_TCAM_DB_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_DB_ENTRY_GET_DB_ENTRY_GET_TCAM_DB_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tcam_db_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_DB_ENTRY_GET_DB_ENTRY_GET_ENTRY_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_DB_ENTRY_GET_DB_ENTRY_GET_ENTRY_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_entry_id = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_tcam_db_entry_get_unsafe(
          unit,
          prm_tcam_db_id,
          prm_entry_id,
          &prm_priority,
          &prm_entry,
          &prm_action,
          &prm_found
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_tcam_db_entry_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_tcam_db_entry_get");   
    goto exit; 
  } 

  soc_sand_os_printf("priority: %hd\n\r",prm_priority);

  SOC_PB_TCAM_ENTRY_print(&prm_entry);

  soc_sand_os_printf("action: %u\n\r",prm_action);

  
  goto exit; 
exit: 
  return ui_ret; 
} 

/******************************************************************** 
 *  Function handler: db_entry_search (section tcam)
 ********************************************************************/
int 
  ui_pb_api_tcam_db_entry_search(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_tcam_db_id=0;
  uint32 
    prm_mask_index = 0xFFFFFFFF;  
  uint32 
    prm_value_index = 0xFFFFFFFF;  
  SOC_PB_TCAM_ENTRY   
    prm_key;
  uint32   
    prm_entry_id;
  uint8   
    prm_found;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_tcam"); 
  soc_sand_proc_name = "soc_pb_tcam_db_entry_search"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_TCAM_ENTRY_clear(&prm_key);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_DB_ENTRY_SEARCH_DB_ENTRY_SEARCH_TCAM_DB_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_DB_ENTRY_SEARCH_DB_ENTRY_SEARCH_TCAM_DB_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tcam_db_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_DB_ENTRY_SEARCH_DB_ENTRY_SEARCH_KEY_MASK_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_DB_ENTRY_SEARCH_DB_ENTRY_SEARCH_KEY_MASK_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_mask_index = (uint32)param_val->value.ulong_value;
  } 

  if(prm_mask_index != 0xFFFFFFFF)          
  { 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_DB_ENTRY_SEARCH_DB_ENTRY_SEARCH_KEY_MASK_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_DB_ENTRY_SEARCH_DB_ENTRY_SEARCH_KEY_MASK_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key.mask[ prm_mask_index] = (uint32)param_val->value.ulong_value;
  } 

  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_DB_ENTRY_SEARCH_DB_ENTRY_SEARCH_KEY_VALUE_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_DB_ENTRY_SEARCH_DB_ENTRY_SEARCH_KEY_VALUE_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_value_index = (uint32)param_val->value.ulong_value;
  } 

  if(prm_value_index != 0xFFFFFFFF)          
  { 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_DB_ENTRY_SEARCH_DB_ENTRY_SEARCH_KEY_VALUE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_DB_ENTRY_SEARCH_DB_ENTRY_SEARCH_KEY_VALUE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key.value[ prm_value_index] = (uint32)param_val->value.ulong_value;
  } 

  }   


  /* Call function */
  ret = soc_pb_tcam_db_entry_search_unsafe(
          unit,
          prm_tcam_db_id,
          &prm_key,
          &prm_entry_id,
          &prm_found
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_tcam_db_entry_search - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_tcam_db_entry_search");   
    goto exit; 
  } 

  soc_sand_os_printf("entry_id: %d\n\r",prm_entry_id);

  soc_sand_os_printf("found: %u\n\r",prm_found);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: db_entry_remove (section tcam)
 ********************************************************************/
int 
  ui_pb_api_tcam_db_entry_remove(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_tcam_db_id = 0;
  uint32   
    prm_entry_id = 0;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_tcam"); 
  soc_sand_proc_name = "soc_pb_tcam_db_entry_remove"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_DB_ENTRY_REMOVE_DB_ENTRY_REMOVE_TCAM_DB_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_DB_ENTRY_REMOVE_DB_ENTRY_REMOVE_TCAM_DB_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tcam_db_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_DB_ENTRY_REMOVE_DB_ENTRY_REMOVE_ENTRY_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_DB_ENTRY_REMOVE_DB_ENTRY_REMOVE_ENTRY_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_entry_id = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_tcam_db_entry_remove_unsafe(
          unit,
          prm_tcam_db_id,
          prm_entry_id
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_tcam_db_entry_remove - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_tcam_db_entry_remove");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: db_is_bank_used (section tcam)
 ********************************************************************/
int 
  ui_pb_api_tcam_db_is_bank_used(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_tcam_db_id = 0;
  uint32   
    prm_bank_id = 0;
  uint8   
    prm_is_used;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_tcam"); 
  soc_sand_proc_name = "soc_pb_tcam_db_is_bank_used"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_DB_IS_BANK_USED_DB_IS_BANK_USED_TCAM_DB_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_DB_IS_BANK_USED_DB_IS_BANK_USED_TCAM_DB_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tcam_db_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_DB_IS_BANK_USED_DB_IS_BANK_USED_BANK_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_DB_IS_BANK_USED_DB_IS_BANK_USED_BANK_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_bank_id = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_tcam_db_is_bank_used_unsafe(
          unit,
          prm_tcam_db_id,
          prm_bank_id,
          &prm_is_used
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_tcam_db_is_bank_used - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_tcam_db_is_bank_used");   
    goto exit; 
  } 

  soc_sand_os_printf("is_used: %u\n\r",prm_is_used);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
#endif
#ifdef UI_TCAM/* { tcam*/
/******************************************************************** 
 *  Section handler: tcam
 ********************************************************************/ 
int 
  ui_pb_api_tcam( 
    CURRENT_LINE *current_line 
  ) 
{   
  int
    ret = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_pb_api_tcam"); 
 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_BANK_INIT_BANK_INIT_ID,1)) 
  { 
    ret = ui_pb_api_tcam_bank_init(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_DB_CREATE_DB_CREATE_ID,1)) 
  { 
    ret = ui_pb_api_tcam_db_create(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_DB_DESTROY_DB_DESTROY_ID,1)) 
  { 
    ret = ui_pb_api_tcam_db_destroy(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_DB_BANK_ADD_DB_BANK_ADD_ID,1)) 
  { 
    ret = ui_pb_api_tcam_db_bank_add(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_DB_BANK_REMOVE_DB_BANK_REMOVE_ID,1)) 
  { 
    ret = ui_pb_api_tcam_db_bank_remove(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_DB_NOF_BANKS_GET_DB_NOF_BANKS_GET_ID,1)) 
  { 
    ret = ui_pb_api_tcam_db_nof_banks_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_DB_BANK_PREFIX_GET_DB_BANK_PREFIX_GET_ID,1)) 
  { 
    ret = ui_pb_api_tcam_db_bank_prefix_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_DB_ENTRY_SIZE_GET_DB_ENTRY_SIZE_GET_ID,1)) 
  { 
    ret = ui_pb_api_tcam_db_entry_size_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_DB_ENTRY_ADD_DB_ENTRY_ADD_ID,1)) 
  { 
    ret = ui_pb_api_tcam_db_entry_add(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_DB_ENTRY_GET_DB_ENTRY_GET_ID,1)) 
  { 
    ret = ui_pb_api_tcam_db_entry_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_DB_ENTRY_SEARCH_DB_ENTRY_SEARCH_ID,1)) 
  { 
    ret = ui_pb_api_tcam_db_entry_search(current_line); 
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_DB_ENTRY_REMOVE_DB_ENTRY_REMOVE_ID,1)) 
  { 
    ret = ui_pb_api_tcam_db_entry_remove(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_DB_IS_BANK_USED_DB_IS_BANK_USED_ID,1)) 
  { 
    ret = ui_pb_api_tcam_db_is_bank_used(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after tcam***", TRUE); 
  }
  
  ui_ret = ret; 
    
  goto exit; 
exit:        
  return ui_ret; 
}                
          
#endif /* tcam */ 


#endif /* LINK_PB_LIBRARIES */ 

