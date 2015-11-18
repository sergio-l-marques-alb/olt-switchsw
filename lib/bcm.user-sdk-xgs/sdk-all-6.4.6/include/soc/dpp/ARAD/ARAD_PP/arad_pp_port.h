/* $Id: arad_pp_port.h,v 1.10 Broadcom SDK $
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

#ifndef __ARAD_PP_PORT_INCLUDED__
/* { */
#define __ARAD_PP_PORT_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_port.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>

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
  ARAD_PP_PORT_INFO_SET = ARAD_PP_PROC_DESC_BASE_PORT_FIRST,
  ARAD_PP_PORT_INFO_SET_PRINT,
  ARAD_PP_PORT_INFO_SET_UNSAFE,
  ARAD_PP_PORT_INFO_SET_VERIFY,
  ARAD_PP_PORT_INFO_GET,
  ARAD_PP_PORT_INFO_GET_PRINT,
  ARAD_PP_PORT_INFO_GET_VERIFY,
  ARAD_PP_PORT_INFO_GET_UNSAFE,
  ARAD_PP_PORT_STP_STATE_SET,
  ARAD_PP_PORT_STP_STATE_SET_PRINT,
  ARAD_PP_PORT_STP_STATE_SET_UNSAFE,
  ARAD_PP_PORT_STP_STATE_SET_VERIFY,
  ARAD_PP_PORT_STP_STATE_GET,
  ARAD_PP_PORT_STP_STATE_GET_PRINT,
  ARAD_PP_PORT_STP_STATE_GET_VERIFY,
  ARAD_PP_PORT_STP_STATE_GET_UNSAFE,
  ARAD_PP_PORT_TM_TO_PP_MAP_SET,
  ARAD_PP_PORT_TM_TO_PP_MAP_SET_PRINT,
  ARAD_PP_PORT_TM_TO_PP_MAP_SET_UNSAFE,
  ARAD_PP_PORT_TM_TO_PP_MAP_SET_VERIFY,
  ARAD_PP_PORT_TM_TO_PP_MAP_GET,
  ARAD_PP_PORT_TM_TO_PP_MAP_GET_PRINT,
  ARAD_PP_PORT_TM_TO_PP_MAP_GET_VERIFY,
  ARAD_PP_PORT_TM_TO_PP_MAP_GET_UNSAFE,
  ARAD_PP_PORT_GET_PROCS_PTR,
  ARAD_PP_PORT_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */
   ARAD_PP_PORTS_REGS_INIT,
   ARAD_PP_PORTS_INIT,

  /*
   * Last element. Do no touch.
   */
  ARAD_PP_PORT_PROCEDURE_DESC_LAST
} ARAD_PP_PORT_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  ARAD_PP_PORT_TOPOLOGY_ID_NDX_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_PORT_FIRST,
  ARAD_PP_PORT_STP_STATE_OUT_OF_RANGE_ERR,
  ARAD_PP_PORT_SYS_PHY_PORT_ID_OUT_OF_RANGE_ERR,
  ARAD_PP_PORT_DIRECTION_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_PORT_LEARN_DEST_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_PORT_PORT_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_PP_PORT_TPID_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_PP_PORT_VLAN_DOMAIN_OUT_OF_RANGE_ERR,
  ARAD_PP_PORT_PORT_TYPE_OUT_OF_RANGE_ERR,
  ARAD_PP_PORT_ORIENTATION_OUT_OF_RANGE_ERR,
  ARAD_PP_PORT_ETHER_TYPE_BASED_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_PP_PORT_MTU_OUT_OF_RANGE_ERR,
  ARAD_PP_PORT_DA_NOT_FOUND_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_PP_PORT_VT_PROFILE_OUT_OF_RANGE_ERR,
  ARAD_PP_PORT_VSI_PROFILE_OUT_OF_RANGE_ERR,
  /*
   * } Auto generated. Do not edit previous section.
   */

  /*
   * Last element. Do no touch.
   */
  ARAD_PP_PORT_ERR_LAST
} ARAD_PP_PORT_ERR;


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
  arad_pp_port_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );


/*********************************************************************
* NAME:
 *   arad_pp_port_info_set_unsafe
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
  arad_pp_port_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  ARAD_PP_PORT_INFO                           *port_info
  );


uint32
  arad_pp_port_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  ARAD_PP_PORT_INFO                           *port_info
  );

uint32
  arad_pp_port_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_port_info_set_unsafe" API.
 *     Refer to "arad_pp_port_info_set_unsafe" API for details.
*********************************************************************/
uint32
  arad_pp_port_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx,
    SOC_SAND_OUT ARAD_PP_PORT_INFO                           *port_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_port_stp_state_set_unsafe
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
  arad_pp_port_stp_state_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  uint32                                  topology_id_ndx,
    SOC_SAND_IN  ARAD_PP_PORT_STP_STATE                      stp_state
  );

uint32
  arad_pp_port_stp_state_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  uint32                                  topology_id_ndx,
    SOC_SAND_IN  ARAD_PP_PORT_STP_STATE                      stp_state
  );

uint32
  arad_pp_port_stp_state_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  uint32                                  topology_id_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_port_stp_state_set_unsafe" API.
 *     Refer to "arad_pp_port_stp_state_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  arad_pp_port_stp_state_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  uint32                                  topology_id_ndx,
    SOC_SAND_OUT ARAD_PP_PORT_STP_STATE                      *stp_state
  );
uint32
  ARAD_PP_PORT_INFO_verify(
    SOC_SAND_IN  int               unit,
    SOC_SAND_IN  ARAD_PP_PORT_INFO *info
  );

soc_error_t arad_pp_port_property_set(int unit, int core, int port, SOC_PPC_PORT_PROPERTY port_property, uint32 value);
soc_error_t arad_pp_port_property_get(int unit, int core, int port, SOC_PPC_PORT_PROPERTY port_property, uint32 *value);

/*********************************************************************
* NAME:
*     arad_pp_port_additional_tpids_set
* TYPE:
*   PROC
* FUNCTION:
*     Set the additional TPIDs.
* INPUT:
*  int                  unit - Identifier of the device to access.
*  SOC_PPC_ADDITIONAL_TPID_VALUES additional_tpids - Array of additional TPIDs.
*     For ARAD/ARAD+, only additional_tpids->tpid_vals[0] is used.
*     For Jericho, 
*     additional_tpids->tpid_vals[0] is used as Tpid1AdditionalValue1. 
*     additional_tpids->tpid_vals[1] is used as Tpid1AdditionalValue2.
*     additional_tpids->tpid_vals[2] is used as Tpid2AdditionalValue1.
*     additional_tpids->tpid_vals[3] is used as Tpid2AdditionalValue2.
* RETURNS:
*     SOC_E_XXX.
*********************************************************************/
soc_error_t arad_pp_port_additional_tpids_set(int unit, SOC_PPC_ADDITIONAL_TPID_VALUES *additional_tpids);

/*********************************************************************
* NAME:
*     arad_pp_port_additional_tpids_get
* TYPE:
*   PROC
* FUNCTION:
*     Get the additional TPIDs.
* INPUT:
*  int                  unit - Identifier of the device to access.
*  SOC_PPC_ADDITIONAL_TPID_VALUES additional_tpids - Array of additional TPIDs.
*     For ARAD/ARAD+, only additional_tpids->tpid_vals[0] is used.
*     For Jericho, 
*     additional_tpids->tpid_vals[0] is used as Tpid1AdditionalValue1. 
*     additional_tpids->tpid_vals[1] is used as Tpid1AdditionalValue2.
*     additional_tpids->tpid_vals[2] is used as Tpid2AdditionalValue1.
*     additional_tpids->tpid_vals[3] is used as Tpid2AdditionalValue2.
* RETURNS:
*     SOC_E_XXX.
*********************************************************************/
soc_error_t arad_pp_port_additional_tpids_get(int unit, SOC_PPC_ADDITIONAL_TPID_VALUES *additional_tpids);

/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_PP_PORT_INCLUDED__*/
#endif
