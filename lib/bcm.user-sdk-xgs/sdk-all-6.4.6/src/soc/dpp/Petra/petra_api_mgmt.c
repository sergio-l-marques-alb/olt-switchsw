/* $Id: petra_api_mgmt.c,v 1.11 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/soc_petra/src/soc_petra_api_mgmt.c
*
* MODULE PREFIX:  soc_petra_mgmt
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
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_api_mgmt.h>
#include <soc/dpp/Petra/petra_mgmt.h>
#include <soc/dpp/Petra/petra_init.h>
#include <soc/dpp/Petra/petra_api_framework.h>
#include <soc/dpp/Petra/petra_general.h>
#include <soc/dpp/Petra/petra_sw_db.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>
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
  soc_petra_register_device(
             uint32                  *base_address,
    SOC_SAND_IN  SOC_SAND_RESET_DEVICE_FUNC_PTR reset_device_ptr,
    SOC_SAND_OUT int                 *unit_ptr
  )
{
  uint32
    res;
  int unit;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_REGISTER_DEVICE);

  SOC_SAND_CHECK_NULL_INPUT(base_address);
  SOC_SAND_CHECK_NULL_INPUT(unit_ptr);

  res = soc_petra_register_device_unsafe(
          base_address,
          reset_device_ptr,
          &unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *unit_ptr = unit;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_register_device()",0,0);
}

/*********************************************************************
*     Undo soc_petra_register_device()
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_unregister_device(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_UNREGISTER_DEVICE);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_petra_unregister_device_unsafe(
    unit
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_unregister_device()",0,0);
}

/*********************************************************************
*     Set soc_petra device operation mode. This defines
*     configurations such as support for certain header types
*     etc.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mgmt_operation_mode_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MGMT_OPERATION_MODE *op_mode
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_OPERATION_MODE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(op_mode);

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_mgmt_operation_mode_verify(
    unit,
    op_mode
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_mgmt_operation_mode_set_unsafe(
    unit,
    op_mode
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_operation_mode_set()",0,0);
}

/*********************************************************************
*     Set soc_petra device operation mode. This defines
*     configurations such as support for certain header types
*     etc.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mgmt_operation_mode_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_MGMT_OPERATION_MODE *op_mode
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_OPERATION_MODE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(op_mode);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_mgmt_operation_mode_get_unsafe(
    unit,
    op_mode
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_operation_mode_get()",0,0);
}

/*********************************************************************
*     Bytes-worth of a single credit. It should be configured
*     the same in all the FAPs in the systems, and should be
*     set before programming the scheduler.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mgmt_credit_worth_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  credit_worth
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_CREDIT_WORTH_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_petra_mgmt_credit_worth_verify(
    unit,
    credit_worth
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_mgmt_credit_worth_set_unsafe(
    unit,
    credit_worth
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_credit_worth_set()",0,0);
}

/*********************************************************************
*     Bytes-worth of a single credit. It should be configured
*     the same in all the FAPs in the systems, and should be
*     set before programming the scheduler.
*     Details: in the H file. (search for prototype)
*********************************************************************/
int
  soc_petra_mgmt_credit_worth_get_dispatch(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint32             *credit_worth
  )
{
    uint32 res,rv;

    res = soc_petra_mgmt_credit_worth_get(unit, credit_worth);
    rv = handle_sand_result(res);
    SOC_IF_ERROR_RETURN(rv);

    return SOC_E_NONE;
}


uint32
  soc_petra_mgmt_credit_worth_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint32                  *credit_worth
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_CREDIT_WORTH_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(credit_worth);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_mgmt_credit_worth_get_unsafe(
    unit,
    credit_worth
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_credit_worth_get()",0,0);
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
  soc_petra_mgmt_init_sequence_phase1(
    SOC_SAND_IN     int                 unit,
    SOC_SAND_INOUT  SOC_PETRA_HW_ADJUSTMENTS      *hw_adjust,
    SOC_SAND_IN     SOC_PETRA_INIT_BASIC_CONF     *basic_conf,
    SOC_SAND_IN     SOC_PETRA_INIT_PORTS          *fap_ports,
    SOC_SAND_IN     uint8                 silent
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_INIT_SEQUENCE_PHASE1);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(hw_adjust);
  SOC_SAND_CHECK_NULL_INPUT(basic_conf);
  SOC_SAND_CHECK_NULL_INPUT(fap_ports);

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

   res = soc_petra_mgmt_init_sequence_phase1_verify(
    unit,
    hw_adjust,
    basic_conf,
    fap_ports
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

  res = soc_petra_mgmt_init_sequence_phase1_unsafe(
    unit,
    hw_adjust,
    basic_conf,
    fap_ports,
    silent
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_init_sequence_phase1()",0,0);
}

/*********************************************************************
*     Out-of-reset sequence. Enable/Disable the device from
*     receiving and transmitting control cells.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mgmt_init_sequence_phase2(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INIT_OOR            *oor_info,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_INIT_SEQUENCE_PHASE2);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(oor_info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  res = soc_petra_mgmt_init_sequence_phase2_verify(
    unit,
    oor_info
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_petra_mgmt_init_sequence_phase2_unsafe(
    unit,
    oor_info,
    silent
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_init_sequence_phase2()",0,0);
}

/*********************************************************************
*     Set the fabric system ID of the device. Must be unique
*     in the system.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mgmt_system_fap_id_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 sys_fap_id
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_SYSTEM_FAP_ID_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_mgmt_system_fap_id_verify(
    unit,
    sys_fap_id
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_petra_mgmt_system_fap_id_set_unsafe(
    unit,
    sys_fap_id
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_system_fap_id_set()",0,0);
}

/*********************************************************************
*     Set the fabric system ID of the device. Must be unique
*     in the system.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mgmt_system_fap_id_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint32                 *sys_fap_id
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_SYSTEM_FAP_ID_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(sys_fap_id);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_mgmt_system_fap_id_get_unsafe(
    unit,
    sys_fap_id
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_system_fap_id_get()",0,0);
}

/*********************************************************************
*     Initialize a sub-set of the HW interfaces of the device.
*     The function might be called more than once, each time
*     with different fields, indicated to be written to the
*     device
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mgmt_hw_interfaces_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_HW_ADJUSTMENTS      *hw_adjust,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_HW_INTERFACES_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(hw_adjust);

  /*
   *	Invalid for Soc_petra-B
   */
  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_mgmt_hw_interfaces_verify(
    unit,
    hw_adjust
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_petra_mgmt_hw_interfaces_set_unsafe(
          unit,
          hw_adjust,
          silent
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_hw_interfaces_set()",0,0);
}

/*********************************************************************
*     Enable / Disable the device from receiving and
*     transmitting control cells.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mgmt_all_ctrl_cells_enable_set(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint8  enable
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_ALL_CTRL_CELLS_ENABLE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_petra_mgmt_all_ctrl_cells_enable_verify(
    unit,
    enable
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_mgmt_all_ctrl_cells_enable_set_unsafe(
    unit,
    enable
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_all_ctrl_cells_enable_set()",0,0);
}

/*********************************************************************
*     Enable / Disable the device from receiving and
*     transmitting control cells.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mgmt_all_ctrl_cells_enable_get(
    SOC_SAND_IN  int  unit,
    SOC_SAND_OUT uint8  *enable
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_ALL_CTRL_CELLS_ENABLE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_mgmt_all_ctrl_cells_enable_get_unsafe(
    unit,
    enable
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_all_ctrl_cells_enable_get()",0,0);
}

/*********************************************************************
*     Enable / Disable the device from receiving and
*     transmitting traffic.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mgmt_enable_traffic_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 enable
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_ENABLE_TRAFFIC_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_petra_mgmt_enable_traffic_verify(
    unit,
    enable
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_mgmt_enable_traffic_set_unsafe(
    unit,
    enable
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_enable_traffic_set()",0,0);
}

/*********************************************************************
*     Enable / Disable the device from receiving and
*     transmitting traffic.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mgmt_enable_traffic_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint8                 *enable
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_ENABLE_TRAFFIC_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(enable);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_mgmt_enable_traffic_get_unsafe(
    unit,
    enable
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_enable_traffic_get()",0,0);
}

/*********************************************************************
*     Set the boundaries (minimal/maximal allowed size) for
*     the expected packets. The limitation can be performed
*     based on the packet size before or after the ingress
*     editing (external and internal configuration mode,
*     accordingly). Packets outside the specified range are
*     dropped.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_mgmt_pckt_size_range_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx,
    SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE      *size_range
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_PCKT_SIZE_RANGE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(size_range);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_mgmt_pckt_size_range_verify(
    unit,
    conf_mode_ndx,
    size_range
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit_semaphore);

  res = soc_petra_mgmt_pckt_size_range_set_unsafe(
    unit,
    conf_mode_ndx,
    size_range
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_pckt_size_range_set()",0,0);
}

/*********************************************************************
*     Set the boundaries (minimal/maximal allowed size) for
*     the expected packets. The limitation can be performed
*     based on the packet size before or after the ingress
*     editing (external and internal configuration mode,
*     accordingly). Packets outside the specified range are
*     dropped.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_mgmt_pckt_size_range_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx,
    SOC_SAND_OUT SOC_PETRA_MGMT_PCKT_SIZE      *size_range
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_PCKT_SIZE_RANGE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(size_range);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_mgmt_pckt_size_range_get_unsafe(
    unit,
    conf_mode_ndx,
    size_range
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_pckt_size_range_get()",0,0);
}

/*********************************************************************
*     Get the core clock frequency of the device.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mgmt_core_frequency_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT uint32                                  *clk_freq
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_CORE_FREQUENCY_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_mgmt_core_frequency_get_unsafe(
          unit,
          clk_freq
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PETRA_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mgmt_core_frequency_get()", 0, 0);
}

/*********************************************************************
*     Get the core clock frequency of the device.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mgmt_init_sequence_fixes_apply(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_INIT_SEQUENCE_FIXES_APPLY);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_petra_mgmt_init_sequence_fixes_apply_unsafe(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PETRA_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mgmt_init_sequence_fixes_apply()", 0, 0);
}

void
  soc_petra_PETRA_REVISION_CTRL_REV_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_REVISION_CTRL_REV_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_REVISION_CTRL_REV_INFO));
  info->device_rev_id = SOC_PETRA_REVISION_NOF_IDS;
  info->device_sub_type = SOC_PETRA_REVISION_NOF_TYPES;
  info->enable_bug_fixes = 0;
  info->enable_features = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_REVISION_CTRL_clear(
    SOC_SAND_OUT SOC_PETRA_REVISION_CTRL *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_REVISION_CTRL));
  info->nof_valid_entries = 0;
  for (ind=0; ind<SOC_PETRA_MAX_NOF_REVISIONS; ++ind)
  {
    soc_petra_PETRA_REVISION_CTRL_REV_INFO_clear(&(info->revision_info[ind]));
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_HW_PLL_PARAMS_clear(
    SOC_SAND_OUT SOC_PETRA_HW_PLL_PARAMS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_HW_PLL_PARAMS));
  info->m = 0;
  info->n = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_HW_ADJ_DDR_clear(
    SOC_SAND_OUT SOC_PETRA_HW_ADJ_DDR *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_HW_ADJ_DDR));
  info->enable = 0;
  for (ind=0; ind<SOC_DPP_HW_DRAM_INTERFACES_MAX_PETRAB; ++ind)
  {
    info->is_valid[ind] = 0;
  }
  info->dram_type = SOC_PETRA_DRAM_NOF_TYPES;
  soc_petra_PETRA_HW_PLL_PARAMS_clear(&(info->pll_conf));
  info->nof_banks = SOC_PETRA_NOF_DRAM_NUM_BANKS;
  info->nof_columns = SOC_PETRA_NOF_DRAM_NUMS_COLUMNS;
  info->dram_size_total_mbyte = 0;
  info->dbuff_size = SOC_PETRA_ITM_NOF_DBUFF_SIZES;
  info->conf_mode = SOC_PETRA_HW_DRAM_CONF_MODE_BUFFER;
  for (ind=0; ind<SOC_PETRA_HW_DRAM_CONF_SIZE_MAX; ++ind)
  {
    soc_petra_PETRA_REG_INFO_clear(&info->dram_conf.buffer_mode.buff_seq[ind]);
  }
  info->dram_conf.buffer_mode.buff_len = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_HW_ADJ_STAT_IF_clear(
    SOC_SAND_OUT SOC_PETRA_HW_ADJ_STAT_IF *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_HW_ADJ_STAT_IF));
  soc_petra_PETRA_STAT_IF_INFO_clear(&info->if_conf);
  soc_petra_PETRA_STAT_IF_REPORT_INFO_clear(&info->rep_conf);
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_HW_ADJ_QDR_clear(
    SOC_SAND_OUT SOC_PETRA_HW_ADJ_QDR *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_HW_ADJ_QDR));
  info->enable = 0;
  info->protection_type = SOC_PETRA_HW_NOF_QDR_PROTECT_TYPES;
  info->is_core_clock_freq = 0;
  soc_petra_PETRA_HW_PLL_PARAMS_clear(&(info->pll_conf));
  info->qdr_size_mbit = SOC_PETRA_HW_NOF_QDR_SIZE_MBITS;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_HW_ADJ_SERDES_clear(
    SOC_SAND_OUT SOC_PETRA_HW_ADJ_SERDES *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_HW_ADJ_SERDES));
  info->enable = 0;
  soc_petra_PETRA_SRD_ALL_INFO_clear(&(info->conf));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_HW_ADJ_FABRIC_clear(
    SOC_SAND_OUT SOC_PETRA_HW_ADJ_FABRIC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_HW_ADJ_FABRIC));
  info->enable = 0;
  info->connect_mode = SOC_PETRA_FABRIC_NOF_CONNECT_MODES;
  info->ftmh_extension = SOC_PETRA_PORTS_FTMH_EXT_OUTLIF_NEVER;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_HW_ADJ_MAL_clear(
    SOC_SAND_OUT SOC_PETRA_HW_ADJ_MAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_HW_ADJ_MAL));
  info->mal_ndx = 0;
  soc_petra_PETRA_NIF_INFO_clear(&(info->conf));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_HW_ADJ_SPAUI_clear(
    SOC_SAND_OUT SOC_PETRA_HW_ADJ_SPAUI *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_HW_ADJ_SPAUI));
  info->mal_ndx = 0;
  soc_petra_PETRA_NIF_SPAUI_INFO_clear(&(info->conf));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_HW_ADJ_SGMII_clear(
    SOC_SAND_OUT SOC_PETRA_HW_ADJ_SGMII *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_HW_ADJ_SGMII));
  info->nif_ndx = SOC_PETRA_IF_NOF_NIFS;
  soc_petra_PETRA_NIF_SGMII_INFO_clear(&(info->conf));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_HW_ADJ_FAT_PIPE_clear(
    SOC_SAND_OUT SOC_PETRA_HW_ADJ_FAT_PIPE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_HW_ADJ_FAT_PIPE));
  info->enable = 0;
  soc_petra_PETRA_NIF_FAT_PIPE_INFO_clear(&(info->conf));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_HW_ADJ_NIF_clear(
    SOC_SAND_OUT SOC_PETRA_HW_ADJ_NIF *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_HW_ADJ_NIF));
  info->mal_nof_entries = 0;
  for (ind=0; ind<SOC_PETRA_NOF_MAC_LANES; ++ind)
  {
    soc_petra_PETRA_HW_ADJ_MAL_clear(&(info->mal[ind]));
  }
  info->spaui_nof_entries = 0;
  for (ind=0; ind<SOC_PETRA_NOF_MAC_LANES; ++ind)
  {
    soc_petra_PETRA_HW_ADJ_SPAUI_clear(&(info->spaui[ind]));
  }
  info->sgmii_nof_entries = 0;
  for (ind=0; ind<SOC_PETRA_IF_NOF_NIFS; ++ind)
  {
    soc_petra_PETRA_HW_ADJ_SGMII_clear(&(info->sgmii[ind]));
  }
  info->fat_pipe_enable = 0;
  soc_petra_PETRA_HW_ADJ_FAT_PIPE_clear(&(info->fat_pipe));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_HW_ADJ_CORE_FREQ_clear(
    SOC_SAND_OUT SOC_PETRA_HW_ADJ_CORE_FREQ *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_HW_ADJ_CORE_FREQ));
  info->enable = 0;
  info->frequency = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_HW_ADJUSTMENTS_clear(
    SOC_SAND_OUT SOC_PETRA_HW_ADJUSTMENTS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_HW_ADJUSTMENTS));
  soc_petra_PETRA_HW_ADJ_DDR_clear(&(info->dram));
  soc_petra_PETRA_HW_ADJ_QDR_clear(&(info->qdr));
  soc_petra_PETRA_HW_ADJ_SERDES_clear(&(info->serdes));
  soc_petra_PETRA_HW_ADJ_FABRIC_clear(&(info->fabric));
  soc_petra_PETRA_HW_ADJ_NIF_clear(&(info->nif));
  soc_petra_PETRA_HW_ADJ_CORE_FREQ_clear(&(info->core_freq));
  soc_petra_PETRA_HW_ADJ_STAT_IF_clear(&(info->stat_if));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_INIT_STAG_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_INIT_STAG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_INIT_STAG_INFO));
  soc_petra_PETRA_IHP_PCKT_STAG_HDR_DATA_clear(&(info->fld_conf));
  soc_petra_PETRA_ITM_STAG_INFO_clear(&(info->encoding));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_INIT_BASIC_CONF_clear(
    SOC_SAND_OUT SOC_PETRA_INIT_BASIC_CONF *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_INIT_BASIC_CONF));
  info->credit_worth_enable = 0;
  info->credit_worth = 0;
  info->stag_enable = 0;
  info->mem_shadow_enable = 0;
  soc_petra_PETRA_INIT_STAG_INFO_clear(&(info->stag));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_INIT_PORT_HDR_TYPE_clear(
    SOC_SAND_OUT SOC_PETRA_INIT_PORT_HDR_TYPE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_INIT_PORT_HDR_TYPE));
  info->port_ndx = 0;
  info->header_type_in = SOC_PETRA_PORT_NOF_HEADER_TYPES;
  info->header_type_out = SOC_PETRA_PORT_NOF_HEADER_TYPES;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_INIT_PORT_TO_IF_MAP_clear(
    SOC_SAND_OUT SOC_PETRA_INIT_PORT_TO_IF_MAP *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_INIT_PORT_TO_IF_MAP));
  info->port_ndx = 0;
  soc_petra_PETRA_PORT2IF_MAPPING_INFO_clear(&(info->conf));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_INIT_PCKT_HDR_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_INIT_PCKT_HDR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_INIT_PCKT_HDR_INFO));
  info->port_ndx = 0;
  soc_petra_PETRA_IHP_PORT_INFO_clear(&(info->conf));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_INIT_PORTS_clear(
    SOC_SAND_OUT SOC_PETRA_INIT_PORTS *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_INIT_PORTS));
  info->hdr_type_nof_entries = 0;
  for (ind=0; ind<SOC_PETRA_NOF_FAP_PORTS; ++ind)
  {
    soc_petra_PETRA_INIT_PORT_HDR_TYPE_clear(&(info->hdr_type[ind]));
  }
  info->if_map_nof_entries = 0;
  for (ind=0; ind<SOC_PETRA_NOF_FAP_PORTS; ++ind)
  {
    soc_petra_PETRA_INIT_PORT_TO_IF_MAP_clear(&(info->if_map[ind]));
  }
  info->packet_hdr_info_nof_entries = 0;
  for (ind=0; ind<SOC_PETRA_NOF_FAP_PORTS; ++ind)
  {
    soc_petra_PETRA_INIT_PCKT_HDR_INFO_clear(&(info->packet_hdr_info[ind]));
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_MGMT_SRD_REF_CLKS_clear(
    SOC_SAND_OUT SOC_PETRA_MGMT_SRD_REF_CLKS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_MGMT_SRD_REF_CLKS));
  info->nif_ref_clk = SOC_PETRA_MGMT_NOF_SRD_REF_CLKS;
  info->fabric_ref_clk = SOC_PETRA_MGMT_NOF_SRD_REF_CLKS;
  info->combo_ref_clk = SOC_PETRA_MGMT_NOF_SRD_REF_CLKS;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_MGMT_PCKT_SIZE_clear(
    SOC_SAND_OUT SOC_PETRA_MGMT_PCKT_SIZE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_MGMT_PCKT_SIZE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_MGMT_OPERATION_MODE_clear(
    SOC_SAND_OUT SOC_PETRA_MGMT_OPERATION_MODE *info
  )
{
  uint32
    idx;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_MGMT_OPERATION_MODE));
  info->device_sub_type = SOC_PETRA_REVISION_NOF_TYPES;
  info->pp_enable = FALSE;
  info->is_fap20_in_system = FALSE;
  info->is_fap21_in_system = FALSE;
  info->is_a1_or_below_in_system = FALSE;
  info->is_fe200_fabric = FALSE;
  soc_petra_PETRA_FABRIC_CELL_FORMAT_clear(&(info->fabric_cell_format));
  for (idx = 0; idx < SOC_PETRA_COMBO_NOF_QRTTS; idx++)
  {
    info->is_combo_nif[idx] = 0;
  }
  soc_petra_PETRA_MGMT_SRD_REF_CLKS_clear(&(info->ref_clocks_conf));
  info->stag_enable = 0;
  info->egr_mc_16k_groups_enable = TRUE;
  info->tdm_mode = SOC_PETRA_MGMT_NOF_TDM_MODES;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_INIT_OOR_clear(
    SOC_SAND_OUT SOC_PETRA_INIT_OOR *info
  )
{
  uint32 ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PETRA_INIT_OOR));
  for (ind=0; ind<SOC_PETRA_NOF_MAC_LANES; ++ind)
  {
    info->mal[ind] = 0;
  }
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PETRA_DEBUG_IS_LVL1

const char*
  soc_petra_PETRA_REVISION_ID_to_string(
    SOC_SAND_IN SOC_PETRA_REVISION_ID enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_REVISION_ID_1:
    str = "REVISION_ID_1";
  break;

  case SOC_PETRA_REVISION_NOF_IDS:
    str = "REVISION_NOF_IDS";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

const char*
  soc_petra_PETRA_REVISION_SUB_TYPE_ID_to_string(
    SOC_SAND_IN SOC_PETRA_REVISION_SUB_TYPE_ID enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_REVISION_TYPE_88210:
    str = "REVISION_TYPE_88210";
  break;

  case SOC_PETRA_REVISION_TYPE_88220:
    str = "REVISION_TYPE_88220";
  break;

  case SOC_PETRA_REVISION_TYPE_88230:
    str = "REVISION_TYPE_88230";
  break;

  case SOC_PETRA_REVISION_TYPE_88330:
    str = "REVISION_TYPE_88330";
  break;

  case SOC_PETRA_REVISION_NOF_TYPES:
    str = "REVISION_NOF_TYPES";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

const char*
  soc_petra_PETRA_HW_DRAM_CONF_MODE_to_string(
    SOC_SAND_IN SOC_PETRA_HW_DRAM_CONF_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_HW_DRAM_CONF_MODE_BUFFER:
    str = "SOC_PETRA_HW_DRAM_CONF_MODE_BUFFER";
  break;

  case SOC_PETRA_HW_DRAM_CONF_MODE_PARAMS:
    str = "SOC_PETRA_HW_DRAM_CONF_MODE_PARAMS";
  break;

  case SOC_PETRA_HW_NOF_DRAM_CONF_MODES:
    str = "SOC_PETRA_HW_NOF_DRAM_CONF_MODES";
  break;

  default:
    str = " Unknown";
  }
  return str;
}


const char*
  soc_petra_PETRA_HW_QDR_PROTECT_TYPE_to_string(
    SOC_SAND_IN SOC_PETRA_HW_QDR_PROTECT_TYPE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_HW_QDR_PROTECT_TYPE_PARITY:
    str = "PARITY";
  break;

  case SOC_PETRA_HW_QDR_PROTECT_TYPE_ECC:
    str = "ECC";
  break;

  case SOC_PETRA_HW_NOF_QDR_PROTECT_TYPES:
    str = "NOF_QDR_PROTECT_TYPES";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

const char*
  soc_petra_PETRA_HW_QDR_SIZE_MBIT_to_string(
    SOC_SAND_IN SOC_PETRA_HW_QDR_SIZE_MBIT enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_HW_QDR_SIZE_MBIT_18:
    str = "MBIT_18";
  break;

  case SOC_PETRA_HW_QDR_SIZE_MBIT_36:
    str = "MBIT_36";
  break;

  case SOC_PETRA_HW_QDR_SIZE_MBIT_72:
    str = "MBIT_72";
  break;

  case SOC_PETRA_HW_QDR_SIZE_MBIT_144:
    str = "MBIT_144";
  break;

  case SOC_PETRA_HW_NOF_QDR_SIZE_MBITS:
    str = "NOF_QDR_SIZE_MBITS";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

const char*
  soc_petra_PETRA_MGMT_FABRIC_HDR_TYPE_to_string(
    SOC_SAND_IN SOC_PETRA_MGMT_FABRIC_HDR_TYPE enum_val
  )
{
  return SOC_TMC_MGMT_FABRIC_HDR_TYPE_to_string(enum_val);
}

const char*
  soc_petra_PETRA_MGMT_SRD_REF_CLK_to_string(
    SOC_SAND_IN SOC_PETRA_MGMT_SRD_REF_CLK enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_MGMT_SRD_REF_CLK_125_00:
    str = "125.00 MHz";
  break;

  case SOC_PETRA_MGMT_SRD_REF_CLK_156_25:
    str = "156.25 MHz";
  break;

  case SOC_PETRA_MGMT_SRD_REF_CLK_200_00:
    str = "200.00 MHz";
  break;

  case SOC_PETRA_MGMT_SRD_REF_CLK_312_50:
    str = "312.50 MHz";
  break;

  case SOC_PETRA_MGMT_SRD_REF_CLK_218_75:
    str = "218.75 MHz";
  break;

  case SOC_PETRA_MGMT_SRD_REF_CLK_212_50:
    str = "212.50 MHz";
  break;

  case SOC_PETRA_MGMT_NOF_SRD_REF_CLKS:
    str = "MGMT_NOF_SRD_REF_CLKS";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

const char*
  soc_petra_PETRA_MGMT_PCKT_SIZE_CONF_MODE_to_string(
    SOC_SAND_IN SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE_EXTERN:
    str = "EXTERNAL";
  break;

  case SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE_INTERN:
    str = "INTERNAL";
  break;

  case SOC_PETRA_MGMT_NOF_PCKT_SIZE_CONF_MODES:
    str = "NOF_PCKT_SIZE_CONF_MODES";
  break;

  default:
    str = " Unknown";
  }
  return str;
}

const char*
  soc_petra_PETRA_MGMT_TDM_MODE_to_string(
    SOC_SAND_IN  SOC_PETRA_MGMT_TDM_MODE enum_val
  )
{
  return SOC_TMC_MGMT_TDM_MODE_to_string(enum_val);
}
void
  soc_petra_PETRA_REVISION_CTRL_REV_INFO_print(
    SOC_SAND_IN SOC_PETRA_REVISION_CTRL_REV_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf(
    "  Device_rev_id:    %s \n\r",
    soc_petra_PETRA_REVISION_ID_to_string(info->device_rev_id)
  );
  soc_sand_os_printf(
    "  Device_sub_type:  %s \n\r",
    soc_petra_PETRA_REVISION_SUB_TYPE_ID_to_string(info->device_sub_type)
  );
  soc_sand_os_printf("  Enable_bug_fixes: %u\n\r",info->enable_bug_fixes);
  soc_sand_os_printf("  Enable_features:  %u\n\r",info->enable_features);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_REVISION_CTRL_print(
    SOC_SAND_IN SOC_PETRA_REVISION_CTRL *info
  )
{
  uint32 ind=0;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("Nof_valid_entries: %u[Entries]\n\r",info->nof_valid_entries);
  for (ind=0; ind<SOC_PETRA_MAX_NOF_REVISIONS; ++ind)
  {
    soc_sand_os_printf("Revision_info[%u]:\n\r",ind);
    soc_petra_PETRA_REVISION_CTRL_REV_INFO_print(&(info->revision_info[ind]));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_HW_PLL_PARAMS_print(
    SOC_SAND_IN SOC_PETRA_HW_PLL_PARAMS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf(" m:                     %u\n\r",info->m);
  soc_sand_os_printf(" n:                     %u\n\r",info->n);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_HW_ADJ_DDR_print(
    SOC_SAND_IN SOC_PETRA_HW_ADJ_DDR *info
  )
{
  uint32 ind=0;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf(" Enable:                %u\n\r",info->enable);
  for (ind=0; ind<SOC_DPP_HW_DRAM_INTERFACES_MAX_PETRAB; ++ind)
  {
    soc_sand_os_printf(" Is_valid[%u]:          %u\n\r",ind,info->is_valid[ind]);
  }
  soc_sand_os_printf(
    " Dram_type:             %s\n\r",
    soc_petra_PETRA_DRAM_TYPE_to_string(info->dram_type)
  );
  soc_sand_os_printf(" Pll_conf:\n\r");
  soc_petra_PETRA_HW_PLL_PARAMS_print(&(info->pll_conf));
  soc_sand_os_printf(
    " Nof_banks:             %s[Banks]\n\r",
    soc_petra_PETRA_DRAM_NUM_BANKS_to_string(info->nof_banks)
  );
  soc_sand_os_printf(
    " Nof_columns:           %s[Columns]\n\r",
    soc_petra_PETRA_DRAM_NUM_COLUMNS_to_string(info->nof_columns)
  );
  soc_sand_os_printf(" Dram_size_total_mbyte: %u[MBytes]\n\r",info->dram_size_total_mbyte);
  soc_sand_os_printf(
    " Dbuff_size:            %s[Bytes]\n\r",
    soc_petra_PETRA_ITM_DBUFF_SIZE_BYTES_to_string(info->dbuff_size)
  );
  if (info->conf_mode == SOC_PETRA_HW_DRAM_CONF_MODE_PARAMS)
  {
    soc_sand_os_printf(" Dram freq: %u\n\r",info->dram_conf.params_mode.dram_freq);
    soc_petra_PETRA_DRAM_INFO_print(info->dram_type, &info->dram_conf.params_mode.params);
  }
  if (info->conf_mode == SOC_PETRA_HW_DRAM_CONF_MODE_BUFFER)
  {
    for (ind=0; ind<info->dram_conf.buffer_mode.buff_len; ++ind)
    {
      soc_petra_PETRA_REG_INFO_print(&info->dram_conf.buffer_mode.buff_seq[ind]);
    }
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_HW_ADJ_CORE_FREQ_print(
    SOC_SAND_IN  SOC_PETRA_HW_ADJ_CORE_FREQ *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("enable: %u\n\r",info->enable);
  soc_sand_os_printf("frequency: %u[MHz]\n\r",info->frequency);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_HW_ADJ_STAT_IF_print(
    SOC_SAND_IN SOC_PETRA_HW_ADJ_STAT_IF *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_petra_PETRA_STAT_IF_INFO_print(&info->if_conf);
  soc_petra_PETRA_STAT_IF_REPORT_INFO_print(&info->rep_conf);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_HW_ADJ_QDR_print(
    SOC_SAND_IN SOC_PETRA_HW_ADJ_QDR *info
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
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_HW_ADJ_SERDES_print(
    SOC_SAND_IN SOC_PETRA_HW_ADJ_SERDES *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf(" Enable:                %u\n\r",info->enable);
  soc_sand_os_printf(" Conf:\n\r");
  soc_petra_PETRA_SRD_ALL_INFO_print(SOC_PETRA_SRD_TX_PHYS_CONF_MODE_INTERNAL,&(info->conf));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_HW_ADJ_FABRIC_print(
    SOC_SAND_IN SOC_PETRA_HW_ADJ_FABRIC *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf(" Enable:                      %u\n\r",info->enable);
  soc_sand_os_printf(
    " Connect_mode:                %s \n\r",
    soc_petra_PETRA_FABRIC_CONNECT_MODE_to_string(info->connect_mode)
  );
  soc_sand_os_printf(
    "Ftmh_extension:               %s \n\r",
    soc_petra_PETRA_PORTS_FTMH_EXT_OUTLIF_to_string(info->ftmh_extension)
  );
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_HW_ADJ_MAL_print(
    SOC_SAND_IN SOC_PETRA_HW_ADJ_MAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("Mal_ndx: %u\n\r",info->mal_ndx);
  soc_sand_os_printf("  Conf:\n\r");
  soc_petra_PETRA_NIF_INFO_print(&(info->conf));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_HW_ADJ_SPAUI_print(
    SOC_SAND_IN SOC_PETRA_HW_ADJ_SPAUI *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("Mal_ndx: %u\n\r",info->mal_ndx);
  soc_sand_os_printf("  Conf:\n\r");
  soc_petra_PETRA_NIF_SPAUI_INFO_print(&(info->conf));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_HW_ADJ_SGMII_print(
    SOC_SAND_IN SOC_PETRA_HW_ADJ_SGMII *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf(
    "Nif_ndx %s \n\r",
    soc_petra_PETRA_INTERFACE_ID_to_string(info->nif_ndx)
  );
  soc_sand_os_printf("  Conf:\n\r");
  soc_petra_PETRA_NIF_SGMII_INFO_print(&(info->conf));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_HW_ADJ_FAT_PIPE_print(
    SOC_SAND_IN SOC_PETRA_HW_ADJ_FAT_PIPE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("Enable: %u\n\r",info->enable);
  soc_sand_os_printf("  Conf:");
  soc_petra_PETRA_NIF_FAT_PIPE_INFO_print(&(info->conf));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_HW_ADJ_NIF_print(
    SOC_SAND_IN SOC_PETRA_HW_ADJ_NIF *info
  )
{
  uint32 ind=0;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf(" Mal_nof_entries:  %u[Entries]\n\r",info->mal_nof_entries);
  for (ind=0; ind<SOC_PETRA_NOF_MAC_LANES; ++ind)
  {
    soc_sand_os_printf(" Mal[%u]: ",ind);
    soc_petra_PETRA_HW_ADJ_MAL_print(&(info->mal[ind]));
  }
  soc_sand_os_printf(" Spaui_nof_entries: %u[Entries]\n\r",info->spaui_nof_entries);
  for (ind=0; ind<SOC_PETRA_NOF_MAC_LANES; ++ind)
  {
    soc_sand_os_printf(" Spaui[%u]: ",ind);
    soc_petra_PETRA_HW_ADJ_SPAUI_print(&(info->spaui[ind]));
  }
  soc_sand_os_printf(" Sgmii_nof_entries: %u[Entries]\n\r",info->sgmii_nof_entries);
  for (ind=0; ind<SOC_PETRA_IF_NOF_NIFS; ++ind)
  {
    soc_sand_os_printf(" Sgmii[%u]: ",ind);
    soc_petra_PETRA_HW_ADJ_SGMII_print(&(info->sgmii[ind]));
  }
  soc_sand_os_printf(" Fat_pipe_enable:   %u\n\r",info->fat_pipe_enable);
  soc_sand_os_printf(" Fat_pipe: ");
  soc_petra_PETRA_HW_ADJ_FAT_PIPE_print(&(info->fat_pipe));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_HW_ADJUSTMENTS_print(
    SOC_SAND_IN SOC_PETRA_HW_ADJUSTMENTS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("Dram:\n\r");
  soc_petra_PETRA_HW_ADJ_DDR_print(&(info->dram));
  soc_sand_os_printf("Qdr:\n\r");
  soc_petra_PETRA_HW_ADJ_QDR_print(&(info->qdr));
  soc_sand_os_printf("Serdes:\n\r");
  soc_petra_PETRA_HW_ADJ_SERDES_print(&(info->serdes));
  soc_sand_os_printf("Fabric:\n\r");
  soc_petra_PETRA_HW_ADJ_FABRIC_print(&(info->fabric));
  soc_sand_os_printf("Nif:\n\r");
  soc_petra_PETRA_HW_ADJ_NIF_print(&(info->nif));
  soc_sand_os_printf("Core_freq:");
  soc_petra_PETRA_HW_ADJ_CORE_FREQ_print(&(info->core_freq));
  soc_sand_os_printf("Stat_if:");
  soc_petra_PETRA_HW_ADJ_STAT_IF_print(&(info->stat_if));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_INIT_STAG_INFO_print(
    SOC_SAND_IN SOC_PETRA_INIT_STAG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf(" Fld_conf:\n\r");
  soc_petra_PETRA_IHP_PCKT_STAG_HDR_DATA_print(&(info->fld_conf));
  soc_sand_os_printf(" Encoding:\n\r");
  soc_petra_PETRA_ITM_STAG_INFO_print(&(info->encoding));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_INIT_BASIC_CONF_print(
    SOC_SAND_IN SOC_PETRA_INIT_BASIC_CONF *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("Credit_worth_enable: %u\n\r",info->credit_worth_enable);
  soc_sand_os_printf("Credit_worth:        %u[Bytes]\n\r",info->credit_worth);
  soc_sand_os_printf("Stag_enable:         %u\n\r",info->stag_enable);
  soc_sand_os_printf("Stag:\n\r");
  soc_petra_PETRA_INIT_STAG_INFO_print(&(info->stag));
  soc_sand_os_printf("Mem_shadow_enable: %u\n\r",info->mem_shadow_enable);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_INIT_PORT_HDR_TYPE_print(
    SOC_SAND_IN SOC_PETRA_INIT_PORT_HDR_TYPE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf(" Port_ndx: %u\n\r",info->port_ndx);
  soc_sand_os_printf(
    " header_type_in: %s \n\r",
    soc_petra_PETRA_PORT_HEADER_TYPE_to_string(info->header_type_in)
  );
  soc_sand_os_printf(
    " header_type_out: %s \n\r",
    soc_petra_PETRA_PORT_HEADER_TYPE_to_string(info->header_type_out)
  );
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_INIT_PORT_TO_IF_MAP_print(
    SOC_SAND_IN SOC_PETRA_INIT_PORT_TO_IF_MAP *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf(" Port_ndx: %u\n\r",info->port_ndx);
  soc_sand_os_printf(" Conf:\n\r");
  soc_petra_PETRA_PORT2IF_MAPPING_INFO_print(&(info->conf));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_INIT_PCKT_HDR_INFO_print(
    SOC_SAND_IN SOC_PETRA_INIT_PCKT_HDR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf(" Port_ndx: %u\n\r",info->port_ndx);
  soc_sand_os_printf(" Conf:\n\r");
  soc_petra_PETRA_IHP_PORT_INFO_print(&(info->conf));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_INIT_PORTS_print(
    SOC_SAND_IN SOC_PETRA_INIT_PORTS *info
  )
{
  uint32 ind=0;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("Hdr_type_nof_entries:        %u[Entries]\n\r",info->hdr_type_nof_entries);
  for (ind=0; ind<SOC_PETRA_NOF_FAP_PORTS; ++ind)
  {
    soc_sand_os_printf("Hdr_type[%2u]:\n\r",ind);
    soc_petra_PETRA_INIT_PORT_HDR_TYPE_print(&(info->hdr_type[ind]));
  }
  soc_sand_os_printf("If_map_nof_entries:          %u[Entries]\n\r",info->if_map_nof_entries);
  for (ind=0; ind<SOC_PETRA_NOF_FAP_PORTS; ++ind)
  {
    soc_sand_os_printf("If_map[%2u]:\n\r",ind);
    soc_petra_PETRA_INIT_PORT_TO_IF_MAP_print(&(info->if_map[ind]));
  }
  soc_sand_os_printf("Packet_hdr_info_nof_entries: %u[Entries]\n\r",info->packet_hdr_info_nof_entries);
  for (ind=0; ind<SOC_PETRA_NOF_FAP_PORTS; ++ind)
  {
    soc_sand_os_printf("Packet_hdr_info[%2u]:\n\r",ind);
    soc_petra_PETRA_INIT_PCKT_HDR_INFO_print(&(info->packet_hdr_info[ind]));
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_MGMT_SRD_REF_CLKS_print(
    SOC_SAND_IN SOC_PETRA_MGMT_SRD_REF_CLKS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  /* The macro SOC_PETRA_MGMT_IS_SRD_REF_CLK_ENUM may be used for signed varible also */
  /* coverity[unsigned_compare : FALSE] */
  if (SOC_PETRA_MGMT_IS_SRD_REF_CLK_ENUM(info->nif_ref_clk))
  {
    soc_sand_os_printf(
      " Nif_ref_clk:                  %s \n\r",
      soc_petra_PETRA_MGMT_SRD_REF_CLK_to_string(info->nif_ref_clk)
    );
  }
  else
  {
     soc_sand_os_printf(
      " Nif_ref_clk:                  %u.%02u MHz\n\r",
      info->nif_ref_clk/1000, info->nif_ref_clk%1000
    );
  }

  /* The macro SOC_PETRA_MGMT_IS_SRD_REF_CLK_ENUM may be used for signed varible also */
  /* coverity[unsigned_compare : FALSE] */
  if (SOC_PETRA_MGMT_IS_SRD_REF_CLK_ENUM(info->fabric_ref_clk))
  {
    soc_sand_os_printf(
      " Fabric_ref_clk:               %s \n\r",
      soc_petra_PETRA_MGMT_SRD_REF_CLK_to_string(info->fabric_ref_clk)
    );
  }
  else
  {
     soc_sand_os_printf(
      " Fabric_ref_clk:               %u.%02u MHz\n\r",
      info->fabric_ref_clk/1000, info->fabric_ref_clk%1000
    );
  }

  /* The macro SOC_PETRA_MGMT_IS_SRD_REF_CLK_ENUM may be used for signed varible also */
  /* coverity[unsigned_compare : FALSE] */
  if (SOC_PETRA_MGMT_IS_SRD_REF_CLK_ENUM(info->nif_ref_clk))
  {
    soc_sand_os_printf(
      " Combo_ref_clk:                %s \n\r",
      soc_petra_PETRA_MGMT_SRD_REF_CLK_to_string(info->combo_ref_clk)
    );
  }
  else
  {
     soc_sand_os_printf(
      " Combo_ref_clk:                 %u.%02u MHz\n\r",
      info->combo_ref_clk/1000, info->combo_ref_clk%1000
    );
  }

exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_MGMT_PCKT_SIZE_print(
    SOC_SAND_IN SOC_PETRA_MGMT_PCKT_SIZE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_TMC_MGMT_PCKT_SIZE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_MGMT_OPERATION_MODE_print(
    SOC_SAND_IN SOC_PETRA_MGMT_OPERATION_MODE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf(
    "Device_sub_type:               %s \n\r",
    soc_petra_PETRA_REVISION_SUB_TYPE_ID_to_string(info->device_sub_type)
  );
  soc_sand_os_printf("Pp_enable:                     %u\n\r",info->pp_enable);
  soc_sand_os_printf("Is_fap20_in_system:            %u\n\r",info->is_fap20_in_system);
  soc_sand_os_printf("Is_fap21_in_system:            %u\n\r",info->is_fap21_in_system);
  soc_sand_os_printf("Is_a1_or_below_in_system:            %u\n\r",info->is_a1_or_below_in_system);
  soc_sand_os_printf("Is_fe200_fabric:               %u\n\r",info->is_fe200_fabric);
  soc_sand_os_printf("Fabric_cell_format: ");
  soc_petra_PETRA_FABRIC_CELL_FORMAT_print(&(info->fabric_cell_format));
  soc_sand_os_printf("is_combo_nif: %u %u\n\r",info->is_combo_nif[0], info->is_combo_nif[1]);
  soc_sand_os_printf("Ref_clocks_conf:\n\r");
  soc_petra_PETRA_MGMT_SRD_REF_CLKS_print(&(info->ref_clocks_conf));
  soc_sand_os_printf("Stag_enable:                   %u\n\r",info->stag_enable);
  soc_sand_os_printf("Egr_mc_16k_groups_enable:      %u\n\r",info->egr_mc_16k_groups_enable);
  soc_sand_os_printf("Tdm_mode: %s ", soc_petra_PETRA_MGMT_TDM_MODE_to_string(info->tdm_mode));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  soc_petra_PETRA_INIT_OOR_print(
    SOC_SAND_IN SOC_PETRA_INIT_OOR *info
  )
{
  uint32 ind=0;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind=0; ind<SOC_PETRA_NOF_MAC_LANES; ++ind)
  {
    soc_sand_os_printf("Mal[%u]: %u\n\r",ind,info->mal[ind]);
  }
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PETRA_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

