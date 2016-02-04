/* $Id: arad_pp_api_rif.h,v 1.13 Broadcom SDK $
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

#ifndef __ARAD_PP_API_RIF_INCLUDED__
/* { */
#define __ARAD_PP_API_RIF_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_rif.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_general.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_mpls_term.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     Indicates that the RIF is not updated.                  */
#define  ARAD_PP_RIF_IS_NOT_UPDATED (SOC_PPC_RIF_IS_NOT_UPDATED)

/*
 * termination key to include both SIP
 */
#define  ARAD_PP_RIF_IP_TERM_FLAG_IPV6       SOC_PPC_RIF_IP_TERM_FLAG_IPV6
#define  ARAD_PP_RIF_IP_TERM_FLAG_KEY_ONLY   SOC_PPC_RIF_IP_TERM_FLAG_KEY_ONLY
#define  ARAD_PP_RIF_IP_TERM_IPMC_BIDIR      SOC_PPC_RIF_IP_TERM_IPMC_BIDIR
#define  ARAD_PP_RIF_IP_TERM_FLAG_USE_DIP_DUMMY (SOC_PPC_RIF_IP_TERM_FLAG_USE_DIP_DUMMY)

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

typedef SOC_PPC_RIF_MPLS_LABELS_RANGE                        ARAD_PP_RIF_MPLS_LABELS_RANGE;
typedef SOC_PPC_MPLS_LABEL_RIF_KEY                           ARAD_PP_MPLS_LABEL_RIF_KEY;
typedef SOC_PPC_RIF_INFO                                     ARAD_PP_RIF_INFO;
typedef SOC_PPC_RIF_IP_TERM_INFO                             ARAD_PP_RIF_IP_TERM_INFO;
typedef SOC_PPC_RIF_IP_TERM_KEY                              ARAD_PP_RIF_IP_TERM_KEY;

/*     SOC_PPD_RIF_MPLS_LABEL_XXX flags */
#define ARAD_PP_RIF_MPLS_LABEL_FRR                          (SOC_PPC_RIF_MPLS_LABEL_FRR)
#define ARAD_PP_RIF_MPLS_LABEL_COUPLING                     (SOC_PPC_RIF_MPLS_LABEL_COUPLING)
#define ARAD_PP_RIF_MPLS_LABEL_ELI                          (SOC_PPC_RIF_MPLS_LABEL_ELI)
#define ARAD_PP_RIF_MPLS_LABEL_EXPECT_BOS                   (SOC_PPC_RIF_MPLS_LABEL_EXPECT_BOS)
#define ARAD_PP_RIF_MPLS_LABEL_LOOKUP_NONE                  (SOC_PPC_RIF_MPLS_LABEL_LOOKUP_NONE)

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
 *   arad_pp_rif_mpls_labels_range_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the MPLS labels that may be mapped to Router
 *   Interfaces
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_RIF_MPLS_LABELS_RANGE               *rif_labels_range -
 *     First and Last MPLS Labels to be mapped to Router
 *     Interfaces. Range Size: 0-896K
 * REMARKS:
 *   - Arad-B: Ignored- T20E: Set the MPLS labels that may
 *   be mapped to Router Interfaces.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_rif_mpls_labels_range_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_RIF_MPLS_LABELS_RANGE               *rif_labels_range
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_rif_mpls_labels_range_set" API.
 *     Refer to "arad_pp_rif_mpls_labels_range_set" API for
 *     details.
*********************************************************************/
uint32
  arad_pp_rif_mpls_labels_range_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT ARAD_PP_RIF_MPLS_LABELS_RANGE               *rif_labels_range
  );

/*********************************************************************
* NAME:
 *   arad_pp_rif_mpls_label_map_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Enable MPLS labels termination and setting the Router
 *   interface according to the terminated MPLS label.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_MPLS_LABEL_RIF_KEY                  *mpls_key -
 *     The MPLS label, and optionally the VSID, to be
 *     terminated and mapped to RIF
 *   SOC_SAND_IN  ARAD_PP_LIF_ID                              lif_index -
 *     LIF table index
 *   SOC_SAND_IN  ARAD_PP_MPLS_TERM_INFO                      *term_info -
 *     MPLS Termination info including type of termination
 *     (pipe/uniform), RIF
 *   SOC_SAND_IN  ARAD_PP_RIF_INFO                            *rif_info -
 *     RIF attributes. RIF ID, ...
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success -
 *     SOC_SAND_FAILURE_OUT_OF_RESOURCES: There is no space in the
 *     Exact Match table
 * REMARKS:
 *   T20E: The lif_index is ignored. Terminating the label via
 *   'soc_ppd_rif_mpls_label_map_add()' enables:- Mapping to a
 *   dedicated RIF, while range termination map to a global
 *   RIF- Termination of labels outside the range-
 *   Termination according to both MPLS label and the VSID,
 *   and not only according to the MPLS label. The VSID was
 *   assigned to the packet, according to the Link Layer
 *   Ethernet header's AC.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_rif_mpls_label_map_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_MPLS_LABEL_RIF_KEY                  *mpls_key,
    SOC_SAND_IN  ARAD_PP_LIF_ID                              lif_index,
    SOC_SAND_IN  ARAD_PP_MPLS_TERM_INFO                      *term_info,
    SOC_SAND_IN  ARAD_PP_RIF_INFO                            *rif_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

/*********************************************************************
* NAME:
 *   arad_pp_rif_mpls_label_map_remove
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove MPLS label that was mapped to a RIF-Tunnel
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_MPLS_LABEL_RIF_KEY                  *mpls_key -
 *     The MPLS label, and optionally the VSID, to be
 *     terminated and mapped to RIF
 *   SOC_SAND_INOUT ARAD_PP_LIF_ID                              *lif_index -
 *     LIF table index. Input when Lookup NONE flag is set.
 * REMARKS:
 *   - Unbind the mapping of the MPLS label to the LIF table
 *   from the SEM table- Invalidate the 'lif_index' entry in
 *   the LIF table- The 'lif_index' is returned to the user
 *   to enable management of the LIF table
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_rif_mpls_label_map_remove(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_MPLS_LABEL_RIF_KEY                  *mpls_key,
    SOC_SAND_INOUT ARAD_PP_LIF_ID                              *lif_index
  );

/*********************************************************************
* NAME:
 *   arad_pp_rif_mpls_label_map_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get MPLS label termination and Router interface info
 *   according to the terminated MPLS label.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_MPLS_LABEL_RIF_KEY                  *mpls_key -
 *     The MPLS label, and optionally the VSID, to be
 *     terminated and mapped to RIF
 *   SOC_SAND_OUT ARAD_PP_LIF_ID                              *lif_index -
 *     LIF table index
 *   SOC_SAND_OUT ARAD_PP_MPLS_TERM_INFO                      *term_info -
 *     MPLS Termination info including type of termination
 *     (pipe/uniform), RIF-id>. The term_info.cos-profile is
 *     ignored. Instead the rif_info.cos-profile is used.
 *   SOC_SAND_OUT ARAD_PP_RIF_INFO                            *rif_info -
 *     RIF attributes. RIF ID, ...
 *   SOC_SAND_OUT uint8                                 *found -
 *     Was key found
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_rif_mpls_label_map_get(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  ARAD_PP_MPLS_LABEL_RIF_KEY *mpls_key,
    SOC_SAND_OUT ARAD_PP_LIF_ID             *lif_index,
    SOC_SAND_OUT ARAD_PP_MPLS_TERM_INFO     *term_info,
    SOC_SAND_OUT ARAD_PP_RIF_INFO           *rif_info,
    SOC_SAND_OUT uint8                  *found
  );

/*********************************************************************
* NAME:
 *   arad_pp_rif_ip_tunnel_term_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Enable IP Tunnels termination and setting the Router
 *   interface according to the terminated IP tunnel.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  dip_key -
 *     Destination IP
 *   SOC_SAND_IN  ARAD_PP_LIF_ID                              lif_index -
 *     Entry in the LIF table
 *   SOC_SAND_IN  ARAD_PP_RIF_IP_TERM_INFO                    *term_info -
 *     IP tunnel Termination info including type of termination
 *     RIF-id, cos-profile.
 *   SOC_SAND_IN  ARAD_PP_RIF_INFO                            *rif_info -
 *     RIF attributes. RIF ID, ...
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success -
 *     SOC_SAND_FAILURE_OUT_OF_RESOURCES: There is no space in the
 *     Exact Match table
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_rif_ip_tunnel_term_add(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  ARAD_PP_RIF_IP_TERM_KEY      *term_key,
    SOC_SAND_IN  ARAD_PP_LIF_ID             lif_index,
    SOC_SAND_IN  ARAD_PP_RIF_IP_TERM_INFO   *term_info,
    SOC_SAND_IN  ARAD_PP_RIF_INFO           *rif_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE       *success
  );

/*********************************************************************
* NAME:
 *   arad_pp_rif_ip_tunnel_term_remove
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove the IP Tunnel
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  dip_key -
 *     Destination IP
 *   SOC_SAND_OUT ARAD_PP_LIF_ID                              *lif_index -
 *     Entry in the LIF table
 * REMARKS:
 *   - Unbind the mapping of the IP Tunnel to the LIF table
 *   from the SEM table- Invalidate the 'lif_index' entry in
 *   the LIF table- The 'lif_index' is returned to the user
 *   to enable management of the LIF table
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_rif_ip_tunnel_term_remove(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  ARAD_PP_RIF_IP_TERM_KEY      *term_key,
    SOC_SAND_OUT ARAD_PP_LIF_ID             *lif_index
  );

/*********************************************************************
* NAME:
 *   arad_pp_rif_ip_tunnel_term_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get IP Tunnels termination and Router interface info
 *   according to the terminated IP tunnel.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  dip_key -
 *     Destination IP
 *   SOC_SAND_OUT ARAD_PP_LIF_ID                              *lif_index -
 *     Entry in the LIF table
 *   SOC_SAND_OUT ARAD_PP_RIF_IP_TERM_INFO                    *term_info -
 *     IP tunnel Termination info including type of termination
 *     RIF-id. The term_info.cos-profile is ignored. Instead
 *     the rif_info.cos-profile is used.
 *   SOC_SAND_OUT ARAD_PP_RIF_INFO                            *rif_info -
 *     RIF attributes. RIF ID, ...
 *   SOC_SAND_OUT uint8                                 *found -
 *     Was key found
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_rif_ip_tunnel_term_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_RIF_IP_TERM_KEY                *term_key,
    SOC_SAND_OUT ARAD_PP_LIF_ID                         *lif_index,
    SOC_SAND_OUT ARAD_PP_RIF_IP_TERM_INFO               *term_info,
    SOC_SAND_OUT ARAD_PP_RIF_INFO                       *rif_info,
    SOC_SAND_OUT uint8                                 *found
  );

/*********************************************************************
* NAME:
 *   arad_pp_rif_ip_tunnel_term_remove
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the Router Interface according to the VSID.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_VSI_ID                              vsid_ndx -
 *     VSID. Equal to the RIF-ID
 *   SOC_SAND_IN  ARAD_PP_RIF_INFO                            *rif_info -
 *     RIF attributes. in this case RIF_ID has to be equal to
 *     vsid_ndx
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_rif_vsid_map_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_VSI_ID                              vsid_ndx,
    SOC_SAND_IN  ARAD_PP_RIF_INFO                            *rif_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_rif_vsid_map_set" API.
 *     Refer to "arad_pp_rif_vsid_map_set" API for details.
*********************************************************************/
uint32
  arad_pp_rif_vsid_map_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_VSI_ID                              vsid_ndx,
    SOC_SAND_OUT ARAD_PP_RIF_INFO                            *rif_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_rif_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the Router Interface according to the VSID.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_RIF_ID                              rif_ndx -
 *     Router Interface ID. Range: 0 - 4K-1.
 *   SOC_SAND_IN  ARAD_PP_RIF_INFO                            *rif_info -
 *     RIF attributes. CoS-Profile relvant only if the RIF-ID
 *     used as VSI-RIF.
 * REMARKS:
 *   - use this API to set attributes of RIF obtained upon
 *   MPLS tunnel termination by range, or reserved values.-
 *   can be used also to update RIF attributes of RIF set
 *   according to IP/MPLS tunnel termination or VSI.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_rif_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_RIF_ID                              rif_ndx,
    SOC_SAND_IN  ARAD_PP_RIF_INFO                            *rif_info
  );

/*********************************************************************
*     Gets the configuration set by the "arad_pp_rif_info_set"
 *     API.
 *     Refer to "arad_pp_rif_info_set" API for details.
*********************************************************************/
uint32
  arad_pp_rif_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_RIF_ID                              rif_ndx,
    SOC_SAND_OUT ARAD_PP_RIF_INFO                            *rif_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_rif_ttl_scope_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set TTL value for TTL-scope.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                 ttl_scope_ndx -
 *     TTL scope index. Range: 0-7. set according to RIF. See
 *     SOC_PPD_RIF_INFO.
 *   SOC_SAND_IN  SOC_SAND_PP_IP_TTL                            ttl_val -
 *     TTL value. Range: 0 -255.
 * REMARKS:
 *   - When packet is routed (IP/MPLS routing) to this RIF
 *   then packet's TTL is compared againsy the TTL if it less
 *   or equal then packet is filter. Range: 0 - 7. use
 *   soc_ppd_trap_eg_profile_info_set(SOC_PPD_TRAP_EG_TYPE_TTL_SCOPE,
 *   eg_trap_info) to set how to handle packets match
 *   condition of this filter.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_rif_ttl_scope_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 ttl_scope_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IP_TTL                            ttl_val
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_rif_ttl_scope_set" API.
 *     Refer to "arad_pp_rif_ttl_scope_set" API for details.
*********************************************************************/
uint32
  arad_pp_rif_ttl_scope_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 ttl_scope_ndx,
    SOC_SAND_OUT SOC_SAND_PP_IP_TTL                            *ttl_val
  );

/*********************************************************************
 * NAME:
 *   arad_pp_rif_native_routing_vlan_tags_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *        set native vsi compensation per rif
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_RIF_ID                               intf_id -
 *     RIF id. 
 *   SOC_SAND_IN  uint8                          native_routing_vlan_tags -
 *     Number of tags in native ethernet. Range: 0 - 2.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
arad_pp_rif_native_routing_vlan_tags_set(
   SOC_SAND_IN  int                                    unit, 
   SOC_SAND_IN  ARAD_PP_RIF_ID                         intf_id, 
   SOC_SAND_IN  uint8                                  native_routing_vlan_tags
   );


/*********************************************************************
*     get native vsi compensation per rif
 *     "arad_pp_rif_native_routing_vlan_tags_get" API.
 *     Refer to "arad_pp_rif_native_routing_vlan_tags_set" API for details.
*********************************************************************/
uint32
arad_pp_rif_native_routing_vlan_tags_get(
   SOC_SAND_IN  int                                    unit,
   SOC_SAND_IN  ARAD_PP_RIF_ID                         intf_id,
   SOC_SAND_OUT  uint8                                 *native_routing_vlan_tags
   );

void
  ARAD_PP_RIF_MPLS_LABELS_RANGE_clear(
    SOC_SAND_OUT ARAD_PP_RIF_MPLS_LABELS_RANGE *info
  );


void
  ARAD_PP_MPLS_LABEL_RIF_KEY_clear(
    SOC_SAND_OUT ARAD_PP_MPLS_LABEL_RIF_KEY *info
  );


void
  ARAD_PP_RIF_INFO_clear(
    SOC_SAND_OUT ARAD_PP_RIF_INFO *info
  );

void
  ARAD_PP_RIF_IP_TERM_INFO_clear(
    SOC_SAND_OUT ARAD_PP_RIF_IP_TERM_INFO *info
  );

void
  ARAD_PP_RIF_IP_TERM_KEY_clear(
    SOC_SAND_OUT ARAD_PP_RIF_IP_TERM_KEY *key
  );

#endif /*__ARAD_PP_API_RIF_INCLUDED__*/
