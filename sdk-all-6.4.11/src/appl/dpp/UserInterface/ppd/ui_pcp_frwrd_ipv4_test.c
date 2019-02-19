/* $Id: ui_pcp_frwrd_ipv4_test.c,v 1.6 Broadcom SDK $
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
#include <appl/diag/dpp/utils_defi.h> 
#include <appl/dpp/UserInterface/ui_defi.h> 
 
#include <appl/dpp/UserInterface/ui_pure_defi_ppd_api.h> 
 
  
#include <soc/dpp/PCP/pcp_frwrd_ipv4_test.h>
#include <appl/dpp/UserInterface/ppd/ui_pcp_frwrd_ipv4_test.h>

#if LINK_PPD_LIBRARIES

#ifdef UI_PPD_FRWRD_IPV4_TEST
/******************************************************************** 
 *  Function handler: frwrd_ipv4_test_clear_vrf (section pcp_frwrd_ipv4_test)
 ********************************************************************/
int 
  ui_pcp_frwrd_ipv4_test_frwrd_ipv4_test_clear_vrf(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  PCP_FRWRD_IPV4_CLEAR_INFO   
    prm_clear_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_frwrd_ipv4_test"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv4_test_clear_vrf"; 
 
  unit = soc_ppd_get_default_unit(); 
  PCP_FRWRD_IPV4_CLEAR_INFO_clear(&prm_clear_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_TEST_CLEAR_VRF_FRWRD_IPV4_TEST_CLEAR_VRF_CLEAR_INFO_VRF_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_TEST_CLEAR_VRF_FRWRD_IPV4_TEST_CLEAR_VRF_CLEAR_INFO_VRF_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_clear_info.vrf = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = pcp_frwrd_ipv4_test_clear_vrf(
          unit,
          &prm_clear_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_frwrd_ipv4_test_clear_vrf - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv4_test_clear_vrf");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: frwrd_ipv4_test_print_mem (section pcp_frwrd_ipv4_test)
 ********************************************************************/
int 
  ui_pcp_frwrd_ipv4_test_frwrd_ipv4_test_print_mem(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_frwrd_ipv4_test"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv4_test_print_mem"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 

  /* Call function */
  ret = pcp_frwrd_ipv4_test_print_mem(
          unit,
          0xff,
          4
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_frwrd_ipv4_test_print_mem - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv4_test_print_mem");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: frwrd_ipv4_test_lookup (section soc_ppd_frwrd_ipv4_test)
 ********************************************************************/
int 
  ui_pcp_frwrd_ipv4_test_frwrd_ipv4_test_lookup(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  PCP_FRWRD_IPV4_TEST_LKUP_INFO   
    prm_lkup_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_frwrd_ipv4_test"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv4_test_lookup"; 
 
  unit = soc_ppd_get_default_unit(); 
  PCP_FRWRD_IPV4_TEST_LKUP_INFO_clear(&prm_lkup_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_TEST_LOOKUP_FRWRD_IPV4_TEST_LOOKUP_LKUP_INFO_KEY_KEY_SUBNET_PREFIX_LEN_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_TEST_LOOKUP_FRWRD_IPV4_TEST_LOOKUP_LKUP_INFO_KEY_KEY_SUBNET_PREFIX_LEN_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lkup_info.key.key.subnet.prefix_len = (uint8)param_val->value.ulong_value;
  } 

  if (!get_val_of(
         current_line,(int *)&match_index,SOC_PARAM_PPD_FRWRD_IPV4_TEST_LOOKUP_FRWRD_IPV4_TEST_LOOKUP_LKUP_INFO_KEY_KEY_SUBNET_IP_ADDRESS_ID,1,  
         &param_val,VAL_IP,err_msg)) 
  { 
    prm_lkup_info.key.key.subnet.ip_address = param_val->value.ip_value;
  } 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_TEST_LOOKUP_FRWRD_IPV4_TEST_LOOKUP_LKUP_INFO_KEY_VRF_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_TEST_LOOKUP_FRWRD_IPV4_TEST_LOOKUP_LKUP_INFO_KEY_VRF_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_lkup_info.key.vrf = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = pcp_frwrd_ipv4_test_lookup(
          unit,
          &prm_lkup_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_ppd_frwrd_ipv4_test_lookup - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv4_test_lookup");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
}   
/******************************************************************** 
 *  Function handler: frwrd_ipv4_test_vrf (section pcp_frwrd_ipv4_test)
 ********************************************************************/
int 
  ui_pcp_frwrd_ipv4_test_frwrd_ipv4_test_vrf(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_vrf_ndx;
  uint32   
    prm_nof_iterations=0;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_frwrd_ipv4_test"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv4_test_vrf"; 
 
  unit = soc_ppd_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_TEST_VRF_FRWRD_IPV4_TEST_VRF_VRF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_TEST_VRF_FRWRD_IPV4_TEST_VRF_VRF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_vrf_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter vrf_ndx after frwrd_ipv4_test_vrf***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_TEST_VRF_FRWRD_IPV4_TEST_VRF_NOF_ITERATIONS_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_TEST_VRF_FRWRD_IPV4_TEST_VRF_NOF_ITERATIONS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_nof_iterations = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = pcp_frwrd_ipv4_test_vrf(
          unit,
          prm_vrf_ndx,
          prm_nof_iterations
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_frwrd_ipv4_test_vrf - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv4_test_vrf");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: frwrd_ipv4_test_run (section pcp_frwrd_ipv4_test)
 ********************************************************************/
int 
  ui_pcp_frwrd_ipv4_test_frwrd_ipv4_test_run(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  PCP_FRWRD_IPV4_TEST_INFO   
    prm_tes_info;
   
  UI_MACROS_INIT_FUNCTION("ui_ppd_frwrd_ipv4_test"); 
  soc_sand_proc_name = "soc_ppd_frwrd_ipv4_test_run"; 
 
  unit = soc_ppd_get_default_unit(); 
  PCP_FRWRD_IPV4_TEST_INFO_clear(&prm_tes_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_TEST_RUN_FRWRD_IPV4_TEST_RUN_TES_INFO_FEC_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_TEST_RUN_FRWRD_IPV4_TEST_RUN_TES_INFO_FEC_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tes_info.fec_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_TEST_RUN_FRWRD_IPV4_TEST_RUN_TES_INFO_ADD_PROB_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_TEST_RUN_FRWRD_IPV4_TEST_RUN_TES_INFO_ADD_PROB_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tes_info.add_prob = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_TEST_RUN_FRWRD_IPV4_TEST_RUN_TES_INFO_REMOVE_PROB_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_TEST_RUN_FRWRD_IPV4_TEST_RUN_TES_INFO_REMOVE_PROB_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tes_info.remove_prob = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_TEST_RUN_FRWRD_IPV4_TEST_RUN_TES_INFO_PRINT_LEVEL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_TEST_RUN_FRWRD_IPV4_TEST_RUN_TES_INFO_PRINT_LEVEL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tes_info.print_level = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_TEST_RUN_FRWRD_IPV4_TEST_RUN_TES_INFO_HW_TEST_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_TEST_RUN_FRWRD_IPV4_TEST_RUN_TES_INFO_HW_TEST_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tes_info.hw_test = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_TEST_RUN_FRWRD_IPV4_TEST_RUN_TES_INFO_TEST_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_TEST_RUN_FRWRD_IPV4_TEST_RUN_TES_INFO_TEST_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tes_info.test = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_TEST_RUN_FRWRD_IPV4_TEST_RUN_TES_INFO_CACHE_CHANGE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_TEST_RUN_FRWRD_IPV4_TEST_RUN_TES_INFO_CACHE_CHANGE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tes_info.cache_change = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_TEST_RUN_FRWRD_IPV4_TEST_RUN_TES_INFO_DEFRAGE_RATE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_TEST_RUN_FRWRD_IPV4_TEST_RUN_TES_INFO_DEFRAGE_RATE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tes_info.defragment_rate = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_TEST_RUN_FRWRD_IPV4_TEST_RUN_TES_INFO_DEFRAGE_BNK_BMP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_TEST_RUN_FRWRD_IPV4_TEST_RUN_TES_INFO_DEFRAGE_BNK_BMP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tes_info.defragment_banks_bmp = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_TEST_RUN_FRWRD_IPV4_TEST_RUN_TES_INFO_NOF_ROUTES_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_TEST_RUN_FRWRD_IPV4_TEST_RUN_TES_INFO_NOF_ROUTES_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tes_info.nof_routes = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_TEST_RUN_FRWRD_IPV4_TEST_RUN_TES_INFO_SEED_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(SOC_PARAM_PPD_FRWRD_IPV4_TEST_RUN_FRWRD_IPV4_TEST_RUN_TES_INFO_SEED_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_tes_info.seed = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = pcp_frwrd_ipv4_test_run(
          unit,
          &prm_tes_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_frwrd_ipv4_test_run - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_ppd_frwrd_ipv4_test_run");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
#endif
#ifdef UI_PPD_FRWRD_IPV4_TEST/* { pcp_frwrd_ipv4_test*/
/******************************************************************** 
 *  Section handler: pcp_frwrd_ipv4_test
 ********************************************************************/ 
int 
  ui_ppd_frwrd_ipv4_test_pcp( 
    CURRENT_LINE *current_line 
  ) 
{   
  int
    ret = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_ppd_frwrd_ipv4_test"); 
 
  if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_TEST_CLEAR_VRF_FRWRD_IPV4_TEST_CLEAR_VRF_ID,1)) 
  { 
    ret = ui_pcp_frwrd_ipv4_test_frwrd_ipv4_test_clear_vrf(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_TEST_PRINT_MEM_FRWRD_IPV4_TEST_PRINT_MEM_ID,1)) 
  { 
    ret = ui_pcp_frwrd_ipv4_test_frwrd_ipv4_test_print_mem(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_TEST_LOOKUP_FRWRD_IPV4_TEST_LOOKUP_ID,1)) 
  { 
    ret = ui_pcp_frwrd_ipv4_test_frwrd_ipv4_test_lookup(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_TEST_VRF_FRWRD_IPV4_TEST_VRF_ID,1)) 
  { 
    ret = ui_pcp_frwrd_ipv4_test_frwrd_ipv4_test_vrf(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,SOC_PARAM_PPD_FRWRD_IPV4_TEST_RUN_FRWRD_IPV4_TEST_RUN_ID,1)) 
  { 
    ret = ui_pcp_frwrd_ipv4_test_frwrd_ipv4_test_run(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after pcp_frwrd_ipv4_test***", TRUE); 
  } 

   ui_ret = ret;
    
  goto exit; 
exit:        
  return ui_ret; 
}                
          
#endif /* pcp_frwrd_ipv4_test */ 


#endif /* LINK_PPD_LIBRARIES */ 

