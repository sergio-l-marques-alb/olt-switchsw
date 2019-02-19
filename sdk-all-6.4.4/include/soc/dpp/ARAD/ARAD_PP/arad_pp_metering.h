/* $Id: arad_pp_metering.h,v 1.16 Broadcom SDK $
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

#ifndef __ARAD_PP_METERING_INCLUDED__
/* { */
#define __ARAD_PP_METERING_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_api_metering.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_PROFILE_METER_PROFILE_NOF_MEMBER 32
#define ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_GLOBAL_METER_PROFILE_NOF_MEMBER 8
/* IDR_ETHERNET_METER_CONFIGm has 1300 entry */
#define ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_CONFIG_METER_PROFILE_NOF_MEMBER_BIT 1300
#define ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_CONFIG_METER_PROFILE_NOF_MEMBER_BYTE 41
/* IDR_GLOBAL_METER_PROFILESm has 8 entry */
#define ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_ENABLE_STATUS_BIT_MAIN 1308
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

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  ARAD_PP_MTR_GLBL_INFO_SET = ARAD_PP_PROC_DESC_BASE_METERING_FIRST,
  ARAD_PP_MTR_GLBL_INFO_SET_PRINT,
  ARAD_PP_MTR_GLBL_INFO_SET_UNSAFE,
  ARAD_PP_MTR_GLBL_INFO_SET_VERIFY,
  ARAD_PP_MTR_GLBL_INFO_GET,
  ARAD_PP_MTR_GLBL_INFO_GET_PRINT,
  ARAD_PP_MTR_GLBL_INFO_GET_VERIFY,
  ARAD_PP_MTR_GLBL_INFO_GET_UNSAFE,
  ARAD_PP_MTR_METERS_GROUP_INFO_SET,
  ARAD_PP_MTR_METERS_GROUP_INFO_SET_PRINT,
  ARAD_PP_MTR_METERS_GROUP_INFO_SET_UNSAFE,
  ARAD_PP_MTR_METERS_GROUP_INFO_SET_VERIFY,
  ARAD_PP_MTR_METERS_GROUP_INFO_GET,
  ARAD_PP_MTR_METERS_GROUP_INFO_GET_PRINT,
  ARAD_PP_MTR_METERS_GROUP_INFO_GET_VERIFY,
  ARAD_PP_MTR_METERS_GROUP_INFO_GET_UNSAFE,
  ARAD_PP_MTR_BW_PROFILE_ADD,
  ARAD_PP_MTR_BW_PROFILE_ADD_PRINT,
  ARAD_PP_MTR_BW_PROFILE_ADD_UNSAFE,
  ARAD_PP_MTR_BW_PROFILE_ADD_VERIFY,
  ARAD_PP_MTR_BW_PROFILE_GET,
  ARAD_PP_MTR_BW_PROFILE_GET_PRINT,
  ARAD_PP_MTR_BW_PROFILE_GET_UNSAFE,
  ARAD_PP_MTR_BW_PROFILE_GET_VERIFY,
  ARAD_PP_MTR_BW_PROFILE_REMOVE,
  ARAD_PP_MTR_BW_PROFILE_REMOVE_PRINT,
  ARAD_PP_MTR_BW_PROFILE_REMOVE_UNSAFE,
  ARAD_PP_MTR_BW_PROFILE_REMOVE_VERIFY,
  ARAD_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_SET,
  ARAD_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_SET_PRINT,
  ARAD_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_SET_UNSAFE,
  ARAD_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_SET_VERIFY,
  ARAD_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_GET,
  ARAD_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_GET_PRINT,
  ARAD_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_GET_VERIFY,
  ARAD_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_GET_UNSAFE,
  ARAD_PP_MTR_ETH_POLICER_ENABLE_SET,
  ARAD_PP_MTR_ETH_POLICER_ENABLE_SET_PRINT,
  ARAD_PP_MTR_ETH_POLICER_ENABLE_SET_UNSAFE,
  ARAD_PP_MTR_ETH_POLICER_ENABLE_SET_VERIFY,
  ARAD_PP_MTR_ETH_POLICER_ENABLE_GET,
  ARAD_PP_MTR_ETH_POLICER_ENABLE_GET_PRINT,
  ARAD_PP_MTR_ETH_POLICER_ENABLE_GET_VERIFY,
  ARAD_PP_MTR_ETH_POLICER_ENABLE_GET_UNSAFE,
  ARAD_PP_MTR_ETH_POLICER_PARAMS_SET,
  ARAD_PP_MTR_ETH_POLICER_PARAMS_SET_PRINT,
  ARAD_PP_MTR_ETH_POLICER_PARAMS_SET_UNSAFE,
  ARAD_PP_MTR_ETH_POLICER_PARAMS_SET_VERIFY,
  ARAD_PP_MTR_ETH_POLICER_PARAMS_GET,
  ARAD_PP_MTR_ETH_POLICER_PARAMS_GET_PRINT,
  ARAD_PP_MTR_ETH_POLICER_PARAMS_GET_VERIFY,
  ARAD_PP_MTR_ETH_POLICER_PARAMS_GET_UNSAFE,
  ARAD_PP_METERING_INIT_UNSAFE,
  ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_SET,
  ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_SET_PRINT,
  ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_SET_UNSAFE,
  ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_SET_VERIFY,
  ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_GET,
  ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_GET_PRINT,
  ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_GET_VERIFY,
  ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_GET_UNSAFE,
  ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET,
  ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET_PRINT,
  ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET_UNSAFE,
  ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET_VERIFY,
  ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_GET,
  ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_GET_PRINT,
  ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_GET_VERIFY,
  ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_GET_UNSAFE,
  ARAD_PP_METERING_GET_PROCS_PTR,
  ARAD_PP_METERING_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */
  ARAD_PP_MTR_IR_VAL_FROM_REVERSE_EXP_MNT,
  ARAD_PP_MTR_BS_VAL_TO_EXP_MNT,
  ARAD_PP_MTR_CHK_PROFILE_RATE,
  ARAD_PP_MTR_IR_VAL_TO_MAX_REV_EXP,
  ARAD_PP_MTR_PROFILE_RATE_TO_RES_EXP_MNT,
  ARAD_PP_METERING_PCD_INIT,

  /*
   * Last element. Do no touch.
   */
  ARAD_PP_METERING_PROCEDURE_DESC_LAST
} ARAD_PP_METERING_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  ARAD_PP_METERING_MTR_GROUP_NDX_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_METERING_FIRST,
  ARAD_PP_METERING_BW_PROFILE_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_METERING_SUCCESS_OUT_OF_RANGE_ERR,
  ARAD_PP_METERING_BW_PROFILE_ID_OUT_OF_RANGE_ERR,
  ARAD_PP_METERING_RESULT_USE_OUT_OF_RANGE_ERR,
  ARAD_PP_METERING_METERED_TYPE_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_METERING_FIRST_METERED_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_METERING_NOF_METER_INS_OUT_OF_RANGE_ERR,
  ARAD_PP_METERING_ETH_TYPE_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_METERING_GLBL_PROFILE_IDX_OUT_OF_RANGE_ERR,
  ARAD_PP_METERING_GROUP_OUT_OF_RANGE_ERR,
  ARAD_PP_METERING_ID_OUT_OF_RANGE_ERR,
  ARAD_PP_METERING_IS_HR_ENABLED_OUT_OF_RANGE_ERR,
  ARAD_PP_METERING_MAX_PACKET_SIZE_OUT_OF_RANGE_ERR,
  ARAD_PP_METERING_CIR_OUT_OF_RANGE_ERR,
  ARAD_PP_METERING_EIR_OUT_OF_RANGE_ERR,
  ARAD_PP_METERING_CBS_OUT_OF_RANGE_ERR,
  ARAD_PP_METERING_EBS_OUT_OF_RANGE_ERR,
  ARAD_PP_METERING_COLOR_MODE_OUT_OF_RANGE_ERR,
  ARAD_PP_METERING_TYPE_OUT_OF_RANGE_ERR,
  /*
   * } Auto generated. Do not edit previous section.
   */
  
  ARAD_PP_METERING_CHK_PROFILE_RATE_ERR,
  ARAD_PP_METERING_VAL_FROM_REVERSE_EXP_MNT_CLK_OF_ERR,
  ARAD_PP_METERING_UPDATE_ALL_COPIES_MAX_RANGE_ERR,
  ARAD_PP_METERING_BW_PROFILE_HR_RATE_ERR,
  ARAD_PP_METERING_BW_PROFILE_LR_RATE_ERR,
  ARAD_PP_MTR_ETH_POLICER_ADD_FAIL_ERR,
  ARAD_PP_MTR_ETH_POLICER_PARAMS_SET_VERIFY_OUT_OF_RANGE_ERR,
  ARAD_PP_METERING_POLICER_INGRESS_COUNT_INVALID_ERR,
  ARAD_PP_METERING_POLICER_INGRESS_SHARING_MODE_INVALID_ERR,
  ARAD_PP_METERING_MAX_IR_INVALID_ERR,
  ARAD_PP_METERING_POLICER_RESULT_PARALLEL_COLOR_MAPPING_INVALID_ERR,
  ARAD_PP_METERING_POLICER_RESULT_PARALLEL_BUCKET_UPDATE_INVALID_ERR,
  
  /*
   * Last element. Do no touch.
   */
  ARAD_PP_METERING_ERR_LAST
} ARAD_PP_METERING_ERR;

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
  arad_pp_metering_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );
/*********************************************************************
* NAME:
 *   arad_pp_mtr_meters_group_info_set_unsafe
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
  arad_pp_mtr_meters_group_info_set_unsafe(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  ARAD_PP_MTR_GROUP_INFO                  *mtr_group_info
  );

uint32
  arad_pp_mtr_meters_group_info_set_verify(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  ARAD_PP_MTR_GROUP_INFO                  *mtr_group_info
  );

uint32
  arad_pp_mtr_meters_group_info_get_verify(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint32                                  mtr_group_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_mtr_meters_group_info_set_unsafe" API.
 *     Refer to "arad_pp_mtr_meters_group_info_set_unsafe" API
 *     for details.
*********************************************************************/
uint32
  arad_pp_mtr_meters_group_info_get_unsafe(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_OUT ARAD_PP_MTR_GROUP_INFO                  *mtr_group_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_mtr_bw_profile_add_unsafe
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
  arad_pp_mtr_bw_profile_add_unsafe(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_ndx,
    SOC_SAND_IN  ARAD_PP_MTR_BW_PROFILE_INFO             *bw_profile_info,
    SOC_SAND_OUT ARAD_PP_MTR_BW_PROFILE_INFO             *exact_bw_profile_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                *success
  );

uint32
  arad_pp_mtr_bw_profile_add_verify(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_ndx,
    SOC_SAND_IN  ARAD_PP_MTR_BW_PROFILE_INFO             *bw_profile_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_mtr_bw_profile_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get Bandwidth Profile attributes
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  mtr_group_ndx -
 *     Meters Group. In T20E has to be zero.
 *     In Petra-B Range 0- 1.
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
  arad_pp_mtr_bw_profile_get_unsafe(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_ndx,
    SOC_SAND_OUT ARAD_PP_MTR_BW_PROFILE_INFO             *bw_profile_info
  );

uint32
  arad_pp_mtr_bw_profile_get_verify(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_ndx
  );

/*********************************************************************
* NAME:
 *   arad_pp_mtr_bw_profile_remove_unsafe
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
  arad_pp_mtr_bw_profile_remove_unsafe(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_ndx
  );

uint32
  arad_pp_mtr_bw_profile_remove_verify(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_ndx
  );

/*********************************************************************
* NAME:
 *   arad_pp_mtr_meter_ins_to_bw_profile_map_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set meter attributes by mapping meter instance to
 *   bandwidth profile.
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
  arad_pp_mtr_meter_ins_to_bw_profile_map_set_unsafe(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  ARAD_PP_MTR_METER_ID                    *meter_ins_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_id
  );

uint32
  arad_pp_mtr_meter_ins_to_bw_profile_map_set_verify(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  ARAD_PP_MTR_METER_ID                    *meter_ins_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_id
  );

uint32
  arad_pp_mtr_meter_ins_to_bw_profile_map_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_MTR_METER_ID                   *meter_ins_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_mtr_meter_ins_to_bw_profile_map_set_unsafe" API.
 *     Refer to
 *     "arad_pp_mtr_meter_ins_to_bw_profile_map_set_unsafe" API
 *     for details.
*********************************************************************/
uint32
  arad_pp_mtr_meter_ins_to_bw_profile_map_get_unsafe(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  ARAD_PP_MTR_METER_ID                    *meter_ins_ndx,
    SOC_SAND_OUT uint32                                  *bw_profile_id
  );

/*********************************************************************
* NAME:
 *   arad_pp_mtr_eth_policer_enable_set_unsafe
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
  arad_pp_mtr_eth_policer_enable_set_unsafe(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  uint8                                 enable
  );

uint32
  arad_pp_mtr_eth_policer_enable_set_verify(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  uint8                                 enable
  );

uint32
  arad_pp_mtr_eth_policer_enable_get_verify(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_mtr_eth_policer_enable_set_unsafe" API.
 *     Refer to "arad_pp_mtr_eth_policer_enable_set_unsafe" API
 *     for details.
*********************************************************************/
uint32
  arad_pp_mtr_eth_policer_enable_get_unsafe(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_OUT uint8                                 *enable
  );

/*********************************************************************
* NAME:
 *   arad_pp_mtr_eth_policer_params_set_unsafe
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
 *   - Arad-B only, if called for T20E error is returned.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_mtr_eth_policer_params_set_unsafe(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  ARAD_PP_PORT                                port_ndx,
    SOC_SAND_IN  ARAD_PP_MTR_ETH_TYPE                        eth_type_ndx,
    SOC_SAND_IN  ARAD_PP_MTR_BW_PROFILE_INFO                 *policer_info
  );

uint32
  arad_pp_mtr_eth_policer_params_set_verify(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  ARAD_PP_PORT                                port_ndx,
    SOC_SAND_IN  ARAD_PP_MTR_ETH_TYPE                        eth_type_ndx,
    SOC_SAND_IN  ARAD_PP_MTR_BW_PROFILE_INFO                 *policer_info
  );

uint32
  arad_pp_mtr_eth_policer_params_get_verify(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  ARAD_PP_PORT                                port_ndx,
    SOC_SAND_IN  ARAD_PP_MTR_ETH_TYPE                        eth_type_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_mtr_eth_policer_params_set_unsafe" API.
 *     Refer to "arad_pp_mtr_eth_policer_params_set_unsafe" API
 *     for details.
*********************************************************************/
uint32
  arad_pp_mtr_eth_policer_params_get_unsafe(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  ARAD_PP_PORT                                port_ndx,
    SOC_SAND_IN  ARAD_PP_MTR_ETH_TYPE                        eth_type_ndx,
    SOC_SAND_OUT ARAD_PP_MTR_BW_PROFILE_INFO                 *policer_info
  );

/*********************************************************************
* NAME:         
 *   arad_pp_mtr_eth_policer_glbl_profile_set_unsafe
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
  arad_pp_mtr_eth_policer_glbl_profile_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      glbl_profile_idx,
    SOC_SAND_IN  ARAD_PP_MTR_BW_PROFILE_INFO *policer_info
  );

uint32  
  arad_pp_mtr_eth_policer_glbl_profile_set_verify(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  uint32                       glbl_profile_idx,
    SOC_SAND_IN  ARAD_PP_MTR_BW_PROFILE_INFO *policer_info
  );

uint32  
  arad_pp_mtr_eth_policer_glbl_profile_get_verify(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  glbl_profile_idx
  );

/*********************************************************************
*     Gets the configuration set by the 
 *     "arad_pp_mtr_eth_policer_glbl_profile_set_unsafe" API.    
 *     Refer to "arad_pp_mtr_eth_policer_glbl_profile_set_unsafe" 
 *     API for details.                                        
*********************************************************************/
uint32  
  arad_pp_mtr_eth_policer_glbl_profile_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      glbl_profile_idx,
    SOC_SAND_OUT ARAD_PP_MTR_BW_PROFILE_INFO *policer_info
  );

/*********************************************************************
* NAME:         
 *   arad_pp_mtr_eth_policer_glbl_profile_map_set_unsafe
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
  arad_pp_mtr_eth_policer_glbl_profile_map_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_PP_PORT                port_ndx,
    SOC_SAND_IN  ARAD_PP_MTR_ETH_TYPE        eth_type_ndx,
    SOC_SAND_IN  uint32                      glbl_profile_idx
  );

uint32  
  arad_pp_mtr_eth_policer_glbl_profile_map_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_PP_PORT                port_ndx,
    SOC_SAND_IN  ARAD_PP_MTR_ETH_TYPE        eth_type_ndx,
    SOC_SAND_IN  uint32                      glbl_profile_idx
  );

uint32  
  arad_pp_mtr_eth_policer_glbl_profile_map_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_PP_PORT                port_ndx,
    SOC_SAND_IN  ARAD_PP_MTR_ETH_TYPE        eth_type_ndx
  );

/*********************************************************************
*     Gets the configuration set by the 
 *     "arad_pp_mtr_eth_policer_glbl_profile_map_set_unsafe" API. 
 *     Refer to 
 *     "arad_pp_mtr_eth_policer_glbl_profile_map_set_unsafe" API 
 *     for details.                                            
*********************************************************************/
uint32  
  arad_pp_mtr_eth_policer_glbl_profile_map_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_PP_PORT                port_ndx,
    SOC_SAND_IN  ARAD_PP_MTR_ETH_TYPE        eth_type_ndx,
    SOC_SAND_OUT uint32                      *glbl_profile_idx
  );

/*********************************************************************
* NAME:
 *   arad_pp_metering_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   arad_pp_api_metering module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_metering_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   arad_pp_metering_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   arad_pp_api_metering module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_metering_get_errs_ptr(void);

uint32
  ARAD_PP_MTR_METER_ID_verify(
    SOC_SAND_IN  ARAD_PP_MTR_METER_ID *info
  );
uint32
  ARAD_PP_MTR_GROUP_INFO_verify(
    SOC_SAND_IN  ARAD_PP_MTR_GROUP_INFO *info
  );

uint32
  ARAD_PP_MTR_BW_PROFILE_INFO_verify(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  ARAD_PP_MTR_BW_PROFILE_INFO *info
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_PP_METERING_INCLUDED__*/
#endif

