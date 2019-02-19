/* $Id: arad_pp_eg_mirror.h,v 1.11 Broadcom SDK $
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

#ifndef __ARAD_PP_EG_MIRROR_INCLUDED__
/* { */
#define __ARAD_PP_EG_MIRROR_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_eg_mirror.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define ARAD_PP_EG_MIRROR_NOF_VID_MIRROR_INDICES (7)

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
  ARAD_PP_EG_MIRROR_PORT_VLAN_ADD = ARAD_PP_PROC_DESC_BASE_EG_MIRROR_FIRST,
  ARAD_PP_EG_MIRROR_PORT_VLAN_ADD_PRINT,
  ARAD_PP_EG_MIRROR_PORT_VLAN_ADD_UNSAFE,
  ARAD_PP_EG_MIRROR_PORT_VLAN_ADD_VERIFY,
  ARAD_PP_EG_MIRROR_PORT_VLAN_REMOVE,
  ARAD_PP_EG_MIRROR_PORT_VLAN_REMOVE_PRINT,
  ARAD_PP_EG_MIRROR_PORT_VLAN_REMOVE_UNSAFE,
  ARAD_PP_EG_MIRROR_PORT_VLAN_REMOVE_VERIFY,
  ARAD_PP_EG_MIRROR_PORT_VLAN_GET,
  ARAD_PP_EG_MIRROR_PORT_VLAN_GET_PRINT,
  ARAD_PP_EG_MIRROR_PORT_VLAN_GET_UNSAFE,
  ARAD_PP_EG_MIRROR_PORT_VLAN_GET_VERIFY,
  ARAD_PP_EG_MIRROR_PORT_DFLT_SET,
  ARAD_PP_EG_MIRROR_PORT_DFLT_SET_PRINT,
  ARAD_PP_EG_MIRROR_PORT_DFLT_SET_UNSAFE,
  ARAD_PP_EG_MIRROR_PORT_DFLT_SET_VERIFY,
  ARAD_PP_EG_MIRROR_PORT_DFLT_GET,
  ARAD_PP_EG_MIRROR_PORT_APPL_SET,
  ARAD_PP_EG_MIRROR_PORT_APPL_GET,
  ARAD_PP_EG_MIRROR_PORT_APPL_SET_UNSAFE,
  ARAD_PP_EG_MIRROR_PORT_APPL_GET_UNSAFE,
  ARAD_PP_EG_MIRROR_PORT_DFLT_GET_PRINT,
  ARAD_PP_EG_MIRROR_PORT_DFLT_GET_VERIFY,
  ARAD_PP_EG_MIRROR_PORT_DFLT_GET_UNSAFE,
  ARAD_PP_EG_MIRROR_PORT_INFO_GET,
  ARAD_PP_EG_MIRROR_PORT_INFO_GET_PRINT,
  ARAD_PP_EG_MIRROR_PORT_INFO_GET_VERIFY,
  ARAD_PP_EG_MIRROR_PORT_INFO_GET_UNSAFE,
  ARAD_PP_EG_MIRROR_GET_PROCS_PTR,
  ARAD_PP_EG_MIRROR_GET_ERRS_PTR,
  ARAD_PP_EG_MIRROR_RECYCLE_COMMAND_TRAP_SET,
  ARAD_PP_EG_MIRROR_RECYCLE_COMMAND_TRAP_SET_UNSAFE,
  ARAD_PP_EG_MIRROR_RECYCLE_COMMAND_TRAP_SET_VERIFY,
  ARAD_PP_EG_MIRROR_RECYCLE_COMMAND_TRAP_GET,
  ARAD_PP_EG_MIRROR_RECYCLE_COMMAND_TRAP_GET_UNSAFE,
  ARAD_PP_EG_MIRROR_RECYCLE_COMMAND_TRAP_GET_VERIFY,
  /*
   * } Auto generated. Do not edit previous section.
   */

  /*
   * Last element. Do no touch.
   */
  ARAD_PP_EG_MIRROR_PROCEDURE_DESC_LAST
} ARAD_PP_EG_MIRROR_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  ARAD_PP_EG_MIRROR_ENABLE_MIRROR_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_EG_MIRROR_FIRST,
  ARAD_PP_EG_MIRROR_SUCCESS_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_MIRROR_RECYCLE_COMMAND_OUT_OF_RANGE_ERR,
  ARAD_PP_LLP_MIRROR_TRAP_CODE_OUT_OF_RANGE_ERR,  
  /*
   * } Auto generated. Do not edit previous section.
   */

  /*
   * Last element. Do no touch.
   */
  ARAD_PP_EG_MIRROR_ERR_LAST
} ARAD_PP_EG_MIRROR_ERR;



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
  arad_pp_eg_mirror_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
* NAME:
 *   arad_pp_eg_mirror_port_vlan_add_unsafe
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
 *   SOC_SAND_IN  ARAD_PP_PORT                                out_port_ndx -
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
 *   - Arad-B only. Error is returned if called for T20E.-
 *   The VID considered is the outer VID set by the egress
 *   vlan editing i.e. the outer-VID the packet would has if
 *   the packet transmitted tagged according to
 *   soc_ppd_eg_vlan_edit_port_vlan_transmit_outer_tag_set().-
 *   packet is mirrored only if - out-port x vlan was set
 *   enable mirror OR out-port default to enable mirroring -
 *   AND out-TM-port enables mirroring- The mirrored packet
 *   is mirrored to channel set according to out-TM-port. To
 *   see TM-port configuration see Arad UM document
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_eg_mirror_port_vlan_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  ARAD_PP_PORT                        pp_port,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success,
    SOC_SAND_IN dpp_outbound_mirror_config_t        *config

  );

uint32
  arad_pp_eg_mirror_port_vlan_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_PORT                                out_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  uint8                                 enable_mirror

  );

/*********************************************************************
* NAME:
 *   arad_pp_eg_mirror_port_vlan_remove_unsafe
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
 *   SOC_SAND_IN  ARAD_PP_PORT                                out_port_ndx -
 *     Local port ID.
 *   SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx -
 *     VLAN ID. Range: 0 - 4095.
 * REMARKS:
 *   - Arad-B only. Error is returned if called for T20E.-
 *   The VID considered is the outer VID set by the egress
 *   vlan editing i.e. the outer-VID the packet would has if
 *   the packet transmitted tagged according to
 *   soc_ppd_eg_vlan_edit_port_vlan_transmit_outer_tag_set().
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_eg_mirror_port_vlan_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  ARAD_PP_PORT                                out_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx
  );

uint32
  arad_pp_eg_mirror_port_vlan_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_PORT                                out_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx
  );

/*********************************************************************
* NAME:
 *   arad_pp_eg_mirror_port_vlan_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the assigned mirroring profile for port and VLAN.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_PORT                                out_port_ndx -
 *     Local port ID.
 *   SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx -
 *     VLAN ID. Range: 0 - 4095.
 *   SOC_SAND_OUT uint8                                 *enable_mirror -
 *     Whether mirroring is enabled on this port x vlan
 * REMARKS:
 *   - Arad-B only. Error is returned if called for T20E.-
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
  arad_pp_eg_mirror_port_vlan_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                core_id,
    SOC_SAND_IN  ARAD_PP_PORT                                out_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_OUT dpp_outbound_mirror_config_t          *config
  );

uint32
  arad_pp_eg_mirror_port_vlan_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_PORT                                out_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx
  );

/*********************************************************************
* NAME:
 *   arad_pp_eg_mirror_port_dflt_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set default mirroring profiles for port
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx -
 *     Local port ID.
 *   SOC_SAND_IN  ARAD_PP_EG_MIRROR_PORT_DFLT_INFO            *dflt_mirroring_info -
 *     Port default mirroring profiles for tagged and untagged
 *     packets.
 * REMARKS:
 *   - Arad-B only. Error is returned if called for T20E.-
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
 *   see TM-port configuration see Arad UM document
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_eg_mirror_port_dflt_set_unsafe(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  int                                core_id,
    SOC_SAND_IN  ARAD_PP_PORT                       pp_port,
    SOC_SAND_IN dpp_outbound_mirror_config_t        *config

  );

/*********************************************************************
* NAME:
 *   arad_pp_eg_mirror_port_appl_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Enable or disable mirroring for a port by other (than mirroring) applications.
 * INPUT:
 *   SOC_SAND_IN  int         unit - Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_PORT   local_port_ndx - Local port ID.
 *   SOC_SAND_IN  uint8          enable - 0 will disable, other values will enable.
 * REMARKS:
 *   not supported in Petra
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_eg_mirror_port_appl_set_unsafe(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  ARAD_PP_PORT   local_port, 
    SOC_SAND_IN  uint8         enable
  );

uint32
  arad_pp_eg_mirror_port_dflt_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx
  );

uint32
  arad_pp_eg_mirror_port_dflt_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_eg_mirror_port_dflt_set_unsafe" API.
 *     Refer to "arad_pp_eg_mirror_port_dflt_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  arad_pp_eg_mirror_port_dflt_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx,
    SOC_SAND_OUT dpp_outbound_mirror_config_t        *config
  );

/*********************************************************************
*     Check if mirroring for a port by other (than mirroring) applications is enabled
*********************************************************************/
uint32
  arad_pp_eg_mirror_port_appl_get_unsafe(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  ARAD_PP_PORT  local_port_ndx,
    SOC_SAND_OUT uint8         *is_enabled /* returns 0 if disabled, or a different vale if enabled */
  );

/*********************************************************************
*     Set RECYCLE_COMMAND table with trap code
*********************************************************************/
uint32
  arad_pp_eg_mirror_recycle_command_trap_set_unsafe(
    SOC_SAND_IN  int        unit,      /* Identifier of the device to access */
    SOC_SAND_IN  uint32        recycle_command, /* Equal to mirror profile */
    SOC_SAND_IN  uint32        trap_code, /* PPD - not HW code */
    SOC_SAND_IN  uint32        snoop_strength,
    SOC_SAND_IN  uint32        forward_strengh
  );

uint32
  arad_pp_eg_mirror_recycle_command_trap_set_verify(
    SOC_SAND_IN  int        unit,      /* Identifier of the device to access */
    SOC_SAND_IN  uint32        recycle_command, /* Equal to mirror profile */
    SOC_SAND_IN  uint32        trap_code, /* PPD - not HW code */
    SOC_SAND_IN  uint32        snoop_strength,
    SOC_SAND_IN  uint32        forward_strengh
  );

uint32
  arad_pp_eg_mirror_recycle_command_trap_get_unsafe(
    SOC_SAND_IN  int        unit,      /* Identifier of the device to access */
    SOC_SAND_IN  uint32        recycle_command, /* Equal to mirror profile */
    SOC_SAND_OUT  uint32       *trap_code, /* PPD - not HW code */
    SOC_SAND_OUT  uint32       *snoop_strength
  );

uint32
  arad_pp_eg_mirror_recycle_command_trap_get_verify(
    SOC_SAND_IN  int        unit,      /* Identifier of the device to access */
    SOC_SAND_IN  uint32        recycle_command /* Equal to mirror profile */
  );

uint32
  arad_pp_eg_mirror_port_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx
  );

/*********************************************************************
*     Gets the port information. 
*********************************************************************/
uint32
  arad_pp_eg_mirror_port_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  ARAD_PP_PORT                        pp_port,
    SOC_SAND_OUT ARAD_PP_EG_MIRROR_PORT_INFO            *info
  );



uint32
  ARAD_PP_EG_MIRROR_PORT_DFLT_INFO_verify(
    SOC_SAND_IN  ARAD_PP_EG_MIRROR_PORT_DFLT_INFO *info
  );

uint32
  ARAD_PP_EG_MIRROR_PORT_INFO_verify(
    SOC_SAND_IN  ARAD_PP_EG_MIRROR_PORT_INFO *info
  );

/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_PP_EG_MIRROR_INCLUDED__*/
#endif
