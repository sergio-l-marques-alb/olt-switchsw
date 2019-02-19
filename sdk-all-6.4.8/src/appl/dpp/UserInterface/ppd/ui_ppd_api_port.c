/* $Id: ui_ppd_api_port.c,v 1.9 Broadcom SDK $
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
#include <shared/bsl.h> 
#include <soc/dpp/SAND/Utils/sand_os_interface.h> 
#include <appl/diag/dpp/utils_defi.h> 
#include <appl/dpp/UserInterface/ui_defi.h> 
 
#include <appl/dpp/UserInterface/ui_pure_defi_ppd_api.h> 
 
  
#include <soc/dpp/PPD/ppd_api_port.h>
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_port.h>

#if LINK_PPD_LIBRARIES

#ifdef UI_PORT
/******************************************************************** 
 *  Function handler: info_set (section port)
 ********************************************************************/
int 
  ui_ppd_api_port_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPD_PORT   
    prm_local_port_ndx;
  SOC_PPD_PORT_INFO   
    prm_port_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_port"); 
  soc_sand_proc_name = "soc_ppd_port_info_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPD_PORT_INFO_clear(&prm_port_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_LOCAL_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_LOCAL_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_local_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter local_port_ndx after info_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_port_info_get(
          unit,
          SOC_CORE_DEFAULT,
          prm_local_port_ndx,
          &prm_port_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_port_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_port_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_MTU_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_MTU_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.mtu = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_NOT_AUTHORIZED_802_1X_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_NOT_AUTHORIZED_802_1X_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.not_authorized_802_1x = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_EXTEND_P2P_INFO_ENABLE_OUTGOING_EXTEND_P2P_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_EXTEND_P2P_INFO_ENABLE_OUTGOING_EXTEND_P2P_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.extend_p2p_info.enable_outgoing_extend_p2p = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_EXTEND_P2P_INFO_ENABLE_INCOMING_EXTEND_P2P_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_EXTEND_P2P_INFO_ENABLE_INCOMING_EXTEND_P2P_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.extend_p2p_info.enable_incoming_extend_p2p = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_ETHER_TYPE_BASED_PROFILE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_ETHER_TYPE_BASED_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.ether_type_based_profile = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_ORIENTATION_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_ORIENTATION_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.orientation = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_SAME_INTERFACE_FILTER_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_SAME_INTERFACE_FILTER_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.enable_same_interfac_filter = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_IS_LEARN_AC_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_IS_LEARN_AC_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.is_learn_ac = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_ENABLE_LEARNING_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_ENABLE_LEARNING_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.enable_learning = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_IS_PBP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_IS_PBP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.is_pbp = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_VLAN_TRANSLATION_PROFILE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_VLAN_TRANSLATION_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.vlan_translation_profile = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_TUNNEL_TERMINATION_PROFILE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_TUNNEL_TERMINATION_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.tunnel_termination_profile = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_INITIAL_ACTION_PROFILE_SNOOP_ACTION_STRENGTH_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_INITIAL_ACTION_PROFILE_SNOOP_ACTION_STRENGTH_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.initial_action_profile.snoop_action_strength = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_INITIAL_ACTION_PROFILE_FRWRD_ACTION_STRENGTH_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_INITIAL_ACTION_PROFILE_FRWRD_ACTION_STRENGTH_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.initial_action_profile.frwrd_action_strength = (uint32)param_val->value.ulong_value;
  } 

  /*trap-code
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_INITIAL_ACTION_PROFILE_TRAP_CODE_LSB_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_INITIAL_ACTION_PROFILE_TRAP_CODE_LSB_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.initial_action_profile.trap_code = (SOC_PPD_TRAP_CODE)param_val->numeric_equivalent;
  } 
  */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_INITIAL_ACTION_PROFILE_TRAP_CODE_LSB_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_INITIAL_ACTION_PROFILE_TRAP_CODE_LSB_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.initial_action_profile.trap_code = (SOC_PPD_TRAP_CODE)param_val->value.ulong_value;
  } 


  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_PORT_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_PORT_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.port_type = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_VLAN_DOMAIN_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_VLAN_DOMAIN_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.vlan_domain = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_TPID_PROFILE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_TPID_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.tpid_profile = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_DA_NOT_FOUND_PROFILE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_DA_NOT_FOUND_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.da_not_found_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_PORT_PROFILE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_PORT_INFO_PORT_PROFILE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_port_info.port_profile = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_port_info_set(
          unit,
		  SOC_CORE_DEFAULT,
          prm_local_port_ndx,
          &prm_port_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_port_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_port_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: info_get (section port)
 ********************************************************************/
int 
  ui_ppd_api_port_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPD_PORT   
    prm_local_port_ndx;
  SOC_PPD_PORT_INFO   
    prm_port_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_port"); 
  soc_sand_proc_name = "soc_ppd_port_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPD_PORT_INFO_clear(&prm_port_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_GET_INFO_GET_LOCAL_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_INFO_GET_INFO_GET_LOCAL_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_local_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter local_port_ndx after info_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_port_info_get(
          unit,
          SOC_CORE_DEFAULT,
          prm_local_port_ndx,
          &prm_port_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_port_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_port_info_get");   
    goto exit; 
  } 

  SOC_PPD_PORT_INFO_print(&prm_port_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: stp_state_set (section port)
 ********************************************************************/
int 
  ui_ppd_api_port_stp_state_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPD_PORT   
    prm_local_port_ndx;
  uint32   
    prm_topology_id_ndx;
  SOC_PPD_PORT_STP_STATE   
    prm_stp_state;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_port"); 
  soc_sand_proc_name = "soc_ppd_port_stp_state_set"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_STP_STATE_SET_STP_STATE_SET_LOCAL_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_STP_STATE_SET_STP_STATE_SET_LOCAL_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_local_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter local_port_ndx after stp_state_set***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_STP_STATE_SET_STP_STATE_SET_TOPOLOGY_ID_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_STP_STATE_SET_STP_STATE_SET_TOPOLOGY_ID_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_topology_id_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter topology_id_ndx after stp_state_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_port_stp_state_get(
          unit,
          SOC_CORE_DEFAULT,
          prm_local_port_ndx,
          prm_topology_id_ndx,
          &prm_stp_state
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_port_stp_state_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_port_stp_state_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_STP_STATE_SET_STP_STATE_SET_STP_STATE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_PORT_STP_STATE_SET_STP_STATE_SET_STP_STATE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_stp_state = param_val->numeric_equivalent;
  } 


  /* Call function */
  ret = soc_ppd_port_stp_state_set(
          unit,
          SOC_CORE_DEFAULT,
          prm_local_port_ndx,
          prm_topology_id_ndx,
          prm_stp_state
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_port_stp_state_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_port_stp_state_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: stp_state_get (section port)
 ********************************************************************/
int 
  ui_ppd_api_port_stp_state_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPD_PORT   
    prm_local_port_ndx;
  uint32   
    prm_topology_id_ndx;
  SOC_PPD_PORT_STP_STATE   
    prm_stp_state;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_port"); 
  soc_sand_proc_name = "soc_ppd_port_stp_state_get"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_STP_STATE_GET_STP_STATE_GET_LOCAL_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_STP_STATE_GET_STP_STATE_GET_LOCAL_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_local_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter local_port_ndx after stp_state_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_STP_STATE_GET_STP_STATE_GET_TOPOLOGY_ID_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_STP_STATE_GET_STP_STATE_GET_TOPOLOGY_ID_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_topology_id_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter topology_id_ndx after stp_state_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_port_stp_state_get(
          unit,
          SOC_CORE_DEFAULT,
          prm_local_port_ndx,
          prm_topology_id_ndx,
          &prm_stp_state
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_port_stp_state_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_port_stp_state_get");   
    goto exit; 
  } 

  cli_out("stp_state: %s\n\r",SOC_PPD_PORT_STP_STATE_to_string(prm_stp_state));

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: local_port_to_sys_phy_map_set (section port)
 ********************************************************************/
int 
  ui_ppd_api_port_local_port_to_sys_phy_map_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPD_PORT   
    prm_local_port_ndx;
  uint32   
    prm_sys_phy_port_id;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_port"); 
  soc_sand_proc_name = "soc_ppd_port_local_port_to_sys_phy_map_set"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_LOCAL_PORT_TO_SYS_PHY_MAP_SET_LOCAL_PORT_TO_SYS_PHY_MAP_SET_LOCAL_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_LOCAL_PORT_TO_SYS_PHY_MAP_SET_LOCAL_PORT_TO_SYS_PHY_MAP_SET_LOCAL_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_local_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter local_port_ndx after local_port_to_sys_phy_map_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_port_local_port_to_sys_phy_map_get(
          unit,
          prm_local_port_ndx,
          &prm_sys_phy_port_id
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_port_local_port_to_sys_phy_map_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_port_local_port_to_sys_phy_map_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_LOCAL_PORT_TO_SYS_PHY_MAP_SET_LOCAL_PORT_TO_SYS_PHY_MAP_SET_SYS_PHY_PORT_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_LOCAL_PORT_TO_SYS_PHY_MAP_SET_LOCAL_PORT_TO_SYS_PHY_MAP_SET_SYS_PHY_PORT_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_sys_phy_port_id = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_port_local_port_to_sys_phy_map_set(
          unit,
          prm_local_port_ndx,
          prm_sys_phy_port_id
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_port_local_port_to_sys_phy_map_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_port_local_port_to_sys_phy_map_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: local_port_to_sys_phy_map_get (section port)
 ********************************************************************/
int 
  ui_ppd_api_port_local_port_to_sys_phy_map_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPD_PORT   
    prm_local_port_ndx;
  uint32   
    prm_sys_phy_port_id;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_port"); 
  soc_sand_proc_name = "soc_ppd_port_local_port_to_sys_phy_map_get"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_LOCAL_PORT_TO_SYS_PHY_MAP_GET_LOCAL_PORT_TO_SYS_PHY_MAP_GET_LOCAL_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_LOCAL_PORT_TO_SYS_PHY_MAP_GET_LOCAL_PORT_TO_SYS_PHY_MAP_GET_LOCAL_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_local_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter local_port_ndx after local_port_to_sys_phy_map_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_port_local_port_to_sys_phy_map_get(
          unit,
          prm_local_port_ndx,
          &prm_sys_phy_port_id
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_port_local_port_to_sys_phy_map_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_port_local_port_to_sys_phy_map_get");   
    goto exit; 
  } 

  cli_out("sys_phy_port_id: %u\n\r",prm_sys_phy_port_id);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: tm_to_pp_map_set (section port)
 ********************************************************************/
int 
  ui_ppd_api_port_tm_to_pp_map_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPD_TM_PORT   
    prm_local_tm_port_ndx;
  SOC_PPD_PORT_DIRECTION   
    prm_direction_ndx=3;
  SOC_PPD_PORT   
    prm_local_pp_port_in,
    prm_local_pp_port_out;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_port"); 
  soc_sand_proc_name = "soc_ppd_port_tm_to_pp_map_set"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_TM_TO_PP_MAP_SET_TM_TO_PP_MAP_SET_LOCAL_TM_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_TM_TO_PP_MAP_SET_TM_TO_PP_MAP_SET_LOCAL_TM_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_local_tm_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter local_tm_port_ndx after tm_to_pp_map_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_port_tm_to_pp_map_get(
          unit,
          prm_local_tm_port_ndx,
          &prm_local_pp_port_in,
          &prm_local_pp_port_out
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_port_tm_to_pp_map_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_port_tm_to_pp_map_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_TM_TO_PP_MAP_SET_TM_TO_PP_MAP_SET_DIRECTION_NDX_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_PORT_TM_TO_PP_MAP_SET_TM_TO_PP_MAP_SET_DIRECTION_NDX_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_direction_ndx = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_TM_TO_PP_MAP_SET_TM_TO_PP_MAP_SET_LOCAL_PP_PORT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_TM_TO_PP_MAP_SET_TM_TO_PP_MAP_SET_LOCAL_PP_PORT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_local_pp_port_in = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_port_tm_to_pp_map_set(
          unit,
          prm_local_tm_port_ndx,
          prm_direction_ndx,
          prm_local_pp_port_in
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_port_tm_to_pp_map_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_port_tm_to_pp_map_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: tm_to_pp_map_get (section port)
 ********************************************************************/
int 
  ui_ppd_api_port_tm_to_pp_map_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPD_TM_PORT   
    prm_local_tm_port_ndx;
  SOC_PPD_PORT   
    prm_local_pp_port_in,
    prm_local_pp_port_out;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_port"); 
  soc_sand_proc_name = "soc_ppd_port_tm_to_pp_map_get"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_TM_TO_PP_MAP_GET_TM_TO_PP_MAP_GET_LOCAL_TM_PORT_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_PORT_TM_TO_PP_MAP_GET_TM_TO_PP_MAP_GET_LOCAL_TM_PORT_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_local_tm_port_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter local_tm_port_ndx after tm_to_pp_map_get***", TRUE); 
    goto exit; 
  } 

  /* Call function */
  ret = soc_ppd_port_tm_to_pp_map_get(
          unit,
          prm_local_tm_port_ndx,
          &prm_local_pp_port_in,
          &prm_local_pp_port_out
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_port_tm_to_pp_map_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_port_tm_to_pp_map_get");   
    goto exit; 
  } 

  cli_out("incoming local pp_port: %u\n\r",prm_local_pp_port_in);
  cli_out("outgoing local pp_port: %u\n\r",prm_local_pp_port_out);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
#endif
#ifdef UI_PORT/* { port*/
/******************************************************************** 
 *  Section handler: port
 ********************************************************************/ 
int 
  ui_ppd_api_port( 
    CURRENT_LINE *current_line 
  ) 
{   
  int
    ret = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_port"); 
 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_SET_INFO_SET_ID,1)) 
  { 
    ret = ui_ppd_api_port_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_INFO_GET_INFO_GET_ID,1)) 
  { 
    ret = ui_ppd_api_port_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_STP_STATE_SET_STP_STATE_SET_ID,1)) 
  { 
    ret = ui_ppd_api_port_stp_state_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_STP_STATE_GET_STP_STATE_GET_ID,1)) 
  { 
    ret = ui_ppd_api_port_stp_state_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_LOCAL_PORT_TO_SYS_PHY_MAP_SET_LOCAL_PORT_TO_SYS_PHY_MAP_SET_ID,1)) 
  { 
    ret = ui_ppd_api_port_local_port_to_sys_phy_map_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_LOCAL_PORT_TO_SYS_PHY_MAP_GET_LOCAL_PORT_TO_SYS_PHY_MAP_GET_ID,1)) 
  { 
    ret = ui_ppd_api_port_local_port_to_sys_phy_map_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_TM_TO_PP_MAP_SET_TM_TO_PP_MAP_SET_ID,1)) 
  { 
    ret = ui_ppd_api_port_tm_to_pp_map_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_PORT_TM_TO_PP_MAP_GET_TM_TO_PP_MAP_GET_ID,1)) 
  { 
    ret = ui_ppd_api_port_tm_to_pp_map_get(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after port***", TRUE); 
  }
  
  ui_ret = ret; 
    
  goto exit; 
exit:        
  return ui_ret; 
}                
          
#endif /* port */ 


#endif /* LINK_PPD_LIBRARIES */ 

