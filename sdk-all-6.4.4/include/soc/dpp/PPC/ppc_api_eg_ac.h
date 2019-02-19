/* $Id: ppc_api_eg_ac.h,v 1.11 Broadcom SDK $
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
* FILENAME:       DuneDriver/ppc/include/soc_ppc_api_eg_ac.h
*
* MODULE PREFIX:  soc_ppc_eg
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

#ifndef __SOC_PPC_API_EG_AC_INCLUDED__
/* { */
#define __SOC_PPC_API_EG_AC_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_general.h>
#include <soc/dpp/PPC/ppc_api_eg_encap.h>

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

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  VLAN Domain ID. The VLAN domain is either a port, or a
   *  group of ports that share the same VLAN IDs space. In
   *  T20E has to be equal to local-port-id.
   */
  uint32 vlan_domain;
  /*
   *  The VSI the packet is associated with. For Tunnel
   *  encapsulated packets, this is the VSI from the Egress
   *  Encapsulation.
   */
  SOC_PPC_VSI_ID vsi;

} SOC_PPC_EG_AC_VBP_KEY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  VLAN Domain ID. All ports use this vlan domain has to be
   *  of type CEP. The VLAN domain is either a port, or a group
   *  of ports that share the same VLAN IDs space. In T20E has
   *  to be equal to local-port-id.
   */
  uint32 vlan_domain;
  /*
   *  The resolved C-VLAN ID. This is the C-VLAN ID on the
   *  packet (on VLAN tag Header) or a C-VID set according to
   *  the PEP default (PEP-PVID).
   */
  SOC_SAND_PP_VLAN_ID cvid;
  /*
   *  Edit profile to enable different editing according to
   *  PEP. The edit profile is set according to
   *  soc_ppd_eg_edit_pep_info_set(). Used to give different
   *  treatment for packets in the same C-Component, depending
   *  to the PEP they pass from. In Soc_petra-B has to be 0. Range:
   *  0 - 7.
   */
  uint32 pep_edit_profile;
  /* 
   * The VSI the packet is associated with. For Tunnel 
   * encapsulated packets, this is the VSI from the Egress 
   * Encapsulation. 
   */ 
  SOC_PPC_VSI_ID vsi; 

} SOC_PPC_EG_AC_CEP_PORT_KEY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  C-VLAN ID. Used for CVID translation in the
   *  C-Component. Range: 0 - 4095.
   */
  SOC_SAND_PP_VLAN_ID cvid;
  /*
   *  User priority to set in the packet header. Range: 0 - 7.
   */
  SOC_SAND_PP_PCP_UP up;
  /*
   *  Profile used to build the UP of the packet. See COS
   *  Mapping APIs
   *  below:soc_ppd_eg_vlan_edit_pcp_map_stag_set()soc_ppd_eg_vlan_edit_pcp_map_ctag_set()soc_ppd_eg_vlan_edit_pcp_map_untagged_set()These
   *  APIs map COS parameters (UP/PCP/DEI/TC/DP) (and
   *  pcp_profile) to PCP and DEI. These profiles let the user
   *  configure up to 16 such mappings. Range: 0 - 15.
   */
  uint8 pcp_profile;
  /*
   *  Profile used to set the Egress command.
   */
  uint32 edit_profile;

} SOC_PPC_EG_VLAN_EDIT_CEP_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  VLAN ID. Range: 0 - 4095.
   */
  SOC_SAND_PP_VLAN_ID vid;
  /*
   *  Priority Code Point. Refers to the IEEE 802.1p priority.
   *  For C-Tag it is the User Priority. Range: 0 - 7.
   */
  SOC_SAND_PP_PCP_UP pcp;
  /*
   *  Drop Eligibility Indicator. For C-tag, this is the CFI
   *  (Canonical Format Indicator) and has to be 0. Range: 0 -
   *  1.
   */
  SOC_SAND_PP_DEI_CFI dei;

} SOC_PPC_EG_VLAN_EDIT_VLAN_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Vlan tag, C-tag, or S-tag to be used for the VLAN
   *  editing. According to
   *  soc_ppd_eg_vlan_edit_command_info_set(), the user may -
   *  select to build/replace the VID of the packet with one
   *  of these tags. - set the UP/PCP of the packet to this
   *  value, or to map it according to COS attributes (see
   *  pcp_profile).
   *  Note: for Soc_petra-B, only the VIDs fields of these Tags are
   *  relevant, the PCP-DEI may be acquired by mapping from
   *  pcp_profile.
   */
  SOC_PPC_EG_VLAN_EDIT_VLAN_INFO vlan_tags[SOC_PPC_VLAN_TAGS_MAX];
  /*
   *  Number of Vlan tags. Soc_petra-B and T20E support 2 tags:
   *  vlan_tags[0] is used for building the outer tag and
   *  vlan_tags[1] for building the inner tag.
   */
  uint32 nof_tags;
  /*
   *  Profile according to which to edit the VLAN tags. See
   *  soc_ppd_eg_vlan_edit_command_info_set(). According to
   *  tag-format which represents the Tag structure of the
   *  packet and this edit-profile, the user may define an
   *  Edit command to build the header of the outgoing
   *  packets. Range:
   *    Up till Jerico: 0 - 15,
   *    Jericho and above: 0-31
   */
  uint32 edit_profile;
  /*
   *  Profile to build the packet PCP-DEI/UP. See COS Mapping
   *  APIs in Egress Edit Module: soc_ppd_eg_vlan_edit_pcp_map_stag_set,
   *  soc_ppd_eg_vlan_edit_pcp_map_ctag_set, soc_ppd_eg_vlan_edit_pcp_map_untagged_set.
   *  These APIs map COS parameters (UP/PCP/DEI/TC/DP) (and
   *  pcp_profile) to PCP and DEI. These profiles let the user
   *  configure up to 16 such mappings. Range: 0 - 15.
   */
  uint32 pcp_profile;

  uint32 lif_profile;

  /* 
   * OAM LIF indication value. 
   * If set, then OAM LIF value is equal to the OutLIF index.
   * ARAD only. 
   */ 
  uint32 oam_lif_set;
#ifdef BCM_88660_A0
  /* 
   * Data entry indication value. 
   * If set, the entire EEDB line is pushed to the data, which is transferred to the programmable editor.
   * When set, all entry is used where first entry is the regular out-AC and second entry 
   * contains data information. ARAD Plus only. 
   */
  uint32 use_as_data_entry;

  /* 
   * Second EEDB entry Data information is passed to the programmable editor for flexiable settings.
   * An example of use in PON application, addition of Tunnel-ID in case of 3-tags manipulation.
   * Tunnel-TPID(16b), PCP-DEI(4b), Tunnel-ID(12b), 2lsbs identifier.
   * ARAD Plus only. 
   */
  uint32 data[SOC_PPC_EG_ENCAP_DATA_INFO_MAX_SIZE];
#endif
} SOC_PPC_EG_AC_VLAN_EDIT_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Information to edit packet header.
   */
  SOC_PPC_EG_AC_VLAN_EDIT_INFO edit_info;

} SOC_PPC_EG_AC_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  The level of the MEP. Range: 0 - 7.
   */
  SOC_PPC_MP_LEVEL mp_level;
  /*
   *  If True, then enable MP for this ACF.
   */
  uint8 is_valid;

} SOC_PPC_EG_AC_MP_INFO;


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

void
  SOC_PPC_EG_AC_VBP_KEY_clear(
    SOC_SAND_OUT SOC_PPC_EG_AC_VBP_KEY *info
  );

void
  SOC_PPC_EG_AC_CEP_PORT_KEY_clear(
    SOC_SAND_OUT SOC_PPC_EG_AC_CEP_PORT_KEY *info
  );

void
  SOC_PPC_EG_VLAN_EDIT_CEP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_VLAN_EDIT_CEP_INFO *info
  );

void
  SOC_PPC_EG_VLAN_EDIT_VLAN_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_VLAN_EDIT_VLAN_INFO *info
  );

void
  SOC_PPC_EG_AC_VLAN_EDIT_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_AC_VLAN_EDIT_INFO *info
  );

void
  SOC_PPC_EG_AC_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_AC_INFO *info
  );

void
  SOC_PPC_EG_AC_MP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_EG_AC_MP_INFO *info
  );

#if SOC_PPC_DEBUG_IS_LVL1

void
  SOC_PPC_EG_AC_VBP_KEY_print(
    SOC_SAND_IN  SOC_PPC_EG_AC_VBP_KEY *info
  );

void
  SOC_PPC_EG_AC_CEP_PORT_KEY_print(
    SOC_SAND_IN  SOC_PPC_EG_AC_CEP_PORT_KEY *info
  );

void
  SOC_PPC_EG_VLAN_EDIT_CEP_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_CEP_INFO *info
  );

void
  SOC_PPC_EG_VLAN_EDIT_VLAN_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_VLAN_EDIT_VLAN_INFO *info
  );

void
  SOC_PPC_EG_AC_VLAN_EDIT_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_AC_VLAN_EDIT_INFO *info
  );

void
  SOC_PPC_EG_AC_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_AC_INFO *info
  );

void
  SOC_PPC_EG_AC_MP_INFO_print(
    SOC_SAND_IN  SOC_PPC_EG_AC_MP_INFO *info
  );

#endif /* SOC_PPC_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PPC_API_EG_AC_INCLUDED__*/
#endif

