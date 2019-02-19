/* $Id: arad_pp_api_llp_mirror.h,v 1.3 Broadcom SDK $
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

#ifndef __ARAD_PP_API_LLP_MIRROR_INCLUDED__
/* { */
#define __ARAD_PP_API_LLP_MIRROR_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_llp_mirror.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_general.h>

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

typedef SOC_PPC_LLP_MIRROR_PORT_DFLT_INFO                      ARAD_PP_LLP_MIRROR_PORT_DFLT_INFO;

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
 *   arad_pp_llp_mirror_port_vlan_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set a mirroring for port and VLAN, so all incoming
 *   packets enter from the given port and identified with
 *   the given VID will be associated with Mirror command
 *   (Enables mirroring (copying) the packets to additional
 *   destination.
 * INPUT:
 *   SOC_SAND_IN  int                                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx -
 *     Local port ID.
 *   SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx -
 *     VLAN ID. Range: 0 - 4095.
 *   SOC_SAND_IN  uint32                                  mirror_profile -
 *     Mirroring profile to associate with packets, the
 *     resolution of this mirroring occurs in Arad-TM device.
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
 *   assigned more than one mirror profile then: - in Arad:
 *   last assignment is considered. - in T20E: the
 *   snoop/mirror command with highest strength is
 *   considered.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_llp_mirror_port_vlan_add(
    SOC_SAND_IN  int                                 unit,
	SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  uint32                                  mirror_profile,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

/*********************************************************************
* NAME:
 *   arad_pp_llp_mirror_port_vlan_remove
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove a mirroring for port and VLAN
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx -
 *     Local port ID.
 *   SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx -
 *     VLAN ID. Range: 0 - 4095.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_llp_mirror_port_vlan_remove(
    SOC_SAND_IN  int                                 unit,
	SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx
  );

/*********************************************************************
* NAME:
 *   arad_pp_llp_mirror_port_vlan_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the assigned mirroring profile for port and VLAN.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx -
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
  arad_pp_llp_mirror_port_vlan_get(
    SOC_SAND_IN  int                                 unit,
	SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_OUT uint32                                  *mirror_profile
  );

/*********************************************************************
* NAME:
 *   arad_pp_llp_mirror_port_dflt_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set default mirroring profiles for port
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx -
 *     Local port ID.
 *   SOC_SAND_IN  ARAD_PP_LLP_MIRROR_PORT_DFLT_INFO           *dflt_mirroring_info -
 *     Port default mirroring profiles for tagged and untagged
 *     packets.
 * REMARKS:
 *   - To assign a specific mirror profile for port x vlan
 *   use soc_ppd_llp_trap_port_vlan_mirroring_add().
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_llp_mirror_port_dflt_set(
    SOC_SAND_IN  int                                 unit,
	SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  ARAD_PP_LLP_MIRROR_PORT_DFLT_INFO           *dflt_mirroring_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_llp_mirror_port_dflt_set" API.
 *     Refer to "arad_pp_llp_mirror_port_dflt_set" API for
 *     details.
*********************************************************************/
uint32
  arad_pp_llp_mirror_port_dflt_get(
    SOC_SAND_IN  int                                 unit,
	SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx,
    SOC_SAND_OUT ARAD_PP_LLP_MIRROR_PORT_DFLT_INFO           *dflt_mirroring_info
  );

void
  ARAD_PP_LLP_MIRROR_PORT_DFLT_INFO_clear(
    SOC_SAND_OUT ARAD_PP_LLP_MIRROR_PORT_DFLT_INFO *info
  );

#if ARAD_PP_DEBUG_IS_LVL1

void
  ARAD_PP_LLP_MIRROR_PORT_DFLT_INFO_print(
    SOC_SAND_IN  ARAD_PP_LLP_MIRROR_PORT_DFLT_INFO *info
  );

#endif /* ARAD_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_PP_API_LLP_MIRROR_INCLUDED__*/
#endif

