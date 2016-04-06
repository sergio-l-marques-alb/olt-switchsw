/* $Id: arad_pp_api_frwrd_bmact.h,v 1.8 Broadcom SDK $
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

#ifndef __ARAD_PP_API_FRWRD_BMACT_INCLUDED__
/* { */
#define __ARAD_PP_API_FRWRD_BMACT_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_frwrd_bmact.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     The B-MACT FID is the B-VID                             */
#define  ARAD_PP_BFID_EQUAL_TO_BVID (SOC_PPC_BFID_EQUAL_TO_BVID)

/*     The B-MACT FID is '0'. Enable shared learning           */
#define  ARAD_PP_BFID_IS_0 (SOC_PPC_BFID_IS_0)

/*     flags for SOC_PPC_BMACT_ENTRY_XXX flags */
#define  ARAD_PP_BMACT_ENTRY_TYPE_FRWRD      (SOC_PPC_BMACT_ENTRY_TYPE_FRWRD)
#define  ARAD_PP_BMACT_ENTRY_TYPE_LEARN      (SOC_PPC_BMACT_ENTRY_TYPE_LEARN)
#define  ARAD_PP_BMACT_ENTRY_TYPE_MC_DEST    (SOC_PPC_BMACT_ENTRY_TYPE_MC_DEST)


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

typedef SOC_PPC_BMACT_BVID_INFO                                ARAD_PP_BMACT_BVID_INFO;
typedef SOC_PPC_BMACT_PBB_TE_VID_RANGE                         ARAD_PP_BMACT_PBB_TE_VID_RANGE;
typedef SOC_PPC_BMACT_ENTRY_KEY                                ARAD_PP_BMACT_ENTRY_KEY;
typedef SOC_PPC_BMACT_ENTRY_INFO                               ARAD_PP_BMACT_ENTRY_INFO;

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
*   arad_pp_frwrd_bmact_init
* TYPE:
*   PROC
* FUNCTION:
*   Init device to support Mac in mac. 
*   User cannot disable it once configured. 
*   Only reboot can reset the configurations.
* INPUT:
*   SOC_SAND_IN  int                                 unit -
*     Identifier of the device to access.
* REMARKS:
*   None.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_bmact_init(
    SOC_SAND_IN  int                                     unit
  );

/*********************************************************************
* NAME:
 *   arad_pp_frwrd_bmact_bvid_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the B-VID bridging attributes. Backbone Mac
 *   addresses that do not serve as MyMAC for I-components
 *   that are processed according to their B-VID
 * INPUT:
 *   SOC_SAND_IN  int                                     unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                           bvid_ndx -
 *     Backbone VID
 *   SOC_SAND_IN  ARAD_PP_BMACT_BVID_INFO                     *bvid_info -
 *     B-VID attributes
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_bmact_bvid_info_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                    bvid_ndx,
    SOC_SAND_IN  ARAD_PP_BMACT_BVID_INFO                *bvid_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_frwrd_bmact_bvid_info_set" API.
 *     Refer to "arad_pp_frwrd_bmact_bvid_info_set" API for
 *     details.
*********************************************************************/
uint32
  arad_pp_frwrd_bmact_bvid_info_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_SAND_PP_VLAN_ID                    bvid_ndx,
    SOC_SAND_OUT ARAD_PP_BMACT_BVID_INFO                *bvid_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_frwrd_bmact_pbb_te_bvid_range_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the BVID range for Traffic Engineered Provider
 *   Backbone Bridging
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_BMACT_PBB_TE_VID_RANGE              *pbb_te_bvids -
 *     Range of B-VIDs, to be used as PBB-TE services
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_bmact_pbb_te_bvid_range_set(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_BMACT_PBB_TE_VID_RANGE         *pbb_te_bvids
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_frwrd_bmact_pbb_te_bvid_range_set" API.
 *     Refer to "arad_pp_frwrd_bmact_pbb_te_bvid_range_set" API
 *     for details.
*********************************************************************/
uint32
  arad_pp_frwrd_bmact_pbb_te_bvid_range_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT ARAD_PP_BMACT_PBB_TE_VID_RANGE         *pbb_te_bvids
  );

/*********************************************************************
* NAME:
 *   arad_pp_frwrd_bmact_entry_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add an entry to the B-MACT DB.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_BMACT_ENTRY_KEY                     *bmac_key -
 *     B-VID and B-MAC
 *   SOC_SAND_IN  ARAD_PP_BMACT_ENTRY_INFO                    *bmact_entry_info -
 *     B-MACT entry attributes
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success -
 *     SOC_SAND_FAILURE_OUT_OF_RESOURCES: There is no space in the
 *     Exact Match table
 * REMARKS:
 *   Unless PBB-TE is expected to be called by the B-MAC
 *   learning process.
 *   For ARAD, BMACT forwrading and learning information are located on two
 *   different logical databases. BMACT DB for forwarding and MIM Tunnel
 *   Learn information DB for learning in case of Termination.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_bmact_entry_add(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_BMACT_ENTRY_KEY                *bmac_key,
    SOC_SAND_IN  ARAD_PP_BMACT_ENTRY_INFO               *bmact_entry_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE               *success
  );

/*********************************************************************
* NAME:
 *   arad_pp_frwrd_bmact_entry_remove
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove an entry from the B-MACT DB.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_BMACT_ENTRY_KEY                     *bmac_key -
 *     B-VID and B-MAC
 * REMARKS:
 *   Unless PBB-TE is expected to be called by the B-MAC
 *   learning process
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_bmact_entry_remove(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_BMACT_ENTRY_KEY                     *bmac_key
  );

/*********************************************************************
* NAME:
 *   arad_pp_frwrd_bmact_entry_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get an entry from the B-MACT DB.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_BMACT_ENTRY_KEY                     *bmac_key -
 *     B-VID and B-MAC
 *   SOC_SAND_OUT  ARAD_PP_BMACT_ENTRY_INFO                   *bmact_entry_info -
 *     B-MACT entry attributes
 *   SOC_SAND_OUT  uint8                                *found -
 *     FALSE: the entry was not found
 * REMARKS:
 *   Unless PBB-TE is expected to be called by the B-MAC
 *   learning process
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_bmact_entry_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_BMACT_ENTRY_KEY                *bmac_key,
    SOC_SAND_OUT ARAD_PP_BMACT_ENTRY_INFO               *bmact_entry_info,
    SOC_SAND_OUT uint8                                  *found
  );

/*********************************************************************
* NAME:
*   arad_pp_frwrd_bmact_eg_vlan_pcp_map_set
* TYPE:
*   PROC
* FUNCTION:
*   Set mapping from COS parameters (DP and TC) to the PCP
*   and DEI values to be set in the transmitted packet's
*   I-tag. This is the mapping to be used when the incoming
*   packet has no tags or pcp profile is set to use TC and
*   DP for the mapping.
* INPUT:
*   SOC_SAND_IN  int                               unit -
*     Identifier of the device to access.
*   SOC_SAND_IN  uint32                                pcp_profile_ndx -
*     The PCP profile is set according to OUT-AC setting.
*     Range: 0 - 0.
*   SOC_SAND_IN  SOC_SAND_PP_TC                              tc_ndx -
*     Traffic Class. Calculated at the ingress. See COS
*     module. Range: 0 - 7.
*   SOC_SAND_IN  SOC_SAND_PP_DP                              dp_ndx -
*     Drop Precedence. Calculated at the ingress. See COS
*     module. Range: 0 - 3.
*   SOC_SAND_IN  SOC_SAND_PP_PCP_UP                          out_pcp -
*     The mapped PCP to set in the transmitted packet header.
*     Range: 0 - 7.
*   SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                         out_dei -
*     The mapped DEI to set in the transmitted packet header.
*     Range: 0 - 1.
* REMARKS:
*   - This mapping is used when the packet has no Tags or
*   pcp profile set to use TC and DP for the mapping. This value of PCP and
*   DEI will be used when the source of PCP DEI is selected
*   to be SOC_PPD_bmac_eg_vlanTAG_PCP_DEI_SRC_MAP.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_frwrd_bmact_eg_vlan_pcp_map_set(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                              tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                              dp_ndx,
    SOC_SAND_IN  SOC_SAND_PP_PCP_UP                          out_pcp,
    SOC_SAND_IN  SOC_SAND_PP_DEI_CFI                         out_dei
  );

/*********************************************************************
*     Gets the configuration set by the
*     "arad_pp_frwrd_bmact_eg_vlan_pcp_map_set" API.
*     Refer to "arad_pp_frwrd_bmact_eg_vlan_pcp_map_set" API for
*     details.
*********************************************************************/
uint32
  arad_pp_frwrd_bmact_eg_vlan_pcp_map_get(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  uint32                                      pcp_profile_ndx,
    SOC_SAND_IN  SOC_SAND_PP_TC                              tc_ndx,
    SOC_SAND_IN  SOC_SAND_PP_DP                              dp_ndx,
    SOC_SAND_OUT SOC_SAND_PP_PCP_UP                          *out_pcp,
    SOC_SAND_OUT SOC_SAND_PP_DEI_CFI                         *out_dei
  );
void
  ARAD_PP_BMACT_BVID_INFO_clear(
    SOC_SAND_OUT ARAD_PP_BMACT_BVID_INFO *info
  );

void
  ARAD_PP_BMACT_PBB_TE_VID_RANGE_clear(
    SOC_SAND_OUT ARAD_PP_BMACT_PBB_TE_VID_RANGE *info
  );
void
  ARAD_PP_BMACT_ENTRY_INFO_clear(
    SOC_SAND_OUT ARAD_PP_BMACT_ENTRY_INFO *info
  );
#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_PP_API_FRWRD_BMACT_INCLUDED__*/
#endif

