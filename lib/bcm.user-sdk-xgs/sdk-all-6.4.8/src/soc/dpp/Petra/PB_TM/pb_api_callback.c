/* $Id: soc_pb_api_callback.c,v 1.5 Broadcom SDK $
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

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_params.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>
#include <soc/dpp/SAND/Management/sand_callback_handles.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/SAND_FM/sand_user_callback.h>

#include <soc/dpp/Petra/PB_TM/pb_callback.h>
#include <soc/dpp/Petra/PB_TM/pb_api_callback.h>
#include <soc/dpp/Petra/PB_TM/pb_interrupt_service.h>

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

uint32
  soc_pb_callback_function_register(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PB_INTERRUPT_CAUSE        interrupt_cause,
    SOC_SAND_IN  SOC_SAND_USER_CALLBACK        *user_callback_structure,
    SOC_SAND_OUT uint32                  *callback_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(user_callback_structure);
  SOC_SAND_CHECK_NULL_INPUT(callback_id);


  if (user_callback_structure->interrupt_not_poll)
  {
    SOC_SAND_TAKE_DEVICE_SEMAPHORE;
  }

  res = soc_pb_callback_function_register_unsafe(
          unit,
          interrupt_cause,
          user_callback_structure,
          callback_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_semaphore);

exit_semaphore:
  if (user_callback_structure->interrupt_not_poll)
  {
    SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_callback_function_register()",0,0);
}

uint32
  soc_pb_callback_function_unregister(
    SOC_SAND_IN  uint32                 driver_callback_id
  )
{
  uint32
    res = SOC_SAND_OK;
    
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if ( !soc_sand_general_get_driver_is_started() )
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_DRIVER_NOT_STARTED, 1, exit);
  }

  res = soc_sand_handles_unregister_handle(
          driver_callback_id
        );
  
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  res = soc_pb_interrupt_specific_cause_monitor_stop_unsafe(
          ((SOC_SAND_CALLBACK_HANDLE*)(INT_TO_PTR(driver_callback_id)))->unit,
          ((SOC_SAND_CALLBACK_HANDLE*)(INT_TO_PTR(driver_callback_id)))->soc_sand_interrupt_handle
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_callback_function_register()",0,0);
}

uint32
  soc_pb_callback_all_functions_unregister(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  /*
   * Take mutexes in the right order -> delta_list, device, rest of them
   */
  if (SOC_SAND_OK != soc_sand_tcm_callback_delta_list_take_mutex())
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_SEM_TAKE_FAIL, 2, exit);
  }

  if (SOC_SAND_OK != soc_sand_take_chip_descriptor_mutex(unit))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_SEM_TAKE_FAIL, 3, exit_tcm_delta_mutex);
  }

  res = soc_pb_callback_all_functions_unregister_unsafe(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit_chip_descriptor_mutex);

exit_chip_descriptor_mutex:
  if (SOC_SAND_OK != soc_sand_give_chip_descriptor_mutex(unit))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_SEM_GIVE_FAIL, 6, exit_tcm_delta_mutex);
  }

exit_tcm_delta_mutex:
  if (SOC_SAND_OK != soc_sand_tcm_callback_delta_list_give_mutex())
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_SEM_GIVE_FAIL, 7, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_callback_all_functions_unregister()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>
