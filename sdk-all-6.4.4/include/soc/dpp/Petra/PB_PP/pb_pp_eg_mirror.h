/* $Id: pb_pp_eg_mirror.h,v 1.7 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/include/soc_pb_pp_eg_mirror.h
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

#ifndef __SOC_PB_PP_EG_MIRROR_INCLUDED__
/* { */
#define __SOC_PB_PP_EG_MIRROR_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_api_eg_mirror.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_EG_MIRROR_NOF_VID_MIRROR_INDICES (7)

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
  SOC_PB_PP_EG_MIRROR_PORT_VLAN_ADD = SOC_PB_PP_PROC_DESC_BASE_EG_MIRROR_FIRST,
  SOC_PB_PP_EG_MIRROR_PORT_VLAN_ADD_PRINT,
  SOC_PB_PP_EG_MIRROR_PORT_VLAN_ADD_UNSAFE,
  SOC_PB_PP_EG_MIRROR_PORT_VLAN_ADD_VERIFY,
  SOC_PB_PP_EG_MIRROR_PORT_VLAN_REMOVE,
  SOC_PB_PP_EG_MIRROR_PORT_VLAN_REMOVE_PRINT,
  SOC_PB_PP_EG_MIRROR_PORT_VLAN_REMOVE_UNSAFE,
  SOC_PB_PP_EG_MIRROR_PORT_VLAN_REMOVE_VERIFY,
  SOC_PB_PP_EG_MIRROR_PORT_VLAN_GET,
  SOC_PB_PP_EG_MIRROR_PORT_VLAN_GET_PRINT,
  SOC_PB_PP_EG_MIRROR_PORT_VLAN_GET_UNSAFE,
  SOC_PB_PP_EG_MIRROR_PORT_VLAN_GET_VERIFY,
  SOC_PB_PP_EG_MIRROR_PORT_DFLT_SET,
  SOC_PB_PP_EG_MIRROR_PORT_DFLT_SET_PRINT,
  SOC_PB_PP_EG_MIRROR_PORT_DFLT_SET_UNSAFE,
  SOC_PB_PP_EG_MIRROR_PORT_DFLT_SET_VERIFY,
  SOC_PB_PP_EG_MIRROR_PORT_DFLT_GET,
  SOC_PB_PP_EG_MIRROR_PORT_DFLT_GET_PRINT,
  SOC_PB_PP_EG_MIRROR_PORT_DFLT_GET_VERIFY,
  SOC_PB_PP_EG_MIRROR_PORT_DFLT_GET_UNSAFE,
  SOC_PB_PP_EG_MIRROR_GET_PROCS_PTR,
  SOC_PB_PP_EG_MIRROR_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */

  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_EG_MIRROR_PROCEDURE_DESC_LAST
} SOC_PB_PP_EG_MIRROR_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PB_PP_EG_MIRROR_ENABLE_MIRROR_OUT_OF_RANGE_ERR = SOC_PB_PP_ERR_DESC_BASE_EG_MIRROR_FIRST,
  SOC_PB_PP_EG_MIRROR_SUCCESS_OUT_OF_RANGE_ERR,
  /*
   * } Auto generated. Do not edit previous section.
   */

  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_EG_MIRROR_ERR_LAST
} SOC_PB_PP_EG_MIRROR_ERR;

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
  soc_pb_pp_eg_mirror_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_mirror_port_vlan_add_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set outbound mirroring for out-port and VLAN, so all
 *   outgoing packets leave from the given port and with the
 *   given VID will be mirrored or not according to
 *   'enable_mirror'
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_PORT                                out_port_ndx -
 *     Local port ID.
 *   SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx -
 *     VLAN ID. Range: 0 - 4095.
 *   SOC_SAND_IN  uint8                                 enable_mirror -
 *     TRUE packets will be mirrored. FALSE packet will not be
 *     mirrored
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success -
 *     Whether the operation succeeds (upon add). Add operation
 *     may fail if there are no sufficient resources.
 *     Successful add is guaranteed for up to 7 different VIDs.
 * REMARKS:
 *   - Soc_petra-B only. Error is returned if called for T20E.-
 *   The VID considered is the outer VID set by the egress
 *   vlan editing i.e. the outer-VID the packet would has if
 *   the packet transmitted tagged according to
 *   soc_ppd_eg_vlan_edit_port_vlan_transmit_outer_tag_set().-
 *   packet is mirrored only if - out-port x vlan was set
 *   enable mirror OR out-port default to enable mirroring -
 *   AND out-TM-port enables mirroring- The mirrored packet
 *   is mirrored to channel set according to out-TM-port. To
 *   see TM-port configuration see Soc_petra UM document
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_eg_mirror_port_vlan_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                out_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success,
    SOC_SAND_IN  dpp_outbound_mirror_config_t        *config

  );

uint32
  soc_pb_pp_eg_mirror_port_vlan_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                out_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  dpp_outbound_mirror_config_t        *config

  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_mirror_port_vlan_remove_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove a mirroring for port and VLAN, upon this packet
 *   transmitted out this out_port_ndx and vid_ndx will be
 *   mirrored or not according to default configuration for
 *   out_port_ndx. see soc_ppd_eg_mirror_port_dflt_set()
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_PORT                                out_port_ndx -
 *     Local port ID.
 *   SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx -
 *     VLAN ID. Range: 0 - 4095.
 * REMARKS:
 *   - Soc_petra-B only. Error is returned if called for T20E.-
 *   The VID considered is the outer VID set by the egress
 *   vlan editing i.e. the outer-VID the packet would has if
 *   the packet transmitted tagged according to
 *   soc_ppd_eg_vlan_edit_port_vlan_transmit_outer_tag_set().
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_eg_mirror_port_vlan_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                out_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx
  );

uint32
  soc_pb_pp_eg_mirror_port_vlan_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                out_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_mirror_port_vlan_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the assigned mirroring profile for port and VLAN.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_PORT                                out_port_ndx -
 *     Local port ID.
 *   SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx -
 *     VLAN ID. Range: 0 - 4095.
 *   SOC_SAND_OUT uint8                                 *enable_mirror -
 *     Whether mirroring is enabled on this port x vlan
 * REMARKS:
 *   - Soc_petra-B only. Error is returned if called for T20E.-
 *   The VID considered is the outer VID set by the egress
 *   vlan editing i.e. the outer-VID the packet would has if
 *   the packet transmitted tagged according to
 *   soc_ppd_eg_vlan_edit_port_vlan_transmit_outer_tag_set().- If
 *   no mirror profile was associated to the Port and VLAN
 *   (by soc_ppd_eg_port_vlan_mirror_add()), the default is
 *   returned according to soc_ppd_eg_mirror_port_dflt_set().
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_eg_mirror_port_vlan_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                out_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_OUT uint8                                 *enable_mirror
  );

uint32
  soc_pb_pp_eg_mirror_port_vlan_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                out_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_mirror_port_dflt_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set default mirroring profiles for port
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx -
 *     Local port ID.
 *   SOC_SAND_IN  SOC_PB_PP_EG_MIRROR_PORT_DFLT_INFO            *dflt_mirroring_info -
 *     Port default mirroring profiles for tagged and untagged
 *     packets.
 * REMARKS:
 *   - Soc_petra-B only. Error is returned if called for T20E.-
 *   To assign a specific mirror profile for port x vlan use
 *   soc_ppd_eg_port_vlan_mirror_add().- The VID considered is
 *   the outer VID set by the egress vlan editing i.e. the
 *   outer-VID the packet would has if the packet transmitted
 *   tagged according to
 *   soc_ppd_eg_vlan_edit_port_vlan_transmit_outer_tag_set().-
 *   packet is mirrored only if - out-port x vlan was set
 *   enable mirror OR out-port default to enable mirroring -
 *   AND out-TM-port enables mirroring- The mirrored packet
 *   is mirrored to channel set according to out-TM-port. To
 *   see TM-port configuration see Soc_petra UM document
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_eg_mirror_port_dflt_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_MIRROR_PORT_DFLT_INFO            *dflt_mirroring_info
  );

uint32
  soc_pb_pp_eg_mirror_port_dflt_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx
  );

uint32
  soc_pb_pp_eg_mirror_port_dflt_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_eg_mirror_port_dflt_set_unsafe" API.
 *     Refer to "soc_pb_pp_eg_mirror_port_dflt_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_eg_mirror_port_dflt_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_OUT dpp_outbound_mirror_config_t            *config
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_mirror_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   soc_pb_pp_api_eg_mirror module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_eg_mirror_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_mirror_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   soc_pb_pp_api_eg_mirror module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_eg_mirror_get_errs_ptr(void);

uint32
  SOC_PB_PP_EG_MIRROR_PORT_DFLT_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_EG_MIRROR_PORT_DFLT_INFO *info
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PP_EG_MIRROR_INCLUDED__*/
#endif

