/* $Id: ui_ppd_api_eg_mirror.c,v 1.6 Broadcom SDK $
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
#include <shared/bsl.h> 
#include <soc/dpp/SAND/Utils/sand_os_interface.h> 
#include <appl/diag/dpp/utils_defi.h> 
#include <appl/dpp/UserInterface/ui_defi.h> 
 

#include <soc/dpp/PPD/ppd_api_eg_mirror.h>                                                               
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_eg_mirror.h>

#if LINK_PPD_LIBRARIES

#ifdef UI_EG_MIRROR
/******************************************************************** 
 *  Function handler: port_vlan_add (section eg_mirror)
 ********************************************************************/
int 
  ui_ppd_api_eg_mirror_port_vlan_add(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPD_PORT   
    prm_out_port_ndx;
  SOC_SAND_PP_VLAN_ID   
    prm_vid_ndx;
  uint8   
    prm_enable_mirror = FALSE;
  SOC_SAND_SUCCESS_FAILURE   
    prm_success;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_mirror"); 
  soc_sand_proc_name = "soc_ppd_eg_mirror_port_vlan_add"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_MIRROR_PORT_VLAN_ADD_PORT_VLAN_ADD_OUT_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_MIRROR_PORT_VLAN_ADD_PORT_VLAN_ADD_OUT_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_out_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter out_port_ndx after port_vlan_add***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_MIRROR_PORT_VLAN_ADD_PORT_VLAN_ADD_VID_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_MIRROR_PORT_VLAN_ADD_PORT_VLAN_ADD_VID_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_vid_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter vid_ndx after port_vlan_add***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_MIRROR_PORT_VLAN_ADD_PORT_VLAN_ADD_ENABLE_MIRROR_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_MIRROR_PORT_VLAN_ADD_PORT_VLAN_ADD_ENABLE_MIRROR_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_enable_mirror = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_eg_mirror_port_vlan_add(
          unit,
          prm_out_port_ndx,
          prm_vid_ndx,
          prm_enable_mirror,
          &prm_success
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_mirror_port_vlan_add - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_mirror_port_vlan_add");   
    goto exit; 
  } 

  cli_out("success: %s\n\r",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: port_vlan_remove (section eg_mirror)
 ********************************************************************/
int 
  ui_ppd_api_eg_mirror_port_vlan_remove(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPD_PORT   
    prm_out_port_ndx;
  SOC_SAND_PP_VLAN_ID   
    prm_vid_ndx;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_mirror"); 
  soc_sand_proc_name = "soc_ppd_eg_mirror_port_vlan_remove"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_MIRROR_PORT_VLAN_REMOVE_PORT_VLAN_REMOVE_OUT_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_MIRROR_PORT_VLAN_REMOVE_PORT_VLAN_REMOVE_OUT_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_out_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter out_port_ndx after port_vlan_remove***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_MIRROR_PORT_VLAN_REMOVE_PORT_VLAN_REMOVE_VID_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_MIRROR_PORT_VLAN_REMOVE_PORT_VLAN_REMOVE_VID_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_vid_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter vid_ndx after port_vlan_remove***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_eg_mirror_port_vlan_remove(
          unit,
          prm_out_port_ndx,
          prm_vid_ndx
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_mirror_port_vlan_remove - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_mirror_port_vlan_remove");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: port_vlan_get (section eg_mirror)
 ********************************************************************/
int 
  ui_ppd_api_eg_mirror_port_vlan_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPD_PORT   
    prm_out_port_ndx;
  SOC_SAND_PP_VLAN_ID   
    prm_vid_ndx;
  uint8   
    prm_enable_mirror;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_mirror"); 
  soc_sand_proc_name = "soc_ppd_eg_mirror_port_vlan_get"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_MIRROR_PORT_VLAN_GET_PORT_VLAN_GET_OUT_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_MIRROR_PORT_VLAN_GET_PORT_VLAN_GET_OUT_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_out_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter out_port_ndx after port_vlan_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_MIRROR_PORT_VLAN_GET_PORT_VLAN_GET_VID_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_MIRROR_PORT_VLAN_GET_PORT_VLAN_GET_VID_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_vid_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter vid_ndx after port_vlan_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_eg_mirror_port_vlan_get(
          unit,
          prm_out_port_ndx,
          prm_vid_ndx,
          &prm_enable_mirror
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_mirror_port_vlan_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_mirror_port_vlan_get");   
    goto exit; 
  } 

  cli_out("enable_mirror: %u\n\r",prm_enable_mirror);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: port_dflt_set (section eg_mirror)
 ********************************************************************/
int 
  ui_ppd_api_eg_mirror_port_dflt_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPD_PORT   
    prm_local_port_ndx;
  SOC_PPD_EG_MIRROR_PORT_DFLT_INFO   
    prm_dflt_mirroring_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_mirror"); 
  soc_sand_proc_name = "soc_ppd_eg_mirror_port_dflt_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPD_EG_MIRROR_PORT_DFLT_INFO_clear(&prm_dflt_mirroring_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_MIRROR_PORT_DFLT_SET_PORT_DFLT_SET_LOCAL_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_MIRROR_PORT_DFLT_SET_PORT_DFLT_SET_LOCAL_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_local_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter local_port_ndx after port_dflt_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_eg_mirror_port_dflt_get(
          unit,
          prm_local_port_ndx,
          &prm_dflt_mirroring_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_mirror_port_dflt_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_mirror_port_dflt_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_MIRROR_PORT_DFLT_SET_PORT_DFLT_SET_DFLT_MIRRORING_INFO_ENABLE_MIRROR_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_MIRROR_PORT_DFLT_SET_PORT_DFLT_SET_DFLT_MIRRORING_INFO_ENABLE_MIRROR_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_dflt_mirroring_info.enable_mirror = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_eg_mirror_port_dflt_set(
          unit,
          prm_local_port_ndx,
          &prm_dflt_mirroring_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_mirror_port_dflt_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_mirror_port_dflt_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: port_dflt_get (section eg_mirror)
 ********************************************************************/
int 
  ui_ppd_api_eg_mirror_port_dflt_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPD_PORT   
    prm_local_port_ndx;
  SOC_PPD_EG_MIRROR_PORT_DFLT_INFO   
    prm_dflt_mirroring_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_mirror"); 
  soc_sand_proc_name = "soc_ppd_eg_mirror_port_dflt_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPD_EG_MIRROR_PORT_DFLT_INFO_clear(&prm_dflt_mirroring_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_MIRROR_PORT_DFLT_GET_PORT_DFLT_GET_LOCAL_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_EG_MIRROR_PORT_DFLT_GET_PORT_DFLT_GET_LOCAL_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_local_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter local_port_ndx after port_dflt_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_eg_mirror_port_dflt_get(
          unit,
          prm_local_port_ndx,
          &prm_dflt_mirroring_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_eg_mirror_port_dflt_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_eg_mirror_port_dflt_get");   
    goto exit; 
  } 

  SOC_PPD_EG_MIRROR_PORT_DFLT_INFO_print(&prm_dflt_mirroring_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
#endif
#ifdef UI_EG_MIRROR/* { eg_mirror*/
/******************************************************************** 
 *  Section handler: eg_mirror
 ********************************************************************/ 
int 
  ui_ppd_api_eg_mirror( 
    CURRENT_LINE *current_line 
  ) 
{   
  int
    ret = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_eg_mirror"); 
 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_MIRROR_PORT_VLAN_ADD_PORT_VLAN_ADD_ID,1)) 
  { 
    ret = ui_ppd_api_eg_mirror_port_vlan_add(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_MIRROR_PORT_VLAN_REMOVE_PORT_VLAN_REMOVE_ID,1)) 
  { 
    ret = ui_ppd_api_eg_mirror_port_vlan_remove(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_MIRROR_PORT_VLAN_GET_PORT_VLAN_GET_ID,1)) 
  { 
    ret = ui_ppd_api_eg_mirror_port_vlan_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_MIRROR_PORT_DFLT_SET_PORT_DFLT_SET_ID,1)) 
  { 
    ret = ui_ppd_api_eg_mirror_port_dflt_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_EG_MIRROR_PORT_DFLT_GET_PORT_DFLT_GET_ID,1)) 
  { 
    ret = ui_ppd_api_eg_mirror_port_dflt_get(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after eg_mirror***", TRUE); 
  } 
  
  ui_ret = ret; 
    
  goto exit; 
exit:        
  return ui_ret; 
}                
          
#endif /* eg_mirror */ 


#endif /* LINK_PPD_LIBRARIES */ 

