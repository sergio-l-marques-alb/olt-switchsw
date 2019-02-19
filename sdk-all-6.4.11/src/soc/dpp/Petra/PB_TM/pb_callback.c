/* $Id: pb_callback.c,v 1.8 Broadcom SDK $
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

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Management/sand_callback_handles.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>

#include <soc/dpp/Petra/petra_api_reg_access.h>
#include <soc/dpp/Petra/PB_TM/pb_api_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_interrupt_service.h>
#include <soc/dpp/Petra/PB_TM/pb_api_interrupt_service.h>
#include <soc/dpp/Petra/petra_general.h>
#include <soc/dpp/Petra/PB_TM/pb_callback.h>
#include <soc/dpp/Petra/PB_TM/pb_chip_regs.h>
#include <soc/dpp/Petra/PB_TM/pb_reg_access.h>

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
  soc_pb_callback_user_callback_function(
    SOC_SAND_OUT uint32   *buffer,
    SOC_SAND_IN  uint32   size
  )
{
  uint32
    orig_val,
    res = SOC_SAND_OK;
  uint32
    *new_buf;
  int unit;
  SOC_PB_USER_CALLBACK_STRUCT
    *callback_struct = (SOC_PB_USER_CALLBACK_STRUCT*)buffer;
  SOC_PB_INTERRUPT_SINGLE_STRUCT
    value;
  SOC_PB_INTERRUPT_MONITORED_CAUSE
    monitored_cause;
  uint8
    instance,
    nof_instances;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CALLBACK_USER_CALLBACK_FUNCTION);

  unit = callback_struct->unit;
  monitored_cause = soc_pb_interrupt_cause2monitored_cause(unit, callback_struct->interrupt_cause); 

  SOC_SAND_ERR_IF_ABOVE_MAX(monitored_cause, (SOC_PB_INTERRUPT_MAX_MONITORED_CAUSES-1), SOC_PETRA_INTERRUPT_INVALID_MONITORED_CAUSE_ERR, 10, exit);
  
  res = soc_pb_interrupt_monitored_cause_nof_instances_get(
          unit, 
          callback_struct->interrupt_cause, 
          &nof_instances
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);

  if(nof_instances == 1)
  {
    /*
     * These is not a multi-instance interrupt
     */ 
    res = soc_pb_interrupt_single_clear_unsafe(
            unit,
            monitored_cause,
            &value
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

    callback_struct->user_callback_func(
      callback_struct->user_callback_id,
      (uint32*)(&value),
      &new_buf,
      0,
      ex,
      callback_struct->unit,
      callback_struct->soc_sand_tcm_callback_id,
      0,
      0
    );
  }
  else
  {
    /*
     * These is a multi-instance interrupt.
     * Call the callback function for each instance for which the interrupt occured.
     */ 
    for(instance = 0; instance < nof_instances; ++instance)
    {
      res = soc_pb_interrupt_single_clear_with_instance_unsafe(
              unit,
              monitored_cause,
              instance,
              &orig_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      if(orig_val != 0)
      {
        /*
         * The interrupt of the specific instance occurred.
         * The instance index is provided to the callback as a parameter (following the callbaack_id parameter)
         */
        callback_struct->user_callback_func(
        callback_struct->user_callback_id,
          (uint32*)(&value),
          &new_buf,
          0,
          ex,
          callback_struct->unit,
          callback_struct->soc_sand_tcm_callback_id,
          instance,
          0
        );
      }
    }
  }

/* Clear interrupt */
  res = soc_pb_interrupt_clear_unsafe(
          unit,
          soc_sand_get_chip_descriptor_interrupt_bitstream(unit)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_callback_user_callback_function()",0,0);
}

uint32
  soc_pb_callback_function_register_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PB_INTERRUPT_CAUSE     interrupt_cause,
    SOC_SAND_IN  SOC_SAND_USER_CALLBACK        *user_callback_structure,
    SOC_SAND_OUT uint32                  *callback_id
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    callback_struct_size;
  uint32
    *callback_struct = NULL;
  uint32
    callback_engine_private_handle ;
  uint32
    callback_exist = FALSE;
  SOC_SAND_TCM_CALLBACK
    callback_func;
  SOC_PB_USER_CALLBACK_STRUCT
    *user_callback_struct = NULL;
  SOC_PB_INTERRUPT_MONITORED_CAUSE
    monitored_cause;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CALLBACK_FUNCTION_REGISTER_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(user_callback_structure);
  SOC_SAND_CHECK_NULL_INPUT(callback_id);

  if (callback_exist)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_CALLBACK_ALREADY_REGISTERED_ERR, 20, exit);
  }

  res = soc_pb_interrupt_specific_cause_monitor_start_unsafe(
          unit,
          interrupt_cause,
          &monitored_cause
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  /* No need to Free. Will be freed upon the call of soc_pb_callback_function_unregister */
  SOC_PETRA_ALLOC(user_callback_struct, SOC_PB_USER_CALLBACK_STRUCT, 1);

  user_callback_struct->unit          = unit;
  user_callback_struct->user_callback_func = user_callback_structure->callback_func;
  user_callback_struct->user_callback_id   = user_callback_structure->user_callback_id;
  user_callback_struct->interrupt_cause    = interrupt_cause;

  callback_func        = soc_pb_callback_user_callback_function;
  callback_struct_size = sizeof(SOC_PB_USER_CALLBACK_STRUCT);
  callback_struct      = (uint32*)user_callback_struct;

  if (user_callback_structure->interrupt_not_poll)
  {
    /* Interrupt activation. { */
    ret = soc_sand_register_event_callback(
            callback_func,
            callback_struct,
            callback_struct_size,
            unit,
            monitored_cause,
            &callback_engine_private_handle
          );
    SOC_SAND_CHECK_FUNC_RESULT(ret, 30, exit);
    /* } */

    /* unmask the specific bit at the device { */
    res = soc_pb_interrupt_monitored_cause_mask_bit_set_unsafe(
            unit,
            monitored_cause,
            FALSE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    /* } */
  }
  else
  {
    ret = soc_sand_tcm_register_polling_callback(
            callback_func,
            callback_struct,
            callback_struct_size,
            SOC_SAND_POLLING,
            user_callback_structure->callback_rate,
            SOC_SAND_INFINITE_POLLING,
            &callback_engine_private_handle
          );
    SOC_SAND_CHECK_FUNC_RESULT(ret, 50, exit);
  }

  ret = soc_sand_handles_register_handle(
          (user_callback_structure->interrupt_not_poll ? SOC_SAND_BIT(1) : SOC_SAND_BIT(0)),
          unit,
          SOC_PB_CALLBACK_USER_CALLBACK_FUNCTION,
          callback_engine_private_handle,
          monitored_cause,
          callback_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(ret, 60, exit);

  user_callback_struct = ((SOC_PB_USER_CALLBACK_STRUCT*)callback_struct);
  user_callback_struct->soc_sand_tcm_callback_id = *callback_id;

   /* After the success of the registration we set this pointer to NULL in oredr it will not be freed with SOC_PETRA_FREE in the exit
    * In case of failure we would like that the allocation will be freed.
    */
  user_callback_struct = NULL;

exit:
  SOC_PETRA_FREE(user_callback_struct);
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_register_callback_function_unsafe()",0,0);
}

uint32
  soc_pb_callback_all_functions_unregister_unsafe(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CALLBACK_ALL_FUNCTIONS_UNREGISTER_UNSAFE);

  res = soc_sand_handles_unregister_all_device_handles(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  res = soc_pb_interrupt_all_specific_cause_monitor_stop_unsafe(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_callback_all_functions_unregister_unsafe()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>
