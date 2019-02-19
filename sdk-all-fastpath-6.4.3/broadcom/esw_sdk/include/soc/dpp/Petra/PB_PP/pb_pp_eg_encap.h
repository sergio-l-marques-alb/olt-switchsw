/* $Id: pb_pp_eg_encap.h,v 1.9 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/include/soc_pb_pp_eg_encap.h
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

#ifndef __SOC_PB_PP_EG_ENCAP_INCLUDED__
/* { */
#define __SOC_PB_PP_EG_ENCAP_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_api_eg_encap.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>

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
  SOC_PB_PP_EG_ENCAP_RANGE_INFO_SET = SOC_PB_PP_PROC_DESC_BASE_EG_ENCAP_FIRST,
  SOC_PB_PP_EG_ENCAP_RANGE_INFO_SET_PRINT,
  SOC_PB_PP_EG_ENCAP_RANGE_INFO_SET_UNSAFE,
  SOC_PB_PP_EG_ENCAP_RANGE_INFO_SET_VERIFY,
  SOC_PB_PP_EG_ENCAP_RANGE_INFO_GET,
  SOC_PB_PP_EG_ENCAP_RANGE_INFO_GET_PRINT,
  SOC_PB_PP_EG_ENCAP_RANGE_INFO_GET_VERIFY,
  SOC_PB_PP_EG_ENCAP_RANGE_INFO_GET_UNSAFE,
  SOC_PB_PP_EG_ENCAP_NULL_LIF_ENTRY_ADD,
  SOC_PB_PP_EG_ENCAP_NULL_LIF_ENTRY_ADD_PRINT,
  SOC_PB_PP_EG_ENCAP_NULL_LIF_ENTRY_ADD_UNSAFE,
  SOC_PB_PP_EG_ENCAP_NULL_LIF_ENTRY_ADD_VERIFY,
  SOC_PB_PP_EG_ENCAP_AC_ENTRY_ADD,
  SOC_PB_PP_EG_ENCAP_AC_ENTRY_ADD_PRINT,
  SOC_PB_PP_EG_ENCAP_AC_ENTRY_ADD_UNSAFE,
  SOC_PB_PP_EG_ENCAP_AC_ENTRY_ADD_VERIFY,
  SOC_PB_PP_EG_ENCAP_SWAP_COMMAND_ENTRY_ADD,
  SOC_PB_PP_EG_ENCAP_SWAP_COMMAND_ENTRY_ADD_PRINT,
  SOC_PB_PP_EG_ENCAP_SWAP_COMMAND_ENTRY_ADD_UNSAFE,
  SOC_PB_PP_EG_ENCAP_SWAP_COMMAND_ENTRY_ADD_VERIFY,
  SOC_PB_PP_EG_ENCAP_PWE_ENTRY_ADD,
  SOC_PB_PP_EG_ENCAP_PWE_ENTRY_ADD_PRINT,
  SOC_PB_PP_EG_ENCAP_PWE_ENTRY_ADD_UNSAFE,
  SOC_PB_PP_EG_ENCAP_PWE_ENTRY_ADD_VERIFY,
  SOC_PB_PP_EG_ENCAP_POP_COMMAND_ENTRY_ADD,
  SOC_PB_PP_EG_ENCAP_POP_COMMAND_ENTRY_ADD_PRINT,
  SOC_PB_PP_EG_ENCAP_POP_COMMAND_ENTRY_ADD_UNSAFE,
  SOC_PB_PP_EG_ENCAP_POP_COMMAND_ENTRY_ADD_VERIFY,
  SOC_PB_PP_EG_ENCAP_VSI_ENTRY_ADD,
  SOC_PB_PP_EG_ENCAP_VSI_ENTRY_ADD_PRINT,
  SOC_PB_PP_EG_ENCAP_VSI_ENTRY_ADD_UNSAFE,
  SOC_PB_PP_EG_ENCAP_VSI_ENTRY_ADD_VERIFY,
  SOC_PB_PP_EG_ENCAP_MPLS_ENTRY_ADD,
  SOC_PB_PP_EG_ENCAP_MPLS_ENTRY_ADD_PRINT,
  SOC_PB_PP_EG_ENCAP_MPLS_ENTRY_ADD_UNSAFE,
  SOC_PB_PP_EG_ENCAP_MPLS_ENTRY_ADD_VERIFY,
  SOC_PB_PP_EG_ENCAP_IPV4_ENTRY_ADD,
  SOC_PB_PP_EG_ENCAP_IPV4_ENTRY_ADD_PRINT,
  SOC_PB_PP_EG_ENCAP_IPV4_ENTRY_ADD_UNSAFE,
  SOC_PB_PP_EG_ENCAP_IPV4_ENTRY_ADD_VERIFY,
  SOC_PB_PP_EG_ENCAP_LL_ENTRY_ADD,
  SOC_PB_PP_EG_ENCAP_LL_ENTRY_ADD_PRINT,
  SOC_PB_PP_EG_ENCAP_LL_ENTRY_ADD_UNSAFE,
  SOC_PB_PP_EG_ENCAP_LL_ENTRY_ADD_VERIFY,
  SOC_PB_PP_EG_ENCAP_ENTRY_REMOVE,
  SOC_PB_PP_EG_ENCAP_ENTRY_REMOVE_PRINT,
  SOC_PB_PP_EG_ENCAP_ENTRY_REMOVE_UNSAFE,
  SOC_PB_PP_EG_ENCAP_ENTRY_REMOVE_VERIFY,
  SOC_PB_PP_EG_ENCAP_ENTRY_GET,
  SOC_PB_PP_EG_ENCAP_ENTRY_GET_PRINT,
  SOC_PB_PP_EG_ENCAP_ENTRY_GET_UNSAFE,
  SOC_PB_PP_EG_ENCAP_ENTRY_GET_VERIFY,
  SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO_SET,
  SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO_SET_PRINT,
  SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO_SET_UNSAFE,
  SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO_SET_VERIFY,
  SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO_GET,
  SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO_GET_PRINT,
  SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO_GET_VERIFY,
  SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO_GET_UNSAFE,
  SOC_PB_PP_EG_ENCAP_PUSH_EXP_INFO_SET,
  SOC_PB_PP_EG_ENCAP_PUSH_EXP_INFO_SET_PRINT,
  SOC_PB_PP_EG_ENCAP_PUSH_EXP_INFO_SET_UNSAFE,
  SOC_PB_PP_EG_ENCAP_PUSH_EXP_INFO_SET_VERIFY,
  SOC_PB_PP_EG_ENCAP_PUSH_EXP_INFO_GET,
  SOC_PB_PP_EG_ENCAP_PUSH_EXP_INFO_GET_PRINT,
  SOC_PB_PP_EG_ENCAP_PUSH_EXP_INFO_GET_VERIFY,
  SOC_PB_PP_EG_ENCAP_PUSH_EXP_INFO_GET_UNSAFE,
  SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO_SET,
  SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO_SET_PRINT,
  SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO_SET_UNSAFE,
  SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO_SET_VERIFY,
  SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO_GET,
  SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO_GET_PRINT,
  SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO_GET_VERIFY,
  SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO_GET_UNSAFE,
  SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_SET,
  SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_SET_PRINT,
  SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_SET_UNSAFE,
  SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_SET_VERIFY,
  SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_GET,
  SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_GET_PRINT,
  SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_GET_VERIFY,
  SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_SRC_IP_GET_UNSAFE,
  SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_SET,
  SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_SET_PRINT,
  SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_SET_UNSAFE,
  SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_SET_VERIFY,
  SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_GET,
  SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_GET_PRINT,
  SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_GET_VERIFY,
  SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TTL_GET_UNSAFE,
  SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_SET,
  SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_SET_PRINT,
  SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_SET_UNSAFE,
  SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_SET_VERIFY,
  SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_GET,
  SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_GET_PRINT,
  SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_GET_VERIFY,
  SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_GLBL_TOS_GET_UNSAFE,
  SOC_PB_PP_EG_ENCAP_GET_PROCS_PTR,
  SOC_PB_PP_EG_ENCAP_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */

  SOC_PB_PP_EG_ENCAP_LL_ENTRY_GET_UNSAFE,
  SOC_PB_PP_EG_ENCAP_TUNNEL_ENTRY_GET_UNSAFE,
  SOC_PB_PP_EG_ENCAP_LIF_ENTRY_GET_UNSAFE,

  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_EG_ENCAP_PROCEDURE_DESC_LAST
} SOC_PB_PP_EG_ENCAP_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PB_PP_EG_ENCAP_LIF_EEP_NDX_OUT_OF_RANGE_ERR = SOC_PB_PP_ERR_DESC_BASE_EG_ENCAP_FIRST,
  SOC_PB_PP_EG_ENCAP_NEXT_EEP_OUT_OF_RANGE_ERR,
  SOC_PB_PP_EG_ENCAP_TUNNEL_EEP_NDX_OUT_OF_RANGE_ERR,
  SOC_PB_PP_EG_ENCAP_LL_EEP_OUT_OF_RANGE_ERR,
  SOC_PB_PP_EG_ENCAP_LL_EEP_NDX_OUT_OF_RANGE_ERR,
  SOC_PB_PP_EG_ENCAP_EEP_TYPE_NDX_OUT_OF_RANGE_ERR,
  SOC_PB_PP_EG_ENCAP_EEP_NDX_OUT_OF_RANGE_ERR,
  SOC_PB_PP_EG_ENCAP_DEPTH_OUT_OF_RANGE_ERR,
  SOC_PB_PP_EG_ENCAP_NOF_ENTRIES_OUT_OF_RANGE_ERR,
  SOC_PB_PP_EG_ENCAP_PROFILE_NDX_OUT_OF_RANGE_ERR,
  SOC_PB_PP_EG_ENCAP_ENTRY_NDX_OUT_OF_RANGE_ERR,
  SOC_PB_PP_EG_ENCAP_SRC_IP_OUT_OF_RANGE_ERR,
  SOC_PB_PP_EG_ENCAP_LL_LIMIT_OUT_OF_RANGE_ERR,
  SOC_PB_PP_EG_ENCAP_IP_TNL_LIMIT_OUT_OF_RANGE_ERR,
  SOC_PB_PP_EG_ENCAP_MPLS_TNL_LIMIT_OUT_OF_RANGE_ERR,
  SOC_PB_PP_EG_ENCAP_SWAP_LABEL_OUT_OF_RANGE_ERR,
  SOC_PB_PP_EG_ENCAP_LABEL_OUT_OF_RANGE_ERR,
  SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_EG_ENCAP_OUT_VSI_OUT_OF_RANGE_ERR,
  SOC_PB_PP_EG_ENCAP_TPID_PROFILE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_EG_ENCAP_POP_TYPE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_EG_ENCAP_TUNNEL_LABEL_OUT_OF_RANGE_ERR,
  SOC_PB_PP_EG_ENCAP_NOF_TUNNELS_OUT_OF_RANGE_ERR,
  SOC_PB_PP_EG_ENCAP_ORIENTATION_OUT_OF_RANGE_ERR,
  SOC_PB_PP_EG_ENCAP_DEST_OUT_OF_RANGE_ERR,
  SOC_PB_PP_EG_ENCAP_SRC_INDEX_OUT_OF_RANGE_ERR,
  SOC_PB_PP_EG_ENCAP_TTL_INDEX_OUT_OF_RANGE_ERR,
  SOC_PB_PP_EG_ENCAP_TOS_INDEX_OUT_OF_RANGE_ERR,
  SOC_PB_PP_EG_ENCAP_PCP_DEI_OUT_OF_RANGE_ERR,
  SOC_PB_PP_EG_ENCAP_TPID_INDEX_OUT_OF_RANGE_ERR,
  SOC_PB_PP_EG_ENCAP_MODEL_OUT_OF_RANGE_ERR,
  SOC_PB_PP_EG_ENCAP_EXP_MARK_MODE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_EG_ENCAP_CW_OUT_OF_RANGE_ERR,
  SOC_PB_PP_EG_ENCAP_ENTRY_TYPE_OUT_OF_RANGE_ERR,
  /*
   * } Auto generated. Do not edit previous section.
   */

  SOC_PB_PP_EG_ENCAP_LIF_EEP_NDX_AC_OUT_OF_RANGE_ERR,
  SOC_PB_PP_EG_ENCAP_LIF_EEP_AND_AC_ID_NOT_EQUAL_ERR,
  SOC_PB_PP_EG_ENCAP_LIF_EEP_NDX_MPLS_TUNNEL_OUT_OF_RANGE_ERR,
  SOC_PB_PP_EG_ENCAP_LIF_EEP_NDX_PWE_MCAST_OUT_OF_RANGE_ERR,
  SOC_PB_PP_EG_ENCAP_IP_TUNNEL_EEP_NDX_OUT_OF_RANGE_ERR,

  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_EG_ENCAP_ERR_LAST
} SOC_PB_PP_EG_ENCAP_ERR;

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
  soc_pb_pp_eg_encap_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_encap_range_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets devision of the Egress Encapsulation Table between
 *   the different usages (Link layer/ IP tunnels/ MPLS
 *   tunnels).
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_RANGE_INFO                 *range_info -
 *     Range information.
 * REMARKS:
 *   - T20E: not supported. Error will be returned if called.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_range_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_RANGE_INFO                 *range_info
  );

uint32
  soc_pb_pp_eg_encap_range_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_RANGE_INFO                 *range_info
  );

uint32
  soc_pb_pp_eg_encap_range_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_eg_encap_range_info_set_unsafe" API.
 *     Refer to "soc_pb_pp_eg_encap_range_info_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_range_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_RANGE_INFO                 *range_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_encap_null_lif_entry_add_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set LIF Editing entry to be NULL Entry.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  lif_eep_ndx -
 *     Egress Encapsulation table entry, of type Out-LIF (First
 *     encapsulation)
 *   SOC_SAND_IN  uint32                                  next_eep -
 *     Pointer to the next egress encapsulation table entry
 * REMARKS:
 *   - T20E Only API- Needed when the packet should only be
 *   encapsulated with Tunnel information and the EEI is NULL
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_null_lif_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  uint32                                  next_eep
  );

uint32
  soc_pb_pp_eg_encap_null_lif_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  uint32                                  next_eep
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_encap_ac_entry_add_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set LIF Editing entry to hold AC ID. Actually maps from
 *   CUD to AC.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  lif_eep_ndx -
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
  soc_pb_pp_eg_encap_ac_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  SOC_PB_PP_AC_ID                               ac_id
  );

uint32
  soc_pb_pp_eg_encap_ac_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  SOC_PB_PP_AC_ID                               ac_id
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_encap_swap_command_entry_add_unsafe
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
  soc_pb_pp_eg_encap_swap_command_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_SWAP_INFO                  *swap_info,
    SOC_SAND_IN  uint32                                  next_eep
  );

uint32
  soc_pb_pp_eg_encap_swap_command_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_SWAP_INFO                  *swap_info,
    SOC_SAND_IN  uint32                                  next_eep
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_encap_pwe_entry_add_unsafe
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
  soc_pb_pp_eg_encap_pwe_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PWE_INFO                   *pwe_info,
    SOC_SAND_IN  uint32                                  next_eep
  );

uint32
  soc_pb_pp_eg_encap_pwe_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PWE_INFO                   *pwe_info,
    SOC_SAND_IN  uint32                                  next_eep
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_encap_pop_command_entry_add_unsafe
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
  soc_pb_pp_eg_encap_pop_command_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_POP_INFO                   *pop_info,
    SOC_SAND_IN  uint32                                  next_eep
  );

uint32
  soc_pb_pp_eg_encap_pop_command_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_POP_INFO                   *pop_info,
    SOC_SAND_IN  uint32                                  next_eep
  );

/*********************************************************************
 *  invalid for Soc_petraB 
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_vsi_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,   
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN SOC_PB_PP_EG_ENCAP_VSI_ENCAP_INFO             *vsi_info, 
    SOC_SAND_IN uint8                                  next_eep_valid, 
    SOC_SAND_IN  uint32                                  next_eep 
  );

uint32
  soc_pb_pp_eg_encap_vsi_entry_add_verify(
    SOC_SAND_IN  int                                 unit,   
    SOC_SAND_IN  uint32                                  lif_eep_ndx,
    SOC_SAND_IN SOC_PB_PP_EG_ENCAP_VSI_ENCAP_INFO             *vsi_info, 
    SOC_SAND_IN uint8                                  next_eep_valid, 
    SOC_SAND_IN  uint32                                  next_eep 
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_encap_mpls_entry_add_unsafe
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
  soc_pb_pp_eg_encap_mpls_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  tunnel_eep_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_MPLS_ENCAP_INFO            *mpls_encap_info,
    SOC_SAND_IN  uint32                                  ll_eep
  );

uint32
  soc_pb_pp_eg_encap_mpls_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  tunnel_eep_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_MPLS_ENCAP_INFO            *mpls_encap_info,
    SOC_SAND_IN  uint32                                  ll_eep
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_encap_ipv4_entry_add_unsafe
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
  soc_pb_pp_eg_encap_ipv4_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  tunnel_eep_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_IPV4_ENCAP_INFO            *ipv4_encap_info,
    SOC_SAND_IN  uint32                                  ll_eep
  );

uint32
  soc_pb_pp_eg_encap_ipv4_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  tunnel_eep_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_IPV4_ENCAP_INFO            *ipv4_encap_info,
    SOC_SAND_IN  uint32                                  ll_eep
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_encap_ll_entry_add_unsafe
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
  soc_pb_pp_eg_encap_ll_entry_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  ll_eep_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_LL_INFO                    *ll_encap_info
  );

uint32
  soc_pb_pp_eg_encap_ll_entry_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  ll_eep_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_LL_INFO                    *ll_encap_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_encap_entry_remove_unsafe
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
  soc_pb_pp_eg_encap_entry_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_EEP_TYPE                   eep_type_ndx,
    SOC_SAND_IN  uint32                                  eep_ndx
  );

uint32
  soc_pb_pp_eg_encap_entry_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_EEP_TYPE                   eep_type_ndx,
    SOC_SAND_IN  uint32                                  eep_ndx
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_encap_entry_get_unsafe
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
 *   SOC_SAND_IN  uint32                                      depth -
 *     Number of encapsulation entries to get, see example of
 *     use in remarks.
 *   SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_ENTRY_INFO                     encap_entry_info -
 *     SOC_PB_PP_NOF_EG_ENCAP_EEP_TYPES]- To include egress
 *     encapsulation entries.
 *   SOC_SAND_OUT uint32                                      next_eep -
 *     SOC_PB_PP_NOF_EG_ENCAP_EEP_TYPES]- To include next Egress
 *     encapsulation pointer.
 *   SOC_SAND_OUT uint32                                      *nof_entries -
 *     Number of valid entries in next_eep and
 *     encap_entry_info.
 * REMARKS:
 *   - To get all egress encapsulations performed for a given
 *   out-lif, set eep_type_ndx =
 *   SOC_PPD_EG_ENCAP_EEP_TYPE_LIF_EEP, eep_ndx = out-lif, depth
 *   = SOC_PB_PP_NOF_EG_ENCAP_EEP_TYPES.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_entry_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_EEP_TYPE                       eep_type_ndx,
    SOC_SAND_IN  uint32                                      eep_ndx,
    SOC_SAND_IN  uint32                                      depth,
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_ENTRY_INFO                     encap_entry_info[SOC_PB_PP_NOF_EG_ENCAP_EEP_TYPES],
    SOC_SAND_OUT uint32                                      next_eep[SOC_PB_PP_NOF_EG_ENCAP_EEP_TYPES],
    SOC_SAND_OUT uint32                                      *nof_entries
  );

uint32
  soc_pb_pp_eg_encap_entry_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_EEP_TYPE                       eep_type_ndx,
    SOC_SAND_IN  uint32                                      eep_ndx,
    SOC_SAND_IN  uint32                                      depth
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_encap_push_profile_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Setting the push profile info - specifying how to build
 *   the label header.
 * INPUT:
 *   SOC_SAND_IN  int                                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                      profile_ndx -
 *     Profile ID. Range: 0 - 7.
 *   SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO              *profile_info -
 *     Push profile information including TTL, EXP, has-CW, and
 *     if it is PIPE mode.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_push_profile_info_set_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO              *profile_info
  );

uint32
  soc_pb_pp_eg_encap_push_profile_info_set_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO              *profile_info
  );

uint32
  soc_pb_pp_eg_encap_push_profile_info_get_verify(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      profile_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_eg_encap_push_profile_info_set_unsafe" API.
 *     Refer to "soc_pb_pp_eg_encap_push_profile_info_set_unsafe"
 *     API for details.
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_push_profile_info_get_unsafe(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      profile_ndx,
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO              *profile_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_encap_push_exp_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the EXP value of the pushed label as mapping of the
 *   TC and DP.
 * INPUT:
 *   SOC_SAND_IN  int                                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PUSH_EXP_KEY                   *exp_key -
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
  soc_pb_pp_eg_encap_push_exp_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PUSH_EXP_KEY               *exp_key,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_EXP                          exp
  );

uint32
  soc_pb_pp_eg_encap_push_exp_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PUSH_EXP_KEY               *exp_key,
    SOC_SAND_IN  SOC_SAND_PP_MPLS_EXP                          exp
  );

uint32
  soc_pb_pp_eg_encap_push_exp_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PUSH_EXP_KEY               *exp_key
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_eg_encap_push_exp_info_set_unsafe" API.
 *     Refer to "soc_pb_pp_eg_encap_push_exp_info_set_unsafe" API
 *     for details.
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_push_exp_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PUSH_EXP_KEY               *exp_key,
    SOC_SAND_OUT SOC_SAND_PP_MPLS_EXP                          *exp
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_encap_pwe_glbl_info_set_unsafe
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
  soc_pb_pp_eg_encap_pwe_glbl_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO              *glbl_info
  );

uint32
  soc_pb_pp_eg_encap_pwe_glbl_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO              *glbl_info
  );

uint32
  soc_pb_pp_eg_encap_pwe_glbl_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_eg_encap_pwe_glbl_info_set_unsafe" API.
 *     Refer to "soc_pb_pp_eg_encap_pwe_glbl_info_set_unsafe" API
 *     for details.
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_pwe_glbl_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO              *glbl_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_encap_ipv4_tunnel_glbl_src_ip_set_unsafe
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
  soc_pb_pp_eg_encap_ipv4_tunnel_glbl_src_ip_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  uint32                                  src_ip
  );

uint32
  soc_pb_pp_eg_encap_ipv4_tunnel_glbl_src_ip_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  uint32                                  src_ip
  );

uint32
  soc_pb_pp_eg_encap_ipv4_tunnel_glbl_src_ip_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_eg_encap_ipv4_tunnel_glbl_src_ip_set_unsafe" API.
 *     Refer to
 *     "soc_pb_pp_eg_encap_ipv4_tunnel_glbl_src_ip_set_unsafe" API
 *     for details.
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_ipv4_tunnel_glbl_src_ip_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_OUT uint32                                  *src_ip
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_encap_ipv4_tunnel_glbl_ttl_set_unsafe
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
  soc_pb_pp_eg_encap_ipv4_tunnel_glbl_ttl_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IP_TTL                            ttl
  );

uint32
  soc_pb_pp_eg_encap_ipv4_tunnel_glbl_ttl_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IP_TTL                            ttl
  );

uint32
  soc_pb_pp_eg_encap_ipv4_tunnel_glbl_ttl_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_eg_encap_ipv4_tunnel_glbl_ttl_set_unsafe" API.
 *     Refer to
 *     "soc_pb_pp_eg_encap_ipv4_tunnel_glbl_ttl_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_ipv4_tunnel_glbl_ttl_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_OUT SOC_SAND_PP_IP_TTL                            *ttl
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_encap_ipv4_tunnel_glbl_tos_set_unsafe
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
  soc_pb_pp_eg_encap_ipv4_tunnel_glbl_tos_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_TOS                          tos
  );

uint32
  soc_pb_pp_eg_encap_ipv4_tunnel_glbl_tos_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_TOS                          tos
  );

uint32
  soc_pb_pp_eg_encap_ipv4_tunnel_glbl_tos_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_eg_encap_ipv4_tunnel_glbl_tos_set_unsafe" API.
 *     Refer to
 *     "soc_pb_pp_eg_encap_ipv4_tunnel_glbl_tos_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_eg_encap_ipv4_tunnel_glbl_tos_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_OUT SOC_SAND_PP_IPV4_TOS                          *tos
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_encap_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   soc_pb_pp_api_eg_encap module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_eg_encap_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_encap_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   soc_pb_pp_api_eg_encap module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_eg_encap_get_errs_ptr(void);

uint32
  SOC_PB_PP_EG_ENCAP_RANGE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_RANGE_INFO *info
  );

uint32
  SOC_PB_PP_EG_ENCAP_SWAP_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_SWAP_INFO *info
  );

uint32
  SOC_PB_PP_EG_ENCAP_PWE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PWE_INFO *info
  );

uint32
  SOC_PB_PP_EG_ENCAP_POP_INTO_ETH_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_POP_INTO_ETH_INFO *info
  );

uint32
  SOC_PB_PP_EG_ENCAP_POP_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_POP_INFO *info
  );

uint32
  SOC_PB_PP_EG_ENCAP_MPLS_TUNNEL_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_MPLS_TUNNEL_INFO *info
  );

uint32
  SOC_PB_PP_EG_ENCAP_MPLS_ENCAP_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_MPLS_ENCAP_INFO *info
  );

uint32
  SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_IPV4_TUNNEL_INFO *info
  );

uint32
  SOC_PB_PP_EG_ENCAP_IPV4_ENCAP_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_IPV4_ENCAP_INFO *info
  );

uint32
  SOC_PB_PP_EG_ENCAP_LL_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_LL_INFO *info
  );

uint32
  SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PUSH_PROFILE_INFO *info
  );

uint32
  SOC_PB_PP_EG_ENCAP_PUSH_EXP_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PUSH_EXP_KEY *info
  );

uint32
  SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_PWE_GLBL_INFO *info
  );

uint32
  SOC_PB_PP_EG_ENCAP_ENTRY_VALUE_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_ENTRY_VALUE *info
  );

uint32
  SOC_PB_PP_EG_ENCAP_ENTRY_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_ENCAP_ENTRY_INFO *info
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PP_EG_ENCAP_INCLUDED__*/
#endif
