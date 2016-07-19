/* $Id: pb_pp_api_eg_filter.h,v 1.8 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/include/soc_pb_pp_api_eg_filter.h
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

#ifndef __SOC_PB_PP_API_EG_FILTER_INCLUDED__
/* { */
#define __SOC_PB_PP_API_EG_FILTER_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_eg_filter.h>
#include <soc/dpp/PPD/ppd_api_llp_parse.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_api_general.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_llp_parse.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_llp_filter.h>

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

#define SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE_PROMISCUOUS        SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_PROMISCUOUS
#define SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE_ISOLATED           SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_ISOLATED
#define SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE_COMMUNITY          SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE_COMMUNITY
#define SOC_PB_PP_NOF_EG_FILTER_PVLAN_PORT_TYPES               SOC_PPC_NOF_EG_FILTER_PVLAN_PORT_TYPES
typedef SOC_PPC_EG_FILTER_PVLAN_PORT_TYPE                      SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE;

#define SOC_PB_PP_EG_FILTER_PORT_ENABLE_NONE                   SOC_PPC_EG_FILTER_PORT_ENABLE_NONE
#define SOC_PB_PP_EG_FILTER_PORT_ENABLE_VSI_MEMBERSHIP         SOC_PPC_EG_FILTER_PORT_ENABLE_VSI_MEMBERSHIP
#define SOC_PB_PP_EG_FILTER_PORT_ENABLE_SAME_INTERFACE         SOC_PPC_EG_FILTER_PORT_ENABLE_SAME_INTERFACE
#define SOC_PB_PP_EG_FILTER_PORT_ENABLE_UC_UNKNOW_DA           SOC_PPC_EG_FILTER_PORT_ENABLE_UC_UNKNOW_DA
#define SOC_PB_PP_EG_FILTER_PORT_ENABLE_MC_UNKNOW_DA           SOC_PPC_EG_FILTER_PORT_ENABLE_MC_UNKNOW_DA
#define SOC_PB_PP_EG_FILTER_PORT_ENABLE_BC_UNKNOW_DA           SOC_PPC_EG_FILTER_PORT_ENABLE_BC_UNKNOW_DA
#define SOC_PB_PP_EG_FILTER_PORT_ENABLE_MTU                    SOC_PPC_EG_FILTER_PORT_ENABLE_MTU
#define SOC_PB_PP_EG_FILTER_PORT_ENABLE_STP                    SOC_PPC_EG_FILTER_PORT_ENABLE_STP
#define SOC_PB_PP_EG_FILTER_PORT_ENABLE_SPLIT_HORIZON          SOC_PPC_EG_FILTER_PORT_ENABLE_SPLIT_HORIZON
#define SOC_PB_PP_EG_FILTER_PORT_ENABLE_ALL                    SOC_PPC_EG_FILTER_PORT_ENABLE_ALL
typedef SOC_PPC_EG_FILTER_PORT_ENABLE                          SOC_PB_PP_EG_FILTER_PORT_ENABLE;

typedef SOC_PPC_EG_FILTER_PORT_INFO                            SOC_PB_PP_EG_FILTER_PORT_INFO;

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
 *   soc_pb_pp_eg_filter_port_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets out-port filtering information, including which
 *   filtering to perform on this specific out-port.
 * INPUT:
 *   SOC_SAND_IN  int                          unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_PORT                         out_port_ndx -
 *     Local out-port ID.
 *   SOC_SAND_IN  SOC_PB_PP_EG_FILTER_PORT_INFO          *port_info -
 *     Filtering information per port.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_port_info_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  int                          core_id,
    SOC_SAND_IN  SOC_PB_PP_PORT                         out_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_FILTER_PORT_INFO          *port_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_eg_filter_port_info_set" API.
 *     Refer to "soc_pb_pp_eg_filter_port_info_set" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_port_info_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  int                          core_id,
    SOC_SAND_IN  SOC_PB_PP_PORT                         out_port_ndx,
    SOC_SAND_OUT SOC_PB_PP_EG_FILTER_PORT_INFO          *port_info
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_filter_vsi_port_membership_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets egress VSI membership, which represents the
 *   Service/S-VLAN membership. Set whether outgoing local
 *   port belongs to the VSI. Packets transmitted out through
 *   a port that is not member of the packet's VSI are
 *   filtered.
 * INPUT:
 *   SOC_SAND_IN  int                          unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_VSI_ID                       vsid_ndx -
 *     SOC_SAND_IN SOC_PPD_VSI_ID vsid_ndx
 *   SOC_SAND_IN  SOC_PB_PP_PORT                         out_port_ndx -
 *     Local out-port ID.
 *   SOC_SAND_IN  uint8                                 is_member -
 *     If TRUE, then the port is member of the VSI; otherwise,
 *     the port is not member.
 * REMARKS:
 *   - T20E: - In T20E, both memberships may be checked for
 *   CEP ports.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_vsi_port_membership_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  int                          core_id,
    SOC_SAND_IN  SOC_PB_PP_VSI_ID                       vsid_ndx,
    SOC_SAND_IN  SOC_PB_PP_PORT                         out_port_ndx,
    SOC_SAND_IN  uint8                          is_member
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_eg_filter_vsi_port_membership_set" API.
 *     Refer to "soc_pb_pp_eg_filter_vsi_port_membership_set" API
 *     for details.
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_vsi_port_membership_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  int                          core_id,
    SOC_SAND_IN  SOC_PB_PP_VSI_ID                       vsid_ndx,
    SOC_SAND_IN  SOC_PB_PP_PORT                         out_port_ndx,
    SOC_SAND_OUT uint8                          *is_member
  );


/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_filter_vsi_membership_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *     Gets egress VSI membership, which represents the
 *     Service/S-VLAN membership. Get all member outgoing local
 *     ports belongs to the VSI. 
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_VSI_ID                              vsid_ndx -
 *     SOC_SAND_IN SOC_PB_PP_VSI_ID vsid_ndx
 *   SOC_SAND_IN  uint32                                ports -
 *     SOC_PB_PP_VLAN_MEMBERSHIP_BITMAP_SIZE] - The VLAN membership
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_vsi_membership_get(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  int                               core_id,
    SOC_SAND_IN  SOC_PB_PP_VSI_ID                       vsid_ndx,
    SOC_SAND_OUT uint32                               ports[SOC_PB_PP_VLAN_MEMBERSHIP_BITMAP_SIZE]
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_filter_cvid_port_membership_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets egress CVID membership (relevant for CEP ports).
 *   Sets whether outgoing local port belongs to the CVID.
 *   Packets transmitted out through a port that is not
 *   member of the packet's CVID are filtered.
 * INPUT:
 *   SOC_SAND_IN  int                          unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                    cvid_ndx -
 *     CVID to set the membership for. Range: 0 - 4095. (4K)
 *   SOC_SAND_IN  SOC_PB_PP_PORT                         out_port_ndx -
 *     Local out-port ID.
 *   SOC_SAND_IN  uint8                          is_member -
 *     If TRUE, then the port is member of the CVID; otherwise,
 *     the port is not member.
 * REMARKS:
 *   - Port type has to be CEP.- It is ensured that packet
 *   has outer tag (C-tag in this case), either arriving on
 *   arriving on the packet or as a result of the editing
 *   process.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_cvid_port_membership_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                    cvid_ndx,
    SOC_SAND_IN  SOC_PB_PP_PORT                         out_port_ndx,
    SOC_SAND_IN  uint8                          is_member
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_eg_filter_cvid_port_membership_set" API.
 *     Refer to "soc_pb_pp_eg_filter_cvid_port_membership_set" API
 *     for details.
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_cvid_port_membership_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                    cvid_ndx,
    SOC_SAND_IN  SOC_PB_PP_PORT                         out_port_ndx,
    SOC_SAND_OUT uint8                          *is_member
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_filter_port_acceptable_frames_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets acceptable frame type on outgoing port.
 * INPUT:
 *   SOC_SAND_IN  int                          unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                           out_port_ndx -
 *     Local out-port ID.
 *   SOC_SAND_IN  SOC_PB_PP_LLP_PARSE_INFO                      *eg_prsr_out_key -
 *     Egress parsing output specifying what TPIDs exist on the
 *     packet
 *   SOC_SAND_IN  uint8                                 accept -
 *     Accept or deny this frame type for this port type.
 * REMARKS:
 *   - T20E: not supported. Error will be returned if
 *   called.- For the profiles (0-3).- This actually refines
 *   the default and 'standard' frame types acceptance set
 *   according to port type.- Use
 *   SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_ANY as
 *   vlan_format_ndx to make a decision for ALL VLAN formats
 *   at once.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_port_acceptable_frames_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                           out_port_ndx,
    SOC_SAND_IN  uint32                           port_profile,
    SOC_SAND_IN  SOC_PB_PP_LLP_PARSE_INFO               *eg_prsr_out_key,
    SOC_SAND_IN  uint8                          accept
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_eg_filter_port_acceptable_frames_set" API.
 *     Refer to "soc_pb_pp_eg_filter_port_acceptable_frames_set"
 *     API for details.
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_port_acceptable_frames_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  out_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_LLP_PARSE_INFO                      *eg_prsr_out_key,
    SOC_SAND_OUT uint8                                 *accept
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_filter_pep_acceptable_frames_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets acceptable frame type for PEP port.
 * INPUT:
 *   SOC_SAND_IN  int                          unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_PEP_KEY                      *pep_key -
 *     PEP key identifier, out-port x VSI.
 *   SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT vlan_format_ndx -
 *     Packet frame type (tags stack structure). Possible
 *     values: - SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_C_TAG, -
 *     SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_NONE-
 *     SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_ANY (presents both
 *     C-Tagged and untagged packets)
 *   SOC_SAND_IN  uint8                          accept -
 *     Accept or deny this frame type for the given PEP.
 * REMARKS:
 *   - Soc_petra-B: not supported. Error will be returned if
 *   called.- use SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT_ANY as
 *   vlan_format_ndx to make a decision for ALL VLAN formats
 *   at once.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_pep_acceptable_frames_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PP_PEP_KEY                      *pep_key,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT vlan_format_ndx,
    SOC_SAND_IN  uint8                          accept
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_eg_filter_pep_acceptable_frames_set" API.
 *     Refer to "soc_pb_pp_eg_filter_pep_acceptable_frames_set" API
 *     for details.
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_pep_acceptable_frames_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PP_PEP_KEY                      *pep_key,
    SOC_SAND_IN  SOC_SAND_PP_ETHERNET_FRAME_VLAN_FORMAT vlan_format_ndx,
    SOC_SAND_OUT uint8                          *accept
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_filter_pvlan_port_type_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the Private VLAN (PVLAN) port type.
 * INPUT:
 *   SOC_SAND_IN  int                          unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                           phy_sys_port_ndx -
 *     Physical System port. Range: 0 - 4095.
 *   SOC_SAND_IN  SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE    pvlan_port_type -
 *     PVLAN port Type (promiscuous, isolated, community)
 * REMARKS:
 *   - Each device has to know (locally) the type of every
 *   system port that may transmit a packet to it.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_pvlan_port_type_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                       *src_sys_port_ndx,
    SOC_SAND_IN  SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE           pvlan_port_type
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_eg_filter_pvlan_port_type_set" API.
 *     Refer to "soc_pb_pp_eg_filter_pvlan_port_type_set" API for
 *     details.
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_pvlan_port_type_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_SYS_PORT_ID                       *src_sys_port_ndx,
    SOC_SAND_OUT SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE           *pvlan_port_type
  );

/*********************************************************************
* NAME:
 *   soc_pb_pp_eg_filter_split_horizon_out_ac_orientation_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets the orientation of out-AC, hub or spoke.
 * INPUT:
 *   SOC_SAND_IN  int                          unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PB_PP_AC_ID                        out_ac_ndx -
 *     The out-AC associated with packet.
 *   SOC_SAND_IN  SOC_SAND_PP_SPLIT_HORIZON_ORIENTATION  orientation -
 *     Out AC orientation Hub or Spoke. Packets forwarded from
 *     hub interface to hub interface will be filtered.
 * REMARKS:
 *   - T20E: not supported. Error will be returned if
 *   called.- In Soc_petra-B this setting will be used only if -
 *   Auxiliary DB allocated for this purpose. - the EEI
 *   associated with the packet has out-AC.- Orientation of
 *   incoming interface set according to
 *   soc_ppd_l2_lif_pwe_add()/ soc_ppd_l2_lif_ac_add()- To set
 *   orientation of tunnel, use
 *   soc_ppd_eg_encap_mpls_encap_entry_add()
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_split_horizon_out_ac_orientation_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PP_AC_ID                        out_ac_ndx,
    SOC_SAND_IN  SOC_SAND_PP_HUB_SPOKE_ORIENTATION  orientation
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_pp_eg_filter_split_horizon_out_ac_orientation_set"
 *     API.
 *     Refer to
 *     "soc_pb_pp_eg_filter_split_horizon_out_ac_orientation_set"
 *     API for details.
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_split_horizon_out_ac_orientation_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PB_PP_AC_ID                        out_ac_ndx,
    SOC_SAND_OUT SOC_SAND_PP_HUB_SPOKE_ORIENTATION  *orientation
  );


/*********************************************************************
* Not supported for Soc_petraB 
* use  soc_pb_pp_eg_filter_split_horizon_out_ac_orientation_set
*     Sets the orientation of out-lif, hub or spoke.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_split_horizon_out_lif_orientation_set(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                        lif_eep_ndx,
    SOC_SAND_IN  SOC_SAND_PP_HUB_SPOKE_ORIENTATION      orientation
  );

/*********************************************************************
* Not supported for Soc_petraB 
* use  soc_pb_pp_eg_filter_split_horizon_out_ac_orientation_get
*     Sets the orientation of out-lif, hub or spoke.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_eg_filter_split_horizon_out_lif_orientation_get(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                         lif_eep_ndx,
    SOC_SAND_OUT SOC_SAND_PP_HUB_SPOKE_ORIENTATION      *orientation
  );

void
  SOC_PB_PP_EG_FILTER_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PB_PP_EG_FILTER_PORT_INFO *info
  );

#if SOC_PB_PP_DEBUG_IS_LVL1

const char*
  SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE_to_string(
    SOC_SAND_IN  SOC_PB_PP_EG_FILTER_PVLAN_PORT_TYPE enum_val
  );

const char*
  SOC_PB_PP_EG_FILTER_PORT_ENABLE_to_string(
    SOC_SAND_IN  SOC_PB_PP_EG_FILTER_PORT_ENABLE enum_val
  );

void
  SOC_PB_PP_EG_FILTER_PORT_INFO_print(
    SOC_SAND_IN  SOC_PB_PP_EG_FILTER_PORT_INFO *info
  );

#endif /* SOC_PB_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_PP_API_EG_FILTER_INCLUDED__*/
#endif

