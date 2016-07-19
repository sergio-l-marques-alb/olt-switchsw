/* $Id: soc_pb_pp_frwrd_ipv4.h,v 1.6 Broadcom SDK $
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

#ifndef __SOC_PB_PP_FRWRD_IPV4_INCLUDED__
/* { */
#define __SOC_PB_PP_FRWRD_IPV4_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_api_frwrd_ipv4.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_lem_access.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/* } */
/*************
 * MACROS    *
 *************/
/* { */

/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PB_PP_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_SET = SOC_PB_PP_PROC_DESC_BASE_FRWRD_IPV4_FIRST,
  SOC_PB_PP_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_SET_PRINT,
  SOC_PB_PP_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_SET_UNSAFE,
  SOC_PB_PP_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_SET_VERIFY,
  SOC_PB_PP_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_GET,
  SOC_PB_PP_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_GET_PRINT,
  SOC_PB_PP_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_GET_VERIFY,
  SOC_PB_PP_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_GET_UNSAFE,
  SOC_PB_PP_FRWRD_IP_ROUTES_CACHE_COMMIT,
  SOC_PB_PP_FRWRD_IP_ROUTES_CACHE_COMMIT_PRINT,
  SOC_PB_PP_FRWRD_IP_ROUTES_CACHE_COMMIT_UNSAFE,
  SOC_PB_PP_FRWRD_IP_ROUTES_CACHE_COMMIT_VERIFY,
  SOC_PB_PP_FRWRD_IPV4_GLBL_INFO_SET,
  SOC_PB_PP_FRWRD_IPV4_GLBL_INFO_SET_PRINT,
  SOC_PB_PP_FRWRD_IPV4_GLBL_INFO_SET_UNSAFE,
  SOC_PB_PP_FRWRD_IPV4_GLBL_INFO_SET_VERIFY,
  SOC_PB_PP_FRWRD_IPV4_GLBL_INFO_GET,
  SOC_PB_PP_FRWRD_IPV4_GLBL_INFO_GET_PRINT,
  SOC_PB_PP_FRWRD_IPV4_GLBL_INFO_GET_VERIFY,
  SOC_PB_PP_FRWRD_IPV4_GLBL_INFO_GET_UNSAFE,
  SOC_PB_PP_FRWRD_IPV4_UC_ROUTE_ADD,
  SOC_PB_PP_FRWRD_IPV4_UC_ROUTE_ADD_PRINT,
  SOC_PB_PP_FRWRD_IPV4_UC_ROUTE_ADD_UNSAFE,
  SOC_PB_PP_FRWRD_IPV4_UC_ROUTE_ADD_VERIFY,
  SOC_PB_PP_FRWRD_IPV4_UC_ROUTE_GET,
  SOC_PB_PP_FRWRD_IPV4_UC_ROUTE_GET_PRINT,
  SOC_PB_PP_FRWRD_IPV4_UC_ROUTE_GET_UNSAFE,
  SOC_PB_PP_FRWRD_IPV4_UC_ROUTE_GET_VERIFY,
  SOC_PB_PP_FRWRD_IPV4_UC_ROUTE_GET_BLOCK,
  SOC_PB_PP_FRWRD_IPV4_UC_ROUTE_GET_BLOCK_PRINT,
  SOC_PB_PP_FRWRD_IPV4_UC_ROUTE_GET_BLOCK_UNSAFE,
  SOC_PB_PP_FRWRD_IPV4_UC_ROUTE_GET_BLOCK_VERIFY,
  SOC_PB_PP_FRWRD_IPV4_UC_ROUTE_REMOVE,
  SOC_PB_PP_FRWRD_IPV4_UC_ROUTE_REMOVE_PRINT,
  SOC_PB_PP_FRWRD_IPV4_UC_ROUTE_REMOVE_UNSAFE,
  SOC_PB_PP_FRWRD_IPV4_UC_ROUTE_REMOVE_VERIFY,
  SOC_PB_PP_FRWRD_IPV4_UC_ROUTING_TABLE_CLEAR,
  SOC_PB_PP_FRWRD_IPV4_UC_ROUTING_TABLE_CLEAR_PRINT,
  SOC_PB_PP_FRWRD_IPV4_UC_ROUTING_TABLE_CLEAR_UNSAFE,
  SOC_PB_PP_FRWRD_IPV4_UC_ROUTING_TABLE_CLEAR_VERIFY,
  SOC_PB_PP_FRWRD_IPV4_HOST_ADD,
  SOC_PB_PP_FRWRD_IPV4_HOST_ADD_PRINT,
  SOC_PB_PP_FRWRD_IPV4_HOST_ADD_UNSAFE,
  SOC_PB_PP_FRWRD_IPV4_HOST_ADD_VERIFY,
  SOC_PB_PP_FRWRD_IPV4_HOST_GET,
  SOC_PB_PP_FRWRD_IPV4_HOST_GET_PRINT,
  SOC_PB_PP_FRWRD_IPV4_HOST_GET_UNSAFE,
  SOC_PB_PP_FRWRD_IPV4_HOST_GET_VERIFY,
  SOC_PB_PP_FRWRD_IPV4_HOST_GET_BLOCK,
  SOC_PB_PP_FRWRD_IPV4_HOST_GET_BLOCK_PRINT,
  SOC_PB_PP_FRWRD_IPV4_HOST_GET_BLOCK_UNSAFE,
  SOC_PB_PP_FRWRD_IPV4_HOST_GET_BLOCK_VERIFY,
  SOC_PB_PP_FRWRD_IPV4_HOST_REMOVE,
  SOC_PB_PP_FRWRD_IPV4_HOST_REMOVE_PRINT,
  SOC_PB_PP_FRWRD_IPV4_HOST_REMOVE_UNSAFE,
  SOC_PB_PP_FRWRD_IPV4_HOST_REMOVE_VERIFY,
  SOC_PB_PP_FRWRD_IPV4_MC_ROUTE_ADD,
  SOC_PB_PP_FRWRD_IPV4_MC_ROUTE_ADD_PRINT,
  SOC_PB_PP_FRWRD_IPV4_MC_ROUTE_ADD_UNSAFE,
  SOC_PB_PP_FRWRD_IPV4_MC_ROUTE_ADD_VERIFY,
  SOC_PB_PP_FRWRD_IPV4_MC_ROUTE_GET,
  SOC_PB_PP_FRWRD_IPV4_MC_ROUTE_GET_PRINT,
  SOC_PB_PP_FRWRD_IPV4_MC_ROUTE_GET_UNSAFE,
  SOC_PB_PP_FRWRD_IPV4_MC_ROUTE_GET_VERIFY,
  SOC_PB_PP_FRWRD_IPV4_MC_ROUTE_GET_BLOCK,
  SOC_PB_PP_FRWRD_IPV4_MC_ROUTE_GET_BLOCK_PRINT,
  SOC_PB_PP_FRWRD_IPV4_MC_ROUTE_GET_BLOCK_UNSAFE,
  SOC_PB_PP_FRWRD_IPV4_MC_ROUTE_GET_BLOCK_VERIFY,
  SOC_PB_PP_FRWRD_IPV4_MC_ROUTE_REMOVE,
  SOC_PB_PP_FRWRD_IPV4_MC_ROUTE_REMOVE_PRINT,
  SOC_PB_PP_FRWRD_IPV4_MC_ROUTE_REMOVE_UNSAFE,
  SOC_PB_PP_FRWRD_IPV4_MC_ROUTE_REMOVE_VERIFY,
  SOC_PB_PP_FRWRD_IPV4_MC_ROUTING_TABLE_CLEAR,
  SOC_PB_PP_FRWRD_IPV4_MC_ROUTING_TABLE_CLEAR_PRINT,
  SOC_PB_PP_FRWRD_IPV4_MC_ROUTING_TABLE_CLEAR_UNSAFE,
  SOC_PB_PP_FRWRD_IPV4_MC_ROUTING_TABLE_CLEAR_VERIFY,
  SOC_PB_PP_FRWRD_IPV4_VRF_INFO_SET,
  SOC_PB_PP_FRWRD_IPV4_VRF_INFO_SET_PRINT,
  SOC_PB_PP_FRWRD_IPV4_VRF_INFO_SET_UNSAFE,
  SOC_PB_PP_FRWRD_IPV4_VRF_INFO_SET_VERIFY,
  SOC_PB_PP_FRWRD_IPV4_VRF_INFO_GET,
  SOC_PB_PP_FRWRD_IPV4_VRF_INFO_GET_PRINT,
  SOC_PB_PP_FRWRD_IPV4_VRF_INFO_GET_VERIFY,
  SOC_PB_PP_FRWRD_IPV4_VRF_INFO_GET_UNSAFE,
  SOC_PB_PP_FRWRD_IPV4_VRF_ROUTE_ADD,
  SOC_PB_PP_FRWRD_IPV4_VRF_ROUTE_ADD_PRINT,
  SOC_PB_PP_FRWRD_IPV4_VRF_ROUTE_ADD_UNSAFE,
  SOC_PB_PP_FRWRD_IPV4_VRF_ROUTE_ADD_VERIFY,
  SOC_PB_PP_FRWRD_IPV4_VRF_IS_SUPPORTED_VERIFY,
  SOC_PB_PP_FRWRD_IPV4_VRF_ROUTE_GET,
  SOC_PB_PP_FRWRD_IPV4_VRF_ROUTE_GET_PRINT,
  SOC_PB_PP_FRWRD_IPV4_VRF_ROUTE_GET_UNSAFE,
  SOC_PB_PP_FRWRD_IPV4_VRF_ROUTE_GET_VERIFY,
  SOC_PB_PP_FRWRD_IPV4_VRF_ROUTE_GET_BLOCK,
  SOC_PB_PP_FRWRD_IPV4_VRF_ROUTE_GET_BLOCK_PRINT,
  SOC_PB_PP_FRWRD_IPV4_VRF_ROUTE_GET_BLOCK_UNSAFE,
  SOC_PB_PP_FRWRD_IPV4_VRF_ROUTE_GET_BLOCK_VERIFY,
  SOC_PB_PP_FRWRD_IPV4_VRF_ROUTE_REMOVE,
  SOC_PB_PP_FRWRD_IPV4_VRF_ROUTE_REMOVE_PRINT,
  SOC_PB_PP_FRWRD_IPV4_VRF_ROUTE_REMOVE_UNSAFE,
  SOC_PB_PP_FRWRD_IPV4_VRF_ROUTE_REMOVE_VERIFY,
  SOC_PB_PP_FRWRD_IPV4_VRF_ROUTING_TABLE_CLEAR,
  SOC_PB_PP_FRWRD_IPV4_VRF_ROUTING_TABLE_CLEAR_PRINT,
  SOC_PB_PP_FRWRD_IPV4_VRF_ROUTING_TABLE_CLEAR_UNSAFE,
  SOC_PB_PP_FRWRD_IPV4_VRF_ROUTING_TABLE_CLEAR_VERIFY,
  SOC_PB_PP_FRWRD_IPV4_VRF_ALL_ROUTING_TABLES_CLEAR,
  SOC_PB_PP_FRWRD_IPV4_VRF_ALL_ROUTING_TABLES_CLEAR_PRINT,
  SOC_PB_PP_FRWRD_IPV4_VRF_ALL_ROUTING_TABLES_CLEAR_UNSAFE,
  SOC_PB_PP_FRWRD_IPV4_VRF_ALL_ROUTING_TABLES_CLEAR_VERIFY,
  SOC_PB_PP_FRWRD_IPV4_MEM_STATUS_GET,
  SOC_PB_PP_FRWRD_IPV4_MEM_STATUS_GET_UNSAFE,
  SOC_PB_PP_FRWRD_IPV4_MEM_STATUS_GET_VERIFY,
  SOC_PB_PP_FRWRD_IPV4_MEM_DEFRAGE,
  SOC_PB_PP_FRWRD_IPV4_MEM_DEFRAGE_UNSAFE,
  SOC_PB_PP_FRWRD_IPV4_MEM_DEFRAGE_VERIFY,
  SOC_PB_PP_FRWRD_IPV4_GET_PROCS_PTR,
  SOC_PB_PP_FRWRD_IPV4_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */
  SOC_PB_PP_FRWRD_IPV4_DEF_ACTION_SET,
  SOC_PB_PP_FRWRD_IPV4_L3_VPN_DEFAULT_ROUTING_ENABLE_SET,
  SOC_PB_PP_FRWRD_IPV4_L3_VPN_DEFAULT_ROUTING_ENABLE_GET,
  SOC_PB_PP_FRWRD_IPV4_LPM_ROUTE_ADD,
  SOC_PB_PP_FRWRD_IPV4_LPM_ROUTE_IS_EXIST,
  SOC_PB_PP_FRWRD_IPV4_LPM_ROUTE_CLEAR,
  SOC_PB_PP_FRWRD_IPV4_LPM_ROUTE_REMOVE,
  SOC_PB_PP_FRWRD_IPV4_LPM_ROUTE_GET,
  SOC_PB_PP_FRWRD_IPV4_LPM_ROUTE_GET_BLOCK,
  SOC_PB_PP_FRWRD_IPV4_LEM_ROUTE_ADD,
  SOC_PB_PP_FRWRD_IPV4_LEM_ROUTE_GET,
  SOC_PB_PP_FRWRD_IPV4_LEM_ROUTE_REMOVE,
  SOC_PB_PP_FRWRD_IPV4_UC_OR_VPN_ROUTE_ADD,
  SOC_PB_PP_FRWRD_IPV4_UC_OR_VPN_ROUTE_GET,
  SOC_PB_PP_FRWRD_IPV4_UC_OR_VPN_ROUTE_GET_BLOCK,
  SOC_PB_PP_FRWRD_IPV4_UC_OR_VPN_ROUTE_REMOVE,
  SOC_PB_PP_FRWRD_IPV4_UC_OR_VPN_ROUTING_TABLE_CLEAR,

/* $Id: soc_pb_pp_frwrd_ipv4.h,v 1.6 Broadcom SDK $
   * Last element. Do no touch.
   */
  SOC_PB_PP_FRWRD_IPV4_PROCEDURE_DESC_LAST
} SOC_PB_PP_FRWRD_IPV4_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PB_PP_FRWRD_IPV4_ROUTE_TYPES_OUT_OF_RANGE_ERR = SOC_PB_PP_ERR_DESC_BASE_FRWRD_IPV4_FIRST,
  SOC_PB_PP_FRWRD_IPV4_SUCCESS_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_IPV4_ROUTE_STATUS_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_IPV4_LOCATION_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_IPV4_FOUND_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_IPV4_ROUTES_STATUS_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_IPV4_ROUTES_LOCATION_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_IPV4_NOF_ENTRIES_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_IPV4_EXACT_MATCH_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_IPV4_MEM_NDX_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_IPV4_ROUTE_VAL_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_IPV4_TYPE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_IPV4_UC_TABLE_RESOUCES_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_IPV4_MC_TABLE_RESOUCES_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_IP_CACHE_MODE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_IPV4_GROUP_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_IPV4_EEP_OUT_OF_RANGE_ERR,
  /*
   * } Auto generated. Do not edit previous section.
   */
  SOC_PB_PP_FRWRD_IPV4_MC_ILLEGAL_DEST_TYPE_ERR,
  SOC_PB_PP_FRWRD_IPV4_MUST_ALL_VRF_ERR,
  SOC_PB_PP_FRWRD_IPV4_CACHE_NOT_SUPPORTED_ERR,
  SOC_PB_PP_FRWRD_IPV4_DEFRAGE_NOT_SUPPORTED_ERR,

  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_FRWRD_IPV4_ERR_LAST
} SOC_PB_PP_FRWRD_IPV4_ERR;

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
  soc_pb_pp_frwrd_ipv4_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ip_routes_cache_mode_enable_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set for which IP routes (IPv4/6 UC/MC) to enable caching
 *   by SW
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx -
 *     Virtual router id. Range: 0 - 255. Use VRF 0 for default
 *     routing table. Use SOC_PPD_FRWRD_IP_ALL_VRFS_ID to apply
 *     setting to all VRFs.
 *   SOC_SAND_IN  uint32                                  route_types -
 *     IP Routes to enable caching for them, use
 *     SOC_SAND_PP_IP_TYPE to select which IP types to enable
 *     caching for them.
 * REMARKS:
 *   - Caching is supported only For IPv4 UC.- For route
 *   types which caching was enabled for them, the Add/remove
 *   operation will affect the SW only.- when cache is
 *   disabled then routes already cached will not be commited
 *   (till calling soc_ppd_frwrd_ip_routes_cache_commit) - use
 *   soc_ppd_frwrd_ip_routes_commit() to synchronize the HW with
 *   the changes made in the SW.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ip_routes_cache_mode_enable_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  uint32                                  route_types
  );

uint32
  soc_pb_pp_frwrd_ip_routes_cache_mode_enable_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  uint32                                  route_types
  );

uint32
  soc_pb_pp_frwrd_ip_routes_cache_mode_enable_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_frwrd_ip_routes_cache_mode_enable_set_unsafe"
 *     API.
 *     Refer to
 *     "soc_pb_pp_frwrd_ip_routes_cache_mode_enable_set_unsafe" API
 *     for details.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ip_routes_cache_mode_enable_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx,
    SOC_SAND_OUT uint32                                  *route_types
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ip_routes_cache_commit_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Synchronize the routing table in the Data plane (HW)
 *   with the routing table in the control plane (SW)
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  route_types -
 *     IP Routes to commit changes for them. use
 *     SOC_SAND_PP_IP_TYPE to select which IP types to commit into
 *     Hardware
 *   SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx -
 *     Virtual router id. Range: 0 - 255. Use VRF 0 for default
 *     routing table. Use SOC_PPD_FRWRD_IP_ALL_VRFS_ID to apply
 *     setting to all VRFs.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success -
 *     Commit may fail if LPM or Host DB (LEM) has no available
 *     entries. In order to examine what is the status of the
 *     SW/HW DBs use soc_ppd_frwrd_ipv4_uc_route_get_block() (or
 *     VRF/MC/IPv6 relevant get_block functions)
 * REMARKS:
 *   - When user select to update the SW only (using
 *   soc_ppd_frwrd_ip_routes_cache_mode_set), this API comes to
 *   synchronize the HW with the changes made in the SW.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ip_routes_cache_commit_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  route_types,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  soc_pb_pp_frwrd_ip_routes_cache_commit_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  route_types,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ipv4_glbl_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Setting global information of the IP routing (including
 *   resources to use)
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_GLBL_INFO                *glbl_info -
 *     Global information.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv4_glbl_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_GLBL_INFO                *glbl_info
  );

uint32
  soc_pb_pp_frwrd_ipv4_glbl_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_GLBL_INFO                *glbl_info
  );

uint32
  soc_pb_pp_frwrd_ipv4_glbl_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_frwrd_ipv4_glbl_info_set_unsafe" API.
 *     Refer to "soc_pb_pp_frwrd_ipv4_glbl_info_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv4_glbl_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IPV4_GLBL_INFO                *glbl_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ipv4_uc_route_add_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add IPv4 route entry to the routing table. Binds between
 *   Ipv4 Unicast route key (IPv4-address/prefix) and a FEC
 *   entry identified by fec_id for a given router. As a
 *   result of this operation, Unicast Ipv4 packets
 *   designated to the IP address matching the given key (as
 *   long there is no more-specific route key) will be routed
 *   according to the information in the FEC entry identified
 *   by fec_id.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_UC_ROUTE_KEY             *route_key -
 *     The routing key: IPv4 subnet
 *   SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_id -
 *     FEC ID.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success -
 *     Whether the operation succeeds. Add operation may fail
 *     if there is no place in the routing DB (LEM/LPM).
 * REMARKS:
 *   - use soc_ppd_frwrd_ip_routes_cache_mode_enable_set() to
 *   determine wehter to cache this route in SW or to write
 *   it directly into HW.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv4_uc_route_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_UC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_id,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  soc_pb_pp_frwrd_ipv4_uc_route_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_UC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_id
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ipv4_uc_route_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets the routing information (system-fec-id) associated
 *   with the given route key.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_UC_ROUTE_KEY             *route_key -
 *     The routing key (IPv4 subnet and a virtual router id)
 *   SOC_SAND_IN  uint8                                 exact_match -
 *     If TRUE returns exact match only; if FALSE returns
 *     longest prefix match.
 *   SOC_SAND_OUT SOC_PB_PP_FEC_ID                              *fec_id -
 *     Routing information (system-fec-id).
 *   SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_STATUS               *route_status -
 *     Indicates whether the returned entry exist in HW
 *     (commited) or pending (either for remove or addition),
 *     relevant only if found is TRUE.
 *   SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_LOCATION             *location -
 *     Indicates whether the returned entry exsit in host table
 *     or LPM/TCAM. Relvant only if found is TRUE. If the entry
 *     is not commited yet then this is the location the route
 *     will be inserted into
 *   SOC_SAND_OUT uint8                                 *found -
 *     If TRUE then route_val has valid data.
 * REMARKS:
 *   - the lookup is performed in the SW shadow. - Found is
 *   always TRUE if exact_match is FALSE, due to the default
 *   route.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv4_uc_route_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_UC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  uint8                                 exact_match,
    SOC_SAND_OUT SOC_PB_PP_FEC_ID                              *fec_id,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_STATUS               *route_status,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_LOCATION             *location,
    SOC_SAND_OUT uint8                                 *found
  );

uint32
  soc_pb_pp_frwrd_ipv4_uc_route_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_UC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  uint8                                 exact_match
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ipv4_uc_route_get_block_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets the Ipv4 UC routing table.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_INOUT SOC_PB_PP_IP_ROUTING_TABLE_RANGE              *block_range -
 *     Defines the range and the order of the block of routing
 *     entries.
 *   SOC_SAND_OUT SOC_PB_PP_FRWRD_IPV4_UC_ROUTE_KEY             *route_keys -
 *     Array of routing keys (IPv4 subnets)
 *   SOC_SAND_OUT SOC_PB_PP_FEC_ID                              *fec_ids -
 *     Array of fec-ids.
 *   SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_STATUS               *routes_status -
 *     For each route Indicates whether it exists in HW
 *     (commited) or pending (either for remove or addition).
 *     Set this parameter to NULL in order to be ignored
 *   SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_LOCATION             *routes_location -
 *     For each route Indicates whether it exsits in host table
 *     or LPM/TCAM. If the entry is not commited yet then this
 *     is the location the route will be inserted into. Set
 *     this parameter to NULL in order to be ignored
 *   SOC_SAND_OUT uint32                                  *nof_entries -
 *     Number of entries in returned arrays.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv4_uc_route_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_PB_PP_IP_ROUTING_TABLE_RANGE              *block_range,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IPV4_UC_ROUTE_KEY             *route_keys,
    SOC_SAND_OUT SOC_PB_PP_FEC_ID                              *fec_ids,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_STATUS               *routes_status,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_LOCATION             *routes_location,
    SOC_SAND_OUT uint32                                  *nof_entries
  );

uint32
  soc_pb_pp_frwrd_ipv4_uc_route_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_PB_PP_IP_ROUTING_TABLE_RANGE              *block_range
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ipv4_uc_route_remove_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove entry from the routing table.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_UC_ROUTE_KEY             *route_key -
 *     The routing key (IPv4 subnet and a virtual router id)
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success -
 *     Whether the operation succeeds. Remove operation may
 *     fail if there is no place in the IPv4 DB (LPM), due to
 *     fail of decompression.
 * REMARKS:
 *   - use soc_ppd_frwrd_ip_routes_cache_mode_enable_set() to
 *   determine wehter to cache this route in SW or to write
 *   it directly into HW.- remove un-exist entry considered
 *   as succeeded operation
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv4_uc_route_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_UC_ROUTE_KEY             *route_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  soc_pb_pp_frwrd_ipv4_uc_route_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_UC_ROUTE_KEY             *route_key
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ipv4_uc_routing_table_clear_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Clear the IPv4 UC routing table.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv4_uc_routing_table_clear_unsafe(
    SOC_SAND_IN  int                                 unit
  );

uint32
  soc_pb_pp_frwrd_ipv4_uc_routing_table_clear_verify(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ipv4_host_add_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add IPv4 entry to the Host table. Binds between Host and
 *   next hop information.
 * INPUT:
 *   SOC_SAND_IN  int                             unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_HOST_KEY             *host_key -
 *     VRF-ID and IP address
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_HOST_ROUTE_INFO      *routing_info -
 *     Routing information. See remarks.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                  *success -
 *     Whether the operation succeeds. Add operation may fail
 *     if there is no place in the LEM DB.
 * REMARKS:
 *   - use soc_ppd_frwrd_ip_routes_cache_mode_enable_set() to
 *   determine wehter to cache this route in SW or to write
 *   it directly into HW. - Supports virtual routing tables
 *   using the vrf_id - user has to supply in routing info
 *   FEC-ptr which set the out-RIF, TM-destination and EEP.
 *   Optionally, user can supply EEP directly from the host
 *   table.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv4_host_add_unsafe(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_HOST_KEY             *host_key,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_HOST_ROUTE_INFO      *routing_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                  *success
  );

uint32
  soc_pb_pp_frwrd_ipv4_host_add_verify(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_HOST_KEY             *host_key,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_HOST_ROUTE_INFO      *routing_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ipv4_host_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets the routing information associated with the given
 *   route key on VRF.
 * INPUT:
 *   SOC_SAND_IN  int                             unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_HOST_KEY             *host_key -
 *     The routing key (IPv4 address and a virtual router id)
 *   SOC_SAND_OUT SOC_PB_PP_FRWRD_IPV4_HOST_ROUTE_INFO      *routing_info -
 *     Routing information (fec-id).
 *   SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_STATUS           *route_status -
 *     Indicates whether the returned entry exist in HW
 *     (commited) or pending (either for remove or addition),
 *     relevant only if found is TRUE.
 *   SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_LOCATION         *location -
 *     Indicates whether the returned entry exsit in host table
 *     or LPM/TCAM. Relvant only if found is TRUE. If the entry
 *     is not commited yet then this is the location the route
 *     will be inserted into
 *   SOC_SAND_OUT uint8                             *found -
 *     If TRUE then route_val has valid data.
 * REMARKS:
 *   - If the host is not found in the host DB, then the host
 *   (exact route/32) is lookuped in the LPM
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv4_host_get_unsafe(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_HOST_KEY             *host_key,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IPV4_HOST_ROUTE_INFO      *routing_info,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_STATUS           *route_status,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_LOCATION         *location,
    SOC_SAND_OUT uint8                             *found
  );

uint32
  soc_pb_pp_frwrd_ipv4_host_get_verify(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_HOST_KEY             *host_key
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ipv4_host_get_block_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets the host table.
 * INPUT:
 *   SOC_SAND_IN  int                             unit -
 *     Identifier of the device to access.
 *   SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                *block_range_key -
 *     Defines the range of routing entries to retrieve.
 *   SOC_SAND_OUT SOC_PB_PP_FRWRD_IPV4_HOST_KEY             *host_keys -
 *     array of routing keys
 *   SOC_SAND_OUT SOC_PB_PP_FRWRD_IPV4_HOST_ROUTE_INFO      *routes_info -
 *     Array of routing information for each host key.
 *   SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_STATUS           *routes_status -
 *     For each route Indicates whether it exists in HW
 *     (commited) or pending (either for remove or addition).
 *     Set this parameter to NULL in order to be ignored
 *   SOC_SAND_OUT uint32                              *nof_entries -
 *     Number of entries in returned Arrays.
 * REMARKS:
 *   - The lookup is performed in the SW shadow.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv4_host_get_block_unsafe(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                *block_range_key,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IPV4_HOST_KEY             *host_keys,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IPV4_HOST_ROUTE_INFO      *routes_info,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_STATUS           *routes_status,
    SOC_SAND_OUT uint32                              *nof_entries
  );

uint32
  soc_pb_pp_frwrd_ipv4_host_get_block_verify(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                *block_range_key
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ipv4_host_remove_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove IPv4 route entry from the routing table of a
 *   virtual router (VRF).
 * INPUT:
 *   SOC_SAND_IN  int                             unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_HOST_KEY             *host_key -
 *     The routing key (IPv4 subnet and a virtual router id)
 * REMARKS:
 *   - use soc_ppd_frwrd_ip_routes_cache_mode_enable_set() to
 *   determine wehter to cache this route in SW or to write
 *   it directly into HW.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv4_host_remove_unsafe(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_HOST_KEY             *host_key
  );

uint32
  soc_pb_pp_frwrd_ipv4_host_remove_verify(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_HOST_KEY             *host_key
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ipv4_mc_route_add_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add IPv4 MC route entry to the routing table. Binds
 *   between Ipv4 Unicast route key (IPv4-address/prefix) and
 *   a FEC entry identified by fec_id for a given router. As
 *   a result of this operation, Unicast Ipv4 packets
 *   designated to IP address matching the given key (as long
 *   there is no more-specific route key) will be routed
 *   according to the information in the FEC entry identified
 *   by fec_id.
 * INPUT:
 *   SOC_SAND_IN  int                             unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_MC_ROUTE_KEY         *route_key -
 *     The routing key IPv4 subnet
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_MC_ROUTE_INFO        *route_info -
 *     Routing information (FEC-id, or MC-group).
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                  *success -
 *     Whether the operation succeeds. Add operation may fail
 *     if there is no available place.
 * REMARKS:
 *   - use soc_ppd_frwrd_ip_routes_cache_mode_enable_set() to
 *   determine wehter to cache this route in SW or to write
 *   it directly into HW.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv4_mc_route_add_unsafe(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_MC_ROUTE_KEY         *route_key,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_MC_ROUTE_INFO        *route_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                  *success
  );

uint32
  soc_pb_pp_frwrd_ipv4_mc_route_add_verify(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_MC_ROUTE_KEY         *route_key,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_MC_ROUTE_INFO        *route_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ipv4_mc_route_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets the routing information (system-fec-id) associated
 *   with the given route key.
 * INPUT:
 *   SOC_SAND_IN  int                             unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_MC_ROUTE_KEY         *route_key -
 *     The routing key (IPv4 subnet and a virtual router id)
 *   SOC_SAND_IN  uint8                             exact_match -
 *     If TRUE returns exact match only; if FALSE returns
 *     longest prefix match.
 *   SOC_SAND_OUT SOC_PB_PP_FRWRD_IPV4_MC_ROUTE_INFO        *route_info -
 *     Routing information (FEC-id, or MC-group).
 *   SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_STATUS           *route_status -
 *     Indicates whether the returned entry exist in HW
 *     (commited) or pending (either for remove or addition),
 *     relevant only if found is TRUE.
 *   SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_LOCATION         *location -
 *     Indicates whether the returned entry exsit in host table
 *     or LPM/TCAM. Relvant only if found is TRUE. If the entry
 *     is not commited yet then this is the location the route
 *     will be inserted into
 *   SOC_SAND_OUT uint8                             *found -
 *     If TRUE then route_val has valid data.
 * REMARKS:
 *   - Found is always TRUE if exact_match is FALSE, due to
 *   the default route.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv4_mc_route_get_unsafe(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_MC_ROUTE_KEY         *route_key,
    SOC_SAND_IN  uint8                             exact_match,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IPV4_MC_ROUTE_INFO        *route_info,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_STATUS           *route_status,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_LOCATION         *location,
    SOC_SAND_OUT uint8                             *found
  );

uint32
  soc_pb_pp_frwrd_ipv4_mc_route_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_MC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  uint8                                 exact_match
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ipv4_mc_route_get_block_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets the Ipv4 MC routing table.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_INOUT SOC_PB_PP_IP_ROUTING_TABLE_RANGE              *block_range_key -
 *     Defines the range and the order of the block of routing
 *     entries.
 *   SOC_SAND_OUT SOC_PB_PP_FRWRD_IPV4_MC_ROUTE_KEY             *route_keys -
 *     Array of routing keys
 *   SOC_SAND_OUT SOC_PB_PP_FEC_ID                              *fec_ids -
 *     Array if fec-ids
 *   SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_STATUS               *routes_status -
 *     For each route Indicates whether it exists in HW
 *     (commited) or pending (either for remove or addition).
 *     Set this parameter to NULL in order to be ignored
 *   SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_LOCATION             *routes_location -
 *     For each route Indicates whether it exsits in host table
 *     or LPM/TCAM. If the entry is not commited yet then this
 *     is the location the route will be inserted into. Set
 *     this parameter to NULL in order to be ignored
 *   SOC_SAND_OUT uint32                                  *nof_entries -
 *     Number of entries in returned Arrays.
 * REMARKS:
 *   - the lookup is performed in the SW shadow.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv4_mc_route_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_PB_PP_IP_ROUTING_TABLE_RANGE              *block_range_key,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IPV4_MC_ROUTE_KEY             *route_keys,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IPV4_MC_ROUTE_INFO        *routes_info,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_STATUS               *routes_status,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_LOCATION             *routes_location,
    SOC_SAND_OUT uint32                                  *nof_entries
  );

uint32
  soc_pb_pp_frwrd_ipv4_mc_route_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_PB_PP_IP_ROUTING_TABLE_RANGE              *block_range_key
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ipv4_mc_route_remove_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   PE uint32 soc_ppd_frwrd_ipv4_mc_route_remove( SOC_SAND_IN
 *   uint32 unit, SOC_SAND_IN SOC_PPD_FRWRD_IPV4_MC_ROUTE_KEY
 *   *route_key, SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE *success);
 *   Remove IPv4 route entry from the routing table.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_MC_ROUTE_KEY             *route_key -
 *     The routing key (IPv4 subnet and a virtual router id)
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success -
 *     Whether the operation succeeds.
 * REMARKS:
 *   - use soc_ppd_frwrd_ip_routes_cache_mode_enable_set() to
 *   determine wehter to cache this route in SW or to write
 *   it directly into HW.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv4_mc_route_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_MC_ROUTE_KEY             *route_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  soc_pb_pp_frwrd_ipv4_mc_route_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_MC_ROUTE_KEY             *route_key
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ipv4_mc_routing_table_clear_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Clear the IPv4 MC routing table.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv4_mc_routing_table_clear_unsafe(
    SOC_SAND_IN  int                                 unit
  );

uint32
  soc_pb_pp_frwrd_ipv4_mc_routing_table_clear_verify(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ipv4_vrf_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Setting global information of the VRF including
 *   (defaults forwarding).
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx -
 *     Virtual router id. Range: 1 - 255.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_VRF_INFO                 *vrf_info -
 *     VRF information.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv4_vrf_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_VRF_INFO                 *vrf_info
  );

uint32
  soc_pb_pp_frwrd_ipv4_vrf_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_VRF_INFO                 *vrf_info
  );

uint32
  soc_pb_pp_frwrd_ipv4_vrf_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_frwrd_ipv4_vrf_info_set_unsafe" API.
 *     Refer to "soc_pb_pp_frwrd_ipv4_vrf_info_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv4_vrf_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IPV4_VRF_INFO                 *vrf_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ipv4_vrf_route_add_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add IPv4 route entry to the virtual routing table (VRF).
 *   Binds between Ipv4 route key (UC/MC IPv4-address\prefix)
 *   and a FEC entry identified by fec_id for a given virtual
 *   router. As a result of this operation, Unicast Ipv4
 *   packets designated to IP address matching the given key
 *   (as long there is no more-specific route key) will be
 *   routed according to the information in the FEC entry
 *   identified by fec_id.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx -
 *     Virtual router id. Range: 1 - 255.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_VPN_ROUTE_KEY            *route_key -
 *     The routing key IPv4 subnet MC/UC.
 *   SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_id -
 *     FEC ID.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success -
 *     Whether the operation succeeds. Add operation may fail
 *     if there is no place in the ILM DB (LEM/LPM).
 * REMARKS:
 *   - use soc_ppd_frwrd_ip_routes_cache_mode_enable_set() to
 *   determine wehter to cache this route in SW or to write
 *   it directly into HW. - For VPN routing, there is no
 *   difference in the lookup functionality between IPV4 MC
 *   and Ipv4 UC. - Supports virtual routing tables using the
 *   vrf_id
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv4_vrf_route_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_id,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  soc_pb_pp_frwrd_ipv4_vrf_route_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_id
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ipv4_vrf_route_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets the routing information (system-fec-id) associated
 *   with the given route key on VRF.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx -
 *     Virtual router id. Range: 1 - 255.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_VPN_ROUTE_KEY            *route_key -
 *     The routing key (IPv4 subnet and a virtual router id)
 *   SOC_SAND_IN  uint8                                 exact_match -
 *     If TRUE returns exact match only; if FALSE returns
 *     longest prefix match.
 *   SOC_SAND_OUT SOC_PB_PP_FEC_ID                              *fec_id -
 *     Routing information (system-fec-id).
 *   SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_STATUS               *route_status -
 *     Indicates whether the returned entry exist in HW
 *     (commited) or pending (either for remove or addition),
 *     relevant only if found is TRUE.
 *   SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_LOCATION             *location -
 *     Indicates whether the returned entry exsit in host table
 *     or LPM/TCAM. Relvant only if found is TRUE. If the entry
 *     is not commited yet then this is the location the route
 *     will be inserted into
 *   SOC_SAND_OUT uint8                                 *found -
 *     If TRUE then route_val has valid data.
 * REMARKS:
 *   - Found is always TRUE if exact_match is FALSE, due to
 *   the default route.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv4_vrf_route_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_IN  uint8                                 exact_match,
    SOC_SAND_OUT SOC_PB_PP_FEC_ID                              *fec_id,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_STATUS               *route_status,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_LOCATION             *location,
    SOC_SAND_OUT uint8                                 *found
  );

uint32
  soc_pb_pp_frwrd_ipv4_vrf_route_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_IN  uint8                                 exact_match
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ipv4_vrf_route_get_block_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets the routing table of a virtual router (VRF).
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx -
 *     Virtual router id. Range: 1 - 255.
 *   SOC_SAND_INOUT SOC_PB_PP_IP_ROUTING_TABLE_RANGE              *block_range_key -
 *     Defines the range and the order of the block of routing
 *     entries.
 *   SOC_SAND_OUT SOC_PB_PP_FRWRD_IPV4_VPN_ROUTE_KEY            *route_keys -
 *     array of routing keys
 *   SOC_SAND_OUT SOC_PB_PP_FEC_ID                              *fec_ids -
 *     Array of fec-ids
 *   SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_STATUS               *routes_status -
 *     For each route Indicates whether it exists in HW
 *     (commited) or pending (either for remove or addition).
 *     Set this parameter to NULL in order to be ignored
 *   SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_LOCATION             *routes_location -
 *     For each route Indicates whether it exsits in host table
 *     or LPM/TCAM. If the entry is not commited yet then this
 *     is the location the route will be inserted into. Set
 *     this parameter to NULL in order to be ignored
 *   SOC_SAND_OUT uint32                                  *nof_entries -
 *     Number of entries in returned Arrays.
 * REMARKS:
 *   - the lookup is performed in the SW shadow.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv4_vrf_route_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx,
    SOC_SAND_INOUT SOC_PB_PP_IP_ROUTING_TABLE_RANGE              *block_range_key,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IPV4_VPN_ROUTE_KEY            *route_keys,
    SOC_SAND_OUT SOC_PB_PP_FEC_ID                              *fec_ids,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_STATUS               *routes_status,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_LOCATION             *routes_location,
    SOC_SAND_OUT uint32                                  *nof_entries
  );

uint32
  soc_pb_pp_frwrd_ipv4_vrf_route_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx,
    SOC_SAND_INOUT SOC_PB_PP_IP_ROUTING_TABLE_RANGE              *block_range_key
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ipv4_vrf_route_remove_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove IPv4 route entry from the routing table of a
 *   virtual router (VRF).
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx -
 *     Virtual router id. Range: 1 - 255.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_VPN_ROUTE_KEY            *route_key -
 *     The routing key (IPv4 subnet and a virtual router id)
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success -
 *     Whether the operation succeeded.
 * REMARKS:
 *   - use soc_ppd_frwrd_ip_routes_cache_mode_enable_set() to
 *   determine wehter to cache this route in SW or to write
 *   it directly into HW.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv4_vrf_route_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  soc_pb_pp_frwrd_ipv4_vrf_route_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_VPN_ROUTE_KEY            *route_key
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ipv4_vrf_routing_table_clear_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Clear IPv4 routing table of VRF
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx -
 *     Virtual router id. Range: 1 - 255.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv4_vrf_routing_table_clear_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx
  );

uint32
  soc_pb_pp_frwrd_ipv4_vrf_routing_table_clear_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ipv4_vrf_all_routing_tables_clear_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Clear IPv4 routing tables for all VRFs.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv4_vrf_all_routing_tables_clear_unsafe(
    SOC_SAND_IN  int                                 unit
  );

uint32
  soc_pb_pp_frwrd_ipv4_vrf_all_routing_tables_clear_verify(
    SOC_SAND_IN  int                                 unit
  );

uint32
    soc_pb_pp_frwrd_em_dest_to_fec(
      SOC_SAND_IN uint32 dest,
      SOC_SAND_IN  uint32                  asd_buffer,
      SOC_SAND_OUT SOC_PB_PP_FRWRD_IPV4_HOST_ROUTE_INFO *route_info
    );


/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ipv4_mem_status_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Returns the status and usage of memory.
 * INPUT:
 *   SOC_SAND_IN  int                        unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                         mem_ndx -
 *     memory/table id. Range: 0 - 4.
 *   SOC_SAND_OUT SOC_PB_PP_FRWRD_IPV4_MEM_STATUS      *mem_status -
 *     Status and usage of memory
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv4_mem_status_get_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                         mem_ndx,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IPV4_MEM_STATUS      *mem_status
  );

uint32
  soc_pb_pp_frwrd_ipv4_mem_status_get_verify(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                         mem_ndx
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ipv4_mem_defrage_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Returns the status and usage of memory.
 * INPUT:
 *   SOC_SAND_IN  int                        unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                         mem_ndx -
 *     memory/table id. Range: 0 - 4.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_MEM_DEFRAG_INFO *defrag_info -
 *     Defragment parameters, determine how many iterations and
 *     table access to perform before this call returns.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv4_mem_defrage_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                         mem_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_MEM_DEFRAG_INFO *defrag_info
  );

uint32
  soc_pb_pp_frwrd_ipv4_mem_defrage_verify(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                         mem_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_MEM_DEFRAG_INFO *defrag_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ipv4_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   soc_pb_pp_api_frwrd_ipv4 module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_frwrd_ipv4_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ipv4_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   soc_pb_pp_api_frwrd_ipv4 module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_frwrd_ipv4_get_errs_ptr(void);

uint32
    soc_pb_pp_frwrd_ipv4_em_dest_to_sand_dest(
      SOC_SAND_IN int               unit,
      SOC_SAND_IN uint32                dest,
      SOC_SAND_OUT SOC_SAND_PP_DESTINATION_ID *dest_id
    );
uint32
    soc_pb_pp_frwrd_ipv4_sand_dest_to_em_dest(
      SOC_SAND_IN int              unit,
      SOC_SAND_IN SOC_SAND_PP_DESTINATION_ID *dest_id,
      SOC_SAND_OUT uint32 *dest
    );


uint32
  SOC_PB_PP_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_VAL_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_VAL *info
  );

uint32
  SOC_PB_PP_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_ROUTER_DEFAULT_ACTION *info
  );

uint32
  SOC_PB_PP_FRWRD_IPV4_ROUTER_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_ROUTER_INFO *info
  );

uint32
  SOC_PB_PP_FRWRD_IPV4_VRF_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_VRF_INFO *info
  );

uint32
  SOC_PB_PP_FRWRD_IPV4_GLBL_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_GLBL_INFO *info
  );

uint32
  SOC_PB_PP_FRWRD_IPV4_UC_ROUTE_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_UC_ROUTE_KEY *info
  );

uint32
  SOC_PB_PP_FRWRD_IPV4_MC_ROUTE_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_MC_ROUTE_KEY *info
  );

uint32
  SOC_PB_PP_FRWRD_IPV4_VPN_ROUTE_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_VPN_ROUTE_KEY *info
  );

uint32
  SOC_PB_PP_FRWRD_IPV4_HOST_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_HOST_KEY *info
  );

uint32
  SOC_PB_PP_FRWRD_IPV4_HOST_ROUTE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_HOST_ROUTE_INFO *info
  );

uint32
  SOC_PB_PP_FRWRD_IPV4_MC_ROUTE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_MC_ROUTE_INFO *info
  );


uint32
  SOC_PB_PP_FRWRD_IPV4_MEM_DEFRAG_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV4_MEM_DEFRAG_INFO *info
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PP_FRWRD_IPV4_INCLUDED__*/
#endif

