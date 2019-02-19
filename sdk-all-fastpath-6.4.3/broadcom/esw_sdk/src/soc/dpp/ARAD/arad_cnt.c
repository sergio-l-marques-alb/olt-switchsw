#include <soc/mcm/memregs.h>
#if defined(BCM_88650_A0)


/* $Id: arad_cnt.c,v 1.59 Broadcom SDK $
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

#define _ERR_MSG_MODULE_NAME BSL_SOC_CNT

/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/bsl.h>
#include <soc/dcmn/error.h>

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/mem.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/ARAD/arad_tbl_access.h>

#include <soc/dpp/ARAD/arad_api_framework.h>
#include <soc/dpp/ARAD/arad_framework.h>
#include <soc/dpp/ARAD/arad_api_ports.h>
#include <soc/dpp/ARAD/arad_cnt.h>
#include <soc/dpp/ARAD/arad_reg_access.h>
#include <soc/dpp/ARAD/arad_sw_db.h>


/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define ARAD_PROCESSOR_NDX_MAX                                   (ARAD_CNT_NOF_PROCESSOR_IDS-1)
#define ARAD_CNT_PORT_NDX_MAX                                    (ARAD_PORT_NOF_PP_PORTS - 1)
#define ARAD_HDR_COMPENSATION_INGRESS_MIN                        (-31)
#define ARAD_HDR_COMPENSATION_INGRESS_MAX                        (31)
#define ARAD_HDR_COMPENSATION_EGRESS_MIN                         (-64)
#define ARAD_HDR_COMPENSATION_EGRESS_MAX                         (63)
#define ARAD_CNT_VOQ_PARAMS_Q_SET_SIZE_MAX                       (ARAD_CNT_NOF_Q_SET_SIZES-1)
#define ARAD_CNT_TYPE_MAX                                        (ARAD_CNT_NOF_MODE_EG_TYPES-1)
#define ARAD_CNT_COUNTERS_INFO_SRC_TYPE_MAX                      (ARAD_CNT_NOF_SRC_TYPES-1)
#define ARAD_CNT_COUNTERS_INFO_MODE_ING_MAX                      (ARAD_CNT_NOF_MODE_INGS-1)
#define ARAD_CNT_COUNTERS_INFO_MODE_EG_MAX                       (ARAD_CNT_NOF_MODE_EGS-1)
#define ARAD_CNT_BASE_VAL_MAX                                    (0)
#define ARAD_CNT_COUNTERS_INFO_STAG_MAX                          (15)
#define ARAD_CNT_RESULT_ARR_NOF_COUNTERS_MAX                     (16)


#define ARAD_CNT_MODE_ING_NO_COLOR_FLD_VAL                       (1)
#define ARAD_CNT_MODE_ING_COLOR_RES_LOW_FLD_VAL                  (3)
#define ARAD_CNT_MODE_ING_COLOR_RES_HI_FLD_VAL                   (4)
#define ARAD_CNT_MODE_ING_ONE_ENTRY_FLD_VAL                      (0)
#define ARAD_CNT_MODE_ING_COLOR_ENQ_HI_FLD_VAL                   (2)
/*In Arad plus packets and packets has special value for the group size register*/
#ifdef BCM_88660_A0
#define ARAD_CNT_MODE_ING_PKTS_AND_PKTS_FLD_VAL                  (5)
#endif

#ifdef BCM_88650_B0
#define ARAD_CNT_ONE_ENTRY_MODE_ALL_VAL                           (0)
#define ARAD_CNT_ONE_ENTRY_MODE_DROP_VAL                          (1)
#define ARAD_CNT_ONE_ENTRY_MODE_FWD_VAL                           (2)
#endif

#define ARAD_CNT_MODE_EG_TYPE_TM_FLD_VAL                           (1)
#define ARAD_CNT_MODE_EG_TYPE_OUTLIF_FLD_VAL                       (2)
#define ARAD_CNT_MODE_EG_TYPE_VSI_FLD_VAL                          (3)
#define ARAD_CNT_MODE_EG_TYPE_PMF_FLD_VAL                          (0)
 
#define ARAD_CNT_MODE_EG_TYPE_EGQ_PROFILE_TM_FLD_VAL               (5)
#define ARAD_CNT_MODE_EG_TYPE_EGQ_PROFILE_TM_PORT_FLD_VAL          (4)
#define ARAD_CNT_MODE_EG_TYPE_EGQ_PROFILE_VSI_FLD_VAL              (3)
#define ARAD_CNT_MODE_EG_TYPE_EGQ_PROFILE_OUTLIF_FLD_VAL           (2)
 
#define ARAD_CNT_SRC_TYPE_ING_PP_FLD_VAL                         (0)
#define ARAD_CNT_SRC_TYPE_VOQ_FLD_VAL                            (10)
#define ARAD_CNT_SRC_TYPE_STAG_FLD_VAL                           (11)
#define ARAD_CNT_SRC_TYPE_VSQ_FLD_VAL                            (12)
#define ARAD_CNT_SRC_TYPE_CNM_ID_FLD_VAL                         (13)
#define ARAD_CNT_SRC_TYPE_EGR_PP_FLD_VAL                         (4)
#define ARAD_CNT_SRC_TYPE_ING_PP_MSB_FLD_VAL                     (1)
#define ARAD_CNT_SRC_TYPE_ING_PP_2_LSB_FLD_VAL                   (2)
#define ARAD_CNT_SRC_TYPE_ING_PP_2_MSB_FLD_VAL                   (3)
#define ARAD_CNT_SRC_TYPE_EG_PP_MSB_FLD_VAL                      (5)
#define ARAD_CNT_SRC_TYPE_OAM_ING_A_FLD_VAL                      (14)
#define ARAD_CNT_SRC_TYPE_OAM_ING_B_FLD_VAL                      (15)
#define ARAD_CNT_SRC_TYPE_OAM_ING_C_FLD_VAL                      (16)
#define ARAD_CNT_SRC_TYPE_OAM_ING_D_FLD_VAL                      (17)
#define ARAD_CNT_SRC_TYPE_EPNI_A_FLD_VAL                       (6)
#define ARAD_CNT_SRC_TYPE_EPNI_B_FLD_VAL                       (7)
#define ARAD_CNT_SRC_TYPE_EPNI_C_FLD_VAL                       (8)
#define ARAD_CNT_SRC_TYPE_EPNI_D_FLD_VAL                       (9)




#define ARAD_CNT_PKT_TRESH                                       (0x5801) /* 2^22 */
#define ARAD_CNT_OCT_TRESH                                       (0x7401) /* 2^29 */
#define ARAD_CNT_PREFETCH_TMR_CFG_MS                             (0x30d40) /* 0.03 ms in clocks */
#define ARAD_CNT_PREFETCH_TMR_CFG_ALGO                           (0x1e) /* 10 ms - 30 */
#define ARAD_CNT_PREFETCH_TMR_CFG_SEQ                            (0x12c) /* 50 ms - 300 */
#define ARAD_CNT_READ_DIRECT_READ                                (2)

#define ARAD_EPNI_BASE_VAL_TM                                    0x8000 /* Disable bit 15 of EPNI TM */
/*The value we use to mark that a specific hw counter (fwd_green, drop_green, fwd_yellow...) is not counted*/
#define ARAD_CNT_DO_NOT_COUNT_VAL                               (SOC_TMC_CNT_BMAP_OFFSET_INVALID_VAL)
#define ARAD_CNT_BMAP_OFFSET_COUNT                              (SOC_TMC_CNT_BMAP_OFFSET_COUNT)

/*

0 - LRT-LR0 , if LR0(maximal-visited) > Th, then read LR0, otherwise, if LRT (over threshold) exists read current LRT, else read LR0.
1 - LR0, read only maximal-visited.
2 - LRT, read only counters above threshold.
3 - Direct read, read counter according to ReqAddr

*/


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

CONST uint8 ch[4] =  {SOC_MEM_FIFO_DMA_CHANNEL_0, SOC_MEM_FIFO_DMA_CHANNEL_1,SOC_MEM_FIFO_DMA_CHANNEL_2,SOC_MEM_FIFO_DMA_CHANNEL_3}; 


/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

STATIC
void arad_cnt_convert_enum_val2_q_set_size(
        SOC_SAND_IN ARAD_CNT_Q_SET_SIZE enum_q_set_size,
        SOC_SAND_OUT uint32 *q_set_size_val
        )
 { 

switch (enum_q_set_size)
  {
    case ARAD_CNT_Q_SET_SIZE_1_Q:
		*q_set_size_val = 1;
      break;
        
    case ARAD_CNT_Q_SET_SIZE_2_Q:
      *q_set_size_val = 2;
      break;

    case ARAD_CNT_Q_SET_SIZE_4_Q:
      *q_set_size_val = 4;
      break;

    case ARAD_CNT_Q_SET_SIZE_8_Q:
      *q_set_size_val = 8;
      break;

    default:
		*q_set_size_val = 1;
      break;
  }

}

/*********************************************************************
* NAME:
*     arad_cnt_init
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
  arad_cnt_init(
    SOC_SAND_IN  int                 unit
  )
{
  ARAD_CNT_PROCESSOR_ID
	  cnt_proc_ndx;
  uint32
    res;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_INIT);

  /* 
   * The interrupt configuration is a legacy of Petra-B 
   * to see if in the algorithmic FIFO there are still to read 
   * In Arad, no need to set them, otherwise the interrupt interferes 
   * for ex. during the snake 
   */

  /*
   *  Enable the cache reading and configuring the fetch time (10 ms)
   *  the considerations for choosing 10ms are : implementation of the algorithm. The parameters
   *  used were  the counter size = 27 bit and the number of counters 16K.
   */
  for (cnt_proc_ndx = 0; cnt_proc_ndx < ARAD_CNT_NOF_PROCESSOR_IDS; cnt_proc_ndx++) {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  101 + cnt_proc_ndx,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CRPS_CRPS_GENERAL_CFGr, REG_PORT_ANY,  cnt_proc_ndx, CRPS_N_CACHE_READ_ENf,  0x1));

      /*
       *  Set the counter thresholds to a fixed value. 
       */
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  111 + cnt_proc_ndx,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CRPS_CRPS_THRESHOLDS_CFGr, REG_PORT_ANY,  cnt_proc_ndx, CRPS_N_PKT_CNT_THRESHf,  ARAD_CNT_PKT_TRESH));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  121 + cnt_proc_ndx,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CRPS_CRPS_THRESHOLDS_CFGr, REG_PORT_ANY,  cnt_proc_ndx, CRPS_N_OCT_CNT_THRESHf,  ARAD_CNT_OCT_TRESH));

      /* 
       * Set the Counter processor periods to indicate how many clocks it should be read 
       * Period reoslution: 1ms 
       * Period for algorithmic: 10 ms 
       * Period for sequential: 100 ms  
       */
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1241 + cnt_proc_ndx,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CRPS_CRPS_PREFETCH_TIMER_PERIOD_CFGr, REG_PORT_ANY,  cnt_proc_ndx, CRPS_N_PREFETCH_PERIOD_CFGf,  ARAD_CNT_PREFETCH_TMR_CFG_MS));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1201 + cnt_proc_ndx,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CRPS_CRPS_PREFETCH_TIMERS_CFGr, REG_PORT_ANY,  cnt_proc_ndx, CRPS_N_ALGORITHM_TMR_ENf,  0x1));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1211 + cnt_proc_ndx,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CRPS_CRPS_PREFETCH_TIMERS_CFGr, REG_PORT_ANY,  cnt_proc_ndx, CRPS_N_ALGORITHM_TMR_CFGf,  ARAD_CNT_PREFETCH_TMR_CFG_ALGO));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1221 + cnt_proc_ndx,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CRPS_CRPS_PREFETCH_TIMERS_CFGr, REG_PORT_ANY,  cnt_proc_ndx, CRPS_N_SEQUENTIAL_TMR_ENf,  0x1));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1231 + cnt_proc_ndx,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CRPS_CRPS_PREFETCH_TIMERS_CFGr, REG_PORT_ANY,  cnt_proc_ndx, CRPS_N_SEQUENTIAL_TMR_CFGf,  ARAD_CNT_PREFETCH_TMR_CFG_SEQ));
  }

  /* 
   * Init the EPNI registers
   */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  131,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_COUNTER_OUT_LIF_RANGE_0r, SOC_CORE_ALL, 0, COUNTER_OUT_LIF_RANGE_0_LOWf,  0));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  132,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_COUNTER_OUT_LIF_RANGE_0r, SOC_CORE_ALL, 0, COUNTER_OUT_LIF_RANGE_0_HIGHf,  0xFFFF));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  133,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_COUNTER_OUT_LIF_RANGE_1r, SOC_CORE_ALL, 0, COUNTER_OUT_LIF_RANGE_1_LOWf,  0));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  134,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, EPNI_COUNTER_OUT_LIF_RANGE_1r, SOC_CORE_ALL, 0, COUNTER_OUT_LIF_RANGE_1_HIGHf,  0xFFFF));

  /* 
   * Init the IQM registers
   */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  135,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_IQM_ENABLERSr, SOC_CORE_ALL, 0, CRPS_CMD_ENf,  0x1));

  /* 
   * Do not take in account packets with Queue-invalid in IQM (also for Stat-interface)
   */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  140,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_CNTPROCESSOR_CONFIG_1r, SOC_CORE_ALL, 0, CRPS_IQM_ACT_ON_QNVALIDf,  0x0));


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in arad_cnt_init()", 0, 0);
}


uint32
ARAD_CNT_MODE_EG_verify(
  SOC_SAND_IN  ARAD_CNT_MODE_EG *info
   )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

/*  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, ARAD_CNT_TYPE_MAX, ARAD_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 11, exit);*/
  SOC_SAND_ERR_IF_ABOVE_MAX(info->base_val, ARAD_CNT_BASE_VAL_MAX, ARAD_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_CNT_MODE_EG_verify()",0,0);
}
STATIC
  uint32
    arad_cnt_get_source_id(
      SOC_SAND_IN  ARAD_CNT_SRC_TYPE src_type,
      SOC_SAND_IN  uint32       proc_id,
      SOC_SAND_OUT uint32       *src
    )
{
  uint32
    fld_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_CNT_GET_SOURCE_ID);
  
  switch(src_type)
  {
  case ARAD_CNT_SRC_TYPE_ING_PP:
    fld_val = ARAD_CNT_SRC_TYPE_ING_PP_FLD_VAL;
    break;
  
  case ARAD_CNT_SRC_TYPE_VOQ:
  case ARAD_CNT_SRC_TYPE_STAG:
  case ARAD_CNT_SRC_TYPE_VSQ:
  case ARAD_CNT_SRC_TYPE_CNM_ID:
    fld_val = ARAD_CNT_SRC_TYPE_VOQ_FLD_VAL + proc_id;
    break;
  
  case ARAD_CNT_SRC_TYPE_EGR_PP:
    fld_val = ARAD_CNT_SRC_TYPE_EGR_PP_FLD_VAL;
    break;
 
  case ARAD_CNT_SRC_TYPE_ING_PP_MSB:
	fld_val = ARAD_CNT_SRC_TYPE_ING_PP_MSB_FLD_VAL;
	break;

  case ARAD_CNT_SRC_TYPE_ING_PP_2_LSB:
	fld_val = ARAD_CNT_SRC_TYPE_ING_PP_2_LSB_FLD_VAL;
	break;

  case ARAD_CNT_SRC_TYPE_ING_PP_2_MSB:
	fld_val = ARAD_CNT_SRC_TYPE_ING_PP_2_MSB_FLD_VAL;
	break;

  case ARAD_CNT_SRC_TYPE_EG_PP_MSB:
	fld_val = ARAD_CNT_SRC_TYPE_EG_PP_MSB_FLD_VAL;
	break;

  case ARAD_CNT_SRC_TYPE_OAM_ING_A:
  case ARAD_CNT_SRC_TYPE_OAM_ING_B:
  case ARAD_CNT_SRC_TYPE_OAM_ING_C:
  case ARAD_CNT_SRC_TYPE_OAM_ING_D:
	fld_val = ARAD_CNT_SRC_TYPE_OAM_ING_A_FLD_VAL + (src_type - ARAD_CNT_SRC_TYPE_OAM_ING_A);
	break;

  case ARAD_CNT_SRC_TYPE_EPNI_A:
  case ARAD_CNT_SRC_TYPE_EPNI_B:
  case ARAD_CNT_SRC_TYPE_EPNI_C:
  case ARAD_CNT_SRC_TYPE_EPNI_D:
	fld_val = ARAD_CNT_SRC_TYPE_EPNI_A_FLD_VAL + (src_type - ARAD_CNT_SRC_TYPE_EPNI_A);
	break;

  default:
    SOC_SAND_SET_ERROR_CODE(ARAD_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 23, exit);
  }

  *src = fld_val;

  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("arad_cnt_get_source_id()", 0, 0);

}
  
/*
 * Get the processor id e.g decide wether the counting is
 * done on processor A or on processor B
 */
STATIC
  uint32
    arad_cnt_get_processor_id(
      SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID processor_ndx,
      SOC_SAND_OUT uint32           *proc_id
    )

{
  uint32
    fld_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(ARAD_CNT_GET_PROCESSOR_ID);

  switch (processor_ndx)
  {
	  /* 1x1 mapping */
  case ARAD_CNT_PROCESSOR_ID_A:
  case ARAD_CNT_PROCESSOR_ID_B:
  case ARAD_CNT_PROCESSOR_ID_C:
  case ARAD_CNT_PROCESSOR_ID_D:
    fld_val = processor_ndx;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(ARAD_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 30, exit);
  }

  *proc_id = fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in processor_id_get_unsafe()", 0, 0);

}

/*
 * Get the number of counters and the following registers values from info:
 * CrpsCntSrcGroupSizes
 * CrpsOneEntryModeCntCmd(arad_b0 and above)
 *
 */
STATIC
uint32
  arad_cnt_ingress_params_get(
    SOC_SAND_IN  ARAD_CNT_COUNTERS_INFO *info,
    SOC_SAND_OUT uint32           *group_size,
    SOC_SAND_OUT uint32           *nof_counters,
    SOC_SAND_OUT uint32           *one_entry_mode_cnt
  )
{

  uint32
    fld_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  
 
  *one_entry_mode_cnt = ARAD_CNT_ONE_ENTRY_MODE_ALL_VAL;
  switch(info->mode_ing)
  {
  case ARAD_CNT_MODE_ING_NO_COLOR:
    fld_val = ARAD_CNT_MODE_ING_NO_COLOR_FLD_VAL;
    *nof_counters = 2;
#ifdef BCM_88660_A0
	if (info->format == ARAD_CNT_FORMAT_PKTS_AND_PKTS){
		fld_val = ARAD_CNT_MODE_ING_PKTS_AND_PKTS_FLD_VAL;
		*nof_counters = 1;
	}
#endif
    break;
  
  case ARAD_CNT_MODE_ING_COLOR_RES_LOW:
    fld_val = ARAD_CNT_MODE_ING_COLOR_RES_LOW_FLD_VAL;
    *nof_counters = 4;
    break;
   
  case ARAD_CNT_MODE_ING_COLOR_RES_HI:
	fld_val = ARAD_CNT_MODE_ING_COLOR_RES_HI_FLD_VAL;
    *nof_counters = 5;
	break;

  case ARAD_CNT_MODE_ING_ALL_NO_COLOR:
	fld_val = ARAD_CNT_MODE_ING_ONE_ENTRY_FLD_VAL;
    *nof_counters = 1;
#ifdef BCM_88650_B0
    *one_entry_mode_cnt = ARAD_CNT_ONE_ENTRY_MODE_ALL_VAL;
#endif
	break;

#ifdef BCM_88650_B0
  case ARAD_CNT_MODE_ING_DROP_NO_COLOR:
	fld_val = ARAD_CNT_MODE_ING_ONE_ENTRY_FLD_VAL;
    *nof_counters = 1;
    *one_entry_mode_cnt = ARAD_CNT_ONE_ENTRY_MODE_DROP_VAL;
	break;

  case ARAD_CNT_MODE_ING_FWD_NO_COLOR:
	fld_val = ARAD_CNT_MODE_ING_ONE_ENTRY_FLD_VAL;
    *nof_counters = 1;
    *one_entry_mode_cnt = ARAD_CNT_ONE_ENTRY_MODE_FWD_VAL;
	break;
#endif
  case ARAD_CNT_MODE_ING_COLOR_ENQ_HI:
	fld_val = ARAD_CNT_MODE_ING_COLOR_ENQ_HI_FLD_VAL;
    *nof_counters = 2;
#ifdef BCM_88660_A0
	*one_entry_mode_cnt = ARAD_CNT_ONE_ENTRY_MODE_ALL_VAL;
#endif
	break;
#ifdef BCM_88660_A0
  case ARAD_CNT_MODE_ING_CONFIGURABLE_OFFSETS:
    fld_val = info->custom_mode_params.nof_counters - 1;
    *nof_counters = info->custom_mode_params.nof_counters;
    break;
/* In arad plus green not green there is option for drop/fwd/all */
  case ARAD_CNT_MODE_ING_FWD_SIMPLE_COLOR:
	fld_val = ARAD_CNT_MODE_ING_COLOR_ENQ_HI_FLD_VAL;
	*nof_counters = 2;
	*one_entry_mode_cnt = ARAD_CNT_ONE_ENTRY_MODE_FWD_VAL;
	break;
  case ARAD_CNT_MODE_ING_DROP_SIMPLE_COLOR:
	fld_val = ARAD_CNT_MODE_ING_COLOR_ENQ_HI_FLD_VAL;
	*nof_counters = 2;
	*one_entry_mode_cnt = ARAD_CNT_ONE_ENTRY_MODE_DROP_VAL;
	break;
#endif
  default:
    SOC_SAND_SET_ERROR_CODE(ARAD_CNT_ING_MODE_OUT_OF_RANGE_ERR, 10, exit);
  }

  *group_size = fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_ingress_params_get()", 0, 0);

}


STATIC
  uint32
    arad_cnt_id_compute(
      SOC_SAND_IN int             unit,
      SOC_SAND_IN  ARAD_CNT_COUNTERS_INFO counter_info,
      SOC_SAND_IN  uint32             counter_ndx,
      SOC_SAND_OUT uint32            *real_counter_id
      )
{
  uint32 nof_entries, group_size = 0, one_entry_cmd = 0;
  uint32 res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = arad_cnt_ingress_params_get(&counter_info, &group_size, &nof_entries, &one_entry_cmd);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, fail);

  *real_counter_id = counter_ndx / nof_entries;

  ARAD_DO_NOTHING_AND_EXIT;

fail:
  *real_counter_id = 0;
  ARAD_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_id_compute()", 0, 0);
}

/*********************************************************************
 *     Configure the DMA for Counter Processor
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_cnt_dma_set_unsafe(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID          processor_ndx
  )
{
#if ARAD_CNT_USE_DMA
    
    int cmc = SOC_PCI_CMC(unit);
    int entry_words;
    uint32 start_addr;
    int     blk;
    uint8   at;
    
    soc_error_t rv = SOC_E_NONE;
	uint32 	*host_buff; 
#endif /* ARAD_CNT_USE_DMA */

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_DMA_SET_UNSAFE);

#if ARAD_CNT_USE_DMA
if (!SAL_BOOT_PLISIM ) {

  if (!SOC_UNIT_NUM_VALID(unit)) {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 4, exit);
  }
  entry_words = BYTES2WORDS(soc_reg_bytes((unit), CRPS_CRPS_CACHE_RD_RESULTSr)); 
 
  host_buff = soc_cm_salloc((unit), SOC_TMC_CNT_CACHE_LENGTH_ARAD * entry_words * sizeof(uint32),
                           "Counters DMA Buffer");
  if (host_buff == NULL) {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_MALLOC_FAIL, 52, exit);   
  }
  sal_memset(host_buff, 0, SOC_TMC_CNT_CACHE_LENGTH_ARAD * entry_words * sizeof(uint32));
  
  arad_sw_db_cnt_buff_and_index_set((unit), processor_ndx, host_buff, 0);

  rv = _soc_mem_sbus_fifo_dma_stop((unit), ch[processor_ndx]);
  if (SOC_FAILURE(rv)) {
      SOC_SAND_SET_ERROR_CODE(ARAD_CNT_START_FIFO_DMA_ERR, 53, exit);   
  }

  rv = _soc_mem_sbus_fifo_dma_start_memreg((unit), ch[processor_ndx], 
											FALSE /*is_mem*/, 0, CRPS_CRPS_CACHE_RD_RESULTSr, MEM_BLOCK_ANY, 0,
											SOC_TMC_CNT_CACHE_LENGTH_ARAD, host_buff);
  if (SOC_FAILURE(rv)) {
      SOC_SAND_SET_ERROR_CODE(ARAD_CNT_START_FIFO_DMA_ERR, 54, exit);   
  }

  /* Set the start addr for each proc */
  start_addr = soc_reg_addr_get(unit, CRPS_CRPS_CACHE_RD_RESULTSr, REG_PORT_ANY, 0, FALSE, &blk, &at);
  start_addr =start_addr + processor_ndx*entry_words;
  soc_pci_write(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_SBUS_START_ADDRESS_OFFSET(cmc, ch[processor_ndx]), start_addr);
}
#endif /* ARAD_CNT_USE_DMA */
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_dma_set()", 0, 0);

}
uint32
  arad_cnt_dma_unset_unsafe(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID          processor_ndx
  )
{
#if ARAD_CNT_USE_DMA 
  uint32 *host_buff=NULL, buff_index; 
  soc_error_t rv = SOC_E_NONE; 
#endif /* ARAD_CNT_USE_DMA */ 

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_DMA_UNSET_UNSAFE); 

#if ARAD_CNT_USE_DMA 
  if (!SAL_BOOT_PLISIM ) { 
    rv = _soc_mem_sbus_fifo_dma_stop(unit, ch[processor_ndx]); 
    if (SOC_FAILURE(rv)) { 
      SOC_SAND_SET_ERROR_CODE(ARAD_CNT_START_FIFO_DMA_ERR, 10, exit); 
    } 
    arad_sw_db_cnt_buff_and_index_get(unit, processor_ndx, &host_buff, &buff_index); 
    if (host_buff != NULL) { 
      soc_cm_sfree(unit, host_buff); 
      arad_sw_db_cnt_buff_and_index_set(unit, processor_ndx, NULL, 0); 
    } 
  } 
#endif 
  ARAD_DO_NOTHING_AND_EXIT; 
exit: 
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_dma_unset()", 0, 0); 
}



#ifdef BCM_88660_A0
uint32
  _arad_cnt_counter_bmap_mem_by_src_type_get(
    SOC_SAND_IN  int                unit,
	SOC_SAND_IN  uint32                proc_id,
	SOC_SAND_IN  ARAD_CNT_SRC_TYPE     src_type,
	SOC_SAND_OUT soc_mem_t             *mem
	   )
{
  soc_mem_t       
	  iqm_counter_bmap_mem [] = {CRPS_IQM_OFFSET_BMAP_Am, CRPS_IQM_OFFSET_BMAP_Bm, CRPS_IQM_OFFSET_BMAP_Cm, CRPS_IQM_OFFSET_BMAP_Dm},
	  counter_bmap_mem = INVALIDf;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  switch (src_type) {
	case ARAD_CNT_SRC_TYPE_ING_PP:
	case ARAD_CNT_SRC_TYPE_ING_PP_MSB:
      counter_bmap_mem = CRPS_IRPP_OFFSET_BMAP_Am;
	  break;	  
    case ARAD_CNT_SRC_TYPE_VOQ:
    case ARAD_CNT_SRC_TYPE_STAG:
    case ARAD_CNT_SRC_TYPE_VSQ:
    case ARAD_CNT_SRC_TYPE_CNM_ID:
      counter_bmap_mem = iqm_counter_bmap_mem[proc_id];
      break;  
	case ARAD_CNT_SRC_TYPE_EGR_PP:
	case ARAD_CNT_SRC_TYPE_EG_PP_MSB:
      counter_bmap_mem = CRPS_EGQ_OFFSET_BMAPm;
	  break;
	case ARAD_CNT_SRC_TYPE_ING_PP_2_LSB:
	case ARAD_CNT_SRC_TYPE_ING_PP_2_MSB:
	  counter_bmap_mem = CRPS_IRPP_OFFSET_BMAP_Bm;
	  break;	
	case ARAD_CNT_SRC_TYPE_OAM_ING_A:
	case ARAD_CNT_SRC_TYPE_OAM_ING_B:
	case ARAD_CNT_SRC_TYPE_OAM_ING_C:
	case ARAD_CNT_SRC_TYPE_OAM_ING_D:
	/* Always 1 count for OAM */
      break;
	case ARAD_CNT_SRC_TYPE_EPNI_A:
    case ARAD_CNT_SRC_TYPE_EPNI_B:
      counter_bmap_mem = CRPS_EPNI_OFFSET_BMAP_Am;
	  break;
	case ARAD_CNT_SRC_TYPE_EPNI_C:
	case ARAD_CNT_SRC_TYPE_EPNI_D:
      counter_bmap_mem = CRPS_EPNI_OFFSET_BMAP_Bm;
      break;
    default:
		SOC_SAND_SET_ERROR_CODE(ARAD_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 10, exit);
	}
	*mem = counter_bmap_mem;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in _arad_cnt_counter_bmap_mem_by_src_type_get()", 0, 0);
}
#endif


#ifdef BCM_88660_A0
uint32
  _arad_cnt_do_not_count_field_by_src_type_get(
    SOC_SAND_IN  int                unit,
	SOC_SAND_IN  uint32                proc_id,
	SOC_SAND_IN  ARAD_CNT_SRC_TYPE     src_type,
	SOC_SAND_OUT soc_field_t           *field
	   )
{
  soc_field_t       
	  iqm_do_not_count_entryf[] = {IQM_A_DO_NOT_COUNT_OFFSETf,  IQM_B_DO_NOT_COUNT_OFFSETf, IQM_C_DO_NOT_COUNT_OFFSETf, IQM_D_DO_NOT_COUNT_OFFSETf},
	  do_not_count_fld = INVALIDf;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  switch (src_type) {
	case ARAD_CNT_SRC_TYPE_ING_PP:
	case ARAD_CNT_SRC_TYPE_ING_PP_MSB:
      do_not_count_fld = IRPP_A_DO_NOT_COUNT_OFFSETf;
	  break;	  
    case ARAD_CNT_SRC_TYPE_VOQ:
    case ARAD_CNT_SRC_TYPE_STAG:
    case ARAD_CNT_SRC_TYPE_VSQ:
    case ARAD_CNT_SRC_TYPE_CNM_ID:
      do_not_count_fld = iqm_do_not_count_entryf[proc_id];
      break;  
	case ARAD_CNT_SRC_TYPE_EGR_PP:
	case ARAD_CNT_SRC_TYPE_EG_PP_MSB:
      do_not_count_fld = EGQ_DO_NOT_COUNT_OFFSETf;
	  break;
	case ARAD_CNT_SRC_TYPE_ING_PP_2_LSB:
	case ARAD_CNT_SRC_TYPE_ING_PP_2_MSB:
	  do_not_count_fld = IRPP_B_DO_NOT_COUNT_OFFSETf;
	  break;	
	case ARAD_CNT_SRC_TYPE_OAM_ING_A:
	case ARAD_CNT_SRC_TYPE_OAM_ING_B:
	case ARAD_CNT_SRC_TYPE_OAM_ING_C:
	case ARAD_CNT_SRC_TYPE_OAM_ING_D:
	/* Always 1 count for OAM */
      break;
	case ARAD_CNT_SRC_TYPE_EPNI_A:
    case ARAD_CNT_SRC_TYPE_EPNI_B:
      do_not_count_fld = EPNI_A_DO_NOT_COUNT_OFFSETf;
	  break;
	case ARAD_CNT_SRC_TYPE_EPNI_C:
	case ARAD_CNT_SRC_TYPE_EPNI_D:
      do_not_count_fld = EPNI_B_DO_NOT_COUNT_OFFSETf;
      break;
    default:
		SOC_SAND_SET_ERROR_CODE(ARAD_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 10, exit);
	}
	*field = do_not_count_fld;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in _arad_cnt_do_not_count_field_by_src_type_get()", 0, 0);
}
#endif


#ifdef BCM_88650_B0
uint32
  _arad_cnt_one_entry_field_by_src_type_get(
    SOC_SAND_IN  int                unit,
	SOC_SAND_IN  uint32                proc_id, 
	SOC_SAND_IN  ARAD_CNT_SRC_TYPE     src_type,
	SOC_SAND_OUT soc_field_t           *field
	   )
{
  soc_field_t       
	  iqm_one_entryf[] = {IQM_A_ONE_ENTRY_CNT_CMDf, IQM_B_ONE_ENTRY_CNT_CMDf, IQM_C_ONE_ENTRY_CNT_CMDf, IQM_D_ONE_ENTRY_CNT_CMDf},
	  one_entry_mode_fld = INVALIDf;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  switch (src_type) {
	case ARAD_CNT_SRC_TYPE_ING_PP:
	case ARAD_CNT_SRC_TYPE_ING_PP_MSB:
      one_entry_mode_fld = IRPP_A_ONE_ENTRY_CNT_CMDf;
	  break;	  
    case ARAD_CNT_SRC_TYPE_VOQ:
    case ARAD_CNT_SRC_TYPE_STAG:
    case ARAD_CNT_SRC_TYPE_VSQ:
    case ARAD_CNT_SRC_TYPE_CNM_ID:
      one_entry_mode_fld = iqm_one_entryf[proc_id]; 
      break;  
	case ARAD_CNT_SRC_TYPE_EGR_PP:
	case ARAD_CNT_SRC_TYPE_EG_PP_MSB:
      one_entry_mode_fld = EGQ_ONE_ENTRY_CNT_CMDf;
	  break;
	case ARAD_CNT_SRC_TYPE_ING_PP_2_LSB:
	case ARAD_CNT_SRC_TYPE_ING_PP_2_MSB:
	  one_entry_mode_fld = IRPP_B_ONE_ENTRY_CNT_CMDf;
	  break;	
	case ARAD_CNT_SRC_TYPE_OAM_ING_A:
	case ARAD_CNT_SRC_TYPE_OAM_ING_B:
	case ARAD_CNT_SRC_TYPE_OAM_ING_C:
	case ARAD_CNT_SRC_TYPE_OAM_ING_D:
	/* Always 1 count for OAM */
      break;
	case ARAD_CNT_SRC_TYPE_EPNI_A:
    case ARAD_CNT_SRC_TYPE_EPNI_B:
      one_entry_mode_fld = EPNI_A_ONE_ENTRY_CNT_CMDf;
	  break;
	case ARAD_CNT_SRC_TYPE_EPNI_C:
	case ARAD_CNT_SRC_TYPE_EPNI_D:
      one_entry_mode_fld = EPNI_B_ONE_ENTRY_CNT_CMDf;
      break;
    default:
		SOC_SAND_SET_ERROR_CODE(ARAD_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 10, exit);
	}
	*field = one_entry_mode_fld;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in _arad_cnt_one_entry_field_by_src_type_get()", 0, 0);
}
#endif


uint32
  _arad_cnt_group_size_field_by_src_type_get(
    SOC_SAND_IN  int                unit,
	SOC_SAND_IN  uint32                proc_id, 
	SOC_SAND_IN  ARAD_CNT_SRC_TYPE     src_type,
	SOC_SAND_OUT soc_field_t           *field
	   )
{
  soc_field_t       
	  iqm_group_sizef[] = {IQM_A_GROUP_SIZEf, IQM_B_GROUP_SIZEf, IQM_C_GROUP_SIZEf, IQM_D_GROUP_SIZEf},
	  group_size_fld = INVALIDf;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  switch (src_type) {
	case ARAD_CNT_SRC_TYPE_ING_PP:
	case ARAD_CNT_SRC_TYPE_ING_PP_MSB:
	  group_size_fld = IRPP_A_GROUP_SIZEf;
	  break;	  
    case ARAD_CNT_SRC_TYPE_VOQ:
    case ARAD_CNT_SRC_TYPE_STAG:
    case ARAD_CNT_SRC_TYPE_VSQ:
    case ARAD_CNT_SRC_TYPE_CNM_ID:
      group_size_fld = iqm_group_sizef[proc_id];
	  break;
	case ARAD_CNT_SRC_TYPE_EGR_PP:
	case ARAD_CNT_SRC_TYPE_EG_PP_MSB:
	  group_size_fld = EGQ_GROUP_SIZEf;
	  break;
	case ARAD_CNT_SRC_TYPE_ING_PP_2_LSB:
	case ARAD_CNT_SRC_TYPE_ING_PP_2_MSB:
      group_size_fld = IRPP_B_GROUP_SIZEf;
	  break;
	case ARAD_CNT_SRC_TYPE_OAM_ING_A:
	case ARAD_CNT_SRC_TYPE_OAM_ING_B:
	case ARAD_CNT_SRC_TYPE_OAM_ING_C:
	case ARAD_CNT_SRC_TYPE_OAM_ING_D:
	  /* Always 1 count for OAM */
	  break;
	case ARAD_CNT_SRC_TYPE_EPNI_A:
	case ARAD_CNT_SRC_TYPE_EPNI_B:
	  group_size_fld = EPNI_A_GROUP_SIZEf;
	  break;
	case ARAD_CNT_SRC_TYPE_EPNI_C:
	case ARAD_CNT_SRC_TYPE_EPNI_D:
	  group_size_fld = EPNI_B_GROUP_SIZEf;
      break;
	default:
		SOC_SAND_SET_ERROR_CODE(ARAD_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 10, exit);
	}
	*field = group_size_fld;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in _arad_cnt_group_size_field_by_src_type_get()", 0, 0);
}

/*********************************************************************
 *     Configure the EPNI counting related registers.
 *     the base val paramaeter has no meaning in ARAD_CNT_MODE_EG_TYPE_TM and
 *     ARAD_CNT_MODE_EG_TYPE_TM_PORt
 *     affects the statistic interface egress counting in billing mode.
 *       
*********************************************************************/
uint32 
   arad_cnt_epni_regs_set_unsafe(
      SOC_SAND_IN  int                 unit,
      SOC_SAND_IN  ARAD_CNT_SRC_TYPE      src_type,
      SOC_SAND_IN ARAD_CNT_MODE_EG_TYPES  eg_mode_type,
      SOC_SAND_IN uint32                  base_val
      )
{
    uint32
	  data[20],
      base_val_pmf = 0,
      base_val_tm = 0,
      base_val_vsi = 0,
      base_val_outlif = 0,
      tm_out_port_selector = 0,
      fld_val = 0,
      pp_port_ndx = 0,
      res = SOC_SAND_OK;
    soc_field_t base_val_fld;
    soc_reg_t base_val_reg;
    uint64 field64;
    uint32 is_counter_0 = FALSE;
    int     core;
#ifdef BCM_88650_B0
       
#endif 

    SOC_SAND_INIT_ERROR_DEFINITIONS(0);

    is_counter_0 = ((src_type == ARAD_CNT_SRC_TYPE_EPNI_A) || (src_type == ARAD_CNT_SRC_TYPE_EPNI_B));

    switch (eg_mode_type)
    {
    case ARAD_CNT_MODE_EG_TYPE_TM:
      base_val_tm = ARAD_EPNI_BASE_VAL_TM;
      fld_val = ARAD_CNT_MODE_EG_TYPE_TM_FLD_VAL;
      tm_out_port_selector = 1; /* Out-Port is TM */
      break;
    
    case ARAD_CNT_MODE_EG_TYPE_VSI:
      base_val_vsi = base_val;
      fld_val = ARAD_CNT_MODE_EG_TYPE_VSI_FLD_VAL;
      break;

    case ARAD_CNT_MODE_EG_TYPE_OUTLIF:
      
      base_val_outlif = base_val;
      fld_val = ARAD_CNT_MODE_EG_TYPE_OUTLIF_FLD_VAL;
      break;

    case ARAD_CNT_MODE_EG_TYPE_PMF:
      base_val_pmf = base_val;
      fld_val = ARAD_CNT_MODE_EG_TYPE_PMF_FLD_VAL;
      break;
    case ARAD_CNT_MODE_EG_TYPE_TM_PORT:
      base_val_tm = ARAD_EPNI_BASE_VAL_TM;
      fld_val = ARAD_CNT_MODE_EG_TYPE_TM_FLD_VAL;
      tm_out_port_selector = 0; /* Out-Port is PP */
      break;
    default:
      break;
    }

    /* In case of TM work mode, set the out port selector to the right value in the configuration table. */
    if (eg_mode_type == ARAD_CNT_MODE_EG_TYPE_TM || eg_mode_type == ARAD_CNT_MODE_EG_TYPE_TM_PORT) {
        soc_reg_t counter_port_tm_reg;

        counter_port_tm_reg = EPNI_COUNTER_PORT_TMr;
#ifdef BCM_88650_B0
        if (SOC_IS_ARAD_B0_AND_ABOVE(unit)) {
            counter_port_tm_reg = EPNI_STATISTICS_REPORTING_CONFIGURATIONr;
        }
#endif
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  21,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, counter_port_tm_reg, REG_PORT_ANY, 0, COUNTER_PORT_TMf,  tm_out_port_selector));
    }

    if (eg_mode_type == ARAD_CNT_MODE_EG_TYPE_OUTLIF) {
        /*set the counting source map*/
        base_val_fld = (is_counter_0)? COUNTER_0_SOURCEf:COUNTER_1_SOURCEf;
        for(pp_port_ndx = 0;  pp_port_ndx < ARAD_PORT_NOF_PP_PORTS;  pp_port_ndx++)
        {
    		SOC_SAND_SOC_IF_ERROR_RETURN(res, 29, exit, READ_EPNI_COUNTER_SOURCE_MAPm(unit, MEM_BLOCK_ANY, pp_port_ndx, data));
            soc_EPNI_COUNTER_SOURCE_MAPm_field32_set(unit, data, base_val_fld, 1); 
            SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, WRITE_EPNI_COUNTER_SOURCE_MAPm(unit, MEM_BLOCK_ANY, pp_port_ndx, data));
        }
    }

    
	base_val_reg = (is_counter_0)? EPNI_COUNTER_0_BASEr:EPNI_COUNTER_1_BASEr;
	base_val_fld = (is_counter_0)? COUNTER_0_MODE_0_BASEf:COUNTER_1_MODE_0_BASEf;

    COMPILER_64_SET(field64,0,base_val_pmf);    
	SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  25,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, base_val_reg, REG_PORT_ANY, 0, base_val_fld + (COUNTER_0_MODE_0_BASEf - COUNTER_0_MODE_0_BASEf),  field64));
    COMPILER_64_SET(field64,0,base_val_tm);    
	SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  26,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, base_val_reg, REG_PORT_ANY, 0, base_val_fld + (COUNTER_0_MODE_1_BASEf - COUNTER_0_MODE_0_BASEf),  field64));
    COMPILER_64_SET(field64,0,base_val_vsi);
	SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  27,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, base_val_reg, REG_PORT_ANY, 0, base_val_fld + (COUNTER_0_MODE_2_BASEf - COUNTER_0_MODE_0_BASEf),  field64));
    COMPILER_64_SET(field64,0,base_val_outlif);
	SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  28,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, base_val_reg, REG_PORT_ANY, 0, base_val_fld + (COUNTER_0_MODE_3_BASEf - COUNTER_0_MODE_0_BASEf),  field64));

    for (core=0; core<SOC_DPP_DEFS_GET(unit, nof_cores); core++) {
        for(pp_port_ndx = 0;  pp_port_ndx < SOC_TMC_NOF_FAP_PORTS_PER_CORE;  pp_port_ndx++)
        {
    		SOC_SAND_SOC_IF_ERROR_RETURN(res, 29, exit, READ_EPNI_PP_COUNTER_TABLEm(unit, EPNI_BLOCK(unit, core), pp_port_ndx, data));

    		base_val_fld = (is_counter_0)? COUNTER_0_MODEf:COUNTER_1_MODEf;
    		soc_EPNI_PP_COUNTER_TABLEm_field32_set(unit, data, base_val_fld, fld_val);

    		base_val_fld = (is_counter_0)? COUNTER_0_POINTER_BASEf:COUNTER_1_POINTER_BASEf;
    		soc_EPNI_PP_COUNTER_TABLEm_field32_set(unit, data, base_val_fld, 0);

    		SOC_SAND_SOC_IF_ERROR_RETURN(res, 30, exit, WRITE_EPNI_PP_COUNTER_TABLEm(unit, EPNI_BLOCK(unit, core), pp_port_ndx, data));
        }
    }
exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_epni_regs_set_unsafe()", 0, 0);
}


/*********************************************************************
 *     Configure the Counter Processor: its counting mode and
 *     its counting source.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_cnt_counters_set_unsafe(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID          processor_ndx,
    SOC_SAND_IN  ARAD_CNT_COUNTERS_INFO         *info
  )
{
  uint32
	  data[20],
    proc_id,
    group_size_val=0,
      nof_counters = 0,
      one_entry_mode_cnt = 0,
    src_type=0,
    top_q,
    /* 
     * Select the Out-Port field in the Counter-Id for the TM counter mode.
     * 0 - PP, 1 - TM 
     */ 
    
    fld_val = 0,
    res = SOC_SAND_OK;
  uint32
    pp_port_ndx;
  soc_field_t
      crps_iqm_cmd_src_type_f[] = {CRPS_IQM_CMD_SRC_TYPE_Af, CRPS_IQM_CMD_SRC_TYPE_Bf, CRPS_IQM_CMD_SRC_TYPE_Cf, CRPS_IQM_CMD_SRC_TYPE_Df},
      crps_iqm_cmd_queue_shift_f[] = {CRPS_IQM_CMD_QUEUE_SHIFT_Af, CRPS_IQM_CMD_QUEUE_SHIFT_Bf, CRPS_IQM_CMD_QUEUE_SHIFT_Cf, CRPS_IQM_CMD_QUEUE_SHIFT_Df},
      crps_iqm_cmd_base_q_f[] = {CRPS_IQM_CMD_BASE_Q_Af, CRPS_IQM_CMD_BASE_Q_Bf, CRPS_IQM_CMD_BASE_Q_Cf, CRPS_IQM_CMD_BASE_Q_Df},
      crps_iqm_cmd_top_q_f[] = {CRPS_IQM_CMD_TOP_Q_Af, CRPS_IQM_CMD_TOP_Q_Bf, CRPS_IQM_CMD_TOP_Q_Cf, CRPS_IQM_CMD_TOP_Q_Df},
	  group_size_field = INVALIDf;
  soc_reg_t
      iqm_cnt_comman_cfg_1_r[] = {IQM_CNT_COMMAN_CFG_1_Ar, IQM_CNT_COMMAN_CFG_1_Br, IQM_CNT_COMMAN_CFG_1_Cr, IQM_CNT_COMMAN_CFG_1_Dr},
      iqm_cnt_comman_cfg_2_r[] = {IQM_CNT_COMMAN_CFG_2_Ar, IQM_CNT_COMMAN_CFG_2_Br, IQM_CNT_COMMAN_CFG_2_Cr, IQM_CNT_COMMAN_CFG_2_Dr};
#ifdef BCM_88650_B0
       
#endif 

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_COUNTERS_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

#ifdef BCM_88650_B0
 if (!SOC_UNIT_NUM_VALID(unit)) {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 4, exit);
  }
#endif

#if ARAD_CNT_USE_DMA
if (!SAL_BOOT_PLISIM ) {
  res = arad_cnt_dma_set_unsafe(unit, processor_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
 
}
#endif
  res = arad_cnt_get_processor_id(
          processor_ndx,
          &proc_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 6, exit);

  res = arad_cnt_get_source_id(
          info->src_type,
          proc_id,
          &src_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 7, exit);
  
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CRPS_CRPS_CNT_SRC_CFGr, REG_PORT_ANY,  proc_id, CRPS_N_CNT_SRCf,  src_type));

  res = arad_cnt_ingress_params_get(
			info,
			&group_size_val,
			&nof_counters,
			&one_entry_mode_cnt
		  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);
  res = _arad_cnt_group_size_field_by_src_type_get(unit, proc_id, info->src_type, &group_size_field);
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
  if (group_size_field != INVALIDf) {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res, 10, exit, ARAD_REG_ACCESS_ERR, soc_reg_above_64_field32_modify(unit, CRPS_CRPS_CNT_SRC_GROUP_SIZESr, REG_PORT_ANY, 0, group_size_field, group_size_val));
  }
  if ((info->src_type == ARAD_CNT_SRC_TYPE_VOQ) || (info->src_type == ARAD_CNT_SRC_TYPE_STAG) || 
	  (info->src_type == ARAD_CNT_SRC_TYPE_VSQ) || (info->src_type == ARAD_CNT_SRC_TYPE_CNM_ID)){
     SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  14,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_CNT_PROCESSOR_CONFIG_2r, SOC_CORE_ALL, 0, crps_iqm_cmd_src_type_f[proc_id],  info->src_type - ARAD_CNT_SRC_TYPE_VOQ ));
  }

#ifdef BCM_88650_B0
  if (SOC_IS_ARAD_B0_AND_ABOVE(unit)) {
    soc_field_t one_entry_mode_field = INVALIDf;
	res = _arad_cnt_one_entry_field_by_src_type_get(unit, proc_id, info->src_type, &one_entry_mode_field);
	SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
	if (one_entry_mode_field != INVALIDf){
	  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  16,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CRPS_CRPS_ONE_ENTRY_MODE_CNT_CMDr, REG_PORT_ANY, 0, one_entry_mode_field,  one_entry_mode_cnt));
	}
  }
#endif
/*configurable mode handling*/
#ifdef BCM_88660_A0
  if ((SOC_IS_ARADPLUS(unit)) && (info->mode_ing == ARAD_CNT_MODE_ING_CONFIGURABLE_OFFSETS)){
    soc_field_t do_not_count_field = INVALIDf;
    uint64 do_not_count_val;
	soc_mem_t src_btmap_mem = INVALIDm;
	soc_field_t bmap_fields[] = {GREEN_ADMITf, GREEN_DISCARDf, YELLOW_1_ADMITf, YELLOW_1_DISCARDf, YELLOW_2_ADMITf, YELLOW_2_DISCARDf, RED_ADMITf, RED_DISCARDf};
	uint32 reg_val = 0;
	uint32 entry_index, counter_index;

    COMPILER_64_SET(do_not_count_val, 0, ARAD_CNT_DO_NOT_COUNT_VAL);
	/*set entry for filter for counters that will not be count*/
	res = _arad_cnt_do_not_count_field_by_src_type_get(unit, proc_id, info->src_type, &do_not_count_field);
	SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);
	SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  18,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_modify(unit, CRPS_CRPS_DO_NOT_COUNT_FILTERr, REG_PORT_ANY, 0, do_not_count_field,  do_not_count_val));
	res = _arad_cnt_counter_bmap_mem_by_src_type_get(unit, proc_id, info->src_type, &src_btmap_mem);
	SOC_SAND_CHECK_FUNC_RESULT(res, 19, exit);
	/*entries iteration- iter over all including the filter entry*/
	for(entry_index = 0 ; entry_index < ARAD_CNT_BMAP_OFFSET_COUNT; entry_index++){
	  /*iter on bmap*/
	  for(counter_index = 0 ; counter_index < ARAD_CNT_BMAP_OFFSET_COUNT; counter_index++){
		if(SHR_BITGET(&(info->custom_mode_params.entries_bmaps[entry_index]), counter_index)){
		  /*if the counter  is part of the entry we have to write the entry index to the register field of the counter*/
          soc_mem_field_set(unit, src_btmap_mem, &reg_val , bmap_fields[counter_index], &entry_index); 
		}
	  }
	}
	res = soc_mem_write(unit, src_btmap_mem, MEM_BLOCK_ALL, group_size_val, &reg_val);
	SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 20, exit);
  }
#endif

  if((info->src_type == SOC_TMC_CNT_SRC_TYPE_EPNI_A) || (info->src_type == SOC_TMC_CNT_SRC_TYPE_EPNI_B) || 
     (info->src_type == SOC_TMC_CNT_SRC_TYPE_EPNI_C) || (info->src_type == SOC_TMC_CNT_SRC_TYPE_EPNI_D)){
      res = arad_cnt_epni_regs_set_unsafe(unit, info->src_type, info->mode_eg.type, info->mode_eg.base_val);
      SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 30, exit);
  }

  /* Set the egress work mode */
  if((info->src_type == SOC_TMC_CNT_SRC_TYPE_EGR_PP) || (info->src_type == SOC_TMC_CNT_SRC_TYPE_EGR_PP_MSB))
  {
    switch (info->mode_eg.type)
    {
    case ARAD_CNT_MODE_EG_TYPE_TM:
      fld_val = ARAD_CNT_MODE_EG_TYPE_EGQ_PROFILE_TM_FLD_VAL;
  	  break;
    
    case ARAD_CNT_MODE_EG_TYPE_VSI:
      fld_val = ARAD_CNT_MODE_EG_TYPE_EGQ_PROFILE_VSI_FLD_VAL;
      break;

	case ARAD_CNT_MODE_EG_TYPE_OUTLIF:
      fld_val = ARAD_CNT_MODE_EG_TYPE_EGQ_PROFILE_OUTLIF_FLD_VAL;
      break;

    case ARAD_CNT_MODE_EG_TYPE_PMF:
        fld_val = ARAD_CNT_MODE_EG_TYPE_PMF_FLD_VAL;
        break;
    case ARAD_CNT_MODE_EG_TYPE_TM_PORT:
        fld_val = ARAD_CNT_MODE_EG_TYPE_EGQ_PROFILE_TM_PORT_FLD_VAL;
    default:
      break;
    }

    for(pp_port_ndx = 0;  pp_port_ndx < ARAD_PORT_NOF_PP_PORTS;  pp_port_ndx++)
    {
		SOC_SAND_SOC_IF_ERROR_RETURN(res, 1000, exit, READ_EGQ_PP_PPCTm(unit, MEM_BLOCK_ANY, pp_port_ndx, data));
		soc_EGQ_PP_PPCTm_field32_set(unit, data, COUNTER_PROFILEf, fld_val);
		SOC_SAND_SOC_IF_ERROR_RETURN(res, 1010, exit, WRITE_EGQ_PP_PPCTm(unit, MEM_BLOCK_ANY, pp_port_ndx, data));
    }
  }
  
  /*
   *  Set the VOQ parameters
   */
  if (info->src_type == ARAD_CNT_SRC_TYPE_VOQ)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1160,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_CNT_PROCESSOR_CONFIG_2r, SOC_CORE_ALL, 0, crps_iqm_cmd_queue_shift_f[proc_id],  info->voq_cnt.q_set_size));
  
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1170,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, iqm_cnt_comman_cfg_1_r[proc_id], REG_PORT_ANY, 0, crps_iqm_cmd_base_q_f[proc_id],  info->voq_cnt.start_q));

    /* Compute the top Queue = Base-Queue + (32K / #counters-per-group) * #VOQs per Counter */
    top_q = info->voq_cnt.start_q + (((ARAD_COUNTER_NDX_MAX + 1) / nof_counters) * (1 << info->voq_cnt.q_set_size));
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1171,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, iqm_cnt_comman_cfg_2_r[proc_id], REG_PORT_ANY, 0, crps_iqm_cmd_top_q_f[proc_id],  top_q));
  }

  /*
   *  Set the Statistic-Tag parameters
   */
  if (info->src_type == ARAD_CNT_SRC_TYPE_STAG)
  {
    SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1240,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IQM_STATISTICS_TAG_CONFIGURATIONr, SOC_CORE_ALL, 0, STAT_TAG_CRPS_CMD_LSBf, info->stag_lsb));
  }
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1250,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CRPS_CRPS_GENERAL_CFGr, REG_PORT_ANY,  processor_ndx, CRPS_N_COUNT_MODEf,  info->format));

  /* set replicated_pkts parameters */
  
  if (info->replicated_pkts == SOC_TMC_CNT_REPLICATED_PKTS_FRWRD) {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1260,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IRR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, COUNT_ALL_COPIESf, 0));
  } else {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1270,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, IRR_STATIC_CONFIGURATIONr, REG_PORT_ANY, 0, COUNT_ALL_COPIESf, 1));
  }

  
  /* In wide mode disable the algorithmic read */
  if ((info->format == ARAD_CNT_FORMAT_PKTS) || (info->format == ARAD_CNT_FORMAT_BYTES)|| (info->format == ARAD_CNT_FORMAT_MAX_QUEUE_SIZE) ) {
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1201 + processor_ndx,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CRPS_CRPS_PREFETCH_TIMERS_CFGr, REG_PORT_ANY,  processor_ndx, CRPS_N_ALGORITHM_TMR_ENf,  0x0));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1221 + processor_ndx,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CRPS_CRPS_PREFETCH_TIMERS_CFGr, REG_PORT_ANY,  processor_ndx, CRPS_N_SEQUENTIAL_TMR_ENf,  0x0));
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  1222 + processor_ndx,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CRPS_CRPS_GENERAL_CFGr, REG_PORT_ANY,  processor_ndx, CRPS_N_CACHE_READ_ENf,  0x0));
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_counters_set_unsafe()", 0, 0);
}

uint32
  arad_cnt_dma_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID         processor_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_DMA_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(processor_ndx, ARAD_PROCESSOR_NDX_MAX, ARAD_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 10, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_dma_set_verify()", 0, 0);
}



uint32
  arad_cnt_dma_unset_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID         processor_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_DMA_UNSET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(processor_ndx, ARAD_PROCESSOR_NDX_MAX, ARAD_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 11, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_dma_unset_verify()", 0, 0);
}

uint32
  arad_cnt_counters_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID         processor_ndx,
    SOC_SAND_IN  ARAD_CNT_COUNTERS_INFO        *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_COUNTERS_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(processor_ndx, ARAD_PROCESSOR_NDX_MAX, ARAD_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->src_type, ARAD_CNT_COUNTERS_INFO_SRC_TYPE_MAX, ARAD_CNT_SRC_TYPE_OUT_OF_RANGE_ERR, 11, exit);
  
  if((info->src_type >= ARAD_CNT_SRC_TYPE_EPNI_A) && (info->src_type <= ARAD_CNT_SRC_TYPE_EPNI_D))
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->mode_eg.type, ARAD_CNT_COUNTERS_INFO_MODE_EG_MAX, ARAD_CNT_MODE_EG_OUT_OF_RANGE_ERR, 12, exit);
    if( info->stag_lsb !=0)
    {
      SOC_SAND_SET_ERROR_CODE(ARAD_CNT_DEFINED_LSB_STAG_ERR, 19, exit);
    }
  }

  {
    if(info->src_type == ARAD_CNT_SRC_TYPE_VOQ)
    {
      SOC_SAND_ERR_IF_ABOVE_MAX(info->voq_cnt.q_set_size, ARAD_CNT_VOQ_PARAMS_Q_SET_SIZE_MAX, ARAD_CNT_Q_SET_SIZE_OUT_OF_RANGE_ERR , 23, exit);
      SOC_SAND_ERR_IF_ABOVE_MAX(info->voq_cnt.start_q, SOC_DPP_DEFS_GET(unit, nof_queues) - 1, ARAD_CNT_START_Q_OUT_OF_RANGE_ERR, 29, exit);
    }

    if( info->src_type == ARAD_CNT_SRC_TYPE_STAG)
    {
       SOC_SAND_ERR_IF_ABOVE_MAX(info->stag_lsb, ARAD_CNT_COUNTERS_INFO_STAG_MAX, ARAD_CNT_STAG_LSB_OUT_OF_RANGE_ERR, 37 , exit);
    }
    else if( info->stag_lsb !=0)
    {
      SOC_SAND_SET_ERROR_CODE(ARAD_CNT_DEFINED_LSB_STAG_ERR, 43, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_counters_set_verify()", 0, 0);
}

uint32
  arad_cnt_counters_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID         processor_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_COUNTERS_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(processor_ndx, ARAD_PROCESSOR_NDX_MAX, ARAD_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_counters_get_verify()", 0, 0);
}

/*********************************************************************
*     Configure the Counter Processor: its counting mode and
 *     its counting source.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_cnt_counters_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID         processor_ndx,
    SOC_SAND_OUT ARAD_CNT_COUNTERS_INFO        *info
  )

{
  uint32
	  data,
    res = SOC_SAND_OK,
    proc_id,
    fld_val2 = 0,
	is_configurable_mode = FALSE,
#ifdef BCM_88650_B0
    one_entry_cmd = 0,
#endif
    reg_val,
    fld_val;
  soc_reg_above_64_val_t
    reg_above_64;
  soc_reg_t
      iqm_cnt_comman_cfg_1_r[] = {IQM_CNT_COMMAN_CFG_1_Ar, IQM_CNT_COMMAN_CFG_1_Br, IQM_CNT_COMMAN_CFG_1_Cr, IQM_CNT_COMMAN_CFG_1_Dr},
	  base_val_reg;
  soc_field_t
      crps_iqm_cmd_src_type_f[] = {CRPS_IQM_CMD_SRC_TYPE_Af, CRPS_IQM_CMD_SRC_TYPE_Bf, CRPS_IQM_CMD_SRC_TYPE_Cf, CRPS_IQM_CMD_SRC_TYPE_Df},
      crps_iqm_cmd_queue_shift_f[] = {CRPS_IQM_CMD_QUEUE_SHIFT_Af, CRPS_IQM_CMD_QUEUE_SHIFT_Bf, CRPS_IQM_CMD_QUEUE_SHIFT_Cf, CRPS_IQM_CMD_QUEUE_SHIFT_Df},
      crps_iqm_cmd_base_q_f[] = {CRPS_IQM_CMD_BASE_Q_Af, CRPS_IQM_CMD_BASE_Q_Bf, CRPS_IQM_CMD_BASE_Q_Cf, CRPS_IQM_CMD_BASE_Q_Df},
	  base_val_fld,
	  size_fld_source;
  uint64
      field64;
  int core = 0; 

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_COUNTERS_GET_UNSAFE);

  

  SOC_SAND_CHECK_NULL_INPUT(info);

  ARAD_CNT_COUNTERS_INFO_clear(info);

#ifdef BCM_88650_B0
 if (!SOC_UNIT_NUM_VALID(unit)) {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 10, exit);
  }
#endif
  res = arad_cnt_get_processor_id(
          processor_ndx,
          &proc_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  READ_CRPS_CRPS_CNT_SRC_CFGr(unit, proc_id, &reg_val);
  fld_val = soc_reg_field_get(unit, CRPS_CRPS_CNT_SRC_CFGr, reg_val, CRPS_N_CNT_SRCf);

  READ_CRPS_CRPS_GENERAL_CFGr(unit, proc_id, &reg_val);
  info->format = soc_reg_field_get(unit, CRPS_CRPS_GENERAL_CFGr, reg_val, CRPS_N_COUNT_MODEf);

  switch(fld_val)
  {
  case ARAD_CNT_SRC_TYPE_EGR_PP_FLD_VAL:
    info->src_type = ARAD_CNT_SRC_TYPE_EGR_PP;
    break;

  case ARAD_CNT_SRC_TYPE_ING_PP_FLD_VAL:
    info->src_type = ARAD_CNT_SRC_TYPE_ING_PP;
    break;

  case ARAD_CNT_SRC_TYPE_VOQ_FLD_VAL:
  case ARAD_CNT_SRC_TYPE_STAG_FLD_VAL:
  case ARAD_CNT_SRC_TYPE_CNM_ID_FLD_VAL:
  case ARAD_CNT_SRC_TYPE_VSQ_FLD_VAL:
	SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  50,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_CNT_PROCESSOR_CONFIG_2r, SOC_CORE_ALL, 0, crps_iqm_cmd_src_type_f[proc_id], &fld_val2));
    info->src_type = ARAD_CNT_SRC_TYPE_VOQ + fld_val2;
    break;

  case ARAD_CNT_SRC_TYPE_ING_PP_MSB_FLD_VAL:
	  info->src_type = ARAD_CNT_SRC_TYPE_ING_PP_MSB;
	  break;

  case ARAD_CNT_SRC_TYPE_ING_PP_2_LSB_FLD_VAL:
	  info->src_type = ARAD_CNT_SRC_TYPE_ING_PP_2_LSB;
	break;

  case ARAD_CNT_SRC_TYPE_ING_PP_2_MSB_FLD_VAL:
	  info->src_type = ARAD_CNT_SRC_TYPE_ING_PP_2_MSB;
	break;

  case ARAD_CNT_SRC_TYPE_EG_PP_MSB_FLD_VAL:
	  info->src_type = ARAD_CNT_SRC_TYPE_EG_PP_MSB;
	break;

  case ARAD_CNT_SRC_TYPE_OAM_ING_A_FLD_VAL:
  case ARAD_CNT_SRC_TYPE_OAM_ING_B_FLD_VAL:
  case ARAD_CNT_SRC_TYPE_OAM_ING_C_FLD_VAL:
  case ARAD_CNT_SRC_TYPE_OAM_ING_D_FLD_VAL:
	  info->src_type = ARAD_CNT_SRC_TYPE_OAM_ING_A + (fld_val - ARAD_CNT_SRC_TYPE_OAM_ING_A_FLD_VAL);
	break;

  case ARAD_CNT_SRC_TYPE_EPNI_A_FLD_VAL:
  case ARAD_CNT_SRC_TYPE_EPNI_B_FLD_VAL:
  case ARAD_CNT_SRC_TYPE_EPNI_C_FLD_VAL:
  case ARAD_CNT_SRC_TYPE_EPNI_D_FLD_VAL:
	  info->src_type = ARAD_CNT_SRC_TYPE_EPNI_A + (fld_val - ARAD_CNT_SRC_TYPE_EPNI_A_FLD_VAL);
	break;

  default:
    SOC_SAND_SET_ERROR_CODE(ARAD_CNT_SRC_TYPE_OUT_OF_RANGE_ERR, 60, exit);
  }

  if((info->src_type >= ARAD_CNT_SRC_TYPE_EPNI_A) && (info->src_type <= ARAD_CNT_SRC_TYPE_EPNI_D))
  {
    
	  SOC_SAND_SOC_IF_ERROR_RETURN(res, 70, exit, READ_EPNI_PP_COUNTER_TABLEm(unit, EPNI_BLOCK(unit, core), 0, &data));

	  base_val_fld = ((info->src_type == ARAD_CNT_SRC_TYPE_EPNI_A) || (info->src_type == ARAD_CNT_SRC_TYPE_EPNI_B))? 
						  COUNTER_0_MODEf:COUNTER_1_MODEf;
	  fld_val = soc_EPNI_PP_COUNTER_TABLEm_field32_get(unit, &data, base_val_fld);

	  base_val_reg = ((info->src_type == ARAD_CNT_SRC_TYPE_EPNI_A) || (info->src_type == ARAD_CNT_SRC_TYPE_EPNI_B))? 
						  EPNI_COUNTER_0_BASEr:EPNI_COUNTER_1_BASEr;
	  base_val_fld = ((info->src_type == ARAD_CNT_SRC_TYPE_EPNI_A) || (info->src_type == ARAD_CNT_SRC_TYPE_EPNI_B))? 
						  COUNTER_0_MODE_0_BASEf:COUNTER_1_MODE_0_BASEf;

    switch (fld_val)
    {
    case ARAD_CNT_MODE_EG_TYPE_TM_FLD_VAL:
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  71,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_read(unit, base_val_reg, REG_PORT_ANY, 0, base_val_fld + (COUNTER_0_MODE_1_BASEf - COUNTER_0_MODE_0_BASEf), &field64));
      info->mode_eg.type = ARAD_CNT_MODE_EG_TYPE_TM;
      COMPILER_64_TO_32_LO(info->mode_eg.base_val, field64);
    break;
    
    case ARAD_CNT_MODE_EG_TYPE_OUTLIF_FLD_VAL:
	  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  72,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_read(unit, base_val_reg, REG_PORT_ANY, 0, base_val_fld + (COUNTER_0_MODE_3_BASEf - COUNTER_0_MODE_0_BASEf), &field64));
      info->mode_eg.type = ARAD_CNT_MODE_EG_TYPE_OUTLIF;
      info->mode_eg.base_val = fld_val;
    break;

    case ARAD_CNT_MODE_EG_TYPE_VSI_FLD_VAL:
      SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  73,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_read(unit, base_val_reg, REG_PORT_ANY, 0, base_val_fld + (COUNTER_0_MODE_2_BASEf - COUNTER_0_MODE_0_BASEf), &field64));
      info->mode_eg.type = ARAD_CNT_MODE_EG_TYPE_VSI;
      COMPILER_64_TO_32_LO(info->mode_eg.base_val, field64);
	  break;

    case ARAD_CNT_MODE_EG_TYPE_PMF_FLD_VAL:
	  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  74,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_read(unit, base_val_reg, REG_PORT_ANY, 0, base_val_fld + (COUNTER_0_MODE_0_BASEf - COUNTER_0_MODE_0_BASEf), &field64));
      info->mode_eg.type = ARAD_CNT_MODE_EG_TYPE_VSI;
      COMPILER_64_TO_32_LO(info->mode_eg.base_val, field64);
    break;
    }
  }

  /* get group size field*/
  res = _arad_cnt_group_size_field_by_src_type_get(unit, proc_id, info->src_type, &size_fld_source);
  SOC_SAND_CHECK_FUNC_RESULT(res, 75, exit);


  if (size_fld_source != INVALIDf) {
    SOC_SAND_SOC_IF_ERROR_RETURN(res, 76, exit, READ_CRPS_CRPS_CNT_SRC_GROUP_SIZESr(unit, reg_above_64));
    fld_val = soc_reg_above_64_field32_get(unit, CRPS_CRPS_CNT_SRC_GROUP_SIZESr, reg_above_64, size_fld_source);
  }
  else{
      /* special case that the src is not mapping to specific src_type field like OAM*/
      switch(info->src_type){
      case ARAD_CNT_SRC_TYPE_OAM_ING_A:
      case ARAD_CNT_SRC_TYPE_OAM_ING_B:
      case ARAD_CNT_SRC_TYPE_OAM_ING_C:
      case ARAD_CNT_SRC_TYPE_OAM_ING_D:
          fld_val = ARAD_CNT_MODE_ING_NO_COLOR_FLD_VAL;
          break;
      default:
          SOC_SAND_SET_ERROR_CODE(ARAD_CNT_ING_MODE_OUT_OF_RANGE_ERR, 77, exit);
      }
  }

/*get one entry mode field in case of Arad_B0 and above*/
#ifdef BCM_88650_B0
  if (SOC_IS_ARAD_B0_AND_ABOVE(unit) ){
	soc_field_t one_entry_fld_source = INVALIDf;

	res = _arad_cnt_one_entry_field_by_src_type_get(unit, proc_id, info->src_type, &one_entry_fld_source);
	SOC_SAND_CHECK_FUNC_RESULT(res, 232, exit);
    if(one_entry_fld_source != INVALIDf){
        SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  100,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, CRPS_CRPS_ONE_ENTRY_MODE_CNT_CMDr, REG_PORT_ANY, 0, one_entry_fld_source, &one_entry_cmd));
    }
  }
#endif

/* check if configurable mode*/
#ifdef BCM_88660_A0
  if (SOC_IS_ARADPLUS(unit)){
	soc_field_t do_not_count_field = INVALIDf;
	uint64 do_not_count_val;

    COMPILER_64_ZERO(do_not_count_val);
	/*get the do not count value to know if we are in configurable mode*/
	res = _arad_cnt_do_not_count_field_by_src_type_get(unit, proc_id, info->src_type, &do_not_count_field);
	SOC_SAND_CHECK_FUNC_RESULT(res, 233, exit);
    if(do_not_count_field != INVALIDf){
    	SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  234,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field64_read(unit, CRPS_CRPS_DO_NOT_COUNT_FILTERr, REG_PORT_ANY, 0, do_not_count_field, &do_not_count_val));
    	if (COMPILER_64_LO(do_not_count_val) == ARAD_CNT_DO_NOT_COUNT_VAL){
    	  is_configurable_mode = TRUE;
          info->mode_ing = ARAD_CNT_MODE_ING_CONFIGURABLE_OFFSETS;
          /*check if its legall val*/
          if ((fld_val >= 5) || (fld_val == 2)){
            SOC_SAND_SET_ERROR_CODE(ARAD_CNT_ING_MODE_OUT_OF_RANGE_ERR, 235, exit);
          }
          info->custom_mode_params.nof_counters = fld_val + 1;
        }
    }
  }
#endif 
  if(!is_configurable_mode){
    /* restorethe ing_mode from the group_size and one_entry mode regs*/
    switch(fld_val)
    {
    case ARAD_CNT_MODE_ING_NO_COLOR_FLD_VAL:
      info->mode_ing = ARAD_CNT_MODE_ING_NO_COLOR;
      break;

    case ARAD_CNT_MODE_ING_COLOR_RES_LOW_FLD_VAL:
      info->mode_ing = ARAD_CNT_MODE_ING_COLOR_RES_LOW;
      break;

    case ARAD_CNT_MODE_ING_COLOR_RES_HI_FLD_VAL:
      info->mode_ing = ARAD_CNT_MODE_ING_COLOR_RES_HI;
      break;

	case ARAD_CNT_MODE_ING_ONE_ENTRY_FLD_VAL:
	  info->mode_ing = ARAD_CNT_MODE_ING_ALL_NO_COLOR;
#ifdef BCM_88650_B0
      if (SOC_IS_ARAD_B0_AND_ABOVE(unit)) {
          if (one_entry_cmd == ARAD_CNT_ONE_ENTRY_MODE_ALL_VAL) {
              info->mode_ing = ARAD_CNT_MODE_ING_ALL_NO_COLOR;
          } else if (one_entry_cmd == ARAD_CNT_ONE_ENTRY_MODE_DROP_VAL) {
              info->mode_ing = ARAD_CNT_MODE_ING_DROP_NO_COLOR;
          } else if (one_entry_cmd == ARAD_CNT_ONE_ENTRY_MODE_FWD_VAL) {
              info->mode_ing = ARAD_CNT_MODE_ING_FWD_NO_COLOR;
          } else {
              info->mode_ing = ARAD_CNT_MODE_ING_ALL_NO_COLOR;
          }
      }
#endif
	  break;

	case ARAD_CNT_MODE_ING_COLOR_ENQ_HI_FLD_VAL:
	  info->mode_ing = ARAD_CNT_MODE_ING_COLOR_ENQ_HI;
#ifdef BCM_88660_A0
      /* green ,not green fwd/drop Arad plus*/
	  if(SOC_IS_ARADPLUS(unit)){
	    if (one_entry_cmd == ARAD_CNT_ONE_ENTRY_MODE_DROP_VAL) {
          info->mode_ing = ARAD_CNT_MODE_ING_DROP_SIMPLE_COLOR;
		} else if (one_entry_cmd == ARAD_CNT_ONE_ENTRY_MODE_FWD_VAL) {
            info->mode_ing = ARAD_CNT_MODE_ING_FWD_SIMPLE_COLOR;
		}
	  }
      break;
    /*packets and packets Arad plus*/
	case ARAD_CNT_MODE_ING_PKTS_AND_PKTS_FLD_VAL:
	  if(SOC_IS_ARAD_B1_AND_BELOW(unit)){
		SOC_SAND_SET_ERROR_CODE(ARAD_CNT_ING_MODE_OUT_OF_RANGE_ERR, 105, exit);
	  }
	  info->mode_ing = ARAD_CNT_MODE_ING_NO_COLOR;
#endif
	  break;

    default:
      SOC_SAND_SET_ERROR_CODE(ARAD_CNT_ING_MODE_OUT_OF_RANGE_ERR, 110, exit);
    }
  }

  if (info->src_type == ARAD_CNT_SRC_TYPE_VOQ)
  {
	  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  120,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_CNT_PROCESSOR_CONFIG_2r, SOC_CORE_ALL, 0, crps_iqm_cmd_queue_shift_f[proc_id], &info->voq_cnt.q_set_size));

	  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  130,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, iqm_cnt_comman_cfg_1_r[proc_id], REG_PORT_ANY, 0, crps_iqm_cmd_base_q_f[proc_id], &info->voq_cnt.start_q));
  }

  if( info->src_type == ARAD_CNT_SRC_TYPE_STAG )
  {
	SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  140,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, IQM_STATISTICS_TAG_CONFIGURATIONr, SOC_CORE_ALL, 0, STAT_TAG_CRPS_CMD_LSBf, &info->stag_lsb));
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_counters_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Get the current status of the counter processor.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_cnt_status_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID         processor_ndx,
    SOC_SAND_OUT ARAD_CNT_STATUS               *proc_status
  )
{
  uint32
    res = SOC_SAND_OK;
   
    
  uint32
    reg_val,
    fld_val;
  ARAD_CNT_COUNTERS_INFO
    counter_info;
  uint32
    cnt_ndx;
  soc_field_t
	  invalid_ad_acc_fld[ARAD_CNT_NOF_PROCESSOR_IDS] = {
		  CRPS_0_INVLD_PTR_ACCf, CRPS_1_INVLD_PTR_ACCf, CRPS_2_INVLD_PTR_ACCf, CRPS_3_INVLD_PTR_ACCf},
	  preread_fifo_full_fld[ARAD_CNT_NOF_PROCESSOR_IDS] = {
		  CRPS_0_PRE_READ_FIFO_FULLf, CRPS_1_PRE_READ_FIFO_FULLf, CRPS_2_PRE_READ_FIFO_FULLf, CRPS_3_PRE_READ_FIFO_FULLf};

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_STATUS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(proc_status);

  ARAD_CNT_STATUS_clear(proc_status);


  /*
   * Find if the id is valid
   */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  20,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, CRPS_INTERRUPT_REGISTERr, REG_PORT_ANY, 0, invalid_ad_acc_fld[processor_ndx], &fld_val));
  proc_status->is_cnt_id_invalid = SOC_SAND_NUM2BOOL(fld_val);
  
  /*
   * Read if the cache is full
   */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  30,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, CRPS_INTERRUPT_REGISTERr, REG_PORT_ANY, 0, preread_fifo_full_fld[processor_ndx], &fld_val));
  proc_status->is_cache_full = SOC_SAND_NUM2BOOL(fld_val);
  
  /*
   * Get the number of active counters
   */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  40,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, CRPS_CRPS_COUNTER_STS_1r, REG_PORT_ANY,  processor_ndx, FIELD_16_30f, &fld_val));
  proc_status->nof_active_cnts = fld_val;

  /*
   * Get the values of the last overflown counter
   */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  45,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, CRPS_CRPS_COUNTER_STS_1r, REG_PORT_ANY,  processor_ndx, CRPS_N_OVTH_CNTRS_CNTf, &fld_val));
  proc_status->overflow_cnt.is_overflow = SOC_SAND_NUM2BOOL(fld_val);

  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  46,  exit, ARAD_REG_ACCESS_ERR,READ_CRPS_CRPS_CNT_OVRF_STSr(unit,  processor_ndx, &reg_val));
  ARAD_FLD_FROM_REG(CRPS_CRPS_CNT_OVRF_STSr, CRPS_N_COUNTER_OVF_ADDRf, fld_val, reg_val, 50, exit);

  ARAD_CNT_COUNTERS_INFO_clear(&counter_info);
  res = arad_cnt_counters_get_unsafe(
    unit,
    processor_ndx,
    &counter_info
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  res = arad_cnt_id_compute(
    unit,
    counter_info,
    fld_val,
    &cnt_ndx
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  proc_status->overflow_cnt.last_cnt_id = cnt_ndx;

  ARAD_FLD_FROM_REG(CRPS_CRPS_CNT_OVRF_STSr, CRPS_N_IS_OVF_CNT_OCTSf, fld_val, reg_val, 50, exit);
  proc_status->overflow_cnt.is_byte_overflow = SOC_SAND_NUM2BOOL(fld_val);

  ARAD_FLD_FROM_REG(CRPS_CRPS_CNT_OVRF_STSr, CRPS_N_IS_OVF_CNT_PKTSf, fld_val, reg_val, 60, exit);
  proc_status->overflow_cnt.is_pckt_overflow = SOC_SAND_NUM2BOOL(fld_val);
  
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_status_get_unsafe()", 0, 0);
}

uint32
  arad_cnt_status_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID         processor_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_STATUS_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(processor_ndx, ARAD_PROCESSOR_NDX_MAX, ARAD_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_status_get_verify()", 0, 0);
}

/*********************************************************************
*     Read the counter values saved in a cache according to
 *     the polling algorithm (algorithmic method).
 *     Details: in the H file. (search for prototype)
 *     In wide mode (59 bits) the algorithmitic read is not used.  
*********************************************************************/
uint32
  arad_cnt_algorithmic_read_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID         processor_ndx,
    SOC_SAND_OUT ARAD_CNT_RESULT_ARR           *result_arr
  )
{
  uint32
    res = SOC_SAND_OK,
    nof_read_cnt,
    counters_2read=0,
    valid = 0,
    indx = 0;
  ARAD_CNT_COUNTERS_INFO
    counter_info;
  soc_reg_above_64_val_t
      fld_val,
      reg_val;
  
 
#if ARAD_CNT_USE_DMA
  uint32 *orig_entry,rval;
  int entry_words=0;
  int j, cnt;
  uint32	*host_buff, buff_index;

  int cmc;

#endif
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_ALGORITHMIC_READ_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(result_arr);
  
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  SOC_REG_ABOVE_64_CLEAR(fld_val);
  
  if (!SOC_UNIT_NUM_VALID(unit)) {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 4, exit);
  }
  res = arad_cnt_counters_get_verify(
          unit,
          processor_ndx
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  ARAD_CNT_COUNTERS_INFO_clear(&counter_info);

  res = handle_sand_result(arad_cnt_counters_get_unsafe(
      unit,
      processor_ndx,
      &counter_info
    ));
  SOC_SAND_SOC_CHECK_FUNC_RESULT(res, 7, exit);

  /* if wide mode return null values*/
  if ((counter_info.format == SOC_TMC_CNT_FORMAT_PKTS) || (counter_info.format == SOC_TMC_CNT_FORMAT_BYTES) || (counter_info.format == SOC_TMC_CNT_FORMAT_MAX_QUEUE_SIZE)) {
      SOC_SAND_EXIT_NO_ERROR;
  }

  /*read the counters*/
#if ARAD_CNT_USE_DMA
if (!SAL_BOOT_PLISIM ) {
   cmc = SOC_PCI_CMC(unit);

   rval = soc_pci_read(unit, CMIC_CMCx_FIFO_CHy_RD_DMA_STAT_OFFSET(cmc, ch[processor_ndx]));
   arad_sw_db_cnt_buff_and_index_get(unit, processor_ndx, &host_buff, &buff_index);

  LOG_DEBUG(BSL_LS_SOC_STAT,
            (BSL_META_U(unit,
                        "DMA_STAT_OFFSET=%x (%d)\n\n"),  rval, rval));
     
   res = _soc_mem_sbus_fifo_dma_get_num_entries(unit, ch[processor_ndx], &cnt);
   entry_words = BYTES2WORDS(soc_reg_bytes(unit, CRPS_CRPS_CACHE_RD_RESULTSr));

   counters_2read = cnt;
}
#else
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  8,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_read(unit, CRPS_CRPS_PRE_RD_FIFO_STS_1r, REG_PORT_ANY,  processor_ndx, CRPS_N_CACHE_STATUS_CNTf, &counters_2read));
#endif
  SOC_SAND_ERR_IF_ABOVE_MAX(counters_2read, (ARAD_CNT_CACHE_LENGTH - 1), ARAD_CNT_CACHE_LENGTH_OUT_OF_RANGE_ERR, 9, exit);

  nof_read_cnt = 0; 
  for(indx = 0;  indx < counters_2read;  indx++)
  {
#if ARAD_CNT_USE_DMA
    if (!SAL_BOOT_PLISIM ) {
      orig_entry = host_buff + (buff_index * entry_words);
      for(j = 0; j < 3; j++) {
        reg_val[j] = orig_entry[j];
      }
      LOG_VERBOSE(BSL_LS_SOC_STAT,
                  (BSL_META_U(unit,
                              "algorithmic_read:proc=%d curr_line=%x val=%.8x %.8x %.8x\n"),  
                              processor_ndx,
                   buff_index,
                   (unsigned int)(reg_val[0]),(unsigned int)reg_val[1],(unsigned int)reg_val[2]));

      buff_index ++;
      if (buff_index >= ARAD_CNT_CACHE_LENGTH) {	 
         buff_index = 0;
      }
    }
#else
    READ_CRPS_CRPS_CACHE_RD_RESULTSr(unit, processor_ndx, reg_val);
#endif
    ARAD_FLD_FROM_REG_ABOVE_64(CRPS_CRPS_CACHE_RD_RESULTSr, CRPS_N_CACHE_RD_DATA_VALIDf, fld_val, reg_val, 10, exit);
    valid = fld_val[0];

    if(valid){
      /*if valid copy the counter to the results array*/
      ARAD_FLD_FROM_REG_ABOVE_64(CRPS_CRPS_CACHE_RD_RESULTSr, CRPS_N_CACHE_RD_PKT_CNTf, fld_val, reg_val, 11, exit);
      result_arr->cnt_result[nof_read_cnt].pkt_cnt = fld_val[0];
      ARAD_FLD_FROM_REG_ABOVE_64(CRPS_CRPS_CACHE_RD_RESULTSr, CRPS_N_CACHE_RD_OCT_CNTf, fld_val, reg_val, 12, exit);
      COMPILER_64_SET(result_arr->cnt_result[nof_read_cnt].byte_cnt, fld_val[1], fld_val[0]);
      ARAD_FLD_FROM_REG_ABOVE_64(CRPS_CRPS_CACHE_RD_RESULTSr, CRPS_N_CACHE_RD_CNT_ADDRf, fld_val, reg_val, 13, exit);
      result_arr->cnt_result[nof_read_cnt].counter_id = fld_val[0];

      nof_read_cnt++;
    }
  }
  /*save the number of entries read*/
  result_arr->nof_counters = nof_read_cnt;
  
#if ARAD_CNT_USE_DMA
if (!SAL_BOOT_PLISIM ) {
  (void)_soc_mem_sbus_fifo_dma_set_entries_read(unit, ch[processor_ndx], nof_read_cnt);
  arad_sw_db_cnt_buff_and_index_set(unit, processor_ndx, host_buff, buff_index);
}
#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_algorithmic_read_unsafe()", 0, 0);
}


uint32
  arad_cnt_algorithmic_read_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID         processor_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_ALGORITHMIC_READ_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(processor_ndx, ARAD_PROCESSOR_NDX_MAX, ARAD_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_algorithmic_read_verify()", 0, 0);
}

/*********************************************************************
*     Read the counter value according to the counter.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_cnt_direct_read_unsafe(
    SOC_SAND_IN  int                         unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID          processor_ndx,
    SOC_SAND_IN  uint32                         counter_ndx,
    SOC_SAND_OUT ARAD_CNT_RESULT                *read_rslt
  )
{
  uint32
    res = SOC_SAND_OK;

  ARAD_CNT_COUNTERS_INFO
    counter_info;
  uint32
     cnt_ndx = 0;


 soc_reg_above_64_val_t 
      reg_val, valid_bit, oct_cnt, pkt_cnt;

  

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_DIRECT_READ_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(read_rslt);

 
  SOC_REG_ABOVE_64_CLEAR(reg_val);
  SOC_REG_ABOVE_64_CLEAR(valid_bit);
  SOC_REG_ABOVE_64_CLEAR(oct_cnt);
  SOC_REG_ABOVE_64_CLEAR(pkt_cnt);
  ARAD_CNT_RESULT_clear(read_rslt);

  if (!SOC_UNIT_NUM_VALID(unit)) {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ILLEGAL_DEVICE_ID, 4, exit);
  }

   res = arad_cnt_counters_get_verify(
    unit,
    processor_ndx
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  ARAD_CNT_COUNTERS_INFO_clear(&counter_info);
  res = arad_cnt_counters_get_unsafe(
    unit,
    processor_ndx,
    &counter_info
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 6, exit);

  /*
   * Step 1. : Trigger on
   */
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  12,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CRPS_CNT_PROCESSOR_READr, REG_PORT_ANY,  processor_ndx, CRPS_N_RD_REQ_TYPEf,  ARAD_CNT_READ_DIRECT_READ));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  14,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CRPS_CNT_PROCESSOR_READr, REG_PORT_ANY,  processor_ndx, CRPS_N_RD_REQ_ADDRf,  counter_ndx));
  SOC_SAND_SOC_IF_ERROR_RETURN_ERR_VAL(res,  10,  exit, ARAD_REG_ACCESS_ERR,soc_reg_above_64_field32_modify(unit, CRPS_CNT_PROCESSOR_READr, REG_PORT_ANY,  processor_ndx, CRPS_N_RD_REQf,  0x1));

 /*
  * Step 2. Read the counter value if valid
  */
  
  SOC_SAND_SOC_IF_ERROR_RETURN(res, 20, exit, READ_CRPS_CRPS_DIRECT_RD_RESULTr(unit, processor_ndx, reg_val));
  

  soc_reg_above_64_field_get(unit, CRPS_CRPS_DIRECT_RD_RESULTr, reg_val, CRPS_N_DIRECT_RD_DATA_VALIDf, valid_bit);
  if (valid_bit[0]) {
      soc_reg_above_64_field_get(unit, CRPS_CRPS_DIRECT_RD_RESULTr, reg_val, CRPS_N_DIRECT_RD_PKT_CNTf, pkt_cnt);
      read_rslt->pkt_cnt = pkt_cnt[0];

      soc_reg_above_64_field_get(unit, CRPS_CRPS_DIRECT_RD_RESULTr, reg_val, CRPS_N_DIRECT_RD_OCT_CNTf, oct_cnt);
      COMPILER_64_SET(read_rslt->byte_cnt, oct_cnt[1], oct_cnt[0]);
  }
  res = arad_cnt_id_compute(
    unit,
    counter_info,
    counter_ndx,
    &cnt_ndx
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  read_rslt->counter_id = cnt_ndx;
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_direct_read_unsafe()", 0, counter_ndx);
}

uint32
  arad_cnt_direct_read_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID         processor_ndx,
    SOC_SAND_IN  uint32                    counter_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_DIRECT_READ_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(processor_ndx, ARAD_PROCESSOR_NDX_MAX, ARAD_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(counter_ndx, ARAD_COUNTER_NDX_MAX, ARAD_COUNTER_NDX_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_direct_read_verify()", 0, counter_ndx);
}

/*********************************************************************
*     Convert a queue index to the counter index.
*     The computation of the counter_ndx is: f((queue_ndx - start_q)/q_set_size))
*     where f is the counter id compute which compute the id according to the ingress mode.
*********************************************************************/
uint32
  arad_cnt_q2cnt_id_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID         processor_ndx,
    SOC_SAND_IN  uint32                   queue_ndx,
    SOC_SAND_OUT uint32                   *counter_ndx
  )
{
  uint32
    q_set_size_val = 1,
    res = SOC_SAND_OK;
  ARAD_CNT_COUNTERS_INFO
    info;
  uint32
    counter_id,
    fld_val;


  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_Q2CNT_ID_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(counter_ndx);
  
  res = arad_cnt_counters_get_unsafe(
          unit,
          processor_ndx,
          &info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if(info.src_type != ARAD_CNT_SRC_TYPE_VOQ)
  {
    SOC_SAND_SET_ERROR_CODE(ARAD_CNT_PROC_SRC_TYPE_NOT_VOQ_ERR, 20, exit);
  }

  arad_cnt_convert_enum_val2_q_set_size(
    info.voq_cnt.q_set_size,
    &q_set_size_val);

   counter_id = ((queue_ndx - info.voq_cnt.start_q)/q_set_size_val);
  
   res = arad_cnt_id_compute(
     unit,
     info,
     counter_id,
     &fld_val);
   SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
   *counter_ndx = fld_val;
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_q2cnt_id_unsafe()", queue_ndx, *counter_ndx);
}

uint32
  arad_cnt_q2cnt_id_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  ARAD_CNT_PROCESSOR_ID         processor_ndx,
    SOC_SAND_IN  uint32                   queue_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_Q2CNT_ID_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(queue_ndx, SOC_DPP_DEFS_GET(unit, nof_queues) - 1, ARAD_QUEUE_NDX_OUT_OF_RANGE_ERR, 10, exit);

  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_q2cnt_id_verify()", queue_ndx, 0);
}


/*********************************************************************
*     Configure the header compensation factor for the counter
 *     and meter modules to enable a meter and counter
 *     processing of this packet according to its original
 *     packet size.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_cnt_meter_hdr_compensation_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core_id,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  ARAD_PORT_DIRECTION      direction,
    SOC_SAND_IN  int32                    hdr_compensation
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_IHB_PINFO_LBP_TBL_DATA
    tbl_data_ingress;
  
  int32
    fld_val;
  uint32
    data[20];

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_METER_HDR_COMPENSATION_SET_UNSAFE);

  if (direction == SOC_TMC_PORT_DIRECTION_INCOMING) {
      if (hdr_compensation < 0)
      {
        fld_val = hdr_compensation + 64;
      }
      else
      {
        fld_val = hdr_compensation;
      }
      res = arad_ihb_pinfo_lbp_tbl_get_unsafe(
              unit,
              core_id,
              port_ndx,
              &tbl_data_ingress
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 113, exit);

      tbl_data_ingress.counter_compension = fld_val;
      
      res = arad_ihb_pinfo_lbp_tbl_set_unsafe(
              unit,
              core_id,
              port_ndx,
              &tbl_data_ingress
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 113, exit);
  } else {
      if (hdr_compensation < 0)
      {
        fld_val = hdr_compensation + 128;
      }
      else
      {
        fld_val = hdr_compensation;
      }
      
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 1000, exit, READ_EGQ_PP_PPCTm(unit, MEM_BLOCK_ANY, port_ndx, data));

      soc_EGQ_PP_PPCTm_field32_set(unit, data, COUNTER_COMPENSATIONf, fld_val);

      SOC_SAND_SOC_IF_ERROR_RETURN(res, 1010, exit, WRITE_EGQ_PP_PPCTm(unit, MEM_BLOCK_ANY, port_ndx, data));

 
      SOC_SAND_SOC_IF_ERROR_RETURN(res, 1020, exit, READ_EPNI_PP_COUNTER_TABLEm(unit, EPNI_BLOCK(unit, core_id), port_ndx, data));

      soc_EPNI_PP_COUNTER_TABLEm_field32_set(unit, data, COUNTER_COMPENSATIONf, fld_val);

      SOC_SAND_SOC_IF_ERROR_RETURN(res, 1030, exit, WRITE_EPNI_PP_COUNTER_TABLEm(unit, EPNI_BLOCK(unit, core_id), port_ndx, data));
    



  }


  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_meter_hdr_compensation_set_unsafe()", port_ndx, 0);
}

uint32
  arad_cnt_meter_hdr_compensation_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  ARAD_PORT_DIRECTION      direction,
    SOC_SAND_IN  int32                    hdr_compensation
  )
{
  

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_METER_HDR_COMPENSATION_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_ndx, ARAD_CNT_PORT_NDX_MAX, ARAD_PORT_NDX_OUT_OF_RANGE_ERR, 10, exit);
  if (direction == SOC_TMC_PORT_DIRECTION_INCOMING) {
      SOC_SAND_ERR_IF_OUT_OF_RANGE(hdr_compensation, ARAD_HDR_COMPENSATION_INGRESS_MIN, ARAD_HDR_COMPENSATION_INGRESS_MAX, ARAD_HDR_IN_COMPENSATION_OUT_OF_RANGE_ERR, 20, exit);
  } else {
      SOC_SAND_ERR_IF_OUT_OF_RANGE(hdr_compensation, ARAD_HDR_COMPENSATION_EGRESS_MIN, ARAD_HDR_COMPENSATION_EGRESS_MAX, ARAD_HDR_OUT_COMPENSATION_OUT_OF_RANGE_ERR, 20, exit);
  }
  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_meter_hdr_compensation_set_verify()", port_ndx, 0);
}

uint32
  arad_cnt_meter_hdr_compensation_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  ARAD_PORT_DIRECTION      direction

  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_METER_HDR_COMPENSATION_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_ndx, ARAD_CNT_PORT_NDX_MAX, ARAD_PORT_NDX_OUT_OF_RANGE_ERR, 10, exit);

  ARAD_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_meter_hdr_compensation_get_verify()", port_ndx, 0);
}

/*********************************************************************
*     Configure the header compensation factor for the counter
 *     and meter modules to enable a meter and counter
 *     processing of this packet according to its original
 *     packet size.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  arad_cnt_meter_hdr_compensation_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  int                   core_id,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  ARAD_PORT_DIRECTION      direction,
    SOC_SAND_OUT int32                    *hdr_compensation
  )
{
  uint32
    res = SOC_SAND_OK;
  ARAD_IHB_PINFO_LBP_TBL_DATA
    tbl_data;
  int32
    fld_val;
  uint32
    data[20];

 SOC_SAND_INIT_ERROR_DEFINITIONS(ARAD_CNT_METER_HDR_COMPENSATION_GET_UNSAFE);

 SOC_SAND_CHECK_NULL_INPUT(hdr_compensation);
  
 if (direction == SOC_TMC_PORT_DIRECTION_INCOMING) {

     res = arad_ihb_pinfo_lbp_tbl_get_unsafe(
          unit,
          core_id,
          port_ndx,
          &tbl_data
        );
     SOC_SAND_CHECK_FUNC_RESULT(res, 113, exit);

     if (tbl_data.counter_compension >= 32)
     {
         fld_val = tbl_data.counter_compension - 64;
     }
     else
     {
         fld_val = tbl_data.counter_compension;
     }
 } else {
     SOC_SAND_SOC_IF_ERROR_RETURN(res, 1000, exit, READ_EGQ_PP_PPCTm(unit, MEM_BLOCK_ANY, port_ndx, data));

     fld_val = soc_EGQ_PP_PPCTm_field32_get(unit, data, COUNTER_COMPENSATIONf);

     if (fld_val >= 64)
     {
         fld_val = fld_val - 128;
     }
    
 }
  *hdr_compensation = fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in arad_cnt_meter_hdr_compensation_get_unsafe()", port_ndx, 0);
}

#if ARAD_DEBUG_IS_LVL1

uint32
  ARAD_CNT_VOQ_PARAMS_verify(
    SOC_SAND_IN  ARAD_CNT_VOQ_PARAMS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->q_set_size, ARAD_CNT_VOQ_PARAMS_Q_SET_SIZE_MAX, ARAD_CNT_Q_SET_SIZE_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_CNT_VOQ_PARAMS_verify()",0,0);
}

uint32
  ARAD_CNT_COUNTERS_INFO_verify(
    SOC_SAND_IN  ARAD_CNT_COUNTERS_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->src_type, ARAD_CNT_COUNTERS_INFO_SRC_TYPE_MAX, ARAD_CNT_SRC_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->mode_ing, ARAD_CNT_COUNTERS_INFO_MODE_ING_MAX, ARAD_CNT_MODE_ING_OUT_OF_RANGE_ERR, 11, exit);
  
  SOC_SAND_ERR_IF_ABOVE_MAX(info->stag_lsb, ARAD_CNT_COUNTERS_INFO_STAG_MAX, ARAD_CNT_STAG_LSB_OUT_OF_RANGE_ERR , 13, exit);
  ARAD_STRUCT_VERIFY(ARAD_CNT_VOQ_PARAMS, &(info->voq_cnt), 14, exit);
  ARAD_STRUCT_VERIFY(ARAD_CNT_MODE_EG, &(info->mode_eg), 14, exit);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_CNT_COUNTERS_INFO_verify()",0,0);
}

uint32
  ARAD_CNT_OVERFLOW_verify(
    SOC_SAND_IN  ARAD_CNT_OVERFLOW *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);


  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_CNT_OVERFLOW_verify()",0,0);
}

uint32
  ARAD_CNT_STATUS_verify(
    SOC_SAND_IN  ARAD_CNT_STATUS *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  ARAD_STRUCT_VERIFY(ARAD_CNT_OVERFLOW, &(info->overflow_cnt), 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_CNT_STATUS_verify()",0,0);
}

uint32
  ARAD_CNT_RESULT_verify(
    SOC_SAND_IN  ARAD_CNT_RESULT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);


  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_CNT_RESULT_verify()",0,0);
}

uint32
  ARAD_CNT_RESULT_ARR_verify(
    SOC_SAND_IN  ARAD_CNT_RESULT_ARR *info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    ind;


  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < ARAD_CNT_CACHE_LENGTH; ++ind)
  {
    ARAD_STRUCT_VERIFY(ARAD_CNT_RESULT, &(info->cnt_result[ind]), 10, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(info->nof_counters, ARAD_CNT_RESULT_ARR_NOF_COUNTERS_MAX, ARAD_CNT_NOF_COUNTERS_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in ARAD_CNT_RESULT_ARR_verify()",0,0);
}

#endif /* ARAD_DEBUG_IS_LVL1 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>


#endif /* of #if defined(BCM_88650_A0) */

