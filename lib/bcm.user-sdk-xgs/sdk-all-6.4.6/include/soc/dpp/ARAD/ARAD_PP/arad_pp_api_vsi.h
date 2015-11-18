/* $Id$
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

#ifndef __ARAD_PP_API_VSI_INCLUDED__
/* { */
#define __ARAD_PP_API_VSI_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_vsi.h>
#include <soc/dpp/PPD/ppd_api_vsi.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define ARAD_PP_VSI_FID_IS_VSID                              (SOC_PPC_VSI_FID_IS_VSID)

/* } */
/*************
 * MACROS    *
 *************/
/* { */

#define SOC_PPD_VSI_L2CP_KEY_DA_MAC_ADDRESS_LSB_LSB           (0)
#define SOC_PPD_VSI_L2CP_KEY_DA_MAC_ADDRESS_LSB_MSB           (5)
#define SOC_PPD_VSI_L2CP_KEY_DA_MAC_ADDRESS_LSB_SHIFT         (SOC_PPD_VSI_L2CP_KEY_DA_MAC_ADDRESS_LSB_LSB)
#define SOC_PPD_VSI_L2CP_KEY_DA_MAC_ADDRESS_LSB_MASK          (SOC_SAND_BITS_MASK(SOC_PPD_VSI_L2CP_KEY_DA_MAC_ADDRESS_LSB_MSB, SOC_PPD_VSI_L2CP_KEY_DA_MAC_ADDRESS_LSB_LSB))

#define SOC_PPD_VSI_L2CP_KEY_L2CP_PROFILE_LSB                 (6)
#define SOC_PPD_VSI_L2CP_KEY_L2CP_PROFILE_MSB                 (6)
#define SOC_PPD_VSI_L2CP_KEY_L2CP_PROFILE_SHIFT               (SOC_PPD_VSI_L2CP_KEY_L2CP_PROFILE_LSB)
#define SOC_PPD_VSI_L2CP_KEY_L2CP_PROFILE_MASK                (SOC_SAND_BITS_MASK(SOC_PPD_VSI_L2CP_KEY_L2CP_PROFILE_MSB, SOC_PPD_VSI_L2CP_KEY_L2CP_PROFILE_LSB))


#define SOC_PPD_VSI_L2CP_KEY_ENTRY_OFFSET(l2cp_profile, da_mac_address_lsb)  \
          SOC_SAND_SET_FLD_IN_PLACE(da_mac_address_lsb, SOC_PPD_VSI_L2CP_KEY_DA_MAC_ADDRESS_LSB_SHIFT, SOC_PPD_VSI_L2CP_KEY_DA_MAC_ADDRESS_LSB_MASK) | \
          SOC_SAND_SET_FLD_IN_PLACE(l2cp_profile, SOC_PPD_VSI_L2CP_KEY_L2CP_PROFILE_SHIFT, SOC_PPD_VSI_L2CP_KEY_L2CP_PROFILE_MASK)

/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */

typedef SOC_PPC_VSI_INFO                                       ARAD_PP_VSI_INFO;
typedef SOC_PPC_VSI_DEFAULT_FRWRD_KEY                          ARAD_PP_VSI_DEFAULT_FRWRD_KEY;
/*********************************************************************
* NAME:
 *   arad_pp_vsi_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the Virtual Switch Instance information. After
 *   setting the VSI, the user may attach L2 Logical
 *   Interfaces to it: ACs; PWEs
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_VSI_ID                              vsi_ndx -
 *     System VSID. Range: 0-64K
 *   SOC_SAND_IN  ARAD_PP_VSI_INFO                            *vsi_info -
 *     VSI attributes
 * REMARKS:
 *   - Default forwarding destination:T20E: The destination
 *   is fully configurableArad-B: The destination must
 *   correspond to one of the action pointers configured by
 *   soc_ppd_frwrd_mact_vsi_default_info_set()- Arad-B Flooding:
 *   When the flooding multicast ID mapping from the local
 *   VSI is insufficient, the user may either set the
 *   destination as FEC ID with multicast destination, or
 *   utilize the ingress multicast table to remap the MID.-
 *   Arad-B: When the local VSI is > 4K: The EEI cannot be
 *   used as I-SID; Topology ID. Enable-My-MAC and Enable
 *   routing have to be negated. The FID is the VSID.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_vsi_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_VSI_ID                              vsi_ndx,
    SOC_SAND_IN  ARAD_PP_VSI_INFO                            *vsi_info
  );

/*********************************************************************
*     Gets the configuration set by the "arad_pp_vsi_info_set"
 *     API.
 *     Refer to "arad_pp_vsi_info_set" API for details.
*********************************************************************/
uint32
  arad_pp_vsi_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_VSI_ID                              vsi_ndx,
    SOC_SAND_OUT ARAD_PP_VSI_INFO                            *vsi_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_vsi_egress_mtu_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set mtu to vsi.
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                               vsi_profile_ndx -
 *     vsi profile index. Range: 1-3
 *   SOC_SAND_IN  SOC_SAND_IN  SOC_PPD_MTU               mtu_val -
 *     mtu value to set into the profile.
 * REMARKS:
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_vsi_egress_mtu_set(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint8                                is_forwarding_mtu_filter,
    SOC_SAND_IN  uint32                               vsi_profile_ndx,
    SOC_SAND_IN  uint32                               mtu_val
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_vsi_egress_profile_set" API.
 *     Refer to "arad_pp_vsi_egress_profile_set" API for details.
*********************************************************************/
uint32
  arad_pp_vsi_egress_mtu_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint8                                is_forwarding_mtu_filter,
    SOC_SAND_IN  uint32                               vsi_profile_ndx,
    SOC_SAND_OUT  uint32                              *mtu_val
  );

void
  ARAD_PP_VSI_INFO_clear(
    SOC_SAND_OUT ARAD_PP_VSI_INFO *info
  );

/*********************************************************************
 * NAME:
 *   arad_pp_vsi_l2cp_trap_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *     Sets Trap information for Layer 2 control protocol
 *     frames. Packet is an MEF layer 2 control protocol
 *     service frame When DA matches 01-80-c2-00-00-XX where XX
 * = 8'b00xx_xxxx.
 *     Details: in the H file. (search for prototype)
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPD_VSI_L2CP_KEY                 *l2cp_key -
 *     profile and da key.
 *   SOC_SAND_IN  SOC_PPD_VSI_L2CP_HANDLE_TYPE         handle_type -
 *     type (trap/drop/normal/peer).
 * REMARKS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_vsi_l2cp_trap_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPD_VSI_L2CP_KEY                       *l2cp_key,
    SOC_SAND_IN  SOC_PPD_VSI_L2CP_HANDLE_TYPE               handle_type
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_vsi_l2cp_trap_set" API.
 *     Refer to "arad_pp_vsi_l2cp_trap_set" API for details.
*********************************************************************/
uint32
  arad_pp_vsi_l2cp_trap_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPD_VSI_L2CP_KEY                       *l2cp_key,
    SOC_SAND_OUT SOC_PPD_VSI_L2CP_HANDLE_TYPE               *handle_type
  );


#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_PP_API_VSI_INCLUDED__*/
#endif
