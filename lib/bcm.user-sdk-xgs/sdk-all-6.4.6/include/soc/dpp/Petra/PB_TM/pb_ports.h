/* $Id: pb_ports.h,v 1.7 Broadcom SDK $
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


#ifndef __SOC_PB_PORTS_INCLUDED__
/* { */
#define __SOC_PB_PORTS_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_api_ports.h>
#include <soc/dpp/Petra/PB_TM/pb_api_ports.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */
/* $Id: pb_ports.h,v 1.7 Broadcom SDK $
 * Port to interface mapping register value indicating
 * unmapped interface
 */
#define SOC_PB_PORTS_IF_UNMAPPED_INDICATION 0x7f

#define SOC_PB_PP_PORT_NDX_MAX                             (SOC_PB_PORT_NOF_PP_PORTS - 1)

#define SOC_PB_PP_PORT_PP_CONTEXT_QUALIFIER_LSB                    (3)

#define SOC_PB_PORT_EG_MIRROR_NOF_VID_MIRROR_INDICES                 (7)
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
  soc_pb_ports_init(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
* NAME:
*   soc_pb_sys_phys_to_local_port_map_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*   Map System Physical FAP Port to a <mapped_fap_id, mapped_fap_port_id>
*   pair. The mapping is unique - single System Physical
*   Port is mapped to a single local port per specified
*   device. This configuration effects: 1. Resolving
*   destination FAP Id and OFP Id 2. Per-port pruning
* INPUT:
*   SOC_SAND_IN  int                                 unit -
*     Identifier of the device to access.
*   SOC_SAND_IN  uint32                                 sys_phys_port_ndx -
*     The index of system physical port. Range: 0 - 4095.
*   SOC_SAND_IN  uint32                                 mapped_fap_id -
*     The device id of the port that is mapped. Range: 0 -
*     2047.
*   SOC_SAND_IN  uint32                                 mapped_fap_port_id -
*     Local (per device) FAP Port id. Range: 0 - 79.
* REMARKS:
*   1. The mapping is identical for incoming and outgoing
*   FAP Ports. 2. soc_pb_sys_phys the device to system fap port must
*   be performed before calling this API.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_sys_phys_to_local_port_map_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 sys_phys_port_ndx,
    SOC_SAND_IN  uint32                 mapped_fap_id,
    SOC_SAND_IN  uint32                 mapped_fap_port_id
  );

uint32
  soc_pb_local_to_sys_phys_port_map_itself_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 fap_local_port_ndx,
    SOC_SAND_OUT uint32                 *sys_phys_port_id
  );

/*********************************************************************
* NAME:
 *   soc_pb_port_pp_port_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Configure the Port profile for ports of type TM and Raw.
 * INPUT:
 *   SOC_SAND_IN  int                    unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                    pp_port_ndx -
 *     TM Port Profile Index. Range: 0 - 63.
 *   SOC_SAND_IN  SOC_PB_PORT_PP_PORT_INFO     *info -
 *     Attributes of the TM Port Profile. Ignored for Raw
 *     Ports.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE         *success -
 *     If True, then the TM Port Profile is added. Otherwise,
 *     not enough resources may be available.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_port_pp_port_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    pp_port_ndx,
    SOC_SAND_IN  SOC_PB_PORT_PP_PORT_INFO     *info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE         *success
  );

uint32
  soc_pb_port_pp_port_set_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    pp_port_ndx,
    SOC_SAND_IN  SOC_PB_PORT_PP_PORT_INFO     *info
  );

/*********************************************************************
* NAME:
 *   soc_pb_port_pp_port_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the Port profile settings.
 * INPUT:
 *   SOC_SAND_IN  int                    unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                    pp_port_ndx -
 *     TM Port Profile Index. Range: 0 - 63.
 *   SOC_SAND_OUT SOC_PB_PORT_PP_PORT_INFO     *info -
 *     Attributes of the TM Port Profile
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_port_pp_port_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    pp_port_ndx,
    SOC_SAND_OUT SOC_PB_PORT_PP_PORT_INFO     *info
  );

uint32
  soc_pb_port_pp_port_get_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    pp_port_ndx
  );

/*********************************************************************
* NAME:
 *   soc_pb_port_to_pp_port_map_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Map the Port to its Port profile for ports of type TM
 *   and Raw.
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                       port_ndx -
 *     TM Port Index. Range: 0 - 79.
 *   SOC_SAND_IN  uint32    pp_port -
 *     Mapping of the TM Port to its Profile
 * REMARKS:
 *   TM and Raw Ports can be mapped only to existing TM Port
 *   Profile. To add a new TM Port Profile, use the
 *   soc_pb_port_pp_port_set API.
 * RETURNS:
 *   OK or ERROR indication.
 *********************************************************************/
uint32
  soc_pb_port_to_pp_port_map_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION           direction_ndx,
    SOC_SAND_IN  uint32                    pp_port
  );

uint32
  soc_pb_port_to_pp_port_map_set_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION           direction_ndx,
    SOC_SAND_IN  uint32                    pp_port
  );

uint32
  soc_pb_port_to_pp_port_map_get_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    port_ndx
  );

uint32
  soc_pb_ports_set_in_src_sys_port_get_unsafe(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  uint32                                 tm_port_id,
    SOC_SAND_OUT  uint32                               *sys_port
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_port_to_pp_port_map_set_unsafe" API.
 *     Refer to "soc_pb_port_to_pp_port_map_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_port_to_pp_port_map_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    port_ndx,
    SOC_SAND_OUT uint32                    *pp_port_in,
    SOC_SAND_OUT uint32                    *pp_port_out
  );

/*********************************************************************
* NAME:
*   soc_pb_ports_lag_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*   Configure a LAG. A LAG is defined by a group of System
*   Physical Ports that compose it. This configuration
*   affects 1. LAG resolution for queuing at the ingress. 2.
*   LAG-based pruning.
* INPUT:
*   SOC_SAND_IN  int                                 unit -
*     Identifier of the device to access.
*   SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION                      direction_ndx -
*     LAG direction - incoming, outgoing or both. For
*     symmetrical LAG configuration - set direction as "both".
*     For different Incoming and Outgoing LAG configuration -
*     per-direction configuration is required.
*   SOC_SAND_IN  uint32                                 lag_ndx -
*     LAG index. Range: 0 - 255. SOC_SAND_OUT uint32
*   SOC_SAND_IN  SOC_PETRA_PORTS_LAG_INFO                      *info -
*     Lag members. Maximal number of out-going LAG members is
*     16. The number of incoming LAG members is not limited,
*     and it can be the number of Local FAP ports in each
*     device (80).
* REMARKS:
*   1. Local to system port mapping must be configured
*   before using this API (Incoming and Outgoing) - for LAG
*   pruning. 2. LAG configuration must be consistent
*   system-wide, for incoming and outgoing ports. 3. The
*   same system port can be added multiple times. This
*   affects the load-balancing, according to the number of
*   times the port appears in the LAG. 4. The _get function
*   is not symmetric to the set function: both incoming and
*   outgoing settings are returned (direction_ndx is not
*   passed). 5. For the INCOMING-LAG, the _get function
*   returns only LAG member sys-ports that are mapped to
*   local FAP ports, on the local device. 6. Setting LAG
*   with a group of system ports, will first clean-up any
*   previous configuration of the LAG. For example setting
*   LAG 1 with system members 1,2,3,4 and then setting the
*   same LAG with members 3,4,5,6 will clean up the effect
*   of the previous configuration and set up the new
*   configuration.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_ports_lag_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  uint32                 lag_ndx,
    SOC_SAND_IN  SOC_PETRA_PORTS_LAG_INFO      *info
  );

/*********************************************************************
* NAME:
*   soc_pb_ports_lag_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*   Configure a LAG. A LAG is defined by a group of System
*   Physical Ports that compose it. This configuration
*   affects 1. LAG resolution for queuing at the ingress. 2.
*   LAG-based pruning.
* INPUT:
*   SOC_SAND_IN  int                                 unit -
*     Identifier of the device to access.
*   SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION                      direction_ndx -
*     LAG direction - incoming, outgoing or both. For
*     symmetrical LAG configuration - set direction as "both".
*     For different Incoming and Outgoing LAG configuration -
*     per-direction configuration is required.
*   SOC_SAND_IN  uint32                                 lag_ndx -
*     LAG index. Range: 0 - 255. SOC_SAND_OUT uint32
*   SOC_SAND_OUT SOC_PETRA_PORTS_LAG_INFO                      *info -
*     Lag members. Maximal number of out-going LAG members is
*     16. The number of incoming LAG members is not limited,
*     and it can be the number of Local FAP ports in each
*     device (80).
* REMARKS:
*   1. Local to system port mapping must be configured
*   before using this API (Incoming and Outgoing) - for LAG
*   pruning. 2. LAG configuration must be consistent
*   system-wide, for incoming and outgoing ports. 3. The
*   same system port can be added multiple times. This
*   affects the load-balancing, according to the number of
*   times the port appears in the LAG. 4. The _get function
*   is not symmetric to the set function: both incoming and
*   outgoing settings are returned (direction_ndx is not
*   passed). 5. For the INCOMING-LAG, the _get function
*   returns only LAG member sys-ports that are mapped to
*   local FAP ports, on the local device. 6. Setting LAG
*   with a group of system ports, will first clean-up any
*   previous configuration of the LAG. For example setting
*   LAG 1 with system members 1,2,3,4 and then setting the
*   same LAG with members 3,4,5,6 will clean up the effect
*   of the previous configuration and set up the new
*   configuration.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_ports_lag_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 lag_ndx,
    SOC_SAND_OUT SOC_PETRA_PORTS_LAG_INFO      *info_incoming,
    SOC_SAND_OUT SOC_PETRA_PORTS_LAG_INFO      *info_outgoing
  );

/*********************************************************************
* NAME:
*     soc_pb_ports_lag_order_preserve_set_unsafe
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     Per-Lag information
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  uint32                  lag_ndx -
*     LAG index. Range: 0 - 255.
*  SOC_SAND_IN  uint8                 is_order_preserving -
*     is_order_preserving If set, the LAG outlif is chosen
*     according to the Hash mechanism, this gives order
*     preserving for all packets. Otherwise LAG Round Robin
*     takes place, and the outlif are chosen sequentially.
*     Note that this case provides better load balancing but
*     does not preserve order of packets.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_ports_lag_order_preserve_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  lag_ndx,
    SOC_SAND_IN  uint8                 is_order_preserving
  );

/*********************************************************************
* NAME:
*     soc_petra_ports_lag_order_preserve_get_unsafe
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     Per-Lag information
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  uint32                  lag_ndx -
*     LAG index. Range: 0 - 255.
*  SOC_SAND_OUT uint8                 *is_order_preserving -
*     is_order_preserving If set, the LAG outlif is chosen
*     according to the Hash mechanism, this gives order
*     preserving for all packets. Otherwise LAG Round Robin
*     takes place, and the outlif are chosen sequentially.
*     Note that this case provides better load balancing but
*     does not preserve order of packets.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_ports_lag_order_preserve_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  lag_ndx,
    SOC_SAND_OUT uint8                 *is_order_preserving
  );

void
  soc_pb_ports_lag_mem_id_mark_invalid(
    SOC_SAND_INOUT SOC_PETRA_PORTS_LAG_INFO      *info
  );


/*********************************************************************
* NAME:
*   soc_pb_ports_lag_member_add_unsafe
* TYPE:
*   PROC
* FUNCTION:
*   Add a system port as a member in LAG.
* INPUT:
*   SOC_SAND_IN  int                                 unit -
*     Identifier of the device to access.
*   SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION                      direction_ndx -
*     LAG direction - incoming, outgoing or both. For
*     symmetrical LAG configuration - set direction as "both".
*     For different Incoming and Outgoing LAG configuration -
*     per-direction configuration is required.
*   SOC_SAND_IN  uint32                                 lag_ndx -
*     LAG index. Range: 0 - 255.
*   SOC_SAND_IN  SOC_PETRA_PORTS_LAG_MEMBER                    *lag_member -
*     System port to be added as a member, and the
*     member-index.
*   SOC_SAND_OUT uint8                                 *success -
*     TRUE if the sys-port was added to the LAG. FALSE if not
*     added due to reaching maximal number of LAG members in
*     the specified direction.
* REMARKS:
*   1. Replaces soc_pb_ports_lag_sys_port_add (the later kept
*   for backward-compatability).
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_ports_lag_member_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION                      direction_ndx,
    SOC_SAND_IN  uint32                                 lag_ndx,
    SOC_SAND_IN  SOC_PETRA_PORTS_LAG_MEMBER                    *lag_member,
    SOC_SAND_OUT uint8                                 *success
  );


/*********************************************************************
* NAME:
*   soc_pb_ports_lag_sys_port_remove_unsafe
* TYPE:
*   PROC
* FUNCTION:
*   remove a system port from a LAG.
* INPUT:
*   SOC_SAND_IN  int                                 unit -
*     Identifier of the device to access.
*   SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION                      direction_ndx -
*     LAG direction - incoming, outgoing or both. For
*     symmetrical LAG configuration - set direction as "both".
*     For different Incoming and Outgoing LAG configuration -
*     per-direction configuration is required.
*   SOC_SAND_IN  uint32                                 lag_ndx -
*     LAG index. Range: 0 - 255.
*   SOC_SAND_IN  uint32                                 sys_port -
*     system port to be removed as a member.
* REMARKS:
*   If the port cannot be removed (the lag is empty),
*   returns without any action, and without setting an error
*   indication.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_ports_lag_sys_port_remove_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  uint32                 lag_ndx,
    SOC_SAND_IN  SOC_PETRA_PORTS_LAG_MEMBER           *lag_member
  );

uint32
  soc_pb_ports_lag_sys_port_info_get_unsafe(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     sys_port,
    SOC_SAND_OUT SOC_PETRA_PORT_LAG_SYS_PORT_INFO *port_lag_info
  );


/*********************************************************************
*     Configure FAP port header parsing type. The
*     configuration can be for incoming FAP ports, outgoing
*     FAP ports or both.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_port_header_type_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_HEADER_TYPE    header_type
  );

uint32
  soc_pb_port_header_type_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_OUT SOC_PETRA_PORT_HEADER_TYPE    *header_type_incoming,
    SOC_SAND_OUT SOC_PETRA_PORT_HEADER_TYPE    *header_type_outgoing
  );

uint32
  soc_pb_port_parse_header_type_unsafe(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 port_ndx,
    SOC_SAND_IN uint32 port_pp_context,
    SOC_SAND_OUT SOC_PETRA_PORT_HEADER_TYPE * header_type_in
  );

/*********************************************************************
* NAME:
*     soc_petra_port_to_interface_map_set_unsafe
* TYPE:
*   PROC
* DATE:
*   Apr 21 2008
* FUNCTION:
*     Maps the specified FAP Port to interface and channel.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 port_ndx -
*     Fap port index. Range: 0 - 79.
*  SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION      direction_ndx -
*     The direction of the mapped port
*     (incoming/outgoing/both).
*  SOC_SAND_IN  SOC_PETRA_PORT2IF_MAPPING_INFO *info -
*     Port to Interface mapping configuration.
* REMARKS:
*   1. ch_id is only relevant for channelized interfaces -
*   ignored otherwise. 2. To unmap a port without mapping to
*   another interface - use SOC_PETRA_IF_ID_INVALID as
*   info.if_id value. 3. The get function is not symmetric
*   to the set function: both incoming and outgoing settings
*   are returned (direction_ndx is not passed).
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_port_to_interface_map_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT2IF_MAPPING_INFO *info
  );

/*********************************************************************
* NAME:
*     soc_pa_port_to_interface_map_verify
* TYPE:
*   PROC
* DATE:
*   Apr 21 2008
* FUNCTION:
*     Maps the specified FAP Port to interface and channel.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 port_ndx -
*     Fap port index. Range: 0 - 79.
*  SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION      direction_ndx -
*     The direction of the mapped port
*     (incoming/outgoing/both).
*  SOC_SAND_IN  SOC_PETRA_PORT2IF_MAPPING_INFO *info -
*     Port to Interface mapping configuration.
* REMARKS:
*   1. ch_id is only relevant for channelized interfaces -
*   ignored otherwise. 2. To unmap a port without mapping to
*   another interface - use SOC_PETRA_IF_ID_INVALID as
*   info.if_id value. 3. The get function is not symmetric
*   to the set function: both incoming and outgoing settings
*   are returned (direction_ndx is not passed).
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_port_to_interface_map_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT2IF_MAPPING_INFO *info
  );

/*********************************************************************
* NAME:
*     soc_petra_port_to_interface_map_get_unsafe
* TYPE:
*   PROC
* DATE:
*   Apr 21 2008
* FUNCTION:
*     Maps the specified FAP Port to interface and channel.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 port_ndx -
*     Fap port index. Range: 0 - 79.
*  SOC_SAND_OUT SOC_PETRA_PORT2IF_MAPPING_INFO *info_incoming -
*     Incoming port to Interface mapping configuration.
*  SOC_SAND_OUT SOC_PETRA_PORT2IF_MAPPING_INFO *info_outgoing -
*     Outgoing port to Interface mapping configuration.
* RETURNS:
*     OK or ERROR indication.
* REMARKS:
*   1. ch_id is only relevant for channelized interfaces -
*   ignored otherwise. 2. To unmap a port without mapping to
*   another interface - use SOC_PETRA_IF_ID_INVALID as
*   info.if_id value. 3. The get function is not symmetric
*   to the set function: both incoming and outgoing settings
*   are returned (direction_ndx is not passed).
*********************************************************************/
uint32
  soc_pb_port_to_interface_map_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_OUT SOC_PETRA_PORT2IF_MAPPING_INFO *info_incoming,
    SOC_SAND_OUT SOC_PETRA_PORT2IF_MAPPING_INFO *info_outgoing
  );

/*********************************************************************
* NAME:
*     soc_pb_ports_ftmh_extension_set_unsafe
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     This function sets a system wide configuration of the
*     ftmh. The FTMH has 3 options for the FTMH-extension:
*     always allow, never allow, allow only when the packet is
*     multicast.
* INPUT:
*  SOC_SAND_IN  int                  unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_PORTS_FTMH_EXT_OUTLIF ext_option -
*     There 3 options for the FTMH extension.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_ports_ftmh_extension_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PETRA_PORTS_FTMH_EXT_OUTLIF ext_option
  );

/*********************************************************************
* NAME:
*     soc_pb_ports_ftmh_extension_get_unsafe
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     This function sets a system wide configuration of the
*     ftmh. The FTMH has 3 options for the FTMH-extension:
*     always allow, never allow, allow only when the packet is
*     multicast.
* INPUT:
*  SOC_SAND_IN  int                   unit -
*     Identifier of the device to access.
*  SOC_SAND_OUT SOC_PETRA_PORTS_FTMH_EXT_OUTLIF *ext_option -
*     There 3 options for the FTMH extension.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_ports_ftmh_extension_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_PETRA_PORTS_FTMH_EXT_OUTLIF *ext_option
  );


uint32
  SOC_PB_PORT_PP_PORT_INFO_verify(
    SOC_SAND_IN  SOC_PB_PORT_PP_PORT_INFO *info
  );

/*********************************************************************
*     This function sets what extensions are to be added to
*     the OTMH per port. The OTMH has 3 optional extensions:
*     Outlif (always allow/ never allow/ allow only when the
*     packet is multicast.) Source Sys-Port and Destination
*     Sys-Port.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_ports_otmh_extension_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORTS_OTMH_EXTENSIONS_EN *info
  );

/*********************************************************************
*     This function sets what extensions are to be added to
*     the OTMH per port. The OTMH has 3 optional extensions:
*     Outlif (always allow/ never allow/ allow only when the
*     packet is multicast.) Source Sys-Port and Destination
*     Sys-Port.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_ports_otmh_extension_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_OUT SOC_PETRA_PORTS_OTMH_EXTENSIONS_EN *info
  );

/*********************************************************************
*     Select from the available egress credit compensation
*     values to adjust the credit rate for the various
*     headers: PP (if present), FTMH, DRAM-CRC, Ethernet-IPG,
*     NIF-CRC. This API selects the discount type. The values
*     per port header type and discount type are configured
*     using soc_petra_port_egr_hdr_credit_discount_type_set API.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_port_egr_hdr_credit_discount_select_set_unsafe(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_TYPE    cr_discnt_type
  );

/*********************************************************************
*     Select from the available egress credit compensation
*     values to adjust the credit rate for the various
*     headers: PP (if present), FTMH, DRAM-CRC, Ethernet-IPG,
*     NIF-CRC. This API selects the discount type. The values
*     per port header type and discount type are configured
*     using soc_petra_port_egr_hdr_credit_discount_type_set API.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_port_egr_hdr_credit_discount_select_get_unsafe(
    SOC_SAND_IN  int                              unit,
    SOC_SAND_IN  uint32                              port_ndx,
    SOC_SAND_OUT SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_TYPE *cr_discnt_type
  );

/*********************************************************************
*     This function sets whether a port has an extension added
*     to its ITMH or not.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_ports_itmh_extension_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_OUT uint8                 *ext_en
  );

/*********************************************************************
*     This function sets whether a port has an extension added
*     to its ITMH or not.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_ports_itmh_extension_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  uint8                 ext_en
  );


uint32
  soc_pb_port_forwarding_header_configuration_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            pp_port_ndx,
    SOC_SAND_OUT uint32                           *trap_ndx,
    SOC_SAND_OUT SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_DATA  *fwd_act_profile_tbl_data,
    SOC_SAND_OUT SOC_PB_PP_IHB_SNOOP_ACTION_TBL_DATA     *snoop_tbl_data
  );

uint32
  soc_pb_port_forwarding_header_configuration_set_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  uint32                            pp_port_ndx,
    SOC_SAND_IN  uint32                             trap_ndx,
    SOC_SAND_IN  SOC_PB_PP_IHB_FWD_ACT_PROFILE_TBL_DATA  *fwd_act_profile_tbl_data,
    SOC_SAND_IN  SOC_PB_PP_IHB_SNOOP_ACTION_TBL_DATA     *snoop_tbl_data
  );

/*********************************************************************
* NAME:
 *   soc_pb_port_forwarding_header_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Define the Forwarding header parameters for Raw ports.
 * INPUT:
 *   SOC_SAND_IN  int                       unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                       pp_port_ndx -
 *     TM Port Profile Index. Range: 0 - 63.
 *   SOC_SAND_IN  SOC_PB_PORTS_FORWARDING_HEADER_INFO *info -
 *     Forwarding Header parameters.
 * REMARKS:
 *   This API can be used only if the TM Port Profile is for
 *   Raw Ports.
 *   For Packet Processing customers, the Trap Management and
 *   the Field Processing modules must be used.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_port_forwarding_header_set_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                       pp_port_ndx,
    SOC_SAND_IN  SOC_PB_PORTS_FORWARDING_HEADER_INFO *info
  );

uint32
  soc_pb_port_forwarding_header_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                       pp_port_ndx,
    SOC_SAND_IN  SOC_PB_PORTS_FORWARDING_HEADER_INFO *info
  );

uint32
  soc_pb_port_forwarding_header_get_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                       pp_port_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_port_forwarding_header_set_unsafe" API.
 *     Refer to "soc_pb_port_forwarding_header_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_port_forwarding_header_get_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                       pp_port_ndx,
    SOC_SAND_OUT SOC_PB_PORTS_FORWARDING_HEADER_INFO *info
  );

uint32
  SOC_PB_PORT_COUNTER_INFO_verify(
    SOC_SAND_IN  SOC_PB_PORT_COUNTER_INFO *info
  );

uint32
  SOC_PB_PORTS_FORWARDING_HEADER_INFO_verify(
    SOC_SAND_IN  SOC_PB_PORTS_FORWARDING_HEADER_INFO *info
  );

/*********************************************************************
*     Configure outbound mirroring for the specified port.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_ports_mirror_outbound_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 ofp_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_OUTBOUND_MIRROR_INFO *info
  );

/*********************************************************************
*     Get outbound mirroring configuration for the specified port.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_ports_mirror_outbound_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 ofp_ndx,
    SOC_SAND_OUT SOC_PETRA_PORT_OUTBOUND_MIRROR_INFO *info
  );

uint32
  soc_pb_ports_mirror_outbound_dflt_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 pp_port_ndx,
    SOC_SAND_IN  uint8                 is_enable
  );


/*********************************************************************
*     Allocate and return a free recycle interface channel and a free reassembly
*     context, by allocating an ITM port.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32 
soc_pb_ports_reassembly_context_and_recycle_channel_alloc_unsafe(
    SOC_SAND_IN int unit, 
    SOC_SAND_OUT uint32 *context, 
    SOC_SAND_OUT uint32 *channel);

/*********************************************************************
*     release a used reassembly context, by releasing its ITM port, also release 
*     its reassembly context.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32 
soc_pb_ports_reassembly_context_and_recycle_channel_free_unsafe(
    SOC_SAND_IN int unit, 
    SOC_SAND_IN uint32 context);

#if SOC_PB_DEBUG_IS_LVL1

#endif /* SOC_PB_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PORTS_INCLUDED__*/
#endif
