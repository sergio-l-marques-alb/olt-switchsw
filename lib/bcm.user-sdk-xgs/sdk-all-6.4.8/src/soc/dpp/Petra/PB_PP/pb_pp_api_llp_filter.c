/* $Id: pb_pp_api_llp_filter.c,v 1.6 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/src/soc_pb_pp_api_llp_filter.c
*
* MODULE PREFIX:  soc_pb_pp
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

#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_llp_filter.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_llp_filter.h>

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
*     Sets ingress VLAN membership; which incoming local ports
 *     belong to the VLAN. Packets received on a port that is
 *     not a member of the VLAN the packet is classified to be
 *     filtered.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_filter_ingress_vlan_membership_set(
    SOC_SAND_IN  int                                     unit,
	SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  uint32                                  ports[SOC_PB_PP_VLAN_MEMBERSHIP_BITMAP_SIZE]
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_pb_pp_llp_filter_ingress_vlan_membership_set_verify(
          unit,
          vid_ndx,
          ports
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_llp_filter_ingress_vlan_membership_set_unsafe(
          unit,
          vid_ndx,
          ports
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_filter_ingress_vlan_membership_set()", vid_ndx, 0);
}

/*********************************************************************
*     Sets ingress VLAN membership; which incoming local ports
 *     belong to the VLAN. Packets received on a port that is
 *     not a member of the VLAN the packet is classified to be
 *     filtered.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_filter_ingress_vlan_membership_get(
    SOC_SAND_IN  int                                     unit,
	SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_OUT uint32                                  ports[SOC_PB_PP_VLAN_MEMBERSHIP_BITMAP_SIZE]
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_pb_pp_llp_filter_ingress_vlan_membership_get_verify(
          unit,
          vid_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_llp_filter_ingress_vlan_membership_get_unsafe(
          unit,
          vid_ndx,
          ports
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_filter_ingress_vlan_membership_get()", vid_ndx, 0);
}

/*********************************************************************
*     Add a local port as a member in a VLAN.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_filter_ingress_vlan_membership_port_add(
    SOC_SAND_IN  int                                     unit,
	SOC_SAND_IN   int                              		 core_id,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_pb_pp_llp_filter_ingress_vlan_membership_port_add_verify(
          unit,
          vid_ndx,
          local_port
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_llp_filter_ingress_vlan_membership_port_add_unsafe(
          unit,
          vid_ndx,
          local_port
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_filter_ingress_vlan_membership_port_add()", vid_ndx, 0);
}

/*********************************************************************
*     Remove a local port from the VLAN membership.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_filter_ingress_vlan_membership_port_remove(
    SOC_SAND_IN  int                                     unit,
	SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_pb_pp_llp_filter_ingress_vlan_membership_port_remove_verify(
          unit,
          vid_ndx,
          local_port
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_llp_filter_ingress_vlan_membership_port_remove_unsafe(
          unit,
          vid_ndx,
          local_port
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_filter_ingress_vlan_membership_port_remove()", vid_ndx, 0);
}

/*********************************************************************
*     Sets acceptable frame type on incoming port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_filter_ingress_acceptable_frames_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      port_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT            vlan_format_ndx,
    SOC_SAND_IN  SOC_PB_PP_ACTION_PROFILE                          *action_profile,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(action_profile);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = soc_pb_pp_llp_filter_ingress_acceptable_frames_set_verify(
          unit,
          port_profile_ndx,
          vlan_format_ndx,
          action_profile
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_llp_filter_ingress_acceptable_frames_set_unsafe(
          unit,
          port_profile_ndx,
          vlan_format_ndx,
          action_profile,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_filter_ingress_acceptable_frames_set()", port_profile_ndx, 0);
}

/*********************************************************************
*     Sets acceptable frame type on incoming port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_filter_ingress_acceptable_frames_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      port_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT            vlan_format_ndx,
    SOC_SAND_OUT SOC_PB_PP_ACTION_PROFILE                          *action_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(action_profile);

  res = soc_pb_pp_llp_filter_ingress_acceptable_frames_get_verify(
          unit,
          port_profile_ndx,
          vlan_format_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_llp_filter_ingress_acceptable_frames_get_unsafe(
          unit,
          port_profile_ndx,
          vlan_format_ndx,
          action_profile
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_filter_ingress_acceptable_frames_get()", port_profile_ndx, 0);
}

/*********************************************************************
*     Set a designated VLAN for a port. Incoming Trill packet
 *     will be checked if it has this T-VID; otherwise, packet
 *     will be dropped.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_filter_designated_vlan_set(
    SOC_SAND_IN  int                                     unit,
	SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid,
    SOC_SAND_IN  uint8                                     accept,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_FILTER_DESIGNATED_VLAN_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(success);

  res = soc_pb_pp_llp_filter_designated_vlan_set_verify(
          unit,
          local_port_ndx,
          vid,
          accept
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_llp_filter_designated_vlan_set_unsafe(
          unit,
          local_port_ndx,
          vid,
          accept,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_filter_designated_vlan_set()", local_port_ndx, 0);
}

/*********************************************************************
*     Set a designated VLAN for a port. Incoming Trill packet
 *     will be checked if it has this T-VID; otherwise, packet
 *     will be dropped.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_llp_filter_designated_vlan_get(
    SOC_SAND_IN  int                                     unit,
	SOC_SAND_IN  int                                     core_id,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_SAND_PP_VLAN_ID                           *vid,
    SOC_SAND_OUT uint8                                     *accept
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_LLP_FILTER_DESIGNATED_VLAN_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(vid);
  SOC_SAND_CHECK_NULL_INPUT(accept);

  res = soc_pb_pp_llp_filter_designated_vlan_get_verify(
          unit,
          local_port_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_llp_filter_designated_vlan_get_unsafe(
          unit,
          local_port_ndx,
          vid,
          accept
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_llp_filter_designated_vlan_get()", local_port_ndx, 0);
}

#if SOC_PB_PP_DEBUG_IS_LVL1

#endif /* SOC_PB_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

