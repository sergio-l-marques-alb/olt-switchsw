/* $Id: petra_api_ports.h,v 1.6 Broadcom SDK $
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


#ifndef __SOC_PETRA_API_PORTS_INCLUDED__
/* { */
#define __SOC_PETRA_API_PORTS_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_api_general.h>
#include <soc/dpp/TMC/tmc_api_ports.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     Maximal System Physical Port index. Physical Port
 *     uniquely identifies a FAP port in the system.           */

/*     Maximal number of LAG-s in the system                   */

/*     Maximal number of ports in Out-LAG                      */
#define  SOC_PETRA_PORTS_LAG_OUT_MEMBERS_MAX 16/* (SOC_TMC_PORTS_LAG_OUT_MEMBERS_MAX) */

/* $Id: petra_api_ports.h,v 1.6 Broadcom SDK $
 *   Maximal LAG member index.
 *   Note: though incoming LAG can contain up to 80 members,
 *   the maximal member-id is 15.
 *   If the LAG-member-id is not used by a higher protocol
 *   in the CPU, it's value is not significant (not used for LAG-based pruning)
 */
#define  SOC_PETRA_PORTS_LAG_IN_MEMBERS_MAX (SOC_TMC_PORTS_LAG_IN_MEMBERS_MAX)

/*     Maximal number of member ports in In-LAG               */
#define  SOC_PETRA_PORTS_LAG_MEMBERS_MAX (SOC_TMC_PORTS_LAG_MEMBERS_MAX)
#define SOC_PETRA_PORTS_LAG_MEMBER_ID_MAX (SOC_TMC_PORTS_LAG_MEMBER_ID_MAX)
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

#define SOC_PETRA_PORT_DIRECTION_INCOMING                      SOC_TMC_PORT_DIRECTION_INCOMING
#define SOC_PETRA_PORT_DIRECTION_OUTGOING                      SOC_TMC_PORT_DIRECTION_OUTGOING
#define SOC_PETRA_PORT_DIRECTION_BOTH                          SOC_TMC_PORT_DIRECTION_BOTH
#define SOC_PETRA_PORT_NOF_DIRECTIONS                          SOC_TMC_PORT_NOF_DIRECTIONS
typedef SOC_TMC_PORT_DIRECTION                                 SOC_PETRA_PORT_DIRECTION;

#define SOC_PETRA_PORT_HEADER_TYPE_NONE                        SOC_TMC_PORT_HEADER_TYPE_NONE
#define SOC_PETRA_PORT_HEADER_TYPE_ETH                         SOC_TMC_PORT_HEADER_TYPE_ETH
#define SOC_PETRA_PORT_HEADER_TYPE_RAW                         SOC_TMC_PORT_HEADER_TYPE_RAW
#define SOC_PETRA_PORT_HEADER_TYPE_TM                          SOC_TMC_PORT_HEADER_TYPE_TM
#define SOC_PETRA_PORT_HEADER_TYPE_PROG                        SOC_TMC_PORT_HEADER_TYPE_PROG
#define SOC_PETRA_PORT_HEADER_TYPE_CPU                         SOC_TMC_PORT_HEADER_TYPE_CPU
#define SOC_PETRA_PORT_HEADER_TYPE_STACKING                    SOC_TMC_PORT_HEADER_TYPE_STACKING
#define SOC_PETRA_PORT_HEADER_TYPE_TDM                         SOC_TMC_PORT_HEADER_TYPE_TDM
#define SOC_PETRA_PORT_HEADER_TYPE_TDM_RAW                     SOC_TMC_PORT_HEADER_TYPE_TDM_RAW
#define SOC_PETRA_PORT_HEADER_TYPE_INJECTED                    SOC_TMC_PORT_HEADER_TYPE_INJECTED
#define SOC_PETRA_PORT_NOF_HEADER_TYPES                        SOC_TMC_PORT_NOF_HEADER_TYPES
typedef SOC_TMC_PORT_HEADER_TYPE                               SOC_PETRA_PORT_HEADER_TYPE;

#define SOC_PETRA_PORTS_SNOOP_SIZE_BYTES_64                    SOC_TMC_PORTS_SNOOP_SIZE_BYTES_64
#define SOC_PETRA_PORTS_SNOOP_SIZE_BYTES_192                   SOC_TMC_PORTS_SNOOP_SIZE_BYTES_192
#define SOC_PETRA_PORTS_SNOOP_SIZE_ALL                         SOC_TMC_PORTS_SNOOP_SIZE_ALL
typedef SOC_TMC_PORTS_SNOOP_SIZE                               SOC_PETRA_PORTS_SNOOP_SIZE;

#define SOC_PETRA_PORTS_FTMH_EXT_OUTLIF_NEVER                  SOC_TMC_PORTS_FTMH_EXT_OUTLIF_NEVER
#define SOC_PETRA_PORTS_FTMH_EXT_OUTLIF_IF_MC                  SOC_TMC_PORTS_FTMH_EXT_OUTLIF_IF_MC
#define SOC_PETRA_PORTS_FTMH_EXT_OUTLIF_ALWAYS                 SOC_TMC_PORTS_FTMH_EXT_OUTLIF_ALWAYS
#define SOC_PETRA_PORTS_FTMH_NOF_EXT_OUTLIFS                   SOC_TMC_PORTS_FTMH_NOF_EXT_OUTLIFS
typedef SOC_TMC_PORTS_FTMH_EXT_OUTLIF                          SOC_PETRA_PORTS_FTMH_EXT_OUTLIF;

#define SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_TYPE_A              SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_TYPE_A
#define SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_TYPE_B              SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_TYPE_B
#define SOC_PETRA_PORT_NOF_EGR_HDR_CR_DISCOUNT_TYPES           SOC_TMC_PORT_NOF_EGR_HDR_CR_DISCOUNT_TYPES
typedef SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_TYPE                  SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_TYPE;

typedef SOC_TMC_PORT2IF_MAPPING_INFO                           SOC_PETRA_PORT2IF_MAPPING_INFO;
typedef SOC_TMC_PORTS_LAG_MEMBER                               SOC_PETRA_PORTS_LAG_MEMBER;
typedef SOC_TMC_PORTS_LAG_INFO                                 SOC_PETRA_PORTS_LAG_INFO;
typedef SOC_TMC_PORTS_OVERRIDE_INFO                            SOC_PETRA_PORTS_OVERRIDE_INFO;
typedef SOC_TMC_PORT_INBOUND_MIRROR_INFO                       SOC_PETRA_PORT_INBOUND_MIRROR_INFO;
typedef SOC_TMC_PORT_OUTBOUND_MIRROR_INFO                      SOC_PETRA_PORT_OUTBOUND_MIRROR_INFO;
typedef SOC_TMC_PORT_SNOOP_INFO                                SOC_PETRA_PORT_SNOOP_INFO;
typedef SOC_TMC_PORTS_ITMH_BASE                                SOC_PETRA_PORTS_ITMH_BASE;
typedef SOC_TMC_PORTS_ITMH_EXT_SRC_PORT                        SOC_PETRA_PORTS_ITMH_EXT_SRC_PORT;
typedef SOC_TMC_PORTS_ITMH                                     SOC_PETRA_PORTS_ITMH;
typedef SOC_TMC_PORTS_ISP_INFO                                 SOC_PETRA_PORTS_ISP_INFO;
typedef SOC_TMC_PORTS_STAG_FIELDS                              SOC_PETRA_PORTS_STAG_FIELDS;
typedef SOC_TMC_PORTS_OTMH_EXTENSIONS_EN                       SOC_PETRA_PORTS_OTMH_EXTENSIONS_EN;
typedef SOC_TMC_PORT_EGR_HDR_CR_DISCOUNT_INFO                  SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_INFO;

typedef SOC_TMC_PORT_LAG_SYS_PORT_INFO                         SOC_PETRA_PORT_LAG_SYS_PORT_INFO;

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
*   soc_petra_sys_phys_to_local_port_map_set
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
*   FAP Ports. 2. Mapping the device to system fap port must
*   be performed before calling this API.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_sys_phys_to_local_port_map_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 sys_phys_port_ndx,
    SOC_SAND_IN  uint32                 mapped_fap_id,
    SOC_SAND_IN  uint32                 mapped_fap_port_id
  );

/*********************************************************************
* NAME:
*   soc_petra_sys_phys_to_local_port_map_get
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
*   SOC_SAND_OUT uint32                                 *mapped_fap_id -
*     The device id of the port that is mapped. Range: 0 -
*     2047.
*   SOC_SAND_OUT uint32                                 *mapped_fap_port_id -
*     Local (per device) FAP Port id. Range: 0 - 79.
* REMARKS:
*   1. The mapping is identical for incoming and outgoing
*   FAP Ports. 2. Mapping the device to system fap port must
*   be performed before calling this API.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_sys_phys_to_local_port_map_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 sys_phys_port_ndx,
    SOC_SAND_OUT uint32                 *mapped_fap_id,
    SOC_SAND_OUT uint32                 *mapped_fap_port_id
  );

/*********************************************************************
* NAME:
*     soc_petra_local_to_sys_phys_port_map_get
* TYPE:
*   PROC
* FUNCTION:
*     Get a System Physical FAP Port mapped to a FAP port in
*     the local device. The mapping is unique - single System
*     Physical Port is mapped to a single local port per
*     specified device. This configuration effects: 1.
*     Resolving destination FAP Id and OFP Id 2. Per-port
*     pruning
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 fap_ndx -
*     Local (per device) FAP Port id. Range: 0 - 79.
*  SOC_SAND_IN  uint32                 fap_local_port_ndx -
*     The device id of the port that is mapped.
*  SOC_SAND_OUT uint32                 *sys_phys_port_id -
*     The index of system physical port. Range: 0 - 4095.
* REMARKS:
*     The mapping is identical for incoming and outgoing FAP
*     Ports.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_local_to_sys_phys_port_map_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 fap_ndx,
    SOC_SAND_IN  uint32                 fap_local_port_ndx,
    SOC_SAND_OUT uint32                 *sys_phys_port_id
  );

/*********************************************************************
* NAME:
*     soc_petra_port_to_interface_map_set
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
*   ignored otherwise.
*   2. To unmap a port without mapping to another interface -
*   use SOC_PETRA_IF_ID_NONE as info.if_id value.
*   3. The get function is not entirely symmetric to the set function
*     (where only incoming, outgoing or both directions can be defined).
*     The get function returns both directions.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_port_to_interface_map_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT2IF_MAPPING_INFO *info
  );

int 
soc_petra_port_to_interface_map_set_dispatch(
    SOC_SAND_IN  int                 unit, 
    SOC_SAND_IN  soc_port_t          port,
    SOC_SAND_IN  int                 unmap
    );

/*********************************************************************
* NAME:
*     soc_petra_port_to_interface_map_get
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
*   ignored otherwise.
*   2. To unmap a port without mapping to another interface -
*   use SOC_PETRA_IF_ID_NONE as info.if_id value.
*   3. The get function is not entirely symmetric to the set function
*     (where only incoming, outgoing or both directions can be defined).
*     The get function returns both directions.
*********************************************************************/
int
  soc_petra_port_to_interface_map_get_dispatch(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  int                    core_id,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_OUT SOC_PETRA_INTERFACE_ID *if_id,
    SOC_SAND_OUT uint32                 *channel_id
  );

uint32
  soc_petra_port_to_interface_map_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core_id,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_OUT SOC_PETRA_PORT2IF_MAPPING_INFO *info_incoming,
    SOC_SAND_OUT SOC_PETRA_PORT2IF_MAPPING_INFO *info_outgoing
  );

/********************************************************************* 
* NAME: 
*   soc_petra_ports_is_port_lag_member  
* FUNCTION: 
*   Gives LAG information of . 
* INPUT: 
*   SOC_SAND_IN  int                                 unit - 
*     Identifier of the device to access. 
*   SOC_SAND_IN  uint32                                 port_id - 
*     local port index. 
*   SOC_SAND_OUT  uint8                                *is_in_lag - 
*     set to TRUE if the port is a lag member. 
*   SOC_SAND_OUT  uint32                                *lag_ndx - 
*     is is_in_lag = TRUE, set to LAG index of the port. Range: 0 - 255. 
* 
* RETURNS: 
*   OK or ERROR indication. 
*********************************************************************/ 
   
uint32  
  soc_petra_ports_is_port_lag_member( 
    SOC_SAND_IN  int       unit, 
    SOC_SAND_IN  int       core_id, 
    SOC_SAND_IN  uint32       port_id, 
    SOC_SAND_OUT uint8        *is_in_lag, 
    SOC_SAND_OUT uint32       *lag_id
  );

/*********************************************************************
* NAME:
*   soc_petra_ports_lag_set
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
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ports_lag_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  uint32                 lag_ndx,
    SOC_SAND_IN  SOC_PETRA_PORTS_LAG_INFO      *info
  );

/*********************************************************************
* NAME:
*   soc_petra_ports_lag_get
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
  soc_petra_ports_lag_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 lag_ndx,
    SOC_SAND_OUT SOC_PETRA_PORTS_LAG_INFO      *info_incoming,
    SOC_SAND_OUT SOC_PETRA_PORTS_LAG_INFO      *info_outgoing
  );

/*********************************************************************
* NAME:
*   soc_petra_ports_lag_sys_port_add
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
*   SOC_SAND_IN  uint32                                 sys_port -
*     System port to be added as a member.
* REMARKS:
*   1. If the port cannot be added (maximal number of
*   members reached), returns an error indication. 2. The
*   same system port can be added multiple times. This
*   affects the load-balancing, according to the number of
*   times the port appears in the LAG. 3. This API is for
*   backward-compatability only. It does not accept LAG
*   membership index of the added port as the parameter. It
*   can be used if the membership index is not important -
*   i.e. for LAG-based pruning, and in any case that the CPU
*   does not manage the membership index explicitly.
*   Otherwise, it is replaced by
*   'soc_petra_ports_lag_member_add' API.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ports_lag_sys_port_add(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  uint32                 lag_ndx,
    SOC_SAND_IN  uint32                 sys_port
  );

/*********************************************************************
* NAME:
*   soc_petra_ports_lag_member_add
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
*   1. Replaces soc_petra_ports_lag_sys_port_add (the later kept
*   for backward-compatability).
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ports_lag_member_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION                      direction_ndx,
    SOC_SAND_IN  uint32                                 lag_ndx,
    SOC_SAND_IN  SOC_PETRA_PORTS_LAG_MEMBER                    *lag_member,
    SOC_SAND_OUT uint8                                 *success
  );

/*********************************************************************
* NAME:
*     soc_petra_ports_lag_sys_port_remove
* TYPE:
*   PROC
* DATE:
*   Apr 21 2008
* FUNCTION:
*     Remove Physical System port to be  member of a a LAG. A LAG is defined by a group of System
*     Physical Ports that compose it. This configuration
*     affects 1. LAG resolution for queuing at the ingress. 2.
*     LAG-based pruning.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION      direction_ndx -
*     LAG direction - incoming, outgoing or both. For
*     symmetrical LAG configuration - set direction as "both".
*     For different Incoming and Outgoing LAG configuration -
*     per-direction configuration is required.
*  SOC_SAND_IN  uint32                 lag_ndx -
*     LAG index. Range: 0 - 255. SOC_SAND_OUT uint32
*  SOC_SAND_IN  SOC_PETRA_PORTS_LAG_MEMBER       *lag_member  -
*     physical system port to remove.
* REMARKS:
*   If the port cannot be removed (the lag is empty),
*   returns without any action, and without setting an error
*   indication.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ports_lag_sys_port_remove(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  uint32                 lag_ndx,
    SOC_SAND_IN  SOC_PETRA_PORTS_LAG_MEMBER        *lag_member
  );

/*********************************************************************
* NAME:
*     soc_petra_ports_lag_sys_port_info_get
* TYPE:
*   PROC
* DATE:
*   Apr 21 2008
* FUNCTION:
*   Gets whether the physical system port is a member
*   in outgoing/incomming LAG, if so it also returns the LAG ID
*   it members at
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 sys_port -
*     physical system port
*  SOC_SAND_OUT  SOC_PETRA_PORT_LAG_SYS_PORT_INFO *port_lag_info_incoming -
*     physical system port info regarding the LAG config.
* REMARKS:
*     1. Local to system port mapping must be configured
*     before using this API (Incoming and Outgoing) - for LAG
*     pruning. 2. LAG configuration must be consistent
*     system-wide, for incoming and outgoing ports.
*   3. The get function is not entirely symmetric to the set function
*     (where only incoming, outgoing or both directions can be defined).
*     The get function returns both directions.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ports_lag_sys_port_info_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     sys_port,
    SOC_SAND_OUT SOC_PETRA_PORT_LAG_SYS_PORT_INFO *port_lag_info_incoming
  );


/*********************************************************************
* NAME:
*     soc_petra_ports_lag_order_preserve_set
* TYPE:
*   PROC
* DATE:
*   Oct  3 2007
* FUNCTION:
*     Per-Lag information
* INPUT:
*  SOC_SAND_IN  int                 unit -
*  SOC_SAND_IN  uint32                  lag_ndx -
*     lag_ndx- The RLAG index of which to enable/disable
*     round robin.
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
  soc_petra_ports_lag_order_preserve_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  lag_ndx,
    SOC_SAND_IN  uint8                 is_order_preserving
  );

/*********************************************************************
* NAME:
*     soc_petra_ports_lag_order_preserve_get
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
  soc_petra_ports_lag_order_preserve_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  lag_ndx,
    SOC_SAND_OUT uint8                 *is_order_preserving
  );

/*********************************************************************
* NAME:
*     soc_petra_port_header_type_set
* TYPE:
*   PROC
* DATE:
*   Apr 21 2008
* FUNCTION:
*     Configure FAP port header parsing type. The
*     configuration can be for incoming FAP ports, outgoing
*     FAP ports or both.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 port_ndx -
*     FAP Port index. Range: 0 - 79.
*  SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION      direction_ndx -
*     The direction of the fap port to configure - incoming,
*     outgoing or both.
*  SOC_SAND_IN  SOC_PETRA_PORT_HEADER_TYPE    header_type -
*     Port header parsing type.
* REMARKS:
*     1. Not all header types are valid for all directions.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_port_header_type_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                    core_id,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION      direction_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_HEADER_TYPE    header_type
  );

/*********************************************************************
* NAME:
*     soc_petra_port_header_type_get
* TYPE:
*   PROC
* DATE:
*   Apr 21 2008
* FUNCTION:
*     Configure FAP port header parsing type. The
*     configuration can be for incoming FAP ports, outgoing
*     FAP ports or both.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 port_ndx -
*     FAP Port index. Range: 0 - 79.
*  SOC_SAND_IN  SOC_PETRA_PORT_DIRECTION      direction_ndx -
*     The direction of the fap port to configure - incoming,
*     outgoing or both.
*  SOC_SAND_OUT SOC_PETRA_PORT_HEADER_TYPE    *header_type -
*     Port header parsing type.
* REMARKS:
*     1. Not all header types are valid for all directions.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_port_header_type_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                    core_id,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_OUT SOC_PETRA_PORT_HEADER_TYPE    *header_type_incoming,
    SOC_SAND_OUT SOC_PETRA_PORT_HEADER_TYPE    *header_type_outgoing
  );

/*********************************************************************
* NAME:
*     soc_petra_ports_mirror_inbound_set
* TYPE:
*   PROC
* DATE:
*   Apr 21 2008
* FUNCTION:
*     Configure inbound mirroring for the specified port.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 ifp_ndx -
*     The index of the incoming FAP Port to inbound mirror.
*     Range: 0 - 79.
*  SOC_SAND_IN  SOC_PETRA_PORT_INBOUND_MIRROR_INFO *info -
*     Inbound mirroring configuration.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ports_mirror_inbound_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 ifp_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_INBOUND_MIRROR_INFO *info
  );

/*********************************************************************
* NAME:
*     soc_petra_ports_mirror_inbound_get
* TYPE:
*   PROC
* DATE:
*   Apr 21 2008
* FUNCTION:
*     Configure inbound mirroring for the specified port.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 ifp_ndx -
*     The index of the incoming FAP Port to inbound mirror.
*     Range: 0 - 79.
*  SOC_SAND_OUT SOC_PETRA_PORT_INBOUND_MIRROR_INFO *info -
*     Inbound mirroring configuration.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ports_mirror_inbound_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 ifp_ndx,
    SOC_SAND_OUT SOC_PETRA_PORT_INBOUND_MIRROR_INFO *info
  );

/*********************************************************************
* NAME:
*     soc_petra_ports_mirror_outbound_set
* TYPE:
*   PROC
* DATE:
*   Apr 21 2008
* FUNCTION:
*     Configure outbound mirroring for the specified port.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 ofp_ndx -
*     The index of the outgoing FAP Port to outbound mirror.
*     Range: 0 - 79.
*  SOC_SAND_IN  SOC_PETRA_PORT_OUTBOUND_MIRROR_INFO *info -
*     Outbound mirroring configuration.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ports_mirror_outbound_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 ofp_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_OUTBOUND_MIRROR_INFO *info
  );

/*********************************************************************
* NAME:
*     soc_petra_ports_mirror_outbound_get
* TYPE:
*   PROC
* DATE:
*   Apr 21 2008
* FUNCTION:
*     Configure outbound mirroring for the specified port.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 ofp_ndx -
*     The index of the outgoing FAP Port to outbound mirror.
*     Range: 0 - 79.
*  SOC_SAND_OUT SOC_PETRA_PORT_OUTBOUND_MIRROR_INFO *info -
*     Outbound mirroring configuration.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ports_mirror_outbound_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 ofp_ndx,
    SOC_SAND_OUT SOC_PETRA_PORT_OUTBOUND_MIRROR_INFO *info
  );

/*********************************************************************
* NAME:
*     soc_petra_ports_snoop_set
* TYPE:
*   PROC
* FUNCTION:
*     Configure the snooping function.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 snoop_cmd_ndx -
*     One of the 15 snoop commands. Note that when the packet
*     is received with snoop command 0, it means that the
*     packet is not to be snooped, Therefore there is no
*     meaning to set snoop_cmd_ndx 0. Range: 1-15.
*  SOC_SAND_IN  SOC_PETRA_PORT_SNOOP_INFO     *info -
*     Snooping configuration.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ports_snoop_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 snoop_cmd_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_SNOOP_INFO     *info
  );

/*********************************************************************
* NAME:
*     soc_petra_ports_snoop_get
* TYPE:
*   PROC
* FUNCTION:
*     Configure the snooping function.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 snoop_cmd_ndx -
*     One of the 15 snoop commands. Note that when the packet
*     is received with snoop command 0, it means that the
*     packet is not to be snooped, Therefore there is no
*     meaning to set snoop_cmd_ndx 0. Range: 1-15.
*  SOC_SAND_OUT SOC_PETRA_PORT_SNOOP_INFO     *info -
*     Snooping configuration.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ports_snoop_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 snoop_cmd_ndx,
    SOC_SAND_OUT SOC_PETRA_PORT_SNOOP_INFO     *info
  );

/*********************************************************************
* NAME:
*     soc_petra_ports_itmh_extension_set
* TYPE:
*   PROC
* FUNCTION:
*     This function sets whether a port has an extension added
*     to its ITMH or not.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx -
*     Local port index. Port Index. Range: 0 - 79.
*  SOC_SAND_IN  uint8                 ext_en -
*     Extension Enable/Disable.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ports_itmh_extension_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  uint8                 ext_en
  );

/*********************************************************************
* NAME:
*     soc_petra_ports_itmh_extension_get
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     This function sets whether a port has an extension added
*     to its ITMH or not.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx -
*     Local port index. Port Index. Range: 0 - 79.
*  SOC_SAND_OUT uint8                 *ext_en -
*     Extension Enable/Disable.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ports_itmh_extension_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_OUT uint8                 *ext_en
  );

/*********************************************************************
* NAME:
*     soc_petra_ports_shaping_header_set
* TYPE:
*   PROC
* FUNCTION:
*     Set static ingress shaping configuration per FAP port.
*     A packet is ingress-shaped if the queue id in the
*     IS-ITMH is within the ingress-shaping range (set by the
*     API: 'soc_petra_itm_ingress_shape_set'). The IS-ITMH is
*     stripped of the packet and the ITMH is expected to
*     follow.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx -
*     Local port index. Port Index. Range: 0 - 79.
*  SOC_SAND_IN  SOC_PETRA_PORTS_ISP_INFO      *info -
*     Ingress Shaping header configuration.
* REMARKS:
*     If static shaping is enabled, it is illegal for the
*     incoming packet to have additional IS-ITMH header.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ports_shaping_header_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORTS_ISP_INFO      *info
  );

/*********************************************************************
* NAME:
*     soc_petra_ports_shaping_header_get
* TYPE:
*   PROC
* FUNCTION:
*     Set static ingress shaping configuration per FAP port.
*     A packet is ingress-shaped if the queue id in the
*     IS-ITMH is within the ingress-shaping range (set by the
*     API: 'soc_petra_itm_ingress_shape_set'). The IS-ITMH is
*     stripped of the packet and the ITMH is expected to
*     follow.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx -
*     Local port index. Port Index. Range: 0 - 79.
*  SOC_SAND_OUT SOC_PETRA_PORTS_ISP_INFO      *info -
*     Ingress Shaping header configuration.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ports_shaping_header_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_OUT SOC_PETRA_PORTS_ISP_INFO      *info
  );

/*********************************************************************
* NAME:
*     soc_petra_ports_forwarding_header_set
* TYPE:
*   PROC
* FUNCTION:
*     Set a raw port with the ITMH to be added to the incoming
*     packets.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx -
*     Local port index. Port Index. Range: 0 - 79.
*  SOC_SAND_IN  SOC_PETRA_PORTS_ITMH          *info -
*     The information that is put in the header inc. the
*     header's extension information.
* REMARKS:
*   The get functions always returns an enable field for the
*   extension set to False. To enable the extension, the
*   user should explicitly sets this field to True.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ports_forwarding_header_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORTS_ITMH          *info
  );

/*********************************************************************
* NAME:
*     soc_petra_ports_forwarding_header_get
* TYPE:
*   PROC
* FUNCTION:
*     Set a raw port with the ITMH to be added to the incoming
*     packets.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx -
*     Local port index. Port Index. Range: 0 - 79.
*  SOC_SAND_OUT SOC_PETRA_PORTS_ITMH          *info -
*     The information that is put in the header inc. the
*     header's extension information.
* REMARKS:
*     The get functions always returns an enable field for the extension
*     set to False. To enable the extension, the user should explicitly
*     sets this field to True.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ports_forwarding_header_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_OUT SOC_PETRA_PORTS_ITMH          *info
  );

/*********************************************************************
* NAME:
*     soc_petra_ports_stag_set
* TYPE:
*   PROC
* FUNCTION:
*     The Statistics-Tag is a configurable collection of
*     fields and various packet attributes copied from the
*     packet header. For each field, there is a per Incoming
*     FAP Port selector, indicating whether to add the field
*     to the tag or to omit it.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx -
*     Local port index. Port Index. Range: 0 - 79.
*  SOC_SAND_IN  SOC_PETRA_PORTS_STAG_FIELDS   *info -
*     Fields enablers/disablers.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ports_stag_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORTS_STAG_FIELDS   *info
  );

/*********************************************************************
* NAME:
*     soc_petra_ports_stag_get
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     The Statistics-Tag is a configurable collection of
*     fields and various packet attributes copied from the
*     packet header. For each field, there is a per Incoming
*     FAP Port selector, indicating whether to add the field
*     to the tag or to omit it.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx -
*     Local port index. Port Index. Range: 0 - 79.
*  SOC_SAND_OUT SOC_PETRA_PORTS_STAG_FIELDS   *info -
*     Fields enablers/disablers.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ports_stag_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_OUT SOC_PETRA_PORTS_STAG_FIELDS   *info
  );

/*********************************************************************
* NAME:
*     soc_petra_ports_ftmh_extension_set
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
*  SOC_SAND_IN  SOC_PETRA_PORTS_FTMH_EXT_OUTLIF ext_option -
*     There 3 options for the FTMH extension.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ports_ftmh_extension_set(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PETRA_PORTS_FTMH_EXT_OUTLIF ext_option
  );

/*********************************************************************
* NAME:
*     soc_petra_ports_ftmh_extension_get
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
  soc_petra_ports_ftmh_extension_get(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_PETRA_PORTS_FTMH_EXT_OUTLIF *ext_option
  );

/*********************************************************************
* NAME:
*     soc_petra_ports_otmh_extension_set
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     This function sets what extensions are to be added to
*     the OTMH per port. The OTMH has 3 optional extensions:
*     Outlif (always allow/ never allow/ allow only when the
*     packet is multicast.) Source Sys-Port and Destination
*     Sys-Port.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx -
*     Local port index. Port Index. Range: 0 - 79.
*  SOC_SAND_IN  SOC_PETRA_PORTS_OTMH_EXTENSIONS_EN *info -
*     There 3 options for the OTMH-outlif extension, src-port
*     & dest-port extensions Enable/Disable.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ports_otmh_extension_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORTS_OTMH_EXTENSIONS_EN *info
  );

/*********************************************************************
* NAME:
*     soc_petra_ports_otmh_extension_get
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     This function sets what extensions are to be added to
*     the OTMH per port. The OTMH has 3 optional extensions:
*     Outlif (always allow/ never allow/ allow only when the
*     packet is multicast.) Source Sys-Port and Destination
*     Sys-Port.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx -
*     Local port index. Port Index. Range: 0 - 79.
*  SOC_SAND_OUT SOC_PETRA_PORTS_OTMH_EXTENSIONS_EN *info -
*     There 3 options for the OTMH-outlif extension, src-port
*     & dest-port extensions Enable/Disable.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ports_otmh_extension_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_OUT SOC_PETRA_PORTS_OTMH_EXTENSIONS_EN *info
  );

/*********************************************************************
* NAME:
*     soc_petra_port_egr_hdr_credit_discount_type_set
* TYPE:
*   PROC
* FUNCTION:
*     Per discount type, set the available egress credit
*     compensation value to adjust the credit rate for the
*     various headers: PP (if present), FTMH, DRAM-CRC,
*     Ethernet-IPG, NIF-CRC.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_PORT_HEADER_TYPE    port_hdr_type_ndx -
*     The port header type for which the credit discount is
*     configured (TM/ETH/RAW).
*  SOC_SAND_IN  SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_TYPE cr_discnt_type_ndx -
*     The preset (A/B) that is configured.
*  SOC_SAND_IN  SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_INFO *info -
*     The discount values (signed, can be negative) for the
*     specified discount type and port header type.
* REMARKS:
*     1. The configuration is per port header type. In
*     practice, each port will use the configuration
*     accordingly to its header type. For example, if all
*     ports are TM-ports, the configuration for ETH ports is
*     irrelevant. 2. Credit discount should also be configured
*     in the ingress, using soc_petra_itm_cr_discount_set API. 3.
*     This API only configures the available presets. The
*     specific preset that is used, per port, is configured
*     using the soc_petra_port_egr_hdr_credit_discount_select_set
*     API.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_port_egr_hdr_credit_discount_type_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  SOC_PETRA_PORT_HEADER_TYPE    port_hdr_type_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_TYPE cr_discnt_type_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_INFO *info
  );

/*********************************************************************
* NAME:
*     soc_petra_port_egr_hdr_credit_discount_type_get
* TYPE:
*   PROC
* FUNCTION:
*     Per discount type, set the available egress credit
*     compensation value to adjust the credit rate for the
*     various headers: PP (if present), FTMH, DRAM-CRC,
*     Ethernet-IPG, NIF-CRC.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_PORT_HEADER_TYPE    port_hdr_type_ndx -
*     The port header type for which the credit discount is
*     configured (TM/ETH/RAW).
*  SOC_SAND_IN  SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_TYPE cr_discnt_type_ndx -
*     The preset (A/B) that is configured.
*  SOC_SAND_OUT SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_INFO *info -
*     The discount values (signed, can be negative) for the
*     specified discount type and port header type.
* REMARKS:
*     1. The configuration is per port header type. In
*     practice, each port will use the configuration
*     accordingly to its header type. For example, if all
*     ports are TM-ports, the configuration for ETH ports is
*     irrelevant. 2. Credit discount should also be configured
*     in the ingress, using soc_petra_itm_cr_discount_set API. 3.
*     This API only configures the available presets. The
*     specific preset that is used, per port, is configured
*     using the soc_petra_port_egr_hdr_credit_discount_select_set
*     API.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_port_egr_hdr_credit_discount_type_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_PORT_HEADER_TYPE    port_hdr_type_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_TYPE cr_discnt_type_ndx,
    SOC_SAND_OUT SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_INFO *info
  );

/*********************************************************************
* NAME:
*     soc_petra_port_egr_hdr_credit_discount_select_set
* TYPE:
*   PROC
* FUNCTION:
*     Select from the available egress credit compensation
*     values to adjust the credit rate for the various
*     headers: PP (if present), FTMH, DRAM-CRC, Ethernet-IPG,
*     NIF-CRC. This API selects the discount type. The values
*     per port header type and discount type are configured
*     using soc_petra_port_egr_hdr_credit_discount_type_set API.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 port_ndx -
*     The index of the port to configure. Range: 0 - 79.
*  SOC_SAND_IN  SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_TYPE cr_discnt_type -
*     The preset (A/B) that is selected.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_port_egr_hdr_credit_discount_select_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_IN  SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_TYPE cr_discnt_type
  );

/*********************************************************************
* NAME:
*     soc_petra_port_egr_hdr_credit_discount_select_get
* TYPE:
*   PROC
* FUNCTION:
*     Select from the available egress credit compensation
*     values to adjust the credit rate for the various
*     headers: PP (if present), FTMH, DRAM-CRC, Ethernet-IPG,
*     NIF-CRC. This API selects the discount type. The values
*     per port header type and discount type are configured
*     using soc_petra_port_egr_hdr_credit_discount_type_set API.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 port_ndx -
*     The index of the port to configure. Range: 0 - 79.
*  SOC_SAND_OUT SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_TYPE *cr_discnt_type -
*     The preset (A/B) that is selected.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_port_egr_hdr_credit_discount_select_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32                 port_ndx,
    SOC_SAND_OUT SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_TYPE *cr_discnt_type
  );

/*
 *  Get the MAL index or equivalent of the MAL
 *  to which the given port is mapped at the egress
 */
uint32
  soc_petra_port_ofp_mal_get(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID ofp_ndx,
    SOC_SAND_OUT uint32         *mal_ndx
  );

void
  soc_petra_PETRA_PORT2IF_MAPPING_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_PORT2IF_MAPPING_INFO *info
  );

void
  soc_petra_PETRA_PORTS_LAG_MEMBER_clear(
    SOC_SAND_OUT SOC_PETRA_PORTS_LAG_MEMBER *info
  );

void
  soc_petra_PETRA_PORTS_LAG_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_PORTS_LAG_INFO *info
  );

void
  soc_petra_PETRA_PORTS_OVERRIDE_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_PORTS_OVERRIDE_INFO *info
  );

void
  soc_petra_PETRA_PORT_INBOUND_MIRROR_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_PORT_INBOUND_MIRROR_INFO *info
  );

void
  soc_petra_PETRA_PORT_OUTBOUND_MIRROR_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_PORT_OUTBOUND_MIRROR_INFO *info
  );

void
  soc_petra_PETRA_PORT_SNOOP_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_PORT_SNOOP_INFO *info
  );

void
  soc_petra_PETRA_PORTS_ITMH_BASE_clear(
    SOC_SAND_OUT SOC_PETRA_PORTS_ITMH_BASE *info
  );

void
  soc_petra_PETRA_PORTS_ITMH_EXT_SRC_PORT_clear(
    SOC_SAND_OUT SOC_PETRA_PORTS_ITMH_EXT_SRC_PORT *info
  );

void
  soc_petra_PETRA_PORTS_ITMH_clear(
    SOC_SAND_OUT SOC_PETRA_PORTS_ITMH *info
  );

void
  soc_petra_PETRA_PORTS_ISP_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_PORTS_ISP_INFO *info
  );

void
  soc_petra_PETRA_PORT_LAG_SYS_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_PORT_LAG_SYS_PORT_INFO *info
  );


void
  soc_petra_PETRA_PORTS_STAG_FIELDS_clear(
    SOC_SAND_OUT SOC_PETRA_PORTS_STAG_FIELDS   *info
  );

void
  soc_petra_PETRA_PORTS_OTMH_EXTENSIONS_EN_clear(
    SOC_SAND_OUT SOC_PETRA_PORTS_OTMH_EXTENSIONS_EN *info
  );

void
  soc_petra_PETRA_PORT_EGR_HDR_CR_DISCOUNT_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_INFO *info
  );
#if SOC_PETRA_DEBUG_IS_LVL1


const char*
  soc_petra_PETRA_PORT_DIRECTION_to_string(
    SOC_SAND_IN SOC_PETRA_PORT_DIRECTION enum_val
  );



const char*
  soc_petra_PETRA_PORT_HEADER_TYPE_to_string(
    SOC_SAND_IN SOC_PETRA_PORT_HEADER_TYPE enum_val
  );



const char*
  soc_petra_PETRA_PORTS_SNOOP_SIZE_to_string(
    SOC_SAND_IN SOC_PETRA_PORTS_SNOOP_SIZE enum_val
  );



const char*
  soc_petra_PETRA_PORTS_FTMH_EXT_OUTLIF_to_string(
    SOC_SAND_IN SOC_PETRA_PORTS_FTMH_EXT_OUTLIF enum_val
  );



const char*
  soc_petra_PETRA_PORT_EGR_HDR_CR_DISCOUNT_TYPE_to_string(
    SOC_SAND_IN SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_TYPE enum_val
  );



void
  soc_petra_PETRA_PORT2IF_MAPPING_INFO_print(
    SOC_SAND_IN SOC_PETRA_PORT2IF_MAPPING_INFO *info
  );

void
  soc_petra_PETRA_PORTS_LAG_MEMBER_print(
    SOC_SAND_IN  SOC_PETRA_PORTS_LAG_MEMBER *info
  );

void
  soc_petra_PETRA_PORTS_LAG_INFO_print(
    SOC_SAND_IN SOC_PETRA_PORTS_LAG_INFO *info
  );



void
  soc_petra_PETRA_PORTS_OVERRIDE_INFO_print(
    SOC_SAND_IN SOC_PETRA_PORTS_OVERRIDE_INFO *info
  );



void
  soc_petra_PETRA_PORT_INBOUND_MIRROR_INFO_print(
    SOC_SAND_IN SOC_PETRA_PORT_INBOUND_MIRROR_INFO *info
  );



void
  soc_petra_PETRA_PORT_OUTBOUND_MIRROR_INFO_print(
    SOC_SAND_IN SOC_PETRA_PORT_OUTBOUND_MIRROR_INFO *info
  );

void
  soc_petra_PETRA_PORT_SNOOP_INFO_print(
    SOC_SAND_IN SOC_PETRA_PORT_SNOOP_INFO *info
  );


void
  soc_petra_PETRA_PORTS_ITMH_BASE_print(
    SOC_SAND_IN SOC_PETRA_PORTS_ITMH_BASE *info
  );


void
  soc_petra_PETRA_PORT_LAG_SYS_PORT_INFO_print(
    SOC_SAND_IN SOC_PETRA_PORT_LAG_SYS_PORT_INFO *info
  );


void
  soc_petra_PETRA_PORTS_ITMH_EXT_SRC_PORT_print(
    SOC_SAND_IN SOC_PETRA_PORTS_ITMH_EXT_SRC_PORT *info
  );



void
  soc_petra_PETRA_PORTS_ITMH_print(
    SOC_SAND_IN SOC_PETRA_PORTS_ITMH *info
  );



void
  soc_petra_PETRA_PORTS_ISP_INFO_print(
    SOC_SAND_IN SOC_PETRA_PORTS_ISP_INFO *info
  );



void
  soc_petra_PETRA_PORTS_STAG_FIELDS_print(
    SOC_SAND_IN SOC_PETRA_PORTS_STAG_FIELDS   *info
  );



void
  soc_petra_PETRA_PORTS_OTMH_EXTENSIONS_EN_print(
    SOC_SAND_IN SOC_PETRA_PORTS_OTMH_EXTENSIONS_EN *info
  );



void
  soc_petra_PETRA_PORT_EGR_HDR_CR_DISCOUNT_INFO_print(
    SOC_SAND_IN SOC_PETRA_PORT_EGR_HDR_CR_DISCOUNT_INFO *info
  );


#endif /* SOC_PETRA_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PETRA_API_PORTS_INCLUDED__*/
#endif

