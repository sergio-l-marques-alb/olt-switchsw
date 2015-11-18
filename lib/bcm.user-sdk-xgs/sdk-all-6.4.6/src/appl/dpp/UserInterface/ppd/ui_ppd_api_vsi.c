/* $Id: ui_ppd_api_vsi.c,v 1.6 Broadcom SDK $
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
 

  
#include <soc/dpp/PPD/ppd_api_vsi.h>                                                               
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_vsi.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_lif.h>

#if LINK_PPD_LIBRARIES

#ifdef UI_VSI
/********************************************************************
*  Function handler: map_add (section vsi)
********************************************************************/
int
ui_ppd_api_vsi_map_add(
                       CURRENT_LINE *current_line
                       )
{
  uint32
    ret;
  SOC_PPD_VSI_ID   
    prm_local_vsi_ndx;
  SOC_PPD_SYS_VSI_ID   
    prm_sys_vsid=0;
  SOC_PPD_VSI_ID   
    prm_eg_local_vsid=0;
  SOC_SAND_SUCCESS_FAILURE
    prm_success;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_vsi");
  soc_sand_proc_name = "soc_ppd_vsi_map_add";

  unit = soc_ppd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_VSI_MAP_ADD_MAP_ADD_LOCAL_VSI_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_VSI_MAP_ADD_MAP_ADD_LOCAL_VSI_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_local_vsi_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter local_vsi_ndx after map_add***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_VSI_MAP_ADD_MAP_ADD_SYS_VSID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_VSI_MAP_ADD_MAP_ADD_SYS_VSID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_vsid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_VSI_MAP_ADD_MAP_ADD_EG_LOCAL_VSID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_VSI_MAP_ADD_MAP_ADD_EG_LOCAL_VSID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_eg_local_vsid = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_vsi_map_add(
    unit,
    prm_local_vsi_ndx,
    prm_sys_vsid,
    prm_eg_local_vsid,
    &prm_success
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_vsi_map_add - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_vsi_map_add");
    goto exit;
  }

  send_string_to_screen("--> success:", TRUE);
  cli_out("success: %s\n",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: map_remove (section vsi)
********************************************************************/
int
ui_ppd_api_vsi_map_remove(
                          CURRENT_LINE *current_line
                          )
{
  uint32
    ret;
  SOC_PPD_VSI_ID   
    prm_local_vsi_ndx;
  SOC_PPD_SYS_VSI_ID   
    prm_sys_vsid=0;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_vsi");
  soc_sand_proc_name = "soc_ppd_vsi_map_remove";

  unit = soc_ppd_get_default_unit();

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_VSI_MAP_REMOVE_MAP_REMOVE_LOCAL_VSI_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_VSI_MAP_REMOVE_MAP_REMOVE_LOCAL_VSI_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_local_vsi_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter local_vsi_ndx after map_remove***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_VSI_MAP_REMOVE_MAP_REMOVE_SYS_VSID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_VSI_MAP_REMOVE_MAP_REMOVE_SYS_VSID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_sys_vsid = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_vsi_map_remove(
    unit,
    prm_local_vsi_ndx,
    prm_sys_vsid
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_vsi_map_remove - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_vsi_map_remove");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: default_frwrd_info_set (section vsi)
********************************************************************/
int
ui_ppd_api_vsi_default_frwrd_info_set(
                                      CURRENT_LINE *current_line
                                      )
{
  uint32
    ret;
  SOC_PPD_VSI_DEFAULT_FRWRD_KEY
    prm_dflt_frwrd_key;
  SOC_PPD_ACTION_PROFILE
    prm_action_profile;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_vsi");
  soc_sand_proc_name = "soc_ppd_vsi_default_frwrd_info_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPD_VSI_DEFAULT_FRWRD_KEY_clear(&prm_dflt_frwrd_key);
  SOC_PPD_ACTION_PROFILE_clear(&prm_action_profile);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_VSI_DEFAULT_FRWRD_INFO_SET_DEFAULT_FRWRD_INFO_SET_DFLT_FRWRD_KEY_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_VSI_DEFAULT_FRWRD_INFO_SET_DEFAULT_FRWRD_INFO_SET_DFLT_FRWRD_KEY_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_dflt_frwrd_key.default_forward_profile = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_VSI_DEFAULT_FRWRD_INFO_SET_DEFAULT_FRWRD_INFO_SET_DFLT_FRWRD_KEY_DA_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_VSI_DEFAULT_FRWRD_INFO_SET_DEFAULT_FRWRD_INFO_SET_DFLT_FRWRD_KEY_DA_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_dflt_frwrd_key.da_type = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_VSI_DEFAULT_FRWRD_INFO_SET_DEFAULT_FRWRD_INFO_SET_DFLT_FRWRD_KEY_ORIENTATION_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_VSI_DEFAULT_FRWRD_INFO_SET_DEFAULT_FRWRD_INFO_SET_DFLT_FRWRD_KEY_ORIENTATION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_dflt_frwrd_key.orientation = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter dflt_frwrd_key after default_frwrd_info_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_vsi_default_frwrd_info_get(
    unit,
    &prm_dflt_frwrd_key,
    &prm_action_profile
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_vsi_default_frwrd_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_vsi_default_frwrd_info_get");
    goto exit;
  }

  /*trap-code
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_VSI_DEFAULT_FRWRD_INFO_SET_DEFAULT_FRWRD_INFO_SET_ACTION_PROFILE_TRAP_CODE_LSB_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_VSI_DEFAULT_FRWRD_INFO_SET_DEFAULT_FRWRD_INFO_SET_ACTION_PROFILE_TRAP_CODE_LSB_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action_profile.trap_code = (SOC_PPD_TRAP_CODE)param_val->numeric_equivalent;
  }
  */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_VSI_DEFAULT_FRWRD_INFO_SET_DEFAULT_FRWRD_INFO_SET_ACTION_PROFILE_TRAP_CODE_LSB_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_VSI_DEFAULT_FRWRD_INFO_SET_DEFAULT_FRWRD_INFO_SET_ACTION_PROFILE_TRAP_CODE_LSB_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action_profile.trap_code = (SOC_PPD_TRAP_CODE)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_VSI_DEFAULT_FRWRD_INFO_SET_DEFAULT_FRWRD_INFO_SET_ACTION_PROFILE_FRWRD_STRENGTH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_VSI_DEFAULT_FRWRD_INFO_SET_DEFAULT_FRWRD_INFO_SET_ACTION_PROFILE_FRWRD_STRENGTH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action_profile.frwrd_action_strength = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_VSI_DEFAULT_FRWRD_INFO_SET_DEFAULT_FRWRD_INFO_SET_ACTION_PROFILE_SNOOP_STRENGTH_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_VSI_DEFAULT_FRWRD_INFO_SET_DEFAULT_FRWRD_INFO_SET_ACTION_PROFILE_SNOOP_STRENGTH_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_action_profile.snoop_action_strength = (uint32)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_vsi_default_frwrd_info_set(
    unit,
    &prm_dflt_frwrd_key,
    &prm_action_profile
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_vsi_default_frwrd_info_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_vsi_default_frwrd_info_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: default_frwrd_info_get (section vsi)
********************************************************************/
int
ui_ppd_api_vsi_default_frwrd_info_get(
                                      CURRENT_LINE *current_line
                                      )
{
  uint32
    ret;
  SOC_PPD_VSI_DEFAULT_FRWRD_KEY
    prm_dflt_frwrd_key;
  SOC_PPD_ACTION_PROFILE
    prm_action_profile;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_vsi");
  soc_sand_proc_name = "soc_ppd_vsi_default_frwrd_info_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPD_VSI_DEFAULT_FRWRD_KEY_clear(&prm_dflt_frwrd_key);
  SOC_PPD_ACTION_PROFILE_clear(&prm_action_profile);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_VSI_DEFAULT_FRWRD_INFO_GET_DEFAULT_FRWRD_INFO_GET_DFLT_FRWRD_KEY_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_VSI_DEFAULT_FRWRD_INFO_GET_DEFAULT_FRWRD_INFO_GET_DFLT_FRWRD_KEY_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_dflt_frwrd_key.default_forward_profile = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_VSI_DEFAULT_FRWRD_INFO_GET_DEFAULT_FRWRD_INFO_GET_DFLT_FRWRD_KEY_DA_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_VSI_DEFAULT_FRWRD_INFO_GET_DEFAULT_FRWRD_INFO_GET_DFLT_FRWRD_KEY_DA_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_dflt_frwrd_key.da_type = param_val->numeric_equivalent;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_VSI_DEFAULT_FRWRD_INFO_GET_DEFAULT_FRWRD_INFO_GET_DFLT_FRWRD_KEY_ORIENTATION_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_VSI_DEFAULT_FRWRD_INFO_GET_DEFAULT_FRWRD_INFO_GET_DFLT_FRWRD_KEY_ORIENTATION_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_dflt_frwrd_key.orientation = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter dflt_frwrd_key after default_frwrd_info_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_vsi_default_frwrd_info_get(
    unit,
    &prm_dflt_frwrd_key,
    &prm_action_profile
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_vsi_default_frwrd_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_vsi_default_frwrd_info_get");
    goto exit;
  }

  send_string_to_screen("--> action_profile:", TRUE);
  SOC_PPD_ACTION_PROFILE_print(&prm_action_profile);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: info_set (section vsi)
********************************************************************/
int
ui_ppd_api_vsi_info_set(
                        CURRENT_LINE *current_line
                        )
{
  uint32
    ret;
  SOC_PPD_VSI_ID   
    prm_vsi_ndx;
  SOC_PPD_VSI_INFO
    prm_vsi_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_vsi");
  soc_sand_proc_name = "soc_ppd_vsi_info_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPD_VSI_INFO_clear(&prm_vsi_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_VSI_INFO_SET_INFO_SET_VSI_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_VSI_INFO_SET_INFO_SET_VSI_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsi_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vsi_ndx after info_set***", TRUE);
    goto exit;
  }

  /* This is a set function, so call GET function first */
  ret = soc_ppd_vsi_info_get(
    unit,
    prm_vsi_ndx,
    &prm_vsi_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_vsi_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_vsi_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_VSI_INFO_SET_INFO_SET_VSI_INFO_LIMIT_PROFILE_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_VSI_INFO_SET_INFO_SET_VSI_INFO_LIMIT_PROFILE_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsi_info.mac_learn_profile_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_VSI_INFO_SET_INFO_SET_VSI_INFO_FID_PROFILE_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_VSI_INFO_SET_INFO_SET_VSI_INFO_FID_PROFILE_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsi_info.fid_profile_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_VSI_INFO_SET_INFO_SET_VSI_INFO_ENABLE_MY_MAC_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_VSI_INFO_SET_INFO_SET_VSI_INFO_ENABLE_MY_MAC_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsi_info.enable_my_mac = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_VSI_INFO_SET_INFO_SET_VSI_INFO_STP_TOPOLOGY_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_VSI_INFO_SET_INFO_SET_VSI_INFO_STP_TOPOLOGY_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsi_info.stp_topology_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_DECISION_INFO_ADDITIONAL_INFO_TRAP_INFO_TRAP_ACTION_PROFILE_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_DECISION_INFO_ADDITIONAL_INFO_TRAP_INFO_TRAP_ACTION_PROFILE_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsi_info.default_forward_profile = (uint32)param_val->value.ulong_value;
  }

  ret = ui_ppd_frwrd_decision_set(
          current_line,
          1,
          &(prm_vsi_info.default_forwarding)
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** ui_ppd_frwrd_decision_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "ui_ppd_frwrd_decision_set");
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_vsi_info_set(
    unit,
    prm_vsi_ndx,
    &prm_vsi_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_vsi_info_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_vsi_info_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: info_get (section vsi)
********************************************************************/
int
ui_ppd_api_vsi_info_get(
                        CURRENT_LINE *current_line
                        )
{
  uint32
    ret;
  SOC_PPD_VSI_ID   
    prm_vsi_ndx;
  SOC_PPD_VSI_INFO
    prm_vsi_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_vsi");
  soc_sand_proc_name = "soc_ppd_vsi_info_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPD_VSI_INFO_clear(&prm_vsi_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_VSI_INFO_GET_INFO_GET_VSI_NDX_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_VSI_INFO_GET_INFO_GET_VSI_NDX_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_vsi_ndx = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter vsi_ndx after info_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_vsi_info_get(
    unit,
    prm_vsi_ndx,
    &prm_vsi_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_vsi_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_vsi_info_get");
    goto exit;
  }

  send_string_to_screen("--> vsi_info:", TRUE);
  SOC_PPD_VSI_INFO_print(&prm_vsi_info);

  goto exit;
exit:
  return ui_ret;
}


/********************************************************************
*  Function handler: egress_mtu_set (section vsi)
********************************************************************/
int
ui_ppd_api_vsi_egress_mtu_set(
                        CURRENT_LINE *current_line
                        )
{   
  uint32 
    ret;   
  uint32   
    prm_vsi_profile_ndx;
  uint32   
    prm_mtu_val;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_vsi"); 
  soc_sand_proc_name = "soc_ppd_vsi_egress_mtu_set"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_VSI_EGRESS_MTU_SET_VSI_EGRESS_MTU_SET_VSI_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_VSI_EGRESS_MTU_SET_VSI_EGRESS_MTU_SET_VSI_PROFILE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_vsi_profile_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter vsi_ndx after vsi_egress_mtu_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_vsi_egress_mtu_get(
          unit,
          prm_vsi_profile_ndx,
          &prm_mtu_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_vsi_egress_mtu_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_vsi_egress_mtu_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_VSI_EGRESS_MTU_SET_VSI_EGRESS_MTU_SET_MTU_VAL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_VSI_EGRESS_MTU_SET_VSI_EGRESS_MTU_SET_MTU_VAL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_mtu_val = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_vsi_egress_mtu_set(
          unit,
          prm_vsi_profile_ndx,
          prm_mtu_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_vsi_egress_mtu_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_vsi_egress_mtu_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: egress_mtu_get (section vsi)
 ********************************************************************/
int 
  ui_ppd_api_vsi_egress_mtu_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_vsi_profile_ndx;
  uint32   
    prm_mtu_val;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_vsi"); 
  soc_sand_proc_name = "soc_ppd_vsi_egress_mtu_get"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_VSI_EGRESS_MTU_GET_VSI_EGRESS_MTU_GET_VSI_PROFILE_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_VSI_EGRESS_MTU_GET_VSI_EGRESS_MTU_GET_VSI_PROFILE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_vsi_profile_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter ttl_scope_ndx after ttl_scope_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_ppd_vsi_egress_mtu_get(
          unit,
          prm_vsi_profile_ndx,
          &prm_mtu_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_vsi_egress_mtu_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_vsi_egress_mtu_get");   
    goto exit; 
  } 

  cli_out("mtu_val: %u\n\r",prm_mtu_val);

  
  goto exit; 
exit: 
  return ui_ret; 
} 

/********************************************************************
*  Section handler: vsi
********************************************************************/
int
ui_ppd_api_vsi(
               CURRENT_LINE *current_line
               )
{
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_vsi");

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_VSI_MAP_ADD_MAP_ADD_ID,1))
  {
    ui_ret = ui_ppd_api_vsi_map_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_VSI_MAP_REMOVE_MAP_REMOVE_ID,1))
  {
    ui_ret = ui_ppd_api_vsi_map_remove(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_VSI_DEFAULT_FRWRD_INFO_SET_DEFAULT_FRWRD_INFO_SET_ID,1))
  {
    ui_ret = ui_ppd_api_vsi_default_frwrd_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_VSI_DEFAULT_FRWRD_INFO_GET_DEFAULT_FRWRD_INFO_GET_ID,1))
  {
    ui_ret = ui_ppd_api_vsi_default_frwrd_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_VSI_INFO_SET_INFO_SET_ID,1))
  {
    ui_ret = ui_ppd_api_vsi_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_VSI_INFO_GET_INFO_GET_ID,1))
  {
    ui_ret = ui_ppd_api_vsi_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_VSI_EGRESS_MTU_SET_VSI_EGRESS_MTU_SET_ID,1))
  {
    ui_ret = ui_ppd_api_vsi_egress_mtu_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_VSI_EGRESS_MTU_GET_VSI_EGRESS_MTU_GET_ID,1))
  {
    ui_ret = ui_ppd_api_vsi_egress_mtu_get(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after vsi***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}


#endif
#endif /* LINK_PPD_LIBRARIES */ 

