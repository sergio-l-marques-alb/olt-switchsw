/* $Id: pb_pp_api_eg_filter.c,v 1.9 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/src/soc_pb_pp_api_eg_filter.c
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
#include <soc/dpp/Petra/PB_PP/pb_pp_api_eg_filter.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_eg_filter.h>

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
*     Sets out-port filtering information, including which
 *     filtering to perform on this specific out-port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_port_info_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  int                          core_id,
    SOC_SAND_IN  SOC_PB_PP_PORT                         out_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_FILTER_PORT_INFO          *port_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_PORT_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  res = soc_pb_pp_eg_filter_port_info_set_verify(
          unit,
          out_port_ndx,
          port_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_eg_filter_port_info_set_unsafe(
          unit,
          out_port_ndx,
          port_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_port_info_set()", out_port_ndx, 0);
}

/*********************************************************************
*     Sets out-port filtering information, including which
 *     filtering to perform on this specific out-port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_port_info_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  int                          core_id,
    SOC_SAND_IN  SOC_PB_PP_PORT                         out_port_ndx,
    SOC_SAND_OUT SOC_PB_PP_EG_FILTER_PORT_INFO          *port_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_PORT_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(port_info);

  res = soc_pb_pp_eg_filter_port_info_get_verify(
          unit,
          out_port_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_eg_filter_port_info_get_unsafe(
          unit,
          out_port_ndx,
          port_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_port_info_get()", out_port_ndx, 0);
}

/*********************************************************************
*     Sets egress VSI membership, which represents the
 *     Service/S-VLAN membership. Set whether outgoing local
 *     port belongs to the VSI. Packets transmitted out through
 *     a port that is not member of the packet's VSI are
 *     filtered.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_vsi_port_membership_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  int                          core_id,
    SOC_SAND_IN  SOC_PB_PP_VSI_ID                       vsid_ndx,
    SOC_SAND_IN  SOC_PB_PP_PORT                         out_port_ndx,
    SOC_SAND_IN  uint8                          is_member
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_VSI_PORT_MEMBERSHIP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_pb_pp_eg_filter_vsi_port_membership_set_verify(
          unit,
          vsid_ndx,
          out_port_ndx,
          is_member
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_eg_filter_vsi_port_membership_set_unsafe(
          unit,
          vsid_ndx,
          out_port_ndx,
          is_member
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_vsi_port_membership_set()", vsid_ndx, out_port_ndx);
}

/*********************************************************************
*     Sets egress VSI membership, which represents the
 *     Service/S-VLAN membership. Set whether outgoing local
 *     port belongs to the VSI. Packets transmitted out through
 *     a port that is not member of the packet's VSI are
 *     filtered.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_vsi_port_membership_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  int                          core_id,
    SOC_SAND_IN  SOC_PB_PP_VSI_ID                       vsid_ndx,
    SOC_SAND_IN  SOC_PB_PP_PORT                         out_port_ndx,
    SOC_SAND_OUT uint8                          *is_member
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_VSI_PORT_MEMBERSHIP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(is_member);

  res = soc_pb_pp_eg_filter_vsi_port_membership_get_verify(
          unit,
          vsid_ndx,
          out_port_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_eg_filter_vsi_port_membership_get_unsafe(
          unit,
          vsid_ndx,
          out_port_ndx,
          is_member
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_vsi_port_membership_get()", vsid_ndx, out_port_ndx);
}

/*********************************************************************
 *   Gets egress VSI membership, which represents the
 *   Service/S-VLAN membership. Get all member outgoing local
 *   ports belongs to the VSI.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_vsi_membership_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_PB_PP_VSI_ID                       vsid_ndx,
    SOC_SAND_OUT uint32                               ports[SOC_PB_PP_VLAN_MEMBERSHIP_BITMAP_SIZE]
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_VSI_MEMBERSHIP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_pb_pp_eg_filter_vsi_membership_get_verify(
          unit,
          vsid_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_eg_filter_vsi_membership_get_unsafe(
          unit,
          vsid_ndx,
          ports
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_vsi_membership_get()", vsid_ndx, 0);
}

/*********************************************************************
*     Sets egress CVID membership (relevant for CEP ports).
 *     Sets whether outgoing local port belongs to the CVID.
 *     Packets transmitted out through a port that is not
 *     member of the packet's CVID are filtered.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_cvid_port_membership_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                    cvid_ndx,
    SOC_SAND_IN  SOC_PB_PP_PORT                         out_port_ndx,
    SOC_SAND_IN  uint8                          is_member
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_CVID_PORT_MEMBERSHIP_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_pb_pp_eg_filter_cvid_port_membership_set_verify(
          unit,
          cvid_ndx,
          out_port_ndx,
          is_member
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_eg_filter_cvid_port_membership_set_unsafe(
          unit,
          cvid_ndx,
          out_port_ndx,
          is_member
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_cvid_port_membership_set()", cvid_ndx, out_port_ndx);
}

/*********************************************************************
*     Sets egress CVID membership (relevant for CEP ports).
 *     Sets whether outgoing local port belongs to the CVID.
 *     Packets transmitted out through a port that is not
 *     member of the packet's CVID are filtered.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_cvid_port_membership_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                    cvid_ndx,
    SOC_SAND_IN  SOC_PB_PP_PORT                         out_port_ndx,
    SOC_SAND_OUT uint8                          *is_member
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_CVID_PORT_MEMBERSHIP_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(is_member);

  res = soc_pb_pp_eg_filter_cvid_port_membership_get_verify(
          unit,
          cvid_ndx,
          out_port_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_eg_filter_cvid_port_membership_get_unsafe(
          unit,
          cvid_ndx,
          out_port_ndx,
          is_member
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_cvid_port_membership_get()", cvid_ndx, out_port_ndx);
}

/*********************************************************************
*     Sets acceptable frame type on outgoing port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_port_acceptable_frames_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                           out_port_ndx,
    SOC_SAND_IN  uint32                           port_profile,
    SOC_SAND_IN  SOC_PB_PP_LLP_PARSE_INFO               *eg_prsr_out_key,
    SOC_SAND_IN  uint8                          accept
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_PORT_ACCEPTABLE_FRAMES_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(eg_prsr_out_key);

  res = soc_pb_pp_eg_filter_port_acceptable_frames_set_verify(
          unit,
          out_port_ndx,
          eg_prsr_out_key,
          accept
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_eg_filter_port_acceptable_frames_set_unsafe(
          unit,
          out_port_ndx,
          eg_prsr_out_key,
          accept
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_port_acceptable_frames_set()", out_port_ndx, 0);
}

/*********************************************************************
*     Sets acceptable frame type on outgoing port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_port_acceptable_frames_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                           out_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_PARSE_INFO                      *eg_prsr_out_key,
    SOC_SAND_OUT uint8                          *accept
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_PORT_ACCEPTABLE_FRAMES_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(eg_prsr_out_key);
  SOC_SAND_CHECK_NULL_INPUT(accept);

  res = soc_pb_pp_eg_filter_port_acceptable_frames_get_verify(
          unit,
          out_port_ndx,
          eg_prsr_out_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_eg_filter_port_acceptable_frames_get_unsafe(
          unit,
          out_port_ndx,
          eg_prsr_out_key,
          accept
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_port_acceptable_frames_get()", out_port_ndx, 0);
}

/*********************************************************************
*     Sets acceptable frame type for PEP port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_pep_acceptable_frames_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PP_PEP_KEY                      *pep_key,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT vlan_format_ndx,
    SOC_SAND_IN  uint8                          accept
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_PEP_ACCEPTABLE_FRAMES_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(pep_key);

  res = soc_pb_pp_eg_filter_pep_acceptable_frames_set_verify(
          unit,
          pep_key,
          vlan_format_ndx,
          accept
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_eg_filter_pep_acceptable_frames_set_unsafe(
          unit,
          pep_key,
          vlan_format_ndx,
          accept
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_pep_acceptable_frames_set()", 0, 0);
}

/*********************************************************************
*     Sets acceptable frame type for PEP port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_pep_acceptable_frames_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PP_PEP_KEY                      *pep_key,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT vlan_format_ndx,
    SOC_SAND_OUT uint8                          *accept
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_PEP_ACCEPTABLE_FRAMES_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(pep_key);
  SOC_SAND_CHECK_NULL_INPUT(accept);

  res = soc_pb_pp_eg_filter_pep_acceptable_frames_get_verify(
          unit,
          pep_key,
          vlan_format_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_eg_filter_pep_acceptable_frames_get_unsafe(
          unit,
          pep_key,
          vlan_format_ndx,
          accept
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_pep_acceptable_frames_get()", 0, 0);
}

/*********************************************************************
*     Set the Private VLAN (PVLAN) port type.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_pvlan_port_type_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                       *src_sys_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE    pvlan_port_type
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(src_sys_port_ndx);
  res = soc_pb_pp_eg_filter_pvlan_port_type_set_verify(
          unit,
          src_sys_port_ndx,
          pvlan_port_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_eg_filter_pvlan_port_type_set_unsafe(
          unit,
          src_sys_port_ndx,
          pvlan_port_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_pvlan_port_type_set()", 0, 0);
}

/*********************************************************************
*     Set the Private VLAN (PVLAN) port type.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_pvlan_port_type_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                       *src_sys_port_ndx,
    SOC_SAND_OUT SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE    *pvlan_port_type
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(src_sys_port_ndx);
  SOC_SAND_CHECK_NULL_INPUT(pvlan_port_type);

  res = soc_pb_pp_eg_filter_pvlan_port_type_get_verify(
          unit,
          src_sys_port_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_eg_filter_pvlan_port_type_get_unsafe(
          unit,
          src_sys_port_ndx,
          pvlan_port_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_pvlan_port_type_get()", 0, 0);
}

/*********************************************************************
*     Sets the orientation of out-AC, hub or spoke.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_split_horizon_out_ac_orientation_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PP_AC_ID                        out_ac_ndx,
    SOC_SAND_IN  SOC_SAND_PP_HUB_SPOKE_ORIENTATION      orientation
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_pb_pp_eg_filter_split_horizon_out_ac_orientation_set_verify(
          unit,
          out_ac_ndx,
          orientation
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_eg_filter_split_horizon_out_ac_orientation_set_unsafe(
          unit,
          out_ac_ndx,
          orientation
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_split_horizon_out_ac_orientation_set()", out_ac_ndx, 0);
}

/*********************************************************************
*     Sets the orientation of out-AC, hub or spoke.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_split_horizon_out_ac_orientation_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PP_AC_ID                        out_ac_ndx,
    SOC_SAND_OUT SOC_SAND_PP_HUB_SPOKE_ORIENTATION      *orientation
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_SPLIT_HORIZON_OUT_AC_ORIENTATION_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(orientation);

  res = soc_pb_pp_eg_filter_split_horizon_out_ac_orientation_get_verify(
          unit,
          out_ac_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_eg_filter_split_horizon_out_ac_orientation_get_unsafe(
          unit,
          out_ac_ndx,
          orientation
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_split_horizon_out_ac_orientation_get()", out_ac_ndx, 0);
}


/*********************************************************************
* Not supported for Soc_petraB 
* use  soc_pb_pp_eg_filter_split_horizon_out_ac_orientation_set
*     Sets the orientation of out-lif, hub or spoke.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_split_horizon_out_lif_orientation_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                        lif_eep_ndx,
    SOC_SAND_IN  SOC_SAND_PP_HUB_SPOKE_ORIENTATION      orientation
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_SPLIT_HORIZON_OUT_LIF_ORIENTATION_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  res = soc_pb_pp_eg_filter_split_horizon_out_lif_orientation_set_verify(
          unit,
          lif_eep_ndx,
          orientation
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_eg_filter_split_horizon_out_lif_orientation_set_unsafe(
          unit,
          lif_eep_ndx,
          orientation
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_split_horizon_out_lif_orientation_set()", lif_eep_ndx, 0);
}

/*********************************************************************
* Not supported for Soc_petraB 
* use  soc_pb_pp_eg_filter_split_horizon_out_ac_orientation_get
*     Sets the orientation of out-lif, hub or spoke.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_split_horizon_out_lif_orientation_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                         lif_eep_ndx,
    SOC_SAND_OUT SOC_SAND_PP_HUB_SPOKE_ORIENTATION      *orientation
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_EG_FILTER_SPLIT_HORIZON_OUT_LIF_ORIENTATION_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(orientation);

  res = soc_pb_pp_eg_filter_split_horizon_out_lif_orientation_get_verify(
          unit,
          lif_eep_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = soc_pb_pp_eg_filter_split_horizon_out_lif_orientation_get_unsafe(
          unit,
          lif_eep_ndx,
          orientation
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_eg_filter_split_horizon_out_lif_orientation_get()", lif_eep_ndx, 0);
}

void
  SOC_PB_PP_EG_FILTER_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_EG_FILTER_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_FILTER_PORT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if SOC_PB_PP_DEBUG_IS_LVL1

const char*
  SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE enum_val
  )
{
  return SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_to_string(enum_val);
}

const char*
  SOC_PB_PP_EG_FILTER_PORT_ENABLE_to_string(
    SOC_SAND_IN  SOC_PB_PP_EG_FILTER_PORT_ENABLE enum_val
  )
{
  return SOC_PPC_EG_FILTER_PORT_ENABLE_to_string(enum_val);
}

void
  SOC_PB_PP_EG_FILTER_PORT_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_EG_FILTER_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_EG_FILTER_PORT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* SOC_PB_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

