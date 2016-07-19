/* $Id: pb_pmf_low_level_diag.c,v 1.8 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_TM/src/soc_pb_api_pmf_low_level_diag.c
*
* MODULE PREFIX:  soc_pb_pmf
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
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/Petra/PB_TM/pb_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level_diag.h>
#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level_diag_intern.h>

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
*     Force PMF to execute the given program.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_diag_force_prog_set(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                 prog_id,
    SOC_SAND_IN  uint8                enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_DIAG_FORCE_PROG_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_pb_pmf_diag_force_prog_set_verify(
          unit,
          prog_id,
          enable
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pmf_diag_force_prog_set_unsafe(
          unit,
          prog_id,
          enable
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_diag_force_prog_set()", 0, 0);
}

/*********************************************************************
*     Force PMF to execute the given program.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_diag_force_prog_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_OUT uint32                 *prog_id,
    SOC_SAND_OUT uint8                *enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_DIAG_FORCE_PROG_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(prog_id);
  SOC_SAND_CHECK_NULL_INPUT(enable);

  res = soc_pb_pmf_diag_force_prog_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pmf_diag_force_prog_get_unsafe(
          unit,
          prog_id,
          enable
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_diag_force_prog_get()", 0, 0);
}

/*********************************************************************
*     Returns used programs for all packets since last call.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_diag_selected_progs_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_OUT uint32                 *progs_bmp
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_DIAG_SELECTED_PROGS_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(progs_bmp);

  res = soc_pb_pmf_diag_selected_progs_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pmf_diag_selected_progs_get_unsafe(
          unit,
          progs_bmp
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_diag_selected_progs_get()", 0, 0);
}

/*********************************************************************
*     Force PMF to perform the given action.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_diag_force_action_set(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  SOC_PB_PMF_DIAG_ACTION_INFO  *action_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_DIAG_FORCE_ACTION_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(action_info);

  res = soc_pb_pmf_diag_force_action_set_verify(
          unit,
          action_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pmf_diag_force_action_set_unsafe(
          unit,
          action_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_diag_force_action_set()", 0, 0);
}

/*********************************************************************
*     Force PMF to perform the given action.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_diag_force_action_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_OUT SOC_PB_PMF_DIAG_ACTION_INFO  *action_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_DIAG_FORCE_ACTION_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(action_info);

  res = soc_pb_pmf_diag_force_action_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pmf_diag_force_action_get_unsafe(
          unit,
          action_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_diag_force_action_get()", 0, 0);
}

/*********************************************************************
*     Returns the keys built by the PMF for the last packet
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_diag_built_keys_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_PB_PMF_DIAG_KEYS_INFO       *built_keys
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_DIAG_BUILT_KEYS_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(built_keys);

  res = soc_pb_pmf_diag_built_keys_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pmf_diag_built_keys_get_unsafe(
          unit,
          built_keys
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_diag_built_keys_get()", 0, 0);
}

/*********************************************************************
*     Set pre-configuration so freeze inputs/outputs will be
 *     snapshoot according to this information
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_diag_fem_freeze_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_PMF_DIAG_FEM_FREEZE_INFO *freeze_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_DIAG_FEM_FREEZE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(freeze_info);

  res = soc_pb_pmf_diag_fem_freeze_set_verify(
          unit,
          freeze_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pmf_diag_fem_freeze_set_unsafe(
          unit,
          freeze_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_diag_fem_freeze_set()", 0, 0);
}

/*********************************************************************
*     Set pre-configuration so freeze inputs/outputs will be
 *     snapshoot according to this information
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_diag_fem_freeze_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_PB_PMF_DIAG_FEM_FREEZE_INFO *freeze_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_DIAG_FEM_FREEZE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(freeze_info);

  res = soc_pb_pmf_diag_fem_freeze_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pmf_diag_fem_freeze_get_unsafe(
          unit,
          freeze_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_diag_fem_freeze_get()", 0, 0);
}

/*********************************************************************
*     Get the FEM information including inputs and outputs to
 *     each FEM,
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pmf_diag_fems_info_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint8                   release,
    SOC_SAND_OUT SOC_PB_PMF_DIAG_FEM_INFO        fems_info[8]
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PMF_DIAG_FEMS_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(fems_info);

  res = soc_pb_pmf_diag_fems_info_get_verify(
          unit,
          release
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pmf_diag_fems_info_get_unsafe(
          unit,
          release,
          fems_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pmf_diag_fems_info_get()", 0, 0);
}

void
  SOC_PB_PMF_DIAG_BUFFER_clear(
    SOC_SAND_OUT SOC_PB_PMF_DIAG_BUFFER *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_PMF_DIAG_BUFFER));
  for (ind = 0; ind < SOC_PB_PMF_DIAG_BUFF_MAX_SIZE; ++ind)
  {
    info->buff[ind] = 0;
  }
  info->buff_len = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_DIAG_FORCE_ACTION_VAL_clear(
    SOC_SAND_OUT SOC_PB_PMF_DIAG_FORCE_ACTION_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_PMF_DIAG_FORCE_ACTION_VAL));
  info->type = SOC_PB_PMF_DIAG_FORCE_NOP;
  info->value = 0;
  info->is_valid = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_DIAG_ACTION_VAL_clear(
    SOC_SAND_OUT SOC_PB_PMF_DIAG_ACTION_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_PMF_DIAG_ACTION_VAL));
  info->type = SOC_PB_PMF_FEM_ACTION_TYPE_NOP;
  info->value = 0;
  info->is_valid = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_DIAG_KEYS_INFO_clear(
    SOC_SAND_OUT SOC_PB_PMF_DIAG_KEYS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_PMF_DIAG_KEYS_INFO));
  SOC_PB_PMF_DIAG_BUFFER_clear(&(info->eth));
  SOC_PB_PMF_DIAG_BUFFER_clear(&(info->ipv4));
  SOC_PB_PMF_DIAG_BUFFER_clear(&(info->ipv6));
  SOC_PB_PMF_DIAG_BUFFER_clear(&(info->custom_a));
  SOC_PB_PMF_DIAG_BUFFER_clear(&(info->custom_b));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_DIAG_ACTION_INFO_clear(
    SOC_SAND_OUT SOC_PB_PMF_DIAG_ACTION_INFO *info
  )
{
  uint32
    ind;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_PMF_DIAG_ACTION_INFO));
  for (ind = 0; ind < 4; ++ind)
  {
    SOC_PB_PMF_DIAG_FORCE_ACTION_VAL_clear(&(info->actions[ind]));
  }
  info->strength = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_DIAG_FEM_FREEZE_INFO_clear(
    SOC_SAND_OUT SOC_PB_PMF_DIAG_FEM_FREEZE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_PMF_DIAG_FEM_FREEZE_INFO));
  info->freeze = 0;
  info->pass_num = 0;
  info->prg = 0;
  info->is_prg_valid = 0;
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_DIAG_FEM_INFO_clear(
    SOC_SAND_OUT SOC_PB_PMF_DIAG_FEM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  sal_memset(info, 0x0, sizeof(SOC_PB_PMF_DIAG_FEM_INFO));
  info->in_key = 0;
  info->in_prg = 0;
  SOC_PB_PMF_DIAG_ACTION_VAL_clear(&(info->out_action));
  SOC_SAND_MAGIC_NUM_SET;
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PB_DEBUG_IS_LVL1

const char*
  SOC_PB_PMF_DIAG_FORCE_to_string(
    SOC_SAND_IN  SOC_PB_PMF_DIAG_FORCE enum_val
  )
{
  const char* str = NULL;
  switch(enum_val)
  {
  case SOC_PB_PMF_DIAG_FORCE_DEST:
    str = "dest";
  break;
  case SOC_PB_PMF_DIAG_FORCE_TC:
    str = "tc";
  break;
  case SOC_PB_PMF_DIAG_FORCE_DP:
    str = "dp";
  break;
  case SOC_PB_PMF_DIAG_FORCE_INVALID:
    str = "invalid";
  break;
  case SOC_PB_PMF_DIAG_FORCE_DP_METER:
    str = "dp_meter";
  break;
  case SOC_PB_PMF_DIAG_FORCE_FWD_STRENGTH:
    str = "fwd_strength";
  break;
  case SOC_PB_PMF_DIAG_FORCE_METER_A:
    str = "meter_a";
  break;
  case SOC_PB_PMF_DIAG_FORCE_METER_B:
    str = "meter_b";
  break;
  case SOC_PB_PMF_DIAG_FORCE_COUNTER_A:
    str = "counter_a";
  break;
  case SOC_PB_PMF_DIAG_FORCE_COUNTER_B:
    str = "counter_b";
  break;
  case SOC_PB_PMF_DIAG_FORCE_CPU_TRAP_CODE:
    str = "cpu_trap_code";
  break;
  case SOC_PB_PMF_DIAG_FORCE_SNOOP_CODE:
    str = "snoop_code";
  break;
  case SOC_PB_PMF_DIAG_FORCE_OUTBOUND_MIRROR_DIS:
    str = "outbound_mirror_dis";
  break;
  case SOC_PB_PMF_DIAG_FORCE_EXCLUDE_SRC:
    str = "exclude_src";
  break;
  case SOC_PB_PMF_DIAG_FORCE_INGRESS_SHAPING:
    str = "ingress_shaping";
  break;
  case SOC_PB_PMF_DIAG_FORCE_MIRROR_ACTION:
    str = "mirror_action";
  break;
  case SOC_PB_PMF_DIAG_FORCE_BYTES_TO_REMOVE:
    str = "bytes_to_remove";
  break;
  case SOC_PB_PMF_DIAG_FORCE_HEADER_PROFILE_NDX:
    str = "header_profile_ndx";
  break;
  case SOC_PB_PMF_DIAG_FORCE_SEQ_NUM_TAG:
    str = "seq_num_tag";
  break;
  case SOC_PB_PMF_DIAG_FORCE_STATISTICS_TAG:
    str = "statistics_tag";
  break;
  case SOC_PB_PMF_DIAG_FORCE_LB_KEY:
    str = "lb_key";
  break;
  case SOC_PB_PMF_DIAG_FORCE_ECMP_KEY:
    str = "ecmp_key";
  break;
  case SOC_PB_PMF_DIAG_FORCE_STACKING_ROUTE:
    str = "stacking_route";
  break;
  case SOC_PB_PMF_DIAG_FORCE_OUTLIF:
    str = "outlif";
  break;
  case SOC_PB_PMF_DIAG_FORCE_NOP:
    str = "nop";
  break;
  default:
    str = " Unknown";
  }
  return str;
}

void
  SOC_PB_PMF_DIAG_BUFFER_print(
    SOC_SAND_IN  SOC_PB_PMF_DIAG_BUFFER *info
  )
{
  uint32
    ind,
    len;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  len = (info->buff_len+31)/32;
  for (ind = 0; ind < len; ++ind)
  {
    soc_sand_os_printf("0x%08x   ",info->buff[len - ind - 1]);
    if ((ind+1) % 4 == 0)
    {
      soc_sand_os_printf("\n\r");
    }
  }
  soc_sand_os_printf("\n\r buff_len: %u bits\n\r",info->buff_len);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_DIAG_FORCE_ACTION_VAL_print(
    SOC_SAND_IN  SOC_PB_PMF_DIAG_FORCE_ACTION_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("type %s   ", SOC_PB_PMF_DIAG_FORCE_to_string(info->type));
  soc_sand_os_printf("value: %u  ",info->value);
  soc_sand_os_printf("is_valid: %u  ",info->is_valid);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  SOC_PB_PMF_DIAG_ACTION_VAL_print(
    SOC_SAND_IN  SOC_PB_PMF_DIAG_ACTION_VAL *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("type %s ", SOC_PB_PMF_FEM_ACTION_TYPE_to_string(info->type));
  soc_sand_os_printf("value: %u  ",info->value);
  soc_sand_os_printf("is_valid: %u   ",info->is_valid);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_DIAG_KEYS_INFO_print(
    SOC_SAND_IN  SOC_PB_PMF_DIAG_KEYS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("eth:\n\r");
  SOC_PB_PMF_DIAG_BUFFER_print(&(info->eth));
  soc_sand_os_printf("\n\ripv4:\n\r");
  SOC_PB_PMF_DIAG_BUFFER_print(&(info->ipv4));
  soc_sand_os_printf("\n\ripv6:\n\r");
  SOC_PB_PMF_DIAG_BUFFER_print(&(info->ipv6));
  soc_sand_os_printf("\n\rcustom_a:\n\r");
  SOC_PB_PMF_DIAG_BUFFER_print(&(info->custom_a));
  soc_sand_os_printf("\n\rcustom_b:\n\r");
  SOC_PB_PMF_DIAG_BUFFER_print(&(info->custom_b));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_DIAG_ACTION_INFO_print(
    SOC_SAND_IN  SOC_PB_PMF_DIAG_ACTION_INFO *info
  )
{
  uint32
    ind;
  uint8
    valid_action=FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < 4; ++ind)
  {
    if (info->actions[ind].is_valid)
    {
      valid_action = TRUE;
      soc_sand_os_printf("actions[%u]:",ind);
      SOC_PB_PMF_DIAG_FORCE_ACTION_VAL_print(&(info->actions[ind]));
    }
  }
  if (valid_action)
  {
    soc_sand_os_printf("fwd_strength: %u\n\r",info->strength);
  }
  else
  {
    soc_sand_os_printf("No Action is Forced.\n\r");
  }
  
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_DIAG_FEM_FREEZE_INFO_print(
    SOC_SAND_IN  SOC_PB_PMF_DIAG_FEM_FREEZE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("freeze: %u\n\r",info->freeze);
  soc_sand_os_printf("pass_num: %u\n\r",info->pass_num);
  soc_sand_os_printf("prg: %u\n\r",info->prg);
  soc_sand_os_printf("is_prg_valid: %u\n\r",info->is_prg_valid);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  SOC_PB_PMF_DIAG_FEM_INFO_print(
    SOC_SAND_IN  SOC_PB_PMF_DIAG_FEM_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_sand_os_printf("in_key: 0x%08x  ",info->in_key);
  soc_sand_os_printf("in_prg: %u\n\r",info->in_prg);
  soc_sand_os_printf("out_action:");
  SOC_PB_PMF_DIAG_ACTION_VAL_print(&(info->out_action));
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PB_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

