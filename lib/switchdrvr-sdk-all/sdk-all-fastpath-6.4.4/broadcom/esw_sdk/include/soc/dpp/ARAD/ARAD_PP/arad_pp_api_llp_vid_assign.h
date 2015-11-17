/* $Id: arad_pp_api_llp_vid_assign.h,v 1.3 Broadcom SDK $
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

#ifndef __ARAD_PP_API_LLP_VID_ASSIGN_INCLUDED__
/* { */
#define __ARAD_PP_API_LLP_VID_ASSIGN_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_llp_vid_assign.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_llp_sa_auth.h>

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

#define ARAD_PP_LLP_VID_ASSIGN_MATCH_RULE_TYPE_TAGGED        SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE_TYPE_TAGGED
#define ARAD_PP_LLP_VID_ASSIGN_MATCH_RULE_TYPE_UNTAGGED      SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE_TYPE_UNTAGGED
#define ARAD_PP_LLP_VID_ASSIGN_MATCH_RULE_TYPE_ALL           SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE_TYPE_ALL
#define ARAD_PP_NOF_LLP_VID_ASSIGN_MATCH_RULE_TYPES          SOC_PPC_NOF_LLP_VID_ASSIGN_MATCH_RULE_TYPES
typedef SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE_TYPE                 ARAD_PP_LLP_VID_ASSIGN_MATCH_RULE_TYPE;

typedef SOC_PPC_LLP_VID_ASSIGN_PORT_INFO                       ARAD_PP_LLP_VID_ASSIGN_PORT_INFO;
typedef SOC_PPC_LLP_VID_ASSIGN_MAC_INFO                        ARAD_PP_LLP_VID_ASSIGN_MAC_INFO;
typedef SOC_PPC_LLP_VID_ASSIGN_IPV4_SUBNET_INFO                ARAD_PP_LLP_VID_ASSIGN_IPV4_SUBNET_INFO;
typedef SOC_PPC_LLP_VID_ASSIGN_PRTCL_INFO                      ARAD_PP_LLP_VID_ASSIGN_PRTCL_INFO;
typedef SOC_PPC_LLP_VID_ASSIGN_MATCH_RULE                      ARAD_PP_LLP_VID_ASSIGN_MATCH_RULE;

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
 *   arad_pp_llp_vid_assign_port_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets port information for VID assignment, including PVID
 *   and which assignment mechanism to enable for this port.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx -
 *     Local port ID.
 *   SOC_SAND_IN  ARAD_PP_LLP_VID_ASSIGN_PORT_INFO            *port_vid_assign_info -
 *     The port information for VID assignment.
 * REMARKS:
 *   OK or Error indicationREMARKS
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_llp_vid_assign_port_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  ARAD_PP_LLP_VID_ASSIGN_PORT_INFO            *port_vid_assign_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_llp_vid_assign_port_info_set" API.
 *     Refer to "arad_pp_llp_vid_assign_port_info_set" API for
 *     details.
*********************************************************************/
uint32
  arad_pp_llp_vid_assign_port_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  ARAD_PP_PORT                                local_port_ndx,
    SOC_SAND_OUT ARAD_PP_LLP_VID_ASSIGN_PORT_INFO            *port_vid_assign_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_llp_vid_assign_mac_based_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set assignment to VLAN ID according to source MAC
 *   address.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *mac_address_key -
 *     MAC address, according to which to assign VID.
 *   SOC_SAND_IN  ARAD_PP_LLP_VID_ASSIGN_MAC_INFO             *mac_based_info -
 *     VID assignment information for the given MAC.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success -
 *     Negated when there is no place in the VID assignment
 *     SA-based DB.
 * REMARKS:
 *   - The DB used for VID assignment is shared with
 *   SA-Authorization. When setting Authorization over a SA
 *   MAC according to VID, this is also the VID that may be
 *   used for VID assignment.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_llp_vid_assign_mac_based_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *mac_address_key,
    SOC_SAND_IN  ARAD_PP_LLP_VID_ASSIGN_MAC_INFO             *mac_based_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

/*********************************************************************
* NAME:
 *   arad_pp_llp_vid_assign_mac_based_remove
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove assignment to VLAN ID according to source MAC
 *   address.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *mac_address_key -
 *     MAC address, according to which to assign the VID.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_llp_vid_assign_mac_based_remove(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *mac_address_key
  );

/*********************************************************************
* NAME:
 *   arad_pp_llp_vid_assign_mac_based_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get VLAN ID assignment information according to source
 *   MAC address.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *mac_address_key -
 *     MAC address, according to which to assign VID.
 *   SOC_SAND_OUT ARAD_PP_LLP_VID_ASSIGN_MAC_INFO             *mac_based_info -
 *     VID assignment information for the given MAC.
 *   SOC_SAND_OUT uint8                                 *found -
 *     Is mac_address_key was found
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_llp_vid_assign_mac_based_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_MAC_ADDRESS                       *mac_address_key,
    SOC_SAND_OUT ARAD_PP_LLP_VID_ASSIGN_MAC_INFO             *mac_based_info,
    SOC_SAND_OUT uint8                                 *found
  );

/*********************************************************************
* NAME:
 *   arad_pp_llp_vid_assign_mac_based_get_block
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get VLAN ID assignment information according to source
 *   MAC address.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_LLP_VID_ASSIGN_MATCH_RULE           *rule -
 *     Get only entries that match this rule.
 *   SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range -
 *     Range for iteration
 *   SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS                       *mac_address_key_arr -
 *     MAC address array.
 *   SOC_SAND_OUT ARAD_PP_LLP_VID_ASSIGN_MAC_INFO             *vid_assign_info_arr -
 *     VID assignment information
 *   SOC_SAND_OUT uint32                                  *nof_entries -
 *     Number of returned enties in vid_assign_info_arr
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_llp_vid_assign_mac_based_get_block(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_LLP_VID_ASSIGN_MATCH_RULE           *rule,
    SOC_SAND_INOUT SOC_SAND_TABLE_BLOCK_RANGE                    *block_range,
    SOC_SAND_OUT SOC_SAND_PP_MAC_ADDRESS                       *mac_address_key_arr,
    SOC_SAND_OUT ARAD_PP_LLP_VID_ASSIGN_MAC_INFO             *vid_assign_info_arr,
    SOC_SAND_OUT uint32                                  *nof_entries
  );

/*********************************************************************
* NAME:
 *   arad_pp_llp_vid_assign_ipv4_subnet_based_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set assignment of VLAN ID based on source IPv4 subnet.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  entry_ndx -
 *     Index in the subnet table where to write the
 *     information. Range: AradB: 0 - 15. T20E: 0 - 23.
 *   SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET                       *subnet -
 *     Source Ipv4 Subnet.
 *   SOC_SAND_IN  ARAD_PP_LLP_VID_ASSIGN_IPV4_SUBNET_INFO     *subnet_based_info -
 *     The VLAN assignment Information according to subnet.
 * REMARKS:
 *   - At most one subnet can be set for each etrny_ndx.- The
 *   domain of entry_ndx is shared for VID assignment and TC
 *   mapping, so when use soc_ppd_llp_cos_ipv4_subnet_based_set
 *   API it may override the subnet and entry_is_valid.- If
 *   there is more than one match in the table, then the
 *   entry with the lower index will be taken.- In Arad-B:
 *   lower index with match will be taken if the
 *   is_vid_valid/is_tc_valid of the first matched entry is
 *   FALSE, and then the traverse over the subnets will stop
 *   and the TC value will not be changed.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_llp_vid_assign_ipv4_subnet_based_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET                       *subnet,
    SOC_SAND_IN  ARAD_PP_LLP_VID_ASSIGN_IPV4_SUBNET_INFO     *subnet_based_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_llp_vid_assign_ipv4_subnet_based_set" API.
 *     Refer to "arad_pp_llp_vid_assign_ipv4_subnet_based_set"
 *     API for details.
*********************************************************************/
uint32
  arad_pp_llp_vid_assign_ipv4_subnet_based_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_OUT SOC_SAND_PP_IPV4_SUBNET                       *subnet,
    SOC_SAND_OUT ARAD_PP_LLP_VID_ASSIGN_IPV4_SUBNET_INFO     *subnet_based_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_llp_vid_assign_protocol_based_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets mapping from Ethernet Type and profile to VID.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  port_profile_ndx -
 *     Profile for VID assignment (and COS resolution)
 *     according to L2 Protocol (Ethernet Type). Profile is
 *     determined per port see soc_ppd_port_info_set(). Range: 0 -
 *     7.
 *   SOC_SAND_IN  uint16                                  ether_type_ndx -
 *     Ethernet Type, up to 10 different Ethernet Types,
 *     including
 *     IPv4(0x0800),IPv6(0x86DD),ARP(0x0806),CFM(0x8902), TRILL
 *     MPLS(0x8847). Use SOC_PPD_L2_NEXT_PRTCL_TYPE enumeration to
 *     refer to these constant values.
 *   SOC_SAND_IN  ARAD_PP_LLP_VID_ASSIGN_PRTCL_INFO           *prtcl_assign_info -
 *     VID assignment information according to Ethernet Type.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success -
 *     Whether the set operation succeeded. Operation may fail
 *     if there are no available resources to support the given
 *     (new) Ethernet Type.
 * REMARKS:
 *   - This API uses the same DB used for TC assignment
 *   according to L2 protocol. See
 *   soc_ppd_llp_vid_assign_protocol_based_set()
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_llp_vid_assign_protocol_based_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  uint16                                  ether_type_ndx,
    SOC_SAND_IN  ARAD_PP_LLP_VID_ASSIGN_PRTCL_INFO           *prtcl_assign_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_llp_vid_assign_protocol_based_set" API.
 *     Refer to "arad_pp_llp_vid_assign_protocol_based_set" API
 *     for details.
*********************************************************************/
uint32
  arad_pp_llp_vid_assign_protocol_based_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_profile_ndx,
    SOC_SAND_IN  uint16                                  ether_type_ndx,
    SOC_SAND_OUT ARAD_PP_LLP_VID_ASSIGN_PRTCL_INFO           *prtcl_assign_info
  );

void
  ARAD_PP_LLP_VID_ASSIGN_PORT_INFO_clear(
    SOC_SAND_OUT ARAD_PP_LLP_VID_ASSIGN_PORT_INFO *info
  );

void
  ARAD_PP_LLP_VID_ASSIGN_MAC_INFO_clear(
    SOC_SAND_OUT ARAD_PP_LLP_VID_ASSIGN_MAC_INFO *info
  );

void
  ARAD_PP_LLP_VID_ASSIGN_IPV4_SUBNET_INFO_clear(
    SOC_SAND_OUT ARAD_PP_LLP_VID_ASSIGN_IPV4_SUBNET_INFO *info
  );

void
  ARAD_PP_LLP_VID_ASSIGN_PRTCL_INFO_clear(
    SOC_SAND_OUT ARAD_PP_LLP_VID_ASSIGN_PRTCL_INFO *info
  );

void
  ARAD_PP_LLP_VID_ASSIGN_MATCH_RULE_clear(
    SOC_SAND_OUT ARAD_PP_LLP_VID_ASSIGN_MATCH_RULE *info
  );

#if ARAD_PP_DEBUG_IS_LVL1

const char*
  ARAD_PP_LLP_VID_ASSIGN_MATCH_RULE_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_LLP_VID_ASSIGN_MATCH_RULE_TYPE enum_val
  );

void
  ARAD_PP_LLP_VID_ASSIGN_PORT_INFO_print(
    SOC_SAND_IN  ARAD_PP_LLP_VID_ASSIGN_PORT_INFO *info
  );

void
  ARAD_PP_LLP_VID_ASSIGN_MAC_INFO_print(
    SOC_SAND_IN  ARAD_PP_LLP_VID_ASSIGN_MAC_INFO *info
  );

void
  ARAD_PP_LLP_VID_ASSIGN_IPV4_SUBNET_INFO_print(
    SOC_SAND_IN  ARAD_PP_LLP_VID_ASSIGN_IPV4_SUBNET_INFO *info
  );

void
  ARAD_PP_LLP_VID_ASSIGN_PRTCL_INFO_print(
    SOC_SAND_IN  ARAD_PP_LLP_VID_ASSIGN_PRTCL_INFO *info
  );

void
  ARAD_PP_LLP_VID_ASSIGN_MATCH_RULE_print(
    SOC_SAND_IN  ARAD_PP_LLP_VID_ASSIGN_MATCH_RULE *info
  );

#endif /* ARAD_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_PP_API_LLP_VID_ASSIGN_INCLUDED__*/
#endif

