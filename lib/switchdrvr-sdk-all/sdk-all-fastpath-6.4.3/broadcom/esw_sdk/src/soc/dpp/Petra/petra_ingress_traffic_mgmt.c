/* $Id: petra_ingress_traffic_mgmt.c,v 1.17 Broadcom SDK $
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


/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_ingress_traffic_mgmt.h>

#include <soc/dpp/Petra/petra_ingress_packet_queuing.h>
#include <soc/dpp/Petra/petra_ingress_header_parsing.h>

#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_tbl_access.h>
#include <soc/dpp/Petra/petra_general.h>
#include <soc/dpp/Petra/petra_api_mgmt.h>
#include <soc/dpp/Petra/petra_sw_db.h>
#include <soc/dpp/Petra/petra_chip_tbls.h>
#include <soc/dpp/Petra/petra_fabric.h>


#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/SAND/Utils/sand_u64.h>

#ifdef LINK_PB_LIBRARIES
#include <soc/dpp/Petra/PB_TM/pb_ingress_traffic_mgmt.h>
#include <soc/dpp/Petra/PB_TM/pb_parser.h>
#endif /* LINK_PB_LIBRARIES */


/* } */

/*************
 * DEFINES   *
 *************/
/* { */

/* $Id: petra_ingress_traffic_mgmt.c,v 1.17 Broadcom SDK $
 *
 */


/*
 * max value for BDs size( for tail drop)
 */
#define SOC_PETRA_ITM_MAX_INST_Q_BDS_SIZE 0x3F80000

/* Max & min values for end of category queues:      */
#define SOC_PETRA_ITM_CATEGORY_INFO_VSQ_CTGRY_END_MIN 0
#define SOC_PETRA_ITM_CATEGORY_INFO_VSQ_CTGRY_END_MAX SOC_PETRA_NOF_QUEUES-1


/* Max & min values for struct SOC_PETRA_ITM_WRED_QT_DP_INFO:      */
#define SOC_PETRA_ITM_WRED_QT_DP_INFO_MAX_PROBABILITY_MAX 100

/* Max value WRED max packet size:      */
#define SOC_PETRA_ITM_WRED_MAX_PACKET_SIZE ((1<<14)-128)
#define SOC_PETRA_ITM_WRED_MAX_PACKET_SIZE_FOR_CALC SOC_PETRA_ITM_WRED_MAX_PACKET_SIZE

/* Max values WRED probability:      */
#define SOC_PETRA_ITM_WRED_MAX_PROB 100

#define SOC_PETRA_ITM_STATISTICS_TAG_NOF_BITS 31

/* Max & min values for struct SOC_PETRA_ITM_PRIORITY_MAP_TMPLT:      */
#define SOC_PETRA_ITM_PRIORITY_MAP_NDX_MAX 3

#define SOC_PETRA_ITM_PRIORITY_MAP_TMPLT_MAP_MAX SOC_SAND_U32_MAX

/* System Red drop probability values. Range: 0 - 10000,
 * when 1% is 100.
 */
#define SOC_PETRA_ITM_SYS_RED_DROP_PROB_VAL_MAX 10000

#define SOC_PETRA_ITM_SYS_RED_QUEUE_TH_MAX 15

/* Max & min values for struct SOC_PETRA_ITM_SYS_RED_EG_INFO for CLK=250M (4ns) :
    This is the register's max, to see the actual max value in milliseconds please refer to function
    soc_petra_itm_sys_red_eg_verify */
#define SOC_PETRA_ITM_SYS_RED_EG_INFO_AGING_TIMER_MAX 0x1FFFFF

/* Aging Timer Coefficient is use to convert register value to seconds */
#define SOC_PETRA_ITM_SYS_RED_EG_INFO_AGING_TIMER_CONVERT_COEFFICIENT (1296 * 4)

/* Max & min values for struct SOC_PETRA_ITM_SYS_RED_GLOB_RCS_THS:      */
#define SOC_PETRA_ITM_SYS_RED_GLOB_RCS_THS_UNICAST_RNG_THS_MAX 0x1FFFFF

#define SOC_PA_ITM_SYS_RED_GLOB_RCS_THS_MULTICAST_RNG_THS_MAX 0x1FFF

#ifdef LINK_PB_LIBRARIES
#define SOC_PB_ITM_SYS_RED_GLOB_RCS_THS_MULTICAST_RNG_THS_MAX 0xFFFF
#else
#define SOC_PB_ITM_SYS_RED_GLOB_RCS_THS_MULTICAST_RNG_THS_MAX SOC_PA_ITM_SYS_RED_GLOB_RCS_THS_MULTICAST_RNG_THS_MAX
#endif

#define SOC_PETRA_ITM_SYS_RED_GLOB_RCS_THS_MULTICAST_RNG_THS_MAX \
    SOC_PETRA_CST_VALUE_DISTINCT(ITM_SYS_RED_GLOB_RCS_THS_MULTICAST_RNG_THS_MAX, uint32)

#define SOC_PETRA_ITM_SYS_RED_GLOB_RCS_THS_BDS_RNG_THS_MAX 0xFFFF

/* Max & min values for struct SOC_PETRA_ITM_SYS_RED_GLOB_RCS_VALS:      */
#define SOC_PETRA_ITM_SYS_RED_GLOB_RCS_VALS_RNG_VALS_MAX 15

#define SOC_PETRA_ITM_CR_DISCOUNT_MAX_VAL  127
#define SOC_PETRA_ITM_CR_DISCOUNT_MIN_VAL  -127

#define SOC_PETRA_ITM_HUNGRY_TH_MNT_MSB       6
#define SOC_PETRA_ITM_HUNGRY_TH_MNT_LSB       4
#define SOC_PETRA_ITM_HUNGRY_TH_MNT_NOF_BITS  (SOC_PETRA_ITM_HUNGRY_TH_MNT_MSB - SOC_PETRA_ITM_HUNGRY_TH_MNT_LSB + 1)
#define SOC_PETRA_ITM_HUNGRY_TH_EXP_MSB       3
#define SOC_PETRA_ITM_HUNGRY_TH_EXP_LSB       0
#define SOC_PETRA_ITM_HUNGRY_TH_EXP_NOF_BITS  (SOC_PETRA_ITM_HUNGRY_TH_EXP_MSB - SOC_PETRA_ITM_HUNGRY_TH_EXP_LSB + 1)

#define SOC_PETRA_ITM_HUNGRY_TH_MNT_MAX     ((1<<SOC_PETRA_ITM_HUNGRY_TH_MNT_NOF_BITS)-1)
#define SOC_PETRA_ITM_HUNGRY_TH_EXP_MAX     ((1<<SOC_PETRA_ITM_HUNGRY_TH_EXP_NOF_BITS)-1)
#define SOC_PETRA_ITM_HUNGRY_TH_MAX         \
  (SOC_PETRA_ITM_HUNGRY_TH_MNT_MAX * (1<<SOC_PETRA_ITM_HUNGRY_TH_EXP_MAX))

#define SOC_PETRA_ITM_HUNGRY_TH_MULTIPLIER_VAL_TO_FIELD(val)             \
  ((val) < (1 << (SOC_PETRA_ITM_HUNGRY_TH_MULTIPLIER_OFFSET + 1)) ? 0 : \
  soc_sand_log2_round_down(val) - SOC_PETRA_ITM_HUNGRY_TH_MULTIPLIER_OFFSET)
#define SOC_PETRA_ITM_HUNGRY_TH_MULTIPLIER_FIELD_TO_VAL(field)          \
  (field ? SOC_SAND_BIT(field + SOC_PETRA_ITM_HUNGRY_TH_MULTIPLIER_OFFSET) : 0);

#define SOC_PETRA_ITM_SATISFIED_TH_MNT_MSB       7
#define SOC_PETRA_ITM_SATISFIED_TH_MNT_LSB       4
#define SOC_PETRA_ITM_SATISFIED_TH_MNT_NOF_BITS  (SOC_PETRA_ITM_SATISFIED_TH_MNT_MSB - SOC_PETRA_ITM_SATISFIED_TH_MNT_LSB + 1)
#define SOC_PETRA_ITM_SATISFIED_TH_EXP_MSB       3
#define SOC_PETRA_ITM_SATISFIED_TH_EXP_LSB       0
#define SOC_PETRA_ITM_SATISFIED_TH_EXP_NOF_BITS  (SOC_PETRA_ITM_SATISFIED_TH_EXP_MSB - SOC_PETRA_ITM_SATISFIED_TH_EXP_LSB + 1)

#define SOC_PETRA_ITM_SATISFIED_TH_MNT_MAX     ((1<<SOC_PETRA_ITM_SATISFIED_TH_MNT_NOF_BITS)-1)
#define SOC_PETRA_ITM_SATISFIED_TH_EXP_MAX     ((1<<SOC_PETRA_ITM_SATISFIED_TH_EXP_NOF_BITS)-1)
#define SOC_PETRA_ITM_SATISFIED_TH_MAX         \
  (SOC_PETRA_ITM_SATISFIED_TH_MNT_MAX * (1<<SOC_PETRA_ITM_SATISFIED_TH_EXP_MAX))

#define SOC_PETRA_ITM_WRED_GRANULARITY        16
#define SOC_PETRA_ITM_Q_OCC_GRANULARITY        16

#define SOC_PETRA_ITM_MIN_WRED_AVRG_TH_MNT_MSB       71
#define SOC_PETRA_ITM_MIN_WRED_AVRG_TH_MNT_LSB       65
#define SOC_PETRA_ITM_MIN_WRED_AVRG_TH_MNT_NOF_BITS  (SOC_PETRA_ITM_MIN_WRED_AVRG_TH_MNT_MSB - SOC_PETRA_ITM_MIN_WRED_AVRG_TH_MNT_LSB + 1)
#define SOC_PETRA_ITM_MIN_WRED_AVRG_TH_EXP_MSB       76
#define SOC_PETRA_ITM_MIN_WRED_AVRG_TH_EXP_LSB       72
#define SOC_PETRA_ITM_MIN_WRED_AVRG_TH_EXP_NOF_BITS  (SOC_PETRA_ITM_MIN_WRED_AVRG_TH_EXP_MSB - SOC_PETRA_ITM_MIN_WRED_AVRG_TH_EXP_LSB + 1)

#define SOC_PETRA_ITM_MAX_WRED_AVRG_TH_MNT_MSB       59
#define SOC_PETRA_ITM_MAX_WRED_AVRG_TH_MNT_LSB       53
#define SOC_PETRA_ITM_MAX_WRED_AVRG_TH_MNT_NOF_BITS  (SOC_PETRA_ITM_MAX_WRED_AVRG_TH_MNT_MSB - SOC_PETRA_ITM_MAX_WRED_AVRG_TH_MNT_LSB + 1)
#define SOC_PETRA_ITM_MAX_WRED_AVRG_TH_EXP_MSB       64
#define SOC_PETRA_ITM_MAX_WRED_AVRG_TH_EXP_LSB       60
#define SOC_PETRA_ITM_MAX_WRED_AVRG_TH_EXP_NOF_BITS  (SOC_PETRA_ITM_MAX_WRED_AVRG_TH_EXP_MSB - SOC_PETRA_ITM_MAX_WRED_AVRG_TH_EXP_LSB + 1)

/*2^32/100 is 42949672.96 ==> 42949673 ==> 0x28F5C29*/
#define SOC_PETRA_WRED_NORMALIZE_FACTOR  (0x28F5C29)

#define SOC_PETRA_ITM_VSQ_MIN_WRED_AVRG_TH_MNT_MSB       59
#define SOC_PETRA_ITM_VSQ_MIN_WRED_AVRG_TH_MNT_LSB       53
#define SOC_PETRA_ITM_VSQ_MIN_WRED_AVRG_TH_MNT_NOF_BITS  (SOC_PETRA_ITM_VSQ_MIN_WRED_AVRG_TH_MNT_MSB - SOC_PETRA_ITM_VSQ_MIN_WRED_AVRG_TH_MNT_LSB + 1)
#define SOC_PETRA_ITM_VSQ_MIN_WRED_AVRG_TH_EXP_MSB       64
#define SOC_PETRA_ITM_VSQ_MIN_WRED_AVRG_TH_EXP_LSB       60
#define SOC_PETRA_ITM_VSQ_MIN_WRED_AVRG_TH_EXP_NOF_BITS  (SOC_PETRA_ITM_VSQ_MIN_WRED_AVRG_TH_EXP_MSB - SOC_PETRA_ITM_VSQ_MIN_WRED_AVRG_TH_EXP_LSB + 1)


#define SOC_PETRA_ITM_VSQ_MAX_WRED_AVRG_TH_MNT_MSB       47
#define SOC_PETRA_ITM_VSQ_MAX_WRED_AVRG_TH_MNT_LSB       41
#define SOC_PETRA_ITM_VSQ_MAX_WRED_AVRG_TH_MNT_NOF_BITS  (SOC_PETRA_ITM_VSQ_MAX_WRED_AVRG_TH_MNT_MSB - SOC_PETRA_ITM_VSQ_MAX_WRED_AVRG_TH_MNT_LSB + 1)
#define SOC_PETRA_ITM_VSQ_MAX_WRED_AVRG_TH_EXP_MSB       52
#define SOC_PETRA_ITM_VSQ_MAX_WRED_AVRG_TH_EXP_LSB       48
#define SOC_PETRA_ITM_VSQ_MAX_WRED_AVRG_TH_EXP_NOF_BITS  (SOC_PETRA_ITM_VSQ_MAX_WRED_AVRG_TH_EXP_MSB - SOC_PETRA_ITM_VSQ_MAX_WRED_AVRG_TH_EXP_LSB + 1)


#define SOC_PETRA_ITM_WD_INFO_MIN_SCAN_CYCLE_PERIOD_MICRO_GRANULARITY   256

/* System Red Boundaries */
#define SOC_PETRA_ITM_SYS_RED_Q_BOUNDARY_TH_MNT_MSB   6
#define SOC_PETRA_ITM_SYS_RED_Q_BOUNDARY_TH_MNT_LSB   0
#define SOC_PETRA_ITM_VSQ_SYS_RED_Q_BOUNDARY_TH_MNT_NOF_BITS (SOC_PETRA_ITM_SYS_RED_Q_BOUNDARY_TH_MNT_MSB - SOC_PETRA_ITM_SYS_RED_Q_BOUNDARY_TH_MNT_LSB + 1)

#define SOC_PETRA_ITM_SYS_RED_Q_BOUNDARY_TH_EXP_MSB   11
#define SOC_PETRA_ITM_SYS_RED_Q_BOUNDARY_TH_EXP_LSB   7
#define SOC_PETRA_ITM_VSQ_SYS_RED_Q_BOUNDARY_TH_EXP_NOF_BITS (SOC_PETRA_ITM_SYS_RED_Q_BOUNDARY_TH_EXP_MSB - SOC_PETRA_ITM_SYS_RED_Q_BOUNDARY_TH_EXP_LSB + 1)

#define SOC_PETRA_ITM_TEST_CTGRY_TEST_EN_BIT          0
#define SOC_PETRA_ITM_TEST_CTGRY_TRFFC_TEST_EN_BIT    1
#define SOC_PETRA_ITM_TEST_CTGRY2_3_CNCTN_TEST_EN_BIT 2
#define SOC_PETRA_ITM_TEST_STTSTCS_TAG_TEST_EN_BIT    3

/* (2^22 - 1) */
#define SOC_PETRA_ITM_VSQ_FC_BD_SIZE_MAX              0X3FFFFF
/* (2^28 - 1) for Soc_petra-A */
#define SOC_PA_ITM_VSQ_FC_Q_SIZE_MAX 0XFFFFFFF

#ifdef LINK_PB_LIBRARIES
/* (2^32 - 1) for Soc_petra-B */
#define SOC_PB_ITM_VSQ_FC_Q_SIZE_MAX 0XFFFFFFFF
#else
#define SOC_PB_ITM_VSQ_FC_Q_SIZE_MAX SOC_PA_ITM_VSQ_FC_Q_SIZE_MAX
#endif

#define SOC_PETRA_ITM_VSQ_FC_Q_SIZE_MAX \
  SOC_PETRA_CST_VALUE_DISTINCT(ITM_VSQ_FC_Q_SIZE_MAX, uint32)

/* (256MByte) */
#define SOC_PETRA_ITM_WRED_INFO_MIN_AVRG_TH_MAX       0XFFFFFFF
/* (256MByte) */
#define SOC_PETRA_ITM_WRED_INFO_MAX_AVRG_TH_MAX       0XFFFFFFF

/* (2^24 - 1) */
#define SOC_PETRA_ITM_GLOB_RCS_FC_BDBS_SIZE_MAX       0XFFFFFF

/* (2^23 - 1) */
#define SOC_PETRA_ITM_GLOB_RCS_FC_UC_SIZE_MAX         0X7FFFFF

/* (2^24 - 1) */
#define SOC_PETRA_ITM_GLOB_RCS_FC_FMC_SIZE_MAX        0XFFFFFF

/* (2^24 - 1) */
#define SOC_PETRA_ITM_GLOB_RCS_DROP_BDBS_SIZE_MAX     0XFFFFFF

/* (2^23 - 1) */
#define SOC_PETRA_ITM_GLOB_RCS_DROP_UC_SIZE_MAX       0X7FFFFF

/* (2^22 - 1) */
#define SOC_PETRA_ITM_GLOB_RCS_DROP_MINI_MC_SIZE_MAX  0X3FFFFF

/* (2^24 - 1) */
#define SOC_PETRA_ITM_GLOB_RCS_DROP_FMC_SIZE_MAX      0XFFFF


#define SOC_PA_ITM_ECN_MAPPING_IHP_VAL                (0xE0)
#define SOC_PA_ITM_ECN_MAPPING_ECN_CONGESTED_VAL      (0x3)

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
*     soc_petra_itm_regs_init
* FUNCTION:
*   Initialization of the Soc_petra blocks configured in this module.
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
  soc_petra_itm_regs_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    start_prev,
    size_prev,
    fld_val,
    res;
  uint32
    reg_idx,
    fld_idx;
  uint8
    is_mesh,
    is_single_cntxt;
#ifdef LINK_PB_LIBRARIES
  SOC_PETRA_ITM_DBUFF_SIZE_BYTES
    dbuff_size;
#endif
  SOC_PETRA_REGS
    *regs;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_REGS_INIT);

  regs = soc_petra_regs();

  /*
   *  IQM
   */

  /* Admit Disable */
  SOC_PETRA_REG_SET(regs->iqm.reject_admission_reg, 0xffffffff, 10, exit);

  /* Internal thresholds */
  SOC_PETRA_FLD_SET(regs->iqm.intern_thresh_reg.thr_1, 0xe, 20, exit);

  /*
   *  IPT
   */
  /* Enable stamping Fabric Header */
  SOC_PETRA_FLD_SET(regs->ipt.stamping_fabric_header_enable_reg.stamp_dp, 0x1, 40, exit);
  SOC_PETRA_FLD_SET(regs->ipt.stamping_fabric_header_enable_reg.stamp_fap_port, 0x1, 42, exit);
  SOC_PETRA_FLD_SET(regs->ipt.stamping_fabric_header_enable_reg.stamp_fwdaction, 0x1, 44, exit);
  SOC_PETRA_FLD_SET(regs->ipt.stamping_fabric_header_enable_reg.stamp_outlif, 0x1, 46, exit);

  /* Set snooping action recognition */
  SOC_PETRA_FLD_SET(regs->ipt.mapping_queue_type_to_snoop_packet_reg.snoop_table, SOC_SAND_BIT(SOC_PETRA_ITM_FWD_ACTION_TYPE_SNOOP), 50, exit);

  fld_val = 0x7;
#ifdef LINK_PB_LIBRARIES
  if (SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE)
  {
    SOC_PETRA_FLD_GET(regs->eci.general_controls_reg.dbuff_size, fld_val, 60, exit);

    res = soc_petra_itm_dbuff_internal2size(
            fld_val,
            &dbuff_size
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  
    fld_val = (dbuff_size >= SOC_PETRA_ITM_DBUFF_SIZE_BYTES_2048)?0x7:0xf;
  }
#endif

  /*
   *  IDR
   */
  SOC_PETRA_FLD_SET(regs->idr.dynamic_configuration_reg_cmn.max_dp_threshold, fld_val, 55, exit);

  res = soc_petra_fabric_is_mesh(
          unit,
          &is_mesh,
          &is_single_cntxt
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  if (is_single_cntxt)
  {
    /*
     *	CLOS or Mesh with legacy coexist
     */

    /*
     *	DQCQ
     */
    SOC_PETRA_FLD_SET(regs->ips.low_priority_dqcq_depth_config1_reg.dest0_depth, 0x200, 80, exit);
    SOC_PETRA_FLD_SET(regs->ips.low_priority_dqcq_depth_config1_reg.dest1_depth, 0x200, 81, exit);

    SOC_PETRA_FLD_SET(regs->ips.low_priority_dqcq_depth_config2_reg.dest2_depth, 0x200, 83, exit);
    SOC_PETRA_FLD_SET(regs->ips.low_priority_dqcq_depth_config2_reg.dest3_depth, 0x200, 84, exit);

    SOC_PETRA_FLD_SET(regs->ips.low_priority_dqcq_depth_config3_reg.dest4_depth, 0x0, 85, exit);
    SOC_PETRA_FLD_SET(regs->ips.low_priority_dqcq_depth_config3_reg.dest5_depth, 0x0, 86, exit);

    SOC_PETRA_FLD_SET(regs->ips.low_priority_dqcq_depth_config4_reg.dest6_depth, 0x0, 87, exit);
    SOC_PETRA_FLD_SET(regs->ips.low_priority_dqcq_depth_config4_reg.dest7_depth, 0x0, 88, exit);

    /*
     *	TX Queue-Size
     */
    start_prev = 0x0;
    size_prev  = 0x190;
    SOC_PETRA_FLD_SET(regs->ipt.transmit_data_queue_start_adress_reg[0].dtq_start[0], start_prev, 110, exit);
    SOC_PETRA_FLD_SET(regs->ipt.transmit_data_queue_size_reg[0].dtq_size[0], size_prev, 111, exit);

    start_prev = start_prev + size_prev + 1;
    SOC_PETRA_FLD_SET(regs->ipt.transmit_data_queue_start_adress_reg[0].dtq_start[1], start_prev, 112, exit);
    size_prev = 0x10;
    SOC_PETRA_FLD_SET(regs->ipt.transmit_data_queue_size_reg[0].dtq_size[1], size_prev,  113, exit);

    for (reg_idx = 1; reg_idx < SOC_PETRA_TRANSMIT_DATA_QUEUE_NOF_REGS; reg_idx++)
    {
      for (fld_idx = 0; fld_idx < SOC_PETRA_TRANSMIT_DATA_QUEUE_NOF_FLDS; fld_idx++)
      {
        start_prev = start_prev + size_prev + 1;
        SOC_PETRA_FLD_SET(regs->ipt.transmit_data_queue_start_adress_reg[reg_idx].dtq_start[fld_idx], start_prev, 114, exit);
        SOC_PETRA_FLD_SET(regs->ipt.transmit_data_queue_size_reg[reg_idx].dtq_size[fld_idx], size_prev, 116+reg_idx, exit);
      }
    }
  }
  else
  {
    if (is_mesh == TRUE)
    {
      for (reg_idx = 0; reg_idx < SOC_PETRA_REGS_DBUFF_PTR_Q_THRESH_NOF_REGS; reg_idx++)
      {
        SOC_PETRA_FLD_SET(regs->ipt.drambuffer_pointer_queue_threshold_reg[reg_idx].dpq_th_lp, 0x7f, 100+reg_idx, exit);
      }
    }
  }

  /*
   *	In 64 bytes resolution. For 1024B, set to 16.
   */
  SOC_PETRA_FLD_SET(regs->ips.stored_credits_usage_configuration_reg.mul_pkt_deq_bytes, 16, 120, exit);

  if (SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE)
  {
    SOC_PETRA_FLD_SET(regs->eci.ingress_shaping_queue_boundaries_reg.isp_qnum_low, 0x7ffe, 150,exit);
    SOC_PETRA_FLD_SET(regs->eci.ingress_shaping_queue_boundaries_reg.isp_qnum_high, 0x7ffe, 152,exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_regs_init()",0,0);
}

/*********************************************************************
* NAME:
*     soc_petra_itm_init
* FUNCTION:
*     Initialization of the Soc_petra blocks configured in this module.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Called as part of the initialization sequence.
*********************************************************************/
uint32
  soc_petra_itm_init(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   qdr_nof_entries
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    idx;
  SOC_PETRA_ITM_GLOB_RCS_DROP_TH
    glbl_drop, glbl_drop_exact;
  SOC_PETRA_ITM_GLOB_RCS_FC_TH
    glbl_fc, glbl_fc_exact;
  SOC_PETRA_ITM_CR_WD_INFO
    cr_wd, cr_wd_exact;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_INIT);
  res = soc_petra_itm_regs_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  soc_petra_PETRA_ITM_GLOB_RCS_DROP_TH_clear(&glbl_drop);
  soc_petra_PETRA_ITM_GLOB_RCS_DROP_TH_clear(&glbl_drop_exact);
  soc_petra_PETRA_ITM_GLOB_RCS_FC_TH_clear(&glbl_fc);
  soc_petra_PETRA_ITM_GLOB_RCS_FC_TH_clear(&glbl_fc_exact);
  soc_petra_PETRA_ITM_CR_WD_INFO_clear(&cr_wd);
  soc_petra_PETRA_ITM_CR_WD_INFO_clear(&cr_wd_exact);

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

  res = soc_petra_itm_glob_rcs_fc_set_unsafe(
          unit,
          &glbl_fc,
          &glbl_fc_exact
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  glbl_drop.bdbs[0].set       = 128;
  glbl_drop.bdbs[0].clear     = 512;
  for (idx = 1; idx < SOC_PETRA_NOF_DROP_PRECEDENCE; idx++)
  {
    glbl_drop.bdbs[idx].set   = 256;
    glbl_drop.bdbs[idx].clear = 1024;
  }

  for (idx = 0; idx < SOC_PETRA_NOF_DROP_PRECEDENCE; idx++)
  {
    glbl_drop.bds[idx].set   = 1008 * (qdr_nof_entries/1024);
    glbl_drop.bds[idx].clear =  928 * (qdr_nof_entries/1024);
  }
  for (idx = 0; idx < SOC_PETRA_NOF_DROP_PRECEDENCE; idx++)
  {
    glbl_drop.unicast[idx].set   = 496;
    glbl_drop.unicast[idx].clear = 672;
  }

  for (idx = 0; idx < SOC_PETRA_NOF_DROP_PRECEDENCE; idx++)
  {
    glbl_drop.full_mc[idx].set   = 160;
    glbl_drop.full_mc[idx].clear = 320;
  }

  for (idx = 0; idx < SOC_PETRA_NOF_DROP_PRECEDENCE; idx++)
  {
    glbl_drop.mini_mc[idx].set   = 160;
    glbl_drop.mini_mc[idx].clear = 320;
  }

  res = soc_petra_itm_glob_rcs_drop_set_unsafe(
          unit,
          &glbl_drop,
          &glbl_drop_exact
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  cr_wd.bottom_queue  = 0;
  cr_wd.top_queue     = SOC_PETRA_MAX_QUEUE_ID;
  cr_wd.max_flow_msg_gen_rate_nano = 320;
  cr_wd.min_scan_cycle_period_micro = 1;

 /*
  * Credit watchdog:
  *  1. retransmit flow control messages for active queues that haven't get Credits from
  *     schedule after some time.
  *  2. free "stuck" queues after time threshold to keep DRAM resources.
  */
  res = soc_petra_itm_cr_wd_set_unsafe(
          unit,
          &cr_wd,
          &cr_wd_exact
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

#ifdef LINK_PB_LIBRARIES
  if (SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE)
  {
    res = soc_pb_itm_init(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }
#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_init()",0,0);
}

uint32
  soc_petra_itm_vsq_idx_verify(
    SOC_SAND_IN  SOC_PETRA_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  SOC_PETRA_ITM_VSQ_NDX         vsq_in_group_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_VSQ_IDX_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_group_ndx, SOC_PETRA_ITM_VSQ_GROUP_LAST-1,
    SOC_PETRA_ITM_VSQ_GROUP_ID_OUT_OF_RANGE_ERR, 10, exit
  );

  switch(vsq_group_ndx)
  {
  case SOC_PETRA_ITM_VSQ_GROUP_CTGRY:
    SOC_SAND_ERR_IF_ABOVE_MAX(
      vsq_in_group_ndx, SOC_PETRA_ITM_VSQ_GROUPA_SZE-1,
      SOC_PETRA_ITM_VSQ_NDX_OUT_OF_RANGE_ERR, 30, exit
     );
    break;
  case SOC_PETRA_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS:
   SOC_SAND_ERR_IF_ABOVE_MAX(
      vsq_in_group_ndx, SOC_PETRA_ITM_VSQ_GROUPB_SZE-1,
      SOC_PETRA_ITM_VSQ_NDX_OUT_OF_RANGE_ERR, 32, exit
     );
    break;
  case SOC_PETRA_ITM_VSQ_GROUP_CTGRY_2_3_CNCTN_CLS:
    SOC_SAND_ERR_IF_ABOVE_MAX(
      vsq_in_group_ndx, SOC_PETRA_ITM_VSQ_GROUPC_SZE-1,
      SOC_PETRA_ITM_VSQ_NDX_OUT_OF_RANGE_ERR, 34, exit
     );
    break;
  case SOC_PETRA_ITM_VSQ_GROUP_STTSTCS_TAG:
    SOC_SAND_ERR_IF_ABOVE_MAX(
      vsq_in_group_ndx, SOC_PETRA_ITM_VSQ_GROUPD_SZE-1,
      SOC_PETRA_ITM_VSQ_NDX_OUT_OF_RANGE_ERR, 36, exit
     );
    break;
  default:
    break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "soc_petra_itm_vsq_idx_verify()",vsq_group_ndx,vsq_in_group_ndx);
}

/*********************************************************************
 *     Enable/disable ECN (Explicit Congestion Notification) 
 *     functionality.                                          
 *     Details: in the H file. (search for prototype)          
 *********************************************************************/
uint32  
  soc_petra_itm_ecn_enable_set_unsafe(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint8 enable
  )
{
  uint32
    fld_val,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_ECN_ENABLE_SET_UNSAFE);

  regs = soc_petra_regs();

  /* 
   * Ingress: Set the ECN-Capable mapping 
   */
  fld_val = SOC_PA_ITM_ECN_MAPPING_IHP_VAL;
  SOC_PA_FLD_SET(regs->ihp.ecn_config_reg.ecn_mapping, fld_val, 10,exit);
  fld_val = SOC_SAND_BOOL2NUM(enable);
  SOC_PA_FLD_SET(regs->ihp.ecn_config_reg.ecn_enable, fld_val, 20,exit);
  SOC_PA_FLD_SET(regs->iqm.iqm_enablers_reg.en_ipt_cd, fld_val, 30,exit);
  SOC_PA_FLD_SET(regs->iqm.iqm_enablers_reg.ecn_enable, fld_val, 40,exit);

  /* 
   * Egress: Set the ECN-Capable mapping 
   */
  fld_val = SOC_SAND_BOOL2NUM(enable);
  SOC_PA_FLD_SET(regs->epni.ecn_reg.ecn_en, fld_val, 50,exit);
  fld_val = SOC_PA_ITM_ECN_MAPPING_ECN_CONGESTED_VAL;
  SOC_PA_FLD_SET(regs->epni.ecn_reg.ecn, fld_val, 60,exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_itm_ecn_enable_set_unsafe()", 0, 0);
}


 /*********************************************************************
 *     Enable/disable ECN (Explicit Congestion Notification) 
 *     functionality.                                          
 *     Details: in the H file. (search for prototype)          
 *********************************************************************/
uint32  
  soc_petra_itm_ecn_enable_get_unsafe(
    SOC_SAND_IN  int unit,
    SOC_SAND_OUT uint8 *enable
  )
{
  uint32
    fld_val,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_ECN_ENABLE_GET_UNSAFE);

  regs = soc_petra_regs();

  SOC_SAND_CHECK_NULL_INPUT(enable);

  /* 
   * Ingress: Set the ECN-Capable mapping 
   */
  SOC_PA_FLD_GET(regs->ihp.ecn_config_reg.ecn_enable, fld_val, 10,exit);
  *enable = SOC_SAND_NUM2BOOL(fld_val);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_itm_ecn_enable_get_unsafe()", 0, 0);
}

/*********************************************************************
*     This function sets a buffer with the mantissa-exponent values,
*     when given the desired value and field sizes.
*********************************************************************/
STATIC uint32
  soc_petra_itm_man_exp_buffer_set(
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
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_MAN_EXP_BUFFER_SET);

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
  SOC_SAND_EXIT_AND_SEND_ERROR( "soc_petra_itm_man_exp_buffer_set()",0,0);
}
/*********************************************************************
*     This function gets a the value that is constructed of a buffer
*     with the mantissa-exponent values, when the field sizes.
*********************************************************************/
STATIC uint32
  soc_petra_itm_man_exp_buffer_get(
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
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_MAN_EXP_BUFFER_GET);

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
  SOC_SAND_EXIT_AND_SEND_ERROR( "soc_petra_itm_man_exp_buffer_get()",0,0);
}


/************************************************************************
*     Convert the type SOC_PETRA_ITM_ADMIT_ONE_TEST_TMPLT to uint32
*     in order to be written to the register field.                                                                     */
/************************************************************************/
uint32
  soc_petra_itm_convert_admit_one_test_tmplt_to_u32(
    SOC_SAND_IN SOC_PETRA_ITM_ADMIT_ONE_TEST_TMPLT test,
    SOC_SAND_OUT uint32                       *test_in_sand_u32
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_CONVERT_ADMIT_ONE_TEST_TMPLT_TO_U32);
  SOC_SAND_CHECK_NULL_INPUT(test_in_sand_u32);

  *test_in_sand_u32 =
    (test.ctgry_test_en==TRUE ? 0:1) |
    ((test.ctgry_trffc_test_en==TRUE ? 0:1) << SOC_PETRA_ITM_TEST_CTGRY_TRFFC_TEST_EN_BIT)|
    ((test.ctgry2_3_cnctn_test_en==TRUE ? 0:1) << SOC_PETRA_ITM_TEST_CTGRY2_3_CNCTN_TEST_EN_BIT)|
    ((test.sttstcs_tag_test_en==TRUE ? 0:1) << SOC_PETRA_ITM_TEST_STTSTCS_TAG_TEST_EN_BIT);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "soc_petra_itm_convert_admit_one_test_tmplt_to_u32()",0,0);
}
/*********************************************************************
*     Convert the uint32 test template field from the register
*     to type SOC_PETRA_ITM_ADMIT_ONE_TEST_TMPLT in order to be returned
*     to user.
*********************************************************************/
uint32
  soc_petra_itm_convert_u32_to_admit_one_test_tmplt(
    SOC_SAND_IN  uint32                       test_in_sand_u32,
    SOC_SAND_OUT SOC_PETRA_ITM_ADMIT_ONE_TEST_TMPLT *test
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_CONVERT_U32_TO_ADMIT_ONE_TEST_TMPLT);
  SOC_SAND_CHECK_NULL_INPUT(test);

  test->ctgry_test_en =
    ((test_in_sand_u32 >> SOC_PETRA_ITM_TEST_CTGRY_TEST_EN_BIT) & 1) ? FALSE : TRUE;
  test->ctgry_trffc_test_en =
    ((test_in_sand_u32 >> SOC_PETRA_ITM_TEST_CTGRY_TRFFC_TEST_EN_BIT) & 1) ? FALSE : TRUE;
  test->ctgry2_3_cnctn_test_en =
    ((test_in_sand_u32 >> SOC_PETRA_ITM_TEST_CTGRY2_3_CNCTN_TEST_EN_BIT) & 1) ? FALSE : TRUE;
  test->sttstcs_tag_test_en =
    ((test_in_sand_u32 >> SOC_PETRA_ITM_TEST_STTSTCS_TAG_TEST_EN_BIT) & 1) ? FALSE : TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "soc_petra_itm_convert_u32_to_admit_one_test_tmplt()",0,0);
}
/*********************************************************************
*    Set a credit request hungry configuration
*********************************************************************/
STATIC uint32
  soc_petra_itm_cr_request_info_hungry_table_field_set(
    SOC_SAND_IN  int32 value,
    SOC_SAND_OUT uint32 *output_buffer,
    SOC_SAND_OUT int32 *exact_value
  )
{
  uint32
    res;
  int32
    exact_value_var;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_CR_REQUEST_INFO_HUNGRY_TABLE_FIELD_SET);

  res = soc_petra_itm_man_exp_buffer_set(
          value,
          SOC_PETRA_ITM_HUNGRY_TH_MNT_LSB,
          SOC_PETRA_ITM_HUNGRY_TH_MNT_NOF_BITS,
          SOC_PETRA_ITM_HUNGRY_TH_EXP_LSB,
          SOC_PETRA_ITM_HUNGRY_TH_EXP_NOF_BITS,
          TRUE,
          output_buffer,
          &exact_value_var
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *exact_value = (int32)exact_value_var;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "soc_petra_itm_cr_request_info_hungry_table_field_set()",0,0);
}
/*********************************************************************
*    Set a credit request satisfied configuration
*********************************************************************/
STATIC uint32
  soc_petra_itm_cr_request_info_satisfied_mnt_exp_table_field_set(
    SOC_SAND_IN  int32  value,
    SOC_SAND_OUT uint32  *output_buffer,
    SOC_SAND_OUT uint32 *exact_value
  )
{
  uint32
    res;
  int32
    exact_value_var;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_CR_REQUEST_INFO_SATISFIED_MNT_EXP_TABLE_FIELD_SET);

  res = soc_petra_itm_man_exp_buffer_set(
          value,
          SOC_PETRA_ITM_SATISFIED_TH_MNT_LSB,
          SOC_PETRA_ITM_SATISFIED_TH_MNT_NOF_BITS,
          SOC_PETRA_ITM_SATISFIED_TH_EXP_LSB,
          SOC_PETRA_ITM_SATISFIED_TH_EXP_NOF_BITS,
          FALSE,
          output_buffer,
          &exact_value_var
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *exact_value = (uint32)exact_value_var;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "soc_petra_itm_cr_request_info_satisfied_mnt_exp_table_field_set()",0,0);
}
/*********************************************************************
*    Get a credit request hungry configuration
*********************************************************************/
STATIC uint32
  soc_petra_itm_cr_request_info_hungry_table_field_get(
    SOC_SAND_IN  int32 buffer,
    SOC_SAND_OUT int32 *value
  )
{
  uint32
    res;
  int32
    value_var;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_CR_REQUEST_INFO_HUNGRY_TABLE_FIELD_GET);

  res = soc_petra_itm_man_exp_buffer_get(
          buffer,
          SOC_PETRA_ITM_HUNGRY_TH_MNT_LSB,
          SOC_PETRA_ITM_HUNGRY_TH_MNT_NOF_BITS,
          SOC_PETRA_ITM_HUNGRY_TH_EXP_LSB,
          SOC_PETRA_ITM_HUNGRY_TH_EXP_NOF_BITS,
          TRUE,
          &value_var
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *value = (int32)value_var;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "soc_petra_itm_cr_request_conf_hungry_table_field_get()",0,0);
}
/*********************************************************************
*    Get a credit request satisfied configuration
*********************************************************************/
STATIC uint32
  soc_petra_itm_cr_request_info_satisfied_mnt_exp_table_field_get(
    SOC_SAND_IN  int32 buffer,
    SOC_SAND_OUT uint32 *value
  )
{
  uint32
    res;
  int32
    value_var;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_CR_REQUEST_INFO_SATISFIED_MNT_EXP_TABLE_FIELD_GET);

  res = soc_petra_itm_man_exp_buffer_get(
    buffer,
    SOC_PETRA_ITM_SATISFIED_TH_MNT_LSB,
    SOC_PETRA_ITM_SATISFIED_TH_MNT_NOF_BITS,
    SOC_PETRA_ITM_SATISFIED_TH_EXP_LSB,
    SOC_PETRA_ITM_SATISFIED_TH_EXP_NOF_BITS,
    FALSE,
    &value_var
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  *value = (uint32)value_var;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "soc_petra_itm_cr_request_conf_satisfied_mnt_exp_table_field_get()",0,0);
}
/*********************************************************************
*    Sets the WRED parameters to the value to be written to the table.
*    By converting them appropriately.
*********************************************************************/
STATIC uint32
  soc_petra_itm_WRED_QT_DP_INFO_to_WRED_TBL_DATA(
    SOC_SAND_IN    SOC_PETRA_ITM_WRED_QT_DP_INFO                            *wred_param,
    SOC_SAND_INOUT SOC_PETRA_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_DATA *tbl_data
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
    u64_c2;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_WRED_QT_DP_INFO_TO_WRED_TBL_DATA);

  SOC_SAND_CHECK_NULL_INPUT(wred_param);
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);

  trunced = FALSE;

  /*
   * min_avrg_th
   */
  
  max_avrg_th_16_byte = SOC_SAND_DIV_ROUND_UP(wred_param->min_avrg_th,SOC_PETRA_ITM_WRED_GRANULARITY);
  tbl_data->pq_avrg_min_th = 0;
  res = soc_petra_itm_man_exp_buffer_set(
          max_avrg_th_16_byte,
          SOC_PETRA_ITM_MIN_WRED_AVRG_TH_MNT_LSB,
          SOC_PETRA_ITM_MIN_WRED_AVRG_TH_MNT_NOF_BITS,
          SOC_PETRA_ITM_MIN_WRED_AVRG_TH_EXP_LSB,
          SOC_PETRA_ITM_MIN_WRED_AVRG_TH_EXP_NOF_BITS,
          FALSE,
          &(tbl_data->pq_avrg_min_th),
          &min_avrg_th_exact_wred_granular
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /* min_avrg_th_exact *= SOC_PETRA_ITM_WRED_GRANULARITY; */

  /*
   * max_avrg_th
   */
  tbl_data->pq_avrg_max_th = 0;
  res = soc_petra_itm_man_exp_buffer_set(
          SOC_SAND_DIV_ROUND_UP(wred_param->max_avrg_th,SOC_PETRA_ITM_WRED_GRANULARITY),
          SOC_PETRA_ITM_MAX_WRED_AVRG_TH_MNT_LSB,
          SOC_PETRA_ITM_MAX_WRED_AVRG_TH_MNT_NOF_BITS,
          SOC_PETRA_ITM_MAX_WRED_AVRG_TH_EXP_LSB,
          SOC_PETRA_ITM_MAX_WRED_AVRG_TH_EXP_NOF_BITS,
          FALSE,
          &(tbl_data->pq_avrg_max_th),
          &max_avrg_th_exact_wred_granular
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  /* max_avrg_th_exact *= SOC_PETRA_ITM_WRED_GRANULARITY; */

  /*
   * max_packet_size
   */
  calc = wred_param->max_packet_size;
  if (calc > SOC_PETRA_ITM_WRED_MAX_PACKET_SIZE_FOR_CALC)
  {
    calc = SOC_PETRA_ITM_WRED_MAX_PACKET_SIZE_FOR_CALC;
  }
  calc = SOC_SAND_DIV_ROUND_UP(calc, SOC_PETRA_ITM_WRED_GRANULARITY);
  tbl_data->pq_c3 = (wred_param->max_avrg_th == 0 ? 0 : soc_sand_log2_round_up(calc));

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
  calc = SOC_PETRA_WRED_NORMALIZE_FACTOR * max_prob;
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
    SOC_PETRA_WRED_NORMALIZE_FACTOR,
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
    sal_memcpy(&u64_c2, &u64_1, sizeof(SOC_SAND_U64));
  }

  trunced = soc_sand_u64_to_long(&u64_c2, &tbl_data->pq_c2);

  if (trunced)
  {
    tbl_data->pq_c2 = 0xFFFFFFFF;
  }
  tbl_data->pq_c2 = (wred_param->max_avrg_th == 0 ? 1 : tbl_data->pq_c2);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_WRED_QT_DP_INFO_to_WRED_TBL_DATA()",0,0);
}

/*********************************************************************
*    Gets the WRED parameters from the values in the table.
*    By converting them appropriately.
*********************************************************************/
STATIC uint32
  soc_petra_itm_WRED_TBL_DATA_to_WRED_QT_DP_INFO(
    SOC_SAND_IN  SOC_PETRA_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_DATA  *tbl_data,
    SOC_SAND_OUT SOC_PETRA_ITM_WRED_QT_DP_INFO                             *wred_param
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

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_WRED_TBL_DATA_TO_WRED_QT_DP_INFO);

  SOC_SAND_CHECK_NULL_INPUT(wred_param);
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);

  res = soc_petra_itm_man_exp_buffer_get(
          tbl_data->pq_avrg_min_th,
          SOC_PETRA_ITM_MIN_WRED_AVRG_TH_MNT_LSB,
          SOC_PETRA_ITM_MIN_WRED_AVRG_TH_MNT_NOF_BITS,
          SOC_PETRA_ITM_MIN_WRED_AVRG_TH_EXP_LSB,
          SOC_PETRA_ITM_MIN_WRED_AVRG_TH_EXP_NOF_BITS,
          FALSE,
          &min_avrg_th_var
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  wred_param->min_avrg_th = (uint32)min_avrg_th_var;

  wred_param->min_avrg_th *= SOC_PETRA_ITM_WRED_GRANULARITY;

  /*
   * max_avrg_th
   */

  res = soc_petra_itm_man_exp_buffer_get(
          tbl_data->pq_avrg_max_th,
          SOC_PETRA_ITM_MAX_WRED_AVRG_TH_MNT_LSB,
          SOC_PETRA_ITM_MAX_WRED_AVRG_TH_MNT_NOF_BITS,
          SOC_PETRA_ITM_MAX_WRED_AVRG_TH_EXP_LSB,
          SOC_PETRA_ITM_MAX_WRED_AVRG_TH_EXP_NOF_BITS,
          FALSE,
          &max_avrg_th_var
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  wred_param->max_avrg_th = (uint32)max_avrg_th_var;

  wred_param->max_avrg_th *= SOC_PETRA_ITM_WRED_GRANULARITY;

  /*
   * max_packet_size
   */
  wred_param->max_packet_size = ((tbl_data->pq_c3 == 0) && (tbl_data->pq_c2 == 1)) ? 0 : (0x1 << (tbl_data->pq_c3)) * SOC_PETRA_ITM_WRED_GRANULARITY;

  avrg_th_diff_wred_granular =
    (wred_param->max_avrg_th - wred_param->min_avrg_th) / SOC_PETRA_ITM_WRED_GRANULARITY;

  two_power_c1 = 1<<tbl_data->pq_c1;
  /*
   * C1 = ((2^32)/100)*max-prob / (max-th - min-th) in powers of 2
   * ==>
   * max-prob =  ( 2^C1 * (max-th - min-th) ) / ((2^32)/100)
   */
  soc_sand_u64_multiply_longs(two_power_c1, avrg_th_diff_wred_granular, &u64_1);
  remainder = soc_sand_u64_devide_u64_long(&u64_1, SOC_PETRA_WRED_NORMALIZE_FACTOR, &u64_2);
  soc_sand_u64_to_long(&u64_2, &wred_param->max_probability);

  if(remainder > (SOC_PETRA_WRED_NORMALIZE_FACTOR/2))
  {
    wred_param->max_probability++;
  }

  if(wred_param->max_probability > 100)
  {
    wred_param->max_probability = 100;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_WRED_TBL_DATA_to_WRED_QT_DP_INFO()",0,0);
}
/*********************************************************************
*    Set the VSQ rate class for VSQ-type-A
*********************************************************************/
STATIC uint32
  soc_petra_itm_vsq_group_a_set_rt_class(
    SOC_SAND_IN int                 unit,
    SOC_SAND_IN uint32   vsq_rt_cls,
    SOC_SAND_IN uint32                  vsq_in_group_ndx
  )
{
  uint32
    res;
  SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_DATA
    iqm_vsq_descriptor_rate_class_group_a_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_VSQ_GROUP_A_SET_RT_CLASS);

  iqm_vsq_descriptor_rate_class_group_a_tbl_data.vsq_rc_a = vsq_rt_cls;

  res = soc_petra_iqm_vsq_descriptor_rate_class_group_a_tbl_set_unsafe(
          unit,
          vsq_in_group_ndx,
          &iqm_vsq_descriptor_rate_class_group_a_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_vsq_group_a_set_rt_class()",0,0);
}
/*********************************************************************
*    Set the VSQ rate class for VSQ-type-B
*********************************************************************/
STATIC uint32
  soc_petra_itm_vsq_group_b_set_rt_class(
    SOC_SAND_IN int                 unit,
    SOC_SAND_IN uint32                 vsq_rt_cls,
    SOC_SAND_IN uint32                  vsq_in_group_ndx
  )
{
  uint32
    res;
  SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_DATA
    iqm_vsq_descriptor_rate_class_group_b_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_VSQ_GROUP_B_SET_RT_CLASS);

  iqm_vsq_descriptor_rate_class_group_b_tbl_data.vsq_rc_b = vsq_rt_cls;

  res = soc_petra_iqm_vsq_descriptor_rate_class_group_b_tbl_set_unsafe(
    unit,
    vsq_in_group_ndx,
    &iqm_vsq_descriptor_rate_class_group_b_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_vsq_group_b_set_rt_class()",0,0);
}
/*********************************************************************
*    Set the VSQ rate class for VSQ-type-C
*********************************************************************/
STATIC uint32
  soc_petra_itm_vsq_group_c_set_rt_class(
    SOC_SAND_IN int                 unit,
    SOC_SAND_IN uint32                 vsq_rt_cls,
    SOC_SAND_IN uint32                  vsq_in_group_ndx
  )
{
  uint32
    res;
  SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_DATA
    iqm_vsq_descriptor_rate_class_group_c_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_VSQ_GROUP_C_SET_RT_CLASS);

  iqm_vsq_descriptor_rate_class_group_c_tbl_data.vsq_rc_c = vsq_rt_cls;

  res = soc_petra_iqm_vsq_descriptor_rate_class_group_c_tbl_set_unsafe(
          unit,
          vsq_in_group_ndx,
          &iqm_vsq_descriptor_rate_class_group_c_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_vsq_group_c_set_rt_class()",0,0);
}
/*********************************************************************
*    Set the VSQ rate class for VSQ-type-D
*********************************************************************/
STATIC uint32
  soc_petra_itm_vsq_group_d_set_rt_class(
    SOC_SAND_IN int                 unit,
    SOC_SAND_IN uint32                 vsq_rt_cls,
    SOC_SAND_IN uint32                  vsq_in_group_ndx
  )
{
  uint32
    res;
  SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_DATA
    iqm_vsq_descriptor_rate_class_group_d_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_VSQ_GROUP_D_SET_RT_CLASS);

  iqm_vsq_descriptor_rate_class_group_d_tbl_data.vsq_rc_d = vsq_rt_cls;

  res = soc_petra_iqm_vsq_descriptor_rate_class_group_d_tbl_set_unsafe(
          unit,
          vsq_in_group_ndx,
          &iqm_vsq_descriptor_rate_class_group_d_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_vsq_group_d_set_rt_class()",0,0);
}

/*********************************************************************
*    Get the VSQ rate class for VSQ-type-A
*********************************************************************/
STATIC uint32
  soc_petra_itm_vsq_group_a_get_rt_class(
    SOC_SAND_IN int                 unit,
    SOC_SAND_IN uint32                  vsq_in_group_ndx,
    SOC_SAND_OUT uint32                *vsq_rt_cls
  )
{
  uint32
    res;
  SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_DATA
    iqm_vsq_descriptor_rate_class_group_a_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_VSQ_GROUP_A_SET_RT_CLASS);

  res = soc_petra_iqm_vsq_descriptor_rate_class_group_a_tbl_get_unsafe(
          unit,
          vsq_in_group_ndx,
          &iqm_vsq_descriptor_rate_class_group_a_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  *vsq_rt_cls =
    iqm_vsq_descriptor_rate_class_group_a_tbl_data.vsq_rc_a;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_vsq_group_a_get_rt_class()",0,0);
}
/*********************************************************************
*    Get the VSQ rate class for VSQ-type-B
*********************************************************************/
STATIC uint32
  soc_petra_itm_vsq_group_b_get_rt_class(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  vsq_in_group_ndx,
    SOC_SAND_OUT uint32                *vsq_rt_cls
  )
{
  uint32
    res;
  SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_DATA
    iqm_vsq_descriptor_rate_class_group_b_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_VSQ_GROUP_B_GET_RT_CLASS);

  res = soc_petra_iqm_vsq_descriptor_rate_class_group_b_tbl_get_unsafe(
          unit,
          vsq_in_group_ndx,
          &iqm_vsq_descriptor_rate_class_group_b_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  *vsq_rt_cls =
    iqm_vsq_descriptor_rate_class_group_b_tbl_data.vsq_rc_b;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_vsq_group_b_get_rt_class()",0,0);
}
/*********************************************************************
*    Get the VSQ rate class for VSQ-type-C
*********************************************************************/
STATIC uint32
  soc_petra_itm_vsq_group_c_get_rt_class(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  vsq_in_group_ndx,
    SOC_SAND_OUT uint32                *vsq_rt_cls
  )
{
  uint32
    res;
  SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_DATA
    iqm_vsq_descriptor_rate_class_group_c_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_VSQ_GROUP_C_GET_RT_CLASS);

  res = soc_petra_iqm_vsq_descriptor_rate_class_group_c_tbl_get_unsafe(
          unit,
          vsq_in_group_ndx,
          &iqm_vsq_descriptor_rate_class_group_c_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  *vsq_rt_cls =
    iqm_vsq_descriptor_rate_class_group_c_tbl_data.vsq_rc_c;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_vsq_group_c_get_rt_class()",0,0);
}
/*********************************************************************
*    Get the VSQ rate class for VSQ-type-D
*********************************************************************/
STATIC uint32
  soc_petra_itm_vsq_group_d_get_rt_class(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  vsq_in_group_ndx,
    SOC_SAND_OUT uint32                *vsq_rt_cls
  )
{
  uint32
    res;
  SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_DATA
    iqm_vsq_descriptor_rate_class_group_d_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_VSQ_GROUP_D_GET_RT_CLASS);

  res = soc_petra_iqm_vsq_descriptor_rate_class_group_d_tbl_get_unsafe(
          unit,
          vsq_in_group_ndx,
          &iqm_vsq_descriptor_rate_class_group_d_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

 *vsq_rt_cls =
   iqm_vsq_descriptor_rate_class_group_d_tbl_data.vsq_rc_d;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_vsq_group_d_get_rt_class()",0,0);
}

/*********************************************************************
*    Set the VSQ flow-control info according to vsq-group-id
*********************************************************************/
STATIC uint32
  soc_petra_itm_vsq_group_set_fc_info(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ITM_VSQ_GROUP       vsq_group,
    SOC_SAND_IN  uint32                 vsq_rt_cls,
    SOC_SAND_IN  SOC_PETRA_ITM_VSQ_FC_INFO     *info,
    SOC_SAND_OUT SOC_PETRA_ITM_VSQ_FC_INFO     *exact_info
  )
{
  uint32
    res,
    vsq_bds_th_clear_and_set_mnt_nof_bits,
    vsq_bds_th_clear_and_set_exp_nof_bits,
    vsq_words_th_clear_and_set_mnt_nof_bits,
    vsq_words_th_clear_and_set_exp_nof_bits;
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_DATA
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


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_VSQ_GROUP_SET_FC_INFO);

  res = soc_petra_tbls_get(&tables);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  res = soc_petra_iqm_vsq_flow_control_parameters_table_group_tbl_get_unsafe(
          unit,
          vsq_group,
          vsq_rt_cls,
          &iqm_vsq_flow_control_parameters_table_group_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  vsq_bds_th_clear_and_set_mnt_nof_bits =
    SOC_PETRA_TBL(tables->iqm.vsq_flow_control_parameters_table_group_tbl[vsq_group].clear_threshold_bd_mnt.msb) -
    SOC_PETRA_TBL(tables->iqm.vsq_flow_control_parameters_table_group_tbl[vsq_group].clear_threshold_bd_mnt.lsb) + 1;

  vsq_bds_th_clear_and_set_exp_nof_bits =
    SOC_PETRA_TBL(tables->iqm.vsq_flow_control_parameters_table_group_tbl[vsq_group].clear_threshold_bd_exp.msb) -
    SOC_PETRA_TBL(tables->iqm.vsq_flow_control_parameters_table_group_tbl[vsq_group].clear_threshold_bd_exp.lsb) + 1;

  vsq_words_th_clear_and_set_mnt_nof_bits =
    SOC_PETRA_TBL(tables->iqm.vsq_flow_control_parameters_table_group_tbl[vsq_group].clear_threshold_words_mnt.msb) -
    SOC_PETRA_TBL(tables->iqm.vsq_flow_control_parameters_table_group_tbl[vsq_group].clear_threshold_words_mnt.lsb) + 1;

  vsq_words_th_clear_and_set_exp_nof_bits =
    SOC_PETRA_TBL(tables->iqm.vsq_flow_control_parameters_table_group_tbl[vsq_group].clear_threshold_words_exp.msb) -
    SOC_PETRA_TBL(tables->iqm.vsq_flow_control_parameters_table_group_tbl[vsq_group].clear_threshold_words_exp.lsb) + 1;

  res = soc_sand_break_to_mnt_exp_round_up(
          info->bd_size_fc.clear,
          vsq_bds_th_clear_and_set_mnt_nof_bits,
          vsq_bds_th_clear_and_set_exp_nof_bits,
          1,
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

  res = soc_petra_iqm_vsq_flow_control_parameters_table_group_tbl_set_unsafe(
          unit,
          vsq_group,
          vsq_rt_cls,
          &iqm_vsq_flow_control_parameters_table_group_tbl_data
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_vsq_group_set_fc_info()",0,0);
}

/*********************************************************************
*    Get the VSQ flow-control info according to vsq-group-id
*********************************************************************/
STATIC uint32
  soc_petra_itm_vsq_group_get_fc_info(
    SOC_SAND_IN   int                 unit,
    SOC_SAND_IN   SOC_PETRA_ITM_VSQ_GROUP       vsq_group,
    SOC_SAND_IN   uint32   vsq_rt_cls,
    SOC_SAND_OUT  SOC_PETRA_ITM_VSQ_FC_INFO     *info
  )
{
  uint32
    res;
  SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_DATA
    iqm_vsq_flow_control_parameters_table_group_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_VSQ_GROUP_GET_FC_INFO);

  res = soc_petra_iqm_vsq_flow_control_parameters_table_group_tbl_get_unsafe(
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
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_vsq_group_get_fc_info()",0,0);
}


/*********************************************************************
*    Sets the VSQ-WRED parameters according to vsq-queue-type and
*    drop-precedence to the value to be written to the table.
*    By converting them appropriately.
*********************************************************************/
STATIC uint32
  soc_petra_itm_vsq_WRED_QT_DP_INFO_to_WRED_TBL_DATA(
    SOC_SAND_IN  SOC_PETRA_ITM_WRED_QT_DP_INFO                             *wred_param,
    SOC_SAND_INOUT SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA *tbl_data
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
    u64_c2;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_VSQ_WRED_QT_DP_INFO_TO_WRED_TBL_DATA);

  SOC_SAND_CHECK_NULL_INPUT(wred_param);
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);

  trunced = FALSE;

  /*
   * min_avrg_th
   */

  tbl_data->min_avrg_th = 0;
  max_avrg_th_16_byte = SOC_SAND_DIV_ROUND_UP(wred_param->min_avrg_th,SOC_PETRA_ITM_WRED_GRANULARITY);
  res = soc_petra_itm_man_exp_buffer_set(
          max_avrg_th_16_byte,
          SOC_PETRA_ITM_VSQ_MIN_WRED_AVRG_TH_MNT_LSB,
          SOC_PETRA_ITM_VSQ_MIN_WRED_AVRG_TH_MNT_NOF_BITS,
          SOC_PETRA_ITM_VSQ_MIN_WRED_AVRG_TH_EXP_LSB,
          SOC_PETRA_ITM_VSQ_MIN_WRED_AVRG_TH_EXP_NOF_BITS,
          FALSE,
          &(tbl_data->min_avrg_th),
          &min_avrg_th_exact_wred_granular
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /* min_avrg_th_exact *= SOC_PETRA_ITM_WRED_GRANULARITY; */

  /*
   * max_avrg_th
   */
  tbl_data->max_avrg_th = 0;
  res = soc_petra_itm_man_exp_buffer_set(
          SOC_SAND_DIV_ROUND_UP(wred_param->max_avrg_th,SOC_PETRA_ITM_WRED_GRANULARITY),
          SOC_PETRA_ITM_VSQ_MAX_WRED_AVRG_TH_MNT_LSB,
          SOC_PETRA_ITM_VSQ_MAX_WRED_AVRG_TH_MNT_NOF_BITS,
          SOC_PETRA_ITM_VSQ_MAX_WRED_AVRG_TH_EXP_LSB,
          SOC_PETRA_ITM_VSQ_MAX_WRED_AVRG_TH_EXP_NOF_BITS,
          FALSE,
          &(tbl_data->max_avrg_th),
          &max_avrg_th_exact_wred_granular
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  /* max_avrg_th_exact *= SOC_PETRA_ITM_WRED_GRANULARITY; */

  /*
   * max_packet_size
   */
  calc = wred_param->max_packet_size;
  if (calc > SOC_PETRA_ITM_WRED_MAX_PACKET_SIZE_FOR_CALC)
  {
    calc = SOC_PETRA_ITM_WRED_MAX_PACKET_SIZE_FOR_CALC;
  }
  calc = SOC_SAND_DIV_ROUND_UP(calc, SOC_PETRA_ITM_WRED_GRANULARITY);

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
  calc = SOC_PETRA_WRED_NORMALIZE_FACTOR * max_prob;
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
    SOC_PETRA_WRED_NORMALIZE_FACTOR,
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
    sal_memcpy(&u64_c2, &u64_1, sizeof(SOC_SAND_U64));
  }

  trunced = soc_sand_u64_to_long(&u64_c2, &tbl_data->c2);

  if (trunced)
  {
    tbl_data->c2 = 0xFFFFFFFF;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_vsq_WRED_QT_DP_INFO_to_WRED_TBL_DATA()",0,0);
}

/*********************************************************************
*    Gets the VSQ-WRED parameters according to vsq-queue-type and
*    drop-precedence from the values in the table.
*    By converting them appropriately.
*********************************************************************/
STATIC uint32
  soc_petra_itm_vsq_WRED_TBL_DATA_to_WRED_QT_DP_INFO(
    SOC_SAND_IN  SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA  *tbl_data,
    SOC_SAND_OUT SOC_PETRA_ITM_WRED_QT_DP_INFO                             *wred_param
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

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_VSQ_WRED_TBL_DATA_TO_WRED_QT_DP_INFO);

  SOC_SAND_CHECK_NULL_INPUT(wred_param);
  SOC_SAND_CHECK_NULL_INPUT(tbl_data);



  res = soc_petra_itm_man_exp_buffer_get(
          tbl_data->min_avrg_th,
          SOC_PETRA_ITM_VSQ_MIN_WRED_AVRG_TH_MNT_LSB,
          SOC_PETRA_ITM_VSQ_MIN_WRED_AVRG_TH_MNT_NOF_BITS,
          SOC_PETRA_ITM_VSQ_MIN_WRED_AVRG_TH_EXP_LSB,
          SOC_PETRA_ITM_VSQ_MIN_WRED_AVRG_TH_EXP_NOF_BITS,
          FALSE,
          (int32*)&(wred_param->min_avrg_th)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  wred_param->min_avrg_th *= SOC_PETRA_ITM_WRED_GRANULARITY;

  /*
   * max_avrg_th
   */

  res = soc_petra_itm_man_exp_buffer_get(
          tbl_data->max_avrg_th,
          SOC_PETRA_ITM_VSQ_MAX_WRED_AVRG_TH_MNT_LSB,
          SOC_PETRA_ITM_VSQ_MAX_WRED_AVRG_TH_MNT_NOF_BITS,
          SOC_PETRA_ITM_VSQ_MAX_WRED_AVRG_TH_EXP_LSB,
          SOC_PETRA_ITM_VSQ_MAX_WRED_AVRG_TH_EXP_NOF_BITS,
          FALSE,
          (int32*)&(wred_param->max_avrg_th)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  wred_param->max_avrg_th *= SOC_PETRA_ITM_WRED_GRANULARITY;

  /*
   * max_packet_size
   */

  wred_param->max_packet_size =
    (0x1<<(tbl_data->c3))*SOC_PETRA_ITM_WRED_GRANULARITY;

  /*
   *  Packet size ignore
   */
  wred_param->ignore_packet_size = SOC_SAND_NUM2BOOL(tbl_data->vq_wred_pckt_sz_ignr);


  avrg_th_diff_wred_granular =
    (wred_param->max_avrg_th - wred_param->min_avrg_th) / SOC_PETRA_ITM_WRED_GRANULARITY;

  two_power_c1 = 1<<tbl_data->c1;
  /*
   * C1 = ((2^32)/100)*max-prob / (max-th - min-th) in powers of 2
   * ==>
   * max-prob =  ( 2^C1 * (max-th - min-th) ) / ((2^32)/100)
   */
  soc_sand_u64_multiply_longs(two_power_c1, avrg_th_diff_wred_granular, &u64_1);
  remainder = soc_sand_u64_devide_u64_long(&u64_1, SOC_PETRA_WRED_NORMALIZE_FACTOR, &u64_2);
  soc_sand_u64_to_long(&u64_2, &wred_param->max_probability);

  if(remainder > (SOC_PETRA_WRED_NORMALIZE_FACTOR/2))
  {
    wred_param->max_probability++;
  }

  if(wred_param->max_probability > 100)
  {
    wred_param->max_probability = 100;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_vsq_WRED_TBL_DATA_to_WRED_QT_DP_INFO()",0,0);
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
  soc_petra_itm_dram_buffs_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_ITM_DRAM_BUFFERS_INFO *dram_buffs
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
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_DRAM_BUFFS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(dram_buffs);

  regs = soc_petra_regs();

  /*
   *  ECI:
   *  Get Buffer-pointers boundaries.
   *  Start from the beginning, and go upwards;
   *  Unicast -> Mini Multicast -> Full Multicast ->
   */
  SOC_PETRA_FLD_GET(
    regs->eci.unicast_dbuff_pointers_start_reg.uc_db_ptr_start, uc_start, 20, exit
  );
  SOC_PETRA_FLD_GET(
    regs->eci.unicast_dbuff_pointers_end_reg.uc_db_ptr_end, uc_end, 30, exit
  );

  SOC_PETRA_FLD_GET(
    regs->eci.mini_multicast_dbuff_pointers_start_reg.mn_mul_db_ptr_start, mmc_start, 40, exit
  );
  SOC_PETRA_FLD_GET(
    regs->eci.mini_multicast_dbuff_pointers_end_reg.mn_mul_db_ptr_end, mmc_end, 50, exit
  );

  SOC_PETRA_FLD_GET(
    regs->eci.full_multicast_dbuff_pointers_start_reg.fl_mul_db_ptr_start, fmc_start, 60, exit
  );
  SOC_PETRA_FLD_GET(
    regs->eci.full_multicast_dbuff_pointers_end_reg.fl_mul_db_ptr_end, fmc_end, 70, exit
  );

  if (uc_end > uc_start)
  {
    uc_size = uc_end - uc_start + 1;
  }
  else
  {
    if (uc_end == uc_start)
    {
      SOC_PETRA_FLD_GET(
        regs->idr.static_configuration_reg.fbc_unicast_autogen_enable, uc_enable_val, 80, exit
      );
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
      SOC_PETRA_FLD_GET(
        regs->idr.static_configuration_reg.fbc_mini_multicast_autogen_enable, mmc_enable_val, 80, exit
      );
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
      SOC_PETRA_FLD_GET(
        regs->idr.static_configuration_reg.fbc_full_multicast_autogen_enable, fmc_enable_val, 80, exit
      );
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

  SOC_PETRA_FLD_GET(regs->eci.general_controls_reg.dbuff_size, dbuff_size_internal, 90, exit);

  res = soc_petra_itm_dbuff_internal2size(
          dbuff_size_internal,
          &(dram_buffs->dbuff_size)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  dram_buffs->full_mc_nof_buffs = fmc_size;
  dram_buffs->mini_mc_nof_buffs = mmc_size;
  dram_buffs->uc_nof_buffs = uc_size;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_dram_buffs_get_unsafe()",0,0);
}

/*********************************************************************
*     Sets the Thresholds to trigger/clear the Flow Control
*     Indication. For the different kinds of general resources
*     (bds, unicast, multicast).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_glob_rcs_fc_set_unsafe(
    SOC_SAND_IN   int                 unit,
    SOC_SAND_IN   SOC_PETRA_ITM_GLOB_RCS_FC_TH  *info,
    SOC_SAND_OUT  SOC_PETRA_ITM_GLOB_RCS_FC_TH  *exact_info
  )
{
  uint32
    res,
    bdbs_th_mnt_nof_bits,
    bdbs_th_exp_nof_bits,
    uc_th_mnt_nof_bits,
    uc_th_exp_nof_bits,
    fmc_th_mnt_nof_bits,
    fmc_th_exp_nof_bits;
  uint32
    mnt_val,
    exp_val;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_GLOB_RCS_FC_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_CHECK_NULL_INPUT(exact_info);

  regs = soc_petra_regs();

  /*
    info->bdbs.hp.clear
    info->bdbs.hp.set
    info->bdbs.lp.clear
    info->bdbs.lp.set
  */

  bdbs_th_mnt_nof_bits = SOC_PETRA_FLD_NOF_BITS(
    SOC_PETRA_REG_DB_ACC(regs->iqm.general_flow_control_configuration_bdb_high_priority_reg.fc_set_fr_bdb_th_hp_mnt));
  bdbs_th_exp_nof_bits = SOC_PETRA_FLD_NOF_BITS(
    SOC_PETRA_REG_DB_ACC(regs->iqm.general_flow_control_configuration_bdb_high_priority_reg.fc_set_fr_bdb_th_hp_exp));

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

  SOC_PETRA_FLD_SET(regs->iqm.general_flow_control_configuration_bdb_high_priority_reg.fc_clr_fr_bdb_th_hp_mnt, (uint32)mnt_val, 110, exit);
  SOC_PETRA_FLD_SET(regs->iqm.general_flow_control_configuration_bdb_high_priority_reg.fc_clr_fr_bdb_th_hp_exp, (uint32)exp_val, 120, exit);

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

  SOC_PETRA_FLD_SET(regs->iqm.general_flow_control_configuration_bdb_high_priority_reg.fc_set_fr_bdb_th_hp_mnt, (uint32)mnt_val, 130, exit);
  SOC_PETRA_FLD_SET(regs->iqm.general_flow_control_configuration_bdb_high_priority_reg.fc_set_fr_bdb_th_hp_exp, (uint32)exp_val, 140, exit);

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

  SOC_PETRA_FLD_SET(regs->iqm.general_flow_control_configuration_bdb_low_priority_reg.fc_clr_fr_bdb_th_lp_mnt, (uint32)mnt_val, 150, exit);
  SOC_PETRA_FLD_SET(regs->iqm.general_flow_control_configuration_bdb_low_priority_reg.fc_clr_fr_bdb_th_lp_exp, (uint32)exp_val, 160, exit);

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

  SOC_PETRA_FLD_SET(regs->iqm.general_flow_control_configuration_bdb_low_priority_reg.fc_set_fr_bdb_th_lp_mnt, (uint32)mnt_val, 170, exit);
  SOC_PETRA_FLD_SET(regs->iqm.general_flow_control_configuration_bdb_low_priority_reg.fc_set_fr_bdb_th_lp_exp, (uint32)exp_val, 180, exit);

  /*
    info->unicast.hp.clear
    info->unicast.hp.set
    info->unicast.lp.clear
    info->unicast.lp.set
  */

  uc_th_mnt_nof_bits = SOC_PETRA_FLD_NOF_BITS(
    SOC_PETRA_REG_DB_ACC(regs->iqm.general_clear_flow_control_configuration_dbs_high_priority_reg.fc_clr_fr_db_uc_th_hp_mnt));
  uc_th_exp_nof_bits = SOC_PETRA_FLD_NOF_BITS(
    SOC_PETRA_REG_DB_ACC(regs->iqm.general_clear_flow_control_configuration_dbs_high_priority_reg.fc_clr_fr_db_uc_th_hp_exp));

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

  SOC_PETRA_FLD_SET(regs->iqm.general_clear_flow_control_configuration_dbs_high_priority_reg.fc_clr_fr_db_uc_th_hp_mnt, (uint32)mnt_val, 210, exit);
  SOC_PETRA_FLD_SET(regs->iqm.general_clear_flow_control_configuration_dbs_high_priority_reg.fc_clr_fr_db_uc_th_hp_exp, (uint32)exp_val, 220, exit);

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

  SOC_PETRA_FLD_SET(regs->iqm.general_set_flow_control_configuration_bds_high_priority_reg.fc_set_fr_db_uc_th_hp_mnt, (uint32)mnt_val, 230, exit);
  SOC_PETRA_FLD_SET(regs->iqm.general_set_flow_control_configuration_bds_high_priority_reg.fc_set_fr_db_uc_th_hp_exp, (uint32)exp_val, 240, exit);

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

  SOC_PETRA_FLD_SET(regs->iqm.general_clear_flow_control_configuration_dbs_low_priority_reg.fc_clr_fr_db_uc_th_lp_mnt, (uint32)mnt_val, 250, exit);
  SOC_PETRA_FLD_SET(regs->iqm.general_clear_flow_control_configuration_dbs_low_priority_reg.fc_clr_fr_db_uc_th_lp_exp, (uint32)exp_val, 260, exit);

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

  SOC_PETRA_FLD_SET(regs->iqm.general_set_flow_control_configuration_dbs_low_priority_reg.fc_set_fr_db_uc_th_lp_mnt, (uint32)mnt_val, 270, exit);
  SOC_PETRA_FLD_SET(regs->iqm.general_set_flow_control_configuration_dbs_low_priority_reg.fc_set_fr_db_uc_th_lp_exp, (uint32)exp_val, 280, exit);

  /*
    info->full_mc.hp.clear
    info->full_mc.hp.set
    info->full_mc.lp.clear
    info->full_mc.lp.set
  */

  fmc_th_mnt_nof_bits = SOC_PETRA_FLD_NOF_BITS(
    SOC_PETRA_REG_DB_ACC(regs->iqm.general_clear_flow_control_configuration_dbs_high_priority_reg.fc_clr_fr_db_flmc_th_hp_mnt));
  fmc_th_exp_nof_bits = SOC_PETRA_FLD_NOF_BITS(
    SOC_PETRA_REG_DB_ACC(regs->iqm.general_clear_flow_control_configuration_dbs_high_priority_reg.fc_clr_fr_db_flmc_th_hp_exp));

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

  SOC_PETRA_FLD_SET(regs->iqm.general_clear_flow_control_configuration_dbs_high_priority_reg.fc_clr_fr_db_flmc_th_hp_mnt, (uint32)mnt_val, 310, exit);
  SOC_PETRA_FLD_SET(regs->iqm.general_clear_flow_control_configuration_dbs_high_priority_reg.fc_clr_fr_db_flmc_th_hp_exp, (uint32)exp_val, 320, exit);

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

  SOC_PETRA_FLD_SET(regs->iqm.general_set_flow_control_configuration_bds_high_priority_reg.fc_set_fr_db_flmc_th_hp_mnt, (uint32)mnt_val, 330, exit);
  SOC_PETRA_FLD_SET(regs->iqm.general_set_flow_control_configuration_bds_high_priority_reg.fc_set_fr_db_flmc_th_hp_exp, (uint32)exp_val, 340, exit);

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

  SOC_PETRA_FLD_SET(regs->iqm.general_clear_flow_control_configuration_dbs_low_priority_reg.fc_clr_fr_db_flmc_th_lp_mnt, (uint32)mnt_val, 350, exit);
  SOC_PETRA_FLD_SET(regs->iqm.general_clear_flow_control_configuration_dbs_low_priority_reg.fc_clr_fr_db_flmc_th_lp_exp, (uint32)exp_val, 360, exit);

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

  SOC_PETRA_FLD_SET(regs->iqm.general_set_flow_control_configuration_dbs_low_priority_reg.fc_set_fr_db_flmc_th_lp_mnt, (uint32)mnt_val, 370, exit);
  SOC_PETRA_FLD_SET(regs->iqm.general_set_flow_control_configuration_dbs_low_priority_reg.fc_set_fr_db_flmc_th_lp_exp, (uint32)exp_val, 380, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_glob_rcs_fc_set_unsafe()",0,0);
}

/*********************************************************************
*     Sets the Thresholds to trigger/clear the Flow Control
*     Indication. For the different kinds of general resources
*     (bds, unicast, multicast).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_glob_rcs_fc_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ITM_GLOB_RCS_FC_TH  *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_GLOB_RCS_FC_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  /* BDBs */

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->bdbs.hp.clear, SOC_PETRA_ITM_GLOB_RCS_FC_BDBS_SIZE_MAX,
    SOC_PETRA_ITM_GLOB_RCS_FC_SIZE_OUT_OF_RANGE_ERR, 10, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->bdbs.hp.set, SOC_PETRA_ITM_GLOB_RCS_FC_BDBS_SIZE_MAX,
    SOC_PETRA_ITM_GLOB_RCS_FC_SIZE_OUT_OF_RANGE_ERR, 20, exit
    );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->bdbs.lp.clear, SOC_PETRA_ITM_GLOB_RCS_FC_BDBS_SIZE_MAX,
    SOC_PETRA_ITM_GLOB_RCS_FC_SIZE_OUT_OF_RANGE_ERR, 30, exit
    );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->bdbs.lp.set, SOC_PETRA_ITM_GLOB_RCS_FC_BDBS_SIZE_MAX,
    SOC_PETRA_ITM_GLOB_RCS_FC_SIZE_OUT_OF_RANGE_ERR, 40, exit
    );

  /* Unicast */
  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->unicast.hp.set, SOC_PETRA_ITM_GLOB_RCS_FC_UC_SIZE_MAX,
    SOC_PETRA_ITM_GLOB_RCS_FC_SIZE_OUT_OF_RANGE_ERR, 50, exit
    );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->unicast.hp.clear, SOC_PETRA_ITM_GLOB_RCS_FC_UC_SIZE_MAX,
    SOC_PETRA_ITM_GLOB_RCS_FC_SIZE_OUT_OF_RANGE_ERR, 60, exit
    );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->unicast.lp.set, SOC_PETRA_ITM_GLOB_RCS_FC_UC_SIZE_MAX,
    SOC_PETRA_ITM_GLOB_RCS_FC_SIZE_OUT_OF_RANGE_ERR, 70, exit
    );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->unicast.lp.clear, SOC_PETRA_ITM_GLOB_RCS_FC_UC_SIZE_MAX,
    SOC_PETRA_ITM_GLOB_RCS_FC_SIZE_OUT_OF_RANGE_ERR, 80, exit
    );

  /* Full- Multicast */
  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->full_mc.hp.set, SOC_PETRA_ITM_GLOB_RCS_FC_FMC_SIZE_MAX,
    SOC_PETRA_ITM_GLOB_RCS_FC_SIZE_OUT_OF_RANGE_ERR, 90, exit
    );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->full_mc.hp.clear, SOC_PETRA_ITM_GLOB_RCS_FC_FMC_SIZE_MAX,
    SOC_PETRA_ITM_GLOB_RCS_FC_SIZE_OUT_OF_RANGE_ERR, 100, exit
    );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->full_mc.lp.set, SOC_PETRA_ITM_GLOB_RCS_FC_FMC_SIZE_MAX,
    SOC_PETRA_ITM_GLOB_RCS_FC_SIZE_OUT_OF_RANGE_ERR, 110, exit
    );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->full_mc.lp.clear, SOC_PETRA_ITM_GLOB_RCS_FC_FMC_SIZE_MAX,
    SOC_PETRA_ITM_GLOB_RCS_FC_SIZE_OUT_OF_RANGE_ERR, 120, exit
    );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_glob_rcs_fc_verify()",0,0);
}

/*********************************************************************
*     Sets the Thresholds to trigger/clear the Flow Control
*     Indication. For the different kinds of general resources
*     (bds, unicast, multicast).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_glob_rcs_fc_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_ITM_GLOB_RCS_FC_TH  *info
  )
{
  uint32
    res,
    mnt_val,
    exp_val;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_GLOB_RCS_FC_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  SOC_PETRA_FLD_GET(regs->iqm.general_flow_control_configuration_bdb_high_priority_reg.fc_clr_fr_bdb_th_hp_mnt, mnt_val, 110, exit);
  SOC_PETRA_FLD_GET(regs->iqm.general_flow_control_configuration_bdb_high_priority_reg.fc_clr_fr_bdb_th_hp_exp, exp_val, 120, exit);

  info->bdbs.hp.clear = (mnt_val) * (1<<(exp_val));

  SOC_PETRA_FLD_GET(regs->iqm.general_flow_control_configuration_bdb_high_priority_reg.fc_set_fr_bdb_th_hp_mnt, mnt_val, 130, exit);
  SOC_PETRA_FLD_GET(regs->iqm.general_flow_control_configuration_bdb_high_priority_reg.fc_set_fr_bdb_th_hp_exp, exp_val, 140, exit);

  info->bdbs.hp.set = (mnt_val) * (1<<(exp_val));

  SOC_PETRA_FLD_GET(regs->iqm.general_flow_control_configuration_bdb_low_priority_reg.fc_clr_fr_bdb_th_lp_mnt, mnt_val, 150, exit);
  SOC_PETRA_FLD_GET(regs->iqm.general_flow_control_configuration_bdb_low_priority_reg.fc_clr_fr_bdb_th_lp_exp, exp_val, 160, exit);

  info->bdbs.lp.clear = (mnt_val) * (1<<(exp_val));

  SOC_PETRA_FLD_GET(regs->iqm.general_flow_control_configuration_bdb_low_priority_reg.fc_set_fr_bdb_th_lp_mnt, mnt_val, 170, exit);
  SOC_PETRA_FLD_GET(regs->iqm.general_flow_control_configuration_bdb_low_priority_reg.fc_set_fr_bdb_th_lp_exp, exp_val, 180, exit);

  info->bdbs.lp.set = (mnt_val) * (1<<(exp_val));

  /*
    info->unicast.hp.clear
    info->unicast.hp.set
    info->unicast.lp.clear
    info->unicast.lp.set
  */

  SOC_PETRA_FLD_GET(regs->iqm.general_clear_flow_control_configuration_dbs_high_priority_reg.fc_clr_fr_db_uc_th_hp_mnt, mnt_val, 210, exit);
  SOC_PETRA_FLD_GET(regs->iqm.general_clear_flow_control_configuration_dbs_high_priority_reg.fc_clr_fr_db_uc_th_hp_exp, exp_val, 220, exit);

  info->unicast.hp.clear = (mnt_val) * (1<<(exp_val));

  SOC_PETRA_FLD_GET(regs->iqm.general_set_flow_control_configuration_bds_high_priority_reg.fc_set_fr_db_uc_th_hp_mnt, mnt_val, 230, exit);
  SOC_PETRA_FLD_GET(regs->iqm.general_set_flow_control_configuration_bds_high_priority_reg.fc_set_fr_db_uc_th_hp_exp, exp_val, 240, exit);

  info->unicast.hp.set = (mnt_val) * (1<<(exp_val));

  SOC_PETRA_FLD_GET(regs->iqm.general_clear_flow_control_configuration_dbs_low_priority_reg.fc_clr_fr_db_uc_th_lp_mnt, mnt_val, 250, exit);
  SOC_PETRA_FLD_GET(regs->iqm.general_clear_flow_control_configuration_dbs_low_priority_reg.fc_clr_fr_db_uc_th_lp_exp, exp_val, 260, exit);

  info->unicast.lp.clear = (mnt_val) * (1<<(exp_val));

  SOC_PETRA_FLD_GET(regs->iqm.general_set_flow_control_configuration_dbs_low_priority_reg.fc_set_fr_db_uc_th_lp_mnt, mnt_val, 270, exit);
  SOC_PETRA_FLD_GET(regs->iqm.general_set_flow_control_configuration_dbs_low_priority_reg.fc_set_fr_db_uc_th_lp_exp, exp_val, 280, exit);

  info->unicast.lp.set = (mnt_val) * (1<<(exp_val));

  /*
    info->full_mc.hp.clear
    info->full_mc.hp.set
    info->full_mc.lp.clear
    info->full_mc.lp.set
  */

  SOC_PETRA_FLD_GET(regs->iqm.general_clear_flow_control_configuration_dbs_high_priority_reg.fc_clr_fr_db_flmc_th_hp_mnt, mnt_val, 310, exit);
  SOC_PETRA_FLD_GET(regs->iqm.general_clear_flow_control_configuration_dbs_high_priority_reg.fc_clr_fr_db_flmc_th_hp_exp, exp_val, 320, exit);

  info->full_mc.hp.clear = (mnt_val) * (1<<(exp_val));

  SOC_PETRA_FLD_GET(regs->iqm.general_set_flow_control_configuration_bds_high_priority_reg.fc_set_fr_db_flmc_th_hp_mnt, mnt_val, 330, exit);
  SOC_PETRA_FLD_GET(regs->iqm.general_set_flow_control_configuration_bds_high_priority_reg.fc_set_fr_db_flmc_th_hp_exp, exp_val, 340, exit);

  info->full_mc.hp.set = (mnt_val) * (1<<(exp_val));

  SOC_PETRA_FLD_GET(regs->iqm.general_clear_flow_control_configuration_dbs_low_priority_reg.fc_clr_fr_db_flmc_th_lp_mnt, mnt_val, 350, exit);
  SOC_PETRA_FLD_GET(regs->iqm.general_clear_flow_control_configuration_dbs_low_priority_reg.fc_clr_fr_db_flmc_th_lp_exp, exp_val, 360, exit);

  info->full_mc.lp.clear = (mnt_val) * (1<<(exp_val));

  SOC_PETRA_FLD_GET(regs->iqm.general_set_flow_control_configuration_dbs_low_priority_reg.fc_set_fr_db_flmc_th_lp_mnt, mnt_val, 370, exit);
  SOC_PETRA_FLD_GET(regs->iqm.general_set_flow_control_configuration_dbs_low_priority_reg.fc_set_fr_db_flmc_th_lp_exp, exp_val, 380, exit);

  info->full_mc.lp.set = (mnt_val) * (1<<(exp_val));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_glob_rcs_fc_get_unsafe()",0,0);
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
  soc_petra_itm_glob_rcs_drop_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ITM_GLOB_RCS_DROP_TH *info,
    SOC_SAND_OUT SOC_PETRA_ITM_GLOB_RCS_DROP_TH *exact_info
  )
{
  uint32
    res,
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
    indx;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_GLOB_RCS_DROP_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_CHECK_NULL_INPUT(exact_info);

  regs = soc_petra_regs();

  /*
   *  BDB-s (Buffer Descriptor Buffers)
   */
  bdbs_th_mnt_nof_bits = SOC_PETRA_FLD_NOF_BITS(
    SOC_PETRA_REG_DB_ACC(regs->iqm.general_reject_configuration_bdbs_reg[0].rjct_clr_fr_bdb_th_mnt));
  bdbs_th_exp_nof_bits = SOC_PETRA_FLD_NOF_BITS(
    SOC_PETRA_REG_DB_ACC(regs->iqm.general_reject_configuration_bdbs_reg[0].rjct_clr_fr_bdb_th_exp));

  for (indx = 0 ; indx < SOC_PETRA_NOF_DROP_PRECEDENCE ; indx++)
  {
      res = soc_sand_break_to_mnt_exp_round_up(
              info->bdbs[indx].clear,
              bdbs_th_mnt_nof_bits,
              bdbs_th_exp_nof_bits,
              0,
              &mnt_val,
              &exp_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      exact_info->bdbs[indx].clear = (mnt_val) * (1<<(exp_val));

      SOC_PETRA_FLD_SET(regs->iqm.general_reject_configuration_bdbs_reg[indx].rjct_clr_fr_bdb_th_mnt, (uint32)mnt_val, 110, exit);
      SOC_PETRA_FLD_SET(regs->iqm.general_reject_configuration_bdbs_reg[indx].rjct_clr_fr_bdb_th_exp, (uint32)exp_val, 120, exit);

      res = soc_sand_break_to_mnt_exp_round_up(
              info->bdbs[indx].set,
              bdbs_th_mnt_nof_bits,
              bdbs_th_exp_nof_bits,
              0,
              &mnt_val,
              &exp_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

      exact_info->bdbs[indx].set = (mnt_val) * (1<<(exp_val));

      SOC_PETRA_FLD_SET(regs->iqm.general_reject_configuration_bdbs_reg[indx].rjct_set_fr_bdb_th_mnt, (uint32)mnt_val, 130, exit);
      SOC_PETRA_FLD_SET(regs->iqm.general_reject_configuration_bdbs_reg[indx].rjct_set_fr_bdb_th_exp, (uint32)exp_val, 140, exit);
  }

  /*
   *  BD-s (Buffer Descriptors)
   */
  bds_th_mnt_nof_bits = SOC_PETRA_FLD_NOF_BITS(
    SOC_PETRA_REG_DB_ACC(regs->iqm.general_reject_configuration_bds_reg[0].rjct_clr_oc_bd_th_mnt));
  bds_th_exp_nof_bits = SOC_PETRA_FLD_NOF_BITS(
    SOC_PETRA_REG_DB_ACC(regs->iqm.general_reject_configuration_bds_reg[0].rjct_clr_oc_bd_th_exp));

  for (indx = 0 ; indx < SOC_PETRA_NOF_DROP_PRECEDENCE ; indx++)
  {
      res = soc_sand_break_to_mnt_exp_round_up(
              info->bds[indx].clear,
              bds_th_mnt_nof_bits,
              bds_th_exp_nof_bits,
              0,
              &mnt_val,
              &exp_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      exact_info->bds[indx].clear = (mnt_val) * (1<<(exp_val));

      SOC_PETRA_FLD_SET(regs->iqm.general_reject_configuration_bds_reg[indx].rjct_clr_oc_bd_th_mnt, (uint32)mnt_val, 110, exit);
      SOC_PETRA_FLD_SET(regs->iqm.general_reject_configuration_bds_reg[indx].rjct_clr_oc_bd_th_exp, (uint32)exp_val, 120, exit);

      res = soc_sand_break_to_mnt_exp_round_up(
              info->bds[indx].set,
              bds_th_mnt_nof_bits,
              bds_th_exp_nof_bits,
              0,
              &mnt_val,
              &exp_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

      exact_info->bds[indx].set = (mnt_val) * (1<<(exp_val));

      SOC_PETRA_FLD_SET(regs->iqm.general_reject_configuration_bds_reg[indx].rjct_set_oc_bd_th_mnt, (uint32)mnt_val, 130, exit);
      SOC_PETRA_FLD_SET(regs->iqm.general_reject_configuration_bds_reg[indx].rjct_set_oc_bd_th_exp, (uint32)exp_val, 140, exit);
  }

  /*
    info->unicast[index].clear
    info->unicast[index].set
  */

  uc_th_mnt_nof_bits = SOC_PETRA_FLD_NOF_BITS(
    SOC_PETRA_REG_DB_ACC(regs->iqm.general_reject_clear_configuration_dbuffs_reg[0].rjct_clr_fr_db_uc_th_mnt));
  uc_th_exp_nof_bits = SOC_PETRA_FLD_NOF_BITS(
    SOC_PETRA_REG_DB_ACC(regs->iqm.general_reject_clear_configuration_dbuffs_reg[0].rjct_clr_fr_db_uc_th_exp));

  for (indx = 0 ; indx < SOC_PETRA_NOF_DROP_PRECEDENCE ; indx++)
  {
      res = soc_sand_break_to_mnt_exp_round_up(
              info->unicast[indx].clear,
              uc_th_mnt_nof_bits,
              uc_th_exp_nof_bits,
              0,
              &mnt_val,
              &exp_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

      exact_info->unicast[indx].clear = (mnt_val) * (1<<(exp_val));

      SOC_PETRA_FLD_SET(regs->iqm.general_reject_clear_configuration_dbuffs_reg[indx].rjct_clr_fr_db_uc_th_mnt, (uint32)mnt_val, 210, exit);
      SOC_PETRA_FLD_SET(regs->iqm.general_reject_clear_configuration_dbuffs_reg[indx].rjct_clr_fr_db_uc_th_exp, (uint32)exp_val, 220, exit);

      res = soc_sand_break_to_mnt_exp_round_up(
              info->unicast[indx].set,
              uc_th_mnt_nof_bits,
              uc_th_exp_nof_bits,
              0,
              &mnt_val,
              &exp_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

      exact_info->unicast[indx].set = (mnt_val) * (1<<(exp_val));

      SOC_PETRA_FLD_SET(regs->iqm.general_reject_set_configuration_dbuffs_reg[indx].rjct_set_fr_db_uc_th_mnt, (uint32)mnt_val, 230, exit);
      SOC_PETRA_FLD_SET(regs->iqm.general_reject_set_configuration_dbuffs_reg[indx].rjct_set_fr_db_uc_th_exp, (uint32)exp_val, 240, exit);
  }

  /*
    info->mini_mc[index].clear
    info->mini_mc[index].set
  */

  mini_mc_mnt_nof_bits = SOC_PETRA_FLD_NOF_BITS(
    SOC_PETRA_REG_DB_ACC(regs->iqm.general_reject_clear_configuration_dbuffs_reg[0].rjct_clr_fr_db_mnmc_th_mnt));
  mini_mc_exp_nof_bits = SOC_PETRA_FLD_NOF_BITS(
    SOC_PETRA_REG_DB_ACC(regs->iqm.general_reject_clear_configuration_dbuffs_reg[0].rjct_clr_fr_db_mnmc_th_exp));

  for (indx = 0 ; indx < SOC_PETRA_NOF_DROP_PRECEDENCE ; indx++)
  {
      res = soc_sand_break_to_mnt_exp_round_up(
              info->mini_mc[indx].clear,
              mini_mc_mnt_nof_bits,
              mini_mc_exp_nof_bits,
              0,
              &mnt_val,
              &exp_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

      exact_info->mini_mc[indx].clear = (mnt_val) * (1<<(exp_val));

      SOC_PETRA_FLD_SET(regs->iqm.general_reject_clear_configuration_dbuffs_reg[indx].rjct_clr_fr_db_mnmc_th_mnt, (uint32)mnt_val, 310, exit);
      SOC_PETRA_FLD_SET(regs->iqm.general_reject_clear_configuration_dbuffs_reg[indx].rjct_clr_fr_db_mnmc_th_exp, (uint32)exp_val, 320, exit);

      res = soc_sand_break_to_mnt_exp_round_up(
              info->mini_mc[indx].set,
              mini_mc_mnt_nof_bits,
              mini_mc_exp_nof_bits,
              0,
              &mnt_val,
              &exp_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

      exact_info->mini_mc[indx].set = (mnt_val) * (1<<(exp_val));

      SOC_PETRA_FLD_SET(regs->iqm.general_reject_set_configuration_dbuffs_reg[indx].rjct_set_fr_db_mnmc_th_mnt, (uint32)mnt_val, 330, exit);
      SOC_PETRA_FLD_SET(regs->iqm.general_reject_set_configuration_dbuffs_reg[indx].rjct_set_fr_db_mnmc_th_exp, (uint32)exp_val, 340, exit);
  }

  /*
    info->full_mc[index].clear
    info->full_mc[index].set
   */

  fmc_th_mnt_nof_bits = SOC_PETRA_FLD_NOF_BITS(
    SOC_PETRA_REG_DB_ACC(regs->iqm.general_reject_clear_configuration_dbuffs_reg[0].rjct_clr_fr_db_flmc_th_mnt));
  fmc_th_exp_nof_bits = SOC_PETRA_FLD_NOF_BITS(
   SOC_PETRA_REG_DB_ACC(regs->iqm.general_reject_clear_configuration_dbuffs_reg[0].rjct_clr_fr_db_flmc_th_exp));

  for (indx = 0 ; indx < SOC_PETRA_NOF_DROP_PRECEDENCE ; indx++)
  {
      res = soc_sand_break_to_mnt_exp_round_up(
              info->full_mc[indx].clear,
              fmc_th_mnt_nof_bits,
              fmc_th_exp_nof_bits,
              0,
              &mnt_val,
              &exp_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

      exact_info->full_mc[indx].clear = (mnt_val) * (1<<(exp_val));

      SOC_PETRA_FLD_SET(regs->iqm.general_reject_clear_configuration_dbuffs_reg[indx].rjct_clr_fr_db_flmc_th_mnt, (uint32)mnt_val, 410, exit);
      SOC_PETRA_FLD_SET(regs->iqm.general_reject_clear_configuration_dbuffs_reg[indx].rjct_clr_fr_db_flmc_th_exp, (uint32)exp_val, 420, exit);

      res = soc_sand_break_to_mnt_exp_round_up(
              info->full_mc[indx].set,
              fmc_th_mnt_nof_bits,
              fmc_th_exp_nof_bits,
              0,
              &mnt_val,
              &exp_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

      exact_info->full_mc[indx].set = (mnt_val) * (1<<(exp_val));

      SOC_PETRA_FLD_SET(regs->iqm.general_reject_set_configuration_dbuffs_reg[indx].rjct_set_fr_db_flmc_th_mnt, (uint32)mnt_val, 430, exit);
      SOC_PETRA_FLD_SET(regs->iqm.general_reject_set_configuration_dbuffs_reg[indx].rjct_set_fr_db_flmc_th_exp, (uint32)exp_val, 440, exit);
  }

#ifdef LINK_PB_LIBRARIES
  /*
   *	Extension for Soc_petra-B
   */
  if SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE
  {
    res = soc_pb_itm_glob_rcs_drop_set_unsafe(
            unit,
            info->mem_excess,
            exact_info->mem_excess
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 450, exit);
  }
#endif /* LINK_PB_LIBRARIES */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_glob_rcs_drop_set_unsafe()",0,0);
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
  soc_petra_itm_glob_rcs_drop_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ITM_GLOB_RCS_DROP_TH *info
  )
{
  uint32
    res = SOC_SAND_OK,
    index;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_GLOB_RCS_DROP_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  /* BDBs */

  for (index = 0 ; index < SOC_PETRA_NOF_DROP_PRECEDENCE; index++)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->bdbs[index].clear, SOC_PETRA_ITM_GLOB_RCS_DROP_BDBS_SIZE_MAX,
      SOC_PETRA_ITM_GLOB_RCS_DROP_SIZE_OUT_OF_RANGE_ERR, 10, exit
      );

    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->bdbs[index].set, SOC_PETRA_ITM_GLOB_RCS_DROP_BDBS_SIZE_MAX,
      SOC_PETRA_ITM_GLOB_RCS_DROP_SIZE_OUT_OF_RANGE_ERR, 20, exit
      );
  }

  /* Unicast */

  for (index = 0 ; index < SOC_PETRA_NOF_DROP_PRECEDENCE; index++)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->unicast[index].clear, SOC_PETRA_ITM_GLOB_RCS_DROP_UC_SIZE_MAX,
      SOC_PETRA_ITM_GLOB_RCS_DROP_SIZE_OUT_OF_RANGE_ERR, 30, exit
      );

    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->unicast[index].set, SOC_PETRA_ITM_GLOB_RCS_DROP_UC_SIZE_MAX,
      SOC_PETRA_ITM_GLOB_RCS_DROP_SIZE_OUT_OF_RANGE_ERR, 40, exit
      );
  }

  /* Mini Multicast */

  for (index = 0 ; index < SOC_PETRA_NOF_DROP_PRECEDENCE; index++)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->mini_mc[index].clear, SOC_PETRA_ITM_GLOB_RCS_DROP_MINI_MC_SIZE_MAX,
      SOC_PETRA_ITM_GLOB_RCS_DROP_SIZE_OUT_OF_RANGE_ERR, 50, exit
      );

    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->mini_mc[index].set, SOC_PETRA_ITM_GLOB_RCS_DROP_MINI_MC_SIZE_MAX,
      SOC_PETRA_ITM_GLOB_RCS_DROP_SIZE_OUT_OF_RANGE_ERR, 60, exit
      );
  }

  /* Full Multicast */

  for (index = 0 ; index < SOC_PETRA_NOF_DROP_PRECEDENCE; index++)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->full_mc[index].clear, SOC_PETRA_ITM_GLOB_RCS_DROP_FMC_SIZE_MAX,
      SOC_PETRA_ITM_GLOB_RCS_DROP_SIZE_OUT_OF_RANGE_ERR, 70, exit
      );

    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->full_mc[index].set, SOC_PETRA_ITM_GLOB_RCS_DROP_FMC_SIZE_MAX,
      SOC_PETRA_ITM_GLOB_RCS_DROP_SIZE_OUT_OF_RANGE_ERR, 80, exit
      );
  }

#ifdef LINK_PB_LIBRARIES
  /*
   *	Extension for Soc_petra-B
   */
  if SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE
  {
    res = soc_pb_itm_glob_rcs_drop_verify(
            unit,
            info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
  }
#else
  SOC_SAND_IGNORE_UNUSED_VAR(res);
#endif /* LINK_PB_LIBRARIES */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_glob_rcs_drop_verify()",0,0);
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
  soc_petra_itm_glob_rcs_drop_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_ITM_GLOB_RCS_DROP_TH *info
  )
{
  uint32
    res,
    mnt_val,
    exp_val;
  uint32
    indx;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_GLOB_RCS_DROP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  /*
   * BDB-s (Buffer Descriptor Buffers)
   */

  for (indx = 0 ; indx < SOC_PETRA_NOF_DROP_PRECEDENCE ; indx++)
  {
     SOC_PETRA_FLD_GET(regs->iqm.general_reject_configuration_bdbs_reg[indx].rjct_clr_fr_bdb_th_mnt, mnt_val, 110, exit);
     SOC_PETRA_FLD_GET(regs->iqm.general_reject_configuration_bdbs_reg[indx].rjct_clr_fr_bdb_th_exp, exp_val, 120, exit);

     info->bdbs[indx].clear = (mnt_val) * (1<<(exp_val));

     SOC_PETRA_FLD_GET(regs->iqm.general_reject_configuration_bdbs_reg[indx].rjct_set_fr_bdb_th_mnt, mnt_val, 130, exit);
     SOC_PETRA_FLD_GET(regs->iqm.general_reject_configuration_bdbs_reg[indx].rjct_set_fr_bdb_th_exp, exp_val, 140, exit);

     info->bdbs[indx].set = (mnt_val) * (1<<(exp_val));
  }

  /*
   * BD-s (Buffer Descriptors)
   */

  for (indx = 0 ; indx < SOC_PETRA_NOF_DROP_PRECEDENCE ; indx++)
  {
     SOC_PETRA_FLD_GET(regs->iqm.general_reject_configuration_bds_reg[indx].rjct_clr_oc_bd_th_mnt, mnt_val, 150, exit);
     SOC_PETRA_FLD_GET(regs->iqm.general_reject_configuration_bds_reg[indx].rjct_clr_oc_bd_th_exp, exp_val, 160, exit);

     info->bds[indx].clear = (mnt_val) * (1<<(exp_val));

     SOC_PETRA_FLD_GET(regs->iqm.general_reject_configuration_bds_reg[indx].rjct_set_oc_bd_th_mnt, mnt_val, 170, exit);
     SOC_PETRA_FLD_GET(regs->iqm.general_reject_configuration_bds_reg[indx].rjct_set_oc_bd_th_exp, exp_val, 180, exit);

     info->bds[indx].set = (mnt_val) * (1<<(exp_val));
  }

  /*
    info->unicast[index].clear
    info->unicast[index].set
  */

  for (indx = 0 ; indx < SOC_PETRA_NOF_DROP_PRECEDENCE ; indx++)
  {
     SOC_PETRA_FLD_GET(regs->iqm.general_reject_clear_configuration_dbuffs_reg[indx].rjct_clr_fr_db_uc_th_mnt, mnt_val, 210, exit);
     SOC_PETRA_FLD_GET(regs->iqm.general_reject_clear_configuration_dbuffs_reg[indx].rjct_clr_fr_db_uc_th_exp, exp_val, 220, exit);

     info->unicast[indx].clear = (mnt_val) * (1<<(exp_val));

     SOC_PETRA_FLD_GET(regs->iqm.general_reject_set_configuration_dbuffs_reg[indx].rjct_set_fr_db_uc_th_mnt, mnt_val, 230, exit);
     SOC_PETRA_FLD_GET(regs->iqm.general_reject_set_configuration_dbuffs_reg[indx].rjct_set_fr_db_uc_th_exp, exp_val, 240, exit);

     info->unicast[indx].set = (mnt_val) * (1<<(exp_val));
  }

  /*
    info->mini_mc[index].clear
    info->mini_mc[index].set
  */

  for (indx = 0 ; indx < SOC_PETRA_NOF_DROP_PRECEDENCE ; indx++)
  {
     SOC_PETRA_FLD_GET(regs->iqm.general_reject_clear_configuration_dbuffs_reg[indx].rjct_clr_fr_db_mnmc_th_mnt, mnt_val, 310, exit);
     SOC_PETRA_FLD_GET(regs->iqm.general_reject_clear_configuration_dbuffs_reg[indx].rjct_clr_fr_db_mnmc_th_exp, exp_val, 320, exit);

     info->mini_mc[indx].clear = (mnt_val) * (1<<(exp_val));

     SOC_PETRA_FLD_GET(regs->iqm.general_reject_set_configuration_dbuffs_reg[indx].rjct_set_fr_db_mnmc_th_mnt, mnt_val, 330, exit);
     SOC_PETRA_FLD_GET(regs->iqm.general_reject_set_configuration_dbuffs_reg[indx].rjct_set_fr_db_mnmc_th_exp, exp_val, 340, exit);

     info->mini_mc[indx].set = (mnt_val) * (1<<(exp_val));
  }

  /*
    info->full_mc[index].clear
    info->full_mc[index].set
   */

  for (indx = 0 ; indx < SOC_PETRA_NOF_DROP_PRECEDENCE ; indx++)
  {
     SOC_PETRA_FLD_GET(regs->iqm.general_reject_clear_configuration_dbuffs_reg[indx].rjct_clr_fr_db_flmc_th_mnt, mnt_val, 410, exit);
     SOC_PETRA_FLD_GET(regs->iqm.general_reject_clear_configuration_dbuffs_reg[indx].rjct_clr_fr_db_flmc_th_exp, exp_val, 420, exit);

     info->full_mc[indx].clear = (mnt_val) * (1<<(exp_val));

     SOC_PETRA_FLD_GET(regs->iqm.general_reject_set_configuration_dbuffs_reg[indx].rjct_set_fr_db_flmc_th_mnt, mnt_val, 430, exit);
     SOC_PETRA_FLD_GET(regs->iqm.general_reject_set_configuration_dbuffs_reg[indx].rjct_set_fr_db_flmc_th_exp, exp_val, 440, exit);

     info->full_mc[indx].set = (mnt_val) * (1<<(exp_val));
  }

#ifdef LINK_PB_LIBRARIES
  /*
   *	Extension for Soc_petra-B
   */
  if SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE
  {
    res = soc_pb_itm_glob_rcs_drop_get_unsafe(
            unit,
            info->mem_excess
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 450, exit);
  }
#endif /* LINK_PB_LIBRARIES */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_glob_rcs_drop_get_unsafe()",0,0);
}



/*********************************************************************
*     Defines packet queues categories - in contiguous blocks.
*     IQM queues are divided to 4 categories in contiguous
*     blocks. Category-4 from 'category-end-3' till the last
*     queue (32K).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_category_rngs_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ITM_CATEGORY_RNGS *info
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_CATEGORY_RNGS_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  SOC_PETRA_FLD_SET(regs->iqm.packet_queues_categories1_reg.top_pkt_qcat0, info->vsq_ctgry0_end, 0,exit);
  SOC_PETRA_FLD_SET(regs->iqm.packet_queues_categories1_reg.top_pkt_qcat1, info->vsq_ctgry1_end, 1, exit);
  SOC_PETRA_FLD_SET(regs->iqm.packet_queues_categories2_reg.top_pkt_qcat2, info->vsq_ctgry2_end, 2,exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_category_rngs_set_unsafe()",0,0);
}

/*********************************************************************
*     Defines packet queues categories - in contiguous blocks.
*     IQM queues are divided to 4 categories in contiguous
*     blocks. Category-4 from 'category-end-3' till the last
*     queue (32K).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_category_rngs_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ITM_CATEGORY_RNGS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_CATEGORY_RNGS_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  if ((info->vsq_ctgry0_end > info->vsq_ctgry1_end) ||
      (info->vsq_ctgry1_end > info->vsq_ctgry2_end) ||
      (info->vsq_ctgry0_end > info->vsq_ctgry2_end))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ITM_VSQ_CATEGORY_END_OUT_OF_ORDER_ERR, 10, exit);
  }
  /* SOC_PETRA_ITM_CATEGORY_INFO_VSQ_CTGRY_END_MIN may be changed and be grater than 0*/
  /* coverity[unsigned_compare : FALSE] */
  if ((info->vsq_ctgry0_end < SOC_PETRA_ITM_CATEGORY_INFO_VSQ_CTGRY_END_MIN) ||
    (info->vsq_ctgry0_end > SOC_PETRA_ITM_CATEGORY_INFO_VSQ_CTGRY_END_MAX))
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ITM_VSQ_CATEGORY_END_OUT_OF_RANGE_ERR, 20, exit);

  /* SOC_PETRA_ITM_CATEGORY_INFO_VSQ_CTGRY_END_MIN may be changed and be grater than 0*/
  /* coverity[unsigned_compare : FALSE] */
  if ((info->vsq_ctgry1_end < SOC_PETRA_ITM_CATEGORY_INFO_VSQ_CTGRY_END_MIN) ||
    (info->vsq_ctgry1_end > SOC_PETRA_ITM_CATEGORY_INFO_VSQ_CTGRY_END_MAX))
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ITM_VSQ_CATEGORY_END_OUT_OF_RANGE_ERR, 30, exit);

  /* SOC_PETRA_ITM_CATEGORY_INFO_VSQ_CTGRY_END_MIN may be changed and be grater than 0*/
  /* coverity[unsigned_compare : FALSE] */
  if ((info->vsq_ctgry2_end < SOC_PETRA_ITM_CATEGORY_INFO_VSQ_CTGRY_END_MIN) ||
    (info->vsq_ctgry2_end > SOC_PETRA_ITM_CATEGORY_INFO_VSQ_CTGRY_END_MAX))
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ITM_VSQ_CATEGORY_END_OUT_OF_RANGE_ERR, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_category_rngs_verify()",0,0);
}

/*********************************************************************
*     Defines packet queues categories - in contiguous blocks.
*     IQM queues are divided to 4 categories in contiguous
*     blocks. Category-4 from 'category-end-3' till the last
*     queue (32K).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_category_rngs_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_ITM_CATEGORY_RNGS *info
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_CATEGORY_RNGS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  SOC_PETRA_FLD_GET(regs->iqm.packet_queues_categories1_reg.top_pkt_qcat0, info->vsq_ctgry0_end, 10, exit);
  SOC_PETRA_FLD_GET(regs->iqm.packet_queues_categories1_reg.top_pkt_qcat1, info->vsq_ctgry1_end, 20, exit);
  SOC_PETRA_FLD_GET(regs->iqm.packet_queues_categories2_reg.top_pkt_qcat2, info->vsq_ctgry2_end, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_category_rngs_get_unsafe()",0,0);
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
  soc_petra_itm_admit_test_tmplt_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 admt_tst_ndx,
    SOC_SAND_IN  SOC_PETRA_ITM_ADMIT_TEST_TMPLT_INFO *info
  )
{
  uint32
    res,
    test_a_in_sand_u32,
    test_b_in_sand_u32;
  uint32
    test_a_index,
    test_b_index;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_ADMIT_TEST_TMPLT_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  res = soc_petra_itm_convert_admit_one_test_tmplt_to_u32(
          info->test_a,
          &test_a_in_sand_u32
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,0,exit);

  test_a_index = (admt_tst_ndx * 2);
  SOC_PETRA_FLD_SET(regs->iqm.reject_admission_reg.rjct_tmplt_set[test_a_index], test_a_in_sand_u32, 1,exit);

  res =
    soc_petra_itm_convert_admit_one_test_tmplt_to_u32(
      info->test_b,
      &test_b_in_sand_u32
    );
  SOC_SAND_CHECK_FUNC_RESULT(res,2,exit);

  test_b_index = (admt_tst_ndx * 2) + 1;
  SOC_PETRA_FLD_SET(regs->iqm.reject_admission_reg.rjct_tmplt_set[test_b_index], test_b_in_sand_u32, 3,exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_admit_test_tmplt_set_unsafe()",0,0);
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
  soc_petra_itm_admit_test_tmplt_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 admt_tst_ndx,
    SOC_SAND_IN  SOC_PETRA_ITM_ADMIT_TEST_TMPLT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_ADMIT_TEST_TMPLT_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  if (admt_tst_ndx > (SOC_PETRA_ITM_ADMIT_TSTS_LAST-1))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ITM_ADMT_TEST_NDX_OUT_OF_RANGE_ERR, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_admit_test_tmplt_verify()",0,0);
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
  soc_petra_itm_admit_test_tmplt_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 admt_tst_ndx,
    SOC_SAND_OUT SOC_PETRA_ITM_ADMIT_TEST_TMPLT_INFO *info
  )
{
  uint32
    res,
    test_a_in_sand_u32,
    test_b_in_sand_u32;
  uint32
    test_a_index,
    test_b_index;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_ADMIT_TEST_TMPLT_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  test_a_index = (admt_tst_ndx * 2);
  SOC_PETRA_FLD_GET(regs->iqm.reject_admission_reg.rjct_tmplt_set[test_a_index], test_a_in_sand_u32, 10, exit);

  res =
    soc_petra_itm_convert_u32_to_admit_one_test_tmplt(
      test_a_in_sand_u32,
      &(info->test_a)
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  test_b_index = (admt_tst_ndx * 2) + 1;
    SOC_PETRA_FLD_GET(regs->iqm.reject_admission_reg.rjct_tmplt_set[test_b_index], test_b_in_sand_u32, 20,exit);
    res =
      soc_petra_itm_convert_u32_to_admit_one_test_tmplt(
        test_b_in_sand_u32,
        &(info->test_b)
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_admit_test_tmplt_get_unsafe()",0,0);
}

STATIC uint32
  soc_petra_itm_cr_request_watch_dog_cycle_calc_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint32                  *cycle_time
  )
{
  uint32
    nof_queues = 0,
    qu_to_qu_time = 0,
    all_qus_time = 0,
    tmp,
    ccl_time;
  SOC_PETRA_REGS
    *regs;
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(cycle_time);

  regs = soc_petra_regs();

  /* watch queues */
  SOC_PETRA_FLD_GET(regs->ips.credit_watchdog_queue_boundaries_reg.cr_wd_bottom_q, tmp, 150, exit);
  SOC_PETRA_FLD_GET(regs->ips.credit_watchdog_queue_boundaries_reg.cr_wd_top_q, nof_queues, 160, exit);

  if (nof_queues > tmp)
  {
    nof_queues -= tmp;
    ++nof_queues;
  }
  else
  {
    nof_queues = 0;
  }

  /* time to pass from queue to another */
  SOC_PETRA_FLD_GET(regs->ips.credit_watchdog_configuration_reg.cr_wd_max_flow_msg_gen_rate, qu_to_qu_time, 170, exit);

  SOC_PETRA_FLD_GET(regs->ips.credit_watchdog_configuration_reg.cr_wd_min_scan_cycle_period, all_qus_time, 180, exit);

  if (all_qus_time == 0)
  {
    ccl_time = 0;
  }
  else
  {
    all_qus_time *= SOC_PETRA_ITM_WD_INFO_MIN_SCAN_CYCLE_PERIOD_MICRO_GRANULARITY;
    ccl_time = SOC_SAND_MAX(all_qus_time, qu_to_qu_time * nof_queues);
  }

  *cycle_time = ccl_time;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_cr_request_watch_dog_cycle_calc_unsafe()",0,0);
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
  soc_petra_itm_cr_request_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ITM_QT_NDX          qt_ndx,
    SOC_SAND_IN  SOC_PETRA_ITM_CR_REQUEST_INFO *info,
    SOC_SAND_OUT SOC_PETRA_ITM_CR_REQUEST_INFO *exact_info
  )
{
  uint32
    mul_pckt_deq_fld_val,
    cycle_time = 0,
    tmp;
  uint32
    res;
  SOC_PETRA_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_DATA
    ips_queue_size_based_thresholds_table_tbl_data;
  SOC_PETRA_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_DATA
    ips_credit_balance_based_thresholds_table_tbl_data;
  SOC_PETRA_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_DATA
    ips_empty_queue_credit_balance_table_tbl_data;
  SOC_PETRA_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_DATA
    ips_credit_watchdog_thresholds_table_tbl_data;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_CR_REQUEST_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);

  regs = soc_petra_regs();

  /* set hungry thresholds { */

  res = soc_petra_itm_cr_request_info_hungry_table_field_set(
          info->hungry_th.off_to_normal_th,
          &(ips_queue_size_based_thresholds_table_tbl_data.off_to_norm_msg_th),
          &(exact_info->hungry_th.off_to_normal_th)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_itm_cr_request_info_hungry_table_field_set(
          info->hungry_th.off_to_slow_th,
          &(ips_queue_size_based_thresholds_table_tbl_data.off_to_slow_msg_th),
          &(exact_info->hungry_th.off_to_slow_th)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_itm_cr_request_info_hungry_table_field_set(
          info->hungry_th.slow_to_normal_th,
          &(ips_queue_size_based_thresholds_table_tbl_data.slow_to_norm_msg_th),
          &(exact_info->hungry_th.slow_to_normal_th)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_petra_itm_cr_request_info_hungry_table_field_set(
          info->hungry_th.normal_to_slow_th,
          &(ips_queue_size_based_thresholds_table_tbl_data.norm_to_slow_msg_th),
          &(exact_info->hungry_th.normal_to_slow_th)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  ips_queue_size_based_thresholds_table_tbl_data.fsm_th_mul =
    SOC_PETRA_ITM_HUNGRY_TH_MULTIPLIER_VAL_TO_FIELD(info->hungry_th.multiplier);

  res = soc_petra_ips_queue_size_based_thresholds_table_tbl_set_unsafe(
          unit,
          qt_ndx,
          &ips_queue_size_based_thresholds_table_tbl_data
      );
  SOC_SAND_CHECK_FUNC_RESULT(res, 65, exit);

  exact_info->hungry_th.multiplier =
    SOC_PETRA_ITM_HUNGRY_TH_MULTIPLIER_FIELD_TO_VAL(
      ips_queue_size_based_thresholds_table_tbl_data.fsm_th_mul);
  /* set hungry thresholds } */

  /* set satisfied backoff/backlog thresholds { */
  res = soc_petra_itm_cr_request_info_satisfied_mnt_exp_table_field_set(
          info->satisfied_th.backlog_th.backlog_enter_th,
          &(ips_credit_balance_based_thresholds_table_tbl_data.backlog_enter_qcr_bal_th),
          &(exact_info->satisfied_th.backlog_th.backlog_enter_th)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  res = soc_petra_itm_cr_request_info_satisfied_mnt_exp_table_field_set(
          info->satisfied_th.backlog_th.backlog_exit_th,
          &(ips_credit_balance_based_thresholds_table_tbl_data.backlog_exit_qcr_bal_th),
          &(exact_info->satisfied_th.backlog_th.backlog_exit_th)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

  res = soc_petra_itm_cr_request_info_satisfied_mnt_exp_table_field_set(
          info->satisfied_th.backoff_th.backoff_enter_th,
          &(ips_credit_balance_based_thresholds_table_tbl_data.backoff_enter_qcr_bal_th),
          &(exact_info->satisfied_th.backoff_th.backoff_enter_th)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  res = soc_petra_itm_cr_request_info_satisfied_mnt_exp_table_field_set(
          info->satisfied_th.backoff_th.backoff_exit_th,
          &(ips_credit_balance_based_thresholds_table_tbl_data.backoff_exit_qcr_bal_th),
          &(exact_info->satisfied_th.backoff_th.backoff_exit_th)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

  res = soc_petra_ips_credit_balance_based_thresholds_table_tbl_set_unsafe(
          unit,
          qt_ndx,
          &ips_credit_balance_based_thresholds_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);

  /* set satisfied backoff/backlog thresholds } */

  /* set satisfied empty queue thresholds { */

  if (info->satisfied_th.empty_queues.satisfied_empty_q_th < 0)
  {
    ips_empty_queue_credit_balance_table_tbl_data.empty_qsatisfied_cr_bal =
      (SOC_SAND_U16_MAX + 1) + info->satisfied_th.empty_queues.satisfied_empty_q_th;
  }
  else
  {
    ips_empty_queue_credit_balance_table_tbl_data.empty_qsatisfied_cr_bal =
    info->satisfied_th.empty_queues.satisfied_empty_q_th;
  }

  exact_info->satisfied_th.empty_queues.satisfied_empty_q_th =
    info->satisfied_th.empty_queues.satisfied_empty_q_th;

  if (info->satisfied_th.empty_queues.max_credit_balance_empty_q < 0)
  {
    ips_empty_queue_credit_balance_table_tbl_data.max_empty_qcr_bal =
      (SOC_SAND_U16_MAX + 1) + info->satisfied_th.empty_queues.max_credit_balance_empty_q;
  }
  else
  {
    ips_empty_queue_credit_balance_table_tbl_data.max_empty_qcr_bal =
    info->satisfied_th.empty_queues.max_credit_balance_empty_q;
  }
  exact_info->satisfied_th.empty_queues.max_credit_balance_empty_q =
    info->satisfied_th.empty_queues.max_credit_balance_empty_q;

  ips_empty_queue_credit_balance_table_tbl_data.exceed_max_empty_qcr_bal =
    exact_info->satisfied_th.empty_queues.exceed_max_empty_q =
    info->satisfied_th.empty_queues.exceed_max_empty_q;

  res = soc_petra_ips_empty_queue_credit_balance_table_tbl_set_unsafe(
          unit,
          qt_ndx,
          &ips_empty_queue_credit_balance_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);

 /* set satisfied empty queue thresholds } */

 /* set Watchdog thresholds { */

/*
 * convert from mili-seconds to watch dog units
 */
  res = soc_petra_itm_cr_request_watch_dog_cycle_calc_unsafe(
          unit,
          &cycle_time
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

  if (!SOC_PETRA_IS_DEV_PETRA_A)
  {
    if ((cycle_time != 0) && (info->wd_th.cr_wd_dlt_q_th != 0))
    {
      tmp = info->wd_th.cr_wd_dlt_q_th * soc_petra_chip_kilo_ticks_per_sec_get(unit);
      ips_credit_watchdog_thresholds_table_tbl_data.wd_delete_qth = SOC_SAND_DIV_ROUND_UP(tmp,cycle_time);
      SOC_SAND_LIMIT_FROM_BELOW(ips_credit_watchdog_thresholds_table_tbl_data.wd_delete_qth, 0x2);
      exact_info->wd_th.cr_wd_dlt_q_th = cycle_time * ips_credit_watchdog_thresholds_table_tbl_data.wd_delete_qth / soc_petra_chip_kilo_ticks_per_sec_get(unit);
    }
    else
    {
      ips_credit_watchdog_thresholds_table_tbl_data.wd_delete_qth = 0;
      exact_info->wd_th.cr_wd_dlt_q_th = 0;
    }
  }
  else
  {
    ips_credit_watchdog_thresholds_table_tbl_data.wd_delete_qth = 0;
    exact_info->wd_th.cr_wd_dlt_q_th = 0;
  }

  if ((cycle_time != 0) && (info->wd_th.cr_wd_stts_msg_gen != 0))
  {
    tmp = info->wd_th.cr_wd_stts_msg_gen * soc_petra_chip_kilo_ticks_per_sec_get(unit);
    ips_credit_watchdog_thresholds_table_tbl_data.wd_status_msg_gen_period =
      SOC_SAND_DIV_ROUND_UP(tmp,cycle_time);
    if (ips_credit_watchdog_thresholds_table_tbl_data.wd_delete_qth >= 2)
    {
      if (ips_credit_watchdog_thresholds_table_tbl_data.wd_delete_qth == ips_credit_watchdog_thresholds_table_tbl_data.wd_status_msg_gen_period)
      {
        /*
         *	wd_delete_qth is at least 2
         */
        ips_credit_watchdog_thresholds_table_tbl_data.wd_status_msg_gen_period--;
      }
      else if (ips_credit_watchdog_thresholds_table_tbl_data.wd_delete_qth < ips_credit_watchdog_thresholds_table_tbl_data.wd_status_msg_gen_period)
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_CR_WD_DELETE_BEFORE_STATUS_MSG_ERR, 155, exit);
      }
    }

    exact_info->wd_th.cr_wd_stts_msg_gen = cycle_time * ips_credit_watchdog_thresholds_table_tbl_data.wd_status_msg_gen_period / soc_petra_chip_kilo_ticks_per_sec_get(unit);
  }
  else
  {
    ips_credit_watchdog_thresholds_table_tbl_data.wd_status_msg_gen_period = 0;
    exact_info->wd_th.cr_wd_stts_msg_gen = 0;
  }

  res = soc_petra_ips_credit_watchdog_thresholds_table_tbl_set_unsafe(
          unit,
          qt_ndx,
          &ips_credit_watchdog_thresholds_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);

  /* set Watchdog thresholds } */

  SOC_PETRA_FLD_GET(regs->ips.stored_credits_usage_configuration_reg.mul_pkt_deq, mul_pckt_deq_fld_val, 170, exit);
  tmp = SOC_SAND_BOOL2NUM(info->is_low_latency);
  SOC_SAND_SET_BIT(mul_pckt_deq_fld_val, tmp, qt_ndx);
  SOC_PETRA_FLD_SET(regs->ips.stored_credits_usage_configuration_reg.mul_pkt_deq, mul_pckt_deq_fld_val, 172, exit);
  exact_info->is_low_latency = info->is_low_latency;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_cr_request_set_unsafe()",0,0);
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
  soc_petra_itm_cr_request_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ITM_QT_NDX          qt_ndx,
    SOC_SAND_IN  SOC_PETRA_ITM_CR_REQUEST_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_CR_REQUEST_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  if (qt_ndx > (SOC_PETRA_ITM_IPS_QT_MAX))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ITM_IPS_QT_RNG_OUT_OF_RANGE_ERR, 5, exit);
  }

  if (soc_sand_abs(info->hungry_th.normal_to_slow_th) > \
    SOC_PETRA_ITM_HUNGRY_TH_MAX)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ITM_CR_REQUEST_HUNGRY_TH_OUT_OF_RANGE_ERR, 10, exit);
  }
  if (soc_sand_abs(info->hungry_th.off_to_normal_th) > \
    SOC_PETRA_ITM_HUNGRY_TH_MAX)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ITM_CR_REQUEST_HUNGRY_TH_OUT_OF_RANGE_ERR, 20, exit);
  }
  if (soc_sand_abs(info->hungry_th.off_to_slow_th) > \
    SOC_PETRA_ITM_HUNGRY_TH_MAX)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ITM_CR_REQUEST_HUNGRY_TH_OUT_OF_RANGE_ERR, 30, exit);
  }
  if (soc_sand_abs(info->hungry_th.slow_to_normal_th) > \
    SOC_PETRA_ITM_HUNGRY_TH_MAX)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ITM_CR_REQUEST_HUNGRY_TH_OUT_OF_RANGE_ERR, 40, exit);
  }

  if (info->hungry_th.multiplier && soc_sand_log2_round_down(info->hungry_th.multiplier) <= SOC_PETRA_ITM_HUNGRY_TH_MULTIPLIER_OFFSET)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ITM_CR_REQUEST_HUNGRY_TH_MULTIPLIER_OUT_OF_RANGE_ERR, 45, exit);
  }

  if (info->satisfied_th.backlog_th.backlog_exit_th> \
    SOC_PETRA_ITM_SATISFIED_TH_MAX)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ITM_CR_REQUEST_SATISFIED_TH_OUT_OF_RANGE_ERR, 50, exit);
  }

  if (info->satisfied_th.backlog_th.backlog_enter_th> \
    SOC_PETRA_ITM_SATISFIED_TH_MAX)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ITM_CR_REQUEST_SATISFIED_TH_OUT_OF_RANGE_ERR, 60, exit);
  }

  if (info->satisfied_th.backoff_th.backoff_exit_th> \
    SOC_PETRA_ITM_SATISFIED_TH_MAX)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ITM_CR_REQUEST_SATISFIED_TH_OUT_OF_RANGE_ERR, 70, exit);
  }

  if (info->satisfied_th.backoff_th.backoff_enter_th> \
    SOC_PETRA_ITM_SATISFIED_TH_MAX)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ITM_CR_REQUEST_SATISFIED_TH_OUT_OF_RANGE_ERR, 80, exit);
  }

  if (info->wd_th.cr_wd_dlt_q_th != 0)
  {
    if (!SOC_PETRA_IS_DEV_PETRA_A)
    {
      SOC_SAND_ERR_IF_OUT_OF_RANGE(
        info->wd_th.cr_wd_dlt_q_th, SOC_PETRA_ITM_CR_WD_Q_TH_MIN_MSEC, SOC_PETRA_ITM_CR_WD_Q_TH_MAX_MSEC,
        SOC_PETRA_CR_WD_DEL_TH_OUT_OF_RANGE, 85, exit
      );
    }
    else
    {
       SOC_SAND_SET_ERROR_CODE(SOC_PETRA_CR_WD_DEL_TH_UNSUPPORTED_ERR, 90, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_cr_request_verify()",0,0);
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
  soc_petra_itm_cr_request_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ITM_QT_NDX          qt_ndx,
    SOC_SAND_OUT SOC_PETRA_ITM_CR_REQUEST_INFO *info
  )
{
  uint32
    cycle_time = 0,
    mul_pckt_deq_fld_val,
    tmp;
  uint32
    res;
  SOC_PETRA_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_DATA
    ips_queue_size_based_thresholds_table_tbl_data;
  SOC_PETRA_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_DATA
    ips_credit_balance_based_thresholds_table_tbl_data;
  SOC_PETRA_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_DATA
    ips_empty_queue_credit_balance_table_tbl_data;
  SOC_PETRA_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_DATA
    ips_credit_watchdog_thresholds_table_tbl_data;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_CR_REQUEST_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  regs = soc_petra_regs();

  /* get hungry thresholds { */
  res = soc_petra_ips_queue_size_based_thresholds_table_tbl_get_unsafe(
          unit,
          qt_ndx,
          &ips_queue_size_based_thresholds_table_tbl_data
      );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_petra_itm_cr_request_info_hungry_table_field_get(
          ips_queue_size_based_thresholds_table_tbl_data.off_to_norm_msg_th,
          &(info->hungry_th.off_to_normal_th)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_itm_cr_request_info_hungry_table_field_get(
          ips_queue_size_based_thresholds_table_tbl_data.off_to_slow_msg_th,
          &(info->hungry_th.off_to_slow_th)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_itm_cr_request_info_hungry_table_field_get(
          ips_queue_size_based_thresholds_table_tbl_data.slow_to_norm_msg_th,
          &(info->hungry_th.slow_to_normal_th)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_petra_itm_cr_request_info_hungry_table_field_get(
          ips_queue_size_based_thresholds_table_tbl_data.norm_to_slow_msg_th,
          &(info->hungry_th.normal_to_slow_th)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  info->hungry_th.multiplier =
    SOC_PETRA_ITM_HUNGRY_TH_MULTIPLIER_FIELD_TO_VAL(
      ips_queue_size_based_thresholds_table_tbl_data.fsm_th_mul);
  /* get hungry thresholds } */

  /* get satisfied backoff/backlog thresholds { */

  res = soc_petra_ips_credit_balance_based_thresholds_table_tbl_get_unsafe(
          unit,
          qt_ndx,
          &ips_credit_balance_based_thresholds_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  res = soc_petra_itm_cr_request_info_satisfied_mnt_exp_table_field_get(
          ips_credit_balance_based_thresholds_table_tbl_data.backlog_enter_qcr_bal_th,
          &(info->satisfied_th.backlog_th.backlog_enter_th)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  res = soc_petra_itm_cr_request_info_satisfied_mnt_exp_table_field_get(
          ips_credit_balance_based_thresholds_table_tbl_data.backlog_exit_qcr_bal_th,
          &(info->satisfied_th.backlog_th.backlog_exit_th)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

  res = soc_petra_itm_cr_request_info_satisfied_mnt_exp_table_field_get(
          ips_credit_balance_based_thresholds_table_tbl_data.backoff_enter_qcr_bal_th,
          &(info->satisfied_th.backoff_th.backoff_enter_th)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  res = soc_petra_itm_cr_request_info_satisfied_mnt_exp_table_field_get(
          ips_credit_balance_based_thresholds_table_tbl_data.backoff_exit_qcr_bal_th,
          &(info->satisfied_th.backoff_th.backoff_exit_th)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

  /* get satisfied backoff/backlog thresholds } */

  /* get satisfied empty queue thresholds { */

  res = soc_petra_ips_empty_queue_credit_balance_table_tbl_get_unsafe(
          unit,
          qt_ndx,
          &ips_empty_queue_credit_balance_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);


  if (ips_empty_queue_credit_balance_table_tbl_data.empty_qsatisfied_cr_bal > SOC_SAND_I16_MAX )
  {
    info->satisfied_th.empty_queues.satisfied_empty_q_th =
      (int32)(ips_empty_queue_credit_balance_table_tbl_data.empty_qsatisfied_cr_bal -
      (SOC_SAND_U16_MAX + 1));
  }
  else
  {
    info->satisfied_th.empty_queues.satisfied_empty_q_th =
      (int32)(ips_empty_queue_credit_balance_table_tbl_data.empty_qsatisfied_cr_bal);
  }

  if(ips_empty_queue_credit_balance_table_tbl_data.max_empty_qcr_bal > SOC_SAND_I16_MAX )
  {
    info->satisfied_th.empty_queues.max_credit_balance_empty_q =
      (int32)(ips_empty_queue_credit_balance_table_tbl_data.max_empty_qcr_bal -
      (SOC_SAND_U16_MAX + 1));
  }
  else
  {
    info->satisfied_th.empty_queues.max_credit_balance_empty_q =
    (int32)(ips_empty_queue_credit_balance_table_tbl_data.max_empty_qcr_bal);
  }

  info->satisfied_th.empty_queues.exceed_max_empty_q =
    (uint8)ips_empty_queue_credit_balance_table_tbl_data.exceed_max_empty_qcr_bal;

  /* get satisfied empty queue thresholds } */

  /* get Watchdog thresholds { */
  res = soc_petra_ips_credit_watchdog_thresholds_table_tbl_get_unsafe(
          unit,
          qt_ndx,
          &ips_credit_watchdog_thresholds_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

  res = soc_petra_itm_cr_request_watch_dog_cycle_calc_unsafe(
          unit,
          &cycle_time
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);

  info->wd_th.cr_wd_dlt_q_th =
    ips_credit_watchdog_thresholds_table_tbl_data.wd_delete_qth * cycle_time / soc_petra_chip_kilo_ticks_per_sec_get(unit);

  info->wd_th.cr_wd_stts_msg_gen =
    ips_credit_watchdog_thresholds_table_tbl_data.wd_status_msg_gen_period  * cycle_time / soc_petra_chip_kilo_ticks_per_sec_get(unit);

  /* get Watchdog thresholds } */

  SOC_PETRA_FLD_GET(regs->ips.stored_credits_usage_configuration_reg.mul_pkt_deq, mul_pckt_deq_fld_val, 170, exit);
  tmp = SOC_SAND_GET_BIT(mul_pckt_deq_fld_val, qt_ndx);
  info->is_low_latency = SOC_SAND_NUM2BOOL(tmp);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_cr_request_get_unsafe()",0,0);
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
  soc_petra_itm_cr_discount_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ITM_CR_DISCNT_CLS_NDX cr_cls_ndx,
    SOC_SAND_IN  SOC_PETRA_ITM_CR_DISCOUNT_INFO *info
  )
{
  uint32
    res;
  SOC_PETRA_IQM_CREDIT_DISCOUNT_TABLE_TBL_DATA
    iqm_credit_discount_table_tbl_data;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_CR_DISCOUNT_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  iqm_credit_discount_table_tbl_data.crdt_disc_val = (uint32)soc_sand_abs(info->discount);
  iqm_credit_discount_table_tbl_data.discnt_sign = (info->discount < 0);

  res = soc_petra_iqm_credit_discount_table_tbl_set_unsafe(
          unit,
          cr_cls_ndx,
          &iqm_credit_discount_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_cr_discount_set_unsafe()",0,0);
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
  soc_petra_itm_cr_discount_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ITM_CR_DISCNT_CLS_NDX cr_cls_ndx,
    SOC_SAND_IN  SOC_PETRA_ITM_CR_DISCOUNT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_CR_DISCOUNT_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    cr_cls_ndx, SOC_PETRA_ITM_QT_CR_CLS_MAX,
    SOC_PETRA_ITM_CR_CLS_OUT_OF_RANGE_ERR, 10, exit
  );

  SOC_SAND_ERR_IF_OUT_OF_RANGE(
    info->discount, SOC_PETRA_ITM_CR_DISCOUNT_MIN_VAL, SOC_PETRA_ITM_CR_DISCOUNT_MAX_VAL,
    SOC_PETRA_ITM_CR_DISCOUNT_OUT_OF_RANGE_ERR, 20, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_cr_discount_verify()",cr_cls_ndx,info->discount);
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
  soc_petra_itm_cr_discount_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ITM_CR_DISCNT_CLS_NDX cr_cls_ndx,
    SOC_SAND_OUT SOC_PETRA_ITM_CR_DISCOUNT_INFO *info
  )
{
  uint32
    res;
  SOC_PETRA_IQM_CREDIT_DISCOUNT_TABLE_TBL_DATA
    iqm_credit_discount_table_tbl_data;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_CR_DISCOUNT_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_iqm_credit_discount_table_tbl_get_unsafe(
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
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_cr_discount_get_unsafe()",0,0);
}

/*********************************************************************
*     Each queue (VOQ) is assigned with a test template.
*     This Function sets the admit logic test of the queue
*     per rate-class and drop-precedence (there are 4
*     pre-configured by 'soc_petra_itm_admit_test_tmplt_set'
*     options for test types).
*     Notice that in a queue, is a packet is chosen to be
*     rejected normally, the admit test logic will not affect it.
*     For more information about the admit test template refer to
*     the description of 'soc_petra_itm_admit_test_tmplt_set'.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_queue_test_tmplt_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  SOC_PETRA_ITM_ADMIT_TSTS      test_tmplt
  )
{
  uint32
    res;
  SOC_PETRA_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_DATA
    iqm_packet_queue_red_parameters_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_QUEUE_TEST_TMPLT_SET_UNSAFE);

  res = soc_petra_iqm_packet_queue_red_parameters_table_tbl_get_unsafe(
          unit,
          rt_cls_ndx,
          drop_precedence_ndx,
          &iqm_packet_queue_red_parameters_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  iqm_packet_queue_red_parameters_table_tbl_data.addmit_logic =
    (uint32)test_tmplt;

  res = soc_petra_iqm_packet_queue_red_parameters_table_tbl_set_unsafe(
          unit,
          rt_cls_ndx,
          drop_precedence_ndx,
          &iqm_packet_queue_red_parameters_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_queue_test_tmplt_set_unsafe()",0,0);
}

/*********************************************************************
*     Each queue (VOQ) is assigned with a test template.
*     This Function sets the admit logic test of the queue
*     per rate-class and drop-precedence (there are 4
*     pre-configured by 'soc_petra_itm_admit_test_tmplt_set'
*     options for test types).
*     Notice that in a queue, is a packet is chosen to be
*     rejected normally, the admit test logic will not affect it.
*     For more information about the admit test template refer to
*     the description of 'soc_petra_itm_admit_test_tmplt_set'.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_queue_test_tmplt_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  SOC_PETRA_ITM_ADMIT_TSTS      test_tmplt
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_QUEUE_TEST_TMPLT_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    rt_cls_ndx, SOC_PETRA_ITM_QT_RT_CLS_MAX,
    SOC_PETRA_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    drop_precedence_ndx, SOC_PETRA_MAX_DROP_PRECEDENCE,
    SOC_PETRA_DRPP_OUT_OF_RANGE_ERR, 7, exit
  );

  if (test_tmplt > (SOC_PETRA_ITM_ADMIT_TSTS_LAST-1))
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ITM_ADMT_TEST_ID_OUT_OF_RANGE_ERR, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_queue_test_tmplt_verify()",0,0);
}

/*********************************************************************
*     Each queue (VOQ) is assigned with a test template.
*     This Function sets the admit logic test of the queue
*     per rate-class and drop-precedence (there are 4
*     pre-configured by 'soc_petra_itm_admit_test_tmplt_set'
*     options for test types).
*     Notice that in a queue, is a packet is chosen to be
*     rejected normally, the admit test logic will not affect it.
*     For more information about the admit test template refer to
*     the description of 'soc_petra_itm_admit_test_tmplt_set'.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_queue_test_tmplt_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_OUT SOC_PETRA_ITM_ADMIT_TSTS      *test_tmplt
  )
{
  uint32
    res;
  SOC_PETRA_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_DATA
    iqm_packet_queue_red_parameters_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_QUEUE_TEST_TMPLT_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(test_tmplt);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    rt_cls_ndx, SOC_PETRA_ITM_QT_RT_CLS_MAX,
    SOC_PETRA_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    drop_precedence_ndx, SOC_PETRA_MAX_DROP_PRECEDENCE,
    SOC_PETRA_DRPP_OUT_OF_RANGE_ERR, 7, exit
  );

  res = soc_petra_iqm_packet_queue_red_parameters_table_tbl_get_unsafe(
          unit,
          rt_cls_ndx,
          drop_precedence_ndx,
          &iqm_packet_queue_red_parameters_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  *test_tmplt =
    iqm_packet_queue_red_parameters_table_tbl_data.addmit_logic;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_queue_test_tmplt_get_unsafe()",0,0);
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
*     the functionpetra_itm_wred_info_set.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_wred_exp_wq_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                  exp_wq
  )
{
  uint32
    res;
  SOC_PETRA_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_DATA
    iqm_packet_queue_red_weight_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_WRED_EXP_WQ_SET_UNSAFE);

  res = soc_petra_iqm_packet_queue_red_weight_table_tbl_get_unsafe(
          unit,
          rt_cls_ndx,
          &iqm_packet_queue_red_weight_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  iqm_packet_queue_red_weight_table_tbl_data.pq_weight =
    exp_wq;

  res = soc_petra_iqm_packet_queue_red_weight_table_tbl_set_unsafe(
          unit,
          rt_cls_ndx,
          &iqm_packet_queue_red_weight_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_wred_exp_wq_set_unsafe()",0,0);
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
*     the functionpetra_itm_wred_info_set.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_wred_exp_wq_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                  exp_wq
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_WRED_EXP_WQ_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    rt_cls_ndx, SOC_PETRA_ITM_QT_RT_CLS_MAX,
    SOC_PETRA_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 10, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    exp_wq, SOC_PETRA_ITM_WQ_MAX,
    SOC_PETRA_ITM_WRED_EXP_WT_PARAMETER_OUT_OF_RANGE_ERR, 20, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_wred_exp_wq_verify()",0,0);
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
*     the functionpetra_itm_wred_info_set.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_wred_exp_wq_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_OUT  uint32                  *exp_wq
  )
{
  uint32
    res;
  SOC_PETRA_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_DATA
    iqm_packet_queue_red_weight_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_WRED_EXP_WQ_GET_UNSAFE);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    rt_cls_ndx, SOC_PETRA_ITM_QT_RT_CLS_MAX,
    SOC_PETRA_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 5, exit
  );

  res = soc_petra_iqm_packet_queue_red_weight_table_tbl_get_unsafe(
          unit,
          rt_cls_ndx,
          &iqm_packet_queue_red_weight_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  *exp_wq =
    (uint8)iqm_packet_queue_red_weight_table_tbl_data.pq_weight;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_wred_exp_wq_get_unsafe()",0,0);
}

/*********************************************************************
*     Sets WRED parameters per rate-class and drop precedence,
*     including wred-enable and the admit test logic. Also, as
*     part of the WRED parameters; max-queue,
*     WRED-thresholds/probability. To configure WRED
*     Configuration that is per queue-type only (exponential
*     weight queue), use the function
*     soc_petra_itm_wred_exp_wq_set.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_wred_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  SOC_PETRA_ITM_WRED_QT_DP_INFO *info,
    SOC_SAND_OUT SOC_PETRA_ITM_WRED_QT_DP_INFO *exact_info
  )
{
  uint32
    res;
  SOC_PETRA_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_DATA
    iqm_packet_queue_red_parameters_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_WRED_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);

  res = soc_petra_iqm_packet_queue_red_parameters_table_tbl_get_unsafe(
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

  res = soc_petra_itm_WRED_QT_DP_INFO_to_WRED_TBL_DATA(
          info,
          &iqm_packet_queue_red_parameters_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_itm_WRED_TBL_DATA_to_WRED_QT_DP_INFO(
          &iqm_packet_queue_red_parameters_table_tbl_data,
          exact_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_iqm_packet_queue_red_parameters_table_tbl_set_unsafe(
          unit,
          rt_cls_ndx,
          drop_precedence_ndx,
          &iqm_packet_queue_red_parameters_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_wred_set_unsafe()",0,0);
}

/*********************************************************************
*     Sets WRED parameters per rate-class and drop precedence,
*     including wred-enable and the admit test logic. Also, as
*     part of the WRED parameters; max-queue,
*     WRED-thresholds/probability. To configure WRED
*     Configuration that is per queue-type only (exponential
*     weight queue), use the function
*     soc_petra_itm_wred_exp_wq_set.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_wred_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  SOC_PETRA_ITM_WRED_QT_DP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_WRED_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    rt_cls_ndx, SOC_PETRA_ITM_QT_RT_CLS_MAX,
    SOC_PETRA_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    drop_precedence_ndx, SOC_PETRA_MAX_DROP_PRECEDENCE,
    SOC_PETRA_DRPP_OUT_OF_RANGE_ERR, 7, exit
  );

  if (info->min_avrg_th > info->max_avrg_th)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ITM_WRED_MIN_TH_HIGHER_THEN_MAX_TH_ERR, 50, exit);
  }

  if (info->min_avrg_th > SOC_PETRA_ITM_WRED_INFO_MIN_AVRG_TH_MAX)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ITM_WRED_MIN_AVRG_TH_OUT_OF_RANGE_ERR, 20, exit);
  }

  if (info->max_avrg_th > SOC_PETRA_ITM_WRED_INFO_MAX_AVRG_TH_MAX)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ITM_WRED_MAX_AVRG_TH_OUT_OF_RANGE_ERR, 30, exit);
  }

  if (info->max_probability > SOC_PETRA_ITM_WRED_QT_DP_INFO_MAX_PROBABILITY_MAX)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ITM_WRED_PROB_OUT_OF_RANGE_ERR, 40, exit);
  }

  if (info->max_packet_size > SOC_PETRA_ITM_WRED_MAX_PACKET_SIZE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ITM_WRED_MAX_PACKET_SIZE_OUT_OF_RANGE_ERR, 50, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_wred_verify()",0,0);
}

/*********************************************************************
*     Sets WRED parameters per rate-class and drop precedence,
*     including wred-enable and the admit test logic. Also, as
*     part of the WRED parameters; max-queue,
*     WRED-thresholds/probability. To configure WRED
*     Configuration that is per queue-type only (exponential
*     weight queue), use the function
*     soc_petra_itm_wred_exp_wq_set.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_wred_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_OUT SOC_PETRA_ITM_WRED_QT_DP_INFO *info
  )
{
  uint32
    res;
  SOC_PETRA_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_DATA
    iqm_packet_queue_red_parameters_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_WRED_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    rt_cls_ndx, SOC_PETRA_ITM_QT_RT_CLS_MAX,
    SOC_PETRA_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    drop_precedence_ndx, SOC_PETRA_MAX_DROP_PRECEDENCE,
    SOC_PETRA_DRPP_OUT_OF_RANGE_ERR, 7, exit
  );

  res = soc_petra_iqm_packet_queue_red_parameters_table_tbl_get_unsafe(
          unit,
          rt_cls_ndx,
          drop_precedence_ndx,
          &iqm_packet_queue_red_parameters_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_petra_itm_WRED_TBL_DATA_to_WRED_QT_DP_INFO(
          &iqm_packet_queue_red_parameters_table_tbl_data,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  info->wred_en =
    SOC_SAND_NUM2BOOL(iqm_packet_queue_red_parameters_table_tbl_data.pq_wred_en);

  info->ignore_packet_size =
    SOC_SAND_NUM2BOOL(iqm_packet_queue_red_parameters_table_tbl_data.pq_wred_pckt_sz_ignr);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_wred_get_unsafe()",0,0);
}

/*********************************************************************
*     Sets tail drop parameter - max-queue-size per rate-class
*     and drop precedence. The tail drop mechanism drops
*     packets that are mapped to queues that exceed thresholds
*     of this structure.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_tail_drop_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  SOC_PETRA_ITM_TAIL_DROP_INFO  *info,
    SOC_SAND_OUT SOC_PETRA_ITM_TAIL_DROP_INFO  *exact_info
  )
{
  uint32
    res,
    max_inst_q_siz_mnt_nof_bits,
    max_inst_q_siz_exp_nof_bits;
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_DATA
    iqm_packet_queue_red_parameters_table_tbl_data;
  uint32
    pq_max_que_size_mnt,
    pq_max_que_size_exp;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_TAIL_DROP_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);

  res = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_iqm_packet_queue_red_parameters_table_tbl_get_unsafe(
          unit,
          rt_cls_ndx,
          drop_precedence_ndx,
          &iqm_packet_queue_red_parameters_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  max_inst_q_siz_mnt_nof_bits = SOC_PETRA_FLD_NOF_BITS(SOC_PETRA_TBL(tables->iqm.packet_queue_red_parameters_table_tbl.pq_max_que_size_mnt));
  max_inst_q_siz_exp_nof_bits = SOC_PETRA_FLD_NOF_BITS(SOC_PETRA_TBL(tables->iqm.packet_queue_red_parameters_table_tbl.pq_max_que_size_exp));

  res = soc_sand_break_to_mnt_exp_round_up(
          SOC_SAND_DIV_ROUND_UP(info->max_inst_q_size, SOC_PETRA_ITM_DROP_TAIL_SIZE_RESOLUTION),
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

  exact_info->max_inst_q_size *= SOC_PETRA_ITM_DROP_TAIL_SIZE_RESOLUTION;

  res = soc_petra_iqm_packet_queue_red_parameters_table_tbl_set_unsafe(
          unit,
          rt_cls_ndx,
          drop_precedence_ndx,
          &iqm_packet_queue_red_parameters_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_tail_drop_set_unsafe()",rt_cls_ndx,drop_precedence_ndx);
}

/*********************************************************************
*     Sets tail drop parameter - max-queue-size per rate-class
*     and drop precedence. The tail drop mechanism drops
*     packets that are mapped to queues that exceed thresholds
*     of this structure.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_tail_drop_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  SOC_PETRA_ITM_TAIL_DROP_INFO  *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_TAIL_DROP_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    rt_cls_ndx, SOC_PETRA_ITM_QT_RT_CLS_MAX,
    SOC_PETRA_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    drop_precedence_ndx, SOC_PETRA_MAX_DROP_PRECEDENCE,
    SOC_PETRA_DRPP_OUT_OF_RANGE_ERR, 7, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->max_inst_q_size, SOC_PETRA_MAX_QUEUE_SIZE_BYTES,
    SOC_PETRA_QUEUE_SIZE_OUT_OF_RANGE_ERR, 9, exit
  )

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_tail_drop_verify()", rt_cls_ndx, drop_precedence_ndx);
}

/*********************************************************************
*     Sets tail drop parameter - max-queue-size per rate-class
*     and drop precedence. The tail drop mechanism drops
*     packets that are mapped to queues that exceed thresholds
*     of this structure.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_tail_drop_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_OUT SOC_PETRA_ITM_TAIL_DROP_INFO  *info
  )
{
  uint32
    res;
  SOC_PETRA_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_DATA
    iqm_packet_queue_red_parameters_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_TAIL_DROP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    rt_cls_ndx, SOC_PETRA_ITM_QT_RT_CLS_MAX,
    SOC_PETRA_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    drop_precedence_ndx, SOC_PETRA_MAX_DROP_PRECEDENCE,
    SOC_PETRA_DRPP_OUT_OF_RANGE_ERR, 7, exit
  );

  res = soc_petra_iqm_packet_queue_red_parameters_table_tbl_get_unsafe(
          unit,
          rt_cls_ndx,
          drop_precedence_ndx,
          &iqm_packet_queue_red_parameters_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  info->max_inst_q_size =
    ((iqm_packet_queue_red_parameters_table_tbl_data.pq_max_que_size_mnt) *
    (1<<(iqm_packet_queue_red_parameters_table_tbl_data.pq_max_que_size_exp)));

  info->max_inst_q_size *= SOC_PETRA_ITM_DROP_TAIL_SIZE_RESOLUTION;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_tail_drop_get_unsafe()",rt_cls_ndx,drop_precedence_ndx);
}

/*********************************************************************
*     Sets ingress-queue credit Watchdog thresholds and
*     configuration. includes: start-queue, end-queue and
*     wd-rates.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_cr_wd_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ITM_CR_WD_INFO      *info,
    SOC_SAND_OUT SOC_PETRA_ITM_CR_WD_INFO      *exact_info
  )
{
  uint32
    res,
    reg_val;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_CR_WD_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);

  regs = soc_petra_regs();

  /* set wd queue boundaries { */
  SOC_PETRA_FLD_SET(regs->ips.credit_watchdog_queue_boundaries_reg.cr_wd_bottom_q, info->bottom_queue, 10, exit);
  exact_info->bottom_queue = info->bottom_queue;

  SOC_PETRA_FLD_SET(regs->ips.credit_watchdog_queue_boundaries_reg.cr_wd_top_q, info->top_queue, 11, exit);
  exact_info->top_queue = info->top_queue;
  /* set wd queue boundaries } */

  /* set wd config { */
  res =
    soc_petra_chip_time_to_ticks(
      unit,
      info->max_flow_msg_gen_rate_nano,
      TRUE,
      1,
      TRUE,
      &reg_val
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  SOC_PETRA_FLD_SET(regs->ips.credit_watchdog_configuration_reg.cr_wd_max_flow_msg_gen_rate, reg_val, 21, exit);

  res =
    soc_petra_ticks_to_time(
      unit,
      reg_val,
      TRUE,
      1,
      &(exact_info->max_flow_msg_gen_rate_nano)
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

  res =
    soc_petra_chip_time_to_ticks(
      unit,
      info->min_scan_cycle_period_micro,
      FALSE,
      SOC_PETRA_ITM_WD_INFO_MIN_SCAN_CYCLE_PERIOD_MICRO_GRANULARITY,
      TRUE,
      &reg_val
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 24, exit);

  SOC_PETRA_FLD_SET(regs->ips.credit_watchdog_configuration_reg.cr_wd_min_scan_cycle_period, reg_val, 25,exit);

  res =
    soc_petra_ticks_to_time(
      unit,
      reg_val,
      FALSE,
      SOC_PETRA_ITM_WD_INFO_MIN_SCAN_CYCLE_PERIOD_MICRO_GRANULARITY,
      &(exact_info->min_scan_cycle_period_micro)
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  /* set wd config } */
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_cr_wd_set_unsafe()",0,0);
}

/*********************************************************************
*     Sets ingress-queue credit Watchdog thresholds and
*     configuration. includes: start-queue, end-queue and
*     wd-rates.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_cr_wd_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ITM_CR_WD_INFO      *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_CR_WD_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  if (info->bottom_queue > SOC_PETRA_NOF_QUEUES)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_QUEUE_ID_OUT_OF_RANGE_ERR, 10, exit);
  }

  if (info->top_queue > SOC_PETRA_NOF_QUEUES)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_QUEUE_ID_OUT_OF_RANGE_ERR, 15, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_cr_wd_verify()",0,0);
}

/*********************************************************************
*     Sets ingress-queue credit Watchdog thresholds and
*     configuration. includes: start-queue, end-queue and
*     wd-rates.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_cr_wd_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_ITM_CR_WD_INFO      *info
  )
{
  uint32
    res,
    reg_val = 0;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_CR_WD_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_SET;
  regs = soc_petra_regs();

  /* get wd queue boundaries { */
  SOC_PETRA_FLD_GET(regs->ips.credit_watchdog_queue_boundaries_reg.cr_wd_bottom_q, info->bottom_queue, 10,exit);
  SOC_PETRA_FLD_GET(regs->ips.credit_watchdog_queue_boundaries_reg.cr_wd_top_q, info->top_queue, 15,exit);
 /*
  info->bottom_queue = (uint32)reg_val;
  info->top_queue = (uint32)reg_val;*/

  /* get wd queue boundaries } */

  /* get wd config { */
  reg_val = 0;
  SOC_PETRA_FLD_GET(regs->ips.credit_watchdog_configuration_reg.cr_wd_max_flow_msg_gen_rate, reg_val, 6, exit);
  res =
    soc_petra_ticks_to_time(
      unit,
      reg_val,
      TRUE,
      1,
      &(info->max_flow_msg_gen_rate_nano)
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

  reg_val = 0;
  SOC_PETRA_FLD_GET(regs->ips.credit_watchdog_configuration_reg.cr_wd_min_scan_cycle_period, reg_val, 24, exit);

  res =
    soc_petra_ticks_to_time(
      unit,
      reg_val,
      FALSE,
      SOC_PETRA_ITM_WD_INFO_MIN_SCAN_CYCLE_PERIOD_MICRO_GRANULARITY,
      &(info->min_scan_cycle_period_micro)
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 28, exit);

  /* get wd config } */
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_cr_wd_get_unsafe()",0,0);
}

/*********************************************************************
*     Each Virtual Statistics Queue has a VSQ-Rate-Class.
*     This function assigns a VSQ with its Rate Class.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_vsq_qt_rt_cls_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  SOC_PETRA_ITM_VSQ_NDX         vsq_in_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_VSQ_QT_RT_CLS_SET_UNSAFE);

  switch(vsq_group_ndx)
  {
  case SOC_PETRA_ITM_VSQ_GROUP_CTGRY:
      res = soc_petra_itm_vsq_group_a_set_rt_class(
              unit,
              vsq_rt_cls,
              vsq_in_group_ndx
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    break;
  case SOC_PETRA_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS:
      res = soc_petra_itm_vsq_group_b_set_rt_class(
              unit,
              vsq_rt_cls,
              vsq_in_group_ndx
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    break;
  case SOC_PETRA_ITM_VSQ_GROUP_CTGRY_2_3_CNCTN_CLS:
      res = soc_petra_itm_vsq_group_c_set_rt_class(
              unit,
              vsq_rt_cls,
              vsq_in_group_ndx
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
      break;
  case SOC_PETRA_ITM_VSQ_GROUP_STTSTCS_TAG:
      res = soc_petra_itm_vsq_group_d_set_rt_class(
              unit,
              vsq_rt_cls,
              vsq_in_group_ndx
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
      break;
  default:
      break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_vsq_qt_rt_cls_set_unsafe()",0,0);
}

/*********************************************************************
*     Sets Virtual Statistics Queue Rate-Class.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_vsq_qt_rt_cls_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  SOC_PETRA_ITM_VSQ_NDX         vsq_in_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_VSQ_QT_RT_CLS_VERIFY);

  res = soc_petra_itm_vsq_idx_verify(
          vsq_group_ndx,
          vsq_in_group_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (vsq_rt_cls > (SOC_PETRA_ITM_VSQ_QT_RT_CLS_MAX))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ITM_VSQ_QT_RT_OUT_OF_RANGE_ERR, 40, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_vsq_qt_rt_cls_verify()",0,0);
}

/*********************************************************************
*     Sets Virtual Statistics Queue Rate-Class.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_vsq_qt_rt_cls_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  SOC_PETRA_ITM_VSQ_NDX         vsq_in_group_ndx,
    SOC_SAND_OUT uint32                 *vsq_rt_cls
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_VSQ_QT_RT_CLS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(vsq_rt_cls);

  res = soc_petra_itm_vsq_idx_verify(
          vsq_group_ndx,
          vsq_in_group_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  switch(vsq_group_ndx)
  {
    case SOC_PETRA_ITM_VSQ_GROUP_CTGRY:
      res = soc_petra_itm_vsq_group_a_get_rt_class(
              unit,
              vsq_in_group_ndx,
              vsq_rt_cls
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
      break;
    case SOC_PETRA_ITM_VSQ_GROUP_CTGRY_TRAFFIC_CLS:
      res = soc_petra_itm_vsq_group_b_get_rt_class(
              unit,
              vsq_in_group_ndx,
              vsq_rt_cls
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);
      break;
    case SOC_PETRA_ITM_VSQ_GROUP_CTGRY_2_3_CNCTN_CLS:
      res = soc_petra_itm_vsq_group_c_get_rt_class(
              unit,
              vsq_in_group_ndx,
              vsq_rt_cls
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);
      break;
    case SOC_PETRA_ITM_VSQ_GROUP_STTSTCS_TAG:
      res = soc_petra_itm_vsq_group_d_get_rt_class(
              unit,
              vsq_in_group_ndx,
              vsq_rt_cls
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 18, exit);
      break;
    default:
      break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_vsq_qt_rt_cls_get_unsafe()",0,0);
}

/*********************************************************************
*     Sets Virtual Statistics Queue, includes: vsq-id,
*     rate-class
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_vsq_fc_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  SOC_PETRA_ITM_VSQ_FC_INFO     *info,
    SOC_SAND_OUT SOC_PETRA_ITM_VSQ_FC_INFO     *exact_info
  )
{
  uint32  res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_VSQ_FC_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);


  res = soc_petra_itm_vsq_group_set_fc_info(
          unit,
          vsq_group_ndx,
          vsq_rt_cls_ndx,
          info,
          exact_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,1,exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_vsq_fc_set_unsafe()",0,0);
}

/*********************************************************************
*     Sets Virtual Statistics Queue, includes: vsq-id,
*     rate-class
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_vsq_fc_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  SOC_PETRA_ITM_VSQ_FC_INFO     *info
  )
{
  uint32
    max_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_VSQ_FC_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_group_ndx, SOC_PETRA_ITM_VSQ_GROUP_LAST-1,
    SOC_PETRA_ITM_VSQ_GROUP_ID_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_rt_cls_ndx, SOC_PETRA_ITM_VSQ_QT_RT_CLS_MAX,
    SOC_PETRA_ITM_VSQ_QT_RT_OUT_OF_RANGE_ERR, 7, exit
  );

  max_val = SOC_PETRA_ITM_VSQ_FC_BD_SIZE_MAX;
  if (info->bd_size_fc.clear > max_val)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_VSQ_FC_PARAMETER_OUT_OF_RANGE_ERR, 50, exit);
  }

  if (info->bd_size_fc.set > max_val)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_VSQ_FC_PARAMETER_OUT_OF_RANGE_ERR, 60, exit);
  }

  max_val = SOC_PETRA_ITM_VSQ_FC_Q_SIZE_MAX;
  if (info->q_size_fc.clear > max_val)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_VSQ_FC_PARAMETER_OUT_OF_RANGE_ERR, 70, exit);
  }

  if (info->q_size_fc.set > max_val)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_VSQ_FC_PARAMETER_OUT_OF_RANGE_ERR, 80, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_vsq_fc_verify()",0,0);
}

/*********************************************************************
*     Sets Virtual Statistics Queue, includes: vsq-id,
*     rate-class
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_vsq_fc_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_OUT SOC_PETRA_ITM_VSQ_FC_INFO     *info
  )
{
  uint32
    res;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_VSQ_FC_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_group_ndx, (SOC_PETRA_ITM_VSQ_GROUP_LAST-1),
    SOC_PETRA_ITM_VSQ_GROUP_ID_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_rt_cls_ndx, SOC_PETRA_ITM_VSQ_QT_RT_CLS_MAX,
    SOC_PETRA_ITM_VSQ_QT_RT_OUT_OF_RANGE_ERR, 7, exit
  );

  res = soc_petra_itm_vsq_group_get_fc_info(
          unit,
          vsq_group_ndx,
          vsq_rt_cls_ndx,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res,1,exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_vsq_fc_get_unsafe()",0,0);
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
  soc_petra_itm_vsq_tail_drop_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  SOC_PETRA_ITM_VSQ_TAIL_DROP_INFO  *info,
    SOC_SAND_OUT  SOC_PETRA_ITM_VSQ_TAIL_DROP_INFO  *exact_info
  )
{
  uint32
    res,
    max_inst_q_siz_words_mnt_nof_bits,
    max_inst_q_siz_words_exp_nof_bits,
    max_inst_q_siz_bds_mnt_nof_bits,
    max_inst_q_siz_bds_exp_nof_bits;
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA
    iqm_vsq_queue_parameters_table_group_tbl_data;
  uint32
    vq_max_size_words_mnt,
    vq_max_size_words_exp,
    vq_max_szie_bds_mnt,
    vq_max_szie_bds_exp;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_VSQ_TAIL_DROP_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  res = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_iqm_vsq_queue_parameters_table_group_tbl_get_unsafe(
          unit,
          vsq_group_ndx,
          vsq_rt_cls_ndx,
          drop_precedence_ndx,
          &iqm_vsq_queue_parameters_table_group_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  max_inst_q_siz_words_mnt_nof_bits =
    SOC_PETRA_TBL(tables->iqm.vsq_queue_parameters_table_group_tbl[vsq_group_ndx].vq_max_size_words_mnt.msb) -
    SOC_PETRA_TBL(tables->iqm.vsq_queue_parameters_table_group_tbl[vsq_group_ndx].vq_max_size_words_mnt.lsb) + 1;
  max_inst_q_siz_words_exp_nof_bits =
    SOC_PETRA_TBL(tables->iqm.vsq_queue_parameters_table_group_tbl[vsq_group_ndx].vq_max_size_words_exp.msb) -
    SOC_PETRA_TBL(tables->iqm.vsq_queue_parameters_table_group_tbl[vsq_group_ndx].vq_max_size_words_exp.lsb) + 1;
  max_inst_q_siz_bds_mnt_nof_bits =
    SOC_PETRA_TBL(tables->iqm.vsq_queue_parameters_table_group_tbl[vsq_group_ndx].vq_max_szie_bds_mnt.msb) -
    SOC_PETRA_TBL(tables->iqm.vsq_queue_parameters_table_group_tbl[vsq_group_ndx].vq_max_szie_bds_mnt.lsb) + 1;
  max_inst_q_siz_bds_exp_nof_bits =
    SOC_PETRA_TBL(tables->iqm.vsq_queue_parameters_table_group_tbl[vsq_group_ndx].vq_max_szie_bds_exp.msb) -
    SOC_PETRA_TBL(tables->iqm.vsq_queue_parameters_table_group_tbl[vsq_group_ndx].vq_max_szie_bds_exp.lsb) + 1;

    res = soc_sand_break_to_mnt_exp_round_up(
            SOC_SAND_DIV_ROUND_UP(info->max_inst_q_size,SOC_PETRA_ITM_DROP_TAIL_SIZE_RESOLUTION),
            max_inst_q_siz_words_mnt_nof_bits,
            max_inst_q_siz_words_exp_nof_bits,
            0,
            &(vq_max_size_words_mnt),
            &(vq_max_size_words_exp)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    iqm_vsq_queue_parameters_table_group_tbl_data.vq_max_size_words_mnt = vq_max_size_words_mnt;
    iqm_vsq_queue_parameters_table_group_tbl_data.vq_max_size_words_exp = vq_max_size_words_exp;

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
      exact_info->max_inst_q_size *= SOC_PETRA_ITM_DROP_TAIL_SIZE_RESOLUTION;
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

    res = soc_petra_iqm_vsq_queue_parameters_table_group_tbl_set_unsafe(
            unit,
            vsq_group_ndx,
            vsq_rt_cls_ndx,
            drop_precedence_ndx,
            &iqm_vsq_queue_parameters_table_group_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_vsq_tail_drop_set_unsafe()",0,0);
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
  soc_petra_itm_vsq_tail_drop_verify(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  SOC_PETRA_ITM_VSQ_GROUP           vsq_group_ndx,
    SOC_SAND_IN  uint32                     vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                     drop_precedence_ndx,
    SOC_SAND_IN  SOC_PETRA_ITM_VSQ_TAIL_DROP_INFO  *info
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_VSQ_TAIL_DROP_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_group_ndx, (SOC_PETRA_ITM_VSQ_GROUP_LAST-1),
    SOC_PETRA_ITM_VSQ_GROUP_ID_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_rt_cls_ndx, SOC_PETRA_ITM_VSQ_QT_RT_CLS_MAX,
    SOC_PETRA_ITM_VSQ_QT_RT_OUT_OF_RANGE_ERR, 7, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    drop_precedence_ndx, SOC_PETRA_MAX_DROP_PRECEDENCE,
    SOC_PETRA_DRPP_OUT_OF_RANGE_ERR, 9, exit
  );

  /* Every 32 bit value is valid for max_inst_q_size */

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->max_inst_q_size_bds,SOC_PETRA_ITM_MAX_INST_Q_BDS_SIZE,
    SOC_PETRA_ITM_VSQ_MAX_INST_Q_SIZ_PARAMETER_OUT_OF_RANGE_ERR, 50, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_vsq_tail_drop_verify()",0,0);
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
  soc_petra_itm_vsq_tail_drop_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_OUT SOC_PETRA_ITM_VSQ_TAIL_DROP_INFO  *info
  )
{
  uint32
    res;
  SOC_PETRA_TBLS
    *tables = NULL;
  SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA
    iqm_vsq_queue_parameters_table_group_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_VSQ_TAIL_DROP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_group_ndx, (SOC_PETRA_ITM_VSQ_GROUP_LAST-1),
    SOC_PETRA_ITM_VSQ_GROUP_ID_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_rt_cls_ndx, SOC_PETRA_ITM_VSQ_QT_RT_CLS_MAX,
    SOC_PETRA_ITM_VSQ_QT_RT_OUT_OF_RANGE_ERR, 7, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    drop_precedence_ndx, SOC_PETRA_MAX_DROP_PRECEDENCE,
    SOC_PETRA_DRPP_OUT_OF_RANGE_ERR, 9, exit
  );

  res = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_iqm_vsq_queue_parameters_table_group_tbl_get_unsafe(
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
      info->max_inst_q_size *= SOC_PETRA_ITM_DROP_TAIL_SIZE_RESOLUTION;
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
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_vsq_tail_drop_get_unsafe()",0,0);
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
    soc_petra_itm_vsq_tail_drop_get_default_unsafe (
        SOC_SAND_IN  int                 unit,
        SOC_SAND_OUT SOC_PETRA_ITM_VSQ_TAIL_DROP_INFO  *info
    )
{
    SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_VSQ_TAIL_DROP_GET_DEFAULT_UNSAFE);
    SOC_SAND_CHECK_NULL_INPUT(info);
    info->max_inst_q_size = 0x0;
    info->max_inst_q_size_bds = 0x0;

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
  soc_petra_itm_vsq_wred_gen_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  SOC_PETRA_ITM_VSQ_WRED_GEN_INFO *info
  )
{
  uint32
    res;
  SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_DATA
    iqm_vsq_flow_control_parameters_table_group_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_VSQ_WRED_GEN_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_iqm_vsq_flow_control_parameters_table_group_tbl_get_unsafe(
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

  res = soc_petra_iqm_vsq_flow_control_parameters_table_group_tbl_set_unsafe(
          unit,
          vsq_group_ndx,
          vsq_rt_cls_ndx,
          &iqm_vsq_flow_control_parameters_table_group_tbl_data
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_vsq_wred_gen_set_unsafe()",0,0);
}

/*********************************************************************
*     This procedure sets VSQ WRED general configurations,
*     includes: WRED-enable and exponential-weight-queue (for
*     the WRED algorithm).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_vsq_wred_gen_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  SOC_PETRA_ITM_VSQ_WRED_GEN_INFO *info
  )
{
  uint32
    res,
    max_val;
  SOC_PETRA_TBLS
    *tables = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_VSQ_WRED_GEN_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  res = soc_petra_tbls_get(&(tables));
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_group_ndx, (SOC_PETRA_ITM_VSQ_GROUP_LAST-1),
    SOC_PETRA_ITM_VSQ_GROUP_ID_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_rt_cls_ndx, SOC_PETRA_ITM_VSQ_QT_RT_CLS_MAX,
    SOC_PETRA_ITM_VSQ_QT_RT_OUT_OF_RANGE_ERR, 7, exit
  );

  max_val =
    SOC_PETRA_FLD_MAX(
      SOC_PETRA_TBL(tables->iqm.vsq_flow_control_parameters_table_group_tbl[vsq_group_ndx].red_weight_q)
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if (info->exp_wq > max_val)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ITM_VSQ_WRED_EXP_WT_PARAMETER_OUT_OF_RANGE_ERR, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_vsq_wred_gen_verify()",0,0);
}

/*********************************************************************
*     This procedure sets VSQ WRED general configurations,
*     includes: WRED-enable and exponential-weight-queue (for
*     the WRED algorithm).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_vsq_wred_gen_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_OUT SOC_PETRA_ITM_VSQ_WRED_GEN_INFO *info
  )
{
  uint32
    res;
  SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_DATA
    iqm_vsq_flow_control_parameters_table_group_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_VSQ_WRED_GEN_GET_UNSAFE);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_group_ndx, (SOC_PETRA_ITM_VSQ_GROUP_LAST-1),
    SOC_PETRA_ITM_VSQ_GROUP_ID_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_rt_cls_ndx, SOC_PETRA_ITM_VSQ_QT_RT_CLS_MAX,
    SOC_PETRA_ITM_VSQ_QT_RT_OUT_OF_RANGE_ERR, 7, exit
  );

  res = soc_petra_iqm_vsq_flow_control_parameters_table_group_tbl_get_unsafe(
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
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_vsq_wred_gen_get_unsafe()",0,0);
}

/*********************************************************************
*     A WRED test for each packet versus the packet queue or
*     VSQ that the packet is mapped to is performed. This
*     procedure sets Virtual Statistics Queue WRED, includes:
*     WRED-thresholds/probability.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_vsq_wred_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  SOC_PETRA_ITM_WRED_QT_DP_INFO *info,
    SOC_SAND_OUT SOC_PETRA_ITM_WRED_QT_DP_INFO *exact_info
  )
{
  uint32
    res;
  SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA
    iqm_vsq_queue_parameters_table_group_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_VSQ_WRED_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  res = soc_petra_iqm_vsq_queue_parameters_table_group_tbl_get_unsafe(
          unit,
          vsq_group_ndx,
          vsq_rt_cls_ndx,
          drop_precedence_ndx,
          &iqm_vsq_queue_parameters_table_group_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_petra_itm_vsq_WRED_QT_DP_INFO_to_WRED_TBL_DATA(
          info,
          &iqm_vsq_queue_parameters_table_group_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_itm_vsq_WRED_TBL_DATA_to_WRED_QT_DP_INFO(
          &iqm_vsq_queue_parameters_table_group_tbl_data,
          exact_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_iqm_vsq_queue_parameters_table_group_tbl_set_unsafe(
          unit,
          vsq_group_ndx,
          vsq_rt_cls_ndx,
          drop_precedence_ndx,
          &iqm_vsq_queue_parameters_table_group_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_vsq_wred_set_unsafe()",0,0);
}

/*********************************************************************
*     A WRED test for each packet versus the packet queue or
*     VSQ that the packet is mapped to is performed. This
*     procedure sets Virtual Statistics Queue WRED, includes:
*     WRED-thresholds/probability.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_vsq_wred_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_IN  SOC_PETRA_ITM_WRED_QT_DP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_VSQ_WRED_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_group_ndx, (SOC_PETRA_ITM_VSQ_GROUP_LAST-1),
    SOC_PETRA_ITM_VSQ_GROUP_ID_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_rt_cls_ndx, SOC_PETRA_ITM_VSQ_QT_RT_CLS_MAX,
    SOC_PETRA_ITM_VSQ_QT_RT_OUT_OF_RANGE_ERR, 7, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    drop_precedence_ndx, SOC_PETRA_MAX_DROP_PRECEDENCE,
    SOC_PETRA_DRPP_OUT_OF_RANGE_ERR, 9, exit
  );

  if (info->min_avrg_th > info->max_avrg_th)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ITM_VSQ_WRED_MIN_TH_HIGHER_THEN_MAX_TH_ERR, 60, exit);
  }

  if (info->min_avrg_th > SOC_PETRA_ITM_WRED_INFO_MIN_AVRG_TH_MAX)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ITM_VSQ_MIN_AVRG_TH_OUT_OF_RANGE_ERR, 70, exit);
  }

  if (info->max_avrg_th > SOC_PETRA_ITM_WRED_INFO_MAX_AVRG_TH_MAX)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ITM_VSQ_MAX_AVRG_TH_OUT_OF_RANGE_ERR, 80, exit);
  }

  if (info->max_probability > SOC_PETRA_ITM_WRED_MAX_PROB)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ITM_WRED_PROB_OUT_OF_RANGE_ERR, 90, exit);
  }

  if (info->max_packet_size > SOC_PETRA_ITM_WRED_MAX_PACKET_SIZE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ITM_WRED_MAX_PACKET_SIZE_OUT_OF_RANGE_ERR, 100, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_vsq_wred_verify()",0,0);
}

/*********************************************************************
*     A WRED test for each packet versus the packet queue or
*     VSQ that the packet is mapped to is performed. This
*     procedure sets Virtual Statistics Queue WRED, includes:
*     WRED-thresholds/probability.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_vsq_wred_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  uint32                 vsq_rt_cls_ndx,
    SOC_SAND_IN  uint32                 drop_precedence_ndx,
    SOC_SAND_OUT SOC_PETRA_ITM_WRED_QT_DP_INFO *info
  )
{
  uint32
    res;
  SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA
    iqm_vsq_queue_parameters_table_group_tbl_data;
  SOC_PETRA_ITM_VSQ_WRED_GEN_INFO
    wred_gen;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_VSQ_WRED_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  soc_petra_PETRA_ITM_VSQ_WRED_GEN_INFO_clear(&wred_gen);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_group_ndx, (SOC_PETRA_ITM_VSQ_GROUP_LAST-1),
    SOC_PETRA_ITM_VSQ_GROUP_ID_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    vsq_rt_cls_ndx, SOC_PETRA_ITM_VSQ_QT_RT_CLS_MAX,
    SOC_PETRA_ITM_VSQ_QT_RT_OUT_OF_RANGE_ERR, 7, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    drop_precedence_ndx, SOC_PETRA_MAX_DROP_PRECEDENCE,
    SOC_PETRA_DRPP_OUT_OF_RANGE_ERR, 9, exit
  );

  res = soc_petra_iqm_vsq_queue_parameters_table_group_tbl_get_unsafe(
          unit,
          vsq_group_ndx,
          vsq_rt_cls_ndx,
          drop_precedence_ndx,
          &iqm_vsq_queue_parameters_table_group_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_petra_itm_vsq_WRED_TBL_DATA_to_WRED_QT_DP_INFO(
          &iqm_vsq_queue_parameters_table_group_tbl_data,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  /*
   *  Get the WRED enable indication from the VSQ WRED generation API
   *  (not per DP)
   */

  res = soc_petra_itm_vsq_wred_gen_get_unsafe(
          unit,
          vsq_group_ndx,
          vsq_rt_cls_ndx,
          &wred_gen
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  info->wred_en = wred_gen.wred_en;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_vsq_wred_get_unsafe()",0,0);
}

/*********************************************************************
*     Defines the way the Statistics Tag is used.
*     The statistics tag can be used in the Statistics Interface,
*     for mapping to VSQs and further used for VSQ-based drop/FC decisions,
*     and for setting the packet Drop Precedence
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_stag_set_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  SOC_PETRA_ITM_STAG_INFO      *info
  )
{
  uint32
    en_fld_val = 0x0,
    deq_fld_val = 0x0,
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_STAG_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  switch(info->enable_mode) {
  case SOC_PETRA_ITM_STAG_ENABLE_MODE_DISABLED:
    break;
  case SOC_PETRA_ITM_STAG_ENABLE_MODE_STAT_IF_NO_DEQ:
    en_fld_val = 0x1;
    break;
  case SOC_PETRA_ITM_STAG_ENABLE_MODE_ENABLED_WITH_DEQ:
    en_fld_val = 0x1;
    deq_fld_val = 0x1;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_STAG_ENABLE_MODE_OUT_OF_RANGE_ERR, 5, exit);
  }

  SOC_PA_FLD_SET(regs->iqm.statistics_tag_configuration_reg.stat_tag_vsq_en, en_fld_val, 10,exit);
  SOC_PA_FLD_SET(regs->iqm.statistics_tag_configuration_reg.stat_tag_vsq_msb, info->vsq_index_msb, 11, exit);
  SOC_PA_FLD_SET(regs->iqm.statistics_tag_configuration_reg.stat_tag_vsq_lsb, info->vsq_index_lsb, 12, exit);
  SOC_PA_FLD_SET(regs->iqm.statistics_tag_configuration_reg.stat_tag_dropp_en, info->dropp_en, 13, exit);
  SOC_PA_FLD_SET(regs->iqm.statistics_tag_configuration_reg.stat_tag_dropp_lsb, info->dropp_lsb, 14, exit);
  SOC_PA_FLD_SET(regs->iqm.statistics_tag_configuration_reg.stat_tag_deq_en, deq_fld_val, 15, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_stag_set_unsafe()",0,0);
}

/*********************************************************************
*     Defines the way the Statistics Tag is used.
*     The statistics tag can be used in the Statistics Interface,
*     for mapping to VSQs and further used for VSQ-based drop/FC decisions,
*     and for setting the packet Drop Precedence
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_stag_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ITM_STAG_INFO       *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_STAG_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

  if (info->vsq_index_lsb > info->vsq_index_msb)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ITM_VSQ_LBL_IN_STAT_TAG_LSB_LARGER_THAN_MSB_ERR, 10, exit);
  }
  if (info->vsq_index_lsb > SOC_PETRA_ITM_STATISTICS_TAG_NOF_BITS)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ITM_VSQ_LBL_OUT_OF_RANGE_ERR, 20, exit);
  }

  if (info->vsq_index_msb > SOC_PETRA_ITM_STATISTICS_TAG_NOF_BITS)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ITM_VSQ_LBL_OUT_OF_RANGE_ERR, 30, exit);
  }

  if (info->dropp_lsb > (SOC_PETRA_ITM_STATISTICS_TAG_NOF_BITS - 1))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ITM_VSQ_LBL_DP_LSB_OUT_OF_RANGE_ERR, 40, exit);
  }

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->enable_mode, SOC_PETRA_ITM_NOF_STAG_ENABLE_MODES,
    SOC_PETRA_STAG_ENABLE_MODE_OUT_OF_RANGE_ERR, 50, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_stag_verify()",0,0);
}

/*********************************************************************
*     Defines the way the Statistics Tag is used.
*     The statistics tag can be used in the Statistics Interface,
*     for mapping to VSQs and further used for VSQ-based drop/FC decisions,
*     and for setting the packet Drop Precedence
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_stag_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_ITM_STAG_INFO       *info
  )
{
  uint32
    res,
    stat_tag_vsq_enable,
    stat_tag_deq_enable,
    stat_tag_dropp_enable,
    stat_tag_vsq_index_msb,
    stat_tag_vsq_index_lsb,
    stat_tag_dropp_lsb;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_STAG_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  SOC_PA_FLD_GET(regs->iqm.statistics_tag_configuration_reg.stat_tag_vsq_msb, stat_tag_vsq_index_msb, 10, exit);

  info->vsq_index_msb = (uint32)stat_tag_vsq_index_msb;

  SOC_PA_FLD_GET(regs->iqm.statistics_tag_configuration_reg.stat_tag_vsq_lsb, stat_tag_vsq_index_lsb, 12, exit);

  info->vsq_index_lsb = (uint32)stat_tag_vsq_index_lsb;

  SOC_PA_FLD_GET(regs->iqm.statistics_tag_configuration_reg.stat_tag_vsq_en, stat_tag_vsq_enable, 14,exit);
  SOC_PA_FLD_GET(regs->iqm.statistics_tag_configuration_reg.stat_tag_deq_en, stat_tag_deq_enable, 16, exit);

  if (stat_tag_vsq_enable != 0x0)
  {
    if (stat_tag_deq_enable != 0x0)
    {
      info->enable_mode = SOC_PETRA_ITM_STAG_ENABLE_MODE_ENABLED_WITH_DEQ;
    }
    else
    {
      info->enable_mode = SOC_PETRA_ITM_STAG_ENABLE_MODE_STAT_IF_NO_DEQ;
    }
  }
  else
  {
    info->enable_mode = SOC_PETRA_ITM_STAG_ENABLE_MODE_DISABLED;
  }

  SOC_PA_FLD_GET(regs->iqm.statistics_tag_configuration_reg.stat_tag_dropp_en, stat_tag_dropp_enable, 18, exit);
  info->dropp_en = SOC_SAND_NUM2BOOL(stat_tag_dropp_enable);

  SOC_PA_FLD_GET(regs->iqm.statistics_tag_configuration_reg.stat_tag_dropp_lsb, stat_tag_dropp_lsb, 20, exit);

  info->dropp_lsb = (uint32)stat_tag_dropp_lsb;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_stag_get_unsafe()",0,0);
}

/*********************************************************************
*     Select VSQ for monitoring. The selected VSQ counter can
*     be further read, indicating the number of packets
*     enqueued to the VSQ.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_itm_vsq_counter_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  SOC_PETRA_ITM_VSQ_NDX         vsq_in_group_ndx
  )
{
  uint32
    fld_val,
    reg_val,
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_VSQ_COUNTER_SET_UNSAFE);

  regs = soc_petra_regs();

  reg_val = 0;
  fld_val = vsq_in_group_ndx;
  SOC_PETRA_FLD_TO_REG(regs->iqm.vsq_programmable_counter_select_reg.vsq_prg_cnt_q, fld_val, reg_val, 20, exit);
  fld_val = 0x0;
  SOC_PETRA_FLD_TO_REG(regs->iqm.vsq_programmable_counter_select_reg.vsq_prg_cnt_msk, fld_val, reg_val, 22, exit);
  fld_val = vsq_group_ndx;
  SOC_PETRA_FLD_TO_REG(regs->iqm.vsq_programmable_counter_select_reg.vsq_prg_grp_sel, fld_val, reg_val, 24, exit);
  SOC_PETRA_REG_SET(regs->iqm.vsq_programmable_counter_select_reg, reg_val, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_vsq_count_get_unsafe()",0,0);
}

/*********************************************************************
*     Select VSQ for monitoring. The selected VSQ counter can
*     be further read, indicating the number of packets
*     enqueued to the VSQ.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_itm_vsq_counter_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ITM_VSQ_GROUP       vsq_group_ndx,
    SOC_SAND_IN  SOC_PETRA_ITM_VSQ_NDX         vsq_in_group_ndx
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_VSQ_COUNTER_VERIFY);

   res = soc_petra_itm_vsq_idx_verify(
          vsq_group_ndx,
          vsq_in_group_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_vsq_counter_verify()",0,0);
}

/*********************************************************************
*     Select VSQ for monitoring. The selected VSQ counter can
*     be further read, indicating the number of packets
*     enqueued to the VSQ.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_itm_vsq_counter_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_ITM_VSQ_GROUP       *vsq_group_ndx,
    SOC_SAND_OUT SOC_PETRA_ITM_VSQ_NDX         *vsq_in_group_ndx
  )
{
  uint32
    fld_val,
    reg_val,
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_VSQ_COUNTER_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(vsq_group_ndx);
  SOC_SAND_CHECK_NULL_INPUT(vsq_in_group_ndx);

  regs = soc_petra_regs();

  SOC_PETRA_REG_GET(regs->iqm.vsq_programmable_counter_select_reg, reg_val, 30, exit);

  SOC_PETRA_FLD_FROM_REG(regs->iqm.vsq_programmable_counter_select_reg.vsq_prg_cnt_q, fld_val, reg_val, 20, exit);
  *vsq_in_group_ndx = fld_val;

  SOC_PETRA_FLD_FROM_REG(regs->iqm.vsq_programmable_counter_select_reg.vsq_prg_grp_sel, fld_val, reg_val, 24, exit);
  *vsq_group_ndx = fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_vsq_counter_get_unsafe()",0,0);
}

/*********************************************************************
*     Indicates the number of packets enqueued to the
*     monitored VSQ.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_itm_vsq_counter_read_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint32                  *pckt_count
  )
{
  uint32
    fld_val,
    reg_val,
    cntr_val,
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_VSQ_COUNTER_READ_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(pckt_count);

  regs = soc_petra_regs();

  SOC_PETRA_REG_GET(regs->iqm.vsq_enqueue_packet_counter_reg, reg_val, 10, exit);
  SOC_PETRA_FLD_FROM_REG(regs->iqm.vsq_enqueue_packet_counter_reg.vsq_enq_pkt_cnt_ovf, fld_val, reg_val, 12, exit);

  if (fld_val != 0x0)
  {
    cntr_val = 0xFFFFFFFF;
  }
  else
  {
    SOC_PETRA_FLD_FROM_REG(regs->iqm.vsq_enqueue_packet_counter_reg.vsq_enq_pkt_cnt, cntr_val, reg_val, 14, exit);
  }

  *pckt_count = cntr_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_vsq_counter_read_unsafe()",0,0);
}

/*********************************************************************
 *   Get the location and occupation of the FIFO (VOQ/VSQ) which has a
 *   maximum occupancy.
*********************************************************************/
uint32
  soc_petra_itm_q_max_occupancy_get_unsafe(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  SOC_PETRA_ITM_Q_TYPE                     q_type,
    SOC_SAND_OUT SOC_PETRA_ITM_Q_OCCUPANCY               *q_max_occ
  )
{
  uint32
    q_id,
    q_size,
    bd_size = 0,
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  SOC_SAND_CHECK_NULL_INPUT(q_max_occ);

  regs = soc_petra_regs();

  /* VSQ case*/
  if (q_type == SOC_PETRA_ITM_Q_TYPE_VSQ)
  {
    SOC_PETRA_FLD_GET(regs->iqm.vsq_maximum_occupancy1_reg.vsq_mx_oc_qnum, q_id, 10, exit);
    SOC_PETRA_FLD_GET(regs->iqm.vsq_maximum_occupancy0_reg.vsq_mx_oc_qsz, q_size, 12, exit);
    SOC_PETRA_FLD_GET(regs->iqm.vsq_maximum_occupancy1_reg.vsq_mx_oc_bdsz, bd_size, 14, exit);
  } 
  else /* VOQ case */
  {
    SOC_PETRA_FLD_GET(regs->iqm.maximum_occupancy_queue_size1_reg.mx_oc_qnum, q_id, 20, exit);
    SOC_PETRA_FLD_GET(regs->iqm.maximum_occupancy_queue_size0_reg.mx_oc_qsz, q_size, 22, exit);
  }

  q_max_occ->id = q_id;
  q_max_occ->max_size = q_size * SOC_PETRA_ITM_Q_OCC_GRANULARITY;
  q_max_occ->max_size_bd = bd_size;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_q_max_occupancy_get_unsafe()",0,0);
}

/*********************************************************************
*     Sets the queue types of a queue
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_queue_info_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_ndx,
    SOC_SAND_IN  SOC_PETRA_ITM_QUEUE_INFO      *info
  )
{
  uint32
    res;
  SOC_PETRA_IQM_STATIC_TBL_DATA
    iqm_static_tbl_data;
  SOC_PETRA_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_DATA
    ips_queue_type_lookup_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_QUEUE_INFO_SET_UNSAFE);

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

  ips_queue_type_lookup_table_tbl_data.queue_type_lookup_table =
    info->cr_req_type_ndx;

  res = soc_petra_iqm_static_tbl_set_unsafe(
          unit,
          queue_ndx,
          &iqm_static_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_ips_queue_type_lookup_table_tbl_set_unsafe(
          unit,
          queue_ndx,
          &ips_queue_type_lookup_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_queue_info_set_unsafe()",0,0);
}

/*********************************************************************
*     Sets the queue types of a queue
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_queue_info_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_ndx,
    SOC_SAND_IN  SOC_PETRA_ITM_QUEUE_INFO      *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_QUEUE_INFO_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    queue_ndx, SOC_PETRA_MAX_QUEUE_ID,
    SOC_PETRA_QUEUE_NUM_OUT_OF_RANGE_ERR, 5, exit
  );

  if (info->vsq_traffic_cls > (SOC_PETRA_TR_CLS_MAX))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ITM_VSQ_TRAFFIC_CLS_OUT_OF_RANGE_ERR, 10, exit);
  }
  if (info->credit_cls > (SOC_PETRA_ITM_QT_CR_CLS_MAX))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ITM_CREDIT_CLS_OUT_OF_RANGE_ERR, 20, exit);
  }
  if (info->vsq_connection_cls > (SOC_PETRA_ITM_QT_CC_CLS_MAX))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ITM_VSQ_CONNECTION_CLS_OUT_OF_RANGE_ERR, 30, exit);
  }
  if (info->rate_cls > (SOC_PETRA_ITM_QT_RT_CLS_MAX))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ITM_RATE_CLS_OUT_OF_RANGE_ERR, 40, exit);
  }
  if (info->cr_req_type_ndx > (SOC_PETRA_ITM_IPS_QT_MAX))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ITM_CR_REQ_TYPE_NDX_OUT_OF_RANGE_ERR, 50, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->signature, SOC_PETRA_MAX_SIGNATURE,
    SOC_PETRA_SIGNATURE_OUT_OF_RANGE_ERR, 30, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_queue_info_verify()",0,0);
}

/*********************************************************************
*     Sets the queue types of a queue
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_queue_info_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_ndx,
    SOC_SAND_OUT SOC_PETRA_ITM_QUEUE_INFO      *info
  )
{
  uint32
    res;
  SOC_PETRA_IQM_STATIC_TBL_DATA
    iqm_static_tbl_data;
  SOC_PETRA_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_DATA
    ips_queue_type_lookup_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_QUEUE_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    queue_ndx, SOC_PETRA_MAX_QUEUE_ID,
    SOC_PETRA_QUEUE_NUM_OUT_OF_RANGE_ERR, 5, exit
  );

  res = soc_petra_iqm_static_tbl_get_unsafe(
          unit,
          queue_ndx,
          &iqm_static_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_petra_ips_queue_type_lookup_table_tbl_get_unsafe(
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

  info->cr_req_type_ndx =
    ips_queue_type_lookup_table_tbl_data.queue_type_lookup_table;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_queue_info_get_unsafe()",0,0);
}

/*********************************************************************
*     Sets ingress shaping configuration. This includes
*     ingress shaping queues range, and credit generation
*     configuration.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_ingress_shape_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ITM_INGRESS_SHAPE_INFO *info
  )
{
  uint32
    fld_val,
    q_hi,
    q_low,
    q_base,
    ihp_reg_low,
    ihp_reg_hi,
    rate_internal,
    res;
  SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO
    explicit_info;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_INGRESS_SHAPE_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  res = soc_petra_ipq_explicit_mapping_mode_info_get_unsafe(
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
     *         - For 'Add' mode, Base queue is smaller then SOC_PETRA_MAX_QUEUE_ID - 1
     *         - For 'Decrement' mode, Base queue is bigger than 2.
     */
    if (
        !(explicit_info.queue_id_add_not_decrement)
        && (q_base < 2)
       )
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_BASE_QUEUE_ID_OUT_OF_RANGE_ERR, 12, exit);
    }
    q_low = (explicit_info.queue_id_add_not_decrement?q_base + 2:q_base-1);
    q_hi = q_low - 1;
  }


  /*
   *  ECI
   */
  SOC_PETRA_FLD_SET(regs->eci.ingress_shaping_queue_boundaries_reg.isp_qnum_low, q_low, 20,exit);
  SOC_PETRA_FLD_SET(regs->eci.ingress_shaping_queue_boundaries_reg.isp_qnum_high, q_hi, 30,exit);

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
#ifdef LINK_PB_LIBRARIES
  if (SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE)
  {
    res = soc_pb_parser_ingress_shape_state_set(
            unit,
            info->enable,
            ihp_reg_low,
            ihp_reg_hi
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }
  else
  {
    --ihp_reg_low;
    ++ihp_reg_hi;
    SOC_PA_REG_SET(regs->ihp.ptc_custom_protocols_reg[0], ihp_reg_low, 42, exit);
    SOC_PA_REG_SET(regs->ihp.ptc_custom_protocols_reg[1], ihp_reg_hi, 44, exit);
  }
#else
  --ihp_reg_low;
  ++ihp_reg_hi;
  SOC_PA_REG_SET(regs->ihp.ptc_custom_protocols_reg[0], ihp_reg_low, 42, exit);
  SOC_PA_REG_SET(regs->ihp.ptc_custom_protocols_reg[1], ihp_reg_hi, 44, exit);
#endif

  fld_val = SOC_SAND_BOOL2NUM(info->enable);
  SOC_PETRA_FLD_SET(regs->sch.ingress_shaping_port_configuration_reg.ingress_shaping_enable, fld_val, 50, exit);

  if (info->enable)
  {
    SOC_PETRA_FLD_SET(regs->sch.ingress_shaping_port_configuration_reg.ingress_shaping_port_id, info->sch_port, 60, exit);

    res = soc_petra_intern_rate2clock(
            unit,
            info->rate,
            TRUE, /* is_for_ips */
            &rate_internal
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    SOC_PETRA_FLD_SET(regs->ips.ingress_shape_scheduler_config_reg.iss_max_cr_rate, rate_internal, 80, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_ingress_shape_set_unsafe()",0,0);
}

/*********************************************************************
*     Sets ingress shaping configuration. This includes
*     ingress shaping queues range, and credit generation
*     configuration.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_ingress_shape_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ITM_INGRESS_SHAPE_INFO *info
  )
{
  uint32
    res;
  SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO
    explicit_info;
  uint8
    is_base_q_set;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_INGRESS_SHAPE_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  if (info->enable)
  {

    res = soc_petra_ipq_base_q_is_valid_get_unsafe(
            unit,
            &is_base_q_set
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

    if (!is_base_q_set)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_BASE_Q_NOT_SET_ERR, 7, exit);
    }

    res = soc_petra_fap_port_id_verify(
            unit,
            info->sch_port,
            TRUE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->rate, SOC_PETRA_IF_MAX_RATE_KBPS,
      SOC_PETRA_ITM_ING_SHAPE_SCH_RATE_OUT_OF_RANGE_ERR, 20, exit
    );

    soc_petra_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO_clear(&explicit_info);

    res = soc_petra_ipq_explicit_mapping_mode_info_get_unsafe(
          unit,
          &explicit_info
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    SOC_SAND_ERR_IF_ABOVE_MAX(
      explicit_info.base_queue_id, SOC_PETRA_MAX_QUEUE_ID,
      SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_BASE_QUEUE_ID_OUT_OF_RANGE_ERR, 40, exit
    );

    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->q_range.q_num_low, info->q_range.q_num_high,
      SOC_PETRA_ITM_INGRESS_SHAPING_LOW_BELOW_HIGH_ERR, 50, exit
    );

    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->q_range.q_num_high, SOC_PETRA_MAX_QUEUE_ID,
      SOC_PETRA_QUEUE_NUM_OUT_OF_RANGE_ERR, 50, exit
    );

    if (explicit_info.queue_id_add_not_decrement)
    {
      SOC_SAND_ERR_IF_ABOVE_MAX(
        explicit_info.base_queue_id - 1, info->q_range.q_num_low,
        SOC_PETRA_IHP_IS_RANGE_AND_BASE_Q_MISMATCH_ERR, 52, exit
      );

      SOC_SAND_ERR_IF_ABOVE_MAX(
        explicit_info.base_queue_id + 1, info->q_range.q_num_high,
        SOC_PETRA_IHP_IS_RANGE_AND_BASE_Q_MISMATCH_ERR, 54, exit
      );
    }
    else
    {
      SOC_SAND_ERR_IF_ABOVE_MAX(
        info->q_range.q_num_high - 1, explicit_info.base_queue_id,
        SOC_PETRA_IHP_IS_RANGE_AND_BASE_Q_MISMATCH_ERR, 56, exit
      );

      SOC_SAND_ERR_IF_ABOVE_MAX(
        info->q_range.q_num_low  + 1, explicit_info.base_queue_id,
        SOC_PETRA_IHP_IS_RANGE_AND_BASE_Q_MISMATCH_ERR, 58, exit
      );
    }
  }

  if (SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE)
  {
    if (info->q_range.q_num_high >= SOC_PETRA_MAX_QUEUE_ID)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_INGR_SHP_Q_ABOVE_MAX_ERR, 60, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_ingress_shape_verify()",0,0);
}

/*********************************************************************
*     Sets ingress shaping configuration. This includes
*     ingress shaping queues range, and credit generation
*     configuration.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_ingress_shape_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_ITM_INGRESS_SHAPE_INFO *info
  )
{
  uint32
    rate_internal,
    fld_val,
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_INGRESS_SHAPE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  SOC_PETRA_FLD_GET(regs->eci.ingress_shaping_queue_boundaries_reg.isp_qnum_low, info->q_range.q_num_low, 12, exit);
  SOC_PETRA_FLD_GET(regs->eci.ingress_shaping_queue_boundaries_reg.isp_qnum_high, info->q_range.q_num_high, 14, exit);

  SOC_PETRA_FLD_GET(regs->sch.ingress_shaping_port_configuration_reg.ingress_shaping_port_id, info->sch_port, 16, exit);

  SOC_PETRA_FLD_GET(regs->sch.ingress_shaping_port_configuration_reg.ingress_shaping_enable, fld_val, 18, exit);
  info->enable = SOC_SAND_NUM2BOOL(fld_val);

  SOC_PETRA_FLD_GET(regs->ips.ingress_shape_scheduler_config_reg.iss_max_cr_rate, rate_internal, 20, exit);

  res = soc_petra_intern_clock2rate(
          unit,
          rate_internal,
          TRUE, /* is_for_ips */
          &(info->rate)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_ingress_shape_get_unsafe()",0,0);
}

/*********************************************************************
*     Four sets of queues-priorities maps are held in the
*     device. Per map: describes a segment of 64 contiguous
*     queues. Each queue is either high or low priority.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_priority_map_tmplt_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 map_ndx,
    SOC_SAND_IN  SOC_PETRA_ITM_PRIORITY_MAP_TMPLT *info
  )
{
  uint32
    res;
  SOC_PETRA_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_DATA
    ips_queue_priority_maps_table_tbl_data;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_PRIORITY_MAP_TMPLT_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = SOC_SAND_OK; sal_memcpy(
          ips_queue_priority_maps_table_tbl_data.queue_priority_maps_table,
          info->map,
          SOC_PETRA_ITM_PRIO_MAP_SIZE_IN_UINT32S * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_ips_queue_priority_maps_table_tbl_set_unsafe(
          unit,
          map_ndx,
          &ips_queue_priority_maps_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_priority_map_tmplt_set_unsafe()",0,0);
}

/*********************************************************************
*     Four sets of queues-priorities maps are held in the
*     device. Per map: describes a segment of 64 contiguous
*     queues. Each queue is either high or low priority.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_priority_map_tmplt_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 map_ndx,
    SOC_SAND_IN  SOC_PETRA_ITM_PRIORITY_MAP_TMPLT *info
  )
{
  uint32
    res;
  uint32
    index;
  uint8
    is_mesh,
    is_single_cntxt;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_PRIORITY_MAP_TMPLT_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  res = soc_petra_fabric_is_mesh(
          unit,
          &is_mesh,
          &is_single_cntxt
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 3, exit);

  if (SOC_PETRA_IS_DEV_PETRA_A && ((!is_mesh) || (is_single_cntxt)))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_NON_MESH_CONF_ERR, 5, exit);
  }

  SOC_SAND_ERR_IF_ABOVE_MAX(
    map_ndx, SOC_PETRA_ITM_PRIORITY_MAP_NDX_MAX,
    SOC_PETRA_ITM_PRIORITY_MAP_NDX_OUT_OF_RANGE_ERR, 10, exit
  );

  for (index = 0 ; index < SOC_PETRA_ITM_PRIO_MAP_SIZE_IN_UINT32S; ++index)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->map[index], SOC_PETRA_ITM_PRIORITY_MAP_TMPLT_MAP_MAX,
      SOC_PETRA_ITM_PRIORITY_MAP_NDX_OUT_OF_RANGE_ERR, 20, exit
    );
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_priority_map_tmplt_verify()",0,0);
}

/*********************************************************************
*     Four sets of queues-priorities maps are held in the
*     device. Per map: describes a segment of 64 contiguous
*     queues. Each queue is either high or low priority.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_priority_map_tmplt_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 map_ndx,
    SOC_SAND_OUT SOC_PETRA_ITM_PRIORITY_MAP_TMPLT *info
  )
{
  uint32
    res;
  SOC_PETRA_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_DATA
    ips_queue_priority_maps_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_PRIORITY_MAP_TMPLT_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    map_ndx, SOC_PETRA_ITM_PRIORITY_MAP_NDX_MAX,
    SOC_PETRA_ITM_PRIORITY_MAP_NDX_OUT_OF_RANGE_ERR, 5, exit
  );

  res = soc_petra_ips_queue_priority_maps_table_tbl_get_unsafe(
          unit,
          map_ndx,
          &ips_queue_priority_maps_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = SOC_SAND_OK; sal_memcpy(
          info->map,
          ips_queue_priority_maps_table_tbl_data.queue_priority_maps_table,
          SOC_PETRA_ITM_PRIO_MAP_SIZE_IN_UINT32S * sizeof(uint32)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_priority_map_tmplt_get_unsafe()",0,0);
}

/*********************************************************************
*     The 32K ingress-queues range is segmented into 512
*     segments of 64 contiguous queues, that is, queues 64N to
*     64N+63 that all have the same map-id (one of four).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_priority_map_tmplt_select_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_64_ndx,
    SOC_SAND_IN  uint32                 priority_map
  )
{
  uint32
    res;
  SOC_PETRA_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_DATA
    ips_queue_priority_map_select_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_PRIORITY_MAP_TMPLT_SELECT_SET_UNSAFE);

  res = soc_petra_ips_queue_priority_map_select_tbl_get_unsafe(
          unit,
          queue_64_ndx,
          &ips_queue_priority_map_select_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  ips_queue_priority_map_select_tbl_data.queue_priority_map_select =
    priority_map;

  res = soc_petra_ips_queue_priority_map_select_tbl_set_unsafe(
          unit,
          queue_64_ndx,
          &ips_queue_priority_map_select_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_priority_map_tmplt_select_set_unsafe()",0,0);
}

/*********************************************************************
*     The 32K ingress-queues range is segmented into 512
*     segments of 64 contiguous queues, that is, queues 64N to
*     64N+63 that all have the same map-id (one of four).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_priority_map_tmplt_select_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_64_ndx,
    SOC_SAND_IN  uint32                 priority_map
  )
{
  uint32
    res;
  uint8
    is_mesh,
    is_single_cntxt;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_PRIORITY_MAP_TMPLT_SELECT_VERIFY);

  res = soc_petra_fabric_is_mesh(
          unit,
          &is_mesh,
          &is_single_cntxt
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 3, exit);

  if (SOC_PETRA_IS_DEV_PETRA_A && ((!is_mesh) || (is_single_cntxt)))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_NON_MESH_CONF_ERR, 5, exit);
  }

  SOC_SAND_ERR_IF_ABOVE_MAX(
    queue_64_ndx, (SOC_PETRA_ITM_PRIO_NOF_SEGMENTS - 1),
    SOC_PETRA_ITM_PRIORITY_MAP_SEGMENT_NDX_OUT_OF_RANGE_ERR, 10, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    priority_map, SOC_PETRA_ITM_PRIORITY_MAP_NDX_MAX,
    SOC_PETRA_ITM_PRIORITY_MAP_NDX_OUT_OF_RANGE_ERR, 20, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_priority_map_tmplt_select_verify()",0,0);
}

/*********************************************************************
*     The 32K ingress-queues range is segmented into 512
*     segments of 64 contiguous queues, that is, queues 64N to
*     64N+63 that all have the same map-id (one of four).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_priority_map_tmplt_select_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  queue_64_ndx,
    SOC_SAND_OUT uint32                 *priority_map
  )
{
  uint32
    res;
  SOC_PETRA_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_DATA
    ips_queue_priority_map_select_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_PRIORITY_MAP_TMPLT_SELECT_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(priority_map);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    queue_64_ndx, (SOC_PETRA_ITM_PRIO_NOF_SEGMENTS - 1),
    SOC_PETRA_ITM_PRIORITY_MAP_SEGMENT_NDX_OUT_OF_RANGE_ERR, 10, exit
  );

  ips_queue_priority_map_select_tbl_data.queue_priority_map_select = 0;
  res = soc_petra_ips_queue_priority_map_select_tbl_get_unsafe(
          unit,
          queue_64_ndx,
          &ips_queue_priority_map_select_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  *priority_map =
    (uint32)(ips_queue_priority_map_select_tbl_data.queue_priority_map_select);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_priority_map_tmplt_select_get_unsafe()",0,0);
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
  soc_petra_itm_sys_red_drop_prob_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ITM_SYS_RED_DROP_PROB *info
  )
{
  uint32
    res,
    indx;
  SOC_PETRA_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_DATA
    iqm_system_red_drop_probability_values_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_SYS_RED_DROP_PROB_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  for (indx = 0; indx<SOC_PETRA_ITM_SYS_RED_DRP_PROBS; ++indx)
  {
    iqm_system_red_drop_probability_values_tbl_data.drp_prob =
      info->drop_probs[indx];

    res= soc_petra_iqm_system_red_drop_probability_values_tbl_set_unsafe(
           unit,
           indx,
           &iqm_system_red_drop_probability_values_tbl_data
         );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20+indx, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_sys_red_drop_prob_set_unsafe()",0,0);
}

/*********************************************************************
*     System Red drop probabilities table fill in. The system
*     Red mechanism uses a table of 16 probabilities. The
*     table is used by indexes which choose 1 out of the 16
*     options.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_sys_red_drop_prob_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ITM_SYS_RED_DROP_PROB *info
  )
{
  uint32
    index;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_SYS_RED_DROP_PROB_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  for (index = 0 ; index < SOC_PETRA_ITM_SYS_RED_DRP_PROBS; ++index)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->drop_probs[index], SOC_PETRA_ITM_SYS_RED_DROP_PROB_VAL_MAX,
      SOC_PETRA_ITM_PRIORITY_MAP_NDX_OUT_OF_RANGE_ERR, 10, exit
    );
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_sys_red_drop_prob_verify()",0,0);
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
  soc_petra_itm_sys_red_drop_prob_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_ITM_SYS_RED_DROP_PROB *info
  )
{
  uint32
    res,
    indx;
  SOC_PETRA_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_DATA
    iqm_system_red_drop_probability_values_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_SYS_RED_DROP_PROB_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  for (indx = 0; indx<SOC_PETRA_ITM_SYS_RED_DRP_PROBS; ++indx)
  {
    res= soc_petra_iqm_system_red_drop_probability_values_tbl_get_unsafe(
           unit,
           indx,
           &iqm_system_red_drop_probability_values_tbl_data
         );
    info->drop_probs[indx] =
      iqm_system_red_drop_probability_values_tbl_data.drp_prob;

    SOC_SAND_CHECK_FUNC_RESULT(res, 20+indx, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_sys_red_drop_prob_get_unsafe()",0,0);
}

/*********************************************************************
*     System Red queue size boundaries, per queue type - rate
*     class. The queue size ranges table is set. For each
*     queue type and drop-precedence,
*     drop/pass/drop-with-probability parameters are set using
*     the function soc_petra_itm_sys_red_qt_dp_info_set.
*     Note that the System-Red mechanism is a system-wide
*     attribute and it should be configured homogeneously
*     in all FAPs.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_sys_red_queue_size_boundaries_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  SOC_PETRA_ITM_SYS_RED_QT_INFO *info,
    SOC_SAND_OUT SOC_PETRA_ITM_SYS_RED_QT_INFO *exact_info
  )
{
  uint32
    res,
    boundary_i;
  SOC_PETRA_IQM_SYSTEM_RED_TBL_DATA
    iqm_system_red_tbl_data;
  int32
    exact_value_var;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_SYS_RED_QUEUE_SIZE_BOUNDARIES_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);

  for (boundary_i = 0; boundary_i < (SOC_PETRA_ITM_SYS_RED_Q_SIZE_RANGES-1); boundary_i++)
  {

    res = soc_petra_itm_man_exp_buffer_set(
            info->queue_size_boundaries[boundary_i],
            SOC_PETRA_ITM_SYS_RED_Q_BOUNDARY_TH_MNT_LSB,
            SOC_PETRA_ITM_VSQ_SYS_RED_Q_BOUNDARY_TH_MNT_NOF_BITS,
            SOC_PETRA_ITM_SYS_RED_Q_BOUNDARY_TH_EXP_LSB,
            SOC_PETRA_ITM_VSQ_SYS_RED_Q_BOUNDARY_TH_EXP_NOF_BITS,
            FALSE,
            &iqm_system_red_tbl_data.qsz_rng_th[boundary_i],
            &exact_value_var
         );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    exact_info->queue_size_boundaries[boundary_i] = (uint32)exact_value_var;
  }

  res = soc_petra_iqm_system_red_tbl_set_unsafe(
    unit,
    rt_cls_ndx,
    &iqm_system_red_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_sys_red_queue_size_boundaries_set_unsafe()",0,0);
}

/*********************************************************************
*     System Red queue size boundaries, per queue type - rate
*     class. The queue size ranges table is set. For each
*     queue type and drop-precedence,
*     drop/pass/drop-with-probability parameters are set using
*     the function soc_petra_itm_sys_red_qt_dp_info_set.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_sys_red_queue_size_boundaries_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  SOC_PETRA_ITM_SYS_RED_QT_INFO *info
  )
{
  uint32
    index;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_SYS_RED_QUEUE_SIZE_BOUNDARIES_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    rt_cls_ndx, SOC_PETRA_ITM_QT_RT_CLS_MAX,
    SOC_PETRA_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 10, exit
  );

  for (index = 0 ; index < (SOC_PETRA_ITM_SYS_RED_Q_SIZE_RANGES-1); ++index)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->queue_size_boundaries[index], SOC_PETRA_MAX_QUEUE_SIZE_BYTES,
      SOC_PETRA_ITM_PRIORITY_MAP_NDX_OUT_OF_RANGE_ERR, 20, exit
    );
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_sys_red_queue_size_boundaries_verify()",0,0);
}

/*********************************************************************
*     System Red queue size boundaries, per queue type - rate
*     class. The queue size ranges table is set. For each
*     queue type and drop-precedence,
*     drop/pass/drop-with-probability parameters are set using
*     the function soc_petra_itm_sys_red_qt_dp_info_set.
*     Note that the System-Red mechanism is a system-wide
*     attribute and it should be configured homogeneously
*     in all FAPs.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_itm_sys_red_queue_size_boundaries_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_OUT SOC_PETRA_ITM_SYS_RED_QT_INFO *info
  )
{
  uint32
    res,
    boundary_i;
  SOC_PETRA_IQM_SYSTEM_RED_TBL_DATA
    iqm_system_red_tbl_data;
  int32
    exact_value_var;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_SYS_RED_QUEUE_SIZE_BOUNDARIES_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    rt_cls_ndx, SOC_PETRA_ITM_QT_RT_CLS_MAX,
    SOC_PETRA_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 10, exit
  );

  res = soc_petra_iqm_system_red_tbl_get_unsafe(
    unit,
    rt_cls_ndx,
    &iqm_system_red_tbl_data
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  for (boundary_i = 0; boundary_i < (SOC_PETRA_ITM_SYS_RED_Q_SIZE_RANGES-1); boundary_i++)
  {
    res = soc_petra_itm_man_exp_buffer_get(
            iqm_system_red_tbl_data.qsz_rng_th[boundary_i],
            SOC_PETRA_ITM_SYS_RED_Q_BOUNDARY_TH_MNT_LSB,
            SOC_PETRA_ITM_VSQ_SYS_RED_Q_BOUNDARY_TH_MNT_NOF_BITS,
            SOC_PETRA_ITM_SYS_RED_Q_BOUNDARY_TH_EXP_LSB,
            SOC_PETRA_ITM_VSQ_SYS_RED_Q_BOUNDARY_TH_EXP_NOF_BITS,
            FALSE,
            &exact_value_var
         );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    info->queue_size_boundaries[boundary_i] = (uint32)exact_value_var;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_sys_red_queue_size_boundaries_get_unsafe()",0,0);
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
  soc_petra_itm_sys_red_q_based_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 sys_red_dp_ndx,
    SOC_SAND_IN  SOC_PETRA_ITM_SYS_RED_QT_DP_INFO *info
  )
{
  uint32
    res;
  SOC_PETRA_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_DATA
    iqm_system_red_parameters_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_SYS_RED_Q_BASED_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_iqm_system_red_parameters_table_tbl_get_unsafe(
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

  res = soc_petra_iqm_system_red_parameters_table_tbl_set_unsafe(
          unit,
          rt_cls_ndx,
          sys_red_dp_ndx,
          &iqm_system_red_parameters_table_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_sys_red_q_based_set_unsafe()",0,0);
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
  soc_petra_itm_sys_red_q_based_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 sys_red_dp_ndx,
    SOC_SAND_IN  SOC_PETRA_ITM_SYS_RED_QT_DP_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_SYS_RED_Q_BASED_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    rt_cls_ndx, SOC_PETRA_ITM_QT_RT_CLS_MAX,
    SOC_PETRA_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    sys_red_dp_ndx, SOC_PETRA_MAX_DROP_PRECEDENCE,
    SOC_PETRA_DRPP_OUT_OF_RANGE_ERR, 7, exit
  );

  if (info->adm_th > SOC_PETRA_ITM_SYS_RED_QUEUE_TH_MAX)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ITM_SYS_RED_QUEUE_TH_OUT_OF_RANGE_ERR, 10, exit);
  }
  if(info->prob_th > SOC_PETRA_ITM_SYS_RED_QUEUE_TH_MAX)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ITM_SYS_RED_QUEUE_TH_OUT_OF_RANGE_ERR, 20, exit);
  }
  if (info->drp_th > SOC_PETRA_ITM_SYS_RED_QUEUE_TH_MAX)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ITM_SYS_RED_QUEUE_TH_OUT_OF_RANGE_ERR, 30, exit);
  }
  if (info->drp_prob_low > (SOC_PETRA_ITM_SYS_RED_DRP_PROBS-1))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ITM_SYS_RED_DRP_PROB_INDEX_OUT_OF_RANGE_ERR, 40, exit);
  }
  if (info->drp_prob_high > (SOC_PETRA_ITM_SYS_RED_DRP_PROBS-1))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ITM_SYS_RED_DRP_PROB_INDEX_OUT_OF_RANGE_ERR, 50, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_sys_red_q_based_verify()",0,0);
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
  soc_petra_itm_sys_red_q_based_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 rt_cls_ndx,
    SOC_SAND_IN  uint32                 sys_red_dp_ndx,
    SOC_SAND_OUT SOC_PETRA_ITM_SYS_RED_QT_DP_INFO *info
  )
{
  uint32
    res;
  SOC_PETRA_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_DATA
    iqm_system_red_parameters_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_SYS_RED_Q_BASED_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    rt_cls_ndx, SOC_PETRA_ITM_QT_RT_CLS_MAX,
    SOC_PETRA_ITM_QT_RT_CLS_RNG_OUT_OF_RANGE_ERR, 5, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    sys_red_dp_ndx, SOC_PETRA_MAX_DROP_PRECEDENCE,
    SOC_PETRA_DRPP_OUT_OF_RANGE_ERR, 7, exit
  );

  res = soc_petra_iqm_system_red_parameters_table_tbl_get_unsafe(
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
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_sys_red_q_based_get_unsafe()",0,0);
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
  soc_petra_itm_sys_red_eg_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ITM_SYS_RED_EG_INFO *info,
    SOC_SAND_OUT SOC_PETRA_ITM_SYS_RED_EG_INFO *exact_info
  )
{
  uint32
    res,
    aging_timer,
    aging_timer_ns,
    device_mega_ticks_per_sec,
    convert_coefficient_val;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_SYS_RED_EG_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_info);

  regs = soc_petra_regs();
  device_mega_ticks_per_sec = soc_petra_chip_mega_ticks_per_sec_get(unit);

  SOC_PETRA_FLD_SET(regs->sch.system_red_configuration_reg.enable_sys_red, info->enable, 0,exit);

  /* Overflow protection - will convert aging timer from ms to ns */
  if(info->aging_timer > SOC_SAND_DIV_ROUND_DOWN(SAL_UINT32_MAX,1000000))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ITM_SYS_RED_EG_INFO_AGING_TIMER_OUT_OF_RANGE_ERR, 10, exit);
  }

  /* AgingTimerRegisterValue = VAL(ns) / (1296 * CLK(mhz) / 250) */
  /* The convert_coefficient_val should be 1296 for 250Mhz CLK  */
  convert_coefficient_val = SOC_PETRA_ITM_SYS_RED_EG_INFO_AGING_TIMER_CONVERT_COEFFICIENT * device_mega_ticks_per_sec; 
  convert_coefficient_val = SOC_SAND_DIV_ROUND_UP(convert_coefficient_val,1000);  

  aging_timer_ns = info->aging_timer * 1000000;
  aging_timer = SOC_SAND_DIV_ROUND_UP(aging_timer_ns,convert_coefficient_val);

  /* Verify the value is less than MAX value */
  if (aging_timer > SOC_PETRA_ITM_SYS_RED_EG_INFO_AGING_TIMER_MAX)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_ITM_SYS_RED_EG_INFO_AGING_TIMER_OUT_OF_RANGE_ERR, 20, exit);
  }

  SOC_PETRA_FLD_SET(regs->sch.system_red_configuration_reg.aging_timer_cfg, aging_timer, 1,exit);

  SOC_PETRA_FLD_SET(regs->sch.system_red_configuration_reg.reset_xpired_qsz, info->reset_expired_q_size, 2,exit);

  SOC_PETRA_FLD_SET(regs->sch.system_red_configuration_reg.aging_only_dec_pqs, info->aging_only_dec_q_size, 3,exit);

  exact_info->enable = info->enable;
  exact_info->reset_expired_q_size = info->reset_expired_q_size;
  exact_info->aging_only_dec_q_size = info->aging_only_dec_q_size;

  aging_timer = aging_timer * convert_coefficient_val;
  aging_timer = SOC_SAND_DIV_ROUND(aging_timer,1000000);
  exact_info->aging_timer = aging_timer;


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_sys_red_eg_set_unsafe()",0,0);
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
  soc_petra_itm_sys_red_eg_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ITM_SYS_RED_EG_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_SYS_RED_EG_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);
  
  /* Checking the timer value against the MAX value is done in the soc_petra_itm_sys_red_eg_set() function */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_sys_red_eg_verify()",0,0);
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
  soc_petra_itm_sys_red_eg_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_ITM_SYS_RED_EG_INFO *info
  )
{
  uint32
    res,
    sys_red_enable,
    reset_expired_q_size_en,
    aging_only_dec_q_size_en;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_SYS_RED_EG_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  SOC_PETRA_FLD_GET(regs->sch.system_red_configuration_reg.enable_sys_red, sys_red_enable, 10, exit);
  info->enable = (uint8)sys_red_enable;

  SOC_PETRA_FLD_GET(regs->sch.system_red_configuration_reg.aging_timer_cfg, info->aging_timer, 12, exit);

  SOC_PETRA_FLD_GET(regs->sch.system_red_configuration_reg.reset_xpired_qsz, reset_expired_q_size_en, 14, exit);
  info->reset_expired_q_size = (uint8)reset_expired_q_size_en;

  SOC_PETRA_FLD_GET(regs->sch.system_red_configuration_reg.aging_only_dec_pqs, aging_only_dec_q_size_en, 16, exit);
  info->aging_only_dec_q_size = (uint8)aging_only_dec_q_size_en;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_sys_red_eg_get_unsafe()",0,0);
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
  soc_petra_itm_sys_red_glob_rcs_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ITM_SYS_RED_GLOB_RCS_INFO *info
  )
{
  uint32
    res,
    indx;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_SYS_RED_GLOB_RCS_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

   /*
    * Free Unicast Buffers Thresholds
    */
  for (indx = 0 ; indx < (SOC_PETRA_ITM_SYS_RED_BUFFS_RNGS-1) ; ++indx)
  {
    SOC_PETRA_FLD_SET(regs->iqm.free_unicast_dbuff_threshold_reg[indx].fr_un_db_th, info->thresholds.unicast_rng_ths[indx], 0,exit);

    SOC_PETRA_FLD_SET(regs->iqm.free_unicast_dbuff_range_values_reg.fr_un_db_rng_val[indx], info->values.unicast_rng_vals[indx], 1,exit);
  }
   /*
    * Configure Last (Unicast) Value
    */
  SOC_PETRA_FLD_SET(regs->iqm.free_unicast_dbuff_range_values_reg.fr_un_db_rng_val[indx], info->values.unicast_rng_vals[indx], 2,exit);

    /*
    * Free Multicast Buffers Thresholds
    */
  for (indx = 0 ; indx < SOC_PETRA_ITM_SYS_RED_BUFFS_RNGS-1 ; ++indx)
  {
    SOC_PETRA_FLD_SET(regs->iqm.free_full_multicast_dbuff_threshold_reg[indx].fr_fl_mul_db_th, info->thresholds.multicast_rng_ths[indx], 3,exit);

    SOC_PETRA_FLD_SET(regs->iqm.free_full_multicast_dbuff_range_values_reg.fr_fl_ml_db_rng_val[indx], info->values.multicast_rng_vals[indx], 4,exit);
  }

   /*
    * Configure Last (Multicast) Value
    */
  SOC_PETRA_FLD_SET(regs->iqm.free_full_multicast_dbuff_range_values_reg.fr_fl_ml_db_rng_val[indx], info->values.multicast_rng_vals[indx], 5,exit);

   /*
    * Free BD Buffers Thresholds
    */

  for (indx = 0 ; indx < SOC_PETRA_ITM_SYS_RED_BUFFS_RNGS-1 ; ++indx)
  {
    SOC_PETRA_FLD_SET(regs->iqm.free_bdb_threshold_reg[indx].fr_bdb_th, info->thresholds.bds_rng_ths[indx], 6,exit);

    SOC_PETRA_FLD_SET(regs->iqm.free_bdb_range_values_reg.fr_bdb_rng_val[indx], info->values.bds_rang_vals[indx], 7,exit);
  }

   /*
    * Configure Last (BDs) Value
    */
  SOC_PETRA_FLD_SET(regs->iqm.free_bdb_range_values_reg.fr_bdb_rng_val[indx], info->values.bds_rang_vals[indx], 8,exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_sys_red_glob_rcs_set_unsafe()",0,0);
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
  soc_petra_itm_sys_red_glob_rcs_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ITM_SYS_RED_GLOB_RCS_INFO *info
  )
{
  uint32
    index;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_SYS_RED_GLOB_RCS_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  for (index = 0 ; index < (SOC_PETRA_ITM_SYS_RED_BUFFS_RNGS-1); ++index)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->thresholds.unicast_rng_ths[index], SOC_PETRA_ITM_SYS_RED_GLOB_RCS_THS_UNICAST_RNG_THS_MAX,
      SOC_PETRA_ITM_SYS_RED_GLOB_RCS_RNG_THS_OUT_OF_RANGE_ERR, 10 + index, exit
    );
  }
  for (index = 0 ; index < (SOC_PETRA_ITM_SYS_RED_BUFFS_RNGS-1); ++index)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->thresholds.multicast_rng_ths[index], SOC_PETRA_ITM_SYS_RED_GLOB_RCS_THS_MULTICAST_RNG_THS_MAX,
      SOC_PETRA_ITM_SYS_RED_GLOB_RCS_RNG_THS_OUT_OF_RANGE_ERR, 30 + index, exit
    );
  }
  for (index = 0 ; index < (SOC_PETRA_ITM_SYS_RED_BUFFS_RNGS-1); ++index)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->thresholds.bds_rng_ths[index], SOC_PETRA_ITM_SYS_RED_GLOB_RCS_THS_BDS_RNG_THS_MAX,
      SOC_PETRA_ITM_SYS_RED_GLOB_RCS_THS_OUT_OF_RANGE_ERR, 50 + index, exit
      );
  }

  for (index = 0 ; index < SOC_PETRA_ITM_SYS_RED_BUFFS_RNGS; ++index)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->values.unicast_rng_vals[index], SOC_PETRA_ITM_SYS_RED_GLOB_RCS_VALS_RNG_VALS_MAX,
      SOC_PETRA_ITM_SYS_RED_GLOB_RCS_RNG_VALS_OUT_OF_RANGE_ERR, 70 + index, exit
    );
  }
  for (index = 0 ; index < SOC_PETRA_ITM_SYS_RED_BUFFS_RNGS; ++index)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->values.multicast_rng_vals[index], SOC_PETRA_ITM_SYS_RED_GLOB_RCS_VALS_RNG_VALS_MAX,
      SOC_PETRA_ITM_SYS_RED_GLOB_RCS_RNG_VALS_OUT_OF_RANGE_ERR, 90 + index, exit
    );
  }
  for (index = 0 ; index < SOC_PETRA_ITM_SYS_RED_BUFFS_RNGS; ++index)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->values.bds_rang_vals[index], SOC_PETRA_ITM_SYS_RED_GLOB_RCS_VALS_RNG_VALS_MAX,
      SOC_PETRA_ITM_SYS_RED_GLOB_RCS_RNG_VALS_OUT_OF_RANGE_ERR, 110 + index, exit
    );
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_sys_red_glob_rcs_verify()",0,0);
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
  soc_petra_itm_sys_red_glob_rcs_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_ITM_SYS_RED_GLOB_RCS_INFO *info
  )
{
  uint32
    res,
    indx;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_SYS_RED_GLOB_RCS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

   /*
    * Free Unicast Buffers Thresholds
    */
  for (indx = 0 ; indx < (SOC_PETRA_ITM_SYS_RED_BUFFS_RNGS-1) ; ++indx)
  {
    SOC_PETRA_FLD_GET(regs->iqm.free_unicast_dbuff_threshold_reg[indx].fr_un_db_th, info->thresholds.unicast_rng_ths[indx], 10, exit);

    SOC_PETRA_FLD_GET(regs->iqm.free_unicast_dbuff_range_values_reg.fr_un_db_rng_val[indx], info->values.unicast_rng_vals[indx], 12, exit);
  }
   /*
    * READ Last (Unicast) Value
    */
  SOC_PETRA_FLD_GET(regs->iqm.free_unicast_dbuff_range_values_reg.fr_un_db_rng_val[indx], info->values.unicast_rng_vals[indx], 14, exit);

    /*
    * Free Multicast Buffers Thresholds
    */
  for (indx = 0 ; indx < SOC_PETRA_ITM_SYS_RED_BUFFS_RNGS-1 ; ++indx)
  {
    SOC_PETRA_FLD_GET(regs->iqm.free_full_multicast_dbuff_threshold_reg[indx].fr_fl_mul_db_th, info->thresholds.multicast_rng_ths[indx], 16, exit);

    SOC_PETRA_FLD_GET(regs->iqm.free_full_multicast_dbuff_range_values_reg.fr_fl_ml_db_rng_val[indx], info->values.multicast_rng_vals[indx], 18, exit);
  }

   /*
    * READ Last (Multicast) Value
    */
  SOC_PETRA_FLD_GET(regs->iqm.free_full_multicast_dbuff_range_values_reg.fr_fl_ml_db_rng_val[indx], info->values.multicast_rng_vals[indx], 20, exit);

   /*
    * Free BD Buffers Thresholds
    */

  for (indx = 0 ; indx < SOC_PETRA_ITM_SYS_RED_BUFFS_RNGS-1 ; ++indx)
  {
    SOC_PETRA_FLD_GET(regs->iqm.free_bdb_threshold_reg[indx].fr_bdb_th, info->thresholds.bds_rng_ths[indx], 22, exit);

    SOC_PETRA_FLD_GET(regs->iqm.free_bdb_range_values_reg.fr_bdb_rng_val[indx], info->values.bds_rang_vals[indx], 24, exit);
  }

   /*
    * READ Last (BDs) Value
    */
  SOC_PETRA_FLD_GET(regs->iqm.free_bdb_range_values_reg.fr_bdb_rng_val[indx], info->values.bds_rang_vals[indx], 26, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_sys_red_glob_rcs_get_unsafe()",0,0);
}

uint32
  soc_petra_itm_dbuff_size2internal(
    SOC_SAND_IN  SOC_PETRA_ITM_DBUFF_SIZE_BYTES dbuff_size,
    SOC_SAND_OUT uint32                   *dbuff_size_internal
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_DBUFF_SIZE2INTERNAL);

  switch(dbuff_size) {
  case SOC_PETRA_ITM_DBUFF_SIZE_BYTES_256:
    *dbuff_size_internal = 0x0;
    break;
  case SOC_PETRA_ITM_DBUFF_SIZE_BYTES_512:
    *dbuff_size_internal = 0x1;
    break;
  case SOC_PETRA_ITM_DBUFF_SIZE_BYTES_1024:
    *dbuff_size_internal = 0x2;
    break;
  case SOC_PETRA_ITM_DBUFF_SIZE_BYTES_2048:
    *dbuff_size_internal = 0x3;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DBUFF_SIZE_INVALID_ERR, 10, exit)
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_dbuff_size2internal()",0,0);
}

uint32
  soc_petra_itm_dbuff_internal2size(
    SOC_SAND_IN  uint32                   dbuff_size_internal,
    SOC_SAND_OUT SOC_PETRA_ITM_DBUFF_SIZE_BYTES *dbuff_size
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_ITM_DBUFF_INTERNAL2SIZE);

  switch(dbuff_size_internal) {
  case 0x0:
    *dbuff_size = SOC_PETRA_ITM_DBUFF_SIZE_BYTES_256;
    break;
  case 0x1:
    *dbuff_size = SOC_PETRA_ITM_DBUFF_SIZE_BYTES_512;
    break;
  case 0x2:
    *dbuff_size = SOC_PETRA_ITM_DBUFF_SIZE_BYTES_1024;
    break;
  case 0x3:
    *dbuff_size = SOC_PETRA_ITM_DBUFF_SIZE_BYTES_2048;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DBUFF_SIZE_INVALID_ERR, 10, exit)
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_itm_dbuff_internal2size()",0,0);
}

#include <soc/dpp/SAND/Utils/sand_footer.h>
