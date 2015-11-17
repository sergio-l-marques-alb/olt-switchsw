/* $Id: arad_pp_api_port.h,v 1.13 Broadcom SDK $
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

#ifndef __ARAD_PP_API_PORT_INCLUDED__
/* { */
#define __ARAD_PP_API_PORT_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_port.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_general.h>

typedef SOC_PPC_PORT_DIRECTION                                 ARAD_PP_PORT_DIRECTION;
#define ARAD_PP_PORT_STP_STATE_BLOCK                         SOC_PPC_PORT_STP_STATE_BLOCK
#define ARAD_PP_PORT_STP_STATE_LEARN                         SOC_PPC_PORT_STP_STATE_LEARN
#define ARAD_PP_PORT_STP_STATE_FORWARD                       SOC_PPC_PORT_STP_STATE_FORWARD
#define ARAD_PP_NOF_PORT_STP_STATES                          SOC_PPC_NOF_PORT_STP_STATES

typedef SOC_PPC_PORT_STP_STATE                                 ARAD_PP_PORT_STP_STATE;
typedef SOC_PPC_PORT_INFO                                      ARAD_PP_PORT_INFO;

#define ARAD_PP_PORT_DEFINED_VT_PROFILE_DEFAULT                SOC_PPC_PORT_DEFINED_VT_PROFILE_DEFAULT
#define ARAD_PP_PORT_DEFINED_VT_PROFILE_FRR_COUPLING           SOC_PPC_PORT_DEFINED_VT_PROFILE_FRR_COUPLING
#define ARAD_PP_PORT_DEFINED_VT_PROFILE_EXPLICIT_NULL          SOC_PPC_PORT_DEFINED_VT_PROFILE_EXPLICIT_NULL
#define ARAD_PP_PORT_DEFINED_VT_PROFILE_FRR_COUPLING_USE_INITIAL_VID SOC_PPC_PORT_DEFINED_VT_PROFILE_FRR_COUPLING_USE_INITIAL_VID
#define ARAD_PP_PORT_DEFINED_VT_PROFILE_USE_INITIAL_VID        SOC_PPC_PORT_DEFINED_VT_PROFILE_USE_INITIAL_VID
#define ARAD_PP_PORT_DEFINED_VT_PROFILE_DOUBLE_TAG_PRIORITY    SOC_PPC_PORT_DEFINED_VT_PROFILE_DOUBLE_TAG_PRIORITY
#define ARAD_PP_PORT_DEFINED_VT_PROFILE_TRILL                  SOC_PPC_PORT_DEFINED_VT_PROFILE_TRILL
#define ARAD_PP_PORT_DEFINED_VT_PROFILE_TRILL_USE_INITIAL_VID  SOC_PPC_PORT_DEFINED_VT_PROFILE_TRILL_USE_INITIAL_VID
#define ARAD_PP_PORT_DEFINED_VT_PROFILE_PON_DEFAULT            SOC_PPC_PORT_DEFINED_VT_PROFILE_PON_DEFAULT
#define ARAD_PP_PORT_DEFINED_VT_PROFILE_PON_TUNNEL_ID          SOC_PPC_PORT_DEFINED_VT_PROFILE_PON_TUNNEL_ID
#define ARAD_PP_PORT_DEFINED_VT_PROFILE_IGNORE_2ND_TAG         SOC_PPC_PORT_DEFINED_VT_PROFILE_IGNORE_2ND_TAG
#define ARAD_PP_PORT_DEFINED_VT_PROFILE_EVB                    SOC_PPC_PORT_DEFINED_VT_PROFILE_EVB
#define ARAD_PP_PORT_DEFINED_VT_PROFILE_DOUBLE_TAG_PRIORITY_INITIAL_VID SOC_PPC_PORT_DEFINED_VT_PROFILE_DOUBLE_TAG_PRIORITY_INITIAL_VID
#define ARAD_PP_NOF_PORT_DEFINED_VT_PROFILES                   SOC_PPC_NOF_PORT_DEFINED_VT_PROFILES
typedef SOC_PPC_PORT_DEFINED_VT_PROFILE                        ARAD_PP_PORT_DEFINED_VT_PROFILE;

#define ARAD_PP_PORT_DEFINED_TT_PROFILE_DEFAULT                SOC_PPC_PORT_DEFINED_TT_PROFILE_DEFAULT
#define ARAD_PP_PORT_DEFINED_TT_PROFILE_FRR_COUPLING           SOC_PPC_PORT_DEFINED_TT_PROFILE_FRR_COUPLING
#define SOC_PPD_NOF_PORT_DEFINED_TT_PROFILES                   SOC_PPC_NOF_PORT_DEFINED_TT_PROFILES
typedef SOC_PPC_PORT_DEFINED_TT_PROFILE                        ARAD_PP_PORT_DEFINED_TT_PROFILE;
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
 *   arad_pp_port_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set main attributes of the port. Generally, these
 *   attributes identify the port and may have use in more
 *   than one module.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx -
 *     Local port ID.
 *   SOC_SAND_IN  ARAD_PP_PORT_INFO                           *port_info -
 *     Port attributes, including main attributes of the port.
 * REMARKS:
 *   - In order to configure the port default AC use
 *   soc_ppd_l2_lif_ac_add() while ignoring the VIDs i.e. set
 *   VIDs to SOC_PPD_LIF_IGNORE_INNER_VID and
 *   SOC_PPD_LIF_IGNORE_OUTER_VID.- Note that not all port
 *   attributes are included in this configuration. Some
 *   attributes that are used by a single module are
 *   configured by APIs in that module. Typically the API
 *   name is soc_ppd_<module_name>_port_info_set.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_port_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  ARAD_PP_PORT_INFO                           *port_info
  );

/*********************************************************************
*     Gets the configuration set by the "arad_pp_port_info_set"
 *     API.
 *     Refer to "arad_pp_port_info_set" API for details.
*********************************************************************/
uint32
  arad_pp_port_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx,
    SOC_SAND_OUT ARAD_PP_PORT_INFO                           *port_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_port_stp_state_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets the STP state of a port in a specific topology ID.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx -
 *     Local port ID.
 *   SOC_SAND_IN  uint32                                  topology_id_ndx -
 *     Topology ID. Set using soc_ppd_vsi_info_set(). Range: 0 -
 *     63.
 *   SOC_SAND_IN  ARAD_PP_PORT_STP_STATE                      stp_state -
 *     STP state of the port (discard/learn/forward)
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_port_stp_state_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  uint32                                  topology_id_ndx,
    SOC_SAND_IN  ARAD_PP_PORT_STP_STATE                      stp_state
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_port_stp_state_set" API.
 *     Refer to "arad_pp_port_stp_state_set" API for details.
*********************************************************************/
uint32
  arad_pp_port_stp_state_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  uint32                                  topology_id_ndx,
    SOC_SAND_OUT ARAD_PP_PORT_STP_STATE                      *stp_state
  );
void
  ARAD_PP_PORT_INFO_clear(
    SOC_SAND_OUT ARAD_PP_PORT_INFO *info
  );

#if ARAD_PP_DEBUG_IS_LVL1

const char*
  ARAD_PP_PORT_DIRECTION_to_string(
    SOC_SAND_IN  ARAD_PP_PORT_DIRECTION enum_val
  );

const char*
  ARAD_PP_PORT_STP_STATE_to_string(
    SOC_SAND_IN  ARAD_PP_PORT_STP_STATE enum_val
  );
void
  ARAD_PP_PORT_INFO_print(
    SOC_SAND_IN  ARAD_PP_PORT_INFO *info
  );

#endif /* ARAD_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_PP_API_PORT_INCLUDED__*/
#endif


