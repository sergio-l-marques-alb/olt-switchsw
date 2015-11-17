/* $Id: ui_ppd_api_frwrd_extend_p2p.c,v 1.5 Broadcom SDK $
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
 

#include <soc/dpp/PPD/ppd_api_frwrd_extend_p2p.h>                                                               
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_frwrd_extend_p2p.h>

#include <appl/dpp/UserInterface/ppd/ui_ppd_api_lif.h>


#if LINK_PPD_LIBRARIES

#ifdef UI_FRWRD_EXTEND_P2P
/********************************************************************
*  Function handler: glbl_info_set (section frwrd_extend_p2p)
********************************************************************/
int
ui_ppd_api_frwrd_extend_p2p_glbl_info_set(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_PPD_FRWRD_EXTEND_P2P_GLBL_INFO
    prm_glbl_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_extend_p2p");
  soc_sand_proc_name = "soc_ppd_frwrd_extend_p2p_glbl_info_set";

  unit = soc_ppd_get_default_unit();
  SOC_PPD_FRWRD_EXTEND_P2P_GLBL_INFO_clear(&prm_glbl_info);

  /* Get parameters */

  /* This is a set function, so call GET function first */
  ret = soc_ppd_frwrd_extend_p2p_glbl_info_get(
    unit,
    &prm_glbl_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_extend_p2p_glbl_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_extend_p2p_glbl_info_get");
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_PWE_INFO_SYSTEM_VSI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_EXTEND_P2P_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_PWE_INFO_SYSTEM_VSI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_glbl_info.pwe_info.system_vsi = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_AC_INFO_SYSTEM_VSI_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_EXTEND_P2P_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_AC_INFO_SYSTEM_VSI_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_glbl_info.ac_info.system_vsi = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_ENABLE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_EXTEND_P2P_GLBL_INFO_SET_GLBL_INFO_SET_GLBL_INFO_ENABLE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_glbl_info.enable = (uint8)param_val->value.ulong_value;
  }

  /* Call function */
  ret = soc_ppd_frwrd_extend_p2p_glbl_info_set(
    unit,
    &prm_glbl_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_extend_p2p_glbl_info_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_extend_p2p_glbl_info_set");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: glbl_info_get (section frwrd_extend_p2p)
********************************************************************/
int
ui_ppd_api_frwrd_extend_p2p_glbl_info_get(
  CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  SOC_PPD_FRWRD_EXTEND_P2P_GLBL_INFO
    prm_glbl_info;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_extend_p2p");
  soc_sand_proc_name = "soc_ppd_frwrd_extend_p2p_glbl_info_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPD_FRWRD_EXTEND_P2P_GLBL_INFO_clear(&prm_glbl_info);

  /* Get parameters */

  /* Call function */
  ret = soc_ppd_frwrd_extend_p2p_glbl_info_get(
    unit,
    &prm_glbl_info
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_extend_p2p_glbl_info_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_extend_p2p_glbl_info_get");
    goto exit;
  }

  send_string_to_screen("--> glbl_info:", TRUE);
  SOC_PPD_FRWRD_EXTEND_P2P_GLBL_INFO_print(&prm_glbl_info);

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: ac_add (section frwrd_extend_p2p)
********************************************************************/
int
ui_ppd_api_frwrd_extend_p2p_ac_add(
                                   CURRENT_LINE *current_line
                                   )
{
  uint32
    ret;
  SOC_PPD_L2_LIF_AC_KEY
    prm_ac_key;
  SOC_PPD_FRWRD_EXTEND_P2P_FRWRD_INFO
    prm_ac_p2p_info;
  SOC_SAND_SUCCESS_FAILURE
    prm_success;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_extend_p2p");
  soc_sand_proc_name = "soc_ppd_frwrd_extend_p2p_ac_add";

  unit = soc_ppd_get_default_unit();
  SOC_PPD_L2_LIF_AC_KEY_clear(&prm_ac_key);
  SOC_PPD_FRWRD_EXTEND_P2P_FRWRD_INFO_clear(&prm_ac_p2p_info);

  prm_ac_key.inner_vid = SOC_PPD_LIF_IGNORE_INNER_VID;
  prm_ac_key.outer_vid = SOC_PPD_LIF_IGNORE_OUTER_VID;

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_AC_ADD_AC_ADD_AC_KEY_INNER_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_EXTEND_P2P_AC_ADD_AC_ADD_AC_KEY_INNER_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_key.inner_vid = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_AC_ADD_AC_ADD_AC_KEY_OUTER_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_EXTEND_P2P_AC_ADD_AC_ADD_AC_KEY_OUTER_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_key.outer_vid = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_AC_ADD_AC_ADD_AC_KEY_VLAN_DOMAIN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_EXTEND_P2P_AC_ADD_AC_ADD_AC_KEY_VLAN_DOMAIN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_key.vlan_domain = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ac_key after ac_add***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_AC_ADD_AC_ADD_AC_P2P_INFO_TPID_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_EXTEND_P2P_AC_ADD_AC_ADD_AC_P2P_INFO_TPID_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_p2p_info.tpid_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_AC_ADD_AC_ADD_AC_P2P_INFO_HAS_CW_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_EXTEND_P2P_AC_ADD_AC_ADD_AC_P2P_INFO_HAS_CW_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_p2p_info.has_cw = (uint8)param_val->value.ulong_value;
  }


  ret = ui_ppd_frwrd_decision_set(
          current_line,
          1,
          &(prm_ac_p2p_info.forward_decision)
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** ui_ppd_frwrd_decision_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "ui_ppd_frwrd_decision_set");
    goto exit;
  }


  /* Call function */
  ret = soc_ppd_frwrd_extend_p2p_ac_add(
    unit,
    &prm_ac_key,
    &prm_ac_p2p_info,
    &prm_success
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_extend_p2p_ac_add - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_extend_p2p_ac_add");
    goto exit;
  }

  send_string_to_screen("--> success:", TRUE);
  cli_out("success: %s\n",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: pwe_add (section frwrd_extend_p2p)
********************************************************************/
int
ui_ppd_api_frwrd_extend_p2p_pwe_add(
                                    CURRENT_LINE *current_line
                                    )
{
  uint32
    ret;
  SOC_PPD_FRWRD_EXTEND_PWE_KEY
    prm_pwe_key;
  SOC_PPD_FRWRD_EXTEND_P2P_FRWRD_INFO
    prm_pwe_p2p_info;
  SOC_SAND_SUCCESS_FAILURE
    prm_success;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_extend_p2p");
  soc_sand_proc_name = "soc_ppd_frwrd_extend_p2p_pwe_add";

  unit = soc_ppd_get_default_unit();
  SOC_PPD_FRWRD_EXTEND_PWE_KEY_clear(&prm_pwe_key);
  SOC_PPD_FRWRD_EXTEND_P2P_FRWRD_INFO_clear(&prm_pwe_p2p_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_PWE_ADD_PWE_ADD_PWE_KEY_INRIF_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_EXTEND_P2P_PWE_ADD_PWE_ADD_PWE_KEY_INRIF_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pwe_key.inrif_port = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_PWE_ADD_PWE_ADD_PWE_KEY_IN_LOCAL_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_EXTEND_P2P_PWE_ADD_PWE_ADD_PWE_KEY_IN_LOCAL_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pwe_key.in_local_port = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_PWE_ADD_PWE_ADD_PWE_KEY_INTERNAL_EXP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_EXTEND_P2P_PWE_ADD_PWE_ADD_PWE_KEY_INTERNAL_EXP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pwe_key.internal_exp = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_PWE_ADD_PWE_ADD_PWE_KEY_IN_LABEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_EXTEND_P2P_PWE_ADD_PWE_ADD_PWE_KEY_IN_LABEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pwe_key.in_label = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter pwe_key after pwe_add***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_PWE_ADD_PWE_ADD_PWE_P2P_INFO_TPID_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_EXTEND_P2P_PWE_ADD_PWE_ADD_PWE_P2P_INFO_TPID_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pwe_p2p_info.tpid_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_PWE_ADD_PWE_ADD_PWE_P2P_INFO_HAS_CW_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_EXTEND_P2P_PWE_ADD_PWE_ADD_PWE_P2P_INFO_HAS_CW_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pwe_p2p_info.has_cw = (uint8)param_val->value.ulong_value;
  }


  ret = ui_ppd_frwrd_decision_set(
          current_line,
          1,
          &(prm_pwe_p2p_info.forward_decision)
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** ui_ppd_frwrd_decision_set - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "ui_ppd_frwrd_decision_set");
    goto exit;
  }


  /* Call function */
  ret = soc_ppd_frwrd_extend_p2p_pwe_add(
    unit,
    &prm_pwe_key,
    &prm_pwe_p2p_info,
    &prm_success
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_extend_p2p_pwe_add - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_extend_p2p_pwe_add");
    goto exit;
  }

  send_string_to_screen("--> success:", TRUE);
  cli_out("success: %s\n",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));

  goto exit;
exit:
  return ui_ret;
}


/********************************************************************
*  Function handler: ac_get (section frwrd_extend_p2p)
********************************************************************/
int
ui_ppd_api_frwrd_extend_p2p_ac_get(
                                   CURRENT_LINE *current_line
                                   )
{
  uint32
    ret;
  SOC_PPD_L2_LIF_AC_KEY
    prm_ac_key;
  SOC_PPD_FRWRD_EXTEND_P2P_FRWRD_INFO
    prm_ac_p2p_info;
  uint8
    prm_found;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_extend_p2p");
  soc_sand_proc_name = "soc_ppd_frwrd_extend_p2p_ac_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPD_L2_LIF_AC_KEY_clear(&prm_ac_key);
  SOC_PPD_FRWRD_EXTEND_P2P_FRWRD_INFO_clear(&prm_ac_p2p_info);

  prm_ac_key.inner_vid = SOC_PPD_LIF_IGNORE_INNER_VID;
  prm_ac_key.outer_vid = SOC_PPD_LIF_IGNORE_OUTER_VID;

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_AC_GET_AC_GET_AC_KEY_INNER_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_EXTEND_P2P_AC_GET_AC_GET_AC_KEY_INNER_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_key.inner_vid = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_AC_GET_AC_GET_AC_KEY_OUTER_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_EXTEND_P2P_AC_GET_AC_GET_AC_KEY_OUTER_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_key.outer_vid = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_AC_GET_AC_GET_AC_KEY_VLAN_DOMAIN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_EXTEND_P2P_AC_GET_AC_GET_AC_KEY_VLAN_DOMAIN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_key.vlan_domain = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ac_key after ac_get***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_frwrd_extend_p2p_ac_get(
    unit,
    &prm_ac_key,
    &prm_ac_p2p_info,
    &prm_found
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_extend_p2p_ac_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_extend_p2p_ac_get");
    goto exit;
  }

  if (prm_found)
  {
    cli_out("\n\r found.\n\r");
  }
  else
  {
    cli_out("\n\r not found.\n\r");
  }
  if (prm_found)
  {
    SOC_PPD_FRWRD_EXTEND_P2P_FRWRD_INFO_print(&prm_ac_p2p_info);
  }
  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: pwe_get (section frwrd_extend_p2p)
********************************************************************/
int
ui_ppd_api_frwrd_extend_p2p_pwe_get(
                                    CURRENT_LINE *current_line
                                    )
{
  uint32
    ret;
  SOC_PPD_FRWRD_EXTEND_PWE_KEY
    prm_pwe_key;
  SOC_PPD_FRWRD_EXTEND_P2P_FRWRD_INFO
    prm_pwe_p2p_info;
  uint8
    prm_found;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_extend_p2p");
  soc_sand_proc_name = "soc_ppd_frwrd_extend_p2p_pwe_get";

  unit = soc_ppd_get_default_unit();
  SOC_PPD_FRWRD_EXTEND_PWE_KEY_clear(&prm_pwe_key);
  SOC_PPD_FRWRD_EXTEND_P2P_FRWRD_INFO_clear(&prm_pwe_p2p_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_PWE_GET_PWE_GET_PWE_KEY_INRIF_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_EXTEND_P2P_PWE_GET_PWE_GET_PWE_KEY_INRIF_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pwe_key.inrif_port = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_PWE_GET_PWE_GET_PWE_KEY_IN_LOCAL_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_EXTEND_P2P_PWE_GET_PWE_GET_PWE_KEY_IN_LOCAL_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pwe_key.in_local_port = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_PWE_GET_PWE_GET_PWE_KEY_INTERNAL_EXP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_EXTEND_P2P_PWE_GET_PWE_GET_PWE_KEY_INTERNAL_EXP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pwe_key.internal_exp = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_PWE_GET_PWE_GET_PWE_KEY_IN_LABEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_EXTEND_P2P_PWE_GET_PWE_GET_PWE_KEY_IN_LABEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pwe_key.in_label = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter pwe_key after pwe_get***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = soc_ppd_frwrd_extend_p2p_pwe_get(
    unit,
    &prm_pwe_key,
    &prm_pwe_p2p_info,
    &prm_found
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_extend_p2p_pwe_get - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_extend_p2p_pwe_get");
    goto exit;
  }


  if (prm_found)
  {
    cli_out("\n\r found.\n\r");
  }
  else
  {
    cli_out("\n\r not found. \n\r");
  }
  if (prm_found)
  {
    SOC_PPD_FRWRD_EXTEND_P2P_FRWRD_INFO_print(&prm_pwe_p2p_info);
  }
  

  goto exit;
exit:
  return ui_ret;
}


/********************************************************************
*  Function handler: ac_remove (section frwrd_extend_p2p)
********************************************************************/
int
ui_ppd_api_frwrd_extend_p2p_ac_remove(
                                      CURRENT_LINE *current_line
                                      )
{
  uint32
    ret;
  SOC_PPD_L2_LIF_AC_KEY
    prm_ac_key;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_extend_p2p");
  soc_sand_proc_name = "soc_ppd_frwrd_extend_p2p_ac_remove";

  unit = soc_ppd_get_default_unit();
  SOC_PPD_L2_LIF_AC_KEY_clear(&prm_ac_key);

  prm_ac_key.inner_vid = SOC_PPD_LIF_IGNORE_INNER_VID;
  prm_ac_key.outer_vid = SOC_PPD_LIF_IGNORE_OUTER_VID;

  /* remove parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_AC_REMOVE_AC_REMOVE_AC_KEY_INNER_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_EXTEND_P2P_AC_REMOVE_AC_REMOVE_AC_KEY_INNER_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_key.inner_vid = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_AC_REMOVE_AC_REMOVE_AC_KEY_OUTER_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_EXTEND_P2P_AC_REMOVE_AC_REMOVE_AC_KEY_OUTER_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_key.outer_vid = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_AC_REMOVE_AC_REMOVE_AC_KEY_VLAN_DOMAIN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_EXTEND_P2P_AC_REMOVE_AC_REMOVE_AC_KEY_VLAN_DOMAIN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_ac_key.vlan_domain = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter ac_key after ac_remove***", TRUE);
    goto exit;
  }

  /* Call function */
  ret = soc_ppd_frwrd_extend_p2p_ac_remove(
    unit,
    &prm_ac_key
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_extend_p2p_ac_remove - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_extend_p2p_ac_remove");
    goto exit;
  }

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Function handler: pwe_remove (section frwrd_extend_p2p)
********************************************************************/
int
ui_ppd_api_frwrd_extend_p2p_pwe_remove(
                                       CURRENT_LINE *current_line
                                       )
{
  uint32
    ret;
  SOC_PPD_FRWRD_EXTEND_PWE_KEY
    prm_pwe_key;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_extend_p2p");
  soc_sand_proc_name = "soc_ppd_frwrd_extend_p2p_pwe_remove";

  unit = soc_ppd_get_default_unit();
  SOC_PPD_FRWRD_EXTEND_PWE_KEY_clear(&prm_pwe_key);

  /* remove parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_PWE_REMOVE_PWE_REMOVE_PWE_KEY_INRIF_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_EXTEND_P2P_PWE_REMOVE_PWE_REMOVE_PWE_KEY_INRIF_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pwe_key.inrif_port = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_PWE_REMOVE_PWE_REMOVE_PWE_KEY_IN_LOCAL_PORT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_EXTEND_P2P_PWE_REMOVE_PWE_REMOVE_PWE_KEY_IN_LOCAL_PORT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pwe_key.in_local_port = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_PWE_REMOVE_PWE_REMOVE_PWE_KEY_INTERNAL_EXP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_EXTEND_P2P_PWE_REMOVE_PWE_REMOVE_PWE_KEY_INTERNAL_EXP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pwe_key.internal_exp = (uint8)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_PWE_REMOVE_PWE_REMOVE_PWE_KEY_IN_LABEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_EXTEND_P2P_PWE_REMOVE_PWE_REMOVE_PWE_KEY_IN_LABEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_pwe_key.in_label = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter pwe_key after pwe_remove***", TRUE);
    goto exit;
  }


  /* Call function */
  ret = soc_ppd_frwrd_extend_p2p_pwe_remove(
    unit,
    &prm_pwe_key
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_extend_p2p_pwe_remove - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_extend_p2p_pwe_remove");
    goto exit;
  }




  goto exit;
exit:
  return ui_ret;
}
/********************************************************************
*  Function handler: mim_add (section frwrd_extend_p2p)
********************************************************************/
int
ui_ppd_api_frwrd_extend_p2p_mim_add(
                                    CURRENT_LINE *current_line
                                    )
{
  uint32
    ret;
  SOC_PPD_L2_LIF_AC_KEY
    prm_mim_key;
  SOC_PPD_FRWRD_EXTEND_P2P_FRWRD_INFO
    prm_mim_p2p_info;
  SOC_SAND_SUCCESS_FAILURE
    prm_success;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_extend_p2p");
  soc_sand_proc_name = "soc_ppd_frwrd_extend_p2p_mim_add";

  unit = soc_ppd_get_default_unit();
  SOC_PPD_L2_LIF_AC_KEY_clear(&prm_mim_key);
  SOC_PPD_FRWRD_EXTEND_P2P_FRWRD_INFO_clear(&prm_mim_p2p_info);

  /* Get parameters */
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_MIM_ADD_MIM_ADD_MIM_KEY_INNER_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_EXTEND_P2P_MIM_ADD_MIM_ADD_MIM_KEY_INNER_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mim_key.inner_vid = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_MIM_ADD_MIM_ADD_MIM_KEY_OUTER_VID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_EXTEND_P2P_MIM_ADD_MIM_ADD_MIM_KEY_OUTER_VID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mim_key.outer_vid = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_MIM_ADD_MIM_ADD_MIM_KEY_VLAN_DOMAIN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_EXTEND_P2P_MIM_ADD_MIM_ADD_MIM_KEY_VLAN_DOMAIN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mim_key.vlan_domain = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter mim_key after mim_add***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_MIM_ADD_MIM_ADD_MIM_P2P_INFO_TPID_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_EXTEND_P2P_MIM_ADD_MIM_ADD_MIM_P2P_INFO_TPID_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mim_p2p_info.tpid_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_MIM_ADD_MIM_ADD_MIM_P2P_INFO_HAS_CW_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_EXTEND_P2P_MIM_ADD_MIM_ADD_MIM_P2P_INFO_HAS_CW_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mim_p2p_info.has_cw = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_MIM_ADD_MIM_ADD_MIM_P2P_INFO_FORWARD_DECISION_ADDITIONAL_INFO_TRAP_INFO_TRAP_ACTION_PROFILE_ID_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_EXTEND_P2P_MIM_ADD_MIM_ADD_MIM_P2P_INFO_FORWARD_DECISION_ADDITIONAL_INFO_TRAP_INFO_TRAP_ACTION_PROFILE_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mim_p2p_info.forward_decision.additional_info.trap_info.action_profile.trap_code = (SOC_PPD_TRAP_CODE)param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_MIM_ADD_MIM_ADD_MIM_P2P_INFO_FORWARD_DECISION_ADDITIONAL_INFO_TRAP_INFO_TRAP_QUALIFIER_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_EXTEND_P2P_MIM_ADD_MIM_ADD_MIM_P2P_INFO_FORWARD_DECISION_ADDITIONAL_INFO_TRAP_INFO_TRAP_QUALIFIER_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mim_p2p_info.forward_decision.additional_info.trap_info.action_profile.frwrd_action_strength = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_MIM_ADD_MIM_ADD_MIM_P2P_INFO_FORWARD_DECISION_ADDITIONAL_INFO_OUTLIF_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_EXTEND_P2P_MIM_ADD_MIM_ADD_MIM_P2P_INFO_FORWARD_DECISION_ADDITIONAL_INFO_OUTLIF_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mim_p2p_info.forward_decision.additional_info.outlif.val = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_MIM_ADD_MIM_ADD_MIM_P2P_INFO_FORWARD_DECISION_ADDITIONAL_INFO_OUTLIF_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_EXTEND_P2P_MIM_ADD_MIM_ADD_MIM_P2P_INFO_FORWARD_DECISION_ADDITIONAL_INFO_OUTLIF_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mim_p2p_info.forward_decision.additional_info.outlif.type = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_MIM_ADD_MIM_ADD_MIM_P2P_INFO_FORWARD_DECISION_ADDITIONAL_INFO_EEI_VAL_ISID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_EXTEND_P2P_MIM_ADD_MIM_ADD_MIM_P2P_INFO_FORWARD_DECISION_ADDITIONAL_INFO_EEI_VAL_ISID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mim_p2p_info.forward_decision.additional_info.eei.val.isid = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_MIM_ADD_MIM_ADD_MIM_P2P_INFO_FORWARD_DECISION_ADDITIONAL_INFO_EEI_VAL_MPLS_COMMAND_PUSH_PROFILE_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_EXTEND_P2P_MIM_ADD_MIM_ADD_MIM_P2P_INFO_FORWARD_DECISION_ADDITIONAL_INFO_EEI_VAL_MPLS_COMMAND_PUSH_PROFILE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mim_p2p_info.forward_decision.additional_info.eei.val.mpls_command.push_profile = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_MIM_ADD_MIM_ADD_MIM_P2P_INFO_FORWARD_DECISION_ADDITIONAL_INFO_EEI_VAL_MPLS_COMMAND_LABEL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_EXTEND_P2P_MIM_ADD_MIM_ADD_MIM_P2P_INFO_FORWARD_DECISION_ADDITIONAL_INFO_EEI_VAL_MPLS_COMMAND_LABEL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mim_p2p_info.forward_decision.additional_info.eei.val.mpls_command.label = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_MIM_ADD_MIM_ADD_MIM_P2P_INFO_FORWARD_DECISION_ADDITIONAL_INFO_EEI_VAL_MPLS_COMMAND_COMMAND_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_EXTEND_P2P_MIM_ADD_MIM_ADD_MIM_P2P_INFO_FORWARD_DECISION_ADDITIONAL_INFO_EEI_VAL_MPLS_COMMAND_COMMAND_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mim_p2p_info.forward_decision.additional_info.eei.val.mpls_command.command = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_MIM_ADD_MIM_ADD_MIM_P2P_INFO_FORWARD_DECISION_ADDITIONAL_INFO_EEI_VAL_TRILL_DEST_DEST_NICK_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_EXTEND_P2P_MIM_ADD_MIM_ADD_MIM_P2P_INFO_FORWARD_DECISION_ADDITIONAL_INFO_EEI_VAL_TRILL_DEST_DEST_NICK_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mim_p2p_info.forward_decision.additional_info.eei.val.trill_dest.dest_nick = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_MIM_ADD_MIM_ADD_MIM_P2P_INFO_FORWARD_DECISION_ADDITIONAL_INFO_EEI_VAL_TRILL_DEST_IS_MULTICAST_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_EXTEND_P2P_MIM_ADD_MIM_ADD_MIM_P2P_INFO_FORWARD_DECISION_ADDITIONAL_INFO_EEI_VAL_TRILL_DEST_IS_MULTICAST_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mim_p2p_info.forward_decision.additional_info.eei.val.trill_dest.is_multicast = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_MIM_ADD_MIM_ADD_MIM_P2P_INFO_FORWARD_DECISION_ADDITIONAL_INFO_EEI_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_EXTEND_P2P_MIM_ADD_MIM_ADD_MIM_P2P_INFO_FORWARD_DECISION_ADDITIONAL_INFO_EEI_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mim_p2p_info.forward_decision.additional_info.eei.type = param_val->numeric_equivalent;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_MIM_ADD_MIM_ADD_MIM_P2P_INFO_FORWARD_DECISION_DEST_ID_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_EXTEND_P2P_MIM_ADD_MIM_ADD_MIM_P2P_INFO_FORWARD_DECISION_DEST_ID_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mim_p2p_info.forward_decision.dest_id = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_MIM_ADD_MIM_ADD_MIM_P2P_INFO_FORWARD_DECISION_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_FRWRD_EXTEND_P2P_MIM_ADD_MIM_ADD_MIM_P2P_INFO_FORWARD_DECISION_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    prm_mim_p2p_info.forward_decision.type = param_val->numeric_equivalent;
  }

  /* Call function */
  ret = soc_ppd_frwrd_extend_p2p_mim_add(
    unit,
    &prm_mim_key,
    &prm_mim_p2p_info,
    &prm_success
    );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK)
  {
    send_string_to_screen(" *** soc_ppd_frwrd_extend_p2p_mim_add - FAIL", TRUE);
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_extend_p2p_mim_add");
    goto exit;
  }

  send_string_to_screen("--> success:", TRUE);
  cli_out("success: %s\n",soc_sand_SAND_SUCCESS_FAILURE_to_string(prm_success));

  goto exit;
exit:
  return ui_ret;
}

/********************************************************************
*  Section handler: frwrd_extend_p2p
********************************************************************/
int
ui_ppd_api_frwrd_extend_p2p(
                            CURRENT_LINE *current_line
                            )
{
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_frwrd_extend_p2p");

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_GLBL_INFO_SET_GLBL_INFO_SET_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_extend_p2p_glbl_info_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_GLBL_INFO_GET_GLBL_INFO_GET_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_extend_p2p_glbl_info_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_AC_ADD_AC_ADD_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_extend_p2p_ac_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_AC_GET_AC_GET_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_extend_p2p_ac_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_AC_REMOVE_AC_REMOVE_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_extend_p2p_ac_remove(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_PWE_ADD_PWE_ADD_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_extend_p2p_pwe_add(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_PWE_GET_PWE_GET_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_extend_p2p_pwe_get(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_PWE_REMOVE_PWE_REMOVE_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_extend_p2p_pwe_remove(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_EXTEND_P2P_MIM_ADD_MIM_ADD_ID,1))
  {
    ui_ret = ui_ppd_api_frwrd_extend_p2p_mim_add(current_line);
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting function name after frwrd_extend_p2p***", TRUE);
  }

  goto exit;
exit:
  return ui_ret;
}

#endif
#endif /* LINK_PPD_LIBRARIES */ 

