/* $Id: ui_ppd_api_diag.c,v 1.10 Broadcom SDK $
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
#include <shared/bsl.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h> 
#include <soc/dpp/TMC/tmc_api_packet.h> 

#include <appl/diag/dpp/utils_defi.h> 
#include <appl/dpp/UserInterface/ui_defi.h> 
 
#include <appl/dpp/UserInterface/ui_pure_defi_ppd_api.h> 
 
  
#include <soc/dpp/PPD/ppd_api_diag.h>
#include <soc/dpp/PPD/ppd_api_frwrd_mact_mgmt.h>
#include <appl/dpp/UserInterface/ppd/ui_ppd_api_diag.h>

#if LINK_PPD_LIBRARIES

#ifdef UI_DIAG
/******************************************************************** 
 *  Function handler: sample_enable_set (section diag)
 ********************************************************************/
int 
  ui_ppd_api_diag_sample_enable_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint8   
    prm_enable;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_diag"); 
  soc_sand_proc_name = "soc_ppd_diag_sample_enable_set"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_diag_sample_enable_get(
          unit,
          &prm_enable
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_diag_sample_enable_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_diag_sample_enable_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_SAMPLE_ENABLE_SET_SAMPLE_ENABLE_SET_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_DIAG_SAMPLE_ENABLE_SET_SAMPLE_ENABLE_SET_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_enable = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_diag_sample_enable_set(
          unit,
          prm_enable
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_diag_sample_enable_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_diag_sample_enable_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: sample_enable_get (section diag)
 ********************************************************************/
int 
  ui_ppd_api_diag_sample_enable_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint8   
    prm_enable;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_diag"); 
  soc_sand_proc_name = "soc_ppd_diag_sample_enable_get"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_ppd_diag_sample_enable_get(
          unit,
          &prm_enable
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_diag_sample_enable_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_diag_sample_enable_get");   
    goto exit; 
  } 

  cli_out("enable: %u\n\r",prm_enable);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: mode_info_set (section diag)
 ********************************************************************/
int 
  ui_ppd_api_diag_mode_info_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPD_DIAG_MODE_INFO   
    prm_mode_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_diag"); 
  soc_sand_proc_name = "soc_ppd_diag_mode_info_set"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPD_DIAG_MODE_INFO_clear(&prm_mode_info);
 
  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_ppd_diag_mode_info_get(
          unit,
          &prm_mode_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_diag_mode_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_diag_mode_info_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_MODE_INFO_SET_MODE_INFO_SET_MODE_INFO_FLAVOR_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_DIAG_MODE_INFO_SET_MODE_INFO_SET_MODE_INFO_FLAVOR_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_mode_info.flavor |= param_val->numeric_equivalent;
    if (param_val->numeric_equivalent == SOC_PPD_DIAG_FLAVOR_NONE)
    {
      prm_mode_info.flavor = SOC_PPD_DIAG_FLAVOR_NONE;
    }
  } 


  /* Call function */
  ret = soc_ppd_diag_mode_info_set(
          unit,
          &prm_mode_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_diag_mode_info_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_diag_mode_info_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: mode_info_get (section diag)
 ********************************************************************/
int 
  ui_ppd_api_diag_mode_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPD_DIAG_MODE_INFO   
    prm_mode_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_diag"); 
  soc_sand_proc_name = "soc_ppd_diag_mode_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPD_DIAG_MODE_INFO_clear(&prm_mode_info);
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_ppd_diag_mode_info_get(
          unit,
          &prm_mode_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_diag_mode_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_diag_mode_info_get");   
    goto exit; 
  } 

  SOC_PPD_DIAG_MODE_INFO_print(&prm_mode_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pkt_trace_clear (section diag)
 ********************************************************************/
int 
  ui_ppd_api_diag_pkt_trace_clear(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_pkt_trace=0,prm_core=0;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_diag"); 
  soc_sand_proc_name = "soc_ppd_diag_pkt_trace_clear"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_PKT_TRACE_CLEAR_PKT_TRACE_CLEAR_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_DIAG_PKT_TRACE_CLEAR_PKT_TRACE_CLEAR_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_pkt_trace = (uint32)param_val->numeric_equivalent;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_PKT_TRACE_CLEAR_PKT_TRACE_CORE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_DIAG_PKT_TRACE_CLEAR_PKT_TRACE_CORE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_core = (uint32)param_val->value.ulong_value;

  } 

  if(prm_core > 0 && SOC_DPP_DEFS_GET(unit, nof_cores) < 2){
      send_string_to_screen("error ilegal core ID for device", TRUE); 
      goto exit; 
  }
  
  /* Call function */
  ret = soc_ppd_diag_pkt_trace_clear(
          unit,
          prm_core,
          prm_pkt_trace
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_diag_pkt_trace_clear - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_diag_pkt_trace_clear");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: received_packet_info_get (section diag)
 ********************************************************************/
int 
  ui_ppd_api_diag_received_packet_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPD_DIAG_RECEIVED_PACKET_INFO   
    prm_rcvd_pkt_info;
   uint32 prm_core=-1;
   int    core, first_core, last_core;
   SOC_PPC_DIAG_RESULT ret_val;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_diag"); 
  soc_sand_proc_name = "soc_ppd_diag_received_packet_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPD_DIAG_RECEIVED_PACKET_INFO_clear(&prm_rcvd_pkt_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_RECEIVED_PACKET_INFO_GET_CORE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_DIAG_RECEIVED_PACKET_INFO_GET_CORE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_core = (uint32)param_val->value.ulong_value;

  } 
  if(prm_core > 0 && SOC_DPP_DEFS_GET(unit, nof_cores) < 2){
      send_string_to_screen("error ilegal core ID for device", TRUE); 
      goto exit; 
  }
  if (prm_core == -1) {
      first_core = 0;
      last_core = SOC_DPP_DEFS_GET(unit, nof_cores);
  } else {
      first_core = prm_core;
      last_core = prm_core;
  }

  for (core=first_core; core < last_core; core++) {

          /* Call function */
          ret = soc_ppd_diag_received_packet_info_get(
                  unit,
        		  prm_core,
                  &prm_rcvd_pkt_info,
                  &ret_val
                );
          if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
          { 
            send_string_to_screen(" *** soc_ppd_diag_received_packet_info_get - FAIL", TRUE); 
            soc_sand_disp_result_proc(ret, "soc_ppd_diag_received_packet_info_get");   
            goto exit; 
          } 
          if (ret_val != SOC_PPC_DIAG_OK) 
          { 
              LOG_CLI((BSL_META_U(unit, "core %u: Packet not found\n\r"),core));
              goto exit;; 
          } 

          SOC_PPD_DIAG_RECEIVED_PACKET_INFO_print(&prm_rcvd_pkt_info);
  }
  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: parsing_info_get (section diag)
 ********************************************************************/
int 
  ui_ppd_api_diag_parsing_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPD_DIAG_PARSING_INFO   
    prm_pars_info;
  uint32 prm_core=0;
  SOC_PPC_DIAG_RESULT ret_val;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_diag"); 
  soc_sand_proc_name = "soc_ppd_diag_parsing_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPD_DIAG_PARSING_INFO_clear(&prm_pars_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_RECEIVED_PACKET_INFO_GET_CORE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_DIAG_RECEIVED_PACKET_INFO_GET_CORE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_core = (uint32)param_val->value.ulong_value;

  } 

  /* Call function */
  ret = soc_ppd_diag_parsing_info_get(
          unit,
		  prm_core,
          &prm_pars_info,
          &ret_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_diag_parsing_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_diag_parsing_info_get");   
    goto exit; 
  } 
  if (ret_val != SOC_PPC_DIAG_OK) {
      LOG_CLI((BSL_META_U(unit, "core %u: Not found\n\r"),prm_core));
      goto exit;
  }

  SOC_PPD_DIAG_PARSING_INFO_print(&prm_pars_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: termination_info_get (section diag)
 ********************************************************************/
int 
  ui_ppd_api_diag_termination_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPD_DIAG_TERM_INFO   
    prm_term_info;
  uint32 prm_core=0; 
  SOC_PPC_DIAG_RESULT ret_val;
  
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_diag"); 
  soc_sand_proc_name = "soc_ppd_diag_termination_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPD_DIAG_TERM_INFO_clear(&prm_term_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_TERMINATION_INFO_GET_CORE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_DIAG_TERMINATION_INFO_GET_CORE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_core = (uint32)param_val->value.ulong_value;

  } 

  /* Call function */
  ret = soc_ppd_diag_termination_info_get(
          unit,
		  prm_core,
          &prm_term_info,
          &ret_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_diag_termination_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_diag_termination_info_get");   
    goto exit; 
  } 
  if (ret_val != SOC_PPC_DIAG_OK) {
      LOG_CLI((BSL_META_U(unit, "core %u: Not found\n\r"),prm_core));
      goto exit;
  }

  SOC_PPD_DIAG_TERM_INFO_print(&prm_term_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: frwrd_lkup_info_get (section diag)
 ********************************************************************/
int 
  ui_ppd_api_diag_frwrd_lkup_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPD_DIAG_FRWRD_LKUP_INFO   
    prm_frwrd_info;
  uint32 prm_core=0; 
  SOC_PPC_DIAG_RESULT ret_val;
  

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_diag"); 
  soc_sand_proc_name = "soc_ppd_diag_frwrd_lkup_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPD_DIAG_FRWRD_LKUP_INFO_clear(&prm_frwrd_info);
 
  /* Get parameters */ 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_FRWRD_LKUP_INFO_GET_CORE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_DIAG_FRWRD_LKUP_INFO_GET_CORE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_core = (uint32)param_val->value.ulong_value;

  } 

  /* Call function */
  ret = soc_ppd_diag_frwrd_lkup_info_get(
          unit,
		  prm_core,
          &prm_frwrd_info,
          &ret_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_diag_frwrd_lkup_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_diag_frwrd_lkup_info_get");   
    goto exit; 
  } 
  if (ret_val != SOC_PPC_DIAG_OK) {
      LOG_CLI((BSL_META_U(unit, "core %u: Not found\n\r"),prm_core));
      goto exit;; 
  }


  SOC_PPD_DIAG_FRWRD_LKUP_INFO_print(&prm_frwrd_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: frwrd_lpm_lkup_get (section diag)
 ********************************************************************/
int 
  ui_ppd_api_diag_frwrd_lpm_lkup_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPD_DIAG_IPV4_VPN_KEY   
    prm_lpm_key;
  uint32   
    prm_fec_ptr;
  uint8   
    prm_found;
  uint32 prm_core=0; 
  SOC_PPC_DIAG_RESULT ret_val;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_diag"); 
  soc_sand_proc_name = "soc_ppd_diag_frwrd_lpm_lkup_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPD_DIAG_IPV4_VPN_KEY_clear(&prm_lpm_key);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_FRWRD_LPM_LKUP_GET_FRWRD_LPM_LKUP_GET_LPM_KEY_KEY_SUBNET_PREFIX_LEN_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_DIAG_FRWRD_LPM_LKUP_GET_FRWRD_LPM_LKUP_GET_LPM_KEY_KEY_SUBNET_PREFIX_LEN_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_lpm_key.key.subnet.prefix_len = (uint8)param_val->value.ulong_value;
  } 

  if (!get_val_of(
    current_line,(int *)&match_index,SOC_PARAM_PPD_DIAG_FRWRD_LPM_LKUP_GET_FRWRD_LPM_LKUP_GET_LPM_KEY_KEY_SUBNET_IP_ADDRESS_ID,1,  
    &param_val,VAL_IP,err_msg))                                                                
  {                                                                                                  
    prm_lpm_key.key.subnet.ip_address = param_val->value.ip_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_FRWRD_LPM_LKUP_GET_FRWRD_LPM_LKUP_GET_LPM_KEY_VRF_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_DIAG_FRWRD_LPM_LKUP_GET_FRWRD_LPM_LKUP_GET_LPM_KEY_VRF_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_lpm_key.vrf = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter lpm_key after frwrd_lpm_lkup_get***", TRUE); 
    goto exit; 
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_FRWRD_LPM_LKUP_GET_FRWRD_LPM_LKUP_GET_LPM_CORE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_DIAG_FRWRD_LPM_LKUP_GET_FRWRD_LPM_LKUP_GET_LPM_CORE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_core = (uint32)param_val->value.ulong_value;

  } 


  /* Call function */
  ret = soc_ppd_diag_frwrd_lpm_lkup_get(
          unit,
		  prm_core,
          &prm_lpm_key,
          &prm_fec_ptr,
          &prm_found,
          &ret_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_diag_frwrd_lpm_lkup_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_diag_frwrd_lpm_lkup_get");   
    goto exit; 
  } 
  if (ret_val != SOC_PPC_DIAG_OK) 
  { 
      LOG_CLI((BSL_META_U(unit, "core %u: Not found\n\r"),prm_core));
       goto exit; 
  } 


  cli_out("fec_ptr: %u\n\r",prm_fec_ptr);

  cli_out("found: %u\n\r",prm_found);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: traps_info_get (section diag)
 ********************************************************************/
int 
  ui_ppd_api_diag_traps_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPD_DIAG_TRAPS_INFO   
    prm_traps_info;
  uint32 prm_core=0; 
  SOC_PPC_DIAG_RESULT ret_val;
  
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_diag"); 
  soc_sand_proc_name = "soc_ppd_diag_traps_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPD_DIAG_TRAPS_INFO_clear(&prm_traps_info);
 
  /* Get parameters */ 
 if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_TRAPS_INFO_GET_CORE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_DIAG_TRAPS_INFO_GET_CORE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_core = (uint32)param_val->value.ulong_value;

  } 
  /* Call function */
  ret = soc_ppd_diag_traps_info_get(
          unit,
		  prm_core,
          &prm_traps_info,
          &ret_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_diag_traps_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_diag_traps_info_get");   
    goto exit; 
  } 
  if (ret_val != SOC_PPC_DIAG_OK) {
      LOG_CLI((BSL_META_U(unit, "core %u: Not found\n\r"),prm_core));
      goto exit;
  }

  SOC_PPD_DIAG_TRAPS_INFO_print(&prm_traps_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  

/******************************************************************** 
 *  Function handler: trapped_packet_info_get (section diag)
 ********************************************************************/
int 
  ui_ppd_api_diag_trapped_packet_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint8   
    prm_buff[400];
  uint32   
    prm_max_size=400,
    ind;
  uint32   
    prm_buff_len;
  SOC_PPD_DIAG_TRAP_PACKET_INFO   
    prm_packet_info;
  uint32
    prm_parse_learn = 0;
  SOC_PPD_FRWRD_MACT_LEARN_MSG
    learn_msg;
  SOC_PPD_FRWRD_MACT_LEARN_MSG_PARSE_INFO
    learn_events;
  SOC_PPC_DIAG_RESULT ret_val;


  UI_MACROS_INIT_FUNCTION("ui_ppd_api_diag"); 
  soc_sand_proc_name = "soc_ppd_diag_trapped_packet_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPD_DIAG_TRAP_PACKET_INFO_clear(&prm_packet_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_TRAPPED_PACKET_INFO_GET_TRAPPED_PACKET_INFO_GET_MAX_SIZE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_DIAG_TRAPPED_PACKET_INFO_GET_TRAPPED_PACKET_INFO_GET_MAX_SIZE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_max_size = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_TRAPPED_PACKET_INFO_GET_TRAPPED_PACKET_INFO_PARSE_LEARN_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_DIAG_TRAPPED_PACKET_INFO_GET_TRAPPED_PACKET_INFO_PARSE_LEARN_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_parse_learn = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_diag_trapped_packet_info_get(
          unit,
          prm_buff,
          prm_max_size,
          &prm_buff_len,
          &prm_packet_info,
          &ret_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_diag_trapped_packet_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_diag_trapped_packet_info_get");   
    goto exit; 
  } 
  if (ret_val != SOC_PPC_DIAG_OK) 
  { 
    send_string_to_screen(" *** Not found", TRUE); 
    goto exit; 
  } 

  if (prm_parse_learn != 3)
  {
    for (ind=0; ind < SOC_SAND_MIN(prm_buff_len,300); ++ind)
    {
      cli_out("Data[%u]:       %02x\n\r", ind,prm_buff[ind]);
    }

    cli_out("buff_len: %u\n\r",prm_buff_len);

    SOC_PPD_DIAG_TRAP_PACKET_INFO_print(&prm_packet_info);
  }

  if (prm_parse_learn != 0)
  {
    learn_msg.max_nof_events = 8;
    learn_events.events = soc_sand_os_malloc(sizeof(SOC_PPC_FRWRD_MACT_EVENT_INFO)*learn_msg.max_nof_events,"learn_events.events");
    
    learn_msg.msg_len = prm_buff_len;
    learn_msg.msg_buffer = soc_sand_os_malloc(sizeof(uint8)* prm_buff_len, "learn_msg.msg_buffer");
    soc_sand_os_memcpy(learn_msg.msg_buffer,prm_buff, prm_buff_len);
    
    ret = soc_ppd_frwrd_mact_learn_msg_parse(unit,&learn_msg,&learn_events);
    if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
    { 
      send_string_to_screen(" *** soc_ppd_diag_trapped_packet_info_get - FAIL", TRUE); 
      soc_sand_disp_result_proc(ret, "soc_ppd_diag_trapped_packet_info_get");   
      goto exit; 
    } 

    /* print result */
    SOC_PPD_FRWRD_MACT_LEARN_MSG_PARSE_INFO_print(&learn_events);

    soc_sand_os_free(learn_events.events);
    soc_sand_os_free(learn_msg.msg_buffer);

  }

  
  goto exit; 
exit: 
  return ui_ret; 
} 

/******************************************************************** 
 *  Function handler: traps_all_to_cpu (section diag)
 ********************************************************************/
int 
  ui_ppd_api_diag_traps_all_to_cpu(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPD_DIAG_TRAP_TO_CPU_INFO   
    prm_trap_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_diag"); 
  soc_sand_proc_name = "soc_ppd_diag_traps_all_to_cpu"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPD_DIAG_TRAP_TO_CPU_INFO_clear(&prm_trap_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_TRAPS_ALL_TO_CPU_TRAPS_ALL_TO_CPU_TRAP_INFO_CPU_DEST_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_DIAG_TRAPS_ALL_TO_CPU_TRAPS_ALL_TO_CPU_TRAP_INFO_CPU_DEST_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_trap_info.cpu_dest = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_diag_traps_all_to_cpu(
          unit,
          &prm_trap_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_diag_traps_all_to_cpu - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_diag_traps_all_to_cpu");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: traps_stat_restore (section diag)
 ********************************************************************/
int 
  ui_ppd_api_diag_traps_stat_restore(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_diag"); 
  soc_sand_proc_name = "soc_ppd_diag_traps_stat_restore"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_ppd_diag_traps_stat_restore(
          unit
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_diag_traps_stat_restore - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_diag_traps_stat_restore");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: frwrd_decision_trace_get (section diag)
 ********************************************************************/
int 
  ui_ppd_api_diag_frwrd_decision_trace_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32 prm_core=0;
  unsigned short err_code; 
  SOC_PPC_DIAG_RESULT ret_val;
	
  SOC_PPD_DIAG_FRWRD_DECISION_TRACE_INFO   
    prm_frwrd_trace_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_diag"); 
  soc_sand_proc_name = "soc_ppd_diag_frwrd_decision_trace_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPD_DIAG_FRWRD_DECISION_TRACE_INFO_clear(&prm_frwrd_trace_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_FRWRD_DECISION_TRACE_GET_CORE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_DIAG_FRWRD_DECISION_TRACE_GET_CORE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_core = (uint32)param_val->value.ulong_value;

  } 

  /* Call function */
  ret = soc_ppd_diag_frwrd_decision_trace_get(
          unit,
		  prm_core,
          &prm_frwrd_trace_info,
          &ret_val
        );
  err_code = soc_sand_get_error_code_from_error_word(ret);

  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_diag_frwrd_decision_trace_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_diag_frwrd_decision_trace_get");   
    goto exit; 
  } 
  if (ret_val != SOC_PPC_DIAG_OK) 
  { 
    send_string_to_screen(" *** Packet not found", TRUE); 
    goto exit; 
  } 
  if (err_code) 
  { 
    send_string_to_screen(" *** Packet not found", TRUE); 
    goto exit; 
  } 


  SOC_PPD_DIAG_FRWRD_DECISION_TRACE_INFO_print(&prm_frwrd_trace_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: learning_info_get (section diag)
 ********************************************************************/
int 
  ui_ppd_api_diag_learning_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPD_DIAG_LEARN_INFO   
    prm_learn_info;
  uint32 prm_core = 0; 
  SOC_PPC_DIAG_RESULT ret_val;
  

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_diag"); 
  soc_sand_proc_name = "soc_ppd_diag_learning_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPD_DIAG_LEARN_INFO_clear(&prm_learn_info);
 
  /* Get parameters */ 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_LEARNING_INFO_GET_CORE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_DIAG_LEARNING_INFO_GET_CORE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_core = (uint32)param_val->value.ulong_value;

  } 

  /* Call function */
  ret = soc_ppd_diag_learning_info_get(
          unit,
          prm_core,
          &prm_learn_info,
          &ret_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_diag_learning_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_diag_learning_info_get");   
    goto exit; 
  } 

  SOC_PPD_DIAG_LEARN_INFO_print(&prm_learn_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: ing_vlan_edit_info_get (section diag)
 ********************************************************************/
int 
  ui_ppd_api_diag_ing_vlan_edit_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPD_DIAG_VLAN_EDIT_RES   
    prm_vec_res;
  uint32 prm_core = 0; 
  SOC_PPC_DIAG_RESULT ret_val;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_diag"); 
  soc_sand_proc_name = "soc_ppd_diag_ing_vlan_edit_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPD_DIAG_VLAN_EDIT_RES_clear(&prm_vec_res);
 
  /* Get parameters */ 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_ING_VLAN_EDIT_INFO_GET_CORE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_DIAG_ING_VLAN_EDIT_INFO_GET_CORE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_core = (uint32)param_val->value.ulong_value;

  } 

  /* Call function */
  ret = soc_ppd_diag_ing_vlan_edit_info_get(
          unit,
		  prm_core,
          &prm_vec_res,
          &ret_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_diag_ing_vlan_edit_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_diag_ing_vlan_edit_info_get");   
    goto exit; 
  } 
  if (ret_val != SOC_PPC_DIAG_OK) 
  { 
      LOG_CLI((BSL_META_U(unit, "core %u: Not found\n\r"),prm_core));
      goto exit;; 
  } 

  SOC_PPD_DIAG_VLAN_EDIT_RES_print(&prm_vec_res);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pkt_associated_tm_info_get (section diag)
 ********************************************************************/
int 
  ui_ppd_api_diag_pkt_associated_tm_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPD_DIAG_PKT_TM_INFO   
    prm_pkt_tm_info;
  uint32 prm_core=0;
  SOC_PPC_DIAG_RESULT ret_val;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_diag"); 
  soc_sand_proc_name = "soc_ppd_diag_pkt_associated_tm_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPD_DIAG_PKT_TM_INFO_clear(&prm_pkt_tm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_PKT_ASSOCIATED_TM_INFO_GET_CORE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_DIAG_PKT_ASSOCIATED_TM_INFO_GET_CORE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_core = (uint32)param_val->value.ulong_value;

  } 


  /* Call function */
  ret = soc_ppd_diag_pkt_associated_tm_info_get(
          unit,
          prm_core,
          &prm_pkt_tm_info,
          &ret_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_diag_pkt_associated_tm_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_diag_pkt_associated_tm_info_get");   
    goto exit; 
  } 
  if (ret_val != SOC_PPC_DIAG_OK) {
      LOG_CLI((BSL_META_U(unit, "core %u: Packet not found\n\r"),prm_core));
      goto exit;; 
  }
  SOC_PPD_DIAG_PKT_TM_INFO_print(&prm_pkt_tm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: encap_info_get (section diag)
 ********************************************************************/
int 
  ui_ppd_api_diag_encap_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPD_DIAG_ENCAP_INFO   
    prm_encap_info;
  uint32 prm_core=-1;
  SOC_PPC_DIAG_RESULT ret_val;
  int    core, first_core, last_core;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_diag"); 
  soc_sand_proc_name = "soc_ppd_diag_encap_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPD_DIAG_ENCAP_INFO_clear(&prm_encap_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_ENCAP_INFO_GET_CORE_ID,1)) 
  { 
	UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_DIAG_ENCAP_INFO_GET_CORE_ID);  
	UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
	prm_core = (uint32)param_val->value.ulong_value;

  } 
  if (prm_core == -1) {
      first_core = 0;
      last_core = SOC_DPP_DEFS_GET(unit, nof_cores);
  } else {
      first_core = prm_core;
      last_core = prm_core;
  }

  for (core=first_core; core < last_core; core++) {

      /* Call function */
      ret = soc_ppd_diag_encap_info_get(
              unit,
    		  core,
              &prm_encap_info,
              &ret_val
            );
      if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
      { 
        send_string_to_screen(" *** soc_ppd_diag_encap_info_get - FAIL", TRUE); 
        soc_sand_disp_result_proc(ret, "soc_ppd_diag_encap_info_get");   
        goto exit; 
      } 
      if (ret_val != SOC_PPC_DIAG_OK) 
      { 
        LOG_CLI((BSL_META_U(unit, "core %u: Nnot found\n\r"),core));
        goto exit; 
      } 

  }
  SOC_PPD_DIAG_ENCAP_INFO_print(unit, &prm_encap_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: eg_drop_log_get (section diag)
 ********************************************************************/
int 
  ui_ppd_api_diag_eg_drop_log_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPD_DIAG_EG_DROP_LOG_INFO   
    prm_eg_drop_log;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_diag"); 
  soc_sand_proc_name = "soc_ppd_diag_eg_drop_log_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPD_DIAG_EG_DROP_LOG_INFO_clear(&prm_eg_drop_log);
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_ppd_diag_eg_drop_log_get(
          unit,
          &prm_eg_drop_log
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_diag_eg_drop_log_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_diag_eg_drop_log_get");   
    goto exit; 
  } 

  SOC_PPD_DIAG_EG_DROP_LOG_INFO_print(&prm_eg_drop_log);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: db_lif_lkup_info_get (section diag)
 ********************************************************************/
int 
  ui_ppd_api_diag_db_lif_lkup_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPD_DIAG_DB_USE_INFO   
    prm_db_info;
  SOC_PPD_DIAG_LIF_LKUP_INFO   
    prm_lkup_info;
  uint32 prm_core=0;
  SOC_PPC_DIAG_RESULT                     ret_val;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_diag"); 
  soc_sand_proc_name = "soc_ppd_diag_db_lif_lkup_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPD_DIAG_DB_USE_INFO_clear(&prm_db_info);
  SOC_PPD_DIAG_LIF_LKUP_INFO_clear(&prm_lkup_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_DB_LIF_LKUP_INFO_GET_DB_LIF_LKUP_INFO_GET_DB_INFO_LKUP_NUM_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_DIAG_DB_LIF_LKUP_INFO_GET_DB_LIF_LKUP_INFO_GET_DB_INFO_LKUP_NUM_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_db_info.lkup_num = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_DB_LIF_LKUP_INFO_GET_DB_LIF_LKUP_INFO_GET_DB_INFO_BANK_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_DIAG_DB_LIF_LKUP_INFO_GET_DB_LIF_LKUP_INFO_GET_DB_INFO_BANK_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_db_info.bank_id = (uint32)param_val->value.ulong_value;
  }
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_DB_LIF_LKUP_INFO_GET_DB_LIF_LKUP_INFO_GET_DB_INFO_CORE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_DIAG_RECEIVED_PACKET_INFO_GET_CORE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_core = (uint32)param_val->value.ulong_value;

  } 


  /* Call function */
  ret = soc_ppd_diag_db_lif_lkup_info_get(
          unit,
          prm_core,
          &prm_db_info,
          &prm_lkup_info,
          &ret_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_diag_db_lif_lkup_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_diag_db_lif_lkup_info_get");   
    goto exit; 
  } 
  if (ret_val != SOC_PPC_DIAG_OK) 
  { 
      LOG_CLI((BSL_META_U(unit, "core %u: Not found\n\r"),prm_core));
       goto exit; 
  } 

  SOC_PPD_DIAG_LIF_LKUP_INFO_print(&prm_lkup_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: db_lem_lkup_info_get (section diag)
 ********************************************************************/
int 
  ui_ppd_api_diag_db_lem_lkup_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPD_DIAG_DB_USE_INFO   
    prm_db_info;
  SOC_PPD_DIAG_LEM_LKUP_TYPE   
    prm_type;
  SOC_PPD_DIAG_LEM_KEY   
    prm_key;
  SOC_PPD_DIAG_LEM_VALUE   
    prm_val;
  uint8   
    prm_valid;
  uint32 prm_core=0;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_diag"); 
  soc_sand_proc_name = "soc_ppd_diag_db_lem_lkup_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPD_DIAG_DB_USE_INFO_clear(&prm_db_info);
  SOC_PPD_DIAG_LEM_KEY_clear(&prm_key);
  SOC_PPD_DIAG_LEM_VALUE_clear(&prm_val);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_DB_LEM_LKUP_INFO_GET_DB_LEM_LKUP_INFO_GET_DB_INFO_LKUP_NUM_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_DIAG_DB_LEM_LKUP_INFO_GET_DB_LEM_LKUP_INFO_GET_DB_INFO_LKUP_NUM_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_db_info.lkup_num = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_DB_LEM_LKUP_INFO_GET_CORE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_DIAG_DB_LEM_LKUP_INFO_GET_CORE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_core = (uint32)param_val->value.ulong_value;

  } 

  /* Call function */
  ret = soc_ppd_diag_db_lem_lkup_info_get(
          unit,
          prm_core,
          &prm_db_info,
          &prm_type,
          &prm_key,
          &prm_val,
          &prm_valid
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_diag_db_lem_lkup_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_diag_db_lem_lkup_info_get");   
    goto exit; 
  } 

  cli_out("type: %s\n\r",SOC_PPD_DIAG_LEM_LKUP_TYPE_to_string(prm_type));

  cli_out("key:\n\r");
  SOC_PPD_DIAG_LEM_KEY_print(&prm_key,prm_type);
  if (prm_valid)
  {
    cli_out("value:\n\r");
    SOC_PPD_DIAG_LEM_VALUE_print(&prm_val,prm_type);
  }
  cli_out("valid: %u\n\r",prm_valid);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: db_tcam_lkup_info_get (section diag)
 ********************************************************************/
int 
  ui_ppd_api_diag_db_tcam_lkup_info_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPD_DIAG_TCAM_USE_INFO   
    prm_use_info;
  SOC_PPD_DIAG_LEM_LKUP_TYPE   
    prm_type;
  SOC_PPD_DIAG_TCAM_KEY   
    prm_key;
  SOC_PPD_DIAG_TCAM_VALUE   
    prm_val;
  uint8   
    prm_valid;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_diag"); 
  soc_sand_proc_name = "soc_ppd_diag_db_tcam_lkup_info_get"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPD_DIAG_TCAM_USE_INFO_clear(&prm_use_info);
  SOC_PPD_DIAG_TCAM_KEY_clear(&prm_key);
  SOC_PPD_DIAG_TCAM_VALUE_clear(&prm_val);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_DB_TCAM_LKUP_INFO_GET_DB_TCAM_LKUP_INFO_GET_USE_INFO_LKUP_USAGE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_DIAG_DB_TCAM_LKUP_INFO_GET_DB_TCAM_LKUP_INFO_GET_USE_INFO_LKUP_USAGE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_use_info.lkup_usage = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_DB_TCAM_LKUP_INFO_GET_DB_TCAM_LKUP_INFO_GET_USE_INFO_LKUP_NUM_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_DIAG_DB_TCAM_LKUP_INFO_GET_DB_TCAM_LKUP_INFO_GET_USE_INFO_LKUP_NUM_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_use_info.lkup_num = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_ppd_diag_db_tcam_lkup_info_get(
          unit,
          &prm_use_info,
          &prm_type,
          &prm_key,
          &prm_val,
          &prm_valid
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_diag_db_tcam_lkup_info_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_diag_db_tcam_lkup_info_get");   
    goto exit; 
  } 

  cli_out("type: %s\n\r",SOC_PPD_DIAG_LEM_LKUP_TYPE_to_string(prm_type));

  SOC_PPD_DIAG_TCAM_KEY_print(&prm_key);

  SOC_PPD_DIAG_TCAM_VALUE_print(&prm_val);

  cli_out("valid: %u\n\r",prm_valid);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: pkt_send (section diag)
 ********************************************************************/
int 
  ui_ppd_api_diag_pkt_send(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PPD_DIAG_PACKET_INFO   
    prm_pkt_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_api_diag"); 
  soc_sand_proc_name = "soc_ppd_diag_pkt_send"; 
 
  unit = soc_ppd_get_default_unit(); 
  SOC_PPD_DIAG_PACKET_INFO_clear(&prm_pkt_info);
  prm_pkt_info.ether_info.tag.tpid = 0x8100;
  prm_pkt_info.total_size = 120;
  prm_pkt_info.type = SOC_PPD_PKT_HDR_STK_TYPE_ETH;
  soc_sand_pp_mac_address_string_parse("bbb1", &prm_pkt_info.ether_info.da);
  soc_sand_pp_mac_address_string_parse("eee", &prm_pkt_info.ether_info.sa);
  prm_pkt_info.ether_info.ether_type = 0x9000;
  prm_pkt_info.ether_info.is_tagged = 1;
  prm_pkt_info.ether_info.tag.vid = 0x31;

 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_PKT_SEND_PKT_SEND_PKT_INFO_ETHER_INFO_TAG_DEI_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_DIAG_PKT_SEND_PKT_SEND_PKT_INFO_ETHER_INFO_TAG_DEI_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_pkt_info.ether_info.tag.dei = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_PKT_SEND_PKT_SEND_PKT_INFO_ETHER_INFO_TAG_PCP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_DIAG_PKT_SEND_PKT_SEND_PKT_INFO_ETHER_INFO_TAG_PCP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_pkt_info.ether_info.tag.pcp = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_PKT_SEND_PKT_SEND_PKT_INFO_ETHER_INFO_TAG_VID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_DIAG_PKT_SEND_PKT_SEND_PKT_INFO_ETHER_INFO_TAG_VID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_pkt_info.ether_info.tag.vid = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_PKT_SEND_PKT_SEND_PKT_INFO_ETHER_INFO_TAG_TPID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_DIAG_PKT_SEND_PKT_SEND_PKT_INFO_ETHER_INFO_TAG_TPID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_pkt_info.ether_info.tag.tpid = (uint16)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_PKT_SEND_PKT_SEND_PKT_INFO_ETHER_INFO_IS_TAGGED_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_DIAG_PKT_SEND_PKT_SEND_PKT_INFO_ETHER_INFO_IS_TAGGED_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_pkt_info.ether_info.is_tagged = (uint8)param_val->value.ulong_value;
  } 

  if (!get_val_of(
         current_line,(int *)&match_index,SOC_PARAM_PPD_DIAG_PKT_SEND_PKT_SEND_PKT_INFO_ETHER_INFO_SA_ID,1,  
         &param_val,VAL_TEXT,err_msg)) 
  { 
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_pkt_info.ether_info.sa));
  } 
  if (!get_val_of(
         current_line,(int *)&match_index,SOC_PARAM_PPD_DIAG_PKT_SEND_PKT_SEND_PKT_INFO_ETHER_INFO_DA_ID,1,  
         &param_val,VAL_TEXT,err_msg)) 
  { 
    soc_sand_pp_mac_address_string_parse(param_val->value.val_text, &(prm_pkt_info.ether_info.da));
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_PKT_SEND_PKT_SEND_PKT_INFO_ETHER_INFO_ETHER_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_DIAG_PKT_SEND_PKT_SEND_PKT_INFO_ETHER_INFO_ETHER_TYPE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_pkt_info.ether_info.ether_type = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_PKT_SEND_PKT_SEND_PKT_INFO_NOF_PACKETS_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_DIAG_PKT_SEND_PKT_SEND_PKT_INFO_NOF_PACKETS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_pkt_info.nof_packets = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_PKT_SEND_PKT_SEND_PKT_INFO_TOTAL_SIZE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_DIAG_PKT_SEND_PKT_SEND_PKT_INFO_TOTAL_SIZE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_pkt_info.total_size = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_PKT_SEND_PKT_SEND_PKT_INFO_IN_TM_PORT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_DIAG_PKT_SEND_PKT_SEND_PKT_INFO_IN_TM_PORT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_pkt_info.in_tm_port = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_PKT_SEND_PKT_SEND_PKT_INFO_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(SOC_PARAM_PPD_DIAG_PKT_SEND_PKT_SEND_PKT_INFO_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_pkt_info.type = param_val->numeric_equivalent;
  } 


  /* Call function */
  ret = soc_ppd_diag_pkt_send(
          unit,
          &prm_pkt_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_diag_pkt_send - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_diag_pkt_send");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
#endif
#ifdef UI_DIAG/* { diag*/
/******************************************************************** 
 *  Section handler: diag
 ********************************************************************/ 
int 
  ui_ppd_api_diag( 
    CURRENT_LINE *current_line 
  ) 
{   
  int
    ret = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_ppd_api_diag"); 
 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_SAMPLE_ENABLE_SET_SAMPLE_ENABLE_SET_ID,1)) 
  { 
    ret = ui_ppd_api_diag_sample_enable_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_SAMPLE_ENABLE_GET_SAMPLE_ENABLE_GET_ID,1)) 
  { 
    ret = ui_ppd_api_diag_sample_enable_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_MODE_INFO_SET_MODE_INFO_SET_ID,1)) 
  { 
    ret = ui_ppd_api_diag_mode_info_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_MODE_INFO_GET_MODE_INFO_GET_ID,1)) 
  { 
    ret = ui_ppd_api_diag_mode_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_PKT_TRACE_CLEAR_PKT_TRACE_CLEAR_PKT_TRACE_ID,1)) 
  { 
    ret = ui_ppd_api_diag_pkt_trace_clear(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_RECEIVED_PACKET_INFO_GET_RECEIVED_PACKET_INFO_GET_ID,1)) 
  { 
    ret = ui_ppd_api_diag_received_packet_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_PARSING_INFO_GET_PARSING_INFO_GET_ID,1)) 
  { 
    ret = ui_ppd_api_diag_parsing_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_TERMINATION_INFO_GET_TERMINATION_INFO_GET_ID,1)) 
  { 
    ret = ui_ppd_api_diag_termination_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_FRWRD_LKUP_INFO_GET_FRWRD_LKUP_INFO_GET_ID,1)) 
  { 
    ret = ui_ppd_api_diag_frwrd_lkup_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_FRWRD_LPM_LKUP_GET_FRWRD_LPM_LKUP_GET_ID,1)) 
  { 
    ret = ui_ppd_api_diag_frwrd_lpm_lkup_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_TRAPS_INFO_GET_TRAPS_INFO_GET_ID,1)) 
  { 
    ret = ui_ppd_api_diag_traps_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_TRAPPED_PACKET_INFO_GET_TRAPPED_PACKET_INFO_GET_ID,1)) 
  { 
    ret = ui_ppd_api_diag_trapped_packet_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_TRAPS_ALL_TO_CPU_TRAPS_ALL_TO_CPU_ID,1)) 
  { 
    ret = ui_ppd_api_diag_traps_all_to_cpu(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_TRAPS_STAT_RESTORE_TRAPS_STAT_RESTORE_ID,1)) 
  { 
    ret = ui_ppd_api_diag_traps_stat_restore(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_FRWRD_DECISION_TRACE_GET_FRWRD_DECISION_TRACE_GET_ID,1)) 
  { 
    ret = ui_ppd_api_diag_frwrd_decision_trace_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_LEARNING_INFO_GET_LEARNING_INFO_GET_ID,1)) 
  { 
    ret = ui_ppd_api_diag_learning_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_ING_VLAN_EDIT_INFO_GET_ING_VLAN_EDIT_INFO_GET_ID,1)) 
  { 
    ret = ui_ppd_api_diag_ing_vlan_edit_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_PKT_ASSOCIATED_TM_INFO_GET_PKT_ASSOCIATED_TM_INFO_GET_ID,1)) 
  { 
    ret = ui_ppd_api_diag_pkt_associated_tm_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_ENCAP_INFO_GET_ENCAP_INFO_GET_ID,1)) 
  { 
    ret = ui_ppd_api_diag_encap_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_EG_DROP_LOG_GET_EG_DROP_LOG_GET_ID,1)) 
  { 
    ret = ui_ppd_api_diag_eg_drop_log_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_DB_LIF_LKUP_INFO_GET_DB_LIF_LKUP_INFO_GET_ID,1)) 
  { 
    ret = ui_ppd_api_diag_db_lif_lkup_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_DB_LEM_LKUP_INFO_GET_DB_LEM_LKUP_INFO_GET_ID,1)) 
  { 
    ret = ui_ppd_api_diag_db_lem_lkup_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_DB_TCAM_LKUP_INFO_GET_DB_TCAM_LKUP_INFO_GET_ID,1)) 
  { 
    ret = ui_ppd_api_diag_db_tcam_lkup_info_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_DIAG_PKT_SEND_PKT_SEND_ID,1)) 
  { 
    ret = ui_ppd_api_diag_pkt_send(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after diag***", TRUE); 
  } 
  
  ui_ret = ret;
    
  goto exit; 
exit:        
  return ui_ret; 
}                
          
#endif /* diag */ 


#endif /* LINK_PPD_LIBRARIES */ 

