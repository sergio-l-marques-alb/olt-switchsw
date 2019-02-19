#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* $Id: arad_pp_api_trap_mgmt.c,v 1.7 Broadcom SDK $
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
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_TRAP

/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_trap_mgmt.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_trap_mgmt.h>

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
*     Set forwarding action profile information.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_trap_frwrd_profile_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  ARAD_PP_TRAP_CODE                               trap_code_ndx,
    SOC_SAND_IN  ARAD_PP_TRAP_FRWRD_ACTION_PROFILE_INFO          *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_FRWRD_PROFILE_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  res = arad_pp_trap_frwrd_profile_info_set_verify(
          unit,
          trap_code_ndx,
          profile_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_trap_frwrd_profile_info_set_unsafe(
          unit,
          trap_code_ndx,
          profile_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_trap_frwrd_profile_info_set()", 0, 0);
}

/*********************************************************************
*     Set forwarding action profile information.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_trap_frwrd_profile_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  ARAD_PP_TRAP_CODE                               trap_code_ndx,
    SOC_SAND_OUT ARAD_PP_TRAP_FRWRD_ACTION_PROFILE_INFO          *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_FRWRD_PROFILE_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  res = arad_pp_trap_frwrd_profile_info_get_verify(
          unit,
          trap_code_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_trap_frwrd_profile_info_get_unsafe(
          unit,
          trap_code_ndx,
          profile_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_trap_frwrd_profile_info_get()", 0, 0);
}

/*********************************************************************
*     Set snoop action profile information.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_trap_snoop_profile_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  ARAD_PP_TRAP_CODE                               trap_code_ndx,
    SOC_SAND_IN  ARAD_PP_TRAP_SNOOP_ACTION_PROFILE_INFO          *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_SNOOP_PROFILE_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  res = arad_pp_trap_snoop_profile_info_set_verify(
          unit,
          trap_code_ndx,
          profile_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_trap_snoop_profile_info_set_unsafe(
          unit,
          trap_code_ndx,
          profile_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_trap_snoop_profile_info_set()", 0, 0);
}

/*********************************************************************
*     Set snoop action profile information.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_trap_snoop_profile_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  ARAD_PP_TRAP_CODE                               trap_code_ndx,
    SOC_SAND_OUT ARAD_PP_TRAP_SNOOP_ACTION_PROFILE_INFO          *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_SNOOP_PROFILE_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  res = arad_pp_trap_snoop_profile_info_get_verify(
          unit,
          trap_code_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_trap_snoop_profile_info_get_unsafe(
          unit,
          trap_code_ndx,
          profile_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_trap_snoop_profile_info_get()", 0, 0);
}

/*********************************************************************
*     Maps egress trap type to egress action profile.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_trap_to_eg_action_map_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      trap_type_bitmap_ndx,
    SOC_SAND_IN  uint32                                      eg_action_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_TO_EG_ACTION_MAP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = arad_pp_trap_to_eg_action_map_set_verify(
          unit,
          trap_type_bitmap_ndx,
          eg_action_profile
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_trap_to_eg_action_map_set_unsafe(
          unit,
          trap_type_bitmap_ndx,
          eg_action_profile
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_trap_to_eg_action_map_set()", 0, 0);
}

/*********************************************************************
*     Maps egress trap type to egress action profile.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_trap_to_eg_action_map_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      trap_type_bitmap_ndx,
    SOC_SAND_OUT uint32                                      *eg_action_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_TO_EG_ACTION_MAP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(eg_action_profile);

  res = arad_pp_trap_to_eg_action_map_get_verify(
          unit,
          trap_type_bitmap_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_trap_to_eg_action_map_get_unsafe(
          unit,
          trap_type_bitmap_ndx,
          eg_action_profile
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_trap_to_eg_action_map_get()", 0, 0);
}

/*********************************************************************
*     Set egress action profile information.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_trap_eg_profile_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      profile_ndx,
    SOC_SAND_IN  ARAD_PP_TRAP_EG_ACTION_PROFILE_INFO             *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_EG_PROFILE_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  res = arad_pp_trap_eg_profile_info_set_verify(
          unit,
          profile_ndx,
          profile_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_trap_eg_profile_info_set_unsafe(
          unit,
          profile_ndx,
          profile_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_trap_eg_profile_info_set()", profile_ndx, 0);
}

/*********************************************************************
*     Set egress action profile information.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_trap_eg_profile_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      profile_ndx,
    SOC_SAND_OUT ARAD_PP_TRAP_EG_ACTION_PROFILE_INFO             *profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_EG_PROFILE_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(profile_info);

  res = arad_pp_trap_eg_profile_info_get_verify(
          unit,
          profile_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_trap_eg_profile_info_get_unsafe(
          unit,
          profile_ndx,
          profile_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_trap_eg_profile_info_get()", profile_ndx, 0);
}

/*********************************************************************
*     Read MACT event from the events FIFO into buffer.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_trap_mact_event_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_OUT uint32                                      buff[ARAD_PP_TRAP_EVENT_BUFF_MAX_SIZE],
    SOC_SAND_OUT uint32                                      *buff_len
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_MACT_EVENT_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(buff_len);

  res = arad_pp_trap_mact_event_get_verify(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_trap_mact_event_get_unsafe(
          unit,
          buff,
          buff_len
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_trap_mact_event_get()", 0, 0);
}

/*********************************************************************
*     The MACT may report different events using the event
 *     FIFO (e.g., learn, age, transplant, and retrieve). This
 *     API Parses the event buffer into a meaningful structure.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_trap_mact_event_parse(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      buff[ARAD_PP_TRAP_EVENT_BUFF_MAX_SIZE],
    SOC_SAND_IN  uint32                                      buff_len,
    SOC_SAND_OUT ARAD_PP_TRAP_MACT_EVENT_INFO                    *mact_event
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_TRAP_MACT_EVENT_PARSE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mact_event);

  res = arad_pp_trap_mact_event_parse_verify(
          unit,
          buff,
          buff_len
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_trap_mact_event_parse_unsafe(
          unit,
          buff,
          buff_len,
          mact_event
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_trap_mact_event_parse()", 0, 0);
}

void
  ARAD_PP_TRAP_ACTION_PROFILE_DEST_INFO_clear(
    SOC_SAND_OUT ARAD_PP_TRAP_ACTION_PROFILE_DEST_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_ACTION_PROFILE_DEST_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_TRAP_ACTION_PROFILE_COUNT_INFO_clear(
    SOC_SAND_OUT ARAD_PP_TRAP_ACTION_PROFILE_COUNT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_ACTION_PROFILE_COUNT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_TRAP_ACTION_PROFILE_METER_INFO_clear(
    SOC_SAND_OUT ARAD_PP_TRAP_ACTION_PROFILE_METER_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_ACTION_PROFILE_METER_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_TRAP_ACTION_PROFILE_POLICE_INFO_clear(
    SOC_SAND_OUT ARAD_PP_TRAP_ACTION_PROFILE_POLICE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_ACTION_PROFILE_POLICE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_TRAP_ACTION_PROFILE_COS_INFO_clear(
    SOC_SAND_OUT ARAD_PP_TRAP_ACTION_PROFILE_COS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_ACTION_PROFILE_COS_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_TRAP_ACTION_PROFILE_PROCESS_INFO_clear(
    SOC_SAND_OUT ARAD_PP_TRAP_ACTION_PROFILE_PROCESS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_ACTION_PROFILE_PROCESS_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(
    SOC_SAND_OUT ARAD_PP_TRAP_FRWRD_ACTION_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_TRAP_SNOOP_ACTION_PROFILE_INFO_clear(
    SOC_SAND_OUT ARAD_PP_TRAP_SNOOP_ACTION_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_TRAP_EG_ACTION_PROFILE_INFO_clear(
    SOC_SAND_OUT ARAD_PP_TRAP_EG_ACTION_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_TRAP_MACT_EVENT_INFO_clear(
    SOC_SAND_OUT ARAD_PP_TRAP_MACT_EVENT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_MACT_EVENT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_TRAP_PACKET_INFO_clear(
    SOC_SAND_OUT ARAD_PP_TRAP_PACKET_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_PACKET_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if ARAD_PP_DEBUG_IS_LVL1

const char*
  ARAD_PP_TRAP_SNOOP_ACTION_SIZE_to_string(
    SOC_SAND_IN  ARAD_PP_TRAP_SNOOP_ACTION_SIZE enum_val
  )
{
  return SOC_PPC_TRAP_SNOOP_ACTION_SIZE_to_string(enum_val);
}

const char*
  ARAD_PP_TRAP_ACTION_PROFILE_OVERWRITE_to_string(
    SOC_SAND_IN  ARAD_PP_TRAP_ACTION_PROFILE_OVERWRITE enum_val
  )
{
  return SOC_PPC_TRAP_ACTION_PROFILE_OVERWRITE_to_string(enum_val);
}

const char*
  ARAD_PP_TRAP_MACT_EVENT_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_TRAP_MACT_EVENT_TYPE enum_val
  )
{
  return SOC_PPC_TRAP_MACT_EVENT_TYPE_to_string(enum_val);
}

const char*
  ARAD_PP_TRAP_CODE_to_string(
    SOC_SAND_IN  ARAD_PP_TRAP_CODE enum_val
  )
{
  return SOC_PPC_TRAP_CODE_to_string(enum_val);
}

const char*
  ARAD_PP_TRAP_EG_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_TRAP_EG_TYPE enum_val
  )
{
  return SOC_PPC_TRAP_EG_TYPE_to_string(enum_val);
}

void
  ARAD_PP_TRAP_ACTION_PROFILE_DEST_INFO_print(
    SOC_SAND_IN  ARAD_PP_TRAP_ACTION_PROFILE_DEST_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_ACTION_PROFILE_DEST_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_TRAP_ACTION_PROFILE_COUNT_INFO_print(
    SOC_SAND_IN  ARAD_PP_TRAP_ACTION_PROFILE_COUNT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_ACTION_PROFILE_COUNT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_TRAP_ACTION_PROFILE_METER_INFO_print(
    SOC_SAND_IN  ARAD_PP_TRAP_ACTION_PROFILE_METER_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_ACTION_PROFILE_METER_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_TRAP_ACTION_PROFILE_POLICE_INFO_print(
    SOC_SAND_IN  ARAD_PP_TRAP_ACTION_PROFILE_POLICE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_ACTION_PROFILE_POLICE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_TRAP_ACTION_PROFILE_COS_INFO_print(
    SOC_SAND_IN  ARAD_PP_TRAP_ACTION_PROFILE_COS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_ACTION_PROFILE_COS_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_TRAP_ACTION_PROFILE_PROCESS_INFO_print(
    SOC_SAND_IN  ARAD_PP_TRAP_ACTION_PROFILE_PROCESS_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_ACTION_PROFILE_PROCESS_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_TRAP_FRWRD_ACTION_PROFILE_INFO_print(
    SOC_SAND_IN  ARAD_PP_TRAP_FRWRD_ACTION_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_FRWRD_ACTION_PROFILE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_TRAP_SNOOP_ACTION_PROFILE_INFO_print(
    SOC_SAND_IN  ARAD_PP_TRAP_SNOOP_ACTION_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_SNOOP_ACTION_PROFILE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_TRAP_EG_ACTION_PROFILE_INFO_print(
    SOC_SAND_IN  ARAD_PP_TRAP_EG_ACTION_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_EG_ACTION_PROFILE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_TRAP_MACT_EVENT_INFO_print(
    SOC_SAND_IN  ARAD_PP_TRAP_MACT_EVENT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_MACT_EVENT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_TRAP_PACKET_INFO_print(
    SOC_SAND_IN  ARAD_PP_TRAP_PACKET_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_TRAP_PACKET_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* ARAD_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif /* of #if defined(BCM_88650_A0) */

