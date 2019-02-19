/* $Id: ui_pb_pp_api_mgmt.c,v 1.6 Broadcom SDK $
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

#if LINK_PB_PP_LIBRARIES

#include <soc/dpp/Petra/PB_PP/pb_pp_api_mgmt.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_mgmt.h>
#include <appl/dpp/UserInterface/pb/ui_pb_pp_api_mgmt.h>



#ifdef UI_PP_MGMT
/******************************************************************** 
 *  Function handler: elk_mode_set (section pp_mgmt)
 ********************************************************************/
int 
  ui_pb_pp_mgmt_elk_mode_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_PP_MGMT_ELK_MODE 
    prm_info_elk_mode = 0;
  uint32           
    ingress_pkt_rate = 0;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_pp_api_mgmt"); 
  soc_sand_proc_name = "soc_pb_pp_mgmt_elk_mode_set"; 
 
  unit = soc_pb_get_default_unit(); 
 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PP_MGMT_ELK_MODE_SET_ELK_MODE_SET_ELK_MODE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PP_MGMT_ELK_MODE_SET_ELK_MODE_SET_ELK_MODE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info_elk_mode = (SOC_PB_PP_MGMT_ELK_MODE)param_val->numeric_equivalent;
  }

  /* Call function */
  ret = soc_pb_pp_mgmt_elk_mode_set(
          unit,
          prm_info_elk_mode,
          &ingress_pkt_rate
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_pp_mgmt_elk_mode_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_pp_mgmt_elk_mode_set");   
    goto exit; 
  }

  soc_sand_os_printf("ingress_pkt_rate=%d\n",ingress_pkt_rate);
  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: elk_mode_get (section pp_mgmt)
 ********************************************************************/
int 
  ui_pb_pp_mgmt_elk_mode_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_PP_MGMT_ELK_MODE 
    prm_info_elk_mode = 0;
  uint32           
    ingress_pkt_rate = 0;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_pp_api_mgmt"); 
  soc_sand_proc_name = "soc_pb_pp_mgmt_elk_mode_get"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_pb_pp_mgmt_elk_mode_get(
          unit,
          &prm_info_elk_mode,
          &ingress_pkt_rate
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_mgmt_rev_b0_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_mgmt_rev_b0_get");   
    goto exit; 
  } 

  SOC_PB_PP_MGMT_ELK_MODE_INFO_print(prm_info_elk_mode);
  soc_sand_os_printf("ingress_pkt_rate=%d\n",ingress_pkt_rate);

  
  goto exit; 
exit: 
  return ui_ret; 
}

/******************************************************************** 
 *  Function handler: use_elk_set (section pp_mgmt)
 ********************************************************************/
int 
  ui_pb_pp_mgmt_use_elk_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_PP_MGMT_LKP_TYPE 
    prm_info_lkp_type = 0;
  uint8           
    prm_info_use_elk = 0;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_pp_api_mgmt"); 
  soc_sand_proc_name = "soc_pb_pp_mgmt_use_elk_set"; 
 
  unit = soc_pb_get_default_unit(); 
 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PP_MGMT_USE_ELK_SET_USE_ELK_SET_LKP_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PP_MGMT_USE_ELK_SET_USE_ELK_SET_LKP_TYPE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info_lkp_type = (SOC_PB_PP_MGMT_LKP_TYPE)param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PP_MGMT_USE_ELK_SET_USE_ELK_SET_USE_ELK_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_PP_MGMT_USE_ELK_SET_USE_ELK_SET_USE_ELK_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info_use_elk = (uint8)param_val->value.ulong_value;
  } 

  /* Call function */
  ret = soc_pb_pp_mgmt_use_elk_set_unsafe(
          unit,
          prm_info_lkp_type,
          prm_info_use_elk
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_pp_mgmt_use_elk_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_pp_mgmt_use_elk_set");   
    goto exit; 
  }

  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: use_elk_get (section pp_mgmt)
 ********************************************************************/
int 
  ui_pb_pp_mgmt_use_elk_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_PP_MGMT_LKP_TYPE 
    prm_info_lkp_type = 0;
  uint8           
    prm_info_use_elk = 0;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_pp_api_mgmt"); 
  soc_sand_proc_name = "soc_pb_pp_mgmt_use_elk_get"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PP_MGMT_USE_ELK_GET_USE_ELK_GET_LKP_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_PP_MGMT_USE_ELK_GET_USE_ELK_GET_LKP_TYPE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info_lkp_type = (SOC_PB_PP_MGMT_LKP_TYPE)param_val->numeric_equivalent;
  }

  /* Call function */
  ret = soc_pb_pp_mgmt_use_elk_get(
          unit,
          prm_info_lkp_type,
          &prm_info_use_elk
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_mgmt_rev_b0_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_mgmt_rev_b0_get");   
    goto exit; 
  } 

  soc_sand_os_printf("use_elk=%d\n",prm_info_use_elk);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
#endif
#ifdef UI_PP_MGMT/* { pp_mgmt*/
/******************************************************************** 
 *  Section handler: pp_mgmt
 ********************************************************************/ 
int 
  ui_pb_pp_api_mgmt( 
    CURRENT_LINE *current_line 
  ) 
{   
  int
    ret = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_pb_pp_api_mgmt"); 
 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PP_MGMT_ELK_MODE_SET_ELK_MODE_SET_ID,1)) 
  { 
    ret = ui_pb_pp_mgmt_elk_mode_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PP_MGMT_ELK_MODE_GET_ELK_MODE_GET_ID,1)) 
  { 
    ret = ui_pb_pp_mgmt_elk_mode_get(current_line); 
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PP_MGMT_USE_ELK_SET_USE_ELK_SET_ID,1)) 
  { 
    ret = ui_pb_pp_mgmt_use_elk_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_PP_MGMT_USE_ELK_GET_USE_ELK_GET_ID,1)) 
  { 
    ret = ui_pb_pp_mgmt_use_elk_get(current_line); 
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
          
#endif /* pp_mgmt */ 


#endif /* LINK_PB_PP_LIBRARIES */ 

