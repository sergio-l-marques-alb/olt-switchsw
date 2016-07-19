/* $Id: ui_pb_api_nif.c,v 1.7 Broadcom SDK $
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
 
  
#include <soc/dpp/Petra/PB_TM/pb_api_nif.h>
#include <appl/dpp/UserInterface/pb/ui_pb_api_nif.h>

#if LINK_PB_LIBRARIES

#ifdef UI_NIF
/******************************************************************** 
 *  Function handler: on_off_set (section nif)
 ********************************************************************/
int 
  ui_pb_api_nif_on_off_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;
  uint32
    prm_nif_ndx;
  SOC_PB_NIF_TYPE
    prm_nif_type;
  SOC_PETRA_INTERFACE_ID 
    prm_nif_id;
  SOC_PB_NIF_STATE_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_nif"); 
  soc_sand_proc_name = "soc_pb_nif_on_off_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_NIF_STATE_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_ON_OFF_SET_ON_OFF_SET_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_NIF_ON_OFF_SET_ON_OFF_SET_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_nif_type = param_val->numeric_equivalent;
  }
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter type after on_off_set***", TRUE); 
    goto exit; 
  } 
   
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_ON_OFF_SET_ON_OFF_SET_NIF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_ON_OFF_SET_ON_OFF_SET_NIF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_nif_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter nif_ndx after type***", TRUE); 
    goto exit; 
  } 
  
  /* Change format to SOC_PETRA_INTERFACE_ID */
  prm_nif_id = soc_pb_nif_type2id(prm_nif_type,prm_nif_ndx);

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_nif_on_off_get(
          unit,
          prm_nif_id,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_on_off_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_on_off_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_ON_OFF_SET_ON_OFF_SET_INFO_SERDES_ALSO_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_ON_OFF_SET_ON_OFF_SET_INFO_SERDES_ALSO_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.serdes_also = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_ON_OFF_SET_ON_OFF_SET_INFO_IS_NIF_ON_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_ON_OFF_SET_ON_OFF_SET_INFO_IS_NIF_ON_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.is_nif_on = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_nif_on_off_set(
          unit,
          prm_nif_id,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_on_off_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_on_off_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: on_off_get (section nif)
 ********************************************************************/
int 
  ui_pb_api_nif_on_off_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;  
  uint32
    prm_nif_ndx;
  SOC_PB_NIF_TYPE
    prm_nif_type;
  SOC_PETRA_INTERFACE_ID 
    prm_nif_id;
  SOC_PB_NIF_STATE_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_nif"); 
  soc_sand_proc_name = "soc_pb_nif_on_off_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_NIF_STATE_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_ON_OFF_GET_ON_OFF_GET_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_NIF_ON_OFF_GET_ON_OFF_GET_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_nif_type = param_val->numeric_equivalent;
  }
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter nif_type after on_off_get***", TRUE); 
    goto exit; 
  } 
  
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_ON_OFF_GET_ON_OFF_GET_NIF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_ON_OFF_GET_ON_OFF_GET_NIF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_nif_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter nif_ndx after type***", TRUE); 
    goto exit; 
  } 
  
  /* Change format to SOC_PETRA_INTERFACE_ID */
  prm_nif_id = soc_pb_nif_type2id(prm_nif_type,prm_nif_ndx);

  /* Call function */
  ret = soc_pb_nif_on_off_get(
          unit,
          prm_nif_id,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_on_off_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_on_off_get");   
    goto exit; 
  } 

  SOC_PB_NIF_STATE_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: loopback_set (section nif)
 ********************************************************************/
int 
  ui_pb_api_nif_loopback_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_NIF_TYPE
    prm_nif_type;
  uint32   
    prm_nif_ndx;
  SOC_PETRA_INTERFACE_ID 
    prm_nif_id;
  uint8   
    prm_enable;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_nif"); 
  soc_sand_proc_name = "soc_pb_nif_loopback_set"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_LOOPBACK_SET_LOOPBACK_SET_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_NIF_LOOPBACK_SET_LOOPBACK_SET_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_nif_type = param_val->numeric_equivalent;
  }
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter nif_type after loopback_set***", TRUE); 
    goto exit; 
  } 
  
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_LOOPBACK_SET_LOOPBACK_SET_NIF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_LOOPBACK_SET_LOOPBACK_SET_NIF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_nif_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter nif_ndx after type***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_LOOPBACK_SET_LOOPBACK_SET_NIF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_LOOPBACK_SET_LOOPBACK_SET_NIF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_nif_ndx = (SOC_PETRA_INTERFACE_ID)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter nif_ndx after loopback_set***", TRUE); 
    goto exit; 
  } 

  /* Change format to SOC_PETRA_INTERFACE_ID */
  prm_nif_id = soc_pb_nif_type2id(prm_nif_type,prm_nif_ndx);

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_nif_loopback_get(
          unit,
          prm_nif_id,
          &prm_enable
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_loopback_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_loopback_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_LOOPBACK_SET_LOOPBACK_SET_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_LOOPBACK_SET_LOOPBACK_SET_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_enable = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_nif_loopback_set(
          unit,
          prm_nif_id,
          prm_enable
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_loopback_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_loopback_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: loopback_get (section nif)
 ********************************************************************/
int 
  ui_pb_api_nif_loopback_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_NIF_TYPE
    prm_nif_type;
  uint32   
    prm_nif_ndx;
  SOC_PETRA_INTERFACE_ID 
    prm_nif_id;
  uint8   
    prm_enable;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_nif"); 
  soc_sand_proc_name = "soc_pb_nif_loopback_get"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_LOOPBACK_GET_LOOPBACK_GET_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_NIF_LOOPBACK_GET_LOOPBACK_GET_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_nif_type = param_val->numeric_equivalent;
  }
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter nif_type after loopback_set***", TRUE); 
    goto exit; 

  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_LOOPBACK_GET_LOOPBACK_GET_NIF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_LOOPBACK_GET_LOOPBACK_GET_NIF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_nif_ndx = (SOC_PETRA_INTERFACE_ID)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter nif_ndx after loopback_get***", TRUE); 
    goto exit; 
  } 

  /* Change format to SOC_PETRA_INTERFACE_ID */
  prm_nif_id = soc_pb_nif_type2id(prm_nif_type,prm_nif_ndx);

  /* Call function */
  ret = soc_pb_nif_loopback_get(
          unit,
          prm_nif_id,
          &prm_enable
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_loopback_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_loopback_get");   
    goto exit; 
  } 

  soc_sand_os_printf("enable: %u\n\r",prm_enable);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: mal_basic_conf_set (section nif)
 ********************************************************************/
int 
  ui_pb_api_nif_mal_basic_conf_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_mal_ndx;
  SOC_PB_NIF_MAL_BASIC_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_nif"); 
  soc_sand_proc_name = "soc_pb_nif_mal_basic_conf_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_NIF_MAL_BASIC_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_MAL_BASIC_CONF_SET_MAL_BASIC_CONF_SET_MAL_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_MAL_BASIC_CONF_SET_MAL_BASIC_CONF_SET_MAL_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_mal_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter mal_ndx after mal_basic_conf_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_nif_mal_basic_conf_get(
          unit,
          prm_mal_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_mal_basic_conf_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_mal_basic_conf_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_MAL_BASIC_CONF_SET_MAL_BASIC_CONF_SET_INFO_TOPOLOGY_IS_QSGMII_ALT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_MAL_BASIC_CONF_SET_MAL_BASIC_CONF_SET_INFO_TOPOLOGY_IS_QSGMII_ALT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.topology.is_qsgmii_alt = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_MAL_BASIC_CONF_SET_MAL_BASIC_CONF_SET_INFO_TOPOLOGY_LANES_SWAP_SWAP_TX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_MAL_BASIC_CONF_SET_MAL_BASIC_CONF_SET_INFO_TOPOLOGY_LANES_SWAP_SWAP_TX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.topology.lanes_swap.swap_tx = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_MAL_BASIC_CONF_SET_MAL_BASIC_CONF_SET_INFO_TOPOLOGY_LANES_SWAP_SWAP_RX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_MAL_BASIC_CONF_SET_MAL_BASIC_CONF_SET_INFO_TOPOLOGY_LANES_SWAP_SWAP_RX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.topology.lanes_swap.swap_rx = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_MAL_BASIC_CONF_SET_MAL_BASIC_CONF_SET_INFO_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_NIF_MAL_BASIC_CONF_SET_MAL_BASIC_CONF_SET_INFO_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.type = param_val->numeric_equivalent;
  } 


  /* Call function */
  ret = soc_pb_nif_mal_basic_conf_set(
          unit,
          prm_mal_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_mal_basic_conf_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_mal_basic_conf_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: mal_basic_conf_get (section nif)
 ********************************************************************/
int 
  ui_pb_api_nif_mal_basic_conf_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_mal_ndx;
  SOC_PB_NIF_MAL_BASIC_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_nif"); 
  soc_sand_proc_name = "soc_pb_nif_mal_basic_conf_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_NIF_MAL_BASIC_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_MAL_BASIC_CONF_GET_MAL_BASIC_CONF_GET_MAL_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_MAL_BASIC_CONF_GET_MAL_BASIC_CONF_GET_MAL_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_mal_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter mal_ndx after mal_basic_conf_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_nif_mal_basic_conf_get(
          unit,
          prm_mal_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_mal_basic_conf_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_mal_basic_conf_get");   
    goto exit; 
  } 

  SOC_PB_NIF_MAL_BASIC_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: min_packet_size_set (section nif)
 ********************************************************************/
int 
  ui_pb_api_nif_min_packet_size_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_mal_ndx;
  SOC_PETRA_CONNECTION_DIRECTION   
    prm_direction_ndx = SOC_PETRA_CONNECTION_DIRECTION_BOTH;
  uint32   
   prm_rx_pckt_size = 0,
   prm_tx_pckt_size = 0;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_nif"); 
  soc_sand_proc_name = "soc_pb_nif_min_packet_size_set"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_MIN_PACKET_SIZE_SET_MIN_PACKET_SIZE_SET_MAL_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_MIN_PACKET_SIZE_SET_MIN_PACKET_SIZE_SET_MAL_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_mal_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter mal_ndx after min_packet_size_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_nif_min_packet_size_get(
          unit,
          prm_mal_ndx,
          &prm_rx_pckt_size,
          &prm_tx_pckt_size
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_min_packet_size_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_min_packet_size_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_MIN_PACKET_SIZE_SET_MIN_PACKET_SIZE_SET_DIRECTION_NDX_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_NIF_MIN_PACKET_SIZE_SET_MIN_PACKET_SIZE_SET_DIRECTION_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_direction_ndx = (SOC_PETRA_CONNECTION_DIRECTION)param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_MIN_PACKET_SIZE_SET_MIN_PACKET_SIZE_SET_PCKT_SIZE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_MIN_PACKET_SIZE_SET_MIN_PACKET_SIZE_SET_PCKT_SIZE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rx_pckt_size = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_nif_min_packet_size_set(
          unit,
          prm_mal_ndx,
          prm_direction_ndx,
          prm_rx_pckt_size
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_min_packet_size_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_min_packet_size_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: min_packet_size_get (section nif)
 ********************************************************************/
int 
  ui_pb_api_nif_min_packet_size_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_mal_ndx;
  uint32
    prm_rx_pckt_size = 0,
    prm_tx_pckt_size = 0;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_nif"); 
  soc_sand_proc_name = "soc_pb_nif_min_packet_size_get"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_MIN_PACKET_SIZE_GET_MIN_PACKET_SIZE_GET_MAL_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_MIN_PACKET_SIZE_GET_MIN_PACKET_SIZE_GET_MAL_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;   
    prm_mal_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter mal_ndx after min_packet_size_get***", TRUE); 
    goto exit; 
  } 

  /* Call function */
  ret = soc_pb_nif_min_packet_size_get(
          unit,
          prm_mal_ndx,
          &prm_rx_pckt_size,
          &prm_tx_pckt_size
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_min_packet_size_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_min_packet_size_get");   
    goto exit; 
  } 

  send_string_to_screen("--> RX: \n\r", TRUE);
  soc_sand_os_printf("pckt_rx_size: %u, ",prm_rx_pckt_size);
  send_string_to_screen("\n\r", TRUE);
  send_string_to_screen("--> TX: \n\r", TRUE);
  soc_sand_os_printf("pckt_tx_size: %u\n\r",prm_tx_pckt_size);
  send_string_to_screen("\n\r", TRUE);


  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: spaui_conf_set (section nif)
 ********************************************************************/
int 
  ui_pb_api_nif_spaui_conf_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32
    prm_nif_ndx;
  SOC_PB_NIF_TYPE
    prm_nif_type;
  SOC_PETRA_INTERFACE_ID 
    prm_nif_id;
  SOC_PETRA_CONNECTION_DIRECTION   
    prm_direction_ndx = SOC_PETRA_CONNECTION_DIRECTION_BOTH;
  SOC_PB_NIF_SPAUI_INFO
    prm_rx_info,
    prm_tx_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_nif"); 
  soc_sand_proc_name = "soc_pb_nif_spaui_conf_set"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_SPAUI_CONF_SET_SPAUI_CONF_SET_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_NIF_SPAUI_CONF_SET_SPAUI_CONF_SET_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_nif_type = param_val->numeric_equivalent;
  }
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter type after spaui_conf_set***", TRUE); 
    goto exit; 
  } 
  
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_SPAUI_CONF_SET_SPAUI_CONF_SET_NIF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_SPAUI_CONF_SET_SPAUI_CONF_SET_NIF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_nif_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter nif_ndx after type***", TRUE); 
    goto exit; 
  } 
  
  /* Change format to SOC_PETRA_INTERFACE_ID */
  prm_nif_id = soc_pb_nif_type2id(prm_nif_type,prm_nif_ndx);

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_nif_spaui_conf_get(
          unit,
          prm_nif_id,
          &prm_rx_info,
          &prm_tx_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_spaui_conf_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_spaui_conf_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_SPAUI_CONF_SET_SPAUI_CONF_SET_DIRECTION_NDX_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_NIF_SPAUI_CONF_SET_SPAUI_CONF_SET_DIRECTION_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_direction_ndx = (SOC_PETRA_CONNECTION_DIRECTION)param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_SPAUI_CONF_SET_SPAUI_CONF_SET_INFO_FAULT_RESPONSE_REMOTE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_NIF_SPAUI_CONF_SET_SPAUI_CONF_SET_INFO_FAULT_RESPONSE_REMOTE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rx_info.fault_response.remote = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_SPAUI_CONF_SET_SPAUI_CONF_SET_INFO_FAULT_RESPONSE_LOCAL_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_NIF_SPAUI_CONF_SET_SPAUI_CONF_SET_INFO_FAULT_RESPONSE_LOCAL_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rx_info.fault_response.local = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_SPAUI_CONF_SET_SPAUI_CONF_SET_INFO_CH_CONF_IS_BURST_INTERLEAVING_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_SPAUI_CONF_SET_SPAUI_CONF_SET_INFO_CH_CONF_IS_BURST_INTERLEAVING_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rx_info.ch_conf.is_burst_interleaving = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_SPAUI_CONF_SET_SPAUI_CONF_SET_INFO_CH_CONF_BCT_CHANNEL_BYTE_NDX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_SPAUI_CONF_SET_SPAUI_CONF_SET_INFO_CH_CONF_BCT_CHANNEL_BYTE_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rx_info.ch_conf.bct_channel_byte_ndx = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_SPAUI_CONF_SET_SPAUI_CONF_SET_INFO_CH_CONF_BCT_SIZE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_SPAUI_CONF_SET_SPAUI_CONF_SET_INFO_CH_CONF_BCT_SIZE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rx_info.ch_conf.bct_size = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_SPAUI_CONF_SET_SPAUI_CONF_SET_INFO_CH_CONF_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_SPAUI_CONF_SET_SPAUI_CONF_SET_INFO_CH_CONF_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rx_info.ch_conf.enable = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_SPAUI_CONF_SET_SPAUI_CONF_SET_INFO_CRC_MODE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_NIF_SPAUI_CONF_SET_SPAUI_CONF_SET_INFO_CRC_MODE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rx_info.crc_mode = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_SPAUI_CONF_SET_SPAUI_CONF_SET_INFO_IPG_DIC_MODE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_NIF_SPAUI_CONF_SET_SPAUI_CONF_SET_INFO_IPG_DIC_MODE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rx_info.ipg.dic_mode = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_SPAUI_CONF_SET_SPAUI_CONF_SET_INFO_IPG_SIZE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_SPAUI_CONF_SET_SPAUI_CONF_SET_INFO_IPG_SIZE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rx_info.ipg.size = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_SPAUI_CONF_SET_SPAUI_CONF_SET_INFO_IPG_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_SPAUI_CONF_SET_SPAUI_CONF_SET_INFO_IPG_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rx_info.ipg.enable = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_SPAUI_CONF_SET_SPAUI_CONF_SET_INFO_PREAMBLE_SKIP_SOP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_SPAUI_CONF_SET_SPAUI_CONF_SET_INFO_PREAMBLE_SKIP_SOP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rx_info.preamble.skip_SOP = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_SPAUI_CONF_SET_SPAUI_CONF_SET_INFO_PREAMBLE_SIZE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_NIF_SPAUI_CONF_SET_SPAUI_CONF_SET_INFO_PREAMBLE_SIZE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rx_info.preamble.size = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_SPAUI_CONF_SET_SPAUI_CONF_SET_INFO_PREAMBLE_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_SPAUI_CONF_SET_SPAUI_CONF_SET_INFO_PREAMBLE_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rx_info.preamble.enable = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_SPAUI_CONF_SET_SPAUI_CONF_SET_INFO_IS_DOUBLE_SIZE_SOP_EVEN_ONLY_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_SPAUI_CONF_SET_SPAUI_CONF_SET_INFO_IS_DOUBLE_SIZE_SOP_EVEN_ONLY_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rx_info.is_double_size_sop_even_only = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_SPAUI_CONF_SET_SPAUI_CONF_SET_INFO_IS_DOUBLE_SIZE_SOP_ODD_ONLY_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_SPAUI_CONF_SET_SPAUI_CONF_SET_INFO_IS_DOUBLE_SIZE_SOP_ODD_ONLY_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rx_info.is_double_size_sop_odd_only = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_SPAUI_CONF_SET_SPAUI_CONF_SET_INFO_LINK_PARTNER_DOUBLE_SIZE_BUS_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_SPAUI_CONF_SET_SPAUI_CONF_SET_INFO_LINK_PARTNER_DOUBLE_SIZE_BUS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rx_info.link_partner_double_size_bus = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_SPAUI_CONF_SET_SPAUI_CONF_SET_INFO_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_SPAUI_CONF_SET_SPAUI_CONF_SET_INFO_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rx_info.enable = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_nif_spaui_conf_set(
          unit,
          prm_nif_id,
          prm_direction_ndx,
          &prm_rx_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_spaui_conf_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_spaui_conf_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: spaui_conf_get (section nif)
 ********************************************************************/
int 
  ui_pb_api_nif_spaui_conf_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PETRA_INTERFACE_ID   
    prm_nif_id;
  SOC_PB_NIF_TYPE
    prm_nif_type;
  uint32
    prm_nif_ndx;
  SOC_PB_NIF_SPAUI_INFO   
    prm_rx_info,
    prm_tx_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_nif"); 
  soc_sand_proc_name = "soc_pb_nif_spaui_conf_get"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_SPAUI_CONF_GET_SPAUI_CONF_GET_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_NIF_SPAUI_CONF_GET_SPAUI_CONF_GET_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_nif_type = param_val->numeric_equivalent;
  }
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter type after spaui_conf_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_SPAUI_CONF_GET_SPAUI_CONF_GET_NIF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_SPAUI_CONF_GET_SPAUI_CONF_GET_NIF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_nif_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter nif_ndx after type***", TRUE); 
    goto exit; 
  } 
  
  /* Change format to SOC_PETRA_INTERFACE_ID */
  prm_nif_id = soc_pb_nif_type2id(prm_nif_type,prm_nif_ndx);

  /* Call function */
  ret = soc_pb_nif_spaui_conf_get(
          unit,
          prm_nif_id,
          &prm_rx_info,
          &prm_tx_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_spaui_conf_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_spaui_conf_get");   
    goto exit; 
  } 

  send_string_to_screen("--> RX: \n\r", TRUE);
  send_string_to_screen("info:", TRUE);
  SOC_PB_NIF_SPAUI_INFO_print(&prm_rx_info);
  send_string_to_screen("\n\r", TRUE);
  send_string_to_screen("--> TX: \n\r", TRUE);
  send_string_to_screen("info:", TRUE);
  SOC_PB_NIF_SPAUI_INFO_print(&prm_tx_info);
  send_string_to_screen("\n\r", TRUE);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: gmii_conf_set (section nif)
 ********************************************************************/
int 
  ui_pb_api_nif_gmii_conf_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32
    prm_nif_ndx;
  SOC_PB_NIF_TYPE
    prm_nif_type;
  SOC_PETRA_INTERFACE_ID   
    prm_nif_id;
  SOC_PB_NIF_GMII_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_nif"); 
  soc_sand_proc_name = "soc_pb_nif_gmii_conf_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_NIF_GMII_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_GMII_CONF_SET_GMII_CONF_SET_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_NIF_GMII_CONF_SET_GMII_CONF_SET_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_nif_type = param_val->numeric_equivalent;
  }
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter type after gmii_conf_get***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_GMII_CONF_SET_GMII_CONF_SET_NIF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_GMII_CONF_SET_GMII_CONF_SET_NIF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_nif_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter nif_ndx after type***", TRUE); 
    goto exit; 
  } 
  
  /* Change format to SOC_PETRA_INTERFACE_ID */
  prm_nif_id = soc_pb_nif_type2id(prm_nif_type,prm_nif_ndx);
  
  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_nif_gmii_conf_get(
          unit,
          prm_nif_id,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_gmii_conf_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_gmii_conf_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_GMII_CONF_SET_GMII_CONF_SET_INFO_RATE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_NIF_GMII_CONF_SET_GMII_CONF_SET_INFO_RATE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.rate = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_GMII_CONF_SET_GMII_CONF_SET_INFO_MODE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_NIF_GMII_CONF_SET_GMII_CONF_SET_INFO_MODE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.mode = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_GMII_CONF_SET_GMII_CONF_SET_INFO_ENABLE_TX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_GMII_CONF_SET_GMII_CONF_SET_INFO_ENABLE_TX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.enable_tx = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_GMII_CONF_SET_GMII_CONF_SET_INFO_ENABLE_RX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_GMII_CONF_SET_GMII_CONF_SET_INFO_ENABLE_RX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.enable_rx = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_nif_gmii_conf_set(
          unit,
          prm_nif_id,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_gmii_conf_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_gmii_conf_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: gmii_conf_get (section nif)
 ********************************************************************/
int 
  ui_pb_api_nif_gmii_conf_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32
    prm_nif_ndx;
  SOC_PB_NIF_TYPE
    prm_nif_type;
  SOC_PETRA_INTERFACE_ID 
    prm_nif_id;  
  SOC_PB_NIF_GMII_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_nif"); 
  soc_sand_proc_name = "soc_pb_nif_gmii_conf_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_NIF_GMII_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_GMII_CONF_GET_GMII_CONF_GET_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_NIF_GMII_CONF_GET_GMII_CONF_GET_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_nif_type = param_val->numeric_equivalent;
  }
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter type after gmii_conf_get***", TRUE); 
    goto exit; 
  } 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_GMII_CONF_GET_GMII_CONF_GET_NIF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_GMII_CONF_GET_GMII_CONF_GET_NIF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_nif_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter nif_ndx after type***", TRUE); 
    goto exit; 
  } 
  
  /* Change format to SOC_PETRA_INTERFACE_ID */
  prm_nif_id = soc_pb_nif_type2id(prm_nif_type,prm_nif_ndx);
  
  /* Call function */
  ret = soc_pb_nif_gmii_conf_get(
          unit,
          prm_nif_id,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_gmii_conf_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_gmii_conf_get");   
    goto exit; 
  } 

  SOC_PB_NIF_GMII_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: gmii_status_get (section nif)
 ********************************************************************/
int 
  ui_pb_api_nif_gmii_status_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32
    prm_nif_ndx;
  SOC_PB_NIF_TYPE
    prm_nif_type;
  SOC_PETRA_INTERFACE_ID 
    prm_nif_id;
  SOC_PB_NIF_GMII_STAT   
    prm_status;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_nif"); 
  soc_sand_proc_name = "soc_pb_nif_gmii_status_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_NIF_GMII_STAT_clear(&prm_status);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_GMII_STATUS_GET_GMII_STATUS_GET_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_NIF_GMII_STATUS_GET_GMII_STATUS_GET_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_nif_type = param_val->numeric_equivalent;
  }
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter type after gmii_status_get***", TRUE); 
    goto exit; 
  } 
   
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_GMII_STATUS_GET_GMII_STATUS_GET_NIF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_GMII_STATUS_GET_GMII_STATUS_GET_NIF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_nif_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter nif_ndx after type***", TRUE); 
    goto exit; 
  } 
  
  /* Change format to SOC_PETRA_INTERFACE_ID */
  prm_nif_id = soc_pb_nif_type2id(prm_nif_type,prm_nif_ndx);  

  /* Call function */
  ret = soc_pb_nif_gmii_status_get(
          unit,
          prm_nif_id,
          &prm_status
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_gmii_status_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_gmii_status_get");   
    goto exit; 
  } 

  SOC_PB_NIF_GMII_STAT_print(&prm_status);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: gmii_rate_set (section nif)
 ********************************************************************/
int 
  ui_pb_api_nif_gmii_rate_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32
    prm_nif_ndx;
  SOC_PB_NIF_TYPE
    prm_nif_type;
  SOC_PETRA_INTERFACE_ID 
    prm_nif_id;
  SOC_PB_NIF_GMII_RATE   
    prm_rate;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_nif"); 
  soc_sand_proc_name = "soc_pb_nif_gmii_rate_set"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_GMII_RATE_SET_GMII_RATE_SET_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_NIF_GMII_RATE_SET_GMII_RATE_SET_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_nif_type = param_val->numeric_equivalent;
  }
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter type after gmii_rate_set***", TRUE); 
    goto exit; 
  } 
   
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_GMII_RATE_SET_GMII_RATE_SET_NIF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_GMII_RATE_SET_GMII_RATE_SET_NIF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_nif_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter nif_ndx after type***", TRUE); 
    goto exit; 
  } 
  
  /* Change format to SOC_PETRA_INTERFACE_ID */
  prm_nif_id = soc_pb_nif_type2id(prm_nif_type,prm_nif_ndx);
 
  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_nif_gmii_rate_get(
          unit,
          prm_nif_id,
          &prm_rate
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_gmii_rate_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_gmii_rate_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_GMII_RATE_SET_GMII_RATE_SET_RATE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_NIF_GMII_RATE_SET_GMII_RATE_SET_RATE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_rate = param_val->numeric_equivalent;
  } 


  /* Call function */
  ret = soc_pb_nif_gmii_rate_set(
          unit,
          prm_nif_id,
          prm_rate
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_gmii_rate_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_gmii_rate_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: gmii_rate_get (section nif)
 ********************************************************************/
int 
  ui_pb_api_nif_gmii_rate_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32
    prm_nif_ndx;
  SOC_PB_NIF_TYPE
    prm_nif_type;
  SOC_PETRA_INTERFACE_ID 
    prm_nif_id;
  SOC_PB_NIF_GMII_RATE   
    prm_rate;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_nif"); 
  soc_sand_proc_name = "soc_pb_nif_gmii_rate_get"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_GMII_RATE_GET_GMII_RATE_GET_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_NIF_GMII_RATE_GET_GMII_RATE_GET_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_nif_type = param_val->numeric_equivalent;
  }
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter type after gmii_rate_get***", TRUE); 
    goto exit; 
  } 
   
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_GMII_RATE_GET_GMII_RATE_GET_NIF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_GMII_RATE_GET_GMII_RATE_GET_NIF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_nif_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter nif_ndx after type***", TRUE); 
    goto exit; 
  } 
  
  /* Change format to SOC_PETRA_INTERFACE_ID */
  prm_nif_id = soc_pb_nif_type2id(prm_nif_type,prm_nif_ndx);
  
  /* Call function */
  ret = soc_pb_nif_gmii_rate_get(
          unit,
          prm_nif_id,
          &prm_rate
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_gmii_rate_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_gmii_rate_get");   
    goto exit; 
  } 

  soc_sand_os_printf("rate: %s\n\r",SOC_PB_NIF_GMII_RATE_to_string(prm_rate));

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: ilkn_set (section nif)
 ********************************************************************/
int 
  ui_pb_api_nif_ilkn_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_NIF_ILKN_ID   
    prm_ilkn_ndx;
  SOC_PETRA_CONNECTION_DIRECTION   
    prm_direction_ndx = SOC_PETRA_CONNECTION_DIRECTION_BOTH;
  SOC_PB_NIF_ILKN_INFO   
    prm_info_rx,
    prm_info_tx;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_nif"); 
  soc_sand_proc_name = "soc_pb_nif_ilkn_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_NIF_ILKN_INFO_clear(&prm_info_rx);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_ILKN_SET_ILKN_SET_ILKN_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_NIF_ILKN_SET_ILKN_SET_ILKN_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_ilkn_ndx = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter ilkn_ndx after ilkn_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_nif_ilkn_get(
          unit,
          prm_ilkn_ndx,
          &prm_info_rx,
          &prm_info_tx
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_ilkn_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_ilkn_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_ILKN_SET_ILKN_SET_DIRECTION_NDX_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_NIF_ILKN_SET_ILKN_SET_DIRECTION_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_direction_ndx = (SOC_PETRA_CONNECTION_DIRECTION)param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_ILKN_SET_ILKN_SET_INFO_METAFRAME_SYNC_PERIOD_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_ILKN_SET_ILKN_SET_INFO_METAFRAME_SYNC_PERIOD_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info_rx.metaframe_sync_period = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_ILKN_SET_ILKN_SET_INFO_IS_BURST_INTERLEAVING_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_ILKN_SET_ILKN_SET_INFO_IS_BURST_INTERLEAVING_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info_rx.is_burst_interleaving = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_ILKN_SET_ILKN_SET_INFO_INVALID_LANE_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_ILKN_SET_ILKN_SET_INFO_INVALID_LANE_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info_rx.invalid_lane_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_ILKN_SET_ILKN_SET_INFO_IS_INVALID_LANE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_ILKN_SET_ILKN_SET_INFO_IS_INVALID_LANE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info_rx.is_invalid_lane = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_ILKN_SET_ILKN_SET_INFO_NOF_LANES_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_ILKN_SET_ILKN_SET_INFO_NOF_LANES_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info_rx.nof_lanes = (uint32)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_nif_ilkn_set(
          unit,
          prm_ilkn_ndx,
          prm_direction_ndx,
          &prm_info_rx
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_ilkn_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_ilkn_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: ilkn_get (section nif)
 ********************************************************************/
int 
  ui_pb_api_nif_ilkn_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_NIF_ILKN_ID   
    prm_ilkn_ndx;
  SOC_PB_NIF_ILKN_INFO   
    prm_info_rx,
    prm_info_tx;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_nif"); 
  soc_sand_proc_name = "soc_pb_nif_ilkn_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_NIF_ILKN_INFO_clear(&prm_info_rx);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_ILKN_GET_ILKN_GET_ILKN_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_NIF_ILKN_GET_ILKN_GET_ILKN_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_ilkn_ndx = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter ilkn_ndx after ilkn_get***", TRUE); 
    goto exit; 
  } 

  /* Call function */
  ret = soc_pb_nif_ilkn_get(
          unit,
          prm_ilkn_ndx,
          &prm_info_rx,
          &prm_info_tx
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_ilkn_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_ilkn_get");   
    goto exit; 
  } 

  SOC_PB_NIF_ILKN_INFO_print(&prm_info_rx);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: ilkn_diag (section nif)
 ********************************************************************/
int 
  ui_pb_api_nif_ilkn_diag(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_NIF_ILKN_ID   
    prm_ilkn_ndx;
  uint8   
    prm_clear_interrupts=0;
  SOC_PB_NIF_ILKN_DIAG_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_nif"); 
  soc_sand_proc_name = "soc_pb_nif_ilkn_diag"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_NIF_ILKN_DIAG_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_ILKN_DIAG_ILKN_DIAG_ILKN_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_NIF_ILKN_DIAG_ILKN_DIAG_ILKN_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_ilkn_ndx = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter ilkn_ndx after ilkn_diag***", TRUE); 
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_ILKN_DIAG_ILKN_DIAG_CLEAR_INTERRUPTS_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_ILKN_DIAG_ILKN_DIAG_CLEAR_INTERRUPTS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_clear_interrupts = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_nif_ilkn_diag(
          unit,
          prm_ilkn_ndx,
          prm_clear_interrupts,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_ilkn_diag - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_ilkn_diag");   
    goto exit; 
  } 

  SOC_PB_NIF_ILKN_DIAG_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: stat_activity_get (section nif)
 ********************************************************************/
int 
  ui_pb_api_nif_stat_activity_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32
    prm_nif_ndx;
  SOC_PB_NIF_TYPE
    prm_nif_type;
  SOC_PETRA_INTERFACE_ID 
    prm_nif_id;
  SOC_PB_NIF_ACTIVITY_STATUS   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_nif"); 
  soc_sand_proc_name = "soc_pb_nif_stat_activity_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_NIF_ACTIVITY_STATUS_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_STAT_ACTIVITY_GET_STAT_ACTIVITY_GET_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_NIF_STAT_ACTIVITY_GET_STAT_ACTIVITY_GET_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_nif_type = param_val->numeric_equivalent;
  }
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter type after stat_activity_get***", TRUE); 
    goto exit; 
  } 
   
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_STAT_ACTIVITY_GET_STAT_ACTIVITY_GET_NIF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_STAT_ACTIVITY_GET_STAT_ACTIVITY_GET_NIF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_nif_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter nif_ndx after type***", TRUE); 
    goto exit; 
  } 
  
  /* Change format to SOC_PETRA_INTERFACE_ID */
  prm_nif_id = soc_pb_nif_type2id(prm_nif_type,prm_nif_ndx);

  /* Call function */
  ret = soc_pb_nif_stat_activity_get(
          unit,
          prm_nif_id,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_stat_activity_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_stat_activity_get");   
    goto exit; 
  } 

  SOC_PB_NIF_ACTIVITY_STATUS_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: stat_activity_all_get (section nif)
 ********************************************************************/
int 
  ui_pb_api_nif_stat_activity_all_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_NIF_ACTIVITY_STATUS_ALL   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_nif"); 
  soc_sand_proc_name = "soc_pb_nif_stat_activity_all_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_NIF_ACTIVITY_STATUS_ALL_clear(&prm_info);
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_pb_nif_stat_activity_all_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_stat_activity_all_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_stat_activity_all_get");   
    goto exit; 
  } 

  SOC_PB_NIF_ACTIVITY_STATUS_ALL_print(unit, &prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: link_status_get (section nif)
 ********************************************************************/
int 
  ui_pb_api_nif_link_status_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32
    prm_nif_ndx;
  SOC_PB_NIF_TYPE
    prm_nif_type;
  SOC_PETRA_INTERFACE_ID 
    prm_nif_id;
  SOC_PB_NIF_LINK_STATUS   
    prm_link_status;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_nif"); 
  soc_sand_proc_name = "soc_pb_nif_link_status_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_NIF_LINK_STATUS_clear(&prm_link_status);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_LINK_STATUS_GET_LINK_STATUS_GET_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_NIF_LINK_STATUS_GET_LINK_STATUS_GET_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_nif_type = param_val->numeric_equivalent;
  }
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter type after link_status_get***", TRUE); 
    goto exit; 
  } 
   
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_LINK_STATUS_GET_LINK_STATUS_GET_NIF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_LINK_STATUS_GET_LINK_STATUS_GET_NIF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_nif_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter nif_ndx after type***", TRUE); 
    goto exit; 
  } 
  
  /* Change format to SOC_PETRA_INTERFACE_ID */
  prm_nif_id = soc_pb_nif_type2id(prm_nif_type,prm_nif_ndx);
  
  /* Call function */
  ret = soc_pb_nif_link_status_get(
          unit,
          prm_nif_id,
          &prm_link_status
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_link_status_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_link_status_get");   
    goto exit; 
  } 

  SOC_PB_NIF_LINK_STATUS_print(&prm_link_status);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: synce_clk_set (section nif)
 ********************************************************************/
int 
  ui_pb_api_nif_synce_clk_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_NIF_SYNCE_CLK_ID   
    prm_clk_ndx;
  SOC_PB_NIF_SYNCE_CLK   
    prm_clk;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_nif"); 
  soc_sand_proc_name = "soc_pb_nif_synce_clk_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_NIF_SYNCE_CLK_clear(&prm_clk);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_SYNCE_CLK_SET_SYNCE_CLK_SET_CLK_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_NIF_SYNCE_CLK_SET_SYNCE_CLK_SET_CLK_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_clk_ndx = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter clk_ndx after synce_clk_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_nif_synce_clk_get(
          unit,
          prm_clk_ndx,
          &prm_clk
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_synce_clk_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_synce_clk_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_SYNCE_CLK_SET_SYNCE_CLK_SET_CLK_SQUELCH_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_SYNCE_CLK_SET_SYNCE_CLK_SET_CLK_SQUELCH_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_clk.squelch_enable = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_SYNCE_CLK_SET_SYNCE_CLK_SET_CLK_CLK_DIVIDER_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_NIF_SYNCE_CLK_SET_SYNCE_CLK_SET_CLK_CLK_DIVIDER_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_clk.clk_divider = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_SYNCE_CLK_SET_SYNCE_CLK_SET_CLK_NIF_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_SYNCE_CLK_SET_SYNCE_CLK_SET_CLK_NIF_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_clk.nif_id = (SOC_PETRA_INTERFACE_ID)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_SYNCE_CLK_SET_SYNCE_CLK_SET_CLK_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_SYNCE_CLK_SET_SYNCE_CLK_SET_CLK_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_clk.enable = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_nif_synce_clk_set(
          unit,
          prm_clk_ndx,
          &prm_clk
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_synce_clk_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_synce_clk_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: synce_clk_get (section nif)
 ********************************************************************/
int 
  ui_pb_api_nif_synce_clk_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_NIF_SYNCE_CLK_ID   
    prm_clk_ndx;
  SOC_PB_NIF_SYNCE_CLK   
    prm_clk;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_nif"); 
  soc_sand_proc_name = "soc_pb_nif_synce_clk_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_NIF_SYNCE_CLK_clear(&prm_clk);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_SYNCE_CLK_GET_SYNCE_CLK_GET_CLK_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_NIF_SYNCE_CLK_GET_SYNCE_CLK_GET_CLK_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_clk_ndx = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter clk_ndx after synce_clk_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_nif_synce_clk_get(
          unit,
          prm_clk_ndx,
          &prm_clk
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_synce_clk_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_synce_clk_get");   
    goto exit; 
  } 

  SOC_PB_NIF_SYNCE_CLK_print(&prm_clk);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: synce_mode_set (section nif)
 ********************************************************************/
int 
  ui_pb_api_nif_synce_mode_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_NIF_SYNCE_MODE   
    prm_mode;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_nif"); 
  soc_sand_proc_name = "soc_pb_nif_synce_mode_set"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_nif_synce_mode_get(
          unit,
          &prm_mode
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_synce_mode_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_synce_mode_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_SYNCE_MODE_SET_SYNCE_MODE_SET_MODE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_NIF_SYNCE_MODE_SET_SYNCE_MODE_SET_MODE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_mode = param_val->numeric_equivalent;
  } 


  /* Call function */
  ret = soc_pb_nif_synce_mode_set(
          unit,
          prm_mode
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_synce_mode_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_synce_mode_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: synce_mode_get (section nif)
 ********************************************************************/
int 
  ui_pb_api_nif_synce_mode_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_NIF_SYNCE_MODE   
    prm_mode;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_nif"); 
  soc_sand_proc_name = "soc_pb_nif_synce_mode_get"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_pb_nif_synce_mode_get(
          unit,
          &prm_mode
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_synce_mode_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_synce_mode_get");   
    goto exit; 
  } 

  soc_sand_os_printf("mode: %s\n\r",SOC_PB_NIF_SYNCE_MODE_to_string(prm_mode));

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: ptp_clk_reset_value_get (section nif)
 ********************************************************************/
int 
  ui_pb_api_nif_ptp_clk_reset_value_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_NIF_PTP_CLK_RST_VAL   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_nif"); 
  soc_sand_proc_name = "soc_pb_nif_ptp_clk_reset_value_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_NIF_PTP_CLK_RST_VAL_clear(&prm_info);
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_pb_nif_ptp_clk_reset_value_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_ptp_clk_reset_value_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_ptp_clk_reset_value_get");   
    goto exit; 
  } 

  SOC_PB_NIF_PTP_CLK_RST_VAL_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: ptp_clk_set (section nif)
 ********************************************************************/
int 
  ui_pb_api_nif_ptp_clk_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;     
  SOC_PB_NIF_PTP_CLK_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_nif"); 
  soc_sand_proc_name = "soc_pb_nif_ptp_clk_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_NIF_PTP_CLK_INFO_clear(&prm_info);
 
  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_nif_ptp_clk_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_ptp_clk_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_ptp_clk_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_PTP_CLK_SET_PTP_CLK_SET_INFO_SYNC_AUTOINC_INTERVAL_LSB_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_PTP_CLK_SET_PTP_CLK_SET_INFO_SYNC_AUTOINC_INTERVAL_LSB_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.sync_autoinc_interval.arr[0] = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_PTP_CLK_SET_PTP_CLK_SET_INFO_SYNC_AUTOINC_INTERVAL_MSB_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_PTP_CLK_SET_PTP_CLK_SET_INFO_SYNC_AUTOINC_INTERVAL_MSB_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.sync_autoinc_interval.arr[1] = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_PTP_CLK_SET_PTP_CLK_SET_INFO_SYNC_RESET_VAL_LSB_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_PTP_CLK_SET_PTP_CLK_SET_INFO_SYNC_RESET_VAL_LSB_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.sync_reset_val.arr[0] = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_PTP_CLK_SET_PTP_CLK_SET_INFO_SYNC_RESET_VAL_MSB_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_PTP_CLK_SET_PTP_CLK_SET_INFO_SYNC_RESET_VAL_MSB_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.sync_reset_val.arr[1] = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_PTP_CLK_SET_PTP_CLK_SET_INFO_CLK_FREQ_KHZ_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_PTP_CLK_SET_PTP_CLK_SET_INFO_CLK_FREQ_KHZ_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.ptp_clk_delta = (uint32)param_val->value.ulong_value;
  } 

  /* Call function */
  ret = soc_pb_nif_ptp_clk_set(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_ptp_clk_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_ptp_clk_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: ptp_clk_get (section nif)
 ********************************************************************/
int 
  ui_pb_api_nif_ptp_clk_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_NIF_PTP_CLK_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_nif"); 
  soc_sand_proc_name = "soc_pb_nif_ptp_clk_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_NIF_PTP_CLK_INFO_clear(&prm_info);
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_pb_nif_ptp_clk_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_ptp_clk_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_ptp_clk_get");   
    goto exit; 
  } 

  SOC_PB_NIF_PTP_CLK_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: ptp_conf_set (section nif)
 ********************************************************************/
int 
  ui_pb_api_nif_ptp_conf_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32
    prm_nif_ndx;
  SOC_PB_NIF_TYPE
    prm_nif_type;
  SOC_PETRA_INTERFACE_ID 
    prm_nif_id;
  SOC_PETRA_CONNECTION_DIRECTION   
    prm_direction_ndx = SOC_PETRA_CONNECTION_DIRECTION_BOTH;
  SOC_PB_NIF_PTP_INFO   
    prm_info_rx,
    prm_info_tx;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_nif"); 
  soc_sand_proc_name = "soc_pb_nif_ptp_conf_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_NIF_PTP_INFO_clear(&prm_info_rx);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_PTP_CONF_SET_PTP_CONF_SET_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_NIF_PTP_CONF_SET_PTP_CONF_SET_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_nif_type = param_val->numeric_equivalent;
  }
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter type after ptp_conf_set***", TRUE); 
    goto exit; 
  } 
   
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_PTP_CONF_SET_PTP_CONF_SET_NIF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_PTP_CONF_SET_PTP_CONF_SET_NIF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_nif_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter nif_ndx after type***", TRUE); 
    goto exit; 
  } 
  
  /* Change format to SOC_PETRA_INTERFACE_ID */
  prm_nif_id = soc_pb_nif_type2id(prm_nif_type,prm_nif_ndx);
  
  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_nif_ptp_conf_get(
          unit,
          prm_nif_id,
          &prm_info_rx,
          &prm_info_tx
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_ptp_conf_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_ptp_conf_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_PTP_CONF_SET_PTP_CONF_SET_DIRECTION_NDX_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_NIF_PTP_CONF_SET_PTP_CONF_SET_DIRECTION_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_direction_ndx = (SOC_PETRA_CONNECTION_DIRECTION)param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_PTP_CONF_SET_PTP_CONF_SET_INFO_WIRE_DELAY_NS_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_PTP_CONF_SET_PTP_CONF_SET_INFO_WIRE_DELAY_NS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info_rx.wire_delay_ns = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_PTP_CONF_SET_PTP_CONF_SET_INFO_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_PTP_CONF_SET_PTP_CONF_SET_INFO_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info_rx.enable = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_nif_ptp_conf_set(
          unit,
          prm_nif_id,
          prm_direction_ndx,
          &prm_info_rx
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_ptp_conf_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_ptp_conf_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: ptp_conf_get (section nif)
 ********************************************************************/
int 
  ui_pb_api_nif_ptp_conf_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32
    prm_nif_ndx;
  SOC_PB_NIF_TYPE
    prm_nif_type;
  SOC_PETRA_INTERFACE_ID 
    prm_nif_id;
  SOC_PB_NIF_PTP_INFO   
    prm_info_rx,
    prm_info_tx;
  uint8
    is_equal;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_nif"); 
  soc_sand_proc_name = "soc_pb_nif_ptp_conf_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_NIF_PTP_INFO_clear(&prm_info_rx);
  SOC_PB_NIF_PTP_INFO_clear(&prm_info_tx);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_PTP_CONF_GET_PTP_CONF_GET_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_NIF_PTP_CONF_GET_PTP_CONF_GET_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_nif_type = param_val->numeric_equivalent;
  }
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter type after ptp_conf_get***", TRUE); 
    goto exit; 
  } 
   
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_PTP_CONF_GET_PTP_CONF_GET_NIF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_PTP_CONF_GET_PTP_CONF_GET_NIF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_nif_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter nif_ndx after type***", TRUE); 
    goto exit; 
  }    

  /* Change format to SOC_PETRA_INTERFACE_ID */
  prm_nif_id = soc_pb_nif_type2id(prm_nif_type,prm_nif_ndx);
  
  /* Call function */
  ret = soc_pb_nif_ptp_conf_get(
          unit,
          prm_nif_id,
          &prm_info_rx,
          &prm_info_tx
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_ptp_conf_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_ptp_conf_get");   
    goto exit; 
  } 

  UI_COMP(&prm_info_rx, &prm_info_tx, SOC_PB_NIF_PTP_INFO, 1, is_equal);
  if (is_equal)
  {
    SOC_PB_NIF_PTP_INFO_print(&prm_info_rx);
  }
  else
  {
    send_string_to_screen("--> RX: ", TRUE);
    SOC_PB_NIF_PTP_INFO_print(&prm_info_rx);
    send_string_to_screen("--> TX: ", TRUE);
    SOC_PB_NIF_PTP_INFO_print(&prm_info_rx);
  }
  soc_sand_os_printf("\n\r");

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: elk_set (section nif)
 ********************************************************************/
int 
  ui_pb_api_nif_elk_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_NIF_ELK_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_nif"); 
  soc_sand_proc_name = "soc_pb_nif_elk_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_NIF_ELK_INFO_clear(&prm_info);
 
  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_nif_elk_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_elk_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_elk_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_ELK_SET_ELK_SET_INFO_MAL_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_ELK_SET_ELK_SET_INFO_MAL_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.mal_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_ELK_SET_ELK_SET_INFO_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_ELK_SET_ELK_SET_INFO_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.enable = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_nif_elk_set(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_elk_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_elk_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: elk_get (section nif)
 ********************************************************************/
int 
  ui_pb_api_nif_elk_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_NIF_ELK_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_nif"); 
  soc_sand_proc_name = "soc_pb_nif_elk_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_NIF_ELK_INFO_clear(&prm_info);
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_pb_nif_elk_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_elk_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_elk_get");   
    goto exit; 
  } 

  SOC_PB_NIF_ELK_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: fatp_mode_set (section nif)
 ********************************************************************/
int 
  ui_pb_api_nif_fatp_mode_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_NIF_FATP_MODE_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_nif"); 
  soc_sand_proc_name = "soc_pb_nif_fatp_mode_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_NIF_FATP_MODE_INFO_clear(&prm_info);
 
  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_nif_fatp_mode_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_fatp_mode_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_fatp_mode_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_FATP_MODE_SET_FATP_MODE_SET_INFO_IS_BYPASS_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_FATP_MODE_SET_FATP_MODE_SET_INFO_IS_BYPASS_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.is_bypass_enable = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_FATP_MODE_SET_FATP_MODE_SET_INFO_MODE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_NIF_FATP_MODE_SET_FATP_MODE_SET_INFO_MODE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.mode = param_val->numeric_equivalent;
  } 


  /* Call function */
  ret = soc_pb_nif_fatp_mode_set(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_fatp_mode_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_fatp_mode_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: fatp_mode_get (section nif)
 ********************************************************************/
int 
  ui_pb_api_nif_fatp_mode_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_NIF_FATP_MODE_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_nif"); 
  soc_sand_proc_name = "soc_pb_nif_fatp_mode_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_NIF_FATP_MODE_INFO_clear(&prm_info);
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_pb_nif_fatp_mode_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_fatp_mode_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_fatp_mode_get");   
    goto exit; 
  } 

  SOC_PB_NIF_FATP_MODE_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: fatp_set (section nif)
 ********************************************************************/
int 
  ui_pb_api_nif_fatp_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_NIF_FATP_ID   
    prm_fatp_ndx;
  SOC_PB_NIF_FATP_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_nif"); 
  soc_sand_proc_name = "soc_pb_nif_fatp_set"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_NIF_FATP_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_FATP_SET_FATP_SET_FATP_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_NIF_FATP_SET_FATP_SET_FATP_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fatp_ndx = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter fatp_ndx after fatp_set***", TRUE); 
    goto exit; 
  } 


  /* This is a set function, so call GET function first */                                                                                
  ret = soc_pb_nif_fatp_get(
          unit,
          prm_fatp_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_fatp_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_fatp_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_FATP_SET_FATP_SET_INFO_NOF_PORTS_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_FATP_SET_FATP_SET_INFO_NOF_PORTS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.nof_ports = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_FATP_SET_FATP_SET_INFO_BASE_PORT_ID_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_FATP_SET_FATP_SET_INFO_BASE_PORT_ID_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.base_port_id = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_FATP_SET_FATP_SET_INFO_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_FATP_SET_FATP_SET_INFO_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.enable = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = soc_pb_nif_fatp_set(
          unit,
          prm_fatp_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_fatp_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_fatp_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: fatp_get (section nif)
 ********************************************************************/
int 
  ui_pb_api_nif_fatp_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_PB_NIF_FATP_ID   
    prm_fatp_ndx;
  SOC_PB_NIF_FATP_INFO   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_nif"); 
  soc_sand_proc_name = "soc_pb_nif_fatp_get"; 
 
  unit = soc_pb_get_default_unit(); 
  SOC_PB_NIF_FATP_INFO_clear(&prm_info);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_FATP_GET_FATP_GET_FATP_NDX_ID,1))          
  {  
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_NIF_FATP_GET_FATP_GET_FATP_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_fatp_ndx = param_val->numeric_equivalent;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter fatp_ndx after fatp_get***", TRUE); 
    goto exit; 
  } 


  /* Call function */
  ret = soc_pb_nif_fatp_get(
          unit,
          prm_fatp_ndx,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_fatp_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_fatp_get");   
    goto exit; 
  } 

  SOC_PB_NIF_FATP_INFO_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: counter_get (section nif)
 ********************************************************************/
int 
  ui_pb_api_nif_counter_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32
    prm_nif_ndx;
  SOC_PB_NIF_TYPE
    prm_nif_type;
  SOC_PETRA_INTERFACE_ID 
    prm_nif_id;
  SOC_PB_NIF_COUNTER_TYPE   
    prm_counter_type = 0;
  SOC_SAND_64CNT   
    prm_counter_val;
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_nif"); 
  soc_sand_proc_name = "soc_pb_nif_counter_get"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_COUNTER_GET_COUNTER_GET_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_NIF_COUNTER_GET_COUNTER_GET_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_nif_type = param_val->numeric_equivalent;
  }
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter type after counter_get***", TRUE); 
    goto exit; 
  } 
   
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_COUNTER_GET_COUNTER_GET_NIF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_COUNTER_GET_COUNTER_GET_NIF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_nif_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter nif_ndx after type***", TRUE); 
    goto exit; 
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_COUNTER_GET_COUNTER_GET_COUNTER_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_NIF_COUNTER_GET_COUNTER_GET_COUNTER_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_counter_type = param_val->numeric_equivalent;
  } 

  /* Change format to SOC_PETRA_INTERFACE_ID */
  prm_nif_id = soc_pb_nif_type2id(prm_nif_type,prm_nif_ndx);
  
  /* Call function */
  ret = soc_pb_nif_counter_get(
          unit,
          prm_nif_id,
          prm_counter_type,
          &prm_counter_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_counter_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_counter_get");   
    goto exit; 
  } 

  soc_sand_64cnt_print(&prm_counter_val, 0);
  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: all_counters_get (section nif)
 ********************************************************************/
int 
  ui_pb_api_nif_all_counters_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32
    prm_nif_ndx;
  SOC_PB_NIF_TYPE
    prm_nif_type;
  SOC_PETRA_INTERFACE_ID 
    prm_nif_id;

  SOC_SAND_64CNT   
    prm_counter_val[SOC_PB_NIF_NOF_COUNTERS];
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_nif"); 
  soc_sand_proc_name = "soc_pb_nif_all_counters_get"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_ALL_COUNTERS_GET_ALL_COUNTERS_GET_TYPE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PB_NIF_ALL_COUNTERS_GET_ALL_COUNTERS_GET_TYPE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_nif_type = param_val->numeric_equivalent;
  }
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter type after all_counter_get***", TRUE); 
    goto exit; 
  } 
   
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_ALL_COUNTERS_GET_ALL_COUNTERS_GET_NIF_NDX_ID,1))          
  {  
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PB_NIF_ALL_COUNTERS_GET_ALL_COUNTERS_GET_NIF_NDX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_nif_ndx = (uint32)param_val->value.ulong_value;
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting parameter nif_ndx after type***", TRUE); 
    goto exit; 
  } 
  
  /* Change format to SOC_PETRA_INTERFACE_ID */
  prm_nif_id = soc_pb_nif_type2id(prm_nif_type,prm_nif_ndx);
  
  /* Call function */
  ret = soc_pb_nif_all_counters_get(
          unit,
          prm_nif_id,
          FALSE,
          prm_counter_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_all_counters_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_all_counters_get");   
    goto exit; 
  } 

  SOC_PB_NIF_ALL_STATISTIC_COUNTERS_print(prm_counter_val);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: all_nifs_all_counters_get (section nif)
 ********************************************************************/
int 
  ui_pb_api_nif_all_nifs_all_counters_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  SOC_SAND_64CNT   
    prm_counters_val[SOC_PB_NIF_NOF_NIFS][SOC_PB_NIF_NOF_COUNTERS];
   
  UI_MACROS_INIT_FUNCTION("ui_pb_api_nif"); 
  soc_sand_proc_name = "soc_pb_nif_all_nifs_all_counters_get"; 
 
  unit = soc_pb_get_default_unit(); 
 
  /* Get parameters */ 

  /* Call function */
  ret = soc_pb_nif_all_nifs_all_counters_get(
          unit,
          FALSE,
          prm_counters_val
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** soc_pb_nif_all_nifs_all_counters_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "soc_pb_nif_all_nifs_all_counters_get");   
    goto exit; 
  } 

  SOC_PB_NIF_ALL_NIFS_ALL_STATISTIC_COUNTERS_print(unit, prm_counters_val);
  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
#endif
#ifdef UI_NIF/* { nif*/
/******************************************************************** 
 *  Section handler: nif
 ********************************************************************/ 
int 
  ui_pb_api_nif( 
    CURRENT_LINE *current_line 
  ) 
{   
  int
    ret = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_pb_api_nif"); 
 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_ON_OFF_SET_ON_OFF_SET_ID,1)) 
  { 
    ret = ui_pb_api_nif_on_off_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_ON_OFF_GET_ON_OFF_GET_ID,1)) 
  { 
    ret = ui_pb_api_nif_on_off_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_LOOPBACK_SET_LOOPBACK_SET_ID,1)) 
  { 
    ret = ui_pb_api_nif_loopback_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_LOOPBACK_GET_LOOPBACK_GET_ID,1)) 
  { 
    ret = ui_pb_api_nif_loopback_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_MAL_BASIC_CONF_SET_MAL_BASIC_CONF_SET_ID,1)) 
  { 
    ret = ui_pb_api_nif_mal_basic_conf_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_MAL_BASIC_CONF_GET_MAL_BASIC_CONF_GET_ID,1)) 
  { 
    ret = ui_pb_api_nif_mal_basic_conf_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_MIN_PACKET_SIZE_SET_MIN_PACKET_SIZE_SET_ID,1)) 
  { 
    ret = ui_pb_api_nif_min_packet_size_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_MIN_PACKET_SIZE_GET_MIN_PACKET_SIZE_GET_ID,1)) 
  { 
    ret = ui_pb_api_nif_min_packet_size_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_SPAUI_CONF_SET_SPAUI_CONF_SET_ID,1)) 
  { 
    ret = ui_pb_api_nif_spaui_conf_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_SPAUI_CONF_GET_SPAUI_CONF_GET_ID,1)) 
  { 
    ret = ui_pb_api_nif_spaui_conf_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_GMII_CONF_SET_GMII_CONF_SET_ID,1)) 
  { 
    ret = ui_pb_api_nif_gmii_conf_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_GMII_CONF_GET_GMII_CONF_GET_ID,1)) 
  { 
    ret = ui_pb_api_nif_gmii_conf_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_GMII_STATUS_GET_GMII_STATUS_GET_ID,1)) 
  { 
    ret = ui_pb_api_nif_gmii_status_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_GMII_RATE_SET_GMII_RATE_SET_ID,1)) 
  { 
    ret = ui_pb_api_nif_gmii_rate_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_GMII_RATE_GET_GMII_RATE_GET_ID,1)) 
  { 
    ret = ui_pb_api_nif_gmii_rate_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_ILKN_SET_ILKN_SET_ID,1)) 
  { 
    ret = ui_pb_api_nif_ilkn_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_ILKN_GET_ILKN_GET_ID,1)) 
  { 
    ret = ui_pb_api_nif_ilkn_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_ILKN_DIAG_ILKN_DIAG_ID,1)) 
  { 
    ret = ui_pb_api_nif_ilkn_diag(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_STAT_ACTIVITY_GET_STAT_ACTIVITY_GET_ID,1)) 
  { 
    ret = ui_pb_api_nif_stat_activity_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_STAT_ACTIVITY_ALL_GET_STAT_ACTIVITY_ALL_GET_ID,1)) 
  { 
    ret = ui_pb_api_nif_stat_activity_all_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_LINK_STATUS_GET_LINK_STATUS_GET_ID,1)) 
  { 
    ret = ui_pb_api_nif_link_status_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_SYNCE_CLK_SET_SYNCE_CLK_SET_ID,1)) 
  { 
    ret = ui_pb_api_nif_synce_clk_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_SYNCE_CLK_GET_SYNCE_CLK_GET_ID,1)) 
  { 
    ret = ui_pb_api_nif_synce_clk_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_SYNCE_MODE_SET_SYNCE_MODE_SET_ID,1)) 
  { 
    ret = ui_pb_api_nif_synce_mode_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_SYNCE_MODE_GET_SYNCE_MODE_GET_ID,1)) 
  { 
    ret = ui_pb_api_nif_synce_mode_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_PTP_CLK_RESET_VALUE_GET_PTP_CLK_RESET_VALUE_GET_ID,1)) 
  { 
    ret = ui_pb_api_nif_ptp_clk_reset_value_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_PTP_CLK_SET_PTP_CLK_SET_ID,1)) 
  { 
    ret = ui_pb_api_nif_ptp_clk_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_PTP_CLK_GET_PTP_CLK_GET_ID,1)) 
  { 
    ret = ui_pb_api_nif_ptp_clk_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_PTP_CONF_SET_PTP_CONF_SET_ID,1)) 
  { 
    ret = ui_pb_api_nif_ptp_conf_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_PTP_CONF_GET_PTP_CONF_GET_ID,1)) 
  { 
    ret = ui_pb_api_nif_ptp_conf_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_ELK_SET_ELK_SET_ID,1)) 
  { 
    ret = ui_pb_api_nif_elk_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_ELK_GET_ELK_GET_ID,1)) 
  { 
    ret = ui_pb_api_nif_elk_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_FATP_MODE_SET_FATP_MODE_SET_ID,1)) 
  { 
    ret = ui_pb_api_nif_fatp_mode_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_FATP_MODE_GET_FATP_MODE_GET_ID,1)) 
  { 
    ret = ui_pb_api_nif_fatp_mode_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_FATP_SET_FATP_SET_ID,1)) 
  { 
    ret = ui_pb_api_nif_fatp_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_FATP_GET_FATP_GET_ID,1)) 
  { 
    ret = ui_pb_api_nif_fatp_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_COUNTER_GET_COUNTER_GET_ID,1)) 
  { 
    ret = ui_pb_api_nif_counter_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_ALL_COUNTERS_GET_ALL_COUNTERS_GET_ID,1)) 
  { 
    ret = ui_pb_api_nif_all_counters_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PB_NIF_ALL_NIFS_ALL_COUNTERS_GET_ALL_NIFS_ALL_COUNTERS_GET_ID,1)) 
  { 
    ret = ui_pb_api_nif_all_nifs_all_counters_get(current_line); 
  } 
  else 
  {    
    send_string_to_screen(" *** SW error - expecting function name after nif***", TRUE); 
  }
  
  ui_ret = ret; 
    
  goto exit; 
exit:        
  return ui_ret; 
}                
          
#endif /* nif */ 


#endif /* LINK_PB_LIBRARIES */ 

