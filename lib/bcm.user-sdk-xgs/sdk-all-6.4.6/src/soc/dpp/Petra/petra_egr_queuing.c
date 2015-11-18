/* $Id: petra_egr_queuing.c,v 1.9 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/soc_petra/src/soc_petra_egr_queuing.c
*
* MODULE PREFIX:  soc_petra_egr
*
* FILE DESCRIPTION:
*
* REMARKS:
* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
******************************************************************/


/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_egr_queuing.h>
#include <soc/dpp/Petra/petra_ofp_rates.h>
#include <soc/dpp/Petra/petra_ports.h>

#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_tbl_access.h>

#include <soc/dpp/Petra/petra_sw_db.h>

#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>

#include <soc/dpp/Petra/petra_general.h>

#ifdef LINK_PB_LIBRARIES
#include <soc/dpp/Petra/PB_TM/pb_egr_queuing.h>
#endif

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PETRA_EGR_THRESH_MNT_NOF_BITS                (5)
#define SOC_PETRA_EGR_THRESH_EXP_NOF_BITS                (4)
#define SOC_PETRA_EGR_THRESH_NOF_BITS (SOC_PETRA_EGR_THRESH_MNT_NOF_BITS + SOC_PETRA_EGR_THRESH_EXP_NOF_BITS)

#define SOC_PETRA_EGR_TC_SHIFT                (2)

#define SOC_PETRA_EGR_DP_SHIFT                (0)


#define SOC_PA_EGR_OFP_SCH_MODES_MAX SOC_PETRA_EGR_HP_OVER_LP_PER_TYPE
#ifdef LINK_PB_LIBRARIES
#define SOC_PB_EGR_OFP_SCH_MODES_MAX SOC_PETRA_EGR_HP_OVER_LP_FAIR
#else
#define SOC_PB_EGR_OFP_SCH_MODES_MAX SOC_PA_EGR_OFP_SCH_MODES_MAX
#endif

#define SOC_PETRA_EGR_OFP_SCH_MODES_MAX       \
  SOC_PETRA_CST_VALUE_DISTINCT(EGR_OFP_SCH_MODES_MAX, uint32)

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
*     soc_petra_egr_queuing_regs_init
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
  soc_petra_egr_queuing_regs_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    reg_val,
    fld_val,
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_QUEUING_REGS_INIT);

  regs = soc_petra_regs();

  /*
   *  Scheduled
   */
  reg_val = 0;
  fld_val = 0x45;
  SOC_PETRA_FLD_TO_REG(regs->egq.schededuled_reject_buffer_threshold_reg.set_reas_rej_schd_buff_th, fld_val, reg_val, 5, exit);
  fld_val = 0x46;
  SOC_PETRA_FLD_TO_REG(regs->egq.schededuled_reject_buffer_threshold_reg.clear_reas_rej_schd_buff_th, fld_val, reg_val, 6, exit);
  SOC_PETRA_REG_SET(regs->egq.schededuled_reject_buffer_threshold_reg, reg_val, 10, exit);

  reg_val = 0;
  fld_val = 0x46;
  SOC_PETRA_FLD_TO_REG(regs->egq.scheduled_reject_descriptor_threshold_reg.set_reas_rej_schd_desc_th, fld_val, reg_val, 15, exit);
  fld_val = 0x47;
  SOC_PETRA_FLD_TO_REG(regs->egq.scheduled_reject_descriptor_threshold_reg.clear_reas_rej_schd_desc_th, fld_val, reg_val, 16, exit);
  SOC_PETRA_REG_SET(regs->egq.scheduled_reject_descriptor_threshold_reg, reg_val, 20, exit);

  /*
   *  Unscheduled
   */
  reg_val = 0;
  fld_val = 0x45;
  SOC_PETRA_FLD_TO_REG(regs->egq.unscheduled_reject_buffer_threshold_reg.set_reas_rej_un_schd_buff_th, fld_val, reg_val, 25, exit);
  fld_val = 0x46;
  SOC_PETRA_FLD_TO_REG(regs->egq.unscheduled_reject_buffer_threshold_reg.clear_reas_rej_un_schd_buff_th, fld_val, reg_val, 26, exit);
  SOC_PETRA_REG_SET(regs->egq.unscheduled_reject_buffer_threshold_reg, reg_val, 30, exit);

  reg_val = 0;
  fld_val = 0x46;
  SOC_PETRA_FLD_TO_REG(regs->egq.unscheduled_reject_descriptor_threshold_reg.set_reas_rej_un_schd_desc_th, fld_val, reg_val, 35, exit);
  fld_val = 0x47;
  SOC_PETRA_FLD_TO_REG(regs->egq.unscheduled_reject_descriptor_threshold_reg.clear_reas_rej_un_schd_desc_th, fld_val, reg_val, 36, exit);
  SOC_PETRA_REG_SET(regs->egq.unscheduled_reject_descriptor_threshold_reg, reg_val, 40, exit);

  /*
   * Egress MC TC translation for 4 to 8 values
   * Mapping 1 to 1 for 0-3
   */
  reg_val = 0x688;
  SOC_PETRA_FLD_SET(regs->egq.shim_layer_translation_multicast_class_mapping_table_reg.mc_class_map4to8, reg_val, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_queuing_regs_init()",0,0);
}

/*********************************************************************
* NAME:
*     soc_petra_egr_queuing_init
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
  soc_petra_egr_queuing_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    if_idx,
    ofp_idx;
  SOC_PETRA_EGR_FC_DEVICE_THRESH
    dev_conf;
  SOC_PETRA_EGR_FC_DEVICE_THRESH
    exact_dev_conf;
  SOC_PETRA_EGR_FC_CHNIF_THRESH
    chnif_conf;
  SOC_PETRA_EGR_FC_CHNIF_THRESH
    exact_chnif_conf;
  SOC_PETRA_EGR_FC_OFP_THRESH
    ofp_conf;
  SOC_PETRA_EGR_FC_OFP_THRESH
    exact_ofp_conf;
  SOC_PETRA_EGR_DROP_THRESH
    drop_conf;
  SOC_PETRA_EGR_DROP_THRESH
    exact_drop_conf;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_QUEUING_INIT);

  res = soc_petra_egr_queuing_regs_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   *  Device Thresholds
   */
  soc_petra_PETRA_EGR_FC_DEVICE_THRESH_clear(&dev_conf);
  soc_petra_PETRA_EGR_FC_DEVICE_THRESH_clear(&exact_dev_conf);
  dev_conf.global.buffers = 3968;
  dev_conf.global.descriptors = 7936;
  dev_conf.global.words = 1015808 / SOC_SAND_NOF_BITS_IN_UINT32;
  dev_conf.scheduled.buffers = 2688;
  dev_conf.scheduled.descriptors = 7936;
  dev_conf.scheduled.words = 1015808 / SOC_SAND_NOF_BITS_IN_UINT32;
  res = soc_petra_egr_dev_fc_set_unsafe(unit, &dev_conf, &exact_dev_conf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  if (SOC_PETRA_IS_DEV_PETRA_A)
  {
    /*
     *  Channelized NIF thresholds
     */
    soc_petra_PETRA_EGR_FC_CHNIF_THRESH_clear(&chnif_conf);
    soc_petra_PETRA_EGR_FC_CHNIF_THRESH_clear(&exact_chnif_conf);
    for (if_idx = 0; if_idx < SOC_PETRA_NOF_MAC_LANES; ++if_idx)
    {
      chnif_conf.words = 1015808 / SOC_SAND_NOF_BITS_IN_UINT32;
      chnif_conf.packets = 7936;
      res = soc_petra_egr_xaui_spaui_fc_set_unsafe(unit, SOC_PETRA_MAL2NIF_NDX(if_idx), &chnif_conf, &exact_chnif_conf);
      SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);
    }
  }

  /*
   *  OFP Thresholds
   */
  soc_petra_PETRA_EGR_FC_OFP_THRESH_clear(&ofp_conf);
  soc_petra_PETRA_EGR_FC_OFP_THRESH_clear(&exact_ofp_conf);
  ofp_conf.words = 32 / SOC_SAND_NOF_BITS_IN_UINT32;
  res = soc_petra_egr_ofp_fc_set_unsafe(unit, SOC_PETRA_EGR_Q_PRIO_LOW, SOC_PETRA_EGR_PORT_THRESH_TYPE_0, &ofp_conf, &exact_ofp_conf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);

  ofp_conf.words = 21504 / SOC_SAND_NOF_BITS_IN_UINT32;
  res = soc_petra_egr_ofp_fc_set_unsafe(unit, SOC_PETRA_EGR_Q_PRIO_LOW, SOC_PETRA_EGR_PORT_THRESH_TYPE_1, &ofp_conf, &exact_ofp_conf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 18, exit);

  ofp_conf.words = 30720 / SOC_SAND_NOF_BITS_IN_UINT32;
  res = soc_petra_egr_ofp_fc_set_unsafe(unit, SOC_PETRA_EGR_Q_PRIO_LOW, SOC_PETRA_EGR_PORT_THRESH_TYPE_2, &ofp_conf, &exact_ofp_conf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  ofp_conf.words = 61440 / SOC_SAND_NOF_BITS_IN_UINT32;
  res = soc_petra_egr_ofp_fc_set_unsafe(unit, SOC_PETRA_EGR_Q_PRIO_LOW, SOC_PETRA_EGR_PORT_THRESH_TYPE_3, &ofp_conf, &exact_ofp_conf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

  ofp_conf.words = 32 / SOC_SAND_NOF_BITS_IN_UINT32;
  res = soc_petra_egr_ofp_fc_set_unsafe(unit, SOC_PETRA_EGR_Q_PRIO_HIGH, SOC_PETRA_EGR_PORT_THRESH_TYPE_0, &ofp_conf, &exact_ofp_conf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 24, exit);

  ofp_conf.words = 21504 / SOC_SAND_NOF_BITS_IN_UINT32;
  res = soc_petra_egr_ofp_fc_set_unsafe(unit, SOC_PETRA_EGR_Q_PRIO_HIGH, SOC_PETRA_EGR_PORT_THRESH_TYPE_1, &ofp_conf, &exact_ofp_conf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 26, exit);

  ofp_conf.words = 30720 / SOC_SAND_NOF_BITS_IN_UINT32;
  res = soc_petra_egr_ofp_fc_set_unsafe(unit, SOC_PETRA_EGR_Q_PRIO_HIGH, SOC_PETRA_EGR_PORT_THRESH_TYPE_2, &ofp_conf, &exact_ofp_conf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 28, exit);

  ofp_conf.words = 61440 / SOC_SAND_NOF_BITS_IN_UINT32;
  res = soc_petra_egr_ofp_fc_set_unsafe(unit, SOC_PETRA_EGR_Q_PRIO_HIGH, SOC_PETRA_EGR_PORT_THRESH_TYPE_3, &ofp_conf, &exact_ofp_conf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  /*
   *  Egress Drop Thresholds - scheduled
   */
  soc_petra_PETRA_EGR_DROP_THRESH_clear(&drop_conf);
  soc_petra_PETRA_EGR_DROP_THRESH_clear(&exact_drop_conf);
  drop_conf.buffers_avail = 15;
  drop_conf.descriptors_avail = 31;
  drop_conf.packets_consumed = 6144;
  drop_conf.words_consumed = 29696;
  drop_conf.queue_words_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_0] =    1;
  drop_conf.queue_words_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_1] = 28672;
  drop_conf.queue_words_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_2] = 28672;
  drop_conf.queue_words_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_3] = 28672;
  res = soc_petra_egr_sched_drop_set_unsafe(unit, SOC_PETRA_EGR_Q_PRIO_LOW, &drop_conf, &exact_drop_conf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

  drop_conf.buffers_avail = 15;
  drop_conf.descriptors_avail = 31;
  drop_conf.packets_consumed = 6144;
  drop_conf.words_consumed = 29696;
  drop_conf.queue_words_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_0] =     1;
  drop_conf.queue_words_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_1] = 28672;
  drop_conf.queue_words_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_2] = 28672;
  drop_conf.queue_words_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_3] = 28672;
  res = soc_petra_egr_sched_drop_set_unsafe(unit, SOC_PETRA_EGR_Q_PRIO_HIGH, &drop_conf, &exact_drop_conf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);
  
  /*
   *  Egress Drop Thresholds - unscheduled
   */
  drop_conf.buffers_avail = 124;
  drop_conf.descriptors_avail = 248;
  drop_conf.packets_consumed = 2816;
  drop_conf.words_consumed = 0 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_words_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_0] =    32 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_words_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_1] = 73728 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_words_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_2] = 73728 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_words_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_3] = 73728 / SOC_SAND_NOF_BITS_IN_UINT32;
  res = soc_petra_egr_unsched_drop_set_unsafe(unit, SOC_PETRA_EGR_Q_PRIO_LOW, 0, &drop_conf, &exact_drop_conf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 36, exit);

  drop_conf.buffers_avail = 248;
  drop_conf.descriptors_avail = 496;
  drop_conf.packets_consumed = 2048;
  drop_conf.words_consumed = 0 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_words_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_0] =    32 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_words_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_1] = 69632 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_words_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_2] = 69632 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_words_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_3] = 69632 / SOC_SAND_NOF_BITS_IN_UINT32;
  res = soc_petra_egr_unsched_drop_set_unsafe(unit, SOC_PETRA_EGR_Q_PRIO_LOW, 1, &drop_conf, &exact_drop_conf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 38, exit);

  drop_conf.buffers_avail = 496;
  drop_conf.descriptors_avail = 992;
  drop_conf.packets_consumed = 1152;
  drop_conf.words_consumed = 0 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_words_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_0] =    32 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_words_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_1] = 65536 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_words_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_2] = 65536 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_words_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_3] = 65536 / SOC_SAND_NOF_BITS_IN_UINT32;
  res = soc_petra_egr_unsched_drop_set_unsafe(unit, SOC_PETRA_EGR_Q_PRIO_LOW, 2, &drop_conf, &exact_drop_conf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  drop_conf.buffers_avail = 496;
  drop_conf.descriptors_avail = 992;
  drop_conf.packets_consumed = 1152;
  drop_conf.words_consumed = 0 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_words_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_0] =    32 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_words_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_1] = 65536 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_words_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_2] = 65536 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_words_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_3] = 65536 / SOC_SAND_NOF_BITS_IN_UINT32;
  res = soc_petra_egr_unsched_drop_set_unsafe(unit, SOC_PETRA_EGR_Q_PRIO_LOW, 3, &drop_conf, &exact_drop_conf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  drop_conf.buffers_avail = 15;
  drop_conf.descriptors_avail = 31;
  drop_conf.packets_consumed = 3840;
  drop_conf.words_consumed = 0 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_words_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_0] =    32 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_words_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_1] = 73728 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_words_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_2] = 73728 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_words_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_3] = 73728 / SOC_SAND_NOF_BITS_IN_UINT32;
  res = soc_petra_egr_unsched_drop_set_unsafe(unit, SOC_PETRA_EGR_Q_PRIO_HIGH, 0, &drop_conf, &exact_drop_conf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

  drop_conf.buffers_avail = 30;
  drop_conf.descriptors_avail = 62;
  drop_conf.packets_consumed = 2560;
  drop_conf.words_consumed = 0 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_words_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_0] =    32 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_words_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_1] = 69632 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_words_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_2] = 69632 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_words_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_3] = 69632 / SOC_SAND_NOF_BITS_IN_UINT32;
  res = soc_petra_egr_unsched_drop_set_unsafe(unit, SOC_PETRA_EGR_Q_PRIO_HIGH, 1, &drop_conf, &exact_drop_conf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

  drop_conf.buffers_avail = 60;
  drop_conf.descriptors_avail = 124;
  drop_conf.packets_consumed = 1792;
  drop_conf.words_consumed = 0 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_words_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_0] =    32 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_words_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_1] = 65536 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_words_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_2] = 65536 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_words_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_3] = 65536 / SOC_SAND_NOF_BITS_IN_UINT32;
  res = soc_petra_egr_unsched_drop_set_unsafe(unit, SOC_PETRA_EGR_Q_PRIO_HIGH, 2, &drop_conf, &exact_drop_conf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);

  drop_conf.buffers_avail = 60;
  drop_conf.descriptors_avail = 124;
  drop_conf.packets_consumed = 1792;
  drop_conf.words_consumed = 0 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_words_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_0] =    32 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_words_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_1] = 65536 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_words_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_2] = 65536 / SOC_SAND_NOF_BITS_IN_UINT32;
  drop_conf.queue_words_consumed[SOC_PETRA_EGR_PORT_THRESH_TYPE_3] = 65536 / SOC_SAND_NOF_BITS_IN_UINT32;
  res = soc_petra_egr_unsched_drop_set_unsafe(unit, SOC_PETRA_EGR_Q_PRIO_HIGH, 3, &drop_conf, &exact_drop_conf);
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  for (ofp_idx = 0; ofp_idx < SOC_PETRA_NOF_FAP_PORTS; ofp_idx++)
  {
    res = soc_petra_egr_ofp_thresh_type_set_unsafe(
            unit,
            ofp_idx,
            SOC_PETRA_EGR_PORT_THRESH_TYPE_0
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  }

#ifdef LINK_PB_LIBRARIES
  if (SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE)
  {
    res = soc_pb_egr_queuing_init(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 65, exit);
  }
#endif

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_queuing_init()",0,0);
}

/*********************************************************************
* NAME:
*     soc_petra_egr_thresh_to_mnt_exp
* TYPE:
*   PROC
* DATE:
*   Sep 20 2007
* FUNCTION:
*     separate a given threshold to mantissa and exponent
* INPUT:
*   SOC_SAND_IN  uint32   thresh
*     threshold as integer
*   SOC_SAND_OUT uint32   *thresh_mnt,
*   SOC_SAND_OUT uint32   *thresh_exp
*     thresh as multiplication of (thresh_mnt * (2 ^ thresh_exp)).
*     Might cause data loss.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
STATIC
  uint32
    soc_petra_egr_thresh_to_mnt_exp(
      SOC_SAND_IN  uint32   thresh,
      SOC_SAND_OUT uint32   *thresh_mnt,
      SOC_SAND_OUT uint32   *thresh_exp
    )
{
  SOC_SAND_RET
    ret;
  uint32
    mnt_th = 0,
    exp_th = 0;
  SOC_PETRA_REG_FIELD
    thresh_exp_fld,
    thresh_mnt_fld;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_THRESH_TO_MNT_EXP);

  SOC_SAND_CHECK_NULL_INPUT(thresh_mnt);
  SOC_SAND_CHECK_NULL_INPUT(thresh_exp);

  SOC_PETRA_FLD_DEFINE(thresh_exp_fld, 0, SOC_PETRA_EGR_THRESH_EXP_NOF_BITS);
  SOC_PETRA_FLD_DEFINE(thresh_mnt_fld, SOC_PETRA_EGR_THRESH_EXP_NOF_BITS, SOC_PETRA_EGR_THRESH_MNT_NOF_BITS);

  ret = soc_sand_break_to_mnt_exp_round_up(
          thresh,
          SOC_PETRA_FLD_NOF_BITS(thresh_mnt_fld),
          SOC_PETRA_FLD_NOF_BITS(thresh_exp_fld),
          0,
          &mnt_th,
          &exp_th
        );
  SOC_SAND_CHECK_FUNC_RESULT(ret, 10, exit);

  SOC_SAND_LIMIT_FROM_ABOVE(mnt_th, SOC_PETRA_FLD_MAX(thresh_mnt_fld));

  SOC_SAND_LIMIT_FROM_ABOVE(exp_th, SOC_PETRA_FLD_MAX(thresh_exp_fld));

  *thresh_mnt = mnt_th;
  *thresh_exp = exp_th;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_thresh_to_mnt_exp()",0,0);
}

/*********************************************************************
* NAME:
*     soc_petra_egr_mnt_exp_to_thresh
* TYPE:
*   PROC
* DATE:
*   Sep 20 2007
* FUNCTION:
*     Converts threshold given as mantissa and exponent to integer representation.
* INPUT:
*   SOC_SAND_IN  uint32   thresh_mnt,
*   SOC_SAND_IN  uint32   thresh_exp,
*     threshold given as mantissa and exponent
*   SOC_SAND_OUT uint32   *thresh
*     thresh integer
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
STATIC
  uint32
    soc_petra_egr_mnt_exp_to_thresh(
      SOC_SAND_IN  uint32   thresh_mnt,
      SOC_SAND_IN  uint32   thresh_exp,
      SOC_SAND_OUT uint32   *thresh
    )
{
  SOC_PETRA_REG_FIELD
    thresh_exp_fld,
    thresh_mnt_fld;
  uint32
    threshold;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_MNT_EXP_TO_THRESH);

  SOC_SAND_CHECK_NULL_INPUT(thresh);

  SOC_PETRA_FLD_DEFINE(thresh_exp_fld, 0, SOC_PETRA_EGR_THRESH_EXP_NOF_BITS);
  SOC_PETRA_FLD_DEFINE(thresh_mnt_fld, SOC_PETRA_EGR_THRESH_EXP_NOF_BITS, SOC_PETRA_EGR_THRESH_MNT_NOF_BITS);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    thresh_mnt, SOC_PETRA_FLD_MAX(thresh_mnt_fld),
    SOC_PETRA_EGR_MANTISSA_OUT_OF_RANGE_ERR, 10, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    thresh_exp, SOC_PETRA_FLD_MAX(thresh_exp_fld),
    SOC_PETRA_EGR_EXPONENT_OUT_OF_RANGE_ERR, 20, exit
  );

  threshold = thresh_mnt * (1 << thresh_exp);

  *thresh = threshold;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_mnt_exp_to_thresh()",0,0);
}

/*********************************************************************
* NAME:
*     soc_petra_egr_thresh_fld_to_mnt_exp
* TYPE:
*   PROC
* DATE:
*   Sep 20 2007
* FUNCTION:
*   Given threshold value and threshold field buffer,
*   the function translates the threshold
*   to mantissa and exp representation, and puts it in the relevant places in
*   the buffer, according to bits supplies by the field
* INPUT:
*   SOC_SAND_IN  SOC_PETRA_REG_FIELD    *thresh_fld,
*     The field from which the bits are taken
*   SOC_SAND_IN  uint32            thresh,
*     The threshold to put (integer representation)
*   SOC_SAND_OUT uint32            *thresh_fld_val
*     The threshold field, in mantissa-exponent representation,
*     to put the results
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   The function does not clear the value of the thresh_fld_val.
*   This way we can build a register of fields, each one
*   in mantissa-exponent representation.
*********************************************************************/
uint32
  soc_petra_egr_thresh_fld_to_mnt_exp(
    SOC_SAND_IN  SOC_PETRA_REG_FIELD     *thresh_fld,
    SOC_SAND_IN  uint32            thresh,
    SOC_SAND_OUT uint32            *exact_thresh,
    SOC_SAND_OUT uint32            *thresh_fld_val
  )
{
  uint32
    res;
  uint32
    mnt_th = 0,
    exp_th = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_THRESH_FLD_TO_MNT_EXP);

  SOC_SAND_CHECK_NULL_INPUT(thresh_fld);
  SOC_SAND_CHECK_NULL_INPUT(exact_thresh);
  SOC_SAND_CHECK_NULL_INPUT(thresh_fld_val);

  if (SOC_PETRA_FLD_NOF_BITS(*thresh_fld) != SOC_PETRA_EGR_THRESH_NOF_BITS)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_EGR_MNT_EXP_FLD_OUT_OF_RANGE_ERR, 10, exit);
  }

  res = soc_petra_egr_thresh_to_mnt_exp(
          thresh,
          &mnt_th,
          &exp_th
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_egr_mnt_exp_to_thresh(
          mnt_th,
          exp_th,
          exact_thresh
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_sand_bitstream_set_any_field(
          &exp_th,
          thresh_fld->lsb,
          SOC_PETRA_EGR_THRESH_EXP_NOF_BITS,
          thresh_fld_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_sand_bitstream_set_any_field(
          &mnt_th,
          thresh_fld->lsb + SOC_PETRA_EGR_THRESH_EXP_NOF_BITS,
          SOC_PETRA_EGR_THRESH_MNT_NOF_BITS,
          thresh_fld_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_thresh_fld_to_mnt_exp()",0,0);
}

/*********************************************************************
* NAME:
*     soc_petra_egr_mnt_exp_to_thresh_fld
* TYPE:
*   PROC
* DATE:
*   Sep 20 2007
* FUNCTION:
*   Given a threshold field buffer,
*   the function calculates the threshold
*   from the mantissa and exp representation as appears in
*   the buffer, according to bits supplied by the thresh_fld*
* INPUT:
*   SOC_SAND_IN  SOC_PETRA_REG_FIELD *thresh_fld,
*   SOC_SAND_IN  uint32         *thresh_fld_val,
*   SOC_SAND_OUT uint32         *thresh*
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_egr_mnt_exp_to_thresh_fld(
    SOC_SAND_IN  SOC_PETRA_REG_FIELD  *thresh_fld,
    SOC_SAND_IN  uint32         *thresh_fld_val_in,
    SOC_SAND_OUT uint32         *thresh_out
  )
{
  uint32
    res;
  uint32
    mnt_th = 0,
    exp_th = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_MNT_EXP_TO_THRESH_FLD);

  SOC_SAND_CHECK_NULL_INPUT(thresh_fld);
  SOC_SAND_CHECK_NULL_INPUT(thresh_fld_val_in);
  SOC_SAND_CHECK_NULL_INPUT(thresh_out);

  if (SOC_PETRA_FLD_NOF_BITS(*thresh_fld) != SOC_PETRA_EGR_THRESH_NOF_BITS)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_EGR_MNT_EXP_FLD_OUT_OF_RANGE_ERR, 10, exit);
  }

  res = soc_sand_bitstream_get_any_field(
          thresh_fld_val_in,
          thresh_fld->lsb,
          SOC_PETRA_EGR_THRESH_EXP_NOF_BITS,
          &exp_th
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_sand_bitstream_get_any_field(
          thresh_fld_val_in,
          thresh_fld->lsb + SOC_PETRA_EGR_THRESH_EXP_NOF_BITS,
          SOC_PETRA_EGR_THRESH_MNT_NOF_BITS,
          &mnt_th
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_egr_mnt_exp_to_thresh(
          mnt_th,
          exp_th,
          thresh_out
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_mnt_exp_to_thresh_fld()",0,0);
}


/*********************************************************************
*     Sets Outgoing FAP Port (OFP) threshold type, per port.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_ofp_thresh_type_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         ofp_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_PORT_THRESH_TYPE ofp_thresh_type
  )
{
  uint32
    res;
  uint32
    reg_idx = 0,
    fld_idx = 0;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_OFP_THRESH_TYPE_SET_UNSAFE);

  regs = soc_petra_regs();
  /*
   * Each 2 bits correspond to a single OFP
   */
  reg_idx = SOC_PETRA_REG_IDX_GET(ofp_ndx, SOC_PETRA_NOF_OFP_TH_TYPE_SEL_FLDS);
  fld_idx = SOC_PETRA_FLD_IDX_GET(ofp_ndx, SOC_PETRA_NOF_OFP_TH_TYPE_SEL_FLDS);

  SOC_PETRA_FLD_SET(regs->egq.ofp_threshold_type_select_reg[reg_idx].qth_sel[fld_idx], ofp_thresh_type, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_ofp_thresh_type_set_unsafe()",0,0);
}

/*********************************************************************
*     Sets Outgoing FAP Port (OFP) threshold type, per port.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_ofp_thresh_type_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         ofp_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_PORT_THRESH_TYPE ofp_thresh_type
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_OFP_THRESH_TYPE_VERIFY);

  res = soc_petra_fap_port_id_verify(unit, ofp_ndx, TRUE);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    ofp_thresh_type, SOC_PETRA_EGR_PORT_NOF_THRESH_TYPES-1,
    SOC_PETRA_EGR_THRESH_TYPE_OUT_OF_RANGE_ERR, 20, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_ofp_thresh_type_verify()",0,0);
}

/*********************************************************************
*     Sets Outgoing FAP Port (OFP) threshold type, per port.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_ofp_thresh_type_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         ofp_ndx,
    SOC_SAND_OUT SOC_PETRA_EGR_PORT_THRESH_TYPE *ofp_thresh_type
  )
{
  uint32
    reg_val = 0,
    res;
  uint32
    reg_idx = 0,
    fld_idx = 0;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_OFP_THRESH_TYPE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ofp_thresh_type);

  res = soc_petra_fap_port_id_verify(unit, ofp_ndx, TRUE);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  regs = soc_petra_regs();
  /*
   * Each 2 bits correspond to a single OFP
   */
  reg_idx = SOC_PETRA_REG_IDX_GET(ofp_ndx, SOC_PETRA_NOF_OFP_TH_TYPE_SEL_FLDS);
  fld_idx = SOC_PETRA_FLD_IDX_GET(ofp_ndx, SOC_PETRA_NOF_OFP_TH_TYPE_SEL_FLDS);

  SOC_PETRA_FLD_GET(regs->egq.ofp_threshold_type_select_reg[reg_idx].qth_sel[fld_idx], (reg_val), 20, exit);

  *ofp_thresh_type = (SOC_PETRA_EGR_PORT_THRESH_TYPE)reg_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_ofp_thresh_type_get_unsafe()",0,0);
}

/*********************************************************************
*     Set scheduled drop thresholds for egress queues per
*     queue-priority.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_sched_drop_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_DROP_THRESH     *thresh,
    SOC_SAND_OUT SOC_PETRA_EGR_DROP_THRESH     *exact_thresh
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;
  uint32
    thresh_reg = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_SCHED_DROP_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(thresh);
  SOC_SAND_CHECK_NULL_INPUT(exact_thresh);

  regs = soc_petra_regs();

  res = soc_petra_egr_thresh_fld_to_mnt_exp(
           SOC_PETRA_REG_DB_ACC_REF(regs->egq.sched_qs_rsc_consumption_threshs_reg_0[prio_ndx].total_sch_wrds),
           thresh->words_consumed,
           &(exact_thresh->words_consumed),
           &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_egr_thresh_fld_to_mnt_exp(
           SOC_PETRA_REG_DB_ACC_REF(regs->egq.sched_qs_rsc_consumption_threshs_reg_0[prio_ndx].total_sch_pkts),
           thresh->packets_consumed,
           &(exact_thresh->packets_consumed),
           &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  SOC_PETRA_REG_SET(regs->egq.sched_qs_rsc_consumption_threshs_reg_0[prio_ndx], thresh_reg, 30, exit);

  /*
   * Thresh register must be cleared for each new register
   */
  thresh_reg = 0;

  res = soc_petra_egr_thresh_fld_to_mnt_exp(
           SOC_PETRA_REG_DB_ACC_REF(regs->egq.sched_qs_rsc_consumption_threshs_reg_1[prio_ndx].avail_sch_dscs),
           thresh->descriptors_avail,
           &(exact_thresh->descriptors_avail),
           &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_egr_thresh_fld_to_mnt_exp(
           SOC_PETRA_REG_DB_ACC_REF(regs->egq.sched_qs_rsc_consumption_threshs_reg_1[prio_ndx].avail_sch_bufs),
           thresh->buffers_avail,
           &(exact_thresh->buffers_avail),
           &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  SOC_PETRA_REG_SET(regs->egq.sched_qs_rsc_consumption_threshs_reg_1[prio_ndx], thresh_reg, 60, exit);

  /*
   * Thresh register must be cleared for each new register
   */
  thresh_reg = 0;

  res = soc_petra_egr_thresh_fld_to_mnt_exp(
           SOC_PETRA_REG_DB_ACC_REF(regs->egq.sched_qs_rsc_consumption_threshs_reg_2[prio_ndx].sch_queue_wrds_type0),
           thresh->queue_words_consumed[0],
           &(exact_thresh->queue_words_consumed[0]),
           &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  res = soc_petra_egr_thresh_fld_to_mnt_exp(
           SOC_PETRA_REG_DB_ACC_REF(regs->egq.sched_qs_rsc_consumption_threshs_reg_2[prio_ndx].sch_queue_wrds_type1),
           thresh->queue_words_consumed[1],
           &(exact_thresh->queue_words_consumed[1]),
           &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  SOC_PETRA_REG_SET(regs->egq.sched_qs_rsc_consumption_threshs_reg_2[prio_ndx], thresh_reg, 90, exit);

  /*
   * Thresh register must be cleared for each new register
   */
  thresh_reg = 0;

  res = soc_petra_egr_thresh_fld_to_mnt_exp(
           SOC_PETRA_REG_DB_ACC_REF(regs->egq.sched_qs_rsc_consumption_threshs_reg_3[prio_ndx].sch_queue_wrds_type2),
           thresh->queue_words_consumed[2],
           &(exact_thresh->queue_words_consumed[2]),
           &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  res = soc_petra_egr_thresh_fld_to_mnt_exp(
           SOC_PETRA_REG_DB_ACC_REF(regs->egq.sched_qs_rsc_consumption_threshs_reg_3[prio_ndx].sch_queue_wrds_type3),
           thresh->queue_words_consumed[3],
           &(exact_thresh->queue_words_consumed[3]),
           &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

  SOC_PETRA_REG_SET(regs->egq.sched_qs_rsc_consumption_threshs_reg_3[prio_ndx], thresh_reg, 120, exit);

#ifdef LINK_PB_LIBRARIES
  if (SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE)
  {
    /*
     * Thresh register must be cleared for each new register
     */
    thresh_reg = 0;

    res = soc_petra_egr_thresh_fld_to_mnt_exp(
             SOC_PB_REG_DB_ACC_REF(regs->egq.sched_qs_rsc_consumption_threshs_reg_4[prio_ndx].sch_queue_pkts_type0),
             thresh->queue_pkts_consumed[0],
             &(exact_thresh->queue_pkts_consumed[0]),
             &thresh_reg
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);

    res = soc_petra_egr_thresh_fld_to_mnt_exp(
             SOC_PB_REG_DB_ACC_REF(regs->egq.sched_qs_rsc_consumption_threshs_reg_4[prio_ndx].sch_queue_pkts_type1),
             thresh->queue_pkts_consumed[1],
             &(exact_thresh->queue_pkts_consumed[1]),
             &thresh_reg
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);

    SOC_PB_REG_SET(regs->egq.sched_qs_rsc_consumption_threshs_reg_4[prio_ndx], thresh_reg, 150, exit);


    /*
     * Thresh register must be cleared for each new register
     */
    thresh_reg = 0;

    res = soc_petra_egr_thresh_fld_to_mnt_exp(
             SOC_PB_REG_DB_ACC_REF(regs->egq.sched_qs_rsc_consumption_threshs_reg_5[prio_ndx].sch_queue_pkts_type2),
             thresh->queue_pkts_consumed[2],
             &(exact_thresh->queue_pkts_consumed[2]),
             &thresh_reg
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);

    res = soc_petra_egr_thresh_fld_to_mnt_exp(
             SOC_PB_REG_DB_ACC_REF(regs->egq.sched_qs_rsc_consumption_threshs_reg_5[prio_ndx].sch_queue_pkts_type3),
             thresh->queue_pkts_consumed[3],
             &(exact_thresh->queue_pkts_consumed[3]),
             &thresh_reg
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);

    SOC_PB_REG_SET(regs->egq.sched_qs_rsc_consumption_threshs_reg_5[prio_ndx], thresh_reg, 150, exit);
  }
#endif
  /*
   * Thresh register must be cleared for each new register
   */
  thresh_reg = 0;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_sched_drop_set_unsafe()",0,0);
}

/*********************************************************************
*     Set scheduled drop thresholds for egress queues per
*     queue-priority.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_sched_drop_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_DROP_THRESH     *thresh
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_SCHED_DROP_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(thresh);

  SOC_SAND_MAGIC_NUM_VERIFY(thresh);
  SOC_SAND_ERR_IF_ABOVE_MAX(
    prio_ndx, SOC_PETRA_EGR_NOF_Q_PRIO-1,
    SOC_PETRA_EGR_Q_PRIO_OUT_OF_RANGE_ERR, 10, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_sched_drop_verify()",0,0);
}

/*********************************************************************
*     Set scheduled drop thresholds for egress queues per
*     queue-priority.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_sched_drop_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_OUT SOC_PETRA_EGR_DROP_THRESH     *thresh
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;
  uint32
    thresh_reg = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_SCHED_DROP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(thresh);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    prio_ndx, SOC_PETRA_EGR_NOF_Q_PRIO-1,
    SOC_PETRA_EGR_Q_PRIO_OUT_OF_RANGE_ERR, 10, exit
  );

  regs = soc_petra_regs();

  SOC_PETRA_REG_GET(regs->egq.sched_qs_rsc_consumption_threshs_reg_0[prio_ndx], thresh_reg, 20, exit);

  res = soc_petra_egr_mnt_exp_to_thresh_fld(
           SOC_PETRA_REG_DB_ACC_REF(regs->egq.sched_qs_rsc_consumption_threshs_reg_0[prio_ndx].total_sch_wrds),
           &thresh_reg,
           &(thresh->words_consumed)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_egr_mnt_exp_to_thresh_fld(
           SOC_PETRA_REG_DB_ACC_REF(regs->egq.sched_qs_rsc_consumption_threshs_reg_0[prio_ndx].total_sch_pkts),
           &thresh_reg,
           &(thresh->packets_consumed)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);


  SOC_PETRA_REG_GET(regs->egq.sched_qs_rsc_consumption_threshs_reg_1[prio_ndx], thresh_reg, 50, exit);

  res = soc_petra_egr_mnt_exp_to_thresh_fld(
           SOC_PETRA_REG_DB_ACC_REF(regs->egq.sched_qs_rsc_consumption_threshs_reg_1[prio_ndx].avail_sch_dscs),
           &thresh_reg,
           &(thresh->descriptors_avail)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  res = soc_petra_egr_mnt_exp_to_thresh_fld(
           SOC_PETRA_REG_DB_ACC_REF(regs->egq.sched_qs_rsc_consumption_threshs_reg_1[prio_ndx].avail_sch_bufs),
           &thresh_reg,
           &(thresh->buffers_avail)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  SOC_PETRA_REG_GET(regs->egq.sched_qs_rsc_consumption_threshs_reg_2[prio_ndx], thresh_reg, 80, exit);

  res = soc_petra_egr_mnt_exp_to_thresh_fld(
           SOC_PETRA_REG_DB_ACC_REF(regs->egq.sched_qs_rsc_consumption_threshs_reg_2[prio_ndx].sch_queue_wrds_type0),
           &thresh_reg,
           &(thresh->queue_words_consumed[0])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

  res = soc_petra_egr_mnt_exp_to_thresh_fld(
           SOC_PETRA_REG_DB_ACC_REF(regs->egq.sched_qs_rsc_consumption_threshs_reg_2[prio_ndx].sch_queue_wrds_type1),
           &thresh_reg,
           &(thresh->queue_words_consumed[1])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  SOC_PETRA_REG_GET(regs->egq.sched_qs_rsc_consumption_threshs_reg_3[prio_ndx], thresh_reg, 110, exit);

  res = soc_petra_egr_mnt_exp_to_thresh_fld(
           SOC_PETRA_REG_DB_ACC_REF(regs->egq.sched_qs_rsc_consumption_threshs_reg_3[prio_ndx].sch_queue_wrds_type2),
           &thresh_reg,
           &(thresh->queue_words_consumed[2])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);

  res = soc_petra_egr_mnt_exp_to_thresh_fld(
           SOC_PETRA_REG_DB_ACC_REF(regs->egq.sched_qs_rsc_consumption_threshs_reg_3[prio_ndx].sch_queue_wrds_type3),
           &thresh_reg,
           &(thresh->queue_words_consumed[3])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);

#ifdef LINK_PB_LIBRARIES
  if (SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE)
  {
    SOC_PB_REG_GET(regs->egq.sched_qs_rsc_consumption_threshs_reg_4[prio_ndx], thresh_reg, 120, exit);

    res = soc_petra_egr_mnt_exp_to_thresh_fld(
      SOC_PB_REG_DB_ACC_REF(regs->egq.sched_qs_rsc_consumption_threshs_reg_4[prio_ndx].sch_queue_pkts_type0),
      &thresh_reg,
      &(thresh->queue_pkts_consumed[0])
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);

    res = soc_petra_egr_mnt_exp_to_thresh_fld(
      SOC_PB_REG_DB_ACC_REF(regs->egq.sched_qs_rsc_consumption_threshs_reg_4[prio_ndx].sch_queue_pkts_type1),
      &thresh_reg,
      &(thresh->queue_pkts_consumed[1])
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

    SOC_PB_REG_GET(regs->egq.sched_qs_rsc_consumption_threshs_reg_5[prio_ndx], thresh_reg, 160, exit);

    res = soc_petra_egr_mnt_exp_to_thresh_fld(
      SOC_PB_REG_DB_ACC_REF(regs->egq.sched_qs_rsc_consumption_threshs_reg_5[prio_ndx].sch_queue_pkts_type2),
      &thresh_reg,
      &(thresh->queue_pkts_consumed[2])
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);

    res = soc_petra_egr_mnt_exp_to_thresh_fld(
      SOC_PB_REG_DB_ACC_REF(regs->egq.sched_qs_rsc_consumption_threshs_reg_5[prio_ndx].sch_queue_pkts_type3),
      &thresh_reg,
      &(thresh->queue_pkts_consumed[3])
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);
  }
#endif


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_sched_drop_get_unsafe()",0,0);
}

/*********************************************************************
*     Set unscheduled drop thresholds for egress queues, per
*     queue-priority and drop precedence.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pa_egr_unsched_drop_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  uint32                 dp_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_DROP_THRESH     *thresh,
    SOC_SAND_OUT SOC_PETRA_EGR_DROP_THRESH     *exact_thresh
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;
  uint32
    thresh_reg = 0;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PA_EGR_UNSCHED_DROP_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(thresh);
  SOC_SAND_CHECK_NULL_INPUT(exact_thresh);

  regs = soc_petra_regs();

  res = soc_petra_egr_thresh_fld_to_mnt_exp(
           SOC_PA_REG_DB_ACC_REF(regs->egq.unsched_qs_threshs_for_dp_reg_0[prio_ndx][dp_ndx].total_usc_wrds),
           thresh->words_consumed,
           &(exact_thresh->words_consumed),
           &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_egr_thresh_fld_to_mnt_exp(
           SOC_PA_REG_DB_ACC_REF(regs->egq.unsched_qs_threshs_for_dp_reg_0[prio_ndx][dp_ndx].total_usc_pkts),
           thresh->packets_consumed,
           &(exact_thresh->packets_consumed),
           &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  SOC_PA_REG_SET(regs->egq.unsched_qs_threshs_for_dp_reg_0[prio_ndx][dp_ndx], thresh_reg, 30, exit);

  res = soc_petra_egr_thresh_fld_to_mnt_exp(
           SOC_PA_REG_DB_ACC_REF(regs->egq.unsched_qs_threshs_for_dp_reg_1[prio_ndx][dp_ndx].avail_usc_desc),
           thresh->descriptors_avail,
           &(exact_thresh->descriptors_avail),
           &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_egr_thresh_fld_to_mnt_exp(
           SOC_PA_REG_DB_ACC_REF(regs->egq.unsched_qs_threshs_for_dp_reg_1[prio_ndx][dp_ndx].avail_usc_bufs),
           thresh->buffers_avail,
           &(exact_thresh->buffers_avail),
           &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  SOC_PA_REG_SET(regs->egq.unsched_qs_threshs_for_dp_reg_1[prio_ndx][dp_ndx], thresh_reg, 60, exit);

  /*
   * Thresh register must be cleared for each new register
   */
  thresh_reg = 0;

  res = soc_petra_egr_thresh_fld_to_mnt_exp(
           SOC_PA_REG_DB_ACC_REF(regs->egq.unsched_qs_threshs_for_dp_reg_2[prio_ndx][dp_ndx].uch_queue_wrds_type0),
           thresh->queue_words_consumed[0],
           &(exact_thresh->queue_words_consumed[0]),
           &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  res = soc_petra_egr_thresh_fld_to_mnt_exp(
           SOC_PA_REG_DB_ACC_REF(regs->egq.unsched_qs_threshs_for_dp_reg_2[prio_ndx][dp_ndx].uch_queue_wrds_type1),
           thresh->queue_words_consumed[1],
           &(exact_thresh->queue_words_consumed[1]),
           &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  SOC_PA_REG_SET(regs->egq.unsched_qs_threshs_for_dp_reg_2[prio_ndx][dp_ndx], thresh_reg, 90, exit);

  /*
   * Thresh register must be cleared for each new register
   */
  thresh_reg = 0;

  res = soc_petra_egr_thresh_fld_to_mnt_exp(
           SOC_PA_REG_DB_ACC_REF(regs->egq.unsched_qs_threshs_for_dp_reg_3[prio_ndx][dp_ndx].uch_queue_wrds_type2),
           thresh->queue_words_consumed[2],
           &(exact_thresh->queue_words_consumed[2]),
           &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  res = soc_petra_egr_thresh_fld_to_mnt_exp(
           SOC_PA_REG_DB_ACC_REF(regs->egq.unsched_qs_threshs_for_dp_reg_3[prio_ndx][dp_ndx].uch_queue_wrds_type3),
           thresh->queue_words_consumed[3],
           &(exact_thresh->queue_words_consumed[3]),
           &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

  SOC_PA_REG_SET(regs->egq.unsched_qs_threshs_for_dp_reg_3[prio_ndx][dp_ndx], thresh_reg, 120, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_egr_unsched_drop_set_unsafe()",0,0);
}

uint32
  soc_petra_egr_unsched_drop_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  uint32                 dp_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_DROP_THRESH     *thresh,
    SOC_SAND_OUT SOC_PETRA_EGR_DROP_THRESH     *exact_thresh
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_UNSCHED_DROP_SET_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(egr_unsched_drop_set_unsafe,(unit, prio_ndx, dp_ndx, thresh, exact_thresh));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_unsched_drop_set_unsafe()",0,0);
}

/*********************************************************************
*     Set unscheduled drop thresholds for egress queues, per
*     queue-priority and drop precedence.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_unsched_drop_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  uint32                 dp_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_DROP_THRESH     *thresh
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_UNSCHED_DROP_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(thresh);
  SOC_SAND_MAGIC_NUM_VERIFY(thresh);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    prio_ndx, SOC_PETRA_EGR_NOF_Q_PRIO-1,
    SOC_PETRA_EGR_Q_PRIO_OUT_OF_RANGE_ERR, 10, exit
  );

  res = soc_petra_drop_precedence_verify(dp_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_unsched_drop_verify()",0,0);
}

/*********************************************************************
*     Set unscheduled drop thresholds for egress queues, per
*     queue-priority and drop precedence.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pa_egr_unsched_drop_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  uint32                 dp_ndx,
    SOC_SAND_OUT SOC_PETRA_EGR_DROP_THRESH     *thresh
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;
  uint32
    thresh_reg = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_PA_EGR_UNSCHED_DROP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(thresh);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    prio_ndx, SOC_PETRA_EGR_NOF_Q_PRIO-1,
    SOC_PETRA_EGR_Q_PRIO_OUT_OF_RANGE_ERR, 10, exit
  );

  res = soc_petra_drop_precedence_verify(dp_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  regs = soc_petra_regs();

  SOC_PA_REG_GET(regs->egq.unsched_qs_threshs_for_dp_reg_0[prio_ndx][dp_ndx], thresh_reg, 30, exit);

  res = soc_petra_egr_mnt_exp_to_thresh_fld(
          SOC_PA_REG_DB_ACC_REF(regs->egq.unsched_qs_threshs_for_dp_reg_0[prio_ndx][dp_ndx].total_usc_wrds),
          &thresh_reg,
          &(thresh->words_consumed)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_egr_mnt_exp_to_thresh_fld(
          SOC_PA_REG_DB_ACC_REF(regs->egq.unsched_qs_threshs_for_dp_reg_0[prio_ndx][dp_ndx].total_usc_pkts),
          &thresh_reg,
          &(thresh->packets_consumed)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  SOC_PA_REG_GET(regs->egq.unsched_qs_threshs_for_dp_reg_1[prio_ndx][dp_ndx], thresh_reg, 60, exit);

  res = soc_petra_egr_mnt_exp_to_thresh_fld(
          SOC_PA_REG_DB_ACC_REF(regs->egq.unsched_qs_threshs_for_dp_reg_1[prio_ndx][dp_ndx].avail_usc_desc),
          &thresh_reg,
          &(thresh->descriptors_avail)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  res = soc_petra_egr_mnt_exp_to_thresh_fld(
          SOC_PA_REG_DB_ACC_REF(regs->egq.unsched_qs_threshs_for_dp_reg_1[prio_ndx][dp_ndx].avail_usc_bufs),
          &thresh_reg,
          &(thresh->buffers_avail)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);


  SOC_PA_REG_GET(regs->egq.unsched_qs_threshs_for_dp_reg_2[prio_ndx][dp_ndx], thresh_reg, 90, exit);

  res = soc_petra_egr_mnt_exp_to_thresh_fld(
          SOC_PA_REG_DB_ACC_REF(regs->egq.unsched_qs_threshs_for_dp_reg_2[prio_ndx][dp_ndx].uch_queue_wrds_type0),
          &thresh_reg,
          &(thresh->queue_words_consumed[0])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  res = soc_petra_egr_mnt_exp_to_thresh_fld(
          SOC_PA_REG_DB_ACC_REF(regs->egq.unsched_qs_threshs_for_dp_reg_2[prio_ndx][dp_ndx].uch_queue_wrds_type1),
          &thresh_reg,
          &(thresh->queue_words_consumed[1])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);


  SOC_PA_REG_GET(regs->egq.unsched_qs_threshs_for_dp_reg_3[prio_ndx][dp_ndx], thresh_reg, 120, exit);

  res = soc_petra_egr_mnt_exp_to_thresh_fld(
          SOC_PA_REG_DB_ACC_REF(regs->egq.unsched_qs_threshs_for_dp_reg_3[prio_ndx][dp_ndx].uch_queue_wrds_type2),
          &thresh_reg,
          &(thresh->queue_words_consumed[2])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);

  res = soc_petra_egr_mnt_exp_to_thresh_fld(
          SOC_PA_REG_DB_ACC_REF(regs->egq.unsched_qs_threshs_for_dp_reg_3[prio_ndx][dp_ndx].uch_queue_wrds_type3),
          &thresh_reg,
          &(thresh->queue_words_consumed[3])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_egr_unsched_drop_get_unsafe()",0,0);
}

uint32
  soc_petra_egr_unsched_drop_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  uint32                 dp_ndx,
    SOC_SAND_OUT SOC_PETRA_EGR_DROP_THRESH     *thresh
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_UNSCHED_DROP_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(thresh);

  SOC_PETRA_DIFF_DEVICE_CALL(egr_unsched_drop_get_unsafe,(unit, prio_ndx, dp_ndx, thresh));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_unsched_drop_get_unsafe()",0,0);
}

/*********************************************************************
*     Set Flow Control thresholds for egress queues, based on
*     device-level resources. Threshold are set for overall
*     resources, and scheduled resources.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_dev_fc_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_FC_DEVICE_THRESH *thresh,
    SOC_SAND_OUT SOC_PETRA_EGR_FC_DEVICE_THRESH *exact_thresh
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;
  uint32
    thresh_reg = 0;
  uint32
    thresh_fld = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_DEV_FC_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(thresh);
  SOC_SAND_CHECK_NULL_INPUT(exact_thresh);

  regs = soc_petra_regs();

  res = soc_petra_egr_thresh_fld_to_mnt_exp(
           SOC_PETRA_REG_DB_ACC_REF(regs->egq.total_rcs_consumption_threshs_conf_for_dev_flow_ctrl_reg_1.total_dvc_fc_wrds),
           thresh->global.words,
           &(exact_thresh->global.words),
           &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_PETRA_FLD_FROM_REG(regs->egq.total_rcs_consumption_threshs_conf_for_dev_flow_ctrl_reg_1.total_dvc_fc_wrds, thresh_fld, thresh_reg, 20, exit);

  SOC_PETRA_FLD_SET(regs->egq.total_rcs_consumption_threshs_conf_for_dev_flow_ctrl_reg_1.total_dvc_fc_wrds, thresh_fld, 30, exit);

  /*
   * Thresh register must be cleared for each new register
   */
  thresh_reg = 0;

  res = soc_petra_egr_thresh_fld_to_mnt_exp(
           SOC_PETRA_REG_DB_ACC_REF(regs->egq.total_rcs_consumption_threshs_conf_for_dev_flow_ctrl_reg_2.total_dvc_fc_bufs),
           thresh->global.buffers,
           &(exact_thresh->global.buffers),
           &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_egr_thresh_fld_to_mnt_exp(
           SOC_PETRA_REG_DB_ACC_REF(regs->egq.total_rcs_consumption_threshs_conf_for_dev_flow_ctrl_reg_2.total_dvc_fc_dscs),
           thresh->global.descriptors,
           &(exact_thresh->global.descriptors),
           &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  SOC_PETRA_REG_SET(regs->egq.total_rcs_consumption_threshs_conf_for_dev_flow_ctrl_reg_2, thresh_reg, 60, exit);

  /* Scheduled { */

  res = soc_petra_egr_thresh_fld_to_mnt_exp(
           SOC_PETRA_REG_DB_ACC_REF(regs->egq.total_rcs_consumption_threshs_conf_for_dev_flow_ctrl_reg_0.total_dvc_fc_sch_bufs),
           thresh->scheduled.buffers,
           &(exact_thresh->scheduled.buffers),
           &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);

  res = soc_petra_egr_thresh_fld_to_mnt_exp(
           SOC_PETRA_REG_DB_ACC_REF(regs->egq.total_rcs_consumption_threshs_conf_for_dev_flow_ctrl_reg_0.total_dvc_fc_sch_wrds),
           thresh->scheduled.words,
           &(exact_thresh->scheduled.words),
           &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);

  SOC_PETRA_REG_SET(regs->egq.total_rcs_consumption_threshs_conf_for_dev_flow_ctrl_reg_0, thresh_reg, 150, exit);

  /*
   * Thresh register must be cleared for each new register
   */
  thresh_reg = 0;

  res = soc_petra_egr_thresh_fld_to_mnt_exp(
           SOC_PETRA_REG_DB_ACC_REF(regs->egq.total_rcs_consumption_threshs_conf_for_dev_flow_ctrl_reg_1.total_dvc_fc_sch_dscs),
           thresh->scheduled.descriptors,
           &(exact_thresh->scheduled.descriptors),
           &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);

  SOC_PETRA_FLD_FROM_REG(regs->egq.total_rcs_consumption_threshs_conf_for_dev_flow_ctrl_reg_1.total_dvc_fc_sch_dscs, thresh_fld, thresh_reg, 170, exit);

  SOC_PETRA_FLD_SET(regs->egq.total_rcs_consumption_threshs_conf_for_dev_flow_ctrl_reg_1.total_dvc_fc_sch_dscs, thresh_fld, 180, exit);

  /* Scheduled } */


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_dev_fc_set_unsafe()",0,0);
}

/*********************************************************************
*     Set Flow Control thresholds for egress queues, based on
*     device-level resources. Threshold are set for overall
*     resources, and scheduled resources.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_dev_fc_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_FC_DEVICE_THRESH *thresh
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_DEV_FC_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(thresh);
  SOC_SAND_MAGIC_NUM_VERIFY(thresh);
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_dev_fc_verify()",0,0);
}

/*********************************************************************
*     Set Flow Control thresholds for egress queues, based on
*     device-level resources. Threshold are set for overall
*     resources, and scheduled resources.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_dev_fc_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_EGR_FC_DEVICE_THRESH *thresh
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;
  uint32
    thresh_reg = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_DEV_FC_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(thresh);

  regs = soc_petra_regs();

  SOC_PETRA_REG_GET(regs->egq.total_rcs_consumption_threshs_conf_for_dev_flow_ctrl_reg_1, thresh_reg, 10, exit);

  res = soc_petra_egr_mnt_exp_to_thresh_fld(
          SOC_PETRA_REG_DB_ACC_REF(regs->egq.total_rcs_consumption_threshs_conf_for_dev_flow_ctrl_reg_1.total_dvc_fc_wrds),
          &thresh_reg,
          &(thresh->global.words)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);


  SOC_PETRA_REG_GET(regs->egq.total_rcs_consumption_threshs_conf_for_dev_flow_ctrl_reg_2, thresh_reg, 30, exit);

  res = soc_petra_egr_mnt_exp_to_thresh_fld(
          SOC_PETRA_REG_DB_ACC_REF(regs->egq.total_rcs_consumption_threshs_conf_for_dev_flow_ctrl_reg_2.total_dvc_fc_bufs),
          &thresh_reg,
          &(thresh->global.buffers)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_egr_mnt_exp_to_thresh_fld(
          SOC_PETRA_REG_DB_ACC_REF(regs->egq.total_rcs_consumption_threshs_conf_for_dev_flow_ctrl_reg_2.total_dvc_fc_dscs),
          &thresh_reg,
          &(thresh->global.descriptors)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  /* Scheduled { */

  SOC_PETRA_REG_GET(regs->egq.total_rcs_consumption_threshs_conf_for_dev_flow_ctrl_reg_0, thresh_reg, 140, exit);

  res = soc_petra_egr_mnt_exp_to_thresh_fld(
           SOC_PETRA_REG_DB_ACC_REF(regs->egq.total_rcs_consumption_threshs_conf_for_dev_flow_ctrl_reg_0.total_dvc_fc_sch_bufs),
           &thresh_reg,
           &(thresh->scheduled.buffers)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

  res = soc_petra_egr_mnt_exp_to_thresh_fld(
           SOC_PETRA_REG_DB_ACC_REF(regs->egq.total_rcs_consumption_threshs_conf_for_dev_flow_ctrl_reg_0.total_dvc_fc_sch_wrds),
           &thresh_reg,
           &(thresh->scheduled.words)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);

  SOC_PETRA_REG_GET(regs->egq.total_rcs_consumption_threshs_conf_for_dev_flow_ctrl_reg_1, thresh_reg, 170, exit);

  res = soc_petra_egr_mnt_exp_to_thresh_fld(
           SOC_PETRA_REG_DB_ACC_REF(regs->egq.total_rcs_consumption_threshs_conf_for_dev_flow_ctrl_reg_1.total_dvc_fc_sch_dscs),
           &thresh_reg,
           &(thresh->scheduled.descriptors)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);

  /* Scheduled } */


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_dev_fc_get_unsafe()",0,0);
}

/*********************************************************************
*     Set Flow Control thresholds for egress queues, per
*     channelized interface port, based on Channelized NIF
*     Ports resources.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pa_egr_xaui_spaui_fc_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        if_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_FC_CHNIF_THRESH *thresh,
    SOC_SAND_OUT SOC_PETRA_EGR_FC_CHNIF_THRESH *exact_thresh
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_REG_FIELD
    *fld;
  uint32
    thresh_reg = 0;
  uint32
    thresh_fld = 0;
  uint32
    reg_idx = 0,
    fld_idx = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_EGR_CHNIF_FC_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(thresh);
  SOC_SAND_CHECK_NULL_INPUT(exact_thresh);

  regs = soc_petra_regs();

  if (SOC_PETRA_IS_CPU_IF_ID(if_ndx))
  {
    reg_idx = SOC_PETRA_NOF_CH_PORTS_TOTAL_FC_THRESH_REGS - 1; /* 4 */
    fld_idx = 0;
  }
  else if (SOC_PETRA_IS_RCY_IF_ID(if_ndx))
  {
    reg_idx = SOC_PETRA_NOF_CH_PORTS_TOTAL_FC_THRESH_REGS - 1; /* 4 */
    fld_idx = SOC_PETRA_NOF_CH_PORTS_TOTAL_FC_THRESH_FLDS - 1; /* 1 */
  }
  else if (soc_petra_is_channelized_nif_id(if_ndx))
  {
    reg_idx =
        ((if_ndx/SOC_PETRA_MAX_NIFS_PER_MAL)/SOC_PETRA_NOF_CH_PORTS_TOTAL_FC_THRESH_FLDS);
    fld_idx =
      ((if_ndx/SOC_PETRA_MAX_NIFS_PER_MAL)%SOC_PETRA_NOF_CH_PORTS_TOTAL_FC_THRESH_FLDS);
  }
  else
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_INTERFACE_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  }

  fld = SOC_PA_REG_DB_ACC_REF(regs->egq.channelized_ports_total_packets_fc_thresh_reg[reg_idx].total_cp_npkts_th[fld_idx]);

  res = soc_petra_egr_thresh_fld_to_mnt_exp(
          fld,
          thresh->packets,
          &(exact_thresh->packets),
          &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  SOC_PA_IMPLICIT_FLD_FROM_REG(*fld, thresh_fld, thresh_reg, 30, exit);

  SOC_PA_IMPLICIT_FLD_SET(*fld, thresh_fld, 40, exit);

  fld = SOC_PA_REG_DB_ACC_REF(regs->egq.channelized_ports_total_words_fc_thresh_reg[reg_idx].total_cp_nwrds_th[fld_idx]);

  res = soc_petra_egr_thresh_fld_to_mnt_exp(
           fld,
           thresh->words,
           &(exact_thresh->words),
           &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  SOC_PA_IMPLICIT_FLD_FROM_REG(*fld, thresh_fld, thresh_reg, 60, exit);

  SOC_PA_IMPLICIT_FLD_SET(*fld, thresh_fld, 70, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_egr_xaui_spaui_fc_set_unsafe()",0,0);
}

uint32
  soc_petra_egr_xaui_spaui_fc_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        if_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_FC_CHNIF_THRESH *thresh,
    SOC_SAND_OUT SOC_PETRA_EGR_FC_CHNIF_THRESH *exact_thresh
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_CHNIF_FC_SET_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(egr_xaui_spaui_fc_set_unsafe,(unit, if_ndx, thresh, exact_thresh));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_xaui_spaui_fc_set_unsafe()",0,0);
}

/*********************************************************************
*     Set Flow Control thresholds for egress queues, per
*     channelized interface port, based on Channelized NIF
*     Ports resources.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pa_egr_xaui_spaui_fc_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        if_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_FC_CHNIF_THRESH *thresh
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_EGR_CHNIF_FC_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(thresh);

  SOC_SAND_MAGIC_NUM_VERIFY(thresh);

  res = soc_petra_interface_id_verify(unit, if_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (soc_petra_is_channelized_nif_id(if_ndx) == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_NOT_A_CHANNELIZED_IF_ERR, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_egr_xaui_spaui_fc_verify()",0,0);
}

uint32
  soc_petra_egr_xaui_spaui_fc_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        if_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_FC_CHNIF_THRESH *thresh
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_CHNIF_FC_VERIFY);

  SOC_PETRA_DIFF_DEVICE_CALL(egr_xaui_spaui_fc_verify,(unit, if_ndx, thresh));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_xaui_spaui_fc_verify()",0,0);
}

/*********************************************************************
*     Set Flow Control thresholds for egress queues, per
*     channelized interface port, based on Channelized NIF
*     Ports resources.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pa_egr_xaui_spaui_fc_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        if_ndx,
    SOC_SAND_OUT SOC_PETRA_EGR_FC_CHNIF_THRESH *thresh
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_REG_FIELD
    *fld;
  uint32
    thresh_reg = 0;
  uint32
    reg_idx = 0,
    fld_idx = 0;
  SOC_PETRA_REG_ADDR
    *addr = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_EGR_CHNIF_FC_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(thresh);

  res = soc_petra_interface_id_verify(unit, if_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (soc_petra_is_channelized_interface_id(if_ndx) == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_NOT_A_CHANNELIZED_IF_ERR, 20, exit);
  }

  regs = soc_petra_regs();

  if (SOC_PETRA_IS_CPU_IF_ID(if_ndx))
  {
    reg_idx = SOC_PETRA_NOF_CH_PORTS_TOTAL_FC_THRESH_REGS - 1; /* 4 */
    fld_idx = 0;
  } else if (SOC_PETRA_IS_RCY_IF_ID(if_ndx))
  {
    reg_idx = SOC_PETRA_NOF_CH_PORTS_TOTAL_FC_THRESH_REGS - 1; /* 4 */
    fld_idx = SOC_PETRA_NOF_CH_PORTS_TOTAL_FC_THRESH_FLDS - 1; /* 1 */
  } else if (soc_petra_is_channelized_nif_id(if_ndx))
  {
    reg_idx =
        ((if_ndx/SOC_PETRA_MAX_NIFS_PER_MAL)/SOC_PETRA_NOF_CH_PORTS_TOTAL_FC_THRESH_FLDS);
    fld_idx =
      ((if_ndx/SOC_PETRA_MAX_NIFS_PER_MAL)%SOC_PETRA_NOF_CH_PORTS_TOTAL_FC_THRESH_FLDS);
  } else
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_INTERFACE_TYPE_OUT_OF_RANGE_ERR, 10, exit);
  }

   fld = SOC_PA_REG_DB_ACC_REF(regs->egq.channelized_ports_total_packets_fc_thresh_reg[reg_idx].total_cp_npkts_th[fld_idx]);
   addr = SOC_PA_REG_DB_ACC_REF(regs->egq.channelized_ports_total_packets_fc_thresh_reg[reg_idx].addr);

   res = soc_petra_read_reg_unsafe(
          unit,
          &(*addr),
          SOC_PETRA_DEFAULT_INSTANCE,
          &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

   res = soc_petra_egr_mnt_exp_to_thresh_fld(
           fld,
           &thresh_reg,
           &(thresh->packets)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  fld = SOC_PA_REG_DB_ACC_REF(regs->egq.channelized_ports_total_words_fc_thresh_reg[reg_idx].total_cp_nwrds_th[fld_idx]);
  addr = SOC_PA_REG_DB_ACC_REF(regs->egq.channelized_ports_total_words_fc_thresh_reg[reg_idx].addr);
  res = soc_petra_read_reg_unsafe(
          unit,
          addr,
          SOC_PETRA_DEFAULT_INSTANCE,
          &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  res = soc_petra_egr_mnt_exp_to_thresh_fld(
           fld,
           &thresh_reg,
           &(thresh->words)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_egr_xaui_spaui_fc_get_unsafe()",0,0);
}

uint32
  soc_petra_egr_xaui_spaui_fc_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        if_ndx,
    SOC_SAND_OUT SOC_PETRA_EGR_FC_CHNIF_THRESH *thresh
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_CHNIF_FC_GET_UNSAFE);
  
  SOC_PETRA_DIFF_DEVICE_CALL(egr_xaui_spaui_fc_get_unsafe,(unit, if_ndx, thresh));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_xaui_spaui_fc_get_unsafe()",0,0);
}

/*********************************************************************
*     Set Flow Control thresholds for egress queues, per port
*     queue priority and threshold type, based on Outgoing FAP
*     Port (OFP) resources.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_ofp_fc_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_PORT_THRESH_TYPE ofp_type_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_FC_OFP_THRESH   *thresh,
    SOC_SAND_OUT SOC_PETRA_EGR_FC_OFP_THRESH   *exact_thresh
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_REG_FIELD
    *fld;
  uint32
    thresh_reg = 0,
    thresh_fld = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_OFP_FC_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(thresh);
  SOC_SAND_CHECK_NULL_INPUT(exact_thresh);

  regs = soc_petra_regs();

  switch(ofp_type_ndx)
  {
  case SOC_PETRA_EGR_PORT_THRESH_TYPE_0:
    fld = SOC_PETRA_REG_DB_ACC_REF(regs->egq.sched_qs_ofp_flow_ctrl_thresh_types_reg_0[prio_ndx].fcl_wrds_type0);
    break;
  case SOC_PETRA_EGR_PORT_THRESH_TYPE_1:
    fld = SOC_PETRA_REG_DB_ACC_REF(regs->egq.sched_qs_ofp_flow_ctrl_thresh_types_reg_0[prio_ndx].fcl_wrds_type1);
    break;
  case SOC_PETRA_EGR_PORT_THRESH_TYPE_2:
    fld = SOC_PETRA_REG_DB_ACC_REF(regs->egq.sched_qs_ofp_flow_ctrl_thresh_types_reg_1[prio_ndx].fcl_wrds_type2);
      break;
  case SOC_PETRA_EGR_PORT_THRESH_TYPE_3:
    fld = SOC_PETRA_REG_DB_ACC_REF(regs->egq.sched_qs_ofp_flow_ctrl_thresh_types_reg_1[prio_ndx].fcl_wrds_type3);
      break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_EGR_THRESH_TYPE_OUT_OF_RANGE_ERR, 10, exit);
      break;
  }

  res = soc_petra_egr_thresh_fld_to_mnt_exp(
           fld,
           thresh->words,
           &(exact_thresh->words),
           &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  SOC_PETRA_IMPLICIT_FLD_FROM_REG(*fld, thresh_fld, thresh_reg, 30, exit);

  SOC_PETRA_IMPLICIT_FLD_SET(*fld, thresh_fld, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_ofp_fc_set_unsafe()",0,0);
}

/*********************************************************************
*     Set Flow Control thresholds for egress queues, per port
*     queue priority and threshold type, based on Outgoing FAP
*     Port (OFP) resources.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_ofp_fc_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_PORT_THRESH_TYPE ofp_type_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_FC_OFP_THRESH   *thresh
  )
{

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_OFP_FC_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(thresh);

  SOC_SAND_MAGIC_NUM_VERIFY(thresh);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    prio_ndx, SOC_PETRA_EGR_NOF_Q_PRIO-1,
    SOC_PETRA_EGR_Q_PRIO_OUT_OF_RANGE_ERR, 10, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    ofp_type_ndx, SOC_PETRA_EGR_PORT_NOF_THRESH_TYPES-1,
    SOC_PETRA_EGR_THRESH_TYPE_OUT_OF_RANGE_ERR, 20, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_ofp_fc_verify()",0,0);
}

/*********************************************************************
*     Set Flow Control thresholds for egress queues, per port
*     queue priority and threshold type, based on Outgoing FAP
*     Port (OFP) resources.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_ofp_fc_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_PORT_THRESH_TYPE ofp_type_ndx,
    SOC_SAND_OUT SOC_PETRA_EGR_FC_OFP_THRESH   *thresh
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_REG_FIELD
    *fld;
  uint32
    thresh_reg = 0;
  SOC_PETRA_REG_ADDR
    *addr = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_OFP_FC_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(thresh);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    prio_ndx, SOC_PETRA_EGR_NOF_Q_PRIO-1,
    SOC_PETRA_EGR_Q_PRIO_OUT_OF_RANGE_ERR, 10, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    ofp_type_ndx, SOC_PETRA_EGR_PORT_NOF_THRESH_TYPES-1,
    SOC_PETRA_EGR_THRESH_TYPE_OUT_OF_RANGE_ERR, 20, exit
  );

  regs = soc_petra_regs();

  switch(ofp_type_ndx)
  {
  case SOC_PETRA_EGR_PORT_THRESH_TYPE_0:
    fld = SOC_PETRA_REG_DB_ACC_REF(regs->egq.sched_qs_ofp_flow_ctrl_thresh_types_reg_0[prio_ndx].fcl_wrds_type0);
    addr = SOC_PETRA_REG_DB_ACC_REF(regs->egq.sched_qs_ofp_flow_ctrl_thresh_types_reg_0[prio_ndx].addr);
    break;
  case SOC_PETRA_EGR_PORT_THRESH_TYPE_1:
    fld = SOC_PETRA_REG_DB_ACC_REF(regs->egq.sched_qs_ofp_flow_ctrl_thresh_types_reg_0[prio_ndx].fcl_wrds_type1);
    addr = SOC_PETRA_REG_DB_ACC_REF(regs->egq.sched_qs_ofp_flow_ctrl_thresh_types_reg_0[prio_ndx].addr);
    break;
  case SOC_PETRA_EGR_PORT_THRESH_TYPE_2:
    fld = SOC_PETRA_REG_DB_ACC_REF(regs->egq.sched_qs_ofp_flow_ctrl_thresh_types_reg_1[prio_ndx].fcl_wrds_type2);
    addr = SOC_PETRA_REG_DB_ACC_REF(regs->egq.sched_qs_ofp_flow_ctrl_thresh_types_reg_1[prio_ndx].addr);
    break;
  case SOC_PETRA_EGR_PORT_THRESH_TYPE_3:
    fld = SOC_PETRA_REG_DB_ACC_REF(regs->egq.sched_qs_ofp_flow_ctrl_thresh_types_reg_1[prio_ndx].fcl_wrds_type3);
    addr = SOC_PETRA_REG_DB_ACC_REF(regs->egq.sched_qs_ofp_flow_ctrl_thresh_types_reg_1[prio_ndx].addr);
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_EGR_THRESH_TYPE_OUT_OF_RANGE_ERR, 30, exit);
      break;
  }

  res = soc_petra_read_reg_unsafe(
          unit,
          addr,
          SOC_PETRA_DEFAULT_INSTANCE,
          &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_egr_mnt_exp_to_thresh_fld(
           fld,
           &thresh_reg,
           &(thresh->words)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_ofp_fc_get_unsafe()",0,0);
}

/*********************************************************************
*     Set Flow Control thresholds for egress queues, per MCI
*     priority, based on unscheduled traffic resources.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_mci_fc_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_MCI_PRIO        mci_prio_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_FC_MCI_THRESH   *thresh,
    SOC_SAND_OUT SOC_PETRA_EGR_FC_MCI_THRESH   *exact_thresh
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;
  uint32
    thresh_reg = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_MCI_FC_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(thresh);
  SOC_SAND_CHECK_NULL_INPUT(exact_thresh);

  regs = soc_petra_regs();

  res = soc_petra_egr_thresh_fld_to_mnt_exp(
           SOC_PETRA_REG_DB_ACC_REF(regs->egq.mci_thresolds_configuration_reg_0[mci_prio_ndx].total_usc_wrds),
           thresh->words_unsch,
           &(exact_thresh->words_unsch),
           &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_egr_thresh_fld_to_mnt_exp(
           SOC_PETRA_REG_DB_ACC_REF(regs->egq.mci_thresolds_configuration_reg_0[mci_prio_ndx].total_usc_pkts),
           thresh->packets_unsch,
           &(exact_thresh->packets_unsch),
           &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  SOC_PETRA_REG_SET(regs->egq.mci_thresolds_configuration_reg_0[mci_prio_ndx], thresh_reg, 30, exit);

  /*
   * Thresh register must be cleared for each new register
   */
  thresh_reg = 0;

  res = soc_petra_egr_thresh_fld_to_mnt_exp(
           SOC_PETRA_REG_DB_ACC_REF(regs->egq.mci_thresolds_configuration_reg_1[mci_prio_ndx].total_dscs),
           thresh->descriptors_total,
           &(exact_thresh->descriptors_total),
           &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_egr_thresh_fld_to_mnt_exp(
           SOC_PETRA_REG_DB_ACC_REF(regs->egq.mci_thresolds_configuration_reg_1[mci_prio_ndx].total_usc_bufs),
           thresh->buffers_unsch,
           &(exact_thresh->buffers_unsch),
           &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  SOC_PETRA_REG_SET(regs->egq.mci_thresolds_configuration_reg_1[mci_prio_ndx], thresh_reg, 60, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_mci_fc_set_unsafe()",0,0);
}

/*********************************************************************
*     Set Flow Control thresholds for egress queues, per MCI
*     priority, based on unscheduled traffic resources.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_mci_fc_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_MCI_PRIO        mci_prio_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_FC_MCI_THRESH   *thresh
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_MCI_FC_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(thresh);
  SOC_SAND_MAGIC_NUM_VERIFY(thresh);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    mci_prio_ndx, SOC_PETRA_EGR_NOF_MCI_PRIORITIES-1,
    SOC_PETRA_EGR_MCI_PRIO_OUT_OF_RANGE_ERR, 10, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_mci_fc_verify()",0,0);
}

/*********************************************************************
*     Set Flow Control thresholds for egress queues, per MCI
*     priority, based on unscheduled traffic resources.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_mci_fc_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_MCI_PRIO        mci_prio_ndx,
    SOC_SAND_OUT SOC_PETRA_EGR_FC_MCI_THRESH   *thresh
  )
{
  uint32
    res;

  SOC_PETRA_REGS
    *regs;
  uint32
    thresh_reg = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_MCI_FC_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(thresh);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    mci_prio_ndx, SOC_PETRA_EGR_NOF_MCI_PRIORITIES-1,
    SOC_PETRA_EGR_MCI_PRIO_OUT_OF_RANGE_ERR, 10, exit
  );

  regs = soc_petra_regs();

  res = soc_petra_read_reg_unsafe(
          unit,
          SOC_PETRA_REG_DB_ACC_REF(regs->egq.mci_thresolds_configuration_reg_0[mci_prio_ndx].addr),
          SOC_PETRA_DEFAULT_INSTANCE,
          &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_egr_mnt_exp_to_thresh_fld(
           SOC_PETRA_REG_DB_ACC_REF(regs->egq.mci_thresolds_configuration_reg_0[mci_prio_ndx].total_usc_wrds),
           &thresh_reg,
           &(thresh->words_unsch)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_egr_mnt_exp_to_thresh_fld(
           SOC_PETRA_REG_DB_ACC_REF(regs->egq.mci_thresolds_configuration_reg_0[mci_prio_ndx].total_usc_pkts),
           &thresh_reg,
           &(thresh->packets_unsch)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_petra_read_reg_unsafe(
          unit,
          SOC_PETRA_REG_DB_ACC_REF(regs->egq.mci_thresolds_configuration_reg_1[mci_prio_ndx].addr),
          SOC_PETRA_DEFAULT_INSTANCE,
          &thresh_reg
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  res = soc_petra_egr_mnt_exp_to_thresh_fld(
           SOC_PETRA_REG_DB_ACC_REF(regs->egq.mci_thresolds_configuration_reg_1[mci_prio_ndx].total_dscs),
           &thresh_reg,
           &(thresh->descriptors_total)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_egr_mnt_exp_to_thresh_fld(
           SOC_PETRA_REG_DB_ACC_REF(regs->egq.mci_thresolds_configuration_reg_1[mci_prio_ndx].total_usc_bufs),
           &thresh_reg,
           &(thresh->buffers_unsch)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_mci_fc_get_unsafe()",0,0);
}

/*********************************************************************
*     Enable/Disable Flow Control for- MCI (Multicast
*     Congestion Indication)- ERP (Egress Replication Port)
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_mci_fc_enable_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_MCI_PRIO        mci_prio_ndx,
    SOC_SAND_IN  uint8                 mci_enable,
    SOC_SAND_IN  uint8                 erp_enable
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_MCI_FC_ENABLE_SET_UNSAFE);

  regs = soc_petra_regs();

  switch(mci_prio_ndx)
  {
  case SOC_PETRA_EGR_MCI_GUARANTEED:
    SOC_PETRA_FLD_SET(regs->egq.disable_mci_reg.mci0_dis, (mci_enable?0x0:0x1), 10, exit);
    break;
  case SOC_PETRA_EGR_MCI_BE:
    SOC_PETRA_FLD_SET(regs->egq.disable_mci_reg.mci1_dis, (mci_enable?0x0:0x1), 20, exit);

    SOC_PETRA_FLD_SET(regs->egq.disable_mci_reg.erp_fc_en, (erp_enable?0x1:0x0), 30, exit);
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_EGR_MCI_PRIO_OUT_OF_RANGE_ERR, 40, exit);
      break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_mci_fc_enable_set_unsafe()",0,0);
}

/*********************************************************************
*     Enable/Disable Flow Control for- MCI (Multicast
*     Congestion Indication)- ERP (Egress Replication Port)
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_mci_fc_enable_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_MCI_PRIO        mci_prio_ndx,
    SOC_SAND_IN  uint8                 mci_enable,
    SOC_SAND_IN  uint8                 erp_enable
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_MCI_FC_ENABLE_VERIFY);

   SOC_SAND_ERR_IF_ABOVE_MAX(
    mci_prio_ndx, SOC_PETRA_EGR_NOF_MCI_PRIORITIES-1,
    SOC_PETRA_EGR_MCI_PRIO_OUT_OF_RANGE_ERR, 10, exit
  );

  if (erp_enable == TRUE)
  {
    if(mci_prio_ndx != SOC_PETRA_EGR_MCI_BE)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_EGR_MCI_PRIO_AND_ID_MISMATCH_ERR, 20, exit);
    }

    if (mci_enable == TRUE)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_EGR_MCI_ERP_AND_MCI_ENABLE_ERR, 30, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_mci_fc_enable_verify()",0,0);
}

/*********************************************************************
*     Enable/Disable Flow Control for- MCI (Multicast
*     Congestion Indication)- ERP (Egress Replication Port)
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_mci_fc_enable_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_MCI_PRIO        mci_prio_ndx,
    SOC_SAND_OUT uint8                 *mci_enable,
    SOC_SAND_OUT uint8                 *erp_enable
  )
{
  uint32
    fld_val,
    res;

  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_MCI_FC_ENABLE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(mci_enable);
  SOC_SAND_CHECK_NULL_INPUT(erp_enable);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    mci_prio_ndx, SOC_PETRA_EGR_NOF_MCI_PRIORITIES-1,
    SOC_PETRA_EGR_MCI_PRIO_OUT_OF_RANGE_ERR, 10, exit
  );

  regs = soc_petra_regs();

  switch(mci_prio_ndx)
  {
  case SOC_PETRA_EGR_MCI_GUARANTEED:
    SOC_PETRA_FLD_GET(regs->egq.disable_mci_reg.mci0_dis, fld_val, 20, exit);

    *mci_enable = !((uint8)fld_val);
    break;
  case SOC_PETRA_EGR_MCI_BE:
    SOC_PETRA_FLD_GET(regs->egq.disable_mci_reg.mci1_dis, fld_val, 30, exit);
    *mci_enable = !((uint8)fld_val);

    SOC_PETRA_FLD_GET(regs->egq.disable_mci_reg.erp_fc_en, fld_val, 40, exit);
    *erp_enable = (uint8)fld_val;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_EGR_MCI_PRIO_OUT_OF_RANGE_ERR, 50, exit);
      break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_mci_fc_enable_get_unsafe()",0,0);
}

/*********************************************************************
*     Set egress scheduling mode for outgoing FAP ports. Two
*     possible modes exist: - SP between the HP (low delay)
*     and the LP (Normal) queues, and SP among the two HP
*     queues, and WFQ among the LP queues. This configuration
*     ensures that any low delay traffic is always sent ahead
*     of any normal traffic. - WFQ among overall egress
*     scheduled and unscheduled traffic, and SP among HP and
*     LP. This configuration first allocates the bandwidth
*     between the scheduled and the unscheduled traffic types,
*     then for each type ensures that low delay (HP) is sent
*     before normal (LP) traffic.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_ofp_sch_mode_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_OFP_SCH_MODE    *sch_mode
  )
{
  uint32
    sch_mode_val = 0,
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_OFP_SCH_MODE_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(sch_mode);

  regs = soc_petra_regs();

  switch(*sch_mode)
  {
  case SOC_PETRA_EGR_HP_OVER_LP_PER_TYPE:
    sch_mode_val = 0x0;
    break;
  case SOC_PETRA_EGR_HP_OVER_LP_ALL:
    sch_mode_val = 0x1;
    break;
  case SOC_PETRA_EGR_HP_OVER_LP_FAIR:
    sch_mode_val = 0x2;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_EGR_OFP_SCH_MODE_OUT_OF_RANGE_ERR, 10, exit);
      break;
  }

  SOC_PETRA_FLD_SET(regs->egq.egress_de_queue_scheme_reg.schd_queue_scheme, sch_mode_val, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_ofp_sch_mode_set_unsafe()",0,0);
}

/*********************************************************************
*     Set egress scheduling mode for outgoing FAP ports. Two
*     possible modes exist: - SP between the HP (low delay)
*     and the LP (Normal) queues, and SP among the two HP
*     queues, and WFQ among the LP queues. This configuration
*     ensures that any low delay traffic is always sent ahead
*     of any normal traffic. - WFQ among overall egress
*     scheduled and unscheduled traffic, and SP among HP and
*     LP. This configuration first allocates the bandwidth
*     between the scheduled and the unscheduled traffic types,
*     then for each type ensures that low delay (HP) is sent
*     before normal (LP) traffic.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_ofp_sch_mode_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_OFP_SCH_MODE    *sch_mode
  )
{
  uint32
    mode;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_OFP_SCH_MODE_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(sch_mode);

  mode = *sch_mode;

  SOC_SAND_ERR_IF_ABOVE_MAX(
    mode, SOC_PETRA_EGR_OFP_SCH_MODES_MAX,
    SOC_PETRA_EGR_OFP_SCH_MODE_OUT_OF_RANGE_ERR, 10, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_ofp_sch_mode_verify()",0,0);
}

/*********************************************************************
*     Set egress scheduling mode for outgoing FAP ports. Two
*     possible modes exist: - SP between the HP (low delay)
*     and the LP (Normal) queues, and SP among the two HP
*     queues, and WFQ among the LP queues. This configuration
*     ensures that any low delay traffic is always sent ahead
*     of any normal traffic. - WFQ among overall egress
*     scheduled and unscheduled traffic, and SP among HP and
*     LP. This configuration first allocates the bandwidth
*     between the scheduled and the unscheduled traffic types,
*     then for each type ensures that low delay (HP) is sent
*     before normal (LP) traffic.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_ofp_sch_mode_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_EGR_OFP_SCH_MODE    *sch_mode
  )
{
  uint32
    sch_mode_val,
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_OFP_SCH_MODE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(sch_mode);

  regs = soc_petra_regs();

  SOC_PETRA_FLD_GET(regs->egq.egress_de_queue_scheme_reg.schd_queue_scheme, sch_mode_val, 10, exit);

  switch(sch_mode_val)
  {
  case 0x0:
    *sch_mode = SOC_PETRA_EGR_HP_OVER_LP_PER_TYPE;
    break;
  case 0x1:
    *sch_mode = SOC_PETRA_EGR_HP_OVER_LP_ALL;
    break;
  case 0x2:
    *sch_mode = SOC_PETRA_EGR_HP_OVER_LP_FAIR;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_EGR_OFP_SCH_MODE_OUT_OF_RANGE_ERR, 20, exit);
      break;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_ofp_sch_mode_get_unsafe()",0,0);
}

/*********************************************************************
*     Set per-port egress scheduling information.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_ofp_scheduling_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         ofp_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_OFP_SCH_INFO    *info
  )
{
  uint32
    offset,
    nif_prio_fld_val,
#ifdef LINK_PB_LIBRARIES
    ofp_idx_lowest_prio_fld_val,
#endif
    res;
  uint32
    nof_fatp_ports,
    reg_idx = 0,
    fld_idx = 0;
#ifdef LINK_PB_LIBRARIES
  uint32
    fld_val;
  uint32
    mal_id_lowest_prio,
    reg2_idx = 0,
    fld2_idx = 0;
#endif
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_EGQ_DWM_TBL_DATA
    dwm_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_OFP_SCHEDULING_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  nof_fatp_ports = soc_petra_sw_db_fat_pipe_nof_ports_get(unit);

  switch (info->nif_priority)
  {
  case SOC_PETRA_EGR_OFP_INTERFACE_PRIO_HIGH:
    nif_prio_fld_val = 0x0;
  	break;
  case SOC_PETRA_EGR_OFP_INTERFACE_PRIO_MID:
    nif_prio_fld_val = 0x1;
  	break;
  case SOC_PETRA_EGR_OFP_INTERFACE_PRIO_LOW:
    nif_prio_fld_val = 0x3;
  	break;
  case SOC_PETRA_EGR_OFP_INTERFACE_PRIO_PFC_LOWEST:
    nif_prio_fld_val = 0x2;
  	break;
  default:
    nif_prio_fld_val = 0x3;
  }

  if (SOC_PETRA_IS_FAT_PIPE_PORT(ofp_ndx, nof_fatp_ports))
  {
    SOC_PETRA_FLD_SET(regs->egq.oc768_unicast_multicast_weight_reg.oc768_unicast_weight, info->ofp_wfq.sched_weight, 10, exit);
    SOC_PETRA_FLD_SET(regs->egq.oc768_unicast_multicast_weight_reg.oc768_multicast_weight, info->ofp_wfq.unsched_weight, 15, exit);
  }
  else
  {
    /* Set NIF priority. Ignored by the HW unless is mapped to channelized NIF { */
    if (info->nif_priority == SOC_PETRA_EGR_OFP_INTERFACE_PRIO_PFC_LOWEST)
    {
      if (SOC_PETRA_IS_DEV_PETRA_A)
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_EGR_OFP_CHNIF_PRIO_OUT_OF_RANGE_ERR, 17, exit);
      }
#ifdef LINK_PB_LIBRARIES
      else
      {
        /*
         * Some precotion here: validate that no other port on this mal is already set to lowest priority.
         * If it is, set it to "low".
         */
        res = soc_petra_port_ofp_mal_get_unsafe(
                unit,
                ofp_ndx,
                &mal_id_lowest_prio
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 19, exit);

        /*
         * What is the OFP currently configured as lowest priority on this MAL?
         */
        reg2_idx = SOC_PETRA_REG_IDX_GET(mal_id_lowest_prio, SOC_PETRA_NOF_EGR_PORT_PRIO_CONF_FLDS);
        fld2_idx = SOC_PETRA_FLD_IDX_GET(mal_id_lowest_prio, SOC_PETRA_NOF_EGR_PORT_PRIO_CONF_FLDS);
        SOC_PB_FLD_GET(regs->egq.mal_priority3[reg2_idx].mal_priority3_index[fld2_idx], ofp_idx_lowest_prio_fld_val, 21, exit);

        /*
         * Is this a potential NIF OFP?
         */
        if(SOC_SAND_IS_VAL_IN_RANGE(ofp_idx_lowest_prio_fld_val, 1, SOC_PETRA_MAX_FAP_PORT_ID))
        {
          /*
           * Each 2 bits correspond to a single OFP
           */
          reg_idx = SOC_PETRA_REG_IDX_GET(ofp_idx_lowest_prio_fld_val, SOC_PETRA_NOF_EGR_PORT_PRIO_CONF_FLDS);
          fld_idx = SOC_PETRA_FLD_IDX_GET(ofp_idx_lowest_prio_fld_val, SOC_PETRA_NOF_EGR_PORT_PRIO_CONF_FLDS);
          SOC_PETRA_FLD_GET(regs->egq.egress_port_prio_conf_reg[reg_idx].port_priority[fld_idx], fld_val, 23, exit);

          /* Is another  */
          if (fld_val == 0x2)
          {
            SOC_PETRA_FLD_SET(regs->egq.egress_port_prio_conf_reg[reg_idx].port_priority[fld_idx], 0x3, 25, exit);
          }
        }

        /*
         * Set the new lowest-priority OFP on the MAL
         */
        SOC_PB_FLD_SET(regs->egq.mal_priority3[reg2_idx].mal_priority3_index[fld2_idx], ofp_idx_lowest_prio_fld_val, 21, exit);
      }
#endif
    }

    /*
     * Set the OFP nif-priority. Each 2 bits correspond to a single OFP
     */
    reg_idx = SOC_PETRA_REG_IDX_GET(ofp_ndx, SOC_PETRA_NOF_EGR_PORT_PRIO_CONF_FLDS);
    fld_idx = SOC_PETRA_FLD_IDX_GET(ofp_ndx, SOC_PETRA_NOF_EGR_PORT_PRIO_CONF_FLDS);
    SOC_PETRA_FLD_SET(regs->egq.egress_port_prio_conf_reg[reg_idx].port_priority[fld_idx], nif_prio_fld_val, 30, exit);
    /* Set NIF priority. } */

    /* Set WFQ weights { */
    offset = ofp_ndx;
    dwm_tbl_data.uc_or_uc_low_queue_weight = info->ofp_wfq.sched_weight;
    dwm_tbl_data.mc_or_mc_low_queue_weight = info->ofp_wfq.unsched_weight;

    res = soc_petra_egq_dwm_tbl_set_unsafe(
            unit,
            offset,
            &dwm_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

#ifdef LINK_PB_LIBRARIES
    if (SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE)
    {
      SOC_PB_EGQ_DWM_NEW_TBL_DATA
        dwm_new_tbl_data;

      /* Set WFQ high priority */
      /* Ignored by the HW, unless Scheduler mode type is HP_OVER_LP_FAIR */

      offset = ofp_ndx;
      dwm_new_tbl_data.uc_high_queue_weight = info->ofp_wfq_high.sched_weight;
      dwm_new_tbl_data.mc_high_queue_weight = info->ofp_wfq_high.unsched_weight;

      res = soc_pb_egq_dwm_new_tbl_set_unsafe(
              unit,
              offset,
              &dwm_new_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);
    }
#endif
    /* Set WFQ weights } */
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_ofp_scheduling_set_unsafe()",0,0);
}

/*********************************************************************
*     Set per-port egress scheduling information.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_ofp_scheduling_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         ofp_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_OFP_SCH_INFO    *info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_OFP_SCHEDULING_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_MAGIC_NUM_VERIFY(info);

  res = soc_petra_fap_port_id_verify(unit, ofp_ndx, FALSE);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * Verify NIF priority.
   * Set in any case, but ignored by HW unless is mapped to channelized NIF {
   */
  if (SOC_PETRA_IS_DEV_PETRA_A)
  {
    /* SOC_PETRA_EGR_OFP_INTERFACE_PRIO_PFC_LOWEST is valid for Soc_petra-B only */
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->nif_priority, SOC_PETRA_EGR_OFP_CHNIF_NOF_PRIORITIES-2,
      SOC_PETRA_EGR_OFP_CHNIF_PRIO_OUT_OF_RANGE_ERR, 20, exit
    );
  }
  else
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      info->nif_priority, SOC_PETRA_EGR_OFP_CHNIF_NOF_PRIORITIES-1,
      SOC_PETRA_EGR_OFP_CHNIF_PRIO_OUT_OF_RANGE_ERR, 20, exit
    );
}

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->ofp_wfq.sched_weight, SOC_PETRA_EGR_OFP_SCH_WFQ_WEIGHT_MAX,
    SOC_PETRA_EGR_OFP_SCH_WFQ_WEIGHT_OUT_OF_RANGE_ERR, 30, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->ofp_wfq.unsched_weight, SOC_PETRA_EGR_OFP_SCH_WFQ_WEIGHT_MAX,
    SOC_PETRA_EGR_OFP_SCH_WFQ_WEIGHT_OUT_OF_RANGE_ERR, 40, exit
  );
 
  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->ofp_wfq_high.sched_weight, SOC_PETRA_EGR_OFP_SCH_WFQ_WEIGHT_MAX,
    SOC_PETRA_EGR_OFP_SCH_WFQ_WEIGHT_OUT_OF_RANGE_ERR, 40, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->ofp_wfq_high.unsched_weight, SOC_PETRA_EGR_OFP_SCH_WFQ_WEIGHT_MAX,
    SOC_PETRA_EGR_OFP_SCH_WFQ_WEIGHT_OUT_OF_RANGE_ERR, 50, exit
  );


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_ofp_scheduling_verify()",0,0);
}

/*********************************************************************
*     Set per-port egress scheduling information.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_ofp_scheduling_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         ofp_ndx,
    SOC_SAND_OUT SOC_PETRA_EGR_OFP_SCH_INFO    *info
  )
{
  uint32
    offset,
    reg_val,
    res;
  uint32
    nof_fatp_ports,
    reg_idx = 0,
    fld_idx = 0;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_EGQ_DWM_TBL_DATA
    dwm_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_OFP_SCHEDULING_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  res = soc_petra_fap_port_id_verify(unit, ofp_ndx, FALSE);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  regs = soc_petra_regs();

  nof_fatp_ports = soc_petra_sw_db_fat_pipe_nof_ports_get(unit);

  if (SOC_PETRA_IS_FAT_PIPE_PORT(ofp_ndx, nof_fatp_ports))
  {
    SOC_PETRA_FLD_GET(regs->egq.oc768_unicast_multicast_weight_reg.oc768_unicast_weight, info->ofp_wfq.sched_weight, 10, exit);
    SOC_PETRA_FLD_GET(regs->egq.oc768_unicast_multicast_weight_reg.oc768_multicast_weight, info->ofp_wfq.unsched_weight, 15, exit);
    info->nif_priority = SOC_PETRA_EGR_OFP_INTERFACE_PRIO_NONE;
  }
  else
  {
    /* Get NIF priority. Ignored by the HW unless is mapped to channelized NIF { */
    /*
     * Each 2 bits correspond to a single OFP
     */
    reg_idx = SOC_PETRA_REG_IDX_GET(ofp_ndx, SOC_PETRA_NOF_EGR_PORT_PRIO_CONF_FLDS);
    fld_idx = SOC_PETRA_FLD_IDX_GET(ofp_ndx, SOC_PETRA_NOF_EGR_PORT_PRIO_CONF_FLDS);

    SOC_PETRA_FLD_GET(regs->egq.egress_port_prio_conf_reg[reg_idx].port_priority[fld_idx], reg_val, 20, exit);

    info->nif_priority = (SOC_PETRA_EGR_OFP_INTERFACE_PRIO)reg_val;
    /* Get NIF priority. } */

    /* Get WFQ weights { */
    offset = ofp_ndx;

    res = soc_petra_egq_dwm_tbl_get_unsafe(
            unit,
            offset,
            &dwm_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    info->ofp_wfq.sched_weight = dwm_tbl_data.uc_or_uc_low_queue_weight;
    info->ofp_wfq.unsched_weight = dwm_tbl_data.mc_or_mc_low_queue_weight;

#ifdef LINK_PB_LIBRARIES
    if (SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE)
    {
      SOC_PB_EGQ_DWM_NEW_TBL_DATA
        dwm_new_tbl_data;

      /* WFQ high priority */

      offset = ofp_ndx;

      res = soc_pb_egq_dwm_new_tbl_get_unsafe(
              unit,
              offset,
              &dwm_new_tbl_data
             );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      info->ofp_wfq_high.sched_weight = dwm_new_tbl_data.uc_high_queue_weight;
      info->ofp_wfq_high.unsched_weight = dwm_new_tbl_data.mc_high_queue_weight;
    }
#endif
    /* Get WFQ weights } */
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_ofp_scheduling_get_unsafe()",0,0);
}

/*********************************************************************
*     Sets Unscheduled Traffic Drop Priority,
*     based on packet TC and DP.
*     According to the OFP Drop Threshold Type,
*     Unscheduled Packets with different Drop Priorities
*     are dropped based on the appropriate Drop Thresholds
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_unsched_drop_prio_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32  tc_ndx,
    SOC_SAND_IN  uint32  dp_ndx,
    SOC_SAND_IN  uint32   drop_prio
  )
{
  uint32
    res;
  uint32
    pos = 0;
  uint32
    reg_val;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_UNSCHED_DROP_PRIO_SET_UNSAFE);

  regs = soc_petra_regs();

  pos += dp_ndx * SOC_SAND_BIT(SOC_PETRA_EGR_DP_SHIFT);
  pos += tc_ndx * SOC_SAND_BIT(SOC_PETRA_EGR_TC_SHIFT);
  pos *= SOC_PETRA_REGS_MC_2_MCDP_TABLE_REGS;

  SOC_PA_REG_GET(regs->egq.multicast_to_multicast_drop_precedence_lookup_table_reg[pos / SOC_SAND_NOF_BITS_IN_UINT32], reg_val, 10, exit);
  reg_val &= SOC_SAND_ZERO_BITS_MASK(pos + 1, pos);
  reg_val |= SOC_SAND_SET_FLD_IN_PLACE(drop_prio, pos, SOC_SAND_BITS_MASK(pos + 1, pos));
  SOC_PA_REG_SET(regs->egq.multicast_to_multicast_drop_precedence_lookup_table_reg[pos / SOC_SAND_NOF_BITS_IN_UINT32], reg_val, 12, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_unsched_drop_prio_set_unsafe()",0,0);
}

/*********************************************************************
*     Sets Unscheduled Traffic Drop Priority,
*     based on packet TC and DP.
*     According to the OFP Drop Threshold Type,
*     Unscheduled Packets with different Drop Priorities
*     are dropped based on the appropriate Drop Thresholds
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_unsched_drop_prio_verify(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32  tc_ndx,
    SOC_SAND_IN  uint32  dp_ndx,
    SOC_SAND_IN  uint32   drop_prio
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_UNSCHED_DROP_PRIO_VERIFY);


  res = soc_petra_traffic_class_verify(tc_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_drop_precedence_verify(dp_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    drop_prio, SOC_PETRA_NOF_DROP_PRECEDENCE,
    SOC_PETRA_UNSCHED_PRIO_OUT_OF_RANGE_ERR, 30, exit
  )

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_unsched_drop_prio_verify()",0,0);
}

/*********************************************************************
*     Gets Unscheduled Traffic Drop Priority,
*     based on packet TC and DP.
*     According to the OFP Drop Threshold Type,
*     Unscheduled Packets with different Drop Priorities
*     are dropped based on the appropriate Drop Thresholds
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_unsched_drop_prio_get_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32  tc_ndx,
    SOC_SAND_IN  uint32  dp_ndx,
    SOC_SAND_OUT uint32   *drop_prio
  )
{
  uint32
    res;
  uint32
    pos = 0;
  uint32
    reg_val;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_UNSCHED_DROP_PRIO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(drop_prio);

  regs = soc_petra_regs();

  pos += dp_ndx * SOC_SAND_BIT(SOC_PETRA_EGR_DP_SHIFT);
  pos += tc_ndx * SOC_SAND_BIT(SOC_PETRA_EGR_TC_SHIFT);
  pos *= SOC_PETRA_REGS_MC_2_MCDP_TABLE_REGS;

  SOC_PA_REG_GET(regs->egq.multicast_to_multicast_drop_precedence_lookup_table_reg[pos / SOC_SAND_NOF_BITS_IN_UINT32], reg_val, 10, exit);
  *drop_prio = SOC_SAND_GET_FLD_FROM_PLACE(reg_val, pos, SOC_SAND_BITS_MASK(pos + 1, pos));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_unsched_drop_prio_get_unsafe()",0,0);
}

/*********************************************************************
*     Sets egress queue priority per traffic class and drop
*     precedence.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_q_prio_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO_MAPPING_TYPE map_type_ndx,
    SOC_SAND_IN  int                 is_sched_mc,
    SOC_SAND_IN  uint32                 tc_ndx,
    SOC_SAND_IN  uint32                 dp_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO          prio
  )
{
  uint32
    prio_reg_val = 0,
    prio_bit_idx = 0,
    prio_bit_val = 0,
    res;
  SOC_PETRA_REG_ADDR
    *prio_reg_addr;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_Q_PRIO_SET_UNSAFE);

  regs = soc_petra_regs();

  switch(map_type_ndx)
  {
  case SOC_PETRA_EGR_UCAST_TO_SCHED:
      if(is_sched_mc) {
          prio_reg_addr = SOC_PA_REG_DB_ACC_REF(regs->egq.multicast_to_unicast_class_lookup_table_reg.addr);
      } else {
          prio_reg_addr = SOC_PA_REG_DB_ACC_REF(regs->egq.unicast_to_unicast_class_lookup_table_reg.addr);
      }
    break;
  case SOC_PETRA_EGR_MCAST_TO_UNSCHED:
    prio_reg_addr = SOC_PA_REG_DB_ACC_REF(regs->egq.multicast_to_multicast_class_lookup_table_reg.addr);
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_EGR_Q_PRIO_OUT_OF_RANGE_ERR, 10, exit);
      break;
  }

  res = soc_petra_read_reg_unsafe(
          unit,
          prio_reg_addr,
          SOC_PETRA_DEFAULT_INSTANCE,
          &prio_reg_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /* The bit to set is concatenation of {TC(3-bits),DP(2-bits)} */
  prio_bit_idx = (tc_ndx << 2) | dp_ndx;
  prio_bit_val = (uint32)prio;
  SOC_SAND_SET_BIT(prio_reg_val, prio_bit_val, prio_bit_idx);

  res = soc_petra_write_reg_unsafe(
          unit,
          prio_reg_addr,
          SOC_PETRA_DEFAULT_INSTANCE,
          prio_reg_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if (map_type_ndx == SOC_PETRA_EGR_MCAST_TO_UNSCHED)
  {
    SOC_PA_REG_SET(regs->egq.mc_priority_lookup_table_reg, prio_reg_val, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_q_prio_set_unsafe()",0,0);
}

/*********************************************************************
*     Sets egress queue priority per traffic class and drop
*     precedence.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_q_prio_verify(
    SOC_SAND_IN  int                      unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO_MAPPING_TYPE  map_type_ndx,
    SOC_SAND_IN  uint32                      tc_ndx,
    SOC_SAND_IN  uint32                      dp_ndx,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO               prio
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_Q_PRIO_VERIFY);

  res = soc_petra_traffic_class_verify(tc_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_drop_precedence_verify(dp_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    map_type_ndx, SOC_PETRA_EGR_NOF_Q_PRIO_MAPPING_TYPES-1,
    SOC_PETRA_EGR_Q_PRIO_MAPPING_TYPE_OUT_OF_RANGE_ERR, 30, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    prio, SOC_PETRA_EGR_NOF_Q_PRIO,
    SOC_PETRA_EGR_Q_PRIO_OUT_OF_RANGE_ERR, 40, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_q_prio_verify()",0,0);
}

/*********************************************************************
*     Sets egress queue priority per traffic class and drop
*     precedence.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_egr_q_prio_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_EGR_Q_PRIO_MAPPING_TYPE map_type_ndx,
    SOC_SAND_IN  int                 is_sched_mc,
    SOC_SAND_IN  uint32                 tc_ndx,
    SOC_SAND_IN  uint32                 dp_ndx,
    SOC_SAND_OUT SOC_PETRA_EGR_Q_PRIO          *prio
  )
{
  uint32
    prio_reg_val = 0,
    prio_bit_idx = 0,
    prio_bit_val = 0,
    res;
  SOC_PETRA_REG_ADDR
    *prio_reg_addr;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_EGR_Q_PRIO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(prio);

  res = soc_petra_traffic_class_verify(tc_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_drop_precedence_verify(dp_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    map_type_ndx, SOC_PETRA_EGR_NOF_Q_PRIO_MAPPING_TYPES-1,
    SOC_PETRA_EGR_Q_PRIO_MAPPING_TYPE_OUT_OF_RANGE_ERR, 30, exit
  );

  regs = soc_petra_regs();

  switch(map_type_ndx)
  {
  case SOC_PETRA_EGR_UCAST_TO_SCHED:
      if(is_sched_mc) {
          prio_reg_addr = SOC_PA_REG_DB_ACC_REF(regs->egq.multicast_to_unicast_class_lookup_table_reg.addr);
      } else {
          prio_reg_addr = SOC_PA_REG_DB_ACC_REF(regs->egq.unicast_to_unicast_class_lookup_table_reg.addr);
      }
    break;
  case SOC_PETRA_EGR_MCAST_TO_UNSCHED:
    prio_reg_addr = SOC_PA_REG_DB_ACC_REF(regs->egq.multicast_to_multicast_class_lookup_table_reg.addr);
      break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_EGR_Q_PRIO_OUT_OF_RANGE_ERR, 10, exit);
      break;
  }

  res = soc_petra_read_reg_unsafe(
          unit,
          prio_reg_addr,
          SOC_PETRA_DEFAULT_INSTANCE,
          &prio_reg_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  /* The bit to set is concatenation of {TC(3-bits),DP(2-bits)} */
  prio_bit_idx = (tc_ndx << 2) | dp_ndx;

  prio_bit_val = SOC_SAND_GET_BIT(prio_reg_val, prio_bit_idx);
  *prio = prio_bit_val?SOC_PETRA_EGR_Q_PRIO_HIGH:SOC_PETRA_EGR_Q_PRIO_LOW;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_egr_q_prio_get_unsafe()",0,0);
}

/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>

