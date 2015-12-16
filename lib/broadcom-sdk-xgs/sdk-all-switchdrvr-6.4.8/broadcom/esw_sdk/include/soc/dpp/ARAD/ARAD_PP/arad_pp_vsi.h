/* $Id: arad_pp_vsi.h,v 1.9 Broadcom SDK $
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

#ifndef __ARAD_PP_VSI_INCLUDED__
/* { */
#define __ARAD_PP_VSI_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_vsi.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define ARAD_PP_STP_TOPOLOGY_MIN                    (0)

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
  ARAD_PP_VSI_MAP_ADD = ARAD_PP_PROC_DESC_BASE_VSI_FIRST,
  ARAD_PP_VSI_MAP_ADD_PRINT,
  ARAD_PP_VSI_MAP_ADD_UNSAFE,
  ARAD_PP_VSI_SYS_TO_LOCAL_MAP_GET_UNSAFE,
  ARAD_PP_VSI_MAP_ADD_VERIFY,
  ARAD_PP_VSI_MAP_REMOVE,
  ARAD_PP_VSI_MAP_REMOVE_PRINT,
  ARAD_PP_VSI_MAP_REMOVE_UNSAFE,
  ARAD_PP_VSI_MAP_REMOVE_VERIFY,
  ARAD_PP_VSI_DEFAULT_FRWRD_INFO_SET,
  ARAD_PP_VSI_DEFAULT_FRWRD_INFO_SET_PRINT,
  ARAD_PP_VSI_DEFAULT_FRWRD_INFO_SET_UNSAFE,
  ARAD_PP_VSI_DEFAULT_FRWRD_INFO_SET_VERIFY,
  ARAD_PP_VSI_DEFAULT_FRWRD_INFO_GET,
  ARAD_PP_VSI_DEFAULT_FRWRD_INFO_GET_PRINT,
  ARAD_PP_VSI_DEFAULT_FRWRD_INFO_GET_VERIFY,
  ARAD_PP_VSI_DEFAULT_FRWRD_INFO_GET_UNSAFE,
  ARAD_PP_VSI_INFO_SET,
  ARAD_PP_VSI_INFO_SET_PRINT,
  ARAD_PP_VSI_INFO_SET_UNSAFE,
  ARAD_PP_VSI_INFO_SET_VERIFY,
  ARAD_PP_VSI_INFO_GET,
  ARAD_PP_VSI_INFO_GET_PRINT,
  ARAD_PP_VSI_INFO_GET_VERIFY,
  ARAD_PP_VSI_INFO_GET_UNSAFE,
  ARAD_PP_VSI_GET_PROCS_PTR,
  ARAD_PP_VSI_GET_ERRS_PTR,
  ARAD_PP_VSI_EGRESS_PROFILE_SET,
  ARAD_PP_VSI_EGRESS_PROFILE_SET_UNSAFE,
  ARAD_PP_VSI_EGRESS_PROFILE_SET_VERIFY,
  ARAD_PP_VSI_EGRESS_PROFILE_GET,
  ARAD_PP_VSI_EGRESS_PROFILE_GET_UNSAFE,
  ARAD_PP_VSI_EGRESS_PROFILE_GET_VERIFY,
  /*
   * } Auto generated. Do not edit previous section.
   */

  /*
   * Last element. Do no touch.
   */
  ARAD_PP_VSI_PROCEDURE_DESC_LAST
} ARAD_PP_VSI_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  ARAD_PP_VSI_SUCCESS_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_VSI_FIRST,
  ARAD_PP_VSI_DEFAULT_FORWARD_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_PP_VSI_STP_TOPOLOGY_ID_OUT_OF_RANGE_ERR,
  ARAD_PP_VSI_FID_PROFILE_ID_OUT_OF_RANGE_ERR,
  ARAD_PP_VSI_MAC_LEARN_PROFILE_ID_OUT_OF_RANGE_ERR,
  ARAD_PP_VSI_ORIENTATION_OUT_OF_RANGE_ERR,
  ARAD_PP_VSI_DA_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_VSI_EGRESS_PROFILE_INDEX_OUT_OF_RANGE_ERR,
  /*
   * } Auto generated. Do not edit previous section.
   */

  ARAD_PP_VSI_PROFILE_ID_OUT_OF_RANGE_ERR,
  ARAD_PP_VSI_TRAP_CODE_OUT_OF_RANGE_ERR,
  ARAD_PP_VSI_OUT_OF_RANGE_ERR,
  ARAD_PP_VSI_MAP_NOT_SUPPORTED_ERR,

  /*
   * Last element. Do no touch.
   */
  ARAD_PP_VSI_ERR_LAST
} ARAD_PP_VSI_ERR;

typedef struct
{
  uint32 vdc_vsi_mapping_info; /* bits[0:23] - VNI/TNI, bits[24:31] - vlan domain */
}ARAD_PP_VDC_VSI_MAP; 

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
  arad_pp_vsi_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
* NAME:
 *   arad_pp_vsi_info_set_unsafe
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
  arad_pp_vsi_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_VSI_ID                              vsi_ndx,
    SOC_SAND_IN  ARAD_PP_VSI_INFO                            *vsi_info
  );

uint32
  arad_pp_vsi_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_VSI_ID                              vsi_ndx,
    SOC_SAND_IN  ARAD_PP_VSI_INFO                            *vsi_info
  );

uint32
  arad_pp_vsi_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_VSI_ID                              vsi_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_vsi_info_set_unsafe" API.
 *     Refer to "arad_pp_vsi_info_set_unsafe" API for details.
*********************************************************************/
uint32
  arad_pp_vsi_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_VSI_ID                              vsi_ndx,
    SOC_SAND_OUT ARAD_PP_VSI_INFO                            *vsi_info
  );

/*********************************************************************
 * NAME:
 *   arad_pp_vsi_egress_mtu_set_unsafe
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
  arad_pp_vsi_egress_mtu_set_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint8                                is_forwarding_mtu_filter,
    SOC_SAND_IN  uint32                               vsi_profile_ndx,
    SOC_SAND_IN  uint32                               mtu_val
  );

uint32
  arad_pp_vsi_egress_mtu_set_verify(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               vsi_profile_ndx,
    SOC_SAND_IN  uint32                               mtu_val
  );

uint32
  arad_pp_vsi_egress_mtu_get_verify(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint32                               vsi_profile_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
*     "arad_pp_vsi_egress_profile_set_unsafe" API.
*     Refer to "arad_pp_vsi_egress_profile_set_unsafe" API for
*     details.
*********************************************************************/
uint32
  arad_pp_vsi_egress_mtu_get_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint8                                is_forwarding_mtu_filter,
    SOC_SAND_IN  uint32                               vsi_profile_ndx,
    SOC_SAND_OUT  uint32                              *mtu_val
  );

/*********************************************************************
*     Gets the mtu profile that is associated with a given MTU
*     alue, according to MTU check (Forwarding Layer or Llink Layer).
*********************************************************************/
uint32
  arad_pp_vsi_egress_mtu_profile_get(
     SOC_SAND_IN  int       unit,
     SOC_SAND_IN  uint8     is_forwarding_mtu_filter,
     SOC_SAND_IN  uint32    mtu_value,
     SOC_SAND_OUT uint32    *mtu_profile
  );

/*********************************************************************
*     Enable or disable the Forwarding Layer or Link Layer mtu
*     filtering per header code.
*********************************************************************/
uint32
  arad_pp_vsi_egress_mtu_check_enable_set(
     SOC_SAND_IN int        unit,
     SOC_SAND_IN uint8      is_forwarding_mtu_filter,
     SOC_SAND_IN uint32     header_code,
     SOC_SAND_IN uint8      enable
  );

/*********************************************************************
*     Check if Forwarding Layer or Link Layer mtu filtering
*     is enabled or disabled according to header code.
*********************************************************************/
uint32
  arad_pp_vsi_egress_mtu_check_enable_get(
     SOC_SAND_IN  int        unit,
     SOC_SAND_IN  uint8      is_forwarding_mtu_filter,
     SOC_SAND_IN  uint32     header_code,
     SOC_SAND_OUT uint8      *enable
  );

/*********************************************************************
 * NAME:
 *   arad_pp_vsi_l2cp_trap_set_unsafe
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
  arad_pp_vsi_l2cp_trap_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPD_VSI_L2CP_KEY                       *l2cp_key,
    SOC_SAND_IN  SOC_PPD_VSI_L2CP_HANDLE_TYPE               handle_type
  );

/*********************************************************************
*     Gets the configuration set by the
*     "arad_pp_vsi_l2cp_trap_set_unsafe" API.
*     Refer to "arad_pp_vsi_l2cp_trap_set_unsafe" API for
*     details.
*********************************************************************/
uint32
  arad_pp_vsi_l2cp_trap_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPD_VSI_L2CP_KEY                       *l2cp_key,
    SOC_SAND_OUT SOC_PPD_VSI_L2CP_HANDLE_TYPE               *handle_type
  );

uint32
  arad_pp_vsi_l2cp_trap_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPD_VSI_L2CP_KEY                       *l2cp_key,
    SOC_SAND_IN  SOC_PPD_VSI_L2CP_HANDLE_TYPE               handle_type
  );

uint32
  arad_pp_vsi_l2cp_trap_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PPD_VSI_L2CP_KEY                       *l2cp_key
  );

uint32
  ARAD_PP_VSI_INFO_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  ARAD_PP_VSI_INFO *info,
    SOC_SAND_IN  ARAD_PP_VSI_ID    vsi_ndx
  );

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_PP_VSI_INCLUDED__*/
#endif
