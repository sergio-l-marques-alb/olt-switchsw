/* $Id: ui_pcp_api_mgmt.c,v 1.5 Broadcom SDK $
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
 
#include <soc/dpp/SAND/Utils/sand_os_interface.h> 
#include <appl/diag/dpp/utils_defi.h> 
#include <appl/dpp/UserInterface/ui_defi.h> 
 
#include <appl/dpp/UserInterface/ui_pure_defi_pcp_api.h> 
 
  
#include <soc/dpp/PCP/pcp_api_mgmt.h>
#include <appl/dpp/UserInterface/pcp/ui_pcp_api_mgmt.h>

#if LINK_PCP_LIBRARIES

#ifdef UI_MGMT
/******************************************************************** 
 *  Function handler: register_device (section mgmt)
 ********************************************************************/
int 
  ui_pcp_api_mgmt_register_device(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32   
    prm_base_address;
  SOC_SAND_RESET_DEVICE_FUNC_PTR   
    prm_reset_device_ptr=0;
  int prm_unit_ptr;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_api_mgmt"); 
  soc_sand_proc_name = "pcp_register_device"; 
 
  unit = pcp_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_REGISTER_DEVICE_REGISTER_DEVICE_BASE_ADDRESS_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_REGISTER_DEVICE_REGISTER_DEVICE_BASE_ADDRESS_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_base_address = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_REGISTER_DEVICE_REGISTER_DEVICE_RESET_DEVICE_PTR_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_REGISTER_DEVICE_REGISTER_DEVICE_RESET_DEVICE_PTR_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_reset_device_ptr = (SOC_SAND_RESET_DEVICE_FUNC_PTR)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_REGISTER_DEVICE_REGISTER_DEVICE_DEVICE_ID_PTR_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_REGISTER_DEVICE_REGISTER_DEVICE_DEVICE_ID_PTR_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_unit_ptr = (int)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = pcp_register_device(
          &prm_base_address,
          prm_reset_device_ptr,
          &prm_unit_ptr
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_register_device - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_register_device");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: unregister_device (section mgmt)
 ********************************************************************/
int 
  ui_pcp_api_mgmt_unregister_device(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_api_mgmt"); 
  soc_sand_proc_name = "pcp_unregister_device"; 
 
  unit = pcp_get_default_unit(); 
 
  /* Get parameters */ 

  /* Call function */
  ret = pcp_unregister_device(
          unit
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_unregister_device - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_unregister_device");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: op_mode_set (section mgmt)
 ********************************************************************/
int 
  ui_pcp_api_mgmt_op_mode_set(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint32 
    prm_head_ptr_index = 0xFFFFFFFF;  
  PCP_MGMT_OP_MODE   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_api_mgmt"); 
  soc_sand_proc_name = "pcp_mgmt_op_mode_set"; 
 
  unit = pcp_get_default_unit(); 
  PCP_MGMT_OP_MODE_clear(&prm_info);
 
  /* Get parameters */ 

  /* This is a set function, so call GET function first */                                                                                
  ret = pcp_mgmt_op_mode_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_mgmt_op_mode_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_mgmt_op_mode_get");   
    goto exit; 
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_STS_CONF_WORD_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_STS_CONF_WORD_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.sts.conf_word = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_STS_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_STS_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.sts.enable = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_OAM_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_OAM_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.oam.enable = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_DMA_RX_MODE_CAL_M_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_DMA_RX_MODE_CAL_M_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.dma.rx_mode.cal_m = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_DMA_RX_MODE_CAL_LEN_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_DMA_RX_MODE_CAL_LEN_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.dma.rx_mode.cal_len = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_DMA_RX_MODE_THRESHOLDS_DROP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_DMA_RX_MODE_THRESHOLDS_DROP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.dma.rx_mode.thresholds.drop = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_DMA_RX_MODE_THRESHOLDS_FC_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_DMA_RX_MODE_THRESHOLDS_FC_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.dma.rx_mode.thresholds.fc = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_DMA_RX_MODE_BUFF_SIZE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_DMA_RX_MODE_BUFF_SIZE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.dma.rx_mode.buff_size = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_DMA_RX_MODE_HEAD_PTR_INDEX_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_DMA_RX_MODE_HEAD_PTR_INDEX_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_head_ptr_index = (uint32)param_val->value.ulong_value;
  } 

  if(prm_head_ptr_index != 0xFFFFFFFF)          
  { 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_DMA_RX_MODE_HEAD_PTR_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_DMA_RX_MODE_HEAD_PTR_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.dma.rx_mode.head_ptr[ prm_head_ptr_index] = (uint32)param_val->value.ulong_value;
  } 

  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_DMA_RX_MODE_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_DMA_RX_MODE_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.dma.rx_mode.enable = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_DMA_TX_THRESHOLDS_DROP_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_DMA_TX_THRESHOLDS_DROP_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.dma.tx.thresholds.drop = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_DMA_TX_THRESHOLDS_FC_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_DMA_TX_THRESHOLDS_FC_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.dma.tx.thresholds.fc = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_DMA_TX_HEAD_PTR_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_DMA_TX_HEAD_PTR_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.dma.tx.head_ptr = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_DMA_TX_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_DMA_TX_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.dma.tx.enable = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_DMA_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_DMA_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.dma.enable = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_ELK_MODE_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_ELK_MODE_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.elk.mode = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_ELK_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_INFO_ELK_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_info.elk.enable = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = pcp_mgmt_op_mode_set(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_mgmt_op_mode_set - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_mgmt_op_mode_set");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: op_mode_get (section mgmt)
 ********************************************************************/
int 
  ui_pcp_api_mgmt_op_mode_get(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  PCP_MGMT_OP_MODE   
    prm_info;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_api_mgmt"); 
  soc_sand_proc_name = "pcp_mgmt_op_mode_get"; 
 
  unit = pcp_get_default_unit(); 
  PCP_MGMT_OP_MODE_clear(&prm_info);
 
  /* Get parameters */ 

  /* Call function */
  ret = pcp_mgmt_op_mode_get(
          unit,
          &prm_info
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_mgmt_op_mode_get - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_mgmt_op_mode_get");   
    goto exit; 
  } 

  PCP_MGMT_OP_MODE_print(&prm_info);

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: init_sequence_phase1 (section mgmt)
 ********************************************************************/
int 
  ui_pcp_api_mgmt_init_sequence_phase1(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  PCP_HW_ADJUSTMENTS   
    prm_hw_adjust;
  uint8   
    prm_silent=0;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_api_mgmt"); 
  soc_sand_proc_name = "pcp_mgmt_init_sequence_phase1"; 
 
  unit = pcp_get_default_unit(); 
  PCP_HW_ADJUSTMENTS_clear(&prm_hw_adjust);
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_ADJUST_ELK_IF_SERDES_INFO_TX_VODCTRL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_ADJUST_ELK_IF_SERDES_INFO_TX_VODCTRL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_hw_adjust.elk_if.serdes_info.tx_vodctrl = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_ADJUST_ELK_IF_SERDES_INFO_TX_PREEMP_2T_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_ADJUST_ELK_IF_SERDES_INFO_TX_PREEMP_2T_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_hw_adjust.elk_if.serdes_info.tx_preemp_2t = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_ADJUST_ELK_IF_SERDES_INFO_TX_PREEMP_1T_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_ADJUST_ELK_IF_SERDES_INFO_TX_PREEMP_1T_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_hw_adjust.elk_if.serdes_info.tx_preemp_1t = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_ADJUST_ELK_IF_SERDES_INFO_TX_PREEMP_0T_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_ADJUST_ELK_IF_SERDES_INFO_TX_PREEMP_0T_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_hw_adjust.elk_if.serdes_info.tx_preemp_0t = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_ADJUST_ELK_IF_SERDES_INFO_RX_EQ_DC_GAIN_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_ADJUST_ELK_IF_SERDES_INFO_RX_EQ_DC_GAIN_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_hw_adjust.elk_if.serdes_info.rx_eq_dc_gain = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_ADJUST_ELK_IF_SERDES_INFO_RX_EQ_CTRL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_ADJUST_ELK_IF_SERDES_INFO_RX_EQ_CTRL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_hw_adjust.elk_if.serdes_info.rx_eq_ctrl = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_ADJUST_ELK_IF_ENABLE_FLOW_CTRL_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_ADJUST_ELK_IF_ENABLE_FLOW_CTRL_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_hw_adjust.elk_if.enable_flow_ctrl = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_ADJUST_ELK_IF_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_ADJUST_ELK_IF_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_hw_adjust.elk_if.enable = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_ADJUST_QDR_QDR_SIZE_MBIT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_ADJUST_QDR_QDR_SIZE_MBIT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_hw_adjust.qdr.qdr_size_mbit = (PCP_MGMT_HW_IF_QDR_SIZE_MBIT)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_ADJUST_DRAM_SIZE_MBIT_ID,1)) 
  { 
    UI_MACROS_GET_SYMBOL_VAL(PARAM_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_ADJUST_DRAM_SIZE_MBIT_ID); 
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_hw_adjust.dram.size_mbit = param_val->numeric_equivalent;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_ADJUST_PCI_BURST_SIZE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_ADJUST_PCI_BURST_SIZE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_hw_adjust.pci.burst_size = (uint32)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_ADJUST_PCI_ENABLE_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_HW_ADJUST_PCI_ENABLE_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_hw_adjust.pci.enable = (uint8)param_val->value.ulong_value;
  } 

  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_SILENT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_SILENT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_silent = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = pcp_mgmt_init_sequence_phase1(
          unit,
          &prm_hw_adjust,
          prm_silent
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_mgmt_init_sequence_phase1 - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_mgmt_init_sequence_phase1");   
    goto exit; 
  } 

  
  goto exit; 
exit: 
  return ui_ret; 
} 
  
/******************************************************************** 
 *  Function handler: init_sequence_phase2 (section mgmt)
 ********************************************************************/
int 
  ui_pcp_api_mgmt_init_sequence_phase2(
    CURRENT_LINE *current_line 
  ) 
{   
  uint32 
    ret;   
  uint8   
    prm_silent=0;
   
  UI_MACROS_INIT_FUNCTION("ui_pcp_api_mgmt"); 
  soc_sand_proc_name = "pcp_mgmt_init_sequence_phase2"; 
 
  unit = pcp_get_default_unit(); 
 
  /* Get parameters */ 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_MGMT_INIT_SEQUENCE_PHASE2_INIT_SEQUENCE_PHASE2_SILENT_ID,1)) 
  { 
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_PCP_MGMT_INIT_SEQUENCE_PHASE2_INIT_SEQUENCE_PHASE2_SILENT_ID);  
    UI_MACROS_CHECK_GET_VAL_OF_ERROR; 
    prm_silent = (uint8)param_val->value.ulong_value;
  } 


  /* Call function */
  ret = pcp_mgmt_init_sequence_phase2(
          unit,
          prm_silent
        );
  if (soc_sand_get_error_code_from_error_word(ret) != SOC_SAND_OK) 
  { 
    send_string_to_screen(" *** pcp_mgmt_init_sequence_phase2 - FAIL", TRUE); 
    soc_sand_disp_result_proc(ret, "pcp_mgmt_init_sequence_phase2");   
    goto exit; 
  } 

  
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
  ui_pcp_api_mgmt( 
    CURRENT_LINE *current_line 
  ) 
{   
  int
    ret = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_pcp_api_mgmt"); 
 
  if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_REGISTER_DEVICE_REGISTER_DEVICE_ID,1)) 
  { 
    ret = ui_pcp_api_mgmt_register_device(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_UNREGISTER_DEVICE_UNREGISTER_DEVICE_ID,1)) 
  { 
    ret = ui_pcp_api_mgmt_unregister_device(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_MGMT_OP_MODE_SET_OP_MODE_SET_ID,1)) 
  { 
    ret = ui_pcp_api_mgmt_op_mode_set(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_MGMT_OP_MODE_GET_OP_MODE_GET_ID,1)) 
  { 
    ret = ui_pcp_api_mgmt_op_mode_get(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_MGMT_INIT_SEQUENCE_PHASE1_INIT_SEQUENCE_PHASE1_ID,1)) 
  { 
    ret = ui_pcp_api_mgmt_init_sequence_phase1(current_line); 
  } 
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_PCP_MGMT_INIT_SEQUENCE_PHASE2_INIT_SEQUENCE_PHASE2_ID,1)) 
  { 
    ret = ui_pcp_api_mgmt_init_sequence_phase2(current_line); 
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


#endif /* LINK_PCP_LIBRARIES */ 

