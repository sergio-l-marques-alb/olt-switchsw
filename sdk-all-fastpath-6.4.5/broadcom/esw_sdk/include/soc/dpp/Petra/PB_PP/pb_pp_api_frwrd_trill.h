/* $Id: pb_pp_api_frwrd_trill.h,v 1.6 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/include/soc_pb_pp_api_frwrd_trill.h
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

#ifndef __SOC_PB_PP_API_FRWRD_TRILL_INCLUDED__
/* { */
#define __SOC_PB_PP_API_FRWRD_TRILL_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_frwrd_trill.h>
#include <soc/dpp/PPD/ppd_api_lif.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_api_general.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_lif.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     When Adding multicast route, the following fields of the
 *     route key are optional: FID, Ingress NickName. and
 *     Adjacent NickName.                                      */

/*     Use to accept SA from all source system ports in
 *     soc_ppd_frwrd_trill_ adj _info_set ()                       */

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

typedef SOC_PPC_TRILL_MC_MASKED_FIELDS                         SOC_PB_PP_TRILL_MC_MASKED_FIELDS;
typedef SOC_PPC_TRILL_MC_ROUTE_KEY                             SOC_PB_PP_TRILL_MC_ROUTE_KEY;
typedef SOC_PPC_TRILL_ADJ_INFO                                 SOC_PB_PP_TRILL_ADJ_INFO;
typedef SOC_PPC_FRWRD_TRILL_GLOBAL_INFO                        SOC_PB_PP_FRWRD_TRILL_GLOBAL_INFO;

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
 *   soc_pb_pp_frwrd_trill_multicast_key_mask_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the key type of TRILL multicast routes lookup. The
 *   following fields are optional: Ing-Nick-key;
 *   Adjacent-EEP-key; FID-key
 * INPUT:
 *   SOC_SAND_IN  int                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_TRILL_MC_MASKED_FIELDS  *masked_fields -
 *     Trill multicast route fields to be masked upon lookup
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_trill_multicast_key_mask_set(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PB_PP_TRILL_MC_MASKED_FIELDS  *masked_fields
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_frwrd_trill_multicast_key_mask_set" API.
 *     Refer to "soc_pb_pp_frwrd_trill_multicast_key_mask_set" API
 *     for details.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_trill_multicast_key_mask_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_OUT SOC_PB_PP_TRILL_MC_MASKED_FIELDS  *masked_fields
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_trill_unicast_route_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Map nick-name to a FEC entry ID. Used for forwarding
 *   packets with the nick name as destination to the FEC,
 *   and to associate the FEC as learning information, upon
 *   receiving packets with the Nick-Name as the source
 *   address
 * INPUT:
 *   SOC_SAND_IN  int                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_LIF_ID                  lif_index -
 *     Index to the Logical interfaces table
 *   SOC_SAND_IN  uint32                      nickname_key -
 *     TRILL NickName key
 *   SOC_SAND_IN  SOC_PPD_L2_LIF_TRILL_INFO    *trill_info-
 *     trill_info attributes:
 *     fec_id - FEC Entry ID. The FEC is expected to hold the out-port as
 *              destination and an EEP that points to the link layer
 *              encapsulation towards the adjacent RBridge. The FEC ID
 *              may also point to ECMP with list of adjacent RBridges,
 *              and the connected ports.
 *     learn_enable - indicate if learn enable on this interface.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE          *success -
 *     SOC_SAND_FAILURE_OUT_OF_RESOURCES: There is no space in
 *     SEMSAND_FAILURE_OUT_OF_RESOURCES_2: There is no space in
 *     LEM
 * REMARKS:
 *   Written to both the Logical interfaces table for
 *   learning purposes, and to the LEM table for forwarding
 *   purposes
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_trill_unicast_route_add(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID            lif_index,
    SOC_SAND_IN  uint32                      nickname_key,
    SOC_SAND_IN  SOC_PPD_L2_LIF_TRILL_INFO   *trill_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE    *success
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_trill_unicast_route_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get mapping of TRILL nickname to FEC ID and LIF index
 * INPUT:
 *   SOC_SAND_IN  int                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                      nickname_key -
 *     TRILL NickName key
 *   SOC_SAND_OUT SOC_PB_PP_LIF_ID                  *lif_index -
 *     Index to the Logical interfaces table
 *   SOC_SAND_OUT SOC_PB_PP_L2_LIF_TRILL_INFO   *trill_info -
 *     Trill attributes(FEC Entry ID,learn_enable).
 *   SOC_SAND_OUT uint8                     *is_found -
 *     indicates if entry was found
 * REMARKS:
 *   The 'lif_index' is returned to the user to enable the
 *   LIF table management
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_trill_unicast_route_get(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  uint32                        nickname_key,
    SOC_SAND_OUT SOC_PB_PP_LIF_ID             *lif_index,
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_TRILL_INFO  *trill_info,
    SOC_SAND_OUT uint8                        *is_found
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_trill_unicast_route_remove
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove TRILL nick-name
 * INPUT:
 *   SOC_SAND_IN  int                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                      nickname_key -
 *     TRILL NickName key
 *   SOC_SAND_OUT SOC_PB_PP_LIF_ID                  *lif_index -
 *     Index to the Logical interfaces table
 * REMARKS:
 *   Removed from both the SEM table and to the LEM table. The
 *   'lif_index' is returned to user to enable his management
 *   of the LIF table
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_trill_unicast_route_remove(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                      nickname_key,
    SOC_SAND_OUT SOC_PB_PP_LIF_ID                  *lif_index
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_trill_multicast_route_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Map a TRILL distribution tree to a FEC
 * INPUT:
 *   SOC_SAND_IN  int                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_TRILL_MC_ROUTE_KEY      *trill_mc_key -
 *     TRILL multicast key. Contain the Distribution tree, and
 *     may also contain: FID, Originator RBridge Nick-Name, and
 *     Adjacent RBridge Nick-Name
 *   SOC_SAND_IN  uint32                      mc_id -
 *     Multicast ID. The multicast ID should contain the L2
 *     assigned forwarder ports and a list of the adjacent
 *     RBridges, according to the distribution tree topology.
 *     An adjacent RBridge is pointed via the port connected to
 *     it, and a Copy Unique Data that points to the
 *     encapsulation pointer that contains its link-layer
 *     encapsulation.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE          *success -
 *     SOC_SAND_FAILURE_OUT_OF_RESOURCES: There is no space in the
 *     Exact Match table
 * REMARKS:
 *   To fully support TRILL RPF,
 *   soc_ppd_frwrd_trill_multicast_route_set() is called for each
 *   Ingress-NickName, each time with the allowed valid
 *   Adjacent, according to the tree structure. This way, the
 *   Trill RPF is supported. Trill RPF validate that the
 *   packet did not arrive to the RBridge from adjacent that
 *   is not allowed for the packet originator. ECMP with list
 *   of multicast FEC destination is supported. However, the
 *   common usage of this option is when a flooding is
 *   originated upon an unknown MAC address or multicast MAC
 *   address. When the key to the multicast is the TRILL
 *   header, the distribution tree is already chosen, and
 *   therefore the route is to a known FEC that points to a
 *   multicast group.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_trill_multicast_route_add(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PB_PP_TRILL_MC_ROUTE_KEY      *trill_mc_key,
    SOC_SAND_IN  uint32                      mc_id,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE          *success
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_trill_multicast_route_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get Mapping of TRILL distribution tree to a FEC
 * INPUT:
 *   SOC_SAND_IN  int                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_TRILL_MC_ROUTE_KEY      *trill_mc_key -
 *     TRILL multicast key. Contain the Distribution tree, and
 *     may also contain: FID, Originator RBridge Nick-Name, and
 *     Adjacent RBridge Nick-Name
 *   SOC_SAND_OUT uint32                      *mc_id -
 *     Multicast ID. The multicast ID should contain the L2
 *     assigned forwarder ports and a list of the adjacent
 *     RBridges, according to the distribution tree topology.
 *     An adjacent RBridge is pointed via the port connected to
 *     it, and a Copy Unique Data that points to the
 *     encapsulation pointer that contains its link-layer
 *     encapsulation.
 *   SOC_SAND_OUT uint8                     *is_found -
 *     indicates if entry was found
 * REMARKS:
 *   none
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_trill_multicast_route_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PB_PP_TRILL_MC_ROUTE_KEY      *trill_mc_key,
    SOC_SAND_OUT uint32                      *mc_id,
    SOC_SAND_OUT uint8                     *is_found
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_trill_multicast_route_remove
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove a TRILL distribution tree mapping
 * INPUT:
 *   SOC_SAND_IN  int                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_TRILL_MC_ROUTE_KEY      *trill_mc_key -
 *     TRILL multicast key. Contain the Distribution tree, and
 *     may also contain: FID, Originator RBridge Nick-Name, and
 *     Adjacent RBridge Nick-Name
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_trill_multicast_route_remove(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PB_PP_TRILL_MC_ROUTE_KEY      *trill_mc_key
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_trill_adj_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Map SA MAC adress to expected adjacent EEP and expected
 *   system port in SA-Based_adj db. Used for authenticating
 *   incoming trill packets
 * INPUT:
 *   SOC_SAND_IN  int                     unit -
 *     The device id
 *   SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS           *mac_address_key -
 *     MAC address to set authentication over it.
 *   SOC_SAND_IN  SOC_PB_PP_TRILL_ADJ_INFO          *mac_auth_info -
 *     Authentication information for the given MAC.
 *   SOC_SAND_IN  uint8                     enable -
 *     Set to TRUE to add the entry, and to FALSE to remove the
 *     entry from the DB.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE          *success -
 *     Whether the operation succeeds (upon add). Add operation
 *     may fail if there is no place in the SA Auth DB.
 * REMARKS:
 *   - The DB used for SA Based Adj is also shared for SA
 *   Authorization
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_trill_adj_info_set(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS           *mac_address_key,
    SOC_SAND_IN  SOC_PB_PP_TRILL_ADJ_INFO          *mac_auth_info,
    SOC_SAND_IN  uint8                     enable,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE          *success
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_frwrd_trill_adj_info_set" API.
 *     Refer to "soc_pb_pp_frwrd_trill_adj_info_set" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_trill_adj_info_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS           *mac_address_key,
    SOC_SAND_OUT SOC_PB_PP_TRILL_ADJ_INFO          *mac_auth_info,
    SOC_SAND_OUT uint8                     *enable
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_frwrd_trill_global_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set TRILL global attributes
 * INPUT:
 *   SOC_SAND_IN  int                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_FRWRD_TRILL_GLOBAL_INFO *glbl_info -
 *     global settings info
 * REMARKS:
 *   Sets TRILL initiale TTL value
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_trill_global_info_set(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PB_PP_FRWRD_TRILL_GLOBAL_INFO *glbl_info
  );

/*********************************************************************
 *     Gets the configuration set by the
 *     "soc_pb_pp_frwrd_trill_global_info_set" API.
 *     Refer to "soc_pb_pp_frwrd_trill_global_info_set" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_frwrd_trill_global_info_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_OUT SOC_PB_PP_FRWRD_TRILL_GLOBAL_INFO *glbl_info
  );

void
  SOC_PB_PP_TRILL_MC_MASKED_FIELDS_clear(
    SOC_SAND_OUT SOC_PB_PP_TRILL_MC_MASKED_FIELDS *info
  );

void
  SOC_PB_PP_TRILL_MC_ROUTE_KEY_clear(
    SOC_SAND_OUT SOC_PB_PP_TRILL_MC_ROUTE_KEY *info
  );

void
  SOC_PB_PP_TRILL_ADJ_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_TRILL_ADJ_INFO *info
  );

void
  SOC_PB_PP_FRWRD_TRILL_GLOBAL_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_FRWRD_TRILL_GLOBAL_INFO *info
  );

#if SOC_PB_PP_DEBUG_IS_LVL1

void
  SOC_PB_PP_TRILL_MC_MASKED_FIELDS_print(
    SOC_SAND_IN  SOC_PB_PP_TRILL_MC_MASKED_FIELDS *info
  );

void
  SOC_PB_PP_TRILL_MC_ROUTE_KEY_print(
    SOC_SAND_IN  SOC_PB_PP_TRILL_MC_ROUTE_KEY *info
  );

void
  SOC_PB_PP_TRILL_ADJ_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_TRILL_ADJ_INFO *info
  );

void
  SOC_PB_PP_FRWRD_TRILL_GLOBAL_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_FRWRD_TRILL_GLOBAL_INFO *info
  );

#endif /* SOC_PB_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PP_API_FRWRD_TRILL_INCLUDED__*/
#endif

