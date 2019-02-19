/* $Id: arad_pp_api_metering.h,v 1.13 Broadcom SDK $
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

#ifndef __ARAD_PP_API_METERING_INCLUDED__
/* { */
#define __ARAD_PP_API_METERING_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_metering.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */
#define ARAD_PP_MTR_ETH_TYPE_UNKNOW_UC                       SOC_PPC_MTR_ETH_TYPE_UNKNOW_UC
#define ARAD_PP_MTR_ETH_TYPE_KNOW_UC                         SOC_PPC_MTR_ETH_TYPE_KNOW_UC
#define ARAD_PP_MTR_ETH_TYPE_UNKNOW_MC                       SOC_PPC_MTR_ETH_TYPE_UNKNOW_MC
#define ARAD_PP_MTR_ETH_TYPE_KNOW_MC                         SOC_PPC_MTR_ETH_TYPE_KNOW_MC
#define ARAD_PP_MTR_ETH_TYPE_BC                              SOC_PPC_MTR_ETH_TYPE_BC
#define ARAD_PP_NOF_MTR_ETH_TYPES                            SOC_PPC_NOF_MTR_ETH_TYPES
typedef SOC_PPC_MTR_ETH_TYPE                                   ARAD_PP_MTR_ETH_TYPE;

#define ARAD_PP_MTR_COLOR_MODE_BLIND                         SOC_PPC_MTR_COLOR_MODE_BLIND
#define ARAD_PP_MTR_COLOR_MODE_AWARE                         SOC_PPC_MTR_COLOR_MODE_AWARE
#define ARAD_PP_NOF_MTR_COLOR_MODES                          SOC_PPC_NOF_MTR_COLOR_MODES
typedef SOC_PPC_MTR_COLOR_MODE                                 ARAD_PP_MTR_COLOR_MODE;

#define ARAD_PP_MTR_RES_USE_NONE                             SOC_PPC_MTR_RES_USE_NONE
#define ARAD_PP_MTR_RES_USE_OW_DP                            SOC_PPC_MTR_RES_USE_OW_DP
#define ARAD_PP_MTR_RES_USE_OW_DE                            SOC_PPC_MTR_RES_USE_OW_DE
#define ARAD_PP_MTR_RES_USE_OW_DP_DE                         SOC_PPC_MTR_RES_USE_OW_DP_DE
#define ARAD_PP_NOF_MTR_RES_USES                             SOC_PPC_NOF_MTR_RES_USES
typedef SOC_PPC_MTR_RES_USE                                    ARAD_PP_MTR_RES_USE;
typedef SOC_PPC_MTR_METER_ID                                   ARAD_PP_MTR_METER_ID;
typedef SOC_PPC_MTR_GLBL_INFO                                  ARAD_PP_MTR_GLBL_INFO;
typedef SOC_PPC_MTR_GROUP_INFO                                 ARAD_PP_MTR_GROUP_INFO;
typedef SOC_PPC_MTR_BW_PROFILE_INFO                            ARAD_PP_MTR_BW_PROFILE_INFO;

#define ARAD_PP_BW_PROFILE_IR_MAX_UNLIMITED 0xffffffff
/*********************************************************************
* NAME:
 *   arad_pp_mtr_meters_group_info_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  mtr_group_ndx -
 *     Meters Group. Range 0 - 1.
 *   SOC_SAND_IN  ARAD_PP_MTR_GROUP_INFO                      *mtr_group_info -
 *     Per metering group information
 * REMARKS:
 *   - Relevant only for Arad-B.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_mtr_meters_group_info_set(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  ARAD_PP_MTR_GROUP_INFO                  *mtr_group_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_mtr_meters_group_info_set" API.
 *     Refer to "arad_pp_mtr_meters_group_info_set" API for
 *     details.
*********************************************************************/
uint32
  arad_pp_mtr_meters_group_info_get(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_OUT ARAD_PP_MTR_GROUP_INFO                  *mtr_group_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_mtr_bw_profile_add
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add Bandwidth Profile and set it attributes
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  mtr_group_ndx -
 *     Meters Group. In T20E has to be zero. In Arad-B Range 0
 *     - 1.
 *   SOC_SAND_IN  uint32                                  bw_profile_ndx -
 *     Bandwidth Profile ID
 *   SOC_SAND_IN  ARAD_PP_MTR_BW_PROFILE_INFO                 *bw_profile_info -
 *     Bandwidth profile attributes
 *   SOC_SAND_OUT ARAD_PP_MTR_BW_PROFILE_INFO                 *exact_bw_profile_info -
 *     Exact Bandwidth profile attributes as written to the
 *     device.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success -
 *     Whether the operation succeeds (upon add). Operation may
 *     fail upon unavailable resources. In Arad-B this
 *     operation always success.
 * REMARKS:
 *   This function configures bandwith profile, to assign
 *   meter instance with this profile use
 *   soc_ppd_mtr_meter_ins_to_bw_profile_map_set()- Arad-B if
 *   High-rate metering is enabled then - 0-447 are use for
 *   normal profiles - 448-511 used for high rate profile. if
 *   High-rate metering is disabled then - 0-511 are use for
 *   normal profiles In Normal Profile: Information Rates
 *   (CIR and EIR) are comprised between 64 Kbps and 19 Gbps.
 *   The burst sizes (CBS and EBS) are comprised between 64B
 *   and 1,040,384B. In High-rate Profile: Information Rates
 *   (CIR and EIR) are between 9.6 Gbps and 120 Gbps. The
 *   burst sizes (CBS and EBS) are comprised between 64B and
 *   4,161,536B
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_mtr_bw_profile_add(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_ndx,
    SOC_SAND_IN  ARAD_PP_MTR_BW_PROFILE_INFO             *bw_profile_info,
    SOC_SAND_OUT ARAD_PP_MTR_BW_PROFILE_INFO             *exact_bw_profile_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                *success
  );

/*********************************************************************
* NAME:
 *   arad_pp_mtr_bw_profile_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get Bandwidth Profile attributes
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  mtr_group_ndx -
 *     Meters Group. In T20E has to be zero. In Arad-B Range 0
 *     - 1.
 *   SOC_SAND_IN  uint32                                  bw_profile_ndx -
 *     Bandwidth Profile ID
 *   SOC_SAND_OUT ARAD_PP_MTR_BW_PROFILE_INFO                 *bw_profile_info -
 *     Bandwidth profile attributes
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_mtr_bw_profile_get(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_ndx,
    SOC_SAND_OUT ARAD_PP_MTR_BW_PROFILE_INFO             *bw_profile_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_mtr_bw_profile_remove
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove Bandwidth Profile
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  mtr_group_ndx -
 *     Meters Group. In T20E has to be zero. In Arad-B Range 0
 *     - 1.
 *   SOC_SAND_IN  uint32                                  bw_profile_ndx -
 *     Bandwidth Profile ID
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_mtr_bw_profile_remove(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_ndx
  );

/*********************************************************************
* NAME:
 *   arad_pp_mtr_meter_ins_to_bw_profile_map_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set meter attributes by mapping meter instance to
 *   bandwidth profile. Will also init the bandwidth profile existing
 *	 CBL and EBL counters.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_MTR_METER_ID                        *meter_ins_ndx -
 *     Metering Instance ID
 *   SOC_SAND_IN  uint32                                  bw_profile_id -
 *     bandwidth profile ID.
 * REMARKS:
 *   - in T20E the group in SOC_PPD_MTR_METER_ID has to be zero-
 *   in Arad-B the meter instance mapped into profile in the
 *   same group the meter instance belongs to.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_mtr_meter_ins_to_bw_profile_map_set(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  ARAD_PP_MTR_METER_ID                    *meter_ins_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_id
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_mtr_meter_ins_to_bw_profile_map_set" API.
 *     Refer to "arad_pp_mtr_meter_ins_to_bw_profile_map_set" API
 *     for details.
*********************************************************************/
uint32
  arad_pp_mtr_meter_ins_to_bw_profile_map_get(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  ARAD_PP_MTR_METER_ID                    *meter_ins_ndx,
    SOC_SAND_OUT uint32                                  *bw_profile_id
  );

/*********************************************************************
* NAME:
 *   arad_pp_mtr_eth_policer_enable_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Enable / Disable Ethernet policing.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint8                                 enable -
 *     TRUE: Enable Ethernet policing.
 * REMARKS:
 *   - Arad-B only, if called for T20E error is returned.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_mtr_eth_policer_enable_set(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  uint8                                 enable
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_mtr_eth_policer_enable_set" API.
 *     Refer to "arad_pp_mtr_eth_policer_enable_set" API for
 *     details.
*********************************************************************/
uint32
  arad_pp_mtr_eth_policer_enable_get(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_OUT uint8                                 *enable
  );

/*********************************************************************
* NAME:
 *   arad_pp_mtr_eth_policer_params_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set policer attributes of the Ethernet policer. Enable
 *   policing per ingress port and Ethernet type.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  ARAD_PP_PORT                                port_ndx -
 *     Port ID
 *   SOC_SAND_IN  ARAD_PP_MTR_ETH_TYPE                        eth_type_ndx -
 *     Ethernet traffic type (UC/BC/...)
 *   SOC_SAND_IN  ARAD_PP_MTR_BW_PROFILE_INFO                 *policer_info -
 *     Policer attributes
 * REMARKS:
 *   - Petra-B and ARAD only, if called for T20E error is returned.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_mtr_eth_policer_params_set(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  ARAD_PP_PORT                                port_ndx,
    SOC_SAND_IN  ARAD_PP_MTR_ETH_TYPE                        eth_type_ndx,
    SOC_SAND_IN  ARAD_PP_MTR_BW_PROFILE_INFO                 *policer_info
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_mtr_eth_policer_params_set" API.
 *     Refer to "arad_pp_mtr_eth_policer_params_set" API for
 *     details.
*********************************************************************/
uint32
  arad_pp_mtr_eth_policer_params_get(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  ARAD_PP_PORT                                port_ndx,
    SOC_SAND_IN  ARAD_PP_MTR_ETH_TYPE                        eth_type_ndx,
    SOC_SAND_OUT ARAD_PP_MTR_BW_PROFILE_INFO                 *policer_info
  );

/*********************************************************************
* NAME:         
 *   arad_pp_mtr_eth_policer_glbl_profile_set
 * TYPE:         
 *   PROC        
 * FUNCTION:       
 *   Set Ethernet policer Global Profile attributes.         
 * INPUT:
 *   SOC_SAND_IN  int                 unit - 
 *     Identifier of the device to access.                     
 *   SOC_SAND_IN  uint32                  glbl_profile_idx - 
 *     Global Profile index                                    
 *   SOC_SAND_IN  ARAD_PP_MTR_BW_PROFILE_INFO *policer_info - 
 *     Policer attributes                                      
 * REMARKS:         
 *   - uses only cir, cbs, cir_disable fileds from 
 *   SOC_PPD_MTR_BW_PROFILE_INFO to configure policer attributes. 
 * RETURNS:         
 *   OK or ERROR indication.
*********************************************************************/
uint32  
  arad_pp_mtr_eth_policer_glbl_profile_set(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      glbl_profile_idx,
    SOC_SAND_IN  ARAD_PP_MTR_BW_PROFILE_INFO *policer_info
  );

/*********************************************************************
*     Gets the configuration set by the 
 *     "arad_pp_mtr_eth_policer_glbl_profile_set" API.           
 *     Refer to "arad_pp_mtr_eth_policer_glbl_profile_set" API 
 *     for details.                                            
*********************************************************************/
uint32  
  arad_pp_mtr_eth_policer_glbl_profile_get(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN uint32                       glbl_profile_idx,
    SOC_SAND_OUT ARAD_PP_MTR_BW_PROFILE_INFO *policer_info
  );

/*********************************************************************
* NAME:         
 *   arad_pp_mtr_eth_policer_glbl_profile_map_set
 * TYPE:         
 *   PROC        
 * FUNCTION:       
 *   Map Ethernet policer per ingress port and Ethernet type 
 *   to Ethernet policer Global Profile.                     
 * INPUT:
 *   SOC_SAND_IN  int                 unit - 
 *     Identifier of the device to access.                     
 *   SOC_SAND_IN  ARAD_PP_PORT                port_ndx - 
 *     Port ID                                                 
 *   SOC_SAND_IN  ARAD_PP_MTR_ETH_TYPE        eth_type_ndx - 
 *     Ethernet traffic type (UC/BC/...)                       
 *   SOC_SAND_IN  uint32                  glbl_profile_idx - 
 *     Global Profile index                                    
 * REMARKS:         
 *   None                                                    
 * RETURNS:         
 *   OK or ERROR indication.
*********************************************************************/
uint32  
  arad_pp_mtr_eth_policer_glbl_profile_map_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_PP_PORT                port_ndx,
    SOC_SAND_IN  ARAD_PP_MTR_ETH_TYPE        eth_type_ndx,
    SOC_SAND_IN  uint32                  glbl_profile_idx
  );

/*********************************************************************
*     Gets the configuration set by the 
 *     "arad_pp_mtr_eth_policer_glbl_profile_map_set" API.       
 *     Refer to "arad_pp_mtr_eth_policer_glbl_profile_map_set" 
 *     API for details.                                        
*********************************************************************/
uint32  
  arad_pp_mtr_eth_policer_glbl_profile_map_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_PP_PORT                port_ndx,
    SOC_SAND_IN  ARAD_PP_MTR_ETH_TYPE        eth_type_ndx,
    SOC_SAND_OUT uint32                  *glbl_profile_idx
  );
void
  ARAD_PP_MTR_GROUP_INFO_clear(
    SOC_SAND_OUT ARAD_PP_MTR_GROUP_INFO *info
  );

void
  ARAD_PP_MTR_BW_PROFILE_INFO_clear(
    SOC_SAND_OUT ARAD_PP_MTR_BW_PROFILE_INFO *info
  );

#if ARAD_PP_DEBUG_IS_LVL1

const char*
  ARAD_PP_MTR_ETH_TYPE_to_string(
    SOC_SAND_IN  ARAD_PP_MTR_ETH_TYPE enum_val
  );

const char*
  ARAD_PP_MTR_COLOR_MODE_to_string(
    SOC_SAND_IN  ARAD_PP_MTR_COLOR_MODE enum_val
  );

const char*
  ARAD_PP_MTR_RES_USE_to_string(
    SOC_SAND_IN  ARAD_PP_MTR_RES_USE enum_val
  );
void
  ARAD_PP_MTR_METER_ID_print(
    SOC_SAND_IN  ARAD_PP_MTR_METER_ID *info
  );

void
  ARAD_PP_MTR_GLBL_INFO_print(
    SOC_SAND_IN  ARAD_PP_MTR_GLBL_INFO *info
  );

void
  ARAD_PP_MTR_GROUP_INFO_print(
    SOC_SAND_IN  ARAD_PP_MTR_GROUP_INFO *info
  );

void
  ARAD_PP_MTR_BW_PROFILE_INFO_print(
    SOC_SAND_IN  ARAD_PP_MTR_BW_PROFILE_INFO *info
  );

#endif /* ARAD_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_PP_API_METERING_INCLUDED__*/
#endif

