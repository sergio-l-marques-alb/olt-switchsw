/* $Id: pb_pp_rif.h,v 1.7 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/include/soc_pb_pp_rif.h
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

#ifndef __SOC_PB_PP_RIF_INCLUDED__
/* { */
#define __SOC_PB_PP_RIF_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_api_rif.h>
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
  SOC_PB_PP_RIF_MPLS_LABELS_RANGE_SET = SOC_PB_PP_PROC_DESC_BASE_RIF_FIRST,
  SOC_PB_PP_RIF_MPLS_LABELS_RANGE_SET_PRINT,
  SOC_PB_PP_RIF_MPLS_LABELS_RANGE_SET_UNSAFE,
  SOC_PB_PP_RIF_MPLS_LABELS_RANGE_SET_VERIFY,
  SOC_PB_PP_RIF_MPLS_LABELS_RANGE_GET,
  SOC_PB_PP_RIF_MPLS_LABELS_RANGE_GET_PRINT,
  SOC_PB_PP_RIF_MPLS_LABELS_RANGE_GET_VERIFY,
  SOC_PB_PP_RIF_MPLS_LABELS_RANGE_GET_UNSAFE,
  SOC_PB_PP_RIF_MPLS_LABEL_MAP_ADD,
  SOC_PB_PP_RIF_MPLS_LABEL_MAP_ADD_PRINT,
  SOC_PB_PP_RIF_MPLS_LABEL_MAP_ADD_UNSAFE,
  SOC_PB_PP_RIF_MPLS_LABEL_MAP_ADD_INTERNAL_UNSAFE,
  SOC_PB_PP_RIF_MPLS_LABEL_MAP_ADD_VERIFY,
  SOC_PB_PP_RIF_MPLS_LABEL_MAP_REMOVE,
  SOC_PB_PP_RIF_MPLS_LABEL_MAP_REMOVE_PRINT,
  SOC_PB_PP_RIF_MPLS_LABEL_MAP_REMOVE_UNSAFE,
  SOC_PB_PP_RIF_MPLS_LABEL_MAP_REMOVE_VERIFY,
  SOC_PB_PP_RIF_MPLS_LABEL_MAP_GET,
  SOC_PB_PP_RIF_MPLS_LABEL_MAP_GET_PRINT,
  SOC_PB_PP_RIF_MPLS_LABEL_MAP_GET_UNSAFE,
  SOC_PB_PP_RIF_MPLS_LABEL_MAP_GET_INTERNAL_UNSAFE,
  SOC_PB_PP_RIF_MPLS_LABEL_MAP_GET_VERIFY,
  SOC_PB_PP_RIF_IP_TUNNEL_MAP_ADD,
  SOC_PB_PP_RIF_IP_TUNNEL_MAP_ADD_PRINT,
  SOC_PB_PP_RIF_IP_TUNNEL_MAP_ADD_UNSAFE,
  SOC_PB_PP_RIF_IP_TUNNEL_MAP_ADD_INTERNAL_UNSAFE,
  SOC_PB_PP_RIF_IP_TUNNEL_MAP_ADD_VERIFY,
  SOC_PB_PP_RIF_IP_TUNNEL_MAP_REMOVE,
  SOC_PB_PP_RIF_IP_TUNNEL_MAP_REMOVE_PRINT,
  SOC_PB_PP_RIF_IP_TUNNEL_MAP_REMOVE_UNSAFE,
  SOC_PB_PP_RIF_IP_TUNNEL_MAP_REMOVE_VERIFY,
  SOC_PB_PP_RIF_IP_TUNNEL_MAP_GET,
  SOC_PB_PP_RIF_IP_TUNNEL_MAP_GET_PRINT,
  SOC_PB_PP_RIF_IP_TUNNEL_MAP_GET_UNSAFE,
  SOC_PB_PP_RIF_IP_TUNNEL_MAP_GET_INTERNAL_UNSAFE,
  SOC_PB_PP_RIF_IP_TUNNEL_MAP_GET_VERIFY,
  SOC_PB_PP_RIF_VSID_MAP_SET,
  SOC_PB_PP_RIF_VSID_MAP_SET_PRINT,
  SOC_PB_PP_RIF_VSID_MAP_SET_UNSAFE,
  SOC_PB_PP_RIF_VSID_MAP_SET_VERIFY,
  SOC_PB_PP_RIF_VSID_MAP_GET,
  SOC_PB_PP_RIF_VSID_MAP_GET_PRINT,
  SOC_PB_PP_RIF_VSID_MAP_GET_VERIFY,
  SOC_PB_PP_RIF_VSID_MAP_GET_UNSAFE,
  SOC_PB_PP_RIF_INFO_SET,
  SOC_PB_PP_RIF_INFO_SET_PRINT,
  SOC_PB_PP_RIF_INFO_SET_UNSAFE,
  SOC_PB_PP_RIF_INFO_SET_VERIFY,
  SOC_PB_PP_RIF_INFO_GET,
  SOC_PB_PP_RIF_INFO_GET_PRINT,
  SOC_PB_PP_RIF_INFO_GET_VERIFY,
  SOC_PB_PP_RIF_INFO_GET_UNSAFE,
  SOC_PB_PP_RIF_TTL_SCOPE_SET,
  SOC_PB_PP_RIF_TTL_SCOPE_SET_PRINT,
  SOC_PB_PP_RIF_TTL_SCOPE_SET_UNSAFE,
  SOC_PB_PP_RIF_TTL_SCOPE_SET_VERIFY,
  SOC_PB_PP_RIF_TTL_SCOPE_GET,
  SOC_PB_PP_RIF_TTL_SCOPE_GET_PRINT,
  SOC_PB_PP_RIF_TTL_SCOPE_GET_VERIFY,
  SOC_PB_PP_RIF_TTL_SCOPE_GET_UNSAFE,
  SOC_PB_PP_RIF_GET_PROCS_PTR,
  SOC_PB_PP_RIF_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */

  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_RIF_PROCEDURE_DESC_LAST
} SOC_PB_PP_RIF_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PB_PP_RIF_SUCCESS_OUT_OF_RANGE_ERR = SOC_PB_PP_ERR_DESC_BASE_RIF_FIRST,
  SOC_PB_PP_RIF_DIP_KEY_OUT_OF_RANGE_ERR,
  SOC_PB_PP_RIF_TTL_SCOPE_NDX_OUT_OF_RANGE_ERR,
  SOC_PB_PP_RIF_FIRST_LABEL_OUT_OF_RANGE_ERR,
  SOC_PB_PP_RIF_LAST_LABEL_OUT_OF_RANGE_ERR,
  SOC_PB_PP_RIF_COS_PROFILE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_RIF_TTL_SCOPE_INDEX_OUT_OF_RANGE_ERR,
  SOC_PB_PP_RIF_ROUTING_ENABLERS_BM_OUT_OF_RANGE_ERR,
  SOC_PB_PP_RIF_MPLS_LABEL_INVALID_RANGE_ERR,

  /*
   * } Auto generated. Do not edit previous section.
   */
  SOC_PB_PP_RIF_MPLS_LABEL_KEY_ILLEGAL_ERR,



  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_RIF_ERR_LAST
} SOC_PB_PP_RIF_ERR;

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
  soc_pb_pp_rif_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_rif_mpls_labels_range_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the MPLS labels that may be mapped to Router
 *   Interfaces
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_RIF_MPLS_LABELS_RANGE               *rif_labels_range -
 *     First and Last MPLS Labels to be mapped to Router
 *     Interfaces. Range Size: 0-896K
 * REMARKS:
 *   - Soc_petra-B: Ignored- T20E: Set the MPLS labels that may
 *   be mapped to Router Interfaces.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_rif_mpls_labels_range_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_RIF_MPLS_LABELS_RANGE               *rif_labels_range
  );

uint32
  soc_pb_pp_rif_mpls_labels_range_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_RIF_MPLS_LABELS_RANGE               *rif_labels_range
  );

uint32
  soc_pb_pp_rif_mpls_labels_range_get_verify(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_rif_mpls_labels_range_set_unsafe" API.
 *     Refer to "soc_pb_pp_rif_mpls_labels_range_set_unsafe" API
 *     for details.
*********************************************************************/
uint32
  soc_pb_pp_rif_mpls_labels_range_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_RIF_MPLS_LABELS_RANGE               *rif_labels_range
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_rif_mpls_label_map_add_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Enable MPLS labels termination and setting the Router
 *   interface according to the terminated MPLS label.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_MPLS_LABEL_RIF_KEY                  *mpls_key -
 *     The MPLS label, and optionally the VSID, to be
 *     terminated and mapped to RIF
 *   SOC_SAND_IN  SOC_PB_PP_LIF_ID                              lif_index -
 *     LIF table index
 *   SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_INFO                      *term_info -
 *     MPLS Termination info including type of termination
 *     (pipe/uniform), RIF
 *   SOC_SAND_IN  SOC_PB_PP_RIF_INFO                            *rif_info -
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
  soc_pb_pp_rif_mpls_label_map_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MPLS_LABEL_RIF_KEY                  *mpls_key,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_INFO                      *term_info,
    SOC_SAND_IN  SOC_PB_PP_RIF_INFO                            *rif_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  soc_pb_pp_rif_mpls_label_map_add_internal_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MPLS_LABEL_RIF_KEY                  *mpls_key,
    SOC_SAND_IN  uint8                                 ignore_key,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_INFO                      *term_info,
    SOC_SAND_IN  SOC_PB_PP_RIF_INFO                            *rif_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  soc_pb_pp_rif_mpls_label_map_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MPLS_LABEL_RIF_KEY                  *mpls_key,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PB_PP_MPLS_TERM_INFO                      *term_info,
    SOC_SAND_IN  SOC_PB_PP_RIF_INFO                            *rif_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_rif_mpls_label_map_remove_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove MPLS label that was mapped to a RIF-Tunnel
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_MPLS_LABEL_RIF_KEY                  *mpls_key -
 *     The MPLS label, and optionally the VSID, to be
 *     terminated and mapped to RIF
 *   SOC_SAND_OUT SOC_PB_PP_LIF_ID                              *lif_index -
 *     LIF table index
 * REMARKS:
 *   - Unbind the mapping of the MPLS label to the LIF table
 *   from the SEM table- Invalidate the 'lif_index' entry in
 *   the LIF table- The 'lif_index' is returned to the user
 *   to enable management of the LIF table
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_rif_mpls_label_map_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MPLS_LABEL_RIF_KEY                  *mpls_key,
    SOC_SAND_OUT SOC_PB_PP_LIF_ID                              *lif_index
  );

uint32
  soc_pb_pp_rif_mpls_label_map_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MPLS_LABEL_RIF_KEY                  *mpls_key
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_rif_mpls_label_map_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get MPLS label termination and Router interface info
 *   according to the terminated MPLS label.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_MPLS_LABEL_RIF_KEY                  *mpls_key -
 *     The MPLS label, and optionally the VSID, to be
 *     terminated and mapped to RIF
 *   SOC_SAND_OUT SOC_PB_PP_LIF_ID                              *lif_index -
 *     LIF table index
 *   SOC_SAND_OUT SOC_PB_PP_MPLS_TERM_INFO                      *term_info -
 *     MPLS Termination info including type of termination
 *     (pipe/uniform), RIF-id>. The term_info.cos-profile is
 *     ignored. Instead the rif_info.cos-profile is used.
 *   SOC_SAND_OUT SOC_PB_PP_RIF_INFO                            *rif_info -
 *     RIF attributes. RIF ID, ...
 *   SOC_SAND_OUT uint8                                 *found -
 *     Was key found
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_rif_mpls_label_map_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MPLS_LABEL_RIF_KEY                  *mpls_key,
    SOC_SAND_OUT SOC_PB_PP_LIF_ID                              *lif_index,
    SOC_SAND_OUT SOC_PB_PP_MPLS_TERM_INFO                      *term_info,
    SOC_SAND_OUT SOC_PB_PP_RIF_INFO                            *rif_info,
    SOC_SAND_OUT uint8                                 *found
  );

uint32
  soc_pb_pp_rif_mpls_label_map_get_internal_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MPLS_LABEL_RIF_KEY                  *mpls_key,
    SOC_SAND_IN  uint8                                 ignore_key,
    SOC_SAND_OUT  SOC_PB_PP_LIF_ID                             *lif_index,
    SOC_SAND_OUT  SOC_PB_PP_MPLS_TERM_INFO                     *term_info,
    SOC_SAND_OUT  SOC_PB_PP_RIF_INFO                           *rif_info,
    SOC_SAND_OUT uint8                                 *found
  );

uint32
  soc_pb_pp_rif_mpls_label_map_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MPLS_LABEL_RIF_KEY                  *mpls_key
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_rif_ip_tunnel_map_add_unsafe
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
 *   SOC_SAND_IN  SOC_PB_PP_LIF_ID                              lif_index -
 *     Entry in the LIF table
 *   SOC_SAND_IN  SOC_PB_PP_RIF_IP_TERM_INFO                    *term_info -
 *     IP tunnel Termination info including type of termination
 *     RIF-id, cos-profile.
 *   SOC_SAND_IN  SOC_PB_PP_RIF_INFO                            *rif_info -
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
  soc_pb_pp_rif_ip_tunnel_map_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  dip_key,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PB_PP_RIF_IP_TERM_INFO                    *term_info,
    SOC_SAND_IN  SOC_PB_PP_RIF_INFO                            *rif_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  soc_pb_pp_rif_ip_tunnel_map_add_internal_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  dip_key,
    SOC_SAND_IN  uint8                                 ignore_key,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PB_PP_RIF_IP_TERM_INFO                    *term_info,
    SOC_SAND_IN  SOC_PB_PP_RIF_INFO                            *rif_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  soc_pb_pp_rif_ip_tunnel_map_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  dip_key,
    SOC_SAND_IN  SOC_PB_PP_LIF_ID                              lif_index,
    SOC_SAND_IN  SOC_PB_PP_RIF_IP_TERM_INFO                    *term_info,
    SOC_SAND_IN  SOC_PB_PP_RIF_INFO                            *rif_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_rif_ip_tunnel_map_remove_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove the IP Tunnel
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  dip_key -
 *     Destination IP
 *   SOC_SAND_OUT SOC_PB_PP_LIF_ID                              *lif_index -
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
  soc_pb_pp_rif_ip_tunnel_map_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  dip_key,
    SOC_SAND_OUT SOC_PB_PP_LIF_ID                              *lif_index
  );

uint32
  soc_pb_pp_rif_ip_tunnel_map_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  dip_key
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_rif_ip_tunnel_map_get_unsafe
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
 *   SOC_SAND_OUT SOC_PB_PP_LIF_ID                              *lif_index -
 *     Entry in the LIF table
 *   SOC_SAND_OUT SOC_PB_PP_RIF_IP_TERM_INFO                    *term_info -
 *     IP tunnel Termination info including type of termination
 *     RIF-id. The term_info.cos-profile is ignored. Instead
 *     the rif_info.cos-profile is used.
 *   SOC_SAND_OUT SOC_PB_PP_RIF_INFO                            *rif_info -
 *     RIF attributes. RIF ID, ...
 *   SOC_SAND_OUT uint8                                 *found -
 *     Was key found
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_rif_ip_tunnel_map_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  dip_key,
    SOC_SAND_OUT SOC_PB_PP_LIF_ID                              *lif_index,
    SOC_SAND_OUT SOC_PB_PP_RIF_IP_TERM_INFO                    *term_info,
    SOC_SAND_OUT SOC_PB_PP_RIF_INFO                            *rif_info,
    SOC_SAND_OUT uint8                                 *found
  );


uint32
  soc_pb_pp_rif_ip_tunnel_map_get_internal_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  dip_key,
    SOC_SAND_IN  uint8                                 ignore_key,
    SOC_SAND_OUT  SOC_PB_PP_LIF_ID                             *lif_index,
    SOC_SAND_OUT  SOC_PB_PP_RIF_IP_TERM_INFO                   *term_info,
    SOC_SAND_OUT  SOC_PB_PP_RIF_INFO                           *rif_info,
    SOC_SAND_OUT uint8                                 *found
  );


uint32
  soc_pb_pp_rif_ip_tunnel_map_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  dip_key
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_rif_ip_tunnel_map_remove_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove the IP Tunnel
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_VSI_ID                              vsid_ndx -
 *     VSID. Equal to the RIF-ID
 *   SOC_SAND_IN  SOC_PB_PP_RIF_INFO                            *rif_info -
 *     RIF attributes. in this case RIF_ID has to be equal to
 *     vsid_ndx
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_rif_vsid_map_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VSI_ID                              vsid_ndx,
    SOC_SAND_IN  SOC_PB_PP_RIF_INFO                            *rif_info
  );

uint32
  soc_pb_pp_rif_vsid_map_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VSI_ID                              vsid_ndx,
    SOC_SAND_IN  SOC_PB_PP_RIF_INFO                            *rif_info
  );

uint32
  soc_pb_pp_rif_vsid_map_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VSI_ID                              vsid_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_rif_vsid_map_set_unsafe" API.
 *     Refer to "soc_pb_pp_rif_vsid_map_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_rif_vsid_map_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_VSI_ID                              vsid_ndx,
    SOC_SAND_OUT SOC_PB_PP_RIF_INFO                            *rif_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_rif_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the Router Interface according to the VSID.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_RIF_ID                              rif_ndx -
 *     Router Interface ID. Range: 0 - 4K-1.
 *   SOC_SAND_IN  SOC_PB_PP_RIF_INFO                            *rif_info -
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
  soc_pb_pp_rif_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_RIF_ID                              rif_ndx,
    SOC_SAND_IN  SOC_PB_PP_RIF_INFO                            *rif_info
  );

uint32
  soc_pb_pp_rif_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_RIF_ID                              rif_ndx,
    SOC_SAND_IN  SOC_PB_PP_RIF_INFO                            *rif_info
  );

uint32
  soc_pb_pp_rif_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_RIF_ID                              rif_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_rif_info_set_unsafe" API.
 *     Refer to "soc_pb_pp_rif_info_set_unsafe" API for details.
*********************************************************************/
uint32
  soc_pb_pp_rif_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_RIF_ID                              rif_ndx,
    SOC_SAND_OUT SOC_PB_PP_RIF_INFO                            *rif_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_rif_ttl_scope_set_unsafe
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
  soc_pb_pp_rif_ttl_scope_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 ttl_scope_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IP_TTL                            ttl_val
  );

uint32
  soc_pb_pp_rif_ttl_scope_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 ttl_scope_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IP_TTL                            ttl_val
  );

uint32
  soc_pb_pp_rif_ttl_scope_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 ttl_scope_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_rif_ttl_scope_set_unsafe" API.
 *     Refer to "soc_pb_pp_rif_ttl_scope_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_rif_ttl_scope_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 ttl_scope_ndx,
    SOC_SAND_OUT SOC_SAND_PP_IP_TTL                            *ttl_val
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_rif_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   soc_pb_pp_api_rif module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_rif_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   soc_pb_pp_rif_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   soc_pb_pp_api_rif module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_rif_get_errs_ptr(void);

uint32
  SOC_PB_PP_RIF_MPLS_LABELS_RANGE_verify(
    SOC_SAND_IN  SOC_PB_PP_RIF_MPLS_LABELS_RANGE *info
  );

uint32
  SOC_PB_PP_MPLS_LABEL_RIF_KEY_verify(
    SOC_SAND_IN  SOC_PB_PP_MPLS_LABEL_RIF_KEY *info
  );

uint32
  SOC_PB_PP_RIF_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_RIF_INFO *info
  );

uint32
  SOC_PB_PP_RIF_IP_TERM_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_RIF_IP_TERM_INFO *info
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PP_RIF_INCLUDED__*/
#endif

