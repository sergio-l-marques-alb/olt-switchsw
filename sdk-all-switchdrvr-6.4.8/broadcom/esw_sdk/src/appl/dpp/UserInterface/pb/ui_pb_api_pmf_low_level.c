/* $Id: ui_pb_api_pmf_low_level.c,v 1.6 Broadcom SDK $
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
#include <appl/dpp/UserInterface/ui_pure_defi_ppd_api.h> 
 

#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level.h>
#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level_ce.h>
#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level_db.h>
#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level_fem_tag.h>
#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level_pgm.h>
#include <appl/dpp/UserInterface/pb/ui_pb_api_pmf_low_level.h>

#ifdef UI_PMF_LOW_LEVEL
/******************************************************************** 
 *  Function handler: pmf_ce_packet_header_entry_set (section pmf_low_level)
 ********************************************************************/
int 
  ui_pb_api_pmf_low_level_pmf_ce_packet_header_entry_set_unsafe(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_pmf_pgm_ndx;
  uint32   
    prm_pmf_key;
  uint32   
    prm_ce_ndx;
  SOC_TMC_PMF_CE_PACKET_HEADER_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_pmf_low_level"); 
  soc_sand_proc_name = "soc_ppd_pmf_ce_packet_header_entry_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_TMC_PMF_CE_PACKET_HEADER_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_CE_PACKET_HEADER_ENTRY_SET_PMF_CE_PACKET_HEADER_ENTRY_SET_PMF_PGM_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_CE_PACKET_HEADER_ENTRY_SET_PMF_CE_PACKET_HEADER_ENTRY_SET_PMF_PGM_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_pmf_pgm_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter pmf_pgm_ndx after pmf_ce_packet_header_entry_set***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_CE_PACKET_HEADER_ENTRY_SET_PMF_CE_PACKET_HEADER_ENTRY_SET_PMF_KEY_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_CE_PACKET_HEADER_ENTRY_SET_PMF_CE_PACKET_HEADER_ENTRY_SET_PMF_KEY_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_pmf_key = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter pmf_key after pmf_ce_packet_header_entry_set***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_CE_PACKET_HEADER_ENTRY_SET_PMF_CE_PACKET_HEADER_ENTRY_SET_CE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_CE_PACKET_HEADER_ENTRY_SET_PMF_CE_PACKET_HEADER_ENTRY_SET_CE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_ce_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter ce_ndx after pmf_ce_packet_header_entry_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_pmf_ce_packet_header_entry_get_unsafe(
          unit,
          prm_pmf_pgm_ndx,
          prm_pmf_key,
          prm_ce_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_ce_packet_header_entry_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_ce_packet_header_entry_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_CE_PACKET_HEADER_ENTRY_SET_PMF_CE_PACKET_HEADER_ENTRY_SET_INFO_NOF_BITS_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_CE_PACKET_HEADER_ENTRY_SET_PMF_CE_PACKET_HEADER_ENTRY_SET_INFO_NOF_BITS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.nof_bits = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_CE_PACKET_HEADER_ENTRY_SET_PMF_CE_PACKET_HEADER_ENTRY_SET_INFO_OFFSET_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_CE_PACKET_HEADER_ENTRY_SET_PMF_CE_PACKET_HEADER_ENTRY_SET_INFO_OFFSET_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.offset = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_CE_PACKET_HEADER_ENTRY_SET_PMF_CE_PACKET_HEADER_ENTRY_SET_INFO_SUB_HEADER_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PMF_CE_PACKET_HEADER_ENTRY_SET_PMF_CE_PACKET_HEADER_ENTRY_SET_INFO_SUB_HEADER_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.sub_header = param_val->numeric_equivalent;
  } 


  /* Call function */
  ret = soc_pb_pmf_ce_packet_header_entry_set_unsafe(
          unit,
          prm_pmf_pgm_ndx,
          prm_pmf_key,
          prm_ce_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_ce_packet_header_entry_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_ce_packet_header_entry_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pmf_ce_packet_header_entry_get (section pmf_low_level)
 ********************************************************************/
int 
  ui_pb_api_pmf_low_level_pmf_ce_packet_header_entry_get_unsafe(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_pmf_pgm_ndx;
  uint32   
    prm_pmf_key;
  uint32   
    prm_ce_ndx;
  SOC_TMC_PMF_CE_PACKET_HEADER_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_pmf_low_level"); 
  soc_sand_proc_name = "soc_ppd_pmf_ce_packet_header_entry_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_TMC_PMF_CE_PACKET_HEADER_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_CE_PACKET_HEADER_ENTRY_GET_PMF_CE_PACKET_HEADER_ENTRY_GET_PMF_PGM_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_CE_PACKET_HEADER_ENTRY_GET_PMF_CE_PACKET_HEADER_ENTRY_GET_PMF_PGM_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_pmf_pgm_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter pmf_pgm_ndx after pmf_ce_packet_header_entry_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_CE_PACKET_HEADER_ENTRY_GET_PMF_CE_PACKET_HEADER_ENTRY_GET_PMF_KEY_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_CE_PACKET_HEADER_ENTRY_GET_PMF_CE_PACKET_HEADER_ENTRY_GET_PMF_KEY_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_pmf_key = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter pmf_key after pmf_ce_packet_header_entry_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_CE_PACKET_HEADER_ENTRY_GET_PMF_CE_PACKET_HEADER_ENTRY_GET_CE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_CE_PACKET_HEADER_ENTRY_GET_PMF_CE_PACKET_HEADER_ENTRY_GET_CE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_ce_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter ce_ndx after pmf_ce_packet_header_entry_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_pmf_ce_packet_header_entry_get_unsafe(
          unit,
          prm_pmf_pgm_ndx,
          prm_pmf_key,
          prm_ce_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_ce_packet_header_entry_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_ce_packet_header_entry_get");   
    goto exit; 
  } 

  SOC_TMC_PMF_CE_PACKET_HEADER_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pmf_ce_irpp_info_entry_set (section pmf_low_level)
 ********************************************************************/
int 
  ui_pb_api_pmf_low_level_pmf_ce_irpp_info_entry_set_unsafe(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_pmf_pgm_ndx;
  uint32   
    prm_pmf_key;
  uint32   
    prm_ce_ndx;
  SOC_TMC_PMF_IRPP_INFO_FIELD   
    prm_irpp_field;
  uint32
    nof_bits;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_pmf_low_level"); 
  soc_sand_proc_name = "soc_ppd_pmf_ce_irpp_info_entry_set"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_CE_IRPP_INFO_ENTRY_SET_PMF_CE_IRPP_INFO_ENTRY_SET_PMF_PGM_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_CE_IRPP_INFO_ENTRY_SET_PMF_CE_IRPP_INFO_ENTRY_SET_PMF_PGM_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_pmf_pgm_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter pmf_pgm_ndx after pmf_ce_irpp_info_entry_set***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_CE_IRPP_INFO_ENTRY_SET_PMF_CE_IRPP_INFO_ENTRY_SET_PMF_KEY_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_CE_IRPP_INFO_ENTRY_SET_PMF_CE_IRPP_INFO_ENTRY_SET_PMF_KEY_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_pmf_key = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter pmf_key after pmf_ce_irpp_info_entry_set***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_CE_IRPP_INFO_ENTRY_SET_PMF_CE_IRPP_INFO_ENTRY_SET_CE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_CE_IRPP_INFO_ENTRY_SET_PMF_CE_IRPP_INFO_ENTRY_SET_CE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_ce_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter ce_ndx after pmf_ce_irpp_info_entry_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_pmf_ce_irpp_info_entry_get_unsafe(
          unit,
          prm_pmf_pgm_ndx,
          prm_pmf_key,
          prm_ce_ndx,
          &nof_bits,
          &prm_irpp_field
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_ce_irpp_info_entry_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_ce_irpp_info_entry_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_CE_IRPP_INFO_ENTRY_SET_PMF_CE_IRPP_INFO_ENTRY_SET_IRPP_FIELD_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PMF_CE_IRPP_INFO_ENTRY_SET_PMF_CE_IRPP_INFO_ENTRY_SET_IRPP_FIELD_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_irpp_field = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_CE_IRPP_INFO_ENTRY_SET_PMF_CE_IRPP_INFO_ENTRY_SET_SIZE_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_CE_IRPP_INFO_ENTRY_SET_PMF_CE_IRPP_INFO_ENTRY_SET_SIZE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    nof_bits = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_pmf_ce_irpp_info_entry_set_unsafe(
          unit,
          prm_pmf_pgm_ndx,
          prm_pmf_key,
          prm_ce_ndx,
          nof_bits,
          prm_irpp_field
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_ce_irpp_info_entry_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_ce_irpp_info_entry_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pmf_ce_irpp_info_entry_get (section pmf_low_level)
 ********************************************************************/
int 
  ui_pb_api_pmf_low_level_pmf_ce_irpp_info_entry_get_unsafe(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    nof_bits,
    ret;   
  uint32   
    prm_pmf_pgm_ndx;
  uint32   
    prm_pmf_key;
  uint32   
    prm_ce_ndx;
  SOC_TMC_PMF_IRPP_INFO_FIELD   
    prm_irpp_field;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_pmf_low_level"); 
  soc_sand_proc_name = "soc_ppd_pmf_ce_irpp_info_entry_get"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_CE_IRPP_INFO_ENTRY_GET_PMF_CE_IRPP_INFO_ENTRY_GET_PMF_PGM_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_CE_IRPP_INFO_ENTRY_GET_PMF_CE_IRPP_INFO_ENTRY_GET_PMF_PGM_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_pmf_pgm_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter pmf_pgm_ndx after pmf_ce_irpp_info_entry_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_CE_IRPP_INFO_ENTRY_GET_PMF_CE_IRPP_INFO_ENTRY_GET_PMF_KEY_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_CE_IRPP_INFO_ENTRY_GET_PMF_CE_IRPP_INFO_ENTRY_GET_PMF_KEY_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_pmf_key = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter pmf_key after pmf_ce_irpp_info_entry_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_CE_IRPP_INFO_ENTRY_GET_PMF_CE_IRPP_INFO_ENTRY_GET_CE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_CE_IRPP_INFO_ENTRY_GET_PMF_CE_IRPP_INFO_ENTRY_GET_CE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_ce_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter ce_ndx after pmf_ce_irpp_info_entry_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_pmf_ce_irpp_info_entry_get_unsafe(
          unit,
          prm_pmf_pgm_ndx,
          prm_pmf_key,
          prm_ce_ndx,
          &nof_bits,
          &prm_irpp_field
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_ce_irpp_info_entry_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_ce_irpp_info_entry_get");   
    goto exit; 
  } 

  soc_sand_os_printf("nof_bits: %d\n\r",nof_bits);
  soc_sand_os_printf("irpp_field: %s\n\r",SOC_TMC_PMF_IRPP_INFO_FIELD_to_string(prm_irpp_field));

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pmf_ce_nop_entry_set (section pmf_low_level)
 ********************************************************************/
int 
  ui_pb_api_pmf_low_level_pmf_ce_nop_entry_set_unsafe(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32
    prm_loc,
    ret;   
  uint32   
    prm_pmf_pgm_ndx;
  uint32   
    prm_pmf_key;
  uint32   
    prm_ce_ndx;
  uint8   
    prm_is_ce_not_valid;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_pmf_low_level"); 
  soc_sand_proc_name = "soc_ppd_pmf_ce_nop_entry_set"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_CE_NOP_ENTRY_SET_PMF_CE_NOP_ENTRY_SET_PMF_PGM_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_CE_NOP_ENTRY_SET_PMF_CE_NOP_ENTRY_SET_PMF_PGM_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_pmf_pgm_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter pmf_pgm_ndx after pmf_ce_nop_entry_set***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_CE_NOP_ENTRY_SET_PMF_CE_NOP_ENTRY_SET_PMF_KEY_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_CE_NOP_ENTRY_SET_PMF_CE_NOP_ENTRY_SET_PMF_KEY_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_pmf_key = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter pmf_key after pmf_ce_nop_entry_set***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_CE_NOP_ENTRY_SET_PMF_CE_NOP_ENTRY_SET_CE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_CE_NOP_ENTRY_SET_PMF_CE_NOP_ENTRY_SET_CE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_ce_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter ce_ndx after pmf_ce_nop_entry_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_pmf_ce_nop_entry_get_unsafe(
          unit,
          prm_pmf_pgm_ndx,
          prm_pmf_key,
          prm_ce_ndx,
          &prm_is_ce_not_valid,
          &prm_loc
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_ce_nop_entry_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_ce_nop_entry_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_CE_NOP_ENTRY_SET_PMF_CE_NOP_ENTRY_SET_IS_CE_NOT_VALID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_CE_NOP_ENTRY_SET_PMF_CE_NOP_ENTRY_SET_IS_CE_NOT_VALID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_is_ce_not_valid = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_pmf_ce_nop_entry_set_unsafe(
          unit,
          prm_pmf_pgm_ndx,
          prm_pmf_key,
          prm_ce_ndx,
          prm_is_ce_not_valid
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_ce_nop_entry_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_ce_nop_entry_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pmf_ce_nop_entry_get (section pmf_low_level)
 ********************************************************************/
int 
  ui_pb_api_pmf_low_level_pmf_ce_nop_entry_get_unsafe(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32
    prm_loc,
    ret;   
  uint32   
    prm_pmf_pgm_ndx;
  uint32   
    prm_pmf_key;
  uint32   
    prm_ce_ndx;
  uint8   
    prm_is_ce_not_valid;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_pmf_low_level"); 
  soc_sand_proc_name = "soc_ppd_pmf_ce_nop_entry_get"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_CE_NOP_ENTRY_GET_PMF_CE_NOP_ENTRY_GET_PMF_PGM_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_CE_NOP_ENTRY_GET_PMF_CE_NOP_ENTRY_GET_PMF_PGM_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_pmf_pgm_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter pmf_pgm_ndx after pmf_ce_nop_entry_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_CE_NOP_ENTRY_GET_PMF_CE_NOP_ENTRY_GET_PMF_KEY_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_CE_NOP_ENTRY_GET_PMF_CE_NOP_ENTRY_GET_PMF_KEY_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_pmf_key = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter pmf_key after pmf_ce_nop_entry_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_CE_NOP_ENTRY_GET_PMF_CE_NOP_ENTRY_GET_CE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_CE_NOP_ENTRY_GET_PMF_CE_NOP_ENTRY_GET_CE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_ce_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter ce_ndx after pmf_ce_nop_entry_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_pmf_ce_nop_entry_get_unsafe(
          unit,
          prm_pmf_pgm_ndx,
          prm_pmf_key,
          prm_ce_ndx,
          &prm_is_ce_not_valid,
          &prm_loc
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_ce_nop_entry_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_ce_nop_entry_get");   
    goto exit; 
  } 

  soc_sand_os_printf("is_ce_not_valid: %u\n\r",prm_is_ce_not_valid);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pmf_tcam_lookup_set (section pmf_low_level)
 ********************************************************************/
int 
  ui_pb_api_pmf_low_level_pmf_tcam_lookup_set_unsafe(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_TMC_PMF_LKP_PROFILE   
    prm_lkp_profile_ndx;
  uint32   
    prm_db_id_ndx;
  SOC_TMC_PMF_TCAM_KEY_SELECTION_INFO   
    prm_info;
  SOC_SAND_SUCCESS_FAILURE   
    prm_success;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_pmf_low_level"); 
  soc_sand_proc_name = "soc_ppd_pmf_tcam_lookup_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_TMC_PMF_LKP_PROFILE_clear(&prm_lkp_profile_ndx);
  SOC_TMC_PMF_TCAM_KEY_SELECTION_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_LOOKUP_SET_PMF_TCAM_LOOKUP_SET_LKP_PROFILE_NDX_CYCLE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_TCAM_LOOKUP_SET_PMF_TCAM_LOOKUP_SET_LKP_PROFILE_NDX_CYCLE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_lkp_profile_ndx.cycle_ndx = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_LOOKUP_SET_PMF_TCAM_LOOKUP_SET_LKP_PROFILE_NDX_ID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_TCAM_LOOKUP_SET_PMF_TCAM_LOOKUP_SET_LKP_PROFILE_NDX_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_lkp_profile_ndx.id = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter lkp_profile_ndx after pmf_tcam_lookup_set***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_LOOKUP_SET_PMF_TCAM_LOOKUP_SET_DB_ID_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_TCAM_LOOKUP_SET_PMF_TCAM_LOOKUP_SET_DB_ID_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_db_id_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter db_id_ndx after pmf_tcam_lookup_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_pmf_tcam_lookup_get_unsafe(
          unit,
          &prm_lkp_profile_ndx,
          prm_db_id_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_tcam_lookup_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_tcam_lookup_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_LOOKUP_SET_PMF_TCAM_LOOKUP_SET_INFO_KEY_SRC_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PMF_TCAM_LOOKUP_SET_PMF_TCAM_LOOKUP_SET_INFO_KEY_SRC_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.key_src = param_val->numeric_equivalent;
  } 


  /* Call function */
  ret = soc_pb_pmf_tcam_lookup_set_unsafe(
          unit,
          &prm_lkp_profile_ndx,
          prm_db_id_ndx,
          &prm_info,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_tcam_lookup_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_tcam_lookup_set");   
    goto exit; 
  } 

  soc_sand_os_printf("success: %s\n\r",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pmf_tcam_lookup_get (section pmf_low_level)
 ********************************************************************/
int 
  ui_pb_api_pmf_low_level_pmf_tcam_lookup_get_unsafe(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_TMC_PMF_LKP_PROFILE   
    prm_lkp_profile_ndx;
  uint32   
    prm_db_id_ndx;
  SOC_TMC_PMF_TCAM_KEY_SELECTION_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_pmf_low_level"); 
  soc_sand_proc_name = "soc_ppd_pmf_tcam_lookup_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_TMC_PMF_LKP_PROFILE_clear(&prm_lkp_profile_ndx);
  SOC_TMC_PMF_TCAM_KEY_SELECTION_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_LOOKUP_GET_PMF_TCAM_LOOKUP_GET_LKP_PROFILE_NDX_CYCLE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_TCAM_LOOKUP_GET_PMF_TCAM_LOOKUP_GET_LKP_PROFILE_NDX_CYCLE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_lkp_profile_ndx.cycle_ndx = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_LOOKUP_GET_PMF_TCAM_LOOKUP_GET_LKP_PROFILE_NDX_ID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_TCAM_LOOKUP_GET_PMF_TCAM_LOOKUP_GET_LKP_PROFILE_NDX_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_lkp_profile_ndx.id = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter lkp_profile_ndx after pmf_tcam_lookup_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_LOOKUP_GET_PMF_TCAM_LOOKUP_GET_DB_ID_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_TCAM_LOOKUP_GET_PMF_TCAM_LOOKUP_GET_DB_ID_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_db_id_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter db_id_ndx after pmf_tcam_lookup_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_pmf_tcam_lookup_get_unsafe(
          unit,
          &prm_lkp_profile_ndx,
          prm_db_id_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_tcam_lookup_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_tcam_lookup_get");   
    goto exit; 
  } 

  SOC_TMC_PMF_TCAM_KEY_SELECTION_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pmf_tcam_entry_add (section pmf_low_level)
 ********************************************************************/
int 
  ui_pb_api_pmf_low_level_pmf_tcam_entry_add(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_TMC_PMF_TCAM_ENTRY_ID   
    prm_entry_ndx;
  uint32 
    prm_mask_index = 0xFFFFFFFF;  
  uint32 
    prm_val_index = 0xFFFFFFFF;  
  SOC_TMC_PMF_TCAM_DATA
    prm_tcam_data;
  SOC_SAND_SUCCESS_FAILURE
    prm_success;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_pmf_low_level"); 
  soc_sand_proc_name = "soc_ppd_pmf_tcam_entry_add"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_TMC_PMF_TCAM_ENTRY_ID_clear(&prm_entry_ndx);
  SOC_TMC_PMF_TCAM_DATA_clear(&prm_tcam_data);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_ENTRY_ADD_PMF_TCAM_ENTRY_ADD_ENTRY_NDX_ENTRY_ID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_TCAM_ENTRY_ADD_PMF_TCAM_ENTRY_ADD_ENTRY_NDX_ENTRY_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_entry_ndx.entry_id = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_ENTRY_ADD_PMF_TCAM_ENTRY_ADD_ENTRY_NDX_DB_ID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_TCAM_ENTRY_ADD_PMF_TCAM_ENTRY_ADD_ENTRY_NDX_DB_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_entry_ndx.db_id = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter entry_ndx after pmf_tcam_entry_add***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_ENTRY_ADD_PMF_TCAM_ENTRY_ADD_TCAM_DATA_OUTPUT_VAL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_TCAM_ENTRY_ADD_PMF_TCAM_ENTRY_ADD_TCAM_DATA_OUTPUT_VAL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tcam_data.output.val = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_ENTRY_ADD_PMF_TCAM_ENTRY_ADD_TCAM_DATA_PRIORITY_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_TCAM_ENTRY_ADD_PMF_TCAM_ENTRY_ADD_TCAM_DATA_PRIORITY_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tcam_data.priority = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_ENTRY_ADD_PMF_TCAM_ENTRY_ADD_TCAM_DATA_KEY_PGM_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_TCAM_ENTRY_ADD_PMF_TCAM_ENTRY_ADD_TCAM_DATA_KEY_PGM_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tcam_data.key.pmf_pgm_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_ENTRY_ADD_PMF_TCAM_ENTRY_ADD_TCAM_DATA_KEY_FORMAT_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PMF_TCAM_ENTRY_ADD_PMF_TCAM_ENTRY_ADD_TCAM_DATA_KEY_FORMAT_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tcam_data.key.format.type = SOC_PB_TCAM_KEY_FORMAT_TYPE_PMF;
    prm_tcam_data.key.format.pmf = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_ENTRY_ADD_PMF_TCAM_ENTRY_ADD_TCAM_DATA_KEY_DATA_MASK_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_TCAM_ENTRY_ADD_PMF_TCAM_ENTRY_ADD_TCAM_DATA_KEY_DATA_MASK_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_mask_index = (uint32)param_val->value.ulong_value;
  } 

  if(prm_mask_index != 0xFFFFFFFF)          
  { 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_ENTRY_ADD_PMF_TCAM_ENTRY_ADD_TCAM_DATA_KEY_DATA_MASK_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_TCAM_ENTRY_ADD_PMF_TCAM_ENTRY_ADD_TCAM_DATA_KEY_DATA_MASK_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tcam_data.key.data.mask[ prm_mask_index] = (uint32)param_val->value.ulong_value;
  } 

  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_ENTRY_ADD_PMF_TCAM_ENTRY_ADD_TCAM_DATA_KEY_DATA_VAL_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_TCAM_ENTRY_ADD_PMF_TCAM_ENTRY_ADD_TCAM_DATA_KEY_DATA_VAL_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_val_index = (uint32)param_val->value.ulong_value;
  } 

  if(prm_val_index != 0xFFFFFFFF)          
  { 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_ENTRY_ADD_PMF_TCAM_ENTRY_ADD_TCAM_DATA_KEY_DATA_VAL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_TCAM_ENTRY_ADD_PMF_TCAM_ENTRY_ADD_TCAM_DATA_KEY_DATA_VAL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tcam_data.key.data.val[ prm_val_index] = (uint32)param_val->value.ulong_value;
  } 

  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_ENTRY_ADD_PMF_TCAM_ENTRY_ADD_TCAM_DATA_KEY_SIZE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PMF_TCAM_ENTRY_ADD_PMF_TCAM_ENTRY_ADD_TCAM_DATA_KEY_SIZE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tcam_data.key.size = param_val->numeric_equivalent;
  } 


  /* Call function */
  ret = soc_pb_pmf_tcam_entry_add_unsafe(
          unit,
          &prm_entry_ndx,
          &prm_tcam_data,
          TRUE,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_tcam_entry_add - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_tcam_entry_add");   
    goto exit; 
  } 

  soc_sand_os_printf("success: %s\n\r",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pmf_tcam_entry_get (section pmf_low_level)
 ********************************************************************/
int 
  ui_pb_api_pmf_low_level_pmf_tcam_entry_get_unsafe(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_TMC_PMF_TCAM_ENTRY_ID   
    prm_entry_ndx;
  SOC_TMC_PMF_TCAM_DATA   
    prm_tcam_data;
  uint8   
    prm_is_found;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_pmf_low_level"); 
  soc_sand_proc_name = "soc_ppd_pmf_tcam_entry_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_TMC_PMF_TCAM_ENTRY_ID_clear(&prm_entry_ndx);
  SOC_TMC_PMF_TCAM_DATA_clear(&prm_tcam_data);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_ENTRY_GET_PMF_TCAM_ENTRY_GET_ENTRY_NDX_ENTRY_ID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_TCAM_ENTRY_GET_PMF_TCAM_ENTRY_GET_ENTRY_NDX_ENTRY_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_entry_ndx.entry_id = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_ENTRY_GET_PMF_TCAM_ENTRY_GET_ENTRY_NDX_DB_ID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_TCAM_ENTRY_GET_PMF_TCAM_ENTRY_GET_ENTRY_NDX_DB_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_entry_ndx.db_id = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter entry_ndx after pmf_tcam_entry_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_pmf_tcam_entry_get_unsafe(
          unit,
          &prm_entry_ndx,
          &prm_tcam_data,
          &prm_is_found
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_tcam_entry_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_tcam_entry_get");   
    goto exit; 
  } 

  SOC_TMC_PMF_TCAM_DATA_print(&prm_tcam_data);

  soc_sand_os_printf("is_found: %u\n\r",prm_is_found);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pmf_tcam_entry_remove (section pmf_low_level)
 ********************************************************************/
int 
  ui_pb_api_pmf_low_level_pmf_tcam_entry_remove(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_TMC_PMF_TCAM_ENTRY_ID   
    prm_entry_ndx;
  uint8
    found;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_pmf_low_level"); 
  soc_sand_proc_name = "soc_ppd_pmf_tcam_entry_remove"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_TMC_PMF_TCAM_ENTRY_ID_clear(&prm_entry_ndx);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_ENTRY_REMOVE_PMF_TCAM_ENTRY_REMOVE_ENTRY_NDX_ENTRY_ID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_TCAM_ENTRY_REMOVE_PMF_TCAM_ENTRY_REMOVE_ENTRY_NDX_ENTRY_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_entry_ndx.entry_id = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_ENTRY_REMOVE_PMF_TCAM_ENTRY_REMOVE_ENTRY_NDX_DB_ID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_TCAM_ENTRY_REMOVE_PMF_TCAM_ENTRY_REMOVE_ENTRY_NDX_DB_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_entry_ndx.db_id = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter entry_ndx after pmf_tcam_entry_remove***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_pmf_tcam_entry_remove_unsafe(
          unit,
          &prm_entry_ndx,
          &found
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_tcam_entry_remove - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_tcam_entry_remove");   
    goto exit; 
  } 


  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pmf_tcam_key_clear (section pmf_low_level)
 ********************************************************************/
int 
  ui_pb_api_pmf_low_level_pmf_tcam_key_clear(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_TCAM_KEY_FORMAT   
    prm_key_format;
  SOC_TMC_TCAM_KEY   
    prm_tcam_key;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_pmf_low_level"); 
  soc_sand_proc_name = "soc_ppd_pmf_tcam_key_clear"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_TMC_TCAM_KEY_clear(&prm_tcam_key);
  SOC_PB_TCAM_KEY_FORMAT_clear(&prm_key_format);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_KEY_CLEAR_PMF_TCAM_KEY_CLEAR_KEY_FORMAT_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PMF_TCAM_KEY_CLEAR_PMF_TCAM_KEY_CLEAR_KEY_FORMAT_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key_format.type = SOC_PB_TCAM_KEY_FORMAT_TYPE_PMF;
    prm_key_format.pmf = param_val->numeric_equivalent;
  } 


  /* Call function */
  ret = soc_pb_tcam_key_clear_unsafe(
          unit,
          &prm_key_format,
          &prm_tcam_key
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_tcam_key_clear - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_tcam_key_clear");   
    goto exit; 
  } 

  SOC_TMC_TCAM_KEY_print(&prm_tcam_key);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pmf_tcam_key_val_set (section pmf_low_level)
 ********************************************************************/
int 
  ui_pb_api_pmf_low_level_pmf_tcam_key_val_set_unsafe(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32 
    prm_val_index = 0xFFFFFFFF;  
  SOC_TMC_TCAM_KEY   
    prm_tcam_key;
  SOC_TMC_TCAM_KEY_FLD_TYPE   
    prm_fld_type_ndx;
  SOC_TMC_TCAM_KEY_FLD_VAL   
    prm_fld_val;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_pmf_low_level"); 
  soc_sand_proc_name = "soc_ppd_pmf_tcam_key_val_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_TMC_TCAM_KEY_clear(&prm_tcam_key);
  prm_tcam_key.format.type = SOC_PB_TCAM_KEY_FORMAT_TYPE_PMF;
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_KEY_VAL_SET_PMF_TCAM_KEY_VAL_SET_TCAM_KEY_FORMAT_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PMF_TCAM_KEY_VAL_SET_PMF_TCAM_KEY_VAL_SET_TCAM_KEY_FORMAT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tcam_key.format.pmf = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_KEY_VAL_SET_PMF_TCAM_KEY_VAL_SET_TCAM_KEY_SIZE_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PMF_TCAM_KEY_VAL_SET_PMF_TCAM_KEY_VAL_SET_TCAM_KEY_SIZE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tcam_key.size = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter tcam_key after pmf_tcam_key_val_set***", TRUE); 
    goto exit; 
  } 

  SOC_TMC_TCAM_KEY_FLD_TYPE_clear(&(prm_tcam_key.format), &prm_fld_type_ndx);
  SOC_TMC_TCAM_KEY_FLD_VAL_clear(&prm_fld_val);
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_KEY_VAL_SET_PMF_TCAM_KEY_VAL_SET_FLD_TYPE_NDX_A_B_PMF_PGM_ID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_TCAM_KEY_VAL_SET_PMF_TCAM_KEY_VAL_SET_FLD_TYPE_NDX_A_B_PMF_PGM_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_fld_type_ndx.a_b.pmf_pgm_id = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_KEY_VAL_SET_PMF_TCAM_KEY_VAL_SET_FLD_TYPE_NDX_A_B_SUB_HEADER_OFFSET_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_TCAM_KEY_VAL_SET_PMF_TCAM_KEY_VAL_SET_FLD_TYPE_NDX_A_B_SUB_HEADER_OFFSET_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_fld_type_ndx.a_b.sub_header_offset = (int32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_KEY_VAL_SET_PMF_TCAM_KEY_VAL_SET_FLD_TYPE_NDX_A_B_SUB_HEADER_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PMF_TCAM_KEY_VAL_SET_PMF_TCAM_KEY_VAL_SET_FLD_TYPE_NDX_A_B_SUB_HEADER_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fld_type_ndx.a_b.sub_header = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_KEY_VAL_SET_PMF_TCAM_KEY_VAL_SET_FLD_TYPE_NDX_A_B_IRPP_FLD_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PMF_TCAM_KEY_VAL_SET_PMF_TCAM_KEY_VAL_SET_FLD_TYPE_NDX_A_B_IRPP_FLD_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fld_type_ndx.a_b.irpp_fld = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_KEY_VAL_SET_PMF_TCAM_KEY_VAL_SET_FLD_TYPE_NDX_A_B_LOC_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PMF_TCAM_KEY_VAL_SET_PMF_TCAM_KEY_VAL_SET_FLD_TYPE_NDX_A_B_LOC_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fld_type_ndx.a_b.loc = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_KEY_VAL_SET_PMF_TCAM_KEY_VAL_SET_FLD_TYPE_NDX_L3_IPV6_FLD_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PMF_TCAM_KEY_VAL_SET_PMF_TCAM_KEY_VAL_SET_FLD_TYPE_NDX_L3_IPV6_FLD_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fld_type_ndx.l3.ipv6_fld = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_KEY_VAL_SET_PMF_TCAM_KEY_VAL_SET_FLD_TYPE_NDX_L3_IPV4_FLD_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PMF_TCAM_KEY_VAL_SET_PMF_TCAM_KEY_VAL_SET_FLD_TYPE_NDX_L3_IPV4_FLD_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fld_type_ndx.l3.ipv4_fld = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_KEY_VAL_SET_PMF_TCAM_KEY_VAL_SET_FLD_TYPE_NDX_L3_MODE_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PMF_TCAM_KEY_VAL_SET_PMF_TCAM_KEY_VAL_SET_FLD_TYPE_NDX_L3_MODE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fld_type_ndx.l3.mode = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_KEY_VAL_SET_PMF_TCAM_KEY_VAL_SET_FLD_TYPE_NDX_L2_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PMF_TCAM_KEY_VAL_SET_PMF_TCAM_KEY_VAL_SET_FLD_TYPE_NDX_L2_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fld_type_ndx.l2 = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter fld_type_ndx after pmf_tcam_key_val_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_tcam_key_val_get_unsafe(
          unit,
          &prm_tcam_key,
          &prm_fld_type_ndx,
          &prm_fld_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_tcam_key_val_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_tcam_key_val_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_KEY_VAL_SET_PMF_TCAM_KEY_VAL_SET_FLD_VAL_VAL_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_TCAM_KEY_VAL_SET_PMF_TCAM_KEY_VAL_SET_FLD_VAL_VAL_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_val_index = (uint32)param_val->value.ulong_value;
  } 

  if(prm_val_index != 0xFFFFFFFF)          
  { 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_KEY_VAL_SET_PMF_TCAM_KEY_VAL_SET_FLD_VAL_VAL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_TCAM_KEY_VAL_SET_PMF_TCAM_KEY_VAL_SET_FLD_VAL_VAL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fld_val.val[ prm_val_index] = (uint32)param_val->value.ulong_value;
  } 

  }   


  /* Call function */
  ret = soc_pb_tcam_key_val_set_unsafe(
          unit,
          &prm_tcam_key,
          &prm_fld_type_ndx,
          &prm_fld_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_tcam_key_val_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_tcam_key_val_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pmf_tcam_key_val_get (section pmf_low_level)
 ********************************************************************/
int 
  ui_pb_api_pmf_low_level_pmf_tcam_key_val_get_unsafe(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_TMC_TCAM_KEY   
    prm_tcam_key;
  SOC_TMC_TCAM_KEY_FLD_TYPE   
    prm_fld_type_ndx;
  SOC_TMC_TCAM_KEY_FLD_VAL   
    prm_fld_val;
  uint32 
    prm_mask_index = 0xFFFFFFFF;  
  uint32 
    prm_val_index = 0xFFFFFFFF;  

  UI_MACROS_INIT_FUNCTION("ui_pb_api_pmf_low_level"); 
  soc_sand_proc_name = "soc_ppd_pmf_tcam_key_val_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_TMC_TCAM_KEY_clear(&prm_tcam_key);
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_KEY_VAL_GET_PMF_TCAM_KEY_VAL_GET_TCAM_KEY_PGM_ID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_TCAM_KEY_VAL_GET_PMF_TCAM_KEY_VAL_GET_TCAM_KEY_PGM_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_tcam_key.pmf_pgm_id = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_KEY_VAL_GET_PMF_TCAM_KEY_VAL_GET_TCAM_KEY_FORMAT_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PMF_TCAM_KEY_VAL_GET_PMF_TCAM_KEY_VAL_GET_TCAM_KEY_FORMAT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tcam_key.format.type = SOC_PB_TCAM_KEY_FORMAT_TYPE_PMF;
    prm_tcam_key.format.pmf = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_KEY_VAL_GET_PMF_TCAM_KEY_VAL_GET_TCAM_KEY_DATA_MASK_INDEX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_TCAM_KEY_VAL_GET_PMF_TCAM_KEY_VAL_GET_TCAM_KEY_DATA_MASK_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_mask_index = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_KEY_VAL_GET_PMF_TCAM_KEY_VAL_GET_TCAM_KEY_DATA_MASK_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_TCAM_KEY_VAL_GET_PMF_TCAM_KEY_VAL_GET_TCAM_KEY_DATA_MASK_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_tcam_key.data.mask[ prm_mask_index] = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_KEY_VAL_GET_PMF_TCAM_KEY_VAL_GET_TCAM_KEY_DATA_VAL_INDEX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_TCAM_KEY_VAL_GET_PMF_TCAM_KEY_VAL_GET_TCAM_KEY_DATA_VAL_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_val_index = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_KEY_VAL_GET_PMF_TCAM_KEY_VAL_GET_TCAM_KEY_DATA_VAL_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_TCAM_KEY_VAL_GET_PMF_TCAM_KEY_VAL_GET_TCAM_KEY_DATA_VAL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_tcam_key.data.val[ prm_val_index] = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_KEY_VAL_GET_PMF_TCAM_KEY_VAL_GET_TCAM_KEY_SIZE_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PMF_TCAM_KEY_VAL_GET_PMF_TCAM_KEY_VAL_GET_TCAM_KEY_SIZE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tcam_key.size = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter tcam_key after pmf_tcam_key_val_get***", TRUE); 
    goto exit; 
  } 

  SOC_TMC_TCAM_KEY_FLD_TYPE_clear(&(prm_tcam_key.format), &prm_fld_type_ndx);
  SOC_TMC_TCAM_KEY_FLD_VAL_clear(&prm_fld_val);

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_KEY_VAL_GET_PMF_TCAM_KEY_VAL_GET_FLD_TYPE_NDX_A_B_PMF_PGM_ID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_TCAM_KEY_VAL_GET_PMF_TCAM_KEY_VAL_GET_FLD_TYPE_NDX_A_B_PMF_PGM_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_fld_type_ndx.a_b.pmf_pgm_id = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_KEY_VAL_GET_PMF_TCAM_KEY_VAL_GET_FLD_TYPE_NDX_A_B_SUB_HEADER_OFFSET_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_TCAM_KEY_VAL_GET_PMF_TCAM_KEY_VAL_GET_FLD_TYPE_NDX_A_B_SUB_HEADER_OFFSET_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_fld_type_ndx.a_b.sub_header_offset = (int32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_KEY_VAL_GET_PMF_TCAM_KEY_VAL_GET_FLD_TYPE_NDX_A_B_SUB_HEADER_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PMF_TCAM_KEY_VAL_GET_PMF_TCAM_KEY_VAL_GET_FLD_TYPE_NDX_A_B_SUB_HEADER_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fld_type_ndx.a_b.sub_header = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_KEY_VAL_GET_PMF_TCAM_KEY_VAL_GET_FLD_TYPE_NDX_A_B_IRPP_FLD_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PMF_TCAM_KEY_VAL_GET_PMF_TCAM_KEY_VAL_GET_FLD_TYPE_NDX_A_B_IRPP_FLD_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fld_type_ndx.a_b.irpp_fld = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_KEY_VAL_GET_PMF_TCAM_KEY_VAL_GET_FLD_TYPE_NDX_A_B_LOC_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PMF_TCAM_KEY_VAL_GET_PMF_TCAM_KEY_VAL_GET_FLD_TYPE_NDX_A_B_LOC_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fld_type_ndx.a_b.loc = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_KEY_VAL_GET_PMF_TCAM_KEY_VAL_GET_FLD_TYPE_NDX_L3_IPV6_FLD_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PMF_TCAM_KEY_VAL_GET_PMF_TCAM_KEY_VAL_GET_FLD_TYPE_NDX_L3_IPV6_FLD_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fld_type_ndx.l3.ipv6_fld = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_KEY_VAL_GET_PMF_TCAM_KEY_VAL_GET_FLD_TYPE_NDX_L3_IPV4_FLD_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PMF_TCAM_KEY_VAL_GET_PMF_TCAM_KEY_VAL_GET_FLD_TYPE_NDX_L3_IPV4_FLD_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fld_type_ndx.l3.ipv4_fld = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_KEY_VAL_GET_PMF_TCAM_KEY_VAL_GET_FLD_TYPE_NDX_L3_MODE_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PMF_TCAM_KEY_VAL_GET_PMF_TCAM_KEY_VAL_GET_FLD_TYPE_NDX_L3_MODE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fld_type_ndx.l3.mode = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_KEY_VAL_GET_PMF_TCAM_KEY_VAL_GET_FLD_TYPE_NDX_L2_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PMF_TCAM_KEY_VAL_GET_PMF_TCAM_KEY_VAL_GET_FLD_TYPE_NDX_L2_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fld_type_ndx.l2 = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter fld_type_ndx after pmf_tcam_key_val_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_tcam_key_val_get_unsafe(
          unit,
          &prm_tcam_key,
          &prm_fld_type_ndx,
          &prm_fld_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_tcam_key_val_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_tcam_key_val_get");   
    goto exit; 
  } 

  SOC_TMC_TCAM_KEY_FLD_VAL_print(&prm_fld_val);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pmf_tcam_key_masked_val_set (section pmf_low_level)
 ********************************************************************/
int 
  ui_pb_api_pmf_low_level_pmf_tcam_key_masked_val_set_unsafe(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32 
    prm_val_index = 0xFFFFFFFF;  
  SOC_TMC_TCAM_KEY   
    prm_tcam_key;
  SOC_TMC_TCAM_KEY_FLD_TYPE   
    prm_fld_type_ndx;
  SOC_TMC_TCAM_KEY_FLD_VAL   
    prm_fld_val;
  SOC_TMC_TCAM_KEY_FLD_VAL   
    prm_mask_val;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_pmf_low_level"); 
  soc_sand_proc_name = "soc_ppd_pmf_tcam_key_masked_val_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_TMC_TCAM_KEY_clear(&prm_tcam_key);
  SOC_TMC_TCAM_KEY_FLD_VAL_clear(&prm_fld_val);
  SOC_TMC_TCAM_KEY_FLD_VAL_clear(&prm_mask_val);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_KEY_MASKED_VAL_SET_PMF_TCAM_KEY_MASKED_VAL_SET_TCAM_KEY_FORMAT_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_TCAM_KEY_MASKED_VAL_SET_PMF_TCAM_KEY_MASKED_VAL_SET_TCAM_KEY_FORMAT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tcam_key.format.type = SOC_PB_TCAM_KEY_FORMAT_TYPE_PMF;
    prm_tcam_key.format.pmf = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_KEY_MASKED_VAL_SET_PMF_TCAM_KEY_MASKED_VAL_SET_TCAM_KEY_SIZE_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_TCAM_KEY_MASKED_VAL_SET_PMF_TCAM_KEY_MASKED_VAL_SET_TCAM_KEY_SIZE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tcam_key.size = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter tcam_key after pmf_tcam_key_masked_val_set***", TRUE); 
    goto exit; 
  } 

  SOC_TMC_TCAM_KEY_FLD_TYPE_clear(&(prm_tcam_key.format), &prm_fld_type_ndx);
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_KEY_MASKED_VAL_SET_PMF_TCAM_KEY_MASKED_VAL_SET_FLD_TYPE_NDX_A_B_PMF_PGM_ID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_KEY_MASKED_VAL_SET_PMF_TCAM_KEY_MASKED_VAL_SET_FLD_TYPE_NDX_A_B_PMF_PGM_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_fld_type_ndx.a_b.pmf_pgm_id = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_KEY_MASKED_VAL_SET_PMF_TCAM_KEY_MASKED_VAL_SET_FLD_TYPE_NDX_A_B_SUB_HEADER_OFFSET_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_KEY_MASKED_VAL_SET_PMF_TCAM_KEY_MASKED_VAL_SET_FLD_TYPE_NDX_A_B_SUB_HEADER_OFFSET_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_fld_type_ndx.a_b.sub_header_offset = (int32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_KEY_MASKED_VAL_SET_PMF_TCAM_KEY_MASKED_VAL_SET_FLD_TYPE_NDX_A_B_SUB_HEADER_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_TCAM_KEY_MASKED_VAL_SET_PMF_TCAM_KEY_MASKED_VAL_SET_FLD_TYPE_NDX_A_B_SUB_HEADER_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fld_type_ndx.a_b.sub_header = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_KEY_MASKED_VAL_SET_PMF_TCAM_KEY_MASKED_VAL_SET_FLD_TYPE_NDX_A_B_IRPP_FLD_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_TCAM_KEY_MASKED_VAL_SET_PMF_TCAM_KEY_MASKED_VAL_SET_FLD_TYPE_NDX_A_B_IRPP_FLD_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fld_type_ndx.a_b.irpp_fld = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_KEY_MASKED_VAL_SET_PMF_TCAM_KEY_MASKED_VAL_SET_FLD_TYPE_NDX_A_B_LOC_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_TCAM_KEY_MASKED_VAL_SET_PMF_TCAM_KEY_MASKED_VAL_SET_FLD_TYPE_NDX_A_B_LOC_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fld_type_ndx.a_b.loc = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_KEY_MASKED_VAL_SET_PMF_TCAM_KEY_MASKED_VAL_SET_FLD_TYPE_NDX_L3_IPV6_FLD_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_TCAM_KEY_MASKED_VAL_SET_PMF_TCAM_KEY_MASKED_VAL_SET_FLD_TYPE_NDX_L3_IPV6_FLD_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fld_type_ndx.l3.ipv6_fld = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_KEY_MASKED_VAL_SET_PMF_TCAM_KEY_MASKED_VAL_SET_FLD_TYPE_NDX_L3_IPV4_FLD_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_TCAM_KEY_MASKED_VAL_SET_PMF_TCAM_KEY_MASKED_VAL_SET_FLD_TYPE_NDX_L3_IPV4_FLD_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fld_type_ndx.l3.ipv4_fld = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_KEY_MASKED_VAL_SET_PMF_TCAM_KEY_MASKED_VAL_SET_FLD_TYPE_NDX_L3_MODE_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_TCAM_KEY_MASKED_VAL_SET_PMF_TCAM_KEY_MASKED_VAL_SET_FLD_TYPE_NDX_L3_MODE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fld_type_ndx.l3.mode = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_KEY_MASKED_VAL_SET_PMF_TCAM_KEY_MASKED_VAL_SET_FLD_TYPE_NDX_L2_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_TCAM_KEY_MASKED_VAL_SET_PMF_TCAM_KEY_MASKED_VAL_SET_FLD_TYPE_NDX_L2_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fld_type_ndx.l2 = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter fld_type_ndx after pmf_tcam_key_masked_val_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_tcam_key_masked_val_get_unsafe(
          unit,
          &prm_tcam_key,
          &prm_fld_type_ndx,
          &prm_fld_val,
          &prm_mask_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_tcam_key_masked_val_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_tcam_key_masked_val_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_KEY_MASKED_VAL_SET_PMF_TCAM_KEY_MASKED_VAL_SET_FLD_VAL_VAL_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_KEY_MASKED_VAL_SET_PMF_TCAM_KEY_MASKED_VAL_SET_FLD_VAL_VAL_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_val_index = (uint32)param_val->value.ulong_value;
  } 

  if(prm_val_index != 0xFFFFFFFF)          
  { 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_KEY_MASKED_VAL_SET_PMF_TCAM_KEY_MASKED_VAL_SET_FLD_VAL_VAL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_KEY_MASKED_VAL_SET_PMF_TCAM_KEY_MASKED_VAL_SET_FLD_VAL_VAL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fld_val.val[ prm_val_index] = (uint32)param_val->value.ulong_value;
  } 

  }   

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_KEY_MASKED_VAL_SET_PMF_TCAM_KEY_MASKED_VAL_SET_MASK_VAL_VAL_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_KEY_MASKED_VAL_SET_PMF_TCAM_KEY_MASKED_VAL_SET_MASK_VAL_VAL_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_val_index = (uint32)param_val->value.ulong_value;
  } 

  if(prm_val_index != 0xFFFFFFFF)          
  { 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_KEY_MASKED_VAL_SET_PMF_TCAM_KEY_MASKED_VAL_SET_MASK_VAL_VAL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_KEY_MASKED_VAL_SET_PMF_TCAM_KEY_MASKED_VAL_SET_MASK_VAL_VAL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_mask_val.val[ prm_val_index] = (uint32)param_val->value.ulong_value;
  } 

  }   


  /* Call function */
  ret = soc_pb_tcam_key_masked_val_set_unsafe(
          unit,
          &prm_tcam_key,
          &prm_fld_type_ndx,
          &prm_fld_val,
          &prm_mask_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_tcam_key_masked_val_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_tcam_key_masked_val_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pmf_tcam_key_masked_val_get (section pmf_low_level)
 ********************************************************************/
int 
  ui_pb_api_pmf_low_level_pmf_tcam_key_masked_val_get_unsafe(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_TMC_TCAM_KEY   
    prm_tcam_key;
  SOC_TMC_TCAM_KEY_FLD_TYPE   
    prm_fld_type_ndx;
  SOC_TMC_TCAM_KEY_FLD_VAL   
    prm_fld_val;
  SOC_TMC_TCAM_KEY_FLD_VAL   
    prm_mask_val;
  uint32 
    prm_mask_index = 0xFFFFFFFF;  
  uint32 
    prm_val_index = 0xFFFFFFFF;  

  UI_MACROS_INIT_FUNCTION("ui_pb_api_pmf_low_level"); 
  soc_sand_proc_name = "soc_ppd_pmf_tcam_key_masked_val_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_TMC_TCAM_KEY_clear(&prm_tcam_key);
  SOC_TMC_TCAM_KEY_FLD_VAL_clear(&prm_fld_val);
  SOC_TMC_TCAM_KEY_FLD_VAL_clear(&prm_mask_val);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_KEY_MASKED_VAL_GET_PMF_TCAM_KEY_MASKED_VAL_GET_TCAM_KEY_PGM_ID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_KEY_MASKED_VAL_GET_PMF_TCAM_KEY_MASKED_VAL_GET_TCAM_KEY_PGM_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_tcam_key.pmf_pgm_id = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_KEY_MASKED_VAL_GET_PMF_TCAM_KEY_MASKED_VAL_GET_TCAM_KEY_FORMAT_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_TCAM_KEY_MASKED_VAL_GET_PMF_TCAM_KEY_MASKED_VAL_GET_TCAM_KEY_FORMAT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tcam_key.format.type = SOC_PB_TCAM_KEY_FORMAT_TYPE_PMF;
    prm_tcam_key.format.pmf = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_KEY_MASKED_VAL_GET_PMF_TCAM_KEY_MASKED_VAL_GET_TCAM_KEY_DATA_MASK_INDEX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_KEY_MASKED_VAL_GET_PMF_TCAM_KEY_MASKED_VAL_GET_TCAM_KEY_DATA_MASK_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_mask_index = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_KEY_MASKED_VAL_GET_PMF_TCAM_KEY_MASKED_VAL_GET_TCAM_KEY_DATA_MASK_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_KEY_MASKED_VAL_GET_PMF_TCAM_KEY_MASKED_VAL_GET_TCAM_KEY_DATA_MASK_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_tcam_key.data.mask[ prm_mask_index] = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_KEY_MASKED_VAL_GET_PMF_TCAM_KEY_MASKED_VAL_GET_TCAM_KEY_DATA_VAL_INDEX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_KEY_MASKED_VAL_GET_PMF_TCAM_KEY_MASKED_VAL_GET_TCAM_KEY_DATA_VAL_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_val_index = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_KEY_MASKED_VAL_GET_PMF_TCAM_KEY_MASKED_VAL_GET_TCAM_KEY_DATA_VAL_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_KEY_MASKED_VAL_GET_PMF_TCAM_KEY_MASKED_VAL_GET_TCAM_KEY_DATA_VAL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_tcam_key.data.val[ prm_val_index] = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_KEY_MASKED_VAL_GET_PMF_TCAM_KEY_MASKED_VAL_GET_TCAM_KEY_SIZE_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_TCAM_KEY_MASKED_VAL_GET_PMF_TCAM_KEY_MASKED_VAL_GET_TCAM_KEY_SIZE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tcam_key.size = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter tcam_key after pmf_tcam_key_masked_val_get***", TRUE); 
    goto exit; 
  } 

  SOC_TMC_TCAM_KEY_FLD_TYPE_clear(&(prm_tcam_key.format), &prm_fld_type_ndx);
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_KEY_MASKED_VAL_GET_PMF_TCAM_KEY_MASKED_VAL_GET_FLD_TYPE_NDX_A_B_PMF_PGM_ID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_KEY_MASKED_VAL_GET_PMF_TCAM_KEY_MASKED_VAL_GET_FLD_TYPE_NDX_A_B_PMF_PGM_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_fld_type_ndx.a_b.pmf_pgm_id = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_KEY_MASKED_VAL_GET_PMF_TCAM_KEY_MASKED_VAL_GET_FLD_TYPE_NDX_A_B_SUB_HEADER_OFFSET_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TCAM_KEY_MASKED_VAL_GET_PMF_TCAM_KEY_MASKED_VAL_GET_FLD_TYPE_NDX_A_B_SUB_HEADER_OFFSET_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_fld_type_ndx.a_b.sub_header_offset = (int32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_KEY_MASKED_VAL_GET_PMF_TCAM_KEY_MASKED_VAL_GET_FLD_TYPE_NDX_A_B_SUB_HEADER_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_TCAM_KEY_MASKED_VAL_GET_PMF_TCAM_KEY_MASKED_VAL_GET_FLD_TYPE_NDX_A_B_SUB_HEADER_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fld_type_ndx.a_b.sub_header = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_KEY_MASKED_VAL_GET_PMF_TCAM_KEY_MASKED_VAL_GET_FLD_TYPE_NDX_A_B_IRPP_FLD_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_TCAM_KEY_MASKED_VAL_GET_PMF_TCAM_KEY_MASKED_VAL_GET_FLD_TYPE_NDX_A_B_IRPP_FLD_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fld_type_ndx.a_b.irpp_fld = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_KEY_MASKED_VAL_GET_PMF_TCAM_KEY_MASKED_VAL_GET_FLD_TYPE_NDX_A_B_LOC_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_TCAM_KEY_MASKED_VAL_GET_PMF_TCAM_KEY_MASKED_VAL_GET_FLD_TYPE_NDX_A_B_LOC_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fld_type_ndx.a_b.loc = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_KEY_MASKED_VAL_GET_PMF_TCAM_KEY_MASKED_VAL_GET_FLD_TYPE_NDX_L3_IPV6_FLD_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_TCAM_KEY_MASKED_VAL_GET_PMF_TCAM_KEY_MASKED_VAL_GET_FLD_TYPE_NDX_L3_IPV6_FLD_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fld_type_ndx.l3.ipv6_fld = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_KEY_MASKED_VAL_GET_PMF_TCAM_KEY_MASKED_VAL_GET_FLD_TYPE_NDX_L3_IPV4_FLD_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_TCAM_KEY_MASKED_VAL_GET_PMF_TCAM_KEY_MASKED_VAL_GET_FLD_TYPE_NDX_L3_IPV4_FLD_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fld_type_ndx.l3.ipv4_fld = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_KEY_MASKED_VAL_GET_PMF_TCAM_KEY_MASKED_VAL_GET_FLD_TYPE_NDX_L3_MODE_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_TCAM_KEY_MASKED_VAL_GET_PMF_TCAM_KEY_MASKED_VAL_GET_FLD_TYPE_NDX_L3_MODE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fld_type_ndx.l3.mode = param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_KEY_MASKED_VAL_GET_PMF_TCAM_KEY_MASKED_VAL_GET_FLD_TYPE_NDX_L2_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_TCAM_KEY_MASKED_VAL_GET_PMF_TCAM_KEY_MASKED_VAL_GET_FLD_TYPE_NDX_L2_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fld_type_ndx.l2 = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter fld_type_ndx after pmf_tcam_key_masked_val_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_tcam_key_masked_val_get_unsafe(
          unit,
          &prm_tcam_key,
          &prm_fld_type_ndx,
          &prm_fld_val,
          &prm_mask_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_tcam_key_masked_val_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_tcam_key_masked_val_get");   
    goto exit; 
  } 

  SOC_TMC_TCAM_KEY_FLD_VAL_print(&prm_fld_val);

  SOC_TMC_TCAM_KEY_FLD_VAL_print(&prm_mask_val);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pmf_db_direct_tbl_key_src_set (section pmf_low_level)
 ********************************************************************/
int 
  ui_pb_api_pmf_low_level_pmf_db_direct_tbl_key_src_set_unsafe(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_TMC_PMF_LKP_PROFILE   
    prm_lkp_profile_ndx;
  SOC_TMC_PMF_DIRECT_TBL_KEY_SRC   
    prm_key_src;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_pmf_low_level"); 
  soc_sand_proc_name = "soc_ppd_pmf_db_direct_tbl_key_src_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_TMC_PMF_LKP_PROFILE_clear(&prm_lkp_profile_ndx);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DB_DIRECT_TBL_KEY_SRC_SET_PMF_DB_DIRECT_TBL_KEY_SRC_SET_LKP_PROFILE_NDX_CYCLE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_DB_DIRECT_TBL_KEY_SRC_SET_PMF_DB_DIRECT_TBL_KEY_SRC_SET_LKP_PROFILE_NDX_CYCLE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_lkp_profile_ndx.cycle_ndx = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DB_DIRECT_TBL_KEY_SRC_SET_PMF_DB_DIRECT_TBL_KEY_SRC_SET_LKP_PROFILE_NDX_ID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_DB_DIRECT_TBL_KEY_SRC_SET_PMF_DB_DIRECT_TBL_KEY_SRC_SET_LKP_PROFILE_NDX_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_lkp_profile_ndx.id = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter lkp_profile_ndx after pmf_db_direct_tbl_key_src_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_pmf_db_direct_tbl_key_src_get_unsafe(
          unit,
          &prm_lkp_profile_ndx,
          &prm_key_src
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_db_direct_tbl_key_src_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_db_direct_tbl_key_src_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DB_DIRECT_TBL_KEY_SRC_SET_PMF_DB_DIRECT_TBL_KEY_SRC_SET_KEY_SRC_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PMF_DB_DIRECT_TBL_KEY_SRC_SET_PMF_DB_DIRECT_TBL_KEY_SRC_SET_KEY_SRC_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_key_src = param_val->numeric_equivalent;
  } 


  /* Call function */
  ret = soc_pb_pmf_db_direct_tbl_key_src_set_unsafe(
          unit,
          &prm_lkp_profile_ndx,
          prm_key_src
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_db_direct_tbl_key_src_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_db_direct_tbl_key_src_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pmf_db_direct_tbl_key_src_get (section pmf_low_level)
 ********************************************************************/
int 
  ui_pb_api_pmf_low_level_pmf_db_direct_tbl_key_src_get_unsafe(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_TMC_PMF_LKP_PROFILE   
    prm_lkp_profile_ndx;
  SOC_TMC_PMF_DIRECT_TBL_KEY_SRC   
    prm_key_src;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_pmf_low_level"); 
  soc_sand_proc_name = "soc_ppd_pmf_db_direct_tbl_key_src_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_TMC_PMF_LKP_PROFILE_clear(&prm_lkp_profile_ndx);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DB_DIRECT_TBL_KEY_SRC_GET_PMF_DB_DIRECT_TBL_KEY_SRC_GET_LKP_PROFILE_NDX_CYCLE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_DB_DIRECT_TBL_KEY_SRC_GET_PMF_DB_DIRECT_TBL_KEY_SRC_GET_LKP_PROFILE_NDX_CYCLE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_lkp_profile_ndx.cycle_ndx = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DB_DIRECT_TBL_KEY_SRC_GET_PMF_DB_DIRECT_TBL_KEY_SRC_GET_LKP_PROFILE_NDX_ID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_DB_DIRECT_TBL_KEY_SRC_GET_PMF_DB_DIRECT_TBL_KEY_SRC_GET_LKP_PROFILE_NDX_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_lkp_profile_ndx.id = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter lkp_profile_ndx after pmf_db_direct_tbl_key_src_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_pmf_db_direct_tbl_key_src_get_unsafe(
          unit,
          &prm_lkp_profile_ndx,
          &prm_key_src
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_db_direct_tbl_key_src_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_db_direct_tbl_key_src_get");   
    goto exit; 
  } 

  soc_sand_os_printf("key_src: %s\n\r",SOC_TMC_PMF_DIRECT_TBL_KEY_SRC_to_string(prm_key_src));

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pmf_db_direct_tbl_entry_set (section pmf_low_level)
 ********************************************************************/
int 
  ui_pb_api_pmf_low_level_pmf_db_direct_tbl_entry_set_unsafe(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_entry_ndx;
  SOC_TMC_PMF_DIRECT_TBL_DATA   
    prm_data;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_pmf_low_level"); 
  soc_sand_proc_name = "soc_ppd_pmf_db_direct_tbl_entry_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_TMC_PMF_DIRECT_TBL_DATA_clear(&prm_data);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DB_DIRECT_TBL_ENTRY_SET_PMF_DB_DIRECT_TBL_ENTRY_SET_ENTRY_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_DB_DIRECT_TBL_ENTRY_SET_PMF_DB_DIRECT_TBL_ENTRY_SET_ENTRY_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_entry_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter entry_ndx after pmf_db_direct_tbl_entry_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_pmf_db_direct_tbl_entry_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_db_direct_tbl_entry_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_db_direct_tbl_entry_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DB_DIRECT_TBL_ENTRY_SET_PMF_DB_DIRECT_TBL_ENTRY_SET_DATA_VAL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_DB_DIRECT_TBL_ENTRY_SET_PMF_DB_DIRECT_TBL_ENTRY_SET_DATA_VAL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_data.val = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_pmf_db_direct_tbl_entry_set_unsafe(
          unit,
          prm_entry_ndx,
          &prm_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_db_direct_tbl_entry_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_db_direct_tbl_entry_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pmf_db_direct_tbl_entry_get (section pmf_low_level)
 ********************************************************************/
int 
  ui_pb_api_pmf_low_level_pmf_db_direct_tbl_entry_get_unsafe(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_entry_ndx;
  SOC_TMC_PMF_DIRECT_TBL_DATA   
    prm_data;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_pmf_low_level"); 
  soc_sand_proc_name = "soc_ppd_pmf_db_direct_tbl_entry_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_TMC_PMF_DIRECT_TBL_DATA_clear(&prm_data);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DB_DIRECT_TBL_ENTRY_GET_PMF_DB_DIRECT_TBL_ENTRY_GET_ENTRY_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_DB_DIRECT_TBL_ENTRY_GET_PMF_DB_DIRECT_TBL_ENTRY_GET_ENTRY_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_entry_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter entry_ndx after pmf_db_direct_tbl_entry_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_pmf_db_direct_tbl_entry_get_unsafe(
          unit,
          prm_entry_ndx,
          &prm_data
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_db_direct_tbl_entry_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_db_direct_tbl_entry_get");   
    goto exit; 
  } 

  SOC_TMC_PMF_DIRECT_TBL_DATA_print(&prm_data);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pmf_db_fem_input_set (section pmf_low_level)
 ********************************************************************/
int 
  ui_pb_api_pmf_low_level_pmf_db_fem_input_set_unsafe(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_TMC_PMF_LKP_PROFILE   
    prm_lkp_profile_ndx;
  uint32   
    prm_fem_ndx;
  SOC_TMC_PMF_FEM_INPUT_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_pmf_low_level"); 
  soc_sand_proc_name = "soc_ppd_pmf_db_fem_input_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_TMC_PMF_LKP_PROFILE_clear(&prm_lkp_profile_ndx);
  SOC_TMC_PMF_FEM_INPUT_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DB_FEM_INPUT_SET_PMF_DB_FEM_INPUT_SET_LKP_PROFILE_NDX_CYCLE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_DB_FEM_INPUT_SET_PMF_DB_FEM_INPUT_SET_LKP_PROFILE_NDX_CYCLE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_lkp_profile_ndx.cycle_ndx = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DB_FEM_INPUT_SET_PMF_DB_FEM_INPUT_SET_LKP_PROFILE_NDX_ID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_DB_FEM_INPUT_SET_PMF_DB_FEM_INPUT_SET_LKP_PROFILE_NDX_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_lkp_profile_ndx.id = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter lkp_profile_ndx after pmf_db_fem_input_set***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DB_FEM_INPUT_SET_PMF_DB_FEM_INPUT_SET_FEM_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_DB_FEM_INPUT_SET_PMF_DB_FEM_INPUT_SET_FEM_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_fem_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter fem_ndx after pmf_db_fem_input_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_pmf_db_fem_input_get_unsafe(
          unit,
          &prm_lkp_profile_ndx,
          prm_fem_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_db_fem_input_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_db_fem_input_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DB_FEM_INPUT_SET_PMF_DB_FEM_INPUT_SET_INFO_DB_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_DB_FEM_INPUT_SET_PMF_DB_FEM_INPUT_SET_INFO_DB_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.db_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DB_FEM_INPUT_SET_PMF_DB_FEM_INPUT_SET_INFO_SRC_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PMF_DB_FEM_INPUT_SET_PMF_DB_FEM_INPUT_SET_INFO_SRC_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.src = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DB_FEM_INPUT_SET_PMF_DB_FEM_INPUT_SET_INFO_PGM_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_DB_FEM_INPUT_SET_PMF_DB_FEM_INPUT_SET_INFO_PGM_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.pgm_id = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_pmf_db_fem_input_set_unsafe(
          unit,
          &prm_lkp_profile_ndx,
          prm_fem_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_db_fem_input_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_db_fem_input_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pmf_db_fem_input_get (section pmf_low_level)
 ********************************************************************/
int 
  ui_pb_api_pmf_low_level_pmf_db_fem_input_get_unsafe(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_TMC_PMF_LKP_PROFILE   
    prm_lkp_profile_ndx;
  uint32   
    prm_fem_ndx;
  SOC_TMC_PMF_FEM_INPUT_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_pmf_low_level"); 
  soc_sand_proc_name = "soc_ppd_pmf_db_fem_input_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_TMC_PMF_LKP_PROFILE_clear(&prm_lkp_profile_ndx);
  SOC_TMC_PMF_FEM_INPUT_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DB_FEM_INPUT_GET_PMF_DB_FEM_INPUT_GET_LKP_PROFILE_NDX_CYCLE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_DB_FEM_INPUT_GET_PMF_DB_FEM_INPUT_GET_LKP_PROFILE_NDX_CYCLE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_lkp_profile_ndx.cycle_ndx = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DB_FEM_INPUT_GET_PMF_DB_FEM_INPUT_GET_LKP_PROFILE_NDX_ID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_DB_FEM_INPUT_GET_PMF_DB_FEM_INPUT_GET_LKP_PROFILE_NDX_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_lkp_profile_ndx.id = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter lkp_profile_ndx after pmf_db_fem_input_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DB_FEM_INPUT_GET_PMF_DB_FEM_INPUT_GET_FEM_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_DB_FEM_INPUT_GET_PMF_DB_FEM_INPUT_GET_FEM_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_fem_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter fem_ndx after pmf_db_fem_input_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_pmf_db_fem_input_get_unsafe(
          unit,
          &prm_lkp_profile_ndx,
          prm_fem_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_db_fem_input_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_db_fem_input_get");   
    goto exit; 
  } 

  SOC_TMC_PMF_FEM_INPUT_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pmf_db_tag_select_set (section pmf_low_level)
 ********************************************************************/
int 
  ui_pb_api_pmf_low_level_pmf_db_tag_select_set_unsafe(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_tag_profile_ndx;
  SOC_TMC_PMF_TAG_TYPE   
    prm_tag_type_ndx;
  SOC_TMC_PMF_TAG_SRC_INFO   
    prm_tag_src;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_pmf_low_level"); 
  soc_sand_proc_name = "soc_ppd_pmf_db_tag_select_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_TMC_PMF_TAG_SRC_INFO_clear(&prm_tag_src);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DB_TAG_SELECT_SET_PMF_DB_TAG_SELECT_SET_TAG_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_DB_TAG_SELECT_SET_PMF_DB_TAG_SELECT_SET_TAG_PROFILE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_tag_profile_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter tag_profile_ndx after pmf_db_tag_select_set***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DB_TAG_SELECT_SET_PMF_DB_TAG_SELECT_SET_TAG_TYPE_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PMF_DB_TAG_SELECT_SET_PMF_DB_TAG_SELECT_SET_TAG_TYPE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tag_type_ndx = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter tag_type_ndx after pmf_db_tag_select_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_pmf_db_tag_select_get_unsafe(
          unit,
          prm_tag_profile_ndx,
          prm_tag_type_ndx,
          &prm_tag_src
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_db_tag_select_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_db_tag_select_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DB_TAG_SELECT_SET_PMF_DB_TAG_SELECT_SET_TAG_SRC_STAT_TAG_LSB_POSITION_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_DB_TAG_SELECT_SET_PMF_DB_TAG_SELECT_SET_TAG_SRC_STAT_TAG_LSB_POSITION_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tag_src.stat_tag_lsb_position = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DB_TAG_SELECT_SET_PMF_DB_TAG_SELECT_SET_TAG_SRC_VAL_SRC_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PMF_DB_TAG_SELECT_SET_PMF_DB_TAG_SELECT_SET_TAG_SRC_VAL_SRC_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tag_src.val_src = param_val->numeric_equivalent;
  } 


  /* Call function */
  ret = soc_pb_pmf_db_tag_select_set_unsafe(
          unit,
          prm_tag_profile_ndx,
          prm_tag_type_ndx,
          &prm_tag_src
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_db_tag_select_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_db_tag_select_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pmf_db_tag_select_get (section pmf_low_level)
 ********************************************************************/
int 
  ui_pb_api_pmf_low_level_pmf_db_tag_select_get_unsafe(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_tag_profile_ndx;
  SOC_TMC_PMF_TAG_TYPE   
    prm_tag_type_ndx;
  SOC_TMC_PMF_TAG_SRC_INFO   
    prm_tag_src;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_pmf_low_level"); 
  soc_sand_proc_name = "soc_ppd_pmf_db_tag_select_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_TMC_PMF_TAG_SRC_INFO_clear(&prm_tag_src);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DB_TAG_SELECT_GET_PMF_DB_TAG_SELECT_GET_TAG_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_DB_TAG_SELECT_GET_PMF_DB_TAG_SELECT_GET_TAG_PROFILE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_tag_profile_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter tag_profile_ndx after pmf_db_tag_select_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DB_TAG_SELECT_GET_PMF_DB_TAG_SELECT_GET_TAG_TYPE_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PMF_DB_TAG_SELECT_GET_PMF_DB_TAG_SELECT_GET_TAG_TYPE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tag_type_ndx = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter tag_type_ndx after pmf_db_tag_select_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_pmf_db_tag_select_get_unsafe(
          unit,
          prm_tag_profile_ndx,
          prm_tag_type_ndx,
          &prm_tag_src
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_db_tag_select_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_db_tag_select_get");   
    goto exit; 
  } 

  SOC_TMC_PMF_TAG_SRC_INFO_print(&prm_tag_src);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pmf_fem_select_bits_set (section pmf_low_level)
 ********************************************************************/
int 
  ui_pb_api_pmf_low_level_pmf_fem_select_bits_set_unsafe(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_TMC_PMF_FEM_NDX   
    prm_fem_ndx;
  uint32   
    prm_fem_pgm_ndx;
  SOC_TMC_PMF_FEM_SELECTED_BITS_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_pmf_low_level"); 
  soc_sand_proc_name = "soc_ppd_pmf_fem_select_bits_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_TMC_PMF_FEM_NDX_clear(&prm_fem_ndx);
  SOC_TMC_PMF_FEM_SELECTED_BITS_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_FEM_SELECT_BITS_SET_PMF_FEM_SELECT_BITS_SET_FEM_NDX_CYCLE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_FEM_SELECT_BITS_SET_PMF_FEM_SELECT_BITS_SET_FEM_NDX_CYCLE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_fem_ndx.cycle_ndx = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_FEM_SELECT_BITS_SET_PMF_FEM_SELECT_BITS_SET_FEM_NDX_ID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_FEM_SELECT_BITS_SET_PMF_FEM_SELECT_BITS_SET_FEM_NDX_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_fem_ndx.id = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter fem_ndx after pmf_fem_select_bits_set***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_FEM_SELECT_BITS_SET_PMF_FEM_SELECT_BITS_SET_FEM_PGM_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_FEM_SELECT_BITS_SET_PMF_FEM_SELECT_BITS_SET_FEM_PGM_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_fem_pgm_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter fem_pgm_ndx after pmf_fem_select_bits_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_pmf_fem_select_bits_get_unsafe(
          unit,
          &prm_fem_ndx,
          prm_fem_pgm_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_fem_select_bits_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_fem_select_bits_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_FEM_SELECT_BITS_SET_PMF_FEM_SELECT_BITS_SET_INFO_SEL_BIT_MSB_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_FEM_SELECT_BITS_SET_PMF_FEM_SELECT_BITS_SET_INFO_SEL_BIT_MSB_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.sel_bit_msb = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_pmf_fem_select_bits_set_unsafe(
          unit,
          &prm_fem_ndx,
          prm_fem_pgm_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_fem_select_bits_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_fem_select_bits_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pmf_fem_select_bits_get (section pmf_low_level)
 ********************************************************************/
int 
  ui_pb_api_pmf_low_level_pmf_fem_select_bits_get_unsafe(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_TMC_PMF_FEM_NDX   
    prm_fem_ndx;
  uint32   
    prm_fem_pgm_ndx;
  SOC_TMC_PMF_FEM_SELECTED_BITS_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_pmf_low_level"); 
  soc_sand_proc_name = "soc_ppd_pmf_fem_select_bits_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_TMC_PMF_FEM_NDX_clear(&prm_fem_ndx);
  SOC_TMC_PMF_FEM_SELECTED_BITS_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_FEM_SELECT_BITS_GET_PMF_FEM_SELECT_BITS_GET_FEM_NDX_CYCLE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_FEM_SELECT_BITS_GET_PMF_FEM_SELECT_BITS_GET_FEM_NDX_CYCLE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_fem_ndx.cycle_ndx = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_FEM_SELECT_BITS_GET_PMF_FEM_SELECT_BITS_GET_FEM_NDX_ID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_FEM_SELECT_BITS_GET_PMF_FEM_SELECT_BITS_GET_FEM_NDX_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_fem_ndx.id = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter fem_ndx after pmf_fem_select_bits_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_FEM_SELECT_BITS_GET_PMF_FEM_SELECT_BITS_GET_FEM_PGM_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_FEM_SELECT_BITS_GET_PMF_FEM_SELECT_BITS_GET_FEM_PGM_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_fem_pgm_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter fem_pgm_ndx after pmf_fem_select_bits_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_pmf_fem_select_bits_get_unsafe(
          unit,
          &prm_fem_ndx,
          prm_fem_pgm_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_fem_select_bits_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_fem_select_bits_get");   
    goto exit; 
  } 

  SOC_TMC_PMF_FEM_SELECTED_BITS_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pmf_fem_action_format_map_set (section pmf_low_level)
 ********************************************************************/
int 
  ui_pb_api_pmf_low_level_pmf_fem_action_format_map_set_unsafe(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_TMC_PMF_FEM_NDX   
    prm_fem_ndx;
  uint32   
    prm_fem_pgm_ndx;
  uint32   
    prm_selected_bits_ndx;
  SOC_TMC_PMF_FEM_ACTION_FORMAT_MAP_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_pmf_low_level"); 
  soc_sand_proc_name = "soc_ppd_pmf_fem_action_format_map_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_TMC_PMF_FEM_NDX_clear(&prm_fem_ndx);
  SOC_TMC_PMF_FEM_ACTION_FORMAT_MAP_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_FEM_ACTION_FORMAT_MAP_SET_PMF_FEM_ACTION_FORMAT_MAP_SET_FEM_NDX_CYCLE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_FEM_ACTION_FORMAT_MAP_SET_PMF_FEM_ACTION_FORMAT_MAP_SET_FEM_NDX_CYCLE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_fem_ndx.cycle_ndx = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_FEM_ACTION_FORMAT_MAP_SET_PMF_FEM_ACTION_FORMAT_MAP_SET_FEM_NDX_ID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_FEM_ACTION_FORMAT_MAP_SET_PMF_FEM_ACTION_FORMAT_MAP_SET_FEM_NDX_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_fem_ndx.id = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter fem_ndx after pmf_fem_action_format_map_set***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_FEM_ACTION_FORMAT_MAP_SET_PMF_FEM_ACTION_FORMAT_MAP_SET_FEM_PGM_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_FEM_ACTION_FORMAT_MAP_SET_PMF_FEM_ACTION_FORMAT_MAP_SET_FEM_PGM_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_fem_pgm_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter fem_pgm_ndx after pmf_fem_action_format_map_set***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_FEM_ACTION_FORMAT_MAP_SET_PMF_FEM_ACTION_FORMAT_MAP_SET_SELECTED_BITS_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_FEM_ACTION_FORMAT_MAP_SET_PMF_FEM_ACTION_FORMAT_MAP_SET_SELECTED_BITS_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_selected_bits_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter selected_bits_ndx after pmf_fem_action_format_map_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_pmf_fem_action_format_map_get_unsafe(
          unit,
          &prm_fem_ndx,
          prm_fem_pgm_ndx,
          prm_selected_bits_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_fem_action_format_map_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_fem_action_format_map_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_FEM_ACTION_FORMAT_MAP_SET_PMF_FEM_ACTION_FORMAT_MAP_SET_INFO_MAP_DATA_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_FEM_ACTION_FORMAT_MAP_SET_PMF_FEM_ACTION_FORMAT_MAP_SET_INFO_MAP_DATA_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.map_data = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_FEM_ACTION_FORMAT_MAP_SET_PMF_FEM_ACTION_FORMAT_MAP_SET_INFO_ACTION_FOMAT_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_FEM_ACTION_FORMAT_MAP_SET_PMF_FEM_ACTION_FORMAT_MAP_SET_INFO_ACTION_FOMAT_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.action_fomat_id = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_pmf_fem_action_format_map_set_unsafe(
          unit,
          &prm_fem_ndx,
          prm_fem_pgm_ndx,
          prm_selected_bits_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_fem_action_format_map_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_fem_action_format_map_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pmf_fem_action_format_map_get (section pmf_low_level)
 ********************************************************************/
int 
  ui_pb_api_pmf_low_level_pmf_fem_action_format_map_get_unsafe(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_TMC_PMF_FEM_NDX   
    prm_fem_ndx;
  uint32   
    prm_fem_pgm_ndx;
  uint32   
    prm_selected_bits_ndx;
  SOC_TMC_PMF_FEM_ACTION_FORMAT_MAP_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_pmf_low_level"); 
  soc_sand_proc_name = "soc_ppd_pmf_fem_action_format_map_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_TMC_PMF_FEM_NDX_clear(&prm_fem_ndx);
  SOC_TMC_PMF_FEM_ACTION_FORMAT_MAP_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_FEM_ACTION_FORMAT_MAP_GET_PMF_FEM_ACTION_FORMAT_MAP_GET_FEM_NDX_CYCLE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_FEM_ACTION_FORMAT_MAP_GET_PMF_FEM_ACTION_FORMAT_MAP_GET_FEM_NDX_CYCLE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_fem_ndx.cycle_ndx = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_FEM_ACTION_FORMAT_MAP_GET_PMF_FEM_ACTION_FORMAT_MAP_GET_FEM_NDX_ID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_FEM_ACTION_FORMAT_MAP_GET_PMF_FEM_ACTION_FORMAT_MAP_GET_FEM_NDX_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_fem_ndx.id = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter fem_ndx after pmf_fem_action_format_map_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_FEM_ACTION_FORMAT_MAP_GET_PMF_FEM_ACTION_FORMAT_MAP_GET_FEM_PGM_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_FEM_ACTION_FORMAT_MAP_GET_PMF_FEM_ACTION_FORMAT_MAP_GET_FEM_PGM_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_fem_pgm_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter fem_pgm_ndx after pmf_fem_action_format_map_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_FEM_ACTION_FORMAT_MAP_GET_PMF_FEM_ACTION_FORMAT_MAP_GET_SELECTED_BITS_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_FEM_ACTION_FORMAT_MAP_GET_PMF_FEM_ACTION_FORMAT_MAP_GET_SELECTED_BITS_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_selected_bits_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter selected_bits_ndx after pmf_fem_action_format_map_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_pmf_fem_action_format_map_get_unsafe(
          unit,
          &prm_fem_ndx,
          prm_fem_pgm_ndx,
          prm_selected_bits_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_fem_action_format_map_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_fem_action_format_map_get");   
    goto exit; 
  } 

  SOC_TMC_PMF_FEM_ACTION_FORMAT_MAP_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pmf_fem_action_format_set (section pmf_low_level)
 ********************************************************************/
int 
  ui_pb_api_pmf_low_level_pmf_fem_action_format_set_unsafe(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_TMC_PMF_FEM_NDX   
    prm_fem_ndx;
  uint32   
    prm_action_fomat_ndx;
  uint32 
    prm_bit_loc_index = 0xFFFFFFFF;  
  SOC_TMC_PMF_FEM_ACTION_FORMAT_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_pmf_low_level"); 
  soc_sand_proc_name = "soc_ppd_pmf_fem_action_format_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_TMC_PMF_FEM_NDX_clear(&prm_fem_ndx);
  SOC_TMC_PMF_FEM_ACTION_FORMAT_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_FEM_ACTION_FORMAT_SET_PMF_FEM_ACTION_FORMAT_SET_FEM_NDX_CYCLE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_FEM_ACTION_FORMAT_SET_PMF_FEM_ACTION_FORMAT_SET_FEM_NDX_CYCLE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_fem_ndx.cycle_ndx = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_FEM_ACTION_FORMAT_SET_PMF_FEM_ACTION_FORMAT_SET_FEM_NDX_ID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_FEM_ACTION_FORMAT_SET_PMF_FEM_ACTION_FORMAT_SET_FEM_NDX_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_fem_ndx.id = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter fem_ndx after pmf_fem_action_format_set***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_FEM_ACTION_FORMAT_SET_PMF_FEM_ACTION_FORMAT_SET_ACTION_FOMAT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_FEM_ACTION_FORMAT_SET_PMF_FEM_ACTION_FORMAT_SET_ACTION_FOMAT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_action_fomat_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter action_fomat_ndx after pmf_fem_action_format_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_pmf_fem_action_format_get_unsafe(
          unit,
          &prm_fem_ndx,
          prm_action_fomat_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_fem_action_format_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_fem_action_format_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_FEM_ACTION_FORMAT_SET_PMF_FEM_ACTION_FORMAT_SET_INFO_BASE_VALUE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_FEM_ACTION_FORMAT_SET_PMF_FEM_ACTION_FORMAT_SET_INFO_BASE_VALUE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.base_value = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_FEM_ACTION_FORMAT_SET_PMF_FEM_ACTION_FORMAT_SET_INFO_BIT_LOC_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_FEM_ACTION_FORMAT_SET_PMF_FEM_ACTION_FORMAT_SET_INFO_BIT_LOC_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_bit_loc_index = (uint32)param_val->value.ulong_value;
  } 

  if(prm_bit_loc_index != 0xFFFFFFFF)          
  { 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_FEM_ACTION_FORMAT_SET_PMF_FEM_ACTION_FORMAT_SET_INFO_BIT_LOC_VAL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_FEM_ACTION_FORMAT_SET_PMF_FEM_ACTION_FORMAT_SET_INFO_BIT_LOC_VAL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.bit_loc[ prm_bit_loc_index].val = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_FEM_ACTION_FORMAT_SET_PMF_FEM_ACTION_FORMAT_SET_INFO_BIT_LOC_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PMF_FEM_ACTION_FORMAT_SET_PMF_FEM_ACTION_FORMAT_SET_INFO_BIT_LOC_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.bit_loc[ prm_bit_loc_index].type = param_val->numeric_equivalent;
  } 

  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_FEM_ACTION_FORMAT_SET_PMF_FEM_ACTION_FORMAT_SET_INFO_SIZE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_FEM_ACTION_FORMAT_SET_PMF_FEM_ACTION_FORMAT_SET_INFO_SIZE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.size = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_FEM_ACTION_FORMAT_SET_PMF_FEM_ACTION_FORMAT_SET_INFO_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PMF_FEM_ACTION_FORMAT_SET_PMF_FEM_ACTION_FORMAT_SET_INFO_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.type = param_val->numeric_equivalent;
  } 


  /* Call function */
  ret = soc_pb_pmf_fem_action_format_set_unsafe(
          unit,
          &prm_fem_ndx,
          prm_action_fomat_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_fem_action_format_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_fem_action_format_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pmf_fem_action_format_get (section pmf_low_level)
 ********************************************************************/
int 
  ui_pb_api_pmf_low_level_pmf_fem_action_format_get_unsafe(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_TMC_PMF_FEM_NDX   
    prm_fem_ndx;
  uint32   
    prm_action_fomat_ndx;
  SOC_TMC_PMF_FEM_ACTION_FORMAT_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_pmf_low_level"); 
  soc_sand_proc_name = "soc_ppd_pmf_fem_action_format_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_TMC_PMF_FEM_NDX_clear(&prm_fem_ndx);
  SOC_TMC_PMF_FEM_ACTION_FORMAT_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_FEM_ACTION_FORMAT_GET_PMF_FEM_ACTION_FORMAT_GET_FEM_NDX_CYCLE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_FEM_ACTION_FORMAT_GET_PMF_FEM_ACTION_FORMAT_GET_FEM_NDX_CYCLE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_fem_ndx.cycle_ndx = (uint32)param_val->value.ulong_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_FEM_ACTION_FORMAT_GET_PMF_FEM_ACTION_FORMAT_GET_FEM_NDX_ID_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_FEM_ACTION_FORMAT_GET_PMF_FEM_ACTION_FORMAT_GET_FEM_NDX_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_fem_ndx.id = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter fem_ndx after pmf_fem_action_format_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_FEM_ACTION_FORMAT_GET_PMF_FEM_ACTION_FORMAT_GET_ACTION_FOMAT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_FEM_ACTION_FORMAT_GET_PMF_FEM_ACTION_FORMAT_GET_ACTION_FOMAT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_action_fomat_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter action_fomat_ndx after pmf_fem_action_format_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_pmf_fem_action_format_get_unsafe(
          unit,
          &prm_fem_ndx,
          prm_action_fomat_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_fem_action_format_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_fem_action_format_get");   
    goto exit; 
  } 

  SOC_TMC_PMF_FEM_ACTION_FORMAT_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pmf_pgm_selection_entry_set (section pmf_low_level)
 ********************************************************************/
int 
  ui_pb_api_pmf_low_level_pmf_pgm_selection_entry_set_unsafe(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_TMC_PMF_PGM_SELECTION_ENTRY   
    prm_entry_ndx;
  uint32 
    prm_is_pgm_valid_index = 0xFFFFFFFF;  
  SOC_TMC_PMF_PGM_VALIDITY_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_pmf_low_level"); 
  soc_sand_proc_name = "soc_ppd_pmf_pgm_selection_entry_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_TMC_PMF_PGM_SELECTION_ENTRY_clear(&prm_entry_ndx);
  SOC_TMC_PMF_PGM_VALIDITY_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SELECTION_ENTRY_SET_PMF_PGM_SELECTION_ENTRY_SET_ENTRY_NDX_TYPE_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PMF_PGM_SELECTION_ENTRY_SET_PMF_PGM_SELECTION_ENTRY_SET_ENTRY_NDX_TYPE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_entry_ndx.type = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter entry_ndx after pmf_pgm_selection_entry_set***", TRUE); 
    goto exit; 
  } 

  switch(prm_entry_ndx.type) 
  {
  case SOC_TMC_PMF_PGM_SEL_TYPE_EEI_OUTLIF_15_8:
    if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SELECTION_ENTRY_SET_PMF_PGM_SELECTION_ENTRY_SET_ENTRY_NDX_VAL_EEI_OUTLIF_15_8_ID,1))          
    {  
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_PGM_SELECTION_ENTRY_SET_PMF_PGM_SELECTION_ENTRY_SET_ENTRY_NDX_VAL_EEI_OUTLIF_15_8_ID);  
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
      prm_entry_ndx.val.eei_outlif_15_8 = (uint32)param_val->value.ulong_value;
    } 
    break;

 case SOC_TMC_PMF_PGM_SEL_TYPE_EEI_OUTLIF_7_0:
   if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SELECTION_ENTRY_SET_PMF_PGM_SELECTION_ENTRY_SET_ENTRY_NDX_VAL_EEI_OUTLIF_7_0_ID,1))          
   {  
     UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_PGM_SELECTION_ENTRY_SET_PMF_PGM_SELECTION_ENTRY_SET_ENTRY_NDX_VAL_EEI_OUTLIF_7_0_ID);  
     UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
     prm_entry_ndx.val.eei_outlif_7_0 = (uint32)param_val->value.ulong_value;
   } 
  break;

  case SOC_TMC_PMF_PGM_SEL_TYPE_SEM_NDX_7_0:
    if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SELECTION_ENTRY_SET_PMF_PGM_SELECTION_ENTRY_SET_ENTRY_NDX_VAL_SEM_7_0_NDX_ID,1))          
    {  
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_PGM_SELECTION_ENTRY_SET_PMF_PGM_SELECTION_ENTRY_SET_ENTRY_NDX_VAL_SEM_7_0_NDX_ID);  
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
      prm_entry_ndx.val.sem_7_0_ndx = (uint32)param_val->value.ulong_value;
    } 
    break;

  case SOC_TMC_PMF_PGM_SEL_TYPE_PFQ_SEM_NDX_13_8:
   if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SELECTION_ENTRY_SET_PMF_PGM_SELECTION_ENTRY_SET_ENTRY_NDX_VAL_PFQ_SEM_SEM_13_8_NDX_ID,1))          
   {  
     UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_PGM_SELECTION_ENTRY_SET_PMF_PGM_SELECTION_ENTRY_SET_ENTRY_NDX_VAL_PFQ_SEM_SEM_13_8_NDX_ID);  
     UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
     prm_entry_ndx.val.pfq_sem.sem_13_8_ndx = (uint32)param_val->value.ulong_value;
   } 
   if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SELECTION_ENTRY_SET_PMF_PGM_SELECTION_ENTRY_SET_ENTRY_NDX_VAL_PFQ_SEM_PFQ_ID,1))          
   {  
     UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_PGM_SELECTION_ENTRY_SET_PMF_PGM_SELECTION_ENTRY_SET_ENTRY_NDX_VAL_PFQ_SEM_PFQ_ID);  
     UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
     prm_entry_ndx.val.pfq_sem.pfq = (uint32)param_val->value.ulong_value;
   } 
   break;

  case SOC_TMC_PMF_PGM_SEL_TYPE_FWDING_TTC_CODE:
    if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SELECTION_ENTRY_SET_PMF_PGM_SELECTION_ENTRY_SET_ENTRY_NDX_VAL_FWD_TTC_TTC_ID,1))          
    {  
      UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PMF_PGM_SELECTION_ENTRY_SET_PMF_PGM_SELECTION_ENTRY_SET_ENTRY_NDX_VAL_FWD_TTC_TTC_ID);  
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
      prm_entry_ndx.val.fwd_ttc.ttc = param_val->numeric_equivalent;
    } 
    if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SELECTION_ENTRY_SET_PMF_PGM_SELECTION_ENTRY_SET_ENTRY_NDX_VAL_FWD_TTC_FWD_ID,1))          
    {  
      UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PMF_PGM_SELECTION_ENTRY_SET_PMF_PGM_SELECTION_ENTRY_SET_ENTRY_NDX_VAL_FWD_TTC_FWD_ID);  
      UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
      prm_entry_ndx.val.fwd_ttc.fwd = param_val->numeric_equivalent;
    } 
    break;

  case SOC_TMC_PMF_PGM_SEL_TYPE_LOOKUPS:
    if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SELECTION_ENTRY_SET_PMF_PGM_SELECTION_ENTRY_SET_ENTRY_NDX_VAL_LKP_ELK_FOUND_ID,1))          
    {  
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_PGM_SELECTION_ENTRY_SET_PMF_PGM_SELECTION_ENTRY_SET_ENTRY_NDX_VAL_LKP_ELK_FOUND_ID);  
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
      prm_entry_ndx.val.lkp.elk_found = (uint8)param_val->value.ulong_value;
    } 
    if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SELECTION_ENTRY_SET_PMF_PGM_SELECTION_ENTRY_SET_ENTRY_NDX_VAL_LKP_TCAM_FOUND_ID,1))          
    {  
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_PGM_SELECTION_ENTRY_SET_PMF_PGM_SELECTION_ENTRY_SET_ENTRY_NDX_VAL_LKP_TCAM_FOUND_ID);  
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
      prm_entry_ndx.val.lkp.tcam_found = (uint8)param_val->value.ulong_value;
    } 
    if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SELECTION_ENTRY_SET_PMF_PGM_SELECTION_ENTRY_SET_ENTRY_NDX_VAL_LKP_LPM_2ND_NOT_DFLT_ID,1))          
    {  
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_PGM_SELECTION_ENTRY_SET_PMF_PGM_SELECTION_ENTRY_SET_ENTRY_NDX_VAL_LKP_LPM_2ND_NOT_DFLT_ID);  
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
      prm_entry_ndx.val.lkp.lpm_2nd_not_dflt = (uint8)param_val->value.ulong_value;
    } 
    if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SELECTION_ENTRY_SET_PMF_PGM_SELECTION_ENTRY_SET_ENTRY_NDX_VAL_LKP_LPM_1ST_NOT_DFLT_ID,1))          
    {  
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_PGM_SELECTION_ENTRY_SET_PMF_PGM_SELECTION_ENTRY_SET_ENTRY_NDX_VAL_LKP_LPM_1ST_NOT_DFLT_ID);  
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
      prm_entry_ndx.val.lkp.lpm_1st_not_dflt = (uint8)param_val->value.ulong_value;
    } 
    if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SELECTION_ENTRY_SET_PMF_PGM_SELECTION_ENTRY_SET_ENTRY_NDX_VAL_LKP_SEM_2ND_FOUND_ID,1))          
    {  
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_PGM_SELECTION_ENTRY_SET_PMF_PGM_SELECTION_ENTRY_SET_ENTRY_NDX_VAL_LKP_SEM_2ND_FOUND_ID);  
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
      prm_entry_ndx.val.lkp.sem_2nd_found = (uint8)param_val->value.ulong_value;
    } 
    if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SELECTION_ENTRY_SET_PMF_PGM_SELECTION_ENTRY_SET_ENTRY_NDX_VAL_LKP_SEM_1ST_FOUND_ID,1))          
    {  
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_PGM_SELECTION_ENTRY_SET_PMF_PGM_SELECTION_ENTRY_SET_ENTRY_NDX_VAL_LKP_SEM_1ST_FOUND_ID);  
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
      prm_entry_ndx.val.lkp.sem_1st_found = (uint8)param_val->value.ulong_value;
    } 
    if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SELECTION_ENTRY_SET_PMF_PGM_SELECTION_ENTRY_SET_ENTRY_NDX_VAL_LKP_LEM_2ND_FOUND_ID,1))          
    {  
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_PGM_SELECTION_ENTRY_SET_PMF_PGM_SELECTION_ENTRY_SET_ENTRY_NDX_VAL_LKP_LEM_2ND_FOUND_ID);  
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
      prm_entry_ndx.val.lkp.lem_2nd_found = (uint8)param_val->value.ulong_value;
    } 
    if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SELECTION_ENTRY_SET_PMF_PGM_SELECTION_ENTRY_SET_ENTRY_NDX_VAL_LKP_LEM_1ST_FOUND_ID,1))          
    {  
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_PGM_SELECTION_ENTRY_SET_PMF_PGM_SELECTION_ENTRY_SET_ENTRY_NDX_VAL_LKP_LEM_1ST_FOUND_ID);  
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
      prm_entry_ndx.val.lkp.lem_1st_found = (uint8)param_val->value.ulong_value;
    } 
    break;

  case SOC_TMC_PMF_PGM_SEL_TYPE_PARSER_PMF_PRO:
    if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SELECTION_ENTRY_SET_PMF_PGM_SELECTION_ENTRY_SET_ENTRY_NDX_VAL_PRSR_PMF_PORT_PMF_ID,1))          
    {  
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_PGM_SELECTION_ENTRY_SET_PMF_PGM_SELECTION_ENTRY_SET_ENTRY_NDX_VAL_PRSR_PMF_PORT_PMF_ID);  
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
      prm_entry_ndx.val.prsr_pmf.port_pmf = (uint32)param_val->value.ulong_value;
    } 
    if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SELECTION_ENTRY_SET_PMF_PGM_SELECTION_ENTRY_SET_ENTRY_NDX_VAL_PRSR_PMF_PRSR_ID,1))          
    {  
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_PGM_SELECTION_ENTRY_SET_PMF_PGM_SELECTION_ENTRY_SET_ENTRY_NDX_VAL_PRSR_PMF_PRSR_ID);  
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
      prm_entry_ndx.val.prsr_pmf.prsr = (uint32)param_val->value.ulong_value;
    } 
    break;

  case SOC_TMC_PMF_PGM_SEL_TYPE_LLVP_PFC:
    if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SELECTION_ENTRY_SET_PMF_PGM_SELECTION_ENTRY_SET_ENTRY_NDX_VAL_LLVP_PFC_PMF_PRO_ID,1))          
    {  
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_PGM_SELECTION_ENTRY_SET_PMF_PGM_SELECTION_ENTRY_SET_ENTRY_NDX_VAL_LLVP_PFC_PMF_PRO_ID);  
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
      prm_entry_ndx.val.llvp_pfc.pmf_pro = (uint32)param_val->value.ulong_value;
    } 
    if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SELECTION_ENTRY_SET_PMF_PGM_SELECTION_ENTRY_SET_ENTRY_NDX_VAL_LLVP_PFC_LLVP_ID,1))          
    {  
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_PGM_SELECTION_ENTRY_SET_PMF_PGM_SELECTION_ENTRY_SET_ENTRY_NDX_VAL_LLVP_PFC_LLVP_ID);  
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
      prm_entry_ndx.val.llvp_pfc.llvp = (uint32)param_val->value.ulong_value;
    }  
    break;
  
  default:
    break;
 }


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_pmf_pgm_selection_entry_get_unsafe(
          unit,
          &prm_entry_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_pgm_selection_entry_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_pgm_selection_entry_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SELECTION_ENTRY_SET_PMF_PGM_SELECTION_ENTRY_SET_INFO_IS_PGM_VALID_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_PGM_SELECTION_ENTRY_SET_PMF_PGM_SELECTION_ENTRY_SET_INFO_IS_PGM_VALID_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_is_pgm_valid_index = (uint32)param_val->value.ulong_value;
  } 

  if(prm_is_pgm_valid_index != 0xFFFFFFFF)          
  { 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SELECTION_ENTRY_SET_PMF_PGM_SELECTION_ENTRY_SET_INFO_IS_PGM_VALID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_PGM_SELECTION_ENTRY_SET_PMF_PGM_SELECTION_ENTRY_SET_INFO_IS_PGM_VALID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.is_pgm_valid[ prm_is_pgm_valid_index] = (uint8)param_val->value.ulong_value;
  } 

  }   


  /* Call function */
  ret = soc_pb_pmf_pgm_selection_entry_set_unsafe(
          unit,
          &prm_entry_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_pgm_selection_entry_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_pgm_selection_entry_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pmf_pgm_selection_entry_get (section pmf_low_level)
 ********************************************************************/
int 
  ui_pb_api_pmf_low_level_pmf_pgm_selection_entry_get_unsafe(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_TMC_PMF_PGM_SELECTION_ENTRY   
    prm_entry_ndx;
  SOC_TMC_PMF_PGM_VALIDITY_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_pmf_low_level"); 
  soc_sand_proc_name = "soc_ppd_pmf_pgm_selection_entry_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_TMC_PMF_PGM_SELECTION_ENTRY_clear(&prm_entry_ndx);
  SOC_TMC_PMF_PGM_VALIDITY_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SELECTION_ENTRY_GET_PMF_PGM_SELECTION_ENTRY_GET_ENTRY_NDX_TYPE_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PMF_PGM_SELECTION_ENTRY_GET_PMF_PGM_SELECTION_ENTRY_GET_ENTRY_NDX_TYPE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_entry_ndx.type = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter entry_ndx after pmf_pgm_selection_entry_get***", TRUE); 
    goto exit; 
  } 

  switch(prm_entry_ndx.type) 
  {
  case SOC_TMC_PMF_PGM_SEL_TYPE_EEI_OUTLIF_15_8:
    if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SELECTION_ENTRY_GET_PMF_PGM_SELECTION_ENTRY_GET_ENTRY_NDX_VAL_EEI_OUTLIF_15_8_ID,1))          
    {  
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_PGM_SELECTION_ENTRY_GET_PMF_PGM_SELECTION_ENTRY_GET_ENTRY_NDX_VAL_EEI_OUTLIF_15_8_ID);  
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
      prm_entry_ndx.val.eei_outlif_15_8 = (uint32)param_val->value.ulong_value;
    } 
    break;

  case SOC_TMC_PMF_PGM_SEL_TYPE_EEI_OUTLIF_7_0:
    if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SELECTION_ENTRY_GET_PMF_PGM_SELECTION_ENTRY_GET_ENTRY_NDX_VAL_EEI_OUTLIF_7_0_ID,1))          
    {  
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_PGM_SELECTION_ENTRY_GET_PMF_PGM_SELECTION_ENTRY_GET_ENTRY_NDX_VAL_EEI_OUTLIF_7_0_ID);  
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
      prm_entry_ndx.val.eei_outlif_7_0 = (uint32)param_val->value.ulong_value;
    } 
    break;

  case SOC_TMC_PMF_PGM_SEL_TYPE_SEM_NDX_7_0:
    if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SELECTION_ENTRY_GET_PMF_PGM_SELECTION_ENTRY_GET_ENTRY_NDX_VAL_SEM_7_0_NDX_ID,1))          
    {  
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_PGM_SELECTION_ENTRY_GET_PMF_PGM_SELECTION_ENTRY_GET_ENTRY_NDX_VAL_SEM_7_0_NDX_ID);  
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
      prm_entry_ndx.val.sem_7_0_ndx = (uint32)param_val->value.ulong_value;
    } 
    break;

  case SOC_TMC_PMF_PGM_SEL_TYPE_PFQ_SEM_NDX_13_8:
    if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SELECTION_ENTRY_GET_PMF_PGM_SELECTION_ENTRY_GET_ENTRY_NDX_VAL_PFQ_SEM_SEM_13_8_NDX_ID,1))          
    {  
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_PGM_SELECTION_ENTRY_GET_PMF_PGM_SELECTION_ENTRY_GET_ENTRY_NDX_VAL_PFQ_SEM_SEM_13_8_NDX_ID);  
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
      prm_entry_ndx.val.pfq_sem.sem_13_8_ndx = (uint32)param_val->value.ulong_value;
    } 
    if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SELECTION_ENTRY_GET_PMF_PGM_SELECTION_ENTRY_GET_ENTRY_NDX_VAL_PFQ_SEM_PFQ_ID,1))          
    {  
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_PGM_SELECTION_ENTRY_GET_PMF_PGM_SELECTION_ENTRY_GET_ENTRY_NDX_VAL_PFQ_SEM_PFQ_ID);  
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
      prm_entry_ndx.val.pfq_sem.pfq = (uint32)param_val->value.ulong_value;
    } 
    break;

  case SOC_TMC_PMF_PGM_SEL_TYPE_FWDING_TTC_CODE:
    if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SELECTION_ENTRY_GET_PMF_PGM_SELECTION_ENTRY_GET_ENTRY_NDX_VAL_FWD_TTC_TTC_ID,1))          
    {  
      UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PMF_PGM_SELECTION_ENTRY_GET_PMF_PGM_SELECTION_ENTRY_GET_ENTRY_NDX_VAL_FWD_TTC_TTC_ID);  
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
      prm_entry_ndx.val.fwd_ttc.ttc = param_val->numeric_equivalent;
    } 
    if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SELECTION_ENTRY_GET_PMF_PGM_SELECTION_ENTRY_GET_ENTRY_NDX_VAL_FWD_TTC_FWD_ID,1))          
    {  
      UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PMF_PGM_SELECTION_ENTRY_GET_PMF_PGM_SELECTION_ENTRY_GET_ENTRY_NDX_VAL_FWD_TTC_FWD_ID);  
      UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
      prm_entry_ndx.val.fwd_ttc.fwd = param_val->numeric_equivalent;
    } 
    break;

  case SOC_TMC_PMF_PGM_SEL_TYPE_LOOKUPS:
    if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SELECTION_ENTRY_GET_PMF_PGM_SELECTION_ENTRY_GET_ENTRY_NDX_VAL_LKP_ELK_FOUND_ID,1))          
    {  
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_PGM_SELECTION_ENTRY_GET_PMF_PGM_SELECTION_ENTRY_GET_ENTRY_NDX_VAL_LKP_ELK_FOUND_ID);  
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
      prm_entry_ndx.val.lkp.elk_found = (uint8)param_val->value.ulong_value;
    } 
    if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SELECTION_ENTRY_GET_PMF_PGM_SELECTION_ENTRY_GET_ENTRY_NDX_VAL_LKP_TCAM_FOUND_ID,1))          
    {  
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_PGM_SELECTION_ENTRY_GET_PMF_PGM_SELECTION_ENTRY_GET_ENTRY_NDX_VAL_LKP_TCAM_FOUND_ID);  
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
      prm_entry_ndx.val.lkp.tcam_found = (uint8)param_val->value.ulong_value;
    } 
    if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SELECTION_ENTRY_GET_PMF_PGM_SELECTION_ENTRY_GET_ENTRY_NDX_VAL_LKP_LPM_2ND_NOT_DFLT_ID,1))          
    {  
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_PGM_SELECTION_ENTRY_GET_PMF_PGM_SELECTION_ENTRY_GET_ENTRY_NDX_VAL_LKP_LPM_2ND_NOT_DFLT_ID);  
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
      prm_entry_ndx.val.lkp.lpm_2nd_not_dflt = (uint8)param_val->value.ulong_value;
    } 
    if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SELECTION_ENTRY_GET_PMF_PGM_SELECTION_ENTRY_GET_ENTRY_NDX_VAL_LKP_LPM_1ST_NOT_DFLT_ID,1))          
    {  
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_PGM_SELECTION_ENTRY_GET_PMF_PGM_SELECTION_ENTRY_GET_ENTRY_NDX_VAL_LKP_LPM_1ST_NOT_DFLT_ID);  
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
      prm_entry_ndx.val.lkp.lpm_1st_not_dflt = (uint8)param_val->value.ulong_value;
    } 
    if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SELECTION_ENTRY_GET_PMF_PGM_SELECTION_ENTRY_GET_ENTRY_NDX_VAL_LKP_SEM_2ND_FOUND_ID,1))          
    {  
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_PGM_SELECTION_ENTRY_GET_PMF_PGM_SELECTION_ENTRY_GET_ENTRY_NDX_VAL_LKP_SEM_2ND_FOUND_ID);  
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
      prm_entry_ndx.val.lkp.sem_2nd_found = (uint8)param_val->value.ulong_value;
    } 
    if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SELECTION_ENTRY_GET_PMF_PGM_SELECTION_ENTRY_GET_ENTRY_NDX_VAL_LKP_SEM_1ST_FOUND_ID,1))          
    {  
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_PGM_SELECTION_ENTRY_GET_PMF_PGM_SELECTION_ENTRY_GET_ENTRY_NDX_VAL_LKP_SEM_1ST_FOUND_ID);  
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
      prm_entry_ndx.val.lkp.sem_1st_found = (uint8)param_val->value.ulong_value;
    } 
    if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SELECTION_ENTRY_GET_PMF_PGM_SELECTION_ENTRY_GET_ENTRY_NDX_VAL_LKP_LEM_2ND_FOUND_ID,1))          
    {  
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_PGM_SELECTION_ENTRY_GET_PMF_PGM_SELECTION_ENTRY_GET_ENTRY_NDX_VAL_LKP_LEM_2ND_FOUND_ID);  
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
      prm_entry_ndx.val.lkp.lem_2nd_found = (uint8)param_val->value.ulong_value;
    } 
    if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SELECTION_ENTRY_GET_PMF_PGM_SELECTION_ENTRY_GET_ENTRY_NDX_VAL_LKP_LEM_1ST_FOUND_ID,1))          
    {  
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_PGM_SELECTION_ENTRY_GET_PMF_PGM_SELECTION_ENTRY_GET_ENTRY_NDX_VAL_LKP_LEM_1ST_FOUND_ID);  
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
      prm_entry_ndx.val.lkp.lem_1st_found = (uint8)param_val->value.ulong_value;
    } 
    break;

  case SOC_TMC_PMF_PGM_SEL_TYPE_PARSER_PMF_PRO:
    if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SELECTION_ENTRY_GET_PMF_PGM_SELECTION_ENTRY_GET_ENTRY_NDX_VAL_PRSR_PMF_PORT_PMF_ID,1))          
    {  
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_PGM_SELECTION_ENTRY_GET_PMF_PGM_SELECTION_ENTRY_GET_ENTRY_NDX_VAL_PRSR_PMF_PORT_PMF_ID);  
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
      prm_entry_ndx.val.prsr_pmf.port_pmf = (uint32)param_val->value.ulong_value;
    } 
    if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SELECTION_ENTRY_GET_PMF_PGM_SELECTION_ENTRY_GET_ENTRY_NDX_VAL_PRSR_PMF_PRSR_ID,1))          
    {  
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_PGM_SELECTION_ENTRY_GET_PMF_PGM_SELECTION_ENTRY_GET_ENTRY_NDX_VAL_PRSR_PMF_PRSR_ID);  
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
      prm_entry_ndx.val.prsr_pmf.prsr = (uint32)param_val->value.ulong_value;
    } 
    break;

  case SOC_TMC_PMF_PGM_SEL_TYPE_LLVP_PFC:
    if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SELECTION_ENTRY_GET_PMF_PGM_SELECTION_ENTRY_GET_ENTRY_NDX_VAL_LLVP_PFC_PMF_PRO_ID,1))          
    {  
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_PGM_SELECTION_ENTRY_GET_PMF_PGM_SELECTION_ENTRY_GET_ENTRY_NDX_VAL_LLVP_PFC_PMF_PRO_ID);  
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
      prm_entry_ndx.val.llvp_pfc.pmf_pro = (uint32)param_val->value.ulong_value;
    } 
    if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SELECTION_ENTRY_GET_PMF_PGM_SELECTION_ENTRY_GET_ENTRY_NDX_VAL_LLVP_PFC_LLVP_ID,1))          
    {  
      UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_PGM_SELECTION_ENTRY_GET_PMF_PGM_SELECTION_ENTRY_GET_ENTRY_NDX_VAL_LLVP_PFC_LLVP_ID);  
      UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
      prm_entry_ndx.val.llvp_pfc.llvp = (uint32)param_val->value.ulong_value;
    }  
    break;

  default:
    break;
  }


  /* Call function */
  ret = soc_pb_pmf_pgm_selection_entry_get_unsafe(
          unit,
          &prm_entry_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_pgm_selection_entry_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_pgm_selection_entry_get");   
    goto exit; 
  } 

  SOC_TMC_PMF_PGM_VALIDITY_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pmf_pgm_set (section pmf_low_level)
 ********************************************************************/
int 
  ui_pb_api_pmf_low_level_pmf_pgm_set_unsafe(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_pmf_pgm_ndx;
  uint32 
    prm_lkp_profile_id_index = 0xFFFFFFFF;  
  SOC_TMC_PMF_PGM_INFO   
    prm_info;
  SOC_SAND_SUCCESS_FAILURE
    success = SOC_SAND_SUCCESS;

  UI_MACROS_INIT_FUNCTION("ui_pb_api_pmf_low_level"); 
  soc_sand_proc_name = "soc_ppd_pmf_pgm_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_TMC_PMF_PGM_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SET_PMF_PGM_SET_PMF_PGM_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_PGM_SET_PMF_PGM_SET_PMF_PGM_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_pmf_pgm_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter pmf_pgm_ndx after pmf_pgm_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_pmf_pgm_get_unsafe(
          unit,
          prm_pmf_pgm_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_pgm_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_pgm_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SET_PMF_PGM_SET_INFO_COPY_PGM_VAR_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_PGM_SET_PMF_PGM_SET_INFO_COPY_PGM_VAR_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.copy_pgm_var = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SET_PMF_PGM_SET_INFO_FC_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PMF_PGM_SET_PMF_PGM_SET_INFO_FC_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.fc_type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SET_PMF_PGM_SET_INFO_BYTES_TO_RMV_NOF_BYTES_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_PGM_SET_PMF_PGM_SET_INFO_BYTES_TO_RMV_NOF_BYTES_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.bytes_to_rmv.nof_bytes = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SET_PMF_PGM_SET_INFO_BYTES_TO_RMV_HEADER_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PMF_PGM_SET_PMF_PGM_SET_INFO_BYTES_TO_RMV_HEADER_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.bytes_to_rmv.header_type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SET_PMF_PGM_SET_INFO_HEADER_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PMF_PGM_SET_PMF_PGM_SET_INFO_HEADER_TYPE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.header_type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SET_PMF_PGM_SET_INFO_TAG_PROFILE_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_PGM_SET_PMF_PGM_SET_INFO_TAG_PROFILE_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.tag_profile_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SET_PMF_PGM_SET_INFO_LKP_PROFILE_ID_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_PGM_SET_PMF_PGM_SET_INFO_LKP_PROFILE_ID_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lkp_profile_id_index = (uint32)param_val->value.ulong_value;
  } 

  if(prm_lkp_profile_id_index != 0xFFFFFFFF)          
  { 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SET_PMF_PGM_SET_INFO_LKP_PROFILE_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_PGM_SET_PMF_PGM_SET_INFO_LKP_PROFILE_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.lkp_profile_id[ prm_lkp_profile_id_index] = (uint32)param_val->value.ulong_value;
  } 

  }   


  /* Call function */
  ret = soc_pb_pmf_pgm_set_unsafe(
          unit,
          prm_pmf_pgm_ndx,
          &prm_info,
          &success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_pgm_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_pgm_set");   
    goto exit; 
  } 

  soc_sand_os_printf("success: %s\n\r",soc_sand_SAND_SUCCESS_FAILURE_to_string(success));
  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pmf_pgm_get (section pmf_low_level)
 ********************************************************************/
int 
  ui_pb_api_pmf_low_level_pmf_pgm_get_unsafe(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_pmf_pgm_ndx;
  SOC_TMC_PMF_PGM_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_pmf_low_level"); 
  soc_sand_proc_name = "soc_ppd_pmf_pgm_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_TMC_PMF_PGM_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_GET_PMF_PGM_GET_PMF_PGM_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PMF_PGM_GET_PMF_PGM_GET_PMF_PGM_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_pmf_pgm_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter pmf_pgm_ndx after pmf_pgm_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_pmf_pgm_get_unsafe(
          unit,
          prm_pmf_pgm_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_pmf_pgm_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_pmf_pgm_get");   
    goto exit; 
  } 

  SOC_TMC_PMF_PGM_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
#endif
#ifdef UI_PMF_LOW_LEVEL/* { pmf_low_level*/
/******************************************************************** 
 *  Section handler: pmf_low_level
 ********************************************************************/ 
int 
  ui_pb_api_pmf_low_level( 
    CURRENT_LINE *current_line 
  ) 
{   
  int
    ret = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_pb_api_pmf_low_level"); 
 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_CE_PACKET_HEADER_ENTRY_SET_PMF_CE_PACKET_HEADER_ENTRY_SET_ID,1)) 
  { 
    ret = ui_pb_api_pmf_low_level_pmf_ce_packet_header_entry_set_unsafe(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_CE_PACKET_HEADER_ENTRY_GET_PMF_CE_PACKET_HEADER_ENTRY_GET_ID,1)) 
  { 
    ret = ui_pb_api_pmf_low_level_pmf_ce_packet_header_entry_get_unsafe(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_CE_IRPP_INFO_ENTRY_SET_PMF_CE_IRPP_INFO_ENTRY_SET_ID,1)) 
  { 
    ret = ui_pb_api_pmf_low_level_pmf_ce_irpp_info_entry_set_unsafe(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_CE_IRPP_INFO_ENTRY_GET_PMF_CE_IRPP_INFO_ENTRY_GET_ID,1)) 
  { 
    ret = ui_pb_api_pmf_low_level_pmf_ce_irpp_info_entry_get_unsafe(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_CE_NOP_ENTRY_SET_PMF_CE_NOP_ENTRY_SET_ID,1)) 
  { 
    ret = ui_pb_api_pmf_low_level_pmf_ce_nop_entry_set_unsafe(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_CE_NOP_ENTRY_GET_PMF_CE_NOP_ENTRY_GET_ID,1)) 
  { 
    ret = ui_pb_api_pmf_low_level_pmf_ce_nop_entry_get_unsafe(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_LOOKUP_SET_PMF_TCAM_LOOKUP_SET_ID,1)) 
  { 
    ret = ui_pb_api_pmf_low_level_pmf_tcam_lookup_set_unsafe(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_LOOKUP_GET_PMF_TCAM_LOOKUP_GET_ID,1)) 
  { 
    ret = ui_pb_api_pmf_low_level_pmf_tcam_lookup_get_unsafe(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_ENTRY_ADD_PMF_TCAM_ENTRY_ADD_ID,1)) 
  { 
    ret = ui_pb_api_pmf_low_level_pmf_tcam_entry_add(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_ENTRY_GET_PMF_TCAM_ENTRY_GET_ID,1)) 
  { 
    ret = ui_pb_api_pmf_low_level_pmf_tcam_entry_get_unsafe(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_ENTRY_REMOVE_PMF_TCAM_ENTRY_REMOVE_ID,1)) 
  { 
    ret = ui_pb_api_pmf_low_level_pmf_tcam_entry_remove(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_KEY_CLEAR_PMF_TCAM_KEY_CLEAR_ID,1)) 
  { 
    ret = ui_pb_api_pmf_low_level_pmf_tcam_key_clear(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_KEY_VAL_SET_PMF_TCAM_KEY_VAL_SET_ID,1)) 
  { 
    ret = ui_pb_api_pmf_low_level_pmf_tcam_key_val_set_unsafe(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_TCAM_KEY_VAL_GET_PMF_TCAM_KEY_VAL_GET_ID,1)) 
  { 
    ret = ui_pb_api_pmf_low_level_pmf_tcam_key_val_get_unsafe(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_KEY_MASKED_VAL_SET_PMF_TCAM_KEY_MASKED_VAL_SET_ID,1)) 
  { 
    ret = ui_pb_api_pmf_low_level_pmf_tcam_key_masked_val_set_unsafe(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TCAM_KEY_MASKED_VAL_GET_PMF_TCAM_KEY_MASKED_VAL_GET_ID,1)) 
  { 
    ret = ui_pb_api_pmf_low_level_pmf_tcam_key_masked_val_get_unsafe(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DB_DIRECT_TBL_KEY_SRC_SET_PMF_DB_DIRECT_TBL_KEY_SRC_SET_ID,1)) 
  { 
    ret = ui_pb_api_pmf_low_level_pmf_db_direct_tbl_key_src_set_unsafe(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DB_DIRECT_TBL_KEY_SRC_GET_PMF_DB_DIRECT_TBL_KEY_SRC_GET_ID,1)) 
  { 
    ret = ui_pb_api_pmf_low_level_pmf_db_direct_tbl_key_src_get_unsafe(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DB_DIRECT_TBL_ENTRY_SET_PMF_DB_DIRECT_TBL_ENTRY_SET_ID,1)) 
  { 
    ret = ui_pb_api_pmf_low_level_pmf_db_direct_tbl_entry_set_unsafe(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DB_DIRECT_TBL_ENTRY_GET_PMF_DB_DIRECT_TBL_ENTRY_GET_ID,1)) 
  { 
    ret = ui_pb_api_pmf_low_level_pmf_db_direct_tbl_entry_get_unsafe(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DB_FEM_INPUT_SET_PMF_DB_FEM_INPUT_SET_ID,1)) 
  { 
    ret = ui_pb_api_pmf_low_level_pmf_db_fem_input_set_unsafe(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DB_FEM_INPUT_GET_PMF_DB_FEM_INPUT_GET_ID,1)) 
  { 
    ret = ui_pb_api_pmf_low_level_pmf_db_fem_input_get_unsafe(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DB_TAG_SELECT_SET_PMF_DB_TAG_SELECT_SET_ID,1)) 
  { 
    ret = ui_pb_api_pmf_low_level_pmf_db_tag_select_set_unsafe(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_DB_TAG_SELECT_GET_PMF_DB_TAG_SELECT_GET_ID,1)) 
  { 
    ret = ui_pb_api_pmf_low_level_pmf_db_tag_select_get_unsafe(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_FEM_SELECT_BITS_SET_PMF_FEM_SELECT_BITS_SET_ID,1)) 
  { 
    ret = ui_pb_api_pmf_low_level_pmf_fem_select_bits_set_unsafe(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_FEM_SELECT_BITS_GET_PMF_FEM_SELECT_BITS_GET_ID,1)) 
  { 
    ret = ui_pb_api_pmf_low_level_pmf_fem_select_bits_get_unsafe(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_FEM_ACTION_FORMAT_MAP_SET_PMF_FEM_ACTION_FORMAT_MAP_SET_ID,1)) 
  { 
    ret = ui_pb_api_pmf_low_level_pmf_fem_action_format_map_set_unsafe(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_FEM_ACTION_FORMAT_MAP_GET_PMF_FEM_ACTION_FORMAT_MAP_GET_ID,1)) 
  { 
    ret = ui_pb_api_pmf_low_level_pmf_fem_action_format_map_get_unsafe(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_FEM_ACTION_FORMAT_SET_PMF_FEM_ACTION_FORMAT_SET_ID,1)) 
  { 
    ret = ui_pb_api_pmf_low_level_pmf_fem_action_format_set_unsafe(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_FEM_ACTION_FORMAT_GET_PMF_FEM_ACTION_FORMAT_GET_ID,1)) 
  { 
    ret = ui_pb_api_pmf_low_level_pmf_fem_action_format_get_unsafe(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SELECTION_ENTRY_SET_PMF_PGM_SELECTION_ENTRY_SET_ID,1)) 
  { 
    ret = ui_pb_api_pmf_low_level_pmf_pgm_selection_entry_set_unsafe(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SELECTION_ENTRY_GET_PMF_PGM_SELECTION_ENTRY_GET_ID,1)) 
  { 
    ret = ui_pb_api_pmf_low_level_pmf_pgm_selection_entry_get_unsafe(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_SET_PMF_PGM_SET_ID,1)) 
  { 
    ret = ui_pb_api_pmf_low_level_pmf_pgm_set_unsafe(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PMF_PGM_GET_PMF_PGM_GET_ID,1)) 
  { 
    ret = ui_pb_api_pmf_low_level_pmf_pgm_get_unsafe(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after pmf_low_level***", TRUE); 
  }
  
  ui_ret = ret; 
    
  goto exit; 
exit:        
  return ui_ret; 
}                
          
#endif /* pmf_low_level */ 
