/* $Id: pb_cnt.c,v 1.8 Broadcom SDK $
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

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/Petra/PB_TM/pb_api_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_cnt.h>
#include <soc/dpp/Petra/PB_TM/pb_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_api_ports.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_PROCESSOR_NDX_MAX                                   (SOC_PB_CNT_NOF_PROCESSOR_IDS-1)
#define SOC_PB_COUNTER_NDX_MAX                                     (8*1024-1)
#define SOC_PB_CNT_QUEUE_NDX_MAX                                   (32*1024-1)
#define SOC_PB_CNT_PORT_NDX_MAX                                    (63)
#define SOC_PB_HDR_COMPENSATION_MIN                                (-31)
#define SOC_PB_HDR_COMPENSATION_MAX                                (31)
#define SOC_PB_CNT_VOQ_PARAMS_Q_SET_SIZE_MAX                       (SOC_PB_CNT_NOF_Q_SET_SIZES-1)
#define SOC_PB_CNT_TYPE_MAX                                        (SOC_PB_CNT_NOF_MODE_EG_TYPES-1)
#define SOC_PB_CNT_COUNTERS_INFO_SRC_TYPE_MAX                      (SOC_PB_CNT_NOF_SRC_TYPES-1)
#define SOC_PB_CNT_COUNTERS_INFO_MODE_ING_MAX                      (SOC_PB_CNT_NOF_MODE_INGS-1)
#define SOC_PB_CNT_COUNTERS_INFO_MODE_EG_MAX                       (SOC_PB_CNT_NOF_MODE_EGS-1)
#define SOC_PB_CNT_BASE_VAL_MAX                                    (8*1024-1)
#define SOC_PB_CNT_COUNTERS_INFO_STAG_MAX                          (15)
#define SOC_PB_CNT_RESULT_ARR_NOF_COUNTERS_MAX                     (16)

#define SOC_PB_CNT_PROCESSOR_ID_A_FLD_VAL                          (0)
#define SOC_PB_CNT_PROCESSOR_ID_B_FLD_VAL                          (1)

#define SOC_PB_CNT_MODE_STATISTICS_NO_COLOR_FLD_VAL                       (0)
#define SOC_PB_CNT_MODE_STATISTICS_COLOR_RES_LOW_FLD_VAL                  (1)
#define SOC_PB_CNT_MODE_STATISTICS_COLOR_RES_HI_FLD_VAL                   (2)


#define SOC_PB_CNT_MODE_EG_RES_NO_COLOR_FLD_VAL                        (0)
#define SOC_PB_CNT_MODE_EG_RES_COLOR_FLD_VAL                           (1)

#define SOC_PB_CNT_MODE_EG_TYPE_TM_FLD_VAL                             (0)
#define SOC_PB_CNT_MODE_EG_TYPE_VSI_FLD_VAL                           (1)
#define SOC_PB_CNT_MODE_EG_TYPE_OUTLIF_FLD_VAL                          (2)
 

#define SOC_PB_CNT_SRC_TYPE_ING_PP_FLD_VAL                         (0)
#define SOC_PB_CNT_SRC_TYPE_VOQ_FLD_VAL                            (1)
#define SOC_PB_CNT_SRC_TYPE_STAG_FLD_VAL                           (2)
#define SOC_PB_CNT_SRC_TYPE_VSQ_FLD_VAL                            (3)
#define SOC_PB_CNT_SRC_TYPE_CNM_ID_FLD_VAL                         (4)
#define SOC_PB_CNT_SRC_TYPE_EGR_PP_FLD_VAL                         (5)


#define SOC_PB_CNT_Q_SET_SIZE_1_Q_FLD_VAL                          (0)
#define SOC_PB_CNT_Q_SET_SIZE_2_Q_FLD_VAL                          (1)
#define SOC_PB_CNT_Q_SET_SIZE_4_Q_FLD_VAL                          (2)
#define SOC_PB_CNT_Q_SET_SIZE_8_Q_FLD_VAL                          (3)

#define SOC_PB_CNT_PKT_TRESH                                       (0x1E40) /* 2^21 */
#define SOC_PB_CNT_OCT_TRESH                                       (0x7001) /* 2^28 */
#define SOC_PB_CNT_PREFETCH_TMR_CFG                                (10)
#define SOC_PB_CNT_INIT_WAIT_ITERATIONS                            (50)
#define SOC_PB_CNT_INIT_TIMER_ITERATIONS                           (100)
#define SOC_PB_CNT_INIT_TIMER_DELAY_MSEC                           (1)
#define SOC_PB_CNT_READ_DIRECT_READ                                (3)

/* $Id: pb_cnt.c,v 1.8 Broadcom SDK $
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

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */


STATIC uint32 soc_convert_enum_val2_q_set_size(
        SOC_SAND_IN SOC_PB_CNT_Q_SET_SIZE enum_q_set_size,
        SOC_SAND_OUT uint32 *q_set_size_val
        )
{ 
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  switch (enum_q_set_size)
  {
    case SOC_PB_CNT_Q_SET_SIZE_1_Q:

      *q_set_size_val = 1;
      break;
        
    case SOC_PB_CNT_Q_SET_SIZE_2_Q:
      *q_set_size_val = 2;
      break;

    case SOC_PB_CNT_Q_SET_SIZE_4_Q:
      *q_set_size_val = 4;
      break;

    case SOC_PB_CNT_Q_SET_SIZE_8_Q:
      *q_set_size_val = 8;
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_CNT_Q_SET_SIZE_OUT_OF_RANGE_ERR, 10, exit);
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_convert_enum_val2_q_set_size()", 0, 0);
}

/*********************************************************************
* NAME:
*     soc_pb_cnt_init
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
  soc_pb_cnt_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNT_INIT);

  regs = soc_petra_regs();
  
  /*
   *  Enable the Counter Processors
   */
  SOC_PB_FLD_SET(regs->iqm.iqm_init_reg.crps_init[SOC_PB_CNT_PROCESSOR_ID_A], 0, 10, exit);
  SOC_PB_FLD_SET(regs->iqm.iqm_init_reg.crps_init[SOC_PB_CNT_PROCESSOR_ID_B],0, 11,exit);
  SOC_PB_FLD_SET(regs->iqm.iqm_enablers_reg.crps_cmd_en[SOC_PB_CNT_PROCESSOR_ID_A],1,13,exit);
  SOC_PB_FLD_SET(regs->iqm.iqm_enablers_reg.crps_cmd_en[SOC_PB_CNT_PROCESSOR_ID_B],1,17,exit);
  
  /*
   *  Enable the interrupt register mask
   */
  SOC_PB_FLD_SET(regs->iqm.crps_interrupt_register_mask_reg.crps_cnt_ovf_mask[SOC_PB_CNT_PROCESSOR_ID_A],1,19,exit);
  SOC_PB_FLD_SET(regs->iqm.crps_interrupt_register_mask_reg.crps_cnt_ovf_mask[SOC_PB_CNT_PROCESSOR_ID_B],1,23,exit);
  SOC_PB_FLD_SET(regs->iqm.crps_interrupt_register_mask_reg.crps_invld_ad_acc_mask[SOC_PB_CNT_PROCESSOR_ID_A],1,29,exit);
  SOC_PB_FLD_SET(regs->iqm.crps_interrupt_register_mask_reg.crps_invld_ad_acc_mask[SOC_PB_CNT_PROCESSOR_ID_B],1,31,exit);
  SOC_PB_FLD_SET(regs->iqm.crps_interrupt_register_mask_reg.crps_invld_ptr_acc_mask[SOC_PB_CNT_PROCESSOR_ID_A],1,37,exit);
  SOC_PB_FLD_SET(regs->iqm.crps_interrupt_register_mask_reg.crps_invld_ptr_acc_mask[SOC_PB_CNT_PROCESSOR_ID_B],1,41,exit);
  SOC_PB_FLD_SET(regs->iqm.crps_interrupt_register_mask_reg.crps_pre_read_fifo_full_mask[SOC_PB_CNT_PROCESSOR_ID_A],1,43,exit);
  SOC_PB_FLD_SET(regs->iqm.crps_interrupt_register_mask_reg.crps_pre_read_fifo_full_mask[SOC_PB_CNT_PROCESSOR_ID_B],1,53,exit);

  SOC_PB_FLD_SET(regs->iqm.crps_interrupt_register_mask_reg.crps_pre_read_fifo_not_empty_mask[SOC_PB_CNT_PROCESSOR_ID_A],1,59,exit);
  SOC_PB_FLD_SET(regs->iqm.crps_interrupt_register_mask_reg.crps_pre_read_fifo_not_empty_mask[SOC_PB_CNT_PROCESSOR_ID_B],1,61,exit);
  SOC_PB_FLD_SET(regs->iqm.crps_interrupt_register_mask_reg.crps_pre_read_fifo_not_empty_mask[SOC_PB_CNT_PROCESSOR_ID_A],1,67,exit);
  SOC_PB_FLD_SET(regs->iqm.crps_interrupt_register_mask_reg.crps_pre_read_fifo_not_empty_mask[SOC_PB_CNT_PROCESSOR_ID_B],1,71,exit);
  
  SOC_PB_FLD_SET(regs->iqm.crps_interrupt_register_mask_reg.crps_direct_rd_when_waiting_mask[SOC_PB_CNT_PROCESSOR_ID_A],1,73,exit);
  SOC_PB_FLD_SET(regs->iqm.crps_interrupt_register_mask_reg.crps_direct_rd_when_waiting_mask[SOC_PB_CNT_PROCESSOR_ID_B],1,83,exit);
  SOC_PB_FLD_SET(regs->iqm.crps_interrupt_register_mask_reg.crps_static_rd_when_waiting_mask[SOC_PB_CNT_PROCESSOR_ID_A],1,89,exit);
  SOC_PB_FLD_SET(regs->iqm.crps_interrupt_register_mask_reg.crps_static_rd_when_waiting_mask[SOC_PB_CNT_PROCESSOR_ID_B],1,91,exit);
  
  /*
   *  Enable the cash reading and configuring the fetch time (10 ms)
   *  the considerations for choosing 10ms are : implementation of the LRT algorithm from-
   *  Efficient Implementation of a Statistics Counter Architecture by Varghese. The parameters
   *  used were  the counter size = 27 bit and the number of counters 16K.
   */
  SOC_PB_FLD_SET(regs->iqm.cntprocessor_config1_reg.crps_cache_read_en[SOC_PB_CNT_PROCESSOR_ID_A], 0x1, 101,exit);
  SOC_PB_FLD_SET(regs->iqm.cntprocessor_config1_reg.crps_cache_read_en[SOC_PB_CNT_PROCESSOR_ID_B], 0x1, 103,exit);
  SOC_PB_FLD_SET(regs->iqm.cntprocessor_config1_reg.crps_prefetch_tmr_cfg[SOC_PB_CNT_PROCESSOR_ID_A],SOC_PB_CNT_PREFETCH_TMR_CFG,107,exit);
  SOC_PB_FLD_SET(regs->iqm.cntprocessor_config1_reg.crps_prefetch_tmr_cfg[SOC_PB_CNT_PROCESSOR_ID_B],SOC_PB_CNT_PREFETCH_TMR_CFG,109,exit);
  
  /*
   *  Set the counter thresholds to a fixed value
   */
  SOC_PB_FLD_SET(regs->iqm.cnt_processor_config_reg[SOC_PB_CNT_PROCESSOR_ID_A].crps_pkt_cnt_thresh, SOC_PB_CNT_PKT_TRESH, 119,exit);
  SOC_PB_FLD_SET(regs->iqm.cnt_processor_config_reg[SOC_PB_CNT_PROCESSOR_ID_B].crps_pkt_cnt_thresh, SOC_PB_CNT_PKT_TRESH,123,exit);
  SOC_PB_FLD_SET(regs->iqm.cnt_processor_config_reg[SOC_PB_CNT_PROCESSOR_ID_A].crps_oct_cnt_thresh, SOC_PB_CNT_OCT_TRESH,129,exit);
  SOC_PB_FLD_SET(regs->iqm.cnt_processor_config_reg[SOC_PB_CNT_PROCESSOR_ID_B].crps_oct_cnt_thresh, SOC_PB_CNT_OCT_TRESH,131,exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_cnt_init()", 0, 0);
}

STATIC
  void
    soc_pb_cnt_id_compute(
      SOC_SAND_IN  SOC_PB_CNT_COUNTERS_INFO counter_info,
      SOC_SAND_IN  uint32             counter_ndx,
      SOC_SAND_OUT uint32            *real_counter_id
      )
{
  uint32
    cnt_ndx=0;

  if (counter_info.src_type == SOC_PB_CNT_SRC_TYPE_EGR_PP)
  {
    if (counter_info.mode_eg.resolution == SOC_PB_CNT_MODE_EG_RES_NO_COLOR)
    {
      cnt_ndx = counter_ndx;
    }
    else
    {
      cnt_ndx = counter_ndx / 2;
    }
  }
  else
  {
    switch(counter_info.mode_statistics)
    {
    case SOC_PB_CNT_MODE_STATISTICS_NO_COLOR:
      cnt_ndx = counter_ndx * 2;
      break;

    case SOC_PB_CNT_MODE_STATISTICS_COLOR_RES_LOW:
      cnt_ndx = counter_ndx * 4;
      break;

    case SOC_PB_CNT_MODE_STATISTICS_COLOR_RES_HI:
      cnt_ndx = counter_ndx * 5;
      break;

    default:
      break;
    }
  }

  *real_counter_id = cnt_ndx;
}

uint32
SOC_PB_CNT_MODE_EG_verify(
  SOC_SAND_IN  SOC_PB_CNT_MODE_EG *info
   )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->type, SOC_PB_CNT_TYPE_MAX, SOC_PB_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->base_val, SOC_PB_CNT_BASE_VAL_MAX, SOC_PB_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 12, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_CNT_MODE_EG_verify()",0,0);
}
STATIC
  uint32
    soc_pb_cnt_get_source_id(
      SOC_SAND_IN  SOC_PB_CNT_SRC_TYPE src_type,
      SOC_SAND_OUT uint32       *src
    )
{
  uint32
    fld_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNT_GET_SOURCE_ID);
  
  switch(src_type)
  {
  case SOC_PB_CNT_SRC_TYPE_ING_PP:
    fld_val = SOC_PB_CNT_SRC_TYPE_ING_PP_FLD_VAL;
    break;
  
  case SOC_PB_CNT_SRC_TYPE_VOQ:
    fld_val = SOC_PB_CNT_SRC_TYPE_VOQ_FLD_VAL;
    break;
  
  case SOC_PB_CNT_SRC_TYPE_STAG:
     fld_val = SOC_PB_CNT_SRC_TYPE_STAG_FLD_VAL;
     break;
 
  case SOC_PB_CNT_SRC_TYPE_VSQ:
     fld_val = SOC_PB_CNT_SRC_TYPE_VSQ_FLD_VAL;
     break;
  
  case SOC_PB_CNT_SRC_TYPE_CNM_ID:
    fld_val = SOC_PB_CNT_SRC_TYPE_CNM_ID_FLD_VAL;
    break;
  
  case SOC_PB_CNT_SRC_TYPE_EGR_PP:
    fld_val = SOC_PB_CNT_SRC_TYPE_EGR_PP_FLD_VAL;
    break;
 
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 23, exit);
  }

  *src = fld_val;

  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("soc_pb_cnt_get_source_id()", 0, 0);

}
  
/*
 * Get the processor id e.g decide wether the counting is
 * done on processor A or on processor B
 */
STATIC
  uint32
    soc_pb_cnt_get_processor_id(
      SOC_SAND_IN  SOC_PB_CNT_PROCESSOR_ID processor_ndx,
      SOC_SAND_OUT uint32           *proc_id
    )

{
  uint32
    fld_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNT_GET_PROCESSOR_ID);

  switch (processor_ndx)
  {
  
  case SOC_PB_CNT_PROCESSOR_ID_A:
    fld_val = SOC_PB_CNT_PROCESSOR_ID_A_FLD_VAL;
    break;

  case SOC_PB_CNT_PROCESSOR_ID_B:
    fld_val = SOC_PB_CNT_PROCESSOR_ID_B_FLD_VAL;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 30, exit);
  }

  *proc_id = fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in processor_id_get_unsafe()", 0, 0);

}

/*
 * Get the Ingress mode
 */
STATIC
uint32
  soc_pb_cnt_get_ingress_mode(
    SOC_SAND_IN  SOC_PB_CNT_MODE_STATISTICS ing,
    SOC_SAND_OUT uint32           *ing_mode
  )
{

  uint32
    fld_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNT_GET_INGRESS_MODE);
  
  switch(ing)
  {
   
  case SOC_PB_CNT_MODE_STATISTICS_NO_COLOR:
    fld_val = SOC_PB_CNT_MODE_STATISTICS_NO_COLOR_FLD_VAL;
    break;
  
  case SOC_PB_CNT_MODE_STATISTICS_COLOR_RES_LOW:
    fld_val = SOC_PB_CNT_MODE_STATISTICS_COLOR_RES_LOW_FLD_VAL;
    break;
   
  case SOC_PB_CNT_MODE_STATISTICS_COLOR_RES_HI:
    fld_val = SOC_PB_CNT_MODE_STATISTICS_COLOR_RES_HI_FLD_VAL;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_CNT_ING_MODE_OUT_OF_RANGE_ERR, 33, exit);
  }

  *ing_mode = fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in processor_id_get_unsafe()", 0, 0);

}

 /*
  *  Get the Egress mode
  */
STATIC
uint32
  soc_pb_cnt_get_egress_mode(
    SOC_SAND_IN  SOC_PB_CNT_MODE_EG_RES      resolution,
    SOC_SAND_OUT uint32               *res_hw
  )
{
  
  uint32
    fld_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNT_GET_EGRESS_MODE);
  
  switch(resolution)
  {
  
  case SOC_PB_CNT_MODE_EG_RES_NO_COLOR:
    fld_val = SOC_PB_CNT_MODE_EG_RES_NO_COLOR_FLD_VAL;
    break;
  
  case SOC_PB_CNT_MODE_EG_RES_COLOR:
    fld_val = SOC_PB_CNT_MODE_EG_RES_COLOR_FLD_VAL;
    break;
  
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_CNT_EG_MODE_OUT_OF_RANGE_ERR, 33, exit);
  }
  *res_hw = fld_val;


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in processor_id_get_unsafe()", 0, 0);

}

/*********************************************************************
 *     Configure the Counter Processor: its counting mode and
 *     its counting source.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_cnt_counters_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_CNT_PROCESSOR_ID         processor_ndx,
    SOC_SAND_IN  SOC_PB_CNT_COUNTERS_INFO        *info
  )
{
  uint32
    proc_id,
    ing_mode=0,
    src_type=0,
    top_q,
    fld_val_tm_low = 0xFFFF,
    fld_val_tm_high = 0xFFFF,
    fld_val_vsi_low = 0xFFFF,
    fld_val_vsi_high = 0xFFFF,
    fld_val_outlif_low = 0xFFFF,
    fld_val_outlif_high = 0xFFFF,
    base_val_tm = 0,
    base_val_vsi = 0,
    base_val_outlif = 0,
    fld_val = 0,
    eg_mode_resolution=0,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;
  SOC_PB_PP_REGS
    *pp_regs;
  SOC_PB_PP_EPNI_PP_PCT_TBL_DATA
      tbl_data;
  uint32
    pp_port_ndx;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNT_COUNTERS_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();
  pp_regs = soc_pb_pp_regs();

  res = soc_pb_cnt_get_processor_id(
          processor_ndx,
          &proc_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 3, exit);

  res = soc_pb_cnt_get_source_id(
          info->src_type,
          &src_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 7, exit);
  
  SOC_PB_FLD_SET(regs->iqm.cnt_processor_config4_reg.crps_src_type[proc_id], src_type, 10, exit);

 /*
  *  If the source type is Ingress, set the Ingress mode
  */
  if (src_type != SOC_PB_CNT_SRC_TYPE_EGR_PP)
  {
    res = soc_pb_cnt_get_ingress_mode(
            info->mode_statistics,
            &ing_mode
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 19, exit);
    
    SOC_PB_FLD_SET(regs->iqm.cntprocessor_config1_reg.crps_cnt_work_mode[proc_id], ing_mode, 23, exit);
  }
  else /* Set the egress work mode */
  {
    res = soc_pb_cnt_get_egress_mode(
            info->mode_eg.resolution,
             &eg_mode_resolution
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    
    switch (info->mode_eg.type)
    {
    case SOC_PB_CNT_MODE_EG_TYPE_TM:
      fld_val_tm_low = 0x0;
      base_val_tm = info->mode_eg.base_val;
      fld_val = SOC_PB_CNT_MODE_EG_TYPE_TM_FLD_VAL;
  	  break;
    
    case SOC_PB_CNT_MODE_EG_TYPE_VSI_FLD_VAL:
      fld_val_vsi_low = 0x0;
      base_val_vsi = info->mode_eg.base_val;
      fld_val = SOC_PB_CNT_MODE_EG_TYPE_VSI_FLD_VAL;
      break;

    case SOC_PB_CNT_MODE_EG_TYPE_OUTLIF_FLD_VAL:
      fld_val_outlif_low = 0x0;
      base_val_outlif = info->mode_eg.base_val;
      fld_val = SOC_PB_CNT_MODE_EG_TYPE_OUTLIF_FLD_VAL;
      break;

    default:
      break;
    }
    SOC_PB_PP_FLD_SET(pp_regs->epni.counter_range_low_reg0_reg.counter_range_low_tm, fld_val_tm_low, 120, exit);
    SOC_PB_PP_FLD_SET(pp_regs->epni.counter_range_high_reg0_reg.counter_range_high_tm, fld_val_tm_high, 130, exit);

    SOC_PB_PP_FLD_SET(pp_regs->epni.counter_range_low_reg0_reg.counter_range_low_vsi, fld_val_vsi_low, 140, exit);
    SOC_PB_PP_FLD_SET(pp_regs->epni.counter_range_high_reg0_reg.counter_range_high_vsi, fld_val_vsi_high, 150, exit);

    SOC_PB_PP_FLD_SET(pp_regs->epni.counter_range_low_reg1_reg.counter_range_low_outlif, fld_val_outlif_low, 160, exit);
    SOC_PB_PP_FLD_SET(pp_regs->epni.counter_range_high_reg1_reg.counter_range_high_outlif, fld_val_outlif_high, 170, exit);

    SOC_PB_PP_FLD_SET(pp_regs->epni.counter_base_reg0_reg.counter_base_tm, base_val_tm, 173, exit);
    SOC_PB_PP_FLD_SET(pp_regs->epni.counter_base_reg0_reg.counter_base_vsi, base_val_vsi, 176, exit);
    SOC_PB_PP_FLD_SET(pp_regs->epni.counter_base_reg1_reg.counter_base_outlif, base_val_outlif, 180, exit);

    SOC_PB_FLD_SET(regs->iqm.cntprocessor_config1_reg.crps_cnt_work_mode[proc_id],eg_mode_resolution,199,exit);

    for(pp_port_ndx = 0;  pp_port_ndx < SOC_PB_PORT_NOF_PP_PORTS;  pp_port_ndx++)
    {
      res = soc_pb_pp_epni_pp_pct_tbl_get_unsafe(
              unit,
              pp_port_ndx,
              &tbl_data
              );
      SOC_SAND_CHECK_FUNC_RESULT(res, 121, exit);

      tbl_data.count_mode = fld_val;
      tbl_data.count_enable = 0x1;

      res = soc_pb_pp_epni_pp_pct_tbl_set_unsafe(
               unit,
               pp_port_ndx,
               &tbl_data
             );
      SOC_SAND_CHECK_FUNC_RESULT(res, 123, exit);
    }
  }
  
  /*
   *  Set the VOQ parameters
   */
  if (info->src_type == SOC_PB_CNT_SRC_TYPE_VOQ)
  {
    SOC_PB_FLD_SET(regs->iqm.cnt_processor_config4_reg.crps_queue_shift[proc_id],info->voq_cnt.q_set_size,1160,exit);
  
    SOC_PB_FLD_SET(regs->iqm.cnt_processor_shift_cfg1_reg[proc_id].crps_base_q,info->voq_cnt.start_q,1170,exit);
    top_q = SOC_PB_CNT_QUEUE_NDX_MAX;
    SOC_PB_FLD_SET(regs->iqm.cnt_processor_shift_cfg1_reg[proc_id].crps_top_q,top_q,1171,exit);
  }

  /*
   *  Set the Statistic-Tag parameters
   */
  if (info->src_type == SOC_PB_CNT_SRC_TYPE_STAG)
  {
    SOC_PB_FLD_SET(regs->iqm.statistics_tag_configuration_reg.stat_tag_crps_cmd_lsb,info->stag_lsb,1240,exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnt_counters_set_unsafe()", 0, 0);
}



uint32
  soc_pb_cnt_counters_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_CNT_PROCESSOR_ID         processor_ndx,
    SOC_SAND_IN  SOC_PB_CNT_COUNTERS_INFO        *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNT_COUNTERS_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(processor_ndx, SOC_PB_PROCESSOR_NDX_MAX, SOC_PB_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->src_type, SOC_PB_CNT_COUNTERS_INFO_SRC_TYPE_MAX, SOC_PB_CNT_SRC_TYPE_OUT_OF_RANGE_ERR, 11, exit);
  
  if( info->src_type == SOC_PB_CNT_SRC_TYPE_EGR_PP )
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(info->mode_eg.type, SOC_PB_CNT_COUNTERS_INFO_MODE_EG_MAX, SOC_PB_CNT_MODE_EG_OUT_OF_RANGE_ERR, 12, exit);
    if( info->stag_lsb !=0)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_CNT_DEFINED_LSB_STAG_ERR, 19, exit);
    }
    if(processor_ndx != SOC_PB_CNT_PROCESSOR_ID_B)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_CNT_EG_MODE_AND_PROC_A, 21, exit);
    }
  }
  else /* Ingress */
  {
    if(info->src_type == SOC_PB_CNT_SRC_TYPE_VOQ)
    {
      SOC_SAND_ERR_IF_ABOVE_MAX(info->voq_cnt.q_set_size, SOC_PB_CNT_VOQ_PARAMS_Q_SET_SIZE_MAX, SOC_PB_CNT_Q_SET_SIZE_OUT_OF_RANGE_ERR , 23, exit);
      SOC_SAND_ERR_IF_ABOVE_MAX(info->voq_cnt.start_q, SOC_PB_CNT_QUEUE_NDX_MAX, SOC_PB_CNT_START_Q_OUT_OF_RANGE_ERR, 29, exit);
    }

    if( info->src_type == SOC_PB_CNT_SRC_TYPE_STAG)
    {
       SOC_SAND_ERR_IF_ABOVE_MAX(info->stag_lsb, SOC_PB_CNT_COUNTERS_INFO_STAG_MAX, SOC_PB_CNT_STAG_LSB_OUT_OF_RANGE_ERR, 37 , exit);
    }
    else if( info->stag_lsb !=0)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_CNT_DEFINED_LSB_STAG_ERR, 43, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnt_counters_set_verify()", 0, 0);
}

uint32
  soc_pb_cnt_counters_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_CNT_PROCESSOR_ID         processor_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNT_COUNTERS_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(processor_ndx, SOC_PB_PROCESSOR_NDX_MAX, SOC_PB_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnt_counters_get_verify()", 0, 0);
}

/*********************************************************************
*     Configure the Counter Processor: its counting mode and
 *     its counting source.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_cnt_counters_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_CNT_PROCESSOR_ID         processor_ndx,
    SOC_SAND_OUT SOC_PB_CNT_COUNTERS_INFO        *info
  )

{
  uint32
    res = SOC_SAND_OK,
    proc_id,
    fld_val;
  SOC_PETRA_REGS
    *regs;
  SOC_PB_PP_REGS
    *pp_regs;
  SOC_PB_PP_EPNI_PP_PCT_TBL_DATA
    tbl_data;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNT_COUNTERS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_CNT_COUNTERS_INFO_clear(info);

  regs = soc_petra_regs();

  pp_regs = soc_pb_pp_regs();

  res = soc_pb_cnt_get_processor_id(
          processor_ndx,
          &proc_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_PB_FLD_GET(regs->iqm.cnt_processor_config4_reg.crps_src_type[proc_id], fld_val, 17, exit);
  
  switch(fld_val)
  {
  case SOC_PB_CNT_SRC_TYPE_EGR_PP_FLD_VAL:
    info->src_type = SOC_PB_CNT_SRC_TYPE_EGR_PP;
    break;

  case SOC_PB_CNT_SRC_TYPE_ING_PP_FLD_VAL:
    info->src_type = SOC_PB_CNT_SRC_TYPE_ING_PP;
    break;

  case SOC_PB_CNT_SRC_TYPE_VOQ_FLD_VAL:
    info->src_type = SOC_PB_CNT_SRC_TYPE_VOQ;
    break;

  case SOC_PB_CNT_SRC_TYPE_STAG_FLD_VAL:
    info->src_type = SOC_PB_CNT_SRC_TYPE_STAG;
    break;

  case SOC_PB_CNT_SRC_TYPE_CNM_ID_FLD_VAL:
    info->src_type = SOC_PB_CNT_SRC_TYPE_CNM_ID;
    break;

  case SOC_PB_CNT_SRC_TYPE_VSQ_FLD_VAL:
    info->src_type = SOC_PB_CNT_SRC_TYPE_VSQ;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_CNT_SRC_TYPE_OUT_OF_RANGE_ERR, 123, exit);
  }

  if(info->src_type == SOC_PB_CNT_SRC_TYPE_EGR_PP)
  {
    
    res = soc_pb_pp_epni_pp_pct_tbl_get_unsafe(
            unit,
            0,
            &tbl_data
          );
    fld_val = tbl_data.count_mode;

    switch (fld_val)
    {
    case SOC_PB_CNT_MODE_EG_TYPE_TM_FLD_VAL:
    
      SOC_PB_PP_FLD_GET(pp_regs->epni.counter_base_reg0_reg.counter_base_tm, fld_val, 226, exit);
      info->mode_eg.type = SOC_PB_CNT_MODE_EG_TYPE_TM;
      info->mode_eg.base_val = fld_val;
    
    break;
    
    case SOC_PB_CNT_MODE_EG_TYPE_OUTLIF_FLD_VAL:

      SOC_PB_PP_FLD_GET(pp_regs->epni.counter_base_reg1_reg.counter_base_outlif, fld_val, 326, exit);
      info->mode_eg.type = SOC_PB_CNT_MODE_EG_TYPE_OUTLIF;
      info->mode_eg.base_val = fld_val;
      
    break;

    case SOC_PB_CNT_MODE_EG_TYPE_VSI_FLD_VAL:
      
      SOC_PB_PP_FLD_GET(pp_regs->epni.counter_base_reg0_reg.counter_base_vsi, fld_val, 246, exit);
      info->mode_eg.type = SOC_PB_CNT_MODE_EG_TYPE_VSI;
      info->mode_eg.base_val = fld_val;

    break;
    }
 
    SOC_PB_FLD_GET(regs->iqm.cntprocessor_config1_reg.crps_cnt_work_mode[proc_id],fld_val,299,exit);
 
    switch(fld_val)
    {
    case SOC_PB_CNT_MODE_EG_RES_NO_COLOR_FLD_VAL:
      info->mode_eg.resolution = SOC_PB_CNT_MODE_EG_RES_NO_COLOR;
      break;

    case SOC_PB_CNT_MODE_EG_RES_COLOR_FLD_VAL:
      info->mode_eg.resolution = SOC_PB_CNT_MODE_EG_RES_COLOR;
      break;

    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_CNT_EG_MODE_OUT_OF_RANGE_ERR, 631, exit);
    }
  }
  else /* Ingress */
  {
    SOC_PB_FLD_GET(regs->iqm.cntprocessor_config1_reg.crps_cnt_work_mode[proc_id], fld_val, 37, exit);

    switch(fld_val)
    {
    case SOC_PB_CNT_MODE_STATISTICS_NO_COLOR_FLD_VAL:
      info->mode_statistics = SOC_PB_CNT_MODE_STATISTICS_NO_COLOR;
      break;

    case SOC_PB_CNT_MODE_STATISTICS_COLOR_RES_LOW_FLD_VAL:
      info->mode_statistics = SOC_PB_CNT_MODE_STATISTICS_COLOR_RES_LOW;
      break;

    case SOC_PB_CNT_MODE_STATISTICS_COLOR_RES_HI_FLD_VAL:
      info->mode_statistics = SOC_PB_CNT_MODE_STATISTICS_COLOR_RES_HI;
      break;

    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_CNT_ING_MODE_OUT_OF_RANGE_ERR, 941, exit);
    }
  }

  if (info->src_type == SOC_PB_CNT_SRC_TYPE_VOQ)
  {
    SOC_PB_FLD_GET(regs->iqm.cnt_processor_config4_reg.crps_queue_shift[proc_id], fld_val, 1043, exit);

    switch(fld_val)
    {
    case SOC_PB_CNT_Q_SET_SIZE_1_Q_FLD_VAL:
      info->voq_cnt.q_set_size = SOC_PB_CNT_Q_SET_SIZE_1_Q;
      break;

    case SOC_PB_CNT_Q_SET_SIZE_2_Q_FLD_VAL:
      info->voq_cnt.q_set_size = SOC_PB_CNT_Q_SET_SIZE_2_Q;
      break;

    case SOC_PB_CNT_Q_SET_SIZE_4_Q_FLD_VAL:
      info->voq_cnt.q_set_size = SOC_PB_CNT_Q_SET_SIZE_4_Q;
      break;

    case SOC_PB_CNT_Q_SET_SIZE_8_Q_FLD_VAL:
      info->voq_cnt.q_set_size = SOC_PB_CNT_Q_SET_SIZE_8_Q;
      break;

    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_CNT_VOQ_PARAMS_OUT_OF_RANGE_ERR, 1153, exit);
    }

    SOC_PB_FLD_GET(regs->iqm.cnt_processor_shift_cfg1_reg[proc_id].crps_base_q, fld_val,1259,exit);
    info->voq_cnt.start_q = fld_val;
  }

  if( info->src_type == SOC_PB_CNT_SRC_TYPE_STAG )
  {
    SOC_PB_FLD_GET(regs->iqm.statistics_tag_configuration_reg.stat_tag_crps_cmd_lsb, fld_val, 1367, exit);
    info->stag_lsb = fld_val;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnt_counters_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Get the current status of the counter processor.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_cnt_status_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_CNT_PROCESSOR_ID         processor_ndx,
    SOC_SAND_OUT SOC_PB_CNT_STATUS               *proc_status
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;
  uint32
    processor_hw_id,
    reg_val,
    fld_val;
  SOC_PB_CNT_COUNTERS_INFO
    counter_info;
  uint32
    cnt_ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNT_STATUS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(proc_status);

  SOC_PB_CNT_STATUS_clear(proc_status);

  regs = soc_petra_regs();
  
  /*
   *  Translate the processor index to its HW number
   */
  switch (processor_ndx)
  {
  case SOC_PB_CNT_PROCESSOR_ID_A:
    processor_hw_id = SOC_PB_CNT_PROCESSOR_ID_A_FLD_VAL;
  	break;

  case SOC_PB_CNT_PROCESSOR_ID_B:
    processor_hw_id = SOC_PB_CNT_PROCESSOR_ID_B_FLD_VAL;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 10, exit);
  }

  /*
   * Find if the id is valid
   */
  SOC_PB_FLD_GET(regs->iqm.crps_interrupt_reg.crps_invld_ad_acc[processor_hw_id], fld_val, 20, exit);
  proc_status->is_cnt_id_invalid = SOC_SAND_NUM2BOOL(fld_val);
  
  /*
   * Read if the cache is full
   */
  SOC_PB_FLD_GET(regs->iqm.crps_interrupt_reg.crps_pre_read_fifo_full[processor_ndx], fld_val, 30, exit);
  proc_status->is_cache_full = SOC_SAND_NUM2BOOL(fld_val);
  
  /*
   * Get the number of active counters
   */
  SOC_PB_FLD_GET(regs->iqm.crps_counter_sts1_reg[processor_ndx].crps_act_cntrs_cnt, fld_val, 40, exit);
  proc_status->nof_active_cnts = fld_val;

  /*
   * Get the values of the last overflown counter
   */
  SOC_PB_FLD_GET(regs->iqm.crps_interrupt_reg.crps_cnt_ovf[processor_ndx], fld_val, 45, exit);
  proc_status->overflow_cnt.is_overflow = SOC_SAND_NUM2BOOL(fld_val);

  SOC_PB_REG_GET(regs->iqm.crps_cnt_ovrf_sts_reg, reg_val, 46, exit);

  SOC_PB_FLD_FROM_REG(regs->iqm.crps_cnt_ovrf_sts_reg.crps_counter_ovf_addr[processor_ndx], fld_val, reg_val, 50, exit);

  SOC_PB_CNT_COUNTERS_INFO_clear(&counter_info);
  res = soc_pb_cnt_counters_get_unsafe(
    unit,
    processor_ndx,
    &counter_info
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  soc_pb_cnt_id_compute(
    counter_info,
    fld_val,
    &cnt_ndx
    );
  proc_status->overflow_cnt.last_cnt_id = cnt_ndx;

  SOC_PB_FLD_FROM_REG(regs->iqm.crps_cnt_ovrf_sts_reg.crps_is_ovf_cnt_octs[processor_ndx], fld_val, reg_val, 70, exit);
  proc_status->overflow_cnt.is_byte_overflow = SOC_SAND_NUM2BOOL(fld_val);

  SOC_PB_FLD_FROM_REG(regs->iqm.crps_cnt_ovrf_sts_reg.crps_is_ovf_cnt_pkts[processor_ndx], fld_val, reg_val, 80, exit);
  proc_status->overflow_cnt.is_pckt_overflow = SOC_SAND_NUM2BOOL(fld_val);
  
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnt_status_get_unsafe()", 0, 0);
}

uint32
  soc_pb_cnt_status_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_CNT_PROCESSOR_ID         processor_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNT_STATUS_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(processor_ndx, SOC_PB_PROCESSOR_NDX_MAX, SOC_PB_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnt_status_get_verify()", 0, 0);
}

/*********************************************************************
*     Read the counter values saved in a cache according to
 *     the polling algorithm (algorithmic method).
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_cnt_algorithmic_read_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_CNT_PROCESSOR_ID         processor_ndx,
    SOC_SAND_OUT SOC_PB_CNT_RESULT_ARR           *result_arr
  )
{
  uint32
    res = SOC_SAND_OK,
    nof_read_cnt,
    fld_val,
    fld_val2,
    fld_val3,
    reg_val,
    indx = 0;
  SOC_PB_CNT_RESULT
    cnt_res[SOC_PB_CNT_CACHE_LENGTH];
  SOC_PETRA_REGS
    *regs;
  uint8
    not_empty,
    more_counters_2read;
  SOC_PB_CNT_COUNTERS_INFO
    counter_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNT_ALGORITHMIC_READ_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(result_arr);

  regs = soc_petra_regs();

  /*
   * Init variable
   */
  for(indx = 0;  indx < SOC_PB_CNT_CACHE_LENGTH;  indx++)
  {
    COMPILER_64_ZERO(cnt_res[indx].pkt_cnt);
    COMPILER_64_ZERO(cnt_res[indx].byte_cnt);
  }

  /*
   * This function consist of three steps:
   * 1. Check if the values are valid and the counter queue is not empty.
   * 2. While the counter queue is not empty and the index is less than 15 copy to temp array.
   * 3. Copy from the temp array to the result table.
   */
  SOC_PB_FLD_GET( regs->iqm.crps_interrupt_reg.crps_pre_read_fifo_not_empty[processor_ndx], fld_val, 10, exit);
  not_empty = SOC_SAND_NUM2BOOL(fld_val);
  more_counters_2read = not_empty;
  SOC_PB_FLD_SET( regs->iqm.crps_interrupt_reg.crps_pre_read_fifo_not_empty[processor_ndx], 0x1, 11, exit);

  /*
   * Step 2. Read the counters.
   */
  nof_read_cnt = 0;
  for(indx = 0;  (indx < SOC_PB_CNT_CACHE_LENGTH) && (more_counters_2read == TRUE);  indx++)
  {
    SOC_PB_REG_GET(regs->iqm.cnt_proccesso_rd_result0_reg[processor_ndx], reg_val, 5, exit);
    SOC_PB_FLD_GET(regs->iqm.cnt_proccesso_rd_result1_reg[processor_ndx].crps_pkts_cnt, fld_val2,13,exit);
    SOC_PB_FLD_GET(regs->iqm.cnt_proccesso_rd_result2_reg[processor_ndx].crps_octs_cnt, fld_val3,17,exit);

    SOC_PB_FLD_FROM_REG(regs->iqm.cnt_proccesso_rd_result0_reg[processor_ndx].crps_data_valid, fld_val, reg_val,7,exit);

    /*
     *  Valid entry
     */
    if (fld_val == 0x1)
    {
      SOC_PB_FLD_FROM_REG(regs->iqm.cnt_proccesso_rd_result0_reg[processor_ndx].crps_cnt_addr, cnt_res[nof_read_cnt].counter_id, reg_val,14,exit);
      COMPILER_64_SET(cnt_res[nof_read_cnt].pkt_cnt, 0, fld_val2);
      COMPILER_64_SET(cnt_res[nof_read_cnt].byte_cnt, 0, fld_val3);
      nof_read_cnt++;
    }
        
    SOC_PB_FLD_GET( regs->iqm.crps_interrupt_reg.crps_pre_read_fifo_not_empty[processor_ndx], fld_val, 18, exit);
    more_counters_2read = SOC_SAND_NUM2BOOL(fld_val);
    SOC_PB_FLD_SET( regs->iqm.crps_interrupt_reg.crps_pre_read_fifo_not_empty[processor_ndx], 0x1, 19, exit);
  }

  /*
   * Step 3.
   */
  res = soc_pb_cnt_counters_get_verify(
          unit,
          processor_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);

  SOC_PB_CNT_COUNTERS_INFO_clear(&counter_info);
  res = soc_pb_cnt_counters_get_unsafe(
          unit,
          processor_ndx,
          &counter_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  for (indx = 0 ;  indx < nof_read_cnt; indx++)
  {
    result_arr->cnt_result[indx].counter_id = cnt_res[indx].counter_id;
    COMPILER_64_SET(result_arr->cnt_result[indx].pkt_cnt , COMPILER_64_HI(cnt_res[indx].pkt_cnt), COMPILER_64_LO(cnt_res[indx].pkt_cnt));
    COMPILER_64_SET(result_arr->cnt_result[indx].byte_cnt, COMPILER_64_HI(cnt_res[indx].byte_cnt), COMPILER_64_LO(cnt_res[indx].byte_cnt));
  }

  result_arr->nof_counters = nof_read_cnt;
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnt_algorithmic_read_unsafe()", 0, 0);
}

uint32
  soc_pb_cnt_algorithmic_read_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_CNT_PROCESSOR_ID         processor_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNT_ALGORITHMIC_READ_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(processor_ndx, SOC_PB_PROCESSOR_NDX_MAX, SOC_PB_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnt_algorithmic_read_verify()", 0, 0);
}

/*********************************************************************
*     Read the counter value according to the counter.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_cnt_direct_read_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_CNT_PROCESSOR_ID         processor_ndx,
    SOC_SAND_IN  uint32                    counter_ndx,
    SOC_SAND_OUT SOC_PB_CNT_RESULT               *read_rslt
  )
{
  uint32
    res = SOC_SAND_OK,
    fld_val;
   
  SOC_PB_IQM_CNTS_MEM_TBL_DATA
    tbl_data;
  SOC_PB_CNT_COUNTERS_INFO
    counter_info;
  SOC_PETRA_REGS
    *regs;
  uint8
    poll_success=0;
  SOC_PETRA_POLL_INFO
    poll_info;
  uint32
     cnt_ndx = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNT_DIRECT_READ_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(read_rslt);

  regs = soc_petra_regs();

  /*
   * This function consists of three sequential steps:
   * 1.  Initializing the counter read register values values- read_request_type, address.
   * 2. Polling method- trying to get a valid data in a time slot determined by hardware factors
   *    in the same fashion as other polling method.
   * 3. If the polling method failed return error message otherwise read the results directly
   *    from the register.
   */

  /*
   * Step 1. : Trigger off
   */
  SOC_PB_FLD_SET(regs->iqm.cnt_processor_read_reg[processor_ndx].crps_rd_req, 0, 13, exit);
  SOC_PB_FLD_SET(regs->iqm.cnt_processor_read_reg[processor_ndx].crps_rd_req_type, SOC_PB_CNT_READ_DIRECT_READ, 16, exit);
  SOC_PB_FLD_SET(regs->iqm.cnt_processor_read_reg[processor_ndx].crps_rd_req_addr, counter_ndx, 17, exit);
 /*
  *  Trigger on
  */
  SOC_PB_FLD_SET(regs->iqm.cnt_processor_read_reg[processor_ndx].crps_rd_req, 1, 18, exit);

 /*
  * Step 2. : Polling method
  */
  poll_info.expected_value      = 0x1;
  poll_info.busy_wait_nof_iters = SOC_PB_CNT_INIT_WAIT_ITERATIONS;
  poll_info.timer_nof_iters     = SOC_PB_CNT_INIT_TIMER_ITERATIONS;
  poll_info.timer_delay_msec    = SOC_PB_CNT_INIT_TIMER_DELAY_MSEC;

  res = soc_petra_status_fld_poll_unsafe(
          unit,
          SOC_PB_REG_DB_ACC_REF(regs->iqm.cnt_proccesso_direct_rd_result0_reg[processor_ndx].crps_direct_data_valid),
          SOC_PETRA_DEFAULT_INSTANCE,
          &poll_info,
          &poll_success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

 /*
  * Step 3.
  */
  if (!poll_success)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_CNT_DIRECT_READ_OUT_OF_TIME_ERR, 55, exit);
  }

  SOC_PB_FLD_GET(regs->iqm.cnt_proccesso_direct_rd_result1_reg[processor_ndx].crps_direct_pkts_cnt, fld_val, 61, exit);
  tbl_data.packets_counter = fld_val;
  SOC_PB_FLD_GET(regs->iqm.cnt_proccesso_direct_rd_result2_reg[processor_ndx].crps_direct_octs_cnt, fld_val, 67, exit);
  tbl_data.octets_counter = fld_val;

  res = soc_pb_cnt_counters_get_verify(
    unit,
    processor_ndx
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 113, exit);

  SOC_PB_CNT_COUNTERS_INFO_clear(&counter_info);
  res = soc_pb_cnt_counters_get_unsafe(
    unit,
    processor_ndx,
    &counter_info
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 115, exit);

  soc_pb_cnt_id_compute(
    counter_info,
    counter_ndx,
    &cnt_ndx
    );
  read_rslt->counter_id = cnt_ndx;
  COMPILER_64_SET(read_rslt->byte_cnt, 0, tbl_data.octets_counter);
  COMPILER_64_SET(read_rslt->pkt_cnt, 0, tbl_data.packets_counter);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnt_direct_read_unsafe()", 0, counter_ndx);
}

uint32
  soc_pb_cnt_direct_read_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_CNT_PROCESSOR_ID         processor_ndx,
    SOC_SAND_IN  uint32                    counter_ndx
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNT_DIRECT_READ_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(processor_ndx, SOC_PB_PROCESSOR_NDX_MAX, SOC_PB_PROCESSOR_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(counter_ndx, SOC_PB_COUNTER_NDX_MAX, SOC_PB_COUNTER_NDX_OUT_OF_RANGE_ERR, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnt_direct_read_verify()", 0, counter_ndx);
}

/*********************************************************************
*     Convert a queue index to the counter index.
*     The computation of the counter_ndx is: f((queue_ndx - start_q)/q_set_size))
*     where f is the counter id compute which compute the id according to the ingress mode.
*********************************************************************/
uint32
  soc_pb_cnt_q2cnt_id_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_CNT_PROCESSOR_ID         processor_ndx,
    SOC_SAND_IN  uint32                   queue_ndx,
    SOC_SAND_OUT uint32                   *counter_ndx
  )
{
  uint32
    q_set_size_val = 1,
    res = SOC_SAND_OK;
  SOC_PB_CNT_COUNTERS_INFO
    info;
  uint32
    counter_id,
    fld_val;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNT_Q2CNT_ID_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(counter_ndx);
  
  res = soc_pb_cnt_counters_get_unsafe(
          unit,
          processor_ndx,
          &info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if(info.src_type != SOC_PB_CNT_SRC_TYPE_VOQ)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_CNT_PROC_SRC_TYPE_NOT_VOQ_ERR, 20, exit);
  }

  res = soc_convert_enum_val2_q_set_size(
    info.voq_cnt.q_set_size,
    &q_set_size_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
   counter_id = ((queue_ndx - info.voq_cnt.start_q)/q_set_size_val);
  
   soc_pb_cnt_id_compute(
     info,
     counter_id,
     &fld_val);

   *counter_ndx = fld_val;
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnt_q2cnt_id_unsafe()", queue_ndx, *counter_ndx);
}

uint32
  soc_pb_cnt_q2cnt_id_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_CNT_PROCESSOR_ID         processor_ndx,
    SOC_SAND_IN  uint32                   queue_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNT_Q2CNT_ID_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(queue_ndx, SOC_PB_CNT_QUEUE_NDX_MAX, SOC_PB_QUEUE_NDX_OUT_OF_RANGE_ERR, 10, exit);

  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnt_q2cnt_id_verify()", queue_ndx, 0);
}

/*********************************************************************
*     Convert a counter index to the respective queue index.
*     The computation of the queue_ndx: g(cnt_ndx) * info.voq_cnt.q_set_size + info.voq_cnt.start_q
*     where g is the inverse function of counter id compute which compute the id according to the ingress mode.
*********************************************************************/
uint32
  soc_pb_cnt_cnt2q_id_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_CNT_PROCESSOR_ID         processor_ndx,
    SOC_SAND_IN  uint32                   counter_ndx,
    SOC_SAND_OUT uint32                   *queue_ndx
  )
{
  uint32
    q_set_size_val = 1,
    res = SOC_SAND_OK;
  SOC_PB_CNT_COUNTERS_INFO
    info;
  uint32    
    cnt_ndx = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNT_CNT2Q_ID_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(queue_ndx);

  res = soc_pb_cnt_counters_get_unsafe(
    unit,
    processor_ndx,
    &info
    );

  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (info.src_type != SOC_PB_CNT_SRC_TYPE_VOQ )
  {

    SOC_SAND_SET_ERROR_CODE(SOC_PB_CNT_PROC_SRC_TYPE_NOT_VOQ_ERR, 20, exit);

  }

  switch(info.mode_statistics)
  {
  case SOC_PB_CNT_MODE_STATISTICS_NO_COLOR:
    cnt_ndx = counter_ndx / 2;
    break;

  case SOC_PB_CNT_MODE_STATISTICS_COLOR_RES_LOW:
    cnt_ndx = counter_ndx / 4;
    break;

  case SOC_PB_CNT_MODE_STATISTICS_COLOR_RES_HI:
    cnt_ndx = counter_ndx / 5;
    break;

  default:
    break;
  }

  res = soc_convert_enum_val2_q_set_size(
    info.voq_cnt.q_set_size,
    &q_set_size_val);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  *queue_ndx = cnt_ndx * q_set_size_val + info.voq_cnt.start_q;


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnt_cnt2q_id_unsafe()", counter_ndx, *queue_ndx);
}

uint32
  soc_pb_cnt_cnt2q_id_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_CNT_PROCESSOR_ID         processor_ndx,
    SOC_SAND_IN  uint32                   counter_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNT_CNT2Q_ID_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(counter_ndx, SOC_PB_COUNTER_NDX_MAX, SOC_PB_COUNTER_NDX_OUT_OF_RANGE_ERR, 10, exit);

  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnt_cnt2q_id_verify()", counter_ndx, 0);
}

/*********************************************************************
*     Configure the header compensation factor for the counter
 *     and meter modules to enable a meter and counter
 *     processing of this packet according to its original
 *     packet size.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_cnt_meter_hdr_compensation_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  int32                    hdr_compensation
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_IHB_PINFO_LBP_TBL_DATA
    tbl_data;
  int32
    fld_val;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNT_METER_HDR_COMPENSATION_SET_UNSAFE);

  if (hdr_compensation < 0)
  {
    fld_val = hdr_compensation + 64;
  }
  else
  {
    fld_val = hdr_compensation;
  }

  tbl_data.counter_compension = fld_val;
  res = soc_pb_ihb_pinfo_lbp_tbl_set_unsafe(
          unit,
          port_ndx,
          &tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 113, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnt_meter_hdr_compensation_set_unsafe()", port_ndx, 0);
}

uint32
  soc_pb_cnt_meter_hdr_compensation_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_IN  int32                    hdr_compensation
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNT_METER_HDR_COMPENSATION_SET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_ndx, SOC_PB_CNT_PORT_NDX_MAX, SOC_PB_PORT_NDX_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_OUT_OF_RANGE(hdr_compensation, SOC_PB_HDR_COMPENSATION_MIN, SOC_PB_HDR_COMPENSATION_MAX, SOC_PB_HDR_COMPENSATION_OUT_OF_RANGE_ERR, 20, exit);
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnt_meter_hdr_compensation_set_verify()", port_ndx, 0);
}

uint32
  soc_pb_cnt_meter_hdr_compensation_get_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNT_METER_HDR_COMPENSATION_GET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(port_ndx, SOC_PB_CNT_PORT_NDX_MAX, SOC_PB_PORT_NDX_OUT_OF_RANGE_ERR, 10, exit);

  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnt_meter_hdr_compensation_get_verify()", port_ndx, 0);
}

/*********************************************************************
*     Configure the header compensation factor for the counter
 *     and meter modules to enable a meter and counter
 *     processing of this packet according to its original
 *     packet size.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_cnt_meter_hdr_compensation_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   port_ndx,
    SOC_SAND_OUT int32                    *hdr_compensation
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_IHB_PINFO_LBP_TBL_DATA
    tbl_data;
  int32
    fld_val;
 SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_CNT_METER_HDR_COMPENSATION_GET_UNSAFE);

 SOC_SAND_CHECK_NULL_INPUT(hdr_compensation);
  
  res = soc_pb_ihb_pinfo_lbp_tbl_get_unsafe(
          unit,
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
  
  *hdr_compensation = fld_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_cnt_meter_hdr_compensation_get_unsafe()", port_ndx, 0);
}

#if SOC_PB_DEBUG_IS_LVL1

uint32
  SOC_PB_CNT_VOQ_PARAMS_verify(
    SOC_SAND_IN  SOC_PB_CNT_VOQ_PARAMS *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->q_set_size, SOC_PB_CNT_VOQ_PARAMS_Q_SET_SIZE_MAX, SOC_PB_CNT_Q_SET_SIZE_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_CNT_VOQ_PARAMS_verify()",0,0);
}

uint32
  SOC_PB_CNT_COUNTERS_INFO_verify(
    SOC_SAND_IN  SOC_PB_CNT_COUNTERS_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->src_type, SOC_PB_CNT_COUNTERS_INFO_SRC_TYPE_MAX, SOC_PB_CNT_SRC_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->mode_statistics, SOC_PB_CNT_COUNTERS_INFO_MODE_ING_MAX, SOC_PB_CNT_MODE_STATISTICS_OUT_OF_RANGE_ERR, 11, exit);
  
  SOC_SAND_ERR_IF_ABOVE_MAX(info->stag_lsb, SOC_PB_CNT_COUNTERS_INFO_STAG_MAX, SOC_PB_CNT_STAG_LSB_OUT_OF_RANGE_ERR , 13, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_CNT_VOQ_PARAMS, &(info->voq_cnt), 14, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_CNT_MODE_EG, &(info->mode_eg), 14, exit);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_CNT_COUNTERS_INFO_verify()",0,0);
}

uint32
  SOC_PB_CNT_OVERFLOW_verify(
    SOC_SAND_IN  SOC_PB_CNT_OVERFLOW *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);


  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_CNT_OVERFLOW_verify()",0,0);
}

uint32
  SOC_PB_CNT_STATUS_verify(
    SOC_SAND_IN  SOC_PB_CNT_STATUS *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_STRUCT_VERIFY(SOC_PB_CNT_OVERFLOW, &(info->overflow_cnt), 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_CNT_STATUS_verify()",0,0);
}

uint32
  SOC_PB_CNT_RESULT_verify(
    SOC_SAND_IN  SOC_PB_CNT_RESULT *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);


  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_CNT_RESULT_verify()",0,0);
}

uint32
  SOC_PB_CNT_RESULT_ARR_verify(
    SOC_SAND_IN  SOC_PB_CNT_RESULT_ARR *info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    ind;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  for (ind = 0; ind < SOC_PB_CNT_CACHE_LENGTH; ++ind)
  {
    SOC_PB_STRUCT_VERIFY(SOC_PB_CNT_RESULT, &(info->cnt_result[ind]), 10, exit);
  }
  SOC_SAND_ERR_IF_ABOVE_MAX(info->nof_counters, SOC_PB_CNT_RESULT_ARR_NOF_COUNTERS_MAX, SOC_PB_CNT_NOF_COUNTERS_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_CNT_RESULT_ARR_verify()",0,0);
}

#endif /* SOC_PB_DEBUG_IS_LVL1 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

