/* $Id: ppd_api_frwrd_ipv4.h,v 1.15 Broadcom SDK $
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
* FILENAME:       DuneDriver/ppd/include/soc_ppd_api_frwrd_ipv4.h
*
* MODULE PREFIX:  soc_ppd_frwrd
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

#ifndef __SOC_PPD_API_FRWRD_IPV4_INCLUDED__
/* { */
#define __SOC_PPD_API_FRWRD_IPV4_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPD/ppd_api_framework.h>
#include <soc/dpp/PPC/ppc_api_frwrd_ipv4.h>

#include <soc/dpp/PPD/ppd_api_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     The value SOC_PPD_FRWRD_IP_ALL_VRFS_ID stands for all VRFs.
 *     May Used to apply setting to all VRFs (when mentioned)  */
#define  SOC_PPD_FRWRD_IP_ALL_VRFS_ID (SOC_PPC_FRWRD_IP_ALL_VRFS_ID)

#define  SOC_PPD_FRWRD_IP_HOST_ONLY (SOC_PPC_FRWRD_IP_HOST_ONLY)

#define  SOC_PPD_FRWRD_IP_LPM_ONLY (SOC_PPC_FRWRD_IP_LPM_ONLY)


/* get exact match entry in LPM/DB */
#define  SOC_PPD_FRWRD_IP_EXACT_MATCH  (SOC_PPC_FRWRD_IP_EXACT_MATCH)
/* clear hit indication on get  */
#define  SOC_PPD_FRWRD_IP_CLEAR_ON_GET  (SOC_PPC_FRWRD_IP_CLEAR_ON_GET)

/* host flags */



/* clear accessed indication   */
#define  SOC_PPD_FRWRD_IP_HOST_CLEAR_ON_GET  (SOC_PPC_FRWRD_IP_HOST_CLEAR_ON_GET)

/* for host-get-block get accessed entries only */
#define  SOC_PPD_FRWRD_IP_HOST_GET_ACCESSED_ONLY  (SOC_PPC_FRWRD_IP_HOST_GET_ACCESSED_ONLY)

/* for get-block get accessed status (require another access to HW) */
#define  SOC_PPD_FRWRD_IP_HOST_GET_ACCESSS_STATUS   (SOC_PPC_FRWRD_IP_HOST_GET_ACCESSS_STATUS)

/* for IPMC BIDIR mask Intf */
#define  SOC_PPD_FRWRD_IP_MC_BIDIR_IGNORE_RIF   (SOC_PPC_FRWRD_IP_MC_BIDIR_IGNORE_RIF)


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
  SOC_PPD_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_SET = SOC_PPD_PROC_DESC_BASE_FRWRD_IPV4_FIRST,
  SOC_PPD_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_SET_PRINT,
  SOC_PPD_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_GET,
  SOC_PPD_FRWRD_IP_ROUTES_CACHE_MODE_ENABLE_GET_PRINT,
  SOC_PPD_FRWRD_IP_ROUTES_CACHE_COMMIT,
  SOC_PPD_FRWRD_IP_ROUTES_CACHE_COMMIT_PRINT,
  SOC_PPD_FRWRD_IPV4_GLBL_INFO_SET,
  SOC_PPD_FRWRD_IPV4_GLBL_INFO_SET_PRINT,
  SOC_PPD_FRWRD_IPV4_GLBL_INFO_GET,
  SOC_PPD_FRWRD_IPV4_GLBL_INFO_GET_PRINT,
  SOC_PPD_FRWRD_IPV4_UC_ROUTE_ADD,
  SOC_PPD_FRWRD_IPV4_UC_ROUTE_ADD_PRINT,
  SOC_PPD_FRWRD_IPV4_UC_ROUTE_GET,
  SOC_PPD_FRWRD_IPV4_UC_ROUTE_GET_PRINT,
  SOC_PPD_FRWRD_IPV4_UC_ROUTE_GET_BLOCK,
  SOC_PPD_FRWRD_IPV4_UC_ROUTE_GET_BLOCK_PRINT,
  SOC_PPD_FRWRD_IPV4_UC_ROUTE_REMOVE,
  SOC_PPD_FRWRD_IPV4_UC_ROUTE_REMOVE_PRINT,
  SOC_PPD_FRWRD_IPV4_UC_ROUTING_TABLE_CLEAR,
  SOC_PPD_FRWRD_IPV4_UC_ROUTING_TABLE_CLEAR_PRINT,
  SOC_PPD_FRWRD_IPV4_HOST_ADD,
  SOC_PPD_FRWRD_IPV4_HOST_ADD_PRINT,
  SOC_PPD_FRWRD_IPV4_HOST_GET,
  SOC_PPD_FRWRD_IPV4_HOST_GET_PRINT,
  SOC_PPD_FRWRD_IPV4_HOST_GET_BLOCK,
  SOC_PPD_FRWRD_IPV4_HOST_GET_BLOCK_PRINT,
  SOC_PPD_FRWRD_IPV4_HOST_REMOVE,
  SOC_PPD_FRWRD_IPV4_HOST_REMOVE_PRINT,
  SOC_PPD_FRWRD_IPV4_MC_ROUTE_ADD,
  SOC_PPD_FRWRD_IPV4_MC_ROUTE_ADD_PRINT,
  SOC_PPD_FRWRD_IPV4_MC_ROUTE_GET,
  SOC_PPD_FRWRD_IPV4_MC_ROUTE_GET_PRINT,
  SOC_PPD_FRWRD_IPV4_MC_ROUTE_GET_BLOCK,
  SOC_PPD_FRWRD_IPV4_MC_ROUTE_GET_BLOCK_PRINT,
  SOC_PPD_FRWRD_IPV4_MC_ROUTE_REMOVE,
  SOC_PPD_FRWRD_IPV4_MC_ROUTE_REMOVE_PRINT,
  SOC_PPD_FRWRD_IPV4_MC_ROUTING_TABLE_CLEAR,
  SOC_PPD_FRWRD_IPV4_MC_ROUTING_TABLE_CLEAR_PRINT,
  SOC_PPD_FRWRD_IPV4_VRF_INFO_SET,
  SOC_PPD_FRWRD_IPV4_VRF_INFO_SET_PRINT,
  SOC_PPD_FRWRD_IPV4_VRF_INFO_GET,
  SOC_PPD_FRWRD_IPV4_VRF_INFO_GET_PRINT,
  SOC_PPD_FRWRD_IPV4_VRF_ROUTE_ADD,
  SOC_PPD_FRWRD_IPV4_VRF_ROUTE_ADD_PRINT,
  SOC_PPD_FRWRD_IPV4_VRF_ROUTE_GET,
  SOC_PPD_FRWRD_IPV4_VRF_ROUTE_GET_PRINT,
  SOC_PPD_FRWRD_IPV4_VRF_ROUTE_GET_BLOCK,
  SOC_PPD_FRWRD_IPV4_VRF_ROUTE_GET_BLOCK_PRINT,
  SOC_PPD_FRWRD_IPV4_VRF_ROUTE_REMOVE,
  SOC_PPD_FRWRD_IPV4_VRF_ROUTE_REMOVE_PRINT,
  SOC_PPD_FRWRD_IPV4_VRF_ROUTING_TABLE_CLEAR,
  SOC_PPD_FRWRD_IPV4_VRF_ROUTING_TABLE_CLEAR_PRINT,
  SOC_PPD_FRWRD_IPV4_VRF_ALL_ROUTING_TABLES_CLEAR,
  SOC_PPD_FRWRD_IPV4_VRF_ALL_ROUTING_TABLES_CLEAR_PRINT,
  SOC_PPD_FRWRD_IPV4_MEM_STATUS_GET,
  SOC_PPD_FRWRD_IPV4_MEM_STATUS_GET_PRINT,
  SOC_PPD_FRWRD_IPV4_MEM_DEFRAGE,
  SOC_PPD_FRWRD_IPV4_MEM_DEFRAGE_PRINT,
  SOC_PPD_FRWRD_IPV4_GET_PROCS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */
  /*
   * Last element. Do no touch.
   */
  SOC_PPD_FRWRD_IPV4_PROCEDURE_DESC_LAST
} SOC_PPD_FRWRD_IPV4_PROCEDURE_DESC;
#define SOC_PPD_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_TYPE_FEC      SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_TYPE_FEC
#define SOC_PPD_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_TYPE_ACTION_PROFILE SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_TYPE_ACTION_PROFILE
typedef SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_TYPE          SOC_PPD_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_TYPE;

#define SOC_PPD_FRWRD_IPV4_HOST_TABLE_RESOURCE_LPM_THEN_LEM    SOC_PPC_FRWRD_IPV4_HOST_TABLE_RESOURCE_LPM_THEN_LEM
#define SOC_PPD_FRWRD_IPV4_HOST_TABLE_RESOURCE_LPM_ONLY        SOC_PPC_FRWRD_IPV4_HOST_TABLE_RESOURCE_LPM_ONLY
#define SOC_PPD_FRWRD_IPV4_HOST_TABLE_RESOURCE_LEM_THEN_LPM    SOC_PPC_FRWRD_IPV4_HOST_TABLE_RESOURCE_LEM_THEN_LPM
#define SOC_PPD_FRWRD_IPV4_HOST_TABLE_RESOURCE_LEM_ONLY        SOC_PPC_FRWRD_IPV4_HOST_TABLE_RESOURCE_LEM_ONLY
typedef SOC_PPC_FRWRD_IPV4_HOST_TABLE_RESOURCE                 SOC_PPD_FRWRD_IPV4_HOST_TABLE_RESOURCE;

#define SOC_PPD_FRWRD_IPV4_MC_HOST_TABLE_RESOURCE_TCAM_ONLY    SOC_PPC_FRWRD_IPV4_MC_HOST_TABLE_RESOURCE_TCAM_ONLY
typedef SOC_PPC_FRWRD_IPV4_MC_HOST_TABLE_RESOURCE              SOC_PPD_FRWRD_IPV4_MC_HOST_TABLE_RESOURCE;

#define SOC_PPD_FRWRD_IP_CACHE_MODE_NONE                        SOC_PPC_FRWRD_IP_CACHE_MODE_NONE
#define SOC_PPD_FRWRD_IP_CACHE_MODE_IPV4_UC_LPM                 SOC_PPC_FRWRD_IP_CACHE_MODE_IPV4_UC_LPM
typedef SOC_PPC_FRWRD_IP_CACHE_MODE                             SOC_PPD_FRWRD_IP_CACHE_MODE;

#define SOC_PPD_FRWRD_IP_ROUTE_STATUS_ACCESSED                 SOC_PPC_FRWRD_IP_ROUTE_STATUS_ACCESSED
typedef SOC_PPC_FRWRD_IP_ROUTE_STATUS                          SOC_PPD_FRWRD_IP_ROUTE_STATUS;

typedef SOC_PPC_FRWRD_IP_ROUTE_LOCATION                        SOC_PPD_FRWRD_IP_ROUTE_LOCATION;

typedef SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_VAL           SOC_PPD_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_VAL;
typedef SOC_PPC_FRWRD_IPV4_ROUTER_DEFAULT_ACTION               SOC_PPD_FRWRD_IPV4_ROUTER_DEFAULT_ACTION;
typedef SOC_PPC_FRWRD_IPV4_ROUTER_INFO                         SOC_PPD_FRWRD_IPV4_ROUTER_INFO;
typedef SOC_PPC_FRWRD_IPV4_VRF_INFO                            SOC_PPD_FRWRD_IPV4_VRF_INFO;
typedef SOC_PPC_FRWRD_IPV4_GLBL_INFO                           SOC_PPD_FRWRD_IPV4_GLBL_INFO;
typedef SOC_PPC_FRWRD_IPV4_UC_ROUTE_KEY                        SOC_PPD_FRWRD_IPV4_UC_ROUTE_KEY;
typedef SOC_PPC_FRWRD_IPV4_MC_ROUTE_KEY                        SOC_PPD_FRWRD_IPV4_MC_ROUTE_KEY;
typedef SOC_PPC_FRWRD_IP_MC_RP_INFO                            SOC_PPD_FRWRD_IP_MC_RP_INFO;
typedef SOC_PPC_FRWRD_IPV4_VPN_ROUTE_KEY                       SOC_PPD_FRWRD_IPV4_VPN_ROUTE_KEY;
typedef SOC_PPC_FRWRD_IPV4_HOST_KEY                            SOC_PPD_FRWRD_IPV4_HOST_KEY;
typedef SOC_PPC_FRWRD_IPV4_HOST_ROUTE_INFO                     SOC_PPD_FRWRD_IPV4_HOST_ROUTE_INFO;
typedef SOC_PPC_FRWRD_IPV4_MC_ROUTE_INFO                       SOC_PPD_FRWRD_IPV4_MC_ROUTE_INFO;
typedef SOC_PPC_FRWRD_IPV4_MEM_STATUS                          SOC_PPD_FRWRD_IPV4_MEM_STATUS;
typedef SOC_PPC_FRWRD_IPV4_MEM_DEFRAG_INFO                     SOC_PPD_FRWRD_IPV4_MEM_DEFRAG_INFO;

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
* NAME:
 *   soc_ppd_frwrd_ip_routes_cache_mode_enable_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set for which IP routes (IPv4/6 UC/MC) to enable caching
 *   by SW
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_VRF_ID                              vrf_ndx -
 *     Virtual router id. Range: 0 - 255. Use VRF 0 for default
 *     routing table. Use SOC_PPD_FRWRD_IP_ALL_VRFS_ID to apply
 *     setting to all VRFs.
 *   SOC_SAND_IN  uint32                                route_types -
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
  soc_ppd_frwrd_ip_routes_cache_mode_enable_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  uint32                                route_types
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_ppd_frwrd_ip_routes_cache_mode_enable_set" API.
 *     Refer to "soc_ppd_frwrd_ip_routes_cache_mode_enable_set" API
 *     for details.
*********************************************************************/
uint32
  soc_ppd_frwrd_ip_routes_cache_mode_enable_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VRF_ID                              vrf_ndx,
    SOC_SAND_OUT uint32                                *route_types
  );

/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_ip_routes_cache_commit
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Synchronize the routing table in the Data plane (HW)
 *   with the routing table in the control plane (SW)
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                route_types -
 *     IP Routes to commit changes for them. use
 *     SOC_SAND_PP_IP_TYPE to select which IP types to commit into
 *     Hardware
 *   SOC_SAND_IN  SOC_PPD_VRF_ID                              vrf_ndx -
 *     Virtual router id. Range: 0 - 255. Use VRF 0 for default
 *     routing table. Use SOC_PPD_FRWRD_IP_ALL_VRFS_ID to apply
 *     setting to all VRFs.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success -
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
  soc_ppd_frwrd_ip_routes_cache_commit(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                route_types,
    SOC_SAND_IN  SOC_PPD_VRF_ID                              vrf_ndx,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  );

/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_ipv4_glbl_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Setting global information of the IP routing (including
 *   resources to use)
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_GLBL_INFO                *glbl_info -
 *     Global information.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_glbl_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_GLBL_INFO                *glbl_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_ppd_frwrd_ipv4_glbl_info_set" API.
 *     Refer to "soc_ppd_frwrd_ipv4_glbl_info_set" API for details.
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_glbl_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT SOC_PPD_FRWRD_IPV4_GLBL_INFO                *glbl_info
  );

/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_ipv4_uc_route_add
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
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_UC_ROUTE_KEY             *route_key -
 *     The routing key: IPv4 subnet
 *   SOC_SAND_IN  SOC_PPD_FEC_ID                              fec_id -
 *     FEC ID.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success -
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
  soc_ppd_frwrd_ipv4_uc_route_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_UC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  SOC_PPD_FEC_ID                              fec_id,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  );

/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_ipv4_uc_route_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets the routing information (system-fec-id) associated
 *   with the given route key.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_UC_ROUTE_KEY             *route_key -
 *     The routing key (IPv4 subnet and a virtual router id)
 *   SOC_SAND_IN  uint8                               exact_match -
 *     If TRUE returns exact match only; if FALSE returns
 *     longest prefix match.
 *   SOC_SAND_OUT SOC_PPD_FEC_ID                              *fec_id -
 *     Routing information (system-fec-id).
 *   SOC_SAND_OUT SOC_PPD_FRWRD_IP_ROUTE_STATUS               *route_status -
 *     Indicates whether the returned entry exist in HW
 *     (commited) or pending (either for remove or addition),
 *     relevant only if found is TRUE.
 *   SOC_SAND_OUT SOC_PPD_FRWRD_IP_ROUTE_LOCATION             *location -
 *     Indicates whether the returned entry exsit in host table
 *     or LPM/TCAM. Relvant only if found is TRUE. If the entry
 *     is not commited yet then this is the location the route
 *     will be inserted into
 *   SOC_SAND_OUT uint8                               *found -
 *     If TRUE then route_val has valid data.
 * REMARKS:
 *   - the lookup is performed in the SW shadow. - Found is
 *   always TRUE if exact_match is FALSE, due to the default
 *   route.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_uc_route_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_UC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  uint8                               exact_match,
    SOC_SAND_OUT SOC_PPD_FEC_ID                              *fec_id,
    SOC_SAND_OUT SOC_PPD_FRWRD_IP_ROUTE_STATUS               *route_status,
    SOC_SAND_OUT SOC_PPD_FRWRD_IP_ROUTE_LOCATION             *location,
    SOC_SAND_OUT uint8                               *found
  );

/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_ipv4_uc_route_get_block
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets the Ipv4 UC routing table.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_INOUT SOC_PPD_IP_ROUTING_TABLE_RANGE              *block_range -
 *     Defines the range and the order of the block of routing
 *     entries.
 *   SOC_SAND_OUT SOC_PPD_FRWRD_IPV4_UC_ROUTE_KEY             *route_keys -
 *     Array of routing keys (IPv4 subnets)
 *   SOC_SAND_OUT SOC_PPD_FEC_ID                              *fec_ids -
 *     Array of fec-ids.
 *   SOC_SAND_OUT SOC_PPD_FRWRD_IP_ROUTE_STATUS               *routes_status -
 *     For each route Indicates whether it exists in HW
 *     (commited) or pending (either for remove or addition).
 *     Set this parameter to NULL in order to be ignored
 *   SOC_SAND_OUT SOC_PPD_FRWRD_IP_ROUTE_LOCATION             *routes_location -
 *     For each route Indicates whether it exsits in host table
 *     or LPM/TCAM. If the entry is not commited yet then this
 *     is the location the route will be inserted into. Set
 *     this parameter to NULL in order to be ignored
 *   SOC_SAND_OUT uint32                                *nof_entries -
 *     Number of entries in returned arrays.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_uc_route_get_block(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_INOUT SOC_PPD_IP_ROUTING_TABLE_RANGE              *block_range,
    SOC_SAND_OUT SOC_PPD_FRWRD_IPV4_UC_ROUTE_KEY             *route_keys,
    SOC_SAND_OUT SOC_PPD_FEC_ID                              *fec_ids,
    SOC_SAND_OUT SOC_PPD_FRWRD_IP_ROUTE_STATUS               *routes_status,
    SOC_SAND_OUT SOC_PPD_FRWRD_IP_ROUTE_LOCATION             *routes_location,
    SOC_SAND_OUT uint32                                *nof_entries
  );

/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_ipv4_uc_route_remove
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove entry from the routing table.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_UC_ROUTE_KEY             *route_key -
 *     The routing key (IPv4 subnet and a virtual router id)
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success -
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
  soc_ppd_frwrd_ipv4_uc_route_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_UC_ROUTE_KEY             *route_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  );

/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_ipv4_uc_routing_table_clear
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Clear the IPv4 UC routing table.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_uc_routing_table_clear(
    SOC_SAND_IN  int                               unit
  );

/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_ipv4_host_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add IPv4 entry to the Host table. Binds between Host and
 *   next hop information.
 * INPUT:
 *   SOC_SAND_IN  int                           unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_HOST_KEY             *host_key -
 *     VRF-ID and IP address
 *   SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_HOST_ROUTE_INFO      *routing_info -
 *     Routing information. See remarks.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                *success -
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
  soc_ppd_frwrd_ipv4_host_add(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_HOST_KEY             *host_key,
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_HOST_ROUTE_INFO      *routing_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                *success
  );

/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_ipv4_host_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets the routing information associated with the given
 *   route key on VRF.
 * INPUT:
 *   SOC_SAND_IN  int                           unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_HOST_KEY             *host_key -
 *     The routing key (IPv4 address and a virtual router id)
 *   SOC_SAND_IN  uint32                                  flags -
 *     flags, see SOC_PPD_FRWRD_IP_HOST_
 *   SOC_SAND_OUT SOC_PPD_FRWRD_IPV4_HOST_ROUTE_INFO      *routing_info -
 *     Routing information (fec-id).
 *   SOC_SAND_OUT SOC_PPD_FRWRD_IP_ROUTE_STATUS           *route_status -
 *     Indicates whether the returned entry exist in HW
 *     (commited) or pending (either for remove or addition),
 *     relevant only if found is TRUE.
 *   SOC_SAND_OUT SOC_PPD_FRWRD_IP_ROUTE_LOCATION         *location -
 *     Indicates whether the returned entry exsit in host table
 *     or LPM/TCAM. Relvant only if found is TRUE. If the entry
 *     is not commited yet then this is the location the route
 *     will be inserted into
 *   SOC_SAND_OUT uint8                           *found -
 *     If TRUE then route_val has valid data.
 * REMARKS:
 *   - If the host is not found in the host DB, then the host
 *   (exact route/32) is lookuped in the LPM
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_host_get(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_HOST_KEY             *host_key,
    SOC_SAND_IN  uint32                                  flags,
    SOC_SAND_OUT SOC_PPD_FRWRD_IPV4_HOST_ROUTE_INFO      *routing_info,
    SOC_SAND_OUT SOC_PPD_FRWRD_IP_ROUTE_STATUS           *route_status,
    SOC_SAND_OUT SOC_PPD_FRWRD_IP_ROUTE_LOCATION         *location,
    SOC_SAND_OUT uint8                           *found
  );

/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_ipv4_host_get_block
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets the host table.
 * INPUT:
 *   SOC_SAND_IN  int                           unit -
 *     Identifier of the device to access.
 *   SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE              *block_range_key -
 *     Defines the range of routing entries to retrieve.
 *   SOC_SAND_OUT SOC_PPD_FRWRD_IPV4_HOST_KEY             *host_keys -
 *     array of routing keys
 *   SOC_SAND_OUT SOC_PPD_FRWRD_IPV4_HOST_ROUTE_INFO      *routes_info -
 *     Array of routing information for each host key.
 *   SOC_SAND_OUT SOC_PPD_FRWRD_IP_ROUTE_STATUS           *routes_status -
 *     For each route Indicates whether it exists in HW
 *     (commited) or pending (either for remove or addition).
 *     Set this parameter to NULL in order to be ignored
 *   SOC_SAND_OUT uint32                            *nof_entries -
 *     Number of entries in returned Arrays.
 * REMARKS:
 *   - The lookup is performed in the SW shadow.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_host_get_block(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE              *block_range_key,
    SOC_SAND_IN  uint32                                  flags,
    SOC_SAND_OUT SOC_PPD_FRWRD_IPV4_HOST_KEY             *host_keys,
    SOC_SAND_OUT SOC_PPD_FRWRD_IPV4_HOST_ROUTE_INFO      *routes_info,
    SOC_SAND_OUT SOC_PPD_FRWRD_IP_ROUTE_STATUS           *routes_status,
    SOC_SAND_OUT uint32                            *nof_entries
  );

/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_ipv4_host_remove
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove IPv4 route entry from the routing table of a
 *   virtual router (VRF).
 * INPUT:
 *   SOC_SAND_IN  int                           unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_HOST_KEY             *host_key -
 *     The routing key (IPv4 subnet and a virtual router id)
 * REMARKS:
 *   - use soc_ppd_frwrd_ip_routes_cache_mode_enable_set() to
 *   determine wehter to cache this route in SW or to write
 *   it directly into HW.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_host_remove(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_HOST_KEY             *host_key
  );

/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_ipv4_mc_route_add
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
 *   SOC_SAND_IN  int                           unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_MC_ROUTE_KEY         *route_key -
 *     The routing key IPv4 subnet
 *   SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_MC_ROUTE_INFO        *route_info -
 *     Routing information (FEC-id, or MC-group).
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                *success -
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
  soc_ppd_frwrd_ipv4_mc_route_add(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_MC_ROUTE_KEY         *route_key,
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_MC_ROUTE_INFO        *route_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                *success
  );

/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_ipv4_mc_route_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets the routing information (system-fec-id) associated
 *   with the given route key.
 * INPUT:
 *   SOC_SAND_IN  int                           unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_MC_ROUTE_KEY         *route_key -
 *     The routing key (IPv4 subnet and a virtual router id)
 *   SOC_SAND_IN  uint8                               flags -
 *     see SOC_PPD_FRWRD_IP_ flags:
 *     SOC_PPD_FRWRD_IP_EXACT_MATCH: If present returns exact match only; if FALSE returns
 *     longest prefix match.
 *     SOC_PPD_FRWRD_IP_CLEAR_ON_GET: clear hit indication. (ARAD only)
 *   SOC_SAND_OUT SOC_PPD_FRWRD_IPV4_MC_ROUTE_INFO        *route_info -
 *     Routing information (FEC-id, or MC-group).
 *   SOC_SAND_OUT SOC_PPD_FRWRD_IP_ROUTE_STATUS           *route_status -
 *     Indicates whether the returned entry exist in HW
 *     (commited) or pending (either for remove or addition),
 *     relevant only if found is TRUE.
 *   SOC_SAND_OUT SOC_PPD_FRWRD_IP_ROUTE_LOCATION         *location -
 *     Indicates whether the returned entry exsit in host table
 *     or LPM/TCAM. Relvant only if found is TRUE. If the entry
 *     is not commited yet then this is the location the route
 *     will be inserted into
 *   SOC_SAND_OUT uint8                           *found -
 *     If TRUE then route_val has valid data.
 * REMARKS:
 *   - Found is always TRUE if exact_match is FALSE, due to
 *   the default route.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_mc_route_get(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_MC_ROUTE_KEY         *route_key,
    SOC_SAND_IN  uint8                           flags,
    SOC_SAND_OUT SOC_PPD_FRWRD_IPV4_MC_ROUTE_INFO        *route_info,
    SOC_SAND_OUT SOC_PPD_FRWRD_IP_ROUTE_STATUS           *route_status,
    SOC_SAND_OUT SOC_PPD_FRWRD_IP_ROUTE_LOCATION         *location,
    SOC_SAND_OUT uint8                           *found
  );

/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_ipv4_mc_route_get_block
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets the Ipv4 MC routing table.
 * INPUT:
 *   SOC_SAND_IN  int                           unit -
 *     Identifier of the device to access.
 *   SOC_SAND_INOUT SOC_PPD_IP_ROUTING_TABLE_RANGE          *block_range_key -
 *     Defines the range and the order of the block of routing
 *     entries.
 *   SOC_SAND_OUT SOC_PPD_FRWRD_IPV4_MC_ROUTE_KEY         *route_keys -
 *     Array of routing keys
 *   SOC_SAND_OUT SOC_PPD_FRWRD_IPV4_MC_ROUTE_INFO        *routes_info -
 *     Routing information (FEC-id, or MC-group).
 *   SOC_SAND_OUT SOC_PPD_FRWRD_IP_ROUTE_STATUS           *routes_status -
 *     For each route Indicates whether it exists in HW
 *     (commited) or pending (either for remove or addition).
 *     Set this parameter to NULL in order to be ignored
 *   SOC_SAND_OUT SOC_PPD_FRWRD_IP_ROUTE_LOCATION         *routes_location -
 *     For each route Indicates whether it exsits in host table
 *     or LPM/TCAM. If the entry is not commited yet then this
 *     is the location the route will be inserted into. Set
 *     this parameter to NULL in order to be ignored
 *   SOC_SAND_OUT uint32                            *nof_entries -
 *     Number of entries in returned Arrays.
 * REMARKS:
 *   - the lookup is performed in the SW shadow.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_mc_route_get_block(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_INOUT SOC_PPD_IP_ROUTING_TABLE_RANGE          *block_range_key,
    SOC_SAND_OUT SOC_PPD_FRWRD_IPV4_MC_ROUTE_KEY         *route_keys,
    SOC_SAND_OUT SOC_PPD_FRWRD_IPV4_MC_ROUTE_INFO        *routes_info,
    SOC_SAND_OUT SOC_PPD_FRWRD_IP_ROUTE_STATUS           *routes_status,
    SOC_SAND_OUT SOC_PPD_FRWRD_IP_ROUTE_LOCATION         *routes_location,
    SOC_SAND_OUT uint32                            *nof_entries
  );

/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_ipv4_mc_route_remove
 * TYPE:
 *   PROC
 * FUNCTION:
 *   PE uint32 soc_ppd_frwrd_ipv4_mc_route_remove( SOC_SAND_IN
 *   uint32 unit, SOC_SAND_IN SOC_PPD_FRWRD_IPV4_MC_ROUTE_KEY
 *   *route_key, SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE *success);
 *   Remove IPv4 route entry from the routing table.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_MC_ROUTE_KEY             *route_key -
 *     The routing key (IPv4 subnet and a virtual router id)
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success -
 *     Whether the operation succeeds.
 * REMARKS:
 *   - use soc_ppd_frwrd_ip_routes_cache_mode_enable_set() to
 *   determine wehter to cache this route in SW or to write
 *   it directly into HW.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_mc_route_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_MC_ROUTE_KEY             *route_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  );

/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_ipv4_mc_routing_table_clear
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Clear the IPv4 MC routing table.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_mc_routing_table_clear(
    SOC_SAND_IN  int                               unit
  );


/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_ipmc_rp_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add IPMC RP. 
 * INPUT:
 *   SOC_SAND_IN  int                           unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_FRWRD_IP_MC_RP_INFO             *rp_info -
 *     RP-ID and active l3-intf
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
  soc_ppd_frwrd_ipmc_rp_add(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_IP_MC_RP_INFO      *rp_info
  );


/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_ipmc_rp_get_block
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets the host table.
 * INPUT:
 *   SOC_SAND_IN  int                           unit -
 *     Identifier of the device to access.
 *   SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE              *block_range_key -
 *     Defines the range of routing entries to retrieve.
 *   SOC_SAND_IN  uint32                                  rp_id -
 *    RP-ID
 *   SOC_SAND_OUT uint32      *l3_intfs -
 *     Array of active l3-interfaces on given RP-ID
 *   SOC_SAND_OUT uint32                            *nof_entries -
 *     Number of entries in returned Arrays.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_frwrd_ipmc_rp_get_block(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint32                                  rp_id,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE            *block_range_key,
    SOC_SAND_OUT int32                                  *l3_intfs,
    SOC_SAND_OUT uint32                                  *nof_entries
  );

/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_ipmc_rp_remove
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove IPMC route entry from the routing table of a
 *   virtual router (VRF).
 * INPUT:
 *   SOC_SAND_IN  int                           unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_FRWRD_IP_MC_RP_INFO             *rp_info -
 *     The routing key (IPMC subnet and a virtual router id)
 * REMARKS:
 *   - use soc_ppd_frwrd_ip_routes_cache_mode_enable_set() to
 *   determine wehter to cache this route in SW or to write
 *   it directly into HW.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_frwrd_ipmc_rp_remove(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_IP_MC_RP_INFO      *rp_info
  );


/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_ipv4_vrf_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Setting global information of the VRF including
 *   (defaults forwarding).
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_VRF_ID                              vrf_ndx -
 *     Virtual router id. Range: 1 - 255.
 *   SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_VRF_INFO                 *vrf_info -
 *     VRF information.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_vrf_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_VRF_INFO                 *vrf_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_ppd_frwrd_ipv4_vrf_info_set" API.
 *     Refer to "soc_ppd_frwrd_ipv4_vrf_info_set" API for details.
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_vrf_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VRF_ID                              vrf_ndx,
    SOC_SAND_OUT SOC_PPD_FRWRD_IPV4_VRF_INFO                 *vrf_info
  );

/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_ipv4_vrf_route_add
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
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_VRF_ID                              vrf_ndx -
 *     Virtual router id. Range: 1 - 255.
 *   SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_VPN_ROUTE_KEY            *route_key -
 *     The routing key IPv4 subnet MC/UC.
 *   SOC_SAND_IN  SOC_PPD_FEC_ID                              fec_id -
 *     FEC ID.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success -
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
  soc_ppd_frwrd_ipv4_vrf_route_add(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_IN  SOC_PPD_FEC_ID                              fec_id,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  );

/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_ipv4_vrf_route_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets the routing information (system-fec-id) associated
 *   with the given route key on VRF.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_VRF_ID                              vrf_ndx -
 *     Virtual router id. Range: 1 - 255.
 *   SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_VPN_ROUTE_KEY            *route_key -
 *     The routing key (IPv4 subnet and a virtual router id)
 *   SOC_SAND_IN  uint8                               exact_match -
 *     If TRUE returns exact match only; if FALSE returns
 *     longest prefix match.
 *   SOC_SAND_OUT SOC_PPD_FEC_ID                              *fec_id -
 *     Routing information (system-fec-id).
 *   SOC_SAND_OUT SOC_PPD_FRWRD_IP_ROUTE_STATUS               *route_status -
 *     Indicates whether the returned entry exist in HW
 *     (commited) or pending (either for remove or addition),
 *     relevant only if found is TRUE.
 *   SOC_SAND_OUT SOC_PPD_FRWRD_IP_ROUTE_LOCATION             *location -
 *     Indicates whether the returned entry exsit in host table
 *     or LPM/TCAM. Relvant only if found is TRUE. If the entry
 *     is not commited yet then this is the location the route
 *     will be inserted into
 *   SOC_SAND_OUT uint8                               *found -
 *     If TRUE then route_val has valid data.
 * REMARKS:
 *   - Found is always TRUE if exact_match is FALSE, due to
 *   the default route.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_vrf_route_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_IN  uint8                               exact_match,
    SOC_SAND_OUT SOC_PPD_FEC_ID                              *fec_id,
    SOC_SAND_OUT SOC_PPD_FRWRD_IP_ROUTE_STATUS               *route_status,
    SOC_SAND_OUT SOC_PPD_FRWRD_IP_ROUTE_LOCATION             *location,
    SOC_SAND_OUT uint8                               *found
  );

/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_ipv4_vrf_route_get_block
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets the routing table of a virtual router (VRF).
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_VRF_ID                              vrf_ndx -
 *     Virtual router id. Range: 1 - 255.
 *   SOC_SAND_INOUT SOC_PPD_IP_ROUTING_TABLE_RANGE              *block_range_key -
 *     Defines the range and the order of the block of routing
 *     entries.
 *   SOC_SAND_OUT SOC_PPD_FRWRD_IPV4_VPN_ROUTE_KEY            *route_keys -
 *     array of routing keys
 *   SOC_SAND_OUT SOC_PPD_FEC_ID                              *fec_ids -
 *     Array of fec-ids
 *   SOC_SAND_OUT SOC_PPD_FRWRD_IP_ROUTE_STATUS               *routes_status -
 *     For each route Indicates whether it exists in HW
 *     (commited) or pending (either for remove or addition).
 *     Set this parameter to NULL in order to be ignored
 *   SOC_SAND_OUT SOC_PPD_FRWRD_IP_ROUTE_LOCATION             *routes_location -
 *     For each route Indicates whether it exsits in host table
 *     or LPM/TCAM. If the entry is not commited yet then this
 *     is the location the route will be inserted into. Set
 *     this parameter to NULL in order to be ignored
 *   SOC_SAND_OUT uint32                                *nof_entries -
 *     Number of entries in returned Arrays.
 * REMARKS:
 *   - the lookup is performed in the SW shadow.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_vrf_route_get_block(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VRF_ID                              vrf_ndx,
    SOC_SAND_INOUT SOC_PPD_IP_ROUTING_TABLE_RANGE              *block_range_key,
    SOC_SAND_OUT SOC_PPD_FRWRD_IPV4_VPN_ROUTE_KEY            *route_keys,
    SOC_SAND_OUT SOC_PPD_FEC_ID                              *fec_ids,
    SOC_SAND_OUT SOC_PPD_FRWRD_IP_ROUTE_STATUS               *routes_status,
    SOC_SAND_OUT SOC_PPD_FRWRD_IP_ROUTE_LOCATION             *routes_location,
    SOC_SAND_OUT uint32                                *nof_entries
  );

/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_ipv4_vrf_route_remove
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove IPv4 route entry from the routing table of a
 *   virtual router (VRF).
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_VRF_ID                              vrf_ndx -
 *     Virtual router id. Range: 1 - 255.
 *   SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_VPN_ROUTE_KEY            *route_key -
 *     The routing key (IPv4 subnet and a virtual router id)
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success -
 *     Whether the operation succeeded.
 * REMARKS:
 *   - use soc_ppd_frwrd_ip_routes_cache_mode_enable_set() to
 *   determine wehter to cache this route in SW or to write
 *   it directly into HW.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_vrf_route_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success
  );

/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_ipv4_vrf_routing_table_clear
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Clear IPv4 routing table of VRF
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_VRF_ID                              vrf_ndx -
 *     Virtual router id. Range: 1 - 255.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_vrf_routing_table_clear(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VRF_ID                              vrf_ndx
  );

/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_ipv4_vrf_all_routing_tables_clear
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Clear IPv4 routing tables for all VRFs.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_vrf_all_routing_tables_clear(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                 flags
  );

/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_ipv4_mem_status_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Returns the status and usage of memory.
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                       mem_ndx -
 *     memory/table id. Range: 0 - 4.
 *   SOC_SAND_OUT SOC_PPD_FRWRD_IPV4_MEM_STATUS      *mem_status -
 *     Status and usage of memory
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_mem_status_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                       mem_ndx,
    SOC_SAND_OUT SOC_PPD_FRWRD_IPV4_MEM_STATUS      *mem_status
  );

/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_ipv4_mem_defrage
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Returns the status and usage of memory.
 * INPUT:
 *   SOC_SAND_IN  int                      unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                       mem_ndx -
 *     memory/table id. Range: 0 - 4.
 *   SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_MEM_DEFRAG_INFO *defrag_info -
 *     Defragment parameters, determine how many iterations and
 *     table access to perform before this call returns.
 * REMARKS:
 *   for Soc_petra-B can be called only if according to operation mode defragement is enabled
 *   see SOC_PB_PP_MGMT_IPV4_OP_MODE_SUPPORT_DEFRAG, soc_pb_pp_mgmt_operation_mode_set
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_ppd_frwrd_ipv4_mem_defrage(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                       mem_ndx,
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_MEM_DEFRAG_INFO *defrag_info
  );

/*********************************************************************
* NAME:
 *   soc_ppd_frwrd_ipv4_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   soc_ppd_api_frwrd_ipv4 module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  soc_ppd_frwrd_ipv4_get_procs_ptr(void);
void
  SOC_PPD_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_VAL_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_VAL *info
  );

void
  SOC_PPD_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_IPV4_ROUTER_DEFAULT_ACTION *info
  );

void
  SOC_PPD_FRWRD_IPV4_ROUTER_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_IPV4_ROUTER_INFO *info
  );

void
  SOC_PPD_FRWRD_IPV4_VRF_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_IPV4_VRF_INFO *info
  );

void
  SOC_PPD_FRWRD_IPV4_GLBL_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_IPV4_GLBL_INFO *info
  );

void
  SOC_PPD_FRWRD_IPV4_UC_ROUTE_KEY_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_IPV4_UC_ROUTE_KEY *info
  );

void
  SOC_PPD_FRWRD_IPV4_MC_ROUTE_KEY_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_IPV4_MC_ROUTE_KEY *info
  );
void
  SOC_PPD_FRWRD_IP_MC_RP_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_IP_MC_RP_INFO *info
  );
void
  SOC_PPD_FRWRD_IPV4_VPN_ROUTE_KEY_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_IPV4_VPN_ROUTE_KEY *info
  );

void
  SOC_PPD_FRWRD_IPV4_HOST_KEY_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_IPV4_HOST_KEY *info
  );

void
  SOC_PPD_FRWRD_IPV4_HOST_ROUTE_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_IPV4_HOST_ROUTE_INFO *info
  );

void
  SOC_PPD_FRWRD_IPV4_MC_ROUTE_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_IPV4_MC_ROUTE_INFO *info
  );

void
  SOC_PPD_FRWRD_IPV4_MEM_STATUS_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_IPV4_MEM_STATUS *info
  );

void
  SOC_PPD_FRWRD_IPV4_MEM_DEFRAG_INFO_clear(
    SOC_SAND_OUT SOC_PPD_FRWRD_IPV4_MEM_DEFRAG_INFO *info
  );

#if SOC_PPD_DEBUG_IS_LVL1

const char*
  SOC_PPD_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_TYPE_to_string(
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_TYPE enum_val
  );

const char*
  SOC_PPD_FRWRD_IPV4_HOST_TABLE_RESOURCE_to_string(
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_HOST_TABLE_RESOURCE enum_val
  );

const char*
  SOC_PPD_FRWRD_IPV4_MC_HOST_TABLE_RESOURCE_to_string(
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_MC_HOST_TABLE_RESOURCE enum_val
  );

const char*
  SOC_PPD_FRWRD_IP_CACHE_MODE_to_string(
    SOC_SAND_IN  SOC_PPD_FRWRD_IP_CACHE_MODE enum_val
  );

const char*
  SOC_PPD_FRWRD_IP_ROUTE_STATUS_to_string(
    SOC_SAND_IN  SOC_PPD_FRWRD_IP_ROUTE_STATUS enum_val
  );

const char*
  SOC_PPD_FRWRD_IP_ROUTE_LOCATION_to_string(
    SOC_SAND_IN  SOC_PPD_FRWRD_IP_ROUTE_LOCATION enum_val
  );

void
  SOC_PPD_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_VAL_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_VAL *info
  );

void
  SOC_PPD_FRWRD_IPV4_ROUTER_DEFAULT_ACTION_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_ROUTER_DEFAULT_ACTION *info
  );

void
  SOC_PPD_FRWRD_IPV4_ROUTER_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_ROUTER_INFO *info
  );

void
  SOC_PPD_FRWRD_IPV4_VRF_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_VRF_INFO *info
  );

void
  SOC_PPD_FRWRD_IPV4_GLBL_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_GLBL_INFO *info
  );

void
  SOC_PPD_FRWRD_IPV4_UC_ROUTE_KEY_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_UC_ROUTE_KEY *info
  );

void
  SOC_PPD_FRWRD_IPV4_MC_ROUTE_KEY_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_MC_ROUTE_KEY *info
  );

void
  SOC_PPD_FRWRD_IP_MC_RP_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_IP_MC_RP_INFO *info
  );

void
  SOC_PPD_FRWRD_IPV4_VPN_ROUTE_KEY_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_VPN_ROUTE_KEY *info
  );

void
  SOC_PPD_FRWRD_IPV4_HOST_KEY_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_HOST_KEY *info
  );

void
  SOC_PPD_FRWRD_IPV4_HOST_ROUTE_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_HOST_ROUTE_INFO *info
  );

void
  SOC_PPD_FRWRD_IPV4_MC_ROUTE_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_MC_ROUTE_INFO *info
  );

void
  SOC_PPD_FRWRD_IPV4_MEM_STATUS_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_MEM_STATUS *info
  );

void
  SOC_PPD_FRWRD_IPV4_MEM_DEFRAG_INFO_print(
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_MEM_DEFRAG_INFO *info
  );

#endif /* SOC_PPD_DEBUG_IS_LVL1 */

#if SOC_PPD_DEBUG_IS_LVL3

void
  soc_ppd_frwrd_ip_routes_cache_mode_enable_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  uint32                                route_types
  );

void
  soc_ppd_frwrd_ip_routes_cache_mode_enable_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VRF_ID                              vrf_ndx
  );

void
  soc_ppd_frwrd_ip_routes_cache_commit_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                                route_types,
    SOC_SAND_IN  SOC_PPD_VRF_ID                              vrf_ndx
  );

void
  soc_ppd_frwrd_ipv4_glbl_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_GLBL_INFO                *glbl_info
  );

void
  soc_ppd_frwrd_ipv4_glbl_info_get_print(
    SOC_SAND_IN  int                               unit
  );

void
  soc_ppd_frwrd_ipv4_uc_route_add_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_UC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  SOC_PPD_FEC_ID                              fec_id
  );

void
  soc_ppd_frwrd_ipv4_uc_route_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_UC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  uint8                               exact_match
  );

void
  soc_ppd_frwrd_ipv4_uc_route_get_block_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_INOUT SOC_PPD_IP_ROUTING_TABLE_RANGE              *block_range
  );

void
  soc_ppd_frwrd_ipv4_uc_route_remove_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_UC_ROUTE_KEY             *route_key
  );

void
  soc_ppd_frwrd_ipv4_uc_routing_table_clear_print(
    SOC_SAND_IN  int                               unit
  );

void
  soc_ppd_frwrd_ipv4_host_add_print(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_HOST_KEY             *host_key,
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_HOST_ROUTE_INFO      *routing_info
  );

void
  soc_ppd_frwrd_ipv4_host_get_print(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_HOST_KEY             *host_key
  );

void
  soc_ppd_frwrd_ipv4_host_get_block_print(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE              *block_range_key
  );

void
  soc_ppd_frwrd_ipv4_host_remove_print(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_HOST_KEY             *host_key
  );

void
  soc_ppd_frwrd_ipv4_mc_route_add_print(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_MC_ROUTE_KEY         *route_key,
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_MC_ROUTE_INFO        *route_info
  );

void
  soc_ppd_frwrd_ipv4_mc_route_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_MC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  uint8                               exact_match
  );

void
  soc_ppd_frwrd_ipv4_mc_route_get_block_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_INOUT SOC_PPD_IP_ROUTING_TABLE_RANGE              *block_range_key
  );

void
  soc_ppd_frwrd_ipv4_mc_route_remove_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_MC_ROUTE_KEY             *route_key
  );

void
  soc_ppd_frwrd_ipv4_mc_routing_table_clear_print(
    SOC_SAND_IN  int                               unit
  );

void
  soc_ppd_frwrd_ipv4_vrf_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_VRF_INFO                 *vrf_info
  );

void
  soc_ppd_frwrd_ipv4_vrf_info_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VRF_ID                              vrf_ndx
  );

void
  soc_ppd_frwrd_ipv4_vrf_route_add_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_IN  SOC_PPD_FEC_ID                              fec_id
  );

void
  soc_ppd_frwrd_ipv4_vrf_route_get_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_IN  uint8                               exact_match
  );

void
  soc_ppd_frwrd_ipv4_vrf_route_get_block_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VRF_ID                              vrf_ndx,
    SOC_SAND_INOUT SOC_PPD_IP_ROUTING_TABLE_RANGE              *block_range_key
  );

void
  soc_ppd_frwrd_ipv4_vrf_route_remove_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_VPN_ROUTE_KEY            *route_key
  );

void
  soc_ppd_frwrd_ipv4_vrf_routing_table_clear_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  SOC_PPD_VRF_ID                              vrf_ndx
  );

void
  soc_ppd_frwrd_ipv4_vrf_all_routing_tables_clear_print(
    SOC_SAND_IN  int                               unit
  );

void
  soc_ppd_frwrd_ipv4_mem_status_get_print(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                       mem_ndx
  );

void
  soc_ppd_frwrd_ipv4_mem_defrage_print(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                       mem_ndx,
    SOC_SAND_IN  SOC_PPD_FRWRD_IPV4_MEM_DEFRAG_INFO *defrag_info
  );

#endif /* SOC_PPD_DEBUG_IS_LVL3 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PPD_API_FRWRD_IPV4_INCLUDED__*/
#endif

