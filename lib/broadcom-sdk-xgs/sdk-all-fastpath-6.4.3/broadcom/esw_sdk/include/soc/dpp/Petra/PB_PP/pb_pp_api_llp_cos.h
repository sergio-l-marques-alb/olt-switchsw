/* $Id: pb_pp_api_llp_cos.h,v 1.6 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/include/soc_pb_pp_api_llp_cos.h
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

#ifndef __SOC_PB_PP_API_LLP_COS_INCLUDED__
/* { */
#define __SOC_PB_PP_API_LLP_COS_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_llp_cos.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_api_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     use to indicates no matching source/destination l4 ports
 *     ranges                                                  */

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

#define SOC_PB_PP_LLP_COS_MAPPING_TABLE_UP_TO_DE_TC            SOC_PPC_LLP_COS_MAPPING_TABLE_UP_TO_DE_TC
#define SOC_PB_PP_LLP_COS_MAPPING_TABLE_IPV4_TOS_TO_DP_TC_VALID SOC_PPC_LLP_COS_MAPPING_TABLE_IPV4_TOS_TO_DP_TC_VALID
#define SOC_PB_PP_LLP_COS_MAPPING_TABLE_IPV6_TC_TO_DP_TC_VALID SOC_PPC_LLP_COS_MAPPING_TABLE_IPV6_TC_TO_DP_TC_VALID
#define SOC_PB_PP_LLP_COS_MAPPING_TABLE_INCOMING_UP_TO_UP      SOC_PPC_LLP_COS_MAPPING_TABLE_INCOMING_UP_TO_UP
#define SOC_PB_PP_LLP_COS_MAPPING_TABLE_TC_TO_UP               SOC_PPC_LLP_COS_MAPPING_TABLE_TC_TO_UP
#define SOC_PB_PP_LLP_COS_MAPPING_TABLE_DE_TO_DP               SOC_PPC_LLP_COS_MAPPING_TABLE_DE_TO_DP
#define SOC_PB_PP_LLP_COS_MAPPING_TABLE_UP_TO_DP               SOC_PPC_LLP_COS_MAPPING_TABLE_UP_TO_DP
#define SOC_PB_PP_NOF_LLP_COS_MAPPING_TABLES                   SOC_PPC_NOF_LLP_COS_MAPPING_TABLES
typedef SOC_PPC_LLP_COS_MAPPING_TABLE                          SOC_PB_PP_LLP_COS_MAPPING_TABLE;

typedef SOC_PPC_LLP_COS_GLBL_INFO                              SOC_PB_PP_LLP_COS_GLBL_INFO;
typedef SOC_PPC_LLP_COS_TC_INFO                                SOC_PB_PP_LLP_COS_TC_INFO;
typedef SOC_PPC_LLP_COS_UP_USE                                 SOC_PB_PP_LLP_COS_UP_USE;
typedef SOC_PPC_LLP_COS_PORT_L2_TABLE_INDEXES                  SOC_PB_PP_LLP_COS_PORT_L2_TABLE_INDEXES;
typedef SOC_PPC_LLP_COS_PORT_L2_INFO                           SOC_PB_PP_LLP_COS_PORT_L2_INFO;
typedef SOC_PPC_LLP_COS_PORT_L3_INFO                           SOC_PB_PP_LLP_COS_PORT_L3_INFO;
typedef SOC_PPC_LLP_COS_PORT_L4_INFO                           SOC_PB_PP_LLP_COS_PORT_L4_INFO;
typedef SOC_PPC_LLP_COS_PORT_INFO                              SOC_PB_PP_LLP_COS_PORT_INFO;
typedef SOC_PPC_LLP_COS_MAPPING_TABLE_ENTRY_INFO               SOC_PB_PP_LLP_COS_MAPPING_TABLE_ENTRY_INFO;
typedef SOC_PPC_LLP_COS_IPV4_SUBNET_INFO                       SOC_PB_PP_LLP_COS_IPV4_SUBNET_INFO;
typedef SOC_PPC_LLP_COS_PRTCL_INFO                             SOC_PB_PP_LLP_COS_PRTCL_INFO;
typedef SOC_PPC_LLP_COS_L4_PORT_RANGE_INFO                     SOC_PB_PP_LLP_COS_L4_PORT_RANGE_INFO;

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
 *   soc_pb_pp_llp_cos_glbl_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets global information for COS resolution, including
 *   default drop precedence.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_LLP_COS_GLBL_INFO                   *glbl_info -
 *     Global information for COS resolution.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_llp_cos_glbl_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_GLBL_INFO                   *glbl_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_llp_cos_glbl_info_set" API.
 *     Refer to "soc_pb_pp_llp_cos_glbl_info_set" API for details.
*********************************************************************/
uint32
  soc_pb_pp_llp_cos_glbl_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_LLP_COS_GLBL_INFO                   *glbl_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_llp_cos_port_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets port information for COS resolution, including
 *   which mechanisms to perform, table instance to use for
 *   mapping,...
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx -
 *     Local port ID.
 *   SOC_SAND_IN  SOC_PB_PP_LLP_COS_PORT_INFO                   *port_info -
 *     Port information for COS resolution.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_llp_cos_port_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_PORT_INFO                   *port_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_llp_cos_port_info_set" API.
 *     Refer to "soc_pb_pp_llp_cos_port_info_set" API for details.
*********************************************************************/
uint32
  soc_pb_pp_llp_cos_port_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_OUT SOC_PB_PP_LLP_COS_PORT_INFO                   *port_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_llp_cos_mapping_table_entry_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set mapping from packet fields to TC/DP, includes the
 *   following mappings:- UP to DE and TC.- IPv4 TOS to DP
 *   and TC.- IPv6 TC to DP and TC.- DE to DP- UP to DP.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_LLP_COS_MAPPING_TABLE               mapping_tbl_ndx -
 *     Selects the mapping table to modify (TOS to TC, UP to TC
 *     etc...)
 *   SOC_SAND_IN  uint32                                  table_id_ndx -
 *     Selects the instance of the table. One mapping may have
 *     more than one table, for example, TOS to TC has two
 *     tables, and this parameter selects one of these tables.
 *     Each port is assigned one of these tables to be used -
 *     see soc_ppd_llp_cos_port_info_set()
 *   SOC_SAND_IN  uint32                                  entry_ndx -
 *     Mapped value may be UP/TOS/TC/DE/EXP according to the
 *     selected table.
 *   SOC_SAND_IN  SOC_PB_PP_LLP_COS_MAPPING_TABLE_ENTRY_INFO    *entry -
 *     The value to map to. May be TC or DP according to the
 *     selected table.
 * REMARKS:
 *   - The order of values in the entry (value1 and value2)
 *   is determined according to their appearance in the
 *   Enumerator SOC_PPD_LLP_COS_MAPPING_TABLE - Example for
 *   mapping IPv4 To TC and DP:
 *   mapping_tbl_ndx=SOC_PPD_LLP_COS_MAPPING_TABLE_IPV4_TOS_TO_DP_TC_VALID
 *   table_id_ndx = 0 or 1 entry_ndx = 0..255 (this is
 *   actually the mapped TOS) entry.value1 = 0...3 (DP value)
 *   entry.value2 = 0...7 (TC value) entry.valid = is TC valid
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_llp_cos_mapping_table_entry_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_MAPPING_TABLE               mapping_tbl_ndx,
    SOC_SAND_IN  uint32                                  table_id_ndx,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_MAPPING_TABLE_ENTRY_INFO    *entry
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_llp_cos_mapping_table_entry_set" API.
 *     Refer to "soc_pb_pp_llp_cos_mapping_table_entry_set" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_llp_cos_mapping_table_entry_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_MAPPING_TABLE               mapping_tbl_ndx,
    SOC_SAND_IN  uint32                                  table_id_ndx,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_OUT SOC_PB_PP_LLP_COS_MAPPING_TABLE_ENTRY_INFO    *entry
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_llp_cos_l4_port_range_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets range of L4 ports. Packets L4 ports will be
 *   compared to this range in order to set TC value
 *   correspondingly.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  range_ndx -
 *     Range ID. Range: 0 - 2.
 *   SOC_SAND_IN  SOC_PB_PP_LLP_COS_L4_PORT_RANGE_INFO          *range_info -
 *     Range of L4 ports.
 * REMARKS:
 *   - Lower value range_ndx has priority over higher value.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_llp_cos_l4_port_range_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  range_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_L4_PORT_RANGE_INFO          *range_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_llp_cos_l4_port_range_info_set" API.
 *     Refer to "soc_pb_pp_llp_cos_l4_port_range_info_set" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_llp_cos_l4_port_range_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  range_ndx,
    SOC_SAND_OUT SOC_PB_PP_LLP_COS_L4_PORT_RANGE_INFO          *range_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_llp_cos_l4_port_range_to_tc_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set mapping to Traffic class according to L4 information
 *   includingSource/Destination and protocol type (TCP or
 *   UDP). Packet L4 destination and source ports are
 *   compared to the ranges set by
 *   soc_ppd_llp_cos_l4_port_range_info_set(). Then according to
 *   comparison result: - 0-3 for source port (where 3 means
 *   was not match) - 0-3 for destination port (where 3 means
 *   was not match)and according to protocol type (UDP/TCP),
 *   this API is used to determine the TC value to set to the
 *   packet
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_SAND_PP_L4_PRTCL_TYPE                     l4_prtcl_type_ndx -
 *     L4 Protocol type (UDP/TCP).
 *   SOC_SAND_IN  uint32                                  src_port_match_range_ndx -
 *     The ranges match the packet L4 source port - set to
 *     SOC_PPD_LLP_COS_l4_RANG_RANGE_NO_MATCH for packet with no
 *     match.
 *   SOC_SAND_IN  uint32                                  dest_port_match_range_ndx -
 *     The ranges match the packet L4 destination port - set to
 *     SOC_PPD_LLP_COS_l4_RANG_RANGE_NO_MATCH for packet with no
 *     match.
 *   SOC_SAND_IN  SOC_PB_PP_LLP_COS_TC_INFO                     *tc_info -
 *     TC information value and valid.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_llp_cos_l4_port_range_to_tc_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_L4_PRTCL_TYPE                     l4_prtcl_type_ndx,
    SOC_SAND_IN  uint32                                  src_port_match_range_ndx,
    SOC_SAND_IN  uint32                                  dest_port_match_range_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_TC_INFO                     *tc_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_llp_cos_l4_port_range_to_tc_info_set" API.
 *     Refer to "soc_pb_pp_llp_cos_l4_port_range_to_tc_info_set"
 *     API for details.
*********************************************************************/
uint32
  soc_pb_pp_llp_cos_l4_port_range_to_tc_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_L4_PRTCL_TYPE                     l4_prtcl_type_ndx,
    SOC_SAND_IN  uint32                                  src_port_match_range_ndx,
    SOC_SAND_IN  uint32                                  dest_port_match_range_ndx,
    SOC_SAND_OUT SOC_PB_PP_LLP_COS_TC_INFO                     *tc_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_llp_cos_ipv4_subnet_based_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set COS parameters based on source IPv4 subnet.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  entry_ndx -
 *     Entry index in the subnet table. Range: Soc_petra-B: 0 - 15.
 *     T20E: 0 - 23.
 *   SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET                       *subnet -
 *     Source Ipv4 Subnet.
 *   SOC_SAND_IN  SOC_PB_PP_LLP_COS_IPV4_SUBNET_INFO            *subnet_based_info -
 *     The COS resolution Information according to subnet.
 * REMARKS:
 *   - The domain of entry_ndx is shared for VID assignment
 *   and TC mapping - see
 *   soc_ppd_llp_vid_assign_ipv4_subnet_based_set()- If there is
 *   more than one match in the table, then the entry with
 *   the lower index will be taken.- In Soc_petra-B: lower index
 *   with match will be taken if the is_vid_valid/is_tc_valid
 *   of the first matched entry is FALSE, and then the
 *   traverse over the subnets will stop and the TC value
 *   will not be changed.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_llp_cos_ipv4_subnet_based_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_SUBNET                       *subnet,
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_IPV4_SUBNET_INFO            *subnet_based_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_llp_cos_ipv4_subnet_based_set" API.
 *     Refer to "soc_pb_pp_llp_cos_ipv4_subnet_based_set" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_llp_cos_ipv4_subnet_based_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  entry_ndx,
    SOC_SAND_OUT SOC_SAND_PP_IPV4_SUBNET                       *subnet,
    SOC_SAND_OUT SOC_PB_PP_LLP_COS_IPV4_SUBNET_INFO            *subnet_based_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_llp_cos_protocol_based_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets mapping from Ethernet Type and profile to COS
 *   parameters.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  profile_ndx -
 *     Profile for COS resolution (and VID assignment)
 *     according to L2 Protocol (Ethernet Type). Profile is
 *     determined per port see soc_ppd_port_info_set() Range: 0 -
 *     7.
 *   SOC_SAND_IN  SOC_SAND_PP_ETHER_TYPE                        ether_type_ndx -
 *     Ethernet Type, up to 10 different Ethernet Types,
 *     including
 *     IPv4(0x0800),IPv6(0x86DD),ARP(0x0806),CFM(0x8902), TRILL
 *     MPLS(0x8847). Use SOC_PPD_L2_NEXT_PRTCL_TYPE enumeration to
 *     refer to these constant values.
 *   SOC_SAND_IN  SOC_PB_PP_LLP_COS_PRTCL_INFO                  *prtcl_assign_info -
 *     COS resolution information according to Ethernet Type.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success -
 *     Whether the set operation succeeded. Operation may fail
 *     if there are no available resources to support the given
 *     (new) Ethernet Type.
 * REMARKS:
 *   - This API uses the same DB used for VID protocol-based
 *   assignment. See soc_ppd_llp_vid_assign_protocol_based_set()
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_llp_cos_protocol_based_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_ETHER_TYPE                        ether_type_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_PRTCL_INFO                  *prtcl_assign_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_llp_cos_protocol_based_set" API.
 *     Refer to "soc_pb_pp_llp_cos_protocol_based_set" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_llp_cos_protocol_based_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_ETHER_TYPE                        ether_type_ndx,
    SOC_SAND_OUT SOC_PB_PP_LLP_COS_PRTCL_INFO                  *prtcl_assign_info
  );

void
  SOC_PB_PP_LLP_COS_GLBL_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_LLP_COS_GLBL_INFO *info
  );

void
  SOC_PB_PP_LLP_COS_TC_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_LLP_COS_TC_INFO *info
  );

void
  SOC_PB_PP_LLP_COS_UP_USE_clear(
    SOC_SAND_OUT SOC_PB_PP_LLP_COS_UP_USE *info
  );

void
  SOC_PB_PP_LLP_COS_PORT_L2_TABLE_INDEXES_clear(
    SOC_SAND_OUT SOC_PB_PP_LLP_COS_PORT_L2_TABLE_INDEXES *info
  );

void
  SOC_PB_PP_LLP_COS_PORT_L2_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_LLP_COS_PORT_L2_INFO *info
  );

void
  SOC_PB_PP_LLP_COS_PORT_L3_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_LLP_COS_PORT_L3_INFO *info
  );

void
  SOC_PB_PP_LLP_COS_PORT_L4_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_LLP_COS_PORT_L4_INFO *info
  );

void
  SOC_PB_PP_LLP_COS_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_LLP_COS_PORT_INFO *info
  );

void
  SOC_PB_PP_LLP_COS_MAPPING_TABLE_ENTRY_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_LLP_COS_MAPPING_TABLE_ENTRY_INFO *info
  );

void
  SOC_PB_PP_LLP_COS_IPV4_SUBNET_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_LLP_COS_IPV4_SUBNET_INFO *info
  );

void
  SOC_PB_PP_LLP_COS_PRTCL_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_LLP_COS_PRTCL_INFO *info
  );

void
  SOC_PB_PP_LLP_COS_L4_PORT_RANGE_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_LLP_COS_L4_PORT_RANGE_INFO *info
  );

#if SOC_PB_PP_DEBUG_IS_LVL1

const char*
  SOC_PB_PP_LLP_COS_MAPPING_TABLE_to_string(
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_MAPPING_TABLE enum_val
  );

void
  SOC_PB_PP_LLP_COS_GLBL_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_GLBL_INFO *info
  );

void
  SOC_PB_PP_LLP_COS_TC_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_TC_INFO *info
  );

void
  SOC_PB_PP_LLP_COS_UP_USE_print(
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_UP_USE *info
  );

void
  SOC_PB_PP_LLP_COS_PORT_L2_TABLE_INDEXES_print(
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_PORT_L2_TABLE_INDEXES *info
  );

void
  SOC_PB_PP_LLP_COS_PORT_L2_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_PORT_L2_INFO *info
  );

void
  SOC_PB_PP_LLP_COS_PORT_L3_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_PORT_L3_INFO *info
  );

void
  SOC_PB_PP_LLP_COS_PORT_L4_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_PORT_L4_INFO *info
  );

void
  SOC_PB_PP_LLP_COS_PORT_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_PORT_INFO *info
  );

void
  SOC_PB_PP_LLP_COS_MAPPING_TABLE_ENTRY_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_MAPPING_TABLE_ENTRY_INFO *info
  );

void
  SOC_PB_PP_LLP_COS_IPV4_SUBNET_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_IPV4_SUBNET_INFO *info
  );

void
  SOC_PB_PP_LLP_COS_PRTCL_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_PRTCL_INFO *info
  );

void
  SOC_PB_PP_LLP_COS_L4_PORT_RANGE_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_LLP_COS_L4_PORT_RANGE_INFO *info
  );

#endif /* SOC_PB_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PP_API_LLP_COS_INCLUDED__*/
#endif

