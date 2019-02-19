/* $Id: pb_pp_metering.c,v 1.11 Broadcom SDK $
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

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/Petra/PB_TM/pb_api_mgmt.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_metering.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_sw_db.h>

#include <soc/dpp/Petra/PB_TM/pb_tbl_access.h>
#include <soc/dpp/SAND/Utils/sand_multi_set.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PP_METERING_MTR_GROUP_NDX_MAX                       (1)
#define SOC_PB_PP_METERING_BW_PROFILE_ID_MAX                       (511)
#define SOC_PB_PP_METERING_BW_PROFILE_NDX_MAX                      (511)
#define SOC_PB_PP_METERING_ETH_TYPE_NDX_MAX                        (SOC_PB_PP_NOF_MTR_ETH_TYPES-1)
#define SOC_PB_PP_METERING_GLBL_PROFILE_IDX_MIN                    (1)
#define SOC_PB_PP_METERING_GLBL_PROFILE_IDX_MAX                    (8 - 1)
#define SOC_PB_PP_METERING_GROUP_MAX                               (SOC_PB_PP_METERING_MTR_GROUP_NDX_MAX)
#define SOC_PB_PP_METERING_ID_MAX                                  (8192)
#define SOC_PB_PP_METERING_MAX_PACKET_SIZE_MAX                     (16*1024-1)
#define SOC_PB_PP_METERING_COLOR_MODE_MAX                          (SOC_PB_PP_NOF_MTR_COLOR_MODES-1)
#define SOC_PB_PP_METERING_UPDATE_ALL_COPIES_MAX                   (1)

#define SOC_PB_PP_MTR_VAL_FROM_REVERSE_EXP_MNT_CLK_OF 8000

#define SOC_PB_PP_MTR_VAL_TO_EXP_MNT_REVERSE_LR_MAX_EXP 18
#define SOC_PB_PP_MTR_VAL_TO_EXP_MNT_REVERSE_HR_MAX_EXP 3
#define SOC_PB_PP_MTR_VAL_TO_EXP_MNT_LR_MAX_EXP 13
#define SOC_PB_PP_MTR_VAL_TO_EXP_MNT_HR_MAX_EXP 15
#define SOC_PB_PP_MTR_VAL_TO_EXP_MNT_ETH_POLICER_RATE_MAX_EXP 11 /* HW can support more. */
#define SOC_PB_PP_MTR_VAL_TO_EXP_MNT_ETH_POLICER_BURST_MAX_EXP 11 /* HW can support more */

#define SOC_PB_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT 63

#define SOC_PB_PP_MTR_VAL_EXP_MNT_EQ_CONST_MULTI 1
#define SOC_PB_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC 64
#define SOC_PB_PP_MTR_VAL_EXP_MNT_EQ_CONST_MULTI_HR 4000
#define SOC_PB_PP_MTR_VAL_EXP_MNT_EQ_CONST_MULTI_ETH_POLICER_RATE_PETRAB_A0 128
#define SOC_PB_PP_MTR_VAL_EXP_MNT_EQ_CONST_MULTI_ETH_POLICER_RATE_PETRAB_B0 2
#define SOC_PB_PP_MTR_VAL_EXP_MNT_EQ_CONST_MULTI_ETH_POLICER_BURST 256

#define SOC_PB_PP_MTR_VAL_EXP_MNT_EQ_CONST_MULTI_LR 500

#define SOC_PB_PP_BW_PROFILE_ADD_VERIFY_HR_BS_MAX 4161536
#define SOC_PB_PP_BW_PROFILE_ADD_VERIFY_HR_BS_MIN 64
#define SOC_PB_PP_BW_PROFILE_ADD_VERIFY_HR_IR_MAX 120000000
#define SOC_PB_PP_BW_PROFILE_ADD_VERIFY_HR_IR_MIN 9660000
#define SOC_PB_PP_BW_PROFILE_ADD_VERIFY_LR_BS_MAX 1040384
#define SOC_PB_PP_BW_PROFILE_ADD_VERIFY_LR_BS_MIN 64
#define SOC_PB_PP_BW_PROFILE_ADD_VERIFY_LR_IR_MAX 19000000
#define SOC_PB_PP_BW_PROFILE_ADD_VERIFY_LR_IR_MIN 64

#define SOC_PB_PP_METERING_BW_PROFILE_NDX_HR_MIN 496

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

/* } */
/*************
 * GLOBALS   *
 *************/
/* { */

static SOC_PROCEDURE_DESC_ELEMENT
  Soc_pb_pp_procedure_desc_element_metering[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_GLBL_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_GLBL_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_GLBL_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_GLBL_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_GLBL_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_GLBL_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_GLBL_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_GLBL_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_METERS_GROUP_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_METERS_GROUP_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_METERS_GROUP_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_METERS_GROUP_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_METERS_GROUP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_METERS_GROUP_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_METERS_GROUP_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_METERS_GROUP_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_BW_PROFILE_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_BW_PROFILE_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_BW_PROFILE_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_BW_PROFILE_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_BW_PROFILE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_BW_PROFILE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_BW_PROFILE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_BW_PROFILE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_BW_PROFILE_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_BW_PROFILE_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_BW_PROFILE_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_BW_PROFILE_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_ETH_POLICER_ENABLE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_ETH_POLICER_ENABLE_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_ETH_POLICER_ENABLE_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_ETH_POLICER_ENABLE_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_ETH_POLICER_ENABLE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_ETH_POLICER_ENABLE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_ETH_POLICER_ENABLE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_ETH_POLICER_ENABLE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_ETH_POLICER_PARAMS_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_ETH_POLICER_PARAMS_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_ETH_POLICER_PARAMS_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_ETH_POLICER_PARAMS_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_ETH_POLICER_PARAMS_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_ETH_POLICER_PARAMS_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_ETH_POLICER_PARAMS_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_ETH_POLICER_PARAMS_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_ETH_POLICER_GLBL_PROFILE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_ETH_POLICER_GLBL_PROFILE_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_ETH_POLICER_GLBL_PROFILE_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_ETH_POLICER_GLBL_PROFILE_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_ETH_POLICER_GLBL_PROFILE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_ETH_POLICER_GLBL_PROFILE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_ETH_POLICER_GLBL_PROFILE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_ETH_POLICER_GLBL_PROFILE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_METERING_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_METERING_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */

  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_VAL_FROM_REVERSE_EXP_MNT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_VAL_TO_EXP_MNT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_PP_MTR_CHK_PROFILE_RATE),

  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

static SOC_ERROR_DESC_ELEMENT
  Soc_pb_pp_error_desc_element_metering[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    SOC_PB_PP_METERING_MTR_GROUP_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_METERING_MTR_GROUP_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'mtr_group_ndx' is out of range. \n\r "
    "The range is: 0 - 1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_METERING_BW_PROFILE_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_METERING_BW_PROFILE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'bw_profile_ndx' is out of range. \n\r "
    "The range is: 0 - 511.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_METERING_SUCCESS_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_METERING_SUCCESS_OUT_OF_RANGE_ERR",
    "The parameter 'success' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_NOF_SUCCESS_FAILURES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_METERING_BW_PROFILE_ID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_METERING_BW_PROFILE_ID_OUT_OF_RANGE_ERR",
    "The parameter 'bw_profile_id' is out of range. \n\r "
    "The range is: 0 - 511.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_METERING_RESULT_USE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_METERING_RESULT_USE_OUT_OF_RANGE_ERR",
    "The parameter 'result_use' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_MTR_RES_USES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_METERING_METERED_TYPE_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_METERING_METERED_TYPE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'metered_type_ndx' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_MTR_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_METERING_FIRST_METERED_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_METERING_FIRST_METERED_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'first_metered_ndx' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_MTR_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_METERING_NOF_METER_INS_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_METERING_NOF_METER_INS_OUT_OF_RANGE_ERR",
    "The parameter 'nof_meter_ins' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_MTR_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_METERING_ETH_TYPE_NDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_METERING_ETH_TYPE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'eth_type_ndx' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_MTR_ETH_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_METERING_GLBL_PROFILE_IDX_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_METERING_GLBL_PROFILE_IDX_OUT_OF_RANGE_ERR",
    "The parameter 'glbl_profile_idx' is out of range. \n\r "
    "The range is: 1 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_METERING_GROUP_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_METERING_GROUP_OUT_OF_RANGE_ERR",
    "The parameter 'group' is out of range. \n\r "
    "The range is: 0 - 1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_METERING_ID_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_METERING_ID_OUT_OF_RANGE_ERR",
    "The parameter 'id' is out of range. \n\r "
    "The range is: 0 - 511.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_METERING_IS_HR_ENABLED_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_METERING_IS_HR_ENABLED_OUT_OF_RANGE_ERR",
    "The parameter 'is_hr_enabled' is out of range. \n\r "
    "The range is: 0 - 1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_METERING_MAX_PACKET_SIZE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_METERING_MAX_PACKET_SIZE_OUT_OF_RANGE_ERR",
    "The parameter 'max_packet_size' is out of range. \n\r "
    "The range is: 0 - 16*1024-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_METERING_CIR_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_METERING_CIR_OUT_OF_RANGE_ERR",
    "The parameter 'cir' is out of range. \n\r "
    "The range is: 64K - 120G.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_METERING_EIR_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_METERING_EIR_OUT_OF_RANGE_ERR",
    "The parameter 'eir' is out of range. \n\r "
    "The range is: 64K - 120G.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_METERING_CBS_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_METERING_CBS_OUT_OF_RANGE_ERR",
    "The parameter 'cbs' is out of range. \n\r "
    "The range is: 64B - 4161536B.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_METERING_EBS_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_METERING_EBS_OUT_OF_RANGE_ERR",
    "The parameter 'ebs' is out of range. \n\r "
    "The range is: 64B - 4161536B.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_METERING_COLOR_MODE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_METERING_COLOR_MODE_OUT_OF_RANGE_ERR",
    "The parameter 'color_mode' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_MTR_COLOR_MODES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_METERING_TYPE_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_METERING_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'type' is out of range. \n\r "
    "The range is: 0 - SOC_PB_PP_NOF_MTR_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */
  {
    SOC_PB_PP_METERING_CHK_PROFILE_RATE_ERR,
    "SOC_PB_PP_METERING_CHK_PROFILE_RATE_ERR",
    "The parameters BW_PROFILE are mixed High rate - Low rate. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_METERING_VAL_FROM_REVERSE_EXP_MNT_CLK_OF_ERR,
    "SOC_PB_PP_METERING_VAL_FROM_REVERSE_EXP_MNT_CLK_OF_ERR",
    "The clock core frequency may cause Overflow. \n\r "
    "The range is: 0 - SOC_PB_PP_METERING_VAL_FROM_REVERSE_EXP_MNT_CLK_OF.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_METERING_UPDATE_ALL_COPIES_MAX_RANGE_ERR,
    "SOC_PB_PP_METERING_UPDATE_ALL_COPIES_MAX_RANGE_ERR",
    "The parameter 'update_all_copies' is out of range. \n\r "
    "The range is: 0 - 1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_METERING_BW_PROFILE_HR_RATE_ERR,
    "SOC_PB_PP_METERING_BW_PROFILE_HR_RATE_ERR",
    "Profile rate parameters are for High Rate. \n\r "
    "MeterProcc is in Low rate or profile index is for Low rate\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_METERING_BW_PROFILE_LR_RATE_ERR,
    "SOC_PB_PP_METERING_BW_PROFILE_LR_RATE_ERR",
    "Profile rate parameters are for Low Rate. \n\r "
    "MeterProcc is in High rate and Profile index is for High rate\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_MTR_ETH_POLICER_ADD_FAIL_ERR,
    "SOC_PB_PP_MTR_ETH_POLICER_ADD_FAIL_ERR",
    "Fail to add profile to multiset. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_PP_MTR_ETH_POLICER_PARAMS_SET_VERIFY_OUT_OF_RANGE_ERR,
    "SOC_PB_PP_MTR_ETH_POLICER_PARAMS_SET_VERIFY_OUT_OF_RANGE_ERR",
    "One of the parameters: color_mode, is_coupling_enabled, is_fairness_enabled, disable_cir, disable_eir is out of range. \n\r "
    "This parameters value should be 0 (FALSE), in eth policer. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * Last element. Do no touch.
   */
  SOC_ERR_DESC_ELEMENT_DEF_LAST
};

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

uint32
  soc_pb_pp_mtr_is_b0_fix(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT uint8                                  *is_b0_fix
  )
{
  uint32
    res = SOC_SAND_OK,
    meter_mode = 0;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(is_b0_fix);

  regs = soc_petra_regs();

  SOC_PB_FLD_GET(regs->idr.soc_petra_c_fix.meter_mode_1 , meter_mode, 10, exit);

  /* configure eq_const_multi_eth_policer_rate according to soc_petraB version */
  if ((SOC_PB_REV_A1_OR_BELOW == FALSE) && (meter_mode == 0x1))
  {
    *is_b0_fix = TRUE;
  }
  else
  {
    *is_b0_fix = FALSE;
  }
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mtr_chk_profile_rate()", 0, 0);
}

uint32
  soc_pb_pp_mtr_is_b0_eq_const_multi_eth_policer_rate(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT uint32                                 *eq_const_multi
  )
{
  uint32
    res;
  uint8
    is_b0_fix;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(eq_const_multi);

  res = soc_pb_pp_mtr_is_b0_fix(
          unit,
          &is_b0_fix
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  /* configure eq_const_multi_eth_policer_rate according to soc_petraB version */
  if (is_b0_fix)
  {
    *eq_const_multi = SOC_PB_PP_MTR_VAL_EXP_MNT_EQ_CONST_MULTI_ETH_POLICER_RATE_PETRAB_B0;
  }
  else
  {
    *eq_const_multi = SOC_PB_PP_MTR_VAL_EXP_MNT_EQ_CONST_MULTI_ETH_POLICER_RATE_PETRAB_A0;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mtr_chk_profile_rate()", 0, 0);
}

STATIC uint32
  soc_pb_pp_mtr_chk_profile_rate(
    SOC_SAND_IN int                      unit,
    SOC_SAND_IN uint32                       cir,
    SOC_SAND_IN uint32                       eir,
    SOC_SAND_IN uint32                       cbs,
    SOC_SAND_IN uint32                       ebs,
    SOC_SAND_OUT uint8                     *is_hr
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MTR_CHK_PROFILE_RATE);

  SOC_SAND_CHECK_NULL_INPUT(is_hr);

  /* Check if profile is High rate */
  if ((cbs > SOC_PB_PP_BW_PROFILE_ADD_VERIFY_LR_BS_MAX) || (ebs > SOC_PB_PP_BW_PROFILE_ADD_VERIFY_LR_BS_MAX) ||
      (cir > SOC_PB_PP_BW_PROFILE_ADD_VERIFY_LR_IR_MAX) || (eir > SOC_PB_PP_BW_PROFILE_ADD_VERIFY_LR_IR_MAX))
  {
    /* All attributes should be in High rate */
    if ((cbs < SOC_PB_PP_BW_PROFILE_ADD_VERIFY_HR_BS_MIN) || (ebs < SOC_PB_PP_BW_PROFILE_ADD_VERIFY_HR_BS_MIN) ||
        (cir < SOC_PB_PP_BW_PROFILE_ADD_VERIFY_HR_IR_MIN) || (eir < SOC_PB_PP_BW_PROFILE_ADD_VERIFY_HR_IR_MIN))
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_METERING_CHK_PROFILE_RATE_ERR, 10, exit);
    }
    else
    {
      *is_hr = TRUE;
    }
  }
  else
  {
    *is_hr = FALSE;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mtr_chk_profile_rate()", 0, 0);
}

STATIC uint32
  soc_pb_pp_mtr_val_from_reverse_exp_mnt(
    SOC_SAND_IN int                      unit,
    SOC_SAND_IN uint8                      is_hr,
    SOC_SAND_IN uint32                       exp,
    SOC_SAND_IN uint32                       mnt,
    SOC_SAND_OUT uint32                      *val
  )
{
  uint32
    res = SOC_SAND_OK,
    clk_freq;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MTR_VAL_FROM_REVERSE_EXP_MNT);

  SOC_SAND_CHECK_NULL_INPUT(val);

  res = soc_pb_mgmt_core_frequency_get(
          unit,
          &clk_freq
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (clk_freq > SOC_PB_PP_MTR_VAL_FROM_REVERSE_EXP_MNT_CLK_OF)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_METERING_VAL_FROM_REVERSE_EXP_MNT_CLK_OF_ERR, 10, exit);
  }

  /*Equation to calculate value from exponent and mantissa depends on High/Low Rate work mode */
  if (is_hr)
  {
    res = soc_sand_compute_complex_to_mnt_reverse_exp(
            mnt,
            exp,
            clk_freq * SOC_PB_PP_MTR_VAL_EXP_MNT_EQ_CONST_MULTI_HR,
            SOC_PB_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC,
            val
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  else
  {
    res = soc_sand_compute_complex_to_mnt_reverse_exp(
            mnt,
            exp,
            clk_freq * SOC_PB_PP_MTR_VAL_EXP_MNT_EQ_CONST_MULTI_LR,
            SOC_PB_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC,
            val
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mtr_val_from_reverse_exp_mnt()", 0, 0);
}

STATIC uint32
  soc_pb_pp_mtr_val_to_exp_mnt(
    SOC_SAND_IN int                      unit,
    SOC_SAND_IN uint8                      is_hr,
    SOC_SAND_IN uint32                       is_rev,
    SOC_SAND_IN uint32                       val,
    SOC_SAND_OUT uint32                      *exp,
    SOC_SAND_OUT uint32                      *mnt
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    clk_freq;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MTR_VAL_TO_EXP_MNT);

  SOC_SAND_CHECK_NULL_INPUT(exp);
  SOC_SAND_CHECK_NULL_INPUT(mnt);

  if (is_rev)
  {
    res = soc_pb_mgmt_core_frequency_get(
            unit,
            &clk_freq
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    if (is_hr)
    {
      soc_sand_break_complex_to_mnt_reverse_exp_round_down(
        val,
        SOC_PB_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT,
        SOC_PB_PP_MTR_VAL_TO_EXP_MNT_REVERSE_HR_MAX_EXP,
        clk_freq * SOC_PB_PP_MTR_VAL_EXP_MNT_EQ_CONST_MULTI_HR,
        SOC_PB_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC,
        mnt,
        exp
      );
    }
    else
    {
      soc_sand_break_complex_to_mnt_reverse_exp_round_down(
        val,
        SOC_PB_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT,
        SOC_PB_PP_MTR_VAL_TO_EXP_MNT_REVERSE_LR_MAX_EXP,
        clk_freq * SOC_PB_PP_MTR_VAL_EXP_MNT_EQ_CONST_MULTI_LR,
        SOC_PB_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC,
        mnt,
        exp
      );
    }
  }
  else
  {
     if (is_hr)
    {
      soc_sand_break_complex_to_mnt_exp_round_down(
        val,
        SOC_PB_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT,
        SOC_PB_PP_MTR_VAL_TO_EXP_MNT_HR_MAX_EXP,
        SOC_PB_PP_MTR_VAL_EXP_MNT_EQ_CONST_MULTI,
        SOC_PB_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC,
        mnt,
        exp
      );
    }
    else
    {
      soc_sand_break_complex_to_mnt_exp_round_down(
        val,
        SOC_PB_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT,
        SOC_PB_PP_MTR_VAL_TO_EXP_MNT_LR_MAX_EXP,
        SOC_PB_PP_MTR_VAL_EXP_MNT_EQ_CONST_MULTI,
        SOC_PB_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC,
        mnt,
        exp
      );
    }
  }
   
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mtr_val_to_exp_mnt()", 0, 0);
}

void
  soc_pb_pp_mtr_eth_policer_profile_key_get(
    SOC_SAND_IN  SOC_PB_IDR_ETHERNET_METER_PROFILES_TBL_DATA             profile_tbl_data,
    SOC_SAND_OUT uint32                                            *eth_mtr_profile_multiset_key
  )
{

  *eth_mtr_profile_multiset_key = 0;
  *eth_mtr_profile_multiset_key |= SOC_SAND_SET_BITS_RANGE(profile_tbl_data.rate_mantissa,5,0);
  *eth_mtr_profile_multiset_key |= SOC_SAND_SET_BITS_RANGE(profile_tbl_data.rate_exp,13,6);
  *eth_mtr_profile_multiset_key |= SOC_SAND_SET_BITS_RANGE(profile_tbl_data.burst_mantissa,19,14);
  *eth_mtr_profile_multiset_key |= SOC_SAND_SET_BITS_RANGE(profile_tbl_data.burst_exp,24,20);
  *eth_mtr_profile_multiset_key |= SOC_SAND_SET_BITS_RANGE(profile_tbl_data.packet_mode,25,25);

}

void
  soc_pb_pp_mtr_eth_policer_global_key_get(
    SOC_SAND_IN  SOC_PB_IDR_GLOBAL_METER_PROFILES_TBL_DATA               global_tbl_data,
    SOC_SAND_OUT uint32                                            *eth_mtr_global_multiset_key
  )
{

  *eth_mtr_global_multiset_key = 0;
  *eth_mtr_global_multiset_key |= SOC_SAND_SET_BITS_RANGE(global_tbl_data.rate_mantissa,5,0);
  *eth_mtr_global_multiset_key |= SOC_SAND_SET_BITS_RANGE(global_tbl_data.rate_exp,13,6);
  *eth_mtr_global_multiset_key |= SOC_SAND_SET_BITS_RANGE(global_tbl_data.burst_mantissa,19,14);
  *eth_mtr_global_multiset_key |= SOC_SAND_SET_BITS_RANGE(global_tbl_data.burst_exp,24,20);

}

uint32
  soc_pb_pp_metering_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    dp_cmnd_indx,
    iqm_dp_indx,
    mtr_res_indx,
    entrt_indx=0;
  SOC_PB_IQM_METER_PROCESSOR_RESULT_RESOLVE_TABLE_STATIC_TBL_DATA
    mtr_result_rsolve_tbl_data;
  uint32
    res = SOC_SAND_OK;
  uint32
    reg_val,
    fld_val,
    eth_mtr_profile_multiset_key,
    eth_mtr_profile_multiset_ndx = 0;
  uint8
    eth_mtr_profile_multiset_first_appear;
  SOC_SAND_SUCCESS_FAILURE
    eth_mtr_profile_multiset_success = SOC_SAND_SUCCESS;
  SOC_PETRA_REGS
    *regs;
  SOC_PB_IDR_ETHERNET_METER_PROFILES_TBL_DATA
    profile_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_METERING_INIT_UNSAFE);

  regs = soc_petra_regs();
  
  for (dp_cmnd_indx = 0; dp_cmnd_indx < SOC_PB_PP_NOF_MTR_RES_USES; ++dp_cmnd_indx)
  {
    for (iqm_dp_indx = 0; iqm_dp_indx <= SOC_PB_PP_DP_MAX_VAL; ++iqm_dp_indx)
    {
      for (mtr_res_indx = 0; mtr_res_indx <= SOC_PB_PP_DP_MAX_VAL; ++mtr_res_indx)
      {
		if (dp_cmnd_indx == SOC_PB_PP_MTR_RES_USE_NONE) /* meter command has no effect */
        {
	      /* ingress DP = egress DP = incoming DP */
          mtr_result_rsolve_tbl_data.iqm_dp = iqm_dp_indx;
		  mtr_result_rsolve_tbl_data.etm_de = iqm_dp_indx/2;
        }
		else if (dp_cmnd_indx == SOC_PB_PP_MTR_RES_USE_OW_DP) /* meter command affects ingress */
        {
          mtr_result_rsolve_tbl_data.iqm_dp = mtr_res_indx; /* ingress DP = meter resolved DP (Metering result overwrite the DP) */
		  mtr_result_rsolve_tbl_data.etm_de = iqm_dp_indx/2; /* egress DP = incoming DP */
        }
        else if (dp_cmnd_indx == SOC_PB_PP_MTR_RES_USE_OW_DE) /* meter command affects egress */
        {
	      mtr_result_rsolve_tbl_data.iqm_dp = iqm_dp_indx; /* ingress DP = incoming DP */
          mtr_result_rsolve_tbl_data.etm_de = mtr_res_indx/2; /* egress DP = meter resolved DP (Metering result overwrite the DP) */
        }
        else /* meter command affects both ingress and egress */
        {
		  /* ingress DP = egress DP = meter resolved DP */
		  mtr_result_rsolve_tbl_data.iqm_dp = mtr_res_indx;
          mtr_result_rsolve_tbl_data.etm_de = mtr_res_indx/2;
        }

        res = soc_pb_iqm_meter_processor_result_resolve_table_static_tbl_set_unsafe(
                unit,
                entrt_indx,
                &mtr_result_rsolve_tbl_data
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
        ++entrt_indx;
      }
    }
  }

	/* ReSet MeterProc - Init MeterProc */
  SOC_PB_FLD_SET(regs->iqm.iqm_init_reg.mrps_init_a , 0, 10, exit);
  SOC_PB_FLD_SET(regs->iqm.iqm_init_reg.mrps_init_b , 0, 20, exit);
  
  /* Enable MeterProc */
  SOC_PB_FLD_SET(regs->iqm.iqm_enablers_reg.meter_en , 1, 30, exit);

  /* Activates the leaky buckets refresh in MeterProc*/
  SOC_PB_FLD_SET(regs->iqm.mrps_config1_reg.mtrpa_refresh_ena , 1, 50, exit);
  SOC_PB_FLD_SET(regs->iqm.mrps_config1_reg.mtrpb_refresh_ena , 1, 60, exit);

  /* Enable leaky buckets Update in MeterProc*/
  fld_val = 1;
  SOC_PB_REG_GET(regs->iqm.mrps_config3_reg , reg_val, 70, exit);
  SOC_PB_FLD_TO_REG(regs->iqm.mrps_config3_reg.mtrpa_timer_en,fld_val,reg_val,80,exit);
  SOC_PB_FLD_TO_REG(regs->iqm.mrps_config3_reg.mtrpb_timer_en,fld_val,reg_val,90,exit);
  SOC_PB_REG_SET(regs->iqm.mrps_config3_reg , reg_val, 100, exit);

  /* Add multiset 0 to eth policer profile table */
  SOC_PETRA_CLEAR(&profile_tbl_data,SOC_PB_IDR_ETHERNET_METER_PROFILES_TBL_DATA,1);
  soc_pb_pp_mtr_eth_policer_profile_key_get(
    profile_tbl_data,
    &eth_mtr_profile_multiset_key
  );
  
  res = soc_pb_sw_db_multiset_add(
        unit,
        SOC_PB_PP_SW_DB_MULTI_SET_ETH_POLICER_ETH_METER_PROFILE,
        eth_mtr_profile_multiset_key,
        &eth_mtr_profile_multiset_ndx,
        &eth_mtr_profile_multiset_first_appear,
        &eth_mtr_profile_multiset_success
      );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
     
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_metering_init_unsafe()", 0, 0);
}

/*********************************************************************
*     Sets
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mtr_glbl_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MTR_GLBL_INFO                       *mtr_glbl_info
  )
{
  uint32
    res = SOC_SAND_OK,
    update_all_copies;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MTR_GLBL_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mtr_glbl_info);

  regs = soc_petra_regs();

  update_all_copies = SOC_SAND_BOOL2NUM(mtr_glbl_info->update_all_copies);

  SOC_PB_FLD_SET(regs->irr.static_configuration_reg.meter_update_all_copies , update_all_copies, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mtr_glbl_info_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_mtr_glbl_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MTR_GLBL_INFO                       *mtr_glbl_info
  )
{
  uint32
    res = SOC_SAND_OK;
  

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MTR_GLBL_INFO_SET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_MTR_GLBL_INFO, mtr_glbl_info, 10, exit);
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mtr_glbl_info_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_mtr_glbl_info_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MTR_GLBL_INFO_GET_VERIFY);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mtr_glbl_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Sets
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mtr_glbl_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PB_PP_MTR_GLBL_INFO                       *mtr_glbl_info
  )
{
  uint32
    res = SOC_SAND_OK,
    update_all_copies;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MTR_GLBL_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mtr_glbl_info);

  SOC_PB_PP_MTR_GLBL_INFO_clear(mtr_glbl_info);

  regs = soc_petra_regs();

  SOC_PB_FLD_GET(regs->irr.static_configuration_reg.meter_update_all_copies , update_all_copies, 10, exit);

  mtr_glbl_info->update_all_copies = SOC_SAND_NUM2BOOL(update_all_copies);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mtr_glbl_info_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Sets
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mtr_meters_group_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  SOC_PB_PP_MTR_GROUP_INFO                      *mtr_group_info
  )
{
  uint32
    res = SOC_SAND_OK,
    is_hr;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MTR_METERS_GROUP_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mtr_group_info);

  regs = soc_petra_regs();

  is_hr = SOC_SAND_BOOL2NUM(mtr_group_info->is_hr_enabled);

  if (mtr_group_ndx == 0)
  {
    SOC_PB_FLD_SET(regs->iqm.mrps_config1_reg.mtrpa_high_rate_ena , is_hr, 10, exit);
    SOC_PB_FLD_SET(regs->iqm.mrps_config1_reg.mtrpa_max_packet_size , mtr_group_info->max_packet_size, 20, exit);
  }
  else /* if (mtr_group_ndx == 1) */
  {
    SOC_PB_FLD_SET(regs->iqm.mrps_config1_reg.mtrpb_high_rate_ena , is_hr, 30, exit);
    SOC_PB_FLD_SET(regs->iqm.mrps_config1_reg.mtrpb_max_packet_size , mtr_group_info->max_packet_size, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mtr_meters_group_info_set_unsafe()", mtr_group_ndx, 0);
}

uint32
  soc_pb_pp_mtr_meters_group_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  SOC_PB_PP_MTR_GROUP_INFO                      *mtr_group_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MTR_METERS_GROUP_INFO_SET_VERIFY);
 
  SOC_SAND_ERR_IF_ABOVE_MAX(mtr_group_ndx, SOC_PB_PP_METERING_MTR_GROUP_NDX_MAX, SOC_PB_PP_METERING_MTR_GROUP_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_MTR_GROUP_INFO, mtr_group_info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mtr_meters_group_info_set_verify()", mtr_group_ndx, 0);
}

uint32
  soc_pb_pp_mtr_meters_group_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mtr_group_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MTR_METERS_GROUP_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(mtr_group_ndx, SOC_PB_PP_METERING_MTR_GROUP_NDX_MAX, SOC_PB_PP_METERING_MTR_GROUP_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mtr_meters_group_info_get_verify()", mtr_group_ndx, 0);
}

/*********************************************************************
*     Sets
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mtr_meters_group_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_OUT SOC_PB_PP_MTR_GROUP_INFO                      *mtr_group_info
  )
{
  uint32
    res = SOC_SAND_OK,
    is_hr;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MTR_METERS_GROUP_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mtr_group_info);

  SOC_PB_PP_MTR_GROUP_INFO_clear(mtr_group_info);
  
  regs = soc_petra_regs();

  if (mtr_group_ndx == 0)
  {
    SOC_PB_FLD_GET(regs->iqm.mrps_config1_reg.mtrpa_high_rate_ena , is_hr, 10, exit);
    SOC_PB_FLD_GET(regs->iqm.mrps_config1_reg.mtrpa_max_packet_size , mtr_group_info->max_packet_size, 20, exit);
  }
  else /* if (mtr_group_ndx == 1) */
  {
    SOC_PB_FLD_GET(regs->iqm.mrps_config1_reg.mtrpb_high_rate_ena , is_hr, 30, exit);
    SOC_PB_FLD_GET(regs->iqm.mrps_config1_reg.mtrpb_max_packet_size , mtr_group_info->max_packet_size, 40, exit);
  }

  mtr_group_info->is_hr_enabled = SOC_SAND_NUM2BOOL(is_hr);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mtr_meters_group_info_get_unsafe()", mtr_group_ndx, 0);
}

/*********************************************************************
*     Add Bandwidth Profile and set it attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mtr_bw_profile_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_MTR_BW_PROFILE_INFO                 *bw_profile_info,
    SOC_SAND_OUT SOC_PB_PP_MTR_BW_PROFILE_INFO                 *exact_bw_profile_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_IQM_PRFCFG_TBL_DATA
    tbl_data;
  uint8
    is_params_hr = FALSE,
    is_procc_hr = FALSE;
  SOC_PB_PP_MTR_GROUP_INFO
    mtr_group_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MTR_BW_PROFILE_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(bw_profile_info);
  SOC_SAND_CHECK_NULL_INPUT(exact_bw_profile_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  SOC_PETRA_CLEAR(&tbl_data,SOC_PB_IQM_PRFCFG_TBL_DATA,1);

  res = soc_pb_pp_mtr_chk_profile_rate(
         unit,
         bw_profile_info->cir,
         bw_profile_info->eir,
         bw_profile_info->cbs,
         bw_profile_info->ebs,
         &is_params_hr
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_pb_pp_mtr_meters_group_info_get_unsafe(
          unit,
          mtr_group_ndx,
          &mtr_group_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if ((mtr_group_info.is_hr_enabled == TRUE) && (bw_profile_ndx >= SOC_PB_PP_METERING_BW_PROFILE_NDX_HR_MIN))
  {
    is_procc_hr = TRUE;
  }

  if ((is_params_hr == TRUE) && (is_procc_hr == FALSE))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_METERING_BW_PROFILE_HR_RATE_ERR, 30, exit);
  }
    
  /* transfer user values (cir, eir, cbs, ebs) to mnt and exp*/
  res = soc_pb_pp_mtr_val_to_exp_mnt(
          unit,
          is_procc_hr,
          TRUE,
          bw_profile_info->cir,
          &tbl_data.cir_reverse_exponent,
          &tbl_data.cir_mantissa_64
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);

  res = soc_pb_pp_mtr_val_to_exp_mnt(
          unit,
          is_procc_hr,
          TRUE,
          bw_profile_info->eir,
          &tbl_data.eir_reverse_exponent,
          &tbl_data.eir_mantissa_64
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
  
  res = soc_pb_pp_mtr_val_to_exp_mnt(
          unit,
          is_procc_hr,
          FALSE,
          bw_profile_info->cbs,
          &tbl_data.cbs_exponent,
          &tbl_data.cbs_mantissa_64
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);

  res = soc_pb_pp_mtr_val_to_exp_mnt(
          unit,
          is_procc_hr,
          FALSE,
          bw_profile_info->ebs,
          &tbl_data.ebs_exponent,
          &tbl_data.ebs_mantissa_64
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);

  tbl_data.color_aware   = bw_profile_info->color_mode == SOC_PB_PP_MTR_COLOR_MODE_AWARE ? 0x1 : 0x0;
  tbl_data.coupling_flag = SOC_SAND_BOOL2NUM(bw_profile_info->is_coupling_enabled);
  tbl_data.fairness_mode = SOC_SAND_BOOL2NUM(bw_profile_info->is_fairness_enabled);
  tbl_data.reset_cir = SOC_SAND_BOOL2NUM(bw_profile_info->disable_cir);
  tbl_data.reset_eir = SOC_SAND_BOOL2NUM(bw_profile_info->disable_eir);

  /* write calculated values to HW */
  res = soc_pb_iqm_prfcfg_tbl_set_unsafe(
          unit,
          mtr_group_ndx,
          bw_profile_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  /* Read values for Bandwidth profile attributes as written to the device */
  res = soc_pb_pp_mtr_bw_profile_get_unsafe(
    unit,
    mtr_group_ndx,
    bw_profile_ndx,
    exact_bw_profile_info
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);
  *success = SOC_SAND_SUCCESS;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mtr_bw_profile_add_unsafe()", mtr_group_ndx, bw_profile_ndx);
}

uint32
  soc_pb_pp_mtr_bw_profile_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_ndx,
    SOC_SAND_IN  SOC_PB_PP_MTR_BW_PROFILE_INFO                 *bw_profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MTR_BW_PROFILE_ADD_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(mtr_group_ndx, SOC_PB_PP_METERING_MTR_GROUP_NDX_MAX, SOC_PB_PP_METERING_MTR_GROUP_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(bw_profile_ndx, SOC_PB_PP_METERING_BW_PROFILE_NDX_MAX, SOC_PB_PP_METERING_BW_PROFILE_NDX_OUT_OF_RANGE_ERR, 20, exit);
 
  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_MTR_BW_PROFILE_INFO, bw_profile_info, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mtr_bw_profile_add_verify()", mtr_group_ndx, bw_profile_ndx);
}

/*********************************************************************
*     Get Bandwidth Profile attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mtr_bw_profile_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_ndx,
    SOC_SAND_OUT SOC_PB_PP_MTR_BW_PROFILE_INFO                 *bw_profile_info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_IQM_PRFCFG_TBL_DATA
    tbl_data;
  uint8
    is_hr = FALSE;
  SOC_PB_PP_MTR_GROUP_INFO
    mtr_group_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MTR_BW_PROFILE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(bw_profile_info);

  SOC_PB_PP_MTR_BW_PROFILE_INFO_clear(bw_profile_info);

  /* Read calculated values to HW */
  res = soc_pb_iqm_prfcfg_tbl_get_unsafe(
          unit,
          mtr_group_ndx,
          bw_profile_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_pb_pp_mtr_meters_group_info_get_unsafe(
          unit,
          mtr_group_ndx,
          &mtr_group_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  if ((mtr_group_info.is_hr_enabled == TRUE) && (bw_profile_ndx > SOC_PB_PP_METERING_BW_PROFILE_NDX_HR_MIN))
  {
    is_hr = TRUE;
  }
    
  /* Transfer exp, mnt to values (according to High/Low rate) */
  res = soc_pb_pp_mtr_val_from_reverse_exp_mnt(
          unit,
          is_hr,
          tbl_data.cir_reverse_exponent,
          tbl_data.cir_mantissa_64,
          &bw_profile_info->cir
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_pb_pp_mtr_val_from_reverse_exp_mnt(
          unit,
          is_hr,
          tbl_data.eir_reverse_exponent,
          tbl_data.eir_mantissa_64,
          &bw_profile_info->eir
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_sand_compute_complex_to_mnt_exp(
          tbl_data.cbs_mantissa_64,
          tbl_data.cbs_exponent,
          1,
          SOC_PB_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC,
          &bw_profile_info->cbs
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_sand_compute_complex_to_mnt_exp(
          tbl_data.ebs_mantissa_64,
          tbl_data.ebs_exponent,
          1,
          SOC_PB_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC,
          &bw_profile_info->ebs
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  bw_profile_info->color_mode          = tbl_data.color_aware ? SOC_PB_PP_MTR_COLOR_MODE_AWARE : SOC_PB_PP_MTR_COLOR_MODE_BLIND;
  bw_profile_info->is_coupling_enabled = SOC_SAND_NUM2BOOL(tbl_data.coupling_flag);
  bw_profile_info->is_fairness_enabled = SOC_SAND_NUM2BOOL(tbl_data.fairness_mode);
  bw_profile_info->disable_cir = SOC_SAND_NUM2BOOL(tbl_data.reset_cir);
  bw_profile_info->disable_eir = SOC_SAND_NUM2BOOL(tbl_data.reset_eir);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mtr_bw_profile_get_unsafe()", mtr_group_ndx, bw_profile_ndx);
}

uint32
  soc_pb_pp_mtr_bw_profile_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MTR_BW_PROFILE_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(mtr_group_ndx, SOC_PB_PP_METERING_MTR_GROUP_NDX_MAX, SOC_PB_PP_METERING_MTR_GROUP_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(bw_profile_ndx, SOC_PB_PP_METERING_BW_PROFILE_NDX_MAX, SOC_PB_PP_METERING_BW_PROFILE_NDX_OUT_OF_RANGE_ERR, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mtr_bw_profile_get_verify()", mtr_group_ndx, bw_profile_ndx);
}

/*********************************************************************
*     Remove Bandwidth Profile
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mtr_bw_profile_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_ndx
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_IQM_PRFCFG_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MTR_BW_PROFILE_REMOVE_UNSAFE);

  SOC_PETRA_CLEAR(&tbl_data,SOC_PB_IQM_PRFCFG_TBL_DATA,1);

  res = soc_pb_iqm_prfcfg_tbl_set_unsafe(
          unit,
          mtr_group_ndx,
          bw_profile_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mtr_bw_profile_remove_unsafe()", mtr_group_ndx, bw_profile_ndx);
}

uint32
  soc_pb_pp_mtr_bw_profile_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MTR_BW_PROFILE_REMOVE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(mtr_group_ndx, SOC_PB_PP_METERING_MTR_GROUP_NDX_MAX, SOC_PB_PP_METERING_MTR_GROUP_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(bw_profile_ndx, SOC_PB_PP_METERING_BW_PROFILE_NDX_MAX, SOC_PB_PP_METERING_BW_PROFILE_NDX_OUT_OF_RANGE_ERR, 20, exit);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mtr_bw_profile_remove_verify()", mtr_group_ndx, bw_profile_ndx);
}

/*********************************************************************
*     Set meter attributes by mapping meter instance to
 *     bandwidth profile.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mtr_meter_ins_to_bw_profile_map_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MTR_METER_ID                        *meter_ins_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_id
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_IQM_PRFSEL_TBL_DATA
    tbl_data;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(meter_ins_ndx);

  tbl_data.profile = bw_profile_id;
 
  res = soc_pb_iqm_prfsel_tbl_set_unsafe(
          unit,
          meter_ins_ndx->group,
          meter_ins_ndx->id,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mtr_meter_ins_to_bw_profile_map_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_mtr_meter_ins_to_bw_profile_map_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MTR_METER_ID                        *meter_ins_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_SET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_MTR_METER_ID, meter_ins_ndx, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(bw_profile_id, SOC_PB_PP_METERING_BW_PROFILE_ID_MAX, SOC_PB_PP_METERING_BW_PROFILE_ID_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mtr_meter_ins_to_bw_profile_map_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_mtr_meter_ins_to_bw_profile_map_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MTR_METER_ID                        *meter_ins_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_GET_VERIFY);

  SOC_PB_PP_STRUCT_VERIFY(SOC_PB_PP_MTR_METER_ID, meter_ins_ndx, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mtr_meter_ins_to_bw_profile_map_get_verify()", 0, 0);
}

/*********************************************************************
*     Set meter attributes by mapping meter instance to
 *     bandwidth profile.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mtr_meter_ins_to_bw_profile_map_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_MTR_METER_ID                        *meter_ins_ndx,
    SOC_SAND_OUT uint32                                  *bw_profile_id
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_IQM_PRFSEL_TBL_DATA
    tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(meter_ins_ndx);
  SOC_SAND_CHECK_NULL_INPUT(bw_profile_id);
  
  res = soc_pb_iqm_prfsel_tbl_get_unsafe(
          unit,
          meter_ins_ndx->group,
          meter_ins_ndx->id,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  *bw_profile_id = tbl_data.profile;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mtr_meter_ins_to_bw_profile_map_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Enable / Disable Ethernet policing.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mtr_eth_policer_enable_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                 enable
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    idx;
  uint8
    config_status_bit,
    global_tbl_data_enable,
    status_tbl_data_enable;
  SOC_PB_IDR_ETHERNET_METER_STATUS_TBL_DATA
    status_tbl_data_start;
  SOC_PB_IDR_GLOBAL_METER_PROFILES_TBL_DATA
    global_tbl_data_start;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MTR_ETH_POLICER_ENABLE_SET_UNSAFE);

  SOC_PETRA_CLEAR(&status_tbl_data_start,SOC_PB_IDR_ETHERNET_METER_STATUS_TBL_DATA,1);
  SOC_PETRA_CLEAR(&global_tbl_data_start,SOC_PB_IDR_GLOBAL_METER_PROFILES_TBL_DATA,1);

  /* set eth policer global enable status bit */
  soc_pb_pp_sw_db_eth_policer_config_status_bit_set(
    unit,
    SOC_PB_PP_SW_DB_MULTI_SET_ETH_POLICER_ENABLE_STATUS_BIT_MAIN,
    enable
  );

  /* set profile config status bit */
  for (idx = 0 ; idx < SOC_PB_PP_SW_DB_MULTI_SET_ETH_POLICER_CONFIG_METER_PROFILE_NOF_MEMBER_BIT ; idx++)
  {
    res = soc_pb_idr_ethernet_meter_status_tbl_get_unsafe(
            unit,
            idx,
            &status_tbl_data_start
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    soc_pb_pp_sw_db_eth_policer_config_status_bit_get(
      unit,
      idx,
      &config_status_bit
    );

    if ((enable == TRUE) && (config_status_bit == TRUE))
    {
      status_tbl_data_enable = SOC_SAND_BOOL2NUM(TRUE);
    }
    else /* disable: enable == FALSE*/
    {
      status_tbl_data_enable = SOC_SAND_BOOL2NUM(FALSE);
    }

    if (status_tbl_data_start.enable != status_tbl_data_enable)
    {
      status_tbl_data_start.enable = status_tbl_data_enable;
      res = soc_pb_idr_ethernet_meter_status_tbl_set_unsafe(
              unit,
              idx,
              &status_tbl_data_start
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }
  }

  /* set global config status bit */
  for (idx = 0 ; idx < SOC_PB_PP_SW_DB_MULTI_SET_ETH_POLICER_GLOBAL_METER_PROFILE_NOF_MEMBER; idx++)
  {
    res = soc_pb_idr_global_meter_profiles_tbl_get_unsafe(
            unit,
            idx,
            &global_tbl_data_start
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    soc_pb_pp_sw_db_eth_policer_config_status_bit_get(
      unit,
      SOC_PB_PP_SW_DB_MULTI_SET_ETH_POLICER_CONFIG_METER_PROFILE_NOF_MEMBER_BIT + idx,
      &config_status_bit
    );

    if ((enable == TRUE) && (config_status_bit == TRUE))
    {
      global_tbl_data_enable = SOC_SAND_BOOL2NUM(TRUE);
    }
    else /* disable: enable == FALSE*/
    {
      global_tbl_data_enable = SOC_SAND_BOOL2NUM(FALSE);
    }

    if (global_tbl_data_start.enable != global_tbl_data_enable)
    {
      global_tbl_data_start.enable = global_tbl_data_enable;
      res = soc_pb_idr_global_meter_profiles_tbl_set_unsafe(
              unit,
              idx,
              &global_tbl_data_start
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }
  }
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mtr_eth_policer_enable_set_unsafe()", 0, 0);
}

uint32
  soc_pb_pp_mtr_eth_policer_enable_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                 enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MTR_ETH_POLICER_ENABLE_SET_VERIFY);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mtr_eth_policer_enable_set_verify()", 0, 0);
}

uint32
  soc_pb_pp_mtr_eth_policer_enable_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MTR_ETH_POLICER_ENABLE_GET_VERIFY);

  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mtr_eth_policer_enable_get_verify()", 0, 0);
}

/*********************************************************************
*     Enable / Disable Ethernet policing.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mtr_eth_policer_enable_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT uint8                                 *enable
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MTR_ETH_POLICER_ENABLE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(enable);

  /* set eth policer global enable status bit */
  soc_pb_pp_sw_db_eth_policer_config_status_bit_get(
    unit,
    SOC_PB_PP_SW_DB_MULTI_SET_ETH_POLICER_ENABLE_STATUS_BIT_MAIN,
    enable
  );
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mtr_eth_policer_enable_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Set policer attributes of the Ethernet policer. Enable
 *     policing per ingress port and Ethernet type.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mtr_eth_policer_params_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                port_ndx,
    SOC_SAND_IN  SOC_PB_PP_MTR_ETH_TYPE                        eth_type_ndx,
    SOC_SAND_IN  SOC_PB_PP_MTR_BW_PROFILE_INFO                 *policer_info
  )
{
  uint32
    res = SOC_SAND_OK,
    clk_freq,
    eth_mtr_profile_multiset_key = 0x0,
    eth_mtr_profile_multiset_ndx = 0x0,
    config_tbl_offset,
    status_tbl_offset,
    eq_const_multi;
  uint8
    eth_mtr_profile_multiset_first_appear = 0x0,
    eth_mtr_profile_multiset_last_appear = 0x0,
    sw_db_enable_bit,
    main_enable,
    info_enable,
    enable,
    is_b0_fix;
  SOC_SAND_SUCCESS_FAILURE
    eth_mtr_profile_multiset_success = SOC_SAND_SUCCESS;
  SOC_PB_IDR_ETHERNET_METER_STATUS_TBL_DATA
    status_tbl_data;
  SOC_PB_IDR_ETHERNET_METER_CONFIG_TBL_DATA
    config_tbl_data_start,
    config_tbl_data;
  SOC_PB_IDR_ETHERNET_METER_PROFILES_TBL_DATA
    profile_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MTR_ETH_POLICER_PARAMS_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(policer_info);

  SOC_PETRA_CLEAR(&status_tbl_data,SOC_PB_IDR_ETHERNET_METER_STATUS_TBL_DATA,1);
  SOC_PETRA_CLEAR(&config_tbl_data,SOC_PB_IDR_ETHERNET_METER_CONFIG_TBL_DATA,1);
  SOC_PETRA_CLEAR(&config_tbl_data_start,SOC_PB_IDR_ETHERNET_METER_CONFIG_TBL_DATA,1);
  SOC_PETRA_CLEAR(&profile_tbl_data,SOC_PB_IDR_ETHERNET_METER_PROFILES_TBL_DATA,1);
 
  res = soc_pb_mgmt_core_frequency_get(
          unit,
          &clk_freq
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  status_tbl_offset = config_tbl_offset = port_ndx * SOC_PB_PP_NOF_MTR_ETH_TYPES + eth_type_ndx;

  info_enable = SOC_SAND_BOOL2NUM_INVERSE(policer_info->disable_cir);

  /* Get pointers from config table*/
  res = soc_pb_idr_ethernet_meter_config_tbl_get_unsafe(
          unit,
          config_tbl_offset,
          &config_tbl_data_start
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  soc_pb_pp_sw_db_eth_policer_config_status_bit_get(
      unit,
      status_tbl_offset,
      &sw_db_enable_bit
    );

  /* get eth policer main enable status bit */
  soc_pb_pp_sw_db_eth_policer_config_status_bit_get(
    unit,
    SOC_PB_PP_SW_DB_MULTI_SET_ETH_POLICER_ENABLE_STATUS_BIT_MAIN,
    &main_enable
  );

  if (sw_db_enable_bit == TRUE)
  {
    /* profile is enable remove from multiset */   
    res = soc_pb_sw_db_multiset_remove_by_index(
            unit,
            SOC_PB_PP_SW_DB_MULTI_SET_ETH_POLICER_ETH_METER_PROFILE,
            config_tbl_data_start.profile,
            &eth_mtr_profile_multiset_last_appear
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

  /* cir is enable fill profile table */
  if (info_enable == TRUE)
  {
    res = soc_pb_pp_mtr_is_b0_eq_const_multi_eth_policer_rate(
            unit,
            &eq_const_multi
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    
    /* transfer user values (cir, cbs) to man and exp*/
    res = soc_sand_break_complex_to_mnt_exp_round_down(
            policer_info->cir,
            SOC_PB_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT,
            SOC_PB_PP_MTR_VAL_TO_EXP_MNT_ETH_POLICER_RATE_MAX_EXP,
            eq_const_multi * clk_freq,
            SOC_PB_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC,
            &profile_tbl_data.rate_mantissa,
            &profile_tbl_data.rate_exp
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    res = soc_pb_pp_mtr_is_b0_fix(
            unit,
            &is_b0_fix
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 75, exit);
    
    if (is_b0_fix)
    {
      profile_tbl_data.rate_mantissa = 0;
    }

   res = soc_sand_break_complex_to_mnt_exp_round_down(
            policer_info->cbs,
            SOC_PB_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT,
            SOC_PB_PP_MTR_VAL_TO_EXP_MNT_ETH_POLICER_BURST_MAX_EXP,
            SOC_PB_PP_MTR_VAL_EXP_MNT_EQ_CONST_MULTI_ETH_POLICER_BURST,
            SOC_PB_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC,
            &profile_tbl_data.burst_mantissa,
            &profile_tbl_data.burst_exp
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

    profile_tbl_data.packet_mode = SOC_SAND_BOOL2NUM(policer_info->is_packet_mode);

    soc_pb_pp_mtr_eth_policer_profile_key_get(
      profile_tbl_data,
      &eth_mtr_profile_multiset_key
    );
    
    res = soc_pb_sw_db_multiset_add(
          unit,
          SOC_PB_PP_SW_DB_MULTI_SET_ETH_POLICER_ETH_METER_PROFILE,
          eth_mtr_profile_multiset_key,
          &eth_mtr_profile_multiset_ndx,
          &eth_mtr_profile_multiset_first_appear,
          &eth_mtr_profile_multiset_success
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

    if (eth_mtr_profile_multiset_success != SOC_SAND_SUCCESS)
    { 
      /* Add old profile to multi set */
      res = soc_pb_idr_ethernet_meter_profiles_tbl_get_unsafe(
              unit,
              config_tbl_data_start.profile,
              &profile_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);

      soc_pb_pp_mtr_eth_policer_profile_key_get(
        profile_tbl_data,
        &eth_mtr_profile_multiset_key
      );

      res = soc_pb_sw_db_multiset_add(
              unit,
              SOC_PB_PP_SW_DB_MULTI_SET_ETH_POLICER_ETH_METER_PROFILE,
              eth_mtr_profile_multiset_key,
              &eth_mtr_profile_multiset_ndx,
              &eth_mtr_profile_multiset_first_appear,
              &eth_mtr_profile_multiset_success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

      SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_MTR_ETH_POLICER_ADD_FAIL_ERR, 100, exit);
    }
  }

  /* Set sw_db enable bit */
  soc_pb_pp_sw_db_eth_policer_config_status_bit_set(
    unit,
    status_tbl_offset,
    info_enable
  );

  /* Set profile table - if first */
  if (eth_mtr_profile_multiset_first_appear == 0x1)
  {
    res = soc_pb_idr_ethernet_meter_profiles_tbl_set_unsafe(
            unit,
            eth_mtr_profile_multiset_ndx,
            &profile_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);
  }

  /* Set pointer in config table*/
  /* Read old table entry - for global ptr */
  res = soc_pb_idr_ethernet_meter_config_tbl_get_unsafe(
          unit,
          config_tbl_offset,
          &config_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);

  config_tbl_data.profile = eth_mtr_profile_multiset_ndx;

  res = soc_pb_idr_ethernet_meter_config_tbl_set_unsafe(
          unit,
          config_tbl_offset,
          &config_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);

  /* set status table */
  res = soc_pb_idr_ethernet_meter_status_tbl_get_unsafe(
          unit,
          status_tbl_offset,
          &status_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if ((main_enable == TRUE) && (info_enable))
  {
    enable = TRUE;
  }
  else
  {
    enable = FALSE;
  }

  /* Write only if there is a change in state */
  if (status_tbl_data.enable != enable)
  {
    SOC_PETRA_CLEAR(&status_tbl_data,SOC_PB_IDR_ETHERNET_METER_STATUS_TBL_DATA,1);

    status_tbl_data.enable = enable;

    res = soc_pb_idr_ethernet_meter_status_tbl_set_unsafe(
            unit,
            status_tbl_offset,
            &status_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 190, exit);
  }

  /* Set profile table - if last*/
  if ((eth_mtr_profile_multiset_last_appear == 0x1) && (eth_mtr_profile_multiset_ndx != config_tbl_data_start.profile))
  {
    SOC_PETRA_CLEAR(&profile_tbl_data,SOC_PB_IDR_ETHERNET_METER_PROFILES_TBL_DATA,1);

    res = soc_pb_idr_ethernet_meter_profiles_tbl_set_unsafe(
            unit,
            config_tbl_data_start.profile,
            &profile_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mtr_eth_policer_params_set_unsafe()", port_ndx, 0);
}

uint32
  soc_pb_pp_mtr_eth_policer_params_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                port_ndx,
    SOC_SAND_IN  SOC_PB_PP_MTR_ETH_TYPE                        eth_type_ndx,
    SOC_SAND_IN  SOC_PB_PP_MTR_BW_PROFILE_INFO                 *policer_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MTR_ETH_POLICER_PARAMS_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_ndx, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(eth_type_ndx, SOC_PB_PP_METERING_ETH_TYPE_NDX_MAX, SOC_PB_PP_METERING_ETH_TYPE_NDX_OUT_OF_RANGE_ERR, 20, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(policer_info->cir, SOC_PB_PP_BW_PROFILE_ADD_VERIFY_HR_IR_MAX, SOC_PB_PP_METERING_CIR_OUT_OF_RANGE_ERR, 30, exit);
  SOC_SAND_ERR_IF_BELOW_MIN(policer_info->cir, SOC_PB_PP_BW_PROFILE_ADD_VERIFY_LR_IR_MIN, SOC_PB_PP_METERING_CIR_OUT_OF_RANGE_ERR, 40, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(policer_info->cbs, SOC_PB_PP_BW_PROFILE_ADD_VERIFY_HR_BS_MAX, SOC_PB_PP_METERING_CBS_OUT_OF_RANGE_ERR, 50, exit);
  SOC_SAND_ERR_IF_BELOW_MIN(policer_info->cbs, SOC_PB_PP_BW_PROFILE_ADD_VERIFY_LR_BS_MIN, SOC_PB_PP_METERING_CBS_OUT_OF_RANGE_ERR, 60, exit);

  if ((policer_info->color_mode!= FALSE) ||
      (policer_info->is_coupling_enabled != FALSE) ||
      (policer_info->is_fairness_enabled != FALSE) ||
      (policer_info->is_mtr_enabled != FALSE))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PP_MTR_ETH_POLICER_PARAMS_SET_VERIFY_OUT_OF_RANGE_ERR, 70, exit);
  }

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mtr_eth_policer_params_set_verify()", port_ndx, 0);
}

uint32
  soc_pb_pp_mtr_eth_policer_params_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                port_ndx,
    SOC_SAND_IN  SOC_PB_PP_MTR_ETH_TYPE                        eth_type_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MTR_ETH_POLICER_PARAMS_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_ndx, SOC_PB_PP_PORT_MAX, SOC_PB_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(eth_type_ndx, SOC_PB_PP_METERING_ETH_TYPE_NDX_MAX, SOC_PB_PP_METERING_ETH_TYPE_NDX_OUT_OF_RANGE_ERR, 20, exit);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mtr_eth_policer_params_get_verify()", port_ndx, 0);
}

/*********************************************************************
*     Set policer attributes of the Ethernet policer. Enable
 *     policing per ingress port and Ethernet type.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_mtr_eth_policer_params_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                                port_ndx,
    SOC_SAND_IN  SOC_PB_PP_MTR_ETH_TYPE                        eth_type_ndx,
    SOC_SAND_OUT SOC_PB_PP_MTR_BW_PROFILE_INFO                 *policer_info
  )
{
  uint32
    res = SOC_SAND_OK,
    clk_freq,
    config_tbl_offset,
    status_tbl_offset,
    eq_const_multi;
  SOC_PB_IDR_ETHERNET_METER_STATUS_TBL_DATA
    status_tbl_data;
  SOC_PB_IDR_ETHERNET_METER_CONFIG_TBL_DATA
    config_tbl_data;
  SOC_PB_IDR_ETHERNET_METER_PROFILES_TBL_DATA
    profile_tbl_data;
  uint8
    sw_db_enable_bit;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MTR_ETH_POLICER_PARAMS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(policer_info);

  SOC_PB_PP_MTR_BW_PROFILE_INFO_clear(policer_info);

  SOC_PETRA_CLEAR(&config_tbl_data,SOC_PB_IDR_ETHERNET_METER_CONFIG_TBL_DATA,1);
  SOC_PETRA_CLEAR(&profile_tbl_data,SOC_PB_IDR_ETHERNET_METER_PROFILES_TBL_DATA,1);

  res = soc_pb_mgmt_core_frequency_get(
          unit,
          &clk_freq
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  status_tbl_offset = config_tbl_offset = port_ndx * SOC_PB_PP_NOF_MTR_ETH_TYPES + eth_type_ndx;

  /* Get pointers from config table*/
  res = soc_pb_idr_ethernet_meter_config_tbl_get_unsafe(
          unit,
          config_tbl_offset,
          &config_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /* Get enable bit from status table */
  res = soc_pb_idr_ethernet_meter_status_tbl_get_unsafe(
          unit,
          status_tbl_offset,
          &status_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  soc_pb_pp_sw_db_eth_policer_config_status_bit_get(
    unit,
    status_tbl_offset,
    &sw_db_enable_bit
  );

  policer_info->disable_cir = SOC_SAND_NUM2BOOL_INVERSE(sw_db_enable_bit);

  /* Get committed mnt, exp values from profile table */
  res = soc_pb_idr_ethernet_meter_profiles_tbl_get_unsafe(
          unit,
          config_tbl_data.profile,
          &profile_tbl_data
         );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  policer_info->is_packet_mode = SOC_SAND_NUM2BOOL(profile_tbl_data.packet_mode);

  res = soc_pb_pp_mtr_is_b0_eq_const_multi_eth_policer_rate(
          unit,
          &eq_const_multi
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  /* Calc rates from man, exp */
  res = soc_sand_compute_complex_to_mnt_exp(
          profile_tbl_data.rate_mantissa,
          profile_tbl_data.rate_exp,
          eq_const_multi * clk_freq,
          SOC_PB_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC,
          &policer_info->cir
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
 
  res = soc_sand_compute_complex_to_mnt_exp(
          profile_tbl_data.burst_mantissa,
          profile_tbl_data.burst_exp,
          SOC_PB_PP_MTR_VAL_EXP_MNT_EQ_CONST_MULTI_ETH_POLICER_BURST,
          SOC_PB_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC,
          &policer_info->cbs
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mtr_eth_policer_params_get_unsafe()", port_ndx, 0);
}

/*********************************************************************
*     Set Ethernet policer Global Profile attributes.         
 *     Details: in the H file. (search for prototype)          
*********************************************************************/
uint32  
  soc_pb_pp_mtr_eth_policer_glbl_profile_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  glbl_profile_idx,
    SOC_SAND_IN  SOC_PB_PP_MTR_BW_PROFILE_INFO *policer_info
  )
{
  uint32
    clk_freq,
    eq_const_multi,
    res = SOC_SAND_OK;
  SOC_PB_IDR_GLOBAL_METER_PROFILES_TBL_DATA
    global_tbl_data;
  uint8
    main_enable,
    glbl_info_enable,
    is_b0_fix;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MTR_ETH_POLICER_GLBL_PROFILE_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(policer_info);

  SOC_PETRA_CLEAR(&global_tbl_data,SOC_PB_IDR_GLOBAL_METER_PROFILES_TBL_DATA,1);

  res = soc_pb_mgmt_core_frequency_get(
          unit,
          &clk_freq
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_pb_pp_mtr_is_b0_eq_const_multi_eth_policer_rate(
          unit,
          &eq_const_multi
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  /* transfer user values (eir, ebs) to man and exp*/
  res = soc_sand_break_complex_to_mnt_exp_round_down(
    policer_info->cir,
    SOC_PB_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT,
    SOC_PB_PP_MTR_VAL_TO_EXP_MNT_ETH_POLICER_RATE_MAX_EXP,
    eq_const_multi * clk_freq,
    SOC_PB_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC,
    &global_tbl_data.rate_mantissa,
    &global_tbl_data.rate_exp

    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_pb_pp_mtr_is_b0_fix(
          unit,
          &is_b0_fix
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

  if (is_b0_fix)
  {
    global_tbl_data.rate_mantissa = 0;
  }

  res = soc_sand_break_complex_to_mnt_exp_round_down(
    policer_info->cbs,
    SOC_PB_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT,
    SOC_PB_PP_MTR_VAL_TO_EXP_MNT_ETH_POLICER_BURST_MAX_EXP,
    SOC_PB_PP_MTR_VAL_EXP_MNT_EQ_CONST_MULTI_ETH_POLICER_BURST,
    SOC_PB_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC,
    &global_tbl_data.burst_mantissa,
    &global_tbl_data.burst_exp
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  glbl_info_enable = SOC_SAND_BOOL2NUM_INVERSE(policer_info->disable_cir);
  
  /* get eth policer main enable status bit */
  soc_pb_pp_sw_db_eth_policer_config_status_bit_get(
    unit,
    SOC_PB_PP_SW_DB_MULTI_SET_ETH_POLICER_ENABLE_STATUS_BIT_MAIN,
    &main_enable
  );

  if ((main_enable == TRUE) && (glbl_info_enable == TRUE) )
  {
    global_tbl_data.enable = TRUE;
  }
  else 
  {
    global_tbl_data.enable = FALSE;
  }

  /* Start writes */
  /* write to sw_db the enable bit*/
  soc_pb_pp_sw_db_eth_policer_config_status_bit_set(
      unit,
      SOC_PB_PP_SW_DB_MULTI_SET_ETH_POLICER_CONFIG_METER_PROFILE_NOF_MEMBER_BIT + glbl_profile_idx,
      glbl_info_enable
  );

  global_tbl_data.packet_mode = SOC_SAND_BOOL2NUM(policer_info->is_packet_mode);

  res = soc_pb_idr_global_meter_profiles_tbl_set_unsafe(
          unit,
          glbl_profile_idx,
          &global_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mtr_eth_policer_glbl_profile_set_unsafe()", 0, 0);
}

uint32  
  soc_pb_pp_mtr_eth_policer_glbl_profile_set_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  glbl_profile_idx,
    SOC_SAND_IN  SOC_PB_PP_MTR_BW_PROFILE_INFO *policer_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MTR_ETH_POLICER_GLBL_PROFILE_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(glbl_profile_idx, SOC_PB_PP_METERING_GLBL_PROFILE_IDX_MAX, SOC_PB_PP_METERING_GLBL_PROFILE_IDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_BELOW_MIN(glbl_profile_idx, SOC_PB_PP_METERING_GLBL_PROFILE_IDX_MIN, SOC_PB_PP_METERING_GLBL_PROFILE_IDX_OUT_OF_RANGE_ERR, 20, exit);
  
  SOC_SAND_ERR_IF_ABOVE_MAX(policer_info->cir, SOC_PB_PP_BW_PROFILE_ADD_VERIFY_HR_IR_MAX, SOC_PB_PP_METERING_CIR_OUT_OF_RANGE_ERR, 30, exit);
  SOC_SAND_ERR_IF_BELOW_MIN(policer_info->cir, SOC_PB_PP_BW_PROFILE_ADD_VERIFY_LR_IR_MIN, SOC_PB_PP_METERING_CIR_OUT_OF_RANGE_ERR, 40, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(policer_info->cbs, SOC_PB_PP_BW_PROFILE_ADD_VERIFY_HR_BS_MAX, SOC_PB_PP_METERING_CBS_OUT_OF_RANGE_ERR, 50, exit);
  SOC_SAND_ERR_IF_BELOW_MIN(policer_info->cbs, SOC_PB_PP_BW_PROFILE_ADD_VERIFY_LR_BS_MIN, SOC_PB_PP_METERING_CBS_OUT_OF_RANGE_ERR, 60, exit);
  
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mtr_eth_policer_glbl_profile_set_verify()", 0, 0);
}

uint32  
  soc_pb_pp_mtr_eth_policer_glbl_profile_get_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  glbl_profile_idx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MTR_ETH_POLICER_GLBL_PROFILE_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(glbl_profile_idx, SOC_PB_PP_METERING_GLBL_PROFILE_IDX_MAX, SOC_PB_PP_METERING_GLBL_PROFILE_IDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_BELOW_MIN(glbl_profile_idx, SOC_PB_PP_METERING_GLBL_PROFILE_IDX_MIN, SOC_PB_PP_METERING_GLBL_PROFILE_IDX_OUT_OF_RANGE_ERR, 20, exit);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mtr_eth_policer_glbl_profile_get_verify()", 0, 0);
}

/*********************************************************************
*     Set Ethernet policer Global Profile attributes.         
 *     Details: in the H file. (search for prototype)          
*********************************************************************/
uint32  
  soc_pb_pp_mtr_eth_policer_glbl_profile_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  glbl_profile_idx,
    SOC_SAND_OUT SOC_PB_PP_MTR_BW_PROFILE_INFO *policer_info
  )
{
  uint32
    res = SOC_SAND_OK,
    eq_const_multi,
    clk_freq;
  SOC_PB_IDR_GLOBAL_METER_PROFILES_TBL_DATA
    global_tbl_data;
  uint8
    glbl_info_enable;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MTR_ETH_POLICER_GLBL_PROFILE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(policer_info);

  SOC_PB_PP_MTR_BW_PROFILE_INFO_clear(policer_info);

  SOC_PETRA_CLEAR(&global_tbl_data,SOC_PB_IDR_GLOBAL_METER_PROFILES_TBL_DATA,1);

  res = soc_pb_mgmt_core_frequency_get(
          unit,
          &clk_freq
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* Get Excess mnt, exp values from global table */
  res = soc_pb_idr_global_meter_profiles_tbl_get_unsafe(
          unit,
          glbl_profile_idx,
          &global_tbl_data
         );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_pb_pp_mtr_is_b0_eq_const_multi_eth_policer_rate(
          unit,
          &eq_const_multi
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

  /* Calc rates from mnt, exp */
   res = soc_sand_compute_complex_to_mnt_exp(
          global_tbl_data.rate_mantissa,
          global_tbl_data.rate_exp,
          eq_const_multi * clk_freq,
          SOC_PB_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC,
          &policer_info->cir
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

   res = soc_sand_compute_complex_to_mnt_exp(
          global_tbl_data.burst_mantissa,
          global_tbl_data.burst_exp,
          SOC_PB_PP_MTR_VAL_EXP_MNT_EQ_CONST_MULTI_ETH_POLICER_BURST,
          SOC_PB_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC,
          &policer_info->cbs
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

   soc_pb_pp_sw_db_eth_policer_config_status_bit_get(
      unit,
      SOC_PB_PP_SW_DB_MULTI_SET_ETH_POLICER_CONFIG_METER_PROFILE_NOF_MEMBER_BIT + glbl_profile_idx,
      &glbl_info_enable
  );

  policer_info->disable_cir = SOC_SAND_NUM2BOOL_INVERSE(glbl_info_enable);
  policer_info->is_packet_mode = SOC_SAND_NUM2BOOL(global_tbl_data.packet_mode);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mtr_eth_policer_glbl_profile_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Map Ethernet policer per ingress port and Ethernet type 
 *     to Ethernet policer Global Profile.                     
 *     Details: in the H file. (search for prototype)          
*********************************************************************/
uint32  
  soc_pb_pp_mtr_eth_policer_glbl_profile_map_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                port_ndx,
    SOC_SAND_IN  SOC_PB_PP_MTR_ETH_TYPE        eth_type_ndx,
    SOC_SAND_IN  uint32                  glbl_profile_idx
  )
{
  uint32
    config_tbl_offset,
    res = SOC_SAND_OK;
  SOC_PB_IDR_ETHERNET_METER_CONFIG_TBL_DATA
    config_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET_UNSAFE);

  SOC_PETRA_CLEAR(&config_tbl_data,SOC_PB_IDR_ETHERNET_METER_CONFIG_TBL_DATA,1);

  config_tbl_offset = port_ndx * SOC_PB_PP_NOF_MTR_ETH_TYPES + eth_type_ndx;

  /* Get pointers from config table*/
  res = soc_pb_idr_ethernet_meter_config_tbl_get_unsafe(
          unit,
          config_tbl_offset,
          &config_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  config_tbl_data.global_meter_ptr = glbl_profile_idx;

  res = soc_pb_idr_ethernet_meter_config_tbl_set_unsafe(
        unit,
        config_tbl_offset,
        &config_tbl_data
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mtr_eth_policer_glbl_profile_map_set_unsafe()", 0, 0);
}

uint32  
  soc_pb_pp_mtr_eth_policer_glbl_profile_map_set_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                port_ndx,
    SOC_SAND_IN  SOC_PB_PP_MTR_ETH_TYPE        eth_type_ndx,
    SOC_SAND_IN  uint32                  glbl_profile_idx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(eth_type_ndx, SOC_PB_PP_METERING_ETH_TYPE_NDX_MAX, SOC_PB_PP_METERING_ETH_TYPE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(glbl_profile_idx, SOC_PB_PP_METERING_GLBL_PROFILE_IDX_MAX, SOC_PB_PP_METERING_GLBL_PROFILE_IDX_OUT_OF_RANGE_ERR, 20, exit);
  
  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mtr_eth_policer_glbl_profile_map_set_verify()", 0, 0);
}

uint32  
  soc_pb_pp_mtr_eth_policer_glbl_profile_map_get_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                port_ndx,
    SOC_SAND_IN  SOC_PB_PP_MTR_ETH_TYPE        eth_type_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(eth_type_ndx, SOC_PB_PP_METERING_ETH_TYPE_NDX_MAX, SOC_PB_PP_METERING_ETH_TYPE_NDX_OUT_OF_RANGE_ERR, 10, exit);

  SOC_PB_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mtr_eth_policer_glbl_profile_map_get_verify()", 0, 0);
}

/*********************************************************************
*     Map Ethernet policer per ingress port and Ethernet type 
 *     to Ethernet policer Global Profile.                     
 *     Details: in the H file. (search for prototype)          
*********************************************************************/
uint32  
  soc_pb_pp_mtr_eth_policer_glbl_profile_map_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PB_PP_PORT                port_ndx,
    SOC_SAND_IN  SOC_PB_PP_MTR_ETH_TYPE        eth_type_ndx,
    SOC_SAND_OUT uint32                  *glbl_profile_idx
  )
{
  uint32
    config_tbl_offset,
    res = SOC_SAND_OK;
  SOC_PB_IDR_ETHERNET_METER_CONFIG_TBL_DATA
    config_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(glbl_profile_idx);

  SOC_PETRA_CLEAR(&config_tbl_data,SOC_PB_IDR_ETHERNET_METER_CONFIG_TBL_DATA,1);

  config_tbl_offset = port_ndx * SOC_PB_PP_NOF_MTR_ETH_TYPES + eth_type_ndx;

  /* Get pointers from config table*/
  res = soc_pb_idr_ethernet_meter_config_tbl_get_unsafe(
          unit,
          config_tbl_offset,
          &config_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
  *glbl_profile_idx = config_tbl_data.global_meter_ptr;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_pp_mtr_eth_policer_glbl_profile_map_get_unsafe()", 0, 0);
}


/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_pb_pp_api_metering module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_pp_metering_get_procs_ptr(void)
{
  return Soc_pb_pp_procedure_desc_element_metering;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     soc_pb_pp_api_metering module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_pp_metering_get_errs_ptr(void)
{
  return Soc_pb_pp_error_desc_element_metering;
}
uint32
  SOC_PB_PP_MTR_METER_ID_verify(
    SOC_SAND_IN  SOC_PB_PP_MTR_METER_ID *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->group, SOC_PB_PP_METERING_GROUP_MAX, SOC_PB_PP_METERING_GROUP_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->id, SOC_PB_PP_METERING_ID_MAX, SOC_PB_PP_METERING_ID_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_MTR_METER_ID_verify()",0,0);
}

uint32
  SOC_PB_PP_MTR_GLBL_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_MTR_GLBL_INFO *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->update_all_copies, SOC_PB_PP_METERING_UPDATE_ALL_COPIES_MAX, SOC_PB_PP_METERING_UPDATE_ALL_COPIES_MAX_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_MTR_GLBL_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_MTR_GROUP_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_MTR_GROUP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->max_packet_size, SOC_PB_PP_METERING_MAX_PACKET_SIZE_MAX, SOC_PB_PP_METERING_MAX_PACKET_SIZE_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_MTR_GROUP_INFO_verify()",0,0);
}

uint32
  SOC_PB_PP_MTR_BW_PROFILE_INFO_verify(
    SOC_SAND_IN  SOC_PB_PP_MTR_BW_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->cir, SOC_PB_PP_BW_PROFILE_ADD_VERIFY_HR_IR_MAX, SOC_PB_PP_METERING_CIR_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_BELOW_MIN(info->cir, SOC_PB_PP_BW_PROFILE_ADD_VERIFY_LR_IR_MIN, SOC_PB_PP_METERING_CIR_OUT_OF_RANGE_ERR, 20, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->eir, SOC_PB_PP_BW_PROFILE_ADD_VERIFY_HR_IR_MAX, SOC_PB_PP_METERING_EIR_OUT_OF_RANGE_ERR, 30, exit);
  SOC_SAND_ERR_IF_BELOW_MIN(info->eir, SOC_PB_PP_BW_PROFILE_ADD_VERIFY_LR_IR_MIN, SOC_PB_PP_METERING_EIR_OUT_OF_RANGE_ERR, 40, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->cbs, SOC_PB_PP_BW_PROFILE_ADD_VERIFY_HR_BS_MAX, SOC_PB_PP_METERING_CBS_OUT_OF_RANGE_ERR, 50, exit);
  SOC_SAND_ERR_IF_BELOW_MIN(info->cbs, SOC_PB_PP_BW_PROFILE_ADD_VERIFY_LR_BS_MIN, SOC_PB_PP_METERING_CBS_OUT_OF_RANGE_ERR, 60, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->ebs, SOC_PB_PP_BW_PROFILE_ADD_VERIFY_HR_BS_MAX, SOC_PB_PP_METERING_EBS_OUT_OF_RANGE_ERR, 70, exit);
  SOC_SAND_ERR_IF_BELOW_MIN(info->ebs, SOC_PB_PP_BW_PROFILE_ADD_VERIFY_LR_BS_MIN, SOC_PB_PP_METERING_EBS_OUT_OF_RANGE_ERR, 80, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->color_mode, SOC_PB_PP_METERING_COLOR_MODE_MAX, SOC_PB_PP_METERING_COLOR_MODE_OUT_OF_RANGE_ERR, 90, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_PP_MTR_BW_PROFILE_INFO_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

