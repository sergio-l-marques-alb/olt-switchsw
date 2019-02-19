/* $Id: swp_pcp_app.c,v 1.15 Broadcom SDK $
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

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Management/sand_module_management.h>
#include <soc/dpp/SAND/Management/sand_device_management.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>
#include <soc/dpp/SAND/Management/sand_general_params.h>
#include <soc/dpp/SAND/SAND_FM/sand_mem_access.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_u64.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>

#include <appl/dpp/sweep/PCP/sweep_pcp_app.h>
#include <appl/dpp/sweep/PCP/sweep_pcp_bsp_interface.h>
#include <appl/dpp/sweep/PCP/sweep_pcp_file.h>

#include <soc/dpp/PCP/pcp_api_mgmt.h>
#include <soc/dpp/PCP/pcp_tbl_access.h>

#include <soc/dcmn/dcmn_utils_eeprom.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */


/* } */

/*************
 *  MACROS   *
 *************/
/* { */


/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */


/* } */

/*************
 * GLOBALS   *
 *************/
/* { */

static char
  Pcp_agent_error_buffer[SOC_SAND_CALLBACK_BUF_SIZE];
static uint32
  Device_id;
static uint32
  Init_finished = 0;


/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

/*
 * General functions
 */
uint32
  sweep_pcp_app_unit_get(
    SOC_SAND_OUT int *unit
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(unit);

  if (Init_finished == 0)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 10, exit);
  }
    
  *unit = Device_id;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in sweep_pcp_app_unit_get()", 0, 0);
}

/*
 * Device Init Params functions
 */
void
  sweep_SWEEP_PCP_FEATURES_LIST_clear(
    SOC_SAND_OUT SWEEP_PCP_FEATURES_LIST *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(SWEEP_PCP_FEATURES_LIST));
  info->link_level_fc_en = 0;
  info->fec_en = 0;

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  sweep_SWEEP_PCP_DEVICE_INIT_PARAMS_clear(
    SOC_SAND_OUT SWEEP_PCP_DEVICE_INIT_PARAMS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_memset(info, 0x0, sizeof(SWEEP_PCP_DEVICE_INIT_PARAMS));
  info->system_pcp_id = 0;
  PCP_MGMT_OP_MODE_clear(&(info->operation_mode));
  PCP_HW_ADJUSTMENTS_clear(&(info->hw_adjustment));
  info->base_addr = 0;
  sweep_SWEEP_PCP_FEATURES_LIST_clear(&(info->features_list));
  info->srd_phys_param_preset_id = 0;
  info->do_memory_test = 0;

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  sweep_SWEEP_PCP_DEVICE_INIT_PARAMS_defaults(
    SOC_SAND_OUT SWEEP_PCP_DEVICE_INIT_PARAMS *info
  )
{
  uint32
    ind;
  uint8
    is_gfa_bi = 1;
#if defined(__DUNE_GTO_BCM_CPU__)
  int32
    int_value;
#endif

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  /* If failed to read, or card is gfa_bi - tread card as gfa_bi */
#if defined(__DUNE_GTO_BCM_CPU__)
  is_gfa_bi =
      (eeprom_read(LINE_CARD_GFA_PETRA_B_INTERLAKEN, NEGEV_CHASSIS_EEPROM_ADRESS_BOARD_TYPE, 4, &int_value) ||
      (int_value == LINE_CARD_GFA_PETRA_B_INTERLAKEN));
#endif
  /* Operation Mode defaults */

  /* OAM + ELK shoudl be enabled for GFA_bi, disabled otherwise (gfa_bi_2) */

  /* Oam */
  info->operation_mode.oam.enable = is_gfa_bi;
  /* Elk */
  info->operation_mode.elk.enable = is_gfa_bi;

  SOC_SAND_SET_BIT(info->operation_mode.elk.ilm_key_mask_bitmap, 0x1, PCP_MGMT_ELK_OP_MOD_ILM_KEY_MASK_BITMAP_MASK_INRIF);
  SOC_SAND_SET_BIT(info->operation_mode.elk.ilm_key_mask_bitmap, 0x1, PCP_MGMT_ELK_OP_MOD_ILM_KEY_MASK_BITMAP_MASK_PORT);
  info->operation_mode.elk.size_mbit = PCP_MGMT_HW_IF_DRAM_SIZE_MBIT_288;
  info->hw_adjustment.dram.size_mbit = PCP_MGMT_HW_IF_DRAM_SIZE_MBIT_288;
  info->operation_mode.elk.mode = PCP_MGMT_ELK_LKP_MODE_BOTH;
  /* Statistics */
  info->operation_mode.sts.conf_word = 0xa0d9;

  /* Synt Defaults */
  info->bsp_internal.pcp_core_freq = 250000000;
  info->bsp_internal.pcp_elk_freq  = 187500000;

  /* ipv4 defaults */
  info->operation_mode.ipv4_info.nof_vrfs = 0;
  info->operation_mode.ipv4_info.flags = PCP_MGMT_IPV4_OP_MODE_SUPPORT_CACHE;/*|SOC_PB_PP_MGMT_IPV4_OP_MODE_SUPPORT_DEFRAG*/

  for (ind = 0; ind < PCP_NOF_VRFS; ++ind)
  {
    if (ind == 0 )
    {
      info->operation_mode.ipv4_info.max_routes_in_vrf[ind] = 10000;
    }
    else
    {
      info->operation_mode.ipv4_info.max_routes_in_vrf[ind] = 100;
    }
  }

  info->operation_mode.ipv4_info.write_cb = pcp_eci_lpm4_tbl_set_unsafe;

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

/* 
 *     Memory test functions
 */ 

uint32
  sweep_pcp_memory_test(
    SOC_SAND_OUT int                       unit,
    SOC_SAND_IN  uint32                      silent
  )
{
    uint32
        ret = SOC_SAND_OK;

    SWP_INIT_ERR_DEFS("sweep_pcp_memory_test");

    /* Test device Memory. */
    ret = pcp_mgmt_memory_test(
            unit,
            silent
          );
    SWP_EXIT_AND_PRINT_IF_ERR(ret, silent, 10, "Error in Pcp Memory test");


exit:
  SWP_EXIT_AND_PRINT_ERR;
}

/*
 * Initialize
 */
SOC_SAND_RET
  sweep_pcp_init_device_info_get(
    SOC_SAND_OUT SWEEP_PCP_DEVICE_INIT_PARAMS *init_info
  )
{
  sweep_pcp_bsp_base_address(FALSE,&(init_info->base_addr));
  sweep_pcp_bsp_system_pcp_id_get(&init_info->system_pcp_id);

  init_info->features_list.link_level_fc_en = TRUE;

  return SOC_SAND_OK;
}


uint32 sweep_pcp_app_module_open(
  uint32 max_nof_devices,
  uint32 silent
  )
{
  uint32
    is_already_opened = FALSE, /* is the module already open */
    ticks_in_ms = 0,
    soc_tcmenable_flag = 0;
  SOC_SAND_PHYSICAL_ACCESS
    physical_access_curr,
    soc_sand_physical_access ;
  uint32
    ret = SOC_SAND_OK;

  SWP_INIT_ERR_DEFS("sweep_pcp_app_module_open");

  ticks_in_ms = 1000/soc_sand_os_get_ticks_per_sec();

  ret = soc_sand_module_open(
          max_nof_devices,                 /* max devices                      */
          ticks_in_ms,                     /* system_tick_in_ms                */
          SOC_SAND_TCM_DEFAULT_TASK_PRIORITY,  /* tcm task priority                */
          1,                               /* min_time_between_tcm_activation  */
          FALSE,                           /* TCM mockup interrupts            */
          NULL,
          Pcp_agent_error_buffer,
          &is_already_opened
        );
  SWP_EXIT_AND_PRINT_IF_ERR(ret, silent, 4, "sweep_pcp_app_module_open(). soc_sand module open failed - could not initialize device");

  if (is_already_opened && !silent)
  {
    soc_sand_os_printf("sweep_pcp_app_module_open().Soc_sand was already opened.\n\r");
  }

  soc_tcmenable_flag = soc_sand_tcm_get_enable_flag() ;
  soc_sand_tcm_set_enable_flag(FALSE);

  ret = soc_sand_get_physical_access_hook(
          &physical_access_curr
        );
  SWP_EXIT_IF_ERR(ret, 10);

  if (
      (physical_access_curr.physical_write == NULL) ||
      (physical_access_curr.physical_write == soc_sand_eci_write)
     )
  {
    soc_sand_physical_access.physical_write = swp_pcp_bsp_write;
  }
  else
  {
    soc_sand_physical_access.physical_write = physical_access_curr.physical_write;
  }

  if (
    (physical_access_curr.physical_read == NULL) ||
    (physical_access_curr.physical_read == soc_sand_eci_read)
    )
  {
    soc_sand_physical_access.physical_read = swp_pcp_bsp_read;
  }
  else
  {
    soc_sand_physical_access.physical_read = physical_access_curr.physical_read;
  }

  ret =
    soc_sand_set_physical_access_hook(
      &soc_sand_physical_access
    );

  SWP_EXIT_IF_ERR(ret, 6);

  soc_sand_tcm_set_enable_flag(soc_tcmenable_flag); /* restore tcm enable state */

exit:
  SWP_EXIT_AND_PRINT_ERR;
}

/*****************************************************
*NAME
*
* sweep_pcp_init_device
*TYPE:
*  PROC
*DATE:
*  12/03/6006
*FUNCTION:
*  Initializes a single PCP device.
*   This includes phase 1 and phase 2 initializations, and initial device configuration
*INPUT:
*  SOC_SAND_DIRECT:
*    SWP_PCP_INIT_PARAMS* pcp_init_params - pointer to the initial parameters structure
*    SOC_SAND_OUT uint32* device_handle - pointer to the device handle
*    uint32 is_hot_start - true if only the software is reset
*    uint32 silent - if true, the prints are disabled,
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    uint32 - error indication
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
uint32
  sweep_pcp_init_device(
    SOC_SAND_IN  SWEEP_PCP_DEVICE_INIT_PARAMS   *pcp_init_params,
    SOC_SAND_OUT int                      *device_handle,
    SOC_SAND_IN  uint32                      is_hot_start,
    SOC_SAND_IN  uint32                      silent
  )
{
  uint32
    unit,
    *base_address;
  uint32
    res = SOC_SAND_OK;
  SOC_D_USR_APP_FLAVOR
    usr_app_flavor;

  SWP_INIT_ERR_DEFS("sweep_pcp_init_device");

  sweep_pcp_bsp_usr_app_flavor_get(&usr_app_flavor) ;

  if (!pcp_init_params)
  {
    SWP_SET_ERR_AND_EXIT(10);
  }

  if (device_handle == NULL)
  {
    SWP_SET_ERR_AND_EXIT(30);
  }

  unit = *device_handle;
  if (usr_app_flavor == SOC_D_USR_APP_FLAVOR_SERVER)
  {
    sweep_pcp_bsp_base_address(TRUE,&(base_address));
  }
  else
  {
    sweep_pcp_bsp_base_address(FALSE,&(base_address));
  }
/*
 * Device registration.
 * 'base_address' is the user handle, that the driver will call the
 *  read/write callbacks with.
 * 'device_handle' is the driver handle that the SOC_SAND driver allocated
 *  for the device
 */
  res =
    pcp_register_device(
      base_address,
      swp_pcp_bsp_reset,
      device_handle
    );
  SWP_EXIT_AND_PRINT_IF_ERR(res, silent, 40, "failed to register pcp device");

  if ((usr_app_flavor == SOC_D_USR_APP_FLAVOR_SERVER) ||
      (usr_app_flavor == SOC_D_USR_APP_FLAVOR_MINIMAL_NO_APP))
  {
    SWP_PRINT_MSG(silent, " The device is not written to, due to User Flavor");
    goto exit;
  }

  if(is_hot_start)
  {
     goto exit;
  }
  unit = *device_handle;
  Device_id = *device_handle;

  /*
   * Set the device basic characteristics.
   */
  res = pcp_mgmt_op_mode_set(
          unit,
          &(pcp_init_params->operation_mode)
        );
  SWP_EXIT_AND_PRINT_IF_ERR(res, silent, 45, "Operation mode set Failed");

  /*
   * Initialize the device.
   */
  res = pcp_mgmt_init_sequence_phase1(
          unit,
          &(pcp_init_params->hw_adjustment),
          silent
        );
  SWP_EXIT_AND_PRINT_IF_ERR(res, silent, 50, "Init phase 1 Failed");

   /*
   * Enable the device in the system
   */
  res = pcp_mgmt_init_sequence_phase2(
          unit,
          silent
        );
  SWP_EXIT_AND_PRINT_IF_ERR(res, silent, 100, "failure performing pcp phase2 initialization");

exit:
  SWP_EXIT_AND_PRINT_ERR;
}

/*****************************************************
*NAME
*
* sweep_pcp_app_init
*TYPE:
*  PROC
*DATE:
*  21/10/2010
*FUNCTION:
*  Initializes pcp sweep application - sets the application parameter's values
*
*INPUT:
*  SOC_SAND_DIRECT:
*    uint32 is_hot_start - true if only the software is reset
*    uint32 silent - if true, the prints are disabled,
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    uint32 - error indication
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
uint32
  sweep_pcp_app_init(
    uint32 is_hot_start,
    uint32 silent,
    int *unit
  )
{
  uint32
    res = SOC_SAND_OK;
  SWEEP_PCP_DEVICE_INIT_PARAMS
    device_init_params;

  SWP_INIT_ERR_DEFS("sweep_pcp_app_init");

  SWP_PRINT_MSG(silent, "sweep pcp application initialization started...");

  sweep_SWEEP_PCP_DEVICE_INIT_PARAMS_clear(&device_init_params);

  sweep_SWEEP_PCP_DEVICE_INIT_PARAMS_defaults(&device_init_params);

  if(is_hot_start)
  {
    if (!silent)
    {
        SWP_PRINT_MSG(silent, "Hot-initialization\n");
        sweep_pcp_bsp_base_address(FALSE,&(device_init_params.base_addr));
    }
  }
  else
  {
#ifndef __KERNEL__
    res = sweep_pcp_SWEEP_PCP_DEVICE_INIT_PARAMS_load(
      &device_init_params,
      silent
      );
    SWP_EXIT_AND_PRINT_IF_ERR(res, silent, 5, "sweep_pcp_SWEEP_PCP_DEVICE_INIT_PARAMS_load Failed");
#endif /* __KERNEL__ */
    res = sweep_pcp_init_device_info_get(&device_init_params);
    SWP_EXIT_AND_PRINT_IF_ERR(res, silent, 10, "sweep_pcp_init_device_info_get Failed");
  }
  res = sweep_pcp_app_module_open(
          SOC_SAND_MAX_DEVICE,
          silent
        );
  SWP_EXIT_AND_PRINT_IF_ERR(res, silent, 30, "sweep_pcp_app_module_open Failed");

  SWP_PRINT_MSG(silent, "Init Pcp device");

  res = sweep_pcp_init_device(
          &(device_init_params),
          unit,
          is_hot_start,
          silent
        );
  SWP_EXIT_AND_PRINT_IF_ERR(res, silent, 50, "sweep_pcp_init_device Failed");

  if (device_init_params.do_memory_test == TRUE)
  {
      SWP_PRINT_MSG(silent, "Run Pcp Meomory test");
      res = sweep_pcp_memory_test(
                  *unit,
                  silent
                );
      SWP_EXIT_AND_PRINT_IF_ERR(res, silent, 50, "sweep_pcp_memory_test Failed");
  }

  Init_finished = 1;

  SWP_PRINT_MSG(silent, "sweep pcp application initialization finished successfully");

exit:
  SWP_EXIT_AND_PRINT_ERR;
}

#include <soc/dpp/SAND/Utils/sand_footer.h>
/* } */

