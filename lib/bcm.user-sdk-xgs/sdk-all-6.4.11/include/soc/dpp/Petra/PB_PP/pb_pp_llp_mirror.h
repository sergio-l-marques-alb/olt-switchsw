/* $Id: pb_pp_llp_mirror.h,v 1.7 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/include/soc_pb_pp_llp_mirror.h
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

#ifndef __SOC_PB_PP_LLP_MIRROR_INCLUDED__
/* { */
#define __SOC_PB_PP_LLP_MIRROR_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_api_llp_mirror.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_LLP_MIRROR_NOF_VID_MIRROR_INDICES (6)

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
  SOC_PB_PP_LLP_MIRROR_PORT_VLAN_ADD = SOC_PB_PP_PROC_DESC_BASE_LLP_MIRROR_FIRST,
  SOC_PB_PP_LLP_MIRROR_PORT_VLAN_ADD_PRINT,
  SOC_PB_PP_LLP_MIRROR_PORT_VLAN_ADD_UNSAFE,
  SOC_PB_PP_LLP_MIRROR_PORT_VLAN_ADD_VERIFY,
  SOC_PB_PP_LLP_MIRROR_PORT_VLAN_REMOVE,
  SOC_PB_PP_LLP_MIRROR_PORT_VLAN_REMOVE_PRINT,
  SOC_PB_PP_LLP_MIRROR_PORT_VLAN_REMOVE_UNSAFE,
  SOC_PB_PP_LLP_MIRROR_PORT_VLAN_REMOVE_VERIFY,
  SOC_PB_PP_LLP_MIRROR_PORT_VLAN_GET,
  SOC_PB_PP_LLP_MIRROR_PORT_VLAN_GET_PRINT,
  SOC_PB_PP_LLP_MIRROR_PORT_VLAN_GET_UNSAFE,
  SOC_PB_PP_LLP_MIRROR_PORT_VLAN_GET_VERIFY,
  SOC_PB_PP_LLP_MIRROR_PORT_DFLT_SET,
  SOC_PB_PP_LLP_MIRROR_PORT_DFLT_SET_PRINT,
  SOC_PB_PP_LLP_MIRROR_PORT_DFLT_SET_UNSAFE,
  SOC_PB_PP_LLP_MIRROR_PORT_DFLT_SET_VERIFY,
  SOC_PB_PP_LLP_MIRROR_PORT_DFLT_GET,
  SOC_PB_PP_LLP_MIRROR_PORT_DFLT_GET_PRINT,
  SOC_PB_PP_LLP_MIRROR_PORT_DFLT_GET_VERIFY,
  SOC_PB_PP_LLP_MIRROR_PORT_DFLT_GET_UNSAFE,
  SOC_PB_PP_LLP_MIRROR_GET_PROCS_PTR,
  SOC_PB_PP_LLP_MIRROR_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */

  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_LLP_MIRROR_PROCEDURE_DESC_LAST
} SOC_PB_PP_LLP_MIRROR_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PB_PP_LLP_MIRROR_MIRROR_PROFILE_OUT_OF_RANGE_ERR = SOC_PB_PP_ERR_DESC_BASE_LLP_MIRROR_FIRST,
  SOC_PB_PP_LLP_MIRROR_SUCCESS_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LLP_MIRROR_TAGGED_DFLT_OUT_OF_RANGE_ERR,
  SOC_PB_PP_LLP_MIRROR_UNTAGGED_DFLT_OUT_OF_RANGE_ERR,
  /*
   * } Auto generated. Do not edit previous section.
   */

  SOC_PB_PP_VID_NDX_OUT_OF_RANGE_NO_ZERO_ERR,

  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_LLP_MIRROR_ERR_LAST
} SOC_PB_PP_LLP_MIRROR_ERR;

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
  soc_pb_pp_llp_mirror_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_llp_mirror_port_vlan_add_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set a mirroring for port and VLAN, so all incoming
 *   packets enter from the given port and identified with
 *   the given VID will be associated with Mirror command
 *   (Enables mirroring (copying) the packets to additional
 *   destination.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx -
 *     Local port ID.
 *   SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx -
 *     VLAN ID. Range: 0 - 4095.
 *   SOC_SAND_IN  uint32                                  mirror_profile -
 *     Mirroring profile to associate with packets, the
 *     resolution of this mirroring occurs in Soc_petra-TM device.
 *     Range 0-15.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success -
 *     Whether the operation succeeds (upon add). Add operation
 *     may fail if there are no suffiecient resources.
 *     Successful add is guaranteed for up to 6 different VIDs.
 * REMARKS:
 *   - The VID considered is the VID identified as a result
 *   of the ingress parsing, i.e., according to the tag
 *   structure of the incoming packet (and not according to
 *   the initial VID assignment), - In T20E, the mirror
 *   profile is identical to snoop profile. And consequnently
 *   the Snoop command/Profile is transmited to the TM
 *   device.- If during packet processing, the packet was
 *   assigned more than one mirror profile then: - in Soc_petra:
 *   last assignment is considered. - in T20E: the
 *   snoop/mirror command with highest strength is
 *   considered.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_llp_mirror_port_vlan_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  uint32                                  mirror_profile,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  soc_pb_pp_llp_mirror_port_vlan_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  uint32                                  mirror_profile
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_llp_mirror_port_vlan_remove_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove a mirroring for port and VLAN
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx -
 *     Local port ID.
 *   SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx -
 *     VLAN ID. Range: 0 - 4095.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_llp_mirror_port_vlan_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx
  );

uint32
  soc_pb_pp_llp_mirror_port_vlan_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_llp_mirror_port_vlan_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the assigned mirroring profile for port and VLAN.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx -
 *     Local port ID.
 *   SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx -
 *     VLAN ID. Range: 0 - 4095.
 *   SOC_SAND_OUT uint32                                  *mirror_profile -
 *     Mirroring profile associated with the port and VLAN.
 * REMARKS:
 *   - If no mirror profile was associated to the Port and
 *   VLAN (by soc_ppd_llp_trap_port_vlan_mirroring_add()), the
 *   default mirror profile is returned according to
 *   soc_ppd_llp_mirror_port_dflt_set().
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_llp_mirror_port_vlan_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_OUT uint32                                  *mirror_profile
  );

uint32
  soc_pb_pp_llp_mirror_port_vlan_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_llp_mirror_port_dflt_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set default mirroring profiles for port
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx -
 *     Local port ID.
 *   SOC_SAND_IN  SOC_PB_PP_LLP_MIRROR_PORT_DFLT_INFO           *dflt_mirroring_info -
 *     Port default mirroring profiles for tagged and untagged
 *     packets.
 * REMARKS:
 *   - To assign a specific mirror profile for port x vlan
 *   use soc_ppd_llp_trap_port_vlan_mirroring_add().
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_llp_mirror_port_dflt_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_MIRROR_PORT_DFLT_INFO           *dflt_mirroring_info
  );

uint32
  soc_pb_pp_llp_mirror_port_dflt_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_MIRROR_PORT_DFLT_INFO           *dflt_mirroring_info
  );

uint32
  soc_pb_pp_llp_mirror_port_dflt_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_llp_mirror_port_dflt_set_unsafe" API.
 *     Refer to "soc_pb_pp_llp_mirror_port_dflt_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_llp_mirror_port_dflt_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_PB_PP_LLP_MIRROR_PORT_DFLT_INFO           *dflt_mirroring_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_llp_mirror_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   soc_pb_pp_api_llp_mirror module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_llp_mirror_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   soc_pb_pp_llp_mirror_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   soc_pb_pp_api_llp_mirror module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_llp_mirror_get_errs_ptr(void);

uint32
  SOC_PB_PP_LLP_MIRROR_PORT_DFLT_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_LLP_MIRROR_PORT_DFLT_INFO *info
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PP_LLP_MIRROR_INCLUDED__*/
#endif

