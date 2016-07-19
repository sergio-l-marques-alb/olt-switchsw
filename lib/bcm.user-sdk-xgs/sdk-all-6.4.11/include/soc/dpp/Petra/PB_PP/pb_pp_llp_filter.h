/* $Id: pb_pp_llp_filter.h,v 1.7 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/include/soc_pb_pp_llp_filter.h
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

#ifndef __SOC_PB_PP_LLP_FILTER_INCLUDED__
/* { */
#define __SOC_PB_PP_LLP_FILTER_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_api_llp_filter.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_LLP_FILTER_DESIGNATED_VLAN_TABLE_SIZE                (8)

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
  SOC_PB_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_SET = SOC_PB_PP_PROC_DESC_BASE_LLP_FILTER_FIRST,
  SOC_PB_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_SET_PRINT,
  SOC_PB_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_SET_UNSAFE,
  SOC_PB_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_SET_VERIFY,
  SOC_PB_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_GET,
  SOC_PB_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_GET_PRINT,
  SOC_PB_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_GET_VERIFY,
  SOC_PB_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_GET_UNSAFE,
  SOC_PB_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_ADD,
  SOC_PB_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_ADD_PRINT,
  SOC_PB_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_ADD_UNSAFE,
  SOC_PB_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_ADD_VERIFY,
  SOC_PB_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_REMOVE,
  SOC_PB_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_REMOVE_PRINT,
  SOC_PB_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_REMOVE_UNSAFE,
  SOC_PB_PP_LLP_FILTER_INGRESS_VLAN_MEMBERSHIP_PORT_REMOVE_VERIFY,
  SOC_PB_PP_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_SET,
  SOC_PB_PP_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_SET_PRINT,
  SOC_PB_PP_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_SET_UNSAFE,
  SOC_PB_PP_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_SET_VERIFY,
  SOC_PB_PP_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_GET,
  SOC_PB_PP_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_GET_PRINT,
  SOC_PB_PP_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_GET_VERIFY,
  SOC_PB_PP_LLP_FILTER_INGRESS_ACCEPTABLE_FRAMES_GET_UNSAFE,
  SOC_PB_PP_LLP_FILTER_DESIGNATED_VLAN_SET,
  SOC_PB_PP_LLP_FILTER_DESIGNATED_VLAN_SET_PRINT,
  SOC_PB_PP_LLP_FILTER_DESIGNATED_VLAN_SET_UNSAFE,
  SOC_PB_PP_LLP_FILTER_DESIGNATED_VLAN_SET_VERIFY,
  SOC_PB_PP_LLP_FILTER_DESIGNATED_VLAN_GET,
  SOC_PB_PP_LLP_FILTER_DESIGNATED_VLAN_GET_PRINT,
  SOC_PB_PP_LLP_FILTER_DESIGNATED_VLAN_GET_VERIFY,
  SOC_PB_PP_LLP_FILTER_DESIGNATED_VLAN_GET_UNSAFE,
  SOC_PB_PP_LLP_FILTER_GET_PROCS_PTR,
  SOC_PB_PP_LLP_FILTER_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */

  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_LLP_FILTER_PROCEDURE_DESC_LAST
} SOC_PB_PP_LLP_FILTER_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PB_PP_LLP_FILTER_PORT_PROFILE_NDX_OUT_OF_RANGE_ERR = SOC_PB_PP_ERR_DESC_BASE_LLP_FILTER_FIRST,
  SOC_PB_PP_LLP_FILTER_VLAN_FORMAT_NDX_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LLP_FILTER_SUCCESS_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LLP_FILTER_ACCEPT_OUT_OF_RANGE_ERR,
  /*
   * } Auto generated. Do not edit previous section.
   */

  SOC_PB_PP_PORTS_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LLP_UNEXPECTED_TAG_FORMAT_VLAN_FORMAT_ERR,
  SOC_PB_PP_FILTER_VID_NOT_DESIGNATED_ERR,

  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_LLP_FILTER_ERR_LAST
} SOC_PB_PP_LLP_FILTER_ERR;

typedef struct
{
  uint32 arr[SOC_PB_PP_LLP_FILTER_DESIGNATED_VLAN_TABLE_SIZE];
} SOC_PB_PP_LLP_FILTER_DESIGNATED_VLAN_TABLE_REF_COUNT;

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
  soc_pb_pp_llp_filter_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_llp_filter_ingress_vlan_membership_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets ingress VLAN membership; which incoming local ports
 *   belong to the VLAN. Packets received on a port that is
 *   not a member of the VLAN the packet is classified to be
 *   filtered.
 * INPUT:
 *   SOC_SAND_IN  int                                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx -
 *     VLAN ID to set the membership for. Range: 0 - 4095.
 *   SOC_SAND_IN  SOC_PB_PP_PORT                                ports -
 *     SOC_PPD_VLAN_MEMBERSHIP_BITMAP_SIZE] - The VLAN membership:
 *     bitmap of VLAN member ports specific information, each
 *     member occupied one bit. Setting bit to 1 indicates port
 *     is member in the VLAN.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_llp_filter_ingress_vlan_membership_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  uint32                                  ports[SOC_PB_PP_VLAN_MEMBERSHIP_BITMAP_SIZE]
  );

uint32
  soc_pb_pp_llp_filter_ingress_vlan_membership_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  uint32                                  ports[SOC_PB_PP_VLAN_MEMBERSHIP_BITMAP_SIZE]
  );

uint32
  soc_pb_pp_llp_filter_ingress_vlan_membership_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                             vid_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_llp_filter_ingress_vlan_membership_set_unsafe"
 *     API.
 *     Refer to
 *     "soc_pb_pp_llp_filter_ingress_vlan_membership_set_unsafe"
 *     API for details.
*********************************************************************/
uint32
  soc_pb_pp_llp_filter_ingress_vlan_membership_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_OUT uint32                                  ports[SOC_PB_PP_VLAN_MEMBERSHIP_BITMAP_SIZE]
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_llp_filter_ingress_vlan_membership_port_add_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add a local port as a member in a VLAN.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx -
 *     VLAN ID. Range: 0 - 4095.
 *   SOC_SAND_IN  SOC_PB_PP_PORT                                local_port -
 *     the local port to add to the VLAN.
 * REMARKS:
 *   - If the local_port is already member of the vlan
 *   vid_ndx, then the function has no effect.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_llp_filter_ingress_vlan_membership_port_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port
  );

uint32
  soc_pb_pp_llp_filter_ingress_vlan_membership_port_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_llp_filter_ingress_vlan_membership_port_remove_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove a local port from the VLAN membership.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx -
 *     VLAN ID. Range: 0 - 4095.
 *   SOC_SAND_IN  SOC_PB_PP_PORT                                local_port -
 *     The local port to add to the VLAN.
 * REMARKS:
 *   - If the local_port is not a member of the vlan vid_ndx,
 *   then the function has no effect.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_llp_filter_ingress_vlan_membership_port_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port
  );

uint32
  soc_pb_pp_llp_filter_ingress_vlan_membership_port_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_llp_filter_ingress_acceptable_frames_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets acceptable frame type on incoming port.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  port_profile_ndx -
 *     Port Profile ID. Range 0 - 7. Set by
 *     soc_ppd_port_info_set().
 *   SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT        vlan_format_ndx -
 *     Acceptable frame types (S-tag, double Tags etc.). Use
 *     SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_ANY to affect the
 *     status of all possible vlan Tag formats.
 *   SOC_SAND_IN  SOC_PB_PP_ACTION_PROFILE                      *action_profile -
 *     Action profile according to which to
 *     process/drop/forward the packet.trap_code range:
 *     0-3.the CPU code attached with the packet is
 *     SOC_PPD_TRAP_CODE_ACCEPTABLE_FRAME_TYPE_0/1/2/3
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success -
 *     Whether the operation succeeds (upon add). Operation may
 *     fail if there are no available action profiles pointers.
 * REMARKS:
 *   - use SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_ANY as
 *   vlan_format_ndx to make a decision for ALL VLAN formats
 *   at once.- To set mapping from port to port profile use:
 *   1. soc_ppd_port_info_set(local_port_ndx, port_profile)
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_llp_filter_ingress_acceptable_frames_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT        vlan_format_ndx,
    SOC_SAND_IN  SOC_PB_PP_ACTION_PROFILE                      *action_profile,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  soc_pb_pp_llp_filter_ingress_acceptable_frames_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT        vlan_format_ndx,
    SOC_SAND_IN  SOC_PB_PP_ACTION_PROFILE                      *action_profile
  );

uint32
  soc_pb_pp_llp_filter_ingress_acceptable_frames_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT        vlan_format_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_llp_filter_ingress_acceptable_frames_set_unsafe"
 *     API.
 *     Refer to
 *     "soc_pb_pp_llp_filter_ingress_acceptable_frames_set_unsafe"
 *     API for details.
*********************************************************************/
uint32
  soc_pb_pp_llp_filter_ingress_acceptable_frames_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT        vlan_format_ndx,
    SOC_SAND_OUT SOC_PB_PP_ACTION_PROFILE                      *action_profile
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_llp_filter_designated_vlan_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set a designated VLAN for a port. Incoming Trill packet
 *   will be checked if it has this T-VID; otherwise, packet
 *   will be dropped.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx -
 *     Local port ID.
 *   SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid -
 *     VID to set as designated VLAN of the Port. Range:
 *     0-4095.
 *   SOC_SAND_IN  uint8                                 accept -
 *     Accept or deny this frame type for this port type
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success -
 *     Whether the set operation succeeded. Operation may fail
 *     if there are no available resources to support the given
 *     (new) VID.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_llp_filter_designated_vlan_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid,
    SOC_SAND_IN  uint8                                 accept,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  soc_pb_pp_llp_filter_designated_vlan_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid,
    SOC_SAND_IN  uint8                                 accept
  );

uint32
  soc_pb_pp_llp_filter_designated_vlan_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_llp_filter_designated_vlan_set_unsafe" API.
 *     Refer to "soc_pb_pp_llp_filter_designated_vlan_set_unsafe"
 *     API for details.
*********************************************************************/
uint32
  soc_pb_pp_llp_filter_designated_vlan_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_SAND_PP_VLAN_ID                           *vid,
    SOC_SAND_OUT uint8                                 *accept
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_llp_filter_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   soc_pb_pp_api_llp_filter module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_llp_filter_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   soc_pb_pp_llp_filter_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   soc_pb_pp_api_llp_filter module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_llp_filter_get_errs_ptr(void);

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PP_LLP_FILTER_INCLUDED__*/
#endif

