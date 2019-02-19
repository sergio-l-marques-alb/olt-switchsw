/* $Id: soc_pb_pp_frwrd_ipv6.h,v 1.7 Broadcom SDK $
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

#ifndef __SOC_PB_PP_FRWRD_IPV6_INCLUDED__
/* { */
#define __SOC_PB_PP_FRWRD_IPV6_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_api_frwrd_ipv6.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/* $Id: soc_pb_pp_frwrd_ipv6.h,v 1.7 Broadcom SDK $
* the length of key stream
*    1 : VRF
*    16 :  dest IP
*    1: mask len
*/

/*
* the length of key stream
*    2 : in-rif
*    2:  in_rif mask
*    15 : group
*/

/*
* the length of key stream
*    1 : in-rif valid
*    2 : in_rif
*    4 : SIP
*    1 : SIP mask len
*    4 : group
*/

/* length of the key stream that
* is big enough to contain either ipv6 uc / ipvc mc / ipv4 mc
* key stream
*/
#define SOC_PB_PP_IP_TCAM_KEY_LEN_BYTES 20

#define SOC_PB_PP_IPV6_MC_GROUP_NOF_BITS 120


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
  SOC_PB_PP_FRWRD_IPV6_GLBL_INFO_SET = SOC_PB_PP_PROC_DESC_BASE_FRWRD_IPV6_FIRST,
  SOC_PB_PP_FRWRD_IPV6_GLBL_INFO_SET_PRINT,
  SOC_PB_PP_FRWRD_IPV6_GLBL_INFO_SET_UNSAFE,
  SOC_PB_PP_FRWRD_IPV6_GLBL_INFO_SET_VERIFY,
  SOC_PB_PP_FRWRD_IPV6_GLBL_INFO_GET,
  SOC_PB_PP_FRWRD_IPV6_GLBL_INFO_GET_PRINT,
  SOC_PB_PP_FRWRD_IPV6_GLBL_INFO_GET_VERIFY,
  SOC_PB_PP_FRWRD_IPV6_GLBL_INFO_GET_UNSAFE,
  SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_ADD,
  SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_ADD_PRINT,
  SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_ADD_UNSAFE,
  SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_ADD_VERIFY,
  SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_GET,
  SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_GET_PRINT,
  SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_GET_UNSAFE,
  SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_GET_VERIFY,
  SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_GET_BLOCK,
  SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_GET_BLOCK_PRINT,
  SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_GET_BLOCK_UNSAFE,
  SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_GET_BLOCK_VERIFY,
  SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_REMOVE,
  SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_REMOVE_PRINT,
  SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_REMOVE_UNSAFE,
  SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_REMOVE_VERIFY,
  SOC_PB_PP_FRWRD_IPV6_UC_ROUTING_TABLE_CLEAR,
  SOC_PB_PP_FRWRD_IPV6_UC_ROUTING_TABLE_CLEAR_PRINT,
  SOC_PB_PP_FRWRD_IPV6_UC_ROUTING_TABLE_CLEAR_UNSAFE,
  SOC_PB_PP_FRWRD_IPV6_UC_ROUTING_TABLE_CLEAR_VERIFY,
  SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_ADD,
  SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_ADD_PRINT,
  SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_ADD_UNSAFE,
  SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_ADD_VERIFY,
  SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_GET,
  SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_GET_PRINT,
  SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_GET_UNSAFE,
  SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_GET_VERIFY,
  SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_GET_BLOCK,
  SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_GET_BLOCK_PRINT,
  SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_GET_BLOCK_UNSAFE,
  SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_GET_BLOCK_VERIFY,
  SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_REMOVE,
  SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_REMOVE_PRINT,
  SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_REMOVE_UNSAFE,
  SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_REMOVE_VERIFY,
  SOC_PB_PP_FRWRD_IPV6_MC_ROUTING_TABLE_CLEAR,
  SOC_PB_PP_FRWRD_IPV6_MC_ROUTING_TABLE_CLEAR_PRINT,
  SOC_PB_PP_FRWRD_IPV6_MC_ROUTING_TABLE_CLEAR_UNSAFE,
  SOC_PB_PP_FRWRD_IPV6_MC_ROUTING_TABLE_CLEAR_VERIFY,
  SOC_PB_PP_FRWRD_IPV6_VRF_INFO_SET,
  SOC_PB_PP_FRWRD_IPV6_VRF_INFO_SET_PRINT,
  SOC_PB_PP_FRWRD_IPV6_VRF_INFO_SET_UNSAFE,
  SOC_PB_PP_FRWRD_IPV6_VRF_INFO_SET_VERIFY,
  SOC_PB_PP_FRWRD_IPV6_VRF_INFO_GET,
  SOC_PB_PP_FRWRD_IPV6_VRF_INFO_GET_PRINT,
  SOC_PB_PP_FRWRD_IPV6_VRF_INFO_GET_VERIFY,
  SOC_PB_PP_FRWRD_IPV6_VRF_INFO_GET_UNSAFE,
  SOC_PB_PP_FRWRD_IPV6_VRF_ROUTE_ADD,
  SOC_PB_PP_FRWRD_IPV6_VRF_ROUTE_ADD_PRINT,
  SOC_PB_PP_FRWRD_IPV6_VRF_ROUTE_ADD_UNSAFE,
  SOC_PB_PP_FRWRD_IPV6_VRF_ROUTE_ADD_VERIFY,
  SOC_PB_PP_FRWRD_IPV6_VRF_ROUTE_GET,
  SOC_PB_PP_FRWRD_IPV6_VRF_ROUTE_GET_PRINT,
  SOC_PB_PP_FRWRD_IPV6_VRF_ROUTE_GET_UNSAFE,
  SOC_PB_PP_FRWRD_IPV6_VRF_ROUTE_GET_VERIFY,
  SOC_PB_PP_FRWRD_IPV6_VRF_ROUTE_GET_BLOCK,
  SOC_PB_PP_FRWRD_IPV6_VRF_ROUTE_GET_BLOCK_PRINT,
  SOC_PB_PP_FRWRD_IPV6_VRF_ROUTE_GET_BLOCK_UNSAFE,
  SOC_PB_PP_FRWRD_IPV6_VRF_ROUTE_GET_BLOCK_VERIFY,
  SOC_PB_PP_FRWRD_IPV6_VRF_ROUTE_REMOVE,
  SOC_PB_PP_FRWRD_IPV6_VRF_ROUTE_REMOVE_PRINT,
  SOC_PB_PP_FRWRD_IPV6_VRF_ROUTE_REMOVE_UNSAFE,
  SOC_PB_PP_FRWRD_IPV6_VRF_ROUTE_REMOVE_VERIFY,
  SOC_PB_PP_FRWRD_IPV6_VRF_ROUTING_TABLE_CLEAR,
  SOC_PB_PP_FRWRD_IPV6_VRF_ROUTING_TABLE_CLEAR_PRINT,
  SOC_PB_PP_FRWRD_IPV6_VRF_ROUTING_TABLE_CLEAR_UNSAFE,
  SOC_PB_PP_FRWRD_IPV6_VRF_ROUTING_TABLE_CLEAR_VERIFY,
  SOC_PB_PP_FRWRD_IPV6_VRF_ALL_ROUTING_TABLES_CLEAR,
  SOC_PB_PP_FRWRD_IPV6_VRF_ALL_ROUTING_TABLES_CLEAR_PRINT,
  SOC_PB_PP_FRWRD_IPV6_VRF_ALL_ROUTING_TABLES_CLEAR_UNSAFE,
  SOC_PB_PP_FRWRD_IPV6_VRF_ALL_ROUTING_TABLES_CLEAR_VERIFY,
  SOC_PB_PP_FRWRD_IPV6_GET_PROCS_PTR,
  SOC_PB_PP_FRWRD_IPV6_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */

  SOC_PB_PP_FRWRD_IP_TCAM_ROUTE_ADD_UNSAFE,
  SOC_PB_PP_FRWRD_IP_TCAM_ROUTE_GET_UNSAFE,
  SOC_PB_PP_FRWRD_IP_TCAM_ROUTE_GET_BLOCK_UNSAFE,
  SOC_PB_PP_FRWRD_IP_TCAM_ROUTE_REMOVE_UNSAFE,
  SOC_PB_PP_FRWRD_IP_TCAM_ROUTING_TABLE_CLEAR_UNSAFE,

  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_FRWRD_IPV6_PROCEDURE_DESC_LAST
} SOC_PB_PP_FRWRD_IPV6_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PB_PP_FRWRD_IPV6_SUCCESS_OUT_OF_RANGE_ERR = SOC_PB_PP_ERR_DESC_BASE_FRWRD_IPV6_FIRST,
  SOC_PB_PP_FRWRD_IPV6_ROUTE_STATUS_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_IPV6_LOCATION_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_IPV6_FOUND_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_IPV6_ROUTES_STATUS_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_IPV6_ROUTES_LOCATION_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_IPV6_NOF_ENTRIES_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_IPV6_EXACT_MATCH_OUT_OF_RANGE_ERR,
  SOC_PB_PP_FRWRD_IPV6_TYPE_OUT_OF_RANGE_ERR,
  /*
   * } Auto generated. Do not edit previous section.
   */

  SOC_PB_PP_IPV6_DEFAULT_ACTION_TYPE_NOT_SUPPORTED_ERR,
  SOC_PB_PP_IPV6_DEFAULT_ACTION_WRONG_TRAP_CODE_ERR,
  SOC_PB_PP_FRWRD_IPV6_MC_ILLEGAL_DEST_TYPE_ERR,
  SOC_PB_PP_FRWRD_IP_TCAM_ENTRY_DOESNT_EXIST_ERR,

  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_FRWRD_IPV6_ERR_LAST
} SOC_PB_PP_FRWRD_IPV6_ERR;

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
  soc_pb_pp_frwrd_ipv6_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ipv6_glbl_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Setting global information of the IP routing (including
 *   resources to use)
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_GLBL_INFO                *glbl_info -
 *     Global information.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv6_glbl_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_GLBL_INFO                *glbl_info
  );

uint32
  soc_pb_pp_frwrd_ipv6_glbl_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_GLBL_INFO                *glbl_info
  );

uint32
  soc_pb_pp_frwrd_ipv6_glbl_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_frwrd_ipv6_glbl_info_set_unsafe" API.
 *     Refer to "soc_pb_pp_frwrd_ipv6_glbl_info_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv6_glbl_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IPV6_GLBL_INFO                *glbl_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ipv6_uc_route_add_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add IPv6 route entry to the routing table. Binds between
 *   Ipv6 Unicast route key (IPv6-address/prefix) and a FEC
 *   entry identified by fec_id for a given virtual router.
 *   As a result of this operation, Unicast Ipv6 packets
 *   designated to IP address matching the given key (as long
 *   there is no more-specific route key) will be routed
 *   according to the information in the FEC entry identified
 *   by fec_id.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_KEY             *route_key -
 *     The routing key IPv6 subnet
 *   SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_id -
 *     FEC ID.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success -
 *     Whether the operation succeeds. Add operation may fail
 *     if there is no place in the ILM DB (LEM/LPM).
 * REMARKS:
 *   - use soc_ppd_frwrd_ip_routes_cache_mode_enable_set() to
 *   determine wehter to cache this route in SW or to write
 *   it directly into HW.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv6_uc_route_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_id,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  soc_pb_pp_frwrd_ipv6_uc_route_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_id
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ipv6_uc_route_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets the routing information (system-fec-id) associated
 *   with the given route key.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_KEY             *route_key -
 *     The routing key (IPv6 subnet and a virtual router id)
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
  soc_pb_pp_frwrd_ipv6_uc_route_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  uint8                                 exact_match,
    SOC_SAND_OUT SOC_PB_PP_FEC_ID                              *fec_id,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_STATUS               *route_status,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_LOCATION             *location,
    SOC_SAND_OUT uint8                                 *found
  );

uint32
  soc_pb_pp_frwrd_ipv6_uc_route_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_KEY             *route_key,
    SOC_SAND_IN  uint8                                 exact_match
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ipv6_uc_route_get_block_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets the Ipv6 UC routing table.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_INOUT SOC_PB_PP_IP_ROUTING_TABLE_RANGE              *block_range_key -
 *     Defines the range and the order of the block of routing
 *     entries.
 *   SOC_SAND_OUT SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_KEY             *route_keys -
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
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv6_uc_route_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_PB_PP_IP_ROUTING_TABLE_RANGE              *block_range_key,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_KEY             *route_keys,
    SOC_SAND_OUT SOC_PB_PP_FEC_ID                              *fec_ids,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_STATUS               *routes_status,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_LOCATION             *routes_location,
    SOC_SAND_OUT uint32                                  *nof_entries
  );

uint32
  soc_pb_pp_frwrd_ipv6_uc_route_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_PB_PP_IP_ROUTING_TABLE_RANGE              *block_range_key
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ipv6_uc_route_remove_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove IPv6 route entry from the routing table.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_KEY             *route_key -
 *     The routing key (IPv6 subnet and a virtual router id)
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
  soc_pb_pp_frwrd_ipv6_uc_route_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_KEY             *route_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  soc_pb_pp_frwrd_ipv6_uc_route_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_KEY             *route_key
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ipv6_uc_routing_table_clear_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Clear the IPv6 UC routing table.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv6_uc_routing_table_clear_unsafe(
    SOC_SAND_IN  int                                 unit
  );

uint32
  soc_pb_pp_frwrd_ipv6_uc_routing_table_clear_verify(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ipv6_mc_route_add_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add IPv6 MC route entry to the routing table. Binds
 *   between Ipv6 Unicast route key (IPv6-address/prefix) and
 *   a FEC entry identified by fec_id for a given virtual
 *   router. As a result of this operation, Unicast Ipv6
 *   packets designated to IP address matching the given key
 *   (as long there is no more-specific route key) will be
 *   routed according to the information in the FEC entry
 *   identified by fec_id.
 * INPUT:
 *   SOC_SAND_IN  int                             unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_KEY         *route_key -
 *     The routing key IPv6 subnet
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_INFO        *route_info -
 *     Routing information, according to which to route
 *     packets.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                  *success -
 *     Whether the operation succeeds. Add operation may fail
 *     if there is no place in the ILM DB (LEM/LPM).
 * REMARKS:
 *   - use soc_ppd_frwrd_ip_routes_cache_mode_enable_set() to
 *   determine wehter to cache this route in SW or to write
 *   it directly into HW.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv6_mc_route_add_unsafe(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_KEY         *route_key,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_INFO        *route_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                  *success
  );

uint32
  soc_pb_pp_frwrd_ipv6_mc_route_add_verify(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_KEY         *route_key,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_INFO        *route_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ipv6_mc_route_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets the routing information (system-fec-id) associated
 *   with the given route key.
 * INPUT:
 *   SOC_SAND_IN  int                             unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_KEY         *route_key -
 *     The routing key (IPv6 subnet and a virtual router id)
 *   SOC_SAND_IN  uint8                             exact_match -
 *     If TRUE returns exact match only; if FALSE returns
 *     longest prefix match.
 *   SOC_SAND_OUT SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_INFO        *route_info -
 *     Routing information
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
  soc_pb_pp_frwrd_ipv6_mc_route_get_unsafe(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_KEY         *route_key,
    SOC_SAND_IN  uint8                             exact_match,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_INFO        *route_info,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_STATUS           *route_status,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_LOCATION         *location,
    SOC_SAND_OUT uint8                             *found
  );

uint32
  soc_pb_pp_frwrd_ipv6_mc_route_get_verify(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_KEY         *route_key,
    SOC_SAND_IN  uint8                             exact_match
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ipv6_mc_route_get_block_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets the Ipv6 MC routing table.
 * INPUT:
 *   SOC_SAND_IN  int                             unit -
 *     Identifier of the device to access.
 *   SOC_SAND_INOUT SOC_PB_PP_IP_ROUTING_TABLE_RANGE          *block_range_key -
 *     Defines the range and the order of the block of routing
 *     entries.
 *   SOC_SAND_OUT SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_KEY         *route_key -
 *     s - array of routing keys
 *   SOC_SAND_OUT SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_INFO        *routes_info -
 *     Array of fec-ids
 *   SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_STATUS           *routes_status -
 *     For each route Indicates whether it exists in HW
 *     (commited) or pending (either for remove or addition).
 *     Set this parameter to NULL in order to be ignored
 *   SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_LOCATION         *routes_location -
 *     For each route Indicates whether it exsits in host table
 *     or LPM/TCAM. If the entry is not commited yet then this
 *     is the location the route will be inserted into. Set
 *     this parameter to NULL in order to be ignored
 *   SOC_SAND_OUT uint32                              *nof_entries -
 *     Number of entries in returned Arrays.
 * REMARKS:
 *   - the lookup is performed in the SW shadow.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv6_mc_route_get_block_unsafe(
    SOC_SAND_IN  int                             unit,
    SOC_SAND_INOUT SOC_PB_PP_IP_ROUTING_TABLE_RANGE          *block_range_key,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_KEY         *route_key,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_INFO        *routes_info,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_STATUS           *routes_status,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_LOCATION         *routes_location,
    SOC_SAND_OUT uint32                              *nof_entries
  );

uint32
  soc_pb_pp_frwrd_ipv6_mc_route_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT SOC_PB_PP_IP_ROUTING_TABLE_RANGE              *block_range_key
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ipv6_mc_route_remove_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove IPv6 route entry from the routing table.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_KEY             *route_key -
 *     The routing key (IPv6 subnet and a virtual router id)
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
  soc_pb_pp_frwrd_ipv6_mc_route_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_KEY             *route_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  soc_pb_pp_frwrd_ipv6_mc_route_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_KEY             *route_key
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ipv6_mc_routing_table_clear_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Clear the IPv6 MC routing table.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv6_mc_routing_table_clear_unsafe(
    SOC_SAND_IN  int                                 unit
  );

uint32
  soc_pb_pp_frwrd_ipv6_mc_routing_table_clear_verify(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ipv6_vrf_info_set_unsafe
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
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_VRF_INFO                 *vrf_info -
 *     VRF information.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv6_vrf_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_VRF_INFO                 *vrf_info
  );

uint32
  soc_pb_pp_frwrd_ipv6_vrf_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_VRF_INFO                 *vrf_info
  );

uint32
  soc_pb_pp_frwrd_ipv6_vrf_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_frwrd_ipv6_vrf_info_set_unsafe" API.
 *     Refer to "soc_pb_pp_frwrd_ipv6_vrf_info_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv6_vrf_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IPV6_VRF_INFO                 *vrf_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ipv6_vrf_route_add_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add IPv6 route entry to the virtual routing table (VRF).
 *   Binds between Ipv6 route key (UC/MC IPv6-address\prefix)
 *   and a FEC entry identified by fec_id for a given virtual
 *   router. As a result of this operation, Unicast Ipv6
 *   packets designated to IP address matching the given key
 *   (as long there is no more-specific route key) will be
 *   routed according to the information in the FEC entry
 *   identified by fec_id.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx -
 *     Virtual router id. Range: 1 - 255.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_VPN_ROUTE_KEY            *route_key -
 *     The routing key IPv6 subnet MC/UC.
 *   SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_id -
 *     Routing information, according to which to route
 *     packets.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success -
 *     Whether the operation succeeds. Add operation may fail
 *     if there is no place in the ILM DB (LEM/LPM).
 * REMARKS:
 *   - use soc_ppd_frwrd_ip_routes_cache_mode_enable_set() to
 *   determine wehter to cache this route in SW or to write
 *   it directly into HW. - For VPN routing there is no
 *   difference in the lookup functionality between IPV6 MC
 *   and Ipv6 UC. - Supports virtual routing tables using the
 *   vrf_id
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv6_vrf_route_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_id,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  soc_pb_pp_frwrd_ipv6_vrf_route_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_IN  SOC_PB_PP_FEC_ID                              fec_id
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ipv6_vrf_route_get_unsafe
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
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_VPN_ROUTE_KEY            *route_key -
 *     The routing key IPv6 subnet MC/UC.
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
  soc_pb_pp_frwrd_ipv6_vrf_route_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_IN  uint8                                 exact_match,
    SOC_SAND_OUT SOC_PB_PP_FEC_ID                              *fec_id,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_STATUS               *route_status,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_LOCATION             *location,
    SOC_SAND_OUT uint8                                 *found
  );

uint32
  soc_pb_pp_frwrd_ipv6_vrf_route_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_IN  uint8                                 exact_match
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ipv6_vrf_route_get_block_unsafe
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
 *   SOC_SAND_OUT SOC_PB_PP_FRWRD_IPV6_VPN_ROUTE_KEY            *route_keys -
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
  soc_pb_pp_frwrd_ipv6_vrf_route_get_block_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx,
    SOC_SAND_INOUT SOC_PB_PP_IP_ROUTING_TABLE_RANGE              *block_range_key,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IPV6_VPN_ROUTE_KEY            *route_keys,
    SOC_SAND_OUT SOC_PB_PP_FEC_ID                              *fec_ids,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_STATUS               *routes_status,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_IP_ROUTE_LOCATION             *routes_location,
    SOC_SAND_OUT uint32                                  *nof_entries
  );

uint32
  soc_pb_pp_frwrd_ipv6_vrf_route_get_block_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx,
    SOC_SAND_INOUT SOC_PB_PP_IP_ROUTING_TABLE_RANGE              *block_range_key
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ipv6_vrf_route_remove_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove IPv6 route entry from the routing table of a
 *   virtual router (VRF).
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx -
 *     Virtual router id. Range: 1 - 255.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_VPN_ROUTE_KEY            *route_key -
 *     The routing key (IPv6 subnet and a virtual router id)
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
  soc_pb_pp_frwrd_ipv6_vrf_route_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_VPN_ROUTE_KEY            *route_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  soc_pb_pp_frwrd_ipv6_vrf_route_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_VPN_ROUTE_KEY            *route_key
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ipv6_vrf_routing_table_clear_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Clear IPv6 routing table of VRF
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
  soc_pb_pp_frwrd_ipv6_vrf_routing_table_clear_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx
  );

uint32
  soc_pb_pp_frwrd_ipv6_vrf_routing_table_clear_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VRF_ID                              vrf_ndx
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ipv6_vrf_all_routing_tables_clear_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Clear IPv6 routing tables for all VRFs.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_ipv6_vrf_all_routing_tables_clear_unsafe(
    SOC_SAND_IN  int                                 unit
  );

uint32
  soc_pb_pp_frwrd_ipv6_vrf_all_routing_tables_clear_verify(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ipv6_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   soc_pb_pp_api_frwrd_ipv6 module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_frwrd_ipv6_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_ipv6_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   soc_pb_pp_api_frwrd_ipv6 module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_frwrd_ipv6_get_errs_ptr(void);

uint32
  SOC_PB_PP_FRWRD_IPV6_ROUTER_DEFAULT_ACTION_VAL_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_ROUTER_DEFAULT_ACTION_VAL *info
  );

uint32
  SOC_PB_PP_FRWRD_IPV6_ROUTER_DEFAULT_ACTION_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_ROUTER_DEFAULT_ACTION *info
  );

uint32
  SOC_PB_PP_FRWRD_IPV6_ROUTER_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_ROUTER_INFO *info
  );

uint32
  SOC_PB_PP_FRWRD_IPV6_GLBL_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_GLBL_INFO *info
  );

uint32
  SOC_PB_PP_FRWRD_IPV6_VRF_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_VRF_INFO *info
  );

uint32
  SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_UC_ROUTE_KEY *info
  );

uint32
  SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_KEY *info
  );

uint32
  SOC_PB_PP_FRWRD_IPV6_VPN_ROUTE_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_VPN_ROUTE_KEY *info
  );

uint32
  SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_IPV6_MC_ROUTE_INFO *info
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PP_FRWRD_IPV6_INCLUDED__*/
#endif

