
/* $Id: arad_pp_api_frwrd_fcf.h,v 1.1 Broadcom SDK $
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

#ifndef __ARAD_PP_API_FRWRD_FCF_INCLUDED__
/* { */
#define __ARAD_PP_API_FRWRD_FCF_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_frwrd_fcf.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define ARAD_PP_FRWRD_FCF_ZONING_ACT_FRWRD SOC_PPC_FRWRD_FCF_ZONING_ACT_FRWRD
#define ARAD_PP_FRWRD_FCF_ZONING_ACT_REDIRECT  SOC_PPC_FRWRD_FCF_ZONING_ACT_REDIRECT

/* local domain: add to SEM */
#define ARAD_PP_FRWRD_FCF_ROUTE_LOCAL_DOMAIN    SOC_PPC_FRWRD_FCF_ROUTE_LOCAL_DOMAIN
/* add domain to LPM*/
#define ARAD_PP_FRWRD_FCF_ROUTE_DOMAIN          SOC_PPC_FRWRD_FCF_ROUTE_DOMAIN
/* host add to LEM table (24 bits) */
#define ARAD_PP_FRWRD_FCF_ROUTE_HOST            SOC_PPC_FRWRD_FCF_ROUTE_HOST
/* host add to LEM table (24 bits) S_ID*/
#define ARAD_PP_FRWRD_FCF_ROUTE_HOST_NPORT      SOC_PPC_FRWRD_FCF_ROUTE_HOST_NPORT

/* flags for fcf_zoning_table_clear */
/* remove entries match VFT */
#define ARAD_PP_FRWRD_FCF_ZONE_REMOVE_BY_VFT SOC_PPC_FRWRD_FCF_ZONE_REMOVE_BY_VFT
/* remove entries match D_ID */
#define ARAD_PP_FRWRD_FCF_ZONE_REMOVE_BY_D_ID SOC_PPC_FRWRD_FCF_ZONE_REMOVE_BY_D_ID
/* remove entries match S_ID*/
#define ARAD_PP_FRWRD_FCF_ZONE_REMOVE_BY_S_ID SOC_PPC_FRWRD_FCF_ZONE_REMOVE_BY_S_ID


/* Virtual Fabric Identifier */
typedef SOC_PPC_FRWRD_FCF_VFI ARAD_PP_FRWRD_FCF_VFI;

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



#define ARAD_PP_FRWRD_FCF_ROUTE_STATUS_COMMITED                 SOC_PPC_FRWRD_FCF_ROUTE_STATUS_COMMITED
#define ARAD_PP_FRWRD_FCF_ROUTE_STATUS_PEND_ADD                 SOC_PPC_FRWRD_FCF_ROUTE_STATUS_PEND_ADD
#define ARAD_PP_FRWRD_FCF_ROUTE_STATUS_PEND_REMOVE              SOC_PPC_FRWRD_FCF_ROUTE_STATUS_PEND_REMOVE
#define ARAD_PP_FRWRD_FCF_ROUTE_STATUS_ACCESSED              SOC_PPC_FRWRD_FCF_ROUTE_STATUS_ACCESSED
#define ARAD_PP_NOF_FRWRD_FCF_ROUTE_STATUSS                     SOC_PPC_NOF_FRWRD_FCF_ROUTE_STATUSS
typedef SOC_PPC_FRWRD_FCF_ROUTE_STATUS                          ARAD_PP_FRWRD_FCF_ROUTE_STATUS;

typedef SOC_PPC_FRWRD_FCF_GLBL_INFO                    ARAD_PP_FRWRD_FCF_GLBL_INFO;
typedef SOC_PPC_FRWRD_FCF_ROUTE_KEY                    ARAD_PP_FRWRD_FCF_ROUTE_KEY;
typedef SOC_PPC_FRWRD_FCF_ROUTE_INFO                   ARAD_PP_FRWRD_FCF_ROUTE_INFO;
typedef SOC_PPC_FRWRD_FCF_ZONING_KEY                   ARAD_PP_FRWRD_FCF_ZONING_KEY;
typedef SOC_PPC_FRWRD_FCF_ZONING_INFO                  ARAD_PP_FRWRD_FCF_ZONING_INFO;

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
 *   arad_pp_frwrd_fcf_glbl_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Setting global information of the IP routing (including
 *   resources to use)
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_FRWRD_FCF_GLBL_INFO                *glbl_info -
 *     Global information.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_fcf_glbl_info_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_FCF_GLBL_INFO                *glbl_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_frwrd_fcf_glbl_info_set" API.
 *     Refer to "arad_pp_frwrd_fcf_glbl_info_set" API for details.
*********************************************************************/
uint32
  arad_pp_frwrd_fcf_glbl_info_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT ARAD_PP_FRWRD_FCF_GLBL_INFO                *glbl_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_frwrd_fcf_route_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add FCF route entry to the routing table. Binds between
 *   FCF Unicast route key (FCF-address/prefix) and a FEC
 *   entry identified by route_info for a given router. As a
 *   result of this operation, Unicast FCF packets
 *   designated to the IP address matching the given key (as
 *   long there is no more-specific route key) will be routed
 *   according to the information in the FEC entry identified
 *   by route_info.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_FRWRD_FCF_ROUTE_KEY             *route_key -
 *     The routing key: FCF subnet
 *   SOC_SAND_IN  ARAD_PP_FRWRD_FCF_ROUTE_INFO            *route_info -
 *     FEC ID.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success -
 *     Whether the operation succeeds. Add operation may fail
 *     if there is no place in the routing DB (LEM/LPM).
 * REMARKS:
 *   - use arad_pp_frwrd_fcf_routes_cache_mode_enable_set() to
 *   determine wehter to cache this route in SW or to write
 *   it directly into HW.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_fcf_route_add(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_FCF_ROUTE_KEY             *route_key,
    SOC_SAND_IN  ARAD_PP_FRWRD_FCF_ROUTE_INFO            *route_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                *success
  );

/*********************************************************************
* NAME:
 *   arad_pp_frwrd_fcf_route_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets the routing information (system-fec-id) associated
 *   with the given route key.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_FRWRD_FCF_ROUTE_KEY             *route_key -
 *     The routing key (FCF subnet and a virtual fabric identifier id)
 *   SOC_SAND_IN  uint8                               exact_match -
 *     If TRUE returns exact match only; if FALSE returns
 *     longest prefix match.
 *   SOC_SAND_OUT ARAD_PP_FRWRD_FCF_ROUTE_INFO                  *route_info -
 *     Routing information (system-fec-id).
 *   SOC_SAND_OUT ARAD_PP_FRWRD_FCF_ROUTE_STATUS               *route_status -
 *     Indicates whether the returned entry exist in HW
 *     (commited) or pending (either for remove or addition),
 *     relevant only if found is TRUE.
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
  arad_pp_frwrd_fcf_route_get(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_FCF_ROUTE_KEY         *route_key,
    SOC_SAND_IN  uint8                               exact_match,
    SOC_SAND_OUT ARAD_PP_FRWRD_FCF_ROUTE_INFO        *route_info,
    SOC_SAND_OUT ARAD_PP_FRWRD_FCF_ROUTE_STATUS      *route_status,
    SOC_SAND_OUT uint8                               *found
  );

/*********************************************************************
* NAME:
 *   arad_pp_frwrd_fcf_route_get_block
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets the FCF UC routing table.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_INOUT ARAD_PP_IP_ROUTING_TABLE_RANGE              *block_range -
 *     Defines the range and the order of the block of routing
 *     entries.
 *   SOC_SAND_OUT ARAD_PP_FRWRD_FCF_ROUTE_KEY             *route_keys -
 *     Array of routing keys (FCF subnets)
 *   SOC_SAND_OUT ARAD_PP_FRWRD_FCF_ROUTE_INFO        *routes_info -
 *     Array of fec-ids.
 *   SOC_SAND_OUT ARAD_PP_FRWRD_FCF_ROUTE_STATUS               *routes_status -
 *     For each route Indicates whether it exists in HW
 *     (commited) or pending (either for remove or addition).
 *     Set this parameter to NULL in order to be ignored
 *   SOC_SAND_OUT uint32                                *nof_entries -
 *     Number of entries in returned arrays.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_fcf_route_get_block(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_INOUT ARAD_PP_IP_ROUTING_TABLE_RANGE      *block_range,
    SOC_SAND_OUT ARAD_PP_FRWRD_FCF_ROUTE_KEY            *route_keys,
    SOC_SAND_OUT ARAD_PP_FRWRD_FCF_ROUTE_INFO           *routes_info,
    SOC_SAND_OUT ARAD_PP_FRWRD_FCF_ROUTE_STATUS         *routes_status,
    SOC_SAND_OUT uint32                                 *nof_entries
  );

/*********************************************************************
* NAME:
 *   arad_pp_frwrd_fcf_route_remove
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove entry from the routing table.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_FRWRD_FCF_ROUTE_KEY             *route_key -
 *     The routing key (FCF subnet and a virtual fabric identifier id)
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                    *success -
 *     Whether the operation succeeds. Remove operation may
 *     fail if there is no place in the FCF DB (LPM), due to
 *     fail of decompression.
 * REMARKS:
 *   - use arad_pp_frwrd_fcf_routes_cache_mode_enable_set() to
 *   determine wehter to cache this route in SW or to write
 *   it directly into HW.- remove un-exist entry considered
 *   as succeeded operation
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_fcf_route_remove(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_FCF_ROUTE_KEY          *route_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE             *success
  );

/*********************************************************************
* NAME:
 *   arad_pp_frwrd_fcf_routing_table_clear
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Clear the FCF UC routing table.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_fcf_routing_table_clear(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_FCF_VFI                vfi_ndx,
    SOC_SAND_IN  uint32                               flags
   );

/*********************************************************************
* NAME:
 *   arad_pp_frwrd_fcf_zoning_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add FCF entry to the zoning table. Binds between Host and
 *   next hop information.
 * INPUT:
 *   SOC_SAND_IN  int                           unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_FRWRD_FCF_ZONING_KEY             *zoning_key -
 *     VFI-ID and IP address
 *   SOC_SAND_IN  ARAD_PP_FRWRD_FCF_ZONING_INFO      *routing_info -
 *     Routing information. See remarks.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                *success -
 *     Whether the operation succeeds. Add operation may fail
 *     if there is no place in the LEM DB.
 * REMARKS:
 *   - use arad_pp_frwrd_fcf_routes_cache_mode_enable_set() to
 *   determine wehter to cache this route in SW or to write
 *   it directly into HW. - Supports virtual routing tables
 *   using the vfi_id - user has to supply in routing info
 *   FEC-ptr which set the out-RIF, TM-destination and EEP.
 *   Optionally, user can supply EEP directly from the host
 *   table.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_fcf_zoning_add(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_FCF_ZONING_KEY             *zoning_key,
    SOC_SAND_IN  ARAD_PP_FRWRD_FCF_ZONING_INFO      *routing_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                *success
  );

/*********************************************************************
* NAME:
 *   arad_pp_frwrd_fcf_zoning_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets the routing information associated with the given
 *   route key on VFI.
 * INPUT:
 *   SOC_SAND_IN  int                           unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_FRWRD_FCF_ZONING_KEY             *zoning_key -
 *     The routing key (FCF address and a virtual fabric identifier id)
 *   SOC_SAND_OUT ARAD_PP_FRWRD_FCF_ROUTE_INFO      *routing_info -
 *     Routing information (fec-id).
 *   SOC_SAND_OUT ARAD_PP_FRWRD_FCF_ROUTE_STATUS           *route_status -
 *     Indicates whether the returned entry exist in HW
 *     (commited) or pending (either for remove or addition),
 *     relevant only if found is TRUE.
 *   SOC_SAND_OUT uint8                           *found -
 *     If TRUE then route_val has valid data.
 * REMARKS:
 *   - If the host is not found in the host DB, then the host
 *   (exact route/32) is lookuped in the LPM
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_fcf_zoning_get(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_FCF_ZONING_KEY             *zoning_key,
    SOC_SAND_OUT ARAD_PP_FRWRD_FCF_ZONING_INFO      *routing_info,
    SOC_SAND_OUT ARAD_PP_FRWRD_FCF_ROUTE_STATUS           *route_status,
    SOC_SAND_OUT uint8                           *found
  );

/*********************************************************************
* NAME:
 *   arad_pp_frwrd_fcf_zoning_get_block
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Gets the zoning table.
 * INPUT:
 *   SOC_SAND_IN  int                           unit -
 *     Identifier of the device to access.
 *   SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE              *block_range_key -
 *     Defines the range of routing entries to retrieve.
 *   SOC_SAND_OUT ARAD_PP_FRWRD_FCF_ZONING_KEY             *zoning_keys -
 *     array of routing keys
 *   SOC_SAND_OUT ARAD_PP_FRWRD_FCF_ROUTE_INFO      *routes_info -
 *     Array of routing information for each host key.
 *   SOC_SAND_OUT ARAD_PP_FRWRD_FCF_ROUTE_STATUS           *routes_status -
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
  arad_pp_frwrd_fcf_zoning_get_block(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE              *block_range_key,
    SOC_SAND_IN  uint32                                  flags,
    SOC_SAND_OUT ARAD_PP_FRWRD_FCF_ZONING_KEY             *zoning_keys,
    SOC_SAND_OUT ARAD_PP_FRWRD_FCF_ZONING_INFO      *routes_info,
    SOC_SAND_OUT ARAD_PP_FRWRD_FCF_ROUTE_STATUS           *routes_status,
    SOC_SAND_OUT uint32                            *nof_entries
  );

/*********************************************************************
* NAME:
 *   arad_pp_frwrd_fcf_zoning_remove
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove FCF route entry from the routing table of a
 *   virtual fabric identifier (VFI).
 * INPUT:
 *   SOC_SAND_IN  int                           unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_FRWRD_FCF_ZONING_KEY             *zoning_key -
 *     The routing key (FCF subnet and a virtual fabric identifier id)
 * REMARKS:
 *   - use arad_pp_frwrd_fcf_routes_cache_mode_enable_set() to
 *   determine wehter to cache this route in SW or to write
 *   it directly into HW.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_fcf_zoning_remove(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_FCF_ZONING_KEY             *zoning_key
  );


/*********************************************************************
* NAME:
 *   arad_pp_frwrd_fcf_zoning_table_clear
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Clear FCF routing table of VFI
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_FRWRD_FCF_VFI                vfi -
 *     Virtual Fabric Identifier id. Range: 1 - 255.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_fcf_zoning_table_clear(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               flags,
    SOC_SAND_IN  ARAD_PP_FRWRD_FCF_ZONING_KEY         *key,
    SOC_SAND_IN  ARAD_PP_FRWRD_FCF_ZONING_INFO        *info
  );



void
  ARAD_PP_FRWRD_FCF_GLBL_INFO_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_FCF_GLBL_INFO *info
  );

void
  ARAD_PP_FRWRD_FCF_ROUTE_KEY_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_FCF_ROUTE_KEY *info
  );

void
  ARAD_PP_FRWRD_FCF_ROUTE_INFO_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_FCF_ROUTE_INFO *info
  );


void
  ARAD_PP_FRWRD_FCF_ZONING_KEY_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_FCF_ZONING_KEY *info
  );

void
  ARAD_PP_FRWRD_FCF_ZONING_INFO_clear(
    SOC_SAND_OUT ARAD_PP_FRWRD_FCF_ZONING_INFO *info
  );

#if ARAD_PP_DEBUG_IS_LVL1


void
  ARAD_PP_FRWRD_FCF_GLBL_INFO_print(
    SOC_SAND_IN ARAD_PP_FRWRD_FCF_GLBL_INFO *info
  );

void
  ARAD_PP_FRWRD_FCF_ROUTE_KEY_print(
    SOC_SAND_IN ARAD_PP_FRWRD_FCF_ROUTE_KEY *info
  );

void
  ARAD_PP_FRWRD_FCF_ROUTE_INFO_print(
    SOC_SAND_IN ARAD_PP_FRWRD_FCF_ROUTE_INFO *info
  );


void
  ARAD_PP_FRWRD_FCF_ZONING_KEY_print(
    SOC_SAND_IN ARAD_PP_FRWRD_FCF_ZONING_KEY *info
  );

void
  ARAD_PP_FRWRD_FCF_ZONING_INFO_print(
    SOC_SAND_IN ARAD_PP_FRWRD_FCF_ZONING_INFO *info
  );

#endif /* ARAD_PP_DEBUG_IS_LVL1 */

#if ARAD_PP_DEBUG_IS_LVL3


void
  arad_pp_frwrd_fcf_glbl_info_set_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_FCF_GLBL_INFO                *glbl_info
  );

void
  arad_pp_frwrd_fcf_glbl_info_get_print(
    SOC_SAND_IN  int                               unit
  );

void
  arad_pp_frwrd_fcf_route_add_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_FCF_ROUTE_KEY             *route_key,
    SOC_SAND_IN  ARAD_PP_FRWRD_FCF_ROUTE_INFO            *route_info
  );

void
  arad_pp_frwrd_fcf_route_get_print(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_FCF_ROUTE_KEY         *route_key,
    SOC_SAND_IN  uint8                               exact_match
  );

void
  arad_pp_frwrd_fcf_route_get_block_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_INOUT ARAD_PP_IP_ROUTING_TABLE_RANGE              *block_range
  );

void
  arad_pp_frwrd_fcf_route_remove_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_FCF_ROUTE_KEY          *route_key
  );

void
  arad_pp_frwrd_fcf_routing_table_clear_print(
    SOC_SAND_IN  int                               unit
    SOC_SAND_IN  SOC_PPD_FRWRD_FCF_VFI                vfi_ndx,
    SOC_SAND_IN  uint32                               flags
  );

void
  arad_pp_frwrd_fcf_zoning_add_print(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_FCF_ZONING_KEY             *zoning_key,
    SOC_SAND_IN  ARAD_PP_FRWRD_FCF_ZONING_INFO      *routing_info
  );

void
  arad_pp_frwrd_fcf_zoning_get_print(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_FCF_ZONING_KEY             *zoning_key
  );

void
  arad_pp_frwrd_fcf_zoning_get_block_print(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE              *block_range_key
  );

void
  arad_pp_frwrd_fcf_zoning_remove_print(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_FCF_ZONING_KEY             *zoning_key
  );

void
  arad_pp_frwrd_fcf_zoning_table_clear_print(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  ARAD_PP_FRWRD_FCF_VFI                vfi_ndx
    SOC_SAND_IN  uint32                               flags,	
  );


#endif /* ARAD_PP_DEBUG_IS_LVL3 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_PP_API_FRWRD_FCF_INCLUDED__*/
#endif


