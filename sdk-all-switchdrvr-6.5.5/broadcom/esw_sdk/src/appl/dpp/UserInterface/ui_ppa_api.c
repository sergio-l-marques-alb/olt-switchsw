/* $Id: ui_ppa_api.c,v 1.4 Broadcom SDK $
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
 * General include file for reference design.
 */

/*
 * Utilities include file.
 */

#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <appl/diag/dpp/utils_defi.h>
#include <appl/dpp/UserInterface/ui_defi.h>

#include <appl/dpp/UserInterface/ui_pure_defi_ppa_api.h>

#include <appl/dpp/PPA/ppa_bridge_router_app.h>
#include <appl/dpp/PPA/ppa_vpls_app.h>

int 
  ui_ppa_bridge_router_app(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  PPA_BRIDGE_ROUTER_APP_INFO
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppa_bridge_router_app"); 
  soc_sand_proc_name = "ppa_bridge_router_app"; 
 
  prm_info.flow_based_mode = FALSE;
  prm_info.nof_vd = 1;

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PPA_BRIDGE_ROUTER_APP_FLOW_BASED_MODE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PPA_BRIDGE_ROUTER_APP_FLOW_BASED_MODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.flow_based_mode = (uint8)param_val->value.ulong_value;
  }

  /* This is a set function, so call GET function first */                                                                                
  ret = ppa_bridge_router_app(
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** ppa_bridge_router_app - FAIL", TRUE); 
    goto exit; 
  } 

  goto exit; 
exit: 
  return ui_ret; 
} 

int 
  ui_ppa_vpls_app(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  PPA_VPLS_APP_INFO    
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppa_vpls_app"); 
  soc_sand_proc_name = "ppa_vpls_app"; 
 
  prm_info.flow_based_mode = FALSE;
  prm_info.nof_mp_services = PPA_VPLS_APP_MP_NOF_SERVICES;
  prm_info.nof_p2p_services = PPA_VPLS_APP_P2P_NOF_SERVICES;
  prm_info.oam_app = FALSE;
  prm_info.interrupt_not_poll = TRUE;

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PPA_VPLS_APP_FLOW_BASED_MODE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PPA_VPLS_APP_FLOW_BASED_MODE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.flow_based_mode = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PPA_VPLS_APP_POLL_INTERRUPT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PPA_VPLS_APP_POLL_INTERRUPT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.interrupt_not_poll = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PPA_VPLS_APP_OAM_APP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PPA_VPLS_APP_OAM_APP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.oam_app = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PPA_VPLS_APP_NOF_MP_SERVICES_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PPA_VPLS_APP_NOF_MP_SERVICES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.nof_mp_services = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PPA_VPLS_APP_NOF_P2P_SERVICES_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PPA_VPLS_APP_NOF_P2P_SERVICES_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_info.nof_p2p_services = (uint32)param_val->value.ulong_value;
  }

  /* This is a set function, so call GET function first */                                                                                
  ret = ppa_vpls_app(
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** ppa_vpls_app - FAIL", TRUE); 
    goto exit; 
  } 

  goto exit; 
exit: 
  return ui_ret; 
} 

