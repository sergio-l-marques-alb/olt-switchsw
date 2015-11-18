/* $Id: pb_api_mgmt.c,v 1.10 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_TM/src/soc_pb_api_mgmt.c
*
* MODULE PREFIX:  pb
*
* FILE DESCRIPTION:
*
* REMARKS:
* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
******************************************************************/

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/Petra/PB_TM/pb_api_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_api_mgmt.h>
#include <soc/dpp/Petra/PB_TM/pb_mgmt.h>
#include <soc/dpp/Petra/PB_TM/pb_init.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/* } */

#define SOC_PB_NIF_MDIO_CLK_FREQ_KHZ_DEF                                     (500)

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

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

/*********************************************************************
*     This procedure registers a new device to be taken care
*     of by this device driver. Physical device must be
*     accessible by CPU when this call is made..
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_register_device(
             uint32                   *base_address,
    SOC_SAND_IN  SOC_SAND_RESET_DEVICE_FUNC_PTR reset_device_ptr,
    SOC_SAND_OUT int                 *unit_ptr
  )
{
  uint32
    res;
  int unit = *unit_ptr;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_REGISTER_DEVICE);
  SOC_SAND_CHECK_NULL_INPUT(unit_ptr);

  res = soc_pb_register_device_unsafe(
    base_address,
    reset_device_ptr,
    &unit
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *unit_ptr = unit;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_register_device()",0,0);
}

/*********************************************************************
*     Undo soc_pb_register_device()
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_unregister_device(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_UNREGISTER_DEVICE);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_pb_unregister_device_unsafe(
    unit
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_unregister_device()",0,0);
}

/*********************************************************************
*     Set soc_petra device operation mode. This defines
 *     configurations, such as support for certain header
 *     types, etc.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_mgmt_operation_mode_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_MGMT_OPERATION_MODE         *op_mode
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_OPERATION_MODE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(op_mode);

  res = soc_pb_mgmt_operation_mode_set_verify(
          unit,
          op_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_mgmt_operation_mode_set_unsafe(
          unit,
          op_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_mgmt_operation_mode_set()", 0, 0);
}

/*********************************************************************
*     Set soc_petra device operation mode. This defines
 *     configurations, such as support for certain header
 *     types, etc.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_mgmt_operation_mode_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT SOC_PB_MGMT_OPERATION_MODE         *op_mode
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_OPERATION_MODE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(op_mode);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_mgmt_operation_mode_get_unsafe(
          unit,
          op_mode
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_mgmt_operation_mode_get()", 0, 0);
}

/*********************************************************************
*     Initialize the device, including:1. Prevent all the
 *     control cells. 2. Initialize the device tables and
 *     registers to default values. 3. Initialize
 *     board-specific hardware interfaces according to
 *     configurable information, as passed in 'hw_adjust'. 4.
 *     Perform basic device initialization. The configuration
 *     can be enabled/disabled as passed in 'enable_info'.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_mgmt_init_sequence_phase1(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_INOUT  SOC_PB_HW_ADJUSTMENTS              *hw_adjust,
    SOC_SAND_IN  SOC_PB_INIT_BASIC_CONF             *basic_conf,
    SOC_SAND_IN  SOC_PB_INIT_PORTS                  *fap_ports,
    SOC_SAND_IN  uint8                      silent
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_INIT_SEQUENCE_PHASE1);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(hw_adjust);
  SOC_SAND_CHECK_NULL_INPUT(basic_conf);
  SOC_SAND_CHECK_NULL_INPUT(fap_ports);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_mgmt_init_sequence_phase1_verify(
          unit,
          hw_adjust,
          basic_conf,
          fap_ports
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_pb_mgmt_init_sequence_phase1_unsafe(
          unit,
          hw_adjust,
          basic_conf,
          fap_ports,
          silent
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_mgmt_init_sequence_phase1()", 0, 0);
}

/*********************************************************************
*     Out-of-reset sequence. Enable/Disable the device from
 *     receiving and transmitting control cells.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_mgmt_init_sequence_phase2(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_INIT_OOR                    *oor_info,
    SOC_SAND_IN  uint8                      silent
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_INIT_SEQUENCE_PHASE2);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(oor_info);

  res = soc_pb_mgmt_init_sequence_phase2_verify(
          unit,
          oor_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_mgmt_init_sequence_phase2_unsafe(
          unit,
          oor_info,
          silent
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_mgmt_init_sequence_phase2()", 0, 0);
}

/*********************************************************************
*     Initialize a subset of the HW interfaces of the device.
 *     The function might be called more than once, each time
 *     with different fields, indicated to be written to the
 *     device
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_mgmt_hw_interfaces_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PB_HW_ADJUSTMENTS              *hw_adjust,
    SOC_SAND_IN  uint8                      silent
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_HW_INTERFACES_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(hw_adjust);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_mgmt_hw_interfaces_verify(
          unit,
          hw_adjust
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_pb_mgmt_hw_interfaces_set_unsafe(
          unit,
          hw_adjust,
          silent
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_mgmt_hw_interfaces_set()", 0, 0);
}

/*********************************************************************
*     Set the maximal allowed packet size. The limitation can
 *     be performed based on the packet size before or after
 *     the ingress editing (external and internal configuration
 *     mode, accordingly). Packets above the specified value
 *     are dropped.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_mgmt_max_pckt_size_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      port_ndx,
    SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx,
    SOC_SAND_IN  uint32                       max_size
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_MAX_PCKT_SIZE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_pb_mgmt_max_pckt_size_set_verify(
          unit,
          port_ndx,
          conf_mode_ndx,
          max_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_mgmt_max_pckt_size_set_unsafe(
          unit,
          port_ndx,
          conf_mode_ndx,
          max_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_mgmt_max_pckt_size_set()", port_ndx, 0);
}

/*********************************************************************
*     Set the maximal allowed packet size. The limitation can
 *     be performed based on the packet size before or after
 *     the ingress editing (external and internal configuration
 *     mode, accordingly). Packets above the specified value
 *     are dropped.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_mgmt_max_pckt_size_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      port_ndx,
    SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx,
    SOC_SAND_OUT uint32                       *max_size
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_MAX_PCKT_SIZE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(max_size);

  res = soc_pb_mgmt_max_pckt_size_get_verify(
          unit,
          port_ndx,
          conf_mode_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_mgmt_max_pckt_size_get_unsafe(
          unit,
          port_ndx,
          conf_mode_ndx,
          max_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_mgmt_max_pckt_size_get()", port_ndx, 0);
}

/*********************************************************************
*     Get the core clock frequency of the device.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_mgmt_core_frequency_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_OUT uint32                       *clk_freq
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_CORE_FREQUENCY_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  SOC_SAND_CHECK_NULL_INPUT(clk_freq);

  res = soc_pb_mgmt_core_frequency_get_unsafe(
          unit,
          clk_freq
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_mgmt_core_frequency_get()", 0, 0);
}

/*********************************************************************
*     Set the Soc_petra-B B0 revision specific features.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_mgmt_rev_b0_set(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  SOC_PB_MGMT_B0_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_REV_B0_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_pb_mgmt_rev_b0_set_verify(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_mgmt_rev_b0_set_unsafe(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_mgmt_rev_b0_set()", 0, 0);
}

/*********************************************************************
*     Set the Soc_petra-B B0 revision specific features.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_mgmt_rev_b0_get(
    SOC_SAND_IN  int       unit,
    SOC_SAND_OUT SOC_PB_MGMT_B0_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_REV_B0_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_mgmt_rev_b0_get_unsafe(
          unit,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_mgmt_rev_b0_get()", 0, 0);
}

void
  SOC_PB_MGMT_OPERATION_MODE_clear(
    SOC_SAND_OUT SOC_PB_MGMT_OPERATION_MODE *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_MGMT_OPERATION_MODE));
  info->pp_enable = 0;
  info->is_fap20_in_system = 0;
  info->is_fap21_in_system = 0;
  info->is_fe1600_in_system = 0;
  info->is_fe200_fabric = 0;
  soc_petra_PETRA_FABRIC_CELL_FORMAT_clear(&(info->fabric_cell_format));
  for (ind = 0; ind < SOC_PETRA_COMBO_NOF_QRTTS; ++ind)
  {
    info->is_combo_nif[ind] = 0;
  }
  soc_petra_PETRA_MGMT_SRD_REF_CLKS_clear(&(info->ref_clocks_conf));
  info->stag_enable = 0;
  info->egr_mc_16k_groups_enable = 0;
  info->tdm_mode = SOC_PETRA_MGMT_NOF_TDM_MODES;
  info->ftmh_lb_ext_mode = SOC_PB_MGMT_FTMH_LB_EXT_MODE_DISABLED;
  info->add_pph_eep_ext = TRUE;
  SOC_PB_MGMT_CORE_FREQ_HI_RESOLUTION_clear(&(info->core_freq_high_res));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_MGMT_CORE_FREQ_HI_RESOLUTION_clear(
    SOC_SAND_OUT SOC_PB_MGMT_CORE_FREQ_HI_RESOLUTION *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_MGMT_CORE_FREQ_HI_RESOLUTION));
  info->self_freq = SOC_SAND_INTERN_VAL_INVALID_32;
  info->min_fap_freq_in_system = SOC_SAND_INTERN_VAL_INVALID_32;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_INIT_PP_PORT_clear(
    SOC_SAND_OUT SOC_PB_INIT_PP_PORT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_INIT_PP_PORT));
  info->profile_ndx = 0;
  SOC_PB_PORT_PP_PORT_INFO_clear(&(info->conf));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_INIT_PP_PORT_MAP_clear(
    SOC_SAND_OUT SOC_PB_INIT_PP_PORT_MAP *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_INIT_PP_PORT_MAP));
  info->port_ndx = 0;
  info->pp_port = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_INIT_EGR_Q_PROFILE_MAP_clear(
    SOC_SAND_OUT SOC_PB_INIT_EGR_Q_PROFILE_MAP *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_INIT_EGR_Q_PROFILE_MAP));
  info->port_ndx = 0;
  info->conf = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_INIT_PORTS_clear(
    SOC_SAND_OUT SOC_PB_INIT_PORTS *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_INIT_PORTS));
  info->hdr_type_nof_entries = 0;
  for (ind = 0; ind < SOC_PETRA_NOF_FAP_PORTS; ++ind)
  {
    soc_petra_PETRA_INIT_PORT_HDR_TYPE_clear(&(info->hdr_type[ind]));
  }
  info->if_map_nof_entries = 0;
  for (ind = 0; ind < SOC_PETRA_NOF_FAP_PORTS; ++ind)
  {
    soc_petra_PETRA_INIT_PORT_TO_IF_MAP_clear(&(info->if_map[ind]));
  }
  info->pp_port_nof_entries = 0;
  for (ind = 0; ind < SOC_PB_PORT_NOF_PP_PORTS; ++ind)
  {
    SOC_PB_INIT_PP_PORT_clear(&(info->pp_port[ind]));
  }
  info->tm2pp_port_map_nof_entries = 0;
  for (ind = 0; ind < SOC_PETRA_NOF_FAP_PORTS; ++ind)
  {
    SOC_PB_INIT_PP_PORT_MAP_clear(&(info->tm2pp_port_map[ind]));
  }
  info->egr_q_profile_map_nof_entries = 0;
  for (ind = 0; ind < SOC_PETRA_NOF_FAP_PORTS; ++ind)
  {
    SOC_PB_INIT_EGR_Q_PROFILE_MAP_clear(&(info->egr_q_profile_map[ind]));
  }

  for (ind = 0; ind < SOC_PETRA_NOF_FAP_PORTS; ++ind)
  {
    soc_petra_PETRA_PORTS_OTMH_EXTENSIONS_EN_clear(&(info->otmh_ext_en[ind]));
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_HW_ADJ_MAL_clear(
    SOC_SAND_OUT SOC_PB_HW_ADJ_MAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_HW_ADJ_MAL));
  info->mal_ndx = 0;
  SOC_PB_NIF_MAL_BASIC_INFO_clear(&(info->conf));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_HW_ADJ_SPAUI_clear(
    SOC_SAND_OUT SOC_PB_HW_ADJ_SPAUI *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_HW_ADJ_SPAUI));
  info->if_ndx = SOC_PETRA_IF_ID_NONE;
  SOC_PB_NIF_SPAUI_INFO_clear(&(info->conf));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_HW_ADJ_GMII_clear(
    SOC_SAND_OUT SOC_PB_HW_ADJ_GMII *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_HW_ADJ_GMII));
  info->nif_ndx = SOC_PETRA_IF_ID_NONE;
  SOC_PB_NIF_GMII_INFO_clear(&(info->conf));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_HW_ADJ_FATP_clear(
    SOC_SAND_OUT SOC_PB_HW_ADJ_FATP *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_HW_ADJ_FATP));
  info->fatp_ndx = SOC_PB_NIF_NOF_FATP_IDS;
  SOC_PB_NIF_FATP_INFO_clear(&(info->conf));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_HW_ADJ_ILKN_clear(
    SOC_SAND_OUT SOC_PB_HW_ADJ_ILKN *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_HW_ADJ_ILKN));
  info->ilkn_ndx = SOC_PB_NIF_NOF_ILKN_IDS;
  SOC_PB_NIF_ILKN_INFO_clear(&(info->conf));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_HW_ADJ_ELK_clear(
    SOC_SAND_OUT SOC_PB_HW_ADJ_ELK *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_HW_ADJ_ELK));
  SOC_PB_NIF_ELK_INFO_clear(&(info->conf));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_HW_ADJ_SYNCE_clear(
    SOC_SAND_OUT SOC_PB_HW_ADJ_SYNCE *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_HW_ADJ_SYNCE));
  info->is_malg_b_enabled = 0;
  info->mode = SOC_PB_NIF_NOF_SYNCE_MODES;
  for (ind = 0; ind < SOC_PB_NIF_NOF_SYNCE_CLK_IDS; ++ind)
  {
    SOC_PB_NIF_SYNCE_CLK_clear(&(info->conf[ind]));
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_HW_ADJ_MDIO_clear(
    SOC_SAND_OUT SOC_PB_HW_ADJ_MDIO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  info->clk_freq_khz = SOC_PB_NIF_MDIO_CLK_FREQ_KHZ_DEF;
  
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_HW_ADJ_QDR_clear(
    SOC_SAND_OUT SOC_PB_HW_ADJ_QDR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_HW_ADJ_QDR));
  info->enable = 0;
  info->protection_type = SOC_PETRA_HW_NOF_QDR_PROTECT_TYPES;
  info->is_core_clock_freq = 0;
  soc_petra_PETRA_HW_PLL_PARAMS_clear(&(info->pll_conf));
  info->qdr_size_mbit = SOC_PETRA_HW_NOF_QDR_SIZE_MBITS;
  info->qdr_type = SOC_PB_HW_QDR_TYPE_QDR;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_HW_ADJ_NIF_clear(
    SOC_SAND_OUT SOC_PB_HW_ADJ_NIF *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_HW_ADJ_NIF));
  info->mal_nof_entries = 0;
  for (ind = 0; ind < SOC_PB_NOF_MAC_LANES; ++ind)
  {
    SOC_PB_HW_ADJ_MAL_clear(&(info->mal[ind]));
  }
  info->spaui_nof_entries = 0;
  for (ind = 0; ind < SOC_PB_NOF_MAC_LANES; ++ind)
  {
    SOC_PB_HW_ADJ_SPAUI_clear(&(info->spaui[ind]));
  }
  info->gmii_nof_entries = 0;
  for (ind = 0; ind < SOC_PB_NIF_NOF_NIFS; ++ind)
  {
    SOC_PB_HW_ADJ_GMII_clear(&(info->gmii[ind]));
  }
  info->ilkn_nof_entries = 0;
  for (ind = 0; ind < SOC_PB_NIF_NOF_ILKN_IDS; ++ind)
  {
    SOC_PB_HW_ADJ_ILKN_clear(&(info->ilkn[ind]));
  }
  info->fatp_nof_entries = 0;
  SOC_PB_NIF_FATP_MODE_INFO_clear(&(info->fatp_mode));
  for (ind = 0; ind < SOC_PB_NIF_NOF_FATP_IDS; ++ind)
  {
    SOC_PB_HW_ADJ_FATP_clear(&(info->fatp[ind]));
  }
  info->elk_nof_entries = 0;
  SOC_PB_HW_ADJ_ELK_clear(&(info->elk));
  info->synce_nof_entries = 0;
  SOC_PB_HW_ADJ_SYNCE_clear(&(info->synce));
  info->mdio_nof_entries = 0;
  SOC_PB_HW_ADJ_MDIO_clear(&(info->mdio));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_HW_ADJ_FC_SCHEME_clear(
    SOC_SAND_OUT SOC_PB_HW_ADJ_FC_SCHEME *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_HW_ADJ_FC_SCHEME));
  info->enable = 0;
  info->conf = SOC_PB_FC_NOF_NIF_OVERSUBSCR_SCHEMES;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_HW_ADJ_DRAM_PLL_clear(
    SOC_SAND_OUT SOC_PB_HW_ADJ_DRAM_PLL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_HW_ADJ_DRAM_PLL));
  
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_HW_ADJ_STAT_IF_clear(
    SOC_SAND_OUT SOC_PB_HW_ADJ_STAT_IF *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_HW_ADJ_STAT_IF));
  soc_petra_PETRA_STAT_IF_INFO_clear(&(info->if_conf));
  SOC_PB_STAT_IF_REPORT_INFO_clear(&(info->rep_conf));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_HW_ADJ_STREAMING_IF_clear(
    SOC_SAND_OUT SOC_PB_HW_ADJ_STREAMING_IF *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_HW_ADJ_STREAMING_IF));
  info->multi_port_mode = 0;
  info->enable_timeoutcnt = 0;
  info->timeout_prd = 0;
  info->quiet_mode = 0;
  info->discard_bad_parity = 0;
  info->discard_pkt_streaming = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_HW_ADJ_CORE_FREQ_clear(
    SOC_SAND_OUT SOC_PETRA_HW_ADJ_CORE_FREQ *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_HW_ADJ_CORE_FREQ));
  info->enable = 1;
  info->frequency = SOC_PETRA_MGMT_CORE_CLK_RATE_250MHZ;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  SOC_PB_HW_ADJUSTMENTS_clear(
    SOC_SAND_OUT SOC_PB_HW_ADJUSTMENTS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_HW_ADJUSTMENTS));
  soc_petra_PETRA_HW_ADJ_DDR_clear(&(info->dram));
  SOC_PB_HW_ADJ_DRAM_PLL_clear(&(info->dram_pll));
  SOC_PB_HW_ADJ_QDR_clear(&(info->qdr));
  soc_petra_PETRA_HW_ADJ_SERDES_clear(&(info->serdes));
  soc_petra_PETRA_HW_ADJ_FABRIC_clear(&(info->fabric));
  SOC_PB_HW_ADJ_NIF_clear(&(info->nif));
  SOC_PB_HW_ADJ_CORE_FREQ_clear(&(info->core_freq));
  SOC_PB_HW_ADJ_STAT_IF_clear(&(info->stat_if));
  SOC_PB_HW_ADJ_STREAMING_IF_clear(&(info->streaming_if));
  SOC_PB_HW_ADJ_FC_SCHEME_clear(&(info->fc_scheme));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_INIT_STAG_INFO_clear(
    SOC_SAND_OUT SOC_PB_INIT_STAG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_INIT_STAG_INFO));
  SOC_PB_ITM_STAG_INFO_clear(&(info->encoding));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_INIT_BASIC_CONF_clear(
    SOC_SAND_OUT SOC_PB_INIT_BASIC_CONF *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_INIT_BASIC_CONF));
  info->credit_worth_enable = 0;
  info->credit_worth = 0;
  info->stag_enable = 0;
  SOC_PB_INIT_STAG_INFO_clear(&(info->stag));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_INIT_OOR_clear(
    SOC_SAND_OUT SOC_PB_INIT_OOR *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_INIT_OOR));
  info->nof_entries = 0;
  for (ind = 0; ind < SOC_PB_NIF_NOF_NIFS_MAX; ++ind)
  {
    info->nif_id_active[ind] = SOC_PETRA_NIF_ID_NONE;
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_MGMT_B0_INFO_clear(
    SOC_SAND_OUT SOC_PB_MGMT_B0_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_MGMT_B0_INFO));
  info->bugfixes_enabled = FALSE;
  info->tdm_mc_route_mode = SOC_PB_INIT_MC_STATIC_ROUTE_MODE_DIS;
  info->is_fc_sch_mode_cbfc = FALSE;
  info->single_copy_mc_rng_bmp = 0;
  info->inner_eth_encap = SOC_SAND_PP_ETH_ENCAP_TYPE_ETH2;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PB_DEBUG_IS_LVL1


void
  SOC_PB_MGMT_OPERATION_MODE_print(
    SOC_SAND_IN  SOC_PB_MGMT_OPERATION_MODE *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("pp_enable: %u\n\r",info->pp_enable);
  soc_sand_os_printf("is_fap20_in_system: %u\n\r",info->is_fap20_in_system);
  soc_sand_os_printf("is_fap21_in_system: %u\n\r",info->is_fap21_in_system);
  soc_sand_os_printf("is_fe200_fabric: %u\n\r",info->is_fe200_fabric);
  soc_sand_os_printf("is_petra_rev_a_in_system: %u\n\r",info->is_petra_rev_a_in_system);
  soc_sand_os_printf("fabric_cell_format:");
  soc_petra_PETRA_FABRIC_CELL_FORMAT_print(&(info->fabric_cell_format));
  for (ind = 0; ind < SOC_PETRA_COMBO_NOF_QRTTS; ++ind)
  {
    soc_sand_os_printf("is_combo_nif[%u]: %u\n\r",ind,info->is_combo_nif[ind]);
  }
  soc_sand_os_printf("ref_clocks_conf:");
  soc_petra_PETRA_MGMT_SRD_REF_CLKS_print(&(info->ref_clocks_conf));
  soc_sand_os_printf("stag_enable: %u\n\r",info->stag_enable);
  soc_sand_os_printf("egr_mc_16k_groups_enable: %u\n\r",info->egr_mc_16k_groups_enable);
  soc_sand_os_printf("tdm_mode %s ", soc_petra_PETRA_MGMT_TDM_MODE_to_string(info->tdm_mode));
  soc_sand_os_printf("ftmh_lb_ext_mode %s ", soc_pb_PB_MGMT_FTMH_LB_EXT_MODE_to_string(info->ftmh_lb_ext_mode));
  soc_sand_os_printf("add_pph_eep_ext: %u\n\r",info->add_pph_eep_ext);
  SOC_PB_MGMT_CORE_FREQ_HI_RESOLUTION_print(&(info->core_freq_high_res));

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_MGMT_CORE_FREQ_HI_RESOLUTION_print(
    SOC_SAND_IN  SOC_PB_MGMT_CORE_FREQ_HI_RESOLUTION *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  if (info->self_freq == SOC_SAND_INTERN_VAL_INVALID_32)
  {
    soc_sand_os_printf("self_freq (hi resolution) not initialized, driver uses hw_adjustments (low resolution) value:\n\r");
  }
  else
  {
    soc_sand_os_printf("self_freq: %u[KHz]\n\r",info->self_freq);
  }

  if (info->min_fap_freq_in_system == SOC_SAND_INTERN_VAL_INVALID_32)
  {
    soc_sand_os_printf("min_fap_freq_in_system (hi resolution) not initialized.\n\r");
  }
  else
  {
    soc_sand_os_printf("min_fap_freq_in_system: %u[KHz]\n\r",info->min_fap_freq_in_system);
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}


void
  SOC_PB_INIT_PP_PORT_print(
    SOC_SAND_IN  SOC_PB_INIT_PP_PORT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("profile_ndx: %u\n\r",info->profile_ndx);
  soc_sand_os_printf("conf:");
  SOC_PB_PORT_PP_PORT_INFO_print(&(info->conf));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_INIT_PP_PORT_MAP_print(
    SOC_SAND_IN  SOC_PB_INIT_PP_PORT_MAP *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("port_ndx: %u\n\r",info->port_ndx);
  soc_sand_os_printf("pp_port: %u\n\r",info->pp_port);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_INIT_EGR_Q_PROFILE_MAP_print(
    SOC_SAND_IN  SOC_PB_INIT_EGR_Q_PROFILE_MAP *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("port_ndx: %u\n\r",info->port_ndx);
  soc_sand_os_printf("conf: %u\n\r",info->conf);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_INIT_PORTS_print(
    SOC_SAND_IN  SOC_PB_INIT_PORTS *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("hdr_type_nof_entries: %u\n\r",info->hdr_type_nof_entries);
  for (ind = 0; ind < SOC_PETRA_NOF_FAP_PORTS; ++ind)
  {
    soc_sand_os_printf("hdr_type[%u]:",ind);
    soc_petra_PETRA_INIT_PORT_HDR_TYPE_print(&(info->hdr_type[ind]));
  }
  soc_sand_os_printf("if_map_nof_entries: %u\n\r",info->if_map_nof_entries);
  for (ind = 0; ind < SOC_PETRA_NOF_FAP_PORTS; ++ind)
  {
    soc_sand_os_printf("if_map[%u]:",ind);
    soc_petra_PETRA_INIT_PORT_TO_IF_MAP_print(&(info->if_map[ind]));
  }
  soc_sand_os_printf("pp_port_nof_entries: %u\n\r",info->pp_port_nof_entries);
  for (ind = 0; ind < SOC_PB_PORT_NOF_PP_PORTS; ++ind)
  {
    soc_sand_os_printf("pp_port[%u]:",ind);
    SOC_PB_INIT_PP_PORT_print(&(info->pp_port[ind]));
  }
  soc_sand_os_printf("tm2pp_port_map_nof_entries: %u\n\r",info->tm2pp_port_map_nof_entries);
  for (ind = 0; ind < SOC_PETRA_NOF_FAP_PORTS; ++ind)
  {
    soc_sand_os_printf("tm2pp_port_map[%u]:",ind);
    SOC_PB_INIT_PP_PORT_MAP_print(&(info->tm2pp_port_map[ind]));
  }
  soc_sand_os_printf("egr_q_profile_map_nof_entries: %u\n\r",info->egr_q_profile_map_nof_entries);
  for (ind = 0; ind < SOC_PETRA_NOF_FAP_PORTS; ++ind)
  {
    soc_sand_os_printf("egr_q_profile_map[%u]:",ind);
    SOC_PB_INIT_EGR_Q_PROFILE_MAP_print(&(info->egr_q_profile_map[ind]));
  }

  for (ind = 0; ind < SOC_PETRA_NOF_FAP_PORTS; ++ind)
  {
    soc_sand_os_printf("otmh_ext_en[%u]:",ind);
    soc_petra_PETRA_PORTS_OTMH_EXTENSIONS_EN_print(&(info->otmh_ext_en[ind]));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_HW_ADJ_MAL_print(
    SOC_SAND_IN  SOC_PB_HW_ADJ_MAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("mal_ndx: %u\n\r",info->mal_ndx);
  soc_sand_os_printf("conf:");
  SOC_PB_NIF_MAL_BASIC_INFO_print(&(info->conf));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_HW_ADJ_SPAUI_print(
    SOC_SAND_IN  SOC_PB_HW_ADJ_SPAUI *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("if_ndx: %s\n\r",soc_petra_PETRA_INTERFACE_ID_to_string(info->if_ndx));
  soc_sand_os_printf("conf:");
  SOC_PB_NIF_SPAUI_INFO_print(&(info->conf));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_HW_ADJ_GMII_print(
    SOC_SAND_IN  SOC_PB_HW_ADJ_GMII *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("nif_ndx: %s: \n\r", soc_petra_PETRA_INTERFACE_ID_to_string(info->nif_ndx));
  soc_sand_os_printf("conf:");
  SOC_PB_NIF_GMII_INFO_print(&(info->conf));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_HW_ADJ_FATP_print(
    SOC_SAND_IN  SOC_PB_HW_ADJ_FATP *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("fatp_ndx %s ", SOC_PB_NIF_FATP_ID_to_string(info->fatp_ndx));
  soc_sand_os_printf("conf:");
  SOC_PB_NIF_FATP_INFO_print(&(info->conf));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_HW_ADJ_ILKN_print(
    SOC_SAND_IN  SOC_PB_HW_ADJ_ILKN *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("ilkn_ndx %s ", SOC_PB_NIF_ILKN_ID_to_string(info->ilkn_ndx));
  soc_sand_os_printf("conf:");
  SOC_PB_NIF_ILKN_INFO_print(&(info->conf));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_HW_ADJ_ELK_print(
    SOC_SAND_IN  SOC_PB_HW_ADJ_ELK *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("conf:");
  SOC_PB_NIF_ELK_INFO_print(&(info->conf));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_HW_ADJ_SYNCE_print(
    SOC_SAND_IN  SOC_PB_HW_ADJ_SYNCE *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("is_malg_b_enabled: %u\n\r",info->is_malg_b_enabled);
  soc_sand_os_printf("mode %s ", SOC_PB_NIF_SYNCE_MODE_to_string(info->mode));
  for (ind = 0; ind < SOC_PB_NIF_NOF_SYNCE_CLK_IDS; ++ind)
  {
    soc_sand_os_printf("conf[%u]:",ind);
    SOC_PB_NIF_SYNCE_CLK_print(&(info->conf[ind]));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_HW_ADJ_MDIO_print(
     SOC_SAND_IN  SOC_PB_HW_ADJ_MDIO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("mdio clock rate %d [KHz]", info->clk_freq_khz);
 
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

const char*
  SOC_PB_HW_QDR_TYPE_to_string(
    SOC_SAND_IN SOC_PB_HW_QDR_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PB_HW_QDR_TYPE_QDR:
    str = "type-2 (default)";
    break;

  case SOC_PB_HW_QDR_TYPE_QDR2P:
    str = "type-2-plus";
    break;

  case SOC_PB_HW_QDR_TYPE_QDR3:
    str = "type-3";
    break;

  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_PB_HW_ADJ_QDR_print(
    SOC_SAND_IN SOC_PB_HW_ADJ_QDR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf(" Enable:                %u\n\r",info->enable);
  soc_sand_os_printf(
    " Protection_type:       %s\n\r",
    soc_petra_PETRA_HW_QDR_PROTECT_TYPE_to_string(info->protection_type)
  );
  soc_sand_os_printf(" Is_core_clock_freq:    %u\n\r",info->is_core_clock_freq);
  soc_sand_os_printf(" Pll_conf:\n\r");
  soc_petra_PETRA_HW_PLL_PARAMS_print(&(info->pll_conf));
  soc_sand_os_printf(
    " Qdr_size_mbit:         %s[Mbits]\n\r",
    soc_petra_PETRA_HW_QDR_SIZE_MBIT_to_string(info->qdr_size_mbit)
  );
  soc_sand_os_printf(
    " Qdr_type:         %s\n\r",
    SOC_PB_HW_QDR_TYPE_to_string(info->qdr_type)
  );
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_HW_ADJ_NIF_print(
    SOC_SAND_IN  SOC_PB_HW_ADJ_NIF *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("mal_nof_entries: %u\n\r",info->mal_nof_entries);
  for (ind = 0; ind < SOC_PB_NOF_MAC_LANES; ++ind)
  {
    soc_sand_os_printf("mal[%u]:",ind);
    SOC_PB_HW_ADJ_MAL_print(&(info->mal[ind]));
  }
  soc_sand_os_printf("spaui_nof_entries: %u\n\r",info->spaui_nof_entries);
  for (ind = 0; ind < SOC_PB_NOF_MAC_LANES; ++ind)
  {
    soc_sand_os_printf("spaui[%u]:",ind);
    SOC_PB_HW_ADJ_SPAUI_print(&(info->spaui[ind]));
  }
  soc_sand_os_printf("gmii_nof_entries: %u\n\r",info->gmii_nof_entries);
  for (ind = 0; ind < SOC_PB_NIF_NOF_NIFS; ++ind)
  {
    soc_sand_os_printf("gmii[%u]:",ind);
    SOC_PB_HW_ADJ_GMII_print(&(info->gmii[ind]));
  }
  soc_sand_os_printf("ilkn_nof_entries: %u\n\r",info->ilkn_nof_entries);
  for (ind = 0; ind < SOC_PB_NIF_NOF_ILKN_IDS; ++ind)
  {
    soc_sand_os_printf("ilkn[%u]:",ind);
    SOC_PB_HW_ADJ_ILKN_print(&(info->ilkn[ind]));
  }
  soc_sand_os_printf("fatp_nof_entries: %u\n\r",info->fatp_nof_entries);
  soc_sand_os_printf("fatp_mode:");
  SOC_PB_NIF_FATP_MODE_INFO_print(&(info->fatp_mode));
  for (ind = 0; ind < SOC_PB_NIF_NOF_FATP_IDS; ++ind)
  {
    soc_sand_os_printf("fatp[%u]:",ind);
    SOC_PB_HW_ADJ_FATP_print(&(info->fatp[ind]));
  }
  soc_sand_os_printf("elk_nof_entries: %u\n\r",info->elk_nof_entries);
  soc_sand_os_printf("elk:");
  SOC_PB_HW_ADJ_ELK_print(&(info->elk));
  soc_sand_os_printf("synce_nof_entries: %u\n\r",info->synce_nof_entries);
  soc_sand_os_printf("synce:");
  SOC_PB_HW_ADJ_SYNCE_print(&(info->synce));
  SOC_PB_HW_ADJ_MDIO_print(&(info->mdio));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_HW_ADJ_FC_SCHEME_print(
    SOC_SAND_IN  SOC_PB_HW_ADJ_FC_SCHEME *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("enable: %u\n\r",info->enable);
  soc_sand_os_printf("conf %s ", SOC_PB_FC_NIF_OVERSUBSCR_SCHEME_to_string(info->conf));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_HW_ADJ_DRAM_PLL_print(
    SOC_SAND_IN  SOC_PB_HW_ADJ_DRAM_PLL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
  
  soc_sand_os_printf(
    ", r: %u, f: %u, r: %u"
    "\n\r",
    info->r,
    info->f,
    info->q
  );

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_HW_ADJ_STAT_IF_print(
    SOC_SAND_IN  SOC_PB_HW_ADJ_STAT_IF *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("if_conf:");
  soc_petra_PETRA_STAT_IF_INFO_print(&(info->if_conf));
  soc_sand_os_printf("rep_conf:");
  SOC_PB_STAT_IF_REPORT_INFO_print(&(info->rep_conf));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_HW_ADJ_STREAMING_IF_print(
    SOC_SAND_IN  SOC_PB_HW_ADJ_STREAMING_IF *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("multi_port_mode  : %u\n\r",info->multi_port_mode);
  soc_sand_os_printf("enable_timeoutcnt: %u\n\r",info->enable_timeoutcnt);
  soc_sand_os_printf("timeout_prd      : %u\n\r",info->timeout_prd);
  soc_sand_os_printf("quiet_mode       : %u\n\r",info->quiet_mode);
  soc_sand_os_printf("discard_bad_parity: %u\n\r",info->discard_bad_parity);
  soc_sand_os_printf("discard_pkt_streaming: %u\n\r",info->discard_pkt_streaming);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_HW_ADJUSTMENTS_print(
    SOC_SAND_IN  SOC_PB_HW_ADJUSTMENTS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("dram:");
  soc_petra_PETRA_HW_ADJ_DDR_print(&(info->dram));
  SOC_PB_HW_ADJ_DRAM_PLL_print(&(info->dram_pll));
  soc_sand_os_printf("qdr:");
  SOC_PB_HW_ADJ_QDR_print(&(info->qdr));
  soc_sand_os_printf("serdes:");
  soc_petra_PETRA_HW_ADJ_SERDES_print(&(info->serdes));
  soc_sand_os_printf("fabric:");
  soc_petra_PETRA_HW_ADJ_FABRIC_print(&(info->fabric));
  soc_sand_os_printf("nif:");
  SOC_PB_HW_ADJ_NIF_print(&(info->nif));
  soc_sand_os_printf("core_freq:");
  soc_petra_PETRA_HW_ADJ_CORE_FREQ_print(&(info->core_freq));
  soc_sand_os_printf("stat_if:");
  SOC_PB_HW_ADJ_STAT_IF_print(&(info->stat_if));
  soc_sand_os_printf("streaming_if:");
  SOC_PB_HW_ADJ_STREAMING_IF_print(&(info->streaming_if));
  soc_sand_os_printf("fc_scheme:");
  SOC_PB_HW_ADJ_FC_SCHEME_print(&(info->fc_scheme));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_INIT_STAG_INFO_print(
    SOC_SAND_IN  SOC_PB_INIT_STAG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("encoding:");
  SOC_PB_ITM_STAG_INFO_print(&(info->encoding));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_INIT_BASIC_CONF_print(
    SOC_SAND_IN  SOC_PB_INIT_BASIC_CONF *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("credit_worth_enable: %u\n\r",info->credit_worth_enable);
  soc_sand_os_printf("credit_worth: %u\n\r",info->credit_worth);
  soc_sand_os_printf("stag_enable: %u\n\r",info->stag_enable);
  soc_sand_os_printf("stag:");
  SOC_PB_INIT_STAG_INFO_print(&(info->stag));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_INIT_OOR_print(
    SOC_SAND_IN  SOC_PB_INIT_OOR *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("nof_entries: %u\n\r",info->nof_entries);
  for (ind = 0; ind < info->nof_entries; ++ind)
  {
    soc_sand_os_printf("nif_id_active[%s]:",soc_petra_PETRA_INTERFACE_ID_to_string(info->nif_id_active[ind]));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

const char*
  soc_pb_PB_MGMT_FTMH_LB_EXT_MODE_to_string(
    SOC_SAND_IN  SOC_PB_MGMT_FTMH_LB_EXT_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PB_MGMT_FTMH_LB_EXT_MODE_DISABLED:
    str = "disabled";
  break;
  case SOC_PB_MGMT_FTMH_LB_EXT_MODE_8B_LB_KEY_8B_STACKING_ROUTE_HISTORY:
    str = "lb_key_8b_stacking_route_history_8b";
  break;
  case SOC_PB_MGMT_FTMH_LB_EXT_MODE_16B_STACKING_ROUTE_HISTORY:
    str = "stacking_route_history_16b";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

const char*
  SOC_PB_INIT_MC_STATIC_ROUTE_MODE_to_string(
    SOC_SAND_IN  SOC_PB_INIT_MC_STATIC_ROUTE_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PB_INIT_MC_STATIC_ROUTE_MODE_DIS:
    str = "dis";
  break;
  case SOC_PB_INIT_MC_STATIC_ROUTE_MODE_PARTIAL:
    str = "partial";
  break;
  case SOC_PB_INIT_MC_STATIC_ROUTE_MODE_PARTIAL_COMMON:
    str = "partial_common";
    break;
  case SOC_PB_INIT_MC_STATIC_ROUTE_MODE_FULL:
    str = "full";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_PB_MGMT_B0_INFO_print(
    SOC_SAND_IN  SOC_PB_MGMT_B0_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("bugfixes_enabled: %u\n\r",info->bugfixes_enabled);
  soc_sand_os_printf("tdm_mc_route_mode %s ", SOC_PB_INIT_MC_STATIC_ROUTE_MODE_to_string(info->tdm_mc_route_mode));
  soc_sand_os_printf("is_fc_sch_mode_cbfc: %u\n\r",info->is_fc_sch_mode_cbfc);
  soc_sand_os_printf("single_copy_mc_rng_bmp: %u\n\r",info->single_copy_mc_rng_bmp);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PB_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

