#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* $Id: arad_pp_metering.c,v 1.50 Broadcom SDK $
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

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_METERING
#include <soc/mem.h>


/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/bsl.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>


#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_metering.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_general.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_sw_db.h>

#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/SAND/Utils/sand_multi_set.h>
#include <soc/dpp/SAND/Utils/sand_u64.h>
#include <soc/dpp/drv.h>

#include <soc/dpp/ARAD/arad_ingress_traffic_mgmt.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define ARAD_PP_METERING_MTR_GROUP_NDX_MAX                       (1)
#define ARAD_PP_METERING_BW_PROFILE_NDX_MAX                      (0x3ff)
#define ARAD_PP_METERING_ETH_TYPE_NDX_MAX                        (ARAD_PP_NOF_MTR_ETH_TYPES-1)

#define ARAD_PP_METERING_GLBL_PROFILE_IDX_MIN                    (0)
#define ARAD_PP_METERING_GLBL_PROFILE_IDX_MAX                    (7)

#define ARAD_PP_METERING_GROUP_MAX                               (ARAD_PP_METERING_MTR_GROUP_NDX_MAX)
#define ARAD_PP_METERING_ID_MAX                                  (32767)
#define ARAD_PP_METERING_COLOR_MODE_MAX                          (ARAD_PP_NOF_MTR_COLOR_MODES-1)

#define ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_EXP_BS 15
#define ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_BS 63
/* min mnt must be 1, otherwise always 0 */
#define ARAD_PP_MTR_VAL_TO_EXP_MNT_MIN_MNT_IR 1 
#define ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_IR 127
#define ARAD_PP_MTR_VAL_TO_EXP_MNT_MIN_MNT_EXP 0 
#define ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_EXP 7 
#define ARAD_PP_MTR_VAL_TO_EXP_MNT_MIN_REV_EXP 0 
#define ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_REV_EXP 15 

#define ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MULTI_BS 1
#define ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC_BS 64
#define ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC_IR 0
/* 2000/8: transfer from clk from bytes to kbits * 2 */
#define ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_DIV_IR 250

#define ARAD_PP_MTR_PROFILE_VAL_EXP_MNT_EQ_CONST_MULTI_BS 1
#define ARAD_PP_MTR_PROFILE_VAL_EXP_MNT_EQ_CONST_MNT_INC_BS 0
#define ARAD_PP_MTR_PROFILE_VAL_EXP_MNT_EQ_CONST_MNT_INC_IR 0
/* transfer from clk from bytes to kbits */
#define ARAD_PP_MTR_PROFILE_VAL_EXP_MNT_EQ_CONST_MNT_DIV_IR 125

#define ARAD_PP_MTR_PROFILE_VAL_TO_EXP_MNT_MAX_MNT_BS 255 
#define ARAD_PP_MTR_PROFILE_VAL_TO_EXP_MNT_MAX_EXP_BS 31


/* rates in kbps */
/* max rate is 300 Gbps */
#define ARAD_PP_BW_PROFILE_ADD_VERIFY_IR_MIN 18
/* policer has a different min rate */
#define ARAD_PP_MTR_PROFILE_ADD_VERIFY_IR_MIN 146
/* bursts in Bytes */
#define ARAD_PP_BW_PROFILE_ADD_VERIFY_BS_MAX 4161536    
#define ARAD_PP_BW_PROFILE_ADD_VERIFY_BS_MIN 64

/* due to HW limitations max burst size that can be used for calculations cannot be > 29 b' */
#define ARAD_PP_MTR_PROFILE_MAX_BS 536870911    

/* cir, max_cir (if sharing), eir, max_eir
   and if global_shring: max+min rates of 3 other meters of 4 hierarchical_meters */
#define ARAD_PP_BW_PROFILE_MAX_RATES_FOR_CALC 6    

/* PCD - Parallel color decision*/

/* The number of PCDs is dependent on the input bits (there are 6) */
#define ARAD_PP_MTR_PCD_OPT_NUM                         64

/* Possible values for the SOC property policer_result_parallel_color_map. */
#define ARAD_PP_MTR_PCD_RESULT_BEST                     0
#define ARAD_PP_MTR_PCD_RESULT_WORST                    1

/* Possible values for the SOC property policer_result_parallel_bucket_update. */
#define ARAD_PP_MTR_PCD_BUCKET_UPDATE_CONSTANT          0
#define ARAD_PP_MTR_PCD_BUCKET_UPDATE_TRANSPARENT       1
#define ARAD_PP_MTR_PCD_BUCKET_UPDATE_DEFERRED          2

/* For completeness, the pcd address format is:  
    bits [5:4] - MCD-B color decision
    bits [3] - MCD-B other bucket has credits
    bits [2:1] - MCD-A color decision
    bits [0] - MCD-A other bucket has credits
 
   Following are the definitions of the field offsets
   and lengths in the pcd address.
*/
#define ARAD_PP_MTR_PCD_MCDA_COL_LSB                1
#define ARAD_PP_MTR_PCD_MCDB_COL_LSB                4
#define ARAD_PP_MTR_PCD_COL_NOF_BITS                2
#define ARAD_PP_MTR_PCD_MCDA_OTHER_HAS_CREDITS_LSB  0
#define ARAD_PP_MTR_PCD_MCDB_OTHER_HAS_CREDITS_LSB  3
#define ARAD_PP_MTR_PCD_OTHER_HAS_CREDITS_NOF_BITS  1

#define _ARAD_PP_MTR_IR_VAL_TO_MAX_REV_EXP_OPTIMIZED_FOR_BUCKET_UPDATE_RATE_MAX_RATES 10

/* These are the possible colors for the MRPS.
   The in color and out color are chosen from/according
   to these. The committed bucket is considered the 'green'
   bucket, while the excess bucket is considered the
   'yellow' bucket. There is no 'red' bucket.*/
typedef enum {
  ARAD_PP_MTR_PCD_COL_GREEN     = 0,
  ARAD_PP_MTR_PCD_COL_INVALID   = 1,
  ARAD_PP_MTR_PCD_COL_YELLOW    = 2,
  ARAD_PP_MTR_PCD_COL_RED       = 3,
  ARAD_PP_MTR_PCD_NOF_COLS      = 4
} arad_pp_mtr_pcd_col_t;

/* Which color is the equivalent of the invalid.
   This color is used instead of the invalid color when
   the invalid color is received (as input). The invalid
   color is never used in output. */
#define ARAD_PP_MTR_PCD_COL_INVALID_EQUIV ARAD_PP_MTR_PCD_COL_YELLOW

/* Indexes for the meters in a parallel pair. */
typedef enum {
    ARAD_PP_MTR_PCD_PARALLEL_METER_A     = 0,
    ARAD_PP_MTR_PCD_PARALLEL_METER_B     = 1,
    ARAD_PP_MTR_PCD_NOF_PARALLEL_METERS  = 2
} arad_pp_mtr_meter_t;

typedef enum {
    /* No sharing mode (only valid for range_mode == 64). */
    ARAD_PP_MTR_TWO_METER_SHARING_MODE_NONE = 0,
    ARAD_PP_MTR_TWO_METER_SHARING_MODE_SERIAL = 1,
    ARAD_PP_MTR_TWO_METER_SHARING_MODE_PARALLEL = 2
} arad_pp_mtr_two_meter_sharing_mode_t;

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

/* all IDR_PRFCFG fields that exists both in shr_en and shr_dis configuration */
typedef struct
{
  uint32 color_aware;
  uint32 coupling_flag;
  uint32 sharing_flag;
  uint32 cir_mantissa;
  uint32 cir_mantissa_exponent;
  uint32 reset_cir;
  uint32 cbs_mantissa_64;
  uint32 cbs_exponent;
  uint32 eir_mantissa;
  uint32 eir_mantissa_exponent;
  uint32 max_eir_mantissa;
  uint32 max_eir_mantissa_exponent;
  uint32 reset_eir;
  uint32 ebs_mantissa_64;
  uint32 ebs_exponent;

#ifdef BCM_88660_A0
  uint32 packet_mode;
#endif
} __ATTRIBUTE_PACKED__ ARAD_IDR_PRFCFG_TBL_DATA;

typedef struct
{
  uint32 rate_mantissa;
  uint32 rate_exp;
  uint32 burst_mantissa;
  uint32 burst_exp;
  uint32 packet_mode;
  uint32 meter_resolution;
  uint32 color_blind;
} __ATTRIBUTE_PACKED__ ARAD_IDR_ETHERNET_METER_PROFILES_TBL_DATA;

typedef struct
{
  uint32 profile;
  uint32 global_meter_ptr;
  uint32 enable;
  uint32 parity;
} __ATTRIBUTE_PACKED__ ARAD_IDR_ETHERNET_METER_CONFIG_TBL_DATA;

typedef struct
{
  uint32 rate_mantissa;
  uint32 rate_exp;
  uint32 burst_mantissa;
  uint32 burst_exp;
  uint32 packet_mode;
  uint32 meter_resolution;
  uint32 enable;
  uint32 color_blind;
} __ATTRIBUTE_PACKED__ ARAD_IDR_GLOBAL_METER_PROFILES_TBL_DATA;

typedef struct 
{
  /* Does the bucket of color X have credits? */
  /* The red and invalid buckets are meaningless (even though they are set, they are unused). */
  uint8 bucket_credits[ARAD_PP_MTR_PCD_NOF_COLS];
  /* The color (of the bucket) to reduce. */
  uint32 reduce_buck_col;
  /* The lsb of the out color field in the address format. */
  uint8 addr_col_lsb;
  /* The lsb of the other color field in the address format. */
  uint8 addr_oth_cred_lsb;
} arad_pp_mtr_pcd_meter_info;

/* } */
/*************
 * GLOBALS   *
 *************/
/* { */

CONST STATIC SOC_PROCEDURE_DESC_ELEMENT
  Arad_pp_procedure_desc_element_metering[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_GLBL_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_GLBL_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_GLBL_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_GLBL_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_GLBL_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_GLBL_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_GLBL_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_GLBL_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_METERS_GROUP_INFO_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_METERS_GROUP_INFO_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_METERS_GROUP_INFO_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_METERS_GROUP_INFO_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_METERS_GROUP_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_METERS_GROUP_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_METERS_GROUP_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_METERS_GROUP_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_BW_PROFILE_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_BW_PROFILE_ADD_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_BW_PROFILE_ADD_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_BW_PROFILE_ADD_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_BW_PROFILE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_BW_PROFILE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_BW_PROFILE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_BW_PROFILE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_BW_PROFILE_REMOVE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_BW_PROFILE_REMOVE_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_BW_PROFILE_REMOVE_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_BW_PROFILE_REMOVE_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_ENABLE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_ENABLE_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_ENABLE_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_ENABLE_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_ENABLE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_ENABLE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_ENABLE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_ENABLE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_PARAMS_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_PARAMS_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_PARAMS_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_PARAMS_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_PARAMS_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_PARAMS_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_PARAMS_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_PARAMS_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_METERING_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_METERING_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */

  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_IR_VAL_FROM_REVERSE_EXP_MNT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_BS_VAL_TO_EXP_MNT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(ARAD_PP_MTR_CHK_PROFILE_RATE),

  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

CONST STATIC SOC_ERROR_DESC_ELEMENT
  Arad_pp_error_desc_element_metering[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    ARAD_PP_METERING_MTR_GROUP_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_METERING_MTR_GROUP_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'mtr_group_ndx' is out of range. \n\r "
    "The range is: 0 - 1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_BW_PROFILE_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_METERING_BW_PROFILE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'bw_profile_ndx' is out of range. \n\r "
    "The range is: 0 - 511.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_SUCCESS_OUT_OF_RANGE_ERR,
    "ARAD_PP_METERING_SUCCESS_OUT_OF_RANGE_ERR",
    "The parameter 'success' is out of range. \n\r "
    "The range is: 0 - SOC_SAND_NOF_SUCCESS_FAILURES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_BW_PROFILE_ID_OUT_OF_RANGE_ERR,
    "ARAD_PP_METERING_BW_PROFILE_ID_OUT_OF_RANGE_ERR",
    "The parameter 'bw_profile_id' is out of range. \n\r "
    "The range is: 0 - 511.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_RESULT_USE_OUT_OF_RANGE_ERR,
    "ARAD_PP_METERING_RESULT_USE_OUT_OF_RANGE_ERR",
    "The parameter 'result_use' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_MTR_RES_USES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_METERED_TYPE_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_METERING_METERED_TYPE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'metered_type_ndx' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_MTR_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_FIRST_METERED_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_METERING_FIRST_METERED_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'first_metered_ndx' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_MTR_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_NOF_METER_INS_OUT_OF_RANGE_ERR,
    "ARAD_PP_METERING_NOF_METER_INS_OUT_OF_RANGE_ERR",
    "The parameter 'nof_meter_ins' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_MTR_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_ETH_TYPE_NDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_METERING_ETH_TYPE_NDX_OUT_OF_RANGE_ERR",
    "The parameter 'eth_type_ndx' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_MTR_ETH_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_GLBL_PROFILE_IDX_OUT_OF_RANGE_ERR,
    "ARAD_PP_METERING_GLBL_PROFILE_IDX_OUT_OF_RANGE_ERR",
    "The parameter 'glbl_profile_idx' is out of range. \n\r "
    "The range is: 1 - 7.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_GROUP_OUT_OF_RANGE_ERR,
    "ARAD_PP_METERING_GROUP_OUT_OF_RANGE_ERR",
    "The parameter 'group' is out of range. \n\r "
    "The range is: 0 - 1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_ID_OUT_OF_RANGE_ERR,
    "ARAD_PP_METERING_ID_OUT_OF_RANGE_ERR",
    "The parameter 'id' is out of range. \n\r "
    "The range is: 0 - 511.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_IS_HR_ENABLED_OUT_OF_RANGE_ERR,
    "ARAD_PP_METERING_IS_HR_ENABLED_OUT_OF_RANGE_ERR",
    "The parameter 'is_hr_enabled' is out of range. \n\r "
    "The range is: 0 - 1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_MAX_PACKET_SIZE_OUT_OF_RANGE_ERR,
    "ARAD_PP_METERING_MAX_PACKET_SIZE_OUT_OF_RANGE_ERR",
    "The parameter 'max_packet_size' is out of range. \n\r "
    "The range is: 0 - 16*1024-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_CIR_OUT_OF_RANGE_ERR,
    "ARAD_PP_METERING_CIR_OUT_OF_RANGE_ERR",
    "The parameter 'cir' is out of range. \n\r "
    "The range is: 18 KBPS - 9750 GBPS.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_EIR_OUT_OF_RANGE_ERR,
    "ARAD_PP_METERING_EIR_OUT_OF_RANGE_ERR",
    "The parameter 'eir' is out of range. \n\r "
    "The range is: 18 KBPS - 9750 GBPS.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_CBS_OUT_OF_RANGE_ERR,
    "ARAD_PP_METERING_CBS_OUT_OF_RANGE_ERR",
    "The parameter 'cbs' is out of range. \n\r "
    "The range is: 64B - 4MB.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_EBS_OUT_OF_RANGE_ERR,
    "ARAD_PP_METERING_EBS_OUT_OF_RANGE_ERR",
    "The parameter 'ebs' is out of range. \n\r "
    "The range is: 64B - 4MB.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_COLOR_MODE_OUT_OF_RANGE_ERR,
    "ARAD_PP_METERING_COLOR_MODE_OUT_OF_RANGE_ERR",
    "The parameter 'color_mode' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_MTR_COLOR_MODES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_TYPE_OUT_OF_RANGE_ERR,
    "ARAD_PP_METERING_TYPE_OUT_OF_RANGE_ERR",
    "The parameter 'type' is out of range. \n\r "
    "The range is: 0 - ARAD_PP_NOF_MTR_TYPES-1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */
  {
    ARAD_PP_METERING_CHK_PROFILE_RATE_ERR,
    "ARAD_PP_METERING_CHK_PROFILE_RATE_ERR",
    "The parameters BW_PROFILE are mixed High rate - Low rate. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_VAL_FROM_REVERSE_EXP_MNT_CLK_OF_ERR,
    "ARAD_PP_METERING_VAL_FROM_REVERSE_EXP_MNT_CLK_OF_ERR",
    "The clock core frequency may cause Overflow. \n\r "
    "The range is: 0 - ARAD_PP_METERING_VAL_FROM_REVERSE_EXP_MNT_CLK_OF.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_UPDATE_ALL_COPIES_MAX_RANGE_ERR,
    "ARAD_PP_METERING_UPDATE_ALL_COPIES_MAX_RANGE_ERR",
    "The parameter 'update_all_copies' is out of range. \n\r "
    "The range is: 0 - 1.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_BW_PROFILE_HR_RATE_ERR,
    "ARAD_PP_METERING_BW_PROFILE_HR_RATE_ERR",
    "Profile rate parameters are for High Rate. \n\r "
    "MeterProcc is in Low rate or profile index is for Low rate\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_BW_PROFILE_LR_RATE_ERR,
    "ARAD_PP_METERING_BW_PROFILE_LR_RATE_ERR",
    "Profile rate parameters are for Low Rate. \n\r "
    "MeterProcc is in High rate and Profile index is for High rate\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_MTR_ETH_POLICER_ADD_FAIL_ERR,
    "ARAD_PP_MTR_ETH_POLICER_ADD_FAIL_ERR",
    "Fail to add profile to multiset. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_MTR_ETH_POLICER_PARAMS_SET_VERIFY_OUT_OF_RANGE_ERR,
    "ARAD_PP_MTR_ETH_POLICER_PARAMS_SET_VERIFY_OUT_OF_RANGE_ERR",
    "One of the parameters: color_mode, is_coupling_enabled, is_fairness_enabled, disable_cir, disable_eir is out of range. \n\r "
    "This parameters value should be 0 (FALSE), in eth policer. \n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_POLICER_INGRESS_COUNT_INVALID_ERR,
    "ARAD_PP_METERING_POLICER_INGRESS_COUNT_INVALID_ERR",
    "The soc property 'policer_ingress_count' is invalid. \n\r "
    "Valid values are: 32/64.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_POLICER_INGRESS_SHARING_MODE_INVALID_ERR,
    "ARAD_PP_METERING_POLICER_INGRESS_SHARING_MODE_INVALID_ERR",
    "The soc property 'policer_ingress_sharing_mode' is invalid. \n\r "
    "Valid values are: 0 (if policer_ingress_count is 64) or 1/2 (if policer_ingress_count is 32).\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    ARAD_PP_METERING_MAX_IR_INVALID_ERR,
    "ARAD_PP_METERING_MAX_IR_INVALID_ERR",
    "max CIR/max EIR cannot be smaller than CIR/EIR. \n\r ",
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


/*Static function declaraion Start*/

STATIC uint32 arad_pp_metering_pcd_init(SOC_SAND_IN int unit, SOC_SAND_IN arad_pp_mtr_two_meter_sharing_mode_t sharing_mode);
STATIC uint32 arad_pp_metering_pcd_init_serial(SOC_SAND_IN int unit);
STATIC uint32 arad_pp_metering_pcd_init_parallel(SOC_SAND_IN int unit);

/*Static function declaraion End*/

STATIC uint32
  arad_pp_mtr_ir_val_from_reverse_exp_mnt(
    SOC_SAND_IN int                       unit,
    SOC_SAND_IN uint32                       rev_exp,
    SOC_SAND_IN uint32                       mant_exp,
    SOC_SAND_IN uint32                       mnt,
    SOC_SAND_OUT uint32                      *val
  )
{
  uint32
    res = SOC_SAND_OK,
    clk_freq;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_IR_VAL_FROM_REVERSE_EXP_MNT);

  SOC_SAND_CHECK_NULL_INPUT(val);

  clk_freq = arad_chip_ticks_per_sec_get(unit);

  res = soc_sand_compute_complex_to_mnt_exp_reverse_exp(
      mnt,
      mant_exp,
      rev_exp + 2, 
      clk_freq, 
      ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_DIV_IR,
      ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC_IR,
      val
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_ir_val_from_reverse_exp_mnt()", 0, 0);
}

STATIC uint32
  arad_pp_mtr_bs_val_to_exp_mnt(
    SOC_SAND_IN int                       unit,
    SOC_SAND_IN uint32                       val,
    SOC_SAND_OUT uint32                      *exp,
    SOC_SAND_OUT uint32                      *mnt
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_BS_VAL_TO_EXP_MNT);

  SOC_SAND_CHECK_NULL_INPUT(exp);
  SOC_SAND_CHECK_NULL_INPUT(mnt);

  res = soc_sand_break_complex_to_mnt_exp_round_down(
    val,
    ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_BS,
    ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_EXP_BS,
    ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MULTI_BS,
    ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC_BS,
    mnt,
    exp
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
   
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_val_to_exp_mnt()", 0, 0);
}

/* 
 * Calculate the highest shared rev_exp for: 
 * 1) CIR and max CIR (when sharing is not enabled)
 * 2) EIR and max EIR (when sharing is not enabled)
 * 3) CIR, EIR, mac CIR and max EIR (when sharing is enabled)  
 */
STATIC uint32
  arad_pp_mtr_ir_val_to_max_rev_exp(
    SOC_SAND_IN int                       unit,
    SOC_SAND_IN uint32                       min_rate, /* min CIR/EIR */
    SOC_SAND_IN uint32                       max_rate, /* max CIR/EIR */
    SOC_SAND_OUT uint32                      *rev_exp
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    clk_freq; 
  uint32
    calc_min_rate, /* min val calculated for the current rev_exp */
    calc_max_rate; /* max val calculated for the current rev_exp */
  int 
    tmp_rev_exp;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_IR_VAL_TO_MAX_REV_EXP);

  SOC_SAND_CHECK_NULL_INPUT(rev_exp);

  clk_freq = arad_chip_ticks_per_sec_get(unit);

  for (tmp_rev_exp = ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_REV_EXP; tmp_rev_exp >= ARAD_PP_MTR_VAL_TO_EXP_MNT_MIN_REV_EXP; tmp_rev_exp--) {

      /* calculate max rate for this rev_exp (with max mnt and max exp) */
      res = soc_sand_compute_complex_to_mnt_exp_reverse_exp(
          ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_IR,
          ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_EXP,
          tmp_rev_exp + 2, 
          clk_freq,
          ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_DIV_IR,
          ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC_IR, 
          &calc_max_rate 
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      /* calculate min rate for this rev_exp (with min mnt and min exp) */
      res = soc_sand_compute_complex_to_mnt_exp_reverse_exp(
          ARAD_PP_MTR_VAL_TO_EXP_MNT_MIN_MNT_IR,
          ARAD_PP_MTR_VAL_TO_EXP_MNT_MIN_MNT_EXP,
          tmp_rev_exp + 2, 
          clk_freq,
          ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_DIV_IR,
          ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC_IR, 
          &calc_min_rate 
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

      if ((calc_max_rate >= max_rate) && (calc_min_rate <= min_rate)) {
          /* found max rev_exp */
          break;
      }
  }

  if (/*is_shr &&*/ (tmp_rev_exp < ARAD_PP_MTR_VAL_TO_EXP_MNT_MIN_REV_EXP)) {
      /* no shared rev_exp could be found for CIR and EIR */
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_METERING_VAL_FROM_REVERSE_EXP_MNT_CLK_OF_ERR, 50, exit); 
  }

  *rev_exp = tmp_rev_exp;
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_ir_val_to_max_rev_exp()", 0, 0);
}

/*   
 * First calculate the highest shared rev_exp as described in arad_pp_mtr_ir_val_to_max_rev_exp. 
 * Afterwards look for a lower rev_exp, as long as the accuracy of the rates are unaffected by it. 
 *  
 * INPUT
 *   rates - The rev_exp found will match these rates.
 *   nof_rates - The number of rates in the rates array.
 *   rev_exp - The returned rev_exp.
 * NOTES 
 *   Typically we expect that the highest rev_exp has a high exp even for the lowest rate, which are 
 *   directly eliminated without contributing to accuracy. 
 *   Therefore if the exponent is positive, it can always be eliminated in HW along with the rev_exp. 
 *   Afterwards the mantissa can also be optimized, however this may hurt accuracy (however the 
 *   function tries this too as long as accuracy is unaffected). 
 */
STATIC uint32
  arad_pp_mtr_ir_val_to_max_rev_exp_optimized_for_bucket_update_rate(
    SOC_SAND_IN int                          unit,
    SOC_SAND_IN uint32                       *rates,
    SOC_SAND_IN uint32                       nof_rates,
    SOC_SAND_OUT uint32                      *rev_exp
  )
{
  uint32 clk_freq; 
  uint32 min_rate;
  uint32 max_rate;
  int tmp_rev_exp;
  uint32 highest_available_rev_exp = 0;
  uint32 rate_idx = 0;
  /* Absolute rate errors: | user_rate - calc_rate | */
  uint32 rate_errors[_ARAD_PP_MTR_IR_VAL_TO_MAX_REV_EXP_OPTIMIZED_FOR_BUCKET_UPDATE_RATE_MAX_RATES];
  uint32 selected_rev_exp = -1;
  uint32 res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_PROFILE_RATE_TO_RES_EXP_MNT);

  SOC_SAND_CHECK_NULL_INPUT(rev_exp);

  clk_freq = arad_chip_ticks_per_sec_get(unit);

  /* assert(0 < nof_rates && nof_rates <= 10); */
  if (nof_rates <= 0 || 10 < nof_rates) {
    SOC_SAND_SET_ERROR_CODE(1, 0, exit); 
  }
  
  /* Find min and max rates. */
  max_rate = min_rate = rates[0];
  for (rate_idx = 0; rate_idx < nof_rates; rate_idx++) {
    min_rate = SOC_SAND_MIN(rates[rate_idx], min_rate);
    max_rate = SOC_SAND_MAX(rates[rate_idx], min_rate);
  }

  /* First step is to calculate the highest rev_exp where all rates are in range of this rev_exp. */
  res = arad_pp_mtr_ir_val_to_max_rev_exp(unit, min_rate, max_rate, &highest_available_rev_exp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 0, exit);

  selected_rev_exp = highest_available_rev_exp;

  for (tmp_rev_exp = highest_available_rev_exp; tmp_rev_exp >= ARAD_PP_MTR_VAL_TO_EXP_MNT_MIN_REV_EXP; tmp_rev_exp--) {
    for (rate_idx = 0; rate_idx < nof_rates; rate_idx++) {
      uint32 mant;
      uint32 exp;
      uint32 cfg_rate;
      uint32 curr_error;

      /* Calculate the optimal (mantissa, exponent) for all rates with this rev_exp. */
      res = soc_sand_break_complex_to_mnt_exp_round_down_2(
          rates[rate_idx],
          ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_IR,
          ARAD_PP_MTR_VAL_TO_EXP_MNT_MIN_MNT_IR,
          ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_EXP,
          clk_freq,
          (ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_DIV_IR << (tmp_rev_exp + 2)),
          ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC_IR,
          &mant,
          &exp
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 0, exit);

      /* Calculate the configured rate. */
      res = soc_sand_compute_complex_to_mnt_exp_reverse_exp(
         mant,
         exp,
         tmp_rev_exp + 2, 
         clk_freq,
         ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_DIV_IR,
         ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC_IR, 
         &cfg_rate
         );
      SOC_SAND_CHECK_FUNC_RESULT(res, 0, exit);

      /* Calculate the new error and the difference to the old error. */
      /* error = | cfg_rate - user_rate | */
      curr_error = soc_sand_abs(rates[rate_idx] - cfg_rate);
      
      /* First rev_exp, just record the errors. */
      if (tmp_rev_exp == highest_available_rev_exp) {
        rate_errors[rate_idx] = curr_error;
        continue;
      }

      /* Stop if one of the rate errors was worse. */
      if (curr_error > rate_errors[rate_idx]) {
        break;
      }
      rate_errors[rate_idx] = curr_error;
    }
    /* Stop if one of the rate errors was worse. */
    if (rate_idx < nof_rates) {
      break;
    }

    /* If all rates had an equal or better error, then we use this rev_exp. */
    selected_rev_exp = tmp_rev_exp;
  }
  
  *rev_exp = selected_rev_exp;
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_ir_val_to_max_rev_exp_optimized_for_bucket_update_rate()", 0, 0);
}

/* 
 * Break down the rate to mantissa, exponent and resolution.
 * Used for Ethernet Meter Profiles and Global Meter Profiles configuration.
 */
STATIC uint32
  arad_pp_mtr_profile_rate_to_res_exp_mnt(
    SOC_SAND_IN int                       unit,
    SOC_SAND_IN uint32                       val,
    SOC_SAND_OUT uint32                      *rev_exp,
    SOC_SAND_OUT uint32                      *mnt,
    SOC_SAND_OUT uint32                      *exp
  )
{
  uint32
    res = SOC_SAND_OK,
    clk_freq,
    calc_max_rate,
    calc_min_rate;
  int
    tmp_rev_exp;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_PROFILE_RATE_TO_RES_EXP_MNT);

  SOC_SAND_CHECK_NULL_INPUT(rev_exp);
  SOC_SAND_CHECK_NULL_INPUT(mnt);
  SOC_SAND_CHECK_NULL_INPUT(exp);

  clk_freq = arad_chip_ticks_per_sec_get(unit);

  /* find the max res (rev_exp) for this rate:
    max res gives the most accurate rate */
  for (tmp_rev_exp = ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_REV_EXP; tmp_rev_exp >= ARAD_PP_MTR_VAL_TO_EXP_MNT_MIN_REV_EXP; tmp_rev_exp--) {

      /* calculate min_rate and max_rate for this res (rev_exp) */
      soc_sand_compute_complex_to_mnt_exp_reverse_exp(
          ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_IR,
          ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_EXP,
          tmp_rev_exp,
          clk_freq,
          ARAD_PP_MTR_PROFILE_VAL_EXP_MNT_EQ_CONST_MNT_DIV_IR,
          ARAD_PP_MTR_PROFILE_VAL_EXP_MNT_EQ_CONST_MNT_INC_IR,
          &calc_max_rate
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      soc_sand_compute_complex_to_mnt_exp_reverse_exp(
          ARAD_PP_MTR_VAL_TO_EXP_MNT_MIN_MNT_IR,
          ARAD_PP_MTR_VAL_TO_EXP_MNT_MIN_MNT_EXP,
          tmp_rev_exp,
          clk_freq,
          ARAD_PP_MTR_PROFILE_VAL_EXP_MNT_EQ_CONST_MNT_DIV_IR,
          ARAD_PP_MTR_PROFILE_VAL_EXP_MNT_EQ_CONST_MNT_INC_IR,
          &calc_min_rate
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

      if ((calc_max_rate >= val) && (calc_min_rate <= val)) {
          /* found max res */
          break;
      }
  }

  /* calculate mnt and exp, based on res */
  res = soc_sand_break_complex_to_mnt_exp_round_down_2(
      val,
      ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_IR,
      ARAD_PP_MTR_VAL_TO_EXP_MNT_MIN_MNT_IR,
      ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_EXP, 
      clk_freq,
      (ARAD_PP_MTR_PROFILE_VAL_EXP_MNT_EQ_CONST_MNT_DIV_IR << tmp_rev_exp),
      ARAD_PP_MTR_PROFILE_VAL_EXP_MNT_EQ_CONST_MNT_INC_IR, 
      mnt,
      exp
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  /* res (rev-exp) also affects the burst: actual-burst = user-burst << res
     therefore rev-exp should be the smallest possible 
	 we can lower rev-exp by lowering exp as much as possible */
  *rev_exp = tmp_rev_exp;
  if (*rev_exp >= *exp) {
	  *rev_exp = *rev_exp - *exp;
	  *exp = 0;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_profile_rate_to_res_exp_mnt()", 0, 0);
}

/* 
 * Break down the burst size to mantissa and exponent.
 * Used for Ethernet Meter Profiles and Global Meter Profiles configuration. 
 */
STATIC uint32
  arad_pp_mtr_profile_burst_to_exp_mnt(
    SOC_SAND_IN int                       unit,
    SOC_SAND_IN uint32                       val,
    SOC_SAND_OUT uint32                      resolution, /* rate resolution */
    SOC_SAND_OUT uint32                      *mnt,
    SOC_SAND_OUT uint32                      *exp
  )
{
  uint32
    res = SOC_SAND_OK,
	calc_val;
  SOC_SAND_U64
    tmp_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(mnt);
  SOC_SAND_CHECK_NULL_INPUT(exp);

  /* in Policer: rate resolution also affects the burst size: actual-burst = user-burst << res
   that's because in HW: packet-size << res, so must shift the burst as well, otherwise all packets will be dropped */

  /* burst size for calculation cannot be > bucket size (29b'), otherwise all traffic is dropped becuase the bucket does not fill */
  soc_sand_u64_multiply_longs(val, (uint32)(1 << resolution), &tmp_val); 
  res = soc_sand_u64_to_long(&tmp_val, &calc_val);
  if (res /* overflow */ || (calc_val > ARAD_PP_MTR_PROFILE_MAX_BS)) { 
	  /* burst = highest possible val for bucket to fill (HW limitation) */
	  calc_val = ARAD_PP_MTR_PROFILE_MAX_BS;
  }

  res = soc_sand_break_complex_to_mnt_exp_round_down_2(
	  calc_val,
	  ARAD_PP_MTR_PROFILE_VAL_TO_EXP_MNT_MAX_MNT_BS,
	  1,
	  ARAD_PP_MTR_PROFILE_VAL_TO_EXP_MNT_MAX_EXP_BS,
	  ARAD_PP_MTR_PROFILE_VAL_EXP_MNT_EQ_CONST_MULTI_BS,
	  1,
	  ARAD_PP_MTR_PROFILE_VAL_EXP_MNT_EQ_CONST_MNT_INC_BS,
	  mnt,
	  exp
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_profile_burst_to_exp_mnt()", 0, 0);
}

void
  arad_pp_mtr_eth_policer_profile_key_get(
    SOC_SAND_IN  int                                               unit,
    SOC_SAND_IN  ARAD_IDR_ETHERNET_METER_PROFILES_TBL_DATA         profile_tbl_data,
    SOC_SAND_OUT uint32                                            *eth_mtr_profile_multiset_key
  )
{
  *eth_mtr_profile_multiset_key = 0;
  *eth_mtr_profile_multiset_key |= SOC_SAND_SET_BITS_RANGE(profile_tbl_data.rate_mantissa,6,0); 
  *eth_mtr_profile_multiset_key |= SOC_SAND_SET_BITS_RANGE(profile_tbl_data.rate_exp,9,7); 
  *eth_mtr_profile_multiset_key |= SOC_SAND_SET_BITS_RANGE(profile_tbl_data.burst_mantissa,17,10); 
  *eth_mtr_profile_multiset_key |= SOC_SAND_SET_BITS_RANGE(profile_tbl_data.burst_exp,22,18); 
  *eth_mtr_profile_multiset_key |= SOC_SAND_SET_BITS_RANGE(profile_tbl_data.packet_mode,23,23);
  *eth_mtr_profile_multiset_key |= SOC_SAND_SET_BITS_RANGE(profile_tbl_data.meter_resolution,27,24);
  if (SOC_IS_ARADPLUS(unit)) {
    *eth_mtr_profile_multiset_key |= SOC_SAND_SET_BITS_RANGE(profile_tbl_data.color_blind,28,28);
  }
}

void
  arad_pp_mtr_eth_policer_global_key_get(
    SOC_SAND_IN  int                                               unit,
    SOC_SAND_IN  ARAD_IDR_GLOBAL_METER_PROFILES_TBL_DATA           global_tbl_data,
    SOC_SAND_OUT uint32                                            *eth_mtr_global_multiset_key
  )
{
  *eth_mtr_global_multiset_key = 0;
  *eth_mtr_global_multiset_key |= SOC_SAND_SET_BITS_RANGE(global_tbl_data.rate_mantissa,6,0);
  *eth_mtr_global_multiset_key |= SOC_SAND_SET_BITS_RANGE(global_tbl_data.rate_exp,9,7);
  *eth_mtr_global_multiset_key |= SOC_SAND_SET_BITS_RANGE(global_tbl_data.burst_mantissa,17,10);
  *eth_mtr_global_multiset_key |= SOC_SAND_SET_BITS_RANGE(global_tbl_data.burst_exp,22,18);
  *eth_mtr_global_multiset_key |= SOC_SAND_SET_BITS_RANGE(global_tbl_data.packet_mode,23,23);
  *eth_mtr_global_multiset_key |= SOC_SAND_SET_BITS_RANGE(global_tbl_data.meter_resolution,27,24);
  if (SOC_IS_ARADPLUS(unit)) {
    *eth_mtr_global_multiset_key |= SOC_SAND_SET_BITS_RANGE(global_tbl_data.color_blind,28,28);
  }
}

void
  arad_pp_mtr_max_min_vals_for_calc_get(
    SOC_SAND_IN  uint32           *vals,
    SOC_SAND_IN  uint32           nof_vals,
    SOC_SAND_OUT  uint32          *min_val,
    SOC_SAND_OUT  uint32          *max_val
  )
{
    int i = 0;
    *min_val = vals[i]; 
    *max_val = vals[i]; 
    for (i = 1; i < nof_vals; i++) {
        if (*min_val > vals[i]) {
            *min_val = vals[i];
        }
        else if (*max_val < vals[i]) {
            *max_val = vals[i];
        }
    }
}

#ifdef BCM_88660_A0

/**
 * Initialize and set the PCD (parallel color decision) map.
 * Used both to allow more flexibility whe deciding on the out 
 * color and charged bucket in case of Parallel metetring and to
 * verify both updated buckets have sufficent credits in Serial 
 * metering. 
 */
/*STATIC*/ uint32 
  arad_pp_metering_pcd_init(
     SOC_SAND_IN int unit,
	 SOC_SAND_IN arad_pp_mtr_two_meter_sharing_mode_t sharing_mode)
{
	uint32
		res = SOC_SAND_OK;

	SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_METERING_PCD_INIT);

	/* Build PCD map based on sharing mode*/
	if (ARAD_PP_MTR_TWO_METER_SHARING_MODE_SERIAL == sharing_mode) {
		res = arad_pp_metering_pcd_init_serial(unit);
	}
	else{
		res = arad_pp_metering_pcd_init_parallel(unit);
	}
	SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_metering_pcd_init()", 0, 0);
}

/*STATIC*/ uint32
  arad_pp_metering_pcd_init_serial(
	  SOC_SAND_IN int unit)
{
	uint32
		/* Per meter color*/
		mcda_mtr_color,
		mcdb_mtr_color,	
		/* Does the other bucket have credits*/
		mcda_other_credits,
		mcdb_other_credits,
		/* Final color decision for this PCD address*/
		final_color, 
		addr_idx,
		pcd_line;
	uint32
		res = SOC_SAND_OK;
	soc_mem_t
		mem = SOC_IS_JERICHO(unit)? MRPS_PCD_MAPm: IDR_PCD_MAPm;	

	SOC_SAND_INIT_ERROR_DEFINITIONS(0);

	/* Fill PCD table*/
	for (addr_idx = 0; addr_idx < ARAD_PP_MTR_PCD_OPT_NUM; addr_idx++) {

		pcd_line = mcda_mtr_color = mcdb_mtr_color = mcda_other_credits = mcdb_other_credits = final_color = 0;

		/* Get the meter color from address. */
        SHR_BITCOPY_RANGE(&(mcda_mtr_color), 0, &addr_idx, 
                          ARAD_PP_MTR_PCD_MCDA_COL_LSB, ARAD_PP_MTR_PCD_COL_NOF_BITS);
		SHR_BITCOPY_RANGE(&(mcdb_mtr_color), 0, &addr_idx, 
                          ARAD_PP_MTR_PCD_MCDB_COL_LSB, ARAD_PP_MTR_PCD_COL_NOF_BITS);

		/* Get other bucket credits flag from address*/
        SHR_BITCOPY_RANGE(&(mcda_other_credits), 0, &addr_idx, 
                          ARAD_PP_MTR_PCD_MCDA_OTHER_HAS_CREDITS_LSB, ARAD_PP_MTR_PCD_OTHER_HAS_CREDITS_NOF_BITS);
		SHR_BITCOPY_RANGE(&(mcdb_other_credits), 0, &addr_idx, 
                          ARAD_PP_MTR_PCD_MCDB_OTHER_HAS_CREDITS_LSB, ARAD_PP_MTR_PCD_OTHER_HAS_CREDITS_NOF_BITS);


		/* Final color should always be the worse of the two meters decision.
		   If the two meters do not agree, take the worse decision only if the other meter have sufficent credits in
		   its matching bucket. If not- esclate to Red*/
        if((mcda_mtr_color == ARAD_PP_MTR_PCD_COL_RED) || (mcdb_mtr_color == ARAD_PP_MTR_PCD_COL_RED)) {
			final_color = ARAD_PP_MTR_PCD_COL_RED;
		} else  {
			if(mcda_mtr_color == mcdb_mtr_color){
				final_color = mcdb_mtr_color;
			} else {
				if ((mcda_mtr_color == ARAD_PP_MTR_PCD_COL_GREEN) && (mcdb_mtr_color == ARAD_PP_MTR_PCD_COL_YELLOW)){
					if (mcda_other_credits) {
						final_color = ARAD_PP_MTR_PCD_COL_YELLOW;
					}
                    else{
						final_color = ARAD_PP_MTR_PCD_COL_RED;
					}
				}
				else if ((mcda_mtr_color == ARAD_PP_MTR_PCD_COL_YELLOW) && (mcdb_mtr_color == ARAD_PP_MTR_PCD_COL_GREEN)){
					if (mcdb_other_credits) {
						final_color = ARAD_PP_MTR_PCD_COL_YELLOW;
					}
                    else{
                         final_color = ARAD_PP_MTR_PCD_COL_RED;
					}
				}
				else /* Just in case*/
					final_color = ARAD_PP_MTR_PCD_COL_RED;
			}
		}

		 /* Set bucket update and out color to final color*/
		 soc_mem_field32_set(unit, mem, &pcd_line, MCDA_UPDATE_COLORf, final_color);
		 soc_mem_field32_set(unit, mem, &pcd_line, MCDB_UPDATE_COLORf, final_color);
		 soc_mem_field32_set(unit, mem, &pcd_line, OUT_COLORf, final_color);

		 /* Write entry to table*/
		 res = soc_mem_write(unit, mem, MEM_BLOCK_ANY, addr_idx, &pcd_line); 
		 SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 10, exit);
	}

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_metering_pcd_init_serial()", 0, 0);

}

/*STATIC*/ uint32 
  arad_pp_metering_pcd_init_parallel(
     SOC_SAND_IN int unit)
{
  /* Change to meaningful names */
  uint32 
    addr_idx,
    meter_idx,
    color_idx,
    /* The result (best/worst) of parallel meters */
    pcd_result = ARAD_PP_MTR_PCD_RESULT_WORST,
    /* Which bucket is updated in parallel meters*/
    pcd_bucket_update = ARAD_PP_MTR_PCD_BUCKET_UPDATE_CONSTANT,
    /* The result color */
    res_col,
    /* The final pcd line composed of the out color and reduction buckets */
    pcd_line,
    mtr_col,
    oth_cred,
    same_color_creds,
    oth_color_creds,
    oth_col,
    reduce_col;
  uint32
    res = SOC_SAND_OK;
  soc_mem_t
      mem = SOC_IS_JERICHO(unit)? MRPS_PCD_MAPm: IDR_PCD_MAPm;

  /* Per meter info */
  arad_pp_mtr_pcd_meter_info meter_infos[ARAD_PP_MTR_PCD_NOF_PARALLEL_METERS];

  /* The color of the bucket that is indicated by the other bucket of the address. */
  uint8 other_color_map[ARAD_PP_MTR_PCD_NOF_COLS];

  char 
    * propval,
    * propkey;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  /* The color of the 'other bucket' indicated by the address, according to spec. */
  other_color_map[ARAD_PP_MTR_PCD_COL_GREEN] = ARAD_PP_MTR_PCD_COL_YELLOW;
  other_color_map[ARAD_PP_MTR_PCD_COL_YELLOW] = ARAD_PP_MTR_PCD_COL_GREEN;
  other_color_map[ARAD_PP_MTR_PCD_COL_RED] = ARAD_PP_MTR_PCD_COL_YELLOW;

  meter_infos[ARAD_PP_MTR_PCD_PARALLEL_METER_A].addr_col_lsb = ARAD_PP_MTR_PCD_MCDA_COL_LSB;
  meter_infos[ARAD_PP_MTR_PCD_PARALLEL_METER_B].addr_col_lsb = ARAD_PP_MTR_PCD_MCDB_COL_LSB;
  meter_infos[ARAD_PP_MTR_PCD_PARALLEL_METER_A].addr_oth_cred_lsb = ARAD_PP_MTR_PCD_MCDA_OTHER_HAS_CREDITS_LSB;
  meter_infos[ARAD_PP_MTR_PCD_PARALLEL_METER_B].addr_oth_cred_lsb = ARAD_PP_MTR_PCD_MCDB_OTHER_HAS_CREDITS_LSB;
  
  /* Get the parallel result method (best/worst), default is worst. */
  pcd_result = ARAD_PP_MTR_PCD_RESULT_WORST;
  propkey = spn_POLICER_RESULT_PARALLEL_COLOR_MAP;
  propval = soc_property_get_str(unit, propkey);
  if (propval) {
    if (sal_strcmp(propval, "BEST") == 0) {
      pcd_result = ARAD_PP_MTR_PCD_RESULT_BEST;
    } else if (sal_strcmp(propval, "WORST") == 0) {
      pcd_result = ARAD_PP_MTR_PCD_RESULT_WORST;
    } else {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_METERING_POLICER_RESULT_PARALLEL_COLOR_MAPPING_INVALID_ERR, 1000, exit);
    }
  }

  /* Get the parallel result method (constant/transparent/deferred), default is constant. */
  pcd_bucket_update = ARAD_PP_MTR_PCD_BUCKET_UPDATE_CONSTANT,
  propkey = spn_POLICER_RESULT_PARALLEL_BUCKET_UPDATE;
  propval = soc_property_get_str(unit, propkey);
  if (propval) {
    if (sal_strcmp(propval, "CONSTANT") == 0) {
      pcd_bucket_update = ARAD_PP_MTR_PCD_BUCKET_UPDATE_CONSTANT;
    } else if (sal_strcmp(propval, "TRANSPARENT") == 0) {
      pcd_bucket_update = ARAD_PP_MTR_PCD_BUCKET_UPDATE_TRANSPARENT;
    } else if (sal_strcmp(propval, "DEFERRED") == 0) {
      pcd_bucket_update = ARAD_PP_MTR_PCD_BUCKET_UPDATE_DEFERRED;
    } else {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_METERING_POLICER_RESULT_PARALLEL_BUCKET_UPDATE_INVALID_ERR, 2000, exit);
    }
  }

  /* The PCD map is enabled during the global configuration init */
  /* Just need to fill the PCD table. */

  /* There are 64 entries - one per pair of color inputs (6 bits) */
  for (addr_idx = 0; addr_idx < ARAD_PP_MTR_PCD_OPT_NUM; addr_idx++) {
    
    /* We assume that best = min and worst = max, and thus for best we want
       the minimal color and for worst the maximal color.
       Based on this we start res_col at the appropriate extreme (maximal and minimal respectively)
       and apply min/max accordingly for each meter. */
    res_col = (pcd_result == ARAD_PP_MTR_PCD_RESULT_BEST) ? ARAD_PP_MTR_PCD_COL_RED : ARAD_PP_MTR_PCD_COL_GREEN;

    /* Initialize all bucket credits of all meters to none. */
    for (meter_idx = 0; meter_idx < ARAD_PP_MTR_PCD_NOF_PARALLEL_METERS; meter_idx++) {
        for (color_idx = 0; color_idx < ARAD_PP_MTR_PCD_NOF_COLS; color_idx++) {
            meter_infos[meter_idx].bucket_credits[color_idx] = 0;
        }
    }

    /* Calculate res_col and for each meter the bucket credits */
    for (meter_idx = 0; meter_idx < ARAD_PP_MTR_PCD_NOF_PARALLEL_METERS; meter_idx++) {
        mtr_col = 0;
        oth_cred = 0;
        
        /* Get the meter color and other bucket credits from the address. */
        SHR_BITCOPY_RANGE(&(mtr_col), 0, &addr_idx, 
                          meter_infos[meter_idx].addr_col_lsb, ARAD_PP_MTR_PCD_COL_NOF_BITS);
        SHR_BITCOPY_RANGE(&(oth_cred), 0, &addr_idx, 
                          meter_infos[meter_idx].addr_oth_cred_lsb, ARAD_PP_MTR_PCD_OTHER_HAS_CREDITS_NOF_BITS);

        /* Check the values and make them canonical (0/1, RED/YELLOW/GREEN). */
        if (mtr_col == ARAD_PP_MTR_PCD_COL_INVALID) {
            mtr_col = ARAD_PP_MTR_PCD_COL_INVALID_EQUIV;
        }
        oth_cred = oth_cred ? 1 : 0;

        /* Finally deduce the bucket credits according to the color and oth_cred */
        /* Note that we also set the red credits although that has no meaning. */
        /* The bucket with the same color as the one that the meter had output
           must have credits. */
        meter_infos[meter_idx].bucket_credits[mtr_col] = 1;
        meter_infos[meter_idx].bucket_credits[other_color_map[mtr_col]] = oth_cred;

        res_col = (pcd_result == ARAD_PP_MTR_PCD_RESULT_BEST) ? SOC_SAND_MIN(res_col, mtr_col) :
            SOC_SAND_MAX(res_col, mtr_col);
    }

    /* Now res_col is calculated */
    /* Calculate the reduction color according to the meter bucket credits and the result color. */
    for (meter_idx = 0; meter_idx < ARAD_PP_MTR_PCD_NOF_PARALLEL_METERS; meter_idx++) {

        /* Check if same color or other color has credits. */
        oth_col = other_color_map[res_col];
        same_color_creds = meter_infos[meter_idx].bucket_credits[res_col];
        oth_color_creds = meter_infos[meter_idx].bucket_credits[oth_col];
        
        /* If the result is red don't take credits from anyone */
        
        reduce_col = ARAD_PP_MTR_PCD_COL_RED;

        switch (pcd_bucket_update) {
        case ARAD_PP_MTR_PCD_BUCKET_UPDATE_DEFERRED:
            
            /* If red then don't reduce anything. */
            if (res_col == ARAD_PP_MTR_PCD_COL_RED) {
                reduce_col = ARAD_PP_MTR_PCD_COL_RED;
            /* If we have the same color credits then reduce them */
            } else if (same_color_creds) {
                reduce_col = res_col;
            /* If we have other bucket credits then use them */
            } else if (oth_color_creds) {
                reduce_col = oth_col;
            /* Otherwise red */
            } else {
                reduce_col = ARAD_PP_MTR_PCD_COL_RED;
            }

            break;
        case ARAD_PP_MTR_PCD_BUCKET_UPDATE_TRANSPARENT:

            /* If red then don't reduce anything. */
            if (res_col == ARAD_PP_MTR_PCD_COL_RED) {
                reduce_col = ARAD_PP_MTR_PCD_COL_RED;
            /* If we have the same color credits then reduce them */
            } else if (same_color_creds) {
                reduce_col = res_col;
            /* Otherwise red */
            } else {
                reduce_col = ARAD_PP_MTR_PCD_COL_RED;
            }

            break;
        case ARAD_PP_MTR_PCD_BUCKET_UPDATE_CONSTANT:
        default:
             /* If red then don't reduce anything. */
            if (res_col == ARAD_PP_MTR_PCD_COL_RED) {
                reduce_col = ARAD_PP_MTR_PCD_COL_RED;
            /* Otherwise according to result color */
            } else {
                reduce_col = res_col;
            }
            break;
        }

        meter_infos[meter_idx].reduce_buck_col = reduce_col;
    }

    pcd_line = 0;

    soc_mem_field32_set(unit, mem, &pcd_line, MCDA_UPDATE_COLORf, 
                        meter_infos[ARAD_PP_MTR_PCD_PARALLEL_METER_A].reduce_buck_col);
    soc_mem_field32_set(unit, mem, &pcd_line, MCDB_UPDATE_COLORf, 
                        meter_infos[ARAD_PP_MTR_PCD_PARALLEL_METER_B].reduce_buck_col);
    soc_mem_field32_set(unit, mem, &pcd_line, OUT_COLORf, res_col);
    res = soc_mem_write(
            unit,
            mem,
            MEM_BLOCK_ANY,
            addr_idx, 
            &pcd_line
          ); 
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 3000, exit);
  }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_metering_pcd_init_parallel()", 0, 0);
}
#endif /* BCM_88660_A0 */

uint32
  arad_pp_metering_init_unsafe(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    reg_val,
    range_mode,
    reg_val_a,
    reg_val_b,
    reg_val_2;
  uint32
    eth_mtr_global_multiset_key = 0,
    eth_mtr_global_multiset_ndx = 0,
    eth_mtr_profile_multiset_key = 0,
    eth_mtr_profile_multiset_ndx = 0;
  uint8
    eth_mtr_global_multiset_first_appear,
    eth_mtr_profile_multiset_first_appear;
  SOC_SAND_SUCCESS_FAILURE
    eth_mtr_global_multiset_success = SOC_SAND_SUCCESS,
    eth_mtr_profile_multiset_success = SOC_SAND_SUCCESS;
  ARAD_IDR_GLOBAL_METER_PROFILES_TBL_DATA
    global_tbl_data;
  uint8
    range_mode_64k,
    color_blind_mode;
  arad_pp_mtr_two_meter_sharing_mode_t sharing_mode;
    
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_METERING_INIT_UNSAFE);

  res = arad_itm_setup_dp_map(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 2300, exit);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,READ_MRPS_GLBL_CFGr(unit, &reg_val));

  /* Init MCDs */
  soc_reg_field_set(unit, MRPS_GLBL_CFGr, &reg_val, MCDA_INITf, 0);
  soc_reg_field_set(unit, MRPS_GLBL_CFGr, &reg_val, MCDB_INITf, 0);

  /* Set MRPS Range mode (64K/32K) */
  range_mode = soc_property_get(unit, spn_POLICER_INGRESS_COUNT, 64);
  if ((range_mode != 64) && (range_mode != 32)) {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_METERING_POLICER_INGRESS_COUNT_INVALID_ERR, 20, exit);
  }
  range_mode_64k = (range_mode == 64 ? 1 : 0);
  soc_reg_field_set(unit, MRPS_GLBL_CFGr, &reg_val, MCQ_SIZE_SELf, range_mode_64k);

  /* If the soc property custom_feature_meter_pointer_0_enable is 1 and range_mode is 32k then */
  /* meter pointer 0 is NOT invalid (this is a "chicken" soc property). */
  if (SOC_IS_ARADPLUS_AND_BELOW(unit) 
      && (range_mode_64k || (soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "meter_pointer_0_enable", 0) != 1))) {
    /* Meter pointer 0 is always invalid. */
    res = READ_IDR_INVALID_METER_POINTERr(unit, &reg_val_2);
    SOC_SAND_CHECK_FUNC_RESULT(res, 23, exit);

    soc_reg_field_set(unit, IDR_INVALID_METER_POINTERr, &reg_val_2, INVALID_METER_POINTER_VALIDf, 1);
    soc_reg_field_set(unit, IDR_INVALID_METER_POINTERr, &reg_val_2, INVALID_METER_POINTERf, 0);

    res = WRITE_IDR_INVALID_METER_POINTERr(unit, reg_val_2);
    SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);
  }

  /* Set MCDs Sharing mode */
  sharing_mode = soc_property_get(unit, spn_POLICER_INGRESS_SHARING_MODE, 0);
  if ((range_mode_64k && (sharing_mode != ARAD_PP_MTR_TWO_METER_SHARING_MODE_NONE)) || 
      (!range_mode_64k && ((sharing_mode != ARAD_PP_MTR_TWO_METER_SHARING_MODE_SERIAL) && (sharing_mode != ARAD_PP_MTR_TWO_METER_SHARING_MODE_PARALLEL)))) {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_METERING_POLICER_INGRESS_SHARING_MODE_INVALID_ERR, 30, exit);
  }
  if (range_mode == 32) { /* sharing_mode is only relevent in 32K range_mode */
      soc_reg_field_set(unit, MRPS_GLBL_CFGr, &reg_val, MCQ_MCDS_PARALLELf, sharing_mode == ARAD_PP_MTR_TWO_METER_SHARING_MODE_PARALLEL ? 1 : 0);
  }

  /* Set Hierarchical Mode */
  soc_reg_field_set(unit, MRPS_GLBL_CFGr, &reg_val, HIERARCHICAL_MODEf, 0);

  /* Set the FTMH DP source to always be the output of the DP map. */
  /* We assume that when the meter is invalid, the meter DP will be the In-DP. */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  0,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FTMH_DP_OVERWRITEf,  1));

#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit)) {
      /* Set color blind mode. */
      /* NOTE: This mode has become redundant after the introduction of the COLOR_BLIND flag to the rate module and the global ethernet policers. */
      color_blind_mode = soc_property_get(unit, spn_RATE_COLOR_BLIND, 0) ? 1 : 0;
      
      /* EthernetPolicerColorBlindEn */
      /* Should always be set (allows using color blind mode). */
	  if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  __LINE__,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IDR_COMPATIBILITY_REGISTERr, REG_PORT_ANY, 0, SYS_CONFIG_6f, color_blind_mode ? 1 : 0));
      }

      if (color_blind_mode == 1) {
        uint32 profile_data;
        uint32 profile_idx;

        /* In color blind mode all ethernet policer are set to be color blind. */
        for (profile_idx = 0; profile_idx < ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_PROFILE_METER_PROFILE_NOF_MEMBER; profile_idx++) {
          res = READ_IDR_ETHERNET_METER_PROFILESm(unit, MEM_BLOCK_ANY, profile_idx, &profile_data);
          SOC_SAND_CHECK_FUNC_RESULT(res, 576, exit);

          soc_mem_field32_set(unit, IDR_ETHERNET_METER_PROFILESm, &profile_data, COLOR_BLINDf, 1);

          res = WRITE_IDR_ETHERNET_METER_PROFILESm(unit, MEM_BLOCK_ANY, profile_idx, &profile_data);
          SOC_SAND_CHECK_FUNC_RESULT(res, 577, exit);
        }

        /* Additionally we set the color blind bit for the global profiles. */
        for (profile_idx = 0; profile_idx < ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_GLOBAL_METER_PROFILE_NOF_MEMBER; profile_idx++) {
          
          res = READ_IDR_GLOBAL_METER_PROFILESm(unit, MEM_BLOCK_ANY, profile_idx, &profile_data);
          SOC_SAND_CHECK_FUNC_RESULT(res, 578, exit);

          soc_mem_field32_set(unit, IDR_GLOBAL_METER_PROFILESm, &profile_data, COLOR_BLINDf, 1);
          
          res = WRITE_IDR_GLOBAL_METER_PROFILESm(unit, MEM_BLOCK_ANY, profile_idx, &profile_data);
          SOC_SAND_CHECK_FUNC_RESULT(res, 579, exit);
        }
      }

      /* MrpsDpOverrideEn */
      if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  __LINE__,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IDR_SPARE_REGISTER_2r, REG_PORT_ANY, 0, SYS_CONFIG_3f,  soc_property_get(unit, spn_POLICER_COLOR_RESOLUTION_MODE, 0) ? 1 : 0));
      }

      /* Set PacketModeEn in Arad+ to allow for meters with packet/sec rates */
      soc_reg_field_set(unit, MRPS_GLBL_CFGr, &reg_val, PACKET_MODE_ENf, 1);

	  /* Enable the PCD map for parallel mode and serial mode. */
      if ((range_mode == 32) && (sharing_mode != ARAD_PP_MTR_TWO_METER_SHARING_MODE_NONE)){
        soc_reg_field_set(unit, MRPS_GLBL_CFGr, &reg_val, PCD_MAP_ENf, 1);
      }
  }
#endif /* BCM_88660_A0 */

  /* Set */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,WRITE_MRPS_GLBL_CFGr(unit,  reg_val));

  reg_val_a = 0;
  reg_val_b = 0;

  /* Enable leaky buckets Update */
  soc_reg_field_set(unit, MRPS_MCDA_CFGr, &reg_val_a, MCDA_TIMER_ENf, 1);
  soc_reg_field_set(unit, MRPS_MCDB_CFGr, &reg_val_b, MCDB_TIMER_ENf, 1);

  /* Enable leaky buckets Refresh */
  soc_reg_field_set(unit, MRPS_MCDA_CFGr, &reg_val_a, MCDA_REFRESH_ENf, 1);
  soc_reg_field_set(unit, MRPS_MCDB_CFGr, &reg_val_b, MCDB_REFRESH_ENf, 1);

  /* Enable LFR */
  soc_reg_field_set(unit, MRPS_MCDA_CFGr, &reg_val_a, MCDA_CBL_RND_MODE_ENf, 1);
  soc_reg_field_set(unit, MRPS_MCDB_CFGr, &reg_val_b, MCDB_CBL_RND_MODE_ENf, 1);

  /* Set default values to the rest */
  soc_reg_field_set(unit, MRPS_MCDA_CFGr, &reg_val_a, MCDA_ERR_COMP_ENABLEf, 1);
  soc_reg_field_set(unit, MRPS_MCDB_CFGr, &reg_val_b, MCDB_ERR_COMP_ENABLEf, 1);

  soc_reg_field_set(unit, MRPS_MCDA_CFGr, &reg_val_a, MCDA_RND_RANGEf, 3);
  soc_reg_field_set(unit, MRPS_MCDB_CFGr, &reg_val_b, MCDB_RND_RANGEf, 3);

  soc_reg_field_set(unit, MRPS_MCDA_CFGr, &reg_val_a, MCDA_BUBBLE_RATEf, 0xff);
  soc_reg_field_set(unit, MRPS_MCDB_CFGr, &reg_val_b, MCDB_BUBBLE_RATEf, 0xff);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,WRITE_MRPS_MCDA_CFGr(unit,  reg_val_a));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,WRITE_MRPS_MCDB_CFGr(unit,  reg_val_b));

  reg_val_a = 0;
  reg_val_b = 0;

  /* Set default values for refresh quartet index range */
  soc_reg_field_set(unit, MRPS_MCDA_REFRESH_CFGr, &reg_val_a, MCDA_REFRESH_START_INDEXf, 0);
  soc_reg_field_set(unit, MRPS_MCDB_REFRESH_CFGr, &reg_val_b, MCDB_REFRESH_START_INDEXf, 0);

  soc_reg_field_set(unit, MRPS_MCDA_REFRESH_CFGr, &reg_val_a, MCDA_REFRESH_END_INDEXf, 0x1fff);
  soc_reg_field_set(unit, MRPS_MCDB_REFRESH_CFGr, &reg_val_b, MCDB_REFRESH_END_INDEXf, 0x1fff);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,WRITE_MRPS_MCDA_REFRESH_CFGr(unit,  reg_val_a));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,WRITE_MRPS_MCDB_REFRESH_CFGr(unit,  reg_val_b));

#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit)) {
      res = arad_pp_metering_pcd_init(unit, sharing_mode);
      SOC_SAND_CHECK_FUNC_RESULT(res, 888, exit);
	
  }
#endif /* BCM_88660_A0 */

  res = arad_sw_db_multiset_add(
        unit,
        ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_ETH_METER_PROFILE,
        &eth_mtr_profile_multiset_key,
        &eth_mtr_profile_multiset_ndx,
        &eth_mtr_profile_multiset_first_appear,
        &eth_mtr_profile_multiset_success
      );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

   /* Add multiset 0 to eth policer global table */
  ARAD_CLEAR(&global_tbl_data, ARAD_IDR_GLOBAL_METER_PROFILES_TBL_DATA, 1);

  arad_pp_mtr_eth_policer_global_key_get(
    unit,
    global_tbl_data,
    &eth_mtr_global_multiset_key
  );

  res = arad_sw_db_multiset_add(
          unit,
          ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_GLOBAL_METER_PROFILE,
          &eth_mtr_global_multiset_key,
          &eth_mtr_global_multiset_ndx,
          &eth_mtr_global_multiset_first_appear,
          &eth_mtr_global_multiset_success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);
     
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_metering_init_unsafe()", 0, 0);
}


/*********************************************************************
*     Sets
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mtr_meters_group_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 mtr_group_ndx,
    SOC_SAND_IN  ARAD_PP_MTR_GROUP_INFO                 *mtr_group_info
  )
{
  uint32
    res = SOC_SAND_OK;
  soc_reg_t 
    reg;
  soc_field_t
    field_rnd_mode_en,
    field_rnd_range;
  uint32
    reg_val,
    enable;
   
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_METERS_GROUP_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mtr_group_info);

  if (mtr_group_ndx == 0)
  {
    reg = MRPS_MCDA_CFGr;
    field_rnd_mode_en = MCDA_CBL_RND_MODE_ENf;
    field_rnd_range = MCDA_RND_RANGEf;
  }
  else /* if (mtr_group_ndx == 1) */
  {
    reg = MRPS_MCDB_CFGr;
    field_rnd_mode_en = MCDB_CBL_RND_MODE_ENf;
    field_rnd_range = MCDB_RND_RANGEf;
  }

  res = soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  enable =  SOC_SAND_BOOL2NUM(mtr_group_info->rand_mode_enable);
  soc_reg_field_set(unit, reg, &reg_val, field_rnd_mode_en, enable);

  if (enable) {
    /* set the range of random values add/substract to the bucket level as the highest posssible range (-8K - 8K) */
    soc_reg_field_set(unit, reg, &reg_val, field_rnd_range, 0);
  }

  res = soc_reg32_set(unit, reg, REG_PORT_ANY, 0, reg_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit)) {
    /* IPG compensation set for both groups. */
    res = soc_reg_field32_modify(unit, IDR_SPARE_REGISTER_2r, REG_PORT_ANY, IPG_COMPENSATION_ENf,  mtr_group_info->is_ipg_compensation_enabled);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }
#endif /* BCM_88660_A0*/
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_meters_group_info_set_unsafe()", mtr_group_ndx, 0);
}

uint32
  arad_pp_mtr_meters_group_info_set_verify(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  ARAD_PP_MTR_GROUP_INFO                  *mtr_group_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_METERS_GROUP_INFO_SET_VERIFY);
 
  SOC_SAND_ERR_IF_ABOVE_MAX(mtr_group_ndx, ARAD_PP_METERING_MTR_GROUP_NDX_MAX, ARAD_PP_METERING_MTR_GROUP_NDX_OUT_OF_RANGE_ERR, 10, exit);
  ARAD_PP_STRUCT_VERIFY(ARAD_PP_MTR_GROUP_INFO, mtr_group_info, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_meters_group_info_set_verify()", mtr_group_ndx, 0);
}

uint32
  arad_pp_mtr_meters_group_info_get_verify(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint32                                  mtr_group_ndx
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_METERS_GROUP_INFO_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(mtr_group_ndx, ARAD_PP_METERING_MTR_GROUP_NDX_MAX, ARAD_PP_METERING_MTR_GROUP_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_meters_group_info_get_verify()", mtr_group_ndx, 0);
}

/*********************************************************************
*     Sets
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mtr_meters_group_info_get_unsafe(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_OUT ARAD_PP_MTR_GROUP_INFO                  *mtr_group_info
  )
{
  uint32
    res = SOC_SAND_OK;
  soc_reg_t 
    reg;
  soc_field_t
    field_rnd_mode_en;
  uint32
    reg_val,
    enable;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_METERS_GROUP_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mtr_group_info);

  ARAD_PP_MTR_GROUP_INFO_clear(mtr_group_info);
  
  if (mtr_group_ndx == 0)
  {
    reg = MRPS_MCDA_CFGr;
    field_rnd_mode_en = MCDA_CBL_RND_MODE_ENf;
  }
  else /* if (mtr_group_ndx == 1) */
  {
    reg = MRPS_MCDB_CFGr;
    field_rnd_mode_en = MCDB_CBL_RND_MODE_ENf;
  }

  res = soc_reg32_get(unit, reg, REG_PORT_ANY, 0, &reg_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  enable = soc_reg_field_get(unit, reg, reg_val, field_rnd_mode_en);
  mtr_group_info->rand_mode_enable = SOC_SAND_NUM2BOOL(enable);

  mtr_group_info->is_ipg_compensation_enabled = 0;

#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit)) {
    /* IPG compensation is the same for both groups. */
    res = soc_reg32_get(unit, IDR_SPARE_REGISTER_2r, REG_PORT_ANY, 0, &reg_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    mtr_group_info->is_ipg_compensation_enabled = soc_reg_field_get(unit, IDR_SPARE_REGISTER_2r, reg_val, IPG_COMPENSATION_ENf);
  }
#endif /* BCM_88660_A0*/

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_meters_group_info_get_unsafe()", mtr_group_ndx, 0);
}

/*********************************************************************
*     Add Bandwidth Profile and set it attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mtr_bw_profile_add_unsafe(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_ndx,
    SOC_SAND_IN  ARAD_PP_MTR_BW_PROFILE_INFO             *bw_profile_info,
    SOC_SAND_OUT ARAD_PP_MTR_BW_PROFILE_INFO             *exact_bw_profile_info,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                *success
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_IDR_PRFCFG_TBL_DATA
    prfcfg_tbl_data;
  uint32
    clk_freq;
  soc_reg_above_64_val_t
    reg_above64_val;
  soc_mem_t 
    format_mem,
    mem;
  uint32
    entry_offset = bw_profile_ndx;
  uint32
    max_eir_for_calc,
    max_cir_for_calc;
  /* IDR_PRFCFG fields that exists only in shr_en configuration */
  uint32
    max_ir_val = 0,
    min_ir_val = 0,
    ir_reverse_exponent_2,
    max_cir_mantissa_exponent,
    max_cir_mantissa = 127;
  /* IDR_PRFCFG fields that exists only in shr_dis configuration */
  uint32
    cir_reverse_exponent_2,
    eir_reverse_exponent_2;
  uint32 
    rates[ARAD_PP_BW_PROFILE_MAX_RATES_FOR_CALC], 
    nof_rates = -1;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_BW_PROFILE_ADD_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(bw_profile_info);
  SOC_SAND_CHECK_NULL_INPUT(exact_bw_profile_info);
  SOC_SAND_CHECK_NULL_INPUT(success);

  ARAD_CLEAR(&prfcfg_tbl_data, ARAD_IDR_PRFCFG_TBL_DATA, 1);
  SOC_REG_ABOVE_64_CLEAR(reg_above64_val);

  clk_freq = arad_chip_ticks_per_sec_get(unit);

  if (SOC_IS_JERICHO(unit)) {
      goto exit;
  }

  if (bw_profile_info->max_eir == ARAD_PP_BW_PROFILE_IR_MAX_UNLIMITED) { /* max eir unlimited */
      /* max_eir is not used in calculations */
      max_eir_for_calc = bw_profile_info->eir;

      /* -1 is written to HW */
      prfcfg_tbl_data.max_eir_mantissa = ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_IR;
      prfcfg_tbl_data.max_eir_mantissa_exponent = ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_EXP;
  }
  else {
      max_eir_for_calc = bw_profile_info->max_eir;
  }
    
  /* transfer user values (cir, eir, cbs, ebs) to mant and exp */
  if (bw_profile_info->is_sharing_enabled) {

      /* mem = MRPS_MCDA_PRFCFG_SHARING_ENm is for shr_en format */
      format_mem = MRPS_MCDA_PRFCFG_SHARING_ENm;

      if (bw_profile_info->max_cir == ARAD_PP_BW_PROFILE_IR_MAX_UNLIMITED) { /* max cir unlimited */
          /* max_cir is not used in calculations */
          max_cir_for_calc = bw_profile_info->cir;

          /* -1 is written to HW */
          max_cir_mantissa = ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_IR;
          max_cir_mantissa_exponent = ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_EXP;
      }
      else {
          max_cir_for_calc = bw_profile_info->max_cir;
      }

      /* find min_val and max_val of: CIR, EIR, max CIR and max EIR */
      if (bw_profile_info->disable_cir && bw_profile_info->disable_eir) {
          /* both are disabled, no need to calculate anything */
          ir_reverse_exponent_2 = 0;
          max_cir_mantissa = 0;
          max_cir_mantissa_exponent = 0;
      }
      else {
          if (bw_profile_info->disable_cir) {
              /* cir and max-cir not used in calculations */
              rates[0] = bw_profile_info->eir;
              rates[1] = max_eir_for_calc;
              nof_rates = 2;
          }
          else if (bw_profile_info->disable_eir) {
              /* eir not used in calculations */
              rates[0] = bw_profile_info->cir;
              rates[1] = max_cir_for_calc;
              nof_rates = 2;
              if (max_eir_for_calc > ARAD_PP_BW_PROFILE_ADD_VERIFY_IR_MIN) {
                  /* use max-eir */
                  rates[2] = max_eir_for_calc;
                  nof_rates = 3;
              }
          }
          else { /* use all */
              rates[0] = bw_profile_info->cir;
              rates[1] = max_cir_for_calc;
              rates[2] = bw_profile_info->eir;
              rates[3] = max_eir_for_calc;
              nof_rates = 4;
          }
          if ((bw_profile_info->hierarchical_max_rate != 0) && (bw_profile_info->hierarchical_min_rate != 0)) {
              /* global sharing (between 4 meters) is set, so rev-exp is shared between all 4
                 the 3 other meters' max/min CIR/EIR must be taken into account */
              rates[nof_rates] = bw_profile_info->hierarchical_max_rate;
              rates[nof_rates+1] = bw_profile_info->hierarchical_min_rate;
              nof_rates += 2;

          }
          arad_pp_mtr_max_min_vals_for_calc_get(rates, nof_rates, &min_ir_val, &max_ir_val);

          /* Calculate the best rev_exp (with respect to both rate accuracy and bucket update) for: CIR, EIR, max CIR and max EIR */
          res = arad_pp_mtr_ir_val_to_max_rev_exp_optimized_for_bucket_update_rate(
              unit,
              rates,
              nof_rates,
              &ir_reverse_exponent_2
            );
          SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit); 

          /* calculate min exp and max mnt for: CIR, EIR, max CIR and max EIR, based on shared rev_exp */
          res = soc_sand_break_complex_to_mnt_exp_round_down_2(
              bw_profile_info->cir,
              ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_IR,
              ARAD_PP_MTR_VAL_TO_EXP_MNT_MIN_MNT_IR,
              ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_EXP,
              clk_freq,
              (ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_DIV_IR << (ir_reverse_exponent_2 + 2)),
              ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC_IR, 
              &prfcfg_tbl_data.cir_mantissa,
              &prfcfg_tbl_data.cir_mantissa_exponent
            );
          SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

          res = soc_sand_break_complex_to_mnt_exp_round_down_2(
              bw_profile_info->eir,
              ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_IR,
              ARAD_PP_MTR_VAL_TO_EXP_MNT_MIN_MNT_IR,
              ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_EXP,
              clk_freq,
              (ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_DIV_IR << (ir_reverse_exponent_2 + 2)),
              ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC_IR, 
              &prfcfg_tbl_data.eir_mantissa,
              &prfcfg_tbl_data.eir_mantissa_exponent
            );
          SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);

          if (bw_profile_info->max_cir != ARAD_PP_BW_PROFILE_IR_MAX_UNLIMITED) { /* not max cir unlimited */
              res = soc_sand_break_complex_to_mnt_exp_round_down_2(
                  bw_profile_info->max_cir,
                  ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_IR,
                  ARAD_PP_MTR_VAL_TO_EXP_MNT_MIN_MNT_IR,
                  ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_EXP,
                  clk_freq,
                  (ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_DIV_IR << (ir_reverse_exponent_2 + 2)),
                  ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC_IR, 
                  &max_cir_mantissa,
                  &max_cir_mantissa_exponent
                  );
              SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);
          }

          if (bw_profile_info->max_eir != ARAD_PP_BW_PROFILE_IR_MAX_UNLIMITED) { /* not max eir unlimited */
              res = soc_sand_break_complex_to_mnt_exp_round_down_2(
                  bw_profile_info->max_eir,
                  ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_IR,
                  ARAD_PP_MTR_VAL_TO_EXP_MNT_MIN_MNT_IR,
                  ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_EXP,
                  clk_freq,
                  (ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_DIV_IR << (ir_reverse_exponent_2 + 2)),
                  ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC_IR, 
                  &prfcfg_tbl_data.max_eir_mantissa,
                  &prfcfg_tbl_data.max_eir_mantissa_exponent
                  );
              SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
          }
      }

      /* prepare data to write to HW */
      soc_mem_field32_set(unit, format_mem, (void*)reg_above64_val, IR_REV_EXP_2f, ir_reverse_exponent_2);
      soc_mem_field32_set(unit, format_mem, (void*)reg_above64_val, MAX_CIR_MANTf, max_cir_mantissa);
      soc_mem_field32_set(unit, format_mem, (void*)reg_above64_val, MAX_CIR_MANT_EXPf, max_cir_mantissa_exponent);
  }
  else {

      /* mem = MRPS_MCDA_PRFCFG_SHARING_DISm is for shr_dis format */
      format_mem = MRPS_MCDA_PRFCFG_SHARING_DISm;

      /* if coupling is set: eir_rev_exp must be equal to cir_rev_exp
         because Committed bucket share its credits with the Excess bucket
         otherwise: cir-rev-exp and eir-rev-exp are calculated seperatly */
      if (bw_profile_info->is_coupling_enabled) {

        /* find min_val and max_val of: CIR, EIR and max EIR (no max CIR) */
        if (bw_profile_info->disable_cir && bw_profile_info->disable_eir) {
            /* both are disabled, no need to calculate anything */
            cir_reverse_exponent_2 = 0;
            eir_reverse_exponent_2 = 0;
        }
        else {
          if (bw_profile_info->disable_cir) {
              /* cir not used in calculations */
              rates[0] = bw_profile_info->eir;
              rates[1] = max_eir_for_calc;
              nof_rates = 2;
          }
          else if (bw_profile_info->disable_eir) {
              /* eir not used in calculations */
              rates[0] = bw_profile_info->cir;
              nof_rates = 1;
              if (max_eir_for_calc > ARAD_PP_BW_PROFILE_ADD_VERIFY_IR_MIN) {
                  rates[1] = max_eir_for_calc;
                  nof_rates = 2;
              }
          }
          else { /* use all */
              rates[0] = bw_profile_info->eir;
              rates[1] = max_eir_for_calc;
              rates[2] = bw_profile_info->cir;
              nof_rates = 3;
          }
          arad_pp_mtr_max_min_vals_for_calc_get(rates, nof_rates, &min_ir_val, &max_ir_val);

          /* Calculate the best rev_exp (with respect to both rate accuracy and bucket update) for: CIR, EIR and max EIR */
          res = arad_pp_mtr_ir_val_to_max_rev_exp_optimized_for_bucket_update_rate(
            unit,
            rates,
            nof_rates,
            &eir_reverse_exponent_2
            );
          SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);

          cir_reverse_exponent_2 = eir_reverse_exponent_2;
        }
      }
      else { /* no coupling */

        rates[0] = bw_profile_info->cir;
        nof_rates = 1;

        /* Calculate the best rev_exp (with respect to both rate accuracy and bucket update) for:
         * 1) CIR 
         * 2) EIR and max EIR 
         */
        res = arad_pp_mtr_ir_val_to_max_rev_exp_optimized_for_bucket_update_rate(
            unit,
            rates,
            nof_rates,
            &cir_reverse_exponent_2
          );
        SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);

        rates[0] = bw_profile_info->eir;
        rates[1] = max_eir_for_calc;
        nof_rates = 2;

        res = arad_pp_mtr_ir_val_to_max_rev_exp_optimized_for_bucket_update_rate(
            unit,
            rates,
            nof_rates,
            &eir_reverse_exponent_2
          );
        SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);
      }

      /* calculate min exp and max mnt for: CIR, EIR and max EIR, based on their rev_exps */
      res = soc_sand_break_complex_to_mnt_exp_round_down_2(
          bw_profile_info->cir,
          ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_IR,
          ARAD_PP_MTR_VAL_TO_EXP_MNT_MIN_MNT_IR,
          ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_EXP,
          clk_freq,
          (ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_DIV_IR << (cir_reverse_exponent_2 + 2)),
          ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC_IR, 
          &prfcfg_tbl_data.cir_mantissa,
          &prfcfg_tbl_data.cir_mantissa_exponent
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 18, exit);

      res = soc_sand_break_complex_to_mnt_exp_round_down_2(
          bw_profile_info->eir,
          ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_IR,
          ARAD_PP_MTR_VAL_TO_EXP_MNT_MIN_MNT_IR,
          ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_EXP,
          clk_freq,
          (ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_DIV_IR << (eir_reverse_exponent_2 + 2)),
          ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC_IR, 
          &prfcfg_tbl_data.eir_mantissa,
          &prfcfg_tbl_data.eir_mantissa_exponent
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 19, exit);

      if (bw_profile_info->max_eir != ARAD_PP_BW_PROFILE_IR_MAX_UNLIMITED) { /* not max eir unlimited */
          res = soc_sand_break_complex_to_mnt_exp_round_down_2(
              bw_profile_info->max_eir,
              ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_IR,
              ARAD_PP_MTR_VAL_TO_EXP_MNT_MIN_MNT_IR,
              ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_EXP,
              clk_freq,
              (ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_DIV_IR << (eir_reverse_exponent_2 + 2)),
              ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC_IR, 
              &prfcfg_tbl_data.max_eir_mantissa,
              &prfcfg_tbl_data.max_eir_mantissa_exponent
              );
          SOC_SAND_CHECK_FUNC_RESULT(res, 21, exit);
      }

      /* prepare data to write to HW */
      soc_mem_field32_set(unit, format_mem, (void*)reg_above64_val, CIR_REV_EXP_2f, cir_reverse_exponent_2);
      soc_mem_field32_set(unit, format_mem, (void*)reg_above64_val, EIR_REV_EXP_2f, eir_reverse_exponent_2);
  }

  /* trasnfer user values that exist both in shr_en and shr_dis configurations */
  res = arad_pp_mtr_bs_val_to_exp_mnt(
      unit,
      bw_profile_info->cbs,
      &prfcfg_tbl_data.cbs_exponent,
      &prfcfg_tbl_data.cbs_mantissa_64
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

  res = arad_pp_mtr_bs_val_to_exp_mnt(
      unit,
      bw_profile_info->ebs,
      &prfcfg_tbl_data.ebs_exponent,
      &prfcfg_tbl_data.ebs_mantissa_64
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 23, exit);

  prfcfg_tbl_data.color_aware   = bw_profile_info->color_mode == ARAD_PP_MTR_COLOR_MODE_AWARE ? 0x1 : 0x0;
  prfcfg_tbl_data.coupling_flag = SOC_SAND_BOOL2NUM(bw_profile_info->is_coupling_enabled);
  prfcfg_tbl_data.reset_cir = SOC_SAND_BOOL2NUM(bw_profile_info->disable_cir);
  prfcfg_tbl_data.reset_eir = SOC_SAND_BOOL2NUM(bw_profile_info->disable_eir);
#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit)) {
    prfcfg_tbl_data.packet_mode = bw_profile_info->is_packet_mode;
  }
#endif /* BCM_88660_A0 */

  /* prepare data to write to HW */
  soc_mem_field32_set(unit, format_mem, (void*)reg_above64_val, SHARING_FLAGf, bw_profile_info->is_sharing_enabled);
  soc_mem_field32_set(unit, format_mem, (void*)reg_above64_val, COLOR_AWAREf, prfcfg_tbl_data.color_aware);
#ifdef BCM_88660_A0
  /* In Arad+, coupling-flag available only when sharing is disabled */
  if (SOC_IS_ARADPLUS(unit)) {
      if (!bw_profile_info->is_sharing_enabled) {
          soc_mem_field32_set(unit, format_mem, (void*)reg_above64_val, COUPLING_FLAGf, prfcfg_tbl_data.coupling_flag);
      }
  }
  else
#endif /* BCM_88660_A0 */
  {
      soc_mem_field32_set(unit, format_mem, (void*)reg_above64_val, COUPLING_FLAGf, prfcfg_tbl_data.coupling_flag);
  }
  soc_mem_field32_set(unit, format_mem, (void*)reg_above64_val, RESET_CIRf, prfcfg_tbl_data.reset_cir);
  soc_mem_field32_set(unit, format_mem, (void*)reg_above64_val, RESET_EIRf, prfcfg_tbl_data.reset_eir);
  soc_mem_field32_set(unit, format_mem, (void*)reg_above64_val, CBS_MANT_64f, prfcfg_tbl_data.cbs_mantissa_64);
  soc_mem_field32_set(unit, format_mem, (void*)reg_above64_val, CBS_EXPONENTf, prfcfg_tbl_data.cbs_exponent);
  soc_mem_field32_set(unit, format_mem, (void*)reg_above64_val, EBS_MANT_64f, prfcfg_tbl_data.ebs_mantissa_64);
  /*soc_mem_field32_set(unit, format_mem, (void*)reg_above64_val, EBS_EXPONENTf, prfcfg_tbl_data.ebs_exponent);*/
  soc_sand_bitstream_set_any_field(&(prfcfg_tbl_data.ebs_exponent), 21, 4, &(reg_above64_val[0]));
  soc_mem_field32_set(unit, format_mem, (void*)reg_above64_val, CIR_MANTf, prfcfg_tbl_data.cir_mantissa);
  soc_mem_field32_set(unit, format_mem, (void*)reg_above64_val, CIR_MANT_EXPf, prfcfg_tbl_data.cir_mantissa_exponent);
  soc_mem_field32_set(unit, format_mem, (void*)reg_above64_val, EIR_MANTf, prfcfg_tbl_data.eir_mantissa);
  soc_mem_field32_set(unit, format_mem, (void*)reg_above64_val, EIR_MANT_EXPf, prfcfg_tbl_data.eir_mantissa_exponent);
  soc_mem_field32_set(unit, format_mem, (void*)reg_above64_val, MAX_EIR_MANTf, prfcfg_tbl_data.max_eir_mantissa);
  soc_mem_field32_set(unit, format_mem, (void*)reg_above64_val, MAX_EIR_MANT_EXPf, prfcfg_tbl_data.max_eir_mantissa_exponent);
#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit)) {
    soc_mem_field32_set(unit, format_mem, (void*)reg_above64_val, PACKET_MODEf, prfcfg_tbl_data.packet_mode);
  }
#endif /* BCM_88660_A0*/

  /* check which mem to config */
  if (mtr_group_ndx == 0) { /* group A */
      mem = IDR_MCDA_PRFCFG_0m;
  } else { /* group B */
      mem = IDR_MCDB_PRFCFG_0m;
  }

  /* write calculated values to HW */
  res = soc_mem_write(unit, mem, MEM_BLOCK_ANY, entry_offset, reg_above64_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 24, exit);

  /* Read values for Bandwidth profile attributes as written to the device */
  res = arad_pp_mtr_bw_profile_get_unsafe(
    unit,
    mtr_group_ndx,
    bw_profile_ndx,
    exact_bw_profile_info
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);
  
  *success = SOC_SAND_SUCCESS;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_bw_profile_add_unsafe()", mtr_group_ndx, bw_profile_ndx);
}

uint32
  arad_pp_mtr_bw_profile_add_verify(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_ndx,
    SOC_SAND_IN  ARAD_PP_MTR_BW_PROFILE_INFO             *bw_profile_info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_BW_PROFILE_ADD_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(mtr_group_ndx, ARAD_PP_METERING_MTR_GROUP_NDX_MAX, ARAD_PP_METERING_MTR_GROUP_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(bw_profile_ndx, ARAD_PP_METERING_BW_PROFILE_NDX_MAX, ARAD_PP_METERING_BW_PROFILE_NDX_OUT_OF_RANGE_ERR, 20, exit);

  res = ARAD_PP_MTR_BW_PROFILE_INFO_verify(unit, bw_profile_info);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_bw_profile_add_verify()", mtr_group_ndx, bw_profile_ndx);
}

/*********************************************************************
*     Get Bandwidth Profile attributes
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mtr_bw_profile_get_unsafe(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_ndx,
    SOC_SAND_OUT ARAD_PP_MTR_BW_PROFILE_INFO             *bw_profile_info
  )
{
  uint32
    res = SOC_SAND_OK;
  soc_reg_above_64_val_t
    reg_above64_val;
  soc_mem_t
    format_mem, 
    mem;
  uint32
    entry_offset = bw_profile_ndx;
  ARAD_IDR_PRFCFG_TBL_DATA
    prfcfg_tbl_data;
  uint32
    cir_reverse_exponent_2,
    eir_reverse_exponent_2;
  /* IDR_PRFCFG fields that exists only in shr_en configuration */
  uint32
    max_cir_mantissa_exponent = 0,
    max_cir_mantissa = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_BW_PROFILE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(bw_profile_info);

  ARAD_PP_MTR_BW_PROFILE_INFO_clear(bw_profile_info);
  ARAD_CLEAR(&prfcfg_tbl_data, ARAD_IDR_PRFCFG_TBL_DATA, 1);
  SOC_REG_ABOVE_64_CLEAR(reg_above64_val);

  /* check which mem to read from */
  if (mtr_group_ndx == 0) { /* group A */
      mem = IDR_MCDA_PRFCFG_0m;
  } else { /* group B */
      mem = IDR_MCDB_PRFCFG_0m;
  }
 
  /* Read calculated values from HW */
  res = soc_mem_read(unit, mem, MEM_BLOCK_ANY, entry_offset, (void*)reg_above64_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* check if sharing is enabled ot disabled
   * (same field location in MRPS_MCDA_PRFCFG_SHARING_ENm and MRPS_MCDA_PRFCFG_SHARING_DISm)
   */
  bw_profile_info->is_sharing_enabled = soc_mem_field32_get(unit, MRPS_MCDA_PRFCFG_SHARING_ENm, (void*)reg_above64_val, SHARING_FLAGf);

  if (bw_profile_info->is_sharing_enabled) {

      /* mem = MRPS_MCDA_PRFCFG_SHARING_ENm is for shr_en format */
      format_mem = MRPS_MCDA_PRFCFG_SHARING_ENm;

      max_cir_mantissa = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, MAX_CIR_MANTf);
      max_cir_mantissa_exponent = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, MAX_CIR_MANT_EXPf);
      cir_reverse_exponent_2 = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, IR_REV_EXP_2f);
      eir_reverse_exponent_2 = cir_reverse_exponent_2; /* rev_exp is shared between CIR and EIR in shr_en mode */
  }
  else {

      /* mem = MRPS_MCDA_PRFCFG_SHARING_DISm is for shr_dis format */
      format_mem = MRPS_MCDA_PRFCFG_SHARING_DISm;

      cir_reverse_exponent_2 = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, CIR_REV_EXP_2f);
      eir_reverse_exponent_2 = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, EIR_REV_EXP_2f);
  }

  prfcfg_tbl_data.color_aware = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, COLOR_AWAREf);
#ifdef BCM_88660_A0
  /* In Arad+, coupling-flag available only when sharing is disabled */
  if (SOC_IS_ARADPLUS(unit)) {
      if (!bw_profile_info->is_sharing_enabled) {
          prfcfg_tbl_data.coupling_flag = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, COUPLING_FLAGf);
      }
  }
  else
#endif /* BCM_88660_A0 */
  {
      prfcfg_tbl_data.coupling_flag = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, COUPLING_FLAGf);
  }
  prfcfg_tbl_data.reset_cir = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, RESET_CIRf);
  prfcfg_tbl_data.reset_eir = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, RESET_EIRf);
  prfcfg_tbl_data.cbs_mantissa_64 = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, CBS_MANT_64f);
  prfcfg_tbl_data.cbs_exponent = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, CBS_EXPONENTf);
  prfcfg_tbl_data.ebs_mantissa_64 = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, EBS_MANT_64f);
  /*prfcfg_tbl_data.ebs_exponent = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, EBS_EXPONENTf);*/
  soc_sand_bitstream_get_any_field(&(reg_above64_val[0]), 21, 4, &(prfcfg_tbl_data.ebs_exponent));
  prfcfg_tbl_data.cir_mantissa = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, CIR_MANTf);
  prfcfg_tbl_data.cir_mantissa_exponent = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, CIR_MANT_EXPf);
  prfcfg_tbl_data.eir_mantissa = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, EIR_MANTf);
  prfcfg_tbl_data.eir_mantissa_exponent = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, EIR_MANT_EXPf);
  prfcfg_tbl_data.max_eir_mantissa = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, MAX_EIR_MANTf);
  prfcfg_tbl_data.max_eir_mantissa_exponent = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, MAX_EIR_MANT_EXPf);
#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit)) {
    prfcfg_tbl_data.packet_mode = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, PACKET_MODEf);
  }
#endif /* BCM_88660_A0 */

  /* Transfer mnt, mnt_exp and rev_exp to ir values */
  res = arad_pp_mtr_ir_val_from_reverse_exp_mnt(
      unit,
      cir_reverse_exponent_2,
      prfcfg_tbl_data.cir_mantissa_exponent,
      prfcfg_tbl_data.cir_mantissa,
      &bw_profile_info->cir
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = arad_pp_mtr_ir_val_from_reverse_exp_mnt(
      unit,
      eir_reverse_exponent_2,
      prfcfg_tbl_data.eir_mantissa_exponent,
      prfcfg_tbl_data.eir_mantissa,
      &bw_profile_info->eir
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  if ((prfcfg_tbl_data.max_eir_mantissa_exponent == ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_EXP) && 
      (prfcfg_tbl_data.max_eir_mantissa == ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_IR)) { /* unlimited max rate */
      bw_profile_info->max_eir = 0xffffffff;
  }
  else {
      res = arad_pp_mtr_ir_val_from_reverse_exp_mnt(
          unit,
          eir_reverse_exponent_2,
          prfcfg_tbl_data.max_eir_mantissa_exponent,
          prfcfg_tbl_data.max_eir_mantissa,
          &bw_profile_info->max_eir
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

  if (bw_profile_info->is_sharing_enabled) {
      if ((max_cir_mantissa_exponent == ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_EXP) && 
          (max_cir_mantissa == ARAD_PP_MTR_VAL_TO_EXP_MNT_MAX_MNT_IR)) { /* unlimited max rate */
          bw_profile_info->max_cir = 0xffffffff;
      }
      else {
          res = arad_pp_mtr_ir_val_from_reverse_exp_mnt(
              unit,
              cir_reverse_exponent_2,
              max_cir_mantissa_exponent,
              max_cir_mantissa,
              &bw_profile_info->max_cir
              );
          SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
      }
  }

  /* Transfer mnt and exp to bs values */
  res = soc_sand_compute_complex_to_mnt_exp(
          prfcfg_tbl_data.cbs_mantissa_64,
          prfcfg_tbl_data.cbs_exponent,
          ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MULTI_BS,
          ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC_BS,
          &bw_profile_info->cbs
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  res = soc_sand_compute_complex_to_mnt_exp(
          prfcfg_tbl_data.ebs_mantissa_64,
          prfcfg_tbl_data.ebs_exponent,
          ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MULTI_BS,
          ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MNT_INC_BS,
          &bw_profile_info->ebs
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  bw_profile_info->color_mode          = prfcfg_tbl_data.color_aware ? ARAD_PP_MTR_COLOR_MODE_AWARE : ARAD_PP_MTR_COLOR_MODE_BLIND;
  bw_profile_info->is_coupling_enabled = SOC_SAND_NUM2BOOL(prfcfg_tbl_data.coupling_flag);
  bw_profile_info->disable_cir = SOC_SAND_NUM2BOOL(prfcfg_tbl_data.reset_cir);
  bw_profile_info->disable_eir = SOC_SAND_NUM2BOOL(prfcfg_tbl_data.reset_eir);
#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit)) {
    bw_profile_info->is_packet_mode = soc_mem_field32_get(unit, format_mem, (void*)reg_above64_val, PACKET_MODEf);
  }
#endif /* BCM_88660_A0 */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_bw_profile_get_unsafe()", mtr_group_ndx, bw_profile_ndx);
}

uint32
  arad_pp_mtr_bw_profile_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_BW_PROFILE_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(mtr_group_ndx, ARAD_PP_METERING_MTR_GROUP_NDX_MAX, ARAD_PP_METERING_MTR_GROUP_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(bw_profile_ndx, ARAD_PP_METERING_BW_PROFILE_NDX_MAX, ARAD_PP_METERING_BW_PROFILE_NDX_OUT_OF_RANGE_ERR, 20, exit);

  ARAD_PP_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_bw_profile_get_verify()", mtr_group_ndx, bw_profile_ndx);
}

/*********************************************************************
*     Remove Bandwidth Profile
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mtr_bw_profile_remove_unsafe(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_ndx
  )
{
  uint32
    res = SOC_SAND_OK;
  soc_reg_above_64_val_t
    reg_above64_val;
  soc_mem_t
    mem;
  uint32
    entry_offset = bw_profile_ndx;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_BW_PROFILE_REMOVE_UNSAFE);

  SOC_REG_ABOVE_64_CLEAR(reg_above64_val);

  /* check which mem to read from */
  if (mtr_group_ndx == 0) { /* group A */
      mem = IDR_MCDA_PRFCFG_0m;
  } else { /* group B */
      mem = IDR_MCDB_PRFCFG_0m;
  }

  /* clean entry in HW */
  res = soc_mem_write(unit, mem, MEM_BLOCK_ANY, entry_offset, (void*)reg_above64_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_bw_profile_remove_unsafe()", mtr_group_ndx, bw_profile_ndx);
}

uint32
  arad_pp_mtr_bw_profile_remove_verify(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  uint32                                  mtr_group_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_BW_PROFILE_REMOVE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(mtr_group_ndx, ARAD_PP_METERING_MTR_GROUP_NDX_MAX, ARAD_PP_METERING_MTR_GROUP_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(bw_profile_ndx, ARAD_PP_METERING_BW_PROFILE_NDX_MAX, ARAD_PP_METERING_BW_PROFILE_NDX_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_bw_profile_remove_verify()", mtr_group_ndx, bw_profile_ndx);
}

/*********************************************************************
*     Set meter attributes by mapping meter instance to
 *     bandwidth profile.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mtr_meter_ins_to_bw_profile_map_set_unsafe(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  ARAD_PP_MTR_METER_ID                    *meter_ins_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_id
  )
{
  uint32
    res = SOC_SAND_OK,
    array_index,
    index,
    profile_index,
    tmp_id,
	write_access_enable,
    fld_val,
    profile_mem_val[2],
	buckets_mem_val[7];/*dynamic table row is currently 27 bytes*/
  soc_mem_t
    profile_mem, 
	buckets_mem;
  soc_field_t
    profile_fld,
	cbl_bucket_fld,
	ebl_bucket_fld;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(meter_ins_ndx);


  /* MCDA is for meters 0-32K, MCDB is for 32-64K */
  if (meter_ins_ndx->group == 0) {
      profile_mem = IDR_MCDA_PRFSELm;
	  buckets_mem = IDR_MCDA_DYNAMICm;
  }
  else {
      profile_mem = IDR_MCDB_PRFSELm;
	  buckets_mem = IDR_MCDB_DYNAMICm;
  }

  /* in each mem: table_0 is for mtr_id 0-16K, table_1 is for 16-32K */
  if (meter_ins_ndx->id < 16384) {
      array_index = 0;
      tmp_id = meter_ins_ndx->id;
  }
  else {
      array_index = 1;
      tmp_id = meter_ins_ndx->id - 16384;
  }

  /* in each profile selecion table entry there are profile defenitions for 4 meters, we want to set one of the meters*/
  /* in each dynamic data table entry, there are CBL and EBL counter pairs for 4 meters, we want to init one of the pairs */
  index = tmp_id / 4;
  profile_index = tmp_id % 4;

  switch(profile_index) {
  case 0:
      profile_fld = PROFILE_0f;
	  cbl_bucket_fld = CBL_0f;
	  ebl_bucket_fld = EBL_0f;
      break;
  case 1:
      profile_fld = PROFILE_1f;
	  cbl_bucket_fld = CBL_1f;
	  ebl_bucket_fld = EBL_1f;
      break;
  case 2:
      profile_fld = PROFILE_2f;
	  cbl_bucket_fld = CBL_2f;
	  ebl_bucket_fld = EBL_2f;
      break;
  default: /* 3 */
      profile_fld = PROFILE_3f;
	  cbl_bucket_fld = CBL_3f;
	  ebl_bucket_fld = EBL_3f;
  }

  /* get the profile entry */
  res = soc_mem_array_read(unit, profile_mem, array_index, MEM_BLOCK_ANY, index, profile_mem_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* update one of the 4 profiles */
  fld_val = bw_profile_id;
  soc_mem_field_set(unit, profile_mem, profile_mem_val, profile_fld , &fld_val);
 
  res = soc_mem_array_write(unit, profile_mem, array_index, MEM_BLOCK_ANY, index, profile_mem_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /* clear EBL and CBL from remaining tokens from previous profiles */
  /* check if we have write access for dynamic memory, if not- take it*/
  READ_IDR_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, &write_access_enable);
  if (0 == write_access_enable) {
	  SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, WRITE_IDR_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 1));
  }

  /* get bucket entries */
  res = soc_mem_array_read(unit, buckets_mem, array_index, MEM_BLOCK_ANY, index, buckets_mem_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  /* set matching EBL and CBL to -8193 (compensate for LFSR)*/
  fld_val = 0x7FDFFF;
  soc_mem_field_set(unit, buckets_mem, buckets_mem_val, cbl_bucket_fld , &fld_val);
  soc_mem_field_set(unit, buckets_mem, buckets_mem_val, ebl_bucket_fld , &fld_val);

  res = soc_mem_array_write(unit, buckets_mem, array_index, MEM_BLOCK_ANY, index, buckets_mem_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  
  /* if we took write access, disable it*/
  if (0 == write_access_enable) {
	  SOC_SAND_SOC_IF_ERROR_RETURN(res, 60, exit, WRITE_IDR_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, 0));
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_meter_ins_to_bw_profile_map_set_unsafe()", 0, 0);
}

uint32
  arad_pp_mtr_meter_ins_to_bw_profile_map_set_verify(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  ARAD_PP_MTR_METER_ID                    *meter_ins_ndx,
    SOC_SAND_IN  uint32                                  bw_profile_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_SET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(ARAD_PP_MTR_METER_ID, meter_ins_ndx, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(bw_profile_id, ARAD_PP_METERING_BW_PROFILE_NDX_MAX, ARAD_PP_METERING_BW_PROFILE_ID_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_meter_ins_to_bw_profile_map_set_verify()", 0, 0);
}

uint32
  arad_pp_mtr_meter_ins_to_bw_profile_map_get_verify(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  ARAD_PP_MTR_METER_ID                    *meter_ins_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_GET_VERIFY);

  ARAD_PP_STRUCT_VERIFY(ARAD_PP_MTR_METER_ID, meter_ins_ndx, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_meter_ins_to_bw_profile_map_get_verify()", 0, 0);
}

/*********************************************************************
*     Set meter attributes by mapping meter instance to
 *     bandwidth profile.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mtr_meter_ins_to_bw_profile_map_get_unsafe(
    SOC_SAND_IN  int                                  unit,
    SOC_SAND_IN  ARAD_PP_MTR_METER_ID                    *meter_ins_ndx,
    SOC_SAND_OUT uint32                                  *bw_profile_id
  )
{
  uint32
    res = SOC_SAND_OK,
    array_index,
    index,
    profile_index,
    tmp_id,
    profile_id,
    mem_val[2];
  soc_mem_t
    mem;
  soc_field_t
    fld;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_METER_INS_TO_BW_PROFILE_MAP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(meter_ins_ndx);
  SOC_SAND_CHECK_NULL_INPUT(bw_profile_id);
  
  /* MCDA is for meters 0-32K, MCDB is for 32-64K */
  if (meter_ins_ndx->group == 0) {
      mem = IDR_MCDA_PRFSELm;
  }
  else {
      mem = IDR_MCDB_PRFSELm;
  }

  /* in each mem: table_0 is for mtr_id 0-16K, table_1 is for 16-32K */
  if (meter_ins_ndx->id < 16384) {
      array_index = 0;
      tmp_id = meter_ins_ndx->id;
  }
  else {
      array_index = 1;
      tmp_id = meter_ins_ndx->id - 16384;
  }

  /* in each table entry there are 4 profiles, we want to configure one of them */
  index = tmp_id / 4;
  profile_index = tmp_id % 4;

  switch(profile_index) {
  case 0:
      fld = PROFILE_0f;
      break;
  case 1:
      fld = PROFILE_1f;
      break;
  case 2:
      fld = PROFILE_2f;
      break;
  default: /* 3 */
      fld = PROFILE_3f;
  }

  /* get the entry */
  res = soc_mem_array_read(unit, mem, array_index, MEM_BLOCK_ANY, index, mem_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /* get one of the 4 profiles */
  soc_mem_field_get(unit, mem, mem_val, fld, &profile_id);

  *bw_profile_id = profile_id;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_meter_ins_to_bw_profile_map_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Enable / Disable Ethernet policing.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mtr_eth_policer_enable_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                  enable
  )
{
  uint32
    res = SOC_SAND_OK,
    idx,
    status_enable,
    status_enable_start,
    reg_val;
  uint8
    config_status_bit;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_ENABLE_SET_UNSAFE);

  if (SOC_IS_JERICHO(unit)) {
      goto exit;
  }

  /* set eth policer global enable status bit */
  arad_pp_sw_db_eth_policer_config_status_bit_set(
    unit,
    ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_ENABLE_STATUS_BIT_MAIN,
    enable
  );

  /* set profile config status bit */
  for (idx = 0; idx < ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_CONFIG_METER_PROFILE_NOF_MEMBER_BIT; idx++)
  {
      res = READ_IDR_ETHERNET_METER_CONFIGm(unit, MEM_BLOCK_ANY, idx, &reg_val); 
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      soc_mem_field_get(unit, IDR_ETHERNET_METER_CONFIGm, &reg_val, ENABLEf, &status_enable_start);

      arad_pp_sw_db_eth_policer_config_status_bit_get(
          unit,
          idx,
          &config_status_bit
          );

      if ((enable == TRUE) && (config_status_bit == TRUE))
      {
          status_enable = SOC_SAND_BOOL2NUM(TRUE);
      }
      else /* disable: enable == FALSE*/
      {
          status_enable = SOC_SAND_BOOL2NUM(FALSE);
      }

      if (status_enable_start != status_enable)
      {
          soc_mem_field_set(unit, IDR_ETHERNET_METER_CONFIGm, &reg_val, ENABLEf, &status_enable);
          
          res = WRITE_IDR_ETHERNET_METER_CONFIGm(unit, MEM_BLOCK_ANY, idx, &reg_val); 
          SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      }
  }

  /* set global config status bit */
  for (idx = 0 ; idx < ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_GLOBAL_METER_PROFILE_NOF_MEMBER; idx++)
  {
    res = READ_IDR_GLOBAL_METER_PROFILESm(unit, MEM_BLOCK_ANY, idx, &reg_val); 
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    soc_mem_field_get(unit, IDR_GLOBAL_METER_PROFILESm, &reg_val, ENABLEf, &status_enable_start);

    arad_pp_sw_db_eth_policer_config_status_bit_get(
      unit,
      ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_CONFIG_METER_PROFILE_NOF_MEMBER_BIT + idx,
      &config_status_bit
    );

    if ((enable == TRUE) && (config_status_bit == TRUE))
    {
      status_enable = SOC_SAND_BOOL2NUM(TRUE);
    }
    else /* disable: enable == FALSE*/
    {
      status_enable = SOC_SAND_BOOL2NUM(FALSE);
    }

    if (status_enable_start != status_enable)
    {
      soc_mem_field_set(unit, IDR_GLOBAL_METER_PROFILESm, &reg_val, ENABLEf, &status_enable);
          
      res = WRITE_IDR_GLOBAL_METER_PROFILESm(unit, MEM_BLOCK_ANY, idx, &reg_val); 
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }
  }
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_eth_policer_enable_set_unsafe()", 0, 0);
}

uint32
  arad_pp_mtr_eth_policer_enable_set_verify(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  uint8                                 enable
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_ENABLE_SET_VERIFY);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_eth_policer_enable_set_verify()", 0, 0);
}

uint32
  arad_pp_mtr_eth_policer_enable_get_verify(
    SOC_SAND_IN  int                                 unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_ENABLE_GET_VERIFY);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_eth_policer_enable_get_verify()", 0, 0);
}

/*********************************************************************
*     Enable / Disable Ethernet policing.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mtr_eth_policer_enable_get_unsafe(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_OUT uint8                                 *enable
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_ENABLE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(enable);

  /* set eth policer global enable status bit */
  arad_pp_sw_db_eth_policer_config_status_bit_get(
    unit,
    ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_ENABLE_STATUS_BIT_MAIN,
    enable
  );
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_eth_policer_enable_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Set policer attributes of the Ethernet policer. Enable
 *     policing per ingress port and Ethernet type.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_pp_mtr_eth_policer_params_set_unsafe(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  ARAD_PP_PORT                                port_ndx,
    SOC_SAND_IN  ARAD_PP_MTR_ETH_TYPE                        eth_type_ndx,
    SOC_SAND_IN  ARAD_PP_MTR_BW_PROFILE_INFO                 *policer_info
  )
{
  uint32
    res = SOC_SAND_OK,
    eth_mtr_profile_multiset_key = 0x0,
    eth_mtr_profile_multiset_ndx = 0x0,
    config_tbl_offset,
    status_tbl_offset;
  uint32
    eth_mtr_config_val,
    eth_mtr_profile_val;
  uint8
    eth_mtr_profile_multiset_first_appear = 0x0,
    eth_mtr_profile_multiset_last_appear = 0x0,
    sw_db_enable_bit,
    main_enable,
    info_enable,
    enable;
  SOC_SAND_SUCCESS_FAILURE
    eth_mtr_profile_multiset_success = SOC_SAND_SUCCESS;
  ARAD_IDR_ETHERNET_METER_STATUS_TBL_DATA
    status_tbl_data;
  ARAD_IDR_ETHERNET_METER_CONFIG_TBL_DATA
    config_tbl_data_start,
    config_tbl_data;
  ARAD_IDR_ETHERNET_METER_PROFILES_TBL_DATA
    profile_tbl_data;
  int cir, cbs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_PARAMS_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(policer_info);

  ARAD_CLEAR(&status_tbl_data,ARAD_IDR_ETHERNET_METER_STATUS_TBL_DATA,1);
  ARAD_CLEAR(&config_tbl_data,ARAD_IDR_ETHERNET_METER_CONFIG_TBL_DATA,1);
  ARAD_CLEAR(&config_tbl_data_start,ARAD_IDR_ETHERNET_METER_CONFIG_TBL_DATA,1);
  ARAD_CLEAR(&profile_tbl_data,ARAD_IDR_ETHERNET_METER_PROFILES_TBL_DATA,1);

  status_tbl_offset = config_tbl_offset = port_ndx * ARAD_PP_NOF_MTR_ETH_TYPES + eth_type_ndx;

  info_enable = SOC_SAND_BOOL2NUM_INVERSE(policer_info->disable_cir);
  
  /* Get pointers from config table */  
  res = READ_IDR_ETHERNET_METER_CONFIGm(unit, MEM_BLOCK_ANY, config_tbl_offset, &eth_mtr_config_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  config_tbl_data_start.profile = soc_mem_field32_get(unit, IDR_ETHERNET_METER_CONFIGm, &eth_mtr_config_val, PROFILEf);

  arad_pp_sw_db_eth_policer_config_status_bit_get(
      unit,
      status_tbl_offset,
      &sw_db_enable_bit
    );
  
  /* get eth policer main enable status bit */
  arad_pp_sw_db_eth_policer_config_status_bit_get(
    unit,
    ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_ENABLE_STATUS_BIT_MAIN,
    &main_enable
  );

  if (sw_db_enable_bit == TRUE)
  {
    /* profile is enable remove from multiset */   
    res = arad_sw_db_multiset_remove_by_index(
            unit,
            ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_ETH_METER_PROFILE,
            config_tbl_data_start.profile,
            &eth_mtr_profile_multiset_last_appear
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

  cir = policer_info->cir;
  cbs = policer_info->cbs;
  if (policer_info->is_packet_mode)
  {
    /* In packet mode we need to halve the rate. */
    /* 
     * Explanation: 
     * In regular mode, the rate units in h/w are bytes. I.e. if the h/w rate is x then the rate is (x*8) bits per second.
     * Since in regular mode the user inputs the rate in kbit/s, the
     * rate_to_exp_mnt function multiplies by 1024 (from kbit/s to 1024 bps)
     * and then divides by 8 (from x bps to (x/8) Bps).
     *
     * rate_to_exp_mnt rate = user_rate * 1024 / 8 = user_rate * 2^(10-3) = user_rate * 2^7 = user_rate * 128
     *
     * For packet mode however the rate units are 1/64 packet. I.e. if the h/w rate is x then the rate is (x/64) packets per second.
     * Therefore for packet mode we need to take the rate and multiply it by 64.
     *
     * packet_rate = user_rate * 64
     *
     * Finally if we want to use the rate_to_exp_mnt function for packet mode, we need to use half the user_rate for
     * the function:
     * rate_to_exp_mnt rate = (user_rate / 2) * 128 = user_rate * 64 = packet_rate
     */

    cir /= 2;
    cbs /= 2;
  
  }

  /* cir is enable fill profile table */
  if (info_enable == TRUE)
  {
    /* transfer user values (cir, cbs) to mnt, exp and res */
    res = arad_pp_mtr_profile_rate_to_res_exp_mnt(
        unit,
        cir,
        &profile_tbl_data.meter_resolution,
        &profile_tbl_data.rate_mantissa,
        &profile_tbl_data.rate_exp
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    /* res also affects the burst: actual-burst = user-burst << res */
    arad_pp_mtr_profile_burst_to_exp_mnt(
      unit, 
      cbs,
      profile_tbl_data.meter_resolution, /* rate resolution */
      &profile_tbl_data.burst_mantissa,
      &profile_tbl_data.burst_exp
  	  );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

    profile_tbl_data.packet_mode = SOC_SAND_BOOL2NUM(policer_info->is_packet_mode);
    if (SOC_IS_ARADPLUS(unit)) {
      profile_tbl_data.color_blind = policer_info->color_mode == SOC_PPC_MTR_COLOR_MODE_BLIND ? 1 : 0;
    }

    arad_pp_mtr_eth_policer_profile_key_get(
      unit,
      profile_tbl_data,
      &eth_mtr_profile_multiset_key
    );
    
    res = arad_sw_db_multiset_add(
          unit,
          ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_ETH_METER_PROFILE,
          &eth_mtr_profile_multiset_key,
          &eth_mtr_profile_multiset_ndx,
          &eth_mtr_profile_multiset_first_appear,
          &eth_mtr_profile_multiset_success
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

    if (eth_mtr_profile_multiset_success != SOC_SAND_SUCCESS)
    { 
      /* Add old profile to multi set */
      res = READ_IDR_ETHERNET_METER_PROFILESm(unit, MEM_BLOCK_ANY, config_tbl_data_start.profile, &eth_mtr_profile_val);
      SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);

      profile_tbl_data.rate_mantissa = soc_mem_field32_get(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, RATE_MANTISSAf);
      profile_tbl_data.rate_exp = soc_mem_field32_get(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, RATE_EXPf);
      profile_tbl_data.burst_mantissa = soc_mem_field32_get(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, BURST_MANTISSAf);
      profile_tbl_data.burst_exp = soc_mem_field32_get(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, BURST_EXPf);
      profile_tbl_data.packet_mode = soc_mem_field32_get(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, PACKET_MODEf);
      profile_tbl_data.meter_resolution = soc_mem_field32_get(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, METER_RESOLUTIONf);
      if (SOC_IS_ARADPLUS(unit)) {
        profile_tbl_data.color_blind = soc_mem_field32_get(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, COLOR_BLINDf);
      }

      arad_pp_mtr_eth_policer_profile_key_get(
        unit,
        profile_tbl_data,
        &eth_mtr_profile_multiset_key
      );

      res = arad_sw_db_multiset_add(
              unit,
              ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_ETH_METER_PROFILE,
              &eth_mtr_profile_multiset_key,
              &eth_mtr_profile_multiset_ndx,
              &eth_mtr_profile_multiset_first_appear,
              &eth_mtr_profile_multiset_success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

      SOC_SAND_SET_ERROR_CODE(ARAD_PP_MTR_ETH_POLICER_ADD_FAIL_ERR, 100, exit);
    }
  }

  /* Set sw_db enable bit */
  arad_pp_sw_db_eth_policer_config_status_bit_set(
    unit,
    status_tbl_offset,
    info_enable
  );
  
  /* Set profile table - if first */
  if (eth_mtr_profile_multiset_first_appear == 0x1)
  {
    soc_mem_field32_set(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, RATE_MANTISSAf, profile_tbl_data.rate_mantissa);
    soc_mem_field32_set(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, RATE_EXPf, profile_tbl_data.rate_exp);
    soc_mem_field32_set(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, BURST_MANTISSAf, profile_tbl_data.burst_mantissa);
    soc_mem_field32_set(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, BURST_EXPf, profile_tbl_data.burst_exp);
    soc_mem_field32_set(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, PACKET_MODEf, profile_tbl_data.packet_mode);
    soc_mem_field32_set(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, METER_RESOLUTIONf, profile_tbl_data.meter_resolution);
    if (SOC_IS_ARADPLUS(unit)) {
      soc_mem_field32_set(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, COLOR_BLINDf, profile_tbl_data.color_blind);
    }
    
    res = WRITE_IDR_ETHERNET_METER_PROFILESm(unit, MEM_BLOCK_ANY, eth_mtr_profile_multiset_ndx, &eth_mtr_profile_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);
  }

  /* Set pointer in config table */
  /* Read old table entry - for global ptr */
  res = READ_IDR_ETHERNET_METER_CONFIGm(unit, MEM_BLOCK_ANY, config_tbl_offset, &eth_mtr_config_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);
  
  soc_mem_field32_set(unit, IDR_ETHERNET_METER_CONFIGm, &eth_mtr_config_val, PROFILEf, eth_mtr_profile_multiset_ndx);

  /* Set enable bit in config table */
  if ((main_enable == TRUE) && (info_enable))
  {
    enable = TRUE;
  }
  else
  {
    enable = FALSE;
  }

  soc_mem_field32_set(unit, IDR_ETHERNET_METER_CONFIGm, &eth_mtr_config_val, ENABLEf, enable);
 
  res = WRITE_IDR_ETHERNET_METER_CONFIGm(unit, MEM_BLOCK_ANY, config_tbl_offset, &eth_mtr_config_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);

  /* Set profile table - if last */
  if ((eth_mtr_profile_multiset_last_appear == 0x1) && (eth_mtr_profile_multiset_ndx != config_tbl_data_start.profile))
  {
    ARAD_CLEAR(&profile_tbl_data, ARAD_IDR_ETHERNET_METER_PROFILES_TBL_DATA, 1);

    soc_mem_field32_set(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, RATE_MANTISSAf, profile_tbl_data.rate_mantissa);
    soc_mem_field32_set(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, RATE_EXPf, profile_tbl_data.rate_exp);
    soc_mem_field32_set(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, BURST_MANTISSAf, profile_tbl_data.burst_mantissa);
    soc_mem_field32_set(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, BURST_EXPf, profile_tbl_data.burst_exp);
    soc_mem_field32_set(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, PACKET_MODEf, profile_tbl_data.packet_mode);
    soc_mem_field32_set(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, METER_RESOLUTIONf, profile_tbl_data.meter_resolution);
    if (SOC_IS_ARADPLUS(unit)) {
      if (soc_property_get(unit, spn_RATE_COLOR_BLIND, 0)) {
        profile_tbl_data.color_blind = 1;
      }
      soc_mem_field32_set(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, COLOR_BLINDf, profile_tbl_data.color_blind);
    }
    
    res = WRITE_IDR_ETHERNET_METER_PROFILESm(unit, MEM_BLOCK_ANY, config_tbl_data_start.profile, &eth_mtr_profile_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_eth_policer_params_set_unsafe()", port_ndx, 0);
}

uint32
  arad_pp_mtr_eth_policer_params_set_verify(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  ARAD_PP_PORT                                port_ndx,
    SOC_SAND_IN  ARAD_PP_MTR_ETH_TYPE                        eth_type_ndx,
    SOC_SAND_IN  ARAD_PP_MTR_BW_PROFILE_INFO                 *policer_info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_PARAMS_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_ndx, ARAD_PP_PORT_MAX, ARAD_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(eth_type_ndx, ARAD_PP_METERING_ETH_TYPE_NDX_MAX, ARAD_PP_METERING_ETH_TYPE_NDX_OUT_OF_RANGE_ERR, 20, exit);
  
  SOC_SAND_ERR_IF_ABOVE_MAX(policer_info->cir, ARAD_IF_MAX_RATE_KBPS(unit), ARAD_PP_METERING_CIR_OUT_OF_RANGE_ERR, 30, exit);
  SOC_SAND_ERR_IF_BELOW_MIN(policer_info->cir, ARAD_PP_MTR_PROFILE_ADD_VERIFY_IR_MIN, ARAD_PP_METERING_CIR_OUT_OF_RANGE_ERR, 40, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(policer_info->cbs, ARAD_PP_BW_PROFILE_ADD_VERIFY_BS_MAX, ARAD_PP_METERING_CBS_OUT_OF_RANGE_ERR, 50, exit);
  SOC_SAND_ERR_IF_BELOW_MIN(policer_info->cbs, ARAD_PP_BW_PROFILE_ADD_VERIFY_BS_MIN, ARAD_PP_METERING_CBS_OUT_OF_RANGE_ERR, 60, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_eth_policer_params_set_verify()", port_ndx, 0);
}

uint32
  arad_pp_mtr_eth_policer_params_get_verify(
    SOC_SAND_IN  int                                      unit,
    SOC_SAND_IN  ARAD_PP_PORT                                port_ndx,
    SOC_SAND_IN  ARAD_PP_MTR_ETH_TYPE                        eth_type_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_PARAMS_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_ndx, ARAD_PP_PORT_MAX, ARAD_PP_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(eth_type_ndx, ARAD_PP_METERING_ETH_TYPE_NDX_MAX, ARAD_PP_METERING_ETH_TYPE_NDX_OUT_OF_RANGE_ERR, 20, exit);

  ARAD_PP_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_eth_policer_params_get_verify()", port_ndx, 0);
}

/*********************************************************************
 *     Set policer attributes of the Ethernet policer. Enable
 *     policing per ingress port and Ethernet type.
 *     Details: in the H file. (search for prototype)
 *********************************************************************/
uint32
  arad_pp_mtr_eth_policer_params_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  ARAD_PP_PORT                                port_ndx,
    SOC_SAND_IN  ARAD_PP_MTR_ETH_TYPE                        eth_type_ndx,
    SOC_SAND_OUT ARAD_PP_MTR_BW_PROFILE_INFO                 *policer_info
  )
{
  uint32
    res = SOC_SAND_OK,
    clk_freq,
    config_tbl_offset,
    status_tbl_offset,
    tmp_cbs;
  ARAD_IDR_ETHERNET_METER_CONFIG_TBL_DATA
    config_tbl_data;
  ARAD_IDR_ETHERNET_METER_PROFILES_TBL_DATA
    profile_tbl_data;
  uint8
    sw_db_enable_bit;
  uint32
    eth_mtr_config_val,
    eth_mtr_profile_val;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_PARAMS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(policer_info);

  ARAD_PP_MTR_BW_PROFILE_INFO_clear(policer_info);

  ARAD_CLEAR(&config_tbl_data,ARAD_IDR_ETHERNET_METER_CONFIG_TBL_DATA,1);
  ARAD_CLEAR(&profile_tbl_data,ARAD_IDR_ETHERNET_METER_PROFILES_TBL_DATA,1);

  clk_freq = arad_chip_ticks_per_sec_get(unit);

  status_tbl_offset = config_tbl_offset = port_ndx * ARAD_PP_NOF_MTR_ETH_TYPES + eth_type_ndx;

  /* Get pointers from config table*/
  res = READ_IDR_ETHERNET_METER_CONFIGm(unit, MEM_BLOCK_ANY, config_tbl_offset, &eth_mtr_config_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  config_tbl_data.profile = soc_mem_field32_get(unit, IDR_ETHERNET_METER_CONFIGm, &eth_mtr_config_val, PROFILEf);

  /* Get enable bit */
  config_tbl_data.enable = soc_mem_field32_get(unit, IDR_ETHERNET_METER_CONFIGm, &eth_mtr_config_val, ENABLEf);

  arad_pp_sw_db_eth_policer_config_status_bit_get(
    unit,
    status_tbl_offset,
    &sw_db_enable_bit
  );

  policer_info->disable_cir = SOC_SAND_NUM2BOOL_INVERSE(sw_db_enable_bit);

  /* Get committed mnt, exp values from profile table */
  res = READ_IDR_ETHERNET_METER_PROFILESm(unit, MEM_BLOCK_ANY, config_tbl_data.profile, &eth_mtr_profile_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  profile_tbl_data.rate_mantissa = soc_mem_field32_get(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, RATE_MANTISSAf);
  profile_tbl_data.rate_exp = soc_mem_field32_get(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, RATE_EXPf);
  profile_tbl_data.burst_mantissa = soc_mem_field32_get(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, BURST_MANTISSAf);
  profile_tbl_data.burst_exp = soc_mem_field32_get(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, BURST_EXPf);
  profile_tbl_data.packet_mode = soc_mem_field32_get(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, PACKET_MODEf);
  profile_tbl_data.meter_resolution = soc_mem_field32_get(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, METER_RESOLUTIONf);
  if (SOC_IS_ARADPLUS(unit)) {
    profile_tbl_data.color_blind = soc_mem_field32_get(unit, IDR_ETHERNET_METER_PROFILESm, &eth_mtr_profile_val, COLOR_BLINDf);
  }

  policer_info->is_packet_mode = SOC_SAND_NUM2BOOL(profile_tbl_data.packet_mode);
  policer_info->color_mode = profile_tbl_data.color_blind ? SOC_PPC_MTR_COLOR_MODE_BLIND : SOC_PPC_MTR_COLOR_MODE_AWARE;

  /* Calc rates from mnt, exp and res */
  res = soc_sand_compute_complex_to_mnt_exp_reverse_exp(
      profile_tbl_data.rate_mantissa,
      profile_tbl_data.rate_exp,
      profile_tbl_data.meter_resolution,
      clk_freq,
      ARAD_PP_MTR_PROFILE_VAL_EXP_MNT_EQ_CONST_MNT_DIV_IR,
      ARAD_PP_MTR_PROFILE_VAL_EXP_MNT_EQ_CONST_MNT_INC_IR,
      &policer_info->cir
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_compute_complex_to_mnt_exp(
          profile_tbl_data.burst_mantissa,
          profile_tbl_data.burst_exp,
          ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MULTI_BS,
          ARAD_PP_MTR_PROFILE_VAL_EXP_MNT_EQ_CONST_MNT_INC_BS,
          &tmp_cbs
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  policer_info->cbs = (tmp_cbs >> profile_tbl_data.meter_resolution);

  /* For packet mode, the h/w rates are half what the calculation expects (see
   * arad_pp_mtr_eth_policer_params_set_unsafe), so we must double them. */
  if (policer_info->is_packet_mode)
  {
    policer_info->cir *= 2;
    policer_info->cbs *= 2;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_eth_policer_params_get_unsafe()", port_ndx, 0);
}

/*********************************************************************
*     Set Ethernet policer Global Profile attributes.         
 *     Details: in the H file. (search for prototype)          
*********************************************************************/
uint32  
  arad_pp_mtr_eth_policer_glbl_profile_set_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      glbl_profile_idx,
    SOC_SAND_IN  ARAD_PP_MTR_BW_PROFILE_INFO *policer_info
  )
{
  uint32
    res = SOC_SAND_OK,
    reg_val = 0,
    global_enable;
  uint8
    main_enable,
    glbl_info_enable;
  ARAD_IDR_GLOBAL_METER_PROFILES_TBL_DATA
    glbl_tbl_data;
  uint32 color_blind = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(policer_info);

  ARAD_CLEAR(&glbl_tbl_data, ARAD_IDR_GLOBAL_METER_PROFILES_TBL_DATA, 1);

  /* transfer user values (cir, cbs) to mnt, exp and res */
  res = arad_pp_mtr_profile_rate_to_res_exp_mnt(
      unit,
      policer_info->cir,
      &glbl_tbl_data.meter_resolution,
      &glbl_tbl_data.rate_mantissa,
      &glbl_tbl_data.rate_exp
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /* res also affects the burst: actual-burst = user-burst << res */
  arad_pp_mtr_profile_burst_to_exp_mnt(
		unit, 
		policer_info->cbs, 
		glbl_tbl_data.meter_resolution, /* rate resolution */
		&glbl_tbl_data.burst_mantissa,
        &glbl_tbl_data.burst_exp
	);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  glbl_info_enable = SOC_SAND_BOOL2NUM_INVERSE(policer_info->disable_cir);
  
  /* get eth policer main enable status bit */
  arad_pp_sw_db_eth_policer_config_status_bit_get(
    unit,
    ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_ENABLE_STATUS_BIT_MAIN,
    &main_enable
  );

  if ((main_enable == TRUE) && (glbl_info_enable == TRUE) )
  {
    global_enable = TRUE;
  }
  else 
  {
    global_enable = FALSE;
  }
  soc_mem_field_set(unit, IDR_GLOBAL_METER_PROFILESm, &reg_val, ENABLEf, &global_enable);

  /* Start writes */
  /* write to sw_db the enable bit */
  arad_pp_sw_db_eth_policer_config_status_bit_set(
      unit,
      ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_CONFIG_METER_PROFILE_NOF_MEMBER_BIT + glbl_profile_idx,
      glbl_info_enable
  );

  glbl_tbl_data.packet_mode = SOC_SAND_BOOL2NUM(policer_info->is_packet_mode);
  soc_mem_field_set(unit, IDR_GLOBAL_METER_PROFILESm, &reg_val, PACKET_MODEf, &glbl_tbl_data.packet_mode);
  soc_mem_field_set(unit, IDR_GLOBAL_METER_PROFILESm, &reg_val, RATE_EXPf, &glbl_tbl_data.rate_exp);
  soc_mem_field_set(unit, IDR_GLOBAL_METER_PROFILESm, &reg_val, RATE_MANTISSAf, &glbl_tbl_data.rate_mantissa);
  soc_mem_field_set(unit, IDR_GLOBAL_METER_PROFILESm, &reg_val, BURST_EXPf, &glbl_tbl_data.burst_exp);
  soc_mem_field_set(unit, IDR_GLOBAL_METER_PROFILESm, &reg_val, BURST_MANTISSAf, &glbl_tbl_data.burst_mantissa);
  soc_mem_field_set(unit, IDR_GLOBAL_METER_PROFILESm, &reg_val, METER_RESOLUTIONf, &glbl_tbl_data.meter_resolution);
  
  if (SOC_IS_ARADPLUS(unit)) {
    /* If the mode is color blind then all policers are color blind. */
    if (soc_property_get(unit, spn_RATE_COLOR_BLIND, 0) || policer_info->color_mode == SOC_PPC_MTR_COLOR_MODE_BLIND) {
      color_blind = 1;
    }
    soc_mem_field_set(unit, IDR_GLOBAL_METER_PROFILESm, &reg_val, COLOR_BLINDf, &color_blind);
  }

  res = WRITE_IDR_GLOBAL_METER_PROFILESm(unit, MEM_BLOCK_ANY, glbl_profile_idx, &reg_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_eth_policer_glbl_profile_set_unsafe()", 0, 0);
}

uint32  
  arad_pp_mtr_eth_policer_glbl_profile_set_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      glbl_profile_idx,
    SOC_SAND_IN  ARAD_PP_MTR_BW_PROFILE_INFO *policer_info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(glbl_profile_idx, ARAD_PP_METERING_GLBL_PROFILE_IDX_MAX, ARAD_PP_METERING_GLBL_PROFILE_IDX_OUT_OF_RANGE_ERR, 10, exit);
  /* ARAD_PP_METERING_GLBL_PROFILE_IDX_MIN may be changed and be grater than 0*/
  /* coverity[unsigned_compare : FALSE] */
  SOC_SAND_ERR_IF_BELOW_MIN(glbl_profile_idx, ARAD_PP_METERING_GLBL_PROFILE_IDX_MIN, ARAD_PP_METERING_GLBL_PROFILE_IDX_OUT_OF_RANGE_ERR, 20, exit);
  
  SOC_SAND_ERR_IF_ABOVE_MAX(policer_info->cir, ARAD_IF_MAX_RATE_KBPS(unit), ARAD_PP_METERING_CIR_OUT_OF_RANGE_ERR, 30, exit);
  SOC_SAND_ERR_IF_BELOW_MIN(policer_info->cir, ARAD_PP_MTR_PROFILE_ADD_VERIFY_IR_MIN, ARAD_PP_METERING_CIR_OUT_OF_RANGE_ERR, 40, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(policer_info->cbs, ARAD_PP_BW_PROFILE_ADD_VERIFY_BS_MAX, ARAD_PP_METERING_CBS_OUT_OF_RANGE_ERR, 50, exit);
  SOC_SAND_ERR_IF_BELOW_MIN(policer_info->cbs, ARAD_PP_BW_PROFILE_ADD_VERIFY_BS_MIN, ARAD_PP_METERING_CBS_OUT_OF_RANGE_ERR, 60, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_eth_policer_glbl_profile_set_verify()", 0, 0);
}

uint32  
  arad_pp_mtr_eth_policer_glbl_profile_get_verify(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  glbl_profile_idx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(glbl_profile_idx, ARAD_PP_METERING_GLBL_PROFILE_IDX_MAX, ARAD_PP_METERING_GLBL_PROFILE_IDX_OUT_OF_RANGE_ERR, 10, exit);
  /* ARAD_PP_METERING_GLBL_PROFILE_IDX_MIN may be changed and be grater than 0*/
  /* coverity[unsigned_compare : FALSE] */
  SOC_SAND_ERR_IF_BELOW_MIN(glbl_profile_idx, ARAD_PP_METERING_GLBL_PROFILE_IDX_MIN, ARAD_PP_METERING_GLBL_PROFILE_IDX_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_eth_policer_glbl_profile_get_verify()", 0, 0);
}

/*********************************************************************
*     Set Ethernet policer Global Profile attributes.         
 *     Details: in the H file. (search for prototype)          
*********************************************************************/
uint32  
  arad_pp_mtr_eth_policer_glbl_profile_get_unsafe(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  uint32                      glbl_profile_idx,
    SOC_SAND_OUT ARAD_PP_MTR_BW_PROFILE_INFO *policer_info
  )
{
  uint32
    res = SOC_SAND_OK,
    clk_freq,
    reg_val,
    tmp_cbs;
  ARAD_IDR_GLOBAL_METER_PROFILES_TBL_DATA
    global_tbl_data;
  uint8
    glbl_info_enable;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(policer_info);

  ARAD_PP_MTR_BW_PROFILE_INFO_clear(policer_info);
  ARAD_CLEAR(&global_tbl_data, ARAD_IDR_GLOBAL_METER_PROFILES_TBL_DATA, 1);

  clk_freq = arad_chip_ticks_per_sec_get(unit); 

  /* Get Excess mnt, exp values from global table */
  res = READ_IDR_GLOBAL_METER_PROFILESm(unit, MEM_BLOCK_ANY, glbl_profile_idx, &reg_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  soc_mem_field_get(unit, IDR_GLOBAL_METER_PROFILESm, &reg_val, RATE_MANTISSAf, &global_tbl_data.rate_mantissa);
  soc_mem_field_get(unit, IDR_GLOBAL_METER_PROFILESm, &reg_val, RATE_EXPf, &global_tbl_data.rate_exp);
  soc_mem_field_get(unit, IDR_GLOBAL_METER_PROFILESm, &reg_val, BURST_MANTISSAf, &global_tbl_data.burst_mantissa);
  soc_mem_field_get(unit, IDR_GLOBAL_METER_PROFILESm, &reg_val, BURST_EXPf, &global_tbl_data.burst_exp);
  soc_mem_field_get(unit, IDR_GLOBAL_METER_PROFILESm, &reg_val, PACKET_MODEf, &global_tbl_data.packet_mode);
  soc_mem_field_get(unit, IDR_GLOBAL_METER_PROFILESm, &reg_val, METER_RESOLUTIONf, &global_tbl_data.meter_resolution);
  if (SOC_IS_ARADPLUS(unit)) {
    soc_mem_field_get(unit, IDR_GLOBAL_METER_PROFILESm, &reg_val, COLOR_BLINDf, &global_tbl_data.color_blind);
  }

  /* Calc rates from mnt, exp and res */
  res = soc_sand_compute_complex_to_mnt_exp_reverse_exp(
      global_tbl_data.rate_mantissa,
      global_tbl_data.rate_exp,
      global_tbl_data.meter_resolution,
      clk_freq,
      ARAD_PP_MTR_PROFILE_VAL_EXP_MNT_EQ_CONST_MNT_DIV_IR,
      ARAD_PP_MTR_PROFILE_VAL_EXP_MNT_EQ_CONST_MNT_INC_IR,
      &policer_info->cir
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  res = soc_sand_compute_complex_to_mnt_exp(
          global_tbl_data.burst_mantissa,
          global_tbl_data.burst_exp,
          ARAD_PP_MTR_VAL_EXP_MNT_EQ_CONST_MULTI_BS,
          ARAD_PP_MTR_PROFILE_VAL_EXP_MNT_EQ_CONST_MNT_INC_BS,
          &tmp_cbs
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  policer_info->cbs = (tmp_cbs >> global_tbl_data.meter_resolution);

  

  arad_pp_sw_db_eth_policer_config_status_bit_get(
      unit,
      ARAD_PP_SW_DB_MULTI_SET_ETH_POLICER_CONFIG_METER_PROFILE_NOF_MEMBER_BIT + glbl_profile_idx,
      &glbl_info_enable
  );

  policer_info->disable_cir = SOC_SAND_NUM2BOOL_INVERSE(glbl_info_enable);
  policer_info->is_packet_mode = SOC_SAND_NUM2BOOL(global_tbl_data.packet_mode);
  if (SOC_IS_ARADPLUS(unit)) {
    policer_info->color_mode = global_tbl_data.color_blind ? SOC_PPC_MTR_COLOR_MODE_BLIND : SOC_PPC_MTR_COLOR_MODE_AWARE;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_eth_policer_glbl_profile_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Map Ethernet policer per ingress port and Ethernet type 
 *     to Ethernet policer Global Profile.                     
 *     Details: in the H file. (search for prototype)          
*********************************************************************/
uint32  
  arad_pp_mtr_eth_policer_glbl_profile_map_set_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  ARAD_PP_PORT            port_ndx,
    SOC_SAND_IN  ARAD_PP_MTR_ETH_TYPE    eth_type_ndx,
    SOC_SAND_IN  uint32                  glbl_profile_idx
  )
{
  uint32
    config_tbl_offset,
    res = SOC_SAND_OK,
    reg_val,
    glbl_mtr_ptr_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET_UNSAFE);

  config_tbl_offset = port_ndx * ARAD_PP_NOF_MTR_ETH_TYPES + eth_type_ndx;

  /* Get pointers from config table*/
  res = READ_IDR_ETHERNET_METER_CONFIGm(unit, MEM_BLOCK_ANY, config_tbl_offset, &reg_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  glbl_mtr_ptr_val = glbl_profile_idx;
  soc_mem_field_set(unit, IDR_ETHERNET_METER_CONFIGm, &reg_val, GLOBAL_METER_PTRf, &glbl_mtr_ptr_val);

  res = WRITE_IDR_ETHERNET_METER_CONFIGm(unit, MEM_BLOCK_ANY, config_tbl_offset, &reg_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_eth_policer_glbl_profile_map_set_unsafe()", 0, 0);
}

uint32  
  arad_pp_mtr_eth_policer_glbl_profile_map_set_verify(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  ARAD_PP_PORT            port_ndx,
    SOC_SAND_IN  ARAD_PP_MTR_ETH_TYPE    eth_type_ndx,
    SOC_SAND_IN  uint32                  glbl_profile_idx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(eth_type_ndx, ARAD_PP_METERING_ETH_TYPE_NDX_MAX, ARAD_PP_METERING_ETH_TYPE_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(glbl_profile_idx, ARAD_PP_METERING_GLBL_PROFILE_IDX_MAX, ARAD_PP_METERING_GLBL_PROFILE_IDX_OUT_OF_RANGE_ERR, 20, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_eth_policer_glbl_profile_map_set_verify()", 0, 0);
}

uint32  
  arad_pp_mtr_eth_policer_glbl_profile_map_get_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  ARAD_PP_PORT                port_ndx,
    SOC_SAND_IN  ARAD_PP_MTR_ETH_TYPE        eth_type_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(eth_type_ndx, ARAD_PP_METERING_ETH_TYPE_NDX_MAX, ARAD_PP_METERING_ETH_TYPE_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_eth_policer_glbl_profile_map_get_verify()", 0, 0);
}

/*********************************************************************
*     Map Ethernet policer per ingress port and Ethernet type 
 *     to Ethernet policer Global Profile.                     
 *     Details: in the H file. (search for prototype)          
*********************************************************************/
uint32  
  arad_pp_mtr_eth_policer_glbl_profile_map_get_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  ARAD_PP_PORT            port_ndx,
    SOC_SAND_IN  ARAD_PP_MTR_ETH_TYPE    eth_type_ndx,
    SOC_SAND_OUT uint32                  *glbl_profile_idx
  )
{
  uint32
    config_tbl_offset,
    res = SOC_SAND_OK,
    reg_val,
    glbl_mtr_ptr_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_PP_MTR_ETH_POLICER_GLBL_PROFILE_MAP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(glbl_profile_idx);

  config_tbl_offset = port_ndx * ARAD_PP_NOF_MTR_ETH_TYPES + eth_type_ndx;

  /* Get pointers from config table */
  res = READ_IDR_ETHERNET_METER_CONFIGm(unit, MEM_BLOCK_ANY, config_tbl_offset, &reg_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  soc_mem_field_get(unit, IDR_ETHERNET_METER_CONFIGm, &reg_val, GLOBAL_METER_PTRf, &glbl_mtr_ptr_val);
  *glbl_profile_idx = glbl_mtr_ptr_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_pp_mtr_eth_policer_glbl_profile_map_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     arad_pp_api_metering module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_metering_get_procs_ptr(void)
{
  return Arad_pp_procedure_desc_element_metering;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     arad_pp_api_metering module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_metering_get_errs_ptr(void)
{
  return Arad_pp_error_desc_element_metering;
}

uint32
  ARAD_PP_MTR_METER_ID_verify(
    SOC_SAND_IN  ARAD_PP_MTR_METER_ID *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->group, ARAD_PP_METERING_GROUP_MAX, ARAD_PP_METERING_GROUP_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->id, ARAD_PP_METERING_ID_MAX, ARAD_PP_METERING_ID_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PP_MTR_METER_ID_verify()",0,0);
}


uint32
  ARAD_PP_MTR_GROUP_INFO_verify(
    SOC_SAND_IN  ARAD_PP_MTR_GROUP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  /*SOC_SAND_ERR_IF_ABOVE_MAX(info->rand_mode_enable, ARAD_PP_METERING_MAX_PACKET_SIZE_MAX, ARAD_PP_METERING_MAX_PACKET_SIZE_OUT_OF_RANGE_ERR, 11, exit);*/

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PP_MTR_GROUP_INFO_verify()",0,0);
}

uint32
  ARAD_PP_MTR_BW_PROFILE_INFO_verify(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  ARAD_PP_MTR_BW_PROFILE_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->cir, ARAD_IF_MAX_RATE_KBPS(unit), ARAD_PP_METERING_CIR_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_BELOW_MIN(info->cir, ARAD_PP_BW_PROFILE_ADD_VERIFY_IR_MIN, ARAD_PP_METERING_CIR_OUT_OF_RANGE_ERR, 20, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->eir, ARAD_IF_MAX_RATE_KBPS(unit), ARAD_PP_METERING_EIR_OUT_OF_RANGE_ERR, 30, exit);
  SOC_SAND_ERR_IF_BELOW_MIN(info->eir, ARAD_PP_BW_PROFILE_ADD_VERIFY_IR_MIN, ARAD_PP_METERING_EIR_OUT_OF_RANGE_ERR, 40, exit);

  if (info->is_sharing_enabled && (info->max_cir != ARAD_PP_BW_PROFILE_IR_MAX_UNLIMITED)) {
      SOC_SAND_ERR_IF_ABOVE_MAX(info->max_cir, ARAD_IF_MAX_RATE_KBPS(unit), ARAD_PP_METERING_CIR_OUT_OF_RANGE_ERR, 15, exit);
      SOC_SAND_ERR_IF_BELOW_MIN(info->max_cir, ARAD_PP_BW_PROFILE_ADD_VERIFY_IR_MIN, ARAD_PP_METERING_CIR_OUT_OF_RANGE_ERR, 25, exit);
  }

  if (info->max_eir != ARAD_PP_BW_PROFILE_IR_MAX_UNLIMITED) {
      SOC_SAND_ERR_IF_ABOVE_MAX(info->max_eir, ARAD_IF_MAX_RATE_KBPS(unit), ARAD_PP_METERING_EIR_OUT_OF_RANGE_ERR, 35, exit);
      SOC_SAND_ERR_IF_BELOW_MIN(info->max_eir, ARAD_PP_BW_PROFILE_ADD_VERIFY_IR_MIN, ARAD_PP_METERING_EIR_OUT_OF_RANGE_ERR, 45, exit);
  }

  if (info->eir > info->max_eir) {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_METERING_MAX_IR_INVALID_ERR, 95, exit);
  }
  if (info->is_sharing_enabled && (info->cir > info->max_cir)) {
      SOC_SAND_SET_ERROR_CODE(ARAD_PP_METERING_MAX_IR_INVALID_ERR, 100, exit);
  }

  SOC_SAND_ERR_IF_ABOVE_MAX(info->cbs, ARAD_PP_BW_PROFILE_ADD_VERIFY_BS_MAX, ARAD_PP_METERING_CBS_OUT_OF_RANGE_ERR, 50, exit);
  SOC_SAND_ERR_IF_BELOW_MIN(info->cbs, ARAD_PP_BW_PROFILE_ADD_VERIFY_BS_MIN, ARAD_PP_METERING_CBS_OUT_OF_RANGE_ERR, 60, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->ebs, ARAD_PP_BW_PROFILE_ADD_VERIFY_BS_MAX, ARAD_PP_METERING_EBS_OUT_OF_RANGE_ERR, 70, exit);
  SOC_SAND_ERR_IF_BELOW_MIN(info->ebs, ARAD_PP_BW_PROFILE_ADD_VERIFY_BS_MIN, ARAD_PP_METERING_EBS_OUT_OF_RANGE_ERR, 80, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->color_mode, ARAD_PP_METERING_COLOR_MODE_MAX, ARAD_PP_METERING_COLOR_MODE_OUT_OF_RANGE_ERR, 90, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_PP_MTR_BW_PROFILE_INFO_verify()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif /* of #if defined(BCM_88650_A0) */

