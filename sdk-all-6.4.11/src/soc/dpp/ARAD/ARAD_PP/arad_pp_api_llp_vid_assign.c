#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* $Id: arad_pp_api_llp_vid_assign.c,v 1.6 Broadcom SDK $
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

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_LLP

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
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_llp_vid_assign.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_llp_vid_assign.h>

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
*     Sets port information for VID assignment, including PVID
 *     and which assignment mechanism to enable for this port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_llp_vid_assign_port_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  ARAD_PP_LLP_VID_ASSIGN_PORT_INFO            *port_vid_assign_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_VID_ASSIGN_PORT_INFO_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(port_vid_assign_info);

  res = arad_pp_llp_vid_assign_port_info_set_verify(
          unit,
          local_port_ndx,
          port_vid_assign_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_llp_vid_assign_port_info_set_unsafe(
          unit,
          core_id,
          local_port_ndx,
          port_vid_assign_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_vid_assign_port_info_set()", local_port_ndx, 0);
}

/*********************************************************************
*     Sets port information for VID assignment, including PVID
 *     and which assignment mechanism to enable for this port.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_llp_vid_assign_port_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx,
    SOC_SAND_OUT ARAD_PP_LLP_VID_ASSIGN_PORT_INFO            *port_vid_assign_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_VID_ASSIGN_PORT_INFO_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(port_vid_assign_info);

  res = arad_pp_llp_vid_assign_port_info_get_verify(
          unit,
          local_port_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_llp_vid_assign_port_info_get_unsafe(
          unit,
          core_id,
          local_port_ndx,
          port_vid_assign_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_vid_assign_port_info_get()", local_port_ndx, 0);
}

/*********************************************************************
*     Set assignment to VLAN ID according to source MAC
 *     address.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_llp_vid_assign_mac_based_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *mac_address_key,
    SOC_SAND_IN  ARAD_PP_LLP_VID_ASSIGN_MAC_INFO             *mac_based_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_VID_ASSIGN_MAC_BASED_ADD);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mac_address_key);
  SOC_SAND_CHECK_NULL_INPUT(mac_based_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_llp_vid_assign_mac_based_add_verify(
          unit,
          mac_address_key,
          mac_based_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_llp_vid_assign_mac_based_add_unsafe(
          unit,
          mac_address_key,
          mac_based_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_vid_assign_mac_based_add()", 0, 0);
}

/*********************************************************************
*     Remove assignment to VLAN ID according to source MAC
 *     address.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_llp_vid_assign_mac_based_remove(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *mac_address_key
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_VID_ASSIGN_MAC_BASED_REMOVE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mac_address_key);

  res = arad_pp_llp_vid_assign_mac_based_remove_verify(
          unit,
          mac_address_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_llp_vid_assign_mac_based_remove_unsafe(
          unit,
          mac_address_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_vid_assign_mac_based_remove()", 0, 0);
}

/*********************************************************************
*     Get VLAN ID assignment information according to source
 *     MAC address.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_llp_vid_assign_mac_based_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *mac_address_key,
    SOC_SAND_OUT ARAD_PP_LLP_VID_ASSIGN_MAC_INFO             *mac_based_info,
    SOC_SAND_OUT uint8                                 *found
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_VID_ASSIGN_MAC_BASED_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(mac_address_key);
  SOC_SAND_CHECK_NULL_INPUT(mac_based_info);
  SOC_SAND_CHECK_NULL_INPUT(found);

  res = arad_pp_llp_vid_assign_mac_based_get_verify(
          unit,
          mac_address_key
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_llp_vid_assign_mac_based_get_unsafe(
          unit,
          mac_address_key,
          mac_based_info,
          found
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_vid_assign_mac_based_get()", 0, 0);
}

/*********************************************************************
*     Get VLAN ID assignment information according to source
 *     MAC address.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_llp_vid_assign_mac_based_get_block(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_LLP_VID_ASSIGN_MATCH_RULE           *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range,
    SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS                       *mac_address_key_arr,
    SOC_SAND_OUT ARAD_PP_LLP_VID_ASSIGN_MAC_INFO             *vid_assign_info_arr,
    SOC_SAND_OUT uint32                                  *nof_entries
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_VID_ASSIGN_MAC_BASED_GET_BLOCK);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(rule);
  SOC_SAND_CHECK_NULL_INPUT(block_range);
  SOC_SAND_CHECK_NULL_INPUT(mac_address_key_arr);
  SOC_SAND_CHECK_NULL_INPUT(vid_assign_info_arr);
  SOC_SAND_CHECK_NULL_INPUT(nof_entries);

  res = arad_pp_llp_vid_assign_mac_based_get_block_verify(
          unit,
          rule,
          block_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_llp_vid_assign_mac_based_get_block_unsafe(
          unit,
          rule,
          block_range,
          mac_address_key_arr,
          vid_assign_info_arr,
          nof_entries
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_vid_assign_mac_based_get_block()", 0, 0);
}

/*********************************************************************
*     Set assignment of VLAN ID based on source IPv4 subnet.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_llp_vid_assign_ipv4_subnet_based_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET                       *subnet,
    SOC_SAND_IN  ARAD_PP_LLP_VID_ASSIGN_IPV4_SUBNET_INFO     *subnet_based_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_VID_ASSIGN_IPV4_SUBNET_BASED_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(subnet);
  SOC_SAND_CHECK_NULL_INPUT(subnet_based_info);

  res = arad_pp_llp_vid_assign_ipv4_subnet_based_set_verify(
          unit,
          entry_ndx,
          subnet,
          subnet_based_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_llp_vid_assign_ipv4_subnet_based_set_unsafe(
          unit,
          entry_ndx,
          subnet,
          subnet_based_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_vid_assign_ipv4_subnet_based_set()", entry_ndx, 0);
}

/*********************************************************************
*     Set assignment of VLAN ID based on source IPv4 subnet.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_llp_vid_assign_ipv4_subnet_based_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_OUT SOC_SAND_PP_IPV4_SUBNET                       *subnet,
    SOC_SAND_OUT ARAD_PP_LLP_VID_ASSIGN_IPV4_SUBNET_INFO     *subnet_based_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_VID_ASSIGN_IPV4_SUBNET_BASED_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(subnet);
  SOC_SAND_CHECK_NULL_INPUT(subnet_based_info);

  res = arad_pp_llp_vid_assign_ipv4_subnet_based_get_verify(
          unit,
          entry_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_llp_vid_assign_ipv4_subnet_based_get_unsafe(
          unit,
          entry_ndx,
          subnet,
          subnet_based_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_vid_assign_ipv4_subnet_based_get()", entry_ndx, 0);
}

/*********************************************************************
*     Sets mapping from Ethernet Type and profile to VID.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_llp_vid_assign_protocol_based_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  uint16                                  ether_type_ndx,
    SOC_SAND_IN  ARAD_PP_LLP_VID_ASSIGN_PRTCL_INFO           *prtcl_assign_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_VID_ASSIGN_PROTOCOL_BASED_SET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(prtcl_assign_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  res = arad_pp_llp_vid_assign_protocol_based_set_verify(
          unit,
          port_profile_ndx,
          ether_type_ndx,
          prtcl_assign_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_llp_vid_assign_protocol_based_set_unsafe(
          unit,
          port_profile_ndx,
          ether_type_ndx,
          prtcl_assign_info,
          success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_vid_assign_protocol_based_set()", port_profile_ndx, ether_type_ndx);
}

/*********************************************************************
*     Sets mapping from Ethernet Type and profile to VID.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_llp_vid_assign_protocol_based_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  uint16                                  ether_type_ndx,
    SOC_SAND_OUT ARAD_PP_LLP_VID_ASSIGN_PRTCL_INFO           *prtcl_assign_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_LLP_VID_ASSIGN_PROTOCOL_BASED_GET);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;

  SOC_SAND_CHECK_NULL_INPUT(prtcl_assign_info);

  res = arad_pp_llp_vid_assign_protocol_based_get_verify(
          unit,
          port_profile_ndx,
          ether_type_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_TAKE_DEVICE_SEMAPHORE;

  res = arad_pp_llp_vid_assign_protocol_based_get_unsafe(
          unit,
          port_profile_ndx,
          ether_type_ndx,
          prtcl_assign_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit_semaphore);

exit_semaphore:
  SOC_SAND_GIVE_DEVICE_SEMAPHORE;
  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_llp_vid_assign_protocol_based_get()", port_profile_ndx, ether_type_ndx);
}

void
  ARAD_PP_LLP_VID_ASSIGN_PORT_INFO_clear(
    SOC_SAND_OUT ARAD_PP_LLP_VID_ASSIGN_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_VID_ASSIGN_PORT_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_LLP_VID_ASSIGN_MAC_INFO_clear(
    SOC_SAND_OUT ARAD_PP_LLP_VID_ASSIGN_MAC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_VID_ASSIGN_MAC_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_LLP_VID_ASSIGN_IPV4_SUBNET_INFO_clear(
    SOC_SAND_OUT ARAD_PP_LLP_VID_ASSIGN_IPV4_SUBNET_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_VID_ASSIGN_IPV4_SUBNET_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_LLP_VID_ASSIGN_PRTCL_INFO_clear(
    SOC_SAND_OUT ARAD_PP_LLP_VID_ASSIGN_PRTCL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_VID_ASSIGN_PRTCL_INFO_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_LLP_VID_ASSIGN_MATCH_RULE_clear(
    SOC_SAND_OUT ARAD_PP_LLP_VID_ASSIGN_MATCH_RULE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE_clear(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#if ARAD_PP_DEBUG_IS_LVL1

const char*
  ARAD_PP_LLP_VID_ASSIGN_MATCH_RULE_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_LLP_VID_ASSIGN_MATCH_RULE_TYPE enum_val
  )
{
  return SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE_TYPE_to_string(enum_val);
}

void
  ARAD_PP_LLP_VID_ASSIGN_PORT_INFO_print(
    SOC_SAND_IN  ARAD_PP_LLP_VID_ASSIGN_PORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_VID_ASSIGN_PORT_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_LLP_VID_ASSIGN_MAC_INFO_print(
    SOC_SAND_IN  ARAD_PP_LLP_VID_ASSIGN_MAC_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_VID_ASSIGN_MAC_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_LLP_VID_ASSIGN_IPV4_SUBNET_INFO_print(
    SOC_SAND_IN  ARAD_PP_LLP_VID_ASSIGN_IPV4_SUBNET_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_VID_ASSIGN_IPV4_SUBNET_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_LLP_VID_ASSIGN_PRTCL_INFO_print(
    SOC_SAND_IN  ARAD_PP_LLP_VID_ASSIGN_PRTCL_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_VID_ASSIGN_PRTCL_INFO_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

void
  ARAD_PP_LLP_VID_ASSIGN_MATCH_RULE_print(
    SOC_SAND_IN  ARAD_PP_LLP_VID_ASSIGN_MATCH_RULE *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE_print(info);
exit:
  SOC_SAND_VOID_EXIT_AND_SEND_ERROR(0, 0, 0);
}

#endif /* ARAD_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif /* of #if defined(BCM_88650_A0) */

