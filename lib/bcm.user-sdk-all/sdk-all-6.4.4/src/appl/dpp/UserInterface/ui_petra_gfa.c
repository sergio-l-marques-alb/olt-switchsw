/* $Id: ui_petra_gfa.c,v 1.5 Broadcom SDK $
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
 * General include file for reference design.
 */
#include <appl/diag/dpp/ref_sys.h>

#if !DUNE_BCM
#include <bcm_app/dpp/../H/usrApp.h>

#ifdef __VXWORKS__
  #include <CSR/csr_server.h>
  #include <appl/diag/dpp/tasks_info.h>
#endif
#endif
/*
 * Utilities include file.
 */

#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <appl/diag/dpp/utils_defi.h>
#include <appl/dpp/UserInterface/ui_defi.h>

#include <appl/dpp/UserInterface/ui_pure_defi_petra_gfa.h>

#include <appl/diag/dpp/utils_line_gfa_petra.h>
#include <appl/diag/dpp/utils_line_gfa_bi.h>

extern uint32
  soc_petra_get_default_unit();

int
  ui_petra_synt_assert(
  )
{
  SOC_SAND_RET
    ret = SOC_SAND_OK;

  ret = gfa_petra_synt_assert(
        );
  if (ret)
  {
    send_string_to_screen(" *** ui_petra_synt_assert - FAIL", TRUE);
  }
  return ret;
}

int
  ui_petra_start_csr_set(
    CURRENT_LINE *current_line
  )
{
  UI_MACROS_INIT_FUNCTION("ui_petra_synt_set");
  soc_sand_proc_name = "soc_petra_start_csr_set";

#if !DUNE_BCM
#ifdef __VXWORKS__
  taskSpawn(
    "csrServer", 35, 0, 200000,(FUNCPTR)csr_server_init,
    4001,0,0,0,0,0,0,0,0,0
  );
#endif
#endif

  return ui_ret;
}

int
  ui_petra_synt_set(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  GFA_TG_SYNT_TYPE
    syntsizer;
  uint32
    targetfreq;

  UI_MACROS_INIT_FUNCTION("ui_petra_synt_set");
  soc_sand_proc_name = "soc_petra_synt_set";

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GFA_PETRA_SYNT_SET_SYN_TYPE_ID,1))
  {
    UI_MACROS_GET_SYMBOL_VAL(PARAM_GFA_PETRA_SYNT_SET_SYN_TYPE_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    syntsizer = param_val->numeric_equivalent;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter freq_val after synt_type***", TRUE);
    goto exit;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GFA_PETRA_SYNT_SET_FREQ_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GFA_PETRA_SYNT_SET_FREQ_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    targetfreq = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter freq_val after synt_type***", TRUE);
    goto exit;
  }

  ret = gfa_petra_board_synt_set(
          syntsizer,
          targetfreq,
          FALSE
        );
  if (ret)
  {
    send_string_to_screen(" *** ui_petra_synt_assert - FAIL", TRUE);
  }
exit:
  return ui_ret;
}

int
ui_petra_bsp_i2c_read(
                      CURRENT_LINE *current_line
                      )
{
  uint32
    ret;
  uint8
    dev = 0, reg = 0, val = 0;
  uint8
    reg_valid = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_petra_bsp_i2c_read");
  soc_sand_proc_name = "lb_bsp_i2c_read";

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GFA_PETRA_I2C_READ_DEV_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GFA_PETRA_I2C_READ_DEV_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    dev = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GFA_PETRA_I2C_READ_REG_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GFA_PETRA_I2C_READ_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    reg = (uint8)param_val->value.ulong_value;
    reg_valid = TRUE;
  }

  ret = lb_petra_i2c_read(dev, reg, 1, &val);
  if (ret)
  {
    send_string_to_screen(" *** lb_bsp_i2c_read - FAIL", TRUE);
  }
  else
  {
    soc_sand_os_printf( "val: 0x%x\n\r", (int)val);
  }

exit:
  return ui_ret;
}

int
ui_petra_bsp_i2c_write(
                       CURRENT_LINE *current_line
                       )
{
  uint32
    ret;
  uint8
    dev = 0, reg = 0, val = 0;
  uint8
    reg_valid = FALSE;

  UI_MACROS_INIT_FUNCTION("ui_petra_bsp_i2c_write");
  soc_sand_proc_name = "lb_bsp_i2c_write";

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GFA_PETRA_I2C_WRITE_DEV_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GFA_PETRA_I2C_WRITE_DEV_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    dev = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GFA_PETRA_I2C_WRITE_REG_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GFA_PETRA_I2C_WRITE_REG_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    reg = (uint8)param_val->value.ulong_value;
    reg_valid = TRUE;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GFA_PETRA_I2C_WRITE_VAL_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GFA_PETRA_I2C_WRITE_VAL_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    val = (uint8)param_val->value.ulong_value;
  }

  ret = lb_bsp_i2c_write_gen_inner(dev, &val, 1, reg, reg_valid);
  if (ret)
  {
    send_string_to_screen(" *** lb_bsp_i2c_write - FAIL", TRUE);
  }

exit:
  return ui_ret;
}

int
  ui_petra_gfa_bi_hot_swap(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    enable;

  UI_MACROS_INIT_FUNCTION("ui_petra_gfa_bi_hot_swap");
  soc_sand_proc_name = "soc_petra_gfa_bi_hot_swap";

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GFA_PETRA_BI_HOT_SWAP_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GFA_PETRA_BI_HOT_SWAP_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    enable = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting parameter after bi_hot_swap ***", TRUE);
    goto exit;
  }

  ret = gfa_bi_utils_hot_swap_enable(
          FALSE,
          enable
        );
  if (ret)
  {
    send_string_to_screen(" *** gfa_bi_utils_hot_swap_enable - FAIL", TRUE);
  }
exit:
  return ui_ret;
}

int
  ui_petra_gfa_bi_ps_ad_read_all(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint32
    channel,
    measured_voltage;
  uint8
    buffer[2];

  UI_MACROS_INIT_FUNCTION("ui_petra_gfa_bi_ps_ad_read_all");
  soc_sand_proc_name = "soc_petra_gfa_bi_ps_ad_read_all";
  
  channel = 1;
  ret = gfa_bi_ps_ad_read_single(
          channel,
          buffer
        );
  if (ret)
  {
    send_string_to_screen(" *** gfa_bi_ps_ad_read_single - FAIL", TRUE);
    goto exit;
  }

  soc_sand_os_printf( "ui_petra_gfa_bi_ps_ad_read_all. buffer[0]=0x%x, buffer[1]=0x%x\n", buffer[0], buffer[1]);
  
  measured_voltage = ((buffer[0] * 256 + buffer[1]) * 600) / 1000000;
  soc_sand_os_printf( "ui_petra_gfa_bi_ps_ad_read_all. measured_voltage=%d\n",measured_voltage);

exit:
  return ui_ret;
}

int
  ui_petra_gfa_bi_bsp_i2c_write_gen(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret,
    internal_address,
    write_data;
  uint8 
    device_address = 0,
    internal_address_len = 1, 
    write_data_len = 1,
    inner_internal_address[GFA_BI_BSP_I2C_INT_ADDR_LEN],
    inner_write_data[GFA_BI_BSP_I2C_BUFFER_SIZE];

  UI_MACROS_INIT_FUNCTION("ui_petra_gfa_bi_bsp_i2c_write_gen");
  soc_sand_proc_name = "bi_bsp_i2c_write_gen";

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GFA_PETRA_BI_BSP_I2C_WRITE_GEN_DEV_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GFA_PETRA_BI_BSP_I2C_WRITE_GEN_DEV_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    device_address = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GFA_PETRA_BI_BSP_I2C_WRITE_GEN_INT_ADDR_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GFA_PETRA_BI_BSP_I2C_WRITE_GEN_INT_ADDR_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    internal_address = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GFA_PETRA_BI_BSP_I2C_WRITE_GEN_INT_ADDR_LEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GFA_PETRA_BI_BSP_I2C_WRITE_GEN_INT_ADDR_LEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    internal_address_len = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GFA_PETRA_BI_BSP_I2C_WRITE_GEN_WRITE_DATA_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GFA_PETRA_BI_BSP_I2C_WRITE_GEN_WRITE_DATA_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    write_data = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GFA_PETRA_BI_BSP_I2C_WRITE_GEN_WRITE_DATA_LEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GFA_PETRA_BI_BSP_I2C_WRITE_GEN_WRITE_DATA_LEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    write_data_len = (uint8)param_val->value.ulong_value;
  }

  if (internal_address_len == 1)
  {
    inner_internal_address[0] = (uint8)(internal_address);
  }
  else
  {
    memcpy(inner_internal_address, &internal_address, GFA_BI_BSP_I2C_INT_ADDR_LEN * sizeof(uint8));
  }
  
  if (write_data_len == 1)
  {
    inner_write_data[0] = (uint8)(write_data);
  }
  else
  {
    memcpy(inner_write_data, &write_data, sizeof(uint32));
  }  

  ret = gfa_bi_bsp_i2c_write_gen(
          device_address,
          inner_internal_address,
          internal_address_len,
          inner_write_data,
          write_data_len
        );
  if (ret)
  {
    send_string_to_screen(" *** gfa_bi_bsp_i2c_write_gen - FAIL", TRUE);
  }

exit:
  return ui_ret;
}

int
  ui_petra_gfa_bi_bsp_i2c_read_gen(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret;
  uint8
    device_address = 0, 
    internal_address_len = 0,
	  inner_internal_address[GFA_BI_BSP_I2C_INT_ADDR_LEN],
    exp_read_len = 1;
  uint32 
    read_data,
    internal_address;
  uint16 
    read_data_len;
  

  UI_MACROS_INIT_FUNCTION("ui_petra_gfa_bi_bsp_i2c_read_gen");
  soc_sand_proc_name = "bi_bsp_i2c_read_gen";

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GFA_PETRA_BI_BSP_I2C_READ_GEN_DEV_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GFA_PETRA_BI_BSP_I2C_READ_GEN_DEV_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    device_address = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GFA_PETRA_BI_BSP_I2C_READ_GEN_INT_ADDR_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GFA_PETRA_BI_BSP_I2C_READ_GEN_INT_ADDR_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    internal_address = (uint32)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GFA_PETRA_BI_BSP_I2C_READ_GEN_INT_ADDR_LEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GFA_PETRA_BI_BSP_I2C_READ_GEN_INT_ADDR_LEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    internal_address_len = (uint8)param_val->value.ulong_value;
  }

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GFA_PETRA_BI_BSP_I2C_READ_GEN_EXP_READ_LEN_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GFA_PETRA_BI_BSP_I2C_READ_GEN_EXP_READ_LEN_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    exp_read_len = (uint8)param_val->value.ulong_value;
  }
  
  if (internal_address_len == 1)
  {
    inner_internal_address[0] = (uint8)(internal_address);
  }
  else
  {
    memcpy(inner_internal_address, &internal_address, GFA_BI_BSP_I2C_INT_ADDR_LEN * sizeof(uint8));
  }

  ret = gfa_bi_bsp_i2c_read_gen( 
          device_address,
          inner_internal_address,
          internal_address_len,
          exp_read_len,
          &read_data,
          &read_data_len
        );
  if (ret)
  {
    send_string_to_screen(" *** gfa_bi_bsp_i2c_write_gen - FAIL", TRUE);
  }
  else
  {
    soc_sand_os_printf( "ui_petra_gfa_bi_bsp_i2c_read_gen: read_data 0x%x. read_data_len=%d\n", read_data,read_data_len);
  }

exit:
  return ui_ret;
}

int
  ui_petra_gfa_bi_bsp_nlp1024_phy_init_gen(
    CURRENT_LINE *current_line
  )
{
  uint32
    ret,
    port;

  UI_MACROS_INIT_FUNCTION("ui_petra_gfa_bi_bsp_nlp1024_phy_init_gen");
  soc_sand_proc_name = "ui_petra_gfa_bi_bsp_nlp1024_phy_init_gen";

  unit = soc_petra_get_default_unit();

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GFA_PETRA_BI_BSP_NLP1024_PHY_INIT_ID,1))
  {
    UI_MACROS_GET_NUMMERIC_VAL(PARAM_GFA_PETRA_BI_BSP_NLP1024_PHY_INIT_ID);
    UI_MACROS_CHECK_GET_VAL_OF_ERROR;
    port = (uint32)param_val->value.ulong_value;
  }
  else
  {
    send_string_to_screen(" *** SW error - expecting port number after nlp1024_phy_init ***", TRUE);
    goto exit;
  }

  ret = gfa_bi_phy_single_nlp1042_init(
          unit,
          port
        );
  if (ret)
  {
    send_string_to_screen(" *** gfa_bi_phy_single_nlp1042_init - FAIL", TRUE);
  }
exit:
  return ui_ret;
}

int
  subject_petra_gfa(
    CURRENT_LINE *current_line,
    CURRENT_LINE **current_line_ptr
  )
{
  unsigned int
    match_index;
  int
    ui_ret;
  unsigned int
    ;
  char
    *proc_name ;


  proc_name = "subject_petra_gfa" ;
  ui_ret = FALSE ;
  unit = 0;


  /*
   * the rest of the line is empty
   */
  if (current_line->num_param_names == 0)
  {
    send_string_to_screen("\r\n",FALSE) ;
    send_string_to_screen("'subject_petra_gfa()' function was called with no parameters.\r\n",FALSE) ;
    goto exit ;
  }
  /*
   * Else, there are parameters on the line (not just 'subject_petra_gfa').
   */

  send_array_to_screen("\r\n",2) ;

  if(!search_param_val_pairs(current_line,&match_index,PARAM_GFA_PETRA_SYNT_ASSERT_ID,1))
  {
    ui_ret = ui_petra_synt_assert(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GFA_PETRA_SYNT_SET_ID,1))
  {
    ui_ret = ui_petra_synt_set(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GFA_PETRA_START_CSR_ID,1))
  {
    ui_ret = ui_petra_start_csr_set(current_line);
  }
   else if(!search_param_val_pairs(current_line,&match_index,PARAM_GFA_PETRA_I2C_WRITE_ID,1))
  {
    ui_ret = ui_petra_bsp_i2c_write(current_line);
  }  
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GFA_PETRA_I2C_READ_ID,1))
  {
    ui_ret = ui_petra_bsp_i2c_read(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GFA_PETRA_BI_HOT_SWAP_ID,1))
  {
    ui_ret = ui_petra_gfa_bi_hot_swap(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GFA_PETRA_BI_PS_AD_READ_ALL_ID,1))
  {
    ui_ret = ui_petra_gfa_bi_ps_ad_read_all(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GFA_PETRA_BI_BSP_I2C_WRITE_GEN_ID,1))
  {
    ui_ret = ui_petra_gfa_bi_bsp_i2c_write_gen(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GFA_PETRA_BI_BSP_I2C_READ_GEN_ID,1))
  {
    ui_ret = ui_petra_gfa_bi_bsp_i2c_read_gen(current_line);
  }
  else if(!search_param_val_pairs(current_line,&match_index,PARAM_GFA_PETRA_BI_BSP_NLP1024_PHY_INIT_ID,1))
  {
    ui_ret = ui_petra_gfa_bi_bsp_nlp1024_phy_init_gen(current_line);
  }
  else
  {
    /*
     * Enter if an unknown request.
     */
    send_string_to_screen(
      "\r\n"
      "*** soc_petra_gfa command with unknown parameters'.\r\n"
      "    Syntax error/sw error...\r\n",
      TRUE)  ;
    ui_ret = TRUE  ;
    goto exit  ;
  }

exit:
  return (ui_ret);
}

