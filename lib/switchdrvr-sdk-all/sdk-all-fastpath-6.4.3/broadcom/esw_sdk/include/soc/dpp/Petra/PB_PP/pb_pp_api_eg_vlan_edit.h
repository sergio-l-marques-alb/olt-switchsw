/* $Id: pb_pp_api_eg_vlan_edit.h,v 1.6 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/include/soc_pb_pp_api_eg_vlan_edit.h
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

#ifndef __SOC_PB_PP_API_EG_VLAN_EDIT_INCLUDED__
/* { */
#define __SOC_PB_PP_API_EG_VLAN_EDIT_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_eg_vlan_edit.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_api_general.h>

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

#define SOC_PB_PP_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY               SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_EMPTY
#define SOC_PB_PP_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG           SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_OUTER_TAG
#define SOC_PB_PP_EG_VLAN_EDIT_TAG_VID_SRC_INNER_TAG           SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_INNER_TAG
#define SOC_PB_PP_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_0         SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_0
#define SOC_PB_PP_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_1         SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_EDIT_INFO_1
#define SOC_PB_PP_EG_VLAN_EDIT_TAG_VID_SRC_ENCAP_INFO          SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC_ENCAP_INFO
#define SOC_PB_PP_NOF_EG_VLAN_EDIT_TAG_VID_SRCS                SOC_PPC_NOF_EG_VLAN_EDIT_TAG_VID_SRCS_PB
typedef SOC_PPC_EG_VLAN_EDIT_TAG_VID_SRC                       SOC_PB_PP_EG_VLAN_EDIT_TAG_VID_SRC;

#define SOC_PB_PP_EG_VLAN_EDIT_TAG_PCP_DEI_NONE                SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_NONE
#define SOC_PB_PP_EG_VLAN_EDIT_TAG_PCP_DEI_SRC_MAP             SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_SRC_MAP
#define SOC_PB_PP_EG_VLAN_EDIT_TAG_PCP_DEI_SRC_OUTER_TAG       SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_SRC_OUTER_TAG
#define SOC_PB_PP_EG_VLAN_EDIT_TAG_PCP_DEI_SRC_INNER_TAG       SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_SRC_INNER_TAG
#define SOC_PB_PP_NOF_EG_VLAN_EDIT_TAG_PCP_DEI_SRCS            SOC_PPC_NOF_EG_VLAN_EDIT_TAG_PCP_DEI_SRCS
typedef SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_SRC                   SOC_PB_PP_EG_VLAN_EDIT_TAG_PCP_DEI_SRC;

#define SOC_PB_PP_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY_TC_DP       SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY_TC_DP
#define SOC_PB_PP_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY_PCP         SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY_PCP
#define SOC_PB_PP_NOF_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEYS        SOC_PPC_NOF_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEYS_PB
typedef SOC_PPC_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY               SOC_PB_PP_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY;

typedef SOC_PPC_EG_VLAN_EDIT_PEP_INFO                          SOC_PB_PP_EG_VLAN_EDIT_PEP_INFO;
typedef SOC_PPC_EG_VLAN_EDIT_COMMAND_KEY                       SOC_PB_PP_EG_VLAN_EDIT_COMMAND_KEY;
typedef SOC_PPC_EG_VLAN_EDIT_COMMAND_TAG_BUILD_INFO            SOC_PB_PP_EG_VLAN_EDIT_COMMAND_TAG_BUILD_INFO;
typedef SOC_PPC_EG_VLAN_EDIT_COMMAND_INFO                      SOC_PB_PP_EG_VLAN_EDIT_COMMAND_INFO;

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
 *   soc_pb_pp_eg_vlan_edit_pep_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets the editing information for PEP port.
 * INPUT:
 *   SOC_SAND_IN  int                                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_PEP_KEY                                 *pep_key -
 *     The key for this PEP editing, out-port x VSI.
 *   SOC_SAND_IN  SOC_PB_PP_EG_VLAN_EDIT_PEP_INFO                   *pep_info -
 *     The editing information for PEP port, including pep-pvid
 *     and UP, and profiles to set the VID and UP.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_eg_vlan_edit_pep_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_PEP_KEY                                 *pep_key,
    SOC_SAND_IN  SOC_PB_PP_EG_VLAN_EDIT_PEP_INFO                   *pep_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_eg_vlan_edit_pep_info_set" API.
 *     Refer to "soc_pb_pp_eg_vlan_edit_pep_info_set" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_eg_vlan_edit_pep_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_PEP_KEY                                 *pep_key,
    SOC_SAND_OUT SOC_PB_PP_EG_VLAN_EDIT_PEP_INFO                   *pep_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_vlan_edit_command_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the Edit command to perform over packets according
 *   to VLAN tags format and Edit profile.
 * INPUT:
 *   SOC_SAND_IN  int                                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_EG_VLAN_EDIT_COMMAND_KEY                *command_key -
 *     Command key including: - packet tag format: specifies
 *     which Tags found on the packet.- Edit profile: assigned
 *     to packet.
 *   SOC_SAND_IN  SOC_PB_PP_EG_VLAN_EDIT_COMMAND_INFO               *command_info -
 *     Edit command to perform on the packets, including number
 *     of tags to remove, and tags to build.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_eg_vlan_edit_command_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_EG_VLAN_EDIT_COMMAND_KEY                *command_key,
    SOC_SAND_IN  SOC_PB_PP_EG_VLAN_EDIT_COMMAND_INFO               *command_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_eg_vlan_edit_command_info_set" API.
 *     Refer to "soc_pb_pp_eg_vlan_edit_command_info_set" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_eg_vlan_edit_command_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  SOC_PB_PP_EG_VLAN_EDIT_COMMAND_KEY                *command_key,
    SOC_SAND_OUT SOC_PB_PP_EG_VLAN_EDIT_COMMAND_INFO               *command_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_vlan_edit_port_vlan_transmit_outer_tag_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   For port x VLAN, set whether to transmit packets from a
 *   specific port tagged or not, according to the outer-tag
 *   VID.
 * INPUT:
 *   SOC_SAND_IN  int                                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx -
 *     Local port ID.
 *   SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx -
 *     The outer VID. Range: 0 - 4095.
 *   SOC_SAND_IN  uint8                                 transmit_outer_tag -
 *     If TRUE, then transmit a packet with outer-tag. If FALSE,
 *     remove the outer tag from the packet header.
 * REMARKS:
 *   - It is ensured that packet has an outer tag either
 *   arriving on the packet or as a result of the former
 *   (egress) editing process.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_eg_vlan_edit_port_vlan_transmit_outer_tag_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_IN  uint8                                 transmit_outer_tag
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_eg_vlan_edit_port_vlan_transmit_outer_tag_set"
 *     API.
 *     Refer to
 *     "soc_pb_pp_eg_vlan_edit_port_vlan_transmit_outer_tag_set"
 *     API for details.
*********************************************************************/
uint32
  soc_pb_pp_eg_vlan_edit_port_vlan_transmit_outer_tag_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  SOC_PB_PP_PORT                                local_port_ndx,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           vid_ndx,
    SOC_SAND_OUT uint8                                 *transmit_outer_tag
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_vlan_edit_pcp_profile_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   For PCP profile set the key used for mapping. When the
 *   Edit Command is set to determine the PCP-DEI value
 *   according to mapping, then the key used for mapping may
 *   be PCP-DEI, UP or DP and TC.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  pcp_profile_ndx -
 *     The PCP profile is set according to Out-AC setting.
 *     Range: 0 - 7.
 *   SOC_SAND_IN  SOC_PB_PP_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY    key_mapping -
 *     How to build the key to be mapped to get out-PCP-DEI
 *     (either using packet Priority attributes (from VLAN TAGS
 *     UP/PCP/DEI) -if exist- or according to TC/DP)
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_eg_vlan_edit_pcp_profile_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY    key_mapping
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_eg_vlan_edit_pcp_profile_info_set" API.
 *     Refer to "soc_pb_pp_eg_vlan_edit_pcp_profile_info_set" API
 *     for details.
*********************************************************************/
uint32
  soc_pb_pp_eg_vlan_edit_pcp_profile_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_OUT SOC_PB_PP_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY    *key_mapping
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_vlan_edit_pcp_map_stag_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set mapping from outer S-tag PCP and DEI to the PCP and
 *   DEI values to be set in the transmitted packet's tag.
 *   This is the mapping to be used when the incoming packet
 *   is S-tagged (outer Tag) and pcp profile set to use
 *   packet's attribute.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  pcp_profile_ndx -
 *     The PCP profile is set according to OUT-AC setting.
 *     Range: 0 - 7.
 *   SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            tag_pcp_ndx -
 *     The PCP appears in the outer S-Tag of the packet. Range:
 *     0 - 7.
 *   SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           tag_dei_ndx -
 *     The DEI appears in the outer S-Tag of the packet. Range:
 *     0 - 1.
 *   SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            out_pcp -
 *     The mapped PCP to set in the transmitted packet header.
 *     When the mapping used to build C-tag, this value
 *     presents the UP to set in the packet tag. Range: 0 - 7.
 *   SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           out_dei -
 *     The mapped DEI to set in the transmitted packet header.
 *     This is relevant when used for S-Tag. When the mapping
 *     used to build C tag, this value has to be set to 0.
 *     Range: 0 - 1.
 * REMARKS:
 *   - This mapping is used when the outer packet tag is
 *   S-tag and pcp profile set to use packet's attribute.-
 *   When this mapping is used to build the C-tag, then the
 *   out-PCP is actually the UP, and the DEI has to be 0.-
 *   This value of PCP and DEI will be used when the source
 *   of PCP DEI is selected to be
 *   SOC_PPD_EG_VLAN_EDIT_TAG_PCP_DEI_SRC_MAP.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_eg_vlan_edit_pcp_map_stag_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            tag_pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           tag_dei_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           out_dei
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_eg_vlan_edit_pcp_map_stag_set" API.
 *     Refer to "soc_pb_pp_eg_vlan_edit_pcp_map_stag_set" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_eg_vlan_edit_pcp_map_stag_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            tag_pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           tag_dei_ndx,
    SOC_SAND_OUT SOC_SAND_PP_PCP_UP                            *out_pcp,
    SOC_SAND_OUT SOC_SAND_PP_DEI_CFI                           *out_dei
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_vlan_edit_pcp_map_ctag_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set mapping from outer C-tag UP to the PCP and DEI
 *   values to be set in the transmitted packet's tag. This
 *   is the mapping to be used when the incoming packet is
 *   C-tagged (outer Tag) and pcp profile set to use packet's
 *   attribute.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  pcp_profile_ndx -
 *     The PCP profile is set according to OUT-AC setting.
 *     Range: 0 - 7.
 *   SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            tag_up_ndx -
 *     The UP appears in the outer C-Tag of the packet. Range:
 *     0 - 7.
 *   SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            out_pcp -
 *     The mapped PCP to set in the transmitted packet header.
 *     When the mapping used to build C-tag, this value
 *     presents the UP to set in the packet tag. Range: 0 - 7.
 *   SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           out_dei -
 *     The mapped DEI to set in the transmitted packet header.
 *     This is relevant when used for S-Tag. When the mapping
 *     used to build C tag, this value has to be set to 0.
 *     Range: 0 - 1.
 * REMARKS:
 *   - This mapping is used when the outer packet tag is
 *   C-tag and pcp profile set to use packet's attribute.-
 *   When this mapping is used to build the C-tag, then the
 *   out-PCP is actually the UP, and the DEI has to be 0.-
 *   This value of PCP and DEI will be used when the source
 *   of PCP DEI is selected to be
 *   SOC_PPD_EG_VLAN_EDIT_TAG_PCP_DEI_SRC_MAP.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_eg_vlan_edit_pcp_map_ctag_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            tag_up_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           out_dei
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_eg_vlan_edit_pcp_map_ctag_set" API.
 *     Refer to "soc_pb_pp_eg_vlan_edit_pcp_map_ctag_set" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_eg_vlan_edit_pcp_map_ctag_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            tag_up_ndx,
    SOC_SAND_OUT SOC_SAND_PP_PCP_UP                            *out_pcp,
    SOC_SAND_OUT SOC_SAND_PP_DEI_CFI                           *out_dei
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_vlan_edit_pcp_map_untagged_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set mapping from COS parameters (DP and TC) to the PCP
 *   and DEI values to be set in the transmitted packet's
 *   tag. This is the mapping to be used when the incoming
 *   packet has no tags or pcp profile is set to use TC and
 *   DP for the mapping.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  pcp_profile_ndx -
 *     The PCP profile is set according to OUT-AC setting.
 *     Range: 0 - 7.
 *   SOC_SAND_IN  SOC_SAND_PP_TC                                tc_ndx -
 *     Traffic Class. Calculated at the ingress. See COS
 *     module. Range: 0 - 7.
 *   SOC_SAND_IN  SOC_SAND_PP_DP                                dp_ndx -
 *     Drop Precedence. Calculated at the ingress. See COS
 *     module. Range: 0 - 3.
 *   SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            out_pcp -
 *     The mapped PCP to set in the transmitted packet header.
 *     When the mapping used to build C-tag, this value
 *     represents the UP to set in the packet C-tag. Range: 0 -
 *     7.
 *   SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           out_dei -
 *     The mapped DEI to set in the transmitted packet header.
 *     This is relevant when used for S-Tag. When the mapping
 *     used to build C tag, this value has to be set to 0.
 *     Range: 0 - 1.
 * REMARKS:
 *   - This mapping is used when the packet has no Tags or
 *   pcp profile set to use TC and DP for the mapping.- When
 *   the Tag to build is C-Tag, then the out-PCP is actually
 *   the UP, and the DEI has to be 0.- This value of PCP and
 *   DEI will be used when the source of PCP DEI is selected
 *   to be SOC_PPD_EG_VLAN_EDIT_TAG_PCP_DEI_SRC_MAP.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_eg_vlan_edit_pcp_map_untagged_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                                tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                                dp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           out_dei
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_eg_vlan_edit_pcp_map_untagged_set" API.
 *     Refer to "soc_pb_pp_eg_vlan_edit_pcp_map_untagged_set" API
 *     for details.
*********************************************************************/
uint32
  soc_pb_pp_eg_vlan_edit_pcp_map_untagged_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                                tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                                dp_ndx,
    SOC_SAND_OUT SOC_SAND_PP_PCP_UP                            *out_pcp,
    SOC_SAND_OUT SOC_SAND_PP_DEI_CFI                           *out_dei
  );

void
  SOC_PB_PP_EG_VLAN_EDIT_PEP_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_EG_VLAN_EDIT_PEP_INFO *info
  );

void
  SOC_PB_PP_EG_VLAN_EDIT_COMMAND_KEY_clear(
    SOC_SAND_OUT SOC_PB_PP_EG_VLAN_EDIT_COMMAND_KEY *info
  );

void
  SOC_PB_PP_EG_VLAN_EDIT_COMMAND_TAG_BUILD_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_EG_VLAN_EDIT_COMMAND_TAG_BUILD_INFO *info
  );

void
  SOC_PB_PP_EG_VLAN_EDIT_COMMAND_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_EG_VLAN_EDIT_COMMAND_INFO *info
  );

#if SOC_PB_PP_DEBUG_IS_LVL1

const char*
  SOC_PB_PP_EG_VLAN_EDIT_TAG_VID_SRC_to_string(
    SOC_SAND_IN  SOC_PB_PP_EG_VLAN_EDIT_TAG_VID_SRC enum_val
  );

const char*
  SOC_PB_PP_EG_VLAN_EDIT_TAG_PCP_DEI_SRC_to_string(
    SOC_SAND_IN  SOC_PB_PP_EG_VLAN_EDIT_TAG_PCP_DEI_SRC enum_val
  );

const char*
  SOC_PB_PP_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY_to_string(
    SOC_SAND_IN  SOC_PB_PP_EG_VLAN_EDIT_TAG_PCP_DEI_MAP_KEY enum_val
  );

void
  SOC_PB_PP_EG_VLAN_EDIT_PEP_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_EG_VLAN_EDIT_PEP_INFO *info
  );

void
  SOC_PB_PP_EG_VLAN_EDIT_COMMAND_KEY_print(
    SOC_SAND_IN  SOC_PB_PP_EG_VLAN_EDIT_COMMAND_KEY *info
  );

void
  SOC_PB_PP_EG_VLAN_EDIT_COMMAND_TAG_BUILD_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_EG_VLAN_EDIT_COMMAND_TAG_BUILD_INFO *info
  );

void
  SOC_PB_PP_EG_VLAN_EDIT_COMMAND_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_EG_VLAN_EDIT_COMMAND_INFO *info
  );

#endif /* SOC_PB_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PP_API_EG_VLAN_EDIT_INCLUDED__*/
#endif

