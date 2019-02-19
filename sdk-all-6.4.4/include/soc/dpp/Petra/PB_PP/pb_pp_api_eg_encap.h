/* $Id: pb_pp_api_eg_encap.h,v 1.8 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/include/soc_pb_pp_api_eg_encap.h
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

#ifndef __SOC_PB_PP_API_EG_ENCAP_INCLUDED__
/* { */
#define __SOC_PB_PP_API_EG_ENCAP_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_eg_encap.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_api_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     Maximum number of tunnels per encapsulation             */
#define  SOC_PB_PP_EG_ENCAP_MPLS_MAX_NOF_TUNNELS (SOC_PPC_EG_ENCAP_MPLS_MAX_NOF_TUNNELS)

/*      */
#define  SOC_PB_PP_EG_ENCAP_UPPER_BANK_MIN_ENTRY                    (4*1024)

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

#define SOC_PB_PP_EG_ENCAP_EEP_TYPE_LIF_EEP                    SOC_PPC_EG_ENCAP_EEP_TYPE_LIF_EEP
#define SOC_PB_PP_EG_ENCAP_EEP_TYPE_TUNNEL_EEP                 SOC_PPC_EG_ENCAP_EEP_TYPE_TUNNEL_EEP
#define SOC_PB_PP_EG_ENCAP_EEP_TYPE_LL                         SOC_PPC_EG_ENCAP_EEP_TYPE_LL
#define SOC_PB_PP_NOF_EG_ENCAP_EEP_TYPES                       SOC_PPC_NOF_EG_ENCAP_EEP_TYPES_PB
typedef SOC_PPC_EG_ENCAP_EEP_TYPE                              SOC_PB_PP_EG_ENCAP_EEP_TYPE;

#define SOC_PB_PP_EG_ENCAP_EXP_MARK_MODE_FROM_PUSH_PROFILE     SOC_PPC_EG_ENCAP_EXP_MARK_MODE_FROM_PUSH_PROFILE
#define SOC_PB_PP_NOF_EG_ENCAP_EXP_MARK_MODES                  SOC_PPC_NOF_EG_ENCAP_EXP_MARK_MODES
typedef SOC_PPC_EG_ENCAP_EXP_MARK_MODE                         SOC_PB_PP_EG_ENCAP_EXP_MARK_MODE;

#define SOC_PB_PP_EG_ENCAP_ENTRY_TYPE_SWAP_CMND                SOC_PPC_EG_ENCAP_ENTRY_TYPE_SWAP_CMND
#define SOC_PB_PP_EG_ENCAP_ENTRY_TYPE_PWE                      SOC_PPC_EG_ENCAP_ENTRY_TYPE_PWE
#define SOC_PB_PP_EG_ENCAP_ENTRY_TYPE_POP_CMND                 SOC_PPC_EG_ENCAP_ENTRY_TYPE_POP_CMND
#define SOC_PB_PP_EG_ENCAP_ENTRY_TYPE_MPLS_ENCAP               SOC_PPC_EG_ENCAP_ENTRY_TYPE_MPLS_ENCAP
#define SOC_PB_PP_EG_ENCAP_ENTRY_TYPE_IPV4_ENCAP               SOC_PPC_EG_ENCAP_ENTRY_TYPE_IPV4_ENCAP
#define SOC_PB_PP_EG_ENCAP_ENTRY_TYPE_LL_ENCAP                 SOC_PPC_EG_ENCAP_ENTRY_TYPE_LL_ENCAP
#define SOC_PB_PP_NOF_EG_ENCAP_ENTRY_TYPES                     SOC_PPC_NOF_EG_ENCAP_ENTRY_TYPES_PB
typedef SOC_PPC_EG_ENCAP_ENTRY_TYPE                            SOC_PB_PP_EG_ENCAP_ENTRY_TYPE;

typedef SOC_PPC_EG_ENCAP_VSI_ENCAP_INFO                        SOC_PB_PP_EG_ENCAP_VSI_ENCAP_INFO;
typedef SOC_PPC_EG_ENCAP_RANGE_INFO                            SOC_PB_PP_EG_ENCAP_RANGE_INFO;
typedef SOC_PPC_EG_ENCAP_SWAP_INFO                             SOC_PB_PP_EG_ENCAP_SWAP_INFO;
typedef SOC_PPC_EG_ENCAP_PWE_INFO                              SOC_PB_PP_EG_ENCAP_PWE_INFO;
typedef SOC_PPC_EG_ENCAP_POP_INTO_ETH_INFO                     SOC_PB_PP_EG_ENCAP_POP_INTO_ETH_INFO;
typedef SOC_PPC_EG_ENCAP_POP_INFO                              SOC_PB_PP_EG_ENCAP_POP_INFO;
typedef SOC_PPC_EG_ENCAP_MPLS_TUNNEL_INFO                      SOC_PB_PP_EG_ENCAP_MPLS_TUNNEL_INFO;
typedef SOC_PPC_EG_ENCAP_MPLS_ENCAP_INFO                       SOC_PB_PP_EG_ENCAP_MPLS_ENCAP_INFO;
typedef SOC_PPC_EG_ENCAP_IPV4_TUNNEL_INFO                      SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_INFO;
typedef SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO                       SOC_PB_PP_EG_ENCAP_IPV4_ENCAP_INFO;
typedef SOC_PPC_EG_ENCAP_LL_INFO                               SOC_PB_PP_EG_ENCAP_LL_INFO;
typedef SOC_PPC_EG_ENCAP_PUSH_PROFILE_INFO                     SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO;
typedef SOC_PPC_EG_ENCAP_PUSH_EXP_KEY                          SOC_PB_PP_EG_ENCAP_PUSH_EXP_KEY;
typedef SOC_PPC_EG_ENCAP_PWE_GLBL_INFO                         SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO;
typedef SOC_PPC_EG_ENCAP_ENTRY_VALUE                           SOC_PB_PP_EG_ENCAP_ENTRY_VALUE;
typedef SOC_PPC_EG_ENCAP_ENTRY_INFO                            SOC_PB_PP_EG_ENCAP_ENTRY_INFO;

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
 *   soc_pb_pp_eg_encap_range_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets devision of the Egress Encapsulation Table between
 *   the different usages (Link layer/ IP tunnels/ MPLS
 *   tunnels).
 * INPUT:
 *   SOC_SAND_IN  int                                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_RANGE_INFO                     *range_info -
 *     Range information.
 * REMARKS:
 *   - T20E: not supported. Error will be returned if called.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_range_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_RANGE_INFO                     *range_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_eg_encap_range_info_set" API.
 *     Refer to "soc_pb_pp_eg_encap_range_info_set" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_range_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_RANGE_INFO                     *range_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_encap_null_lif_entry_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set LIF Editing entry to be NULL Entry.
 * INPUT:
 *   SOC_SAND_IN  int                                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                      lif_eep_ndx -
 *     Egress Encapsulation table entry, of type Out-LIF (First
 *     encapsulation)
 *   SOC_SAND_IN  uint32                                      next_eep -
 *     Pointer to the next egress encapsulation table entry
 * REMARKS:
 *   - T20E Only API- Needed when the packet should only be
 *   encapsulated with Tunnel information and the EEI is NULL
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_null_lif_entry_add(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      lif_eep_ndx,
    SOC_SAND_IN  uint32                                      next_eep
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_encap_ac_entry_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set LIF Editing entry to hold AC ID. Actually maps from
 *   CUD to AC.
 * INPUT:
 *   SOC_SAND_IN  int                                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                      lif_eep_ndx -
 *     Egress Encapsulation table entry, of type Out-LIF (First
 *     encapsulation)
 *   SOC_SAND_IN  SOC_PB_PP_AC_ID                               ac_id -
 *     The Out-AC ID, the packet is associated with
 * REMARKS:
 *   - In Soc_petra-B: mapping has to maintain the encoding of
 *   the AC in the CUD.- Use this API to MAP CUD (from the TM
 *   header) to AC.- Mapping - Use LIF Editing Table.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_ac_entry_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  SOC_PB_PP_AC_ID                               ac_id
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_encap_swap_command_entry_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set LIF Editing entry to hold MPLS LSR SWAP label.
 *   Needed for MPLS multicast services.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  lif_eep_ndx -
 *     Egress Encapsulation table entry, of type Out-LIF (First
 *     encapsulation)
 *   SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_SWAP_INFO                  *swap_info -
 *     Swap label for LSR applications (typically for
 *     Multicast).
 *   SOC_SAND_IN  uint32                                  next_eep -
 *     Pointer to Egress Encapsulation table entry of type
 *     tunnel (second encapsulation), or link-layer
 * REMARKS:
 *   - In T20E, next_eep always points to Tunnels Editing
 *   Table.- The usage of this API for Multicast is to give a
 *   unique encapsulation for each copy.- If the user needs
 *   SWAP for Unicast packets, then the swap can be done in
 *   the ingress using the EEI.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_swap_command_entry_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_SWAP_INFO                  *swap_info,
    SOC_SAND_IN  uint32                                  next_eep
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_encap_pwe_entry_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set LIF Editing entry to hold PWE info (VC label and
 *   push profile).
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  lif_eep_ndx -
 *     Egress Encapsulation table entry, of type Out-LIF (First
 *     encapsulation)
 *   SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PWE_INFO                   *pwe_info -
 *     PWE info VC label and push profile.
 *   SOC_SAND_IN  uint32                                  next_eep -
 *     Pointer to the next egress encapsulation table entry
 * REMARKS:
 *   - In T20E: next_eep always points to Tunnels Editing
 *   Table.- The usage of this API for Multicast is to give
 *   special treatment for each copy.- In some applications,
 *   such as path protection in VPWS, the VC label can be set
 *   explicitely in the EEI in the ingress PP device, in
 *   which case the 1st encapsulation table is skipped.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_pwe_entry_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PWE_INFO                   *pwe_info,
    SOC_SAND_IN  uint32                                  next_eep
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_encap_pop_command_entry_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set LIF Editing entry to hold MPLS LSR POP command.
 *   Needed for MPLS multicast services.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  lif_eep_ndx -
 *     Egress Encapsulation table entry, of type Out-LIF (First
 *     encapsulation)
 *   SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_POP_INFO                   *pop_info -
 *     POP information including type of POP, and information
 *     to process the inner Ethernet in case the POP is into
 *     Ethernet.
 *   SOC_SAND_IN  uint32                                  next_eep -
 *     Pointer to the next egress encapsulation table entry
 * REMARKS:
 *   - In T20E: next_eep always points to Tunnels Editing
 *   Table.- The usage of this API for Multicast is to give
 *   special treatment for each copy.- If the user needs to
 *   add a POP label for Unicast packet, then the POP command
 *   can be set in the EEI in the ingress.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_pop_command_entry_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_POP_INFO                   *pop_info,
    SOC_SAND_IN  uint32                                  next_eep
  );

/*********************************************************************
 * invalid for Soc_petra B.
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_vsi_entry_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN SOC_PB_PP_EG_ENCAP_VSI_ENCAP_INFO             *vsi_info, 
    SOC_SAND_IN uint8                                  next_eep_valid, 
    SOC_SAND_IN  uint32                                  next_eep
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_encap_mpls_entry_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add MPLS tunnels encapsulation entry to the Tunnels
 *   Editing Table.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  tunnel_eep_ndx -
 *     Egress Encapsulation table entry, of type Tunnel (Second
 *     encapsulation)
 *   SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_MPLS_ENCAP_INFO            *mpls_encap_info -
 *     MPLS encapsulation information.
 *   SOC_SAND_IN  uint32                                  ll_eep -
 *     Points to next encapsulation (LL).
 * REMARKS:
 *   - In order to set entry with no encapsulation set
 *   'nof_tunnels' at 'mpls_encap_info' to zero.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_mpls_entry_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  tunnel_eep_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_MPLS_ENCAP_INFO            *mpls_encap_info,
    SOC_SAND_IN  uint32                                  ll_eep
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_encap_ipv4_entry_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add IPv4 tunnels encapsulation entry to the Egress
 *   Encapsulation Tunnels Editing Table.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  tunnel_eep_ndx -
 *     Egress Encapsulation table entry, of type Tunnel (Second
 *     encapsulation)
 *   SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_IPV4_ENCAP_INFO            *ipv4_encap_info -
 *     IPv4 encapsulation information.
 *   SOC_SAND_IN  uint32                                  ll_eep -
 *     Points to next encapsulation (LL). IP tunnel must point
 *     to a link-layer encapsulation entry and a link layer.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_ipv4_entry_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  tunnel_eep_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_IPV4_ENCAP_INFO            *ipv4_encap_info,
    SOC_SAND_IN  uint32                                  ll_eep
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_encap_ll_entry_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add LL encapsulation entry.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  ll_eep_ndx -
 *     Points to LL table.
 *   SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_LL_INFO                    *ll_encap_info -
 *     LL encapsulation information, including DA-MAC.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_ll_entry_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  ll_eep_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_LL_INFO                    *ll_encap_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_encap_entry_remove
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove entry from the encapsulation Table.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_EEP_TYPE                   eep_type_ndx -
 *     Encapsulation Table to select (LIFs/Tunnels/LL).
 *   SOC_SAND_IN  uint32                                  eep_ndx -
 *     Points into Editing Table according to 'eep_type_ndx'.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_entry_remove(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_EEP_TYPE                   eep_type_ndx,
    SOC_SAND_IN  uint32                                  eep_ndx
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_encap_entry_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get entry information from the Egress encapsulation
 *   tables.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_EEP_TYPE                   eep_type_ndx -
 *     Encapsulation Table to select (LIFs/Tunnels/LL).
 *   SOC_SAND_IN  uint32                                  eep_ndx -
 *     Points into Editing Table according to 'eep_type_ndx'.
 *   SOC_SAND_IN  uint32                                  depth -
 *     Number of encapsulation entries to get, see example of
 *     use in remarks.
 *   SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_ENTRY_INFO                 encap_entry_info -
 *     SOC_PPD_NOF_EG_ENCAP_EEP_TYPES]- To include egress
 *     encapsulation entries.
 *   SOC_SAND_OUT uint32                                  next_eep -
 *     SOC_PPD_NOF_EG_ENCAP_EEP_TYPES]- To include next Egress
 *     encapsulation pointer.
 *   SOC_SAND_OUT uint32                                  *nof_entries -
 *     Number of valid entries in next_eep and
 *     encap_entry_info.
 * REMARKS:
 *   - To get all egress encapsulations performed for a given
 *   out-lif, set eep_type_ndx =
 *   SOC_PPD_EG_ENCAP_EEP_TYPE_LIF_EEP, eep_ndx = out-lif, depth
 *   = SOC_PPD_NOF_EG_ENCAP_EEP_TYPES.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_entry_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_EEP_TYPE                   eep_type_ndx,
    SOC_SAND_IN  uint32                                  eep_ndx,
    SOC_SAND_IN  uint32                                  depth,
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_ENTRY_INFO                 encap_entry_info[SOC_PB_PP_NOF_EG_ENCAP_EEP_TYPES],
    SOC_SAND_OUT uint32                                  next_eep[SOC_PB_PP_NOF_EG_ENCAP_EEP_TYPES],
    SOC_SAND_OUT uint32                                  *nof_entries
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_encap_push_profile_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Setting the push profile info - specifying how to build
 *   the label header.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  profile_ndx -
 *     Profile ID. Range: 0 - 7.
 *   SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO          *profile_info -
 *     Push profile information including TTL, EXP, has-CW, and
 *     if it is PIPE mode.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_push_profile_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO          *profile_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_eg_encap_push_profile_info_set" API.
 *     Refer to "soc_pb_pp_eg_encap_push_profile_info_set" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_push_profile_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx,
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO          *profile_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_encap_push_exp_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the EXP value of the pushed label as mapping of the
 *   TC and DP.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PUSH_EXP_KEY               *exp_key -
 *     The key (set of parameters) that is used to set the EXP.
 *   SOC_SAND_IN  SOC_SAND_PP_MPLS_EXP                          exp -
 *     EXP to put in the pushed label.
 * REMARKS:
 *   - T20E only. Error is returned if called for Soc_petra-B.-
 *   Relevant when exp_mark_mode in push profile entry set to
 *   map EXP value from TC and DP
 *   (SOC_PPD_EG_ENCAP_EXP_MARK_MODE_FROM_PUSH_PROFILE)
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_push_exp_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PUSH_EXP_KEY               *exp_key,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_EXP                          exp
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_eg_encap_push_exp_info_set" API.
 *     Refer to "soc_pb_pp_eg_encap_push_exp_info_set" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_push_exp_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PUSH_EXP_KEY               *exp_key,
    SOC_SAND_OUT SOC_SAND_PP_MPLS_EXP                          *exp
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_encap_pwe_glbl_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set Global information for PWE Encapsulation.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO              *glbl_info -
 *     Global in
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_pwe_glbl_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO              *glbl_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_eg_encap_pwe_glbl_info_set" API.
 *     Refer to "soc_pb_pp_eg_encap_pwe_glbl_info_set" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_pwe_glbl_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO              *glbl_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_encap_ipv4_tunnel_glbl_src_ip_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set source IP address for IPv4 Tunneling.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  entry_ndx -
 *     Entry pointed by IPv4 Tunnel - see
 *     soc_ppd_eg_encap_ipv4_encap_entry_add(). Range: 0 - 15.
 *   SOC_SAND_IN  uint32                                  src_ip -
 *     Source IP address.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_ipv4_tunnel_glbl_src_ip_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  uint32                                  src_ip
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_eg_encap_ipv4_tunnel_glbl_src_ip_set" API.
 *     Refer to "soc_pb_pp_eg_encap_ipv4_tunnel_glbl_src_ip_set"
 *     API for details.
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_ipv4_tunnel_glbl_src_ip_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_OUT uint32                                  *src_ip
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_encap_ipv4_tunnel_glbl_ttl_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set TTL for IPv4 Tunneling.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  entry_ndx -
 *     Entry pointed by IPv4 Tunnel. See
 *     soc_ppd_eg_encap_ipv4_encap_entry_add(). Range: 0 - 3.
 *   SOC_SAND_IN  SOC_SAND_PP_IP_TTL                            ttl -
 *     TTL value to set in the IPV4 tunnel header. Range: 0 -
 *     63.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_ipv4_tunnel_glbl_ttl_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IP_TTL                            ttl
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_eg_encap_ipv4_tunnel_glbl_ttl_set" API.
 *     Refer to "soc_pb_pp_eg_encap_ipv4_tunnel_glbl_ttl_set" API
 *     for details.
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_ipv4_tunnel_glbl_ttl_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_OUT SOC_SAND_PP_IP_TTL                            *ttl
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_encap_ipv4_tunnel_glbl_tos_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set TOS for IPv4 Tunneling.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  entry_ndx -
 *     Entry pointed by IPv4 Tunnel. See
 *     soc_ppd_eg_encap_ipv4_encap_entry_add(). Range: 0 - 15.
 *   SOC_SAND_IN  SOC_SAND_PP_IPV4_TOS                          tos -
 *     TOS value to set in the IPV4 tunnel header. Range: 0 -
 *     255.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_ipv4_tunnel_glbl_tos_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_TOS                          tos
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_eg_encap_ipv4_tunnel_glbl_tos_set" API.
 *     Refer to "soc_pb_pp_eg_encap_ipv4_tunnel_glbl_tos_set" API
 *     for details.
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_ipv4_tunnel_glbl_tos_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_OUT SOC_SAND_PP_IPV4_TOS                          *tos
  );

void
  SOC_PB_PP_EG_ENCAP_RANGE_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_RANGE_INFO *info
  );

void
  SOC_PB_PP_EG_ENCAP_SWAP_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_SWAP_INFO *info
  );

void
  SOC_PB_PP_EG_ENCAP_PWE_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_PWE_INFO *info
  );

void
  SOC_PB_PP_EG_ENCAP_POP_INTO_ETH_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_POP_INTO_ETH_INFO *info
  );

void
  SOC_PB_PP_EG_ENCAP_POP_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_POP_INFO *info
  );

void
  SOC_PB_PP_EG_ENCAP_MPLS_TUNNEL_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_MPLS_TUNNEL_INFO *info
  );

void
  SOC_PB_PP_EG_ENCAP_MPLS_ENCAP_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_MPLS_ENCAP_INFO *info
  );

void
  SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_INFO *info
  );

void
  SOC_PB_PP_EG_ENCAP_IPV4_ENCAP_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_IPV4_ENCAP_INFO *info
  );

void
  SOC_PB_PP_EG_ENCAP_LL_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_LL_INFO *info
  );

void
  SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO *info
  );

void
  SOC_PB_PP_EG_ENCAP_PUSH_EXP_KEY_clear(
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_PUSH_EXP_KEY *info
  );

void
  SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO *info
  );

void
  SOC_PB_PP_EG_ENCAP_ENTRY_VALUE_clear(
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_ENTRY_VALUE *info
  );

void
  SOC_PB_PP_EG_ENCAP_ENTRY_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_ENTRY_INFO *info
  );

#if SOC_PB_PP_DEBUG_IS_LVL1

const char*
  SOC_PB_PP_EG_ENCAP_EEP_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_EEP_TYPE enum_val
  );

const char*
  SOC_PB_PP_EG_ENCAP_EXP_MARK_MODE_to_string(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_EXP_MARK_MODE enum_val
  );

const char*
  SOC_PB_PP_EG_ENCAP_ENTRY_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_ENTRY_TYPE enum_val
  );

void
  SOC_PB_PP_EG_ENCAP_RANGE_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_RANGE_INFO *info
  );

void
  SOC_PB_PP_EG_ENCAP_SWAP_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_SWAP_INFO *info
  );

void
  SOC_PB_PP_EG_ENCAP_PWE_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PWE_INFO *info
  );

void
  SOC_PB_PP_EG_ENCAP_POP_INTO_ETH_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_POP_INTO_ETH_INFO *info
  );

void
  SOC_PB_PP_EG_ENCAP_POP_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_POP_INFO *info
  );

void
  SOC_PB_PP_EG_ENCAP_MPLS_TUNNEL_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_MPLS_TUNNEL_INFO *info
  );

void
  SOC_PB_PP_EG_ENCAP_MPLS_ENCAP_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_MPLS_ENCAP_INFO *info
  );

void
  SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_INFO *info
  );

void
  SOC_PB_PP_EG_ENCAP_IPV4_ENCAP_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_IPV4_ENCAP_INFO *info
  );

void
  SOC_PB_PP_EG_ENCAP_LL_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_LL_INFO *info
  );

void
  SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO *info
  );

void
  SOC_PB_PP_EG_ENCAP_PUSH_EXP_KEY_print(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PUSH_EXP_KEY *info
  );

void
  SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO *info
  );

void
  SOC_PB_PP_EG_ENCAP_ENTRY_VALUE_print(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_ENTRY_VALUE *info
  );

void
  SOC_PB_PP_EG_ENCAP_ENTRY_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_ENTRY_INFO *info
  );

#endif /* SOC_PB_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PP_API_EG_ENCAP_INCLUDED__*/
#endif

