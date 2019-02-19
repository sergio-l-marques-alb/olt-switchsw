/* $Id: pcp_api_frwrd_mact_mgmt.c,v 1.4 Broadcom SDK $
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

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/PCP/pcp_framework.h>
#include <soc/dpp/PCP/pcp_api_frwrd_mact_mgmt.h>
#include <soc/dpp/PCP/pcp_frwrd_mact_mgmt.h>

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
*     Sets the mode of the MACT, including - ingress vs.
 *     egress learning- how each device responds internally to
 *     events (learn/aged-out/refresh) - which events to inform
 *     other devices.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_mact_oper_mode_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_MACT_OPER_MODE_INFO           *oper_mode_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_OPER_MODE_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(oper_mode_info);

  res = pcp_frwrd_mact_oper_mode_info_set_verify(
          unit,
          oper_mode_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_frwrd_mact_oper_mode_info_set_unsafe(
          unit,
          oper_mode_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_oper_mode_info_set()", 0, 0);
}

/*********************************************************************
*     Sets the mode of the MACT, including - ingress vs.
 *     egress learning- how each device responds internally to
 *     events (learn/aged-out/refresh) - which events to inform
 *     other devices.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_mact_oper_mode_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT PCP_FRWRD_MACT_OPER_MODE_INFO           *oper_mode_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_OPER_MODE_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(oper_mode_info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_frwrd_mact_oper_mode_info_get_unsafe(
          unit,
          oper_mode_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_oper_mode_info_get()", 0, 0);
}

/*********************************************************************
*     Sets the aging info including enable aging and aging
 *     time.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_mact_aging_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_MACT_AGING_INFO               *aging_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_AGING_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(aging_info);

  res = pcp_frwrd_mact_aging_info_set_verify(
          unit,
          aging_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_frwrd_mact_aging_info_set_unsafe(
          unit,
          aging_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_aging_info_set()", 0, 0);
}

/*********************************************************************
*     Sets the aging info including enable aging and aging
 *     time.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_mact_aging_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT PCP_FRWRD_MACT_AGING_INFO               *aging_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_AGING_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(aging_info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_frwrd_mact_aging_info_get_unsafe(
          unit,
          aging_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_aging_info_get()", 0, 0);
}

/*********************************************************************
*     Sets the device action upon events invoked by the aging
 *     process: - Whether the device deletes aged-out entries
 *     internally - Whether the device generates an event for
 *     aged-out entries - Whether the device generates an event
 *     for refreshed entries
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_mact_aging_events_handle_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_MACT_AGING_EVENTS_HANDLE            *aging_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_CHECK_NULL_INPUT(aging_info);
  res = pcp_frwrd_mact_aging_events_handle_info_set_verify(
          unit,
          aging_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;
  res = pcp_frwrd_mact_aging_events_handle_info_set_unsafe(
          unit,
          aging_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);
exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_aging_events_handle_info_set()", 0, 0);
}
/*********************************************************************
*     Sets the device action upon events invoked by the aging
 *     process: - Whether the device deletes aged-out entries
 *     internally - Whether the device generates an event for
 *     aged-out entries - Whether the device generates an event
 *     for refreshed entries
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_mact_aging_events_handle_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT PCP_FRWRD_MACT_AGING_EVENTS_HANDLE            *aging_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_AGING_EVENTS_HANDLE_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_CHECK_NULL_INPUT(aging_info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;
  res = pcp_frwrd_mact_aging_events_handle_info_get_unsafe(
          unit,
          aging_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_aging_events_handle_info_get()", 0, 0);
}
/*********************************************************************
*     Maps FID-Profile to FID, for shared learning.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_mact_fid_profile_to_fid_map_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  fid_profile_ndx,
    SOC_SAND_IN  PCP_FID                                 fid
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_SET_ERROR_CODE(PCP_FEATURE_NOT_SUPPORTED_ERR, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_fid_profile_to_fid_map_set()", fid_profile_ndx, 0);
}

/*********************************************************************
*     Maps FID-Profile to FID, for shared learning.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_mact_fid_profile_to_fid_map_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  fid_profile_ndx,
    SOC_SAND_OUT PCP_FID                                 *fid
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_FID_PROFILE_TO_FID_MAP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(fid);

  SOC_SAND_SET_ERROR_CODE(PCP_FEATURE_NOT_SUPPORTED_ERR, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_fid_profile_to_fid_map_get()", fid_profile_ndx, 0);
}

/*********************************************************************
*     Enable the MAC limit feature, which limits per fid the
 *     maximum number of entries allowed to be in the MAC
 *     Table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_mact_mac_limit_glbl_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO           *limit_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(limit_info);

  res = pcp_frwrd_mact_mac_limit_glbl_info_set_verify(
          unit,
          limit_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_frwrd_mact_mac_limit_glbl_info_set_unsafe(
          unit,
          limit_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_mac_limit_glbl_info_set()", 0, 0);
}

/*********************************************************************
*     Enable the MAC limit feature, which limits per fid the
 *     maximum number of entries allowed to be in the MAC
 *     Table.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_mact_mac_limit_glbl_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT PCP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO           *limit_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(limit_info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_frwrd_mact_mac_limit_glbl_info_get_unsafe(
          unit,
          limit_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_mac_limit_glbl_info_get()", 0, 0);
}

/*********************************************************************
*     Set the limit information including the MAC-limit (i.e.,
 *     the maximum number of entries an FID can hold in the MAC
 *     Table), and the notification action if the configured
 *     limit is exceeded.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_mact_learn_profile_limit_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx,
    SOC_SAND_IN  PCP_FRWRD_MACT_MAC_LIMIT_INFO          *limit_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(limit_info);

  res = pcp_frwrd_mact_learn_profile_limit_info_set_verify(
          unit,
          mac_learn_profile_ndx,
          limit_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_frwrd_mact_learn_profile_limit_info_set_unsafe(
          unit,
          mac_learn_profile_ndx,
          limit_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_learn_profile_limit_info_set()", mac_learn_profile_ndx, 0);
}

/*********************************************************************
*     Set the limit information including the MAC-limit (i.e.,
 *     the maximum number of entries an FID can hold in the MAC
 *     Table), and the notification action if the configured
 *     limit is exceeded.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_mact_learn_profile_limit_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx,
    SOC_SAND_OUT PCP_FRWRD_MACT_MAC_LIMIT_INFO          *limit_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_LEARN_PROFILE_LIMIT_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(limit_info);

  res = pcp_frwrd_mact_learn_profile_limit_info_get_verify(
          unit,
          mac_learn_profile_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_frwrd_mact_learn_profile_limit_info_get_unsafe(
          unit,
          mac_learn_profile_ndx,
          limit_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_learn_profile_limit_info_get()", mac_learn_profile_ndx, 0);
}
/*********************************************************************
*     Map the mac-learn-profile to the event-handle profile.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_mact_event_handle_profile_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx,
    SOC_SAND_IN  uint32                                  event_handle_profile
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_EVENT_HANDLE_PROFILE_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  res = pcp_frwrd_mact_event_handle_profile_set_verify(
          unit,
          mac_learn_profile_ndx,
          event_handle_profile
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;
  res = pcp_frwrd_mact_event_handle_profile_set_unsafe(
          unit,
          mac_learn_profile_ndx,
          event_handle_profile
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);
exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_event_handle_profile_set()", mac_learn_profile_ndx, 0);
}
/*********************************************************************
*     Map the mac-learn-profile to the event-handle profile.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_mact_event_handle_profile_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mac_learn_profile_ndx,
    SOC_SAND_OUT uint32                                  *event_handle_profile
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_EVENT_HANDLE_PROFILE_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_CHECK_NULL_INPUT(event_handle_profile);
  res = pcp_frwrd_mact_event_handle_profile_get_verify(
          unit,
          mac_learn_profile_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;
  res = pcp_frwrd_mact_event_handle_profile_get_unsafe(
          unit,
          mac_learn_profile_ndx,
          event_handle_profile
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);
exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_event_handle_profile_get()", mac_learn_profile_ndx, 0);
}

/*********************************************************************
*     Get the last event parameters for the direct access mode
 *     (i.e., if no OLP messages are sent in case of event)
 *     from the Event FIFO.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_mact_event_get(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_OUT PCP_FRWRD_MACT_EVENT_BUFFER            *event_buf
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_EVENT_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(event_buf);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_frwrd_mact_event_get_unsafe(
          unit,
          event_buf
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_event_get()", 0, 0);
}

/*********************************************************************
*     Set how to handle an event according to the event key
 *     parameters (event-type,vsi-handle-profile,is-lag)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_mact_event_handle_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_MACT_EVENT_HANDLE_KEY               *event_key,
    SOC_SAND_IN  PCP_FRWRD_MACT_EVENT_HANDLE_INFO              *handle_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_EVENT_HANDLE_INFO_SET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_CHECK_NULL_INPUT(event_key);
  SOC_SAND_CHECK_NULL_INPUT(handle_info);
  res = pcp_frwrd_mact_event_handle_info_set_verify(
          unit,
          event_key,
          handle_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;
  res = pcp_frwrd_mact_event_handle_info_set_unsafe(
          unit,
          event_key,
          handle_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);
exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_event_handle_info_set()", 0, 0);
}
/*********************************************************************
*     Set how to handle an event according to the event key
 *     parameters (event-type,vsi-handle-profile,is-lag)
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_mact_event_handle_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_MACT_EVENT_HANDLE_KEY               *event_key,
    SOC_SAND_OUT PCP_FRWRD_MACT_EVENT_HANDLE_INFO              *handle_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_EVENT_HANDLE_INFO_GET);
  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_CHECK_NULL_INPUT(event_key);
  SOC_SAND_CHECK_NULL_INPUT(handle_info);
  res = pcp_frwrd_mact_event_handle_info_get_verify(
          unit,
          event_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  SOC_SAND_TAKE_DEVICE_SEMAPHORE;
  res = pcp_frwrd_mact_event_handle_info_get_unsafe(
          unit,
          event_key,
          handle_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);
exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_event_handle_info_get()", 0, 0);
}
/*********************************************************************
*     Set how to distribute the learn messages to other
 *     devices/CPU.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_mact_learn_msgs_distribution_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(distribution_info);

  res = pcp_frwrd_mact_learn_msgs_distribution_info_set_verify(
          unit,
          distribution_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_frwrd_mact_learn_msgs_distribution_info_set_unsafe(
          unit,
          distribution_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);
exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_learn_msgs_distribution_info_set()", 0, 0);
}
/*********************************************************************
*     Set how to distribute the learn messages to other
 *     devices/CPU.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_mact_learn_msgs_distribution_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT PCP_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_LEARN_MSGS_DISTRIBUTION_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(distribution_info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_frwrd_mact_learn_msgs_distribution_info_get_unsafe(
          unit,
          distribution_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_learn_msgs_distribution_info_get()", 0, 0);
}

/*********************************************************************
*     Set how to distribute the shadow messages to the other
 *     devices/CPU.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_mact_shadow_msgs_distribution_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(distribution_info);

  res = pcp_frwrd_mact_shadow_msgs_distribution_info_set_verify(
          unit,
          distribution_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_frwrd_mact_shadow_msgs_distribution_info_set_unsafe(
          unit,
          distribution_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_shadow_msgs_distribution_info_set()", 0, 0);
}

/*********************************************************************
*     Set how to distribute the shadow messages to the other
 *     devices/CPU.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_mact_shadow_msgs_distribution_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT PCP_FRWRD_MACT_MSG_DISTR_INFO                 *distribution_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_SHADOW_MSGS_DISTRIBUTION_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(distribution_info);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_frwrd_mact_shadow_msgs_distribution_info_get_unsafe(
          unit,
          distribution_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_shadow_msgs_distribution_info_get()", 0, 0);
}

uint32
  pcp_frwrd_mact_mac_limit_exceeded_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT  PCP_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO  *exceed_info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(exceed_info);

  SOC_SAND_SET_ERROR_CODE(PCP_FEATURE_NOT_SUPPORTED_ERR, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_mac_limit_exceeded_info_get()", 0, 0);
}

/*********************************************************************
*     Set per port MACT management information including which
 *     profile to activate when SA is known in this port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_mact_port_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  PCP_PORT                                local_port_ndx,
    SOC_SAND_IN  PCP_FRWRD_MACT_PORT_INFO                *port_info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_PORT_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  SOC_SAND_SET_ERROR_CODE(PCP_FEATURE_NOT_SUPPORTED_ERR, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_port_info_set()", local_port_ndx, 0);
}

/*********************************************************************
*     Set per port MACT management information including which
 *     profile to activate when SA is known in this port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_mact_port_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  PCP_PORT                                local_port_ndx,
    SOC_SAND_OUT PCP_FRWRD_MACT_PORT_INFO                *port_info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_PORT_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  SOC_SAND_SET_ERROR_CODE(PCP_FEATURE_NOT_SUPPORTED_ERR, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_port_info_get()", local_port_ndx, 0);
}

/*********************************************************************
*     For each trap type, set the action profile. Different
 *     actions may be assigned to the same trap type according
 *     to the port-profile (4 possibilities).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_mact_trap_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_MACT_TRAP_TYPE                trap_type_ndx,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  PCP_ACTION_PROFILE                      *action_profile
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_TRAP_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(action_profile);

  SOC_SAND_SET_ERROR_CODE(PCP_FEATURE_NOT_SUPPORTED_ERR, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_trap_info_set()", 0, port_profile_ndx);
}

/*********************************************************************
*     For each trap type, set the action profile. Different
 *     actions may be assigned to the same trap type according
 *     to the port-profile (4 possibilities).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_mact_trap_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_MACT_TRAP_TYPE                trap_type_ndx,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_OUT PCP_ACTION_PROFILE                      *action_profile
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_TRAP_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(action_profile);

  SOC_SAND_SET_ERROR_CODE(PCP_FEATURE_NOT_SUPPORTED_ERR, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_trap_info_get()", 0, port_profile_ndx);
}

/*********************************************************************
*     Sets the information for bridging compatible Multicast
 *     MAC addresses.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_mact_ip_compatible_mc_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO    *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_SET_ERROR_CODE(PCP_FEATURE_NOT_SUPPORTED_ERR, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_ip_compatible_mc_info_set()", 0, 0);
}

/*********************************************************************
*     Sets the information for bridging compatible Multicast
 *     MAC addresses.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_mact_ip_compatible_mc_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT PCP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO    *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_SET_ERROR_CODE(PCP_FEATURE_NOT_SUPPORTED_ERR, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_ip_compatible_mc_info_get()", 0, 0);
}

/*********************************************************************
*     The MACT may report different events using the event
 *     FIFO (e.g., learn, age, transplant, and retrieve). This
 *     API Parses the event buffer into a meaningful structure.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_frwrd_mact_event_parse(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  PCP_FRWRD_MACT_EVENT_BUFFER            *event_buf,
    SOC_SAND_OUT PCP_FRWRD_MACT_EVENT_INFO              *mact_event
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_FRWRD_MACT_EVENT_PARSE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(event_buf);
  SOC_SAND_CHECK_NULL_INPUT(mact_event);

  res = pcp_frwrd_mact_event_parse_verify(
          unit,
          event_buf
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_frwrd_mact_event_parse_unsafe(
          unit,
          event_buf,
          mact_event
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_frwrd_mact_event_parse()", 0, 0);
}

/* 
 *	Function from Soc_petra-PP vsi.
 */
/*********************************************************************
*     Set the Virtual Switch Instance information. After
 *     setting the VSI, the user may attach L2 Logical
 *     Interfaces to it: ACs; PWEs
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_vsi_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  PCP_VSI_ID                              vsi_ndx,
    SOC_SAND_IN  PCP_VSI_INFO                            *vsi_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_VSI_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(vsi_info);

  res = pcp_vsi_info_set_verify(
          unit,
          vsi_ndx,
          vsi_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_vsi_info_set_unsafe(
          unit,
          vsi_ndx,
          vsi_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_vsi_info_set()", vsi_ndx, 0);
}

/*********************************************************************
*     Set the Virtual Switch Instance information. After
 *     setting the VSI, the user may attach L2 Logical
 *     Interfaces to it: ACs; PWEs
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  pcp_vsi_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  PCP_VSI_ID                              vsi_ndx,
    SOC_SAND_OUT PCP_VSI_INFO                            *vsi_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(PCP_VSI_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(vsi_info);

  res = pcp_vsi_info_get_verify(
          unit,
          vsi_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = pcp_vsi_info_get_unsafe(
          unit,
          vsi_ndx,
          vsi_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  PCP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in pcp_vsi_info_get()", vsi_ndx, 0);
}

void
  PCP_VSI_INFO_clear(
    SOC_SAND_OUT PCP_VSI_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_VSI_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_clear(
    SOC_SAND_OUT PCP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_FRWRD_MACT_AGING_INFO_clear(
    SOC_SAND_OUT PCP_FRWRD_MACT_AGING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_AGING_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_FRWRD_MACT_EVENT_HANDLE_KEY_clear(
    SOC_SAND_OUT PCP_FRWRD_MACT_EVENT_HANDLE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_FRWRD_MACT_EVENT_HANDLE_INFO_clear(
    SOC_SAND_OUT PCP_FRWRD_MACT_EVENT_HANDLE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_FRWRD_MACT_AGING_EVENTS_HANDLE_clear(
    SOC_SAND_OUT PCP_FRWRD_MACT_AGING_EVENTS_HANDLE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_FRWRD_MACT_MSG_DISTR_INFO_clear(
    SOC_SAND_OUT PCP_FRWRD_MACT_MSG_DISTR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_FRWRD_MACT_EVENT_PROCESSING_INFO_clear(
    SOC_SAND_OUT PCP_FRWRD_MACT_EVENT_PROCESSING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_EVENT_PROCESSING_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_FRWRD_MACT_OPER_MODE_INFO_clear(
    SOC_SAND_OUT PCP_FRWRD_MACT_OPER_MODE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_OPER_MODE_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_FRWRD_MACT_MAC_LIMIT_INFO_clear(
    SOC_SAND_OUT PCP_FRWRD_MACT_MAC_LIMIT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_MAC_LIMIT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_clear(
    SOC_SAND_OUT PCP_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_clear(
    SOC_SAND_OUT PCP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_FRWRD_MACT_PORT_INFO_clear(
    SOC_SAND_OUT PCP_FRWRD_MACT_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_PORT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_FRWRD_MACT_EVENT_LAG_INFO_clear(
    SOC_SAND_OUT PCP_FRWRD_MACT_EVENT_LAG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_EVENT_LAG_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_FRWRD_MACT_EVENT_INFO_clear(
    SOC_SAND_OUT PCP_FRWRD_MACT_EVENT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_EVENT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_FRWRD_MACT_EVENT_BUFFER_clear(
    SOC_SAND_OUT PCP_FRWRD_MACT_EVENT_BUFFER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_EVENT_BUFFER_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if PCP_DEBUG_IS_LVL1
const char*
  PCP_FRWRD_MACT_LEARNING_MODE_to_string(
    SOC_SAND_IN  PCP_FRWRD_MACT_LEARNING_MODE enum_val
  )
{
  return SOC_PPC_FRWRD_MACT_LEARNING_MODE_to_string(enum_val);
}

const char*
  PCP_FRWRD_MACT_SHADOW_MODE_to_string(
    SOC_SAND_IN  PCP_FRWRD_MACT_SHADOW_MODE enum_val
  )
{
  return SOC_PPC_FRWRD_MACT_SHADOW_MODE_to_string(enum_val);
}

const char*
  PCP_FRWRD_MACT_MSG_HDR_TYPE_to_string(
    SOC_SAND_IN  PCP_FRWRD_MACT_MSG_HDR_TYPE enum_val
  )
{
  return SOC_PPC_FRWRD_MACT_MSG_HDR_TYPE_to_string(enum_val);
}

const char*
  PCP_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_to_string(
    SOC_SAND_IN  PCP_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE enum_val
  )
{
  return SOC_PPC_FRWRD_MACT_LIMIT_EXCEED_NOTIFY_TYPE_to_string(enum_val);
}

const char*
  PCP_FRWRD_MACT_TRAP_TYPE_to_string(
    SOC_SAND_IN  PCP_FRWRD_MACT_TRAP_TYPE enum_val
  )
{
  return SOC_PPC_FRWRD_MACT_TRAP_TYPE_to_string(enum_val);
}

const char*
  PCP_FRWRD_MACT_EVENT_TYPE_to_string(
    SOC_SAND_IN  PCP_FRWRD_MACT_EVENT_TYPE enum_val
  )
{
  return SOC_PPC_FRWRD_MACT_EVENT_TYPE_to_string(enum_val);
}

const char*
  PCP_FRWRD_MACT_EVENT_PATH_TYPE_to_string(
    SOC_SAND_IN  PCP_FRWRD_MACT_EVENT_PATH_TYPE enum_val
  )
{
  return SOC_PPC_FRWRD_MACT_EVENT_PATH_TYPE_to_string(enum_val);
}

void
  PCP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_print(
    SOC_SAND_IN  PCP_FRWRD_MACT_IP_COMPATIBLE_MC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_IP_COMPATIBLE_MC_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_FRWRD_MACT_AGING_INFO_print(
    SOC_SAND_IN  PCP_FRWRD_MACT_AGING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_AGING_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_FRWRD_MACT_EVENT_HANDLE_KEY_print(
    SOC_SAND_IN  PCP_FRWRD_MACT_EVENT_HANDLE_KEY *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_KEY_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  PCP_FRWRD_MACT_EVENT_HANDLE_INFO_print(
    SOC_SAND_IN  PCP_FRWRD_MACT_EVENT_HANDLE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_PPC_FRWRD_MACT_EVENT_HANDLE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  PCP_FRWRD_MACT_AGING_EVENTS_HANDLE_print(
    SOC_SAND_IN  PCP_FRWRD_MACT_AGING_EVENTS_HANDLE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_PPC_FRWRD_MACT_AGING_EVENTS_HANDLE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
void
  PCP_FRWRD_MACT_MSG_DISTR_INFO_print(
    SOC_SAND_IN  PCP_FRWRD_MACT_MSG_DISTR_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_MSG_DISTR_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_FRWRD_MACT_EVENT_PROCESSING_INFO_print(
    SOC_SAND_IN  PCP_FRWRD_MACT_EVENT_PROCESSING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_EVENT_PROCESSING_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_FRWRD_MACT_OPER_MODE_INFO_print(
    SOC_SAND_IN  PCP_FRWRD_MACT_OPER_MODE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_OPER_MODE_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_FRWRD_MACT_MAC_LIMIT_INFO_print(
    SOC_SAND_IN  PCP_FRWRD_MACT_MAC_LIMIT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_MAC_LIMIT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_print(
    SOC_SAND_IN  PCP_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_MAC_LIMIT_EXCEEDED_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_print(
    SOC_SAND_IN  PCP_FRWRD_MACT_MAC_LIMIT_GLBL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_MAC_LIMIT_GLBL_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_FRWRD_MACT_PORT_INFO_print(
    SOC_SAND_IN  PCP_FRWRD_MACT_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_PORT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_FRWRD_MACT_EVENT_LAG_INFO_print(
    SOC_SAND_IN  PCP_FRWRD_MACT_EVENT_LAG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_EVENT_LAG_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_FRWRD_MACT_EVENT_INFO_print(
    SOC_SAND_IN  PCP_FRWRD_MACT_EVENT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_EVENT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  PCP_FRWRD_MACT_EVENT_BUFFER_print(
    SOC_SAND_IN  PCP_FRWRD_MACT_EVENT_BUFFER *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_FRWRD_MACT_EVENT_BUFFER_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}
#endif /* PCP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

