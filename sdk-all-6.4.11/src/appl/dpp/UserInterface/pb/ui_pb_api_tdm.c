/* $Id: ui_pb_api_tdm.c,v 1.7 Broadcom SDK $
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
 
  
#include <soc/dpp/Petra/PB_TM/pb_api_tdm.h>
#include <soc/dpp/Petra/petra_sw_db.h>
#include <appl/dpp/UserInterface/pb/ui_pb_api_tdm.h>

#if LINK_PB_LIBRARIES

#ifdef UI_TDM
/******************************************************************** 
 *  Function handler: ftmh_set (section tdm)
 ********************************************************************/
int 
  ui_pb_api_tdm_ftmh_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_port_ndx;
  SOC_PB_TDM_FTMH_INFO   
    prm_info;
  SOC_PETRA_MGMT_TDM_MODE
    tdm_mode;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_tdm"); 
  soc_sand_proc_name = "soc_pb_tdm_ftmh_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_TDM_FTMH_INFO_clear(&prm_info);
  
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TDM_FTMH_SET_FTMH_SET_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TDM_FTMH_SET_FTMH_SET_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter port_ndx after ftmh_set***", TRUE); 
    goto exit; 
  } 
  
  tdm_mode = soc_petra_sw_db_tdm_mode_get(unit);

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_tdm_ftmh_get(
          unit,
          SOC_CORE_DEFAULT,
          prm_port_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_tdm_ftmh_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_tdm_ftmh_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TDM_FTMH_SET_FTMH_SET_INFO_ACTION_ING_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_TDM_FTMH_SET_FTMH_SET_INFO_ACTION_ING_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.action_ing = param_val->numeric_equivalent;
  }
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter action_ing after port_ndx***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TDM_FTMH_SET_FTMH_SET_INFO_ACTION_EG_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_TDM_FTMH_SET_FTMH_SET_INFO_ACTION_EG_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.action_eg = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter action_eg after action_ing***", TRUE); 
    goto exit; 
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TDM_FTMH_SET_FTMH_SET_INFO_IS_MC_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TDM_FTMH_SET_FTMH_SET_INFO_IS_MC_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.is_mc = (uint8)param_val->value.ulong_value;
  } 

  if (tdm_mode == SOC_PETRA_MGMT_TDM_MODE_TDM_STA)
  {
    if (prm_info.is_mc == TRUE)
    {
      prm_info.ftmh.standard_mc.user_def = 0;
      prm_info.ftmh.standard_mc.mc_id = 0;

      if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TDM_FTMH_SET_FTMH_SET_INFO_FTMH_MC_ID_ID,1)) 
      { 
        UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TDM_FTMH_SET_FTMH_SET_INFO_FTMH_MC_ID_ID);  
        UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
        prm_info.ftmh.standard_mc.mc_id = (uint32)param_val->value.ulong_value;
      } 

      if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TDM_FTMH_SET_FTMH_SET_INFO_FTMH_STANDARD_USER_DEF_ID,1)) 
      { 
        UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TDM_FTMH_SET_FTMH_SET_INFO_FTMH_STANDARD_USER_DEF_ID);  
        UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
        prm_info.ftmh.standard_mc.user_def = (uint32)param_val->value.ulong_value;
      } 
    }
    else
    {
      prm_info.ftmh.standard_uc.user_def = 0;
      prm_info.ftmh.standard_uc.sys_phy_port = 0;

      if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TDM_FTMH_SET_FTMH_SET_INFO_FTMH_STANDARD_UC_SYS_PHY_PORT_ID,1)) 
      { 
        UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TDM_FTMH_SET_FTMH_SET_INFO_FTMH_STANDARD_UC_SYS_PHY_PORT_ID);  
        UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
        prm_info.ftmh.standard_uc.sys_phy_port = (uint32)param_val->value.ulong_value;
      } 

      if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TDM_FTMH_SET_FTMH_SET_INFO_FTMH_STANDARD_USER_DEF_ID,1)) 
      { 
        UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TDM_FTMH_SET_FTMH_SET_INFO_FTMH_STANDARD_USER_DEF_ID);  
        UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
        prm_info.ftmh.standard_uc.user_def = (uint32)param_val->value.ulong_value;
      }
    }
  }
  else  
  {
    if (prm_info.is_mc == TRUE)
    {
      prm_info.ftmh.opt_mc.mc_id = 0;
      
      if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TDM_FTMH_SET_FTMH_SET_INFO_FTMH_MC_ID_ID,1)) 
      { 
        UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TDM_FTMH_SET_FTMH_SET_INFO_FTMH_MC_ID_ID);  
        UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
        prm_info.ftmh.opt_mc.mc_id = (uint32)param_val->value.ulong_value;
      } 

    }
    else
    {
      prm_info.ftmh.opt_uc.dest_fap_id = 0;
      prm_info.ftmh.opt_uc.dest_if = 0;

      if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TDM_FTMH_SET_FTMH_SET_INFO_FTMH_OPT_UC_DEST_FAP_ID_ID,1)) 
      { 
        UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TDM_FTMH_SET_FTMH_SET_INFO_FTMH_OPT_UC_DEST_FAP_ID_ID);  
        UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
        prm_info.ftmh.opt_uc.dest_fap_id = (uint32)param_val->value.ulong_value;
      } 

      if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TDM_FTMH_SET_FTMH_SET_INFO_FTMH_OPT_UC_DEST_IF_ID,1)) 
      { 
        UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TDM_FTMH_SET_FTMH_SET_INFO_FTMH_OPT_UC_DEST_IF_ID);  
        UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
        prm_info.ftmh.opt_uc.dest_if = (uint32)param_val->value.ulong_value;
      }
    }
  }
  
  /* Call function */
  ret = soc_pb_tdm_ftmh_set(
          unit,
          SOC_CORE_DEFAULT,
          prm_port_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_tdm_ftmh_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_tdm_ftmh_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: ftmh_get (section tdm)
 ********************************************************************/
int 
  ui_pb_api_tdm_ftmh_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_port_ndx;
  SOC_PB_TDM_FTMH_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_tdm"); 
  soc_sand_proc_name = "soc_pb_tdm_ftmh_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_TDM_FTMH_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TDM_FTMH_GET_FTMH_GET_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TDM_FTMH_GET_FTMH_GET_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter port_ndx after ftmh_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_tdm_ftmh_get(
          unit,
          SOC_CORE_DEFAULT,
          prm_port_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_tdm_ftmh_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_tdm_ftmh_get");   
    goto exit; 
  } 

  SOC_PB_TDM_FTMH_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: opt_size_set (section tdm)
 ********************************************************************/
int 
  ui_pb_api_tdm_opt_size_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_cell_size;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_tdm"); 
  soc_sand_proc_name = "soc_pb_tdm_opt_size_set"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_tdm_opt_size_get(
          unit,
          &prm_cell_size
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_tdm_opt_size_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_tdm_opt_size_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TDM_OPT_SIZE_RANGE_SET_OPT_SIZE_RANGE_SET_CELL_SIZE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TDM_OPT_SIZE_RANGE_SET_OPT_SIZE_RANGE_SET_CELL_SIZE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_cell_size = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_tdm_opt_size_set(
          unit,
          prm_cell_size
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_tdm_opt_size_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_tdm_opt_size_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: opt_size_get (section tdm)
 ********************************************************************/
int 
  ui_pb_api_tdm_opt_size_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_cell_size;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_tdm"); 
  soc_sand_proc_name = "soc_pb_tdm_opt_size_get"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_pb_tdm_opt_size_get(
          unit,
          &prm_cell_size
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_tdm_opt_size_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_tdm_opt_size_get");   
    goto exit; 
  } 

  soc_sand_os_printf("cell_size: %u\n\r",prm_cell_size);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: stand_size_range_set (section tdm)
 ********************************************************************/
int 
  ui_pb_api_tdm_stand_size_range_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_SAND_U32_RANGE   
    prm_size_range;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_tdm"); 
  soc_sand_proc_name = "soc_pb_tdm_stand_size_range_set"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_tdm_stand_size_range_get(
          unit,
          &prm_size_range
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_tdm_stand_size_range_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_tdm_stand_size_range_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TDM_STAND_SIZE_RANGE_SET_STAND_SIZE_RANGE_SET_SIZE_RANGE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TDM_STAND_SIZE_RANGE_SET_STAND_SIZE_RANGE_SET_SIZE_RANGE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_size_range.start = (uint32)param_val->value.ulong_value;
  } 
  
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TDM_STAND_SIZE_RANGE_SET_STAND_SIZE_RANGE_SET_SIZE_RANGE_MAX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TDM_STAND_SIZE_RANGE_SET_STAND_SIZE_RANGE_SET_SIZE_RANGE_MAX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_size_range.end = (uint32)param_val->value.ulong_value;
  } 

  /* Call function */
  ret = soc_pb_tdm_stand_size_range_set(
          unit,
          &prm_size_range
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_tdm_stand_size_range_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_tdm_stand_size_range_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: stand_size_range_get (section tdm)
 ********************************************************************/
int 
  ui_pb_api_tdm_stand_size_range_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_SAND_U32_RANGE   
    prm_size_range;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_tdm"); 
  soc_sand_proc_name = "soc_pb_tdm_stand_size_range_get"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_pb_tdm_stand_size_range_get(
          unit,
          &prm_size_range
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_tdm_stand_size_range_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_tdm_stand_size_range_get");   
    goto exit; 
  } 

  soc_sand_os_printf("size_range: min %u max %u\n\r",prm_size_range.start,prm_size_range.end);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
/******************************************************************** 
 *  Function handler: mc_static_route_set (section tdm)
 ********************************************************************/
int 
  ui_pb_api_tdm_mc_static_route_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_mc_id_route_ndx;
  SOC_PB_TDM_MC_STATIC_ROUTE_INFO   
    prm_route_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_tdm"); 
  soc_sand_proc_name = "soc_pb_tdm_mc_static_route_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_TDM_MC_STATIC_ROUTE_INFO_clear(&prm_route_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TDM_MC_STATIC_ROUTE_SET_MC_STATIC_ROUTE_SET_MC_ID_ROUTE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TDM_MC_STATIC_ROUTE_SET_MC_STATIC_ROUTE_SET_MC_ID_ROUTE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_mc_id_route_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter mc_id_route_ndx after mc_static_route_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_tdm_mc_static_route_get(
          unit,
          prm_mc_id_route_ndx,
          &prm_route_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_tdm_mc_static_route_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_tdm_mc_static_route_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TDM_MC_STATIC_ROUTE_SET_MC_STATIC_ROUTE_SET_ROUTE_INFO_LINK_BITMAP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TDM_MC_STATIC_ROUTE_SET_MC_STATIC_ROUTE_SET_ROUTE_INFO_LINK_BITMAP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_route_info.link_bitmap.arr[0] = param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_tdm_mc_static_route_set(
          unit,
          prm_mc_id_route_ndx,
          &prm_route_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_tdm_mc_static_route_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_tdm_mc_static_route_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: mc_static_route_get (section tdm)
 ********************************************************************/
int 
  ui_pb_api_tdm_mc_static_route_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_mc_id_route_ndx;
  SOC_PB_TDM_MC_STATIC_ROUTE_INFO   
    prm_route_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_tdm"); 
  soc_sand_proc_name = "soc_pb_tdm_mc_static_route_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_TDM_MC_STATIC_ROUTE_INFO_clear(&prm_route_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TDM_MC_STATIC_ROUTE_GET_MC_STATIC_ROUTE_GET_MC_ID_ROUTE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_TDM_MC_STATIC_ROUTE_GET_MC_STATIC_ROUTE_GET_MC_ID_ROUTE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_mc_id_route_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter mc_id_route_ndx after mc_static_route_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_tdm_mc_static_route_get(
          unit,
          prm_mc_id_route_ndx,
          &prm_route_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_tdm_mc_static_route_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_tdm_mc_static_route_get");   
    goto exit; 
  } 

  SOC_PB_TDM_MC_STATIC_ROUTE_INFO_print(&prm_route_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
#endif
#ifdef UI_TDM/* { tdm*/
/******************************************************************** 
 *  Section handler: tdm
 ********************************************************************/ 
int 
  ui_pb_api_tdm( 
    CURRENT_LINE *current_line 
  ) 
{   
  int
    ret = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_pb_api_tdm"); 
 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TDM_FTMH_SET_FTMH_SET_ID,1)) 
  { 
    ret = ui_pb_api_tdm_ftmh_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TDM_FTMH_GET_FTMH_GET_ID,1)) 
  { 
    ret = ui_pb_api_tdm_ftmh_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TDM_OPT_SIZE_RANGE_SET_OPT_SIZE_RANGE_SET_ID,1)) 
  { 
    ret = ui_pb_api_tdm_opt_size_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TDM_OPT_SIZE_RANGE_GET_OPT_SIZE_RANGE_GET_ID,1)) 
  { 
    ret = ui_pb_api_tdm_opt_size_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TDM_STAND_SIZE_RANGE_SET_STAND_SIZE_RANGE_SET_ID,1)) 
  { 
    ret = ui_pb_api_tdm_stand_size_range_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TDM_STAND_SIZE_RANGE_GET_STAND_SIZE_RANGE_GET_ID,1)) 
  { 
    ret = ui_pb_api_tdm_stand_size_range_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TDM_MC_STATIC_ROUTE_SET_MC_STATIC_ROUTE_SET_ID,1)) 
  { 
    ret = ui_pb_api_tdm_mc_static_route_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_TDM_MC_STATIC_ROUTE_GET_MC_STATIC_ROUTE_GET_ID,1)) 
  { 
    ret = ui_pb_api_tdm_mc_static_route_get(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after tdm***", TRUE); 
  }
  
  ui_ret = ret; 
    
  goto exit; 
exit:        
  return ui_ret; 
}                
          
#endif /* tdm */ 


#endif /* LINK_PB_LIBRARIES */ 

