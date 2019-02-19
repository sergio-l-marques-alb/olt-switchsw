/* $Id: ui_pb_api_mgmt.c,v 1.6 Broadcom SDK $
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
 
  
#include <soc/dpp/Petra/PB_TM/pb_api_mgmt.h>
#include <appl/dpp/UserInterface/pb/ui_pb_api_mgmt.h>

#if LINK_PB_LIBRARIES

#ifdef UI_MGMT
/******************************************************************** 
 *  Function handler: rev_b0_set (section mgmt)
 ********************************************************************/
int 
  ui_pb_api_mgmt_rev_b0_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_MGMT_B0_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_mgmt"); 
  soc_sand_proc_name = "soc_pb_mgmt_rev_b0_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_MGMT_B0_INFO_clear(&prm_info);
 
  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_mgmt_rev_b0_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_mgmt_rev_b0_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_mgmt_rev_b0_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_MGMT_REV_B0_SET_REV_B0_SET_INFO_IS_FC_SCH_MODE_CBFC_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_MGMT_REV_B0_SET_REV_B0_SET_INFO_IS_FC_SCH_MODE_CBFC_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.is_fc_sch_mode_cbfc = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_MGMT_REV_B0_SET_REV_B0_SET_INFO_SINGLE_COPY_MC_RNG_BMP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_MGMT_REV_B0_SET_REV_B0_SET_INFO_SINGLE_COPY_MC_RNG_BMP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.single_copy_mc_rng_bmp = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_MGMT_REV_B0_SET_REV_B0_SET_INFO_TDM_MC_ROUTE_MODE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_MGMT_REV_B0_SET_REV_B0_SET_INFO_TDM_MC_ROUTE_MODE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.tdm_mc_route_mode = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_MGMT_REV_B0_SET_REV_B0_SET_INFO_INNER_ETH_ENCAP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_MGMT_REV_B0_SET_REV_B0_SET_INFO_INNER_ETH_ENCAP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.inner_eth_encap = (SOC_SAND_PP_ETH_ENCAP_TYPE)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_MGMT_REV_B0_SET_REV_B0_SET_INFO_BUGFIXES_ENABLED_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_MGMT_REV_B0_SET_REV_B0_SET_INFO_BUGFIXES_ENABLED_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.bugfixes_enabled = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_mgmt_rev_b0_set(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_mgmt_rev_b0_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_mgmt_rev_b0_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: rev_b0_get (section mgmt)
 ********************************************************************/
int 
  ui_pb_api_mgmt_rev_b0_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_MGMT_B0_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_mgmt"); 
  soc_sand_proc_name = "soc_pb_mgmt_rev_b0_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_MGMT_B0_INFO_clear(&prm_info);
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_pb_mgmt_rev_b0_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_mgmt_rev_b0_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_mgmt_rev_b0_get");   
    goto exit; 
  } 

  SOC_PB_MGMT_B0_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
#endif
#ifdef UI_MGMT/* { mgmt*/
/******************************************************************** 
 *  Section handler: mgmt
 ********************************************************************/ 
int 
  ui_pb_api_mgmt( 
    CURRENT_LINE *current_line 
  ) 
{   
  int
    ret = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_pb_api_mgmt"); 
 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_MGMT_REV_B0_SET_REV_B0_SET_ID,1)) 
  { 
    ret = ui_pb_api_mgmt_rev_b0_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_MGMT_REV_B0_GET_REV_B0_GET_ID,1)) 
  { 
    ret = ui_pb_api_mgmt_rev_b0_get(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after mgmt***", TRUE); 
  }
  
  ui_ret = ret; 
    
  goto exit; 
exit:        
  return ui_ret; 
}                
          
#endif /* mgmt */ 


#endif /* LINK_PB_LIBRARIES */ 

