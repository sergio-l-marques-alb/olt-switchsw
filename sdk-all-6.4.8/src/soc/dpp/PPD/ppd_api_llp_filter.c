/* $Id: ppd_api_llp_filter.c,v 1.10 Broadcom SDK $
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
* FILENAME:       DuneDriver/ppd/src/soc_ppd_api_llp_filter.c
*
* MODULE PREFIX:  soc_ppd_llp
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

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_PPD

#include <shared/bsl.h>
#include <soc/dpp/drv.h>

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/PPD/ppd_api_llp_filter.h>
#ifdef LINK_PB_LIBRARIES
  #include <soc/dpp/Petra/PB_PP/pb_pp_api_llp_filter.h>
#endif
#ifdef LINK_T20E_LIBRARIES
  #include <soc/dpp/T20E/t20e_api_llp_filter.h>
#endif
#ifdef LINK_ARAD_LIBRARIES
  #include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_llp_filter.h>
#endif

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

CONST STATIC
  SOC_PROCEDURE_DESC_ELEMENT
    Ppd_procedure_desc_element_llp_filter[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_FILTER_DESIGNATED_VLAN_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_FILTER_DESIGNATED_VLAN_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_FILTER_DESIGNATED_VLAN_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_FILTER_DESIGNATED_VLAN_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_LLP_FILTER_GET_PROCS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */
  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};
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
  soc_ppd_llp_filter_ingress_vlan_membership_set(
    SOC_SAND_IN  int                               unit,
	SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                         vid_ndx,
    SOC_SAND_IN  uint32                                ports[SOC_PPD_VLAN_MEMBERSHIP_BITMAP_SIZE]
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_TMP_DEVICE_CALL(llp_filter_ingress_vlan_membership_set,(unit, core_id, vid_ndx, ports));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_llp_filter_ingress_vlan_membership_set_print,(unit,vid_ndx,ports));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_llp_filter_ingress_vlan_membership_set()", vid_ndx, 0);
}

/*********************************************************************
*     Sets ingress VLAN membership; which incoming local ports
 *     belong to the VLAN. Packets received on a port that is
 *     not a member of the VLAN the packet is classified to be
 *     filtered.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_llp_filter_ingress_vlan_membership_get(
    SOC_SAND_IN  int                               unit,
	SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                         vid_ndx,
    SOC_SAND_OUT uint32                                ports[SOC_PPD_VLAN_MEMBERSHIP_BITMAP_SIZE]
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_TMP_DEVICE_CALL(llp_filter_ingress_vlan_membership_get,(unit, core_id, vid_ndx, ports));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_llp_filter_ingress_vlan_membership_get_print,(unit,vid_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_llp_filter_ingress_vlan_membership_get()", vid_ndx, 0);
}

/*********************************************************************
*     Add a local port as a member in a VLAN.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_llp_filter_ingress_vlan_membership_port_add(
    SOC_SAND_IN  int                               unit,
	SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                         vid_ndx,
    SOC_SAND_IN  SOC_PPD_PORT                                local_port
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_TMP_DEVICE_CALL(llp_filter_ingress_vlan_membership_port_add,(unit, core_id, vid_ndx, local_port));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_llp_filter_ingress_vlan_membership_port_add_print,(unit,vid_ndx,local_port));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_llp_filter_ingress_vlan_membership_port_add()", vid_ndx, 0);
}

/*********************************************************************
*     Remove a local port from the VLAN membership.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_llp_filter_ingress_vlan_membership_port_remove(
    SOC_SAND_IN  int                               unit,
	SOC_SAND_IN   int                              core_id,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                         vid_ndx,
    SOC_SAND_IN  SOC_PPD_PORT                                local_port
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_PPD_ARAD_TMP_DEVICE_CALL(llp_filter_ingress_vlan_membership_port_remove,(unit, core_id, vid_ndx, local_port));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_llp_filter_ingress_vlan_membership_port_remove_print,(unit,vid_ndx,local_port));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_llp_filter_ingress_vlan_membership_port_remove()", vid_ndx, 0);
}

/*********************************************************************
*     Sets acceptable frame type on incoming port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_llp_filter_ingress_acceptable_frames_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                port_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT      vlan_format_ndx,
    SOC_SAND_IN  SOC_PPD_ACTION_PROFILE                      *action_profile,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(action_profile);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(llp_filter_ingress_acceptable_frames_set,(unit, port_profile_ndx, vlan_format_ndx, action_profile, success));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_llp_filter_ingress_acceptable_frames_set_print,(unit,port_profile_ndx,vlan_format_ndx,action_profile));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_llp_filter_ingress_acceptable_frames_set()", port_profile_ndx, 0);
}

/*********************************************************************
*     Sets acceptable frame type on incoming port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_llp_filter_ingress_acceptable_frames_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                port_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT      vlan_format_ndx,
    SOC_SAND_OUT SOC_PPD_ACTION_PROFILE                      *action_profile
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(action_profile);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(llp_filter_ingress_acceptable_frames_get,(unit, port_profile_ndx, vlan_format_ndx, action_profile));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_llp_filter_ingress_acceptable_frames_get_print,(unit,port_profile_ndx,vlan_format_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_llp_filter_ingress_acceptable_frames_get()", port_profile_ndx, 0);
}

/*********************************************************************
*     Set a designated VLAN for a port. Incoming Trill packet
 *     will be checked if it has this T-VID; otherwise, packet
 *     will be dropped.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_llp_filter_designated_vlan_set(
    SOC_SAND_IN  int                               unit,
	SOC_SAND_IN   int                              core_id,
    SOC_SAND_IN  SOC_PPD_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                         vid,
    SOC_SAND_IN  uint8                               accept,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LLP_FILTER_DESIGNATED_VLAN_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(llp_filter_designated_vlan_set,(unit, core_id, local_port_ndx, vid, accept, success));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_llp_filter_designated_vlan_set_print,(unit,local_port_ndx,vid,accept));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_llp_filter_designated_vlan_set()", local_port_ndx, 0);
}

/*********************************************************************
*     Set a designated VLAN for a port. Incoming Trill packet
 *     will be checked if it has this T-VID; otherwise, packet
 *     will be dropped.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_ppd_llp_filter_designated_vlan_get(
    SOC_SAND_IN  int                               unit,
	SOC_SAND_IN   int                              core_id,
    SOC_SAND_IN  SOC_PPD_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_SAND_PP_VLAN_ID                         *vid,
    SOC_SAND_OUT uint8                               *accept
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PPD_LLP_FILTER_DESIGNATED_VLAN_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(vid);
  SOC_SAND_CHECK_NULL_INPUT(accept);

  SOC_PPD_ARAD_TMP_DEVICE_CALL(llp_filter_designated_vlan_get,(unit, core_id, local_port_ndx, vid, accept));

  SOC_PPD_DO_NOTHING_AND_EXIT;
exit:
  SOC_PPD_FUNC_PRINT(soc_ppd_llp_filter_designated_vlan_get_print,(unit,local_port_ndx));
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_ppd_llp_filter_designated_vlan_get()", local_port_ndx, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_ppd_api_llp_filter module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  soc_ppd_llp_filter_get_procs_ptr(
  )
{
  return Ppd_procedure_desc_element_llp_filter;
}
#if SOC_PPD_DEBUG_IS_LVL1

#endif /* SOC_PPD_DEBUG_IS_LVL1 */

#if SOC_PPD_DEBUG_IS_LVL3

void
  soc_ppd_llp_filter_ingress_vlan_membership_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                         vid_ndx,
    SOC_SAND_IN  uint32                                ports[SOC_PPD_VLAN_MEMBERSHIP_BITMAP_SIZE]
  )
{

  uint32
    ind;

  LOG_CLI((BSL_META_U(unit,
                      "vid_ndx: %lu\n\r"),vid_ndx));

  for (ind = 0; ind < SOC_PPD_VLAN_MEMBERSHIP_BITMAP_SIZE; ++ind)
  {
    LOG_CLI((BSL_META_U(unit,
                        "ports[%u]: %lu\n\r"),ind,ports[ind]));
  }

  return;
}
void
  soc_ppd_llp_filter_ingress_vlan_membership_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                         vid_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "vid_ndx: %lu\n\r"),vid_ndx));

  return;
}
void
  soc_ppd_llp_filter_ingress_vlan_membership_port_add_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                         vid_ndx,
    SOC_SAND_IN  SOC_PPD_PORT                                local_port
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "vid_ndx: %lu\n\r"),vid_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "local_port: %lu\n\r"),local_port));

  return;
}
void
  soc_ppd_llp_filter_ingress_vlan_membership_port_remove_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                         vid_ndx,
    SOC_SAND_IN  SOC_PPD_PORT                                local_port
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "vid_ndx: %lu\n\r"),vid_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "local_port: %lu\n\r"),local_port));

  return;
}
void
  soc_ppd_llp_filter_ingress_acceptable_frames_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                port_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT      vlan_format_ndx,
    SOC_SAND_IN  SOC_PPD_ACTION_PROFILE                      *action_profile
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "port_profile_ndx: %lu\n\r"),port_profile_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "vlan_format_ndx %s "), soc_sand_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_to_string(vlan_format_ndx)));

  LOG_CLI((BSL_META_U(unit,
                      "action_profile:")));
  SOC_PPD_ACTION_PROFILE_print((action_profile));

  return;
}
void
  soc_ppd_llp_filter_ingress_acceptable_frames_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                port_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT      vlan_format_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "port_profile_ndx: %lu\n\r"),port_profile_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "vlan_format_ndx %s "), soc_sand_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_to_string(vlan_format_ndx)));

  return;
}
void
  soc_ppd_llp_filter_designated_vlan_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                         vid,
    SOC_SAND_IN  uint8                               accept
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "local_port_ndx: %lu\n\r"),local_port_ndx));

  LOG_CLI((BSL_META_U(unit,
                      "vid: %lu\n\r"),vid));

  LOG_CLI((BSL_META_U(unit,
                      "accept: %u\n\r"),accept));

  return;
}
void
  soc_ppd_llp_filter_designated_vlan_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_PORT                                local_port_ndx
  )
{

  LOG_CLI((BSL_META_U(unit,
                      "local_port_ndx: %lu\n\r"),local_port_ndx));

  return;
}
#endif /* SOC_PPD_DEBUG_IS_LVL3 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

