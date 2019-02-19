/* $Id: arad_pp_api_lif_cos.h,v 1.4 Broadcom SDK $
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

#ifndef __ARAD_PP_API_LIF_COS_INCLUDED__
/* { */
#define __ARAD_PP_API_LIF_COS_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_lif_cos.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_general.h>

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

#define ARAD_PP_LIF_COS_AC_PROFILE_TYPE_NONE                 SOC_PPC_LIF_COS_AC_PROFILE_TYPE_NONE
#define ARAD_PP_LIF_COS_AC_PROFILE_TYPE_FORCE_ALWAYS         SOC_PPC_LIF_COS_AC_PROFILE_TYPE_FORCE_ALWAYS
#define ARAD_PP_LIF_COS_AC_PROFILE_TYPE_MAP_IF_TAG_ELSE_FORCE SOC_PPC_LIF_COS_AC_PROFILE_TYPE_MAP_IF_TAG_ELSE_FORCE
#define ARAD_PP_LIF_COS_AC_PROFILE_TYPE_MAP_IF_IP_ELSE_FORCE SOC_PPC_LIF_COS_AC_PROFILE_TYPE_MAP_IF_IP_ELSE_FORCE
#define ARAD_PP_LIF_COS_AC_PROFILE_TYPE_MAP_IF_TAG_ELSE_NONE SOC_PPC_LIF_COS_AC_PROFILE_TYPE_MAP_IF_TAG_ELSE_NONE
#define ARAD_PP_LIF_COS_AC_PROFILE_TYPE_MAP_IF_IP_ELSE_NONE  SOC_PPC_LIF_COS_AC_PROFILE_TYPE_MAP_IF_IP_ELSE_NONE
#define ARAD_PP_NOF_LIF_COS_AC_PROFILE_TYPES                 SOC_PPC_NOF_LIF_COS_AC_PROFILE_TYPES
typedef SOC_PPC_LIF_COS_AC_PROFILE_TYPE                        ARAD_PP_LIF_COS_AC_PROFILE_TYPE;

#define ARAD_PP_LIF_COS_PWE_PROFILE_TYPE_NONE                SOC_PPC_LIF_COS_PWE_PROFILE_TYPE_NONE
#define ARAD_PP_LIF_COS_PWE_PROFILE_TYPE_FORCE_ALWAYS        SOC_PPC_LIF_COS_PWE_PROFILE_TYPE_FORCE_ALWAYS
#define ARAD_PP_LIF_COS_PWE_PROFILE_TYPE_MAP                 SOC_PPC_LIF_COS_PWE_PROFILE_TYPE_MAP
#define ARAD_PP_NOF_LIF_COS_PWE_PROFILE_TYPES                SOC_PPC_NOF_LIF_COS_PWE_PROFILE_TYPES
typedef SOC_PPC_LIF_COS_PWE_PROFILE_TYPE                       ARAD_PP_LIF_COS_PWE_PROFILE_TYPE;

#define ARAD_PP_LIF_COS_OPCODE_TYPE_L3                       SOC_PPC_LIF_COS_OPCODE_TYPE_L3
#define ARAD_PP_LIF_COS_OPCODE_TYPE_L2                       SOC_PPC_LIF_COS_OPCODE_TYPE_L2
#define ARAD_PP_LIF_COS_OPCODE_TYPE_TC_DP                    SOC_PPC_LIF_COS_OPCODE_TYPE_TC_DP
#define ARAD_PP_NOF_LIF_COS_OPCODE_TYPES                     SOC_PPC_NOF_LIF_COS_OPCODE_TYPES
typedef SOC_PPC_LIF_COS_OPCODE_TYPE                            ARAD_PP_LIF_COS_OPCODE_TYPE;

typedef SOC_PPC_LIF_COS_AC_PROFILE_INFO                        ARAD_PP_LIF_COS_AC_PROFILE_INFO;
typedef SOC_PPC_LIF_COS_PWE_PROFILE_INFO                       ARAD_PP_LIF_COS_PWE_PROFILE_INFO;
typedef SOC_PPC_LIF_COS_PROFILE_INFO                           ARAD_PP_LIF_COS_PROFILE_INFO;
typedef SOC_PPC_LIF_COS_PROFILE_MAP_TBL_ENTRY                  ARAD_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY;
typedef SOC_PPC_LIF_COS_PROFILE_MAP_TBL_IP_KEY                 ARAD_PP_LIF_COS_PROFILE_MAP_TBL_IP_KEY;
typedef SOC_PPC_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY               ARAD_PP_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY;
typedef SOC_PPC_LIF_COS_PROFILE_MAP_TBL_L2_KEY                 ARAD_PP_LIF_COS_PROFILE_MAP_TBL_L2_KEY;
typedef SOC_PPC_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY              ARAD_PP_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY;
typedef SOC_PPC_LIF_COS_OPCODE_ACTION_INFO                     ARAD_PP_LIF_COS_OPCODE_ACTION_INFO;

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
 *   arad_pp_lif_cos_ac_profile_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets COS information of AC COS Profile including (FORCE
 *   to const values, map field from the packet and select
 *   mapping table)
 * INPUT:
 *   SOC_SAND_IN  int                                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                      profile_ndx -
 *     AC COS Profile. Range: 1 - 15. Set by
 *     soc_ppd_l2_lif_ac_add().
 *   SOC_SAND_IN  ARAD_PP_LIF_COS_AC_PROFILE_INFO                 *profile_info -
 *     COS Profile information.
 * REMARKS:
 *   - The profile is set per AC - see soc_ppd_l2_lif_ac_add/
 *   soc_ppd_l2_lif_ac_with_cos_add()- cos profile 0 used for
 *   NOP.- T20E Only.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_lif_cos_ac_profile_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      profile_ndx,
    SOC_SAND_IN  ARAD_PP_LIF_COS_AC_PROFILE_INFO                 *profile_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_lif_cos_ac_profile_info_set" API.
 *     Refer to "arad_pp_lif_cos_ac_profile_info_set" API for
 *     details.
*********************************************************************/
uint32
  arad_pp_lif_cos_ac_profile_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      profile_ndx,
    SOC_SAND_OUT ARAD_PP_LIF_COS_AC_PROFILE_INFO                 *profile_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_lif_cos_pwe_profile_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets COS Profile information (FORCE to const values, map
 *   field from the packet and select mapping table)
 * INPUT:
 *   SOC_SAND_IN  int                                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                      profile_ndx -
 *     Profile. Range: 0 - 15.
 *   SOC_SAND_IN  ARAD_PP_LIF_COS_PWE_PROFILE_INFO                *profile_info -
 *     COS Profile information.
 * REMARKS:
 *   - The profile is determined per PWE - see
 *   soc_ppd_l2_lif_pwe_add()- T20E Only.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_lif_cos_pwe_profile_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      profile_ndx,
    SOC_SAND_IN  ARAD_PP_LIF_COS_PWE_PROFILE_INFO                *profile_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_lif_cos_pwe_profile_info_set" API.
 *     Refer to "arad_pp_lif_cos_pwe_profile_info_set" API for
 *     details.
*********************************************************************/
uint32
  arad_pp_lif_cos_pwe_profile_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      profile_ndx,
    SOC_SAND_OUT ARAD_PP_LIF_COS_PWE_PROFILE_INFO                *profile_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_lif_cos_profile_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set CoS Profile attributes. Incoming LIFs are mapped to
 *   CoS Profile. CoS Profile '0' defined to keep the previous
 *   settings. The other CoS profiles are configured by this
 *   function.
 * INPUT:
 *   SOC_SAND_IN  int                                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                      cos_profile_ndx -
 *     CoS Profile ID. Range: 1 - 15.
 *   SOC_SAND_IN  ARAD_PP_LIF_COS_PROFILE_INFO                    *profile_info -
 *     CoS profile attributes
 * REMARKS:
 *   - Arad-B Only. T20E users should call
 *   soc_ppd_lif_cos_ac_profile_info_set()/soc_ppd_lif_cos_pwe_profile_info_set()
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_lif_cos_profile_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      cos_profile_ndx,
    SOC_SAND_IN  ARAD_PP_LIF_COS_PROFILE_INFO                    *profile_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_lif_cos_profile_info_set" API.
 *     Refer to "arad_pp_lif_cos_profile_info_set" API for
 *     details.
*********************************************************************/
uint32
  arad_pp_lif_cos_profile_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      cos_profile_ndx,
    SOC_SAND_OUT ARAD_PP_LIF_COS_PROFILE_INFO                    *profile_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_lif_cos_profile_map_l2_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set mapping from L2 VLAN Tag fields to DP and TC.
 * INPUT:
 *   SOC_SAND_IN  int                                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                      map_tbl_ndx -
 *     Mapping table ID. Range: 1 - 15. Arad-B: The map table
 *     index is the CoS profile.
 *   SOC_SAND_IN  ARAD_PP_LIF_COS_PROFILE_MAP_TBL_L2_KEY          *map_key -
 *     L2 Key used for mapping, including (UP/PCP, DEI, matched
 *     TPID)
 *   SOC_SAND_IN  ARAD_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY           *map_value -
 *     The COS parameters (DP and TC) mapped to.
 * REMARKS:
 *   - T20E: The mapping table to use is determined according
 *   soc_ppd_lif_cos_ac_profile_info_set().- Arad-B: The mapping
 *   table is the CoS profile defined by the incoming LIF
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_lif_cos_profile_map_l2_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      map_tbl_ndx,
    SOC_SAND_IN  ARAD_PP_LIF_COS_PROFILE_MAP_TBL_L2_KEY          *map_key,
    SOC_SAND_IN  ARAD_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY           *map_value
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_lif_cos_profile_map_l2_info_set" API.
 *     Refer to "arad_pp_lif_cos_profile_map_l2_info_set" API for
 *     details.
*********************************************************************/
uint32
  arad_pp_lif_cos_profile_map_l2_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      map_tbl_ndx,
    SOC_SAND_IN  ARAD_PP_LIF_COS_PROFILE_MAP_TBL_L2_KEY          *map_key,
    SOC_SAND_OUT ARAD_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY           *map_value
  );

/*********************************************************************
* NAME:
 *   arad_pp_lif_cos_profile_map_ip_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set mapping from IP header fields (TOS / DSCP) to DP and
 *   TC.
 * INPUT:
 *   SOC_SAND_IN  int                                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                      map_tbl_ndx -
 *     Mapping table ID. Range: 1 - 15. Arad-B: The map table
 *     index is the CoS profile.
 *   SOC_SAND_IN  ARAD_PP_LIF_COS_PROFILE_MAP_TBL_IP_KEY          *map_key -
 *     IP Key used for mapping, including (DSCP)
 *   SOC_SAND_IN  ARAD_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY           *map_value -
 *     The COS parameters (DP and TC) mapped to.
 * REMARKS:
 *   - T20E: The mapping table to used is determined
 *   according soc_ppd_lif_cos_ac_profile_info_set().- Arad-B:
 *   The mapping table is the CoS profile defined by the
 *   incoming LIF
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_lif_cos_profile_map_ip_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      map_tbl_ndx,
    SOC_SAND_IN  ARAD_PP_LIF_COS_PROFILE_MAP_TBL_IP_KEY          *map_key,
    SOC_SAND_IN  ARAD_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY           *map_value
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_lif_cos_profile_map_ip_info_set" API.
 *     Refer to "arad_pp_lif_cos_profile_map_ip_info_set" API for
 *     details.
*********************************************************************/
uint32
  arad_pp_lif_cos_profile_map_ip_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      map_tbl_ndx,
    SOC_SAND_IN  ARAD_PP_LIF_COS_PROFILE_MAP_TBL_IP_KEY          *map_key,
    SOC_SAND_OUT ARAD_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY           *map_value
  );

/*********************************************************************
* NAME:
 *   arad_pp_lif_cos_profile_map_mpls_label_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set mapping from label fields (EXP) to DP and TC.
 * INPUT:
 *   SOC_SAND_IN  int                                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                      map_tbl_ndx -
 *     Mapping table ID. Range: 1 - 15. Arad-B: The map table
 *     index is the CoS profile.
 *   SOC_SAND_IN  ARAD_PP_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY        *map_key -
 *     Label Key used for mapping, including (EXP)
 *   SOC_SAND_IN  ARAD_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY           *map_value -
 *     The COS parameters (DP and TC) mapped to.
 * REMARKS:
 *   - T20E: The mapping table to used is determined
 *   according soc_ppd_lif_cos_pwe_profile_info_set().- Arad-B:
 *   The mapping table is the CoS profile defined by the
 *   incoming LIF
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_lif_cos_profile_map_mpls_label_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      map_tbl_ndx,
    SOC_SAND_IN  ARAD_PP_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY        *map_key,
    SOC_SAND_IN  ARAD_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY           *map_value
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_lif_cos_profile_map_mpls_label_info_set" API.
 *     Refer to "arad_pp_lif_cos_profile_map_mpls_label_info_set"
 *     API for details.
*********************************************************************/
uint32
  arad_pp_lif_cos_profile_map_mpls_label_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      map_tbl_ndx,
    SOC_SAND_IN  ARAD_PP_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY        *map_key,
    SOC_SAND_OUT ARAD_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY           *map_value
  );

/*********************************************************************
* NAME:
 *   arad_pp_lif_cos_profile_map_tc_dp_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set remapping from TC and DP to TC and DP.
 * INPUT:
 *   SOC_SAND_IN  int                                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                      map_tbl_ndx -
 *     Mapping table ID. Range: 1 - 15. The map table index is
 *     the CoS profile.
 *   SOC_SAND_IN  ARAD_PP_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY       *map_key -
 *     TC and DP, calculated from previous processing stages
 *   SOC_SAND_IN  ARAD_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY           *map_value -
 *     The COS parameters (DP and TC) mapped to.
 * REMARKS:
 *   - Arad-B Only.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_lif_cos_profile_map_tc_dp_info_set(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      map_tbl_ndx,
    SOC_SAND_IN  ARAD_PP_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY       *map_key,
    SOC_SAND_IN  ARAD_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY           *map_value
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_lif_cos_profile_map_tc_dp_info_set" API.
 *     Refer to "arad_pp_lif_cos_profile_map_tc_dp_info_set" API
 *     for details.
*********************************************************************/
uint32
  arad_pp_lif_cos_profile_map_tc_dp_info_get(
    SOC_SAND_IN  int                                     unit,
    SOC_SAND_IN  uint32                                      map_tbl_ndx,
    SOC_SAND_IN  ARAD_PP_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY       *map_key,
    SOC_SAND_OUT ARAD_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY           *map_value
  );

/*********************************************************************
* NAME:
 *   arad_pp_lif_cos_opcode_types_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the Opcode type. Set the mapping of Class of Service
 *   attributes to the AC-Offset.
 * INPUT:
 *   SOC_SAND_IN  int                                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  opcode_ndx -
 *     The ID of the configured Opcode. Range: Arad-B: 0 - 2;
 *     T20E: 0 - 6
 *   SOC_SAND_IN  ARAD_PP_LIF_COS_OPCODE_TYPE                 opcode_type -
 *     The CoS attributes that will affect the AC-Offset
 *     setting.
 * REMARKS:
 *   - After setting the opcode type, one should call the
 *   APIs that map the chosen CoS attributes to AC-offset-
 *   After setting the Opcode type and mapping table(s)
 *   soc_ppd_l2_lif_ac_with_cos_add() may be called to add ACs
 *   according to the CoS attributes
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_lif_cos_opcode_types_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  ARAD_PP_LIF_COS_OPCODE_TYPE                 opcode_type
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_lif_cos_opcode_types_set" API.
 *     Refer to "arad_pp_lif_cos_opcode_types_set" API for
 *     details.
*********************************************************************/
uint32
  arad_pp_lif_cos_opcode_types_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_OUT ARAD_PP_LIF_COS_OPCODE_TYPE                 *opcode_type
  );

/*********************************************************************
* NAME:
 *   arad_pp_lif_cos_opcode_ipv6_tos_map_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the Opcode according to IPv6 TOS field
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  opcode_ndx -
 *     The ID of the configured Opcode. Range: 1 - 3.
 *   SOC_SAND_IN  SOC_SAND_PP_IPV6_TC                           ipv6_tos_ndx -
 *     IPv6 TOS value, as arriving in the packet. Range: 0 -
 *     255.
 *   SOC_SAND_IN  ARAD_PP_LIF_COS_OPCODE_ACTION_INFO          *action_info -
 *     The mapping rules according to the TOS value and Opcode
 *     ID.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_lif_cos_opcode_ipv6_tos_map_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV6_TC                           ipv6_tos_ndx,
    SOC_SAND_IN  ARAD_PP_LIF_COS_OPCODE_ACTION_INFO          *action_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_lif_cos_opcode_ipv6_tos_map_set" API.
 *     Refer to "arad_pp_lif_cos_opcode_ipv6_tos_map_set" API for
 *     details.
*********************************************************************/
uint32
  arad_pp_lif_cos_opcode_ipv6_tos_map_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV6_TC                           ipv6_tos_ndx,
    SOC_SAND_OUT ARAD_PP_LIF_COS_OPCODE_ACTION_INFO          *action_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_lif_cos_opcode_ipv4_tos_map_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the Opcode according to IPv4 TOS field
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  opcode_ndx -
 *     The ID of the configured Opcode. Range: 1 - 3.
 *   SOC_SAND_IN  SOC_SAND_PP_IPV4_TOS                          ipv4_tos_ndx -
 *     IPv4 TOS value, as arriving in the packet. Range: 0 -
 *     255.
 *   SOC_SAND_IN  ARAD_PP_LIF_COS_OPCODE_ACTION_INFO          *action_info -
 *     The mapping rules according to the TOS value and Opcode
 *     ID.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_lif_cos_opcode_ipv4_tos_map_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_TOS                          ipv4_tos_ndx,
    SOC_SAND_IN  ARAD_PP_LIF_COS_OPCODE_ACTION_INFO          *action_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_lif_cos_opcode_ipv4_tos_map_set" API.
 *     Refer to "arad_pp_lif_cos_opcode_ipv4_tos_map_set" API for
 *     details.
*********************************************************************/
uint32
  arad_pp_lif_cos_opcode_ipv4_tos_map_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_IPV4_TOS                          ipv4_tos_ndx,
    SOC_SAND_OUT ARAD_PP_LIF_COS_OPCODE_ACTION_INFO          *action_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_lif_cos_opcode_tc_dp_map_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the Opcode according to Traffic Class and Drop
 *   Precedence
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  opcode_ndx -
 *     The ID of the configured Opcode. Range: 1 - 3.
 *   SOC_SAND_IN  SOC_SAND_PP_TC                                tc_ndx -
 *     Traffic Class. Range: 0 - 7.
 *   SOC_SAND_IN  SOC_SAND_PP_DP                                dp_ndx -
 *     Drop Precedence. Range: 0 - 3.
 *   SOC_SAND_IN  ARAD_PP_LIF_COS_OPCODE_ACTION_INFO          *action_info -
 *     The mapping rules according to the TC, DP and Opcode ID.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_lif_cos_opcode_tc_dp_map_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                                tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                                dp_ndx,
    SOC_SAND_IN  ARAD_PP_LIF_COS_OPCODE_ACTION_INFO          *action_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_lif_cos_opcode_tc_dp_map_set" API.
 *     Refer to "arad_pp_lif_cos_opcode_tc_dp_map_set" API for
 *     details.
*********************************************************************/
uint32
  arad_pp_lif_cos_opcode_tc_dp_map_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                                tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                                dp_ndx,
    SOC_SAND_OUT ARAD_PP_LIF_COS_OPCODE_ACTION_INFO          *action_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_lif_cos_opcode_vlan_tag_map_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the Opcode according to VLAN Tag
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  opcode_ndx -
 *     The ID of the configured Opcode. Range: 1 - 3.
 *   SOC_SAND_IN  uint8                                   tag_type_ndx -
 *     The outer TPID on the packet could be 0 - for None1 -
 *     port outer TPID2 - port inner TPID3 - ISID - TPID
 *   SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            pcp_ndx -
 *     Priority Code Point. Range: 0 - 7.
 *   SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           dei_ndx -
 *     Drop Eligible Indicator. Range: 0 - 1.
 *   SOC_SAND_IN  ARAD_PP_LIF_COS_OPCODE_ACTION_INFO          *action_info -
 *     The mapping rules according to the TC, DP, and Opcode
 *     ID.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_lif_cos_opcode_vlan_tag_map_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  uint8                                   tag_type_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           dei_ndx,
    SOC_SAND_IN  ARAD_PP_LIF_COS_OPCODE_ACTION_INFO          *action_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_lif_cos_opcode_vlan_tag_map_set" API.
 *     Refer to "arad_pp_lif_cos_opcode_vlan_tag_map_set" API for
 *     details.
*********************************************************************/
uint32
  arad_pp_lif_cos_opcode_vlan_tag_map_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  opcode_ndx,
    SOC_SAND_IN  uint8                                   tag_type_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                            pcp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                           dei_ndx,
    SOC_SAND_OUT ARAD_PP_LIF_COS_OPCODE_ACTION_INFO          *action_info
  );

void
  ARAD_PP_LIF_COS_AC_PROFILE_INFO_clear(
    SOC_SAND_OUT ARAD_PP_LIF_COS_AC_PROFILE_INFO *info
  );

void
  ARAD_PP_LIF_COS_PWE_PROFILE_INFO_clear(
    SOC_SAND_OUT ARAD_PP_LIF_COS_PWE_PROFILE_INFO *info
  );

void
  ARAD_PP_LIF_COS_PROFILE_INFO_clear(
    SOC_SAND_OUT ARAD_PP_LIF_COS_PROFILE_INFO *info
  );

void
  ARAD_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY_clear(
    SOC_SAND_OUT ARAD_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY *info
  );

void
  ARAD_PP_LIF_COS_PROFILE_MAP_TBL_IP_KEY_clear(
    SOC_SAND_OUT ARAD_PP_LIF_COS_PROFILE_MAP_TBL_IP_KEY *info
  );

void
  ARAD_PP_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY_clear(
    SOC_SAND_OUT ARAD_PP_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY *info
  );

void
  ARAD_PP_LIF_COS_PROFILE_MAP_TBL_L2_KEY_clear(
    SOC_SAND_OUT ARAD_PP_LIF_COS_PROFILE_MAP_TBL_L2_KEY *info
  );

void
  ARAD_PP_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY_clear(
    SOC_SAND_OUT ARAD_PP_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY *info
  );

void
  ARAD_PP_LIF_COS_OPCODE_ACTION_INFO_clear(
    SOC_SAND_OUT ARAD_PP_LIF_COS_OPCODE_ACTION_INFO *info
  );

#if ARAD_PP_DEBUG_IS_LVL1

const char*
  ARAD_PP_LIF_COS_AC_PROFILE_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_LIF_COS_AC_PROFILE_TYPE enum_val
  );

const char*
  ARAD_PP_LIF_COS_PWE_PROFILE_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_LIF_COS_PWE_PROFILE_TYPE enum_val
  );

const char*
  ARAD_PP_LIF_COS_OPCODE_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_LIF_COS_OPCODE_TYPE enum_val
  );

void
  ARAD_PP_LIF_COS_AC_PROFILE_INFO_print(
    SOC_SAND_IN  ARAD_PP_LIF_COS_AC_PROFILE_INFO *info
  );

void
  ARAD_PP_LIF_COS_PWE_PROFILE_INFO_print(
    SOC_SAND_IN  ARAD_PP_LIF_COS_PWE_PROFILE_INFO *info
  );

void
  ARAD_PP_LIF_COS_PROFILE_INFO_print(
    SOC_SAND_IN  ARAD_PP_LIF_COS_PROFILE_INFO *info
  );

void
  ARAD_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY_print(
    SOC_SAND_IN  ARAD_PP_LIF_COS_PROFILE_MAP_TBL_ENTRY *info
  );

void
  ARAD_PP_LIF_COS_PROFILE_MAP_TBL_IP_KEY_print(
    SOC_SAND_IN  ARAD_PP_LIF_COS_PROFILE_MAP_TBL_IP_KEY *info
  );

void
  ARAD_PP_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY_print(
    SOC_SAND_IN  ARAD_PP_LIF_COS_PROFILE_MAP_TBL_MPLS_KEY *info
  );

void
  ARAD_PP_LIF_COS_PROFILE_MAP_TBL_L2_KEY_print(
    SOC_SAND_IN  ARAD_PP_LIF_COS_PROFILE_MAP_TBL_L2_KEY *info
  );

void
  ARAD_PP_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY_print(
    SOC_SAND_IN  ARAD_PP_LIF_COS_PROFILE_MAP_TBL_TC_DP_KEY *info
  );

void
  ARAD_PP_LIF_COS_OPCODE_ACTION_INFO_print(
    SOC_SAND_IN  ARAD_PP_LIF_COS_OPCODE_ACTION_INFO *info
  );

#endif /* ARAD_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_PP_API_LIF_COS_INCLUDED__*/
#endif

