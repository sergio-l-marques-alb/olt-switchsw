#include <shared/bsl.h>

#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)
/* $Id: arad_ingress_traffic_mgmt.c,v 1.85 Broadcom SDK $
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

#define _ERR_MSG_MODULE_NAME BSL_SOC_COSQ

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/dpp_wb_engine.h>
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/ARAD/arad_ingress_traffic_mgmt.h>

#include <soc/dpp/ARAD/arad_ingress_packet_queuing.h>

#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_tbl_access.h>
#include <soc/dpp/ARAD/arad_general.h>
#include <soc/dpp/ARAD/arad_api_mgmt.h>
#include <soc/dpp/ARAD/arad_sw_db.h>
#include <soc/dpp/ARAD/arad_chip_tbls.h>
#include <soc/dpp/ARAD/arad_fabric.h>
#include <soc/dpp/ARAD/arad_parser.h>
#include <soc/mem.h>
#include <soc/dpp/ARAD/arad_init.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_metering.h>

#include <soc/dpp/SAND/Utils/sand_u64.h>

#include <soc/dpp/SAND/Utils/sand_conv.h>


/* } */

/*************
 * DEFINES   *
 *************/
/* { */

/*
 *
 */


/*
 * max value for BDs size( for tail drop)
 */
#define ARAD_ITM_MAX_INST_Q_BDS_SIZE 0x3F80000

/* Max & min values for end of category queues:      */
#define ARAD_ITM_CATEGORY_INFO_VSQ_CTGRY_END_MIN 0
#define ARAD_ITM_CATEGORY_INFO_VSQ_CTGRY_END_MAX(unit) (SOC_DPP_DEFS_GET(unit, nof_queues)-1)


/* Max & min values for struct ARAD_ITM_WRED_QT_DP_INFO:      */
#define ARAD_ITM_WRED_QT_DP_INFO_MAX_PROBABILITY_MAX 100

/* Max value WRED max packet size:      */
#define ARAD_ITM_WRED_MAX_PACKET_SIZE ((1<<14)-128)
#define ARAD_ITM_WRED_MAX_PACKET_SIZE_FOR_CALC ARAD_ITM_WRED_MAX_PACKET_SIZE

/* Max values WRED probability:      */
#define ARAD_ITM_WRED_MAX_PROB 100


/* Max & min values for struct ARAD_ITM_PRIORITY_MAP_TMPLT:      */
#define ARAD_ITM_PRIORITY_MAP_NDX_MAX 3

#define ARAD_ITM_PRIORITY_MAP_TMPLT_MAP_MAX SOC_SAND_U32_MAX

/* System Red drop probability values. Range: 0 - 0xffff,
 * when x is (x - 1)/(64K - 1) percent.
 */
#define ARAD_ITM_SYS_RED_DROP_PROB_VAL_MAX 0xffff

#define ARAD_ITM_SYS_RED_QUEUE_TH_MAX 15

/* Max & min values for struct ARAD_ITM_SYS_RED_EG_INFO for CLK=250M (4ns) :
   This is the register's max, to see the actual max value in milliseconds please refer to function
   arad_itm_sys_red_eg_verify */
#define ARAD_ITM_SYS_RED_EG_INFO_AGING_TIMER_MAX 0x1FFFFF

/* Aging Timer Coefficient is use to convert register value to seconds */
#define ARAD_ITM_SYS_RED_EG_INFO_AGING_TIMER_CONVERT_COEFFICIENT (1296 * 4)

/* Max & min values for struct ARAD_ITM_SYS_RED_GLOB_RCS_THS:      */
#define ARAD_ITM_SYS_RED_GLOB_RCS_THS_UNICAST_RNG_THS_MAX 0x1FFFFF

#define ARAD_ITM_SYS_RED_GLOB_RCS_THS_MULTICAST_RNG_THS_MAX 0xFFFF

#define ARAD_ITM_SYS_RED_GLOB_RCS_THS_BDS_RNG_THS_MAX 0xFFFF

/* Max & min values for struct ARAD_ITM_SYS_RED_GLOB_RCS_VALS:      */
#define ARAD_ITM_SYS_RED_GLOB_RCS_VALS_RNG_VALS_MAX 15

#define ARAD_ITM_CR_DISCOUNT_MAX_VAL  127
#define ARAD_ITM_CR_DISCOUNT_MIN_VAL  -127

#define ARAD_ITM_HUNGRY_TH_MNT_MSB       6
#define ARAD_ITM_HUNGRY_TH_MNT_LSB       4
#define ARAD_ITM_HUNGRY_TH_MNT_NOF_BITS  (ARAD_ITM_HUNGRY_TH_MNT_MSB - ARAD_ITM_HUNGRY_TH_MNT_LSB + 1)
#define ARAD_ITM_HUNGRY_TH_EXP_MSB       3
#define ARAD_ITM_HUNGRY_TH_EXP_LSB       0
#define ARAD_ITM_HUNGRY_TH_EXP_NOF_BITS  (ARAD_ITM_HUNGRY_TH_EXP_MSB - ARAD_ITM_HUNGRY_TH_EXP_LSB + 1)

#define ARAD_ITM_HUNGRY_TH_MNT_MAX     ((1<<ARAD_ITM_HUNGRY_TH_MNT_NOF_BITS)-1)
#define ARAD_ITM_HUNGRY_TH_EXP_MAX     ((1<<ARAD_ITM_HUNGRY_TH_EXP_NOF_BITS)-1)
#define ARAD_ITM_HUNGRY_TH_MAX         \
  (ARAD_ITM_HUNGRY_TH_MNT_MAX * (1<<ARAD_ITM_HUNGRY_TH_EXP_MAX))

#define ARAD_ITM_HUNGRY_TH_MULTIPLIER_VAL_TO_FIELD(val)             \
  ((val) < (1 << (ARAD_ITM_HUNGRY_TH_MULTIPLIER_OFFSET + 1)) ? 0 : \
  soc_sand_log2_round_down(val) - ARAD_ITM_HUNGRY_TH_MULTIPLIER_OFFSET)
#define ARAD_ITM_HUNGRY_TH_MULTIPLIER_FIELD_TO_VAL(field)          \
  (field ? SOC_SAND_BIT(field + ARAD_ITM_HUNGRY_TH_MULTIPLIER_OFFSET) : 0);

#define ARAD_ITM_SATISFIED_TH_MNT_MSB       7
#define ARAD_ITM_SATISFIED_TH_MNT_LSB       4
#define ARAD_ITM_SATISFIED_TH_MNT_NOF_BITS  (ARAD_ITM_SATISFIED_TH_MNT_MSB - ARAD_ITM_SATISFIED_TH_MNT_LSB + 1)
#define ARAD_ITM_SATISFIED_TH_EXP_MSB       3
#define ARAD_ITM_SATISFIED_TH_EXP_LSB       0
#define ARAD_ITM_SATISFIED_TH_EXP_NOF_BITS  (ARAD_ITM_SATISFIED_TH_EXP_MSB - ARAD_ITM_SATISFIED_TH_EXP_LSB + 1)

#define ARAD_ITM_SATISFIED_TH_MNT_MAX     ((1<<ARAD_ITM_SATISFIED_TH_MNT_NOF_BITS)-1)
#define ARAD_ITM_SATISFIED_TH_EXP_MAX     ((1<<ARAD_ITM_SATISFIED_TH_EXP_NOF_BITS)-1)
#define ARAD_ITM_SATISFIED_TH_MAX         \
  (ARAD_ITM_SATISFIED_TH_MNT_MAX * (1<<ARAD_ITM_SATISFIED_TH_EXP_MAX))

#define ARAD_ITM_WRED_GRANULARITY        16

#define ARAD_ITM_MIN_WRED_AVRG_TH_MNT_MSB       6
#define ARAD_ITM_MIN_WRED_AVRG_TH_MNT_LSB       0
#define ARAD_ITM_MIN_WRED_AVRG_TH_MNT_NOF_BITS  (ARAD_ITM_MIN_WRED_AVRG_TH_MNT_MSB - ARAD_ITM_MIN_WRED_AVRG_TH_MNT_LSB + 1)
#define ARAD_ITM_MIN_WRED_AVRG_TH_EXP_MSB       11
#define ARAD_ITM_MIN_WRED_AVRG_TH_EXP_LSB       7
#define ARAD_ITM_MIN_WRED_AVRG_TH_EXP_NOF_BITS  (ARAD_ITM_MIN_WRED_AVRG_TH_EXP_MSB - ARAD_ITM_MIN_WRED_AVRG_TH_EXP_LSB + 1)

#define ARAD_ITM_MAX_WRED_AVRG_TH_MNT_MSB       6
#define ARAD_ITM_MAX_WRED_AVRG_TH_MNT_LSB       0
#define ARAD_ITM_MAX_WRED_AVRG_TH_MNT_NOF_BITS  (ARAD_ITM_MAX_WRED_AVRG_TH_MNT_MSB - ARAD_ITM_MAX_WRED_AVRG_TH_MNT_LSB + 1)
#define ARAD_ITM_MAX_WRED_AVRG_TH_EXP_MSB       11
#define ARAD_ITM_MAX_WRED_AVRG_TH_EXP_LSB       7
#define ARAD_ITM_MAX_WRED_AVRG_TH_EXP_NOF_BITS  (ARAD_ITM_MAX_WRED_AVRG_TH_EXP_MSB - ARAD_ITM_MAX_WRED_AVRG_TH_EXP_LSB + 1)

/*2^32/100 is 42949672.96 ==> 42949673 ==> 0x28F5C29*/
#define ARAD_WRED_NORMALIZE_FACTOR  (0x28F5C29)

#define ARAD_ITM_VSQ_MIN_WRED_AVRG_TH_MNT_MSB       59
#define ARAD_ITM_VSQ_MIN_WRED_AVRG_TH_MNT_LSB       53
#define ARAD_ITM_VSQ_MIN_WRED_AVRG_TH_MNT_NOF_BITS  (ARAD_ITM_VSQ_MIN_WRED_AVRG_TH_MNT_MSB - ARAD_ITM_VSQ_MIN_WRED_AVRG_TH_MNT_LSB + 1)
#define ARAD_ITM_VSQ_MIN_WRED_AVRG_TH_EXP_MSB       64
#define ARAD_ITM_VSQ_MIN_WRED_AVRG_TH_EXP_LSB       60
#define ARAD_ITM_VSQ_MIN_WRED_AVRG_TH_EXP_NOF_BITS  (ARAD_ITM_VSQ_MIN_WRED_AVRG_TH_EXP_MSB - ARAD_ITM_VSQ_MIN_WRED_AVRG_TH_EXP_LSB + 1)


#define ARAD_ITM_VSQ_MAX_WRED_AVRG_TH_MNT_MSB       47
#define ARAD_ITM_VSQ_MAX_WRED_AVRG_TH_MNT_LSB       41
#define ARAD_ITM_VSQ_MAX_WRED_AVRG_TH_MNT_NOF_BITS  (ARAD_ITM_VSQ_MAX_WRED_AVRG_TH_MNT_MSB - ARAD_ITM_VSQ_MAX_WRED_AVRG_TH_MNT_LSB + 1)
#define ARAD_ITM_VSQ_MAX_WRED_AVRG_TH_EXP_MSB       52
#define ARAD_ITM_VSQ_MAX_WRED_AVRG_TH_EXP_LSB       48
#define ARAD_ITM_VSQ_MAX_WRED_AVRG_TH_EXP_NOF_BITS  (ARAD_ITM_VSQ_MAX_WRED_AVRG_TH_EXP_MSB - ARAD_ITM_VSQ_MAX_WRED_AVRG_TH_EXP_LSB + 1)


#define ARAD_ITM_WD_INFO_MIN_SCAN_CYCLE_PERIOD_MICRO_GRANULARITY   2048
#define ARAD_ITM_CREDIT_WATCHDOG_MAX_THRESHOLD_VALUE 15


/* System Red Boundaries */
#define ARAD_ITM_SYS_RED_Q_BOUNDARY_TH_MNT_MSB   6
#define ARAD_ITM_SYS_RED_Q_BOUNDARY_TH_MNT_LSB   0
#define ARAD_ITM_VSQ_SYS_RED_Q_BOUNDARY_TH_MNT_NOF_BITS (ARAD_ITM_SYS_RED_Q_BOUNDARY_TH_MNT_MSB - ARAD_ITM_SYS_RED_Q_BOUNDARY_TH_MNT_LSB + 1)

#define ARAD_ITM_SYS_RED_Q_BOUNDARY_TH_EXP_MSB   11
#define ARAD_ITM_SYS_RED_Q_BOUNDARY_TH_EXP_LSB   7
#define ARAD_ITM_VSQ_SYS_RED_Q_BOUNDARY_TH_EXP_NOF_BITS (ARAD_ITM_SYS_RED_Q_BOUNDARY_TH_EXP_MSB - ARAD_ITM_SYS_RED_Q_BOUNDARY_TH_EXP_LSB + 1)

#define ARAD_ITM_TEST_CTGRY_TEST_EN_BIT          0
#define ARAD_ITM_TEST_CTGRY_TRFFC_TEST_EN_BIT    1
#define ARAD_ITM_TEST_CTGRY2_3_CNCTN_TEST_EN_BIT 2
#define ARAD_ITM_TEST_STTSTCS_TAG_TEST_EN_BIT    3
#define ARAD_ITM_TEST_LLFC_TEST_EN_BIT           4
#define ARAD_ITM_TEST_PFC_TEST_EN_BIT            5

/* (2^22 - 1) */
#define ARAD_ITM_VSQ_FC_BD_SIZE_MAX              0X3FFFFF
#define ARAD_ITM_VSQ_FC_Q_SIZE_MAX 0XFFFFFFFF


/* The WRED threshold limit foe queues is 2GB and for VSQs it is 4GB */
#define ARAD_Q_WRED_INFO_MIN_AVRG_TH_MAX       0x80000000
#define ARAD_Q_WRED_INFO_MAX_AVRG_TH_MAX       0x80000000

/* (2^24 - 1) */
#define ARAD_ITM_GLOB_RCS_FC_BDBS_SIZE_MAX       0XFFFFFF

/* (2^23 - 1) */
#define ARAD_ITM_GLOB_RCS_FC_UC_SIZE_MAX         0X7FFFFF

/* (2^24 - 1) */
#define ARAD_ITM_GLOB_RCS_FC_FMC_SIZE_MAX        0XFFFFFF

/* (2^24 - 1) */
#define ARAD_ITM_GLOB_RCS_DROP_BDBS_SIZE_MAX     0XFFFFFF

/* (2^16 - 1) *(2 ^ 15) */
#define ARAD_ITM_GLOB_RCS_DROP_BDS_SIZE_MAX      0x7FF8000 

/* (2^23 - 1) */
#define ARAD_ITM_GLOB_RCS_DROP_UC_SIZE_MAX       0X7FFFFF

/* (2^22 - 1) */
#define ARAD_ITM_GLOB_RCS_DROP_MINI_MC_SIZE_MAX  0X3FFFFF

/* (2^24 - 1) */
#define ARAD_ITM_GLOB_RCS_DROP_FMC_SIZE_MAX      0XFFFF

/* (2^15 - 1) */
#define ARAD_ITM_GLOB_RCS_DROP_OCB_SIZE_MAX      0X7FFF


#define ARAD_ITM_GRNT_BYTES_MAX                                  (256*1024*1024)
#define ARAD_ITM_GRNT_BDS_MAX                                    (2*1024*1024)
#define ARAD_ITM_GRNT_BDS_OR_DBS_DISABLED                        0xffffff
#define ARAD_ITM_GRNT_BDS_OR_DBS_MANTISSA_BITS                   8
#define ARAD_ITM_GRNT_BDS_OR_DBS_EXPONENT_BITS                   4
#define ARAD_ITM_GLOB_RCS_DROP_EXCESS_MEM_SIZE_MAX               (128*1024*1024)

#define ARAD_ITM_COMMITTED_BYTES_MANTISSA_NOF_BITS               6
#define ARAD_ITM_COMMITTED_BYTES_EXPONENT_NOF_BITS               5
#define ARAD_ITM_COMMITTED_BYTES_RESOLUTION                      16
#define ARAD_ITM_COMMITTED_BDS_MANTISSA_NOF_BITS                 8
#define ARAD_ITM_COMMITTED_BDS_EXPONENT_NOF_BITS                 4

#define ARAD_ITM_MC_HP_FIFO_DP_0_THRESHOLD(limit)                (((limit)*2)/10)
#define ARAD_ITM_MC_HP_FIFO_DP_1_THRESHOLD(limit)                (((limit)*4)/10)
#define ARAD_ITM_MC_HP_FIFO_DP_2_THRESHOLD(limit)                (((limit)*6)/10)
#define ARAD_ITM_MC_HP_FIFO_DP_3_THRESHOLD(limit)                (((limit)*8)/10)
#define ARAD_ITM_MC_LP_FIFO_DP_0_THRESHOLD(limit)                (((limit)*2)/10)
#define ARAD_ITM_MC_LP_FIFO_DP_1_THRESHOLD(limit)                (((limit)*4)/10)
#define ARAD_ITM_MC_LP_FIFO_DP_2_THRESHOLD(limit)                (((limit)*6)/10)
#define ARAD_ITM_MC_LP_FIFO_DP_3_THRESHOLD(limit)                (((limit)*8)/10)

#define ARAD_ITM_NOF_REASSEMBLY_CONTEXTS                         (192)

/* Mimimum and maximum clock cycles allowed by to scan each queue in credit watchdog */
#define ARAD_CREDIT_WATCHDOG_MIN_Q_SCAN_CYCLES 2
#define ARAD_CREDIT_WATCHDOG_MAX_Q_SCAN_CYCLES 255

/* translate from nano seconds to millliseconds rounding down except for a rounding error of up to 0.05 ms */
#define NS_TO_MS_ROUND(nano) (((nano) + 50000) / 1000000); /* round to n from n-0.05 to n+0.95 */


/* } */

/*************
 *  MACROS   *
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

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

/*********************************************************************
* NAME:
*     arad_itm_regs_init
* FUNCTION:
*   Initialization of the Arad blocks configured in this module.
*   This function directly accesses registers/tables for
*   initializations that are not covered by API-s
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Called as part of the initialization sequence.
*********************************************************************/
STATIC uint32
  arad_itm_regs_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    start_prev,
    size_prev,
    i,
    pfc_tc,
    data,
    res;
  uint32
    reg_idx;
  char 
    *propkey,
    *propval;
  uint32 
    ftmh_stmping; 
  uint8
    is_mesh,
    is_single_cntxt;
  soc_field_t
      dqcq_field_1[ARAD_REGS_DBUFF_PTR_Q_THRESH_NOF_REGS] = 
      {DPQ_DQCQ_TH_1f, DPQ_DQCQ_TH_3f, DPQ_DQCQ_TH_11f, DPQ_DQCQ_TH_13f, DPQ_DQCQ_TH_15f, 
          DPQ_DQCQ_TH_5f, DPQ_DQCQ_TH_7f, DPQ_DQCQ_TH_9f};
   
    
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_REGS_INIT);

  

  /*
   *  IQM
   */

  /* Update FIFO Full Threshold */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  9,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_REG_0124r, REG_PORT_ANY, 0, FIELD_7_11f,  0xe));

  /* Admit Disable */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,WRITE_IQM_REJECT_ADMISSION_Ar(unit, 0x3F3F3F3F));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  11,  exit, ARAD_REG_ACCESS_ERR,WRITE_IQM_REJECT_ADMISSION_Br(unit,  0x3F3F3F3F));

  /* VSQ STE enablers */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  15,  exit, ARAD_REG_ACCESS_ERR,READ_IQM_STE_ENABLERSr(unit, SOC_CORE_ALL, &data));
  data |= 0x3F; /* Enable VSQ A-F */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,WRITE_IQM_STE_ENABLERSr(unit, SOC_CORE_ALL,  data));

  /* NIFTCM */
  for(i=0; i< ARAD_NOF_LOCAL_PORTS; ++i)
  {
    pfc_tc = i & (ARAD_NOF_TRAFFIC_CLASSES - 1);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 1000, exit, WRITE_IQM_NIFTCMm(unit, MEM_BLOCK_ANY, i, &pfc_tc));
  }

  /*
   *  IPT
   */
  /* Enable stamping Fabric Header */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  42,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_STAMPING_FABRIC_HEADER_ENABLEr, REG_PORT_ANY, 0, STAMP_FAP_PORTf,  0x1));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  44,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_STAMPING_FABRIC_HEADER_ENABLEr, REG_PORT_ANY, 0, STAMP_FWDACTIONf,  0x1));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  46,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_STAMPING_FTMH_OUTLIF_ENABLEr, REG_PORT_ANY, 0, STAMP_FTMH_OUTLIF_TYPE_ENf,  0x7775));

  /* XGS CUD stamping mode */
    propkey = spn_XGS_COMPATABILITY_STAMP_CUD;
    propval = soc_property_get_str(unit, propkey);

    if (propval) {
        if (sal_strcmp(propval, "True") == 0) {
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  45,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_ARAD_PACKETS_FORMATr, REG_PORT_ANY, 0, STAMP_OUTLIF_XGS_USR_DEF_ENf,  0x1));
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  46,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_STAMPING_FABRIC_HEADER_ENABLEr, REG_PORT_ANY, 0, STAMP_USER_DEFINED_LSBf,  512-72-48));
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  47,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_STAMPING_FABRIC_HEADER_ENABLEr, REG_PORT_ANY, 0, STAMP_USER_DEFINED_MSBf,  16));
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  48,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_STAMPING_USR_DEF_OUTLIF_ENABLEr, REG_PORT_ANY, 0, STAMP_USR_DEF_OUTLIF_TYPE_ENf,  0x0));

            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  49,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPT_STAMPING_FTMH_OUTLIF_ENABLEr, REG_PORT_ANY, 0, STAMP_FTMH_OUTLIF_TYPE_ENf, &ftmh_stmping));
            SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_STAMPING_FTMH_OUTLIF_ENABLEr, REG_PORT_ANY, 0, STAMP_FTMH_OUTLIF_TYPE_ENf,  ftmh_stmping & ~0x3));            
          
        } else if (sal_strcmp(propval, "False") == 0) {
            /* nothing to do */        
        } else {            
            return SOC_E_FAIL;
        }
    }

  /* Set snooping action recognition */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_MAPPING_QUEUE_TYPE_TO_SNOOP_PACKETr, REG_PORT_ANY, 0, SNOOP_TABLEf,  SOC_SAND_BIT(ARAD_ITM_FWD_ACTION_TYPE_SNOOP)));

  is_mesh = SOC_SAND_NUM2BOOL(SOC_DPP_IS_MESH((unit)));
  is_single_cntxt = SOC_SAND_NUM2BOOL(!is_mesh);

  if (is_single_cntxt)
  {
    /*
     *    CLOS or Mesh with legacy coexist
     */

    /*
     *    DQCQ
     */
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG_1r, REG_PORT_ANY, 0, DEST_0_DEPTHf,  0x200));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  81,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG_1r, REG_PORT_ANY, 0, DEST_1_DEPTHf,  0x200));

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  83,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG_2r, REG_PORT_ANY, 0, DEST_2_DEPTHf,  0x200));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  84,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG_2r, REG_PORT_ANY, 0, DEST_3_DEPTHf,  0x200));

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  85,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG_3r, REG_PORT_ANY, 0, DEST_4_DEPTHf,  0x0));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  86,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG_3r, REG_PORT_ANY, 0, DEST_5_DEPTHf,  0x0));

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  87,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG_4r, REG_PORT_ANY, 0, DEST_6_DEPTHf,  0x0));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  88,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_LOW_PRIORITY_DQCQ_DEPTH_CONFIG_4r, REG_PORT_ANY, 0, DEST_7_DEPTHf,  0x0));

    /*
     *    TX Queue-Size
     */
    start_prev = 0x0;
    size_prev  = 0x190;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  110,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_TRANSMIT_DATA_QUEUE_START_ADRESS_0_1r, REG_PORT_ANY,  0, DTQ_START_0f + 0,  start_prev));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  111,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_TRANSMIT_DATA_QUEUE_SIZE_0_1r, REG_PORT_ANY,  0, DTQ_SIZE_0f + 0,  size_prev));

    start_prev = start_prev + size_prev + 1;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  112,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_TRANSMIT_DATA_QUEUE_START_ADRESS_0_1r, REG_PORT_ANY,  0, DTQ_START_0f + 1,  start_prev));
    size_prev = 0x10;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,   113,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_TRANSMIT_DATA_QUEUE_SIZE_0_1r, REG_PORT_ANY,  0, DTQ_SIZE_0f + 1,  size_prev));

    for (reg_idx = 1; reg_idx < ARAD_TRANSMIT_DATA_QUEUE_NOF_REGS; reg_idx++)
    {
        start_prev = start_prev + size_prev + 1;
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  114,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_TRANSMIT_DATA_QUEUE_START_ADRESS_0_1r + ((reg_idx != ARAD_TRANSMIT_DATA_QUEUE_NOF_REGS-1)?(reg_idx / 2):(-1)), REG_PORT_ANY,  0, DTQ_START_0f + reg_idx,  start_prev));
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  116+reg_idx,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_TRANSMIT_DATA_QUEUE_SIZE_0_1r + ((reg_idx != ARAD_TRANSMIT_DATA_QUEUE_NOF_REGS-1)?(reg_idx / 2):(-1)), REG_PORT_ANY,  0, DTQ_SIZE_0f + reg_idx,  size_prev));
    }
  }
  else
  {
    if (is_mesh == TRUE)
    {
      for (reg_idx = 0; reg_idx < ARAD_REGS_DBUFF_PTR_Q_THRESH_NOF_REGS; reg_idx++)
      {
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  100+reg_idx,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPT_DRAM_BUFFER_POINTER_QUEUE_DQCQ_THRESHOLD_01r + reg_idx, REG_PORT_ANY, 0, dqcq_field_1[reg_idx],  0x7f)); 
      }
    }
  }

  /*
   *    In 64 bytes resolution. For 1024B, set to 16.
   */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  120,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_STORED_CREDITS_USAGE_CONFIGURATIONr, SOC_CORE_ALL, 0, MUL_PKT_DEQ_BYTESf,  16));

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  150, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_2r, REG_PORT_ANY, 0, ISP_QNUM_LOWf,  0x17fef)); /* 96K-17 to allow last 16 queues for workaround */
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  152, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_3r, REG_PORT_ANY, 0, ISP_QNUM_HIGHf,  0x17fef));


  /* Fill table ITMPM with 0x0 values */
  data = 0;
  res = arad_fill_table_with_entry(unit, IQM_ITMPMm, MEM_BLOCK_ANY, &data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit)) 
  {
      uint32 
          mcr_limit_uc,
          mcr_limit_mc_hp,
          mcr_limit_mc_hp_size,
          mcr_limit_mc_lp_size;
      soc_reg_above_64_val_t 
          fld_above_64_val, 
          reg_above_64_val;
      SOC_REG_ABOVE_64_CLEAR(reg_above_64_val);
      SOC_REG_ABOVE_64_CLEAR(fld_above_64_val);

      /* 
       * IRDP - Multicast packets are marked as erroneous (dropped) according threshold configuration 
       * Threshold configuration to is according MC priority and DP   
       * 
       */
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  130,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IRR_COMPATIBILITY_REGISTERr, REG_PORT_ANY, 0, SYS_CONFIG_2f,  0x1));

      /*The unicast, multicast high and multicast low FIFOs share one memory of 768 entries.*/
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  190,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IRR_MCR_FIFO_CONFIGr, REG_PORT_ANY, 0, MCR_LIMIT_UCf, &mcr_limit_uc));    
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  191,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IRR_MCR_FIFO_CONFIGr, REG_PORT_ANY, 0, MCR_LIMIT_MC_HPf, &mcr_limit_mc_hp));
      mcr_limit_mc_hp_size = mcr_limit_mc_hp - mcr_limit_uc;
      mcr_limit_mc_lp_size = (768 - mcr_limit_mc_hp);
      
      /*Set relevant fields in static configuration*/
      ARAD_FLD_FROM_REG_ABOVE_64(IRR_MULTICAST_FIFO_THRESHOLDr, MC_HP_FIFO_DP_0_THRESHOLDf, fld_above_64_val ,reg_above_64_val, 200, exit);
      *fld_above_64_val = ARAD_ITM_MC_HP_FIFO_DP_0_THRESHOLD(mcr_limit_mc_hp_size);
      ARAD_FLD_TO_REG_ABOVE_64(  IRR_MULTICAST_FIFO_THRESHOLDr, MC_HP_FIFO_DP_0_THRESHOLDf, fld_above_64_val ,reg_above_64_val, 201, exit);

      ARAD_FLD_FROM_REG_ABOVE_64(IRR_MULTICAST_FIFO_THRESHOLDr, MC_HP_FIFO_DP_1_THRESHOLDf, fld_above_64_val ,reg_above_64_val, 210, exit);
      *fld_above_64_val = ARAD_ITM_MC_HP_FIFO_DP_1_THRESHOLD(mcr_limit_mc_hp_size);
      ARAD_FLD_TO_REG_ABOVE_64(  IRR_MULTICAST_FIFO_THRESHOLDr, MC_HP_FIFO_DP_1_THRESHOLDf, fld_above_64_val ,reg_above_64_val, 211, exit);

      ARAD_FLD_FROM_REG_ABOVE_64(IRR_MULTICAST_FIFO_THRESHOLDr, MC_HP_FIFO_DP_2_THRESHOLDf, fld_above_64_val ,reg_above_64_val, 220, exit);
      *fld_above_64_val = ARAD_ITM_MC_HP_FIFO_DP_2_THRESHOLD(mcr_limit_mc_hp_size);
      ARAD_FLD_TO_REG_ABOVE_64(  IRR_MULTICAST_FIFO_THRESHOLDr, MC_HP_FIFO_DP_2_THRESHOLDf, fld_above_64_val ,reg_above_64_val, 221, exit);

      ARAD_FLD_FROM_REG_ABOVE_64(IRR_MULTICAST_FIFO_THRESHOLDr, MC_HP_FIFO_DP_3_THRESHOLDf, fld_above_64_val ,reg_above_64_val, 230, exit);
      *fld_above_64_val = ARAD_ITM_MC_HP_FIFO_DP_3_THRESHOLD(mcr_limit_mc_hp_size);
      ARAD_FLD_TO_REG_ABOVE_64(  IRR_MULTICAST_FIFO_THRESHOLDr, MC_HP_FIFO_DP_3_THRESHOLDf, fld_above_64_val ,reg_above_64_val, 231, exit);

      ARAD_FLD_FROM_REG_ABOVE_64(IRR_MULTICAST_FIFO_THRESHOLDr, MC_LP_FIFO_DP_0_THRESHOLDf, fld_above_64_val ,reg_above_64_val, 240, exit);
      *fld_above_64_val = ARAD_ITM_MC_LP_FIFO_DP_0_THRESHOLD(mcr_limit_mc_lp_size);
      ARAD_FLD_TO_REG_ABOVE_64(  IRR_MULTICAST_FIFO_THRESHOLDr, MC_LP_FIFO_DP_0_THRESHOLDf, fld_above_64_val ,reg_above_64_val, 241, exit);

      ARAD_FLD_FROM_REG_ABOVE_64(IRR_MULTICAST_FIFO_THRESHOLDr, MC_LP_FIFO_DP_1_THRESHOLDf, fld_above_64_val ,reg_above_64_val, 250, exit);
      *fld_above_64_val = ARAD_ITM_MC_LP_FIFO_DP_1_THRESHOLD(mcr_limit_mc_lp_size);
      ARAD_FLD_TO_REG_ABOVE_64(  IRR_MULTICAST_FIFO_THRESHOLDr, MC_LP_FIFO_DP_1_THRESHOLDf, fld_above_64_val ,reg_above_64_val, 251, exit);

      ARAD_FLD_FROM_REG_ABOVE_64(IRR_MULTICAST_FIFO_THRESHOLDr, MC_LP_FIFO_DP_2_THRESHOLDf, fld_above_64_val ,reg_above_64_val, 260, exit);
      *fld_above_64_val = ARAD_ITM_MC_LP_FIFO_DP_2_THRESHOLD(mcr_limit_mc_lp_size);
      ARAD_FLD_TO_REG_ABOVE_64(  IRR_MULTICAST_FIFO_THRESHOLDr, MC_LP_FIFO_DP_2_THRESHOLDf, fld_above_64_val ,reg_above_64_val, 261, exit);

      ARAD_FLD_FROM_REG_ABOVE_64(IRR_MULTICAST_FIFO_THRESHOLDr, MC_LP_FIFO_DP_3_THRESHOLDf, fld_above_64_val ,reg_above_64_val, 270, exit);
      *fld_above_64_val = ARAD_ITM_MC_LP_FIFO_DP_3_THRESHOLD(mcr_limit_mc_lp_size);
      ARAD_FLD_TO_REG_ABOVE_64(  IRR_MULTICAST_FIFO_THRESHOLDr, MC_LP_FIFO_DP_3_THRESHOLDf, fld_above_64_val ,reg_above_64_val, 271, exit);
  }
#endif /* BCM_88660_A0 */
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_regs_init()",0,0);
}

/*********************************************************************
* NAME:
*     arad_itm_init
* FUNCTION:
*     Initialization of the Arad blocks configured in this module.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Called as part of the initialization sequence.
*********************************************************************/
uint32
  arad_itm_init(
    SOC_SAND_IN  int  unit
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_INIT_PDM_MODE         
      pdm_mode;
  uint32
    pdm_nof_entries,
    pdm_mode_fld,
	mem_excess_size,
    idx;
  ARAD_ITM_GLOB_RCS_DROP_TH
    glbl_drop, glbl_drop_exact;
  ARAD_ITM_GLOB_RCS_FC_TH
    glbl_fc, glbl_fc_exact;
  ARAD_ITM_VSQ_GROUP
    vsq_group;
  ARAD_ITM_VSQ_FC_INFO
    vsq_fc_info,
    exact_vsq_fc_info;
  ARAD_MGMT_INIT *init = &(SOC_DPP_CONFIG(unit)->arad->init);

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_INIT);
  res = arad_itm_regs_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  arad_ARAD_ITM_GLOB_RCS_DROP_TH_clear(&glbl_drop);
  arad_ARAD_ITM_GLOB_RCS_DROP_TH_clear(&glbl_drop_exact);
  arad_ARAD_ITM_GLOB_RCS_FC_TH_clear(&glbl_fc);
  arad_ARAD_ITM_GLOB_RCS_FC_TH_clear(&glbl_fc_exact);
  arad_ARAD_ITM_VSQ_FC_INFO_clear(&vsq_fc_info);
  arad_ARAD_ITM_VSQ_FC_INFO_clear(&exact_vsq_fc_info);

  /* If we are in ocb_only mode, we need to set all ITM FC registers to 0*/
  if ((init->ocb.ocb_enable != OCB_ONLY) && ((init->ocb.ocb_enable != OCB_ENABLED) || (init->dram.nof_drams != 0))){
      glbl_fc.bdbs.hp.set       = 256;
      glbl_fc.bdbs.hp.clear     = 1024;
      glbl_fc.bdbs.lp.set       = 768;
      glbl_fc.bdbs.lp.clear     = 1500;
      glbl_fc.unicast.hp.set    = 672;
      glbl_fc.unicast.hp.clear  = 1344;
      glbl_fc.unicast.lp.set    = 10752;
      glbl_fc.unicast.lp.clear  = 43008;
      glbl_fc.full_mc.hp.set    = 416;
      glbl_fc.full_mc.hp.clear  = 832;
      glbl_fc.full_mc.lp.set    = 672;
      glbl_fc.full_mc.lp.clear  = 1344;
  }

  res = arad_itm_glob_rcs_fc_set_unsafe(
          unit,
          &glbl_fc,
          &glbl_fc_exact
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  glbl_drop.bdbs[0].set       = 128;
  glbl_drop.bdbs[0].clear     = 512;
  for (idx = 1; idx < ARAD_NOF_DROP_PRECEDENCE; idx++)
  {
    glbl_drop.bdbs[idx].set   = 256;
    glbl_drop.bdbs[idx].clear = 1024;
  }

  /* Get the PDM Mode */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  36,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_IQM_ENABLERSr, SOC_CORE_ALL, 0, VSQ_CD_ENf, &pdm_mode_fld));
  pdm_mode = (pdm_mode_fld)? ARAD_INIT_PDM_MODE_REDUCED:ARAD_INIT_PDM_MODE_SIMPLE;
  res = arad_init_pdm_nof_entries_calc(
            unit,
            pdm_mode,
            &pdm_nof_entries
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);


  for (idx = 0; idx < ARAD_NOF_DROP_PRECEDENCE; idx++)
  {
    
    glbl_drop.bds[idx].set   = 1008 * (pdm_nof_entries / 1024); 
    glbl_drop.bds[idx].clear =  928 * (pdm_nof_entries / 1024); 
  }

  /* If we are in ocb_only mode, we need to set all IQM_GENERAL_REJECT_CLEAR_CONFIGURATION_D_BUFFS registers to 0*/
  if ((init->ocb.ocb_enable != OCB_ONLY) && ((init->ocb.ocb_enable != OCB_ENABLED) || (init->dram.nof_drams != 0))){

      for (idx = 0; idx < ARAD_NOF_DROP_PRECEDENCE; idx++)
      {
        glbl_drop.unicast[idx].set   = 496;
        glbl_drop.unicast[idx].clear = 672;
      }

      for (idx = 0; idx < ARAD_NOF_DROP_PRECEDENCE; idx++)
      {
        glbl_drop.full_mc[idx].set   = 160;
        glbl_drop.full_mc[idx].clear = 320;
      }

      for (idx = 0; idx < ARAD_NOF_DROP_PRECEDENCE; idx++)
      {
        glbl_drop.mini_mc[idx].set   = 160;
        glbl_drop.mini_mc[idx].clear = 320;
      }

      for (idx = 0; idx < ARAD_NOF_DROP_PRECEDENCE; idx++)
      {
          glbl_drop.mem_excess[idx].set   = ARAD_ITM_GLOB_RCS_DROP_EXCESS_MEM_SIZE_MAX;
          glbl_drop.mem_excess[idx].clear = ARAD_ITM_GLOB_RCS_DROP_EXCESS_MEM_SIZE_MAX - (ARAD_ITM_GLOB_RCS_DROP_EXCESS_MEM_SIZE_MAX/8);
      }
  } else {
      mem_excess_size = (((SOC_DPP_DEFS_GET(unit, ocb_memory_size) * 1024 * 1024 /* 8mb*/) / 8 /* 8mB*/) / 128 /* 128 bytes units */);
      for (idx = 0; idx < ARAD_NOF_DROP_PRECEDENCE; idx++) {
          glbl_drop.mem_excess[idx].set   = mem_excess_size;
          glbl_drop.mem_excess[idx].clear = mem_excess_size - (mem_excess_size / 8);
      }
  }

  res = arad_itm_glob_rcs_drop_set_unsafe(
          unit,
          &glbl_drop,
          &glbl_drop_exact
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  /* Flow control, by default set maximal threshold */
  vsq_fc_info.bd_size_fc.set = ARAD_ITM_VSQ_FC_BD_SIZE_MAX;
  vsq_fc_info.bd_size_fc.clear = ARAD_ITM_VSQ_FC_BD_SIZE_MAX;
  vsq_fc_info.q_size_fc.set  = ARAD_ITM_VSQ_FC_Q_SIZE_MAX;
  vsq_fc_info.q_size_fc.clear = ARAD_ITM_VSQ_FC_Q_SIZE_MAX;

  for (vsq_group = 0; vsq_group < ARAD_ITM_VSQ_GROUP_LAST; vsq_group++) 
  {
       res = arad_itm_vsq_fc_set_unsafe(
          unit,
          vsq_group,
          0, /* Default VSQ rate class */
          &vsq_fc_info,
          &exact_vsq_fc_info
       );
       SOC_SAND_CHECK_FUNC_RESULT(res, 50+vsq_group, exit);
  }
 
  /* set the total of the gaurenteed VOQ resorce */
  
  SOC_DPP_CONFIG(unit)->tm.guaranteed_q_resource.total = pdm_nof_entries;
  res = SOC_DPP_WB_ENGINE_SET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_TM_GUARANTEED_Q_RESOURCE, &(SOC_DPP_CONFIG(unit)->tm.guaranteed_q_resource)); /* update warm boot data */
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

  res = arad_itm_setup_dp_map(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_init()",0,0);
}

uint32
  arad_itm_vsq_idx_verify(
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  ARAD_ITM_VSQ_NDX         vsq_in_group_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_ITM_VSQ_IDX_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_group_ndx, ARAD_ITM_VSQ_GROUP_LAST-1,
    ARAD_ITM_VSQ_GROUP_ID_OUT_OF_RANGE_ERR, 10, exit
  );

  switch(vsq_group_ndx)
  {
  case ARAD_ITM_VSQ_GROUP_CTGRY:
    SOC_SAND_ERR_IF_ABOVE_MAX(
      vsq_in_group_ndx, ARAD_ITM_VSQ_GROUPA_SZE-1,
      ARAD_ITM_VSQ_NDX_OUT_OF_RANGE_ERR, 30, exit
     );
    break;
  case ARAD_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS:
   SOC_SAND_ERR_IF_ABOVE_MAX(
      vsq_in_group_ndx, ARAD_ITM_VSQ_GROUPB_SZE-1,
      ARAD_ITM_VSQ_NDX_OUT_OF_RANGE_ERR, 32, exit
     );
    break;
  case ARAD_ITM_VSQ_GROUP_CTGRY_2_3_CNCTN_CLS:
    SOC_SAND_ERR_IF_ABOVE_MAX(
      vsq_in_group_ndx, ARAD_ITM_VSQ_GROUPC_SZE-1,
      ARAD_ITM_VSQ_NDX_OUT_OF_RANGE_ERR, 34, exit
     );
    break;
  case ARAD_ITM_VSQ_GROUP_STTSTCS_TAG:
    SOC_SAND_ERR_IF_ABOVE_MAX(
      vsq_in_group_ndx, ARAD_ITM_VSQ_GROUPD_SZE-1,
      ARAD_ITM_VSQ_NDX_OUT_OF_RANGE_ERR, 36, exit
     );
    break;
  case ARAD_ITM_VSQ_GROUP_LLFC:
    SOC_SAND_ERR_IF_ABOVE_MAX(
      vsq_in_group_ndx, ARAD_ITM_VSQ_GROUPE_SZE-1,
      ARAD_ITM_VSQ_NDX_OUT_OF_RANGE_ERR, 38, exit
    );
    break;
  case ARAD_ITM_VSQ_GROUP_PFC:
    SOC_SAND_ERR_IF_ABOVE_MAX(
      vsq_in_group_ndx, ARAD_ITM_VSQ_GROUPF_SZE-1,
      ARAD_ITM_VSQ_NDX_OUT_OF_RANGE_ERR, 40, exit
    );
    break;
  default:
    break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_itm_vsq_idx_verify()",vsq_group_ndx,vsq_in_group_ndx);
}

/*********************************************************************
*     This function sets a buffer with the mantissa-exponent values,
*     when given the desired value and field sizes.
*********************************************************************/
STATIC uint32
  arad_itm_man_exp_buffer_set(
    SOC_SAND_IN  int32  value,
    SOC_SAND_IN  uint32 mnt_lsb,
    SOC_SAND_IN  uint32 mnt_nof_bits,
    SOC_SAND_IN  uint32 exp_lsb,
    SOC_SAND_IN  uint32 exp_nof_bits,
    SOC_SAND_IN  uint8 is_signed,
    SOC_SAND_OUT uint32  *output_field,
    SOC_SAND_OUT int32  *exact_value
  )
{
  uint32
    res,
    sign,
    rslt,
    mnt_lsb_field_aligned,
    exp_lsb_field_aligned,
    sign_bit_field_aligned,
    temp_val;
  uint32
    mnt,
    exp;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_ITM_MAN_EXP_BUFFER_SET);

  res = soc_sand_break_to_mnt_exp_round_up(
          soc_sand_abs(value),
          mnt_nof_bits,
          exp_nof_bits,
          0,
          &mnt,
          &exp);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  rslt = 0;

  /* Bit offsets are given as in the entry. Here we want the result to
   * begin in bit 0.
   */
  if (mnt_lsb < exp_lsb)
  {
    mnt_lsb_field_aligned = 0;
    exp_lsb_field_aligned = mnt_nof_bits;
    sign_bit_field_aligned = mnt_nof_bits + exp_nof_bits;
  }
  else
  {
    exp_lsb_field_aligned = 0;
    mnt_lsb_field_aligned = exp_nof_bits;
    sign_bit_field_aligned = mnt_nof_bits + exp_nof_bits;
  }

  temp_val = mnt;
  res = soc_sand_bitstream_set_any_field(
          &temp_val,
          mnt_lsb_field_aligned,
          mnt_nof_bits,
          &rslt
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  temp_val = exp;
  res = soc_sand_bitstream_set_any_field(
          &temp_val,
          exp_lsb_field_aligned,
          exp_nof_bits,
          &rslt
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  sign = (value < 0);
  if (is_signed)
  {
    res = soc_sand_bitstream_set_any_field(
            &sign,
            sign_bit_field_aligned,
            1,
            &rslt
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

  *output_field = rslt;

  *exact_value = (mnt * (1<<exp)) * (sign ? -1 : 1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_itm_man_exp_buffer_set()",0,0);
}
/*********************************************************************
*     This function gets a the value that is constructed of a buffer
*     with the mantissa-exponent values, when the field sizes.
*********************************************************************/
STATIC uint32
  arad_itm_man_exp_buffer_get(
    SOC_SAND_IN  uint32  buffer,
    SOC_SAND_IN  uint32 mnt_lsb,
    SOC_SAND_IN  uint32 mnt_nof_bits,
    SOC_SAND_IN  uint32 exp_lsb,
    SOC_SAND_IN  uint32 exp_nof_bits,
    SOC_SAND_IN  uint8 is_signed,
    SOC_SAND_OUT int32  *value
  )
{
  uint32
    res,
    mnt    = 0,
    exp    = 0,
    sign   = 0,
    mnt_lsb_field_aligned,
    exp_lsb_field_aligned,
    sign_bit_field_aligned;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_ITM_MAN_EXP_BUFFER_GET);

  /* Bit offsets are given as in the entry. Here we want the result to
   * begin in bit 0.
   */
  if (mnt_lsb < exp_lsb)
  {
    mnt_lsb_field_aligned = 0;
    exp_lsb_field_aligned = mnt_nof_bits;
    sign_bit_field_aligned = mnt_nof_bits + exp_nof_bits;
  }
  else
  {
    exp_lsb_field_aligned = 0;
    mnt_lsb_field_aligned = exp_nof_bits;
    sign_bit_field_aligned = mnt_nof_bits + exp_nof_bits;
  }

  res = soc_sand_bitstream_get_any_field(
          &buffer,
          mnt_lsb_field_aligned,
          mnt_nof_bits,
          &mnt
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_sand_bitstream_get_any_field(
          &buffer,
          exp_lsb_field_aligned,
          exp_nof_bits,
          &exp
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  sign = 0;
  if (is_signed)
  {
    res = soc_sand_bitstream_get_any_field(
            &buffer,
            sign_bit_field_aligned,
            1,
            &sign
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

  *value = (mnt * (1<<exp)) * (sign ? -1 : 1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_itm_man_exp_buffer_get()",0,0);
}


/************************************************************************
*     Convert the type ARAD_ITM_ADMIT_ONE_TEST_TMPLT to uint32
*     in order to be written to the register field.                                                                     */
/************************************************************************/
uint32
  arad_itm_convert_admit_one_test_tmplt_to_u32(
    SOC_SAND_IN ARAD_ITM_ADMIT_ONE_TEST_TMPLT test,
    SOC_SAND_OUT uint32                       *test_in_sand_u32
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_ITM_CONVERT_ADMIT_ONE_TEST_TMPLT_TO_U32);
  SOC_SAND_CHECK_NULL_INPUT(test_in_sand_u32);

  *test_in_sand_u32 =
    (test.ctgry_test_en==TRUE ? 0:1) |
    ((test.ctgry_trffc_test_en==TRUE ? 0:1) << ARAD_ITM_TEST_CTGRY_TRFFC_TEST_EN_BIT)|
    ((test.ctgry2_3_cnctn_test_en==TRUE ? 0:1) << ARAD_ITM_TEST_CTGRY2_3_CNCTN_TEST_EN_BIT)|
    ((test.sttstcs_tag_test_en==TRUE ? 0:1) << ARAD_ITM_TEST_STTSTCS_TAG_TEST_EN_BIT)|
    ((test.pfc_test_en==TRUE ? 0:1) << ARAD_ITM_TEST_PFC_TEST_EN_BIT)|
    ((test.llfc_test_en==TRUE ? 0:1) << ARAD_ITM_TEST_LLFC_TEST_EN_BIT);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_itm_convert_admit_one_test_tmplt_to_u32()",0,0);
}
/*********************************************************************
*     Convert the uint32 test template field from the register
*     to type ARAD_ITM_ADMIT_ONE_TEST_TMPLT in order to be returned
*     to user.
*********************************************************************/
uint32
  arad_itm_convert_u32_to_admit_one_test_tmplt(
    SOC_SAND_IN  uint32                       test_in_sand_u32,
    SOC_SAND_OUT ARAD_ITM_ADMIT_ONE_TEST_TMPLT *test
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_ITM_CONVERT_U32_TO_ADMIT_ONE_TEST_TMPLT);
  SOC_SAND_CHECK_NULL_INPUT(test);

  test->ctgry_test_en =
    ((test_in_sand_u32 >> ARAD_ITM_TEST_CTGRY_TEST_EN_BIT) & 1) ? FALSE : TRUE;
  test->ctgry_trffc_test_en =
    ((test_in_sand_u32 >> ARAD_ITM_TEST_CTGRY_TRFFC_TEST_EN_BIT) & 1) ? FALSE : TRUE;
  test->ctgry2_3_cnctn_test_en =
    ((test_in_sand_u32 >> ARAD_ITM_TEST_CTGRY2_3_CNCTN_TEST_EN_BIT) & 1) ? FALSE : TRUE;
  test->sttstcs_tag_test_en =
    ((test_in_sand_u32 >> ARAD_ITM_TEST_STTSTCS_TAG_TEST_EN_BIT) & 1) ? FALSE : TRUE;
  test->pfc_test_en =
    ((test_in_sand_u32 >> ARAD_ITM_TEST_PFC_TEST_EN_BIT) & 1) ? FALSE : TRUE;
  test->llfc_test_en =
    ((test_in_sand_u32 >> ARAD_ITM_TEST_LLFC_TEST_EN_BIT) & 1) ? FALSE : TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_itm_convert_u32_to_admit_one_test_tmplt()",0,0);
}
/*********************************************************************
*    Set a credit request hungry configuration
*********************************************************************/
STATIC uint32
  arad_itm_cr_request_info_hungry_table_field_set(
    SOC_SAND_IN  int32 value,
    SOC_SAND_OUT uint32 *output_buffer,
    SOC_SAND_OUT int32 *exact_value,
    SOC_SAND_IN  uint8 resolution
  )
{
  uint32
    res;
  int32
    exact_value_var;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_ITM_CR_REQUEST_INFO_HUNGRY_TABLE_FIELD_SET);

  res = arad_itm_man_exp_buffer_set(
          value / resolution,
          ARAD_ITM_HUNGRY_TH_MNT_LSB,
          ARAD_ITM_HUNGRY_TH_MNT_NOF_BITS,
          ARAD_ITM_HUNGRY_TH_EXP_LSB,
          ARAD_ITM_HUNGRY_TH_EXP_NOF_BITS,
          TRUE,
          output_buffer,
          &exact_value_var
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *exact_value = resolution * (int32)exact_value_var;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_itm_cr_request_info_hungry_table_field_set()", value, resolution);
}
/*********************************************************************
*    Set a credit request satisfied configuration
*********************************************************************/
STATIC uint32
  arad_itm_cr_request_info_satisfied_mnt_exp_table_field_set(
    SOC_SAND_IN  int32  value,
    SOC_SAND_OUT uint32  *output_buffer,
    SOC_SAND_OUT uint32 *exact_value,
    SOC_SAND_IN  uint8 resolution
  )
{
  uint32
    res;
  int32
    exact_value_var;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_ITM_CR_REQUEST_INFO_SATISFIED_MNT_EXP_TABLE_FIELD_SET);

  res = arad_itm_man_exp_buffer_set(
          value / resolution,
          ARAD_ITM_SATISFIED_TH_MNT_LSB,
          ARAD_ITM_SATISFIED_TH_MNT_NOF_BITS,
          ARAD_ITM_SATISFIED_TH_EXP_LSB,
          ARAD_ITM_SATISFIED_TH_EXP_NOF_BITS,
          FALSE,
          output_buffer,
          &exact_value_var
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *exact_value = resolution * (uint32)exact_value_var;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_itm_cr_request_info_satisfied_mnt_exp_table_field_set()",0,0);
}
/*********************************************************************
*    Get a credit request hungry configuration
*********************************************************************/
STATIC uint32
  arad_itm_cr_request_info_hungry_table_field_get(
    SOC_SAND_IN  int32 buffer,
    SOC_SAND_OUT int32 *value,
    SOC_SAND_IN  int32 multiplier
  )
{
  uint32
    res;
  int32
    value_var;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_ITM_CR_REQUEST_INFO_HUNGRY_TABLE_FIELD_GET);

  res = arad_itm_man_exp_buffer_get(
          buffer,
          ARAD_ITM_HUNGRY_TH_MNT_LSB,
          ARAD_ITM_HUNGRY_TH_MNT_NOF_BITS,
          ARAD_ITM_HUNGRY_TH_EXP_LSB,
          ARAD_ITM_HUNGRY_TH_EXP_NOF_BITS,
          TRUE,
          &value_var
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *value = multiplier * (int32)value_var;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_itm_cr_request_info_hungry_table_field_get()", buffer, 0);
}
/*********************************************************************
*    Get a credit request satisfied configuration
*********************************************************************/
STATIC uint32
  arad_itm_cr_request_info_satisfied_mnt_exp_table_field_get(
    SOC_SAND_IN  int32 buffer,
    SOC_SAND_OUT uint32 *value,
    SOC_SAND_IN  int32 multiplier
  )
{
  uint32
    res;
  int32
    value_var;
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_ITM_CR_REQUEST_INFO_SATISFIED_MNT_EXP_TABLE_FIELD_GET);

  res = arad_itm_man_exp_buffer_get(
    buffer,
    ARAD_ITM_SATISFIED_TH_MNT_LSB,
    ARAD_ITM_SATISFIED_TH_MNT_NOF_BITS,
    ARAD_ITM_SATISFIED_TH_EXP_LSB,
    ARAD_ITM_SATISFIED_TH_EXP_NOF_BITS,
    FALSE,
    &value_var
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  *value = multiplier * (uint32)value_var;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_itm_cr_request_info_satisfied_mnt_exp_table_field_get()", buffer, 0);
}
/*********************************************************************
*    Sets the WRED parameters to the value to be written to the table.
*    By converting them appropriately.
*********************************************************************/
STATIC uint32
  arad_itm_WRED_QT_DP_INFO_to_WRED_TBL_DATA(
    SOC_SAND_IN    ARAD_ITM_WRED_QT_DP_INFO                            *wred_param,
    SOC_SAND_INOUT ARAD_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_DATA *tbl_data
  )
{
  uint32
    res,
    max_prob,
    calc,
    max_val_c1,
    max_avrg_th_16_byte;
  int32
    avrg_th_diff_wred_granular = 0;
  int32
    min_avrg_th_exact_wred_granular,
    max_avrg_th_exact_wred_granular;
  uint32
    trunced;
  SOC_SAND_U64
    u64_1,
    u64_2,
    u64_c2 = {{0}};

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_ITM_WRED_QT_DP_INFO_TO_WRED_TBL_DATA);

  SOC_SAND_CHECK_NULL_INPUT(wred_param);
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);

  trunced = FALSE;

  /*
   * min_avrg_th
   */
  
  max_avrg_th_16_byte = SOC_SAND_DIV_ROUND_UP(wred_param->min_avrg_th,ARAD_ITM_WRED_GRANULARITY);
  tbl_data->pq_avrg_min_th = 0;
  res = arad_itm_man_exp_buffer_set(
          max_avrg_th_16_byte,
          ARAD_ITM_MIN_WRED_AVRG_TH_MNT_LSB,
          ARAD_ITM_MIN_WRED_AVRG_TH_MNT_NOF_BITS,
          ARAD_ITM_MIN_WRED_AVRG_TH_EXP_LSB,
          ARAD_ITM_MIN_WRED_AVRG_TH_EXP_NOF_BITS,
          FALSE,
          &(tbl_data->pq_avrg_min_th),
          &min_avrg_th_exact_wred_granular
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /* min_avrg_th_exact *= ARAD_ITM_WRED_GRANULARITY; */

  /*
   * max_avrg_th
   */
  tbl_data->pq_avrg_max_th = 0;
  res = arad_itm_man_exp_buffer_set(
          SOC_SAND_DIV_ROUND_UP(wred_param->max_avrg_th,ARAD_ITM_WRED_GRANULARITY),
          ARAD_ITM_MAX_WRED_AVRG_TH_MNT_LSB,
          ARAD_ITM_MAX_WRED_AVRG_TH_MNT_NOF_BITS,
          ARAD_ITM_MAX_WRED_AVRG_TH_EXP_LSB,
          ARAD_ITM_MAX_WRED_AVRG_TH_EXP_NOF_BITS,
          FALSE,
          &(tbl_data->pq_avrg_max_th),
          &max_avrg_th_exact_wred_granular
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  /* max_avrg_th_exact *= ARAD_ITM_WRED_GRANULARITY; */

  /*
   * max_packet_size
   */
  calc = wred_param->max_packet_size;
  if (calc > ARAD_ITM_WRED_MAX_PACKET_SIZE_FOR_CALC)
  {
    calc = ARAD_ITM_WRED_MAX_PACKET_SIZE_FOR_CALC;
  }
  calc = SOC_SAND_DIV_ROUND_UP(calc, ARAD_ITM_WRED_GRANULARITY);
  tbl_data->pq_c3 = (wred_param->max_avrg_th == 0 ? 0 : soc_sand_log2_round_up(calc));

  /*
   * max_probability
   */
  max_prob = (wred_param->max_probability);

  /*
   * max_probability
   * C1 = ((2^32)/100)*max-prob / (max-th - min-th) in powers of 2
   */
  if(max_prob>=100)
  {
    max_prob = 99;
  }
  calc = ARAD_WRED_NORMALIZE_FACTOR * max_prob;

    /*
     * We do not use 'SOC_SAND_DIV_ROUND' or 'SOC_SAND_DIV_ROUND_UP'
     * because at this point we might have in calc '((2^32)/100)*max-prob'
     * which can be very large number and the other dividers do ADD before
     * the division.
     */
  max_val_c1 = 31; /* soc_sand_log2_round_down(0xFFFFFFFF) */

  avrg_th_diff_wred_granular =
    (max_avrg_th_exact_wred_granular - min_avrg_th_exact_wred_granular);

  if(avrg_th_diff_wred_granular == 0)
  {
    tbl_data->pq_c1 = max_val_c1;
  }
  else
  {
    calc = SOC_SAND_DIV_ROUND_DOWN(calc, avrg_th_diff_wred_granular);
    tbl_data->pq_c1 = soc_sand_log2_round_down(calc);
  }

  if(tbl_data->pq_c1 < max_val_c1)
  {
    /*
     * Check if a bigger C1 gives closer result of the value we add.
     */
    uint32
      now     = 1 <<(tbl_data->pq_c1),
      changed = 1 <<(tbl_data->pq_c1+1),
      diff_with_now,
      diff_with_change;

    diff_with_change = changed-calc;
    diff_with_now    = calc-now;
    if( diff_with_change < diff_with_now)
    {
      tbl_data->pq_c1 += 1;
    }
  }

  SOC_SAND_LIMIT_FROM_ABOVE(tbl_data->pq_c1, max_val_c1);

  if (max_avrg_th_16_byte > 0)
  { 
    max_val_c1 = SOC_SAND_DIV_ROUND_DOWN(0xFFFFFFFF, max_avrg_th_16_byte);
    max_val_c1 = soc_sand_log2_round_down(max_val_c1);
    SOC_SAND_LIMIT_FROM_ABOVE(tbl_data->pq_c1, max_val_c1);
  }

  /*
   * max_probability
   * C2 = FACTOR * max-prob * min-th / (max-th - min-th)
   */
  soc_sand_u64_multiply_longs(
    ARAD_WRED_NORMALIZE_FACTOR,
    max_prob * min_avrg_th_exact_wred_granular,
    &u64_2
  );
  soc_sand_u64_devide_u64_long(&u64_2, avrg_th_diff_wred_granular, &u64_c2);

  /*
   * P =
   */
  soc_sand_u64_multiply_longs(
    min_avrg_th_exact_wred_granular,
    (1 << tbl_data->pq_c1),
    &u64_1
  );

  if(soc_sand_u64_is_bigger(&u64_c2, &u64_1))
  {
    soc_sand_os_memcpy(&u64_c2, &u64_1, sizeof(SOC_SAND_U64));
  }

  trunced = soc_sand_u64_to_long(&u64_c2, &tbl_data->pq_c2);

  if (trunced)
  {
    tbl_data->pq_c2 = 0xFFFFFFFF;
  }
  tbl_data->pq_c2 = (wred_param->max_avrg_th == 0 ? 1 : tbl_data->pq_c2);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_WRED_QT_DP_INFO_to_WRED_TBL_DATA()",0,0);
}

/*********************************************************************
*    Gets the WRED parameters from the values in the table.
*    By converting them appropriately.
*********************************************************************/
STATIC uint32
  arad_itm_WRED_TBL_DATA_to_WRED_QT_DP_INFO(
    SOC_SAND_IN  ARAD_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_DATA  *tbl_data,
    SOC_SAND_OUT ARAD_ITM_WRED_QT_DP_INFO                             *wred_param
  )
{
  uint32
    res;
  uint32
    avrg_th_diff_wred_granular,
    two_power_c1,
    remainder;
  int32
    min_avrg_th_var,
    max_avrg_th_var;
  SOC_SAND_U64
    u64_1,
    u64_2;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_ITM_WRED_TBL_DATA_TO_WRED_QT_DP_INFO);

  SOC_SAND_CHECK_NULL_INPUT(wred_param);
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);

  res = arad_itm_man_exp_buffer_get(
          tbl_data->pq_avrg_min_th,
          ARAD_ITM_MIN_WRED_AVRG_TH_MNT_LSB,
          ARAD_ITM_MIN_WRED_AVRG_TH_MNT_NOF_BITS,
          ARAD_ITM_MIN_WRED_AVRG_TH_EXP_LSB,
          ARAD_ITM_MIN_WRED_AVRG_TH_EXP_NOF_BITS,
          FALSE,
          &min_avrg_th_var
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  wred_param->min_avrg_th = (uint32)min_avrg_th_var;

  wred_param->min_avrg_th *= ARAD_ITM_WRED_GRANULARITY;

  /*
   * max_avrg_th
   */

  res = arad_itm_man_exp_buffer_get(
          tbl_data->pq_avrg_max_th,
          ARAD_ITM_MAX_WRED_AVRG_TH_MNT_LSB,
          ARAD_ITM_MAX_WRED_AVRG_TH_MNT_NOF_BITS,
          ARAD_ITM_MAX_WRED_AVRG_TH_EXP_LSB,
          ARAD_ITM_MAX_WRED_AVRG_TH_EXP_NOF_BITS,
          FALSE,
          &max_avrg_th_var
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  wred_param->max_avrg_th = (uint32)max_avrg_th_var;

  wred_param->max_avrg_th *= ARAD_ITM_WRED_GRANULARITY;

  /*
   * max_packet_size
   */
  wred_param->max_packet_size = ((tbl_data->pq_c3 == 0) && (tbl_data->pq_c2 == 1)) ? 0 : (0x1 << (tbl_data->pq_c3)) * ARAD_ITM_WRED_GRANULARITY;

  avrg_th_diff_wred_granular =
    (wred_param->max_avrg_th - wred_param->min_avrg_th) / ARAD_ITM_WRED_GRANULARITY;

  two_power_c1 = 1<<tbl_data->pq_c1;
  /*
   * C1 = ((2^32)/100)*max-prob / (max-th - min-th) in powers of 2
   * ==>
   * max-prob =  ( 2^C1 * (max-th - min-th) ) / ((2^32)/100)
   */
  soc_sand_u64_multiply_longs(two_power_c1, avrg_th_diff_wred_granular, &u64_1);
  remainder = soc_sand_u64_devide_u64_long(&u64_1, ARAD_WRED_NORMALIZE_FACTOR, &u64_2);
  soc_sand_u64_to_long(&u64_2, &wred_param->max_probability);

  if(remainder > (ARAD_WRED_NORMALIZE_FACTOR/2))
  {
    wred_param->max_probability++;
  }

  if(wred_param->max_probability > 100)
  {
    wred_param->max_probability = 100;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_WRED_TBL_DATA_to_WRED_QT_DP_INFO()",0,0);
}
/*********************************************************************
*    Set the VSQ rate class for VSQ-type-A
*********************************************************************/
STATIC uint32
  arad_itm_vsq_group_a_set_rt_class(
    SOC_SAND_IN int                 unit,
    SOC_SAND_IN uint32   vsq_rt_cls,
    SOC_SAND_IN uint32                  vsq_in_group_ndx
  )
{
  uint32
    res;
  ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_DATA
    iqm_vsq_descriptor_rate_class_group_a_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_GROUP_A_SET_RT_CLASS);

  iqm_vsq_descriptor_rate_class_group_a_tbl_data.vsq_rc_a = vsq_rt_cls;

  res = arad_iqm_vsq_descriptor_rate_class_group_a_tbl_set_unsafe(
          unit,
          vsq_in_group_ndx,
          &iqm_vsq_descriptor_rate_class_group_a_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_group_a_set_rt_class()",0,0);
}
/*********************************************************************
*    Set the VSQ rate class for VSQ-type-B
*********************************************************************/
STATIC uint32
  arad_itm_vsq_group_b_set_rt_class(
    SOC_SAND_IN int                 unit,
    SOC_SAND_IN uint32                 vsq_rt_cls,
    SOC_SAND_IN uint32                  vsq_in_group_ndx
  )
{
  uint32
    res;
  ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_DATA
    iqm_vsq_descriptor_rate_class_group_b_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_GROUP_B_SET_RT_CLASS);

  iqm_vsq_descriptor_rate_class_group_b_tbl_data.vsq_rc_b = vsq_rt_cls;

  res = arad_iqm_vsq_descriptor_rate_class_group_b_tbl_set_unsafe(
    unit,
    vsq_in_group_ndx,
    &iqm_vsq_descriptor_rate_class_group_b_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_group_b_set_rt_class()",0,0);
}
/*********************************************************************
*    Set the VSQ rate class for VSQ-type-C
*********************************************************************/
STATIC uint32
  arad_itm_vsq_group_c_set_rt_class(
    SOC_SAND_IN int                 unit,
    SOC_SAND_IN uint32                 vsq_rt_cls,
    SOC_SAND_IN uint32                  vsq_in_group_ndx
  )
{
  uint32
    res;
  ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_DATA
    iqm_vsq_descriptor_rate_class_group_c_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_GROUP_C_SET_RT_CLASS);

  iqm_vsq_descriptor_rate_class_group_c_tbl_data.vsq_rc_c = vsq_rt_cls;

  res = arad_iqm_vsq_descriptor_rate_class_group_c_tbl_set_unsafe(
          unit,
          vsq_in_group_ndx,
          &iqm_vsq_descriptor_rate_class_group_c_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_group_c_set_rt_class()",0,0);
}
/*********************************************************************
*    Set the VSQ rate class for VSQ-type-D
*********************************************************************/
STATIC uint32
  arad_itm_vsq_group_d_set_rt_class(
    SOC_SAND_IN int                 unit,
    SOC_SAND_IN uint32                 vsq_rt_cls,
    SOC_SAND_IN uint32                  vsq_in_group_ndx
  )
{
  uint32
    res;
  ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_DATA
    iqm_vsq_descriptor_rate_class_group_d_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_GROUP_D_SET_RT_CLASS);

  iqm_vsq_descriptor_rate_class_group_d_tbl_data.vsq_rc_d = vsq_rt_cls;

  res = arad_iqm_vsq_descriptor_rate_class_group_d_tbl_set_unsafe(
          unit,
          vsq_in_group_ndx,
          &iqm_vsq_descriptor_rate_class_group_d_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_group_d_set_rt_class()",0,0);
}

/*********************************************************************
*    Set the VSQ rate class for VSQ-type-E
*********************************************************************/
STATIC uint32
  arad_itm_vsq_group_e_set_rt_class(
    SOC_SAND_IN int                 unit,
    SOC_SAND_IN uint32                 vsq_rt_cls,
    SOC_SAND_IN uint32                  vsq_in_group_ndx
  )
{
  uint32
    res;
  uint32
    vsq_rt_cls_lcl = vsq_rt_cls;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_GROUP_E_SET_RT_CLASS);

  res = WRITE_IQM_VSQDRC_Em(
          unit,
          MEM_BLOCK_ANY,
          vsq_in_group_ndx,
          &vsq_rt_cls_lcl
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_group_e_set_rt_class()",0,0);
}

/*********************************************************************
*    Set the VSQ rate class for VSQ-type-F
*********************************************************************/
STATIC uint32
  arad_itm_vsq_group_f_set_rt_class(
    SOC_SAND_IN int                 unit,
    SOC_SAND_IN uint32                 vsq_rt_cls,
    SOC_SAND_IN uint32                  vsq_in_group_ndx
  )
{
  uint32
    res;
  uint32
    vsq_rt_cls_lcl = vsq_rt_cls;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_GROUP_F_SET_RT_CLASS);

  res = WRITE_IQM_VSQDRC_Fm(
          unit,
          MEM_BLOCK_ANY,
          vsq_in_group_ndx,
          &vsq_rt_cls_lcl
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_group_f_set_rt_class()",0,0);
}

/*********************************************************************
*    Get the VSQ rate class for VSQ-type-A
*********************************************************************/
STATIC uint32
  arad_itm_vsq_group_a_get_rt_class(
    SOC_SAND_IN int                 unit,
    SOC_SAND_IN uint32                  vsq_in_group_ndx,
    SOC_SAND_OUT uint32                *vsq_rt_cls
  )
{
  uint32
    res;
  ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_DATA
    iqm_vsq_descriptor_rate_class_group_a_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_GROUP_A_SET_RT_CLASS);

  res = arad_iqm_vsq_descriptor_rate_class_group_a_tbl_get_unsafe(
          unit,
          vsq_in_group_ndx,
          &iqm_vsq_descriptor_rate_class_group_a_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  *vsq_rt_cls =
    iqm_vsq_descriptor_rate_class_group_a_tbl_data.vsq_rc_a;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_group_a_get_rt_class()",0,0);
}
/*********************************************************************
*    Get the VSQ rate class for VSQ-type-B
*********************************************************************/
STATIC uint32
  arad_itm_vsq_group_b_get_rt_class(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  vsq_in_group_ndx,
    SOC_SAND_OUT uint32                *vsq_rt_cls
  )
{
  uint32
    res;
  ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_DATA
    iqm_vsq_descriptor_rate_class_group_b_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_GROUP_B_GET_RT_CLASS);

  res = arad_iqm_vsq_descriptor_rate_class_group_b_tbl_get_unsafe(
          unit,
          vsq_in_group_ndx,
          &iqm_vsq_descriptor_rate_class_group_b_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  *vsq_rt_cls =
    iqm_vsq_descriptor_rate_class_group_b_tbl_data.vsq_rc_b;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_group_b_get_rt_class()",0,0);
}
/*********************************************************************
*    Get the VSQ rate class for VSQ-type-C
*********************************************************************/
STATIC uint32
  arad_itm_vsq_group_c_get_rt_class(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  vsq_in_group_ndx,
    SOC_SAND_OUT uint32                *vsq_rt_cls
  )
{
  uint32
    res;
  ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_DATA
    iqm_vsq_descriptor_rate_class_group_c_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_GROUP_C_GET_RT_CLASS);

  res = arad_iqm_vsq_descriptor_rate_class_group_c_tbl_get_unsafe(
          unit,
          vsq_in_group_ndx,
          &iqm_vsq_descriptor_rate_class_group_c_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  *vsq_rt_cls =
    iqm_vsq_descriptor_rate_class_group_c_tbl_data.vsq_rc_c;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_group_c_get_rt_class()",0,0);
}
/*********************************************************************
*    Get the VSQ rate class for VSQ-type-D
*********************************************************************/
STATIC uint32
  arad_itm_vsq_group_d_get_rt_class(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  vsq_in_group_ndx,
    SOC_SAND_OUT uint32                *vsq_rt_cls
  )
{
  uint32
    res;
  ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_DATA
    iqm_vsq_descriptor_rate_class_group_d_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_GROUP_D_GET_RT_CLASS);

  res = arad_iqm_vsq_descriptor_rate_class_group_d_tbl_get_unsafe(
          unit,
          vsq_in_group_ndx,
          &iqm_vsq_descriptor_rate_class_group_d_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

 *vsq_rt_cls =
   iqm_vsq_descriptor_rate_class_group_d_tbl_data.vsq_rc_d;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_group_d_get_rt_class()",0,0);
}
/*********************************************************************
*    Get the VSQ rate class for VSQ-type-E
*********************************************************************/
STATIC uint32
  arad_itm_vsq_group_e_get_rt_class(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  vsq_in_group_ndx,
    SOC_SAND_OUT uint32                *vsq_rt_cls
  )
{
  uint32
    res;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_GROUP_E_GET_RT_CLASS);

  res = READ_IQM_VSQDRC_Em(
          unit,
          MEM_BLOCK_ANY,
          vsq_in_group_ndx,
          vsq_rt_cls
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_group_e_get_rt_class()",0,0);
}
/*********************************************************************
*    Get the VSQ rate class for VSQ-type-F
*********************************************************************/
STATIC uint32
  arad_itm_vsq_group_f_get_rt_class(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  vsq_in_group_ndx,
    SOC_SAND_OUT uint32                *vsq_rt_cls
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_GROUP_F_GET_RT_CLASS);

  res = READ_IQM_VSQDRC_Fm(
          unit,
          MEM_BLOCK_ANY,
          vsq_in_group_ndx,
          vsq_rt_cls
        );
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);

 exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_group_f_get_rt_class()",0,0);
}
/*********************************************************************
*    Set the VSQ flow-control info according to vsq-group-id
*********************************************************************/
STATIC uint32
  arad_itm_vsq_group_set_fc_info(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group,
    SOC_SAND_IN  uint32                 vsq_rt_cls,
    SOC_SAND_IN  ARAD_ITM_VSQ_FC_INFO     *info,
    SOC_SAND_OUT ARAD_ITM_VSQ_FC_INFO     *exact_info
  )
{
  uint32
    res,
    vsq_bds_th_clear_and_set_mnt_nof_bits,
    vsq_bds_th_clear_and_set_exp_nof_bits,
    vsq_words_th_clear_and_set_mnt_nof_bits,
    vsq_words_th_clear_and_set_exp_nof_bits;
  ARAD_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_DATA
    iqm_vsq_flow_control_parameters_table_group_tbl_data;
  uint32
    set_threshold_words_mnt,
    set_threshold_words_exp,
    clear_threshold_words_mnt,
    clear_threshold_words_exp,
    set_threshold_bd_mnt,
    set_threshold_bd_exp,
    clear_threshold_bd_mnt,
    clear_threshold_bd_exp;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_GROUP_SET_FC_INFO);

 
 

  res = arad_iqm_vsq_flow_control_parameters_table_group_tbl_get_unsafe(
          unit,
          vsq_group,
          vsq_rt_cls,
          &iqm_vsq_flow_control_parameters_table_group_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  vsq_bds_th_clear_and_set_mnt_nof_bits = 4;
  vsq_bds_th_clear_and_set_exp_nof_bits = soc_mem_field_length(unit, IQM_VQFCPR_MAm, SET_THRESHOLD_BDf) - vsq_bds_th_clear_and_set_mnt_nof_bits;

  vsq_words_th_clear_and_set_mnt_nof_bits = 5;
  vsq_words_th_clear_and_set_exp_nof_bits = soc_mem_field_length(unit, IQM_VQFCPR_MAm, SET_THRESHOLD_WORDSf) - vsq_words_th_clear_and_set_mnt_nof_bits;

  res = soc_sand_break_to_mnt_exp_round_up(
          info->bd_size_fc.clear,
          vsq_bds_th_clear_and_set_mnt_nof_bits,
          vsq_bds_th_clear_and_set_exp_nof_bits,
          0,
          &(clear_threshold_bd_mnt),
          &(clear_threshold_bd_exp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  iqm_vsq_flow_control_parameters_table_group_tbl_data.clear_threshold_bd_mnt = clear_threshold_bd_mnt;
  iqm_vsq_flow_control_parameters_table_group_tbl_data.clear_threshold_bd_exp = clear_threshold_bd_exp;

  res = soc_sand_break_to_mnt_exp_round_up(
          info->bd_size_fc.set,
          vsq_bds_th_clear_and_set_mnt_nof_bits,
          vsq_bds_th_clear_and_set_exp_nof_bits,
          0,
          &(set_threshold_bd_mnt),
          &(set_threshold_bd_exp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  iqm_vsq_flow_control_parameters_table_group_tbl_data.set_threshold_bd_mnt = set_threshold_bd_mnt;
  iqm_vsq_flow_control_parameters_table_group_tbl_data.set_threshold_bd_exp = set_threshold_bd_exp;

  res = soc_sand_break_to_mnt_exp_round_up(
          info->q_size_fc.clear,
          vsq_words_th_clear_and_set_mnt_nof_bits,
          vsq_words_th_clear_and_set_exp_nof_bits,
          0,
          &(clear_threshold_words_mnt),
          &(clear_threshold_words_exp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  iqm_vsq_flow_control_parameters_table_group_tbl_data.clear_threshold_words_mnt = clear_threshold_words_mnt;
  iqm_vsq_flow_control_parameters_table_group_tbl_data.clear_threshold_words_exp = clear_threshold_words_exp;

  res = soc_sand_break_to_mnt_exp_round_up(
          info->q_size_fc.set,
          vsq_words_th_clear_and_set_mnt_nof_bits,
          vsq_words_th_clear_and_set_exp_nof_bits,
          0,
          &(set_threshold_words_mnt),
          &(set_threshold_words_exp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  iqm_vsq_flow_control_parameters_table_group_tbl_data.set_threshold_words_mnt = set_threshold_words_mnt;
  iqm_vsq_flow_control_parameters_table_group_tbl_data.set_threshold_words_exp = set_threshold_words_exp;

  exact_info->bd_size_fc.clear =
      ((iqm_vsq_flow_control_parameters_table_group_tbl_data.\
      clear_threshold_bd_mnt) *
      (1<<(iqm_vsq_flow_control_parameters_table_group_tbl_data.\
      clear_threshold_bd_exp)));
  exact_info->bd_size_fc.set =
    ((iqm_vsq_flow_control_parameters_table_group_tbl_data.\
    set_threshold_bd_mnt) *
      (1<<(iqm_vsq_flow_control_parameters_table_group_tbl_data.\
      set_threshold_bd_exp)));
  exact_info->q_size_fc.clear =
    ((iqm_vsq_flow_control_parameters_table_group_tbl_data.\
    clear_threshold_words_mnt) *
      (1<<(iqm_vsq_flow_control_parameters_table_group_tbl_data.\
      clear_threshold_words_exp)));
  exact_info->q_size_fc.set =
    ((iqm_vsq_flow_control_parameters_table_group_tbl_data.\
    set_threshold_words_mnt) *
      (1<<(iqm_vsq_flow_control_parameters_table_group_tbl_data.\
      set_threshold_words_exp)));

  res = arad_iqm_vsq_flow_control_parameters_table_group_tbl_set_unsafe(
          unit,
          vsq_group,
          vsq_rt_cls,
          &iqm_vsq_flow_control_parameters_table_group_tbl_data
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_group_set_fc_info()",0,0);
}

/*********************************************************************
*    Get the VSQ flow-control info according to vsq-group-id
*********************************************************************/
STATIC uint32
  arad_itm_vsq_group_get_fc_info(
    SOC_SAND_IN   int                 unit,
    SOC_SAND_IN   ARAD_ITM_VSQ_GROUP       vsq_group,
    SOC_SAND_IN   uint32   vsq_rt_cls,
    SOC_SAND_OUT  ARAD_ITM_VSQ_FC_INFO     *info
  )
{
  uint32
    res;
  ARAD_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_DATA
    iqm_vsq_flow_control_parameters_table_group_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_GROUP_GET_FC_INFO);

  res = arad_iqm_vsq_flow_control_parameters_table_group_tbl_get_unsafe(
          unit,
          vsq_group,
          vsq_rt_cls,
          &iqm_vsq_flow_control_parameters_table_group_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  info->bd_size_fc.clear =
      ((iqm_vsq_flow_control_parameters_table_group_tbl_data.\
      clear_threshold_bd_mnt) *
      (1<<(iqm_vsq_flow_control_parameters_table_group_tbl_data.\
      clear_threshold_bd_exp)));
  info->bd_size_fc.set =
    ((iqm_vsq_flow_control_parameters_table_group_tbl_data.\
    set_threshold_bd_mnt) *
      (1<<(iqm_vsq_flow_control_parameters_table_group_tbl_data.\
      set_threshold_bd_exp)));
  info->q_size_fc.clear =
    ((iqm_vsq_flow_control_parameters_table_group_tbl_data.\
    clear_threshold_words_mnt) *
      (1<<(iqm_vsq_flow_control_parameters_table_group_tbl_data.\
      clear_threshold_words_exp)));
  info->q_size_fc.set =
    ((iqm_vsq_flow_control_parameters_table_group_tbl_data.\
    set_threshold_words_mnt) *
      (1<<(iqm_vsq_flow_control_parameters_table_group_tbl_data.\
      set_threshold_words_exp)));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_group_get_fc_info()",0,0);
}


/*********************************************************************
*    Sets the VSQ-WRED parameters according to vsq-queue-type and
*    drop-precedence to the value to be written to the table.
*    By converting them appropriately.
*********************************************************************/
STATIC uint32
  arad_itm_vsq_WRED_QT_DP_INFO_to_WRED_TBL_DATA(
    SOC_SAND_IN  ARAD_ITM_WRED_QT_DP_INFO                             *wred_param,
    SOC_SAND_INOUT ARAD_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA *tbl_data
  )
{
  uint32
    res,
    max_prob,
    calc,
    max_val_c1,
    max_avrg_th_16_byte;
  int32
    avrg_th_diff_wred_granular = 0;
  int32
    min_avrg_th_exact_wred_granular,
    max_avrg_th_exact_wred_granular;
  uint32
    trunced;
  SOC_SAND_U64
    u64_1,
    u64_2,
    u64_c2 = {{0}};

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_ITM_VSQ_WRED_QT_DP_INFO_TO_WRED_TBL_DATA);

  SOC_SAND_CHECK_NULL_INPUT(wred_param);
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);

  trunced = FALSE;

  /*
   * min_avrg_th
   */

  tbl_data->min_avrg_th = 0;
  max_avrg_th_16_byte = SOC_SAND_DIV_ROUND_UP(wred_param->min_avrg_th,ARAD_ITM_WRED_GRANULARITY);
  res = arad_itm_man_exp_buffer_set(
          max_avrg_th_16_byte,
          ARAD_ITM_VSQ_MIN_WRED_AVRG_TH_MNT_LSB,
          ARAD_ITM_VSQ_MIN_WRED_AVRG_TH_MNT_NOF_BITS,
          ARAD_ITM_VSQ_MIN_WRED_AVRG_TH_EXP_LSB,
          ARAD_ITM_VSQ_MIN_WRED_AVRG_TH_EXP_NOF_BITS,
          FALSE,
          &(tbl_data->min_avrg_th),
          &min_avrg_th_exact_wred_granular
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /* min_avrg_th_exact *= ARAD_ITM_WRED_GRANULARITY; */

  /*
   * max_avrg_th
   */
  tbl_data->max_avrg_th = 0;
  res = arad_itm_man_exp_buffer_set(
          SOC_SAND_DIV_ROUND_UP(wred_param->max_avrg_th,ARAD_ITM_WRED_GRANULARITY),
          ARAD_ITM_VSQ_MAX_WRED_AVRG_TH_MNT_LSB,
          ARAD_ITM_VSQ_MAX_WRED_AVRG_TH_MNT_NOF_BITS,
          ARAD_ITM_VSQ_MAX_WRED_AVRG_TH_EXP_LSB,
          ARAD_ITM_VSQ_MAX_WRED_AVRG_TH_EXP_NOF_BITS,
          FALSE,
          &(tbl_data->max_avrg_th),
          &max_avrg_th_exact_wred_granular
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  /* max_avrg_th_exact *= ARAD_ITM_WRED_GRANULARITY; */

  /*
   * max_packet_size
   */
  calc = wred_param->max_packet_size;
  if (calc > ARAD_ITM_WRED_MAX_PACKET_SIZE_FOR_CALC)
  {
    calc = ARAD_ITM_WRED_MAX_PACKET_SIZE_FOR_CALC;
  }
  calc = SOC_SAND_DIV_ROUND_UP(calc, ARAD_ITM_WRED_GRANULARITY);

  tbl_data->c3
    = soc_sand_log2_round_up(calc);

  /*
   *  Packet size ignore
   */
  tbl_data->vq_wred_pckt_sz_ignr = wred_param->ignore_packet_size;

  /*
   * max_probability
   */
  max_prob = (wred_param->max_probability);
  if(max_prob>=100)
  {
    max_prob = 99;
  }

  /*
   * max_probability
   * C1 = ((2^32)/100)*max-prob / (max-th - min-th) in powers of 2
   */
  calc = ARAD_WRED_NORMALIZE_FACTOR * max_prob;
    /*
     * We do not use 'SOC_SAND_DIV_ROUND' or 'SOC_SAND_DIV_ROUND_UP'
     * because at this point we might have in calc '((2^32)/100)*max-prob'
     * which can be very large number and the other dividers do ADD before
     * the division.
     */
  max_val_c1 = 31; /* soc_sand_log2_round_down(0xFFFFFFFF) */

  avrg_th_diff_wred_granular =
    (max_avrg_th_exact_wred_granular - min_avrg_th_exact_wred_granular);

  if(avrg_th_diff_wred_granular == 0)
  {
    tbl_data->c1 = max_val_c1;
  }
  else
  {
    calc = SOC_SAND_DIV_ROUND_DOWN(calc, avrg_th_diff_wred_granular);
    tbl_data->c1 = soc_sand_log2_round_down(calc);
  }
  if(tbl_data->c1 < max_val_c1)
  {
    /*
     * Check if a bigger C1 gives closer result of the value we add.
     */
    uint32
      now     = 1 <<(tbl_data->c1),
      changed = 1 <<(tbl_data->c1+1),
      diff_with_now,
      diff_with_change;

    diff_with_change = changed-calc;

    diff_with_now    = calc-now;
    if( diff_with_change < diff_with_now)
    {
      tbl_data->c1 += 1;
    }
  }

  SOC_SAND_LIMIT_FROM_ABOVE(tbl_data->c1, max_val_c1);

  if (max_avrg_th_16_byte > 0)
  {
    max_val_c1 = SOC_SAND_DIV_ROUND_DOWN(0xFFFFFFFF, max_avrg_th_16_byte);
    max_val_c1 = soc_sand_log2_round_down(max_val_c1);
    SOC_SAND_LIMIT_FROM_ABOVE(tbl_data->c1, max_val_c1);
  }

  /*
   * max_probability
   * C2 = FACTOR * max-prob * min-th / (max-th - min-th)
   */
  soc_sand_u64_multiply_longs(
    ARAD_WRED_NORMALIZE_FACTOR,
    max_prob * min_avrg_th_exact_wred_granular,
    &u64_2
  );
  soc_sand_u64_devide_u64_long(&u64_2, avrg_th_diff_wred_granular, &u64_c2);

  /*
   * P =
   */
  soc_sand_u64_multiply_longs(
    min_avrg_th_exact_wred_granular,
    (1 << tbl_data->c1),
    &u64_1
  );

  if(soc_sand_u64_is_bigger(&u64_c2, &u64_1))
  {
    soc_sand_os_memcpy(&u64_c2, &u64_1, sizeof(SOC_SAND_U64));
  }

  trunced = soc_sand_u64_to_long(&u64_c2, &tbl_data->c2);

  if (trunced)
  {
    tbl_data->c2 = 0xFFFFFFFF;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_WRED_QT_DP_INFO_to_WRED_TBL_DATA()",0,0);
}

/*********************************************************************
*    Gets the VSQ-WRED parameters according to vsq-queue-type and
*    drop-precedence from the values in the table.
*    By converting them appropriately.
*********************************************************************/
STATIC uint32
  arad_itm_vsq_WRED_TBL_DATA_to_WRED_QT_DP_INFO(
    SOC_SAND_IN  ARAD_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA  *tbl_data,
    SOC_SAND_OUT ARAD_ITM_WRED_QT_DP_INFO                             *wred_param
  )
{
  uint32
    res;
  uint32
    avrg_th_diff_wred_granular,
    two_power_c1,
    remainder;
  SOC_SAND_U64
    u64_1,
    u64_2;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_ITM_VSQ_WRED_TBL_DATA_TO_WRED_QT_DP_INFO);

  SOC_SAND_CHECK_NULL_INPUT(wred_param);
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);



  res = arad_itm_man_exp_buffer_get(
          tbl_data->min_avrg_th,
          ARAD_ITM_VSQ_MIN_WRED_AVRG_TH_MNT_LSB,
          ARAD_ITM_VSQ_MIN_WRED_AVRG_TH_MNT_NOF_BITS,
          ARAD_ITM_VSQ_MIN_WRED_AVRG_TH_EXP_LSB,
          ARAD_ITM_VSQ_MIN_WRED_AVRG_TH_EXP_NOF_BITS,
          FALSE,
          (int32*)&(wred_param->min_avrg_th)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  wred_param->min_avrg_th *= ARAD_ITM_WRED_GRANULARITY;

  /*
   * max_avrg_th
   */

  res = arad_itm_man_exp_buffer_get(
          tbl_data->max_avrg_th,
          ARAD_ITM_VSQ_MAX_WRED_AVRG_TH_MNT_LSB,
          ARAD_ITM_VSQ_MAX_WRED_AVRG_TH_MNT_NOF_BITS,
          ARAD_ITM_VSQ_MAX_WRED_AVRG_TH_EXP_LSB,
          ARAD_ITM_VSQ_MAX_WRED_AVRG_TH_EXP_NOF_BITS,
          FALSE,
          (int32*)&(wred_param->max_avrg_th)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  wred_param->max_avrg_th *= ARAD_ITM_WRED_GRANULARITY;

  /*
   * max_packet_size
   */

  wred_param->max_packet_size =
    (0x1<<(tbl_data->c3))*ARAD_ITM_WRED_GRANULARITY;

  /*
   *  Packet size ignore
   */
  wred_param->ignore_packet_size = SOC_SAND_NUM2BOOL(tbl_data->vq_wred_pckt_sz_ignr);


  avrg_th_diff_wred_granular =
    (wred_param->max_avrg_th - wred_param->min_avrg_th) / ARAD_ITM_WRED_GRANULARITY;

  two_power_c1 = 1<<tbl_data->c1;
  /*
   * C1 = ((2^32)/100)*max-prob / (max-th - min-th) in powers of 2
   * ==>
   * max-prob =  ( 2^C1 * (max-th - min-th) ) / ((2^32)/100)
   */
  soc_sand_u64_multiply_longs(two_power_c1, avrg_th_diff_wred_granular, &u64_1);
  remainder = soc_sand_u64_devide_u64_long(&u64_1, ARAD_WRED_NORMALIZE_FACTOR, &u64_2);
  soc_sand_u64_to_long(&u64_2, &wred_param->max_probability);

  if(remainder > (ARAD_WRED_NORMALIZE_FACTOR/2))
  {
    wred_param->max_probability++;
  }

  if(wred_param->max_probability > 100)
  {
    wred_param->max_probability = 100;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_WRED_TBL_DATA_to_WRED_QT_DP_INFO()",0,0);
}

/*********************************************************************
*     DRAM buffers are used to store packets at the ingress.
*     This is a resource shared between Unicast,
*     Full-Multicast and Mini-Multicast packets. There are 2M
*     buffers available. This function sets the buffers share
*     dedicated for Unicast, Full-Multicast and Mini-Multicast
*     packets. This function also sets the size of a single
*     buffer. See remarks below for limitations.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_dram_buffs_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ITM_DRAM_BUFFERS_INFO *dram_buffs
  )
{
  uint32
    uc_start,
    uc_end,
    uc_enable_val,
    uc_size,
    fmc_start,
    fmc_end,
    fmc_enable_val,
    fmc_size,
    mmc_start,
    mmc_end,
    mmc_enable_val,
    mmc_size,
    dbuff_size_internal,
    res;
   
    

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_DRAM_BUFFS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(dram_buffs);

  

  /*
   *  ECI:
   *  Get Buffer-pointers boundaries.
   *  Start from the beginning, and go upwards;
   *  Unicast -> Mini Multicast -> Full Multicast ->
   */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, ECI_GLOBAL_4r, REG_PORT_ANY, 0, UC_DB_PTR_STARTf, &uc_start));
  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, ECI_GLOBAL_5r, REG_PORT_ANY, 0, UC_DB_PTR_ENDf, &uc_end));

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, ECI_GLOBAL_6r, REG_PORT_ANY, 0, MN_MUL_DB_PTR_STARTf, &mmc_start));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, ECI_GLOBAL_7r, REG_PORT_ANY, 0, MN_MUL_DB_PTR_ENDf, &mmc_end));

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, ECI_GLOBAL_8r, REG_PORT_ANY, 0, FL_MUL_DB_PTR_STARTf, &fmc_start));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit , ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, ECI_GLOBAL_9r, REG_PORT_ANY, 0, FL_MUL_DB_PTR_ENDf, &fmc_end));

  if (uc_end > uc_start)
  {
    uc_size = uc_end - uc_start + 1;
  }
  else
  {
    if (uc_end == uc_start)
    {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit , ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_UNICAST_AUTOGEN_ENABLEf, &uc_enable_val));
      if (SOC_SAND_NUM2BOOL(uc_enable_val) == TRUE)
      {
        /* size == 1 */
        uc_size = uc_end - uc_start + 1;
      }
      else
      {
        uc_size = 0x0;
      }
    }
    else
    {
      uc_size = 0x0;
    }
  }

  if (mmc_end > mmc_start)
  {
    mmc_size = mmc_end - mmc_start + 1;
  }
  else
  {
    if (mmc_end == mmc_start)
    {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_MINI_MULTICAST_AUTOGEN_ENABLEf, &mmc_enable_val));
      if (SOC_SAND_NUM2BOOL(mmc_enable_val) == TRUE)
      {
        /* size == 1 */
        mmc_size = mmc_end - mmc_start + 1;
      }
      else
      {
        mmc_size = 0x0;
      }
    }
    else
    {
      mmc_size = 0x0;
    }
  }

  if (fmc_end > fmc_start)
  {
    fmc_size = fmc_end - fmc_start + 1;
  }
  else
  {
    if (fmc_end == fmc_start)
    {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IDR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, FBC_FULL_MULTICAST_AUTOGEN_ENABLEf, &fmc_enable_val));
      if (SOC_SAND_NUM2BOOL(fmc_enable_val) == TRUE)
      {
        /* size == 1 */
        fmc_size = fmc_end - fmc_start + 1;
      }
      else
      {
        fmc_size = 0x0;
      }
    }
    else
    {
      fmc_size = 0x0;
    }
  }

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  90,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, ECI_GLOBAL_1r, REG_PORT_ANY, 0, DBUFF_SIZEf, &dbuff_size_internal));

  res = arad_itm_dbuff_internal2size(
          dbuff_size_internal,
          &(dram_buffs->dbuff_size)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  dram_buffs->full_mc_nof_buffs = fmc_size;
  dram_buffs->mini_mc_nof_buffs = mmc_size;
  dram_buffs->uc_nof_buffs = uc_size;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_dram_buffs_get_unsafe()",0,0);
}

/*********************************************************************
*     Sets the Thresholds to trigger/clear the Flow Control
*     Indication. For the different kinds of general resources
*     (bds, unicast, multicast).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_glob_rcs_fc_set_unsafe(
    SOC_SAND_IN   int                 unit,
    SOC_SAND_IN   ARAD_ITM_GLOB_RCS_FC_TH  *info,
    SOC_SAND_OUT  ARAD_ITM_GLOB_RCS_FC_TH  *exact_info
  )
{
  uint32
    res,
    exp_man,
    bdbs_th_mnt_nof_bits,
    bdbs_th_exp_nof_bits,
    uc_th_mnt_nof_bits,
    uc_th_exp_nof_bits,
    fmc_th_mnt_nof_bits,
    fmc_th_exp_nof_bits;
  uint32
    mnt_val,
    exp_val;
   
    

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_GLOB_RCS_FC_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_CHECK_NULL_INPUT(exact_info);

  

  /*
    info->bdbs.hp.clear
    info->bdbs.hp.set
    info->bdbs.lp.clear
    info->bdbs.lp.set
  */

  bdbs_th_mnt_nof_bits = 8;
  bdbs_th_exp_nof_bits = soc_reg_field_length(unit, IQM_GENERAL_FLOW_CONTROL_CONFIGURATION_BDB_HIGH_PRIORITYr, FC_SET_FR_BDB_TH_HPf) - bdbs_th_mnt_nof_bits;

  res = soc_sand_break_to_mnt_exp_round_up(
          info->bdbs.hp.clear,
          bdbs_th_mnt_nof_bits,
          bdbs_th_exp_nof_bits,
          0,
          &mnt_val,
          &exp_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  exact_info->bdbs.hp.clear = (mnt_val) * (1<<(exp_val));

  arad_iqm_mantissa_exponent_set(
      unit,
      mnt_val,
      exp_val,
      bdbs_th_mnt_nof_bits,
      &exp_man
    );

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  110,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_GENERAL_FLOW_CONTROL_CONFIGURATION_BDB_HIGH_PRIORITYr, REG_PORT_ANY, 0, FC_CLR_FR_BDB_TH_HPf,  exp_man));

  res = soc_sand_break_to_mnt_exp_round_up(
          info->bdbs.hp.set,
          bdbs_th_mnt_nof_bits,
          bdbs_th_exp_nof_bits,
          0,
          &mnt_val,
          &exp_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  exact_info->bdbs.hp.set = (mnt_val) * (1<<(exp_val));

  arad_iqm_mantissa_exponent_set(
      unit,
      mnt_val,
      exp_val,
      bdbs_th_mnt_nof_bits,
      &exp_man
    );

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  130,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_GENERAL_FLOW_CONTROL_CONFIGURATION_BDB_HIGH_PRIORITYr, REG_PORT_ANY, 0, FC_SET_FR_BDB_TH_HPf,  exp_man));

  res = soc_sand_break_to_mnt_exp_round_up(
          info->bdbs.lp.clear,
          bdbs_th_mnt_nof_bits,
          bdbs_th_exp_nof_bits,
          0,
          &mnt_val,
          &exp_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  exact_info->bdbs.lp.clear = (mnt_val) * (1<<(exp_val));

  arad_iqm_mantissa_exponent_set(
      unit,
      mnt_val,
      exp_val,
      bdbs_th_mnt_nof_bits,
      &exp_man
    );

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  150,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_GENERAL_FLOW_CONTROL_CONFIGURATION_BDB_LOW_PRIORITYr, REG_PORT_ANY, 0, FC_CLR_FR_BDB_TH_LPf,  exp_man));

   res = soc_sand_break_to_mnt_exp_round_up(
          info->bdbs.lp.set,
          bdbs_th_mnt_nof_bits,
          bdbs_th_exp_nof_bits,
          0,
          &mnt_val,
          &exp_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  exact_info->bdbs.lp.set = (mnt_val) * (1<<(exp_val));

  arad_iqm_mantissa_exponent_set(
      unit,
      mnt_val,
      exp_val,
      bdbs_th_mnt_nof_bits,
      &exp_man
    );
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  170,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_GENERAL_FLOW_CONTROL_CONFIGURATION_BDB_LOW_PRIORITYr, REG_PORT_ANY, 0, FC_SET_FR_BDB_TH_LPf,  exp_man));

  /*
    info->unicast.hp.clear
    info->unicast.hp.set
    info->unicast.lp.clear
    info->unicast.lp.set
  */

  uc_th_mnt_nof_bits = 7;
  uc_th_exp_nof_bits = soc_reg_field_length(unit, IQM_GENERAL_CLEAR_FLOW_CONTROL_CONFIGURATION_D_BS_HIGH_PRIORITYr, FC_CLR_FR_DB_UC_TH_HPf) - uc_th_mnt_nof_bits;

  res = soc_sand_break_to_mnt_exp_round_up(
          info->unicast.hp.clear,
          uc_th_mnt_nof_bits,
          uc_th_exp_nof_bits,
          0,
          &mnt_val,
          &exp_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  exact_info->unicast.hp.clear = (mnt_val) * (1<<(exp_val));

  arad_iqm_mantissa_exponent_set(
      unit,
      mnt_val,
      exp_val,
      uc_th_mnt_nof_bits,
      &exp_man
    );
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  210,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_GENERAL_CLEAR_FLOW_CONTROL_CONFIGURATION_D_BS_HIGH_PRIORITYr, REG_PORT_ANY, 0, FC_CLR_FR_DB_UC_TH_HPf,  exp_man));

  res = soc_sand_break_to_mnt_exp_round_up(
          info->unicast.hp.set,
          uc_th_mnt_nof_bits,
          uc_th_exp_nof_bits,
          0,
          &mnt_val,
          &exp_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  exact_info->unicast.hp.set = (mnt_val) * (1<<(exp_val));

  arad_iqm_mantissa_exponent_set(
      unit,
      mnt_val,
      exp_val,
      uc_th_mnt_nof_bits,
      &exp_man
    );
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  230,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_GENERAL_SET_FLOW_CONTROL_CONFIGURATION_D_BS_HIGH_PRIORITYr, REG_PORT_ANY, 0, FC_SET_FR_DB_UC_TH_HPf,  exp_man));

  res = soc_sand_break_to_mnt_exp_round_up(
          info->unicast.lp.clear,
          uc_th_mnt_nof_bits,
          uc_th_exp_nof_bits,
          0,
          &mnt_val,
          &exp_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  exact_info->unicast.lp.clear = (mnt_val) * (1<<(exp_val));

  arad_iqm_mantissa_exponent_set(
      unit,
      mnt_val,
      exp_val,
      uc_th_mnt_nof_bits,
      &exp_man
    );
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  250,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_GENERAL_CLEAR_FLOW_CONTROL_CONFIGURATION_D_BS_LOW_PRIORITYr, REG_PORT_ANY, 0, FC_CLR_FR_DB_UC_TH_LPf,  exp_man));

   res = soc_sand_break_to_mnt_exp_round_up(
          info->unicast.lp.set,
          uc_th_mnt_nof_bits,
          uc_th_exp_nof_bits,
          0,
          &mnt_val,
          &exp_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  exact_info->unicast.lp.set = (mnt_val) * (1<<(exp_val));

  arad_iqm_mantissa_exponent_set(
      unit,
      mnt_val,
      exp_val,
      uc_th_mnt_nof_bits,
      &exp_man
    );
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  270,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_GENERAL_SET_FLOW_CONTROL_CONFIGURATION_D_BS_LOW_PRIORITYr, REG_PORT_ANY, 0, FC_SET_FR_DB_UC_TH_LPf,  exp_man));

  /*
    info->full_mc.hp.clear
    info->full_mc.hp.set
    info->full_mc.lp.clear
    info->full_mc.lp.set
  */

  fmc_th_mnt_nof_bits = 7;
  fmc_th_exp_nof_bits = soc_reg_field_length(unit, IQM_GENERAL_CLEAR_FLOW_CONTROL_CONFIGURATION_D_BS_HIGH_PRIORITYr, FC_CLR_FR_DB_FLMC_TH_HPf) - fmc_th_mnt_nof_bits;

  res = soc_sand_break_to_mnt_exp_round_up(
          info->full_mc.hp.clear,
          fmc_th_mnt_nof_bits,
          fmc_th_exp_nof_bits,
          0,
          &mnt_val,
          &exp_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  exact_info->full_mc.hp.clear = (mnt_val) * (1<<(exp_val));

  arad_iqm_mantissa_exponent_set(
      unit,
      mnt_val,
      exp_val,
      fmc_th_mnt_nof_bits,
      &exp_man
    );
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  310,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_GENERAL_CLEAR_FLOW_CONTROL_CONFIGURATION_D_BS_HIGH_PRIORITYr, REG_PORT_ANY, 0, FC_CLR_FR_DB_FLMC_TH_HPf,  exp_man));

  res = soc_sand_break_to_mnt_exp_round_up(
          info->full_mc.hp.set,
          fmc_th_mnt_nof_bits,
          fmc_th_exp_nof_bits,
          0,
          &mnt_val,
          &exp_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  exact_info->full_mc.hp.set = (mnt_val) * (1<<(exp_val));

  arad_iqm_mantissa_exponent_set(
      unit,
      mnt_val,
      exp_val,
      fmc_th_mnt_nof_bits,
      &exp_man
    );
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  330,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_GENERAL_SET_FLOW_CONTROL_CONFIGURATION_D_BS_HIGH_PRIORITYr, REG_PORT_ANY, 0, FC_SET_FR_DB_FLMC_TH_HPf,  exp_man));

  res = soc_sand_break_to_mnt_exp_round_up(
          info->full_mc.lp.clear,
          fmc_th_mnt_nof_bits,
          fmc_th_exp_nof_bits,
          0,
          &mnt_val,
          &exp_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  exact_info->full_mc.lp.clear = (mnt_val) * (1<<(exp_val));

  arad_iqm_mantissa_exponent_set(
      unit,
      mnt_val,
      exp_val,
      fmc_th_mnt_nof_bits,
      &exp_man
    );
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  350,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_GENERAL_CLEAR_FLOW_CONTROL_CONFIGURATION_D_BS_LOW_PRIORITYr, REG_PORT_ANY, 0, FC_CLR_FR_DB_FLMC_TH_LPf,  exp_man));

   res = soc_sand_break_to_mnt_exp_round_up(
          info->full_mc.lp.set,
          fmc_th_mnt_nof_bits,
          fmc_th_exp_nof_bits,
          0,
          &mnt_val,
          &exp_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  exact_info->full_mc.lp.set = (mnt_val) * (1<<(exp_val));

  arad_iqm_mantissa_exponent_set(
      unit,
      mnt_val,
      exp_val,
      fmc_th_mnt_nof_bits,
      &exp_man
    );
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  370,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_GENERAL_SET_FLOW_CONTROL_CONFIGURATION_D_BS_LOW_PRIORITYr, REG_PORT_ANY, 0, FC_SET_FR_DB_FLMC_TH_LPf,  exp_man));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_glob_rcs_fc_set_unsafe()",0,0);
}

/*********************************************************************
*     Sets the Thresholds to trigger/clear the Flow Control
*     Indication. For the different kinds of general resources
*     (bds, unicast, multicast).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_glob_rcs_fc_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_GLOB_RCS_FC_TH  *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_GLOB_RCS_FC_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  /* BDBs */

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->bdbs.hp.clear, ARAD_ITM_GLOB_RCS_FC_BDBS_SIZE_MAX,
    ARAD_ITM_GLOB_RCS_FC_SIZE_OUT_OF_RANGE_ERR, 10, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->bdbs.hp.set, ARAD_ITM_GLOB_RCS_FC_BDBS_SIZE_MAX,
    ARAD_ITM_GLOB_RCS_FC_SIZE_OUT_OF_RANGE_ERR, 20, exit
    );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->bdbs.lp.clear, ARAD_ITM_GLOB_RCS_FC_BDBS_SIZE_MAX,
    ARAD_ITM_GLOB_RCS_FC_SIZE_OUT_OF_RANGE_ERR, 30, exit
    );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->bdbs.lp.set, ARAD_ITM_GLOB_RCS_FC_BDBS_SIZE_MAX,
    ARAD_ITM_GLOB_RCS_FC_SIZE_OUT_OF_RANGE_ERR, 40, exit
    );

  /* Unicast */
  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->unicast.hp.set, ARAD_ITM_GLOB_RCS_FC_UC_SIZE_MAX,
    ARAD_ITM_GLOB_RCS_FC_SIZE_OUT_OF_RANGE_ERR, 50, exit
    );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->unicast.hp.clear, ARAD_ITM_GLOB_RCS_FC_UC_SIZE_MAX,
    ARAD_ITM_GLOB_RCS_FC_SIZE_OUT_OF_RANGE_ERR, 60, exit
    );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->unicast.lp.set, ARAD_ITM_GLOB_RCS_FC_UC_SIZE_MAX,
    ARAD_ITM_GLOB_RCS_FC_SIZE_OUT_OF_RANGE_ERR, 70, exit
    );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->unicast.lp.clear, ARAD_ITM_GLOB_RCS_FC_UC_SIZE_MAX,
    ARAD_ITM_GLOB_RCS_FC_SIZE_OUT_OF_RANGE_ERR, 80, exit
    );

  /* Full- Multicast */
  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->full_mc.hp.set, ARAD_ITM_GLOB_RCS_FC_FMC_SIZE_MAX,
    ARAD_ITM_GLOB_RCS_FC_SIZE_OUT_OF_RANGE_ERR, 90, exit
    );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->full_mc.hp.clear, ARAD_ITM_GLOB_RCS_FC_FMC_SIZE_MAX,
    ARAD_ITM_GLOB_RCS_FC_SIZE_OUT_OF_RANGE_ERR, 100, exit
    );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->full_mc.lp.set, ARAD_ITM_GLOB_RCS_FC_FMC_SIZE_MAX,
    ARAD_ITM_GLOB_RCS_FC_SIZE_OUT_OF_RANGE_ERR, 110, exit
    );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->full_mc.lp.clear, ARAD_ITM_GLOB_RCS_FC_FMC_SIZE_MAX,
    ARAD_ITM_GLOB_RCS_FC_SIZE_OUT_OF_RANGE_ERR, 120, exit
    );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_glob_rcs_fc_verify()",0,0);
}

/*********************************************************************
*     Sets the Thresholds to trigger/clear the Flow Control
*     Indication. For the different kinds of general resources
*     (bds, unicast, multicast).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_glob_rcs_fc_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ITM_GLOB_RCS_FC_TH  *info
  )
{
  uint32
      res,
      bdbs_th_mnt_nof_bits,
      uc_th_mnt_nof_bits,
      fmc_th_mnt_nof_bits,
      exp_man,
    mnt_val,
    exp_val;
   
    

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_GLOB_RCS_FC_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  bdbs_th_mnt_nof_bits = 8;
  

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  110,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_GENERAL_FLOW_CONTROL_CONFIGURATION_BDB_HIGH_PRIORITYr, REG_PORT_ANY, 0, FC_CLR_FR_BDB_TH_HPf, &exp_man));

  arad_iqm_mantissa_exponent_get(
      unit,
      exp_man,
      bdbs_th_mnt_nof_bits,
      &mnt_val,
      &exp_val
  );

  info->bdbs.hp.clear = (mnt_val) * (1<<(exp_val));

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  130,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_GENERAL_FLOW_CONTROL_CONFIGURATION_BDB_HIGH_PRIORITYr, REG_PORT_ANY, 0, FC_SET_FR_BDB_TH_HPf, &exp_man));
  arad_iqm_mantissa_exponent_get(
      unit,
      exp_man,
      bdbs_th_mnt_nof_bits,
      &mnt_val,
      &exp_val
  );

  info->bdbs.hp.set = (mnt_val) * (1<<(exp_val));

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  150,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_GENERAL_FLOW_CONTROL_CONFIGURATION_BDB_LOW_PRIORITYr, REG_PORT_ANY, 0, FC_CLR_FR_BDB_TH_LPf, &exp_man));
  arad_iqm_mantissa_exponent_get(
      unit,
      exp_man,
      bdbs_th_mnt_nof_bits,
      &mnt_val,
      &exp_val
  );

  info->bdbs.lp.clear = (mnt_val) * (1<<(exp_val));

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  170,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_GENERAL_FLOW_CONTROL_CONFIGURATION_BDB_LOW_PRIORITYr, REG_PORT_ANY, 0, FC_SET_FR_BDB_TH_LPf, &exp_man));
  arad_iqm_mantissa_exponent_get(
      unit,
      exp_man,
      bdbs_th_mnt_nof_bits,
      &mnt_val,
      &exp_val
  );

  info->bdbs.lp.set = (mnt_val) * (1<<(exp_val));

  /*
    info->unicast.hp.clear
    info->unicast.hp.set
    info->unicast.lp.clear
    info->unicast.lp.set
  */

  uc_th_mnt_nof_bits = 7;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  210,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_GENERAL_CLEAR_FLOW_CONTROL_CONFIGURATION_D_BS_HIGH_PRIORITYr, REG_PORT_ANY, 0, FC_CLR_FR_DB_UC_TH_HPf, &exp_man));
  arad_iqm_mantissa_exponent_get(
      unit,
      exp_man,
      uc_th_mnt_nof_bits,
      &mnt_val,
      &exp_val
  );

  info->unicast.hp.clear = (mnt_val) * (1<<(exp_val));

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  230,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_GENERAL_SET_FLOW_CONTROL_CONFIGURATION_D_BS_HIGH_PRIORITYr, REG_PORT_ANY, 0, FC_SET_FR_DB_UC_TH_HPf, &exp_man));
  arad_iqm_mantissa_exponent_get(
      unit,
      exp_man,
      uc_th_mnt_nof_bits,
      &mnt_val,
      &exp_val
  );

  info->unicast.hp.set = (mnt_val) * (1<<(exp_val));

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  250,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_GENERAL_CLEAR_FLOW_CONTROL_CONFIGURATION_D_BS_LOW_PRIORITYr, REG_PORT_ANY, 0, FC_CLR_FR_DB_UC_TH_LPf, &exp_man));
  arad_iqm_mantissa_exponent_get(
      unit,
      exp_man,
      uc_th_mnt_nof_bits,
      &mnt_val,
      &exp_val
  );

  info->unicast.lp.clear = (mnt_val) * (1<<(exp_val));

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  270,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_GENERAL_SET_FLOW_CONTROL_CONFIGURATION_D_BS_LOW_PRIORITYr, REG_PORT_ANY, 0, FC_SET_FR_DB_UC_TH_LPf, &exp_man));
  arad_iqm_mantissa_exponent_get(
      unit,
      exp_man,
      uc_th_mnt_nof_bits,
      &mnt_val,
      &exp_val
  );

  info->unicast.lp.set = (mnt_val) * (1<<(exp_val));

  /*
    info->full_mc.hp.clear
    info->full_mc.hp.set
    info->full_mc.lp.clear
    info->full_mc.lp.set
  */

  fmc_th_mnt_nof_bits = 7;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  310,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_GENERAL_CLEAR_FLOW_CONTROL_CONFIGURATION_D_BS_HIGH_PRIORITYr, REG_PORT_ANY, 0, FC_CLR_FR_DB_FLMC_TH_HPf, &exp_man));
  arad_iqm_mantissa_exponent_get(
      unit,
      exp_man,
      fmc_th_mnt_nof_bits,
      &mnt_val,
      &exp_val
  );

  info->full_mc.hp.clear = (mnt_val) * (1<<(exp_val));

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  330,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_GENERAL_SET_FLOW_CONTROL_CONFIGURATION_D_BS_HIGH_PRIORITYr, REG_PORT_ANY, 0, FC_SET_FR_DB_FLMC_TH_HPf, &exp_man));
  arad_iqm_mantissa_exponent_get(
      unit,
      exp_man,
      fmc_th_mnt_nof_bits,
      &mnt_val,
      &exp_val
  );

  info->full_mc.hp.set = (mnt_val) * (1<<(exp_val));

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  350,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_GENERAL_CLEAR_FLOW_CONTROL_CONFIGURATION_D_BS_LOW_PRIORITYr, REG_PORT_ANY, 0, FC_CLR_FR_DB_FLMC_TH_LPf, &exp_man));
  arad_iqm_mantissa_exponent_get(
      unit,
      exp_man,
      fmc_th_mnt_nof_bits,
      &mnt_val,
      &exp_val
  );

  info->full_mc.lp.clear = (mnt_val) * (1<<(exp_val));

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  370,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_GENERAL_SET_FLOW_CONTROL_CONFIGURATION_D_BS_LOW_PRIORITYr, REG_PORT_ANY, 0, FC_SET_FR_DB_FLMC_TH_LPf, &exp_man));
  arad_iqm_mantissa_exponent_get(
      unit,
      exp_man,
      fmc_th_mnt_nof_bits,
      &mnt_val,
      &exp_val
  );

  info->full_mc.lp.set = (mnt_val) * (1<<(exp_val));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_glob_rcs_fc_get_unsafe()",0,0);
}

/*********************************************************************
*     Sets the Thresholds to trigger/clear the Drop mechanism,
*     in which packets are dropped if the buffers of the
*     different kinds have passed their hysteresis thresholds.
*     For the different kinds of general resources (bds,
*     unicast, full-multicast, mini-multicast).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_glob_rcs_drop_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_GLOB_RCS_DROP_TH *info,
    SOC_SAND_OUT ARAD_ITM_GLOB_RCS_DROP_TH *exact_info
  )
{
  uint32
    res,
      exp_man,
    bdbs_th_mnt_nof_bits,
    bdbs_th_exp_nof_bits,
    bds_th_mnt_nof_bits,
    bds_th_exp_nof_bits,
    uc_th_mnt_nof_bits,
    uc_th_exp_nof_bits,
    mini_mc_mnt_nof_bits,
    mini_mc_exp_nof_bits,
    fmc_th_mnt_nof_bits,
    fmc_th_exp_nof_bits;
  uint32
    mnt_val,
    exp_val,
    fld_val,
    indx;
   
    

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_GLOB_RCS_DROP_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_CHECK_NULL_INPUT(exact_info);

  

  /*
   *  BDB-s (Buffer Descriptor Buffers)
   */

  bdbs_th_mnt_nof_bits = 8;
  bdbs_th_exp_nof_bits = soc_reg_field_length(unit, IQM_GENERAL_REJECT_CONFIGURATION_BD_BS_0r, RJCT_CLR_FR_BDB_TH_0f) - bdbs_th_mnt_nof_bits;

  for (indx = 0 ; indx < ARAD_NOF_DROP_PRECEDENCE ; indx++)
  {
      res = soc_sand_break_to_mnt_exp_round_up(
              info->bdbs[indx].clear,
              bdbs_th_mnt_nof_bits,
              bdbs_th_exp_nof_bits,
              0,
              &mnt_val,
              &exp_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      exact_info->bdbs[indx].clear = (mnt_val) * (1<<(exp_val));

      arad_iqm_mantissa_exponent_set(
          unit,
          mnt_val,
          exp_val,
          bdbs_th_mnt_nof_bits,
          &exp_man
        );
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_GENERAL_REJECT_CONFIGURATION_BD_BS_0r + indx, REG_PORT_ANY,  0, RJCT_CLR_FR_BDB_TH_0f + indx,  exp_man));

      res = soc_sand_break_to_mnt_exp_round_up(
              info->bdbs[indx].set,
              bdbs_th_mnt_nof_bits,
              bdbs_th_exp_nof_bits,
              0,
              &mnt_val,
              &exp_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      exact_info->bdbs[indx].set = (mnt_val) * (1<<(exp_val));

      arad_iqm_mantissa_exponent_set(
          unit,
          mnt_val,
          exp_val,
          bdbs_th_mnt_nof_bits,
          &exp_man
        );
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_GENERAL_REJECT_CONFIGURATION_BD_BS_0r + indx, REG_PORT_ANY,  0, RJCT_SET_FR_BDB_TH_0f + indx,  exp_man));
  }

  /*
   *  BD-s (Buffer Descriptors)
   */
  bds_th_mnt_nof_bits = 12;
  bds_th_exp_nof_bits = soc_reg_field_length(unit, IQM_GENERAL_REJECT_CONFIGURATION_B_DS_0r, RJCT_CLR_OC_BD_TH_0f) - bds_th_mnt_nof_bits;

  for (indx = 0 ; indx < ARAD_NOF_DROP_PRECEDENCE ; indx++)
  {
      res = soc_sand_break_to_mnt_exp_round_up(
              info->bds[indx].clear,
              bds_th_mnt_nof_bits,
              bds_th_exp_nof_bits,
              0,
              &mnt_val,
              &exp_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

      exact_info->bds[indx].clear = (mnt_val) * (1<<(exp_val));

      arad_iqm_mantissa_exponent_set(
          unit,
          mnt_val,
          exp_val,
          bds_th_mnt_nof_bits,
          &exp_man
        );
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_GENERAL_REJECT_CONFIGURATION_B_DS_0r + indx, REG_PORT_ANY,  0, RJCT_CLR_OC_BD_TH_0f + indx,  exp_man));

      res = soc_sand_break_to_mnt_exp_round_up(
              info->bds[indx].set,
              bds_th_mnt_nof_bits,
              bds_th_exp_nof_bits,
              0,
              &mnt_val,
              &exp_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

      exact_info->bds[indx].set = (mnt_val) * (1<<(exp_val));

      arad_iqm_mantissa_exponent_set(
          unit,
          mnt_val,
          exp_val,
          bds_th_mnt_nof_bits,
          &exp_man
        );
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_GENERAL_REJECT_CONFIGURATION_B_DS_0r + indx, REG_PORT_ANY,  0, RJCT_SET_OC_BD_TH_0f + indx,  exp_man));
  }

  /*
    info->unicast[index].clear
    info->unicast[index].set
  */
  uc_th_mnt_nof_bits = 7;
  uc_th_exp_nof_bits = soc_reg_field_length(unit, IQM_GENERAL_REJECT_CLEAR_CONFIGURATION_D_BUFFS_0r, RJCT_CLR_FR_DB_UC_TH_0f) - uc_th_mnt_nof_bits;

  for (indx = 0 ; indx < ARAD_NOF_DROP_PRECEDENCE ; indx++)
  {
      res = soc_sand_break_to_mnt_exp_round_up(
              info->unicast[indx].clear,
              uc_th_mnt_nof_bits,
              uc_th_exp_nof_bits,
              0,
              &mnt_val,
              &exp_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

      exact_info->unicast[indx].clear = (mnt_val) * (1<<(exp_val));

      arad_iqm_mantissa_exponent_set(
          unit,
          mnt_val,
          exp_val,
          uc_th_mnt_nof_bits,
          &exp_man
        );
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  100,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_GENERAL_REJECT_CLEAR_CONFIGURATION_D_BUFFS_0r + indx, REG_PORT_ANY,  0, RJCT_CLR_FR_DB_UC_TH_0f + indx,  exp_man));

      res = soc_sand_break_to_mnt_exp_round_up(
              info->unicast[indx].set,
              uc_th_mnt_nof_bits,
              uc_th_exp_nof_bits,
              0,
              &mnt_val,
              &exp_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

      exact_info->unicast[indx].set = (mnt_val) * (1<<(exp_val));

      arad_iqm_mantissa_exponent_set(
          unit,
          mnt_val,
          exp_val,
          uc_th_mnt_nof_bits,
          &exp_man
        );
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  120,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_GENERAL_REJECT_SET_CONFIGURATION_D_BUFFS_0r + indx, REG_PORT_ANY,  0, RJCT_SET_FR_DB_UC_TH_0f + indx,  exp_man));
  }

  /*
    info->mini_mc[index].clear
    info->mini_mc[index].set
  */

  mini_mc_mnt_nof_bits = 6;
  mini_mc_exp_nof_bits = soc_reg_field_length(unit, IQM_GENERAL_REJECT_CLEAR_CONFIGURATION_D_BUFFS_0r, RJCT_CLR_FR_DB_MNMC_TH_0f) - mini_mc_mnt_nof_bits;

  for (indx = 0 ; indx < ARAD_NOF_DROP_PRECEDENCE ; indx++)
  {
      res = soc_sand_break_to_mnt_exp_round_up(
              info->mini_mc[indx].clear,
              mini_mc_mnt_nof_bits,
              mini_mc_exp_nof_bits,
              0,
              &mnt_val,
              &exp_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);

      exact_info->mini_mc[indx].clear = (mnt_val) * (1<<(exp_val));

      arad_iqm_mantissa_exponent_set(
          unit,
          mnt_val,
          exp_val,
          mini_mc_mnt_nof_bits,
          &exp_man
        );
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  140,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_GENERAL_REJECT_CLEAR_CONFIGURATION_D_BUFFS_0r + indx, REG_PORT_ANY,  0, RJCT_CLR_FR_DB_MNMC_TH_0f + indx,  exp_man));

      res = soc_sand_break_to_mnt_exp_round_up(
              info->mini_mc[indx].set,
              mini_mc_mnt_nof_bits,
              mini_mc_exp_nof_bits,
              0,
              &mnt_val,
              &exp_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

      exact_info->mini_mc[indx].set = (mnt_val) * (1<<(exp_val));

      arad_iqm_mantissa_exponent_set(
          unit,
          mnt_val,
          exp_val,
          mini_mc_mnt_nof_bits,
          &exp_man
        );
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  160,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_GENERAL_REJECT_SET_CONFIGURATION_D_BUFFS_0r + indx, REG_PORT_ANY,  0, RJCT_SET_FR_DB_MNMC_TH_0f + indx,  exp_man));
  }

  /*
    info->full_mc[index].clear
    info->full_mc[index].set
   */

  fmc_th_mnt_nof_bits = 7;
  fmc_th_exp_nof_bits = soc_reg_field_length(unit, IQM_GENERAL_REJECT_CLEAR_CONFIGURATION_D_BUFFS_0r, RJCT_CLR_FR_DB_FLMC_TH_0f) - fmc_th_mnt_nof_bits;

  for (indx = 0 ; indx < ARAD_NOF_DROP_PRECEDENCE ; indx++)
  {
      res = soc_sand_break_to_mnt_exp_round_up(
              info->full_mc[indx].clear,
              fmc_th_mnt_nof_bits,
              fmc_th_exp_nof_bits,
              0,
              &mnt_val,
              &exp_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);

      exact_info->full_mc[indx].clear = (mnt_val) * (1<<(exp_val));

      arad_iqm_mantissa_exponent_set(
          unit,
          mnt_val,
          exp_val,
          fmc_th_mnt_nof_bits,
          &exp_man
        );
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  180,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_GENERAL_REJECT_CLEAR_CONFIGURATION_D_BUFFS_0r + indx, REG_PORT_ANY,  0, RJCT_CLR_FR_DB_FLMC_TH_0f + indx,  exp_man));

      res = soc_sand_break_to_mnt_exp_round_up(
              info->full_mc[indx].set,
              fmc_th_mnt_nof_bits,
              fmc_th_exp_nof_bits,
              0,
              &mnt_val,
              &exp_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 190, exit);

      exact_info->full_mc[indx].set = (mnt_val) * (1<<(exp_val));

      arad_iqm_mantissa_exponent_set(
          unit,
          mnt_val,
          exp_val,
          fmc_th_mnt_nof_bits,
          &exp_man
        );
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  200,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_GENERAL_REJECT_SET_CONFIGURATION_D_BUFFS_0r + indx, REG_PORT_ANY,  0, RJCT_SET_FR_DB_FLMC_TH_0f + indx,  exp_man));
  }

#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit)) {
      /* 
        info->ocb_uc[index].set 
        info->ocb_uc[index].clear
        setting ocb unicast thresholds registers 
       */
      for (indx = 0 ; indx < ARAD_NOF_DROP_PRECEDENCE ; indx++) {
          fld_val = info->ocb_uc[indx].set;
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  450,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_RJCT_UC_OCB_TH_0r + indx, REG_PORT_ANY, 0, RJCT_SET_FR_UC_OCB_TH_0f + indx,  fld_val));
          fld_val = info->ocb_uc[indx].clear;
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  460,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_RJCT_UC_OCB_TH_0r + indx, REG_PORT_ANY, 0, RJCT_CLR_FR_UC_OCB_TH_0f + indx,  fld_val));
      }

      /* 
        info->ocb_mc[index].set 
        info->ocb_mc[index].clear
        setting ocb multicast thresholds registers
       */
      for (indx = 0 ; indx < ARAD_NOF_DROP_PRECEDENCE ; indx++) {
          fld_val = info->ocb_mc[indx].set;
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  470,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_RJCT_MC_OCB_TH_0r + indx, REG_PORT_ANY, 0, RJCT_SET_FR_MC_OCB_TH_0f + indx,  fld_val));
          fld_val = info->ocb_mc[indx].clear;
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  480,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_RJCT_MC_OCB_TH_0r + indx, REG_PORT_ANY, 0, RJCT_CLR_FR_MC_OCB_TH_0f + indx,  fld_val));
      }
  }
#endif

  /*
   *    Extension for Arad-B
   */
    res = arad_b_itm_glob_rcs_drop_set_unsafe(
            unit,
            info->mem_excess,
            exact_info->mem_excess
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 210, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_glob_rcs_drop_set_unsafe()",0,0);
}

/*********************************************************************
*     Sets the Thresholds to trigger/clear the Drop mechanism,
*     in which packets are dropped if the buffers of the
*     different kinds have passed their hysteresis thresholds.
*     For the different kinds of general resources (bds,
*     unicast, full-multicast, mini-multicast).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_glob_rcs_drop_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_GLOB_RCS_DROP_TH *info
  )
{
  uint32
    res = SOC_SAND_OK,
    index;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_GLOB_RCS_DROP_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  /* BDBs */

  for (index = 0 ; index < ARAD_NOF_DROP_PRECEDENCE; index++)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->bdbs[index].clear, ARAD_ITM_GLOB_RCS_DROP_BDBS_SIZE_MAX,
      ARAD_ITM_GLOB_RCS_DROP_SIZE_OUT_OF_RANGE_ERR, 10, exit
      );

    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->bdbs[index].set, ARAD_ITM_GLOB_RCS_DROP_BDBS_SIZE_MAX,
      ARAD_ITM_GLOB_RCS_DROP_SIZE_OUT_OF_RANGE_ERR, 20, exit
      );
  }

  /* BDs */

  for (index = 0 ; index < ARAD_NOF_DROP_PRECEDENCE; index++)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->bdbs[index].clear, ARAD_ITM_GLOB_RCS_DROP_BDS_SIZE_MAX,
      ARAD_ITM_GLOB_RCS_DROP_SIZE_OUT_OF_RANGE_ERR, 10, exit
      );

    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->bdbs[index].set, ARAD_ITM_GLOB_RCS_DROP_BDS_SIZE_MAX,
      ARAD_ITM_GLOB_RCS_DROP_SIZE_OUT_OF_RANGE_ERR, 20, exit
      );
  }

  /* Unicast */

  for (index = 0 ; index < ARAD_NOF_DROP_PRECEDENCE; index++)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->unicast[index].clear, ARAD_ITM_GLOB_RCS_DROP_UC_SIZE_MAX,
      ARAD_ITM_GLOB_RCS_DROP_SIZE_OUT_OF_RANGE_ERR, 30, exit
      );

    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->unicast[index].set, ARAD_ITM_GLOB_RCS_DROP_UC_SIZE_MAX,
      ARAD_ITM_GLOB_RCS_DROP_SIZE_OUT_OF_RANGE_ERR, 40, exit
      );
  }

  /* Mini Multicast */

  for (index = 0 ; index < ARAD_NOF_DROP_PRECEDENCE; index++)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->mini_mc[index].clear, ARAD_ITM_GLOB_RCS_DROP_MINI_MC_SIZE_MAX,
      ARAD_ITM_GLOB_RCS_DROP_SIZE_OUT_OF_RANGE_ERR, 50, exit
      );

    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->mini_mc[index].set, ARAD_ITM_GLOB_RCS_DROP_MINI_MC_SIZE_MAX,
      ARAD_ITM_GLOB_RCS_DROP_SIZE_OUT_OF_RANGE_ERR, 60, exit
      );
  }

  /* Full Multicast */

  for (index = 0 ; index < ARAD_NOF_DROP_PRECEDENCE; index++)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->full_mc[index].clear, ARAD_ITM_GLOB_RCS_DROP_FMC_SIZE_MAX,
      ARAD_ITM_GLOB_RCS_DROP_SIZE_OUT_OF_RANGE_ERR, 70, exit
      );

    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->full_mc[index].set, ARAD_ITM_GLOB_RCS_DROP_FMC_SIZE_MAX,
      ARAD_ITM_GLOB_RCS_DROP_SIZE_OUT_OF_RANGE_ERR, 80, exit
      );

  }

  /* OCB */

  for (index = 0 ; index < ARAD_NOF_DROP_PRECEDENCE; index++)
  {
    /* OCB unicast */
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->ocb_uc[index].clear, ARAD_ITM_GLOB_RCS_DROP_OCB_SIZE_MAX,
      ARAD_ITM_GLOB_RCS_DROP_SIZE_OUT_OF_RANGE_ERR, 84, exit
      );

    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->ocb_uc[index].set, ARAD_ITM_GLOB_RCS_DROP_OCB_SIZE_MAX,
      ARAD_ITM_GLOB_RCS_DROP_SIZE_OUT_OF_RANGE_ERR, 85, exit
      );

    /* OCB multicast */
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->ocb_mc[index].clear, 0x7FFF,
      ARAD_ITM_GLOB_RCS_DROP_SIZE_OUT_OF_RANGE_ERR, 84, exit
      );

    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->ocb_mc[index].set, 0x7FFF,
      ARAD_ITM_GLOB_RCS_DROP_SIZE_OUT_OF_RANGE_ERR, 85, exit
      );
  }

  /*
   *    Extension 
   */
    res = arad_b_itm_glob_rcs_drop_verify(
            unit,
            info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_glob_rcs_drop_verify()",0,0);
}

/*********************************************************************
*     Sets the Thresholds to trigger/clear the Drop mechanism,
*     in which packets are dropped if the buffers of the
*     different kinds have passed their hysteresis thresholds.
*     For the different kinds of general resources (bds,
*     unicast, full-multicast, mini-multicast).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_glob_rcs_drop_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ITM_GLOB_RCS_DROP_TH *info
  )
{
  uint32
    res,
      exp_man = 0,
      bdbs_th_mnt_nof_bits,
      bds_th_mnt_nof_bits,
      uc_th_mnt_nof_bits,
      mini_mc_mnt_nof_bits,
      fmc_th_mnt_nof_bits,
    mnt_val,
    exp_val,
    fld_val;
  uint32
    indx;
   
    

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_GLOB_RCS_DROP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  

  /*
   * BDB-s (Buffer Descriptor Buffers)
   */

  bdbs_th_mnt_nof_bits = 8;
  for (indx = 0 ; indx < ARAD_NOF_DROP_PRECEDENCE ; indx++)
  {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  110,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_GENERAL_REJECT_CONFIGURATION_BD_BS_0r + indx, REG_PORT_ANY,  0, RJCT_CLR_FR_BDB_TH_0f + indx, &exp_man));

      arad_iqm_mantissa_exponent_get(
          unit,
          exp_man,
          bdbs_th_mnt_nof_bits,
          &mnt_val,
          &exp_val
      );

     info->bdbs[indx].clear = (mnt_val) * (1<<(exp_val));

     SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  130,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_GENERAL_REJECT_CONFIGURATION_BD_BS_0r + indx, REG_PORT_ANY,  0, RJCT_SET_FR_BDB_TH_0f + indx, &exp_man));

     arad_iqm_mantissa_exponent_get(
         unit,
         exp_man,
         bdbs_th_mnt_nof_bits,
         &mnt_val,
         &exp_val
     );
     info->bdbs[indx].set = (mnt_val) * (1<<(exp_val));
  }

  /*
   * BD-s (Buffer Descriptors)
   */

  bds_th_mnt_nof_bits = 12;
  for (indx = 0 ; indx < ARAD_NOF_DROP_PRECEDENCE ; indx++)
  {
     SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  150,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_GENERAL_REJECT_CONFIGURATION_B_DS_0r + indx, REG_PORT_ANY,  0, RJCT_CLR_OC_BD_TH_0f + indx, &exp_man));

     arad_iqm_mantissa_exponent_get(
         unit,
         exp_man,
         bds_th_mnt_nof_bits,
         &mnt_val,
         &exp_val
     );
     info->bds[indx].clear = (mnt_val) * (1<<(exp_val));

     SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  170,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_GENERAL_REJECT_CONFIGURATION_B_DS_0r + indx, REG_PORT_ANY,  0, RJCT_SET_OC_BD_TH_0f + indx, &exp_man));

     arad_iqm_mantissa_exponent_get(
         unit,
         exp_man,
         bds_th_mnt_nof_bits,
         &mnt_val,
         &exp_val
     );
     info->bds[indx].set = (mnt_val) * (1<<(exp_val));
  }

  /*
    info->unicast[index].clear
    info->unicast[index].set
  */

  uc_th_mnt_nof_bits = 7;
  for (indx = 0 ; indx < ARAD_NOF_DROP_PRECEDENCE ; indx++)
  {
     SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  210,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_GENERAL_REJECT_CLEAR_CONFIGURATION_D_BUFFS_0r + indx, REG_PORT_ANY,  0, RJCT_CLR_FR_DB_UC_TH_0f + indx, &exp_man));

     arad_iqm_mantissa_exponent_get(
         unit,
         exp_man,
         uc_th_mnt_nof_bits,
         &mnt_val,
         &exp_val
     );
     info->unicast[indx].clear = (mnt_val) * (1<<(exp_val));

     SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  230,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_GENERAL_REJECT_SET_CONFIGURATION_D_BUFFS_0r + indx, REG_PORT_ANY,  0, RJCT_SET_FR_DB_UC_TH_0f + indx, &exp_man));

     arad_iqm_mantissa_exponent_get(
         unit,
         exp_man,
         uc_th_mnt_nof_bits,
         &mnt_val,
         &exp_val
     );
     info->unicast[indx].set = (mnt_val) * (1<<(exp_val));
  }

  /*
    info->mini_mc[index].clear
    info->mini_mc[index].set
  */

  mini_mc_mnt_nof_bits = 6;
  for (indx = 0 ; indx < ARAD_NOF_DROP_PRECEDENCE ; indx++)
  {
     SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  310,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_GENERAL_REJECT_CLEAR_CONFIGURATION_D_BUFFS_0r + indx, REG_PORT_ANY,  0, RJCT_CLR_FR_DB_MNMC_TH_0f + indx, &exp_man));

     arad_iqm_mantissa_exponent_get(
         unit,
         exp_man,
         mini_mc_mnt_nof_bits,
         &mnt_val,
         &exp_val
     );
     info->mini_mc[indx].clear = (mnt_val) * (1<<(exp_val));

     SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  340,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_GENERAL_REJECT_SET_CONFIGURATION_D_BUFFS_0r + indx, REG_PORT_ANY,  0, RJCT_SET_FR_DB_MNMC_TH_0f + indx, &exp_man));

     arad_iqm_mantissa_exponent_get(
         unit,
         exp_man,
         mini_mc_mnt_nof_bits,
         &mnt_val,
         &exp_val
     );
     info->mini_mc[indx].set = (mnt_val) * (1<<(exp_val));
  }

  /*
    info->full_mc[index].clear
    info->full_mc[index].set
   */

  fmc_th_mnt_nof_bits = 7;
  for (indx = 0 ; indx < ARAD_NOF_DROP_PRECEDENCE ; indx++)
  {
     SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  410,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_GENERAL_REJECT_CLEAR_CONFIGURATION_D_BUFFS_0r + indx, REG_PORT_ANY,  0, RJCT_CLR_FR_DB_FLMC_TH_0f + indx, &exp_man));

     arad_iqm_mantissa_exponent_get(
         unit,
         exp_man,
         fmc_th_mnt_nof_bits,
         &mnt_val,
         &exp_val
     );
     info->full_mc[indx].clear = (mnt_val) * (1<<(exp_val));

     SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  440,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_GENERAL_REJECT_SET_CONFIGURATION_D_BUFFS_0r + indx, REG_PORT_ANY,  0, RJCT_SET_FR_DB_FLMC_TH_0f + indx, &exp_man));

     arad_iqm_mantissa_exponent_get(
         unit,
         exp_man,
         fmc_th_mnt_nof_bits,
         &mnt_val,
         &exp_val
     );
     info->full_mc[indx].set = (mnt_val) * (1<<(exp_val));
  }

#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit)) {
      /* 
        info->ocb_uc[index].set 
        info->ocb_uc[index].clear
        getting ocb unicast thresholds registers
       */
      for (indx = 0 ; indx < ARAD_NOF_DROP_PRECEDENCE ; indx++) {
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  450,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_RJCT_UC_OCB_TH_0r + indx, REG_PORT_ANY, 0, RJCT_SET_FR_UC_OCB_TH_0f + indx, &fld_val));
          info->ocb_uc[indx].set = fld_val;
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  460,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_RJCT_UC_OCB_TH_0r + indx, REG_PORT_ANY, 0, RJCT_CLR_FR_UC_OCB_TH_0f + indx, &fld_val));
          info->ocb_uc[indx].clear= fld_val;
      }

      /* 
        info->ocb_mc[index].set 
        info->ocb_mc[index].clear
        getting ocb multicast thresholds registers
       */
      for (indx = 0 ; indx < ARAD_NOF_DROP_PRECEDENCE ; indx++) {
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  470,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_RJCT_MC_OCB_TH_0r + indx, REG_PORT_ANY, 0, RJCT_SET_FR_MC_OCB_TH_0f + indx, &fld_val));
          info->ocb_mc[indx].set = fld_val;
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  480,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_RJCT_MC_OCB_TH_0r + indx, REG_PORT_ANY, 0, RJCT_CLR_FR_MC_OCB_TH_0f + indx, &fld_val));
          info->ocb_mc[indx].clear= fld_val;
      }
  }
#endif

  /*
   *    Extension 
   */
    res = arad_b_itm_glob_rcs_drop_get_unsafe(
            unit,
            info->mem_excess
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 490, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_glob_rcs_drop_get_unsafe()",0,0);
}



/*********************************************************************
*     Defines packet queues categories - in contiguous blocks.
*     IQM queues are divided to 4 categories in contiguous
*     blocks. Category-4 from 'category-end-3' till the last
*     queue (32K).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_category_rngs_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_CATEGORY_RNGS *info
  )
{
  uint32 res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_CATEGORY_RNGS_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  0, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_PACKET_QUEUES_CATEGORIES_0r, REG_PORT_ANY, 0, TOP_PKT_Q_CAT_0f,  info->vsq_ctgry0_end));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_PACKET_QUEUES_CATEGORIES_1r, REG_PORT_ANY, 0, TOP_PKT_Q_CAT_1f,  info->vsq_ctgry1_end));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  2, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_PACKET_QUEUES_CATEGORIES_2r, REG_PORT_ANY, 0, TOP_PKT_Q_CAT_2f,  info->vsq_ctgry2_end));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_category_rngs_set_unsafe()",0,0);
}

/*********************************************************************
*     Defines packet queues categories - in contiguous blocks.
*     IQM queues are divided to 4 categories in contiguous
*     blocks. Category-4 from 'category-end-3' till the last
*     queue (32K).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_category_rngs_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_CATEGORY_RNGS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_CATEGORY_RNGS_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  if ((info->vsq_ctgry0_end > info->vsq_ctgry1_end) ||
      (info->vsq_ctgry1_end > info->vsq_ctgry2_end) ||
      (info->vsq_ctgry0_end > info->vsq_ctgry2_end))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_VSQ_CATEGORY_END_OUT_OF_ORDER_ERR, 10, exit);
  }
  /* ARAD_ITM_CATEGORY_INFO_VSQ_CTGRY_END_MIN may be changed and be grater than 0*/
  /* coverity[unsigned_compare : FALSE] */
  if ((info->vsq_ctgry0_end < ARAD_ITM_CATEGORY_INFO_VSQ_CTGRY_END_MIN) ||
    (info->vsq_ctgry0_end > ARAD_ITM_CATEGORY_INFO_VSQ_CTGRY_END_MAX(unit)))
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_VSQ_CATEGORY_END_OUT_OF_RANGE_ERR, 20, exit);

  /* ARAD_ITM_CATEGORY_INFO_VSQ_CTGRY_END_MIN may be changed and be grater than 0*/
  /* coverity[unsigned_compare : FALSE] */
  if ((info->vsq_ctgry1_end < ARAD_ITM_CATEGORY_INFO_VSQ_CTGRY_END_MIN) ||
    (info->vsq_ctgry1_end > ARAD_ITM_CATEGORY_INFO_VSQ_CTGRY_END_MAX(unit)))
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_VSQ_CATEGORY_END_OUT_OF_RANGE_ERR, 30, exit);

  /* ARAD_ITM_CATEGORY_INFO_VSQ_CTGRY_END_MIN may be changed and be grater than 0*/
  /* coverity[unsigned_compare : FALSE] */
  if ((info->vsq_ctgry2_end < ARAD_ITM_CATEGORY_INFO_VSQ_CTGRY_END_MIN) ||
    (info->vsq_ctgry2_end > ARAD_ITM_CATEGORY_INFO_VSQ_CTGRY_END_MAX(unit)))
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_VSQ_CATEGORY_END_OUT_OF_RANGE_ERR, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_category_rngs_verify()",0,0);
}

/*********************************************************************
*     Defines packet queues categories - in contiguous blocks.
*     IQM queues are divided to 4 categories in contiguous
*     blocks. Category-4 from 'category-end-3' till the last
*     queue (32K).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_category_rngs_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ITM_CATEGORY_RNGS *info
  )
{
  uint32 res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_CATEGORY_RNGS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_PACKET_QUEUES_CATEGORIES_0r, REG_PORT_ANY, 0, TOP_PKT_Q_CAT_0f, &info->vsq_ctgry0_end));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_PACKET_QUEUES_CATEGORIES_1r, REG_PORT_ANY, 0, TOP_PKT_Q_CAT_1f, &info->vsq_ctgry1_end));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_PACKET_QUEUES_CATEGORIES_2r, REG_PORT_ANY, 0, TOP_PKT_Q_CAT_2f, &info->vsq_ctgry2_end));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_category_rngs_get_unsafe()",0,0);
}

/*********************************************************************
*     In order to admit a packet to a queue, the packet must
*     pass the admit-test-logic. The packet belogs to some VSQs
*     out of the 4 types of VSQs. For each VSQ which the packet
*     belongs to it encounters WRED and Tail-Drop mechanisms.
*     The admit-test-template determines which, if at all, of
*     the VSQ groups and their reject mechanisms must the packet
*     consider. A test template consists of two optional combinations
*     of VSQ groups to consider (testA, testB).
*     Each queue (VOQ) is assigned with a test template.
*     Notice that in a queue, is a packet is chosen to be rejected
*     normally, the admit test logic will not affect it.
*     From the Data Sheet:
*     The Packet Queue Rate Class is used to select one of four
*     Admission Logic Templates. Each template is an 8-bit variable
*     {a1,b1,c1,d1,a2,b2,c2,d2} applied as detailed below:
*
*     Final-Admit =
*       GL-Admit & PQ-Admit &
*       ((a1 | CT-Admit) & (b1 | CTTC-Admit) &
*             (c1 | CTCC-Admit) & (d1 |STF-Admit ) OR
*         (a2 | CT-Admit) & (b2 | CTTC-Admit)  &
*             (c2 | CTCC-Admit) & (d2 |STF-Admit)) &
*       (!PQ-Sys-Red-Ena | SR-Admit)
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_admit_test_tmplt_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 admt_tst_ndx,
    SOC_SAND_IN  ARAD_ITM_ADMIT_TEST_TMPLT_INFO *info
  )
{
  uint32
    res,
    test_a_in_sand_u32,
    test_b_in_sand_u32;
  uint32
    test_a_index,
    test_b_index;
   
    

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_ADMIT_TEST_TMPLT_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  

  res = arad_itm_convert_admit_one_test_tmplt_to_u32(
          info->test_a,
          &test_a_in_sand_u32
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,0,exit);

  test_a_index = admt_tst_ndx;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_REJECT_ADMISSION_Ar, SOC_CORE_ALL, 0, RJCT_TMPLTA_SET_0f + test_a_index,  test_a_in_sand_u32));

  res =
    arad_itm_convert_admit_one_test_tmplt_to_u32(
      info->test_b,
      &test_b_in_sand_u32
    );
  SOC_SAND_CHECK_FUNC_RESULT(res,2,exit);

  test_b_index = admt_tst_ndx;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  3, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_REJECT_ADMISSION_Br, REG_PORT_ANY, 0, RJCT_TMPLTB_SET_0f + test_b_index,  test_b_in_sand_u32));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_admit_test_tmplt_set_unsafe()",0,0);
}

/*********************************************************************
*     In order to admit a packet to a queue, the packet must
*     pass the admit-test-logic. The packet belogs to some VSQs
*     out of the 4 types of VSQs. For each VSQ which the packet
*     belongs to it encounters WRED and Tail-Drop mechanisms.
*     The admit-test-template determines which, if at all, of
*     the VSQ groups and their reject mechanisms must the packet
*     consider. A test template consists of two optional combinations
*     of VSQ groups to consider (testA, testB).
*     Each queue (VOQ) is assigned with a test template.
*     Notice that in a queue, is a packet is chosen to be rejected
*     normally, the admit test logic will not affect it.
*     From the Data Sheet:
*     The Packet Queue Rate Class is used to select one of four
*     Admission Logic Templates. Each template is an 8-bit variable
*     {a1,b1,c1,d1,a2,b2,c2,d2} applied as detailed below:
*
*     Final-Admit =
*       GL-Admit & PQ-Admit &
*       ((a1 | CT-Admit) & (b1 | CTTC-Admit) &
*             (c1 | CTCC-Admit) & (d1 |STF-Admit ) OR
*         (a2 | CT-Admit) & (b2 | CTTC-Admit)  &
*             (c2 | CTCC-Admit) & (d2 |STF-Admit)) &
*       (!PQ-Sys-Red-Ena | SR-Admit)
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_admit_test_tmplt_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 admt_tst_ndx,
    SOC_SAND_IN  ARAD_ITM_ADMIT_TEST_TMPLT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_ADMIT_TEST_TMPLT_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  if (admt_tst_ndx > (ARAD_ITM_ADMIT_TSTS_LAST-1))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_ADMT_TEST_NDX_OUT_OF_RANGE_ERR, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_admit_test_tmplt_verify()",0,0);
}

/*********************************************************************
*     In order to admit a packet to a queue, the packet must
*     pass the admit-test-logic. The packet belogs to some VSQs
*     out of the 4 types of VSQs. For each VSQ which the packet
*     belongs to it encounters WRED and Tail-Drop mechanisms.
*     The admit-test-template determines which, if at all, of
*     the VSQ groups and their reject mechanisms must the packet
*     consider. A test template consists of two optional combinations
*     of VSQ groups to consider (testA, testB).
*     Each queue (VOQ) is assigned with a test template.
*     Notice that in a queue, is a packet is chosen to be rejected
*     normally, the admit test logic will not affect it.
*     From the Data Sheet:
*     The Packet Queue Rate Class is used to select one of four
*     Admission Logic Templates. Each template is an 8-bit variable
*     {a1,b1,c1,d1,a2,b2,c2,d2} applied as detailed below:
*
*     Final-Admit =
*       GL-Admit & PQ-Admit &
*       ((a1 | CT-Admit) & (b1 | CTTC-Admit) &
*             (c1 | CTCC-Admit) & (d1 |STF-Admit ) OR
*         (a2 | CT-Admit) & (b2 | CTTC-Admit)  &
*             (c2 | CTCC-Admit) & (d2 |STF-Admit)) &
*       (!PQ-Sys-Red-Ena | SR-Admit)
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_admit_test_tmplt_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 admt_tst_ndx,
    SOC_SAND_OUT ARAD_ITM_ADMIT_TEST_TMPLT_INFO *info
  )
{
  uint32
    res,
    test_a_in_sand_u32,
    test_b_in_sand_u32;
  uint32
    test_a_index,
    test_b_index;
   
    

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_ADMIT_TEST_TMPLT_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  

  test_a_index = admt_tst_ndx;
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_REJECT_ADMISSION_Ar, SOC_CORE_ALL, 0, RJCT_TMPLTA_SET_0f + test_a_index, &test_a_in_sand_u32));

  res =
    arad_itm_convert_u32_to_admit_one_test_tmplt(
      test_a_in_sand_u32,
      &(info->test_a)
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  test_b_index = admt_tst_ndx;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_REJECT_ADMISSION_Br, REG_PORT_ANY, 0, RJCT_TMPLTB_SET_0f + test_b_index, &test_b_in_sand_u32));
    res =
      arad_itm_convert_u32_to_admit_one_test_tmplt(
        test_b_in_sand_u32,
        &(info->test_b)
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_admit_test_tmplt_get_unsafe()",0,0);
}

CONST uint32 mode2scan_time[] = ARAD_MIN_SCAN_CYCLE_PERIOD_MICRO_PER_MODE;
/*
 * return credit watchdog scan time in nano seconds and the field value to set it in hardware
 * If the scan time was not yet set, or a scan time was given, or a filed pointer was given, also set the scan time
 */
STATIC uint32
  arad_itm_get_set_cr_watch_dog_scan_calc_unsafe(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 scan_time_in_micro, /* if 0, the default is used */
    SOC_SAND_OUT uint32 *out_scan_time,
    SOC_SAND_OUT uint32 *out_field_value
  )
{
  uint32 res, tmp=0;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (scan_time_in_micro || out_field_value ||
      !SOC_DPP_CONFIG(unit)->arad->exact_credit_watchdog_scan_time_nano) { /* if the field value needs to be calculated */
    /* set time in microseconds before rounding to hardware capabilities */
    uint32 scan_ms = scan_time_in_micro ? scan_time_in_micro : mode2scan_time[GET_CREDIT_WATCHDOG_MODE(unit)];

    res = arad_chip_time_to_ticks( unit, scan_ms, FALSE,
      ARAD_ITM_WD_INFO_MIN_SCAN_CYCLE_PERIOD_MICRO_GRANULARITY, TRUE, &tmp); /* calculate field value */
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    if (out_field_value) {
      *out_field_value = tmp;
    }

    res = arad_ticks_to_time(unit, tmp, TRUE, ARAD_ITM_WD_INFO_MIN_SCAN_CYCLE_PERIOD_MICRO_GRANULARITY,
      &tmp); /* calculate exact value from field */
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    if (IS_CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE(unit)) { /* handle the common FSM mode */
    }
    SOC_DPP_CONFIG(unit)->arad->exact_credit_watchdog_scan_time_nano =
      IS_CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE(unit) ? scan_ms * 1000: tmp;
    if (out_scan_time) {
      *out_scan_time = tmp;
    }
  } else if (out_scan_time) {
    *out_scan_time = SOC_DPP_CONFIG(unit)->arad->exact_credit_watchdog_scan_time_nano;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_get_set_cr_watch_dog_scan_calc_unsafe()", SOC_DPP_CONFIG(unit)->arad->exact_credit_watchdog_scan_time_nano, tmp);
}


/* converts resolution value (1,2,4) to an exponent (0-2) */
static uint8 _credit_balance_resolution_to_exp[5] = {-1, 0, 1, -1, 2};

/* Get the minimal credit balance resolution to support the provided value with the provided maximum hardware field value */
#define ARAD_ITM_MAX_CRBL_RESOLUTION_EXPONENT 2
uint32
  arad_itm_cr_request_call_needed_credit_balance_resolution(
    SOC_SAND_IN  uint32  value,       /* (max absolute) value that the user wanted to set */
    SOC_SAND_IN  uint32  max_allowed, /* maximum value supported by the hardware field (corresponds to value with resolutoin 1 (resolution exponent 0) */
    SOC_SAND_OUT uint8*  credit_balance_resolution /* output credit balance resolution */
  )
{
  uint32 resolution_exp, current_max_allowed = max_allowed; 
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(credit_balance_resolution);

  for (resolution_exp = 0; resolution_exp <= ARAD_ITM_MAX_CRBL_RESOLUTION_EXPONENT; ++resolution_exp) {
    if (value <= current_max_allowed) {
      break;
    }
    current_max_allowed *= 2;
    if (current_max_allowed <= max_allowed) {
      SOC_SAND_SET_ERROR_CODE(ARAD_INTERNAL_ASSERT_ERR, 100, exit); /* internal error - overflow of value */
    }
  }
  *credit_balance_resolution = 1 << resolution_exp;
  if (resolution_exp > ARAD_ITM_MAX_CRBL_RESOLUTION_EXPONENT) {
      SOC_SAND_SET_ERROR_CODE(ARAD_INTERNAL_ASSERT_ERR, 220, exit); /* value too big for hardware field with allowed resolution */
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "arad_itm_cr_request_call_needed_credit_balance_resolution()", value, max_allowed);
}

/*********************************************************************
*     Per queue the device maintains an Off/Normal/Slow Credit
*     Request State. The device has 16 'Credit Request
*     Configurations', one per Credit-Class. Sets the (1)
*     Queue-Size-Thresholds (2) Credit-Balance-Thresholds (3)
*     Empty-Queue-Thresholds (4) Credit-Watchdog
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_cr_request_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_QT_NDX          qt_ndx,
    SOC_SAND_IN  ARAD_ITM_CR_REQUEST_INFO *info,
    SOC_SAND_OUT ARAD_ITM_CR_REQUEST_INFO *exact_info
  )
{
  uint32
    mul_pckt_deq_fld_val,
    cycle_time = 0,
    tmp;
  uint8
    hw_q_type = ARAD_SW_DB_QUEUE_TYPE_NOT_AVAILABLE, cr_bal_resolution, tmp_resolution;
  int32
    int32_min, int32_max;
  uint32
    res, uint32_max;
  ARAD_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_DATA
    ips_queue_size_based_thresholds_table_tbl_data;
  ARAD_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_DATA
    ips_credit_balance_based_thresholds_table_tbl_data;
  ARAD_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_DATA
    ips_empty_queue_credit_balance_table_tbl_data;
  ARAD_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_DATA
    ips_credit_watchdog_thresholds_table_tbl_data;
   soc_reg_t
       resolution_reg;
    

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_CR_REQUEST_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);


  /* We will now calculate the minimum credit balance resolution value that supports all the input thresholds affected by it */

  /* calculate the maximum values of the hungry thresholds */
  int32_min = int32_max = info->hungry_th.off_to_slow_th;
  if (int32_max < info->hungry_th.off_to_normal_th) {
    int32_max = info->hungry_th.off_to_normal_th;
  } else if (int32_min > info->hungry_th.off_to_normal_th) {
    int32_min = info->hungry_th.off_to_normal_th;
  }

  if (int32_max < info->hungry_th.slow_to_normal_th) {
    int32_max = info->hungry_th.slow_to_normal_th;
  } else if (int32_min > info->hungry_th.slow_to_normal_th) {
    int32_min = info->hungry_th.slow_to_normal_th;
  }

  if (int32_max < info->hungry_th.normal_to_slow_th) {
    int32_max = info->hungry_th.normal_to_slow_th;
  } else if (int32_min > info->hungry_th.normal_to_slow_th) {
    int32_min = info->hungry_th.normal_to_slow_th;
  }

  if (int32_min < 0) {
    int32_min = -int32_min;
    if (int32_max < int32_min) {
      int32_max = int32_min;
    }
  } /* now int32_max contains the maximum absolute value of all the hungry thresholds */

  SOC_SAND_CHECK_FUNC_RESULT(arad_itm_cr_request_call_needed_credit_balance_resolution(
    int32_max, 7 * (1 << 12), &cr_bal_resolution), 20, exit); /* calculate resolution needed for hungry thresholds, max field value must be under 32K */

  /* calculate the maximum backoff/backlog threshold and needed resolution */
  uint32_max = info->satisfied_th.backoff_th.backoff_enter_th;
  if (uint32_max < info->satisfied_th.backoff_th.backoff_exit_th) {
    uint32_max = info->satisfied_th.backoff_th.backoff_exit_th;
  }
  if (uint32_max < info->satisfied_th.backlog_th.backlog_enter_th) {
    uint32_max = info->satisfied_th.backlog_th.backlog_enter_th;
  }
  if (uint32_max < info->satisfied_th.backlog_th.backlog_exit_th) {
    uint32_max = info->satisfied_th.backlog_th.backlog_exit_th;
  } /* now uint32_max contains the maximum of all the backoff/backlog thresholds */

  SOC_SAND_CHECK_FUNC_RESULT(arad_itm_cr_request_call_needed_credit_balance_resolution(
    uint32_max, 15 * (1 << 11), &tmp_resolution), 30, exit); /* max field value must be under 32K */
  if (cr_bal_resolution < tmp_resolution) {
    cr_bal_resolution = tmp_resolution;
  }

  /* calculate the maximum values of the satisfied empty queue thresholds and needed resolution */
  int32_min = int32_max = info->satisfied_th.empty_queues.satisfied_empty_q_th;
  if (int32_max < info->satisfied_th.empty_queues.max_credit_balance_empty_q) {
    int32_max = info->satisfied_th.empty_queues.max_credit_balance_empty_q;
  } else if (int32_min > info->satisfied_th.empty_queues.max_credit_balance_empty_q) {
    int32_min = info->satisfied_th.empty_queues.max_credit_balance_empty_q;
  }
  if (int32_min < 0) {
    SOC_SAND_CHECK_FUNC_RESULT(arad_itm_cr_request_call_needed_credit_balance_resolution(
      -int32_min, (1 << 15) - 1, &tmp_resolution), 40, exit); /* max field value must be under 32K */
    if (cr_bal_resolution < tmp_resolution) {
      cr_bal_resolution = tmp_resolution;
    }
  }
  if (int32_max > 0) {
    SOC_SAND_CHECK_FUNC_RESULT(arad_itm_cr_request_call_needed_credit_balance_resolution(
      int32_max, (1 << 15) - 1, &tmp_resolution), 50, exit);
    if (cr_bal_resolution < tmp_resolution) {
      cr_bal_resolution = tmp_resolution;
    }
  } /* We finished calculating the needed credit balance resolution, which is contained in cr_bal_resolution */

  /* set hungry thresholds data */
  res = arad_itm_cr_request_info_hungry_table_field_set(
          info->hungry_th.off_to_normal_th,
          &(ips_queue_size_based_thresholds_table_tbl_data.off_to_norm_msg_th),
          &(exact_info->hungry_th.off_to_normal_th), cr_bal_resolution
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  res = arad_itm_cr_request_info_hungry_table_field_set(
          info->hungry_th.off_to_slow_th,
          &(ips_queue_size_based_thresholds_table_tbl_data.off_to_slow_msg_th),
          &(exact_info->hungry_th.off_to_slow_th), cr_bal_resolution
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

  res = arad_itm_cr_request_info_hungry_table_field_set(
          info->hungry_th.slow_to_normal_th,
          &(ips_queue_size_based_thresholds_table_tbl_data.slow_to_norm_msg_th),
          &(exact_info->hungry_th.slow_to_normal_th), cr_bal_resolution
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);

  res = arad_itm_cr_request_info_hungry_table_field_set(
          info->hungry_th.normal_to_slow_th,
          &(ips_queue_size_based_thresholds_table_tbl_data.norm_to_slow_msg_th),
          &(exact_info->hungry_th.normal_to_slow_th), cr_bal_resolution
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);

  ips_queue_size_based_thresholds_table_tbl_data.fsm_th_mul =
    ARAD_ITM_HUNGRY_TH_MULTIPLIER_VAL_TO_FIELD(info->hungry_th.multiplier);

  /* set satisfied backoff/backlog thresholds data */
  res = arad_itm_cr_request_info_satisfied_mnt_exp_table_field_set(
          info->satisfied_th.backlog_th.backlog_enter_th,
          &(ips_credit_balance_based_thresholds_table_tbl_data.backlog_enter_qcr_bal_th),
          &(exact_info->satisfied_th.backlog_th.backlog_enter_th), cr_bal_resolution
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);

  res = arad_itm_cr_request_info_satisfied_mnt_exp_table_field_set(
          info->satisfied_th.backlog_th.backlog_exit_th,
          &(ips_credit_balance_based_thresholds_table_tbl_data.backlog_exit_qcr_bal_th),
          &(exact_info->satisfied_th.backlog_th.backlog_exit_th), cr_bal_resolution
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 210, exit);

  res = arad_itm_cr_request_info_satisfied_mnt_exp_table_field_set(
          info->satisfied_th.backoff_th.backoff_enter_th,
          &(ips_credit_balance_based_thresholds_table_tbl_data.backoff_enter_qcr_bal_th),
          &(exact_info->satisfied_th.backoff_th.backoff_enter_th), cr_bal_resolution
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 220, exit);

  res = arad_itm_cr_request_info_satisfied_mnt_exp_table_field_set(
          info->satisfied_th.backoff_th.backoff_exit_th,
          &(ips_credit_balance_based_thresholds_table_tbl_data.backoff_exit_qcr_bal_th),
          &(exact_info->satisfied_th.backoff_th.backoff_exit_th), cr_bal_resolution
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 230, exit);

  /* set satisfied empty queue thresholds */
  if (info->satisfied_th.empty_queues.satisfied_empty_q_th < 0)
  {
    ips_empty_queue_credit_balance_table_tbl_data.empty_qsatisfied_cr_bal =
      (SOC_SAND_U16_MAX + 1) + info->satisfied_th.empty_queues.satisfied_empty_q_th / cr_bal_resolution;
  }
  else
  {
    ips_empty_queue_credit_balance_table_tbl_data.empty_qsatisfied_cr_bal =
    info->satisfied_th.empty_queues.satisfied_empty_q_th / cr_bal_resolution;
  }

  exact_info->satisfied_th.empty_queues.satisfied_empty_q_th =
    (info->satisfied_th.empty_queues.satisfied_empty_q_th / cr_bal_resolution) * cr_bal_resolution;

  if (info->satisfied_th.empty_queues.max_credit_balance_empty_q < 0)
  {
    ips_empty_queue_credit_balance_table_tbl_data.max_empty_qcr_bal =
      (SOC_SAND_U16_MAX + 1) + info->satisfied_th.empty_queues.max_credit_balance_empty_q / cr_bal_resolution;
  }
  else
  {
    ips_empty_queue_credit_balance_table_tbl_data.max_empty_qcr_bal =
    info->satisfied_th.empty_queues.max_credit_balance_empty_q / cr_bal_resolution;
  }
  exact_info->satisfied_th.empty_queues.max_credit_balance_empty_q =
    (info->satisfied_th.empty_queues.max_credit_balance_empty_q / cr_bal_resolution) * cr_bal_resolution;

  ips_empty_queue_credit_balance_table_tbl_data.exceed_max_empty_qcr_bal =
    exact_info->satisfied_th.empty_queues.exceed_max_empty_q =
    info->satisfied_th.empty_queues.exceed_max_empty_q;

  /* set Watchdog thresholds */
  if (IS_CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE(unit)) { /* handle the common FSM mode */
    if (info->wd_th.cr_wd_dlt_q_th) {
      uint32 mnt, exp;
      tmp = (info->wd_th.cr_wd_dlt_q_th + (ARAD_CREDIT_WATCHDOG_COMMON_MAX_SCAN_TIME_MS - 1)) /
        ARAD_CREDIT_WATCHDOG_COMMON_MAX_SCAN_TIME_MS; /* tmp now contains the wanted number of scans */
      SOC_SAND_CHECK_FUNC_RESULT(soc_sand_break_to_mnt_exp_round_up(tmp, 4, 4, 0, &mnt, &exp), 330, exit);
      if (exp > ARAD_CREDIT_WATCHDOG_COMMON_MAX_DELETE_EXP - ARAD_CREDIT_WATCHDOG_COMMON_SCAN_TIME_MAX_HALVES) { /* is the value above the limit that HW supports? */
        SOC_SAND_SET_ERROR_CODE(ARAD_CR_WD_DEL_TH_OUT_OF_RANGE, 250, exit);
      }
      exact_info->wd_th.cr_wd_dlt_q_th = ARAD_CREDIT_WATCHDOG_COMMON_MAX_SCAN_TIME_MS * (mnt << exp);
      exact_info->wd_th.cr_wd_stts_msg_gen = 0;
      /* Find how many bits to shift left the exponent to have scan time in the scale of the current scan time */
      switch(SOC_DPP_CONFIG(unit)->arad->exact_credit_watchdog_scan_time_nano) {
        case ARAD_CREDIT_WATCHDOG_COMMON_MIN_SCAN_TIME_NS:     /*  125000 */
          tmp = ARAD_CREDIT_WATCHDOG_COMMON_SCAN_TIME_MAX_HALVES;           /*4*/
          break;
        case 2 * ARAD_CREDIT_WATCHDOG_COMMON_MIN_SCAN_TIME_NS: /*  250000 */
          tmp = ARAD_CREDIT_WATCHDOG_COMMON_SCAN_TIME_MAX_HALVES - 1;       /*3*/
          break;
        case 4 * ARAD_CREDIT_WATCHDOG_COMMON_MIN_SCAN_TIME_NS: /*  500000 */
          tmp = ARAD_CREDIT_WATCHDOG_COMMON_SCAN_TIME_MAX_HALVES - 2;       /*2*/
          break;
        case 8 * ARAD_CREDIT_WATCHDOG_COMMON_MIN_SCAN_TIME_NS: /* 1000000 */
          tmp = ARAD_CREDIT_WATCHDOG_COMMON_SCAN_TIME_MAX_HALVES - 3;       /*1*/
          break;
        case ARAD_CREDIT_WATCHDOG_COMMON_MAX_SCAN_TIME_NS:     /* 2000000 */
          tmp = 0;
          break;
        default:
          SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 260, exit); /* internal error - scan time not supported by common mode */
      }
      ips_credit_watchdog_thresholds_table_tbl_data.wd_delete_qth = mnt;
      ips_credit_watchdog_thresholds_table_tbl_data.wd_status_msg_gen_period = exp + tmp;
    } else {
      ips_credit_watchdog_thresholds_table_tbl_data.wd_delete_qth = 
      ips_credit_watchdog_thresholds_table_tbl_data.wd_status_msg_gen_period = 0;
    }

  } else { /* handle other modes (not common FSM) */

    res = arad_itm_get_set_cr_watch_dog_scan_calc_unsafe(unit, 0, &cycle_time, 0);
    SOC_SAND_CHECK_FUNC_RESULT(res, 300, exit);

    if ((cycle_time != 0) && (info->wd_th.cr_wd_dlt_q_th != 0) && !IS_CREDIT_WATCHDOG_FAST_STATUS_MESSAGE_MODE(unit)) {
      tmp = info->wd_th.cr_wd_dlt_q_th * 1000000;
      ips_credit_watchdog_thresholds_table_tbl_data.wd_delete_qth = SOC_SAND_DIV_ROUND_UP(tmp, cycle_time);
      SOC_SAND_LIMIT_FROM_BELOW(ips_credit_watchdog_thresholds_table_tbl_data.wd_delete_qth, 1);
      SOC_SAND_LIMIT_FROM_ABOVE(ips_credit_watchdog_thresholds_table_tbl_data.wd_delete_qth, ARAD_ITM_CREDIT_WATCHDOG_MAX_THRESHOLD_VALUE);
      exact_info->wd_th.cr_wd_dlt_q_th = NS_TO_MS_ROUND(cycle_time * ips_credit_watchdog_thresholds_table_tbl_data.wd_delete_qth);
    }
    else
    {
      ips_credit_watchdog_thresholds_table_tbl_data.wd_delete_qth = 0;
      exact_info->wd_th.cr_wd_dlt_q_th = 0;
    }

    if ((cycle_time != 0) && (info->wd_th.cr_wd_stts_msg_gen != 0))
    {
      tmp = info->wd_th.cr_wd_stts_msg_gen * 1000000;
      ips_credit_watchdog_thresholds_table_tbl_data.wd_status_msg_gen_period = SOC_SAND_DIV_ROUND_UP(tmp, cycle_time);
      SOC_SAND_LIMIT_FROM_ABOVE(ips_credit_watchdog_thresholds_table_tbl_data.wd_status_msg_gen_period, ARAD_ITM_CREDIT_WATCHDOG_MAX_THRESHOLD_VALUE);
      if (ips_credit_watchdog_thresholds_table_tbl_data.wd_delete_qth > 0)
      {
        if (ips_credit_watchdog_thresholds_table_tbl_data.wd_delete_qth == ips_credit_watchdog_thresholds_table_tbl_data.wd_status_msg_gen_period &&
            ips_credit_watchdog_thresholds_table_tbl_data.wd_delete_qth > 1)
        {
          /*
           *    wd_delete_qth is at least 2
           */
          ips_credit_watchdog_thresholds_table_tbl_data.wd_status_msg_gen_period--;
        }
        else if (ips_credit_watchdog_thresholds_table_tbl_data.wd_delete_qth <= ips_credit_watchdog_thresholds_table_tbl_data.wd_status_msg_gen_period)
        {
          SOC_SAND_SET_ERROR_CODE(ARAD_CR_WD_DELETE_BEFORE_STATUS_MSG_ERR, 400, exit);
        }
      }

      exact_info->wd_th.cr_wd_stts_msg_gen = NS_TO_MS_ROUND(cycle_time * ips_credit_watchdog_thresholds_table_tbl_data.wd_status_msg_gen_period);
    }
    else
    {
      ips_credit_watchdog_thresholds_table_tbl_data.wd_status_msg_gen_period = 0;
      exact_info->wd_th.cr_wd_stts_msg_gen = 0;
    }

  }

  /* Get the hardware queue type (credit request profile) and set it */
  SOC_SAND_CHECK_FUNC_RESULT(arad_sw_db_queue_type_map_get_alloc(unit, qt_ndx, &hw_q_type), 500, exit);
  if (hw_q_type == ARAD_SW_DB_QUEUE_TYPE_NOT_AVAILABLE) { /* If dynamic queue type allocation failed */
    exact_info->wd_th.cr_wd_dlt_q_th = SOC_TMC_ITM_CR_WD_Q_TH_OPERATION_FAILED;
  } else {

    if (hw_q_type >= SOC_TMC_ITM_NOF_QT_NDXS) {
      SOC_SAND_SET_ERROR_CODE(ARAD_INTERNAL_ASSERT_ERR, 520, exit); /* internal error */
    }

    /* update credit balance resolution if needed */
    tmp_resolution = _credit_balance_resolution_to_exp[cr_bal_resolution]; /* get the resolution's exponent */
    resolution_reg = SOC_IS_ARADPLUS(unit)? IPS_REG_014Er: IPS_REG_014Br; /* Different register name in Arad+ */
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  540,  exit, ARAD_REG_ACCESS_ERR,soc_reg32_get(unit, resolution_reg, REG_PORT_ANY,  0, &tmp));
    if ((3 & (tmp >> (2 * hw_q_type))) != tmp_resolution) { /* If the current resolution is different than the required one */
      tmp &= ~(3 << (2 * hw_q_type));
      tmp |= tmp_resolution << (2 * hw_q_type);
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  550,  exit, ARAD_REG_ACCESS_ERR,soc_reg32_set(unit, resolution_reg, REG_PORT_ANY,  0,  tmp));
    }

    SOC_SAND_CHECK_FUNC_RESULT(arad_ips_queue_size_based_thresholds_table_tbl_set_unsafe( /* set hungry thresholds */
      unit, hw_q_type, &ips_queue_size_based_thresholds_table_tbl_data), 580, exit);
    exact_info->hungry_th.multiplier = ARAD_ITM_HUNGRY_TH_MULTIPLIER_FIELD_TO_VAL(
      ips_queue_size_based_thresholds_table_tbl_data.fsm_th_mul);

    SOC_SAND_CHECK_FUNC_RESULT( arad_ips_credit_balance_based_thresholds_table_tbl_set_unsafe( /* set satisfied backoff/backlog thresholds */
      unit, hw_q_type, &ips_credit_balance_based_thresholds_table_tbl_data), 600, exit);

    SOC_SAND_CHECK_FUNC_RESULT(arad_ips_empty_queue_credit_balance_table_tbl_set_unsafe( /* set satisfied empty queue thresholds */
      unit, hw_q_type, &ips_empty_queue_credit_balance_table_tbl_data), 620, exit);

    SOC_SAND_CHECK_FUNC_RESULT(arad_ips_credit_watchdog_thresholds_table_tbl_set_unsafe( /* set Watchdog thresholds */
      unit, hw_q_type, &ips_credit_watchdog_thresholds_table_tbl_data), 640, exit);

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  660,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPS_STORED_CREDITS_USAGE_CONFIGURATIONr, SOC_CORE_ALL, 0, MUL_PKT_DEQf, &mul_pckt_deq_fld_val));
    tmp = SOC_SAND_BOOL2NUM(info->is_low_latency);
    SOC_SAND_SET_BIT(mul_pckt_deq_fld_val, tmp, hw_q_type);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  670,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_STORED_CREDITS_USAGE_CONFIGURATIONr, SOC_CORE_ALL, 0, MUL_PKT_DEQf,  mul_pckt_deq_fld_val));
    exact_info->is_low_latency = info->is_low_latency;
  }

  /* Set if the queue type uses the remote credit value */
  if (SOC_IS_ARADPLUS(unit)) {
    uint32 reg, field, mask = 1 << hw_q_type;
    exact_info->is_remote_credit_value = info->is_remote_credit_value ? TRUE : FALSE;
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 700, exit, ARAD_REG_ACCESS_ERR, READ_IPS_IPS_CREDIT_CONFIG_4r(unit, &reg));
    field = soc_reg_field_get(unit, IPS_IPS_CREDIT_CONFIG_4r, reg, CREDIT_VALUE_Q_TYPE_BITMAPf);
    if (info->is_remote_credit_value) {
      field |= mask;
    } else {
      field &= ~mask;
    }
    soc_reg_field_set(unit, IPS_IPS_CREDIT_CONFIG_4r, &reg, CREDIT_VALUE_Q_TYPE_BITMAPf, field);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 710, exit, ARAD_REG_ACCESS_ERR, WRITE_IPS_IPS_CREDIT_CONFIG_4r(unit, reg));
  } else {
    exact_info->is_remote_credit_value = FALSE;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_cr_request_set_unsafe()", qt_ndx, hw_q_type);
}

/*********************************************************************
*     Per queue the device maintains an Off/Normal/Slow Credit
*     Request State. The device has 16 'Credit Request
*     Configurations', one per Credit-Class. Sets the (1)
*     Queue-Size-Thresholds (2) Credit-Balance-Thresholds (3)
*     Empty-Queue-Thresholds (4) Credit-Watchdog
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_cr_request_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_QT_NDX          qt_ndx,
    SOC_SAND_IN  ARAD_ITM_CR_REQUEST_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_CR_REQUEST_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  if ((qt_ndx < SOC_TMC_ITM_QT_NDX_02 || qt_ndx >= ARAD_ITM_IPS_QT_MAX) &&
      (qt_ndx < SOC_TMC_ITM_PREDEFIEND_OFFSET || qt_ndx >= SOC_TMC_ITM_PREDEFIEND_OFFSET + SOC_TMC_ITM_NOF_QT_STATIC))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_IPS_QT_RNG_OUT_OF_RANGE_ERR, 5, exit);
  }

  if (soc_sand_abs(info->hungry_th.normal_to_slow_th) > \
    ARAD_ITM_HUNGRY_TH_MAX)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_CR_REQUEST_HUNGRY_TH_OUT_OF_RANGE_ERR, 10, exit);
  }
  if (soc_sand_abs(info->hungry_th.off_to_normal_th) > \
    ARAD_ITM_HUNGRY_TH_MAX)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_CR_REQUEST_HUNGRY_TH_OUT_OF_RANGE_ERR, 20, exit);
  }
  if (soc_sand_abs(info->hungry_th.off_to_slow_th) > \
    ARAD_ITM_HUNGRY_TH_MAX)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_CR_REQUEST_HUNGRY_TH_OUT_OF_RANGE_ERR, 30, exit);
  }
  if (soc_sand_abs(info->hungry_th.slow_to_normal_th) > \
    ARAD_ITM_HUNGRY_TH_MAX)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_CR_REQUEST_HUNGRY_TH_OUT_OF_RANGE_ERR, 40, exit);
  }

  if (info->hungry_th.multiplier && soc_sand_log2_round_down(info->hungry_th.multiplier) <= ARAD_ITM_HUNGRY_TH_MULTIPLIER_OFFSET)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_CR_REQUEST_HUNGRY_TH_MULTIPLIER_OUT_OF_RANGE_ERR, 45, exit);
  }

  if (info->satisfied_th.backlog_th.backlog_exit_th> \
    ARAD_ITM_SATISFIED_TH_MAX)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_CR_REQUEST_SATISFIED_TH_OUT_OF_RANGE_ERR, 50, exit);
  }

  if (info->satisfied_th.backlog_th.backlog_enter_th> \
    ARAD_ITM_SATISFIED_TH_MAX)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_CR_REQUEST_SATISFIED_TH_OUT_OF_RANGE_ERR, 60, exit);
  }

  if (info->satisfied_th.backoff_th.backoff_exit_th> \
    ARAD_ITM_SATISFIED_TH_MAX)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_CR_REQUEST_SATISFIED_TH_OUT_OF_RANGE_ERR, 70, exit);
  }

  if (info->satisfied_th.backoff_th.backoff_enter_th> \
    ARAD_ITM_SATISFIED_TH_MAX)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_CR_REQUEST_SATISFIED_TH_OUT_OF_RANGE_ERR, 80, exit);
  }

  if (info->wd_th.cr_wd_dlt_q_th != 0 && !IS_CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE(unit))
  {
      SOC_SAND_ERR_IF_OUT_OF_RANGE(
        info->wd_th.cr_wd_dlt_q_th, ARAD_ITM_CR_WD_Q_TH_MIN_MSEC, ARAD_ITM_CR_WD_Q_TH_MAX_MSEC,
        ARAD_CR_WD_DEL_TH_OUT_OF_RANGE, 85, exit
      );
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_cr_request_verify()", qt_ndx, 0);
}

/*********************************************************************
*     Per queue the device maintains an Off/Normal/Slow Credit
*     Request State. The device has 16 'Credit Request
*     Configurations', one per Credit-Class. Sets the (1)
*     Queue-Size-Thresholds (2) Credit-Balance-Thresholds (3)
*     Empty-Queue-Thresholds (4) Credit-Watchdog
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_cr_request_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_QT_NDX          qt_ndx,
    SOC_SAND_OUT ARAD_ITM_CR_REQUEST_INFO *info
  )
{
  uint32
    mul_pckt_deq_fld_val,
    tmp;
  uint8
    hw_q_type = ARAD_SW_DB_QUEUE_TYPE_NOT_AVAILABLE;
  int32
    cr_bal_resolution = 0; /* should be signed so that multiplying with it does not turn signed values to unsigned */
  uint32
    res;
  ARAD_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_DATA
    ips_queue_size_based_thresholds_table_tbl_data;
  ARAD_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_DATA
    ips_credit_balance_based_thresholds_table_tbl_data;
  ARAD_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_DATA
    ips_empty_queue_credit_balance_table_tbl_data;
  ARAD_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_DATA
    ips_credit_watchdog_thresholds_table_tbl_data;
  soc_reg_t
      resolution_reg;
    

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_CR_REQUEST_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  
  /* Get the hardware queue type (credit request profile) */
  SOC_SAND_CHECK_FUNC_RESULT(arad_sw_db_queue_type_map_get(unit, qt_ndx, &hw_q_type), 10, exit);
  if (hw_q_type == ARAD_SW_DB_QUEUE_TYPE_NOT_AVAILABLE) { /* If dynamic queue type allocation failed */
    info->wd_th.cr_wd_dlt_q_th = SOC_TMC_ITM_CR_WD_Q_TH_OPERATION_FAILED;
    goto exit;
  }

  /* get the credit balance resolution if needed */
  resolution_reg = SOC_IS_ARADPLUS(unit)? IPS_REG_014Er: IPS_REG_014Br; /* Different register name in Arad+ */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  12,  exit, ARAD_REG_ACCESS_ERR,soc_reg32_get(unit, resolution_reg, REG_PORT_ANY,  0, &tmp));
  tmp = 3 & (tmp >> (2 * hw_q_type));
  cr_bal_resolution = 1 << (3 & (tmp >> (2 * hw_q_type)));
  if (tmp > ARAD_ITM_MAX_CRBL_RESOLUTION_EXPONENT) {
    SOC_SAND_SET_ERROR_CODE(ARAD_INTERNAL_ASSERT_ERR, 14, exit); /* internal error - resolution exponent too big */
  }
  cr_bal_resolution = 1 << tmp;

  /* get hungry thresholds { */
  res = arad_ips_queue_size_based_thresholds_table_tbl_get_unsafe(
          unit,
          hw_q_type,
          &ips_queue_size_based_thresholds_table_tbl_data
      );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = arad_itm_cr_request_info_hungry_table_field_get(
          ips_queue_size_based_thresholds_table_tbl_data.off_to_norm_msg_th,
          &(info->hungry_th.off_to_normal_th), cr_bal_resolution
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = arad_itm_cr_request_info_hungry_table_field_get(
          ips_queue_size_based_thresholds_table_tbl_data.off_to_slow_msg_th,
          &(info->hungry_th.off_to_slow_th), cr_bal_resolution
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = arad_itm_cr_request_info_hungry_table_field_get(
          ips_queue_size_based_thresholds_table_tbl_data.slow_to_norm_msg_th,
          &(info->hungry_th.slow_to_normal_th), cr_bal_resolution
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = arad_itm_cr_request_info_hungry_table_field_get(
          ips_queue_size_based_thresholds_table_tbl_data.norm_to_slow_msg_th,
          &(info->hungry_th.normal_to_slow_th), cr_bal_resolution
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  info->hungry_th.multiplier =
    ARAD_ITM_HUNGRY_TH_MULTIPLIER_FIELD_TO_VAL(
      ips_queue_size_based_thresholds_table_tbl_data.fsm_th_mul);
  /* get hungry thresholds } */

  /* get satisfied backoff/backlog thresholds { */

  res = arad_ips_credit_balance_based_thresholds_table_tbl_get_unsafe(
          unit,
          hw_q_type,
          &ips_credit_balance_based_thresholds_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  res = arad_itm_cr_request_info_satisfied_mnt_exp_table_field_get(
          ips_credit_balance_based_thresholds_table_tbl_data.backlog_enter_qcr_bal_th,
          &(info->satisfied_th.backlog_th.backlog_enter_th), cr_bal_resolution
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  res = arad_itm_cr_request_info_satisfied_mnt_exp_table_field_get(
          ips_credit_balance_based_thresholds_table_tbl_data.backlog_exit_qcr_bal_th,
          &(info->satisfied_th.backlog_th.backlog_exit_th), cr_bal_resolution
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

  res = arad_itm_cr_request_info_satisfied_mnt_exp_table_field_get(
          ips_credit_balance_based_thresholds_table_tbl_data.backoff_enter_qcr_bal_th,
          &(info->satisfied_th.backoff_th.backoff_enter_th), cr_bal_resolution
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  res = arad_itm_cr_request_info_satisfied_mnt_exp_table_field_get(
          ips_credit_balance_based_thresholds_table_tbl_data.backoff_exit_qcr_bal_th,
          &(info->satisfied_th.backoff_th.backoff_exit_th), cr_bal_resolution
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

  /* get satisfied backoff/backlog thresholds } */

  /* get satisfied empty queue thresholds { */

  res = arad_ips_empty_queue_credit_balance_table_tbl_get_unsafe(
          unit,
          hw_q_type,
          &ips_empty_queue_credit_balance_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);


  if (ips_empty_queue_credit_balance_table_tbl_data.empty_qsatisfied_cr_bal > SOC_SAND_I16_MAX )
  {
    info->satisfied_th.empty_queues.satisfied_empty_q_th = cr_bal_resolution *
      (int32)(ips_empty_queue_credit_balance_table_tbl_data.empty_qsatisfied_cr_bal -
      (SOC_SAND_U16_MAX + 1));
  }
  else
  {
    info->satisfied_th.empty_queues.satisfied_empty_q_th = cr_bal_resolution *
      (int32)(ips_empty_queue_credit_balance_table_tbl_data.empty_qsatisfied_cr_bal);
  }

  if(ips_empty_queue_credit_balance_table_tbl_data.max_empty_qcr_bal > SOC_SAND_I16_MAX )
  {
    info->satisfied_th.empty_queues.max_credit_balance_empty_q = cr_bal_resolution *
      (int32)(ips_empty_queue_credit_balance_table_tbl_data.max_empty_qcr_bal -
      (SOC_SAND_U16_MAX + 1));
  }
  else
  {
    info->satisfied_th.empty_queues.max_credit_balance_empty_q = cr_bal_resolution *
    (int32)(ips_empty_queue_credit_balance_table_tbl_data.max_empty_qcr_bal);
  }

  info->satisfied_th.empty_queues.exceed_max_empty_q =
    (uint8)ips_empty_queue_credit_balance_table_tbl_data.exceed_max_empty_qcr_bal;

  /* get satisfied empty queue thresholds } */

  /* get Watchdog thresholds { */
  res = arad_ips_credit_watchdog_thresholds_table_tbl_get_unsafe(
          unit,
          hw_q_type,
          &ips_credit_watchdog_thresholds_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);


  if (IS_CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE(unit)) {
    /* The calculation here is (wd_delete_qth << wd_status_msg_gen_period) * exact_credit_watchdog_scan_time_nano / 10^6 */
    /* It is written more complexly to avoid overflow */
    info->wd_th.cr_wd_dlt_q_th = ((ips_credit_watchdog_thresholds_table_tbl_data.wd_delete_qth <<
      ips_credit_watchdog_thresholds_table_tbl_data.wd_status_msg_gen_period) *
      (SOC_DPP_CONFIG(unit)->arad->exact_credit_watchdog_scan_time_nano / ARAD_CREDIT_WATCHDOG_COMMON_MIN_SCAN_TIME_NS)) /
      (1000000 / ARAD_CREDIT_WATCHDOG_COMMON_MIN_SCAN_TIME_NS);
    info->wd_th.cr_wd_stts_msg_gen = 0;
  } else {
    uint32 cycle_time = 0;
    SOC_SAND_CHECK_FUNC_RESULT(arad_itm_get_set_cr_watch_dog_scan_calc_unsafe(
      unit, 0, &cycle_time, 0), 160, exit);
    info->wd_th.cr_wd_dlt_q_th = NS_TO_MS_ROUND(cycle_time * ips_credit_watchdog_thresholds_table_tbl_data.wd_delete_qth);
    info->wd_th.cr_wd_stts_msg_gen = NS_TO_MS_ROUND(cycle_time * ips_credit_watchdog_thresholds_table_tbl_data.wd_status_msg_gen_period);
  }

  /* get Watchdog thresholds } */

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  170,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPS_STORED_CREDITS_USAGE_CONFIGURATIONr, SOC_CORE_ALL, 0, MUL_PKT_DEQf, &mul_pckt_deq_fld_val));
  tmp = SOC_SAND_GET_BIT(mul_pckt_deq_fld_val, hw_q_type);
  info->is_low_latency = SOC_SAND_NUM2BOOL(tmp);
  
  /* return if the queue type uses the remote credit value */
  if (SOC_IS_ARADPLUS(unit)) {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 180, exit, ARAD_REG_ACCESS_ERR, READ_IPS_IPS_CREDIT_CONFIG_4r(unit, &tmp));
    info->is_remote_credit_value = ((soc_reg_field_get(unit, IPS_IPS_CREDIT_CONFIG_4r, tmp, CREDIT_VALUE_Q_TYPE_BITMAPf)
      >> hw_q_type) & 1) ? TRUE : FALSE;
  } else {
    info->is_remote_credit_value = FALSE;
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_cr_request_get_unsafe()", unit, hw_q_type);
}

/*********************************************************************
*     There are 16 possible credit-discount classes.
*     Each Credit Class is configured with a value that
*     is to be added/subtracted from the credit counter at each
*     dequeue of packet. This procedure sets the 16
*     credit-discount values per credit class.
*     The Credit Discount value should be calculated as following:
*     Credit-Discount =
*     -(IPG (20B)+ CRC (size of CRC field only if it is not removed by NP)) +
*     Dune_H (size of FTMH + FTMH extension (if exists)) +
*     NP_H (size of Network Processor Header, or Dune PP Header) + DRAM CRC size.
*     Note that this functionality will take affect only when working with
*     small packet sizes.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_cr_discount_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_CR_DISCNT_CLS_NDX cr_cls_ndx,
    SOC_SAND_IN  ARAD_ITM_CR_DISCOUNT_INFO *info
  )
{
  uint32
    res;
  ARAD_IQM_CREDIT_DISCOUNT_TABLE_TBL_DATA
    iqm_credit_discount_table_tbl_data;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_CR_DISCOUNT_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  iqm_credit_discount_table_tbl_data.crdt_disc_val = (uint32)soc_sand_abs(info->discount);
  iqm_credit_discount_table_tbl_data.discnt_sign = (info->discount < 0);

  res = arad_iqm_credit_discount_table_tbl_set_unsafe(
          unit,
          cr_cls_ndx,
          &iqm_credit_discount_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_cr_discount_set_unsafe()",0,0);
}

/*********************************************************************
*     There are 16 possible credit-discount classes.
*     Each Credit Class is configured with a value that
*     is to be added/subtracted from the credit counter at each
*     dequeue of packet. This procedure sets the 16
*     credit-discount values per credit class.
*     The Credit Discount value should be calculated as following:
*     Credit-Discount =
*     -IPG (20B)+ CRC (size of CRC field only if it is not removed by NP) +
*     NP_H (size of Network Processor Header) + Dune_H (size of ITMH+FTMH).
*     Note that this functionality will take affect only when working with
*     small packet sizes.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_cr_discount_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_CR_DISCNT_CLS_NDX cr_cls_ndx,
    SOC_SAND_IN  ARAD_ITM_CR_DISCOUNT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_CR_DISCOUNT_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    cr_cls_ndx, ARAD_ITM_QT_CR_CLS_MAX,
    ARAD_ITM_CR_CLS_OUT_OF_RANGE_ERR, 10, exit
  );

  SOC_SAND_ERR_IF_OUT_OF_RANGE(
    info->discount, ARAD_ITM_CR_DISCOUNT_MIN_VAL, ARAD_ITM_CR_DISCOUNT_MAX_VAL,
    ARAD_ITM_CR_DISCOUNT_OUT_OF_RANGE_ERR, 20, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_cr_discount_verify()",cr_cls_ndx,info->discount);
}

/*********************************************************************
*     There are 16 possible credit-discount classes.
*     Each Credit Class is configured with a value that
*     is to be added/subtracted from the credit counter at each
*     dequeue of packet. This procedure sets the 16
*     credit-discount values per credit class.
*     The Credit Discount value should be calculated as following:
*     Credit-Discount =
*     -(IPG (20B)+ CRC (size of CRC field only if it is not removed by NP)) +
*     Dune_H (size of FTMH + FTMH extension (if exists)) +
*     NP_H (size of Network Processor Header, or Dune PP Header) + DRAM CRC size.
*     Note that this functionality will take affect only when working with
*     small packet sizes.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_cr_discount_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_CR_DISCNT_CLS_NDX cr_cls_ndx,
    SOC_SAND_OUT ARAD_ITM_CR_DISCOUNT_INFO *info
  )
{
  uint32
    res;
  ARAD_IQM_CREDIT_DISCOUNT_TABLE_TBL_DATA
    iqm_credit_discount_table_tbl_data;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_CR_DISCOUNT_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = arad_iqm_credit_discount_table_tbl_get_unsafe(
          unit,
          cr_cls_ndx,
          &iqm_credit_discount_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  info->discount = iqm_credit_discount_table_tbl_data.crdt_disc_val;
  if (iqm_credit_discount_table_tbl_data.discnt_sign)
  {
    info->discount *= -1;
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_cr_discount_get_unsafe()",0,0);
}

/*********************************************************************
*     Each queue (VOQ) is assigned with a test template.
*     This Function sets the admit logic test of the queue
*     per rate-class and drop-precedence (there are 4
*     pre-configured by 'arad_itm_admit_test_tmplt_set'
*     options for test types).
*     Notice that in a queue, is a packet is chosen to be
*     rejected normally, the admit test logic will not affect it.
*     For more information about the admit test template refer to
*     the description of 'arad_itm_admit_test_tmplt_set'.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_queue_test_tmplt_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  ARAD_ITM_ADMIT_TSTS      test_tmplt
  )
{
  uint32
    res;
  ARAD_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_DATA
    iqm_packet_queue_red_parameters_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_QUEUE_TEST_TMPLT_SET_UNSAFE);

  res = arad_iqm_packet_queue_red_parameters_table_tbl_get_unsafe(
          unit,
          rt_cls_ndx,
          drop_precedence_ndx,
          &iqm_packet_queue_red_parameters_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  iqm_packet_queue_red_parameters_table_tbl_data.addmit_logic =
    (uint32)test_tmplt;

  res = arad_iqm_packet_queue_red_parameters_table_tbl_set_unsafe(
          unit,
          rt_cls_ndx,
          drop_precedence_ndx,
          &iqm_packet_queue_red_parameters_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_queue_test_tmplt_set_unsafe()",0,0);
}

/*********************************************************************
*     Each queue (VOQ) is assigned with a test template.
*     This Function sets the admit logic test of the queue
*     per rate-class and drop-precedence (there are 4
*     pre-configured by 'arad_itm_admit_test_tmplt_set'
*     options for test types).
*     Notice that in a queue, is a packet is chosen to be
*     rejected normally, the admit test logic will not affect it.
*     For more information about the admit test template refer to
*     the description of 'arad_itm_admit_test_tmplt_set'.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_queue_test_tmplt_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  ARAD_ITM_ADMIT_TSTS      test_tmplt
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_QUEUE_TEST_TMPLT_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    rt_cls_ndx, ARAD_ITM_QT_RT_CLS_MAX,
    ARAD_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    drop_precedence_ndx, ARAD_MAX_DROP_PRECEDENCE,
    ARAD_DRPP_OUT_OF_RANGE_ERR, 7, exit
  );

  if (test_tmplt > (ARAD_ITM_ADMIT_TSTS_LAST-1))
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_ADMT_TEST_ID_OUT_OF_RANGE_ERR, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_queue_test_tmplt_verify()",0,0);
}

/*********************************************************************
*     Each queue (VOQ) is assigned with a test template.
*     This Function sets the admit logic test of the queue
*     per rate-class and drop-precedence (there are 4
*     pre-configured by 'arad_itm_admit_test_tmplt_set'
*     options for test types).
*     Notice that in a queue, is a packet is chosen to be
*     rejected normally, the admit test logic will not affect it.
*     For more information about the admit test template refer to
*     the description of 'arad_itm_admit_test_tmplt_set'.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_queue_test_tmplt_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_OUT ARAD_ITM_ADMIT_TSTS      *test_tmplt
  )
{
  uint32
    res;
  ARAD_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_DATA
    iqm_packet_queue_red_parameters_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_QUEUE_TEST_TMPLT_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(test_tmplt);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    rt_cls_ndx, ARAD_ITM_QT_RT_CLS_MAX,
    ARAD_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    drop_precedence_ndx, ARAD_MAX_DROP_PRECEDENCE,
    ARAD_DRPP_OUT_OF_RANGE_ERR, 7, exit
  );

  res = arad_iqm_packet_queue_red_parameters_table_tbl_get_unsafe(
          unit,
          rt_cls_ndx,
          drop_precedence_ndx,
          &iqm_packet_queue_red_parameters_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  *test_tmplt =
    iqm_packet_queue_red_parameters_table_tbl_data.addmit_logic;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_queue_test_tmplt_get_unsafe()",0,0);
}

/*********************************************************************
*     Sets exponential weighted queue per rate-class. The
*     calculation the device does: Average_constant =
*     2^(-RED_exp_weight)if (Instantaneous-Queue-size <
*     Average-queue-size) Average-queue-size =
*     Instantaneous-Queue-size else Average-queue-size =
*     (1-Average_constant)*Average-queue-size +
*     Average_constant*Instantaneous-Queue-size To configure
*     WRED configuration which are per queue-type and dp, use
*     the functionarad_itm_wred_info_set.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_wred_exp_wq_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                  exp_wq
  )
{
  uint32
    res;
  ARAD_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_DATA
    iqm_packet_queue_red_weight_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_WRED_EXP_WQ_SET_UNSAFE);

  res = arad_iqm_packet_queue_red_weight_table_tbl_get_unsafe(
          unit,
          rt_cls_ndx,
          &iqm_packet_queue_red_weight_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  iqm_packet_queue_red_weight_table_tbl_data.pq_weight =
    exp_wq;

  res = arad_iqm_packet_queue_red_weight_table_tbl_set_unsafe(
          unit,
          rt_cls_ndx,
          &iqm_packet_queue_red_weight_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_wred_exp_wq_set_unsafe()",0,0);
}

/*********************************************************************
*     Sets exponential weighted queue per rate-class. The
*     calculation the device does: Average_constant =
*     2^(-RED_exp_weight)if (Instantaneous-Queue-size <
*     Average-queue-size) Average-queue-size =
*     Instantaneous-Queue-sizeelse Average-queue-size =
*     (1-Average_constant)*Average-queue-size +
*     Average_constant*Instantaneous-Queue-size To configure
*     WRED configuration which are per queue-type and dp, use
*     the functionarad_itm_wred_info_set.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_wred_exp_wq_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                  exp_wq
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_WRED_EXP_WQ_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    rt_cls_ndx, ARAD_ITM_QT_RT_CLS_MAX,
    ARAD_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 10, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    exp_wq, ARAD_ITM_WQ_MAX,
    ARAD_ITM_WRED_EXP_WT_PARAMETER_OUT_OF_RANGE_ERR, 20, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_wred_exp_wq_verify()",0,0);
}

/*********************************************************************
*     Sets exponential weighted queue per rate-class. The
*     calculation the device does: Average_constant =
*     2^(-RED_exp_weight)if (Instantaneous-Queue-size <
*     Average-queue-size) Average-queue-size =
*     Instantaneous-Queue-sizeelse Average-queue-size =
*     (1-Average_constant)*Average-queue-size +
*     Average_constant*Instantaneous-Queue-size To configure
*     WRED configuration which are per queue-type and dp, use
*     the functionarad_itm_wred_info_set.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_wred_exp_wq_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_OUT  uint32                  *exp_wq
  )
{
  uint32
    res;
  ARAD_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_DATA
    iqm_packet_queue_red_weight_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_WRED_EXP_WQ_GET_UNSAFE);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    rt_cls_ndx, ARAD_ITM_QT_RT_CLS_MAX,
    ARAD_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 5, exit
  );

  res = arad_iqm_packet_queue_red_weight_table_tbl_get_unsafe(
          unit,
          rt_cls_ndx,
          &iqm_packet_queue_red_weight_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  *exp_wq =
    (uint8)iqm_packet_queue_red_weight_table_tbl_data.pq_weight;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_wred_exp_wq_get_unsafe()",0,0);
}

/*********************************************************************
*     Sets WRED parameters per rate-class and drop precedence,
*     including wred-enable and the admit test logic. Also, as
*     part of the WRED parameters; max-queue,
*     WRED-thresholds/probability. To configure WRED
*     Configuration that is per queue-type only (exponential
*     weight queue), use the function
*     arad_itm_wred_exp_wq_set.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_wred_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  ARAD_ITM_WRED_QT_DP_INFO *info,
    SOC_SAND_OUT ARAD_ITM_WRED_QT_DP_INFO *exact_info
  )
{
  uint32
    res;
  ARAD_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_DATA
    iqm_packet_queue_red_parameters_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_WRED_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);

  res = arad_iqm_packet_queue_red_parameters_table_tbl_get_unsafe(
          unit,
          rt_cls_ndx,
          drop_precedence_ndx,
          &iqm_packet_queue_red_parameters_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  iqm_packet_queue_red_parameters_table_tbl_data.pq_wred_en =
    exact_info->wred_en = SOC_SAND_BOOL2NUM(info->wred_en);

  iqm_packet_queue_red_parameters_table_tbl_data.pq_wred_pckt_sz_ignr =
    exact_info->ignore_packet_size = SOC_SAND_BOOL2NUM(info->ignore_packet_size);

  res = arad_itm_WRED_QT_DP_INFO_to_WRED_TBL_DATA(
          info,
          &iqm_packet_queue_red_parameters_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = arad_itm_WRED_TBL_DATA_to_WRED_QT_DP_INFO(
          &iqm_packet_queue_red_parameters_table_tbl_data,
          exact_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = arad_iqm_packet_queue_red_parameters_table_tbl_set_unsafe(
          unit,
          rt_cls_ndx,
          drop_precedence_ndx,
          &iqm_packet_queue_red_parameters_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_wred_set_unsafe()",0,0);
}

/*********************************************************************
*     Sets WRED parameters per rate-class and drop precedence,
*     including wred-enable and the admit test logic. Also, as
*     part of the WRED parameters; max-queue,
*     WRED-thresholds/probability. To configure WRED
*     Configuration that is per queue-type only (exponential
*     weight queue), use the function
*     arad_itm_wred_exp_wq_set.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_wred_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  ARAD_ITM_WRED_QT_DP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_WRED_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    rt_cls_ndx, ARAD_ITM_QT_RT_CLS_MAX,
    ARAD_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    drop_precedence_ndx, SOC_TMC_NOF_DROP_PRECEDENCE,
    ARAD_DRPP_OUT_OF_RANGE_ERR, 7, exit
  );

  if (info->min_avrg_th > info->max_avrg_th)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_WRED_MIN_TH_HIGHER_THEN_MAX_TH_ERR, 50, exit);
  }

  if (info->min_avrg_th > ARAD_Q_WRED_INFO_MIN_AVRG_TH_MAX)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_WRED_MIN_AVRG_TH_OUT_OF_RANGE_ERR, 20, exit);
  }

  if (info->max_avrg_th > ARAD_Q_WRED_INFO_MAX_AVRG_TH_MAX)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_WRED_MAX_AVRG_TH_OUT_OF_RANGE_ERR, 30, exit);
  }

  if (info->max_probability > ARAD_ITM_WRED_QT_DP_INFO_MAX_PROBABILITY_MAX)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_WRED_PROB_OUT_OF_RANGE_ERR, 40, exit);
  }

  if (info->max_packet_size > ARAD_ITM_WRED_MAX_PACKET_SIZE)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_WRED_MAX_PACKET_SIZE_OUT_OF_RANGE_ERR, 50, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_wred_verify()",0,0);
}

/*********************************************************************
*     Sets WRED parameters per rate-class and drop precedence,
*     including wred-enable and the admit test logic. Also, as
*     part of the WRED parameters; max-queue,
*     WRED-thresholds/probability. To configure WRED
*     Configuration that is per queue-type only (exponential
*     weight queue), use the function
*     arad_itm_wred_exp_wq_set.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_wred_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_OUT ARAD_ITM_WRED_QT_DP_INFO *info
  )
{
  uint32
    res;
  ARAD_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_DATA
    iqm_packet_queue_red_parameters_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_WRED_GET_UNSAFE);

  sal_memset(&iqm_packet_queue_red_parameters_table_tbl_data,0x0,sizeof(ARAD_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_DATA));

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    rt_cls_ndx, ARAD_ITM_QT_RT_CLS_MAX,
    ARAD_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    drop_precedence_ndx, SOC_TMC_NOF_DROP_PRECEDENCE,
    ARAD_DRPP_OUT_OF_RANGE_ERR, 7, exit
  );

  res = arad_iqm_packet_queue_red_parameters_table_tbl_get_unsafe(
          unit,
          rt_cls_ndx,
          drop_precedence_ndx,
          &iqm_packet_queue_red_parameters_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = arad_itm_WRED_TBL_DATA_to_WRED_QT_DP_INFO(
          &iqm_packet_queue_red_parameters_table_tbl_data,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  info->wred_en =
    SOC_SAND_NUM2BOOL(iqm_packet_queue_red_parameters_table_tbl_data.pq_wred_en);

  info->ignore_packet_size =
    SOC_SAND_NUM2BOOL(iqm_packet_queue_red_parameters_table_tbl_data.pq_wred_pckt_sz_ignr);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_wred_get_unsafe()",0,0);
}

/*********************************************************************
*     Sets tail drop parameter - max-queue-size per rate-class
*     and drop precedence. The tail drop mechanism drops
*     packets that are mapped to queues that exceed thresholds
*     of this structure.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_tail_drop_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  ARAD_ITM_TAIL_DROP_INFO  *info,
    SOC_SAND_OUT ARAD_ITM_TAIL_DROP_INFO  *exact_info
  )
{
  uint32
    res,
    max_inst_q_siz_mnt_nof_bits,
    max_inst_q_siz_bds_mnt_nof_bits,
    max_inst_q_siz_exp_nof_bits;
  ARAD_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_DATA
    iqm_packet_queue_red_parameters_table_tbl_data;
  uint32
    pq_max_que_size_mnt,
    pq_max_que_size_exp,
    max_q_size = info->max_inst_q_size,
    resolution = ARAD_ITM_DROP_TAIL_SIZE_RESOLUTION;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_TAIL_DROP_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);

  /* max_inst_q_size */
 
  res = arad_iqm_packet_queue_red_parameters_table_tbl_get_unsafe(
          unit,
          rt_cls_ndx,
          drop_precedence_ndx,
          &iqm_packet_queue_red_parameters_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  res = arad_iqm_packet_queue_red_parameters_tail_drop_mantissa_nof_bits(
             unit,
             drop_precedence_ndx,
             &max_inst_q_siz_mnt_nof_bits,
             &max_inst_q_siz_bds_mnt_nof_bits
         );
  SOC_SAND_CHECK_FUNC_RESULT(res, 21, exit);
  max_inst_q_siz_exp_nof_bits = soc_mem_field_length(unit, IQM_PQREDm, PQ_MAX_QUE_SIZEf) - max_inst_q_siz_mnt_nof_bits;

  if ((drop_precedence_ndx == SOC_TMC_NOF_DROP_PRECEDENCE) && (!SOC_IS_ARADPLUS(unit))) {
    resolution = 1;
    if (max_q_size > ARAD_B0_MAX_ECN_QUEUE_BYTES) { /* size can con not be represented due to HW bug */
      if (max_q_size < ARAD_MAX_QUEUE_SIZE_BYTES) { /* user wanted a real size, below amount of memory */
        SOC_SAND_SET_ERROR_CODE(ARAD_QUEUE_SIZE_OUT_OF_RANGE_ERR, 24, exit);
      }
      max_q_size = ARAD_MAX_QUEUE_SIZE_BYTES; /* queue size in bytes disabled */
    }
  }
  res = soc_sand_break_to_mnt_exp_round_up(
        SOC_SAND_DIV_ROUND_UP(info->max_inst_q_size, resolution),
        max_inst_q_siz_mnt_nof_bits,
        max_inst_q_siz_exp_nof_bits,
        0,
        &(pq_max_que_size_mnt),
        &(pq_max_que_size_exp)
      );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  iqm_packet_queue_red_parameters_table_tbl_data.pq_max_que_size_mnt = pq_max_que_size_mnt;
  iqm_packet_queue_red_parameters_table_tbl_data.pq_max_que_size_exp = pq_max_que_size_exp;

  exact_info->max_inst_q_size = iqm_packet_queue_red_parameters_table_tbl_data.pq_max_que_size_mnt << iqm_packet_queue_red_parameters_table_tbl_data.pq_max_que_size_exp;

  exact_info->max_inst_q_size *= resolution;


  /* max_inst_q_size_bds */
  max_inst_q_siz_exp_nof_bits = soc_mem_field_length(unit, IQM_PQREDm, PQ_MAX_QUE_BUFF_SIZEf) - max_inst_q_siz_bds_mnt_nof_bits;

  res = soc_sand_break_to_mnt_exp_round_up(
          info->max_inst_q_size_bds,
          max_inst_q_siz_bds_mnt_nof_bits,
          max_inst_q_siz_exp_nof_bits,
          0,
          &(pq_max_que_size_mnt),
          &(pq_max_que_size_exp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  iqm_packet_queue_red_parameters_table_tbl_data.pq_max_que_size_bds_mnt = pq_max_que_size_mnt;
  iqm_packet_queue_red_parameters_table_tbl_data.pq_max_que_size_bds_exp = pq_max_que_size_exp;

  exact_info->max_inst_q_size_bds = iqm_packet_queue_red_parameters_table_tbl_data.pq_max_que_size_bds_mnt << iqm_packet_queue_red_parameters_table_tbl_data.pq_max_que_size_bds_exp;

  res = arad_iqm_packet_queue_red_parameters_table_tbl_set_unsafe(
          unit,
          rt_cls_ndx,
          drop_precedence_ndx,
          &iqm_packet_queue_red_parameters_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_tail_drop_set_unsafe()",rt_cls_ndx,drop_precedence_ndx);
}

/*********************************************************************
*     Sets tail drop parameter - max-queue-size per rate-class
*     and drop precedence. The tail drop mechanism drops
*     packets that are mapped to queues that exceed thresholds
*     of this structure.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_tail_drop_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  ARAD_ITM_TAIL_DROP_INFO  *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_TAIL_DROP_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    rt_cls_ndx, ARAD_ITM_QT_RT_CLS_MAX,
    ARAD_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    drop_precedence_ndx, SOC_TMC_NOF_DROP_PRECEDENCE,
    ARAD_DRPP_OUT_OF_RANGE_ERR, 7, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->max_inst_q_size, ARAD_MAX_QUEUE_SIZE_BYTES,
    ARAD_QUEUE_SIZE_OUT_OF_RANGE_ERR, 9, exit
  )

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->max_inst_q_size_bds, ARAD_MAX_QUEUE_SIZE_BDS,
    ARAD_QUEUE_SIZE_OUT_OF_RANGE_ERR, 10, exit
  )

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_tail_drop_verify()", rt_cls_ndx, drop_precedence_ndx);
}

/*********************************************************************
*     Sets tail drop parameter - max-queue-size per rate-class
*     and drop precedence. The tail drop mechanism drops
*     packets that are mapped to queues that exceed thresholds
*     of this structure.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_tail_drop_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_OUT ARAD_ITM_TAIL_DROP_INFO  *info
  )
{
  uint32
    res;
  ARAD_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_DATA
    iqm_packet_queue_red_parameters_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_TAIL_DROP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    rt_cls_ndx, ARAD_ITM_QT_RT_CLS_MAX,
    ARAD_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(drop_precedence_ndx, SOC_TMC_NOF_DROP_PRECEDENCE, ARAD_DRPP_OUT_OF_RANGE_ERR, 7, exit);

  res = arad_iqm_packet_queue_red_parameters_table_tbl_get_unsafe(
          unit,
          rt_cls_ndx,
          drop_precedence_ndx,
          &iqm_packet_queue_red_parameters_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  info->max_inst_q_size =
    ((iqm_packet_queue_red_parameters_table_tbl_data.pq_max_que_size_mnt) *
    (1<<(iqm_packet_queue_red_parameters_table_tbl_data.pq_max_que_size_exp)));

  if ((drop_precedence_ndx != SOC_TMC_NOF_DROP_PRECEDENCE) || SOC_IS_ARADPLUS(unit)) {
    info->max_inst_q_size *= ARAD_ITM_DROP_TAIL_SIZE_RESOLUTION; /* if ECN HW bug does not exist, use resolution */
  }

  info->max_inst_q_size_bds =
    ((iqm_packet_queue_red_parameters_table_tbl_data.pq_max_que_size_bds_mnt) *
    (1<<(iqm_packet_queue_red_parameters_table_tbl_data.pq_max_que_size_bds_exp)));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_tail_drop_get_unsafe()",rt_cls_ndx,drop_precedence_ndx);
}

/*********************************************************************
*     Sets ingress-queue credit Watchdog thresholds and
*     configuration. includes: start-queue, end-queue and
*     wd-rates.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_cr_wd_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_CR_WD_INFO      *info,
    SOC_SAND_OUT ARAD_ITM_CR_WD_INFO      *exact_info
  )
{
  uint32 res, reg_val, conf_reg = 0;
  uint64 conf_reg64;
  uint32 scan_time_us = info->min_scan_cycle_period_micro;
  int core_id = SOC_CORE_ALL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_CR_WD_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(exact_info);

  if (info->bottom_queue == ARAD_CREDIT_WATCHDOG_Q_COMMON_FSM_ADJUST_EXP && 
      info->top_queue == ARAD_CREDIT_WATCHDOG_Q_COMMON_FSM_ADJUST_EXP &&
      info == exact_info) {
      /* handle special calls to this function who do something else than the main purpose of the function */

      if (info->max_flow_msg_gen_rate_nano == ARAD_CREDIT_WATCHDOG_Q_COMMON_FSM_ADJUST_EXP) {
          if (info->min_scan_cycle_period_micro == ARAD_CREDIT_WATCHDOG_Q_COMMON_FSM_ADJUST_EXP) {

              /* for common FSM mode,return in min_scan_cycle_period_micro, the number of scans after which a message is generated */
              if (IS_CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE(unit)) {
                  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 3100, exit, ARAD_REG_ACCESS_ERR,
                    READ_IPS_CREDIT_WATCHDOG_CONFIGURATIONr(unit, core_id, &conf_reg64)); /* read 32/64 bit value */
                  conf_reg = soc_reg64_field32_get(unit, IPS_CREDIT_WATCHDOG_CONFIGURATIONr, conf_reg64, CR_WD_FSM_MODEf);
              }
              exact_info->min_scan_cycle_period_micro = conf_reg == 3 ? 1 : (conf_reg != 2 ? 0 : 2); /* 0 means no message */
          } else {

              /* a wrapper to calling arad_itm_get_set_cr_watch_dog_scan_calc_unsafe(dev, info->min_scan_cycle_period_micro, &info->min_scan_cycle_period_micro, 0) */
              SOC_DPP_CONFIG(unit)->arad->exact_credit_watchdog_scan_time_nano = 0;
              SOC_SAND_CHECK_FUNC_RESULT(arad_itm_get_set_cr_watch_dog_scan_calc_unsafe(unit,
                info->min_scan_cycle_period_micro, &exact_info->min_scan_cycle_period_micro, 0), 3150, exit);
          }
      } else if (IS_CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE(unit)) {

          /* change the delete time exponents of all profiles: decrease/increase by min_scan_cycle_period_micro/max_flow_msg_gen_rate_nano */
          int i;
          if (info->min_scan_cycle_period_micro > ARAD_CREDIT_WATCHDOG_COMMON_SCAN_TIME_MAX_HALVES ||
              info->max_flow_msg_gen_rate_nano > ARAD_CREDIT_WATCHDOG_COMMON_SCAN_TIME_MAX_HALVES ||
              (info->min_scan_cycle_period_micro && info->max_flow_msg_gen_rate_nano)) {
              SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 3200, exit); /* unsupported input */
          }
          for (i = 0; i < 16; ++i) {
              SOC_SAND_SOC_IF_ERROR_RETURN(res, 3210, exit, READ_IPS_CRWDTHm(unit, MEM_BLOCK_ANY, i, &reg_val));
              if (soc_IPS_CRWDTHm_field32_get(unit, &reg_val, WD_DELETE_Q_THf)) {
                  conf_reg = soc_IPS_CRWDTHm_field32_get(unit, &reg_val, WD_STATUS_MSG_GEN_PERIODf);
                  if (info->min_scan_cycle_period_micro) {
                      conf_reg -= info->min_scan_cycle_period_micro;
                  } else {
                      conf_reg += info->max_flow_msg_gen_rate_nano;
                  }
                  if (conf_reg > ARAD_CREDIT_WATCHDOG_COMMON_MAX_DELETE_EXP) {
                      SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 3220, exit); /* exponent too big - internal error */
                  }
                  soc_IPS_CRWDTHm_field32_set(unit, &reg_val, WD_STATUS_MSG_GEN_PERIODf, conf_reg);
                  SOC_SAND_SOC_IF_ERROR_RETURN(res, 3230, exit, WRITE_IPS_CRWDTHm(unit, MEM_BLOCK_ANY, i, &reg_val));
              }
          }
      } else if (IS_CREDIT_WATCHDOG_UNINITIALIZED(unit)) {
          /* set the watchdog mode based on HW when initializing Arad */
          SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 3250, exit, ARAD_REG_ACCESS_ERR,
            READ_IPS_CREDIT_WATCHDOG_CONFIGURATIONr(unit, core_id, &conf_reg64)); /* read 32/64 bit value */
          conf_reg = COMPILER_64_LO(conf_reg64);
          reg_val = soc_reg_field_get(unit, IPS_CREDIT_WATCHDOG_CONFIGURATIONr, conf_reg, CR_WD_MIN_SCAN_CYCLE_PERIODf);
          if (reg_val) { /* is credit watchdog active */
              uint32 scan_ns, i;
              SOC_SAND_CHECK_FUNC_RESULT( arad_ticks_to_time(unit, reg_val, TRUE,
                ARAD_ITM_WD_INFO_MIN_SCAN_CYCLE_PERIOD_MICRO_GRANULARITY, &scan_ns) , 3260, exit);
              if (SOC_IS_ARADPLUS(unit) &&
                  soc_reg64_field32_get(unit, IPS_CREDIT_WATCHDOG_CONFIGURATIONr, conf_reg64, CR_WD_DELETE_Q_EXP_MODEf)) {
                  /* This is common message time mode. Get exact scan time */
                  for (i = ARAD_CREDIT_WATCHDOG_COMMON_MAX_SCAN_TIME_NS; ;)  {
                      if (scan_ns >= i) {
                          if ((scan_ns / 100) * 99 >= i) {
                              SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 3270, exit); /* invalid value */
                          }
                          SOC_DPP_CONFIG(unit)->arad->exact_credit_watchdog_scan_time_nano = i;
                          break;
                      }
                      i /= 2;
                      if (i < ARAD_CREDIT_WATCHDOG_COMMON_MIN_SCAN_TIME_NS) {
                          SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 3275, exit); /* invalid value */
                      }
                  }
                  SET_CREDIT_WATCHDOG_MODE(unit, 
                    soc_reg64_field32_get(unit, IPS_CREDIT_WATCHDOG_CONFIGURATIONr, conf_reg64, CR_WD_FSM_MODEf) != 2 ?
                    CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE : CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE + 1);
              } else {
                  if (scan_ns >= ARAD_MIN_SCAN_CYCLE_PERIOD_MICRO * 1000 &&
                      scan_ns <= ARAD_MIN_SCAN_CYCLE_PERIOD_MICRO * 1010) {
                      SET_CREDIT_WATCHDOG_MODE(unit, CREDIT_WATCHDOG_NORMAL_MODE);
                  } else if (scan_ns >= ARAD_MIN_SCAN_CYCLE_PERIOD_MICRO_AGGRESSIVE_WD_STATUS_MSG * 1000 &&
                             scan_ns <= ARAD_MIN_SCAN_CYCLE_PERIOD_MICRO_AGGRESSIVE_WD_STATUS_MSG * 1010) {
                      SET_CREDIT_WATCHDOG_MODE(unit, CREDIT_WATCHDOG_FAST_STATUS_MESSAGE_MODE);
                  } else {
                      SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 3290, exit); /* invalid value */
                  }
                  SOC_DPP_CONFIG(unit)->arad->exact_credit_watchdog_scan_time_nano = scan_ns;
              }
          } else { /* credit watchdog is not active, get mode from CR_WD_MAX_FLOW_MSG_GEN_RATEf */
              reg_val = soc_reg_field_get(unit, IPS_CREDIT_WATCHDOG_CONFIGURATIONr, conf_reg, CR_WD_MAX_FLOW_MSG_GEN_RATEf);
              if (reg_val <= CREDIT_WATCHDOG_FAST_STATUS_MESSAGE_MODE) {
                  SET_CREDIT_WATCHDOG_MODE(unit,
                    reg_val == CREDIT_WATCHDOG_UNINITIALIZED ? /* If the chip was not configured, use default mode for chip */
                      (SOC_IS_ARADPLUS(unit) ?  CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE : CREDIT_WATCHDOG_NORMAL_MODE) :
                      reg_val); /* else use the mode previously configured, stored at CR_WD_MAX_FLOW_MSG_GEN_RATEf */
                  /* configure the default scan time for the mode */
                  SOC_SAND_CHECK_FUNC_RESULT(arad_itm_get_set_cr_watch_dog_scan_calc_unsafe(unit, 0, 0, 0), 3300, exit);
                  if (!SOC_WARM_BOOT(unit)) {  /* set the default (full) queue range in cold boot */
                      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  3310,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_CR_WD_BOTTOM_Qr, SOC_CORE_ALL, 0, CR_WD_BOTTOM_Qf,  0));
                      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  3320,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_CR_WD_TOP_Qr, SOC_CORE_ALL, 0, CR_WD_TOP_Qf,  SOC_DPP_DEFS_GET(unit, max_queues)-1));
                  }
              } else if (reg_val >= CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE && SOC_IS_ARADPLUS(unit) &&
                         (reg_val -= CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE) <= (ARAD_CREDIT_WATCHDOG_COMMON_SCAN_TIME_MAX_HALVES + 1)) {
                  if (reg_val > ARAD_CREDIT_WATCHDOG_COMMON_SCAN_TIME_MAX_HALVES) {
                      SET_CREDIT_WATCHDOG_MODE(unit, CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE + 1);
                      SOC_DPP_CONFIG(unit)->arad->exact_credit_watchdog_scan_time_nano = ARAD_CREDIT_WATCHDOG_COMMON_MAX_SCAN_TIME_NS;
                  } else {
                      SET_CREDIT_WATCHDOG_MODE(unit, CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE);
                      SOC_DPP_CONFIG(unit)->arad->exact_credit_watchdog_scan_time_nano = ARAD_CREDIT_WATCHDOG_COMMON_MIN_SCAN_TIME_NS << reg_val;
                  }
              } else {
                  SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 3340, exit); /* invalid value */
              }
          }

      } else {
          SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 3990, exit);
      }

      goto exit;
  }

  SOC_SAND_CHECK_NULL_INPUT(info);

  /* set wd queue boundaries { */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_CR_WD_BOTTOM_Qr, SOC_CORE_ALL, 0, CR_WD_BOTTOM_Qf,  info->bottom_queue));
  exact_info->bottom_queue = info->bottom_queue;

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_CR_WD_TOP_Qr, SOC_CORE_ALL, 0, CR_WD_TOP_Qf,  info->top_queue));
  exact_info->top_queue = info->top_queue;
  /* set wd queue boundaries } */

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 30, exit, ARAD_REG_ACCESS_ERR,
    READ_IPS_CREDIT_WATCHDOG_CONFIGURATIONr(unit, core_id, &conf_reg64)); /* read 32/64 bit value */
  conf_reg = COMPILER_64_LO(conf_reg64);
  /* set wd config { */

  if (SOC_IS_ARADPLUS(unit)) {
      if (IS_CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE(unit)) {
          /* In this mode the given scan time also gives the number of scans for a message (0/1/2) */
          uint32 hw_mode;
          switch (scan_time_us) {
            case 0:
              hw_mode = 0;
              break;
            case ARAD_CREDIT_WATCHDOG_COMMON_MIN_SCAN_TIME_NS     / 1000: /*  125000 */
            case 2 * ARAD_CREDIT_WATCHDOG_COMMON_MIN_SCAN_TIME_NS / 1000: /*  250000 */
            case 4 * ARAD_CREDIT_WATCHDOG_COMMON_MIN_SCAN_TIME_NS / 1000: /*  500000 */
            case 8 * ARAD_CREDIT_WATCHDOG_COMMON_MIN_SCAN_TIME_NS / 1000: /* 1000000 */
            case ARAD_CREDIT_WATCHDOG_COMMON_MAX_SCAN_TIME_NS     / 1000: /* 2000000 */
              hw_mode = SOC_DPP_CONFIG(unit)->arad->credit_watchdog_mode == CREDIT_WATCHDOG_COMMON_STATUS_MESSAGE_MODE + 1 ? 2 : 3;
              break;
            default:
              SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 32, exit); /* unsupported common FSM time */
          }
          exact_info->min_scan_cycle_period_micro = scan_time_us;
          soc_reg64_field32_set(unit, IPS_CREDIT_WATCHDOG_CONFIGURATIONr, &conf_reg64, CR_WD_FSM_MODEf, hw_mode);
          soc_reg64_field32_set(unit, IPS_CREDIT_WATCHDOG_CONFIGURATIONr, &conf_reg64, CR_WD_DELETE_Q_EXP_MODEf, 1);
      } else {
          soc_reg64_field32_set(unit, IPS_CREDIT_WATCHDOG_CONFIGURATIONr, &conf_reg64, CR_WD_FSM_MODEf, 1);
          soc_reg64_field32_set(unit, IPS_CREDIT_WATCHDOG_CONFIGURATIONr, &conf_reg64, CR_WD_DELETE_Q_EXP_MODEf, 0);
      }
  }

  /* set time to pass for queue to queue in the scan */
  if (!scan_time_us) { /* if watchdog is disabled, CR_WD_MAX_FLOW_MSG_GEN_RATEf stored the watchdog mode */
    exact_info->max_flow_msg_gen_rate_nano = reg_val = info->max_flow_msg_gen_rate_nano;
  } else if (info->max_flow_msg_gen_rate_nano) { /* if enabled */
    res = arad_chip_time_to_ticks( unit, info->max_flow_msg_gen_rate_nano, TRUE, 1, TRUE, &reg_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit); /* calculate field value */

    res = arad_ticks_to_time( unit, reg_val, TRUE, 1, &(exact_info->max_flow_msg_gen_rate_nano));
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit); /* calculate exact value from field */

    SOC_SAND_LIMIT_FROM_BELOW(reg_val, ARAD_CREDIT_WATCHDOG_MIN_Q_SCAN_CYCLES);
    SOC_SAND_LIMIT_FROM_ABOVE(reg_val, ARAD_CREDIT_WATCHDOG_MAX_Q_SCAN_CYCLES);
  } else {
    exact_info->max_flow_msg_gen_rate_nano = reg_val = 0;
  }

  soc_reg_field_set(unit, IPS_CREDIT_WATCHDOG_CONFIGURATIONr, &conf_reg, CR_WD_MAX_FLOW_MSG_GEN_RATEf, reg_val);

  /* set minimum time for a whole scan */
  if (scan_time_us) { /* if enabled */
    res = arad_itm_get_set_cr_watch_dog_scan_calc_unsafe(unit, scan_time_us, 0, &reg_val);
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    exact_info->min_scan_cycle_period_micro = SOC_DPP_CONFIG(unit)->arad->exact_credit_watchdog_scan_time_nano / 1000;
  } else {
    exact_info->min_scan_cycle_period_micro = reg_val = 0;
  }

  soc_reg_field_set(unit, IPS_CREDIT_WATCHDOG_CONFIGURATIONr, &conf_reg, CR_WD_MIN_SCAN_CYCLE_PERIODf, reg_val);
  COMPILER_64_SET(conf_reg64, COMPILER_64_HI(conf_reg64), conf_reg);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 60, exit, ARAD_REG_ACCESS_ERR,
    WRITE_IPS_CREDIT_WATCHDOG_CONFIGURATIONr(unit, core_id, conf_reg64));
  /* set wd config } */
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_cr_wd_set_unsafe()",0,0);
}

/*********************************************************************
*     Sets ingress-queue credit Watchdog thresholds and
*     configuration. includes: start-queue, end-queue and
*     wd-rates.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_cr_wd_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_CR_WD_INFO      *info
  )
{
   
    

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_CR_WD_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  if (info->bottom_queue != ARAD_CREDIT_WATCHDOG_Q_COMMON_FSM_ADJUST_EXP || 
      info->top_queue != ARAD_CREDIT_WATCHDOG_Q_COMMON_FSM_ADJUST_EXP) {
      if (info->bottom_queue > SOC_DPP_DEFS_GET(unit, nof_queues))
      {
          SOC_SAND_SET_ERROR_CODE(ARAD_QUEUE_ID_OUT_OF_RANGE_ERR, 10, exit);
      }

      if (info->top_queue > SOC_DPP_DEFS_GET(unit, nof_queues))
      {
          SOC_SAND_SET_ERROR_CODE(ARAD_QUEUE_ID_OUT_OF_RANGE_ERR, 15, exit);
      }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_cr_wd_verify()",0,0);
}

/*********************************************************************
*     Sets ingress-queue credit Watchdog thresholds and
*     configuration. includes: start-queue, end-queue and
*     wd-rates.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_cr_wd_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ITM_CR_WD_INFO      *info
  )
{
  uint32
    res,
    reg_val = 0;
  uint64
    reg_val64;
    

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_CR_WD_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_SET;

  /* get wd queue boundaries { */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPS_CR_WD_BOTTOM_Qr, SOC_CORE_ALL, 0, CR_WD_BOTTOM_Qf, &info->bottom_queue));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  15, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPS_CR_WD_TOP_Qr, SOC_CORE_ALL, 0, CR_WD_TOP_Qf, &info->top_queue));
 /*
  info->bottom_queue = (uint32)reg_val;
  info->top_queue = (uint32)reg_val;*/

  /* get wd queue boundaries } */

  /* get wd config { */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 20, exit, ARAD_REG_ACCESS_ERR,
    READ_IPS_CREDIT_WATCHDOG_CONFIGURATIONr(unit, REG_PORT_ANY, &reg_val64));
  reg_val = COMPILER_64_LO(reg_val64);
  res =
    arad_ticks_to_time(
      unit,
      soc_reg_field_get(unit, IPS_CREDIT_WATCHDOG_CONFIGURATIONr, reg_val, CR_WD_MAX_FLOW_MSG_GEN_RATEf),
      TRUE,
      1,
      &(info->max_flow_msg_gen_rate_nano)
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

  res =
    arad_ticks_to_time(
      unit,
      soc_reg_field_get(unit, IPS_CREDIT_WATCHDOG_CONFIGURATIONr, reg_val, CR_WD_MIN_SCAN_CYCLE_PERIODf),
      FALSE,
      ARAD_ITM_WD_INFO_MIN_SCAN_CYCLE_PERIOD_MICRO_GRANULARITY,
      &(info->min_scan_cycle_period_micro)
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 28, exit);

  /* get wd config } */
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_cr_wd_get_unsafe()", unit, 0);
}

/*********************************************************************
*     Each Virtual Statistics Queue has a VSQ-Rate-Class.
*     This function assigns a VSQ with its Rate Class.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_vsq_qt_rt_cls_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  ARAD_ITM_VSQ_NDX         vsq_in_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_QT_RT_CLS_SET_UNSAFE);

  switch(vsq_group_ndx)
  {
  case ARAD_ITM_VSQ_GROUP_CTGRY:
      res = arad_itm_vsq_group_a_set_rt_class(
              unit,
              vsq_rt_cls,
              vsq_in_group_ndx
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    break;
  case ARAD_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS:
      res = arad_itm_vsq_group_b_set_rt_class(
              unit,
              vsq_rt_cls,
              vsq_in_group_ndx
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    break;
  case ARAD_ITM_VSQ_GROUP_CTGRY_2_3_CNCTN_CLS:
      res = arad_itm_vsq_group_c_set_rt_class(
              unit,
              vsq_rt_cls,
              vsq_in_group_ndx
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
      break;
  case ARAD_ITM_VSQ_GROUP_STTSTCS_TAG:
      res = arad_itm_vsq_group_d_set_rt_class(
              unit,
              vsq_rt_cls,
              vsq_in_group_ndx
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
      break;
  case ARAD_ITM_VSQ_GROUP_LLFC:
    res = arad_itm_vsq_group_e_set_rt_class(
      unit,
      vsq_rt_cls,
      vsq_in_group_ndx
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    break;
  case ARAD_ITM_VSQ_GROUP_PFC:
    res = arad_itm_vsq_group_f_set_rt_class(
      unit,
      vsq_rt_cls,
      vsq_in_group_ndx
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    break;
  default:
      break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_qt_rt_cls_set_unsafe()",0,0);
}

/*********************************************************************
*     Sets Virtual Statistics Queue Rate-Class.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_vsq_qt_rt_cls_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  ARAD_ITM_VSQ_NDX         vsq_in_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_QT_RT_CLS_VERIFY);

  res = arad_itm_vsq_idx_verify(
          vsq_group_ndx,
          vsq_in_group_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (vsq_rt_cls > (ARAD_ITM_VSQ_QT_RT_CLS_MAX))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_VSQ_QT_RT_OUT_OF_RANGE_ERR, 40, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_qt_rt_cls_verify()",0,0);
}

/*********************************************************************
*     Sets Virtual Statistics Queue Rate-Class.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_vsq_qt_rt_cls_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  ARAD_ITM_VSQ_NDX         vsq_in_group_ndx,
    SOC_SAND_OUT uint32                 *vsq_rt_cls
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_QT_RT_CLS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(vsq_rt_cls);

  res = arad_itm_vsq_idx_verify(
          vsq_group_ndx,
          vsq_in_group_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  switch(vsq_group_ndx)
  {
    case ARAD_ITM_VSQ_GROUP_CTGRY:
      res = arad_itm_vsq_group_a_get_rt_class(
              unit,
              vsq_in_group_ndx,
              vsq_rt_cls
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
      break;
    case ARAD_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS:
      res = arad_itm_vsq_group_b_get_rt_class(
              unit,
              vsq_in_group_ndx,
              vsq_rt_cls
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);
      break;
    case ARAD_ITM_VSQ_GROUP_CTGRY_2_3_CNCTN_CLS:
      res = arad_itm_vsq_group_c_get_rt_class(
              unit,
              vsq_in_group_ndx,
              vsq_rt_cls
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);
      break;
    case ARAD_ITM_VSQ_GROUP_STTSTCS_TAG:
      res = arad_itm_vsq_group_d_get_rt_class(
              unit,
              vsq_in_group_ndx,
              vsq_rt_cls
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 18, exit);
      break;
    case ARAD_ITM_VSQ_GROUP_LLFC:
      res = arad_itm_vsq_group_e_get_rt_class(
              unit,
              vsq_in_group_ndx,
              vsq_rt_cls
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
      break;
  case ARAD_ITM_VSQ_GROUP_PFC:
    res = arad_itm_vsq_group_f_get_rt_class(
            unit,
            vsq_in_group_ndx,
            vsq_rt_cls
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);
    break;
  default:
    break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_qt_rt_cls_get_unsafe()",0,0);
}

/*********************************************************************
*     Sets Virtual Statistics Queue, includes: vsq-id,
*     rate-class
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_vsq_fc_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  ARAD_ITM_VSQ_FC_INFO     *info,
    SOC_SAND_OUT ARAD_ITM_VSQ_FC_INFO     *exact_info
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_FC_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);


  res = arad_itm_vsq_group_set_fc_info(
          unit,
          vsq_group_ndx,
          vsq_rt_cls_ndx,
          info,
          exact_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,1,exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_fc_set_unsafe()",0,0);
}

/*********************************************************************
*     Sets Virtual Statistics Queue, includes: vsq-id,
*     rate-class
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_vsq_fc_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  ARAD_ITM_VSQ_FC_INFO     *info
  )
{
  uint32
    max_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_FC_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_group_ndx, ARAD_ITM_VSQ_GROUP_LAST-1,
    ARAD_ITM_VSQ_GROUP_ID_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_rt_cls_ndx, ARAD_ITM_VSQ_QT_RT_CLS_MAX,
    ARAD_ITM_VSQ_QT_RT_OUT_OF_RANGE_ERR, 7, exit
  );

  max_val = ARAD_ITM_VSQ_FC_BD_SIZE_MAX;
  if (info->bd_size_fc.clear > max_val)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_VSQ_FC_PARAMETER_OUT_OF_RANGE_ERR, 50, exit);
  }

  if (info->bd_size_fc.set > max_val)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_VSQ_FC_PARAMETER_OUT_OF_RANGE_ERR, 60, exit);
  }

  max_val = ARAD_ITM_VSQ_FC_Q_SIZE_MAX;
  if (info->q_size_fc.clear > max_val)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_VSQ_FC_PARAMETER_OUT_OF_RANGE_ERR, 70, exit);
  }

  if (info->q_size_fc.set > max_val)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_VSQ_FC_PARAMETER_OUT_OF_RANGE_ERR, 80, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_fc_verify()",0,0);
}

/*********************************************************************
*     Sets Virtual Statistics Queue, includes: vsq-id,
*     rate-class
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_vsq_fc_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_OUT ARAD_ITM_VSQ_FC_INFO     *info
  )
{
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_FC_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_group_ndx, (ARAD_ITM_VSQ_GROUP_LAST-1),
    ARAD_ITM_VSQ_GROUP_ID_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_rt_cls_ndx, ARAD_ITM_VSQ_QT_RT_CLS_MAX,
    ARAD_ITM_VSQ_QT_RT_OUT_OF_RANGE_ERR, 7, exit
  );

  res = arad_itm_vsq_group_get_fc_info(
          unit,
          vsq_group_ndx,
          vsq_rt_cls_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,1,exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_fc_get_unsafe()",0,0);
}

/*********************************************************************
*     Sets tail drop parameter on the VSQ - max-queue-size in
*     words and in buffer-descriptors per vsq-rate-class and
*     drop precedence. The tail drop mechanism drops packets
*     that are mapped to queues that exceed thresholds of this
*     structure.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_vsq_tail_drop_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  ARAD_ITM_VSQ_TAIL_DROP_INFO  *info,
    SOC_SAND_OUT  ARAD_ITM_VSQ_TAIL_DROP_INFO  *exact_info
  )
{
  uint32
    res,
    max_inst_q_siz_words_mnt_nof_bits,
    max_inst_q_siz_words_exp_nof_bits,
    max_inst_q_siz_bds_mnt_nof_bits,
    max_inst_q_siz_bds_exp_nof_bits;
  ARAD_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA
    iqm_vsq_queue_parameters_table_group_tbl_data;
  uint32
    vq_max_size_words_mnt,
    vq_max_size_words_exp,
    vq_max_szie_bds_mnt,
    vq_max_szie_bds_exp;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_TAIL_DROP_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

 
 

  res = arad_iqm_vsq_queue_parameters_table_group_tbl_get_unsafe(
          unit,
          vsq_group_ndx,
          vsq_rt_cls_ndx,
          drop_precedence_ndx,
          &iqm_vsq_queue_parameters_table_group_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  max_inst_q_siz_words_mnt_nof_bits = 7;
  max_inst_q_siz_words_exp_nof_bits = soc_mem_field_length(unit, IQM_VQPR_MAm, VQ_MAX_SIZE_WORDSf) - max_inst_q_siz_words_mnt_nof_bits;

    res = soc_sand_break_to_mnt_exp_round_up(
            SOC_SAND_DIV_ROUND_UP(info->max_inst_q_size,ARAD_ITM_DROP_TAIL_SIZE_RESOLUTION),
            max_inst_q_siz_words_mnt_nof_bits,
            max_inst_q_siz_words_exp_nof_bits,
            0,
            &(vq_max_size_words_mnt),
            &(vq_max_size_words_exp)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    iqm_vsq_queue_parameters_table_group_tbl_data.vq_max_size_words_mnt = vq_max_size_words_mnt;
    iqm_vsq_queue_parameters_table_group_tbl_data.vq_max_size_words_exp = vq_max_size_words_exp;

    max_inst_q_siz_bds_mnt_nof_bits = 7;
    max_inst_q_siz_bds_exp_nof_bits = soc_mem_field_length(unit, IQM_VQPR_MAm, VQ_MAX_SIZE_BDSf) - max_inst_q_siz_bds_mnt_nof_bits;

    res = soc_sand_break_to_mnt_exp_round_up(
            info->max_inst_q_size_bds,
            max_inst_q_siz_bds_mnt_nof_bits,
            max_inst_q_siz_bds_exp_nof_bits,
            0,
            &(vq_max_szie_bds_mnt),
            &(vq_max_szie_bds_exp)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    iqm_vsq_queue_parameters_table_group_tbl_data.vq_max_szie_bds_mnt = vq_max_szie_bds_mnt;
    iqm_vsq_queue_parameters_table_group_tbl_data.vq_max_szie_bds_exp = vq_max_szie_bds_exp;

    if (iqm_vsq_queue_parameters_table_group_tbl_data.vq_max_size_words_mnt == 0)
    {
      exact_info->max_inst_q_size = 0;
    }
    else
    {
      exact_info->max_inst_q_size =
        ((iqm_vsq_queue_parameters_table_group_tbl_data.vq_max_size_words_mnt) *
        (1<<(iqm_vsq_queue_parameters_table_group_tbl_data.vq_max_size_words_exp)));
      exact_info->max_inst_q_size *= ARAD_ITM_DROP_TAIL_SIZE_RESOLUTION;
    }

    if(iqm_vsq_queue_parameters_table_group_tbl_data.vq_max_szie_bds_mnt == 0)
    {
      exact_info->max_inst_q_size_bds = 0;
    }
    else
    {
      exact_info->max_inst_q_size_bds =
        ((iqm_vsq_queue_parameters_table_group_tbl_data.vq_max_szie_bds_mnt) *
        (1<<(iqm_vsq_queue_parameters_table_group_tbl_data.vq_max_szie_bds_exp)));
    }

    res = arad_iqm_vsq_queue_parameters_table_group_tbl_set_unsafe(
            unit,
            vsq_group_ndx,
            vsq_rt_cls_ndx,
            drop_precedence_ndx,
            &iqm_vsq_queue_parameters_table_group_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_tail_drop_set_unsafe()",0,0);
}

/*********************************************************************
*     Sets tail drop parameter on the VSQ - max-queue-size in
*     words and in buffer-descriptors per vsq-rate-class and
*     drop precedence. The tail drop mechanism drops packets
*     that are mapped to queues that exceed thresholds of this
*     structure.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_vsq_tail_drop_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP           vsq_group_ndx,
    SOC_SAND_IN  uint32                     vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                     drop_precedence_ndx,
    SOC_SAND_IN  ARAD_ITM_VSQ_TAIL_DROP_INFO  *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_TAIL_DROP_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_group_ndx, (ARAD_ITM_VSQ_GROUP_LAST-1),
    ARAD_ITM_VSQ_GROUP_ID_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_rt_cls_ndx, ARAD_ITM_VSQ_QT_RT_CLS_MAX,
    ARAD_ITM_VSQ_QT_RT_OUT_OF_RANGE_ERR, 7, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    drop_precedence_ndx, ARAD_MAX_DROP_PRECEDENCE,
    ARAD_DRPP_OUT_OF_RANGE_ERR, 9, exit
  );

  /* Every 32 bit value is valid for max_inst_q_size */

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->max_inst_q_size_bds,ARAD_ITM_MAX_INST_Q_BDS_SIZE,
    ARAD_ITM_VSQ_MAX_INST_Q_SIZ_PARAMETER_OUT_OF_RANGE_ERR, 50, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_tail_drop_verify()",0,0);
}

/*********************************************************************
*     Sets tail drop parameter on the VSQ - max-queue-size in
*     words and in buffer-descriptors per vsq-rate-class and
*     drop precedence. The tail drop mechanism drops packets
*     that are mapped to queues that exceed thresholds of this
*     structure.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_vsq_tail_drop_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_OUT ARAD_ITM_VSQ_TAIL_DROP_INFO  *info
  )
{
  uint32
    res;
  ARAD_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA
    iqm_vsq_queue_parameters_table_group_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_TAIL_DROP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_group_ndx, (ARAD_ITM_VSQ_GROUP_LAST-1),
    ARAD_ITM_VSQ_GROUP_ID_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_rt_cls_ndx, ARAD_ITM_VSQ_QT_RT_CLS_MAX,
    ARAD_ITM_VSQ_QT_RT_OUT_OF_RANGE_ERR, 7, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    drop_precedence_ndx, ARAD_MAX_DROP_PRECEDENCE,
    ARAD_DRPP_OUT_OF_RANGE_ERR, 9, exit
  );

 
 

  res = arad_iqm_vsq_queue_parameters_table_group_tbl_get_unsafe(
          unit,
          vsq_group_ndx,
          vsq_rt_cls_ndx,
          drop_precedence_ndx,
          &iqm_vsq_queue_parameters_table_group_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


    if (iqm_vsq_queue_parameters_table_group_tbl_data.vq_max_size_words_mnt == 0)
    {
      info->max_inst_q_size = 0;
    }
    else
    {
      info->max_inst_q_size =
        ((iqm_vsq_queue_parameters_table_group_tbl_data.vq_max_size_words_mnt) *
        (1<<(iqm_vsq_queue_parameters_table_group_tbl_data.vq_max_size_words_exp)));
      info->max_inst_q_size *= ARAD_ITM_DROP_TAIL_SIZE_RESOLUTION;
    }

    if(iqm_vsq_queue_parameters_table_group_tbl_data.vq_max_szie_bds_mnt == 0)
    {
      info->max_inst_q_size_bds = 0;
    }
    else
    {
      info->max_inst_q_size_bds =
        ((iqm_vsq_queue_parameters_table_group_tbl_data.vq_max_szie_bds_mnt) *
        (1<<(iqm_vsq_queue_parameters_table_group_tbl_data.vq_max_szie_bds_exp)));
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_tail_drop_get_unsafe()",0,0);
}

/*********************************************************************
*     Get tail drop default parameters on the VSQ - max-queue-size in
*     words and in buffer-descriptors per vsq-rate-class.
*     The tail drop mechanism drops packets
*     that are mapped to queues that exceed thresholds of this
*     structure.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_vsq_tail_drop_get_default_unsafe(
      SOC_SAND_IN  int                 unit,
      SOC_SAND_OUT ARAD_ITM_VSQ_TAIL_DROP_INFO  *info
  )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_TAIL_DROP_DEFAULT_GET_UNSAFE);
    SOC_SAND_CHECK_NULL_INPUT(info);

    
    info->max_inst_q_size = (0x1 << soc_mem_field_length(unit,IQM_VQPR_MAm,SET_THRESHOLD_WORDSf)) - 1;
    info->max_inst_q_size_bds = (0x1 << soc_mem_field_length(unit,IQM_VQPR_MAm,CLEAR_THRESHOLD_BDf)) - 1;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_tail_drop_get_unsafe()",0,0);
}

/*********************************************************************
*     This procedure sets VSQ WRED general configurations,
*     includes: WRED-enable and exponential-weight-queue (for
*     the WRED algorithm).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_vsq_wred_gen_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  ARAD_ITM_VSQ_WRED_GEN_INFO *info
  )
{
  uint32
    res;
  ARAD_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_DATA
    iqm_vsq_flow_control_parameters_table_group_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_WRED_GEN_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = arad_iqm_vsq_flow_control_parameters_table_group_tbl_get_unsafe(
          unit,
          vsq_group_ndx,
          vsq_rt_cls_ndx,
          &iqm_vsq_flow_control_parameters_table_group_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  iqm_vsq_flow_control_parameters_table_group_tbl_data.red_weight_q =
    info->exp_wq;
  iqm_vsq_flow_control_parameters_table_group_tbl_data.wred_en =
    info->wred_en;

  res = arad_iqm_vsq_flow_control_parameters_table_group_tbl_set_unsafe(
          unit,
          vsq_group_ndx,
          vsq_rt_cls_ndx,
          &iqm_vsq_flow_control_parameters_table_group_tbl_data
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_wred_gen_set_unsafe()",0,0);
}

/*********************************************************************
*     This procedure sets VSQ WRED general configurations,
*     includes: WRED-enable and exponential-weight-queue (for
*     the WRED algorithm).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_vsq_wred_gen_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  ARAD_ITM_VSQ_WRED_GEN_INFO *info
  )
{
  uint32
    max_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_WRED_GEN_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

 
 

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_group_ndx, (ARAD_ITM_VSQ_GROUP_LAST-1),
    ARAD_ITM_VSQ_GROUP_ID_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_rt_cls_ndx, ARAD_ITM_VSQ_QT_RT_CLS_MAX,
    ARAD_ITM_VSQ_QT_RT_OUT_OF_RANGE_ERR, 7, exit
  );

  max_val = soc_mem_field_length(unit, IQM_VQFCPR_MAm, RED_WEIGHT_Qf);
  if (info->exp_wq > max_val)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_VSQ_WRED_EXP_WT_PARAMETER_OUT_OF_RANGE_ERR, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_wred_gen_verify()",0,0);
}

/*********************************************************************
*     This procedure sets VSQ WRED general configurations,
*     includes: WRED-enable and exponential-weight-queue (for
*     the WRED algorithm).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_vsq_wred_gen_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_OUT ARAD_ITM_VSQ_WRED_GEN_INFO *info
  )
{
  uint32
    res;
  ARAD_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_DATA
    iqm_vsq_flow_control_parameters_table_group_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_WRED_GEN_GET_UNSAFE);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_group_ndx, (ARAD_ITM_VSQ_GROUP_LAST-1),
    ARAD_ITM_VSQ_GROUP_ID_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_rt_cls_ndx, ARAD_ITM_VSQ_QT_RT_CLS_MAX,
    ARAD_ITM_VSQ_QT_RT_OUT_OF_RANGE_ERR, 7, exit
  );

  res = arad_iqm_vsq_flow_control_parameters_table_group_tbl_get_unsafe(
          unit,
          vsq_group_ndx,
          vsq_rt_cls_ndx,
          &iqm_vsq_flow_control_parameters_table_group_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  info->exp_wq =
    iqm_vsq_flow_control_parameters_table_group_tbl_data.red_weight_q;
  info->wred_en =
    (uint8)iqm_vsq_flow_control_parameters_table_group_tbl_data.wred_en;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_wred_gen_get_unsafe()",0,0);
}

/*********************************************************************
*     A WRED test for each packet versus the packet queue or
*     VSQ that the packet is mapped to is performed. This
*     procedure sets Virtual Statistics Queue WRED, includes:
*     WRED-thresholds/probability.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_vsq_wred_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  ARAD_ITM_WRED_QT_DP_INFO *info,
    SOC_SAND_OUT ARAD_ITM_WRED_QT_DP_INFO *exact_info
  )
{
  uint32
    res;
  ARAD_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA
    iqm_vsq_queue_parameters_table_group_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_WRED_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  res = arad_iqm_vsq_queue_parameters_table_group_tbl_get_unsafe(
          unit,
          vsq_group_ndx,
          vsq_rt_cls_ndx,
          drop_precedence_ndx,
          &iqm_vsq_queue_parameters_table_group_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = arad_itm_vsq_WRED_QT_DP_INFO_to_WRED_TBL_DATA(
          info,
          &iqm_vsq_queue_parameters_table_group_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = arad_itm_vsq_WRED_TBL_DATA_to_WRED_QT_DP_INFO(
          &iqm_vsq_queue_parameters_table_group_tbl_data,
          exact_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = arad_iqm_vsq_queue_parameters_table_group_tbl_set_unsafe(
          unit,
          vsq_group_ndx,
          vsq_rt_cls_ndx,
          drop_precedence_ndx,
          &iqm_vsq_queue_parameters_table_group_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_wred_set_unsafe()",0,0);
}

/*********************************************************************
*     A WRED test for each packet versus the packet queue or
*     VSQ that the packet is mapped to is performed. This
*     procedure sets Virtual Statistics Queue WRED, includes:
*     WRED-thresholds/probability.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_vsq_wred_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  ARAD_ITM_WRED_QT_DP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_WRED_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_group_ndx, (ARAD_ITM_VSQ_GROUP_LAST-1),
    ARAD_ITM_VSQ_GROUP_ID_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_rt_cls_ndx, ARAD_ITM_VSQ_QT_RT_CLS_MAX,
    ARAD_ITM_VSQ_QT_RT_OUT_OF_RANGE_ERR, 7, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    drop_precedence_ndx, ARAD_MAX_DROP_PRECEDENCE,
    ARAD_DRPP_OUT_OF_RANGE_ERR, 9, exit
  );

  if (info->min_avrg_th > info->max_avrg_th)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_VSQ_WRED_MIN_TH_HIGHER_THEN_MAX_TH_ERR, 60, exit);
  }

  /* Not checking info->min_avrg_th and info->max_avrg_th since the limit is 4GB like the range of the integer */

  if (info->max_probability > ARAD_ITM_WRED_MAX_PROB)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_WRED_PROB_OUT_OF_RANGE_ERR, 90, exit);
  }

  if (info->max_packet_size > ARAD_ITM_WRED_MAX_PACKET_SIZE)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_WRED_MAX_PACKET_SIZE_OUT_OF_RANGE_ERR, 100, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_wred_verify()",0,0);
}

/*********************************************************************
*     A WRED test for each packet versus the packet queue or
*     VSQ that the packet is mapped to is performed. This
*     procedure sets Virtual Statistics Queue WRED, includes:
*     WRED-thresholds/probability.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_vsq_wred_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_OUT ARAD_ITM_WRED_QT_DP_INFO *info
  )
{
  uint32
    res;
  ARAD_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA
    iqm_vsq_queue_parameters_table_group_tbl_data;
  ARAD_ITM_VSQ_WRED_GEN_INFO
    wred_gen;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_WRED_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  arad_ARAD_ITM_VSQ_WRED_GEN_INFO_clear(&wred_gen);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_group_ndx, (ARAD_ITM_VSQ_GROUP_LAST-1),
    ARAD_ITM_VSQ_GROUP_ID_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_rt_cls_ndx, ARAD_ITM_VSQ_QT_RT_CLS_MAX,
    ARAD_ITM_VSQ_QT_RT_OUT_OF_RANGE_ERR, 7, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    drop_precedence_ndx, ARAD_MAX_DROP_PRECEDENCE,
    ARAD_DRPP_OUT_OF_RANGE_ERR, 9, exit
  );

  res = arad_iqm_vsq_queue_parameters_table_group_tbl_get_unsafe(
          unit,
          vsq_group_ndx,
          vsq_rt_cls_ndx,
          drop_precedence_ndx,
          &iqm_vsq_queue_parameters_table_group_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = arad_itm_vsq_WRED_TBL_DATA_to_WRED_QT_DP_INFO(
          &iqm_vsq_queue_parameters_table_group_tbl_data,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  /*
   *  Get the WRED enable indication from the VSQ WRED generation API
   *  (not per DP)
   */

  res = arad_itm_vsq_wred_gen_get_unsafe(
          unit,
          vsq_group_ndx,
          vsq_rt_cls_ndx,
          &wred_gen
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  info->wred_en = wred_gen.wred_en;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_wred_get_unsafe()",0,0);
}

/*********************************************************************
*     Select VSQ for monitoring. The selected VSQ counter can
*     be further read, indicating the number of packets
*     enqueued to the VSQ.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  arad_itm_vsq_counter_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  ARAD_ITM_VSQ_NDX         vsq_in_group_ndx
  )
{
  uint32
    res,
    fld_val,
    reg_val;
   
    

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_COUNTER_SET_UNSAFE);

  

  reg_val = 0;
  fld_val = vsq_in_group_ndx;
  ARAD_FLD_TO_REG(IQM_VSQ_PROGRAMMABLE_COUNTER_SELECTr, VSQ_PRG_CNT_Qf, fld_val, reg_val, 20, exit);
  fld_val = 0x0;
  ARAD_FLD_TO_REG(IQM_VSQ_PROGRAMMABLE_COUNTER_SELECTr, VSQ_PRG_CNT_MSKf, fld_val, reg_val, 22, exit);
  fld_val = vsq_group_ndx;
  ARAD_FLD_TO_REG(IQM_VSQ_PROGRAMMABLE_COUNTER_SELECTr, VSQ_PRG_GRP_SELf, fld_val, reg_val, 24, exit);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,WRITE_IQM_VSQ_PROGRAMMABLE_COUNTER_SELECTr(unit, SOC_CORE_ALL,  reg_val));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_count_get_unsafe()",0,0);
}

/*********************************************************************
*     Select VSQ for monitoring. The selected VSQ counter can
*     be further read, indicating the number of packets
*     enqueued to the VSQ.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  arad_itm_vsq_counter_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  ARAD_ITM_VSQ_NDX         vsq_in_group_ndx
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_COUNTER_VERIFY);

   res = arad_itm_vsq_idx_verify(
          vsq_group_ndx,
          vsq_in_group_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_counter_verify()",0,0);
}

/*********************************************************************
*     Select VSQ for monitoring. The selected VSQ counter can
*     be further read, indicating the number of packets
*     enqueued to the VSQ.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  arad_itm_vsq_counter_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ITM_VSQ_GROUP       *vsq_group_ndx,
    SOC_SAND_OUT ARAD_ITM_VSQ_NDX         *vsq_in_group_ndx
  )
{
  uint32
    res,
    fld_val = 0,
    reg_val;
   
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_COUNTER_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(vsq_group_ndx);
  SOC_SAND_CHECK_NULL_INPUT(vsq_in_group_ndx);

  

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,READ_IQM_VSQ_PROGRAMMABLE_COUNTER_SELECTr(unit, SOC_CORE_ALL, &reg_val));

  ARAD_FLD_FROM_REG(IQM_VSQ_PROGRAMMABLE_COUNTER_SELECTr, VSQ_PRG_CNT_Qf, fld_val, reg_val, 20, exit);
  *vsq_in_group_ndx = fld_val;

  ARAD_FLD_FROM_REG(IQM_VSQ_PROGRAMMABLE_COUNTER_SELECTr, VSQ_PRG_GRP_SELf, fld_val, reg_val, 24, exit);
  *vsq_group_ndx = fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_counter_get_unsafe()",0,0);
}

/*********************************************************************
*     Indicates the number of packets enqueued to the
*     monitored VSQ.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  arad_itm_vsq_counter_read_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint32                  *pckt_count
  )
{
  uint32
    res,
    fld_val = 0,
    reg_val = 0,
    cntr_val = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_VSQ_COUNTER_READ_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(pckt_count);

  

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,READ_IQM_VSQ_ENQUEUE_PACKET_COUNTERr(unit, REG_PORT_ANY, &reg_val));
  ARAD_FLD_FROM_REG(IQM_VSQ_ENQUEUE_PACKET_COUNTERr, VSQ_ENQ_PKT_CNT_OVFf, fld_val, reg_val, 12, exit);

  if (fld_val != 0x0)
  {
    cntr_val = 0xFFFFFFFF;
  }
  else
  {
    ARAD_FLD_FROM_REG(IQM_VSQ_ENQUEUE_PACKET_COUNTERr, VSQ_ENQ_PKT_CNTf, cntr_val, reg_val, 14, exit);
  }

  *pckt_count = cntr_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_vsq_counter_read_unsafe()",0,0);
}


/*********************************************************************
*     Sets the queue types of a queue
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_queue_info_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_ndx,
    SOC_SAND_IN  ARAD_ITM_QUEUE_INFO      *info
  )
{
  uint32
    res;
  ARAD_IQM_STATIC_TBL_DATA
    iqm_static_tbl_data;
  ARAD_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_DATA
    ips_queue_type_lookup_table_tbl_data;
  uint8
    hw_q_type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_QUEUE_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  iqm_static_tbl_data.traffic_class =
    info->vsq_traffic_cls;
  iqm_static_tbl_data.credit_class =
    info->credit_cls;
  iqm_static_tbl_data.connection_class =
    info->vsq_connection_cls;
  iqm_static_tbl_data.rate_class =
    info->rate_cls;
  iqm_static_tbl_data.que_signature =
    info->signature;

  /* Get the hardware queue type (credit request profile) */
  SOC_SAND_CHECK_FUNC_RESULT(arad_sw_db_queue_type_map_get(unit, info->cr_req_type_ndx, &hw_q_type), 10, exit);
  if (hw_q_type == ARAD_SW_DB_QUEUE_TYPE_NOT_AVAILABLE) {
    SOC_SAND_SET_ERROR_CODE(ARAD_INTERNAL_ASSERT_ERR, 99, exit); /* the given user defined queue type was not set by the user */
  }
  ips_queue_type_lookup_table_tbl_data.queue_type_lookup_table = hw_q_type;

  res = arad_iqm_static_tbl_set_unsafe(
          unit,
          queue_ndx,
          &iqm_static_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = arad_ips_queue_type_lookup_table_tbl_set_unsafe(
          unit,
          queue_ndx,
          &ips_queue_type_lookup_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_queue_info_set_unsafe()",0,0);
}

/*********************************************************************
*     Sets the queue types of a queue
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_queue_info_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_ndx,
    SOC_SAND_IN  ARAD_ITM_QUEUE_INFO      *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_QUEUE_INFO_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    queue_ndx, ARAD_MAX_QUEUE_ID(unit),
    ARAD_QUEUE_NUM_OUT_OF_RANGE_ERR, 5, exit
  );

  if (info->vsq_traffic_cls > (ARAD_TR_CLS_MAX))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_VSQ_TRAFFIC_CLS_OUT_OF_RANGE_ERR, 10, exit);
  }
  if (info->credit_cls > (ARAD_ITM_QT_CR_CLS_MAX))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_CREDIT_CLS_OUT_OF_RANGE_ERR, 20, exit);
  }
  if (info->vsq_connection_cls > (ARAD_ITM_QT_CC_CLS_MAX))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_VSQ_CONNECTION_CLS_OUT_OF_RANGE_ERR, 30, exit);
  }
  if (info->rate_cls > (ARAD_ITM_QT_RT_CLS_MAX))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_RATE_CLS_OUT_OF_RANGE_ERR, 40, exit);
  }

  if ((info->cr_req_type_ndx < SOC_TMC_ITM_QT_NDX_02 || info->cr_req_type_ndx >= ARAD_ITM_IPS_QT_MAX) &&
      (info->cr_req_type_ndx < SOC_TMC_ITM_PREDEFIEND_OFFSET || info->cr_req_type_ndx >= SOC_TMC_ITM_PREDEFIEND_OFFSET + SOC_TMC_ITM_NOF_QT_STATIC) &&
      info->cr_req_type_ndx != SOC_TMC_ITM_PREDEFIEND_OFFSET + ARAD_ITM_IPS_QT_MAX)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_CR_REQ_TYPE_NDX_OUT_OF_RANGE_ERR, 50, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->signature, ARAD_MAX_SIGNATURE,
    ARAD_SIGNATURE_OUT_OF_RANGE_ERR, 30, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_queue_info_verify()", queue_ndx, info ? info->cr_req_type_ndx : -1);
}

/*********************************************************************
*     Gets the queue types of a queue
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_queue_info_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_ndx,
    SOC_SAND_OUT ARAD_ITM_QUEUE_INFO      *info
  )
{
  uint32
    res;
  ARAD_IQM_STATIC_TBL_DATA
    iqm_static_tbl_data;
  ARAD_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_DATA
    ips_queue_type_lookup_table_tbl_data;
  uint8
    tolerance_level;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_QUEUE_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    queue_ndx, ARAD_MAX_QUEUE_ID(unit),
    ARAD_QUEUE_NUM_OUT_OF_RANGE_ERR, 5, exit
  );

  res = arad_iqm_static_tbl_get_unsafe(
          unit,
          queue_ndx,
          &iqm_static_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = arad_ips_queue_type_lookup_table_tbl_get_unsafe(
          unit,
          queue_ndx,
          &ips_queue_type_lookup_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 21, exit);

  info->vsq_traffic_cls =
    iqm_static_tbl_data.traffic_class;
  info->credit_cls =
    iqm_static_tbl_data.credit_class;
  info->vsq_connection_cls =
    iqm_static_tbl_data.connection_class;
  info->rate_cls =
    iqm_static_tbl_data.rate_class;
  info->signature =
    iqm_static_tbl_data.que_signature;

  SOC_SAND_CHECK_FUNC_RESULT(arad_sw_db_queue_type_map_reverse_get(unit, ips_queue_type_lookup_table_tbl_data.queue_type_lookup_table, &tolerance_level), 40, exit);
  info->cr_req_type_ndx = tolerance_level;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_queue_info_get_unsafe()",0,0);
}

/*********************************************************************
*     Gets the queue types of a queue
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_queue_dyn_info_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_ndx,
    SOC_SAND_OUT ARAD_ITM_QUEUE_DYN_INFO      *info
  )  
{
  uint32
    res;
  ARAD_IQM_DYNAMIC_TBL_DATA iqm_dynamic_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    queue_ndx, ARAD_MAX_QUEUE_ID(unit),
    ARAD_QUEUE_NUM_OUT_OF_RANGE_ERR, 5, exit
  );

  res = arad_iqm_dynamic_tbl_get_unsafe(
          unit,
          queue_ndx,
          &iqm_dynamic_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  info->pq_head_ptr =
    iqm_dynamic_tbl.pq_head_ptr;
  info->pq_inst_que_size =
    iqm_dynamic_tbl.pq_inst_que_size;
  info->pq_inst_que_buff_size =
    iqm_dynamic_tbl.pq_inst_que_buff_size;
  info->pq_avrg_szie =
    iqm_dynamic_tbl.pq_avrg_szie;
  info->que_not_empty =
    iqm_dynamic_tbl.que_not_empty;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_queue_dyn_info_get_unsafe()",0,0);
}

/*
 * set the dynamic queue thresholds for the guaranteed resource.
 * The threshold is used to achieve the resource guarantee for queues,
 * ensuring that not to much of the resource is allocated bound the total of guarantees.
 */
uint32
  arad_itm_dyn_total_thresh_set_unsafe(
    SOC_SAND_IN  int   unit,
    SOC_SAND_IN  int32    reservation_increase /* the (signed) amount in which the thresholds should decrease (according to 100% as will be set for DP 0) */
  )
{
  uint32
    res;
  soc_dpp_guaranteed_q_resource_t guaranteed_q_resource;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_DYN_TOTAL_THRESH_SET_UNSAFE);

  if (reservation_increase < 0 &&  ((uint32)(-reservation_increase)) > SOC_DPP_CONFIG(unit)->tm.guaranteed_q_resource.used) {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 110, exit); /* internal error - bad keeping of used resource */
  }
  res = SOC_DPP_WB_ENGINE_GET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_TM_GUARANTEED_Q_RESOURCE, &guaranteed_q_resource);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 115, exit);
  /* update the amount of the resource that is left */
  if (reservation_increase) {
    int32 resource_left_calc =  ((int32)(guaranteed_q_resource.total -
            (guaranteed_q_resource.used))) - reservation_increase;
    uint32 resource_left =  resource_left_calc;
    if (resource_left_calc < 0) { /* check if we are out of the resource */
      SOC_SAND_SET_ERROR_CODE(ARAD_ITM_GRNT_OUT_OF_RESOURCE_ERR, 120, exit);
    }

    if (SOC_DPP_CONFIG(unit)->tm.guaranteed_q_mode == SOC_DPP_GUARANTEED_Q_RESOURCE_MEMORY) {

      

    } else {
      uint32 reg_dp0 = ARAD_ITM_GRNT_BDS_OR_DBS_DISABLED;
      uint32 reg_dp1 = ARAD_ITM_GRNT_BDS_OR_DBS_DISABLED;
      uint32 reg_dp2 = ARAD_ITM_GRNT_BDS_OR_DBS_DISABLED;
      uint32 reg_dp3 = ARAD_ITM_GRNT_BDS_OR_DBS_DISABLED;
      if (resource_left) { /* configure drop thresholds according to new amount of resource left */
        
        uint32 mantissa, exponent, field_val;
        SOC_SAND_CHECK_FUNC_RESULT( soc_sand_break_to_mnt_exp_round_down( resource_left,
          ARAD_ITM_GRNT_BDS_OR_DBS_MANTISSA_BITS, ARAD_ITM_GRNT_BDS_OR_DBS_EXPONENT_BITS,
          0, &mantissa, &exponent), 110, exit);
        field_val = mantissa | (exponent << ARAD_ITM_GRNT_BDS_OR_DBS_MANTISSA_BITS);
        soc_reg_field_set(unit, IQM_BUFF_DYN_SIZE_TH_CFG_1r, &reg_dp0, BUFF_DYN_SIZE_RJCT_SET_TH_0f, field_val);
        soc_reg_field_set(unit, IQM_BUFF_DYN_SIZE_TH_CFG_1r, &reg_dp0, BUFF_DYN_SIZE_RJCT_CLR_TH_0f, field_val);

        SOC_SAND_CHECK_FUNC_RESULT( soc_sand_break_to_mnt_exp_round_down( (resource_left / 20) * 17, /* 85% */
          ARAD_ITM_GRNT_BDS_OR_DBS_MANTISSA_BITS, ARAD_ITM_GRNT_BDS_OR_DBS_EXPONENT_BITS,
          0, &mantissa, &exponent), 120, exit);
        field_val = mantissa | (exponent << ARAD_ITM_GRNT_BDS_OR_DBS_MANTISSA_BITS);
        soc_reg_field_set(unit, IQM_BUFF_DYN_SIZE_TH_CFG_2r, &reg_dp1, BUFF_DYN_SIZE_RJCT_SET_TH_1f, field_val);
        soc_reg_field_set(unit, IQM_BUFF_DYN_SIZE_TH_CFG_2r, &reg_dp1, BUFF_DYN_SIZE_RJCT_CLR_TH_1f, field_val);

        SOC_SAND_CHECK_FUNC_RESULT( soc_sand_break_to_mnt_exp_round_down( (resource_left / 20) * 15, /* 75% */
          ARAD_ITM_GRNT_BDS_OR_DBS_MANTISSA_BITS, ARAD_ITM_GRNT_BDS_OR_DBS_EXPONENT_BITS,
          0, &mantissa, &exponent), 130, exit);
        field_val = mantissa | (exponent << ARAD_ITM_GRNT_BDS_OR_DBS_MANTISSA_BITS);
        soc_reg_field_set(unit, IQM_BUFF_DYN_SIZE_TH_CFG_3r, &reg_dp2, BUFF_DYN_SIZE_RJCT_SET_TH_2f, field_val);
        soc_reg_field_set(unit, IQM_BUFF_DYN_SIZE_TH_CFG_3r, &reg_dp2, BUFF_DYN_SIZE_RJCT_CLR_TH_2f, field_val);

        SOC_SAND_CHECK_FUNC_RESULT( soc_sand_break_to_mnt_exp_round_down(0, /* 0% */
          ARAD_ITM_GRNT_BDS_OR_DBS_MANTISSA_BITS, ARAD_ITM_GRNT_BDS_OR_DBS_EXPONENT_BITS,
          0, &mantissa, &exponent), 140, exit);
        field_val = mantissa | (exponent << ARAD_ITM_GRNT_BDS_OR_DBS_MANTISSA_BITS);
        soc_reg_field_set(unit, IQM_BUFF_DYN_SIZE_TH_CFG_4r, &reg_dp3, BUFF_DYN_SIZE_RJCT_SET_TH_3f, field_val);
        soc_reg_field_set(unit, IQM_BUFF_DYN_SIZE_TH_CFG_4r, &reg_dp3, BUFF_DYN_SIZE_RJCT_CLR_TH_3f, field_val);
      }

      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 210, exit, ARAD_REG_ACCESS_ERR,
        WRITE_IQM_BUFF_DYN_SIZE_TH_CFG_1r(unit, reg_dp0));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 220, exit, ARAD_REG_ACCESS_ERR,
        WRITE_IQM_BUFF_DYN_SIZE_TH_CFG_2r(unit, reg_dp1));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 230, exit, ARAD_REG_ACCESS_ERR,
        WRITE_IQM_BUFF_DYN_SIZE_TH_CFG_3r(unit, reg_dp2));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 240, exit, ARAD_REG_ACCESS_ERR,
        WRITE_IQM_BUFF_DYN_SIZE_TH_CFG_4r(unit, reg_dp3));

    }
    guaranteed_q_resource.used += reservation_increase;
    res = SOC_DPP_WB_ENGINE_SET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_ARAD_TM_GUARANTEED_Q_RESOURCE, &guaranteed_q_resource); /* update warm boot data */
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 250, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_dyn_total_thresh_set_unsafe()",0,0);
}

/*********************************************************************
*     Sets ingress shaping configuration. This includes
*     ingress shaping queues range, and credit generation
*     configuration.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_ingress_shape_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_INGRESS_SHAPE_INFO *info
  )
{
  uint32
    fld_val,
    q_hi,
    q_low,
    q_base,
    ihp_reg_low,
    ihp_reg_hi,
    base_port_tc,
    rate_internal,
    res;
  ARAD_IPQ_EXPLICIT_MAPPING_MODE_INFO
    explicit_info;
   
    

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_INGRESS_SHAPE_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  

  res = arad_ipq_explicit_mapping_mode_info_get_unsafe(
          unit,
          &explicit_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,10,exit);

  q_base = explicit_info.base_queue_id;

  if (info->enable)
  {
    q_low = info->q_range.q_num_low;
    q_hi  = info->q_range.q_num_high;
  }
  else
  {
    /*
     *  Set 'hi' below 'low'.
     *  Note: there is an assumption here that:
     *         - For 'Add' mode, Base queue is smaller then ARAD_MAX_QUEUE_ID - 1
     *         - For 'Decrement' mode, Base queue is bigger than 2.
     */
    if (
        !(explicit_info.queue_id_add_not_decrement)
        && (q_base < 2)
       )
    {
      SOC_SAND_SET_ERROR_CODE(ARAD_IPQ_EXPLICIT_MAPPING_MODE_BASE_QUEUE_ID_OUT_OF_RANGE_ERR, 12, exit);
    }
    q_low = (explicit_info.queue_id_add_not_decrement?q_base + 2:q_base-1);
    q_hi = q_low - 1;
  }


  /*
   *  ECI
   */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_2r, REG_PORT_ANY, 0, ISP_QNUM_LOWf,  q_low));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, ECI_GLOBAL_3r, REG_PORT_ANY, 0, ISP_QNUM_HIGHf,  q_hi));

  /*
   *  IHP
   */
  if (explicit_info.queue_id_add_not_decrement)
  {
    ihp_reg_low = q_low  - q_base;
    ihp_reg_hi  = q_hi   - q_base;
  }
  else
  {
    ihp_reg_low = q_base - q_hi;
    ihp_reg_hi  = q_base - q_low;
  }

  res = arad_parser_ingress_shape_state_set(
            unit,
            info->enable,
            ihp_reg_low,
            ihp_reg_hi
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);


  fld_val = SOC_SAND_BOOL2NUM(info->enable);
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, SCH_INGRESS_SHAPING_PORT_CONFIGURATIONr, SOC_CORE_ALL, 0, INGRESS_SHAPING_ENABLEf,  fld_val));

  if (info->enable)
  {
    res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, info->sch_port, 0, &base_port_tc);  
    SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 55, exit);

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, SCH_INGRESS_SHAPING_PORT_CONFIGURATIONr, SOC_CORE_ALL, 0, INGRESS_SHAPING_PORT_IDf,  base_port_tc));

    res = arad_intern_rate2clock(
            unit,
            info->rate,
            TRUE, /* is_for_ips */
            &rate_internal
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_INGRESS_SHAPE_SCHEDULER_CONFIGr, SOC_CORE_ALL, 0, ISS_MAX_CR_RATEf,  rate_internal));
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_ingress_shape_set_unsafe()",0,0);
}
/*********************************************************************
*     Sets ingress shaping configuration. This includes
*     ingress shaping queues range, and credit generation
*     configuration.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_ingress_shape_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_INGRESS_SHAPE_INFO *info
  )
{
  uint32
    res;
  ARAD_IPQ_EXPLICIT_MAPPING_MODE_INFO
    explicit_info;
  uint8
    is_base_q_set;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_INGRESS_SHAPE_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  if (info->enable)
  {

    res = arad_ipq_base_q_is_valid_get_unsafe(
            unit,
            &is_base_q_set
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

    if (!is_base_q_set)
    {
      SOC_SAND_SET_ERROR_CODE(ARAD_BASE_Q_NOT_SET_ERR, 7, exit);
    }
/*
    res = arad_fap_port_id_verify(
            unit,
            info->sch_port,
            TRUE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->rate, ARAD_IF_MAX_RATE_KBPS,
      ARAD_ITM_ING_SHAPE_SCH_RATE_OUT_OF_RANGE_ERR, 20, exit
    );
*/
    arad_ARAD_IPQ_EXPLICIT_MAPPING_MODE_INFO_clear(&explicit_info);

    res = arad_ipq_explicit_mapping_mode_info_get_unsafe(
          unit,
          &explicit_info
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    SOC_SAND_ERR_IF_ABOVE_MAX(
      explicit_info.base_queue_id, ARAD_MAX_QUEUE_ID(unit),
      ARAD_IPQ_EXPLICIT_MAPPING_MODE_BASE_QUEUE_ID_OUT_OF_RANGE_ERR, 40, exit
    );

    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->q_range.q_num_low, info->q_range.q_num_high,
      ARAD_ITM_INGRESS_SHAPING_LOW_BELOW_HIGH_ERR, 50, exit
    );

    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->q_range.q_num_high, ARAD_MAX_QUEUE_ID(unit),
      ARAD_QUEUE_NUM_OUT_OF_RANGE_ERR, 50, exit
    );

    if (explicit_info.queue_id_add_not_decrement)
    {
      SOC_SAND_ERR_IF_ABOVE_MAX(
        explicit_info.base_queue_id, info->q_range.q_num_low + 1,
        ARAD_IHP_IS_RANGE_AND_BASE_Q_MISMATCH_ERR, 52, exit
      );

      SOC_SAND_ERR_IF_ABOVE_MAX(
        explicit_info.base_queue_id + 1, info->q_range.q_num_high,
        ARAD_IHP_IS_RANGE_AND_BASE_Q_MISMATCH_ERR, 54, exit
      );
    }
    else
    {
      SOC_SAND_ERR_IF_ABOVE_MAX(
        info->q_range.q_num_high, explicit_info.base_queue_id + 1,
        ARAD_IHP_IS_RANGE_AND_BASE_Q_MISMATCH_ERR, 56, exit
      );

      SOC_SAND_ERR_IF_ABOVE_MAX(
        info->q_range.q_num_low  + 1, explicit_info.base_queue_id,
        ARAD_IHP_IS_RANGE_AND_BASE_Q_MISMATCH_ERR, 58, exit
      );
    }
  }

    if (info->q_range.q_num_high > ARAD_MAX_QUEUE_ID(unit))
    {
      SOC_SAND_SET_ERROR_CODE(ARAD_INGR_SHP_Q_ABOVE_MAX_ERR, 60, exit);
    }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_ingress_shape_verify()",0,0);
}

/*********************************************************************
*     Sets ingress shaping configuration. This includes
*     ingress shaping queues range, and credit generation
*     configuration.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_ingress_shape_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ITM_INGRESS_SHAPE_INFO *info
  )
{
  uint32
    rate_internal,
    fld_val,
    res,
    dsp_pp,
    base_port_tc,
    nof_prio;
  soc_port_t
    port;
  soc_pbmp_t
    ports_bm;
  int
    core; 
    

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_INGRESS_SHAPE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  12,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, ECI_GLOBAL_2r, REG_PORT_ANY, 0, ISP_QNUM_LOWf, &info->q_range.q_num_low));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  14,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, ECI_GLOBAL_3r, REG_PORT_ANY, 0, ISP_QNUM_HIGHf, &info->q_range.q_num_high));

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  16,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, SCH_INGRESS_SHAPING_PORT_CONFIGURATIONr, SOC_CORE_ALL, 0, INGRESS_SHAPING_PORT_IDf, &base_port_tc));

  /* Look for match base q pair */
  res = soc_port_sw_db_valid_ports_get(unit, 0, &ports_bm);
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 17, exit);

  SOC_PBMP_ITER(ports_bm, port) 
  {
      res = soc_port_sw_db_local_to_tm_port_get(unit, port, &dsp_pp, &core);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 18, exit);

      res = soc_port_sw_db_tm_port_to_base_q_pair_get(unit, dsp_pp, core, &nof_prio);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 19, exit);

      if (base_port_tc == nof_prio) {
          info->sch_port = dsp_pp;
      }
  }
  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, SCH_INGRESS_SHAPING_PORT_CONFIGURATIONr, SOC_CORE_ALL, 0, INGRESS_SHAPING_ENABLEf, &fld_val));
  info->enable = SOC_SAND_NUM2BOOL(fld_val);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  21,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IPS_INGRESS_SHAPE_SCHEDULER_CONFIGr, SOC_CORE_ALL, 0, ISS_MAX_CR_RATEf, &rate_internal));

  res = arad_intern_clock2rate(
          unit,
          rate_internal,
          TRUE, /* is_for_ips */
          &(info->rate)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_ingress_shape_get_unsafe()",0,0);
}

/*********************************************************************
*     Four sets of queues-priorities maps are held in the
*     device. Per map: describes a segment of 64 contiguous
*     queues. Each queue is either high or low priority.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_priority_map_tmplt_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 map_ndx,
    SOC_SAND_IN  ARAD_ITM_PRIORITY_MAP_TMPLT *info
  )
{
  uint32
    res;
  ARAD_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_DATA
    ips_queue_priority_maps_table_tbl_data;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_PRIORITY_MAP_TMPLT_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_sand_os_memcpy(
          ips_queue_priority_maps_table_tbl_data.queue_priority_maps_table,
          info->map,
          ARAD_ITM_PRIO_MAP_SIZE_IN_UINT32S * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = arad_ips_queue_priority_maps_table_tbl_set_unsafe(
          unit,
          map_ndx,
          &ips_queue_priority_maps_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_priority_map_tmplt_set_unsafe()",0,0);
}

/*********************************************************************
*     Four sets of queues-priorities maps are held in the
*     device. Per map: describes a segment of 64 contiguous
*     queues. Each queue is either high or low priority.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_priority_map_tmplt_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 map_ndx,
    SOC_SAND_IN  ARAD_ITM_PRIORITY_MAP_TMPLT *info
  )
{
  uint32
    index;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_PRIORITY_MAP_TMPLT_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    map_ndx, ARAD_ITM_PRIORITY_MAP_NDX_MAX,
    ARAD_ITM_PRIORITY_MAP_NDX_OUT_OF_RANGE_ERR, 10, exit
  );

  for (index = 0 ; index < ARAD_ITM_PRIO_MAP_SIZE_IN_UINT32S; ++index)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->map[index], ARAD_ITM_PRIORITY_MAP_TMPLT_MAP_MAX,
      ARAD_ITM_PRIORITY_MAP_NDX_OUT_OF_RANGE_ERR, 20, exit
    );
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_priority_map_tmplt_verify()",0,0);
}

/*********************************************************************
*     Four sets of queues-priorities maps are held in the
*     device. Per map: describes a segment of 64 contiguous
*     queues. Each queue is either high or low priority.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_priority_map_tmplt_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 map_ndx,
    SOC_SAND_OUT ARAD_ITM_PRIORITY_MAP_TMPLT *info
  )
{
  uint32
    res;
  ARAD_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_DATA
    ips_queue_priority_maps_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_PRIORITY_MAP_TMPLT_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    map_ndx, ARAD_ITM_PRIORITY_MAP_NDX_MAX,
    ARAD_ITM_PRIORITY_MAP_NDX_OUT_OF_RANGE_ERR, 5, exit
  );

  res = arad_ips_queue_priority_maps_table_tbl_get_unsafe(
          unit,
          map_ndx,
          &ips_queue_priority_maps_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_sand_os_memcpy(
          info->map,
          ips_queue_priority_maps_table_tbl_data.queue_priority_maps_table,
          ARAD_ITM_PRIO_MAP_SIZE_IN_UINT32S * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_priority_map_tmplt_get_unsafe()",0,0);
}

/*********************************************************************
*     The 32K ingress-queues range is segmented into 512
*     segments of 64 contiguous queues, that is, queues 64N to
*     64N+63 that all have the same map-id (one of four).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_priority_map_tmplt_select_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_64_ndx,
    SOC_SAND_IN  uint32                 priority_map
  )
{
  uint32
    res;
  ARAD_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_DATA
    ips_queue_priority_map_select_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_PRIORITY_MAP_TMPLT_SELECT_SET_UNSAFE);

  ips_queue_priority_map_select_tbl_data.queue_priority_map_select = priority_map;

  res = arad_ips_queue_priority_map_select_tbl_set_unsafe(unit, queue_64_ndx, &ips_queue_priority_map_select_tbl_data);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_priority_map_tmplt_select_set_unsafe()",0,0);
}

/*********************************************************************
*     The 32K ingress-queues range is segmented into 512
*     segments of 64 contiguous queues, that is, queues 64N to
*     64N+63 that all have the same map-id (one of four).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_priority_map_tmplt_select_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_64_ndx,
    SOC_SAND_IN  uint32                 priority_map
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_PRIORITY_MAP_TMPLT_SELECT_VERIFY);


  SOC_SAND_ERR_IF_ABOVE_MAX(
    queue_64_ndx, (ARAD_ITM_PRIO_NOF_SEGMENTS(unit) - 1),
    ARAD_ITM_PRIORITY_MAP_SEGMENT_NDX_OUT_OF_RANGE_ERR, 10, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    priority_map, ARAD_ITM_PRIORITY_MAP_NDX_MAX,
    ARAD_ITM_PRIORITY_MAP_NDX_OUT_OF_RANGE_ERR, 20, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_priority_map_tmplt_select_verify()",0,0);
}

/*********************************************************************
*     The 32K ingress-queues range is segmented into 512
*     segments of 64 contiguous queues, that is, queues 64N to
*     64N+63 that all have the same map-id (one of four).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_priority_map_tmplt_select_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_64_ndx,
    SOC_SAND_OUT uint32                 *priority_map
  )
{
  uint32
    res;
  ARAD_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_DATA
    ips_queue_priority_map_select_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_PRIORITY_MAP_TMPLT_SELECT_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(priority_map);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    queue_64_ndx, (ARAD_ITM_PRIO_NOF_SEGMENTS(unit) - 1),
    ARAD_ITM_PRIORITY_MAP_SEGMENT_NDX_OUT_OF_RANGE_ERR, 10, exit
  );

  res = arad_ips_queue_priority_map_select_tbl_get_unsafe(
          unit,
          queue_64_ndx,
          &ips_queue_priority_map_select_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *priority_map = ips_queue_priority_map_select_tbl_data.queue_priority_map_select;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_priority_map_tmplt_select_get_unsafe()",0,0);
}

/*********************************************************************
*     System Red drop probabilities table fill in. The system
*     Red mechanism uses a table of 16 probabilities. The
*     table is used by indexes which choose 1 out of the 16
*     options.
*     Note that the System-Red mechanism is a system-wide
*     attribute and it should be configured homogeneously
*     in all FAPs.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_sys_red_drop_prob_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_DROP_PROB *info
  )
{
  uint32
    res,
    indx;
  ARAD_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_DATA
    iqm_system_red_drop_probability_values_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_SYS_RED_DROP_PROB_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  for (indx = 0; indx<ARAD_ITM_SYS_RED_DRP_PROBS; ++indx)
  {
    iqm_system_red_drop_probability_values_tbl_data.drp_prob =
      info->drop_probs[indx];

    res= arad_iqm_system_red_drop_probability_values_tbl_set_unsafe(
           unit,
           indx,
           &iqm_system_red_drop_probability_values_tbl_data
         );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20+indx, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_sys_red_drop_prob_set_unsafe()",0,0);
}

/*********************************************************************
*     System Red drop probabilities table fill in. The system
*     Red mechanism uses a table of 16 probabilities. The
*     table is used by indexes which choose 1 out of the 16
*     options.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_sys_red_drop_prob_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_DROP_PROB *info
  )
{
  uint32
    index;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_SYS_RED_DROP_PROB_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  for (index = 0 ; index < ARAD_ITM_SYS_RED_DRP_PROBS; ++index)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->drop_probs[index], ARAD_ITM_SYS_RED_DROP_PROB_VAL_MAX,
      ARAD_ITM_PRIORITY_MAP_NDX_OUT_OF_RANGE_ERR, 10, exit
    );
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_sys_red_drop_prob_verify()",0,0);
}

/*********************************************************************
*     System Red drop probabilities table fill in. The system
*     Red mechanism uses a table of 16 probabilities. The
*     table is used by indexes which choose 1 out of the 16
*     options.
*     Note that the System-Red mechanism is a system-wide
*     attribute and it should be configured homogeneously
*     in all FAPs.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_sys_red_drop_prob_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_DROP_PROB *info
  )
{
  uint32
    res,
    indx;
  ARAD_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_DATA
    iqm_system_red_drop_probability_values_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_SYS_RED_DROP_PROB_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  for (indx = 0; indx<ARAD_ITM_SYS_RED_DRP_PROBS; ++indx)
  {
    res= arad_iqm_system_red_drop_probability_values_tbl_get_unsafe(
           unit,
           indx,
           &iqm_system_red_drop_probability_values_tbl_data
         );
    info->drop_probs[indx] =
      iqm_system_red_drop_probability_values_tbl_data.drp_prob;

    SOC_SAND_CHECK_FUNC_RESULT(res, 20+indx, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_sys_red_drop_prob_get_unsafe()",0,0);
}

/*********************************************************************
*     System Red queue size boundaries, per queue type - rate
*     class. The queue size ranges table is set. For each
*     queue type and drop-precedence,
*     drop/pass/drop-with-probability parameters are set using
*     the function arad_itm_sys_red_qt_dp_info_set.
*     Note that the System-Red mechanism is a system-wide
*     attribute and it should be configured homogeneously
*     in all FAPs.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_sys_red_queue_size_boundaries_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_QT_INFO *info,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_QT_INFO *exact_info
  )
{
  uint32
    res,
    boundary_i;
  ARAD_IQM_SYSTEM_RED_TBL_DATA
    iqm_system_red_tbl_data;
  int32
    exact_value_var;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_SYS_RED_QUEUE_SIZE_BOUNDARIES_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);

  for (boundary_i = 0; boundary_i < (ARAD_ITM_SYS_RED_Q_SIZE_RANGES-1); boundary_i++)
  {

    res = arad_itm_man_exp_buffer_set(
            info->queue_size_boundaries[boundary_i],
            ARAD_ITM_SYS_RED_Q_BOUNDARY_TH_MNT_LSB,
            ARAD_ITM_VSQ_SYS_RED_Q_BOUNDARY_TH_MNT_NOF_BITS,
            ARAD_ITM_SYS_RED_Q_BOUNDARY_TH_EXP_LSB,
            ARAD_ITM_VSQ_SYS_RED_Q_BOUNDARY_TH_EXP_NOF_BITS,
            FALSE,
            &iqm_system_red_tbl_data.qsz_rng_th[boundary_i],
            &exact_value_var
         );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    exact_info->queue_size_boundaries[boundary_i] = (uint32)exact_value_var;
  }

  res = arad_iqm_system_red_tbl_set_unsafe(
    unit,
    rt_cls_ndx,
    &iqm_system_red_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_sys_red_queue_size_boundaries_set_unsafe()",0,0);
}

/*********************************************************************
*     System Red queue size boundaries, per queue type - rate
*     class. The queue size ranges table is set. For each
*     queue type and drop-precedence,
*     drop/pass/drop-with-probability parameters are set using
*     the function arad_itm_sys_red_qt_dp_info_set.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_sys_red_queue_size_boundaries_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_QT_INFO *info
  )
{
  uint32
    index;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_SYS_RED_QUEUE_SIZE_BOUNDARIES_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    rt_cls_ndx, ARAD_ITM_QT_RT_CLS_MAX,
    ARAD_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 10, exit
  );

  for (index = 0 ; index < (ARAD_ITM_SYS_RED_Q_SIZE_RANGES-1); ++index)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->queue_size_boundaries[index], ARAD_MAX_QUEUE_SIZE_BYTES,
      ARAD_ITM_PRIORITY_MAP_NDX_OUT_OF_RANGE_ERR, 20, exit
    );
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_sys_red_queue_size_boundaries_verify()",0,0);
}

/*********************************************************************
*     System Red queue size boundaries, per queue type - rate
*     class. The queue size ranges table is set. For each
*     queue type and drop-precedence,
*     drop/pass/drop-with-probability parameters are set using
*     the function arad_itm_sys_red_qt_dp_info_set.
*     Note that the System-Red mechanism is a system-wide
*     attribute and it should be configured homogeneously
*     in all FAPs.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_sys_red_queue_size_boundaries_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_QT_INFO *info
  )
{
  uint32
    res,
    boundary_i;
  ARAD_IQM_SYSTEM_RED_TBL_DATA
    iqm_system_red_tbl_data;
  int32
    exact_value_var;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_SYS_RED_QUEUE_SIZE_BOUNDARIES_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    rt_cls_ndx, ARAD_ITM_QT_RT_CLS_MAX,
    ARAD_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 10, exit
  );

  res = arad_iqm_system_red_tbl_get_unsafe(
    unit,
    rt_cls_ndx,
    &iqm_system_red_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  for (boundary_i = 0; boundary_i < (ARAD_ITM_SYS_RED_Q_SIZE_RANGES-1); boundary_i++)
  {
    res = arad_itm_man_exp_buffer_get(
            iqm_system_red_tbl_data.qsz_rng_th[boundary_i],
            ARAD_ITM_SYS_RED_Q_BOUNDARY_TH_MNT_LSB,
            ARAD_ITM_VSQ_SYS_RED_Q_BOUNDARY_TH_MNT_NOF_BITS,
            ARAD_ITM_SYS_RED_Q_BOUNDARY_TH_EXP_LSB,
            ARAD_ITM_VSQ_SYS_RED_Q_BOUNDARY_TH_EXP_NOF_BITS,
            FALSE,
            &exact_value_var
         );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    info->queue_size_boundaries[boundary_i] = (uint32)exact_value_var;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_sys_red_queue_size_boundaries_get_unsafe()",0,0);
}

/*********************************************************************
*     Configures the ingress system red parameters per q-type
*     - rate class and drop-precedence. This includes the
*     thresholds and drop probability, which determine the
*     behavior of the algorithm according to the queue size
*     index.
*     Note that the System-Red mechanism is a system-wide
*     attribute and it should be configured homogeneously
*     in all FAPs.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_sys_red_q_based_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 sys_red_dp_ndx,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_QT_DP_INFO *info
  )
{
  uint32
    res;
  ARAD_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_DATA
    iqm_system_red_parameters_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_SYS_RED_Q_BASED_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = arad_iqm_system_red_parameters_table_tbl_get_unsafe(
          unit,
          rt_cls_ndx,
          sys_red_dp_ndx,
          &iqm_system_red_parameters_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  iqm_system_red_parameters_table_tbl_data.adm_th =
    info->adm_th;
  iqm_system_red_parameters_table_tbl_data.prob_th =
    info->prob_th;
  iqm_system_red_parameters_table_tbl_data.drp_th =
    info->drp_th;
  iqm_system_red_parameters_table_tbl_data.drp_prob_indx1 =
    info->drp_prob_low;
  iqm_system_red_parameters_table_tbl_data.drp_prob_indx2 =
    info->drp_prob_high;
  iqm_system_red_parameters_table_tbl_data.sys_red_en =
    info->enable;

  res = arad_iqm_system_red_parameters_table_tbl_set_unsafe(
          unit,
          rt_cls_ndx,
          sys_red_dp_ndx,
          &iqm_system_red_parameters_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_sys_red_q_based_set_unsafe()",0,0);
}

/*********************************************************************
*     Configures the ingress system red parameters per q-type
*     - rate class and drop-precedence. This includes the
*     thresholds and drop probability, which determine the
*     behavior of the algorithm according to the queue size
*     index.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_sys_red_q_based_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 sys_red_dp_ndx,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_QT_DP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_SYS_RED_Q_BASED_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    rt_cls_ndx, ARAD_ITM_QT_RT_CLS_MAX,
    ARAD_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    sys_red_dp_ndx, ARAD_MAX_DROP_PRECEDENCE,
    ARAD_DRPP_OUT_OF_RANGE_ERR, 7, exit
  );

  if (info->adm_th > ARAD_ITM_SYS_RED_QUEUE_TH_MAX)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_SYS_RED_QUEUE_TH_OUT_OF_RANGE_ERR, 10, exit);
  }
  if(info->prob_th > ARAD_ITM_SYS_RED_QUEUE_TH_MAX)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_SYS_RED_QUEUE_TH_OUT_OF_RANGE_ERR, 20, exit);
  }
  if (info->drp_th > ARAD_ITM_SYS_RED_QUEUE_TH_MAX)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_SYS_RED_QUEUE_TH_OUT_OF_RANGE_ERR, 30, exit);
  }
  if (info->drp_prob_low > (ARAD_ITM_SYS_RED_DRP_PROBS-1))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_SYS_RED_DRP_PROB_INDEX_OUT_OF_RANGE_ERR, 40, exit);
  }
  if (info->drp_prob_high > (ARAD_ITM_SYS_RED_DRP_PROBS-1))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_SYS_RED_DRP_PROB_INDEX_OUT_OF_RANGE_ERR, 50, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_sys_red_q_based_verify()",0,0);
}

/*********************************************************************
*     Configures the ingress system red parameters per q-type
*     - rate class and drop-precedence. This includes the
*     thresholds and drop probability, which determine the
*     behavior of the algorithm according to the queue size
*     index.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_sys_red_q_based_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 sys_red_dp_ndx,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_QT_DP_INFO *info
  )
{
  uint32
    res;
  ARAD_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_DATA
    iqm_system_red_parameters_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_SYS_RED_Q_BASED_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    rt_cls_ndx, ARAD_ITM_QT_RT_CLS_MAX,
    ARAD_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    sys_red_dp_ndx, ARAD_MAX_DROP_PRECEDENCE,
    ARAD_DRPP_OUT_OF_RANGE_ERR, 7, exit
  );

  res = arad_iqm_system_red_parameters_table_tbl_get_unsafe(
          unit,
          rt_cls_ndx,
          sys_red_dp_ndx,
          &iqm_system_red_parameters_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  info->adm_th =
    iqm_system_red_parameters_table_tbl_data.adm_th;
  info->prob_th =
    iqm_system_red_parameters_table_tbl_data.prob_th;
  info->drp_th =
    iqm_system_red_parameters_table_tbl_data.drp_th;
  info->drp_prob_low =
    iqm_system_red_parameters_table_tbl_data.drp_prob_indx1;
  info->drp_prob_high =
    iqm_system_red_parameters_table_tbl_data.drp_prob_indx2;
  info->enable =
    (uint8)iqm_system_red_parameters_table_tbl_data.sys_red_en;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_sys_red_q_based_get_unsafe()",0,0);
}

/*********************************************************************
*     At the outgoing FAP port, a System-Queue-Size is
*     maintained. Per a configurable aging-period time the
*     queue is aged. System-Queue-Size has two again models
*     (when aging time arrived): reset or decrement. Reset
*     sets the System-Queue-Size to zero, decrement decrease
*     the size of the OFP System-Queue-Size with one. Note:
*     though this function is not an ITM function, it resides
*     here due to relevance to other System RED functions.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_sys_red_eg_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_EG_INFO *info,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_EG_INFO *exact_info
  )
{
  uint32
    res,
    aging_timer,
    aging_timer_ns,
    device_mega_ticks_per_sec,
    convert_coefficient_val;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_SYS_RED_EG_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);

  device_mega_ticks_per_sec = arad_chip_mega_ticks_per_sec_get(unit);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  0, exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, SCH_SYSTEM_RED_CONFIGURATIONr, SOC_CORE_ALL, 0, ENABLE_SYS_REDf,  info->enable));

  /* Overflow protection - will convert aging timer from ms to ns */
  if(info->aging_timer > SOC_SAND_DIV_ROUND_DOWN(SAL_UINT32_MAX,1000000))
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_SYS_RED_EG_INFO_AGING_TIMER_OUT_OF_RANGE_ERR, 10, exit);
  }

  /* AgingTimerRegisterValue = VAL(ns) / (1296 * CLK(mhz) / 250) */
  /* The convert_coefficient_val should be 1296 for 250Mhz CLK  */
  convert_coefficient_val = ARAD_ITM_SYS_RED_EG_INFO_AGING_TIMER_CONVERT_COEFFICIENT * device_mega_ticks_per_sec; 
  convert_coefficient_val = SOC_SAND_DIV_ROUND_UP(convert_coefficient_val,1000);  

  aging_timer_ns = info->aging_timer * 1000000;
  aging_timer = SOC_SAND_DIV_ROUND_UP(aging_timer_ns,convert_coefficient_val);
  
  /* Verify the value is less than MAX value */
  if (aging_timer > ARAD_ITM_SYS_RED_EG_INFO_AGING_TIMER_MAX)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_ITM_SYS_RED_EG_INFO_AGING_TIMER_OUT_OF_RANGE_ERR, 30, exit);
  }

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, SCH_SYSTEM_RED_CONFIGURATIONr, SOC_CORE_ALL, 0, AGING_TIMER_CFGf,  info->aging_timer));

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, SCH_SYSTEM_RED_CONFIGURATIONr, SOC_CORE_ALL, 0, RESET_XPIRED_QSZf,  info->reset_expired_q_size));

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, SCH_SYSTEM_RED_CONFIGURATIONr, SOC_CORE_ALL, 0, AGING_ONLY_DEC_PQSf,  info->aging_only_dec_q_size));

  exact_info->enable = info->enable;
  exact_info->reset_expired_q_size = info->reset_expired_q_size;
  exact_info->aging_only_dec_q_size = info->aging_only_dec_q_size;

  aging_timer = aging_timer * convert_coefficient_val;
  aging_timer = SOC_SAND_DIV_ROUND(aging_timer,1000000);
  exact_info->aging_timer = aging_timer;

  if (SOC_IS_ARAD_A0(unit)) {
      /* Due to HW errata disable Aging in Arad-A0 */
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IPS_SYSTEM_RED_AGING_CONFIGURATIONr, SOC_CORE_ALL, 0, SYSTEM_RED_AGE_PERIODf,  0x0));
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_sys_red_eg_set_unsafe()",0,0);
}

/*********************************************************************
*     At the outgoing FAP port, a System-Queue-Size is
*     maintained. Per a configurable aging-period time the
*     queue is aged. System-Queue-Size has two again models
*     (when aging time arrived): reset or decrement. Reset
*     sets the System-Queue-Size to zero, decrement decrease
*     the size of the OFP System-Queue-Size with one. Note:
*     though this function is not an ITM function, it resides
*     here due to relevance to other System RED functions.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_sys_red_eg_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_EG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_SYS_RED_EG_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  /* Checking the timer value against the MAX value is done in the arad_itm_sys_red_eg_set() function */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_sys_red_eg_verify()",0,0);
}

/*********************************************************************
*     At the outgoing FAP port, a System-Queue-Size is
*     maintained. Per a configurable aging-period time the
*     queue is aged. System-Queue-Size has two again models
*     (when aging time arrived): reset or decrement. Reset
*     sets the System-Queue-Size to zero, decrement decrease
*     the size of the OFP System-Queue-Size with one. Note:
*     though this function is not an ITM function, it resides
*     here due to relevance to other System RED functions.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_sys_red_eg_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_EG_INFO *info
  )
{
  uint32
    res,
    sys_red_enable,
    reset_expired_q_size_en,
    aging_only_dec_q_size_en;
   
    

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_SYS_RED_EG_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, SCH_SYSTEM_RED_CONFIGURATIONr, SOC_CORE_ALL, 0, ENABLE_SYS_REDf, &sys_red_enable));
  info->enable = (uint8)sys_red_enable;

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  12,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, SCH_SYSTEM_RED_CONFIGURATIONr, SOC_CORE_ALL, 0, AGING_TIMER_CFGf, &info->aging_timer));

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  14,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, SCH_SYSTEM_RED_CONFIGURATIONr, SOC_CORE_ALL, 0, RESET_XPIRED_QSZf, &reset_expired_q_size_en));
  info->reset_expired_q_size = (uint8)reset_expired_q_size_en;

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  16,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, SCH_SYSTEM_RED_CONFIGURATIONr, SOC_CORE_ALL, 0, AGING_ONLY_DEC_PQSf, &aging_only_dec_q_size_en));
  info->aging_only_dec_q_size = (uint8)aging_only_dec_q_size_en;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_sys_red_eg_get_unsafe()",0,0);
}

/*********************************************************************
*     In the System Red mechanism there is an aspect of
*     Consumed Resources. This mechanism gives the queues a
*     value that is compared with the value of the queue size
*     index - the maximum of the 2 is sent to the threshold
*     tests. The queues are divided to 4 ranges. In 3 types:
*     Free Unicast Data buffers Thresholds, Free Multicast
*     Data buffers Thresholds, Free BD buffers Thresholds.
*     This function determines the thresholds of the ranges
*     and the values of the ranges (0-15).
*     Note that the value of the queue is attributed to the
*     consumed resources (as opposed to the free resources).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
    arad_itm_sys_red_glob_rcs_set_unsafe(
        SOC_SAND_IN  int                 unit,
        SOC_SAND_IN  ARAD_ITM_SYS_RED_GLOB_RCS_INFO *info)
{
    uint32
        res,
        reg_val;

    SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_SYS_RED_GLOB_RCS_SET_UNSAFE);

    SOC_SAND_CHECK_NULL_INPUT(info);


    /*
     * Free Unicast Buffers Thresholds and Values
     */
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  5,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_FREE_UNICAST_DBUFF_THRESHOLD_0r, REG_PORT_ANY, 0, FR_UN_DB_TH_0f,  info->thresholds.unicast_rng_ths[0]));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_FREE_UNICAST_DBUFF_THRESHOLD_1r, REG_PORT_ANY, 0, FR_UN_DB_TH_1f,  info->thresholds.unicast_rng_ths[1]));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  15,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_FREE_UNICAST_DBUFF_THRESHOLD_2r, REG_PORT_ANY, 0, FR_UN_DB_TH_2f,  info->thresholds.unicast_rng_ths[2]));
    reg_val = 0x0;
    ARAD_FLD_TO_REG(IQM_FREE_UNICAST_DBUFF_RANGE_VALUESr, FR_UN_DB_RNG_VAL_0f, info->values.unicast_rng_vals[0], reg_val, 25, exit);
    ARAD_FLD_TO_REG(IQM_FREE_UNICAST_DBUFF_RANGE_VALUESr, FR_UN_DB_RNG_VAL_1f, info->values.unicast_rng_vals[1], reg_val, 30, exit);
    ARAD_FLD_TO_REG(IQM_FREE_UNICAST_DBUFF_RANGE_VALUESr, FR_UN_DB_RNG_VAL_2f, info->values.unicast_rng_vals[2], reg_val, 35, exit);
    ARAD_FLD_TO_REG(IQM_FREE_UNICAST_DBUFF_RANGE_VALUESr, FR_UN_DB_RNG_VAL_3f, info->values.unicast_rng_vals[3], reg_val, 40, exit);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,WRITE_IQM_FREE_UNICAST_DBUFF_RANGE_VALUESr(unit,  reg_val));

    /*
     * Free Multicast Buffers Thresholds and Values
     */
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  55,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_FREE_FULL_MULTICAST_DBUFF_THRESHOLD_0r, REG_PORT_ANY, 0, FR_FL_MUL_DB_TH_0f,  info->thresholds.multicast_rng_ths[0]));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_FREE_FULL_MULTICAST_DBUFF_THRESHOLD_1r, REG_PORT_ANY, 0, FR_FL_MUL_DB_TH_1f,  info->thresholds.multicast_rng_ths[1]));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  65,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_FREE_FULL_MULTICAST_DBUFF_THRESHOLD_2r, REG_PORT_ANY, 0, FR_FL_MUL_DB_TH_2f,  info->thresholds.multicast_rng_ths[2]));
    reg_val = 0x0;
    ARAD_FLD_TO_REG(IQM_FREE_FULL_MULTICAST_DBUFF_RANGE_VALUESr, FR_FL_ML_DB_RNG_VAL_0f, info->values.multicast_rng_vals[0], reg_val, 75, exit);
    ARAD_FLD_TO_REG(IQM_FREE_FULL_MULTICAST_DBUFF_RANGE_VALUESr, FR_FL_ML_DB_RNG_VAL_1f, info->values.multicast_rng_vals[1], reg_val, 80, exit);
    ARAD_FLD_TO_REG(IQM_FREE_FULL_MULTICAST_DBUFF_RANGE_VALUESr, FR_FL_ML_DB_RNG_VAL_2f, info->values.multicast_rng_vals[2], reg_val, 85, exit);
    ARAD_FLD_TO_REG(IQM_FREE_FULL_MULTICAST_DBUFF_RANGE_VALUESr, FR_FL_ML_DB_RNG_VAL_3f, info->values.multicast_rng_vals[3], reg_val, 90, exit);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  100,  exit, ARAD_REG_ACCESS_ERR,WRITE_IQM_FREE_FULL_MULTICAST_DBUFF_RANGE_VALUESr(unit, SOC_CORE_ALL,  reg_val));

    /*
     * Free BD Buffers Thresholds and Values
     */
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  55,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_FREE_BDB_THRESHOLD_0r, REG_PORT_ANY, 0, FR_BDB_TH_0f,  info->thresholds.bds_rng_ths[0]));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_FREE_BDB_THRESHOLD_1r, REG_PORT_ANY, 0, FR_BDB_TH_1f,  info->thresholds.bds_rng_ths[1]));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  65,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_FREE_BDB_THRESHOLD_2r, REG_PORT_ANY, 0, FR_BDB_TH_2f,  info->thresholds.bds_rng_ths[2]));
    reg_val = 0x0;
    ARAD_FLD_TO_REG(IQM_FREE_BDB_RANGE_VALUESr, FR_BDB_RNG_VAL_0f, info->values.bds_rang_vals[0], reg_val, 75, exit);
    ARAD_FLD_TO_REG(IQM_FREE_BDB_RANGE_VALUESr, FR_BDB_RNG_VAL_1f, info->values.bds_rang_vals[1], reg_val, 80, exit);
    ARAD_FLD_TO_REG(IQM_FREE_BDB_RANGE_VALUESr, FR_BDB_RNG_VAL_2f, info->values.bds_rang_vals[2], reg_val, 85, exit);
    ARAD_FLD_TO_REG(IQM_FREE_BDB_RANGE_VALUESr, FR_BDB_RNG_VAL_3f, info->values.bds_rang_vals[3], reg_val, 90, exit);
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  100,  exit, ARAD_REG_ACCESS_ERR,WRITE_IQM_FREE_BDB_RANGE_VALUESr(unit, SOC_CORE_ALL,  reg_val));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_sys_red_glob_rcs_set_unsafe()",0,0);
}

/*********************************************************************
*     In the System Red mechanism there is an aspect of
*     Consumed Resources. This mechanism gives the queues a
*     value that is compared with the value of the queue size
*     index - the maximum of the 2 is sent to the threshold
*     tests. The queues are divided to 4 ranges. In 3 types:
*     Free Unicast Data buffers Thresholds, Free Multicast
*     Data buffers Thresholds, Free BD buffers Thresholds.
*     This function determines the thresholds of the ranges
*     and the values of the ranges (0-15).
*     Note that the value of the queue is attributed to the
*     consumed resources (as opposed to the free resources).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_sys_red_glob_rcs_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_SYS_RED_GLOB_RCS_INFO *info
  )
{
  uint32
    index;
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_SYS_RED_GLOB_RCS_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  for (index = 0 ; index < (ARAD_ITM_SYS_RED_BUFFS_RNGS-1); ++index)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->thresholds.unicast_rng_ths[index], ARAD_ITM_SYS_RED_GLOB_RCS_THS_UNICAST_RNG_THS_MAX,
      ARAD_ITM_SYS_RED_GLOB_RCS_RNG_THS_OUT_OF_RANGE_ERR, 10 + index, exit
    );
  }
  for (index = 0 ; index < (ARAD_ITM_SYS_RED_BUFFS_RNGS-1); ++index)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->thresholds.multicast_rng_ths[index], ARAD_ITM_SYS_RED_GLOB_RCS_THS_MULTICAST_RNG_THS_MAX,
      ARAD_ITM_SYS_RED_GLOB_RCS_RNG_THS_OUT_OF_RANGE_ERR, 30 + index, exit
    );
  }
  for (index = 0 ; index < (ARAD_ITM_SYS_RED_BUFFS_RNGS-1); ++index)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->thresholds.bds_rng_ths[index], ARAD_ITM_SYS_RED_GLOB_RCS_THS_BDS_RNG_THS_MAX,
      ARAD_ITM_SYS_RED_GLOB_RCS_THS_OUT_OF_RANGE_ERR, 50 + index, exit
      );
  }

  for (index = 0 ; index < ARAD_ITM_SYS_RED_BUFFS_RNGS; ++index)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->values.unicast_rng_vals[index], ARAD_ITM_SYS_RED_GLOB_RCS_VALS_RNG_VALS_MAX,
      ARAD_ITM_SYS_RED_GLOB_RCS_RNG_VALS_OUT_OF_RANGE_ERR, 70 + index, exit
    );
  }
  for (index = 0 ; index < ARAD_ITM_SYS_RED_BUFFS_RNGS; ++index)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->values.multicast_rng_vals[index], ARAD_ITM_SYS_RED_GLOB_RCS_VALS_RNG_VALS_MAX,
      ARAD_ITM_SYS_RED_GLOB_RCS_RNG_VALS_OUT_OF_RANGE_ERR, 90 + index, exit
    );
  }
  for (index = 0 ; index < ARAD_ITM_SYS_RED_BUFFS_RNGS; ++index)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->values.bds_rang_vals[index], ARAD_ITM_SYS_RED_GLOB_RCS_VALS_RNG_VALS_MAX,
      ARAD_ITM_SYS_RED_GLOB_RCS_RNG_VALS_OUT_OF_RANGE_ERR, 110 + index, exit
    );
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_sys_red_glob_rcs_verify()",0,0);
}

/*********************************************************************
*     In the System Red mechanism there is an aspect of
*     Consumed Resources. This mechanism gives the queues a
*     value that is compared with the value of the queue size
*     index - the maximum of the 2 is sent to the threshold
*     tests. The queues are divided to 4 ranges. In 3 types:
*     Free Unicast Data buffers Thresholds, Free Multicast
*     Data buffers Thresholds, Free BD buffers Thresholds.
*     This function determines the thresholds of the ranges
*     and the values of the ranges (0-15).
*     Note that the value of the queue is attributed to the
*     consumed resources (as opposed to the free resources).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_sys_red_glob_rcs_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT ARAD_ITM_SYS_RED_GLOB_RCS_INFO *info
  )
{
  uint32
    res,
    indx;
   
    

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_SYS_RED_GLOB_RCS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  

   /*
    * Free Unicast Buffers Thresholds
    */
  for (indx = 0 ; indx < (ARAD_ITM_SYS_RED_BUFFS_RNGS-1) ; ++indx)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_FREE_UNICAST_DBUFF_THRESHOLD_0r + indx, REG_PORT_ANY, 0, FR_UN_DB_TH_0f + indx, &info->thresholds.unicast_rng_ths[indx]));

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  12,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_FREE_UNICAST_DBUFF_RANGE_VALUESr, REG_PORT_ANY, 0, FR_UN_DB_RNG_VAL_0f + indx, &info->values.unicast_rng_vals[indx]));
  }
   /*
    * READ Last (Unicast) Value
    */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  14,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_FREE_UNICAST_DBUFF_RANGE_VALUESr, REG_PORT_ANY, 0, FR_UN_DB_RNG_VAL_0f + indx, &info->values.unicast_rng_vals[indx]));

    /*
    * Free Multicast Buffers Thresholds
    */
  for (indx = 0 ; indx < ARAD_ITM_SYS_RED_BUFFS_RNGS-1 ; ++indx)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  16,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_FREE_FULL_MULTICAST_DBUFF_THRESHOLD_0r + indx, REG_PORT_ANY, 0, FR_FL_MUL_DB_TH_0f + indx, &info->thresholds.multicast_rng_ths[indx]));

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  18,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_FREE_FULL_MULTICAST_DBUFF_RANGE_VALUESr, SOC_CORE_ALL, 0, FR_FL_ML_DB_RNG_VAL_0f + indx, &info->values.multicast_rng_vals[indx]));
  }

   /*
    * READ Last (Multicast) Value
    */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_FREE_FULL_MULTICAST_DBUFF_RANGE_VALUESr, SOC_CORE_ALL, 0, FR_FL_ML_DB_RNG_VAL_0f + indx, &info->values.multicast_rng_vals[indx]));

   /*
    * Free BD Buffers Thresholds
    */

  for (indx = 0 ; indx < ARAD_ITM_SYS_RED_BUFFS_RNGS-1 ; ++indx)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  22,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_FREE_BDB_THRESHOLD_0r + indx, REG_PORT_ANY, 0, FR_BDB_TH_0f + indx, &info->thresholds.bds_rng_ths[indx]));

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  24,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_FREE_BDB_RANGE_VALUESr, SOC_CORE_ALL, 0, FR_BDB_RNG_VAL_0f + indx, &info->values.bds_rang_vals[indx]));
  }

   /*
    * READ Last (BDs) Value
    */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  26,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_FREE_BDB_RANGE_VALUESr, SOC_CORE_ALL, 0, FR_BDB_RNG_VAL_0f + indx, &info->values.bds_rang_vals[indx]));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_sys_red_glob_rcs_get_unsafe()",0,0);
}

uint32
  arad_itm_dbuff_size2internal(
    SOC_SAND_IN  ARAD_ITM_DBUFF_SIZE_BYTES dbuff_size,
    SOC_SAND_OUT uint32                   *dbuff_size_internal
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_ITM_DBUFF_SIZE2INTERNAL);

  switch(dbuff_size) {
  case ARAD_ITM_DBUFF_SIZE_BYTES_256:
    *dbuff_size_internal = 0x0;
    break;
  case ARAD_ITM_DBUFF_SIZE_BYTES_512:
    *dbuff_size_internal = 0x1;
    break;
  case ARAD_ITM_DBUFF_SIZE_BYTES_1024:
    *dbuff_size_internal = 0x2;
    break;
  case ARAD_ITM_DBUFF_SIZE_BYTES_2048:
    *dbuff_size_internal = 0x3;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(ARAD_DBUFF_SIZE_INVALID_ERR, 10, exit)
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_dbuff_size2internal()",0,0);
}

uint32
  arad_itm_dbuff_internal2size(
    SOC_SAND_IN  uint32                   dbuff_size_internal,
    SOC_SAND_OUT ARAD_ITM_DBUFF_SIZE_BYTES *dbuff_size
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_ITM_DBUFF_INTERNAL2SIZE);

  switch(dbuff_size_internal) {
  case 0x0:
    *dbuff_size = ARAD_ITM_DBUFF_SIZE_BYTES_256;
    break;
  case 0x1:
    *dbuff_size = ARAD_ITM_DBUFF_SIZE_BYTES_512;
    break;
  case 0x2:
    *dbuff_size = ARAD_ITM_DBUFF_SIZE_BYTES_1024;
    break;
  case 0x3:
    *dbuff_size = ARAD_ITM_DBUFF_SIZE_BYTES_2048;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(ARAD_DBUFF_SIZE_INVALID_ERR, 10, exit)
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_dbuff_internal2size()",0,0);
}

/*
 *    Extension to the Arad API
 */
uint32
  arad_b_itm_glob_rcs_drop_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  ARAD_ITM_GLOB_RCS_DROP_TH *info
  )
{
  uint32
    index;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_B_ITM_GLOB_RCS_DROP_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  /* Excess memory */

  for (index = 0 ; index < ARAD_NOF_DROP_PRECEDENCE; index++)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->mem_excess[index].clear, ARAD_ITM_GLOB_RCS_DROP_EXCESS_MEM_SIZE_MAX,
      ARAD_ITM_GLOB_RCS_DROP_SIZE_OUT_OF_RANGE_ERR, 10, exit
    );

    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->mem_excess[index].set, ARAD_ITM_GLOB_RCS_DROP_EXCESS_MEM_SIZE_MAX,
      ARAD_ITM_GLOB_RCS_DROP_SIZE_OUT_OF_RANGE_ERR, 20, exit
    );
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_b_itm_glob_rcs_drop_verify()",0,0);
}

uint32
  arad_b_itm_glob_rcs_drop_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_THRESH_WITH_HYST_INFO mem_size[ARAD_NOF_DROP_PRECEDENCE],
    SOC_SAND_OUT ARAD_THRESH_WITH_HYST_INFO exact_mem_size[ARAD_NOF_DROP_PRECEDENCE]
  )
{
  uint32
      exp_man,
    res,
    mnt_nof_bits,
    exp_nof_bits;
  uint32
    mnt_val,
    exp_val,
    indx = 0;
   
    

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_GLOB_RCS_DROP_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mem_size);
  SOC_SAND_CHECK_NULL_INPUT(exact_mem_size);

  

  /*
   *  Excess memory size
   */
  mnt_nof_bits = 12;
  exp_nof_bits = soc_reg_field_length(unit, IQM_DRAM_DYN_SIZE_TH_CFG_0r, DRAM_DYN_SIZE_RJCT_SET_TH_0f) - mnt_nof_bits;

  for (indx = 0 ; indx < ARAD_NOF_DROP_PRECEDENCE ; indx++)
  {
      res = soc_sand_break_to_mnt_exp_round_up(
              mem_size[indx].clear,
              mnt_nof_bits,
              exp_nof_bits,
              0,
              &mnt_val,
              &exp_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      exact_mem_size[indx].clear = (mnt_val) * (1<<(exp_val));

      arad_iqm_mantissa_exponent_set(
          unit,
          mnt_val,
          exp_val,
          mnt_nof_bits,
          &exp_man
        );
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_DRAM_DYN_SIZE_TH_CFG_0r + indx, REG_PORT_ANY,  0, DRAM_DYN_SIZE_RJCT_CLR_TH_0f + indx,  exp_man));

      res = soc_sand_break_to_mnt_exp_round_up(
              mem_size[indx].set,
              mnt_nof_bits,
              exp_nof_bits,
              0,
              &mnt_val,
              &exp_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

      exact_mem_size[indx].set = (mnt_val) * (1<<(exp_val));

      arad_iqm_mantissa_exponent_set(
          unit,
          mnt_val,
          exp_val,
          mnt_nof_bits,
          &exp_man
        );
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_DRAM_DYN_SIZE_TH_CFG_0r + indx, REG_PORT_ANY,  0, DRAM_DYN_SIZE_RJCT_SET_TH_0f + indx,  exp_man));
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_glob_rcs_drop_set_unsafe()", indx, 0);
}

uint32
  arad_b_itm_glob_rcs_drop_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT ARAD_THRESH_WITH_HYST_INFO mem_size[ARAD_NOF_DROP_PRECEDENCE]
  )
{
  uint32
    res,
    exp_man = 0,
    mnt_nof_bits,
    mnt_val,
    exp_val;
   uint32
    indx;
   
    

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_GLOB_RCS_DROP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mem_size);

  

  /*
   * Excess memory
   */
  mnt_nof_bits = 12;

  for (indx = 0 ; indx < ARAD_NOF_DROP_PRECEDENCE ; indx++)
  {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_DRAM_DYN_SIZE_TH_CFG_0r + indx, REG_PORT_ANY,  0, DRAM_DYN_SIZE_RJCT_CLR_TH_0f + indx, &exp_man));

      arad_iqm_mantissa_exponent_get(
          unit,
          exp_man,
          mnt_nof_bits,
          &mnt_val,
          &exp_val
      );

    mem_size[indx].clear = (mnt_val) * (1<<(exp_val));

    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_DRAM_DYN_SIZE_TH_CFG_0r + indx, REG_PORT_ANY,  0, DRAM_DYN_SIZE_RJCT_SET_TH_0f + indx, &exp_man));
    arad_iqm_mantissa_exponent_get(
        unit,
        exp_man,
        mnt_nof_bits,
        &mnt_val,
        &exp_val
    );

    mem_size[indx].set = (mnt_val) * (1<<(exp_val));
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_glob_rcs_drop_get_unsafe()",0,0);

}

/*********************************************************************
*     Set the size of committed queue size (i.e., the
 *     guaranteed memory) for each VOQ, even in the case that a
 *     set of queues consume most of the memory resources.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_committed_q_size_set_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  rt_cls_ndx,
    SOC_SAND_IN  SOC_TMC_ITM_GUARANTEED_INFO *info, 
    SOC_SAND_OUT SOC_TMC_ITM_GUARANTEED_INFO *exact_info 
  )
{
  uint32
    data,
    exp_man,
    res = SOC_SAND_OK;
  uint32
    mnt = 0,
    exp = 0;
   
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_COMMITTED_Q_SIZE_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);

  SOC_SAND_SOC_IF_ERROR_RETURN(res, 100, exit, READ_IQM_GRSPRMm(unit, MEM_BLOCK_ANY, rt_cls_ndx, &data));

  if (info->guaranteed_size != SOC_TMC_ITM_GUARANTEED_INFO_DO_NOT_SET) {
    /* Compute the bytes exponent and the mantissa */
    res = soc_sand_break_to_mnt_exp_round_up(
            info->guaranteed_size / ARAD_ITM_COMMITTED_BYTES_RESOLUTION,
            ARAD_ITM_COMMITTED_BYTES_MANTISSA_NOF_BITS,
            ARAD_ITM_COMMITTED_BYTES_EXPONENT_NOF_BITS,
            0, &mnt, &exp
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);

    /* Write them according to the rate class entry */
    arad_iqm_mantissa_exponent_set(unit, mnt, exp, ARAD_ITM_COMMITTED_BYTES_MANTISSA_NOF_BITS, &exp_man);
    soc_IQM_GRSPRMm_field32_set(unit, &data, GRNT_BYTES_THf, exp_man);
    exact_info->guaranteed_size = mnt * (ARAD_ITM_COMMITTED_BYTES_RESOLUTION << exp);
  } else {
    exp_man = soc_IQM_GRSPRMm_field32_get(unit, &data, GRNT_BYTES_THf);
    arad_iqm_mantissa_exponent_get(unit, exp_man, ARAD_ITM_COMMITTED_BYTES_MANTISSA_NOF_BITS, &mnt, &exp);
    exact_info->guaranteed_size = mnt * (ARAD_ITM_COMMITTED_BYTES_RESOLUTION << exp);
  }

  if (info->guaranteed_size_bds != SOC_TMC_ITM_GUARANTEED_INFO_DO_NOT_SET) {
    /* Compute the buffer descriptors exponent and the mantissa */
    res = soc_sand_break_to_mnt_exp_round_up(
            info->guaranteed_size_bds,
            ARAD_ITM_COMMITTED_BDS_MANTISSA_NOF_BITS,
            ARAD_ITM_COMMITTED_BDS_EXPONENT_NOF_BITS,
            0, &mnt, &exp
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 300, exit);

    /* Write them according to the rate class entry */
    arad_iqm_mantissa_exponent_set(unit, mnt, exp, ARAD_ITM_COMMITTED_BDS_MANTISSA_NOF_BITS, &exp_man);
    soc_IQM_GRSPRMm_field32_set(unit, &data, GRNT_BUFFS_THf, exp_man);
    exact_info->guaranteed_size_bds = mnt << exp;
  } else {
    exp_man = soc_IQM_GRSPRMm_field32_get(unit, &data, GRNT_BUFFS_THf);
    arad_iqm_mantissa_exponent_get(unit, exp_man, ARAD_ITM_COMMITTED_BDS_MANTISSA_NOF_BITS, &mnt, &exp);
    exact_info->guaranteed_size_bds = mnt << exp;
  }

  if ((info->guaranteed_size != SOC_TMC_ITM_GUARANTEED_INFO_DO_NOT_SET) ||
      (info->guaranteed_size_bds != SOC_TMC_ITM_GUARANTEED_INFO_DO_NOT_SET)) {
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 400, exit, WRITE_IQM_GRSPRMm(unit, MEM_BLOCK_ANY, rt_cls_ndx, &data));
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_itm_committed_q_size_set_unsafe()", 0, 0);
}

uint32
  arad_itm_committed_q_size_set_verify(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  rt_cls_ndx,
    SOC_SAND_IN  SOC_TMC_ITM_GUARANTEED_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_COMMITTED_Q_SIZE_SET_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_ERR_IF_ABOVE_MAX(rt_cls_ndx, ARAD_ITM_QT_RT_CLS_MAX, ARAD_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 10, exit);
  if (info->guaranteed_size != SOC_TMC_ITM_GUARANTEED_INFO_DO_NOT_SET) {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->guaranteed_size, ARAD_ITM_GRNT_BYTES_MAX, ARAD_ITM_GRNT_BYTES_OUT_OF_RANGE_ERR, 11, exit);
  }
  if (info->guaranteed_size_bds != SOC_TMC_ITM_GUARANTEED_INFO_DO_NOT_SET) {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->guaranteed_size_bds, ARAD_ITM_GRNT_BDS_MAX, ARAD_ITM_GRNT_BDS_OUT_OF_RANGE_ERR, 12, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_itm_committed_q_size_set_verify()", rt_cls_ndx, 0);
}

uint32
  arad_itm_committed_q_size_get_verify(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  uint32        rt_cls_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_COMMITTED_Q_SIZE_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(rt_cls_ndx, ARAD_ITM_QT_RT_CLS_MAX, ARAD_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_itm_committed_q_size_get_verify()", rt_cls_ndx, 0);
}

/*********************************************************************
*     Set the size of committed queue size (i.e., the
 *     guaranteed memory) for each VOQ, even in the case that a
 *     set of queues consume most of the memory resources.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_committed_q_size_get_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  rt_cls_ndx,
    SOC_SAND_OUT SOC_TMC_ITM_GUARANTEED_INFO *exact_info
  )
{
  uint32
      exp_man,
      data,
    mnt = 0,
    exp = 0;
  uint32 res;
   
    
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_COMMITTED_Q_SIZE_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);

  /*
   *    Exact the exponent and the mantissa to get the exact value
   */
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 1210, exit, READ_IQM_GRSPRMm(unit, MEM_BLOCK_ANY, rt_cls_ndx, &data));

  exp_man = soc_IQM_GRSPRMm_field32_get(unit, &data, GRNT_BYTES_THf);
  arad_iqm_mantissa_exponent_get(unit, exp_man, ARAD_ITM_COMMITTED_BYTES_MANTISSA_NOF_BITS, &mnt, &exp);
  exact_info->guaranteed_size = mnt * (ARAD_ITM_COMMITTED_BYTES_RESOLUTION << exp);

  exp_man = soc_IQM_GRSPRMm_field32_get(unit, &data, GRNT_BUFFS_THf);
  arad_iqm_mantissa_exponent_get(unit, exp_man, ARAD_ITM_COMMITTED_BDS_MANTISSA_NOF_BITS, &mnt, &exp);
  exact_info->guaranteed_size_bds = mnt << exp;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_itm_committed_q_size_get_unsafe()", 0, 0);
}

soc_error_t
arad_itm_pfc_tc_map_set_unsafe(const int unit, const int tc_in, const int port_id, const int tc_out)
{
  int line_id;
  uint32 is_valid;
  uint32 nif_port;
  uint32 mem_val;
  int temp_tc_out = tc_out;
  SOCDNX_INIT_FUNC_DEFS;

  if(port_id < 0 || port_id >= ARAD_ITM_NOF_REASSEMBLY_CONTEXTS) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid port index %d, it should be between 0 to 191\n"),port_id));
  }
  if(tc_in <0 || tc_in > 7) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid tc_in index %d, it should be between 0 to 7\n"),tc_in));
  }
  if(tc_out <0 || tc_out > 7) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid tc_out %d, it should be between 0 to 7\n"),tc_out));
  }

  SOCDNX_IF_ERR_EXIT(READ_IQM_ITMPMm(unit, MEM_BLOCK_ANY, port_id, &mem_val));
  soc_mem_field_get(unit, IQM_ITMPMm, &mem_val, NIF_PORT_VALIDf, &is_valid);
  if(!is_valid) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid port %d\n"),port_id));
  }
  soc_mem_field_get(unit, IQM_ITMPMm, &mem_val, NIF_PORTf, &nif_port);
  line_id = (nif_port <<3) | tc_in;
  SOCDNX_IF_ERR_EXIT(WRITE_IQM_NIFTCMm(unit, MEM_BLOCK_ANY, line_id, &temp_tc_out));

exit:
  SOCDNX_FUNC_RETURN;
}

soc_error_t
arad_itm_pfc_tc_map_get_unsafe(const int unit, const int tc_in, const int port_id, int *tc_out)
{
  int line_id;
  uint32 is_valid;
  uint32 nif_port;
  uint32 mem_val;
  SOCDNX_INIT_FUNC_DEFS;

  if(port_id < 0 || port_id >= ARAD_ITM_NOF_REASSEMBLY_CONTEXTS) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid port index %d, it should be between 0 to 191\n"),port_id));
  }
  if(tc_in <0 || tc_in > 7) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid tc_in index %d, it should be between 0 to 7\n"),tc_in));
  }
  
  SOCDNX_IF_ERR_EXIT(READ_IQM_ITMPMm(unit, MEM_BLOCK_ANY, port_id, &mem_val));
  soc_mem_field_get(unit, IQM_ITMPMm, &mem_val, NIF_PORT_VALIDf, &is_valid);
  if(!is_valid) {
    SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Invalid port %d\n"),port_id));
  }
  soc_mem_field_get(unit, IQM_ITMPMm, &mem_val, NIF_PORTf, &nif_port);
  line_id = (nif_port <<3) | tc_in;
  SOCDNX_IF_ERR_EXIT(READ_IQM_NIFTCMm(unit, MEM_BLOCK_ANY, line_id, tc_out));

exit:
  SOCDNX_FUNC_RETURN;
}

uint32
  arad_itm_dp_discard_set_verify(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  uint32        discard_dp
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_DISCARD_DP_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(discard_dp, ARAD_ITM_DISCARD_DP_MAX, ARAD_ITM_DP_DISCARD_OUT_OF_RANGE_ERR, 5, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_itm_dp_discard_set_verify()", discard_dp, 0);
}

/*********************************************************************
*     Set ECN as enabled or disabled for the device
*********************************************************************/
uint32
  arad_itm_enable_ecn_unsafe(
    SOC_SAND_IN  int   unit,
    SOC_SAND_IN  uint32   enabled /* ECN will be enabled/disabled for non zero/zero values */
  )
{
  uint32 res, enable_bit = enabled ? 1 : 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_IQM_ENABLERSr, SOC_CORE_ALL, 0, ECN_ENABLEf,  enable_bit)); /* Enables not dropping ECN capable packers due to WRED and system RED */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_ECN_REMARKr, SOC_CORE_ALL, 0, ECN_REMARKf,  enable_bit)); /* Enables marking congestion on ECN capable packets */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_itm_enable_ecn_unsafe()", unit, enabled);
}

/*********************************************************************
*     Return if ECN is enabled for the device
*********************************************************************/
uint32
  arad_itm_get_ecn_enabled_unsafe(
    SOC_SAND_IN  int   unit,
    SOC_SAND_OUT uint32   *enabled /* will return non zero if /ECN is enabled */
  )
{
  uint32 res, val;
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(enabled);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 10, exit, ARAD_REG_ACCESS_ERR, READ_EPNI_ECN_REMARKr(unit, REG_PORT_ANY, &val));
  *enabled = soc_reg_field_get(unit, EPNI_ECN_REMARKr, val, ECN_REMARKf);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_itm_enable_ecn_unsafe()", unit, 0);
}

/*********************************************************************
*     Set the drop precedence value above which 
*     all packets will always be discarded.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_dp_discard_set_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  uint32                  discard_dp
  )
{     
  uint32 res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_DISCARD_DP_SET_UNSAFE);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_IQM_ENABLERSr, SOC_CORE_ALL, 0, DSCRD_DPf,  discard_dp));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_itm_dp_discard_set_unsafe()", 0, 0);
}

/*********************************************************************
*     Get the drop precedence value above which 
*     all packets will always be discarded.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_itm_dp_discard_get_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_OUT uint32                  *discard_dp
  )
{     
  uint32 res, fld_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_ITM_DISCARD_DP_GET_UNSAFE);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  15,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_IQM_ENABLERSr, SOC_CORE_ALL, 0, DSCRD_DPf, &fld_val));
  *discard_dp = fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_itm_dp_discard_get_unsafe()", 0, 0);
}

uint32 
  arad_itm_setup_dp_map(
    SOC_SAND_IN  int unit
  )
{
  uint32
    dp_cmnd_indx,
    iqm_dp_indx,
    mtr_res_indx,
    mapped_mtr_res_indx,
    entry_indx=0,
    iqm_dp,
    etm_de,
    tbl_data,
    egress_divisor,
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  /* If there is a PB in the system, then the Egress DP is 1 bit. */
  egress_divisor = SOC_DPP_CONFIG(unit)->tm.is_petrab_in_system ? 2 : 1;
     
#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit) && soc_property_get(unit, spn_POLICER_COLOR_RESOLUTION_MODE, 0)) {
    /* If the detailed color mode is active then we ignore PB. */
    egress_divisor = 1;
  }
#endif /* BCM_88660_A0 */

  /* meter command */
  for (dp_cmnd_indx = 0; dp_cmnd_indx < ARAD_PP_NOF_MTR_RES_USES; ++dp_cmnd_indx)  {
    /* incoming DP */
    for (iqm_dp_indx = 0; iqm_dp_indx <= ARAD_PP_DP_MAX_VAL; ++iqm_dp_indx)  {
      /* meter-processor-dp (resolved DP) */
      for (mtr_res_indx = 0; mtr_res_indx <= ARAD_PP_DP_MAX_VAL; ++mtr_res_indx) {
        
        if (SOC_DPP_CONFIG(unit)->arad->init.pp_enable) {
          mapped_mtr_res_indx = mtr_res_indx;

          if (soc_property_suffix_num_get(unit, 0, "custom_feature", "always_map_result_dp_2_to_1", 0) == 1) {
            if (mtr_res_indx == 2) {
              mapped_mtr_res_indx = 1;
            }
          }

#ifdef BCM_88660_A0
          if (SOC_IS_ARADPLUS(unit) && soc_property_get(unit, spn_POLICER_COLOR_RESOLUTION_MODE, 0)) {
            /* If the detailed color mode is active then mtr red (=1) becomes 2, mtr yellow (=2) becomes 1, and eth red (=3) becomes 3. */
            if (mtr_res_indx == 1) {
              mapped_mtr_res_indx = 2;
            }
            
            if (mtr_res_indx == 2) {
              mapped_mtr_res_indx = 1;
            };
            
            if (mtr_res_indx == 3) {
              mapped_mtr_res_indx = 3;
            }
            
          }
#endif /* BCM_88660_A0 */

          /* PP Mode */
          if (dp_cmnd_indx == ARAD_PP_MTR_RES_USE_NONE) {
            /* meter command has no effect */ 
    	      /* ingress DP = egress DP = incoming DP */
            iqm_dp = iqm_dp_indx;
            etm_de = iqm_dp_indx/egress_divisor;
          } else if (dp_cmnd_indx == ARAD_PP_MTR_RES_USE_OW_DP) {
            /* meter command affects ingress */
            iqm_dp = mapped_mtr_res_indx; /* ingress DP = meter resolved DP (Metering result overwrite the DP) */
            etm_de = iqm_dp_indx/egress_divisor; /* egress DP = incoming DP */
          } else if (dp_cmnd_indx == ARAD_PP_MTR_RES_USE_OW_DE) {
            /* meter command affects egress */
    	      iqm_dp = iqm_dp_indx; /* ingress DP = incoming DP */
            etm_de = mapped_mtr_res_indx/egress_divisor; /* egress DP = meter resolved DP (Metering result overwrite the DP) */
          } else {
            /* meter command affects both ingress and egress */
            /* ingress DP = egress DP = meter resolved DP */
            iqm_dp = mapped_mtr_res_indx;
            etm_de = mapped_mtr_res_indx/egress_divisor;
          }
        } else {
          /* TM Mode */
          /* meter command has no effect */ 
          iqm_dp = iqm_dp_indx;
          etm_de = iqm_dp_indx/egress_divisor;
        }

        res = READ_IDR_DROP_PRECEDENCE_MAPPINGm(unit, MEM_BLOCK_ANY, entry_indx, &tbl_data);
        SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

        soc_mem_field_set(unit, IDR_DROP_PRECEDENCE_MAPPINGm, &tbl_data, INGRESS_DROP_PRECEDENCEf, &iqm_dp);
        soc_mem_field_set(unit, IDR_DROP_PRECEDENCE_MAPPINGm, &tbl_data, EGRESS_DROP_PRECEDENCEf, &etm_de);

        res = WRITE_IDR_DROP_PRECEDENCE_MAPPINGm(unit, MEM_BLOCK_ANY, entry_indx, &tbl_data);
        SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
        
        ++entry_indx;
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_itm_setup_dp_map()", 0, 0);
}

/*
 * Set the alpha value of fair adaptive tail drop for the given rate class and DP.
 * Arad+ only.
 */
uint32
    arad_plus_itm_alpha_set_unsafe(
      SOC_SAND_IN  int       unit,
      SOC_SAND_IN  uint32       rt_cls_ndx,
      SOC_SAND_IN  uint32       drop_precedence_ndx,
      SOC_SAND_IN  int32        alpha 
    )
{
    uint32 data = 0, reg_val = 0, temp_val = 0;
    uint32 res;
    int need_workaround = (SOC_IS_ARADPLUS_A0(unit) || SOC_IS_ARDON(unit));
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    if (alpha >= 0) {
        soc_mem_field32_set(unit, IQM_FRDMTm, &data, FR_RSRC_MAX_TH_VALf, alpha);
    } else {
        soc_mem_field32_set(unit, IQM_FRDMTm, &data, FR_RSRC_MAX_TH_VALf, -alpha);
        soc_mem_field32_set(unit, IQM_FRDMTm, &data, FR_RSRC_MAX_TH_SIGNf, 1);
    }

    if (need_workaround) {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 80, exit, READ_IQM_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, REG_PORT_ANY, &reg_val));
        temp_val = reg_val;
        soc_reg_field_set(unit, IQM_ENABLE_DYNAMIC_MEMORY_ACCESSr, &temp_val, ENABLE_DYNAMIC_MEMORY_ACCESSf, 1);
        if (reg_val == temp_val) {
            need_workaround = 0;
        } else {
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 90, exit, WRITE_IQM_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, REG_PORT_ANY, temp_val));
        }
    }

    SOC_SAND_SOC_IF_ERROR_RETURN(res, 100, exit, WRITE_IQM_FRDMTm(unit, MEM_BLOCK_ANY, rt_cls_ndx * SOC_TMC_NOF_DROP_PRECEDENCE + drop_precedence_ndx, &data));

    if (need_workaround) {
        SOC_SAND_SOC_IF_ERROR_RETURN(res, 110, exit, WRITE_IQM_ENABLE_DYNAMIC_MEMORY_ACCESSr(unit, REG_PORT_ANY, reg_val));
    }

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_plus_itm_alpha_set_unsafe()", rt_cls_ndx, drop_precedence_ndx);
}

/*
 * Get the alpha value of fair adaptive tail drop for the given rate class and DP.
 * Arad+ only.
 */
uint32
    arad_plus_itm_alpha_get_unsafe(
      SOC_SAND_IN  int       unit,
      SOC_SAND_IN  uint32       rt_cls_ndx,
      SOC_SAND_IN  uint32       drop_precedence_ndx,
      SOC_SAND_OUT int32        *alpha 
    )
{
    uint32 data = 0, res;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(alpha);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 100, exit, READ_IQM_FRDMTm(unit, MEM_BLOCK_ANY, rt_cls_ndx * SOC_TMC_NOF_DROP_PRECEDENCE + drop_precedence_ndx, &data));
    res = soc_mem_field32_get(unit, IQM_FRDMTm, &data, FR_RSRC_MAX_TH_VALf);
    *alpha = soc_mem_field32_get(unit, IQM_FRDMTm, &data, FR_RSRC_MAX_TH_SIGNf) ? -res : res;
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_plus_itm_alpha_get_unsafe()", rt_cls_ndx, drop_precedence_ndx);
}



/*
 * Arad+ only: enable/disable fair adaptive tail drop (Free BDs dynamic MAX queue size)
 */
uint32
  arad_plus_itm_fair_adaptive_tail_drop_enable_set_unsafe(
    SOC_SAND_IN  int   unit,
    SOC_SAND_IN  uint8    enabled /* 0=disabled, non zero=enabled */
  )
{
    uint32 res, reg_val;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 150, exit, READ_IQM_FR_RSRC_DYN_TH_SETTINGSr(unit, REG_PORT_ANY, &reg_val));
    soc_reg_field_set(unit, IQM_FR_RSRC_DYN_TH_SETTINGSr, &reg_val, FR_RSRC_DYN_TH_ENABLEf, enabled ? 1 : 0);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 160, exit, WRITE_IQM_FR_RSRC_DYN_TH_SETTINGSr(unit, REG_PORT_ANY, reg_val));

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_fair_adaptive_tail_drop_enable_set_unsafe()", unit, enabled);
}

/*
 * Arad+ only: Check if fair adaptive tail drop (Free BDs dynamic MAX queue size) is enabled.
 */
uint32
  arad_plus_itm_fair_adaptive_tail_drop_enable_get_unsafe(
    SOC_SAND_IN  int   unit,
    SOC_SAND_OUT uint8    *enabled /* return value: 0=disabled, 1=enabled */
  )
{
    uint32 res, reg_val;
    SOC_SAND_INIT_ERROR_DEFINITIONS(0);
    SOC_SAND_CHECK_NULL_INPUT(enabled);
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 150, exit, READ_IQM_FR_RSRC_DYN_TH_SETTINGSr(unit, REG_PORT_ANY, &reg_val));
    *enabled = soc_reg_field_get(unit, IQM_FR_RSRC_DYN_TH_SETTINGSr, reg_val, FR_RSRC_DYN_TH_ENABLEf);

exit:
    SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_itm_fair_adaptive_tail_drop_enable_get_unsafe()", unit, 0);
}


/*
Get Arad ingress congestion statistics.
*/
uint32 arad_itm_congestion_statistics_get_unsafe(
  SOC_SAND_IN int unit,
  SOC_SAND_OUT ARAD_ITM_CGM_CONGENSTION_STATS *stats /* place current statistics output here */
  )
{
  uint32 res, fld_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(stats);

  /* collect current value statistics */  
  /* Current number of free BDBs(buffer descriptors buffers) */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_OCCUPIED_UNICAST_DBUFFS_COUNTERr, REG_PORT_ANY, 0, OC_DB_COUNT_UCf, &fld_val));
  stats->bdb_free = fld_val; /* place the value into the 32 bits integer */

  /* Current number of occupied BDs(Buffer-Descriptor or PDM entries) */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_GLOBAL_RESOURCE_COUNTERS_BDr, REG_PORT_ANY, 0, OC_BD_COUNTf, &fld_val));
  stats->bd_occupied = fld_val; /* place the value into the 32 bits integer */	

  /* Current number of free(available) BDs(Buffer-Descriptor or PDM entries) */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_GLOBAL_RESOURCE_COUNTERS_BD_2r, REG_PORT_ANY, 0, FR_BD_COUNTf, &fld_val));
  stats->bd2_free = fld_val; /* place the value into the 32 bits integer */		

  /* Current number of occupied Unicast Type Dbuffs */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_OCCUPIED_UNICAST_DBUFFS_COUNTERr, REG_PORT_ANY, 0, OC_DB_COUNT_UCf, &fld_val));
  stats->db_uni_occupied = fld_val; /* place the value into the 32 bits integer */		

  /* Current number of free Unicast Type Dbuffs */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_FREE_UNICAST_DBUFFS_COUNTERr, REG_PORT_ANY, 0, FR_DB_COUNT_UCf, &fld_val));
  stats->db_uni_free = fld_val; /* place the value into the 32 bits integer */		

  /* Current number of free Full-Multicast Type Dbuffs */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  60,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_FREE_FULL_MULTICAST_DBUFFS_COUNTERr, REG_PORT_ANY, 0, FR_DB_COUNT_FLMCf, &fld_val));
  stats->db_full_mul_free = fld_val; /* place the value into the 32 bits integer */	

  /* Current number of free Mini-Multicast Type Dbuffs */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  70,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_FREE_MINI_MULTICAST_DBUFFS_COUNTERr, REG_PORT_ANY, 0, FR_DB_COUNT_MNMCf, &fld_val));
  stats->db_mini_mul_free = fld_val; /* place the value into the 32 bits integer */		

  /* Free BDBs minumum occupancy indication */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  80,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_GLOBAL_RESOURCE_MINIMUM_OCCUPANCYr, REG_PORT_ANY, 0, FREE_BDB_MIN_OCf, &fld_val));
  stats->free_bdb_mini_occu = fld_val; /* place the value into the 32 bits integer */		

  /* Free Unicast Type Dbuffs minimal occupancy level */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  90,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_FREE_UNICAST_DBUFFS_MINIMUM_OCCUPANCYr, REG_PORT_ANY, 0, FR_DB_MIN_OCf, &fld_val));
  stats->free_db_uni_mini_occu = fld_val; /* place the value into the 32 bits integer */		
	
  /* Free Full-Multicast Type Dbuffs minimal occupancy level */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  100,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_FREE_FULL_MULTICAST_DBUFFS_MINIMUM_OCCUPANCYr, REG_PORT_ANY, 0, FR_DB_FLMC_MIN_OCf, &fld_val));
  stats->free_bdb_full_mul_mini_occu = fld_val; /* place the value into the 32 bits integer */		

  /* Free Mini-Multicast Type Dbuffs minimal occupancy level */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  110,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_FREE_MINI_MULTICAST_DBUFFS_MINIMUM_OCCUPANCYr, REG_PORT_ANY, 0, FR_DB_MNMC_MIN_OCf, &fld_val));
  stats->free_bdb_mini_mul_mini_occu = fld_val; /* place the value into the 32 bits integer */		
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_itm_congestion_statistics_get_unsafe()", 0, 0);
}
#undef _ERR_MSG_MODULE_NAME

#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif /* of #if defined(BCM_88650_A0) */
