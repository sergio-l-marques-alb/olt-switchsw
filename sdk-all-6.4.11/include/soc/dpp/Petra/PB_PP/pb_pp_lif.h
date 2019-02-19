/* $Id: pb_pp_lif.h,v 1.10 Broadcom SDK $
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

#ifndef __SOC_PB_PP_LIF_INCLUDED__
/* { */
#define __SOC_PB_PP_LIF_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_api_lif.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_LIF_PORT_PROFILE_NDX_MAX                         (7)
#define SOC_PB_PP_SW_DB_MULTI_SET_VLAN_COMPRESSION_RANGE_NOF_MEMBER (32)
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
  SOC_PB_PP_L2_LIF_PWE_MAP_RANGE_SET = SOC_PB_PP_PROC_DESC_BASE_LIF_FIRST,
  SOC_PB_PP_L2_LIF_PWE_MAP_RANGE_SET_PRINT,
  SOC_PB_PP_L2_LIF_PWE_MAP_RANGE_SET_UNSAFE,
  SOC_PB_PP_L2_LIF_PWE_MAP_RANGE_SET_VERIFY,
  SOC_PB_PP_L2_LIF_PWE_MAP_RANGE_GET,
  SOC_PB_PP_L2_LIF_PWE_MAP_RANGE_GET_PRINT,
  SOC_PB_PP_L2_LIF_PWE_MAP_RANGE_GET_VERIFY,
  SOC_PB_PP_L2_LIF_PWE_MAP_RANGE_GET_UNSAFE,
  SOC_PB_PP_L2_LIF_PWE_ADD,
  SOC_PB_PP_L2_LIF_PWE_ADD_PRINT,
  SOC_PB_PP_L2_LIF_PWE_ADD_UNSAFE,
  SOC_PB_PP_L2_LIF_PWE_ADD_INTERNAL_UNSAFE,
  SOC_PB_PP_L2_LIF_PWE_ADD_VERIFY,
  SOC_PB_PP_L2_LIF_PWE_GET,
  SOC_PB_PP_L2_LIF_PWE_GET_PRINT,
  SOC_PB_PP_L2_LIF_PWE_GET_UNSAFE,
  SOC_PB_PP_L2_LIF_PWE_GET_INTERNAL_UNSAFE,
  SOC_PB_PP_L2_LIF_PWE_GET_VERIFY,
  SOC_PB_PP_L2_LIF_PWE_REMOVE,
  SOC_PB_PP_L2_LIF_PWE_REMOVE_PRINT,
  SOC_PB_PP_L2_LIF_PWE_REMOVE_UNSAFE,
  SOC_PB_PP_L2_LIF_PWE_REMOVE_VERIFY,
  SOC_PB_PP_L2_LIF_VLAN_COMPRESSION_ADD,
  SOC_PB_PP_L2_LIF_VLAN_COMPRESSION_ADD_PRINT,
  SOC_PB_PP_L2_LIF_VLAN_COMPRESSION_ADD_UNSAFE,
  SOC_PB_PP_L2_LIF_VLAN_COMPRESSION_ADD_VERIFY,
  SOC_PB_PP_L2_LIF_VLAN_COMPRESSION_RANGE_VALID_SET_UNSAFE,
  SOC_PB_PP_L2_LIF_VLAN_COMPRESSION_RANGE_VALID_GET_UNSAFE,
  SOC_PB_PP_L2_LIF_VLAN_COMPRESSION_IS_VALID_RANGE,
  SOC_PB_PP_L2_LIF_VLAN_COMPRESSION_REMOVE,
  SOC_PB_PP_L2_LIF_VLAN_COMPRESSION_REMOVE_PRINT,
  SOC_PB_PP_L2_LIF_VLAN_COMPRESSION_REMOVE_UNSAFE,
  SOC_PB_PP_L2_LIF_VLAN_COMPRESSION_REMOVE_VERIFY,
  SOC_PB_PP_L2_LIF_VLAN_COMPRESSION_GET,
  SOC_PB_PP_L2_LIF_VLAN_COMPRESSION_GET_PRINT,
  SOC_PB_PP_L2_LIF_VLAN_COMPRESSION_GET_UNSAFE,
  SOC_PB_PP_L2_LIF_VLAN_COMPRESSION_GET_VERIFY,
  SOC_PB_PP_L2_LIF_AC_MAP_KEY_SET,
  SOC_PB_PP_L2_LIF_AC_MAP_KEY_SET_PRINT,
  SOC_PB_PP_L2_LIF_AC_MAP_KEY_SET_UNSAFE,
  SOC_PB_PP_L2_LIF_AC_MAP_KEY_SET_VERIFY,
  SOC_PB_PP_L2_LIF_AC_MAP_KEY_GET,
  SOC_PB_PP_L2_LIF_AC_MAP_KEY_GET_PRINT,
  SOC_PB_PP_L2_LIF_AC_MAP_KEY_GET_VERIFY,
  SOC_PB_PP_L2_LIF_AC_MAP_KEY_GET_UNSAFE,
  SOC_PB_PP_L2_LIF_AC_MP_INFO_SET,
  SOC_PB_PP_L2_LIF_AC_MP_INFO_SET_PRINT,
  SOC_PB_PP_L2_LIF_AC_MP_INFO_SET_UNSAFE,
  SOC_PB_PP_L2_LIF_AC_MP_INFO_SET_VERIFY,
  SOC_PB_PP_L2_LIF_AC_MP_INFO_GET,
  SOC_PB_PP_L2_LIF_AC_MP_INFO_GET_PRINT,
  SOC_PB_PP_L2_LIF_AC_MP_INFO_GET_VERIFY,
  SOC_PB_PP_L2_LIF_AC_MP_INFO_GET_UNSAFE,
  SOC_PB_PP_L2_LIF_AC_ADD,
  SOC_PB_PP_L2_LIF_AC_ADD_PRINT,
  SOC_PB_PP_L2_LIF_AC_ADD_UNSAFE,
  SOC_PB_PP_L2_LIF_AC_ADD_INTERNAL_UNSAFE,
  SOC_PB_PP_L2_LIF_AC_ADD_VERIFY,
  SOC_PB_PP_L2_LIF_AC_GET,
  SOC_PB_PP_L2_LIF_AC_GET_PRINT,
  SOC_PB_PP_L2_LIF_AC_GET_UNSAFE,
  SOC_PB_PP_L2_LIF_AC_GET_INTERNAL_UNSAFE,
  SOC_PB_PP_L2_LIF_AC_GET_VERIFY,
  SOC_PB_PP_L2_LIF_AC_WITH_COS_ADD,
  SOC_PB_PP_L2_LIF_AC_WITH_COS_ADD_PRINT,
  SOC_PB_PP_L2_LIF_AC_WITH_COS_ADD_UNSAFE,
  SOC_PB_PP_L2_LIF_AC_WITH_COS_ADD_VERIFY,
  SOC_PB_PP_L2_LIF_AC_WITH_COS_GET,
  SOC_PB_PP_L2_LIF_AC_WITH_COS_GET_PRINT,
  SOC_PB_PP_L2_LIF_AC_WITH_COS_GET_UNSAFE,
  SOC_PB_PP_L2_LIF_AC_WITH_COS_GET_VERIFY,
  SOC_PB_PP_L2_LIF_AC_REMOVE,
  SOC_PB_PP_L2_LIF_AC_REMOVE_PRINT,
  SOC_PB_PP_L2_LIF_AC_REMOVE_UNSAFE,
  SOC_PB_PP_L2_LIF_AC_REMOVE_VERIFY,
  SOC_PB_PP_L2_LIF_L2CP_TRAP_SET,
  SOC_PB_PP_L2_LIF_L2CP_TRAP_SET_PRINT,
  SOC_PB_PP_L2_LIF_L2CP_TRAP_SET_UNSAFE,
  SOC_PB_PP_L2_LIF_L2CP_TRAP_SET_VERIFY,
  SOC_PB_PP_L2_LIF_L2CP_TRAP_GET,
  SOC_PB_PP_L2_LIF_L2CP_TRAP_GET_PRINT,
  SOC_PB_PP_L2_LIF_L2CP_TRAP_GET_VERIFY,
  SOC_PB_PP_L2_LIF_L2CP_TRAP_GET_UNSAFE,
  SOC_PB_PP_L2_LIF_ISID_ADD,
  SOC_PB_PP_L2_LIF_ISID_ADD_PRINT,
  SOC_PB_PP_L2_LIF_ISID_ADD_UNSAFE,
  SOC_PB_PP_L2_LIF_ISID_ADD_INTERNAL_UNSAFE,
  SOC_PB_PP_L2_LIF_ISID_ADD_VERIFY,
  SOC_PB_PP_L2_LIF_ISID_GET,
  SOC_PB_PP_L2_LIF_ISID_GET_PRINT,
  SOC_PB_PP_L2_LIF_ISID_GET_UNSAFE,
  SOC_PB_PP_L2_LIF_ISID_GET_INTERNAL_UNSAFE,
  SOC_PB_PP_L2_LIF_ISID_GET_VERIFY,
  SOC_PB_PP_L2_LIF_ISID_REMOVE,
  SOC_PB_PP_L2_LIF_ISID_REMOVE_PRINT,
  SOC_PB_PP_L2_LIF_ISID_REMOVE_UNSAFE,
  SOC_PB_PP_L2_LIF_ISID_REMOVE_VERIFY,
  SOC_PB_PP_L2_LIF_VSI_TO_ISID,
  SOC_PB_PP_L2_LIF_VSI_TO_ISID_PRINT,
  SOC_PB_PP_L2_LIF_VSI_TO_ISID_UNSAFE,
  SOC_PB_PP_L2_LIF_VSI_TO_ISID_VERIFY,
  SOC_PB_PP_LIF_GET_PROCS_PTR,
  SOC_PB_PP_LIF_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */

  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_LIF_PROCEDURE_DESC_LAST
} SOC_PB_PP_LIF_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PB_PP_LIF_SUCCESS_OUT_OF_RANGE_ERR = SOC_PB_PP_ERR_DESC_BASE_LIF_FIRST,
  SOC_PB_PP_LIF_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LIF_VLAN_FORMAT_NDX_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LIF_KEY_MAPPING_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LIF_HANDLE_TYPE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LIF_DEFAULT_FRWD_TYPE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LIF_ORIENTATION_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LIF_TPID_PROFILE_INDEX_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LIF_COS_PROFILE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LIF_SERVICE_TYPE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LIF_VLAN_DOMAIN_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LIF_VID_TAG_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LIF_VLAN_RANGE_FIRST_OVER_LAST_ERR,
  SOC_PB_PP_LIF_VLAN_RANGE_VLAN_RANGE_ALREADY_DEFINED_ERR,
  SOC_PB_PP_LIF_KEY_TYPE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LIF_RAW_KEY_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LIF_LEARN_TYPE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LIF_ING_VLAN_EDIT_PROFILE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LIF_EDIT_PCP_PROFILE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LIF_L2CP_PROFILE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LIF_DA_MAC_ADDRESS_LSB_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LIF_OPCODE_ID_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LIF_NOF_LIF_ENTRIES_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LIF_ISID_DOMAIN_OUT_OF_RANGE_ERR,
  /*
   * } Auto generated. Do not edit previous section.
   */

  SOC_PB_PP_LIF_VSI_IS_NOT_P2P_ERR,
  SOC_PB_PP_LIF_ILLEGAL_KEY_MAPPING_ERR,
  SOC_PB_PP_LIF_INNER_EXISTS_OUTER_NOT_ERR,
  SOC_PB_PP_LIF_PWE_COS_PROFILE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LIF_AC_P2P_DEFAULT_FRWRD_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LIF_AC_MP_DEFAULT_FRWRD_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LIF_PWE_P2P_DEFAULT_FRWRD_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LIF_PWE_MP_DEFAULT_FRWRD_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LIF_AC_P2P_ORIENTATION_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LIF_PWE_P2P_ORIENTATION_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LIF_PWE_INFO_INVALID_FIELDS_ERR,

  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_LIF_ERR_LAST
} SOC_PB_PP_LIF_ERR;

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
  soc_pb_pp_lif_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_l2_lif_pwe_map_range_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set MPLS labels that may be used as in-vc-labels
 * INPUT:
 *   SOC_SAND_IN  int                                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_L2_LIF_IN_VC_RANGE                      *in_vc_range -
 *     First and Last MPLS Labels to be used as ingress VC
 *     labels. T20E Range Size: 0-896K
 * REMARKS:
 *   - T20E: Set the MPLS labels that may be used as
 *   in-vc-labels - Soc_petra-B: Ignored
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_pwe_map_range_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_IN_VC_RANGE                      *in_vc_range
  );

uint32
  soc_pb_pp_l2_lif_pwe_map_range_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_IN_VC_RANGE                      *in_vc_range
  );

uint32
  soc_pb_pp_l2_lif_pwe_map_range_get_verify(
    SOC_SAND_IN  int                                     unit
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_l2_lif_pwe_map_range_set_unsafe" API.
 *     Refer to "soc_pb_pp_l2_lif_pwe_map_range_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_pwe_map_range_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_IN_VC_RANGE                      *in_vc_range
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_l2_lif_pwe_add_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Enable an in VC label as a PWE handle, and set the PWE
 *   attributes
 * INPUT:
 *   SOC_SAND_IN  int                                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        in_vc_label -
 *     The In-MPLS label that is attached to the PWE
 *   SOC_SAND_IN  SOC_PB_PP_LIF_ID                              lif_index -
 *     LIF table index
 *   SOC_SAND_IN  SOC_PB_PP_L2_LIF_PWE_INFO                     *pwe_info -
 *     PWE attributes. VSID, Out-VC, ...
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success -
 *     SOC_SAND_FAILURE_OUT_OF_RESOURCES: There is no space in the
 *     Exact Match table
 * REMARKS:
 *   T20E: The lif_index is ignored. Must be set to
 *   T20E_LIF_DEFAULT_PWE_LIF. Soc_petra-B: In multipoint services
 *   (pwe_info.service_type=MP), the default forwarding is
 *   set per VSI and NOT per PWE interface using this API.
 *   When Adding a PWE to the system, the user is expected
 *   to:- Find the Egress Editing pointer (EEP) that points
 *   to the PWE's tunnel and Link-Layer encapsulation.- Add a
 *   FEC, with the PWEs TM destination and the EEP that
 *   points to the tunnel. This FEC is used for learning.-
 *   For multipoint services, add the PWE to the multicast
 *   group that commits the flooding of the unknown MAC
 *   addresses in the VSI.- Call this function, when the
 *   above FEC is part of the learning information.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_pwe_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        in_vc_label,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_PWE_ADDITIONAL_INFO          *pwe_additional_info,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_PWE_INFO                     *pwe_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  soc_pb_pp_l2_lif_pwe_add_internal_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        in_vc_label,
    SOC_SAND_IN  uint8                                 ignore_key,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_PWE_ADDITIONAL_INFO          *pwe_additional_info,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_PWE_INFO                     *pwe_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  soc_pb_pp_l2_lif_pwe_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        in_vc_label,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_PWE_INFO                     *pwe_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_l2_lif_pwe_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get PWE attributes and the LIF-index according to
 *   in_vc_label.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        in_vc_label -
 *     The In-MPLS label that is attached to the PWE
 *   SOC_SAND_OUT SOC_PB_PP_LIF_ID                              lif_index -
 *     The index of the PWE in the LIF table
 *   SOC_SAND_OUT SOC_PB_PP_L2_LIF_PWE_INFO                     *pwe_info -
 *     PWE incoming attributes. VSID, Out-VC, ...
 *   SOC_SAND_OUT uint8                                 *found -
 *     TRUE: The entry was found, 'pwe_info' is validFALSE: The
 *     entry was not found, 'pwe_info' is invalid
 * REMARKS:
 *   T20E: The lif_index is ignored. Soc_petra-B: In multipoint
 *   services, the default forwarding is according to the
 *   VSI. When Adding a PWE to the system, the user is
 *   expected to:- Find the Egress Editing pointer (EEP) that
 *   points to the PWE's tunnel and Link-Layer
 *   encapsulation.- Add a FEC, with the PWEs TM destination
 *   and the EEP that points to the tunnel. This FEC is used
 *   for learning.- For multipoint services, add the PWE to
 *   the multicast group that commits the flooding of the
 *   unknown MAC addresses in the VSI.- Call this function,
 *   when the above FEC is part of the learning
 *   information. In the T20E driver, the AC set API
 *   configures the VSI attributes as well.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_pwe_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        in_vc_label,
    SOC_SAND_OUT SOC_PB_PP_LIF_ID                              *lif_index,
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_PWE_ADDITIONAL_INFO          *pwe_additional_info,
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_PWE_INFO                     *pwe_info,
    SOC_SAND_OUT uint8                                 *found
  );


/* $Id: pb_pp_lif.h,v 1.10 Broadcom SDK $
 * get PWE according to key or lif-id
 */
uint32
  soc_pb_pp_l2_lif_pwe_get_internal_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                            in_vc_label,
    SOC_SAND_IN  uint8                                     ignore_key,
    SOC_SAND_OUT SOC_PB_PP_LIF_ID                                  *lif_index,
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_PWE_ADDITIONAL_INFO              *pwe_additional_info,
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_PWE_INFO                         *pwe_info,
    SOC_SAND_OUT uint8                                     *found
  );


uint32
  soc_pb_pp_l2_lif_pwe_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        in_vc_label
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_l2_lif_pwe_remove_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remvoe the in-VC-label
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        in_vc_label -
 *     The In-MPLS label that is attached to the PWE
 *   SOC_SAND_OUT uint32                                  *lif_index -
 *     LIF table index
 * REMARKS:
 *   - Unbind the mapping of the In-VC-Label to the LIF table
 *   from the SEM table- Invalidate the 'lif_index' entry in
 *   the LIF table- The 'lif_index' is returned to the user
 *   to enable management of the LIF table
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_pwe_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        in_vc_label,
    SOC_SAND_OUT uint32                                  *lif_index,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_PWE_ADDITIONAL_INFO      *pwe_additional_info
  );

uint32
  soc_pb_pp_l2_lif_pwe_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_LABEL                        in_vc_label
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_l2_lif_vlan_compression_add_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Enable VLAN compression when adding Attachment Circuits.
 *   Enables defining an AC according to VLAN domain and a
 *   range of VLANs, and not only according to port*VLAN
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_L2_VLAN_RANGE_KEY                   *vlan_range_key -
 *     VLAN Range Key. When AC is added with the first VLAN in
 *     the range, the AC is defined for all the VLAN in the
 *     range. When the user adds an AC with VID in the middle of
 *     the range, the driver returns ERROR.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success -
 *     SOC_SAND_FAILURE_OUT_OF_RESOURCES: There is no space in the
 *     VLAN ranges table
 * REMARKS:
 *   - Soc_petra-B Only. In the T20E, there are no VLAN ranges,
 *   and an AC is specified per port * VLAN [* C-VLAN]- In
 *   the device, there is a global ranges table, and per port
 *   there is an option to use or not use the range. The
 *   driver manages the global table internally; therefore,
 *   the driver actually does the following:Upon Enable: Add
 *   the VLAN range mapping, unless it already exists, and
 *   enable for the ports in the VLAN domain. Upon Disable:
 *   Remove the VLAN range mapping, unless utilized by other
 *   ports, and the ports mapping in the VLAN domain.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_vlan_compression_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_L2_VLAN_RANGE_KEY                   *vlan_range_key,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  soc_pb_pp_l2_lif_vlan_compression_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_L2_VLAN_RANGE_KEY                   *vlan_range_key
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_l2_lif_vlan_compression_remove_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove a VLAN range from the compressed VLAN ranges
 *   database
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_L2_VLAN_RANGE_KEY                   *vlan_range_key -
 *     VLAN Range Key.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_vlan_compression_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_L2_VLAN_RANGE_KEY                   *vlan_range_key
  );

uint32
  soc_pb_pp_l2_lif_vlan_compression_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_L2_VLAN_RANGE_KEY                   *vlan_range_key
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_l2_lif_vlan_compression_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get all compressed VLAN ranges of a port
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_PORT                         local_port_ndx -
 *     Identifier of the port to access.
 *   SOC_SAND_OUT  SOC_PB_PP_L2_PORT_VLAN_RANGE_INFO     *vlan_range_info -
 *     VLAN Range Info.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_vlan_compression_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                             local_port_ndx,
    SOC_SAND_OUT SOC_PB_PP_L2_PORT_VLAN_RANGE_INFO          *vlan_range_info
  );

uint32
  soc_pb_pp_l2_lif_vlan_compression_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                             local_port_ndx,
    SOC_SAND_OUT SOC_PB_PP_L2_PORT_VLAN_RANGE_INFO          *vlan_range_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_l2_lif_ac_map_key_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets the packets attributes (in-port, VIDs) to consider
 *   when associating an incoming packet to in-AC.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY_QUALIFIER             *qual_key -
 *     The qualifier used to associate the packet into
 *     AC-key Includes port-profile and output from packet parsing
 *   SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE              key_mapping -
 *     AC map key, can be any of the following:- port- port x
 *     Vlan (outer)- port x Vlan x Vlan
 * REMARKS:
 *   - use SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_ANY as
 *   vlan_format_ndx to make a decision for ALL VLAN formats
 *   at once.- In order to assign a port profile to a local
 *   port, use: 1. SOC_PB_PP_port_info_set(local_port_ndx,
 *   port_profile)
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_ac_map_key_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY_QUALIFIER             *qual_key,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE              key_mapping
  );

uint32
  soc_pb_pp_l2_lif_ac_map_key_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY_QUALIFIER                 *qual_key,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE                  key_mapping
  );

uint32
  soc_pb_pp_l2_lif_ac_map_key_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY_QUALIFIER                 *qual_key
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_l2_lif_ac_map_key_set" API.
 *     Refer to "soc_pb_pp_l2_lif_ac_map_key_set" API for details.
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_ac_map_key_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY_QUALIFIER             *qual_key,
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_AC_MAP_KEY_TYPE              *key_mapping
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_l2_lif_ac_mp_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   This function is used to define a Maintenance Point (MP)
 *   on an incoming Attachment-Circuit (port x VLAN x VLAN)
 *   and MD-level, and to determine the action to perform. If
 *   the MP is one of the 4K accelerated MEPs, the function
 *   configures the related OAMP databases and associates the
 *   AC and MD-Level with a user-provided handle. This handle
 *   is later used by user to access OAMP database for this
 *   MEP.
 * INPUT:
 *   SOC_SAND_IN  int                          unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_LIF_ID                       lif_ndx -
 *     LIF table index
 *   SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_MP_INFO            *info -
 *     MP configuration information.
 * REMARKS:
 *   Should only be used if the MP is down MEP or MIP.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_ac_mp_info_set_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                       lif_ndx,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_MP_INFO            *info
  );

uint32
  soc_pb_pp_l2_lif_ac_mp_info_set_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                       lif_ndx,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_MP_INFO            *info
  );

uint32
  soc_pb_pp_l2_lif_ac_mp_info_get_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                       lif_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_l2_lif_ac_mp_info_set_unsafe" API.
 *     Refer to "soc_pb_pp_l2_lif_ac_mp_info_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_ac_mp_info_get_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                       lif_ndx,
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_AC_MP_INFO            *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_l2_lif_ac_add_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Enable an Attachment Circuit, and set the AC attributes.
 *   Bind the ac-key to the LIF, and set the LIF attributes
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY                       *ac_key -
 *     AC key identifies the AC and how to map a packet to it
 *   SOC_SAND_IN  SOC_PB_PP_LIF_ID                              lif_index -
 *     Logical Interfaces table index. The LIF index is also the
 *     AC ID that the packet is learnt with.
 *   SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_INFO                      *ac_info -
 *     AC Attributes. VSID, Ingress Editing attributes, ...
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success -
 *     SOC_SAND_FAILURE_OUT_OF_RESOURCES: There is no space (in the
 *     Exact Match table)
 * REMARKS:
 *   - T20E: The VLAN domain is equal to the port ID- T20E:
 *   The concept of VLAN ranges is not applicable- Soc_petra-B:
 *   In multipoint services, the default forwarding is
 *   according to the VSI- QinQ CEP port: o The VSI is the
 *   S-VIDo 'vid_ndx' is the C-VID tago This API registers
 *   C-VID * port to service o The egress editing module will
 *   map the VSI to the appended S-VIDo No need in AC
 *   learning, because the S-VID is the service, and the
 *   S-VID * port is sufficient for the editing.o Ingress
 *   editing may be used to append / replace / remove C-VID-
 *   MIM: This function is valid for customer facing ACs. In
 *   this case, the VSI holds the I-SID as the EEI- VPLS: The
 *   resolved VSI is the core VSI, and may be updated
 *   according to PWE- LIF index: Should be equal to
 *   'out_ac_ndx' when configuring the Out-AC via
 *   soc_ppd_eg_ac_info_set()When Adding an AC to the system, the
 *   user is expected to:- Set the Out-AC Egress editing.-
 *   For multipoint services, when egress editing is
 *   according to AC, add the AC as a member in the flooding
 *   multicast ID. Otherwise, add the source port- Call this
 *   function
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_ac_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY                       *ac_key,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_INFO                      *ac_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );


uint32
  soc_pb_pp_l2_lif_ac_add_internal_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY                           *ac_key,
    SOC_SAND_IN  uint8                                     ignore_key,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                                  lif_index,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_INFO                          *ac_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  );


uint32
  soc_pb_pp_l2_lif_ac_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY                       *ac_key,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_INFO                      *ac_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_l2_lif_ac_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get an Attachment Circuit, according to AC key.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY                       *ac_key -
 *     AC key identifies the AC and how to map a packet to it
 *   SOC_SAND_OUT SOC_PB_PP_LIF_ID                              *lif_index -
 *     The Logical Interfaces table index that the AC was found
 *     in.
 *   SOC_SAND_OUT SOC_PB_PP_L2_LIF_AC_INFO                      *ac_info -
 *     AC Attributes. VSID, Ingress Editing attributes, ...
 *   SOC_SAND_OUT uint8                                 *found -
 *     TRUE: The entry was found, 'ac_info' is validFALSE: The
 *     entry was not found, 'ac_info' is invalid
 * REMARKS:
 *   Get the AC info and index, according to the AC Key
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_ac_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY                       *ac_key,
    SOC_SAND_OUT SOC_PB_PP_LIF_ID                              *lif_index,
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_AC_INFO                      *ac_info,
    SOC_SAND_OUT uint8                                 *found
  );

uint32
  soc_pb_pp_l2_lif_ac_get_internal_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY                           *ac_key,
    SOC_SAND_IN  uint8                                     ignore_key,
    SOC_SAND_OUT SOC_PB_PP_LIF_ID                                  *lif_index,
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_AC_INFO                          *ac_info,
    SOC_SAND_OUT uint8                                     *found
  );

uint32
  soc_pb_pp_l2_lif_ac_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY                       *ac_key
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_l2_lif_ac_with_cos_add_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add Attachment Circuit (AC) group. Enable defining ACs,
 *   according to Quality of Service attributes, on top of
 *   the usual port * VID [*VID] identification
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY                       *ac_key -
 *     AC key that identifies the ACs group.
 *   SOC_SAND_IN  SOC_PB_PP_LIF_ID                              base_lif_index -
 *     Logical Interfaces table base index. The AC occupies
 *     entries base_lif_index to (base_lif_index + ACs-group
 *     size - 1) in the Logical Interfaces table
 *   SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_GROUP_INFO                *acs_group_info -
 *     ACs Group Attributes. Contain an array of ACs, and
 *     pointer to the mapping attributes between the QoS
 *     information and the ACs in the group.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success -
 *     SOC_SAND_FAILURE_OUT_OF_RESOURCES: There is no space in the
 *     Exact Match table
 * REMARKS:
 *   - See remarks of soc_ppd_l2_lif_ac_add()- Map VLAN Domain
 *   and VID(s) to group of AC Logical interfaces, and set
 *   ACs information. The QoS attribute type and mapping is
 *   according to the opcode_id. - 'nof_lif_entries' is only
 *   used for driver validation. The Number of LIF entries is
 *   derived from the 'opcode_id'.- The opcode types are
 *   configured via the API soc_ppd_lif_cos_opcode_types_set
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_ac_with_cos_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY                       *ac_key,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                              base_lif_index,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_GROUP_INFO                *acs_group_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  soc_pb_pp_l2_lif_ac_with_cos_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY                       *ac_key,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                              base_lif_index,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_GROUP_INFO                *acs_group_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_l2_lif_ac_with_cos_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get Attachment Circuit (AC) group attributes
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY                       *ac_key -
 *     AC key identifies the ACs group and how to map a packet
 *     to it. Out of the ACs group, the specific AC is
 *     according to the QoS params and the Op-code.
 *   SOC_SAND_OUT SOC_PB_PP_LIF_ID                              *base_lif_index -
 *     Logical Interfaces table base index. The LIF index of the
 *     first AC in the ACs array.
 *   SOC_SAND_OUT SOC_PB_PP_L2_LIF_AC_GROUP_INFO                *acs_group_info -
 *     ACs Group Attributes. Contain an array of ACs, and
 *     pointer to the mapping attributes between the QoS
 *     information and the ACs in the group.
 *   SOC_SAND_OUT uint8                                 *found -
 *     TRUE: The entry was found-'acs_group_info' is
 *     validFALSE: The entry was not found-'acs_group_info' is
 *     invalid
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_ac_with_cos_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY                       *ac_key,
    SOC_SAND_OUT SOC_PB_PP_LIF_ID                              *base_lif_index,
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_AC_GROUP_INFO                *acs_group_info,
    SOC_SAND_OUT uint8                                 *found
  );

uint32
  soc_pb_pp_l2_lif_ac_with_cos_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY                       *ac_key
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_l2_lif_ac_remove_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove an Attachment Circuit
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY                       *ac_key -
 *     AC key identifies
 *   SOC_SAND_OUT SOC_PB_PP_LIF_ID                              *lif_index -
 *     Logical Interfaces table index
 * REMARKS:
 *   - Unbind the mapping of the AC to the LIF table from the
 *   SEM table- Invalidate the 'lif_index' entry in the LIF
 *   table- The 'lif_index' is returned to the user to enable
 *   management of the LIF table- Revert both
 *   soc_ppd_l2_lif_ac_with_cos_add() and soc_ppd_l2_lif_ac_add(). In
 *   this case, the driver invalidate all the lif table
 *   entries of the LIF group, and the returned 'lif_index'
 *   is the lif_base_index
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_ac_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY                       *ac_key,
    SOC_SAND_OUT SOC_PB_PP_LIF_ID                              *lif_index
  );

uint32
  soc_pb_pp_l2_lif_ac_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY                       *ac_key
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_l2_lif_l2cp_trap_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets Trap information for Layer 2 control protocol
 *   frames. Packet is an MEF layer 2 control protocol
 *   service frame When DA matches 01-80-c2-00-00-XX where XX
 *   = 8'b00xx_xxxx.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_L2_LIF_L2CP_KEY                     *l2cp_key -
 *     Reserved Multicast key including Destination MAC address
 *     lsb (the msb are constant) and l2cp profile (set
 *     according to in-AC see soc_ppd_l2_lif_ac_add()).
 *   SOC_SAND_IN  SOC_PB_PP_L2_LIF_L2CP_HANDLE_TYPE             handle_type -
 *     How to handle the frame with the l2cp_key, drop,
 *     peer(trap), normal or tunneled (transparent)
 * REMARKS:
 *   - Soc_petra-B only.- This applied only for L2 AC LIF.- in
 *   case L2CP packet to be dropped/trapped(peer) the packet
 *   is assigned Trap code
 *   SOC_PPD_TRAP_CODE_L2CP_DROP/SOC_PPD_TRAP_CODE_L2CP_PEER. - In
 *   order to set the forwarding/snooping action use
 *   soc_ppd_trap_frwrd_profile_info_set()/soc_ppd_trap_snoop_profile_info_set
 *   with the above trap codes
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_l2cp_trap_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_L2CP_KEY                     *l2cp_key,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_L2CP_HANDLE_TYPE             handle_type
  );

uint32
  soc_pb_pp_l2_lif_l2cp_trap_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_L2CP_KEY                     *l2cp_key,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_L2CP_HANDLE_TYPE             handle_type
  );

uint32
  soc_pb_pp_l2_lif_l2cp_trap_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_L2CP_KEY                     *l2cp_key
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_l2_lif_l2cp_trap_set_unsafe" API.
 *     Refer to "soc_pb_pp_l2_lif_l2cp_trap_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_l2cp_trap_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_L2CP_KEY                     *l2cp_key,
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_L2CP_HANDLE_TYPE             *handle_type
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_l2_lif_isid_add_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Map an I-SID to VSI and vice versa. Set I-SID related
 *   attributes. Relevant when the packet arrives from the
 *   PBP
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_SYS_VSI_ID                          vsi_ndx -
 *     System VSID.
 *   SOC_SAND_IN  SOC_PB_PP_L2_LIF_ISID_KEY                     *isid_key -
 *     I-SID, as taken from the I-TAG, and optionally I-SID
 *     domain.
 *   SOC_SAND_IN  SOC_PB_PP_LIF_ID                              lif_index -
 *     Logical Interfaces Table Entry. I-SID mapping to VSI is
 *     done in the LIF table. Soc_petra-B Range: 0-16K.
 *   SOC_SAND_IN  SOC_PB_PP_L2_LIF_ISID_INFO                        *isid_info -
 *     VSID and I-SID related attributes
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success -
 *     SOC_SAND_FAILURE_OUT_OF_RESOURCES: There is no space in
 *     Exact Match
 * REMARKS:
 *   The mapping from the VSI to I-SID is done in the VSID
 *   table. The mapping from the I-SID to VSI is done in the
 *   LIF table
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_isid_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_SYS_VSI_ID                          vsi_ndx,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_ISID_KEY                     *isid_key,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_ISID_INFO                    *isid_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  soc_pb_pp_l2_lif_isid_add_internal_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_SYS_VSI_ID                              vsi_ndx,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_ISID_KEY                         *isid_key,
    SOC_SAND_IN  uint8                                     ignore_key,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                                  lif_index,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_ISID_INFO                        *isid_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                          *success
  );

uint32
  soc_pb_pp_l2_lif_isid_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_SYS_VSI_ID                          vsi_ndx,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_ISID_KEY                     *isid_key,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_ISID_INFO                    *isid_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_l2_lif_isid_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Map an I-SID to VSI and vice versa. Get I-SID related
 *   attributes. Relevant when the packet arrives from the
 *   PBP
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_L2_LIF_ISID_KEY                     *isid_key -
 *     I-SID, as taken from the I-TAG, and optionally I-SID
 *     domain.
 *   SOC_SAND_OUT SOC_PB_PP_LIF_ID                              *lif_index -
 *     Logical Interfaces Table Entry
 *   SOC_SAND_OUT SOC_PB_PP_SYS_VSI_ID                          *vsi_index -
 *     System VSID.
 *   SOC_SAND_OUT SOC_PB_PP_L2_LIF_ISID_INFO                    *isid_info -
 *     VSID and I-SID related attributes
 *   SOC_SAND_OUT uint8                                 *found -
 *     TRUE: ISID was foundFALSE: ISID was not found
 * REMARKS:
 *   The ISID mapping information is taken from the LIF
 *   table.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_isid_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_ISID_KEY                     *isid_key,
    SOC_SAND_OUT SOC_PB_PP_LIF_ID                              *lif_index,
    SOC_SAND_OUT SOC_PB_PP_SYS_VSI_ID                          *vsi_index,
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_ISID_INFO                    *isid_info,
    SOC_SAND_OUT uint8                                 *found
  );

uint32
  soc_pb_pp_l2_lif_isid_get_internal_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_ISID_KEY                         *isid_key,
    SOC_SAND_IN  uint8                                     ignore_key, 
    SOC_SAND_OUT SOC_PB_PP_LIF_ID                                  *lif_index,
    SOC_SAND_OUT SOC_PB_PP_SYS_VSI_ID                              *vsi_index,
    SOC_SAND_OUT SOC_PB_PP_L2_LIF_ISID_INFO                        *isid_info,
    SOC_SAND_OUT uint8                                     *found
  );

uint32
  soc_pb_pp_l2_lif_isid_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_ISID_KEY                     *isid_key
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_l2_lif_vsi_to_isid_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the I-SID related to the VSI
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_SYS_VSI_ID                          vsi_ndx -
 *     System VSID.
 *   SOC_SAND_OUT SOC_SAND_PP_ISID                              *isid_id -
 *     I-SID related to the VSID.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_vsi_to_isid_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_SYS_VSI_ID                              vsi_ndx,
    SOC_SAND_OUT SOC_SAND_PP_ISID                                  *isid_id
  );

uint32
  soc_pb_pp_l2_lif_vsi_to_isid_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_SYS_VSI_ID                              vsi_ndx
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_l2_lif_isid_remove_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Unbind an I-SID from VSI and vice versa
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_SYS_VSI_ID                          vsi_ndx -
 *     System VSID.
 *   SOC_SAND_IN  SOC_PB_PP_L2_LIF_ISID_KEY                     *isid_key -
 *     I-SID, as taken from the I-TAG, and optionally I-SID
 *     domain.
 *   SOC_SAND_OUT SOC_PB_PP_LIF_ID                              lif_index -
 *     Logical Interfaces Table Entry. I-SID mapping to VSI is
 *     done in the LIF table. Soc_petra-B Range: 0-16KT20E Range:
 *     0-128K
 * REMARKS:
 *   - Unbind the mapping from the VSI to the I-SID in the
 *   VSI table- Unbind the mapping of the ISID-key to the LIF
 *   table from the SEM table- Invalidate the 'lif_index'
 *   entry in the LIF table- The 'lif_index' is returned to
 *   the user to enable management of the LIF table
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_l2_lif_isid_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_ISID_KEY                     *isid_key,
    SOC_SAND_OUT SOC_PB_PP_LIF_ID                              *lif_index
  );

uint32
  soc_pb_pp_l2_lif_isid_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_ISID_KEY                     *isid_key
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_lif_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   soc_pb_pp_api_lif module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_lif_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   soc_pb_pp_lif_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   soc_pb_pp_api_lif module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_lif_get_errs_ptr(void);

uint32
  SOC_PB_PP_L2_LIF_AC_KEY_QUALIFIER_verify(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY_QUALIFIER *info
  );

uint32
  SOC_PB_PP_L2_LIF_IN_VC_RANGE_verify(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_IN_VC_RANGE *info
  );

uint32
  SOC_PB_PP_L2_LIF_DEFAULT_FORWARDING_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_DEFAULT_FORWARDING_INFO *info
  );

uint32
  SOC_PB_PP_L2_LIF_PWE_LEARN_RECORD_verify(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_PWE_LEARN_RECORD *info
  );

uint32
  SOC_PB_PP_L2_LIF_PWE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_PWE_INFO *info
  );

uint32
  SOC_PB_PP_L2_VLAN_RANGE_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_L2_VLAN_RANGE_KEY *info
  );

uint32
  SOC_PB_PP_L2_LIF_AC_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_KEY *info
  );

uint32
  SOC_PB_PP_L2_LIF_AC_LEARN_RECORD_verify(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_LEARN_RECORD *info
  );

uint32
  SOC_PB_PP_L2_LIF_AC_ING_EDIT_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_ING_EDIT_INFO *info
  );

uint32
  SOC_PB_PP_L2_LIF_AC_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_INFO *info
  );

uint32
  SOC_PB_PP_L2_LIF_AC_MP_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_MP_INFO *info
  );

uint32
  SOC_PB_PP_L2_LIF_L2CP_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_L2CP_KEY *info
  );

uint32
  SOC_PB_PP_L2_LIF_AC_GROUP_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_AC_GROUP_INFO *info
  );

uint32
  SOC_PB_PP_L2_LIF_ISID_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_ISID_KEY *info
  );

uint32
  SOC_PB_PP_L2_LIF_ISID_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_ISID_INFO *info
  );

uint32
  SOC_PB_PP_L2_LIF_TRILL_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_L2_LIF_TRILL_INFO *info
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PP_LIF_INCLUDED__*/
#endif
