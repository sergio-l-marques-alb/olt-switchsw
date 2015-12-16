/* $Id: pb_pp_port.h,v 1.9 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/include/soc_pb_pp_port.h
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

#ifndef __SOC_PB_PP_PORT_INCLUDED__
/* { */
#define __SOC_PB_PP_PORT_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_api_port.h>
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
  SOC_PB_PP_PORT_INFO_SET = SOC_PB_PP_PROC_DESC_BASE_PORT_FIRST,
  SOC_PB_PP_PORT_INFO_SET_PRINT,
  SOC_PB_PP_PORT_INFO_SET_UNSAFE,
  SOC_PB_PP_PORT_INFO_SET_VERIFY,
  SOC_PB_PP_PORT_INFO_GET,
  SOC_PB_PP_PORT_INFO_GET_PRINT,
  SOC_PB_PP_PORT_INFO_GET_VERIFY,
  SOC_PB_PP_PORT_INFO_GET_UNSAFE,
  SOC_PB_PP_PORT_STP_STATE_SET,
  SOC_PB_PP_PORT_STP_STATE_SET_PRINT,
  SOC_PB_PP_PORT_STP_STATE_SET_UNSAFE,
  SOC_PB_PP_PORT_STP_STATE_SET_VERIFY,
  SOC_PB_PP_PORT_STP_STATE_GET,
  SOC_PB_PP_PORT_STP_STATE_GET_PRINT,
  SOC_PB_PP_PORT_STP_STATE_GET_VERIFY,
  SOC_PB_PP_PORT_STP_STATE_GET_UNSAFE,
  SOC_PB_PP_PORT_TM_TO_PP_MAP_SET,
  SOC_PB_PP_PORT_TM_TO_PP_MAP_SET_PRINT,
  SOC_PB_PP_PORT_TM_TO_PP_MAP_SET_UNSAFE,
  SOC_PB_PP_PORT_TM_TO_PP_MAP_SET_VERIFY,
  SOC_PB_PP_PORT_TM_TO_PP_MAP_GET,
  SOC_PB_PP_PORT_TM_TO_PP_MAP_GET_PRINT,
  SOC_PB_PP_PORT_TM_TO_PP_MAP_GET_VERIFY,
  SOC_PB_PP_PORT_TM_TO_PP_MAP_GET_UNSAFE,
  SOC_PB_PP_PORT_GET_PROCS_PTR,
  SOC_PB_PP_PORT_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */
   SOC_PB_PP_PORTS_REGS_INIT,
   SOC_PB_PP_PORTS_INIT,

  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_PORT_PROCEDURE_DESC_LAST
} SOC_PB_PP_PORT_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PB_PP_PORT_TOPOLOGY_ID_NDX_OUT_OF_RANGE_ERR = SOC_PB_PP_ERR_DESC_BASE_PORT_FIRST,
  SOC_PB_PP_PORT_STP_STATE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_PORT_SYS_PHY_PORT_ID_OUT_OF_RANGE_ERR,
  SOC_PB_PP_PORT_DIRECTION_NDX_OUT_OF_RANGE_ERR,
  SOC_PB_PP_PORT_LEARN_DEST_TYPE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_PORT_PORT_PROFILE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_PORT_TPID_PROFILE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_PORT_VLAN_DOMAIN_OUT_OF_RANGE_ERR,
  SOC_PB_PP_PORT_PORT_TYPE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_PORT_ORIENTATION_OUT_OF_RANGE_ERR,
  SOC_PB_PP_PORT_ETHER_TYPE_BASED_PROFILE_OUT_OF_RANGE_ERR,
  SOC_PB_PP_PORT_MTU_OUT_OF_RANGE_ERR,
  /*
   * } Auto generated. Do not edit previous section.
   */
  SOC_PB_PP_PORT_PORT_INFO_OUT_OF_RANGE_ERR,
  /*
   * Last element. Do no touch.
   */
  SOC_PB_PP_PORT_ERR_LAST
} SOC_PB_PP_PORT_ERR;

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
  soc_pb_pp_port_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_port_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set main attributes of the port. Generally, these
 *   attributes identify the port and may have use in more
 *   than one module.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx -
 *     Local port ID.
 *   SOC_SAND_IN  SOC_PB_PP_PORT_INFO                           *port_info -
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
  soc_pb_pp_port_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_PORT_INFO                           *port_info
  );

uint32
  soc_pb_pp_port_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_PORT_INFO                           *port_info
  );

uint32
  soc_pb_pp_port_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_port_info_set_unsafe" API.
 *     Refer to "soc_pb_pp_port_info_set_unsafe" API for details.
*********************************************************************/
uint32
  soc_pb_pp_port_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_PB_PP_PORT_INFO                           *port_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_port_stp_state_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets the STP state of a port in a specific topology ID.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx -
 *     Local port ID.
 *   SOC_SAND_IN  uint32                                  topology_id_ndx -
 *     Topology ID. Set using soc_ppd_vsi_info_set(). Range: 0 -
 *     63.
 *   SOC_SAND_IN  SOC_PB_PP_PORT_STP_STATE                      stp_state -
 *     STP state of the port (discard/learn/forward)
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_port_stp_state_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  uint32                                  topology_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_PORT_STP_STATE                      stp_state
  );

uint32
  soc_pb_pp_port_stp_state_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  uint32                                  topology_id_ndx,
    SOC_SAND_IN  SOC_PB_PP_PORT_STP_STATE                      stp_state
  );

uint32
  soc_pb_pp_port_stp_state_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  uint32                                  topology_id_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_port_stp_state_set_unsafe" API.
 *     Refer to "soc_pb_pp_port_stp_state_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_port_stp_state_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  uint32                                  topology_id_ndx,
    SOC_SAND_OUT SOC_PB_PP_PORT_STP_STATE                      *stp_state
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_port_tm_to_pp_map_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set mapping from TM local port to PP local port.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_TM_PORT                             local_tm_port_ndx -
 *     Local TM port IDRange: 0 - 80.
 *   SOC_SAND_IN  SOC_PB_PP_PORT_DIRECTION                      direction_ndx -
 *     mapping direction - incoming, outgoing or both. For
 *     symmetrical mapping - set direction as "both".
 *   SOC_SAND_IN  SOC_PB_PP_PORT                                local_pp_port -
 *     Local PP port ID. Per port attributes for packet
 *     processing is associated for this ID. Range: 0 - 63.
 * REMARKS:
 *   1. Soc_petra-B only. Error is returned if called for T20E.
 *   2. This is advanced API, user may choose to stay with
 *   the default mapping which performs 1x1 mapping. 3. Many
 *   TM ports may be mapped to same PP port ID. 4. TM ports
 *   that participate in a LAG SHOULD be mapped to same local
 *   PP port.
 *   3. This is a deprecated API. The user should use instead
 *   for Soc_petra-B the soc_pb_port_to_pp_port_map_set API.
* RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_port_tm_to_pp_map_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_TM_PORT                             local_tm_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_PORT_DIRECTION                      direction_ndx,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_pp_port
  );

uint32
  soc_pb_pp_port_tm_to_pp_map_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_TM_PORT                             local_tm_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_PORT_DIRECTION                      direction_ndx,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_pp_port
  );

uint32
  soc_pb_pp_port_tm_to_pp_map_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_TM_PORT                             local_tm_port_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_port_tm_to_pp_map_set_unsafe" API.
 *     Refer to "soc_pb_pp_port_tm_to_pp_map_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_port_tm_to_pp_map_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_TM_PORT                             local_tm_port_ndx,
    SOC_SAND_OUT SOC_PB_PP_PORT                                *local_pp_port_in,
    SOC_SAND_OUT SOC_PB_PP_PORT                                *local_pp_port_out
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_port_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   soc_pb_pp_api_port module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_port_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   soc_pb_pp_port_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   soc_pb_pp_api_port module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_port_get_errs_ptr(void);

uint32
  SOC_PB_PP_PORT_EXTEND_P2P_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_PORT_EXTEND_P2P_INFO *info
  );

uint32
  SOC_PB_PP_PORT_L2_VPN_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_PORT_L2_VPN_INFO *info
  );

uint32
  SOC_PB_PP_PORT_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_PORT_INFO *info
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PP_PORT_INCLUDED__*/
#endif

