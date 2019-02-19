/* $Id: pb_init.c,v 1.26 Broadcom SDK $
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

#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_workload_status.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>

#include <soc/dpp/Petra/petra_general.h>
#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_tbl_access.h>

#include <soc/dpp/Petra/petra_mgmt.h>
#include <soc/dpp/Petra/petra_nif.h>
#include <soc/dpp/Petra/petra_fabric.h>
#include <soc/dpp/Petra/petra_dram.h>
#include <soc/dpp/Petra/petra_diagnostics.h>
#include <soc/dpp/Petra/petra_statistics.h>
#include <soc/dpp/Petra/petra_ingress_traffic_mgmt.h>
#include <soc/dpp/Petra/petra_ingress_scheduler.h>
#include <soc/dpp/Petra/petra_ingress_packet_queuing.h>
#include <soc/dpp/Petra/petra_egr_queuing.h>
#include <soc/dpp/Petra/petra_stat_if.h>
#include <soc/dpp/Petra/petra_scheduler_end2end.h>
#include <soc/dpp/Petra/petra_ports.h>
#include <soc/dpp/Petra/petra_multicast_linked_list.h>
#include <soc/dpp/Petra/petra_packet.h>
#include <soc/dpp/Petra/petra_cell.h>
#include <soc/dpp/Petra/petra_sw_db.h>
#include <soc/dpp/Petra/petra_egr_queuing.h>
#include <soc/dpp/Petra/petra_init.h>

#include <soc/dpp/Petra/PB_TM/pb_mgmt.h>
#include <soc/dpp/Petra/PB_TM/pb_stat_if.h>
#include <soc/dpp/Petra/PB_TM/pb_ingress_traffic_mgmt.h>
#include <soc/dpp/Petra/PB_TM/pb_nif.h>
#include <soc/dpp/Petra/PB_TM/pb_init.h>
#include <soc/dpp/Petra/PB_TM/pb_parser.h>
#include <soc/dpp/Petra/PB_TM/pb_ports.h>
#include <soc/dpp/Petra/PB_TM/pb_flow_control.h>
#include <soc/dpp/Petra/PB_TM/pb_egr_queuing.h>
#include <soc/dpp/Petra/PB_TM/pb_flow_control.h>
#include <soc/dpp/Petra/PB_TM/pb_interrupt_service.h>
#include <soc/dpp/Petra/PB_TM/pb_cnt.h>
#include <soc/dpp/Petra/PB_TM/pb_tdm.h>
#include <soc/dpp/Petra/PB_TM/pb_egr_acl.h>

#include <soc/dpp/Petra/PB_TM/pb_pmf_low_level.h>
#include <soc/dpp/Petra/PB_TM/pb_tcam.h>
#include <soc/dpp/Petra/PB_TM/pb_ports.h>
#include <soc/dpp/Petra/PB_TM/pb_egr_prog_editor.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_chip_regs.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_init_tbl.h>

#ifdef LINK_PB_PP_LIBRARIES
#include <soc/dpp/Petra/PB_PP/pb_pp_init.h>
#endif

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_MGMT_INIT_TIMER_DELAY_MSEC           50
#define SOC_PB_INIT_MEM_CORR_DELAY_MSEC             50

#define SOC_PB_MGMT_INIT_EGQ_MAX_FRG_FIX            508
#define SOC_PB_MGMT_INIT_EGQ_MAX_FRG_VAR            127

/* $Id: pb_init.c,v 1.26 Broadcom SDK $
 *  QDR wait for init
 */
#define SOC_PB_MGMT_INIT_QDR_BUSY_WAIT_ITERATIONS   50
#define SOC_PB_MGMT_INIT_QDR_TIMER_ITERATIONS       10
#define SOC_PB_MGMT_INIT_QDR_TIMER_DELAY_MSEC       50

#define SOC_PB_MGMT_INIT_QDR_IO_RESET_DELAY_MSEC    20

/*
 *  EGQ wait for init
 */
#define SOC_PB_MGMT_INIT_EGQ_BUSY_WAIT_ITERATIONS   50
#define SOC_PB_MGMT_INIT_EGQ_TIMER_ITERATIONS       10
#define SOC_PB_MGMT_INIT_EGQ_TIMER_DELAY_MSEC       50

/*
 *  DRAM wait for init
 */
#define SOC_PB_MGMT_INIT_DRAM_BUSY_WAIT_ITERATIONS   50
#define SOC_PB_MGMT_INIT_DRAM_TIMER_ITERATIONS       10
#define SOC_PB_MGMT_INIT_DRAM_TIMER_DELAY_MSEC       50

/*
 *  SMS - memory correction
 */
#define SOC_PB_MGMT_INIT_SMS_BUSY_WAIT_ITERATIONS   50
#define SOC_PB_MGMT_INIT_SMS_TIMER_ITERATIONS       10
#define SOC_PB_MGMT_INIT_SMS_TIMER_DELAY_MSEC       50

/*
 *  DRAM buffers
 */
#define SOC_PB_INIT_DRAM_BYTES_FOR_FBC               32
#define SOC_PB_INIT_DRAM_FBC_SEQUENCE_SIZE           11
#define SOC_PB_INIT_DRAM_BUFF_TO_FBC_DELTA_MIN       256

/*
 *  Core frequency
 */
#define SOC_PB_MGMT_CORE_FREQ_MIN          150
#define SOC_PB_MGMT_CORE_FREQ_MAX          300

/*
 *  DRAM Configuration
 */


/*
 *  If set, allows
 *  viewing table initialization percentage
 */
#define SOC_PB_MGMT_TBL_WRKLOAD_DISPLAY_EN 0

/*
 *  If set, allows initialization without
 *  setting all the required interfaces.
 *  This mode is for bring-up/debug only
 */
#define SOC_PB_INIT_DBG_PARTIAL_IF_CFG_EN  0

/*
 *  Controls Duty Cycle (DCF-fix). Normally disabled
 */
#define SOC_PB_DRAM_DUTY_CYCLE_EN          0

/*
 *  OFP rates
 */
#define SOC_PB_OFP_RATES_CAL_LEN_SCH_OFFSET_ADDITION        0x0800
#define SOC_PB_OFP_RATES_CAL_CPU_BASE_SCH_OFFSET            0x0400
#define SOC_PB_OFP_RATES_CAL_RCY_BASE_SCH_OFFSET            0x2400

/*
 *  Minimum packet size in Interlaken mode
 */
#define SOC_PB_INIT_PCKT_SIZE_BYTES_ILKN_MIN                    32
#define SOC_PB_INIT_PCKT_SIZE_BYTES_ILKN_MIN_INTERN             (SOC_PB_INIT_PCKT_SIZE_BYTES_ILKN_MIN + 8 + SOC_PETRA_MGMT_PCKT_RNG_CORRECTION_INTERNAL)
#define SOC_PB_INIT_PCKT_SIZE_BYTES_ILKN_MIN_EXTERN             (SOC_PB_INIT_PCKT_SIZE_BYTES_ILKN_MIN + SOC_PETRA_MGMT_PCKT_RNG_CORRECTION_EXTERNAL)

/* Better QDR window tuning */
#define SOC_PB_MGMT_INIT_QDR_DLL_MEM_DEFAULT 0x20008

/* } */

/*************
 *  MACROS   *
 *************/
/* { */



/*
 *  max value of the count
 *   it's equal to the maximum value count field may get - 1
*/

#define SOC_PB_INIT_COUNT_MAX(val)   \
            (SOC_PETRA_FLD_MAX(val) - 1)

/*
 *  Prints phase 1 initialization advance.
 *  Assumes the following variables are defined:
 *   - uint8 silent
 *   - uint32 stage_id
 */
#define SOC_PB_INIT_PRINT_ADVANCE(str, phase_id)                                \
{                                                                              \
  if (!silent)                                                                 \
  {                                                                            \
    soc_sand_os_printf("    + Phase %u, %.2u: %s\n\r", phase_id, ++stage_id, str); \
  }                                                                            \
}

#define SOC_PB_INIT_PRINT_INTERNAL_ADVANCE(str)                  \
{                                                              \
  if (!silent)                                                 \
  {                                                            \
    soc_sand_os_printf("               ++  %.2u: %s\n\r", ++stage_id, str);  \
  }                                                            \
}

#define SOC_PB_INIT_PRINT_INTERNAL_LVL2(str)                       \
{                                                              \
  if (!silent)                                                 \
  {                                                            \
    soc_sand_os_printf("                       - %s\n\r", str);    \
  }                                                            \
}

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

/*
 *  DRAM buffers boundaries configuration.
 *  This configuration is per-buffer type: unicast, mini-multicast and full-multicast
 */
typedef struct {
  uint32 start;
  uint32 end;
}SOC_PB_MGMT_DBUFF_BOUNDARIES;

typedef struct {
  SOC_PB_MGMT_DBUFF_BOUNDARIES  fmc;
  SOC_PB_MGMT_DBUFF_BOUNDARIES  mmc;
  SOC_PB_MGMT_DBUFF_BOUNDARIES  uc;
  SOC_PB_MGMT_DBUFF_BOUNDARIES  fbc_fmc;
  SOC_PB_MGMT_DBUFF_BOUNDARIES  fbc_mmc;
  SOC_PB_MGMT_DBUFF_BOUNDARIES  fbc_uc;
} SOC_PB_INIT_DBUFFS_BDRY;

/* } */

/*************
 * GLOBALS   *
 *************/
/* { */

/*
 *  Total number of lines in Soc_petra tables.
 */
#if SOC_PB_MGMT_TBL_WRKLOAD_DISPLAY_EN
static uint32
    Soc_pb_mgmt_tbls_total_lines = 0;
static uint32
    Soc_pb_mgmt_tbls_curr_workload = 0;
#endif


/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

uint32
  soc_pb_mgmt_init_qdr_dll_mem_unsafe(
    int unit
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_QDR_QDR_DLL_MEM_TBL_DATA
    dll_mem;
  SOC_PETRA_TBLS
    *tables;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_INIT_QDR_DLL_MEM_UNSAFE);

  tables = soc_petra_tbls();

  dll_mem.qdr_dll_mem = SOC_PB_MGMT_INIT_QDR_DLL_MEM_DEFAULT;

  res = soc_petra_qdr_set_reps_for_tbl_unsafe(
          unit,
          SOC_PETRA_TBL(tables->qdr.qdr_dll_mem.addr.size)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  res = soc_petra_qdr_qdr_dll_mem_tbl_set_unsafe(
          unit,
          0,
          &dll_mem
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_init_qdr_dll_mem_unsafe()", 0, 0);
}

/*
 *  soc_pa_pll is the M/N-style PLL.
 *  soc_pb_pll is F/Q/R-style PLL.
 *  Only one of the above is used.
 */
STATIC uint32
  soc_pb_init_dram_pll_set(
    SOC_SAND_IN int           unit,
    SOC_SAND_IN SOC_PETRA_HW_PLL_PARAMS *soc_pa_pll,
    SOC_SAND_IN SOC_PB_HW_ADJ_DRAM_PLL *soc_pb_pll
  )
{
  uint32
    fld_val = 0,
    reg_val = 0,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_INIT_DRAM_PLL_SET);

  regs = soc_petra_regs();
  SOC_SAND_CHECK_NULL_INPUT(soc_pb_pll);

  if (SOC_PB_REV_ABOVE_A0)
  {
    fld_val = 0x1;

    SOC_PB_REG_GET(regs->eci.ddr_pll_hs_config_reg, reg_val, 20, exit);
    SOC_PB_FLD_TO_REG(regs->eci.ddr_pll_hs_config_reg.ddr_use_hs_pll, fld_val, reg_val,    22, exit);
    SOC_PB_FLD_TO_REG(regs->eci.ddr_pll_hs_config_reg.ddr_pll_hs_divf, soc_pb_pll->f, reg_val, 24, exit);
    SOC_PB_FLD_TO_REG(regs->eci.ddr_pll_hs_config_reg.ddr_pll_hs_divr, soc_pb_pll->r, reg_val, 26, exit);
    SOC_PB_FLD_TO_REG(regs->eci.ddr_pll_hs_config_reg.ddr_pll_hs_divq, soc_pb_pll->q, reg_val, 28, exit);
    SOC_PB_REG_SET(regs->eci.ddr_pll_hs_config_reg, reg_val, 30, exit);

    SOC_PB_REG_GET(regs->eci.ddr_ab_pll_hs_config_reg, reg_val, 31, exit);
    SOC_PB_FLD_TO_REG(regs->eci.ddr_ab_pll_hs_config_reg.ddr_ab_use_hs_pll, fld_val, reg_val,    32, exit);
    SOC_PB_FLD_TO_REG(regs->eci.ddr_ab_pll_hs_config_reg.ddr_ab_pll_hs_divf, soc_pb_pll->f, reg_val, 34, exit);
    SOC_PB_FLD_TO_REG(regs->eci.ddr_ab_pll_hs_config_reg.ddr_ab_pll_hs_divr, soc_pb_pll->r, reg_val, 36, exit);
    SOC_PB_FLD_TO_REG(regs->eci.ddr_ab_pll_hs_config_reg.ddr_ab_pll_hs_divq, soc_pb_pll->q, reg_val, 38, exit);
    SOC_PB_REG_SET(regs->eci.ddr_ab_pll_hs_config_reg, reg_val, 40, exit);

    SOC_PB_REG_GET(regs->eci.ddr_ef_pll_hs_config_reg, reg_val, 41, exit);
    SOC_PB_FLD_TO_REG(regs->eci.ddr_ef_pll_hs_config_reg.ddr_ef_use_hs_pll, fld_val, reg_val,    42, exit);
    SOC_PB_FLD_TO_REG(regs->eci.ddr_ef_pll_hs_config_reg.ddr_ef_pll_hs_divf, soc_pb_pll->f, reg_val, 44, exit);
    SOC_PB_FLD_TO_REG(regs->eci.ddr_ef_pll_hs_config_reg.ddr_ef_pll_hs_divr, soc_pb_pll->r, reg_val, 46, exit);
    SOC_PB_FLD_TO_REG(regs->eci.ddr_ef_pll_hs_config_reg.ddr_ef_pll_hs_divq, soc_pb_pll->q, reg_val, 48, exit);
    SOC_PB_REG_SET(regs->eci.ddr_ef_pll_hs_config_reg, reg_val, 50, exit);
  }
  else
  {
    SOC_SAND_CHECK_NULL_INPUT(soc_pa_pll);
    SOC_PB_FLD_SET(regs->eci.ddr_pll_hs_config_reg.ddr_use_hs_pll,       0x0, 4, exit);
    SOC_PB_FLD_SET(regs->eci.ddr_ab_pll_hs_config_reg.ddr_ab_use_hs_pll, 0x0, 6, exit);
    SOC_PB_FLD_SET(regs->eci.ddr_ef_pll_hs_config_reg.ddr_ef_use_hs_pll, 0x0, 8, exit);

    SOC_PB_FLD_SET(regs->eci.ddr_pll_config_reg.ddr_pll_m, soc_pa_pll->m, 10, exit);
    SOC_PB_FLD_SET(regs->eci.ddr_pll_config_reg.ddr_pll_n, soc_pa_pll->n, 12, exit);

    SOC_PB_FLD_SET(regs->eci.ddr_ab_pll_config_reg.ddr_ab_pll_m, soc_pa_pll->m, 13, exit);
    SOC_PB_FLD_SET(regs->eci.ddr_ab_pll_config_reg.ddr_ab_pll_n, soc_pa_pll->n, 14, exit);

    SOC_PB_FLD_SET(regs->eci.ddr_ef_pll_config_reg.ddr_ef_pll_m, soc_pa_pll->m, 15, exit);
    SOC_PB_FLD_SET(regs->eci.ddr_ef_pll_config_reg.ddr_ef_pll_n, soc_pa_pll->n, 16, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_init_dram_pll_set()", 0, 0);
}

STATIC uint32
  soc_pb_init_mem_correction(
    SOC_SAND_IN int    unit
  )
{
  uint32
    reg_val = 0,
    poll_exit_place = 0,
    res;
  SOC_PETRA_POLL_INFO
    poll_info;
  uint8
    poll_success,
    is_low_sim_active;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_INIT_MEM_CORRECTION_ENABLE);

#ifndef SAND_LOW_LEVEL_SIMULATION
  is_low_sim_active = FALSE;
#else
  is_low_sim_active = soc_sand_low_is_sim_active_get();
#endif

  regs = soc_petra_regs();

  soc_petra_PETRA_POLL_INFO_clear(&poll_info);
  poll_info.expected_value = 0x0;
  poll_info.busy_wait_nof_iters = SOC_PB_MGMT_INIT_SMS_BUSY_WAIT_ITERATIONS;
  poll_info.timer_nof_iters     = SOC_PB_MGMT_INIT_SMS_TIMER_ITERATIONS;
  poll_info.timer_delay_msec    = SOC_PB_MGMT_INIT_SMS_TIMER_DELAY_MSEC;

  /*
   * Follows taking the MCC out of reset
   */

  /*
   * Set MBIST mode
   */
  SOC_PETRA_REG_SET(regs->mcc.memory_configuration_reg, 0x1, 12, exit);
  /*
   * Perform MBIST reset sequence
   */
  SOC_PETRA_REG_SET(regs->mcc.mbist_resets_reg, 0x7, 14, exit);
  SOC_PETRA_REG_SET(regs->mcc.mbist_resets_reg, 0x0, 16, exit);
  SOC_PETRA_REG_SET(regs->mcc.mbist_resets_reg, 0x4, 18, exit);
  SOC_PETRA_REG_SET(regs->mcc.mbist_resets_reg, 0x6, 20, exit);
  SOC_PETRA_REG_SET(regs->mcc.mbist_resets_reg, 0x7, 22, exit);
  SOC_PETRA_REG_SET(regs->mcc.mbist_resets_reg, 0x6, 24, exit);
  SOC_PETRA_REG_SET(regs->mcc.mbist_resets_reg, 0x0, 26, exit);

  /*
   * TAP
   */
  SOC_PETRA_REG_SET(regs->mcc.tap_config_reg, 0x11, 27, exit);

  poll_info.expected_value = 0x0;
  res = soc_petra_status_fld_poll_unsafe(
          unit,
          SOC_PETRA_REG_DB_ACC_REF(regs->mcc.tap_config_reg.tap_cmd),
          SOC_PETRA_DEFAULT_INSTANCE,
          &poll_info,
          &poll_success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 28, exit);

  if (poll_success == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MGMT_SMS_ACTION_TIMOUT_ERR, 29, exit);
  }

  /*
   * SMS
   */
  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x8000c00c, 30, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 2000 + poll_exit_place, exit); poll_exit_place++;

  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x8000c00d, 33, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 2000 + poll_exit_place, exit); poll_exit_place++;

  SOC_PETRA_REG_SET(regs->mcc.tap_gen_reg_1, 0x3, 36, exit);

  SOC_PETRA_REG_SET(regs->mcc.tap_gen_reg_2, 0x0, 37, exit);

  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x80006001, 38, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 2000 + poll_exit_place, exit); poll_exit_place++;

  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x80006009, 41, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 2000 + poll_exit_place, exit); poll_exit_place++;

  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x8000600a, 44, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 2000 + poll_exit_place, exit); poll_exit_place++;

  SOC_PETRA_REG_SET(regs->mcc.tap_gen_reg_1, 0x2b, 47, exit);
  SOC_PETRA_REG_SET(regs->mcc.tap_gen_reg_2, 0x0, 48, exit);
 
  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x80006001, 49, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 2000 + poll_exit_place, exit); poll_exit_place++;

  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x80006006, 51, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 2000 + poll_exit_place, exit); poll_exit_place++;

  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x80006007, 54, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 2000 + poll_exit_place, exit); poll_exit_place++;

  SOC_PETRA_REG_SET(regs->mcc.tap_gen_reg_1, 0x3, 64, exit);

  SOC_PETRA_REG_SET(regs->mcc.tap_gen_reg_2, 0x0, 66, exit);

  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x80006001, 68, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 2000 + poll_exit_place, exit); poll_exit_place++;

  if (poll_success == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_MGMT_SMS_ACTION_TIMOUT_ERR, 72, exit);
  }

  poll_info.expected_value = 0x1;
  res = soc_petra_status_fld_poll_unsafe(
          unit,
          SOC_PETRA_REG_DB_ACC_REF(regs->mcc.sfp_ready_reg.sfp_ready),
          SOC_PETRA_DEFAULT_INSTANCE,
          &poll_info,
          &poll_success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 73, exit);

  if (poll_success == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_MGMT_SMS_ACTION_TIMOUT_ERR, 74, exit);
  }

  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x80006006, 75, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 2000 + poll_exit_place, exit); poll_exit_place++;

  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x80006007, 80, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 2000 + poll_exit_place, exit); poll_exit_place++;

  SOC_PETRA_REG_SET(regs->mcc.tap_gen_reg_1, 0xe, 84, exit);
  SOC_PETRA_REG_SET(regs->mcc.tap_gen_reg_2, 0x0, 86, exit);

  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x80006001, 88, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 2000 + poll_exit_place, exit); poll_exit_place++;
  
  poll_info.expected_value = 0x1;
  res = soc_petra_status_fld_poll_unsafe(
          unit,
          SOC_PETRA_REG_DB_ACC_REF(regs->mcc.sfp_ready_reg.sfp_ready),
          SOC_PETRA_DEFAULT_INSTANCE,
          &poll_info,
          &poll_success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 91, exit);

  if (poll_success == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_MGMT_SMS_ACTION_TIMOUT_ERR, 92, exit);
  }

  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x80006003, 93, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 2000 + poll_exit_place, exit); poll_exit_place++;

  SOC_PETRA_REG_SET(regs->mcc.tap_gen_reg_1, 0x0, 100, exit);
  SOC_PETRA_REG_SET(regs->mcc.tap_gen_reg_2, 0x0, 102, exit);
  
  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x80006002, 104, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 2000 + poll_exit_place, exit); poll_exit_place++;

  poll_info.expected_value = 0x1;
  res = soc_petra_status_fld_poll_unsafe(
          unit,
          SOC_PETRA_REG_DB_ACC_REF(regs->mcc.sfp_ready_reg.sfp_ready),
          SOC_PETRA_DEFAULT_INSTANCE,
          &poll_info,
          &poll_success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 107, exit);

  if (poll_success == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_MGMT_SMS_ACTION_TIMOUT_ERR, 109, exit);
  }

  SOC_PETRA_REG_SET(regs->mcc.tap_res_1_reg, 0x61, 110, exit);

  /*
   * Validate correction validity, if executed
   */
  SOC_PETRA_REG_GET(regs->mcc.tap_res_0_reg, reg_val, 112, exit);
  if (!is_low_sim_active)
  {
    if (
        !((reg_val == 32) ||
          (reg_val == 40))
       )
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MEM_CORRECTION_ERR, 1000+reg_val, exit);
    }
  }

  /*
   * Enter smart mode
   */
  SOC_PETRA_REG_SET(regs->mcc.memory_configuration_reg, 0x3, 116, exit);
  sal_msleep(SOC_PB_INIT_MEM_CORR_DELAY_MSEC);

  poll_info.expected_value = 0x1;
  res = soc_petra_status_fld_poll_unsafe(
          unit,
          SOC_PB_REG_DB_ACC_REF(regs->mcc.sfp_ready_reg.sfp_ready),
          SOC_PETRA_DEFAULT_INSTANCE,
          &poll_info,
          &poll_success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);
  if (poll_success == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_MGMT_SMS_ACTION_TIMOUT_ERR, 117, exit);
  }

  poll_info.expected_value = 0xffffffff;
  res = soc_petra_status_fld_poll_unsafe(
          unit,
          SOC_PB_REG_DB_ACC_REF(regs->mcc.ready_sms_statuses1.ready_sms1),
          SOC_PETRA_DEFAULT_INSTANCE,
          &poll_info,
          &poll_success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 202, exit);
  if (poll_success == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_MGMT_SMS_ACTION_TIMOUT_ERR, 118, exit);
  }
  
  poll_info.expected_value = 0x3ff;
  res = soc_petra_status_fld_poll_unsafe(
          unit,
          SOC_PB_REG_DB_ACC_REF(regs->mcc.ready_sms_statuses2.ready_sms2),
          SOC_PETRA_DEFAULT_INSTANCE,
          &poll_info,
          &poll_success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 204, exit);
  if (poll_success == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_MGMT_SMS_ACTION_TIMOUT_ERR, 119, exit);
  }

  /*
   * Validate memory-BIST success
   */
  SOC_PB_REG_GET(regs->mcc.fail_sms_statuses1, reg_val, 206, exit);
  if (reg_val !=  0)
  {
#if SOC_PB_DEBUG_IS_LVL2
    soc_sand_os_printf("Memory BIST Fail Status-1, value: 0x%.8x", reg_val);
#endif
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MEM_BIST_ERR, 210, exit);
  }

  SOC_PB_REG_GET(regs->mcc.fail_sms_statuses2, reg_val, 212, exit);
  if (reg_val !=  0)
  {
#if SOC_PB_DEBUG_IS_LVL2
    soc_sand_os_printf("Memory BIST Fail Status-2, value: 0x%.8x", reg_val);
#endif
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MEM_BIST_ERR, 214, exit);
  }

  /*
   * Exit smart mode
   */
  SOC_PETRA_REG_SET(regs->mcc.memory_configuration_reg, 0x0, 120, exit);

  /*
   * Initiate load
   */
  SOC_PETRA_REG_SET(regs->mcc.memory_configuration_reg, 0x8, 121, exit);
  sal_msleep(SOC_PB_INIT_MEM_CORR_DELAY_MSEC);
  /*
   * Exit load
   */
  SOC_PETRA_REG_SET(regs->mcc.memory_configuration_reg, 0x0, 122, exit);

  SOC_PETRA_REG_SET(regs->mcc.mbist_resets_reg, 0x4, 124, exit);
  SOC_PETRA_REG_SET(regs->mcc.mbist_resets_reg, 0x6, 126, exit);
  SOC_PETRA_REG_SET(regs->mcc.mbist_resets_reg, 0x0, 128, exit);

  /*
   * Assert TAP reset
   */
  SOC_PETRA_REG_SET(regs->mcc.tap_config_reg, 0x10, 130, exit);

  poll_info.expected_value = 0x0;
  res = soc_petra_status_fld_poll_unsafe(
          unit,
          SOC_PETRA_REG_DB_ACC_REF(regs->mcc.tap_config_reg.tap_cmd),
          SOC_PETRA_DEFAULT_INSTANCE,
          &poll_info,
          &poll_success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 132, exit);

  if (poll_success == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MGMT_SMS_ACTION_TIMOUT_ERR, 134, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_init_mem_correction()", 0, 0);
}

STATIC uint32
  soc_pb_init_dram_nof_buffs_calc(
    SOC_SAND_IN  uint32                   dram_size_total_mbyte,
    SOC_SAND_IN  SOC_PETRA_ITM_DBUFF_SIZE_BYTES dbuff_size,
    SOC_SAND_IN  SOC_PETRA_HW_QDR_PROTECT_TYPE  qdr_protection_mode,
    SOC_SAND_IN  SOC_PETRA_HW_QDR_SIZE_MBIT     qdr_total_size_mbit,
    SOC_SAND_OUT uint32                   *nof_dram_buffs
  )
{
  uint32
    max_buffs_by_qdr,
    qdr_nof_entries,
    max_buffs_by_dram,
    dbuff_nof_bits,
    dbuff_size_log_2;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_INIT_DRAM_NOF_BUFFS_CALC);
  SOC_SAND_CHECK_NULL_INPUT(nof_dram_buffs);

  dbuff_size_log_2 = soc_sand_log2_round_down(dbuff_size);

  if (dbuff_size_log_2 == 0)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DBUFF_SIZE_INVALID_ERR, 10, exit);
  }

  switch(qdr_protection_mode) {
  case SOC_PETRA_HW_QDR_PROTECT_TYPE_ECC:
    dbuff_nof_bits = 29 - dbuff_size_log_2;
    break;
  case SOC_PETRA_HW_QDR_PROTECT_TYPE_PARITY:
    dbuff_nof_bits = 31 - dbuff_size_log_2;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_HW_QDR_PROTECT_TYPE_INVALID_ERR, 20, exit);
  }

  SOC_SAND_LIMIT_FROM_ABOVE(dbuff_nof_bits, 21);

  max_buffs_by_qdr = soc_sand_power_of_2(dbuff_nof_bits);
  qdr_nof_entries = soc_petra_init_qdr_size_to_bits(qdr_total_size_mbit) / SOC_PETRA_INIT_QDR_ENTRY_SIZE_BITS;
  max_buffs_by_qdr = SOC_SAND_MIN(max_buffs_by_qdr, qdr_nof_entries);

  max_buffs_by_dram = (dram_size_total_mbyte * 1024 * 1024)/dbuff_size;
  SOC_SAND_LIMIT_FROM_ABOVE(max_buffs_by_dram, SOC_PETRA_ITM_NOF_DRAM_BUFFS);

  *nof_dram_buffs = SOC_SAND_MIN(max_buffs_by_qdr, max_buffs_by_dram);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_init_dram_nof_buffs_calc()", 0, 0);
}

/*
 * Get the number of fbc (cache) buffers needed for given amount of
 * DRAM buffers.
 */
STATIC uint32
  soc_pb_init_dram_fbc_buffs_get(
    SOC_SAND_IN  uint32  buffs_without_fbc,
    SOC_SAND_IN  uint32  buff_size_bytes,
    SOC_SAND_OUT uint32 *fbc_nof_bufs
  )
{
  uint32
    fbcs_for_buff,
    nof_fbc_buffs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_INIT_DRAM_FBC_BUFFS_GET);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(
    buff_size_bytes, SOC_PETRA_ITM_DBUFF_SIZE_BYTES_MIN, SOC_PETRA_ITM_DBUFF_SIZE_BYTES_MAX,
    SOC_PETRA_ITM_DRAM_BUF_SIZE_OUT_OF_RANGE_ERR, 10, exit
  );

  fbcs_for_buff = SOC_SAND_DIV_ROUND_UP(buff_size_bytes, SOC_PB_INIT_DRAM_BYTES_FOR_FBC);
  nof_fbc_buffs = SOC_SAND_DIV_ROUND_UP(buffs_without_fbc, fbcs_for_buff * SOC_PB_INIT_DRAM_FBC_SEQUENCE_SIZE);

  *fbc_nof_bufs = nof_fbc_buffs;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "soc_pb_init_dram_fbc_buffs_get()", 0, 0);
}

STATIC uint32
  soc_pb_init_dram_max_without_fbc_get(
    SOC_SAND_IN  uint32  buffs_with_fbc,
    SOC_SAND_IN  uint32  buff_size_bytes,
    SOC_SAND_OUT uint32 *buffs_without_fbc
  )
{
  SOC_SAND_U64
    dividend,
    buffs_no_fbc;
  uint32
    divisor;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_INIT_DRAM_MAX_WITHOUT_FBC_GET);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(
    buff_size_bytes, SOC_PETRA_ITM_DBUFF_SIZE_BYTES_MIN, SOC_PETRA_ITM_DBUFF_SIZE_BYTES_MAX,
    SOC_PETRA_ITM_DRAM_BUF_SIZE_OUT_OF_RANGE_ERR, 10, exit
  );

  soc_sand_u64_multiply_longs(
    buffs_with_fbc,
    (SOC_PB_INIT_DRAM_FBC_SEQUENCE_SIZE * buff_size_bytes),
    &dividend
  );

  divisor = SOC_PB_INIT_DRAM_FBC_SEQUENCE_SIZE * buff_size_bytes + SOC_PB_INIT_DRAM_BYTES_FOR_FBC;

  soc_sand_u64_devide_u64_long(
    &dividend,
    divisor,
    &buffs_no_fbc
  );

  soc_sand_u64_to_long(&buffs_no_fbc, buffs_without_fbc);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "soc_pb_init_dram_max_without_fbc_get()", 0, 0);
}

STATIC uint32
  soc_pb_init_dram_buff_boudaries_calc(
    SOC_SAND_IN  uint32                     total_buffs,
    SOC_SAND_IN  SOC_PETRA_ITM_DBUFF_SIZE_BYTES   dbuff_size,
    SOC_SAND_OUT SOC_PB_INIT_DBUFFS_BDRY       *dbuffs
  )
{
  uint32
    res,
    uc_plus_fbc,
    uc_no_fbc = 0,
    uc_no_fbc_orig,
    fmc_fbc_size,
    mmc_fbc_size,
    uc_fbc_size;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_INIT_DRAM_BUFF_BOUDARIES_CALC);

  SOC_SAND_CHECK_NULL_INPUT(dbuffs);

  /*
   *  Full Multicast
   */
  dbuffs->fmc.start  = 0;
  dbuffs->fmc.end    = SOC_SAND_RNG_LAST(dbuffs->fmc.start, SOC_PB_ITM_DBUFF_FMC_MAX);

  /*
   *  Mini Multicast
   */
  dbuffs->mmc.start  = SOC_SAND_RNG_NEXT(dbuffs->fmc.end);
  dbuffs->mmc.end    = SOC_SAND_RNG_LAST(dbuffs->mmc.start, SOC_PETRA_ITM_DBUFF_MMC_MAX);

  /*
   *  FBC-size
   */
  res = soc_pb_init_dram_fbc_buffs_get(
          SOC_SAND_RNG_COUNT(dbuffs->fmc.end, dbuffs->fmc.start),
          dbuff_size,
          &fmc_fbc_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_pb_init_dram_fbc_buffs_get(
          SOC_SAND_RNG_COUNT(dbuffs->mmc.end, dbuffs->mmc.start),
          dbuff_size,
          &mmc_fbc_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /*
   *  Unicast + FBC
   */
  dbuffs->uc.start   = SOC_SAND_RNG_NEXT(dbuffs->mmc.end);

  dbuffs->fbc_fmc.end    = total_buffs - 1;
  dbuffs->fbc_fmc.start  = SOC_SAND_RNG_FIRST(dbuffs->fbc_fmc.end, fmc_fbc_size);

  dbuffs->fbc_mmc.end    = SOC_SAND_RNG_PREV(dbuffs->fbc_fmc.start);
  dbuffs->fbc_mmc.start  = SOC_SAND_RNG_FIRST(dbuffs->fbc_mmc.end, mmc_fbc_size);

  uc_plus_fbc = dbuffs->fbc_mmc.start - dbuffs->uc.start - 1;

  res = soc_pb_init_dram_max_without_fbc_get(
          uc_plus_fbc,
          dbuff_size,
          &uc_no_fbc
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_pb_init_dram_fbc_buffs_get(
          uc_no_fbc,
          dbuff_size,
          &uc_fbc_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  /* Set minimal gap between DRAM regular and FBC buffers */
  uc_no_fbc_orig = uc_no_fbc;
  uc_no_fbc -= SOC_PB_INIT_DRAM_BUFF_TO_FBC_DELTA_MIN;

  if (uc_no_fbc + uc_fbc_size > uc_plus_fbc)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DRAM_BUFFERS_UC_FBC_OVERFLOW_ERR, 60, exit);
  }

  dbuffs->uc.end = SOC_SAND_RNG_LAST(dbuffs->uc.start, uc_no_fbc);

  /*
   *  Set the FBC to start according to the original
   *  UC buffers calculation. This guarantees the minimal gap of
   *  SOC_PB_INIT_DRAM_BUFF_TO_FBC_DELTA_MIN between regular
   *  and FBC buffers.
   */
  dbuffs->fbc_uc.start  = SOC_SAND_RNG_NEXT(SOC_SAND_RNG_LAST(dbuffs->uc.start, uc_no_fbc_orig));
  dbuffs->fbc_uc.end    = SOC_SAND_RNG_LAST(dbuffs->fbc_uc.start, uc_fbc_size);

  if (dbuffs->fbc_uc.end >= dbuffs->fbc_mmc.start)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DRAM_BUFFERS_FBC_OVERFLOW_ERR, 70, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "soc_pb_init_dram_buff_boudaries_calc()", 0, 0);
}

/*
 *  DRAM buffers initialization }
 */


/*
 *  All the configurations in this function must occur before setting
 *  Soc_petra internal blocks Out Of Reset.
 *  All these configurations affect ECC registers.
 *  This includes the following configurations:
 *  - DRAM Buffers
 *  - QDR protection type
 *  - Fabric CRC enable/disable
 *  - DRAM CRC enable/disable
 *  This function must be called at least once.
 */
STATIC uint32
  soc_pb_mgmt_init_before_blocks_oor(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PB_INIT_DBUFFS_BDRY         *dbuffs_bdries,
    SOC_SAND_IN  SOC_PB_HW_ADJUSTMENTS           *hw_adj
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    fld_val,
    reg_val;
  uint8
    is_stag,
    is_fap2x_coexist;
  SOC_PETRA_ITM_DBUFF_SIZE_BYTES
    dbuff_size;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_INIT_BEFORE_BLOCKS_OOR);

  SOC_SAND_CHECK_NULL_INPUT(hw_adj);

  dbuff_size = hw_adj->dram.dbuff_size;

  regs = soc_petra_regs();
  is_fap2x_coexist = soc_petra_sw_db_is_fap2x_in_system_get(unit);

  /*
   * Clear flags register just in case (should be 0x0 by default)
   */
  SOC_PB_REG_SET(regs->eci.spare_flags_reg, 0x0, 98, exit);

  /************************************************************************/
  /*  Configure DRAM PLL                                              */
  /************************************************************************/

  /* power-down/up old PLL signals */
  SOC_PB_REG_GET(regs->eci.pll_resets_reg, reg_val, 100, exit);
  if ((SOC_PB_REV_ABOVE_A0) || (hw_adj->dram.enable == FALSE))
  {
    fld_val = 0x1;
  }
  else
  {
    fld_val = 0x0;
  }
  SOC_PB_FLD_TO_REG(regs->eci.pll_resets_reg.ddr_pll_pwrdn, fld_val, reg_val, 101, exit);
  SOC_PB_FLD_TO_REG(regs->eci.pll_resets_reg.ddr_ab_pll_pwrdn, fld_val, reg_val, 102, exit);
  SOC_PB_FLD_TO_REG(regs->eci.pll_resets_reg.ddr_ef_pll_pwrdn, fld_val, reg_val, 104, exit);
  SOC_PB_REG_SET(regs->eci.pll_resets_reg, reg_val, 1, exit);

  /* In-reset */
  SOC_PB_REG_GET(regs->eci.pll_resets_reg, reg_val, 1, exit);
  fld_val = 0x0;
  SOC_PB_FLD_TO_REG(regs->eci.pll_resets_reg.ddr_pll_rst_n, fld_val, reg_val, 2, exit);
  SOC_PB_FLD_TO_REG(regs->eci.pll_resets_reg.ddr_ab_pll_rst_n, fld_val, reg_val, 3, exit);
  SOC_PB_FLD_TO_REG(regs->eci.pll_resets_reg.ddr_ef_pll_rst_n, fld_val, reg_val, 4, exit);

  /* QDR also */
  SOC_PB_FLD_TO_REG(regs->eci.pll_resets_reg.qdr_pll_rst_n, fld_val, reg_val, 5, exit);
  SOC_PB_REG_SET(regs->eci.pll_resets_reg, reg_val, 6, exit);

  if (hw_adj->dram.enable)
  {
    res = soc_pb_init_dram_pll_set(
            unit,
            &(hw_adj->dram.pll_conf),
            &(hw_adj->dram_pll)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 7, exit);

    /* Out-of-reset */
    SOC_PB_REG_GET(regs->eci.pll_resets_reg, reg_val, 20, exit);
    fld_val = 0x1;
    SOC_PB_FLD_TO_REG(regs->eci.pll_resets_reg.ddr_pll_rst_n, fld_val, reg_val, 21, exit);
    SOC_PB_FLD_TO_REG(regs->eci.pll_resets_reg.ddr_ab_pll_rst_n, fld_val, reg_val, 22, exit);
    SOC_PB_FLD_TO_REG(regs->eci.pll_resets_reg.ddr_ef_pll_rst_n, fld_val, reg_val, 23, exit);
    SOC_PB_REG_SET(regs->eci.pll_resets_reg, reg_val, 24, exit);
  }

  /************************************************************************/
  /*  Configure QDR PLL                                                   */
  /************************************************************************/

  soc_petra_sw_db_qdr_enable_set(unit, hw_adj->qdr.enable);
  
  if (hw_adj->qdr.enable)  {
   
    fld_val = (hw_adj->qdr.is_core_clock_freq)?0x1:0x0;
    SOC_PETRA_FLD_SET(regs->eci.pll_resets_reg.qdr_pll_sel, fld_val, 30, exit);

    if (!hw_adj->qdr.is_core_clock_freq)
    {
      /*
       *  Configure PLL
       */
      SOC_PETRA_FLD_SET(regs->eci.qdr_pll_config_reg.qdr_pll_m, hw_adj->qdr.pll_conf.m, 34, exit);
      SOC_PETRA_FLD_SET(regs->eci.qdr_pll_config_reg.qdr_pll_n, hw_adj->qdr.pll_conf.n, 36, exit);
      SOC_PETRA_FLD_SET(regs->eci.qdr_pll_config_reg.qdr_pll_p, hw_adj->qdr.pll_conf.p, 40, exit);

      /* Out-of-reset */
      SOC_PETRA_FLD_SET(regs->eci.pll_resets_reg.qdr_pll_rst_n, 0x1, 28, exit);
    }
  }

  /* Delay after DRAM/QDR PLL Out-Of-Reset */
  sal_msleep(SOC_PB_MGMT_INIT_TIMER_DELAY_MSEC);

  /*
   *  Streaming Interface (if used)
   */

  SOC_PB_FLD_GET(regs->eci.power_up_configurations_reg.cpuif_mode, fld_val, 70, exit);
  if (fld_val == 0x1)
  {
    /* Streaming is enabled - configure according to the HW-Adjust settings */
    reg_val = SOC_SAND_BOOL2NUM(hw_adj->streaming_if.multi_port_mode);
    SOC_PB_FLD_SET(regs->eci.cpu_streaming_interface_configurations_reg.cpuif_multi_port_mode, reg_val, 30, exit);

    reg_val = SOC_SAND_BOOL2NUM(hw_adj->streaming_if.enable_timeoutcnt);
    SOC_PB_FLD_SET(regs->eci.cpu_streaming_interface_configurations_reg.cpuif_enable_timeoutcnt, reg_val, 30, exit);

    reg_val = hw_adj->streaming_if.timeout_prd;
    SOC_PB_FLD_SET(regs->eci.cpu_streaming_interface_configurations_reg.cpuif_timeout_prd, reg_val, 30, exit);

    reg_val = SOC_SAND_BOOL2NUM(hw_adj->streaming_if.quiet_mode);
    SOC_PB_FLD_SET(regs->eci.cpu_streaming_interface_configurations_reg.cpuif_quiet_mode, reg_val, 30, exit);

    reg_val = SOC_SAND_BOOL2NUM(!hw_adj->streaming_if.discard_bad_parity);
    SOC_PB_FLD_SET(regs->eci.cpu_streaming_interface_configurations_reg.cpuif_no_dis_bad_parity, reg_val, 30, exit);

    reg_val = SOC_SAND_BOOL2NUM(hw_adj->streaming_if.discard_pkt_streaming);
    SOC_PB_FLD_SET(regs->eci.cpu_streaming_interface_configurations_reg.cpuif_dis_pkt_streaming, reg_val, 30, exit);
  }
  else
  {
    /* 
     * If streaming interface is disabled, discard streaming packets 
     */
    SOC_PB_FLD_SET(regs->eci.cpu_streaming_interface_configurations_reg.cpuif_dis_pkt_streaming, 0x1, 30, exit);
  }

  /************************************************************************/
  /* DRAM Buffers                                                         */
  /************************************************************************/

  /*
   *  ECI
   */

  if (hw_adj->dram.enable == TRUE)
  {
    res = soc_petra_itm_dbuff_size2internal(
            dbuff_size,
            &fld_val
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    SOC_PB_FLD_SET(regs->eci.general_controls_reg.dbuff_size, fld_val, 25, exit);

    SOC_PB_FLD_SET(
      regs->eci.unicast_dbuff_pointers_start_reg.uc_db_ptr_start, dbuffs_bdries->uc.start, 27, exit
    );
    SOC_PB_FLD_SET(
      regs->eci.unicast_dbuff_pointers_end_reg.uc_db_ptr_end, dbuffs_bdries->uc.end, 30, exit
    );

    SOC_PB_FLD_SET(
      regs->eci.mini_multicast_dbuff_pointers_start_reg.mn_mul_db_ptr_start, dbuffs_bdries->mmc.start, 40, exit
    );
    SOC_PB_FLD_SET(
      regs->eci.mini_multicast_dbuff_pointers_end_reg.mn_mul_db_ptr_end, dbuffs_bdries->mmc.end, 50, exit
    );

    SOC_PB_FLD_SET(
      regs->eci.full_multicast_dbuff_pointers_start_reg.fl_mul_db_ptr_start, dbuffs_bdries->fmc.start, 60, exit
    );
    SOC_PB_FLD_SET(
      regs->eci.full_multicast_dbuff_pointers_end_reg.fl_mul_db_ptr_end, dbuffs_bdries->fmc.end, 70, exit
    );
  }

  /************************************************************************/
  /* QDR protection type                                                  */
  /************************************************************************/

  if (hw_adj->qdr.enable)
  {
    fld_val =
      (hw_adj->qdr.protection_type == SOC_PETRA_HW_QDR_PROTECT_TYPE_PARITY)?0x1:0x0;

    SOC_PB_FLD_SET(regs->eci.general_controls_reg.qdr_par_sel, fld_val , 90, exit);
  }

  /************************************************************************/
  /* Fabric                                                               */
  /************************************************************************/

  /*
   *  Mesh Mode
   */
  if (
      (hw_adj->fabric.connect_mode == SOC_PETRA_FABRIC_CONNECT_MODE_MESH) &&
      (!is_fap2x_coexist)
     )
  {
    SOC_PB_FLD_SET(regs->eci.general_controls_reg.mesh_mode, 0x1, 100, exit);
  }
  else
  {
    /* Not enabled, also for BACK2BACK and single-context devices) */
    SOC_PB_FLD_SET(regs->eci.general_controls_reg.mesh_mode, 0x0, 110, exit);
  }

  /*
   *  CRC enable and send
   */
  fld_val = 0x1;
  SOC_PB_FLD_SET(regs->eci.general_controls_reg.add_dram_crc, fld_val, 120, exit);

  /*
   *  Disable fabric CRC only if fap20/fap21 in system
   */
  fld_val = SOC_SAND_BOOL2NUM(is_fap2x_coexist);
  SOC_PB_FLD_SET(regs->eci.general_controls_reg.no_fab_crc, fld_val, 130, exit);

  if (soc_petra_sw_db_is_fap20_in_system_get(unit))
  {
    fld_val = 0x1; /* Fap20v header */
  }
  else
  {
    fld_val = 0x0; /* Soc_petra header */
  }
  SOC_PB_FLD_SET(regs->eci.general_controls_reg.hdr_type, fld_val, 140, exit);

  res = soc_petra_ports_ftmh_extension_set_unsafe(
          unit,
          hw_adj->fabric.ftmh_extension
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

  is_stag = soc_petra_sw_db_stag_enable_get(unit);
  fld_val = SOC_SAND_BOOL2NUM(is_stag);

  SOC_PB_FLD_SET(regs->eci.general_controls_reg.stat_tag_en, fld_val, 160, exit);

  /*
   *  MCC + Mbist Resets
   */
  SOC_PETRA_REG_SET(regs->eci.soc_petra_soft_reset_reg, 0x7FFFFFF, 161, exit); 
  SOC_PB_FLD_SET(regs->eci.soc_petra_soft_reset_reg.mcc_reset, 0x0, 162, exit); 


  res = soc_pb_init_mem_correction(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 175, exit);

  /*
   *  Statistics Interface (part 1)
   */
  res = soc_petra_stat_if_info_set_unsafe(
          unit,
          &hw_adj->stat_if.if_conf
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 2, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_mgmt_init_before_blocks_oor()", 0, 0);
}

/*
 *  Complete the initialization based on pre-Out-Of-Reset configuration
 */
STATIC uint32
  soc_pb_mgmt_init_after_blocks_oor(
    SOC_SAND_IN int unit,
    SOC_SAND_IN  SOC_PB_HW_ADJUSTMENTS        *hw_adj,
    SOC_SAND_IN  SOC_PB_INIT_DBUFFS_BDRY      *dbuffs_bdries
  )
{
  uint32
    fld_val,
    res;
  uint8
    is_pp;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_INIT_AFTER_BLOCKS_OOR);

  regs = soc_petra_regs();

  /*
   *  Statistics Interface (part 2)
   */

  if (hw_adj->stat_if.if_conf.enable)
  {
    res = soc_pb_stat_if_report_set_unsafe(
            unit,
            &hw_adj->stat_if.rep_conf
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 2, exit);
  }

  /************************************************************************/
  /* DRAM Buffers - get configuration from ECI                            */
  /************************************************************************/

  if (hw_adj->dram.enable == TRUE)
  {
    /*
     *  IDR - auto-generated +
     *  IDR - FBC (caching) configuration
     */
    SOC_PETRA_FLD_SET(
      regs->idr.fbc_unicast_autogen_start_reg.fbc_unicast_autogen_start, dbuffs_bdries->uc.start, 140, exit
    );
    SOC_PETRA_FLD_SET(
      regs->idr.fbc_unicast_autogen_end_reg.fbc_unicast_autogen_end, dbuffs_bdries->uc.end, 150, exit
    );

    SOC_PETRA_FLD_SET(
      regs->idr.fbc_mini_multicast_autogen_start_reg.fbc_mini_multicast_autogen_start, dbuffs_bdries->mmc.start, 160, exit
    );
    SOC_PETRA_FLD_SET(
      regs->idr.fbc_mini_multicast_autogen_end_reg.fbc_mini_multicast_autogen_end, dbuffs_bdries->mmc.end, 170, exit
    );

    SOC_PETRA_FLD_SET(
      regs->idr.fbc_full_multicast_autogen_start_reg.fbc_full_multicast_autogen_start, dbuffs_bdries->fmc.start, 180, exit
    );
    SOC_PETRA_FLD_SET(
      regs->idr.fbc_full_multicast_autogen_end_reg.fbc_full_multicast_autogen_end, dbuffs_bdries->fmc.end, 190, exit
    );

    SOC_PETRA_FLD_SET(
      regs->idr.fbc_external_unicast_limit_low_reg.fbc_external_unicast_limit_low, dbuffs_bdries->fbc_uc.start, 85, exit
    );
    SOC_PETRA_FLD_SET(
      regs->idr.fbc_external_unicast_limit_high_reg.fbc_external_unicast_limit_high, dbuffs_bdries->fbc_uc.end, 90, exit
    );

    SOC_PETRA_FLD_SET(
      regs->idr.fbc_external_mini_multicast_limit_low_reg.fbc_external_mini_multicast_limit_low, dbuffs_bdries->fbc_mmc.start, 100, exit
    );
    SOC_PETRA_FLD_SET(
      regs->idr.fbc_external_mini_multicast_limit_high_reg.fbc_external_mini_multicast_limit_high, dbuffs_bdries->fbc_mmc.end, 110, exit
    );

    SOC_PETRA_FLD_SET(
      regs->idr.fbc_external_full_multicast_limit_low_reg.fbc_external_full_multicast_limit_low, dbuffs_bdries->fbc_fmc.start, 120, exit
    );
    SOC_PETRA_FLD_SET(
      regs->idr.fbc_external_full_multicast_limit_high_reg.fbc_external_full_multicast_limit_high, dbuffs_bdries->fbc_fmc.end, 130, exit
    );

    /*
     *  IDR - enablers
     */
    fld_val = (dbuffs_bdries->uc.start <= dbuffs_bdries->uc.end)?0x1:0x0;
    SOC_PETRA_FLD_SET(
      regs->idr.static_configuration_reg.fbc_unicast_autogen_enable, fld_val, 200, exit
    );

    fld_val = (dbuffs_bdries->mmc.start <= dbuffs_bdries->mmc.end)?0x1:0x0;
    SOC_PETRA_FLD_SET(
      regs->idr.static_configuration_reg.fbc_mini_multicast_autogen_enable, fld_val , 210, exit
    );

    fld_val = (dbuffs_bdries->fmc.start <= dbuffs_bdries->fmc.end)?0x1:0x0;
    SOC_PETRA_FLD_SET(
      regs->idr.static_configuration_reg.fbc_full_multicast_autogen_enable, fld_val, 220, exit
    );
  }

  /*
   *  TCAM disable if not PP
   */
  is_pp = soc_petra_sw_db_pp_enable_get(unit);

  fld_val = is_pp?0x0:0x1;
  SOC_PB_FLD_SET(regs->ihb.tcam_power_reg.tcam_disable, fld_val, 230, exit);
  SOC_PB_FLD_SET(regs->ihb.tcam_power_reg.tcam_power_down, fld_val, 240, exit);

  SOC_PB_FLD_SET(regs->eci.spare_flags_reg.pp_enable, SOC_SAND_BOOL2NUM(is_pp), 250, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mgmt_init_after_blocks_oor()", 0, 0);
}

/*
 *  Must run before traffic is enabled
 */
STATIC uint32
  soc_pb_mgmt_init_finalize(
    SOC_SAND_IN int unit
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_INIT_SEQUENCE_PHASE2_UNSAFE);

  regs = soc_petra_regs();

  soc_petra_sw_db_qdr_enable_get(unit);

  if (soc_petra_sw_db_qdr_enable_get(unit))
  {
    /*
     * QDR interface is enabled
     */

    SOC_PETRA_FLD_SET(regs->qdr.periodic_enable_reg.prd_trn_en, 0x0, 10, exit);

    /*
     *  Clear QDR Memory and ECC counters
     */
    res = soc_petra_init_qdr_mem_clear(
            unit,
            soc_petra_sw_db_qdr_size_get(unit),
            soc_petra_sw_db_qdr_protection_type_get(unit)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mgmt_init_finalize()", 0, 0);
}

#if SOC_PB_MGMT_TBL_WRKLOAD_DISPLAY_EN
/*
 *  Tables Initialization {
 */
STATIC void
  soc_pb_mgmt_tbl_total_lines_set()
{
  uint32
    lines_count_total = 0;
  uint32
    tbl_ndx;

  SOC_PB_TBLS
    *tables;

  tables = soc_pb_tbls();

  lines_count_total =
    tables->ire.nif_ctxt_map_tbl.addr.size +
    tables->ire.nif_port_to_ctxt_bit_map_tbl.addr.size +
    tables->ire.rcy_ctxt_map_tbl.addr.size +
    tables->ire.tdm_config_tbl.addr.size +
    tables->idr.context_mru_tbl.addr.size +
    tables->idr.ethernet_meter_profiles_tbl.addr.size +
    tables->idr.ethernet_meter_config_tbl.addr.size +
    tables->idr.global_meter_profiles_tbl.addr.size +
    tables->idr.global_meter_status_tbl.addr.size +
    tables->irr.is_ingress_replication_db_tbl.addr.size +
    tables->irr.ingress_replication_multicast_db_tbl.addr.size +
    tables->irr.snoop_mirror_table0_tbl.addr.size +
    tables->irr.snoop_mirror_table1_tbl.addr.size +
    tables->irr.free_pcb_memory_tbl.addr.size +
    tables->irr.pcb_link_table_tbl.addr.size +
    tables->irr.is_free_pcb_memory_tbl.addr.size +
    tables->irr.is_pcb_link_table_tbl.addr.size +
    tables->irr.rpf_memory_tbl.addr.size +
    tables->irr.mcr_memory_tbl.addr.size +
    tables->irr.isf_memory_tbl.addr.size +
    tables->irr.destination_table_tbl.addr.size +
    tables->irr.lag_to_lag_range_tbl.addr.size +
    tables->irr.lag_mapping_tbl.addr.size +
    tables->irr.lag_next_member_tbl.addr.size +
    tables->irr.smooth_division_tbl.addr.size +
    tables->irr.traffic_class_mapping_tbl.addr.size +
    tables->ihp.port_mine_table_lag_port_tbl.addr.size +
    tables->ihp.tm_port_pp_port_config_tbl.addr.size +
    tables->ihp.tm_port_sys_port_config_tbl.addr.size +
    tables->ihp.pp_port_info_tbl.addr.size +
    tables->ihp.pp_port_values_tbl.addr.size +
    tables->ihb.pinfo_lbp_tbl.addr.size +
    tables->ihb.header_profile_tbl.addr.size +
    tables->ihb.pinfo_pmf_tbl.addr.size +
    tables->ihb.packet_format_code_profile_tbl.addr.size +
    tables->ihb.src_dest_port_for_l3_acl_key_tbl.addr.size +
    tables->ihb.direct_1st_pass_key_profile_resolved_data_tbl.addr.size +
    tables->ihb.direct_2nd_pass_key_profile_resolved_data_tbl.addr.size +
    tables->ihb.fem0_4b_1st_pass_key_profile_resolved_data_tbl.addr.size +
    tables->ihb.fem0_4b_1st_pass_program_resolved_data_tbl.addr.size +
    tables->ihb.fem0_4b_1st_pass_map_index_table_tbl.addr.size +
    tables->ihb.fem0_4b_1st_pass_map_table_tbl.addr.size +
    tables->ihb.fem1_4b_1st_pass_key_profile_resolved_data_tbl.addr.size +
    tables->ihb.fem1_4b_1st_pass_program_resolved_data_tbl.addr.size +
    tables->ihb.fem1_4b_1st_pass_map_index_table_tbl.addr.size +
    tables->ihb.fem1_4b_1st_pass_map_table_tbl.addr.size +
    tables->ihb.fem2_14b_1st_pass_key_profile_resolved_data_tbl.addr.size +
    tables->ihb.fem2_14b_1st_pass_program_resolved_data_tbl.addr.size +
    tables->ihb.fem2_14b_1st_pass_map_index_table_tbl.addr.size +
    tables->ihb.fem2_14b_1st_pass_map_table_tbl.addr.size +
    tables->ihb.fem2_14b_1st_pass_offset_table_tbl.addr.size +
    tables->ihb.fem3_14b_1st_pass_key_profile_resolved_data_tbl.addr.size +
    tables->ihb.fem3_14b_1st_pass_program_resolved_data_tbl.addr.size +
    tables->ihb.fem3_14b_1st_pass_map_index_table_tbl.addr.size +
    tables->ihb.fem3_14b_1st_pass_map_table_tbl.addr.size +
    tables->ihb.fem3_14b_1st_pass_offset_table_tbl.addr.size +
    tables->ihb.fem4_14b_1st_pass_key_profile_resolved_data_tbl.addr.size +
    tables->ihb.fem4_14b_1st_pass_program_resolved_data_tbl.addr.size +
    tables->ihb.fem4_14b_1st_pass_map_index_table_tbl.addr.size +
    tables->ihb.fem4_14b_1st_pass_map_table_tbl.addr.size +
    tables->ihb.fem4_14b_1st_pass_offset_table_tbl.addr.size +
    tables->ihb.fem5_17b_1st_pass_key_profile_resolved_data_tbl.addr.size +
    tables->ihb.fem5_17b_1st_pass_program_resolved_data_tbl.addr.size +
    tables->ihb.fem5_17b_1st_pass_map_index_table_tbl.addr.size +
    tables->ihb.fem5_17b_1st_pass_map_table_tbl.addr.size +
    tables->ihb.fem5_17b_1st_pass_offset_table_tbl.addr.size +
    tables->ihb.fem6_17b_1st_pass_key_profile_resolved_data_tbl.addr.size +
    tables->ihb.fem6_17b_1st_pass_program_resolved_data_tbl.addr.size +
    tables->ihb.fem6_17b_1st_pass_map_index_table_tbl.addr.size +
    tables->ihb.fem6_17b_1st_pass_map_table_tbl.addr.size +
    tables->ihb.fem6_17b_1st_pass_offset_table_tbl.addr.size +
    tables->ihb.fem7_17b_1st_pass_key_profile_resolved_data_tbl.addr.size +
    tables->ihb.fem7_17b_1st_pass_program_resolved_data_tbl.addr.size +
    tables->ihb.fem7_17b_1st_pass_map_index_table_tbl.addr.size +
    tables->ihb.fem7_17b_1st_pass_map_table_tbl.addr.size +
    tables->ihb.fem7_17b_1st_pass_offset_table_tbl.addr.size +
    tables->ihb.fem0_4b_2nd_pass_key_profile_resolved_data_tbl.addr.size +
    tables->ihb.fem0_4b_2nd_pass_program_resolved_data_tbl.addr.size +
    tables->ihb.fem0_4b_2nd_pass_map_index_table_tbl.addr.size +
    tables->ihb.fem0_4b_2nd_pass_map_table_tbl.addr.size +
    tables->ihb.fem1_4b_2nd_pass_key_profile_resolved_data_tbl.addr.size +
    tables->ihb.fem1_4b_2nd_pass_program_resolved_data_tbl.addr.size +
    tables->ihb.fem1_4b_2nd_pass_map_index_table_tbl.addr.size +
    tables->ihb.fem1_4b_2nd_pass_map_table_tbl.addr.size +
    tables->ihb.fem2_14b_2nd_pass_key_profile_resolved_data_tbl.addr.size +
    tables->ihb.fem2_14b_2nd_pass_program_resolved_data_tbl.addr.size +
    tables->ihb.fem2_14b_2nd_pass_map_index_table_tbl.addr.size +
    tables->ihb.fem2_14b_2nd_pass_map_table_tbl.addr.size +
    tables->ihb.fem2_14b_2nd_pass_offset_table_tbl.addr.size +
    tables->ihb.fem3_14b_2nd_pass_key_profile_resolved_data_tbl.addr.size +
    tables->ihb.fem3_14b_2nd_pass_program_resolved_data_tbl.addr.size +
    tables->ihb.fem3_14b_2nd_pass_map_index_table_tbl.addr.size +
    tables->ihb.fem3_14b_2nd_pass_map_table_tbl.addr.size +
    tables->ihb.fem3_14b_2nd_pass_offset_table_tbl.addr.size +
    tables->ihb.fem4_14b_2nd_pass_key_profile_resolved_data_tbl.addr.size +
    tables->ihb.fem4_14b_2nd_pass_program_resolved_data_tbl.addr.size +
    tables->ihb.fem4_14b_2nd_pass_map_index_table_tbl.addr.size +
    tables->ihb.fem4_14b_2nd_pass_map_table_tbl.addr.size +
    tables->ihb.fem4_14b_2nd_pass_offset_table_tbl.addr.size +
    tables->ihb.fem5_17b_2nd_pass_key_profile_resolved_data_tbl.addr.size +
    tables->ihb.fem5_17b_2nd_pass_program_resolved_data_tbl.addr.size +
    tables->ihb.fem5_17b_2nd_pass_map_index_table_tbl.addr.size +
    tables->ihb.fem5_17b_2nd_pass_map_table_tbl.addr.size +
    tables->ihb.fem5_17b_2nd_pass_offset_table_tbl.addr.size +
    tables->ihb.fem6_17b_2nd_pass_key_profile_resolved_data_tbl.addr.size +
    tables->ihb.fem6_17b_2nd_pass_program_resolved_data_tbl.addr.size +
    tables->ihb.fem6_17b_2nd_pass_map_index_table_tbl.addr.size +
    tables->ihb.fem6_17b_2nd_pass_map_table_tbl.addr.size +
    tables->ihb.fem6_17b_2nd_pass_offset_table_tbl.addr.size +
    tables->ihb.fem7_17b_2nd_pass_key_profile_resolved_data_tbl.addr.size +
    tables->ihb.fem7_17b_2nd_pass_program_resolved_data_tbl.addr.size +
    tables->ihb.fem7_17b_2nd_pass_map_index_table_tbl.addr.size +
    tables->ihb.fem7_17b_2nd_pass_map_table_tbl.addr.size +
    tables->ihb.fem7_17b_2nd_pass_offset_table_tbl.addr.size +
    tables->iqm.bdb_link_list_tbl.addr.size +
    tables->iqm.dynamic_tbl.addr.size +
    tables->iqm.static_tbl.addr.size +
    tables->iqm.packet_queue_tail_pointer_tbl.addr.size +
    tables->iqm.packet_queue_red_weight_table_tbl.addr.size +
    tables->iqm.credit_discount_table_tbl.addr.size +
    tables->iqm.full_user_count_memory_tbl.addr.size +
    tables->iqm.mini_multicast_user_count_memory_tbl.addr.size +
    tables->iqm.packet_queue_red_parameters_table_tbl.addr.size +
    tables->iqm.vsq_descriptor_rate_class_group_a_tbl.addr.size +
    tables->iqm.vsq_descriptor_rate_class_group_b_tbl.addr.size +
    tables->iqm.vsq_descriptor_rate_class_group_c_tbl.addr.size +
    tables->iqm.vsq_descriptor_rate_class_group_d_tbl.addr.size +
    tables->iqm.vsq_qsize_memory_group_a_tbl.addr.size +
    tables->iqm.vsq_qsize_memory_group_b_tbl.addr.size +
    tables->iqm.vsq_qsize_memory_group_c_tbl.addr.size +
    tables->iqm.vsq_qsize_memory_group_d_tbl.addr.size +
    tables->iqm.vsq_average_qsize_memory_group_a_tbl.addr.size +
    tables->iqm.vsq_average_qsize_memory_group_b_tbl.addr.size +
    tables->iqm.vsq_average_qsize_memory_group_c_tbl.addr.size +
    tables->iqm.vsq_average_qsize_memory_group_d_tbl.addr.size;

  for (tbl_ndx = 0; tbl_ndx < SOC_PETRA_NOF_VSQ_GROUPS; ++tbl_ndx)
  {
    lines_count_total +=
      tables->iqm.vsq_flow_control_parameters_table_group_tbl[tbl_ndx].addr.size +
      tables->iqm.vsq_queue_parameters_table_group_tbl[tbl_ndx].addr.size;
  }

  lines_count_total +=
    tables->iqm.system_red_parameters_table_tbl.addr.size +
    tables->iqm.system_red_drop_probability_values_tbl.addr.size +
    tables->iqm.system_red_tbl.addr.size +
    tables->iqm.cnm_descriptor_static_tbl.addr.size +
    tables->iqm.cnm_descriptor_dynamic_tbl.addr.size +
    tables->iqm.cnm_parameters_table_tbl.addr.size +
    tables->iqm.meter_processor_result_resolve_table_static_tbl.addr.size +
    tables->iqm.prfsela_tbl.addr.size +
    tables->iqm.prfselb_tbl.addr.size +
    tables->iqm.prfcfga_tbl.addr.size +
    tables->iqm.prfcfgb_tbl.addr.size +
    tables->iqm.normal_dynamica_tbl.addr.size +
    tables->iqm.normal_dynamicb_tbl.addr.size +
    tables->iqm.high_dynamica_tbl.addr.size +
    tables->iqm.high_dynamicb_tbl.addr.size +
    tables->iqm.cnts_mema_tbl.addr.size +
    tables->iqm.cnts_memb_tbl.addr.size +
    tables->iqm.ovth_mema_tbl.addr.size +
    tables->iqm.ovth_memb_tbl.addr.size +
    tables->ips.system_physical_port_lookup_table_tbl.addr.size +
    tables->ips.destination_device_and_port_lookup_table_tbl.addr.size +
    tables->ips.flow_id_lookup_table_tbl.addr.size +
    tables->ips.queue_type_lookup_table_tbl.addr.size +
    tables->ips.queue_priority_map_select_tbl.addr.size +
    tables->ips.queue_priority_maps_table_tbl.addr.size +
    tables->ips.queue_size_based_thresholds_table_tbl.addr.size +
    tables->ips.credit_balance_based_thresholds_table_tbl.addr.size +
    tables->ips.empty_queue_credit_balance_table_tbl.addr.size +
    tables->ips.credit_watchdog_thresholds_table_tbl.addr.size +
    tables->ips.queue_descriptor_table_tbl.addr.size +
    tables->ips.queue_size_table_tbl.addr.size +
    tables->ips.system_red_max_queue_size_table_tbl.addr.size +
    tables->ipt.bdq_tbl.addr.size +
    tables->ipt.pcq_tbl.addr.size +
    tables->ipt.sop_mmu_tbl.addr.size +
    tables->ipt.mop_mmu_tbl.addr.size +
    tables->ipt.fdtctl_tbl.addr.size +
    tables->ipt.fdtdata_tbl.addr.size +
    tables->ipt.egqctl_tbl.addr.size +
    tables->ipt.egqdata_tbl.addr.size +
    tables->ipt.egq_txq_wr_addr_tbl.addr.size +
    tables->ipt.egq_txq_rd_addr_tbl.addr.size +
    tables->ipt.fdt_txq_wr_addr_tbl.addr.size +
    tables->ipt.fdt_txq_rd_addr_tbl.addr.size +
    tables->ipt.gci_backoff_mask_tbl.addr.size +
    (tables->egq.nif_scm_tbl.addr.size * SOC_PB_NOF_MAC_LANES) +
    tables->egq.rcy_scm_tbl.addr.size +
    tables->egq.cpu_scm_tbl.addr.size +
    tables->egq.ccm_tbl.addr.size +
    tables->egq.pmc_tbl.addr.size +
    tables->egq.cbm_tbl.addr.size +
    tables->egq.dwm_new_tbl.addr.size +
    tables->egq.dwm_tbl.addr.size +
    tables->egq.pct_tbl.addr.size +
    tables->egq.vlan_table_tbl.addr.size +
    tables->egq.tc_dp_map_tbl.addr.size +
    tables->egq.cfc_flow_control_tbl.addr.size +
    tables->egq.nifa_flow_control_tbl.addr.size +
    tables->egq.nifb_flow_control_tbl.addr.size +
    tables->egq.cpu_last_header_tbl.addr.size +
    tables->egq.ipt_last_header_tbl.addr.size +
    tables->egq.fdr_last_header_tbl.addr.size +
    tables->egq.cpu_packet_counter_tbl.addr.size +
    tables->egq.ipt_packet_counter_tbl.addr.size +
    tables->egq.fdr_packet_counter_tbl.addr.size +
    tables->egq.rqp_packet_counter_tbl.addr.size +
    tables->egq.rqp_discard_packet_counter_tbl.addr.size +
    tables->egq.ehp_unicast_packet_counter_tbl.addr.size +
    tables->egq.ehp_multicast_high_packet_counter_tbl.addr.size +
    tables->egq.ehp_multicast_low_packet_counter_tbl.addr.size +
    tables->egq.ehp_discard_packet_counter_tbl.addr.size +
    tables->egq.pqp_unicast_high_packet_counter_tbl.addr.size +
    tables->egq.pqp_unicast_low_packet_counter_tbl.addr.size +
    tables->egq.pqp_multicast_high_packet_counter_tbl.addr.size +
    tables->egq.pqp_multicast_low_packet_counter_tbl.addr.size +
    tables->egq.pqp_unicast_high_bytes_counter_tbl.addr.size +
    tables->egq.pqp_unicast_low_bytes_counter_tbl.addr.size +
    tables->egq.pqp_multicast_high_bytes_counter_tbl.addr.size +
    tables->egq.pqp_multicast_low_bytes_counter_tbl.addr.size +
    tables->egq.pqp_discard_unicast_packet_counter_tbl.addr.size +
    tables->egq.pqp_discard_multicast_packet_counter_tbl.addr.size +
    tables->egq.fqp_packet_counter_tbl.addr.size +
    tables->egq.fqp_nif_port_mux_tbl.addr.size +
    tables->egq.key_profile_map_index_tbl.addr.size +
    tables->egq.tcam_key_resolution_profile_tbl.addr.size +
    tables->egq.parser_last_sys_record_tbl.addr.size +
    tables->egq.parser_last_nwk_record1_tbl.addr.size +
    tables->egq.parser_last_nwk_record2_tbl.addr.size +
    tables->egq.erpp_debug_tbl.addr.size +
    tables->epni.epe_packet_counter_tbl.addr.size +
    tables->epni.epe_bytes_counter_tbl.addr.size +
    tables->epni.copy_engine_program_tbl[0].addr.size +
    tables->epni.copy_engine_program_tbl[1].addr.size +
    tables->epni.copy_engine_program_tbl[2].addr.size +
    tables->epni.lfem_field_select_map_tbl[0].addr.size +
    tables->epni.lfem_field_select_map_tbl[1].addr.size +
    tables->epni.lfem_field_select_map_tbl[2].addr.size +
    tables->cfc.rcl2_ofp_tbl.addr.size +
    tables->cfc.nifclsb2_ofp_tbl.addr.size +
    tables->cfc.calrxa_tbl.addr.size +
    tables->cfc.calrxb_tbl.addr.size +
    tables->cfc.oob0_sch_map_tbl.addr.size +
    tables->cfc.oob1_sch_map_tbl.addr.size +
    tables->cfc.caltx_tbl.addr.size +
    tables->cfc.ilkn_calrx_tbl.addr.size +
    tables->cfc.ilkn_calrx_tbl.addr.size +
    tables->cfc.ilkn_sch_map_tbl.addr.size +
    tables->cfc.ilkn_sch_map_tbl.addr.size +
    tables->cfc.ilkn_caltx_tbl.addr.size +
    tables->cfc.ilkn_caltx_tbl.addr.size +
    tables->sch.cal_tbl.addr.size +
    tables->sch.dsm_tbl.addr.size +
    tables->sch.shds_tbl.addr.size +
    tables->sch.sem_tbl.addr.size +
    tables->sch.fsf_tbl.addr.size +
    tables->sch.fgm_tbl.addr.size +
    tables->sch.shc_tbl.addr.size +
    tables->sch.scc_tbl.addr.size +
    tables->sch.sct_tbl.addr.size +
    tables->sch.ffm_tbl.addr.size +
    tables->sch.pqs_tbl.addr.size +
    tables->sch.scheduler_init_tbl.addr.size + 0;

  Soc_pb_mgmt_tbls_total_lines = lines_count_total;
}
#endif

#if SOC_PB_MGMT_TBL_WRKLOAD_DISPLAY_EN
STATIC uint32
  soc_pb_mgmt_tbl_total_lines_get()

{
  return Soc_pb_mgmt_tbls_total_lines;
}
#endif

STATIC uint32
  soc_pb_mgmt_ire_tbls_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    nof_lines,
    nof_lines_total,
    lines_max,
    first_entry = 0;
  SOC_PB_TBLS
    *tables;
  SOC_PB_REGS
    *regs;
  SOC_PB_IRE_NIF_CTXT_MAP_TBL_DATA
    ire_nif_ctxt_map_tbl_data;
  SOC_PB_IRE_NIF_PORT_TO_CTXT_BIT_MAP_TBL_DATA
    ire_nif_port_to_ctxt_bit_map_tbl_data;
  SOC_PB_IRE_RCY_CTXT_MAP_TBL_DATA
    ire_rcy_ctxt_map_tbl_data;
  SOC_PB_IRE_TDM_CONFIG_TBL_DATA
    ire_tdm_config_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_IRE_TBLS_INIT);

  res = soc_pb_tbls_get(
          &tables
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  regs = soc_pb_regs();

  ret = SOC_SAND_OK; sal_memset(&ire_nif_ctxt_map_tbl_data, 0x0, sizeof(ire_nif_ctxt_map_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 11, exit);

  ret = SOC_SAND_OK; sal_memset(&ire_nif_port_to_ctxt_bit_map_tbl_data, 0x0, sizeof(ire_nif_port_to_ctxt_bit_map_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 12, exit);

  ret = SOC_SAND_OK; sal_memset(&ire_rcy_ctxt_map_tbl_data, 0x0, sizeof(ire_rcy_ctxt_map_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 13, exit);

  ret = SOC_SAND_OK; sal_memset(&ire_tdm_config_tbl_data, 0x0, sizeof(ire_tdm_config_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 14, exit);

  /*Nif Ctxt Map*/
  nof_lines_total = tables->ire.nif_ctxt_map_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ire.indirect_command_reg.indirect_command_count);
  ire_nif_ctxt_map_tbl_data.fap_port = SOC_PB_PORTS_IF_UNMAPPED_INDICATION;
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_ire_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 101, exit);

       res = soc_pb_ire_nif_ctxt_map_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ire_nif_ctxt_map_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 102, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Nif Port To Ctxt Bit Map*/
  nof_lines_total = tables->ire.nif_port_to_ctxt_bit_map_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ire.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_ire_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 103, exit);

       res = soc_pb_ire_nif_port_to_ctxt_bit_map_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ire_nif_port_to_ctxt_bit_map_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 104, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Rcy Ctxt Map*/
  nof_lines_total = tables->ire.rcy_ctxt_map_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ire.indirect_command_reg.indirect_command_count);
  ire_rcy_ctxt_map_tbl_data.fap_port = SOC_PB_PORTS_IF_UNMAPPED_INDICATION;
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_ire_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 105, exit);

       res = soc_pb_ire_rcy_ctxt_map_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ire_rcy_ctxt_map_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 106, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Tdm Config*/
  nof_lines_total = tables->ire.tdm_config_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ire.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_ire_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 107, exit);

       res = soc_pb_ire_tdm_config_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ire_tdm_config_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 108, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mgmt_ire_tbls_init()",0,0);
}

STATIC uint32
  soc_pb_mgmt_idr_tbls_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    nof_lines,
    nof_lines_total,
    lines_max,
    first_entry = 0;
  SOC_PB_TBLS
    *tables;
  SOC_PB_REGS
    *regs;
  SOC_PB_IDR_ETHERNET_METER_PROFILES_TBL_DATA
    idr_ethernet_meter_profiles_tbl_data;
  SOC_PB_IDR_ETHERNET_METER_CONFIG_TBL_DATA
    idr_ethernet_meter_config_tbl_data;
  SOC_PB_IDR_GLOBAL_METER_PROFILES_TBL_DATA
    idr_global_meter_profiles_tbl_data;
  SOC_PB_IDR_GLOBAL_METER_STATUS_TBL_DATA
    idr_global_meter_status_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_IDR_TBLS_INIT);

  res = soc_pb_tbls_get(
          &tables
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  regs = soc_pb_regs();

  ret = SOC_SAND_OK; sal_memset(&idr_ethernet_meter_profiles_tbl_data, 0x0, sizeof(idr_ethernet_meter_profiles_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 13, exit);

  ret = SOC_SAND_OK; sal_memset(&idr_ethernet_meter_config_tbl_data, 0x0, sizeof(idr_ethernet_meter_config_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 14, exit);

  ret = SOC_SAND_OK; sal_memset(&idr_global_meter_profiles_tbl_data, 0x0, sizeof(idr_global_meter_profiles_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 15, exit);

  ret = SOC_SAND_OK; sal_memset(&idr_global_meter_status_tbl_data, 0x0, sizeof(idr_global_meter_status_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 16, exit);
  
  /*Ethernet Meter Profiles*/
  nof_lines_total = tables->idr.ethernet_meter_profiles_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->idr.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_idr_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 105, exit);

       res = soc_pb_idr_ethernet_meter_profiles_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &idr_ethernet_meter_profiles_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 106, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Ethernet Meter Config*/
  nof_lines_total = tables->idr.ethernet_meter_config_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->idr.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_idr_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 107, exit);

       res = soc_pb_idr_ethernet_meter_config_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &idr_ethernet_meter_config_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 108, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Global Meter Profiles*/
  nof_lines_total = tables->idr.global_meter_profiles_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->idr.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_idr_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 109, exit);

       res = soc_pb_idr_global_meter_profiles_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &idr_global_meter_profiles_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Global Meter Status*/
  nof_lines_total = tables->idr.global_meter_status_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->idr.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_idr_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);

       res = soc_pb_idr_global_meter_status_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &idr_global_meter_status_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 112, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mgmt_idr_tbls_init()",0,0);
}

STATIC uint32
  soc_pb_mgmt_irr_tbls_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    nof_lines,
    nof_lines_total,
    lag_size,
    hash_indx,
    lines_max,
    first_entry = 0;
  SOC_PB_TBLS
    *tables;
  SOC_PB_REGS
    *regs;
  SOC_PETRA_IRR_IS_INGRESS_REPLICATION_DB_TBL_DATA
    is_ingress_replication_db_tbl_data;
  SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL_DATA
    ingress_replication_multicast_db_tbl_data;
  SOC_PB_IRR_MCR_MEMORY_TBL_DATA
    irr_mcr_memory_tbl_data;
  SOC_PB_IRR_ISF_MEMORY_TBL_DATA
    irr_isf_memory_tbl_data;
  SOC_PB_IRR_DESTINATION_TABLE_TBL_DATA
    irr_destination_table_tbl_data;
  SOC_PB_IRR_LAG_TO_LAG_RANGE_TBL_DATA
    irr_lag_to_lag_range_tbl_data;
  SOC_PB_IRR_LAG_MAPPING_TBL_DATA
    irr_lag_mapping_tbl_data;
  SOC_PB_IRR_LAG_NEXT_MEMBER_TBL_DATA
    irr_lag_next_member_tbl_data;
  SOC_PB_IRR_SMOOTH_DIVISION_TBL_DATA
    irr_smooth_division_tbl_data;
  SOC_PB_IRR_TRAFFIC_CLASS_MAPPING_TBL_DATA
    irr_traffic_class_mapping_tbl_data;
  SOC_PB_IRR_SNOOP_MIRROR_TABLE0_TBL_DATA
    irr_snoop_mirror_table0_tbl_data;
  SOC_PB_IRR_SNOOP_MIRROR_TABLE1_TBL_DATA
    irr_snoop_mirror_table1_tbl_data;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_IRR_TBLS_INIT);

  res = soc_pb_tbls_get(
          &tables
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  regs = soc_pb_regs();

  ret = SOC_SAND_OK; sal_memset(&is_ingress_replication_db_tbl_data, 0x0, sizeof(is_ingress_replication_db_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 11, exit);

  ret = SOC_SAND_OK; sal_memset(&ingress_replication_multicast_db_tbl_data, 0x0, sizeof(ingress_replication_multicast_db_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 12, exit);

  ret = SOC_SAND_OK; sal_memset(&irr_mcr_memory_tbl_data, 0x0, sizeof(irr_mcr_memory_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 23, exit);

  ret = SOC_SAND_OK; sal_memset(&irr_isf_memory_tbl_data, 0x0, sizeof(irr_isf_memory_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 24, exit);

  ret = SOC_SAND_OK; sal_memset(&irr_destination_table_tbl_data, 0x0, sizeof(irr_destination_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 15, exit);

  ret = SOC_SAND_OK; sal_memset(&irr_lag_to_lag_range_tbl_data, 0x0, sizeof(irr_lag_to_lag_range_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 16, exit);

  ret = SOC_SAND_OK; sal_memset(&irr_lag_mapping_tbl_data, 0x0, sizeof(irr_lag_mapping_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 17, exit);

  ret = SOC_SAND_OK; sal_memset(&irr_lag_next_member_tbl_data, 0x0, sizeof(irr_lag_next_member_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 18, exit);

  ret = SOC_SAND_OK; sal_memset(&irr_smooth_division_tbl_data, 0x0, sizeof(irr_smooth_division_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 19, exit);

  ret = SOC_SAND_OK; sal_memset(&irr_traffic_class_mapping_tbl_data, 0x0, sizeof(irr_traffic_class_mapping_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 20, exit);

  ret = SOC_SAND_OK; sal_memset(&irr_snoop_mirror_table0_tbl_data, 0x0, sizeof(irr_snoop_mirror_table0_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 21, exit);

  ret = SOC_SAND_OK; sal_memset(&irr_snoop_mirror_table1_tbl_data, 0x0, sizeof(irr_snoop_mirror_table1_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 21, exit);


  /*IRDB*/
  nof_lines_total = tables->irr.is_ingress_replication_db_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->irr.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_irr_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 101, exit);

       res = soc_petra_irr_is_ingress_replication_db_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &is_ingress_replication_db_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 102, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*MCDB*/
  nof_lines_total = tables->irr.ingress_replication_multicast_db_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->irr.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_irr_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 103, exit);

       res = soc_petra_irr_ingress_replication_multicast_db_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ingress_replication_multicast_db_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 104, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Mcr Memory*/
  nof_lines_total = tables->irr.mcr_memory_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->irr.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_irr_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 125, exit);

       res = soc_pb_irr_mcr_memory_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &irr_mcr_memory_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 126, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Isf Memory*/
  nof_lines_total = tables->irr.isf_memory_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->irr.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_irr_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 127, exit);

       res = soc_pb_irr_isf_memory_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &irr_isf_memory_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 128, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Destination Table*/
  nof_lines_total = tables->irr.destination_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->irr.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_irr_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 109, exit);

       res = soc_pb_irr_destination_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &irr_destination_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Lag To Lag Range*/
  nof_lines_total = tables->irr.lag_to_lag_range_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->irr.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_irr_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);

       res = soc_pb_irr_lag_to_lag_range_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &irr_lag_to_lag_range_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 112, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Lag Mapping*/
  nof_lines_total = tables->irr.lag_mapping_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->irr.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_irr_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 113, exit);

       res = soc_pb_irr_lag_mapping_tbl_set_unsafe(
                 unit,
                 first_entry,
                 0,
                 &irr_lag_mapping_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 114, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Lag Next Member*/
  nof_lines_total = tables->irr.lag_next_member_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->irr.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_irr_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 115, exit);

       res = soc_pb_irr_lag_next_member_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &irr_lag_next_member_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 116, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Smooth Division*/
  nof_lines_total = tables->irr.smooth_division_tbl.addr.size;
  for (lag_size = 0; lag_size < SOC_PETRA_PORTS_LAG_OUT_MEMBERS_MAX; ++lag_size)
  {
    for (hash_indx = 0; hash_indx < 256 ; ++hash_indx)
    {
      irr_smooth_division_tbl_data.member = hash_indx % (lag_size + 1);
      res = soc_pb_irr_smooth_division_tbl_set_unsafe(
              unit,
              lag_size,
              hash_indx,
              &irr_smooth_division_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 112, exit);
    }
  }
  first_entry = 0;

  /*Traffic Class Mapping*/
  nof_lines_total = tables->irr.traffic_class_mapping_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->irr.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_irr_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 119, exit);

       res = soc_pb_irr_traffic_class_mapping_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &irr_traffic_class_mapping_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;


  /*snoop mirror table 0*/
  nof_lines_total = tables->irr.snoop_mirror_table0_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->irr.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_irr_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 121, exit);

       res = soc_pb_irr_snoop_mirror_table0_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &irr_snoop_mirror_table0_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 121, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  
  /*snoop mirror table 1*/
  nof_lines_total = tables->irr.snoop_mirror_table1_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->irr.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_irr_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 121, exit);

       res = soc_pb_irr_snoop_mirror_table1_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &irr_snoop_mirror_table1_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 121, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mgmt_irr_tbls_init()",0,0);
}

  uint32
  soc_pb_mgmt_ihp_tbls_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    nof_lines,
    nof_lines_total,
    lines_max,
    first_entry = 0;
  SOC_PB_TBLS
    *tables;
  SOC_PB_REGS
    *regs;
  SOC_PB_IHP_PORT_MINE_TABLE_LAG_PORT_TBL_DATA
    ihp_port_mine_table_lag_port_tbl_data;
  SOC_PB_IHP_TM_PORT_PP_PORT_CONFIG_TBL_DATA
    ihp_tm_port_pp_port_config_tbl_data;
  SOC_PB_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_DATA
    ihp_tm_port_sys_port_config_tbl_data;
  SOC_PB_IHP_PP_PORT_VALUES_TBL_DATA
    ihp_pp_port_values_tbl_data;
  SOC_PB_PP_IHP_PINFO_LLR_TBL_DATA
      ihp_pinfo_llr_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_IHP_TBLS_INIT);

  res = soc_pb_tbls_get(
          &tables
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  regs = soc_pb_regs();

  ret = SOC_SAND_OK; sal_memset(&ihp_tm_port_pp_port_config_tbl_data, 0x0, sizeof(ihp_tm_port_pp_port_config_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 11, exit);

  ret = SOC_SAND_OK; sal_memset(&ihp_tm_port_sys_port_config_tbl_data, 0x0, sizeof(ihp_tm_port_sys_port_config_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 12, exit);

  ret = SOC_SAND_OK; sal_memset(&ihp_pp_port_values_tbl_data, 0x0, sizeof(ihp_pp_port_values_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 14, exit);

  ret = SOC_SAND_OK; sal_memset(&ihp_port_mine_table_lag_port_tbl_data, 0x0, sizeof(ihp_port_mine_table_lag_port_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 15, exit);

  ret = SOC_SAND_OK; sal_memset(&ihp_pinfo_llr_tbl_data, 0x0, sizeof(ihp_pinfo_llr_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 16, exit);

  /*Port Mine Table Lag Port*/
  nof_lines_total = tables->ihp.port_mine_table_lag_port_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihp.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_ihp_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 101, exit);

       res = soc_pb_ihp_port_mine_table_lag_port_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihp_port_mine_table_lag_port_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 102, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Tm Port Pp Port Config*/
  nof_lines_total = tables->ihp.tm_port_pp_port_config_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihp.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_ihp_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 101, exit);

       res = soc_pb_ihp_tm_port_pp_port_config_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihp_tm_port_pp_port_config_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 102, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Tm Port Sys Port Config*/
  nof_lines_total = tables->ihp.tm_port_sys_port_config_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihp.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_ihp_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 103, exit);

       res = soc_pb_ihp_tm_port_sys_port_config_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihp_tm_port_sys_port_config_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 104, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Pp Port Values*/
  nof_lines_total = tables->ihp.pp_port_values_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihp.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_ihp_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 107, exit);

       res = soc_pb_ihp_pp_port_values_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihp_pp_port_values_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /* Port info values */  
  do
  {
    ihp_pinfo_llr_tbl_data.default_cpu_trap_code = 0xff;
    /* TM settings always set snoop strengh to 0x3 when enabled */
    ihp_pinfo_llr_tbl_data.default_action_profile_snp = 0x1;    
    ihp_pinfo_llr_tbl_data.default_action_profile_fwd = 0x0;    
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_ihp_pinfo_llr_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &ihp_pinfo_llr_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 115, exit);
  } while(0);
  first_entry = 0;
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mgmt_ihp_tbls_init()",0,0);
}

uint32
  soc_pb_mgmt_ihb_tbls_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    nof_lines,
    nof_lines_total,
    lines_max,
    first_entry = 0;
  SOC_PB_TBLS
    *tables;
  SOC_PB_REGS
    *regs;
  SOC_PB_IHB_PINFO_LBP_TBL_DATA
    ihb_pinfo_lbp_tbl_data;
  SOC_PB_IHB_PINFO_PMF_TBL_DATA
    ihb_pinfo_pmf_tbl_data;
  SOC_PB_IHB_PACKET_FORMAT_CODE_PROFILE_TBL_DATA
    ihb_packet_format_code_profile_tbl_data;
  SOC_PB_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL_DATA
    ihb_src_dest_port_for_l3_acl_key_tbl_data;
  SOC_PB_IHB_DIRECT_KEY_PROFILE_RESOLVED_DATA_TBL_DATA
    ihb_direct_1st_pass_key_profile_resolved_data_tbl_data;
  SOC_PB_IHB_DIRECT_KEY_PROFILE_RESOLVED_DATA_TBL_DATA
    ihb_direct_2nd_pass_key_profile_resolved_data_tbl_data;
  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA
    ihb_fem0_4b_1st_pass_key_profile_resolved_data_tbl_data;
  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA
    ihb_fem0_4b_1st_pass_program_resolved_data_tbl_data;
  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA
    ihb_fem0_4b_1st_pass_map_index_table_tbl_data;
  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA
    ihb_fem0_4b_1st_pass_map_table_tbl_data;
  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA
    ihb_fem1_4b_1st_pass_key_profile_resolved_data_tbl_data;
  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA
    ihb_fem1_4b_1st_pass_program_resolved_data_tbl_data;
  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA
    ihb_fem1_4b_1st_pass_map_index_table_tbl_data;
  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA
    ihb_fem1_4b_1st_pass_map_table_tbl_data;
  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA
    ihb_fem2_14b_1st_pass_key_profile_resolved_data_tbl_data;
  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA
    ihb_fem2_14b_1st_pass_program_resolved_data_tbl_data;
  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA
    ihb_fem2_14b_1st_pass_map_index_table_tbl_data;
  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA
    ihb_fem2_14b_1st_pass_map_table_tbl_data;
  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA
    ihb_fem2_14b_1st_pass_offset_table_tbl_data;
  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA
    ihb_fem3_14b_1st_pass_key_profile_resolved_data_tbl_data;
  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA
    ihb_fem3_14b_1st_pass_program_resolved_data_tbl_data;
  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA
    ihb_fem3_14b_1st_pass_map_index_table_tbl_data;
  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA
    ihb_fem3_14b_1st_pass_map_table_tbl_data;
  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA
    ihb_fem3_14b_1st_pass_offset_table_tbl_data;
  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA
    ihb_fem4_14b_1st_pass_key_profile_resolved_data_tbl_data;
  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA
    ihb_fem4_14b_1st_pass_program_resolved_data_tbl_data;
  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA
    ihb_fem4_14b_1st_pass_map_index_table_tbl_data;
  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA
    ihb_fem4_14b_1st_pass_map_table_tbl_data;
  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA
    ihb_fem4_14b_1st_pass_offset_table_tbl_data;
  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA
    ihb_fem5_17b_1st_pass_key_profile_resolved_data_tbl_data;
  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA
    ihb_fem5_17b_1st_pass_program_resolved_data_tbl_data;
  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA
    ihb_fem5_17b_1st_pass_map_index_table_tbl_data;
  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA
    ihb_fem5_17b_1st_pass_map_table_tbl_data;
  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA
    ihb_fem5_17b_1st_pass_offset_table_tbl_data;
  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA
    ihb_fem6_17b_1st_pass_key_profile_resolved_data_tbl_data;
  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA
    ihb_fem6_17b_1st_pass_program_resolved_data_tbl_data;
  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA
    ihb_fem6_17b_1st_pass_map_index_table_tbl_data;
  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA
    ihb_fem6_17b_1st_pass_map_table_tbl_data;
  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA
    ihb_fem6_17b_1st_pass_offset_table_tbl_data;
  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA
    ihb_fem7_17b_1st_pass_key_profile_resolved_data_tbl_data;
  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA
    ihb_fem7_17b_1st_pass_program_resolved_data_tbl_data;
  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA
    ihb_fem7_17b_1st_pass_map_index_table_tbl_data;
  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA
    ihb_fem7_17b_1st_pass_map_table_tbl_data;
  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA
    ihb_fem7_17b_1st_pass_offset_table_tbl_data;
  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA
    ihb_fem0_4b_2nd_pass_key_profile_resolved_data_tbl_data;
  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA
    ihb_fem0_4b_2nd_pass_program_resolved_data_tbl_data;
  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA
    ihb_fem0_4b_2nd_pass_map_index_table_tbl_data;
  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA
    ihb_fem0_4b_2nd_pass_map_table_tbl_data;
  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA
    ihb_fem1_4b_2nd_pass_key_profile_resolved_data_tbl_data;
  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA
    ihb_fem1_4b_2nd_pass_program_resolved_data_tbl_data;
  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA
    ihb_fem1_4b_2nd_pass_map_index_table_tbl_data;
  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA
    ihb_fem1_4b_2nd_pass_map_table_tbl_data;
  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA
    ihb_fem2_14b_2nd_pass_key_profile_resolved_data_tbl_data;
  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA
    ihb_fem2_14b_2nd_pass_program_resolved_data_tbl_data;
  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA
    ihb_fem2_14b_2nd_pass_map_index_table_tbl_data;
  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA
    ihb_fem2_14b_2nd_pass_map_table_tbl_data;
  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA
    ihb_fem2_14b_2nd_pass_offset_table_tbl_data;
  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA
    ihb_fem3_14b_2nd_pass_key_profile_resolved_data_tbl_data;
  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA
    ihb_fem3_14b_2nd_pass_program_resolved_data_tbl_data;
  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA
    ihb_fem3_14b_2nd_pass_map_index_table_tbl_data;
  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA
    ihb_fem3_14b_2nd_pass_map_table_tbl_data;
  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA
    ihb_fem3_14b_2nd_pass_offset_table_tbl_data;
  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA
    ihb_fem4_14b_2nd_pass_key_profile_resolved_data_tbl_data;
  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA
    ihb_fem4_14b_2nd_pass_program_resolved_data_tbl_data;
  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA
    ihb_fem4_14b_2nd_pass_map_index_table_tbl_data;
  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA
    ihb_fem4_14b_2nd_pass_map_table_tbl_data;
  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA
    ihb_fem4_14b_2nd_pass_offset_table_tbl_data;
  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA
    ihb_fem5_17b_2nd_pass_key_profile_resolved_data_tbl_data;
  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA
    ihb_fem5_17b_2nd_pass_program_resolved_data_tbl_data;
  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA
    ihb_fem5_17b_2nd_pass_map_index_table_tbl_data;
  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA
    ihb_fem5_17b_2nd_pass_map_table_tbl_data;
  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA
    ihb_fem5_17b_2nd_pass_offset_table_tbl_data;
  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA
    ihb_fem6_17b_2nd_pass_key_profile_resolved_data_tbl_data;
  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA
    ihb_fem6_17b_2nd_pass_program_resolved_data_tbl_data;
  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA
    ihb_fem6_17b_2nd_pass_map_index_table_tbl_data;
  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA
    ihb_fem6_17b_2nd_pass_map_table_tbl_data;
  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA
    ihb_fem6_17b_2nd_pass_offset_table_tbl_data;
  SOC_PB_IHB_FEM_KEY_PROFILE_RESOLVED_DATA_TBL_DATA
    ihb_fem7_17b_2nd_pass_key_profile_resolved_data_tbl_data;
  SOC_PB_IHB_FEM_PROGRAM_RESOLVED_DATA_TBL_DATA
    ihb_fem7_17b_2nd_pass_program_resolved_data_tbl_data;
  SOC_PB_IHB_FEM_MAP_INDEX_TABLE_TBL_DATA
    ihb_fem7_17b_2nd_pass_map_index_table_tbl_data;
  SOC_PB_IHB_FEM_MAP_TABLE_TBL_DATA
    ihb_fem7_17b_2nd_pass_map_table_tbl_data;
  SOC_PB_IHB_FEM_OFFSET_TABLE_TBL_DATA
    ihb_fem7_17b_2nd_pass_offset_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_IHB_TBLS_INIT);

  res = soc_pb_tbls_get(
          &tables
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  regs = soc_pb_regs();

  ret = SOC_SAND_OK; sal_memset(&ihb_pinfo_lbp_tbl_data, 0x0, sizeof(ihb_pinfo_lbp_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 11, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_pinfo_pmf_tbl_data, 0x0, sizeof(ihb_pinfo_pmf_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 11, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_packet_format_code_profile_tbl_data, 0x0, sizeof(ihb_packet_format_code_profile_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 12, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_src_dest_port_for_l3_acl_key_tbl_data, 0x0, sizeof(ihb_src_dest_port_for_l3_acl_key_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 13, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_direct_1st_pass_key_profile_resolved_data_tbl_data, 0x0, sizeof(ihb_direct_1st_pass_key_profile_resolved_data_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 14, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_direct_2nd_pass_key_profile_resolved_data_tbl_data, 0x0, sizeof(ihb_direct_2nd_pass_key_profile_resolved_data_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 15, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem0_4b_1st_pass_key_profile_resolved_data_tbl_data, 0x0, sizeof(ihb_fem0_4b_1st_pass_key_profile_resolved_data_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 16, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem0_4b_1st_pass_program_resolved_data_tbl_data, 0x0, sizeof(ihb_fem0_4b_1st_pass_program_resolved_data_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 17, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem0_4b_1st_pass_map_index_table_tbl_data, 0x0, sizeof(ihb_fem0_4b_1st_pass_map_index_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 18, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem0_4b_1st_pass_map_table_tbl_data, 0x0, sizeof(ihb_fem0_4b_1st_pass_map_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 19, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem1_4b_1st_pass_key_profile_resolved_data_tbl_data, 0x0, sizeof(ihb_fem1_4b_1st_pass_key_profile_resolved_data_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 20, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem1_4b_1st_pass_program_resolved_data_tbl_data, 0x0, sizeof(ihb_fem1_4b_1st_pass_program_resolved_data_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 21, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem1_4b_1st_pass_map_index_table_tbl_data, 0x0, sizeof(ihb_fem1_4b_1st_pass_map_index_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 22, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem1_4b_1st_pass_map_table_tbl_data, 0x0, sizeof(ihb_fem1_4b_1st_pass_map_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 23, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem2_14b_1st_pass_key_profile_resolved_data_tbl_data, 0x0, sizeof(ihb_fem2_14b_1st_pass_key_profile_resolved_data_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 24, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem2_14b_1st_pass_program_resolved_data_tbl_data, 0x0, sizeof(ihb_fem2_14b_1st_pass_program_resolved_data_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 25, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem2_14b_1st_pass_map_index_table_tbl_data, 0x0, sizeof(ihb_fem2_14b_1st_pass_map_index_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 26, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem2_14b_1st_pass_map_table_tbl_data, 0x0, sizeof(ihb_fem2_14b_1st_pass_map_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 27, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem2_14b_1st_pass_offset_table_tbl_data, 0x0, sizeof(ihb_fem2_14b_1st_pass_offset_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 28, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem3_14b_1st_pass_key_profile_resolved_data_tbl_data, 0x0, sizeof(ihb_fem3_14b_1st_pass_key_profile_resolved_data_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 29, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem3_14b_1st_pass_program_resolved_data_tbl_data, 0x0, sizeof(ihb_fem3_14b_1st_pass_program_resolved_data_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 30, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem3_14b_1st_pass_map_index_table_tbl_data, 0x0, sizeof(ihb_fem3_14b_1st_pass_map_index_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 31, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem3_14b_1st_pass_map_table_tbl_data, 0x0, sizeof(ihb_fem3_14b_1st_pass_map_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 32, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem3_14b_1st_pass_offset_table_tbl_data, 0x0, sizeof(ihb_fem3_14b_1st_pass_offset_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 33, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem4_14b_1st_pass_key_profile_resolved_data_tbl_data, 0x0, sizeof(ihb_fem4_14b_1st_pass_key_profile_resolved_data_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 34, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem4_14b_1st_pass_program_resolved_data_tbl_data, 0x0, sizeof(ihb_fem4_14b_1st_pass_program_resolved_data_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 35, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem4_14b_1st_pass_map_index_table_tbl_data, 0x0, sizeof(ihb_fem4_14b_1st_pass_map_index_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 36, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem4_14b_1st_pass_map_table_tbl_data, 0x0, sizeof(ihb_fem4_14b_1st_pass_map_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 37, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem4_14b_1st_pass_offset_table_tbl_data, 0x0, sizeof(ihb_fem4_14b_1st_pass_offset_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 38, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem5_17b_1st_pass_key_profile_resolved_data_tbl_data, 0x0, sizeof(ihb_fem5_17b_1st_pass_key_profile_resolved_data_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 39, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem5_17b_1st_pass_program_resolved_data_tbl_data, 0x0, sizeof(ihb_fem5_17b_1st_pass_program_resolved_data_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 40, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem5_17b_1st_pass_map_index_table_tbl_data, 0x0, sizeof(ihb_fem5_17b_1st_pass_map_index_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 41, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem5_17b_1st_pass_map_table_tbl_data, 0x0, sizeof(ihb_fem5_17b_1st_pass_map_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 42, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem5_17b_1st_pass_offset_table_tbl_data, 0x0, sizeof(ihb_fem5_17b_1st_pass_offset_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 43, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem6_17b_1st_pass_key_profile_resolved_data_tbl_data, 0x0, sizeof(ihb_fem6_17b_1st_pass_key_profile_resolved_data_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 44, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem6_17b_1st_pass_program_resolved_data_tbl_data, 0x0, sizeof(ihb_fem6_17b_1st_pass_program_resolved_data_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 45, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem6_17b_1st_pass_map_index_table_tbl_data, 0x0, sizeof(ihb_fem6_17b_1st_pass_map_index_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 46, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem6_17b_1st_pass_map_table_tbl_data, 0x0, sizeof(ihb_fem6_17b_1st_pass_map_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 47, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem6_17b_1st_pass_offset_table_tbl_data, 0x0, sizeof(ihb_fem6_17b_1st_pass_offset_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 48, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem7_17b_1st_pass_key_profile_resolved_data_tbl_data, 0x0, sizeof(ihb_fem7_17b_1st_pass_key_profile_resolved_data_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 49, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem7_17b_1st_pass_program_resolved_data_tbl_data, 0x0, sizeof(ihb_fem7_17b_1st_pass_program_resolved_data_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 50, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem7_17b_1st_pass_map_index_table_tbl_data, 0x0, sizeof(ihb_fem7_17b_1st_pass_map_index_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 51, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem7_17b_1st_pass_map_table_tbl_data, 0x0, sizeof(ihb_fem7_17b_1st_pass_map_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 52, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem7_17b_1st_pass_offset_table_tbl_data, 0x0, sizeof(ihb_fem7_17b_1st_pass_offset_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 53, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem0_4b_2nd_pass_key_profile_resolved_data_tbl_data, 0x0, sizeof(ihb_fem0_4b_2nd_pass_key_profile_resolved_data_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 54, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem0_4b_2nd_pass_program_resolved_data_tbl_data, 0x0, sizeof(ihb_fem0_4b_2nd_pass_program_resolved_data_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 55, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem0_4b_2nd_pass_map_index_table_tbl_data, 0x0, sizeof(ihb_fem0_4b_2nd_pass_map_index_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 56, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem0_4b_2nd_pass_map_table_tbl_data, 0x0, sizeof(ihb_fem0_4b_2nd_pass_map_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 57, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem1_4b_2nd_pass_key_profile_resolved_data_tbl_data, 0x0, sizeof(ihb_fem1_4b_2nd_pass_key_profile_resolved_data_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 58, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem1_4b_2nd_pass_program_resolved_data_tbl_data, 0x0, sizeof(ihb_fem1_4b_2nd_pass_program_resolved_data_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 59, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem1_4b_2nd_pass_map_index_table_tbl_data, 0x0, sizeof(ihb_fem1_4b_2nd_pass_map_index_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 60, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem1_4b_2nd_pass_map_table_tbl_data, 0x0, sizeof(ihb_fem1_4b_2nd_pass_map_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 61, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem2_14b_2nd_pass_key_profile_resolved_data_tbl_data, 0x0, sizeof(ihb_fem2_14b_2nd_pass_key_profile_resolved_data_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 62, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem2_14b_2nd_pass_program_resolved_data_tbl_data, 0x0, sizeof(ihb_fem2_14b_2nd_pass_program_resolved_data_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 63, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem2_14b_2nd_pass_map_index_table_tbl_data, 0x0, sizeof(ihb_fem2_14b_2nd_pass_map_index_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 64, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem2_14b_2nd_pass_map_table_tbl_data, 0x0, sizeof(ihb_fem2_14b_2nd_pass_map_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 65, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem2_14b_2nd_pass_offset_table_tbl_data, 0x0, sizeof(ihb_fem2_14b_2nd_pass_offset_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 66, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem3_14b_2nd_pass_key_profile_resolved_data_tbl_data, 0x0, sizeof(ihb_fem3_14b_2nd_pass_key_profile_resolved_data_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 67, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem3_14b_2nd_pass_program_resolved_data_tbl_data, 0x0, sizeof(ihb_fem3_14b_2nd_pass_program_resolved_data_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 68, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem3_14b_2nd_pass_map_index_table_tbl_data, 0x0, sizeof(ihb_fem3_14b_2nd_pass_map_index_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 69, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem3_14b_2nd_pass_map_table_tbl_data, 0x0, sizeof(ihb_fem3_14b_2nd_pass_map_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 70, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem3_14b_2nd_pass_offset_table_tbl_data, 0x0, sizeof(ihb_fem3_14b_2nd_pass_offset_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 71, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem4_14b_2nd_pass_key_profile_resolved_data_tbl_data, 0x0, sizeof(ihb_fem4_14b_2nd_pass_key_profile_resolved_data_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 72, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem4_14b_2nd_pass_program_resolved_data_tbl_data, 0x0, sizeof(ihb_fem4_14b_2nd_pass_program_resolved_data_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 73, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem4_14b_2nd_pass_map_index_table_tbl_data, 0x0, sizeof(ihb_fem4_14b_2nd_pass_map_index_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 74, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem4_14b_2nd_pass_map_table_tbl_data, 0x0, sizeof(ihb_fem4_14b_2nd_pass_map_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 75, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem4_14b_2nd_pass_offset_table_tbl_data, 0x0, sizeof(ihb_fem4_14b_2nd_pass_offset_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 76, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem5_17b_2nd_pass_key_profile_resolved_data_tbl_data, 0x0, sizeof(ihb_fem5_17b_2nd_pass_key_profile_resolved_data_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 77, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem5_17b_2nd_pass_program_resolved_data_tbl_data, 0x0, sizeof(ihb_fem5_17b_2nd_pass_program_resolved_data_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 78, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem5_17b_2nd_pass_map_index_table_tbl_data, 0x0, sizeof(ihb_fem5_17b_2nd_pass_map_index_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 79, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem5_17b_2nd_pass_map_table_tbl_data, 0x0, sizeof(ihb_fem5_17b_2nd_pass_map_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 80, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem5_17b_2nd_pass_offset_table_tbl_data, 0x0, sizeof(ihb_fem5_17b_2nd_pass_offset_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 81, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem6_17b_2nd_pass_key_profile_resolved_data_tbl_data, 0x0, sizeof(ihb_fem6_17b_2nd_pass_key_profile_resolved_data_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 82, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem6_17b_2nd_pass_program_resolved_data_tbl_data, 0x0, sizeof(ihb_fem6_17b_2nd_pass_program_resolved_data_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 83, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem6_17b_2nd_pass_map_index_table_tbl_data, 0x0, sizeof(ihb_fem6_17b_2nd_pass_map_index_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 84, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem6_17b_2nd_pass_map_table_tbl_data, 0x0, sizeof(ihb_fem6_17b_2nd_pass_map_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 85, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem6_17b_2nd_pass_offset_table_tbl_data, 0x0, sizeof(ihb_fem6_17b_2nd_pass_offset_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 86, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem7_17b_2nd_pass_key_profile_resolved_data_tbl_data, 0x0, sizeof(ihb_fem7_17b_2nd_pass_key_profile_resolved_data_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 87, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem7_17b_2nd_pass_program_resolved_data_tbl_data, 0x0, sizeof(ihb_fem7_17b_2nd_pass_program_resolved_data_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 88, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem7_17b_2nd_pass_map_index_table_tbl_data, 0x0, sizeof(ihb_fem7_17b_2nd_pass_map_index_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 89, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem7_17b_2nd_pass_map_table_tbl_data, 0x0, sizeof(ihb_fem7_17b_2nd_pass_map_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 90, exit);

  ret = SOC_SAND_OK; sal_memset(&ihb_fem7_17b_2nd_pass_offset_table_tbl_data, 0x0, sizeof(ihb_fem7_17b_2nd_pass_offset_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 93, exit);

  /*Pinfo Lbp*/
  nof_lines_total = tables->ihb.pinfo_lbp_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 101, exit);

       res = soc_pb_ihb_pinfo_lbp_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_pinfo_lbp_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 102, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Pinfo PMF*/
  nof_lines_total = tables->ihb.pinfo_pmf_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 105, exit);

       res = soc_pb_ihb_pinfo_pmf_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_pinfo_pmf_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 106, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Packet Format Code Profile*/
  nof_lines_total = tables->ihb.packet_format_code_profile_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 107, exit);

       res = soc_pb_ihb_packet_format_code_profile_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_packet_format_code_profile_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 108, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Src Dest Port For L3 ACL Key*/
  nof_lines_total = tables->ihb.src_dest_port_for_l3_acl_key_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 109, exit);

       res = soc_pb_ihb_src_dest_port_for_l3_acl_key_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_src_dest_port_for_l3_acl_key_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Direct 1st Pass Key Profile Resolved Data*/
  nof_lines_total = tables->ihb.direct_1st_pass_key_profile_resolved_data_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);

       res = soc_pb_ihb_direct_1st_pass_key_profile_resolved_data_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_direct_1st_pass_key_profile_resolved_data_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 112, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Direct 2nd Pass Key Profile Resolved Data*/
  nof_lines_total = tables->ihb.direct_2nd_pass_key_profile_resolved_data_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 113, exit);

       res = soc_pb_ihb_direct_2nd_pass_key_profile_resolved_data_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_direct_2nd_pass_key_profile_resolved_data_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 114, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM0 4b 1st Pass Key Profile Resolved Data*/
  nof_lines_total = tables->ihb.fem0_4b_1st_pass_key_profile_resolved_data_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 115, exit);

       res = soc_pb_ihb_fem0_4b_1st_pass_key_profile_resolved_data_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem0_4b_1st_pass_key_profile_resolved_data_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 116, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM0 4b 1st Pass Program Resolved Data*/
  nof_lines_total = tables->ihb.fem0_4b_1st_pass_program_resolved_data_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 117, exit);

       res = soc_pb_ihb_fem0_4b_1st_pass_program_resolved_data_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem0_4b_1st_pass_program_resolved_data_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 118, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM0 4b 1st Pass Map Index Table*/
  nof_lines_total = tables->ihb.fem0_4b_1st_pass_map_index_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 119, exit);

       res = soc_pb_ihb_fem0_4b_1st_pass_map_index_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem0_4b_1st_pass_map_index_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM0 4b 1st Pass Map Table*/
  nof_lines_total = tables->ihb.fem0_4b_1st_pass_map_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 121, exit);

       res = soc_pb_ihb_fem0_4b_1st_pass_map_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem0_4b_1st_pass_map_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 122, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM1 4b 1st Pass Key Profile Resolved Data*/
  nof_lines_total = tables->ihb.fem1_4b_1st_pass_key_profile_resolved_data_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 123, exit);

       res = soc_pb_ihb_fem1_4b_1st_pass_key_profile_resolved_data_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem1_4b_1st_pass_key_profile_resolved_data_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 124, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM1 4b 1st Pass Program Resolved Data*/
  nof_lines_total = tables->ihb.fem1_4b_1st_pass_program_resolved_data_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 125, exit);

       res = soc_pb_ihb_fem1_4b_1st_pass_program_resolved_data_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem1_4b_1st_pass_program_resolved_data_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 126, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM1 4b 1st Pass Map Index Table*/
  nof_lines_total = tables->ihb.fem1_4b_1st_pass_map_index_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 127, exit);

       res = soc_pb_ihb_fem1_4b_1st_pass_map_index_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem1_4b_1st_pass_map_index_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 128, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM1 4b 1st Pass Map Table*/
  nof_lines_total = tables->ihb.fem1_4b_1st_pass_map_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 129, exit);

       res = soc_pb_ihb_fem1_4b_1st_pass_map_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem1_4b_1st_pass_map_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM2 14b 1st Pass Key Profile Resolved Data*/
  nof_lines_total = tables->ihb.fem2_14b_1st_pass_key_profile_resolved_data_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 131, exit);

       res = soc_pb_ihb_fem2_14b_1st_pass_key_profile_resolved_data_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem2_14b_1st_pass_key_profile_resolved_data_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 132, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM2 14b 1st Pass Program Resolved Data*/
  nof_lines_total = tables->ihb.fem2_14b_1st_pass_program_resolved_data_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 133, exit);

       res = soc_pb_ihb_fem2_14b_1st_pass_program_resolved_data_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem2_14b_1st_pass_program_resolved_data_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 134, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM2 14b 1st Pass Map Index Table*/
  nof_lines_total = tables->ihb.fem2_14b_1st_pass_map_index_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 135, exit);

       res = soc_pb_ihb_fem2_14b_1st_pass_map_index_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem2_14b_1st_pass_map_index_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 136, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM2 14b 1st Pass Map Table*/
  nof_lines_total = tables->ihb.fem2_14b_1st_pass_map_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 137, exit);

       res = soc_pb_ihb_fem2_14b_1st_pass_map_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem2_14b_1st_pass_map_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 138, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM2 14b 1st Pass Offset Table*/
  nof_lines_total = tables->ihb.fem2_14b_1st_pass_offset_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 139, exit);

       res = soc_pb_ihb_fem2_14b_1st_pass_offset_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem2_14b_1st_pass_offset_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM3 14b 1st Pass Key Profile Resolved Data*/
  nof_lines_total = tables->ihb.fem3_14b_1st_pass_key_profile_resolved_data_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 141, exit);

       res = soc_pb_ihb_fem3_14b_1st_pass_key_profile_resolved_data_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem3_14b_1st_pass_key_profile_resolved_data_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 142, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM3 14b 1st Pass Program Resolved Data*/
  nof_lines_total = tables->ihb.fem3_14b_1st_pass_program_resolved_data_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 143, exit);

       res = soc_pb_ihb_fem3_14b_1st_pass_program_resolved_data_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem3_14b_1st_pass_program_resolved_data_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 144, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM3 14b 1st Pass Map Index Table*/
  nof_lines_total = tables->ihb.fem3_14b_1st_pass_map_index_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 145, exit);

       res = soc_pb_ihb_fem3_14b_1st_pass_map_index_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem3_14b_1st_pass_map_index_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 146, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM3 14b 1st Pass Map Table*/
  nof_lines_total = tables->ihb.fem3_14b_1st_pass_map_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 147, exit);

       res = soc_pb_ihb_fem3_14b_1st_pass_map_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem3_14b_1st_pass_map_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 148, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM3 14b 1st Pass Offset Table*/
  nof_lines_total = tables->ihb.fem3_14b_1st_pass_offset_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 149, exit);

       res = soc_pb_ihb_fem3_14b_1st_pass_offset_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem3_14b_1st_pass_offset_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM4 14b 1st Pass Key Profile Resolved Data*/
  nof_lines_total = tables->ihb.fem4_14b_1st_pass_key_profile_resolved_data_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 151, exit);

       res = soc_pb_ihb_fem4_14b_1st_pass_key_profile_resolved_data_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem4_14b_1st_pass_key_profile_resolved_data_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 152, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM4 14b 1st Pass Program Resolved Data*/
  nof_lines_total = tables->ihb.fem4_14b_1st_pass_program_resolved_data_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 153, exit);

       res = soc_pb_ihb_fem4_14b_1st_pass_program_resolved_data_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem4_14b_1st_pass_program_resolved_data_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 154, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM4 14b 1st Pass Map Index Table*/
  nof_lines_total = tables->ihb.fem4_14b_1st_pass_map_index_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 155, exit);

       res = soc_pb_ihb_fem4_14b_1st_pass_map_index_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem4_14b_1st_pass_map_index_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 156, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM4 14b 1st Pass Map Table*/
  nof_lines_total = tables->ihb.fem4_14b_1st_pass_map_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 157, exit);

       res = soc_pb_ihb_fem4_14b_1st_pass_map_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem4_14b_1st_pass_map_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 158, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM4 14b 1st Pass Offset Table*/
  nof_lines_total = tables->ihb.fem4_14b_1st_pass_offset_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 159, exit);

       res = soc_pb_ihb_fem4_14b_1st_pass_offset_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem4_14b_1st_pass_offset_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM5 17b 1st Pass Key Profile Resolved Data*/
  nof_lines_total = tables->ihb.fem5_17b_1st_pass_key_profile_resolved_data_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 161, exit);

       res = soc_pb_ihb_fem5_17b_1st_pass_key_profile_resolved_data_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem5_17b_1st_pass_key_profile_resolved_data_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 162, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM5 17b 1st Pass Program Resolved Data*/
  nof_lines_total = tables->ihb.fem5_17b_1st_pass_program_resolved_data_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 163, exit);

       res = soc_pb_ihb_fem5_17b_1st_pass_program_resolved_data_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem5_17b_1st_pass_program_resolved_data_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 164, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM5 17b 1st Pass Map Index Table*/
  nof_lines_total = tables->ihb.fem5_17b_1st_pass_map_index_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 165, exit);

       res = soc_pb_ihb_fem5_17b_1st_pass_map_index_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem5_17b_1st_pass_map_index_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 166, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM5 17b 1st Pass Map Table*/
  nof_lines_total = tables->ihb.fem5_17b_1st_pass_map_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 167, exit);

       res = soc_pb_ihb_fem5_17b_1st_pass_map_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem5_17b_1st_pass_map_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 168, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM5 17b 1st Pass Offset Table*/
  nof_lines_total = tables->ihb.fem5_17b_1st_pass_offset_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 169, exit);

       res = soc_pb_ihb_fem5_17b_1st_pass_offset_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem5_17b_1st_pass_offset_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM6 17b 1st Pass Key Profile Resolved Data*/
  nof_lines_total = tables->ihb.fem6_17b_1st_pass_key_profile_resolved_data_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 171, exit);

       res = soc_pb_ihb_fem6_17b_1st_pass_key_profile_resolved_data_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem6_17b_1st_pass_key_profile_resolved_data_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 172, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM6 17b 1st Pass Program Resolved Data*/
  nof_lines_total = tables->ihb.fem6_17b_1st_pass_program_resolved_data_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 173, exit);

       res = soc_pb_ihb_fem6_17b_1st_pass_program_resolved_data_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem6_17b_1st_pass_program_resolved_data_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 174, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM6 17b 1st Pass Map Index Table*/
  nof_lines_total = tables->ihb.fem6_17b_1st_pass_map_index_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 175, exit);

       res = soc_pb_ihb_fem6_17b_1st_pass_map_index_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem6_17b_1st_pass_map_index_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 176, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM6 17b 1st Pass Map Table*/
  nof_lines_total = tables->ihb.fem6_17b_1st_pass_map_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 177, exit);

       res = soc_pb_ihb_fem6_17b_1st_pass_map_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem6_17b_1st_pass_map_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 178, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM6 17b 1st Pass Offset Table*/
  nof_lines_total = tables->ihb.fem6_17b_1st_pass_offset_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 179, exit);

       res = soc_pb_ihb_fem6_17b_1st_pass_offset_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem6_17b_1st_pass_offset_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM7 17b 1st Pass Key Profile Resolved Data*/
  nof_lines_total = tables->ihb.fem7_17b_1st_pass_key_profile_resolved_data_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 181, exit);

       res = soc_pb_ihb_fem7_17b_1st_pass_key_profile_resolved_data_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem7_17b_1st_pass_key_profile_resolved_data_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 182, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM7 17b 1st Pass Program Resolved Data*/
  nof_lines_total = tables->ihb.fem7_17b_1st_pass_program_resolved_data_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 183, exit);

       res = soc_pb_ihb_fem7_17b_1st_pass_program_resolved_data_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem7_17b_1st_pass_program_resolved_data_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 184, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM7 17b 1st Pass Map Index Table*/
  nof_lines_total = tables->ihb.fem7_17b_1st_pass_map_index_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 185, exit);

       res = soc_pb_ihb_fem7_17b_1st_pass_map_index_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem7_17b_1st_pass_map_index_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 186, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM7 17b 1st Pass Map Table*/
  nof_lines_total = tables->ihb.fem7_17b_1st_pass_map_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 187, exit);

       res = soc_pb_ihb_fem7_17b_1st_pass_map_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem7_17b_1st_pass_map_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 188, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM7 17b 1st Pass Offset Table*/
  nof_lines_total = tables->ihb.fem7_17b_1st_pass_offset_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 189, exit);

       res = soc_pb_ihb_fem7_17b_1st_pass_offset_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem7_17b_1st_pass_offset_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 190, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM0 4b 2nd Pass Key Profile Resolved Data*/
  nof_lines_total = tables->ihb.fem0_4b_2nd_pass_key_profile_resolved_data_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 191, exit);

       res = soc_pb_ihb_fem0_4b_2nd_pass_key_profile_resolved_data_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem0_4b_2nd_pass_key_profile_resolved_data_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 192, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM0 4b 2nd Pass Program Resolved Data*/
  nof_lines_total = tables->ihb.fem0_4b_2nd_pass_program_resolved_data_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 193, exit);

       res = soc_pb_ihb_fem0_4b_2nd_pass_program_resolved_data_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem0_4b_2nd_pass_program_resolved_data_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 194, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM0 4b 2nd Pass Map Index Table*/
  nof_lines_total = tables->ihb.fem0_4b_2nd_pass_map_index_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 195, exit);

       res = soc_pb_ihb_fem0_4b_2nd_pass_map_index_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem0_4b_2nd_pass_map_index_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 196, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM0 4b 2nd Pass Map Table*/
  nof_lines_total = tables->ihb.fem0_4b_2nd_pass_map_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 197, exit);

       res = soc_pb_ihb_fem0_4b_2nd_pass_map_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem0_4b_2nd_pass_map_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 198, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM1 4b 2nd Pass Key Profile Resolved Data*/
  nof_lines_total = tables->ihb.fem1_4b_2nd_pass_key_profile_resolved_data_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 199, exit);

       res = soc_pb_ihb_fem1_4b_2nd_pass_key_profile_resolved_data_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem1_4b_2nd_pass_key_profile_resolved_data_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM1 4b 2nd Pass Program Resolved Data*/
  nof_lines_total = tables->ihb.fem1_4b_2nd_pass_program_resolved_data_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 201, exit);

       res = soc_pb_ihb_fem1_4b_2nd_pass_program_resolved_data_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem1_4b_2nd_pass_program_resolved_data_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 202, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM1 4b 2nd Pass Map Index Table*/
  nof_lines_total = tables->ihb.fem1_4b_2nd_pass_map_index_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 203, exit);

       res = soc_pb_ihb_fem1_4b_2nd_pass_map_index_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem1_4b_2nd_pass_map_index_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 204, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM1 4b 2nd Pass Map Table*/
  nof_lines_total = tables->ihb.fem1_4b_2nd_pass_map_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 205, exit);

       res = soc_pb_ihb_fem1_4b_2nd_pass_map_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem1_4b_2nd_pass_map_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 206, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM2 14b 2nd Pass Key Profile Resolved Data*/
  nof_lines_total = tables->ihb.fem2_14b_2nd_pass_key_profile_resolved_data_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 207, exit);

       res = soc_pb_ihb_fem2_14b_2nd_pass_key_profile_resolved_data_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem2_14b_2nd_pass_key_profile_resolved_data_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 208, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM2 14b 2nd Pass Program Resolved Data*/
  nof_lines_total = tables->ihb.fem2_14b_2nd_pass_program_resolved_data_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 209, exit);

       res = soc_pb_ihb_fem2_14b_2nd_pass_program_resolved_data_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem2_14b_2nd_pass_program_resolved_data_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 210, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM2 14b 2nd Pass Map Index Table*/
  nof_lines_total = tables->ihb.fem2_14b_2nd_pass_map_index_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 211, exit);

       res = soc_pb_ihb_fem2_14b_2nd_pass_map_index_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem2_14b_2nd_pass_map_index_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 212, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM2 14b 2nd Pass Map Table*/
  nof_lines_total = tables->ihb.fem2_14b_2nd_pass_map_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 213, exit);

       res = soc_pb_ihb_fem2_14b_2nd_pass_map_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem2_14b_2nd_pass_map_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 214, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM2 14b 2nd Pass Offset Table*/
  nof_lines_total = tables->ihb.fem2_14b_2nd_pass_offset_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 215, exit);

       res = soc_pb_ihb_fem2_14b_2nd_pass_offset_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem2_14b_2nd_pass_offset_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 216, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM3 14b 2nd Pass Key Profile Resolved Data*/
  nof_lines_total = tables->ihb.fem3_14b_2nd_pass_key_profile_resolved_data_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 217, exit);

       res = soc_pb_ihb_fem3_14b_2nd_pass_key_profile_resolved_data_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem3_14b_2nd_pass_key_profile_resolved_data_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 218, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM3 14b 2nd Pass Program Resolved Data*/
  nof_lines_total = tables->ihb.fem3_14b_2nd_pass_program_resolved_data_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 219, exit);

       res = soc_pb_ihb_fem3_14b_2nd_pass_program_resolved_data_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem3_14b_2nd_pass_program_resolved_data_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 220, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM3 14b 2nd Pass Map Index Table*/
  nof_lines_total = tables->ihb.fem3_14b_2nd_pass_map_index_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 221, exit);

       res = soc_pb_ihb_fem3_14b_2nd_pass_map_index_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem3_14b_2nd_pass_map_index_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 222, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM3 14b 2nd Pass Map Table*/
  nof_lines_total = tables->ihb.fem3_14b_2nd_pass_map_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 223, exit);

       res = soc_pb_ihb_fem3_14b_2nd_pass_map_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem3_14b_2nd_pass_map_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 224, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM3 14b 2nd Pass Offset Table*/
  nof_lines_total = tables->ihb.fem3_14b_2nd_pass_offset_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 225, exit);

       res = soc_pb_ihb_fem3_14b_2nd_pass_offset_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem3_14b_2nd_pass_offset_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 226, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM4 14b 2nd Pass Key Profile Resolved Data*/
  nof_lines_total = tables->ihb.fem4_14b_2nd_pass_key_profile_resolved_data_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 227, exit);

       res = soc_pb_ihb_fem4_14b_2nd_pass_key_profile_resolved_data_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem4_14b_2nd_pass_key_profile_resolved_data_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 228, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM4 14b 2nd Pass Program Resolved Data*/
  nof_lines_total = tables->ihb.fem4_14b_2nd_pass_program_resolved_data_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 229, exit);

       res = soc_pb_ihb_fem4_14b_2nd_pass_program_resolved_data_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem4_14b_2nd_pass_program_resolved_data_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 230, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM4 14b 2nd Pass Map Index Table*/
  nof_lines_total = tables->ihb.fem4_14b_2nd_pass_map_index_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 231, exit);

       res = soc_pb_ihb_fem4_14b_2nd_pass_map_index_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem4_14b_2nd_pass_map_index_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 232, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM4 14b 2nd Pass Map Table*/
  nof_lines_total = tables->ihb.fem4_14b_2nd_pass_map_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 233, exit);

       res = soc_pb_ihb_fem4_14b_2nd_pass_map_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem4_14b_2nd_pass_map_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 234, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM4 14b 2nd Pass Offset Table*/
  nof_lines_total = tables->ihb.fem4_14b_2nd_pass_offset_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 235, exit);

       res = soc_pb_ihb_fem4_14b_2nd_pass_offset_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem4_14b_2nd_pass_offset_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 236, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM5 17b 2nd Pass Key Profile Resolved Data*/
  nof_lines_total = tables->ihb.fem5_17b_2nd_pass_key_profile_resolved_data_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 237, exit);

       res = soc_pb_ihb_fem5_17b_2nd_pass_key_profile_resolved_data_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem5_17b_2nd_pass_key_profile_resolved_data_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 238, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM5 17b 2nd Pass Program Resolved Data*/
  nof_lines_total = tables->ihb.fem5_17b_2nd_pass_program_resolved_data_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 239, exit);

       res = soc_pb_ihb_fem5_17b_2nd_pass_program_resolved_data_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem5_17b_2nd_pass_program_resolved_data_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 240, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM5 17b 2nd Pass Map Index Table*/
  nof_lines_total = tables->ihb.fem5_17b_2nd_pass_map_index_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 241, exit);

       res = soc_pb_ihb_fem5_17b_2nd_pass_map_index_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem5_17b_2nd_pass_map_index_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 242, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM5 17b 2nd Pass Map Table*/
  nof_lines_total = tables->ihb.fem5_17b_2nd_pass_map_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 243, exit);

       res = soc_pb_ihb_fem5_17b_2nd_pass_map_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem5_17b_2nd_pass_map_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 244, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM5 17b 2nd Pass Offset Table*/
  nof_lines_total = tables->ihb.fem5_17b_2nd_pass_offset_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 245, exit);

       res = soc_pb_ihb_fem5_17b_2nd_pass_offset_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem5_17b_2nd_pass_offset_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 246, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM6 17b 2nd Pass Key Profile Resolved Data*/
  nof_lines_total = tables->ihb.fem6_17b_2nd_pass_key_profile_resolved_data_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 247, exit);

       res = soc_pb_ihb_fem6_17b_2nd_pass_key_profile_resolved_data_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem6_17b_2nd_pass_key_profile_resolved_data_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 248, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM6 17b 2nd Pass Program Resolved Data*/
  nof_lines_total = tables->ihb.fem6_17b_2nd_pass_program_resolved_data_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 249, exit);

       res = soc_pb_ihb_fem6_17b_2nd_pass_program_resolved_data_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem6_17b_2nd_pass_program_resolved_data_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 250, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM6 17b 2nd Pass Map Index Table*/
  nof_lines_total = tables->ihb.fem6_17b_2nd_pass_map_index_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 251, exit);

       res = soc_pb_ihb_fem6_17b_2nd_pass_map_index_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem6_17b_2nd_pass_map_index_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 252, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM6 17b 2nd Pass Map Table*/
  nof_lines_total = tables->ihb.fem6_17b_2nd_pass_map_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 253, exit);

       res = soc_pb_ihb_fem6_17b_2nd_pass_map_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem6_17b_2nd_pass_map_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 254, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM6 17b 2nd Pass Offset Table*/
  nof_lines_total = tables->ihb.fem6_17b_2nd_pass_offset_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 255, exit);

       res = soc_pb_ihb_fem6_17b_2nd_pass_offset_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem6_17b_2nd_pass_offset_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 256, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM7 17b 2nd Pass Key Profile Resolved Data*/
  nof_lines_total = tables->ihb.fem7_17b_2nd_pass_key_profile_resolved_data_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 257, exit);

       res = soc_pb_ihb_fem7_17b_2nd_pass_key_profile_resolved_data_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem7_17b_2nd_pass_key_profile_resolved_data_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 258, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM7 17b 2nd Pass Program Resolved Data*/
  nof_lines_total = tables->ihb.fem7_17b_2nd_pass_program_resolved_data_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 259, exit);

       res = soc_pb_ihb_fem7_17b_2nd_pass_program_resolved_data_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem7_17b_2nd_pass_program_resolved_data_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 260, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM7 17b 2nd Pass Map Index Table*/
  nof_lines_total = tables->ihb.fem7_17b_2nd_pass_map_index_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 261, exit);

       res = soc_pb_ihb_fem7_17b_2nd_pass_map_index_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem7_17b_2nd_pass_map_index_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 262, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM7 17b 2nd Pass Map Table*/
  nof_lines_total = tables->ihb.fem7_17b_2nd_pass_map_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 263, exit);

       res = soc_pb_ihb_fem7_17b_2nd_pass_map_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem7_17b_2nd_pass_map_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 264, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*FEM7 17b 2nd Pass Offset Table*/
  nof_lines_total = tables->ihb.fem7_17b_2nd_pass_offset_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ihb.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_pb_ihb_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 265, exit);

       res = soc_pb_ihb_fem7_17b_2nd_pass_offset_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihb_fem7_17b_2nd_pass_offset_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 266, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mgmt_ihb_tbls_init()",0,0);
}

STATIC uint32
  soc_pb_mgmt_iqm_tbls_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    tbl_ndx;
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    nof_lines,
    nof_lines_total,
    lines_max,
    first_entry = 0;
  SOC_PB_TBLS
    *tables;
  SOC_PB_REGS
    *regs;
  SOC_PETRA_IQM_STATIC_TBL_DATA
    iqm_static_tbl_data;
  SOC_PETRA_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_DATA
    iqm_packet_queue_red_weight_table_tbl_data;
  SOC_PETRA_IQM_CREDIT_DISCOUNT_TABLE_TBL_DATA
    iqm_credit_discount_table_tbl_data;
  SOC_PETRA_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_DATA
    iqm_packet_queue_red_parameters_table_tbl_data;
  SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_DATA
    iqm_vsq_descriptor_rate_class_group_a_tbl_data;
  SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_DATA
    iqm_vsq_descriptor_rate_class_group_b_tbl_data;
  SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_DATA
    iqm_vsq_descriptor_rate_class_group_c_tbl_data;
  SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_DATA
    iqm_vsq_descriptor_rate_class_group_d_tbl_data;
  SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL_DATA
    iqm_vsq_flow_control_parameters_table_group_tbl_data[SOC_PETRA_NOF_VSQ_GROUPS];
  SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA
    iqm_vsq_queue_parameters_table_group_tbl_data[SOC_PETRA_NOF_VSQ_GROUPS];
  SOC_PETRA_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_DATA
    iqm_system_red_parameters_table_tbl_data;
  SOC_PETRA_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_DATA
    iqm_system_red_drop_probability_values_tbl_data;
  SOC_PETRA_IQM_SYSTEM_RED_TBL_DATA
    iqm_system_red_tbl_data;
  SOC_PB_IQM_CNM_DESCRIPTOR_STATIC_TBL_DATA
    iqm_cnm_descriptor_static_tbl_data;
  SOC_PB_IQM_CNM_PARAMETERS_TABLE_TBL_DATA
    iqm_cnm_parameters_table_tbl_data;
  SOC_PB_IQM_METER_PROCESSOR_RESULT_RESOLVE_TABLE_STATIC_TBL_DATA
    iqm_meter_processor_result_resolve_table_static_tbl_data;
  SOC_PB_IQM_PRFSEL_TBL_DATA
    soc_pb_iqm_prfsel_tbl_data;
  SOC_PB_IQM_PRFCFG_TBL_DATA
    soc_pb_iqm_prfcfg_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_IQM_TBLS_INIT);

  res = soc_pb_tbls_get(
          &tables
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  regs = soc_pb_regs();

  ret = SOC_SAND_OK; sal_memset(&iqm_static_tbl_data, 0x0, sizeof(iqm_static_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 13, exit);

  ret = SOC_SAND_OK; sal_memset(&iqm_packet_queue_red_weight_table_tbl_data, 0x0, sizeof(iqm_packet_queue_red_weight_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 15, exit);

  ret = SOC_SAND_OK; sal_memset(&iqm_credit_discount_table_tbl_data, 0x0, sizeof(iqm_credit_discount_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 16, exit);

  ret = SOC_SAND_OK; sal_memset(&iqm_packet_queue_red_parameters_table_tbl_data, 0x0, sizeof(iqm_packet_queue_red_parameters_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 19, exit);

  ret = SOC_SAND_OK; sal_memset(&iqm_vsq_descriptor_rate_class_group_a_tbl_data, 0x0, sizeof(iqm_vsq_descriptor_rate_class_group_a_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 20, exit);

  ret = SOC_SAND_OK; sal_memset(&iqm_vsq_descriptor_rate_class_group_b_tbl_data, 0x0, sizeof(iqm_vsq_descriptor_rate_class_group_b_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 21, exit);

  ret = SOC_SAND_OK; sal_memset(&iqm_vsq_descriptor_rate_class_group_c_tbl_data, 0x0, sizeof(iqm_vsq_descriptor_rate_class_group_c_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 22, exit);

  ret = SOC_SAND_OK; sal_memset(&iqm_vsq_descriptor_rate_class_group_d_tbl_data, 0x0, sizeof(iqm_vsq_descriptor_rate_class_group_d_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 23, exit);

  for (tbl_ndx = 0; tbl_ndx < SOC_PETRA_NOF_VSQ_GROUPS; ++tbl_ndx)
  {
    ret = SOC_SAND_OK; sal_memset(&(iqm_vsq_flow_control_parameters_table_group_tbl_data[tbl_ndx]), 0x0, sizeof(iqm_vsq_flow_control_parameters_table_group_tbl_data[tbl_ndx]));
    iqm_vsq_flow_control_parameters_table_group_tbl_data[tbl_ndx].avrg_size_en = TRUE;
    SOC_SAND_CHECK_FUNC_RESULT(ret, 32, exit);

    ret = SOC_SAND_OK; sal_memset(&(iqm_vsq_queue_parameters_table_group_tbl_data[tbl_ndx]), 0x0, sizeof(iqm_vsq_queue_parameters_table_group_tbl_data[tbl_ndx]));
    SOC_SAND_CHECK_FUNC_RESULT(ret, 36, exit);
  }

  ret = SOC_SAND_OK; sal_memset(&iqm_system_red_parameters_table_tbl_data, 0x0, sizeof(iqm_system_red_parameters_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 40, exit);

  ret = SOC_SAND_OK; sal_memset(&iqm_system_red_drop_probability_values_tbl_data, 0x0, sizeof(iqm_system_red_drop_probability_values_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 41, exit);

  ret = SOC_SAND_OK; sal_memset(&iqm_system_red_tbl_data, 0x0, sizeof(iqm_system_red_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 42, exit);

  ret = SOC_SAND_OK; sal_memset(&iqm_cnm_descriptor_static_tbl_data, 0x0, sizeof(iqm_cnm_descriptor_static_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 43, exit);

  ret = SOC_SAND_OK; sal_memset(&iqm_cnm_parameters_table_tbl_data, 0x0, sizeof(iqm_cnm_parameters_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 45, exit);

  ret = SOC_SAND_OK; sal_memset(&iqm_meter_processor_result_resolve_table_static_tbl_data, 0x0, sizeof(iqm_meter_processor_result_resolve_table_static_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 46, exit);

  ret = SOC_SAND_OK; sal_memset(&soc_pb_iqm_prfsel_tbl_data, 0x0, sizeof(soc_pb_iqm_prfsel_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 47, exit);

  ret = SOC_SAND_OK; sal_memset(&soc_pb_iqm_prfcfg_tbl_data, 0x0, sizeof(soc_pb_iqm_prfcfg_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 48, exit);


  /*Packet Queue Descriptor (Static)*/
  nof_lines_total = tables->iqm.static_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->iqm.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_iqm_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 105, exit);

       res = soc_petra_iqm_static_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &iqm_static_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 106, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;


  /*Packet Queue Red Weight table*/
  nof_lines_total = tables->iqm.packet_queue_red_weight_table_tbl.addr.size;
  iqm_packet_queue_red_weight_table_tbl_data.avrg_en    = TRUE;
  iqm_packet_queue_red_weight_table_tbl_data.pq_weight  = 2;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->iqm.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_iqm_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 109, exit);

       res = soc_petra_iqm_packet_queue_red_weight_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &iqm_packet_queue_red_weight_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Credit Discount table*/
  nof_lines_total = tables->iqm.credit_discount_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->iqm.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_iqm_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);

       res = soc_petra_iqm_credit_discount_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &iqm_credit_discount_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 112, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;


  /*Packet Queue Red parameters table*/
  nof_lines_total = tables->iqm.packet_queue_red_parameters_table_tbl.addr.size;
  iqm_packet_queue_red_parameters_table_tbl_data.pq_wred_pckt_sz_ignr = TRUE;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->iqm.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_iqm_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 117, exit);

       res = soc_petra_iqm_packet_queue_red_parameters_table_tbl_set_unsafe(
                 unit,
                 0x0,
                 0x0,
                 &iqm_packet_queue_red_parameters_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 118, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*VSQ Descriptor Rate Class - group A*/
  nof_lines_total = tables->iqm.vsq_descriptor_rate_class_group_a_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->iqm.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_iqm_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 119, exit);

       res = soc_petra_iqm_vsq_descriptor_rate_class_group_a_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &iqm_vsq_descriptor_rate_class_group_a_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*VSQ Descriptor Rate Class - group B*/
  nof_lines_total = tables->iqm.vsq_descriptor_rate_class_group_b_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->iqm.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_iqm_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 121, exit);

       res = soc_petra_iqm_vsq_descriptor_rate_class_group_b_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &iqm_vsq_descriptor_rate_class_group_b_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 122, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*VSQ Descriptor Rate Class - group C*/
  nof_lines_total = tables->iqm.vsq_descriptor_rate_class_group_c_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->iqm.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_iqm_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 123, exit);

       res = soc_petra_iqm_vsq_descriptor_rate_class_group_c_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &iqm_vsq_descriptor_rate_class_group_c_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 124, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*VSQ Descriptor Rate Class - group D*/
  nof_lines_total = tables->iqm.vsq_descriptor_rate_class_group_d_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->iqm.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_iqm_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 125, exit);

       res = soc_petra_iqm_vsq_descriptor_rate_class_group_d_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &iqm_vsq_descriptor_rate_class_group_d_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 126, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  for (tbl_ndx = 0; tbl_ndx < SOC_PETRA_NOF_VSQ_GROUPS; ++tbl_ndx)
  {
    nof_lines_total = tables->iqm.vsq_flow_control_parameters_table_group_tbl[tbl_ndx].addr.size;
    lines_max = SOC_PB_INIT_COUNT_MAX(regs->iqm.indirect_command_reg.indirect_command_count);
    do {
         nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
         nof_lines_total -= nof_lines;

         res = soc_petra_iqm_set_reps_for_tbl_unsafe(unit, nof_lines);
         SOC_SAND_CHECK_FUNC_RESULT(res, 143, exit);

         res = soc_petra_iqm_vsq_flow_control_parameters_table_group_tbl_set_unsafe(
                   unit,
                   tbl_ndx,
                   first_entry,
                   &(iqm_vsq_flow_control_parameters_table_group_tbl_data[tbl_ndx])
               );
         SOC_SAND_CHECK_FUNC_RESULT(res, 144, exit);
         first_entry += nof_lines;
    } while(nof_lines_total > 0);
    first_entry = 0;


  /*VSQ Queue Parameters table - group A-D*/
    nof_lines_total = tables->iqm.vsq_queue_parameters_table_group_tbl[tbl_ndx].addr.size;
    lines_max = SOC_PB_INIT_COUNT_MAX(regs->iqm.indirect_command_reg.indirect_command_count);
    do {
         nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
         nof_lines_total -= nof_lines;

         res = soc_petra_iqm_set_reps_for_tbl_unsafe(unit, nof_lines);
         SOC_SAND_CHECK_FUNC_RESULT(res, 151, exit);

         res = soc_petra_iqm_vsq_queue_parameters_table_group_tbl_set_unsafe(
                   unit,
                   tbl_ndx,
                   0x0,
                   0x0,
                   &(iqm_vsq_queue_parameters_table_group_tbl_data[tbl_ndx])
               );
         SOC_SAND_CHECK_FUNC_RESULT(res, 152, exit);
         first_entry += nof_lines;
    } while(nof_lines_total > 0);
    first_entry = 0;
  }

  /*System Red parameters table*/
  nof_lines_total = tables->iqm.system_red_parameters_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->iqm.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_iqm_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 159, exit);

       res = soc_petra_iqm_system_red_parameters_table_tbl_set_unsafe(
                 unit,
                 0x0,
                 0x0,
                 &iqm_system_red_parameters_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*System Red drop probability values*/
  nof_lines_total = tables->iqm.system_red_drop_probability_values_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->iqm.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_iqm_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 161, exit);

       res = soc_petra_iqm_system_red_drop_probability_values_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &iqm_system_red_drop_probability_values_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 162, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Source Qsize range thresholds (System Red)*/
  nof_lines_total = tables->iqm.system_red_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->iqm.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_iqm_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 163, exit);

       res = soc_petra_iqm_system_red_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &iqm_system_red_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 164, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Congestion Point (CNM) descriptor - Static*/
  nof_lines_total = tables->iqm.cnm_descriptor_static_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->iqm.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_iqm_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 165, exit);

       res = soc_pb_iqm_cnm_descriptor_static_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &iqm_cnm_descriptor_static_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 166, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Congestion Point (CNM) parameters table*/
  nof_lines_total = tables->iqm.cnm_parameters_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->iqm.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_iqm_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 169, exit);

       res = soc_pb_iqm_cnm_parameters_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &iqm_cnm_parameters_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Meter-processor result resolve table - Static*/
  nof_lines_total = tables->iqm.meter_processor_result_resolve_table_static_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->iqm.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_iqm_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 171, exit);

       res = soc_pb_iqm_meter_processor_result_resolve_table_static_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &iqm_meter_processor_result_resolve_table_static_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 172, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /* Meter-processor - select tbl 0*/
  nof_lines_total = tables->iqm.prfsel_tbl[0].addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->iqm.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_iqm_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 173, exit);

       res = soc_pb_iqm_prfsel_tbl_set_unsafe(
                 unit,
                 0,
                 first_entry,
                 &soc_pb_iqm_prfsel_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 174, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /* Meter-processor - select tbl 1*/
  nof_lines_total = tables->iqm.prfsel_tbl[1].addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->iqm.indirect_command_reg.indirect_command_count);
  do {
    nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
    nof_lines_total -= nof_lines;

    res = soc_petra_iqm_set_reps_for_tbl_unsafe(unit, nof_lines);
    SOC_SAND_CHECK_FUNC_RESULT(res, 175, exit);

    res = soc_pb_iqm_prfsel_tbl_set_unsafe(
      unit,
      1,
      first_entry,
      &soc_pb_iqm_prfsel_tbl_data
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 176, exit);
    first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /* Meter-processor - config tbl 0*/
  nof_lines_total = tables->iqm.prfcfg_tbl[0].addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->iqm.indirect_command_reg.indirect_command_count);
  do {
    nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
    nof_lines_total -= nof_lines;

    res = soc_petra_iqm_set_reps_for_tbl_unsafe(unit, nof_lines);
    SOC_SAND_CHECK_FUNC_RESULT(res, 177, exit);

    res = soc_pb_iqm_prfcfg_tbl_set_unsafe(
      unit,
      0,
      first_entry,
      &soc_pb_iqm_prfcfg_tbl_data
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 178, exit);
    first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /* Meter-processor - config tbl 1*/
  nof_lines_total = tables->iqm.prfcfg_tbl[1].addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->iqm.indirect_command_reg.indirect_command_count);
  do {
    nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
    nof_lines_total -= nof_lines;

    res = soc_petra_iqm_set_reps_for_tbl_unsafe(unit, nof_lines);
    SOC_SAND_CHECK_FUNC_RESULT(res, 179, exit);

    res = soc_pb_iqm_prfcfg_tbl_set_unsafe(
      unit,
      1,
      first_entry,
      &soc_pb_iqm_prfcfg_tbl_data
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);
    first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mgmt_iqm_tbls_init()",0,0);
}

STATIC uint32
  soc_pb_mgmt_ips_tbls_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    nof_lines,
    nof_lines_total,
    lines_max,
    first_entry = 0;
  SOC_PB_TBLS
    *tables;
  SOC_PB_REGS
    *regs;
  SOC_PETRA_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_DATA
    ips_system_physical_port_lookup_table_tbl_data;
  SOC_PETRA_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_DATA
    ips_destination_device_and_port_lookup_table_tbl_data;
  SOC_PETRA_IPS_FLOW_ID_LOOKUP_TABLE_TBL_DATA
    ips_flow_id_lookup_table_tbl_data;
  SOC_PETRA_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_DATA
    ips_queue_type_lookup_table_tbl_data;
  SOC_PETRA_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_DATA
    ips_queue_priority_map_select_tbl_data;
  SOC_PETRA_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_DATA
    ips_queue_priority_maps_table_tbl_data;
  SOC_PETRA_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_DATA
    ips_queue_size_based_thresholds_table_tbl_data;
  SOC_PETRA_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_DATA
    ips_credit_balance_based_thresholds_table_tbl_data;
  SOC_PETRA_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_DATA
    ips_empty_queue_credit_balance_table_tbl_data;
  SOC_PETRA_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_DATA
    ips_credit_watchdog_thresholds_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_IPS_TBLS_INIT);

  res = soc_pb_tbls_get(
          &tables
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  regs = soc_pb_regs();

  ret = SOC_SAND_OK; sal_memset(&ips_system_physical_port_lookup_table_tbl_data, 0x0, sizeof(ips_system_physical_port_lookup_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 11, exit);

  ret = SOC_SAND_OK; sal_memset(&ips_destination_device_and_port_lookup_table_tbl_data, 0x0, sizeof(ips_destination_device_and_port_lookup_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 12, exit);

  ret = SOC_SAND_OK; sal_memset(&ips_flow_id_lookup_table_tbl_data, 0x0, sizeof(ips_flow_id_lookup_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 13, exit);

  ret = SOC_SAND_OK; sal_memset(&ips_queue_type_lookup_table_tbl_data, 0x0, sizeof(ips_queue_type_lookup_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 14, exit);

  ret = SOC_SAND_OK; sal_memset(&ips_queue_priority_map_select_tbl_data, 0x0, sizeof(ips_queue_priority_map_select_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 15, exit);

  ret = SOC_SAND_OK; sal_memset(&ips_queue_priority_maps_table_tbl_data, 0x0, sizeof(ips_queue_priority_maps_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 16, exit);

  ret = SOC_SAND_OK; sal_memset(&ips_queue_size_based_thresholds_table_tbl_data, 0x0, sizeof(ips_queue_size_based_thresholds_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 17, exit);

  ret = SOC_SAND_OK; sal_memset(&ips_credit_balance_based_thresholds_table_tbl_data, 0x0, sizeof(ips_credit_balance_based_thresholds_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 18, exit);

  ret = SOC_SAND_OK; sal_memset(&ips_empty_queue_credit_balance_table_tbl_data, 0x0, sizeof(ips_empty_queue_credit_balance_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 19, exit);

  ret = SOC_SAND_OK; sal_memset(&ips_credit_watchdog_thresholds_table_tbl_data, 0x0, sizeof(ips_credit_watchdog_thresholds_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 20, exit);

  ips_system_physical_port_lookup_table_tbl_data.sys_phy_port = SOC_PETRA_MAX_SYSTEM_PHYSICAL_PORT_ID;
  nof_lines_total = tables->ips.system_physical_port_lookup_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ips.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_ips_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 101, exit);

       res = soc_petra_ips_system_physical_port_lookup_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ips_system_physical_port_lookup_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 102, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Destination Device And Port Lookup Table*/
  ips_destination_device_and_port_lookup_table_tbl_data.dest_dev = SOC_PETRA_MAX_FAP_ID;
  ips_destination_device_and_port_lookup_table_tbl_data.dest_port = SOC_PETRA_NOF_FAP_PORTS;
  nof_lines_total = tables->ips.destination_device_and_port_lookup_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ips.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_ips_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 103, exit);

       res = soc_petra_ips_destination_device_and_port_lookup_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ips_destination_device_and_port_lookup_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 104, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Flow Id Lookup Table*/
  ips_flow_id_lookup_table_tbl_data.base_flow = SOC_PETRA_IPQ_INVALID_FLOW_QUARTET;
  ips_flow_id_lookup_table_tbl_data.sub_flow_mode = 0x0;
  nof_lines_total = tables->ips.flow_id_lookup_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ips.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_ips_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 105, exit);

       res = soc_petra_ips_flow_id_lookup_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ips_flow_id_lookup_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 106, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Queue Type Lookup Table*/
  nof_lines_total = tables->ips.queue_type_lookup_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ips.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_ips_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 107, exit);

       res = soc_petra_ips_queue_type_lookup_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ips_queue_type_lookup_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 108, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Queue Priority Map Select*/
  nof_lines_total = tables->ips.queue_priority_map_select_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ips.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_ips_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 109, exit);

       res = soc_petra_ips_queue_priority_map_select_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ips_queue_priority_map_select_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Queue Priority Maps Table*/
  nof_lines_total = tables->ips.queue_priority_maps_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ips.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_ips_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);

       res = soc_petra_ips_queue_priority_maps_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ips_queue_priority_maps_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 112, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Queue Size-Based Thresholds Table*/
  ips_queue_size_based_thresholds_table_tbl_data.fsm_th_mul = 0x9;
  ips_queue_size_based_thresholds_table_tbl_data.norm_to_slow_msg_th = 0x1b;
  ips_queue_size_based_thresholds_table_tbl_data.off_to_norm_msg_th = 0x1c;
  ips_queue_size_based_thresholds_table_tbl_data.off_to_slow_msg_th = 0x0;
  ips_queue_size_based_thresholds_table_tbl_data.slow_to_norm_msg_th = 0x1c;

  nof_lines_total = tables->ips.queue_size_based_thresholds_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ips.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_ips_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 113, exit);

       res = soc_petra_ips_queue_size_based_thresholds_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ips_queue_size_based_thresholds_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 114, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Credit Balance Based Thresholds Table*/

  ips_credit_balance_based_thresholds_table_tbl_data.backlog_enter_qcr_bal_th = 0x1a;
  ips_credit_balance_based_thresholds_table_tbl_data.backlog_exit_qcr_bal_th = 0x1a;
  ips_credit_balance_based_thresholds_table_tbl_data.backoff_enter_qcr_bal_th = 0x1e;
  ips_credit_balance_based_thresholds_table_tbl_data.backoff_exit_qcr_bal_th = 0x1e;

  nof_lines_total = tables->ips.credit_balance_based_thresholds_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ips.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_ips_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 115, exit);

       res = soc_petra_ips_credit_balance_based_thresholds_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ips_credit_balance_based_thresholds_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 116, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Empty Queue Credit Balance Table*/
  nof_lines_total = tables->ips.empty_queue_credit_balance_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ips.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_ips_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 117, exit);

       res = soc_petra_ips_empty_queue_credit_balance_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ips_empty_queue_credit_balance_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 118, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Credit Watchdog Thresholds Table*/
  nof_lines_total = tables->ips.credit_watchdog_thresholds_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->ips.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_ips_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 119, exit);

       res = soc_petra_ips_credit_watchdog_thresholds_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ips_credit_watchdog_thresholds_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mgmt_ips_tbls_init()",0,0);
}

uint32
  soc_pb_mgmt_egq_tbls_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    tbl_idx;
  uint32
    nof_lines,
    nof_lines_total,
    lines_max,
    first_entry = 0;
  SOC_PB_EGQ_TC_DP_MAP_TBL_ENTRY
    tc_dp_entry;
  SOC_PB_TBLS
    *tables;
  SOC_PB_REGS
    *regs;
  SOC_PETRA_EGQ_SCM_TBL_DATA
    egq_nif_scm_tbl_data;
  SOC_PETRA_EGQ_SCM_TBL_DATA
    egq_rcy_scm_tbl_data;
  SOC_PETRA_EGQ_SCM_TBL_DATA
    egq_cpu_scm_tbl_data;
  SOC_PETRA_EGQ_CCM_TBL_DATA
    egq_ccm_tbl_data;
  SOC_PETRA_EGQ_PMC_TBL_DATA
    egq_pmc_tbl_data;
  SOC_PETRA_EGQ_CBM_TBL_DATA
    egq_cbm_tbl_data;
  SOC_PB_EGQ_DWM_NEW_TBL_DATA
    egq_dwm_new_tbl_data;
  SOC_PETRA_EGQ_DWM_TBL_DATA
    egq_dwm_tbl_data;
  SOC_PB_EGQ_PCT_TBL_DATA
    egq_pct_tbl_data;
  SOC_PETRA_EGQ_VLAN_TABLE_TBL_DATA
    egq_vlan_table_tbl_data;
  SOC_PB_EGQ_TC_DP_MAP_TBL_DATA
    egq_tc_dp_map_tbl_data;
  SOC_PB_EGQ_FQP_NIF_PORT_MUX_TBL_DATA
    egq_fqp_nif_port_mux_tbl_data;
  SOC_PB_EGQ_KEY_PROFILE_MAP_INDEX_TBL_DATA
    egq_key_profile_map_index_tbl_data;
  SOC_PB_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL_DATA
    egq_tcam_key_resolution_profile_tbl_data;
  SOC_PB_PP_EGQ_PPCT_TBL_DATA
      egq_ppct_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_EGQ_TBLS_INIT);

  res = soc_pb_tbls_get(
          &tables
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  regs = soc_pb_regs();

  ret = SOC_SAND_OK; sal_memset(&egq_rcy_scm_tbl_data, 0x0, sizeof(egq_rcy_scm_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 27, exit);

  ret = SOC_SAND_OK; sal_memset(&egq_cpu_scm_tbl_data, 0x0, sizeof(egq_cpu_scm_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 28, exit);

  ret = SOC_SAND_OK; sal_memset(&egq_ccm_tbl_data, 0x0, sizeof(egq_ccm_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 29, exit);

  ret = SOC_SAND_OK; sal_memset(&egq_pmc_tbl_data, 0x0, sizeof(egq_pmc_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 30, exit);

  ret = SOC_SAND_OK; sal_memset(&egq_cbm_tbl_data, 0x0, sizeof(egq_cbm_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 31, exit);

  ret = SOC_SAND_OK; sal_memset(&egq_dwm_new_tbl_data, 0x0, sizeof(egq_dwm_new_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 32, exit);
  
  ret = SOC_SAND_OK; sal_memset(&egq_dwm_tbl_data, 0x0, sizeof(egq_dwm_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 37, exit);

  ret = SOC_SAND_OK; sal_memset(&egq_dwm_tbl_data, 0x0, sizeof(egq_dwm_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 38, exit);

  ret = SOC_SAND_OK; sal_memset(&egq_pct_tbl_data, 0x0, sizeof(egq_pct_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 41, exit);

  ret = SOC_SAND_OK; sal_memset(&egq_vlan_table_tbl_data, 0x0, sizeof(egq_vlan_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 42, exit);

  ret = SOC_SAND_OK; sal_memset(&egq_tc_dp_map_tbl_data, 0x0, sizeof(egq_tc_dp_map_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 43, exit);

  ret = SOC_SAND_OK; sal_memset(&egq_fqp_nif_port_mux_tbl_data, 0x0, sizeof(egq_fqp_nif_port_mux_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 70, exit);

  ret = SOC_SAND_OK; sal_memset(&egq_key_profile_map_index_tbl_data, 0x0, sizeof(egq_key_profile_map_index_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 71, exit);

  ret = SOC_SAND_OK; sal_memset(&egq_tcam_key_resolution_profile_tbl_data, 0x0, sizeof(egq_tcam_key_resolution_profile_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 72, exit);

  ret = SOC_SAND_OK; sal_memset(&egq_ppct_tbl_data, 0x0, sizeof(egq_ppct_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 73, exit);


  /*Egress Shaper Nif Mal Credit Configuration (Nif Mal Scm)*/
  for (tbl_idx = 0; tbl_idx < SOC_PB_NOF_MAC_LANES; tbl_idx++)
  {
    ret = SOC_SAND_OK; sal_memset(&egq_nif_scm_tbl_data, 0x0, sizeof(egq_nif_scm_tbl_data));
    SOC_SAND_CHECK_FUNC_RESULT(ret, 11, exit);


    nof_lines_total = tables->egq.nif_scm_tbl.addr.size;
    lines_max = SOC_PB_INIT_COUNT_MAX(regs->egq.indirect_command_reg.indirect_command_count);
    do {
         nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
         nof_lines_total -= nof_lines;

         res = soc_petra_egq_set_reps_for_tbl_unsafe(unit, nof_lines);
         SOC_SAND_CHECK_FUNC_RESULT(res, 101, exit);

         res = soc_petra_egq_nif_scm_tbl_set_unsafe(
                   unit,
                   tbl_idx,
                   first_entry,
                   &egq_nif_scm_tbl_data
               );
         SOC_SAND_CHECK_FUNC_RESULT(res, 102, exit);
         first_entry += nof_lines;
    } while(nof_lines_total > 0);
    first_entry = 0;
  }

  /*Egress Shaper Recycling Ports Credit Configuration (Rcy Scm)*/
  nof_lines_total = tables->egq.rcy_scm_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->egq.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_egq_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 133, exit);

       res = soc_petra_egq_rcy_scm_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &egq_rcy_scm_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 134, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Egress Shaper CPUPorts Credit Configuration(Cpu Scm)*/
  nof_lines_total = tables->egq.cpu_scm_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->egq.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_egq_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 135, exit);

       res = soc_petra_egq_cpu_scm_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &egq_cpu_scm_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 136, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Egress Shaper Calendar Selector (CCM)*/
  nof_lines_total = tables->egq.ccm_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->egq.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_egq_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 137, exit);

       res = soc_petra_egq_ccm_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &egq_ccm_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 138, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Per Port Max Credit Memory (PMC)*/
  nof_lines_total = tables->egq.pmc_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->egq.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_egq_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 139, exit);

       res = soc_petra_egq_pmc_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &egq_pmc_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Egress Shaper Per Port Credit Balance Memory (CBM)*/
  nof_lines_total = tables->egq.cbm_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->egq.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_egq_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 141, exit);

       res = soc_petra_egq_cbm_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &egq_cbm_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 142, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Ofp Dequeue Wfq Configuration Memory (DWM_NEW)*/
  egq_dwm_new_tbl_data.mc_high_queue_weight = 1;
  egq_dwm_new_tbl_data.uc_high_queue_weight = 1;
  nof_lines_total = tables->egq.dwm_new_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->egq.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_egq_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 153, exit);

       res = soc_pb_egq_dwm_new_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &egq_dwm_new_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 154, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Ofp Dequeue Wfq Configuration Memory (DWM)*/
  egq_dwm_tbl_data.mc_or_mc_low_queue_weight = 1;
  egq_dwm_tbl_data.uc_or_uc_low_queue_weight = 1;
  nof_lines_total = tables->egq.dwm_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->egq.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_egq_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 155, exit);

       res = soc_petra_egq_dwm_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &egq_dwm_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 156, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Port Configuration Table (PCT)*/
  nof_lines_total = tables->egq.pct_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->egq.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_egq_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 161, exit);

       res = soc_pb_egq_pct_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &egq_pct_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 162, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Vlan Table Configuration Memory(Vlan Table)*/
  nof_lines_total = tables->egq.vlan_table_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->egq.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_egq_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 163, exit);

       res = soc_petra_egq_vlan_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &egq_vlan_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 164, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Tc Dp Map*/
  nof_lines_total = tables->egq.tc_dp_map_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->egq.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_egq_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 165, exit);

       tc_dp_entry.dp = 0;
       tc_dp_entry.is_egr_mc = 0;
       tc_dp_entry.is_syst_mc = 0;
       tc_dp_entry.map_profile = 0;
       tc_dp_entry.tc = 0;

       res = soc_pb_egq_tc_dp_map_tbl_set_unsafe(
                 unit,
                 &tc_dp_entry,
                 &egq_tc_dp_map_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 166, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Fqp Nif Port Mux*/
  egq_fqp_nif_port_mux_tbl_data.fqp_nif_port_mux = SOC_PB_EGQ_NIF_PORT_CAL_BW_INVALID;
  nof_lines_total = tables->egq.fqp_nif_port_mux_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->egq.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_egq_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 219, exit);

       res = soc_pb_egq_fqp_nif_port_mux_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &egq_fqp_nif_port_mux_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 220, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Key Profile Map Index*/
  nof_lines_total = tables->egq.key_profile_map_index_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->egq.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_egq_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 223, exit);

       res = soc_pb_egq_key_profile_map_index_tbl_set_unsafe(
                 unit,
                 first_entry,
                 0,
                 &egq_key_profile_map_index_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 224, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*TCAM Key Resolution Profile*/
  nof_lines_total = tables->egq.tcam_key_resolution_profile_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->egq.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_egq_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 225, exit);

       res = soc_pb_egq_tcam_key_resolution_profile_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &egq_tcam_key_resolution_profile_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 224, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /* PPCT */
  do
  {
    egq_ppct_tbl_data.dst_system_port_id = SOC_PETRA_MAX_SYSTEM_PHYSICAL_PORT_ID;
    egq_ppct_tbl_data.lb_key_max = 255;
    soc_pb_pp_reps_for_tbl_set_unsafe(unit, SOC_PB_PP_MAX_NOF_REPS);
    res = soc_pb_pp_egq_ppct_tbl_set_unsafe(
            unit,
            SOC_PB_PP_FIRST_TBL_ENTRY,
            &egq_ppct_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 225, exit);
  } while(0);

  first_entry = 0;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mgmt_egq_tbls_init()",0,0);
}

uint32
  soc_pb_mgmt_epni_tbls_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    nof_lines,
    nof_lines_total,
    lines_max,
    first_entry = 0,
    tbl_ndx;
  SOC_PB_TBLS
    *tables;
  SOC_PB_REGS
    *regs;
  SOC_PB_EPNI_COPY_ENGINE_PROGRAM_TBL_DATA
    epni_copy_engine_program_tbl_data;
  SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL_DATA
    epni_lfem_field_select_map_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_EPNI_TBLS_INIT);

  res = soc_pb_tbls_get(
          &tables
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  regs = soc_pb_regs();

  ret = SOC_SAND_OK; sal_memset(&epni_copy_engine_program_tbl_data, 0x0, sizeof(epni_copy_engine_program_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 13, exit);

  ret = SOC_SAND_OK; sal_memset(&epni_lfem_field_select_map_tbl_data, 0x0, sizeof(epni_lfem_field_select_map_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 16, exit);

  /*Copy Engine0/1/2 Program*/
  for (tbl_ndx = 0; tbl_ndx < SOC_PB_EPNI_COPY_ENGINE_PROGRAM_NOF_TBLS; ++tbl_ndx)
  {
    nof_lines_total = tables->epni.copy_engine_program_tbl[tbl_ndx].addr.size;
    lines_max = SOC_PB_INIT_COUNT_MAX(regs->epni.indirect_command_reg.indirect_command_count);
    do {
         nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
         nof_lines_total -= nof_lines;

         res = soc_petra_epni_set_reps_for_tbl_unsafe(unit, nof_lines);
         SOC_SAND_CHECK_FUNC_RESULT(res, 105, exit);

         res = soc_pb_epni_copy_engine_program_tbl_set_unsafe(
                   unit,
                   tbl_ndx,
                   first_entry,
                   &epni_copy_engine_program_tbl_data
               );
         SOC_SAND_CHECK_FUNC_RESULT(res, 106, exit);
         first_entry += nof_lines;
    } while(nof_lines_total > 0);
    first_entry = 0;
  }

  /*Lfem0/1/2 Field Select Map*/
  for (tbl_ndx = 0; tbl_ndx < SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_NOF_TBLS; ++tbl_ndx)
  {
    nof_lines_total = tables->epni.lfem_field_select_map_tbl[tbl_ndx].addr.size;
    lines_max = SOC_PB_INIT_COUNT_MAX(regs->epni.indirect_command_reg.indirect_command_count);
    do {
         nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
         nof_lines_total -= nof_lines;

         res = soc_petra_epni_set_reps_for_tbl_unsafe(unit, nof_lines);
         SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);

         res = soc_pb_epni_lfem_field_select_map_tbl_set_unsafe(
                   unit,
                   tbl_ndx,
                   first_entry,
                   &epni_lfem_field_select_map_tbl_data
               );
         SOC_SAND_CHECK_FUNC_RESULT(res, 112, exit);
         first_entry += nof_lines;
    } while(nof_lines_total > 0);
    first_entry = 0;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mgmt_epni_tbls_init()",0,0);
}

STATIC uint32
  soc_pb_mgmt_cfc_tbls_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    nof_lines,
    nof_lines_total,
    lines_max,
    first_entry = 0;
  SOC_PB_TBLS
    *tables;
  SOC_PB_REGS
    *regs;
  SOC_PB_CFC_RCL2_OFP_TBL_DATA
    cfc_rcl2_ofp_tbl_data;
  SOC_PB_CFC_NIFCLSB2_OFP_TBL_DATA
    cfc_nifclsb2_ofp_tbl_data;
  SOC_PB_CFC_CALRX_TBL_DATA
    cfc_calrx_tbl_data;
  SOC_PB_CFC_OOB_SCH_MAP_TBL_DATA
    cfc_oob_sch_map_tbl_data;
  SOC_PB_CFC_CALTX_TBL_DATA
    cfc_caltx_tbl_data;
  SOC_PB_CFC_CALRX_TBL_DATA
    cfc_ilkn_calrx_tbl_data;
  SOC_PB_CFC_OOB_SCH_MAP_TBL_DATA
    cfc_ilkn_sch_map_tbl_data;
  SOC_PB_CFC_CALTX_TBL_DATA
    cfc_ilkn_caltx_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_CFC_TBLS_INIT);

  res = soc_pb_tbls_get(
          &tables
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  regs = soc_pb_regs();

  ret = SOC_SAND_OK; sal_memset(&cfc_rcl2_ofp_tbl_data, 0x0, sizeof(cfc_rcl2_ofp_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 11, exit);

  ret = SOC_SAND_OK; sal_memset(&cfc_nifclsb2_ofp_tbl_data, 0x0, sizeof(cfc_nifclsb2_ofp_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 12, exit);

  ret = SOC_SAND_OK; sal_memset(&cfc_calrx_tbl_data, 0x0, sizeof(cfc_calrx_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 14, exit);

  ret = SOC_SAND_OK; sal_memset(&cfc_oob_sch_map_tbl_data, 0x0, sizeof(cfc_oob_sch_map_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 15, exit);

  ret = SOC_SAND_OK; sal_memset(&cfc_caltx_tbl_data, 0x0, sizeof(cfc_caltx_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 17, exit);

  ret = SOC_SAND_OK; sal_memset(&cfc_ilkn_calrx_tbl_data, 0x0, sizeof(cfc_ilkn_calrx_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 19, exit);

  ret = SOC_SAND_OK; sal_memset(&cfc_ilkn_sch_map_tbl_data, 0x0, sizeof(cfc_ilkn_sch_map_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 21, exit);

  ret = SOC_SAND_OK; sal_memset(&cfc_ilkn_caltx_tbl_data, 0x0, sizeof(cfc_ilkn_caltx_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 23, exit);

  /*RCL2 OFP*/
  nof_lines_total = tables->cfc.rcl2_ofp_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->cfc.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_cfc_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 101, exit);

       res = soc_pb_cfc_rcl2_ofp_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &cfc_rcl2_ofp_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 102, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*NIFCLSB2 OFP*/
  nof_lines_total = tables->cfc.nifclsb2_ofp_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->cfc.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_cfc_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 103, exit);

       res = soc_pb_cfc_nifclsb2_ofp_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &cfc_nifclsb2_ofp_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 104, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*CALRXA*/
  nof_lines_total = tables->cfc.calrxa_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->cfc.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_cfc_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 105, exit);

       res = soc_pb_cfc_oob_calrx_tbl_set_unsafe(
                 unit,
                 SOC_TMC_FC_OOB_ID_A,
                 first_entry,
                 &cfc_calrx_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 106, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*CALRXB*/
  nof_lines_total = tables->cfc.calrxb_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->cfc.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_cfc_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 107, exit);

       res = soc_pb_cfc_oob_calrx_tbl_set_unsafe(
                 unit,
                 SOC_TMC_FC_OOB_ID_B,
                 first_entry,
                 &cfc_calrx_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 108, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Oob0 Sch Map*/
  nof_lines_total = tables->cfc.oob0_sch_map_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->cfc.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_cfc_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 109, exit);

       res = soc_pb_cfc_oob_sch_map_tbl_set_unsafe(
                 unit,
                 SOC_TMC_FC_OOB_ID_A,
                 first_entry,
                 &cfc_oob_sch_map_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Oob1 Sch Map*/
  nof_lines_total = tables->cfc.oob1_sch_map_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->cfc.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_cfc_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);

       res = soc_pb_cfc_oob_sch_map_tbl_set_unsafe(
                 unit,
                 SOC_TMC_FC_OOB_ID_B,
                 first_entry,
                 &cfc_oob_sch_map_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 112, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*CALTX*/
  nof_lines_total = tables->cfc.caltx_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->cfc.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_cfc_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 113, exit);

       res = soc_pb_cfc_oob_caltx_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &cfc_caltx_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 114, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Ilkn0 CALRX*/
  nof_lines_total = tables->cfc.ilkn0_calrx_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->cfc.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_cfc_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 115, exit);

       res = soc_pb_cfc_ilkn_calrx_tbl_set_unsafe(
                 unit,
                 SOC_TMC_FC_OOB_ID_A,
                 first_entry,
                 &cfc_ilkn_calrx_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 116, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Ilkn1 CALRX*/
  nof_lines_total = tables->cfc.ilkn1_calrx_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->cfc.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_cfc_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 117, exit);

       res = soc_pb_cfc_ilkn_calrx_tbl_set_unsafe(
                 unit,
                 SOC_TMC_FC_OOB_ID_B,
                 first_entry,
                 &cfc_ilkn_calrx_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 118, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Ilkn0 Sch Map*/
  nof_lines_total = tables->cfc.ilkn0_sch_map_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->cfc.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_cfc_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 119, exit);

       res = soc_pb_cfc_ilkn_sch_map_tbl_set_unsafe(
                 unit,
                 SOC_TMC_FC_OOB_ID_A,
                 first_entry,
                 &cfc_ilkn_sch_map_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Ilkn1 Sch Map*/
  nof_lines_total = tables->cfc.ilkn1_sch_map_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->cfc.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_cfc_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 121, exit);

       res = soc_pb_cfc_ilkn_sch_map_tbl_set_unsafe(
                 unit,
                 SOC_TMC_FC_OOB_ID_B,
                 first_entry,
                 &cfc_ilkn_sch_map_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 122, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*ILKN0 CALTX*/
  nof_lines_total = tables->cfc.ilkn0_caltx_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->cfc.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_cfc_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 123, exit);

       res = soc_pb_cfc_ilkn_caltx_tbl_set_unsafe(
                 unit,
                 SOC_TMC_FC_OOB_ID_A,
                 first_entry,
                 &cfc_ilkn_caltx_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 124, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*ILKN1 CALTX*/
  nof_lines_total = tables->cfc.ilkn1_caltx_tbl.addr.size;
  lines_max = SOC_PB_INIT_COUNT_MAX(regs->cfc.indirect_command_reg.indirect_command_count);
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_cfc_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 125, exit);

       res = soc_pb_cfc_ilkn_caltx_tbl_set_unsafe(
                 unit,
                 SOC_TMC_FC_OOB_ID_B,
                 first_entry,
                 &cfc_ilkn_caltx_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 126, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mgmt_cfc_tbls_init()",0,0);
}

STATIC uint32
  soc_pb_mgmt_sch_tbls_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    nof_lines,
    first_entry = 0;
  uint32
    cal_set_idx,
    mal_idx;
  SOC_PB_TBLS
    *tables;
  SOC_PETRA_SCH_CAL_TBL_DATA
    sch_cal_tbl_data;
  SOC_PETRA_SCH_DSM_TBL_DATA
    sch_dsm_tbl_data;
  SOC_PETRA_SCH_SEM_TBL_DATA
    sch_sem_tbl_data;
  SOC_PETRA_SCH_FSF_TBL_DATA
    sch_fsf_tbl_data;
  SOC_PETRA_SCH_FGM_TBL_DATA
    sch_fgm_tbl_data;
  SOC_PETRA_SCH_SHC_TBL_DATA
    sch_shc_tbl_data;
  SOC_PETRA_SCH_SCT_TBL_DATA
    sch_sct_tbl_data;
  SOC_PETRA_SCH_SCHEDULER_INIT_TBL_DATA
    sch_scheduler_init_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_SCH_TBLS_INIT);

  res = soc_pb_tbls_get(
          &tables
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  ret = SOC_SAND_OK; sal_memset(&sch_cal_tbl_data, 0x0, sizeof(sch_cal_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 11, exit);
  ret = SOC_SAND_OK; sal_memset(&sch_dsm_tbl_data, 0x0, sizeof(sch_dsm_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 13, exit);

  ret = SOC_SAND_OK; sal_memset(&sch_sem_tbl_data, 0x0, sizeof(sch_sem_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 16, exit);

  ret = SOC_SAND_OK; sal_memset(&sch_fsf_tbl_data, 0x0, sizeof(sch_fsf_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 17, exit);

  ret = SOC_SAND_OK; sal_memset(&sch_fgm_tbl_data, 0x0, sizeof(sch_fgm_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 18, exit);

  ret = SOC_SAND_OK; sal_memset(&sch_shc_tbl_data, 0x0, sizeof(sch_shc_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 19, exit);

  ret = SOC_SAND_OK; sal_memset(&sch_sct_tbl_data, 0x0, sizeof(sch_sct_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 21, exit);

  ret = SOC_SAND_OK; sal_memset(&sch_scheduler_init_tbl_data, 0x0, sizeof(sch_scheduler_init_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 26, exit);

  /*Scheduler Credit Generation Calendar (CAL)*/
  /*
   *  The calendar size is at least one.
   *  Initialize the first entry of each calendar to
   *  non-existing (invalid) OFP index
   */
  sch_cal_tbl_data.hrsel = SOC_PETRA_FAP_PORT_ID_INVALID;
  for (cal_set_idx = 0; cal_set_idx < SOC_PETRA_OFP_NOF_RATES_CAL_SETS; cal_set_idx++)
  {
    /*
     *  MAL calendars
     */
    for (mal_idx = 0; mal_idx < SOC_PB_NOF_MAC_LANES; mal_idx++)
    {
      /*
       *  More complicated encoding than in Soc_petra-A
       */
      first_entry = ((mal_idx % SOC_PETRA_NOF_MAC_LANES) * SOC_PETRA_OFP_NOF_RATES_CAL_SETS
                      + cal_set_idx) * SOC_PETRA_OFP_RATES_CAL_LEN_SCH_OFFSET;
      if (mal_idx >= SOC_PETRA_NOF_MAC_LANES)
      {
        first_entry += SOC_PB_OFP_RATES_CAL_LEN_SCH_OFFSET_ADDITION;
      }

      res = soc_petra_sch_cal_tbl_set_unsafe(
              unit,
              first_entry,
              &sch_cal_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 102, exit);
    }

    first_entry = SOC_PB_OFP_RATES_CAL_CPU_BASE_SCH_OFFSET + cal_set_idx * SOC_PETRA_OFP_RATES_CAL_LEN_SCH_OFFSET;
    res = soc_petra_sch_cal_tbl_set_unsafe(
            unit,
            first_entry,
            &sch_cal_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 103, exit);

    first_entry = SOC_PB_OFP_RATES_CAL_RCY_BASE_SCH_OFFSET + cal_set_idx * SOC_PETRA_OFP_RATES_CAL_LEN_SCH_OFFSET;
    res = soc_petra_sch_cal_tbl_set_unsafe(
            unit,
            first_entry,
            &sch_cal_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 104, exit);
  }
  first_entry = 0;

  /*Dual Shaper Memory (DSM)*/
  nof_lines = tables->sch.dsm_tbl.addr.size;
  res = soc_petra_sch_set_reps_for_tbl_unsafe(unit, nof_lines);
  SOC_SAND_CHECK_FUNC_RESULT(res, 105, exit);

  res = soc_petra_sch_dsm_tbl_set_unsafe(
          unit,
          first_entry,
          &sch_dsm_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 106, exit);

  /*Scheduler Enable Memory (SEM)*/
  nof_lines = tables->sch.sem_tbl.addr.size;
  res = soc_petra_sch_set_reps_for_tbl_unsafe(unit, nof_lines);
  SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);

  res = soc_petra_sch_sem_tbl_set_unsafe(
          unit,
          first_entry,
          &sch_sem_tbl_data
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 112, exit);

  /*Flow Sub-Flow (FSF)*/
  nof_lines = tables->sch.fsf_tbl.addr.size;
  res = soc_petra_sch_set_reps_for_tbl_unsafe(unit, nof_lines);
  SOC_SAND_CHECK_FUNC_RESULT(res, 113, exit);

  res = soc_petra_sch_fsf_tbl_set_unsafe(
          unit,
          first_entry,
          &sch_fsf_tbl_data
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 114, exit);

  /*Flow Group Memory (FGM)*/
  nof_lines = tables->sch.fgm_tbl.addr.size;
  res = soc_petra_sch_set_reps_for_tbl_unsafe(unit, nof_lines);
  SOC_SAND_CHECK_FUNC_RESULT(res, 115, exit);

  res = soc_petra_sch_fgm_tbl_set_unsafe(
          unit,
          first_entry,
          &sch_fgm_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 116, exit);

  /*HR-Scheduler-Configuration (SHC)*/
  nof_lines = tables->sch.shc_tbl.addr.size;
  res = soc_petra_sch_set_reps_for_tbl_unsafe(unit, nof_lines);
  SOC_SAND_CHECK_FUNC_RESULT(res, 117, exit);

  res = soc_petra_sch_shc_tbl_set_unsafe(
            unit,
            first_entry,
            &sch_shc_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 118, exit);

  /*CL-Schedulers Type (SCT)*/
  nof_lines = tables->sch.sct_tbl.addr.size;
  res = soc_petra_sch_set_reps_for_tbl_unsafe(unit, nof_lines);
  SOC_SAND_CHECK_FUNC_RESULT(res, 121, exit);

  res = soc_petra_sch_sct_tbl_set_unsafe(
          unit,
          first_entry,
          &sch_sct_tbl_data
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 122, exit);

  /*Scheduler Init*/
  sch_scheduler_init_tbl_data.schinit = 0x1;

  res = soc_petra_sch_scheduler_init_tbl_set_unsafe(
          unit,
          first_entry,
          &sch_scheduler_init_tbl_data
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 132, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mgmt_sch_tbls_init()",0,0);
}


STATIC uint32
  soc_pb_mgmt_tbls_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_TBLS_INIT);

#if SOC_PB_MGMT_TBL_WRKLOAD_DISPLAY_EN
  Soc_pb_mgmt_tbls_curr_workload = 0;
  soc_pb_mgmt_tbl_total_lines_set();
#endif

  res = soc_pb_mgmt_ire_tbls_init(unit, silent);
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  res = soc_pb_mgmt_idr_tbls_init(unit, silent);
  SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);

  res = soc_pb_mgmt_irr_tbls_init(unit, silent);
  SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);

  res = soc_pb_mgmt_ihp_tbls_init(unit, silent);
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  res = soc_pb_mgmt_ihb_tbls_init(unit, silent);
  SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);

  res = soc_pb_mgmt_iqm_tbls_init(unit, silent);
  SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

  res = soc_pb_mgmt_ips_tbls_init(unit, silent);
  SOC_SAND_CHECK_FUNC_RESULT(res, 19, exit);

  res = soc_pb_mgmt_egq_tbls_init(unit, silent);
  SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

  res = soc_pb_mgmt_epni_tbls_init(unit, silent);
  SOC_SAND_CHECK_FUNC_RESULT(res, 26, exit);

  res = soc_pb_mgmt_cfc_tbls_init(unit, silent);
  SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);

  res = soc_pb_mgmt_sch_tbls_init(unit, silent);
  SOC_SAND_CHECK_FUNC_RESULT(res, 28, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mgmt_tbls_init()", 0, 0);
}
/*
 *  Tables Initialization }
 */

/************************************************************************/
/* Initialize registers according to the operation mode                 */
/************************************************************************/
STATIC uint32
  soc_pb_mgmt_op_mode_related_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PB_HW_ADJUSTMENTS         *hw_adjust
  )
{
  uint32
    fld_val,
    reg_val,
    res = SOC_SAND_OK,
    ftmh_lb_key_enable,
    ftmh_lb_key_ext_mode;
  uint8
    is_var_size_cells,
    bool_val;
  uint32
    nif_grp_ndx;
  SOC_PETRA_MGMT_PCKT_SIZE
    pckt_size_range;
  SOC_PB_MGMT_FTMH_LB_EXT_MODE
    ftmh_lb_ext_mode;
  SOC_PETRA_REGS
    *regs;
  SOC_PB_PP_REGS
    *pp_regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_OP_MODE_RELATED_INIT);

  regs = soc_petra_regs();
  pp_regs = soc_pb_pp_regs();

  /*
   *  Set combo (shared) quartets configuration - towards the NIF or towards the Fabric
   */
  for (nif_grp_ndx = 0; nif_grp_ndx < SOC_PETRA_NOF_NIF_GRPS; nif_grp_ndx++)
  {
    bool_val = soc_petra_sw_db_combo_nif_not_fabric_get(
                 unit,
                 nif_grp_ndx
               );
    fld_val = SOC_SAND_BOOL2NUM(bool_val);
    SOC_PB_FLD_ISET(regs->nif.nif_config_reg.serdes_g4_en, fld_val, nif_grp_ndx, 10, exit);
  }

  bool_val = soc_petra_sw_db_is_fap20_and_a1_in_system_get(unit);
  if (bool_val == TRUE)
  {
    SOC_PB_FLD_SET(regs->mesh_topology.mesh_topology_reg.fld1, 0x7, 20, exit);
  }

  is_var_size_cells = soc_petra_sw_db_is_fabric_variable_cell_size_get(unit);
  if (hw_adjust->nif.ilkn_nof_entries > 0)
  {
    pckt_size_range.min = SOC_PB_INIT_PCKT_SIZE_BYTES_ILKN_MIN_INTERN;
  }
  else
  {
    pckt_size_range.min = (is_var_size_cells)?SOC_PETRA_MGMT_PCKT_SIZE_BYTES_VSC_MIN:SOC_PETRA_MGMT_PCKT_SIZE_BYTES_FSC_MIN;
  }
  pckt_size_range.max = (is_var_size_cells)?SOC_PETRA_MGMT_PCKT_SIZE_BYTES_VSC_MAX:SOC_PETRA_MGMT_PCKT_SIZE_BYTES_FSC_MAX;

  res = soc_pb_mgmt_pckt_size_range_set_unsafe(
          unit,
          SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE_INTERN,
          &pckt_size_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if (hw_adjust->nif.ilkn_nof_entries > 0)
  {
    pckt_size_range.min = SOC_PB_INIT_PCKT_SIZE_BYTES_ILKN_MIN_EXTERN;
    res = soc_pb_mgmt_pckt_size_range_set_unsafe(
            unit,
            SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE_EXTERN,
            &pckt_size_range
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);
  }

  /*
   * FTMH LB mode
   */
  ftmh_lb_ext_mode = soc_petra_sw_db_ftmh_lb_ext_mode_get(unit);
  ftmh_lb_key_enable = ftmh_lb_key_ext_mode = 0;
  if (ftmh_lb_ext_mode != SOC_PB_MGMT_FTMH_LB_EXT_MODE_DISABLED)
  {
    ftmh_lb_key_enable = 0x1;
    ftmh_lb_key_ext_mode =
      (ftmh_lb_ext_mode == SOC_PB_MGMT_FTMH_LB_EXT_MODE_8B_LB_KEY_8B_STACKING_ROUTE_HISTORY ? 0x1 : 0x0);
  }
  
  SOC_PB_PP_REG_GET(pp_regs->eci.system_headers_configurations0_reg, reg_val, 35, exit);
  SOC_PB_PP_FLD_TO_REG(pp_regs->eci.system_headers_configurations0_reg.ftmh_lb_key_ext_enable, ftmh_lb_key_enable, reg_val, 40, exit);
  SOC_PB_PP_FLD_TO_REG(pp_regs->eci.system_headers_configurations0_reg.ftmh_lb_key_ext_mode, ftmh_lb_key_ext_mode, reg_val, 40, exit);
  SOC_PB_PP_REG_SET(pp_regs->eci.system_headers_configurations0_reg, reg_val, 55, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mgmt_op_mode_related_init()", 0, 0);
}

/*
 *  Init sequence -
 *  per-block initialization, hardware adjustments etc. {
 */
STATIC uint32
  soc_pb_mgmt_functional_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PB_HW_ADJUSTMENTS      *hw_adjust,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    qdr_nof_entries,
    res = SOC_SAND_OK;
  SOC_PETRA_STAT_ALL_STATISTIC_COUNTERS
    counters;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_FUNCTIONAL_INIT);

  soc_petra_PETRA_STAT_ALL_STATISTIC_COUNTERS_clear(&counters);

  qdr_nof_entries = soc_petra_init_qdr_size_to_bits(hw_adjust->qdr.qdr_size_mbit) / SOC_PETRA_INIT_QDR_ENTRY_SIZE_BITS;

  res = soc_pb_mgmt_op_mode_related_init(
          unit,
          hw_adjust
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  res = soc_petra_stat_all_counters_get(
          unit,
          SOC_PETRA_STAT_PRINT_LEVEL_ALL,
          0,
          &counters
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 7, exit);

  /* Clear Interrupts */
  res = soc_pb_interrupt_mask_clear_unsafe(
    unit
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 8, exit);

  /* Clear Interrupts */
  res = soc_pb_interrupt_initial_mask_lift(
    unit
    ) ;
  SOC_SAND_CHECK_FUNC_RESULT(res, 9, exit);

  res = soc_petra_egr_queuing_init(
        unit
      );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_fabric_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_pb_flow_control_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_ingress_scheduler_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_pb_interrupt_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  res = soc_petra_ofp_rates_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  res = soc_petra_ports_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  res = soc_petra_scheduler_end2end_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  res = soc_petra_serdes_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

  res = soc_petra_ipq_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 95, exit);

  res = soc_petra_itm_init(
          unit,
          qdr_nof_entries
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  res = soc_petra_stat_statistics_module_initialize(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);

  res = soc_petra_mc_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);

  res = soc_petra_diag_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);

  res = soc_petra_pkt_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

  res = soc_petra_cell_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);

  res = soc_pb_pmf_low_level_init_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);

  res = soc_pb_parser_init(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);

  res = soc_pb_ports_init(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);

  res = soc_pb_egr_prog_editor_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 210, exit);

  res = soc_pb_cnt_init(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 220, exit);
  
  res = soc_pb_tdm_init(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 230, exit);

  res = soc_pb_egr_acl_init(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 240, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mgmt_functional_init()", 0, 0);
}

STATIC uint32
  soc_pb_mgmt_hw_set_defaults(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    fld_val = 0,
    reg_val = 0,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;
  uint32
    inst_idx = 0;
  uint8
    is_variable_not_fixed,
    is_fe1600;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_HW_SET_DEFAULTS);

  regs = soc_petra_regs();
  is_variable_not_fixed = soc_petra_sw_db_is_fabric_variable_cell_size_get(unit);
  is_fe1600 = soc_petra_sw_db_is_fe1600_in_system_get(unit);
  
  /*
   *  IDR
   */
  SOC_PETRA_REG_GET(regs->idr.static_configuration_reg, reg_val, 10, exit);

  fld_val = 0x1;
  SOC_PETRA_FLD_TO_REG(regs->idr.static_configuration_reg.fbc_internal_reuse, fld_val, reg_val, 20, exit);

  SOC_PETRA_REG_SET(regs->idr.static_configuration_reg, reg_val, 60, exit);

  SOC_PB_FLD_SET(regs->idr.reassembly_timeout_reg.reassembly_timeout, 0xFFFFFF, 90, exit);

  /*
   * IRR
   */
  SOC_PB_FLD_SET(regs->irr.dynamic_configuration_reg.use_dest_as_mc_cud, 0x1, 95, exit);

  /*
   *  IQM
   */
  SOC_PB_REG_SET(regs->iqm.iqm_enablers_reg, 0x104020, 90, exit);

  /*
   *  IPS
   */
  SOC_PB_REG_SET(regs->ips.ips_general_configurations_reg, 0xa00, 100, exit);

  /* 
   * IPT 
   */
  SOC_PB_FLD_SET(regs->ipt.ipt_enables_reg.zero_act_links_rate_en, 0x1, 102, exit);

  /*
   *  QDR
   */
  SOC_PETRA_REG_SET(regs->qdr.qdr_dummy_data0_reg, 0x0, 110, exit);
  SOC_PETRA_REG_SET(regs->qdr.qdr_dummy_data1_reg, 0x0, 111, exit);
  SOC_PETRA_REG_SET(regs->qdr.qdr_dummy_data2_reg, 0x0, 112, exit);
  SOC_PETRA_REG_SET(regs->qdr.qdr_dummy_data3_reg, 0x0, 113, exit);
  SOC_PETRA_REG_SET(regs->qdr.qdr_dummy_data4_reg, 0x0, 114, exit);
  SOC_PETRA_REG_SET(regs->qdr.qdr_dummy_data5_reg, 0x0, 115, exit);
  SOC_PETRA_REG_SET(regs->qdr.qdr_dummy_data6_reg, 0x0, 116, exit);
  SOC_PETRA_REG_SET(regs->qdr.qdr_dummy_data7_reg, 0x0, 117, exit);

  /*
   *  Scheduler
   */
  for (inst_idx = 0; inst_idx < SOC_PB_SCH_DVS_CONFIG_REG_NOF_REGS; ++inst_idx)
  {
    SOC_PB_REG_SET(regs->sch.dvs_config_reg[inst_idx], 0x0, 122, exit);
  }

  fld_val = 0x0;
  SOC_PB_REG_GET(regs->sch.dvs_config1_reg, reg_val, 124, exit);
  SOC_PB_FLD_TO_REG(regs->sch.dvs_config1_reg.cpuforce_pause, fld_val, reg_val, 125, exit);
  SOC_PB_FLD_TO_REG(regs->sch.dvs_config1_reg.rcyforce_pause, fld_val, reg_val, 126, exit);
  SOC_PB_FLD_TO_REG(regs->sch.dvs_config1_reg.olpforce_pause, fld_val, reg_val, 127, exit);
  SOC_PB_FLD_TO_REG(regs->sch.dvs_config1_reg.erpforce_pause, fld_val, reg_val, 128, exit);
  SOC_PB_REG_SET(regs->sch.dvs_config1_reg, reg_val, 129, exit);

  /*
   *  FDT
   */
  SOC_PB_FLD_SET(regs->fdt.fdt_enabler_reg.del_crc_pkt, 0x1, 140, exit);
  
  if (is_fe1600)
  {
    SOC_PB_FLD_SET(regs->fdt.fdt_enabler_reg.fe1200_mode, 0x1, 145, exit);
    /* Set intrlvd mode 
    for (inst_idx = 0; inst_idx < SOC_PETRA_BLK_NOF_INSTANCES_FABRIC_MAC; inst_idx++)
    {
      SOC_PB_FLD_ISET(regs->fabric_mac.cntrl_intrlvd_mode_reg.cntrl_intrlvd_mode, 0xFFF, inst_idx, 146, exit);
    }*/
  }
  else
  {
    SOC_PB_FLD_SET(regs->fdt.fdt_enabler_reg.fe1200_mode, 0x0, 145, exit);
  }
  
  if (is_variable_not_fixed)
  {
    SOC_PB_FLD_SET(regs->fdt.fdt_enabler_reg.small_packet_en, 0x1, 141, exit);
  }
  else
  {
    SOC_PB_FLD_SET(regs->fdt.fdt_enabler_reg.en_500m_cell, 0x1, 142, exit);
    SOC_PB_FLD_SET(regs->fdt.fdt_enabler_reg.use_4seg_mode, 0x1, 144, exit);
  }

  /*
   *  RTP
   */
  SOC_PB_FLD_SET(regs->rtp.rtp_enable_reg.aclm, 0x0, 150, exit);
  SOC_PB_FLD_SET(regs->rtp.mc_traverse_rate_reg.mctraverse_rate, 0x0F00, 155, exit);

  /*
   *  Fabric MAC
   */
  for (inst_idx = 0; inst_idx < SOC_PETRA_BLK_NOF_INSTANCES_FABRIC_MAC; inst_idx++)
  {
    /*
     * Setting the MACs RX-TX to count data cells
     * The counter_all that counts the transmitted cells
     * counts non-empty data and control cells
     */
    SOC_PB_FLD_ISET(regs->fabric_mac.enablers_reg.tx_cnt_cfg, 0, inst_idx, 160, exit);

    /*
     * Setting the MACs RX-TX to count data cells
     * The counter_all that counts the good cells to up count the
     * leaky bucket counts non-empty data and control cells
     */
    SOC_PB_FLD_ISET(regs->fabric_mac.enablers_reg.rx_cnt_cfg, 0, inst_idx, 162, exit);
  }

  /*
   *  EGQ
   */
  SOC_PB_REG_GET(regs->egq.cpu_olp_rcy_port_delay_configuration_reg, reg_val, 170, exit);
  fld_val = 12;
  SOC_PB_FLD_TO_REG(regs->egq.cpu_olp_rcy_port_delay_configuration_reg.cpu_port_delay, fld_val, reg_val, 171, exit);
  fld_val = 12;
  SOC_PB_FLD_TO_REG(regs->egq.cpu_olp_rcy_port_delay_configuration_reg.olp_port_delay, fld_val, reg_val, 172, exit);
  fld_val = 1;
  SOC_PB_FLD_TO_REG(regs->egq.cpu_olp_rcy_port_delay_configuration_reg.rcy_port_delay, fld_val,  reg_val, 174, exit);
  SOC_PB_REG_SET(regs->egq.cpu_olp_rcy_port_delay_configuration_reg, reg_val, 176, exit);

  SOC_PB_REG_GET(regs->egq.fqpscheduler_configuration_reg, reg_val, 180, exit);
  fld_val = 1;
  SOC_PB_FLD_TO_REG(regs->egq.fqpscheduler_configuration_reg.cfg_del_req_en, fld_val, reg_val, 181, exit);
  SOC_PB_FLD_TO_REG(regs->egq.fqpscheduler_configuration_reg.cfg_olp_req_en, fld_val, reg_val, 182, exit);
  SOC_PB_FLD_TO_REG(regs->egq.fqpscheduler_configuration_reg.cfg_cpu_req_en, fld_val, reg_val, 183, exit);
  SOC_PB_FLD_TO_REG(regs->egq.fqpscheduler_configuration_reg.cfg_rcy_req_en, fld_val, reg_val, 184, exit);
  SOC_PB_REG_SET(regs->egq.fqpscheduler_configuration_reg, reg_val, 186, exit);
  
  

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mgmt_hw_set_defaults()", 0, 0);
}

uint32
  soc_pb_mgmt_hw_adjust_ddr_init(
    SOC_SAND_IN int          unit,
    SOC_SAND_IN uint32          instance_idx
  )
{
  uint32
#if SOC_PB_DRAM_DUTY_CYCLE_EN
    reg_val,
    fld_val,
#endif
    res;
  SOC_PETRA_REGS
    *regs;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_HW_ADJUST_DDR_INIT);

  regs = soc_petra_regs();

  /* DPRC in/out of reset */
  SOC_PETRA_FLD_SET(regs->eci.soc_petra_soft_reset_reg.dprc_reset[instance_idx], 0x1, 10, exit);
  SOC_PETRA_FLD_SET(regs->eci.soc_petra_soft_reset_reg.dprc_reset[instance_idx], 0x0, 20, exit);

#if SOC_PB_DRAM_DUTY_CYCLE_EN
  /*
   *  Duty cycle
   */
  SOC_PB_REG_IGET(regs->dpi.dcf_reg, reg_val, instance_idx , 30, exit);
  fld_val = 0x0;
  SOC_PB_FLD_TO_REG(regs->dpi.dcf_reg.dcf_lc, fld_val, reg_val, 32, exit);
  fld_val = 0x1;
  SOC_PB_FLD_TO_REG(regs->dpi.dcf_reg.dcf_fcmm, fld_val, reg_val, 34, exit);
  fld_val = 0x0;
  SOC_PB_FLD_TO_REG(regs->dpi.dcf_reg.dcf_thresh, fld_val, reg_val, 36, exit);
  fld_val = 0x0;
  SOC_PB_FLD_TO_REG(regs->dpi.dcf_reg.dcf_ovrd_vec, fld_val, reg_val, 38, exit);
  fld_val = 0x0;
  SOC_PB_FLD_TO_REG(regs->dpi.dcf_reg.dcf_ovrd_pol, fld_val, reg_val, 40, exit);
  SOC_PB_REG_ISET(regs->dpi.dcf_reg, reg_val, instance_idx , 42, exit);

  SOC_PB_REG_IGET(regs->drc.dcf, reg_val, instance_idx , 50, exit);
  fld_val = 0x3;
  SOC_PB_FLD_TO_REG(regs->drc.dcf.dcf_cmd, fld_val, reg_val, 52, exit);
  fld_val = 0x1;
  SOC_PB_FLD_TO_REG(regs->drc.dcf.dcf_ovrd_en, fld_val, reg_val, 52, exit);
  fld_val = 0x0; /* 0 means enabled */
  SOC_PB_FLD_TO_REG(regs->drc.dcf.dcf_enable, fld_val, reg_val, 54, exit);
  SOC_PB_REG_ISET(regs->drc.dcf, reg_val, instance_idx , 56, exit);
#endif

  SOC_PB_FLD_ISET(regs->dpi.dprc_enable_reg.dprc_en, 0x1, instance_idx, 60, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mgmt_hw_adjust_ddr_init()", 0, 0);
}
STATIC uint32
  soc_pb_mgmt_hw_adjust_ddr(
    SOC_SAND_IN int          unit,
    SOC_SAND_IN SOC_PETRA_HW_ADJ_DDR   *hw_adjust
  )
{
  uint32
    fld_val,
    reg_val,
    drams_banks_log,
    buff_size_log,
    same_row,
    val1, val2,
    round_trip_err_count,
    res,
    dram_size_per_interface_mbyte;
  uint32
    instance_idx = 0,
    nof_drams = 0;
  SOC_PETRA_ITM_DBUFF_SIZE_BYTES
    dbuff_size;
  SOC_PETRA_POLL_INFO
    poll_info;
  uint8
    poll_success;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_DIAG_DRAM_ERR_INFO
    error_info;
  SOC_PETRA_DIAG_DRAM_STATUS_INFO
    status_info;
  uint8
    is_new_pll = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_HW_ADJUST_DDR);

  SOC_SAND_CHECK_NULL_INPUT(hw_adjust);

  regs = soc_petra_regs();

  soc_petra_PETRA_POLL_INFO_clear(&poll_info);
  soc_petra_PETRA_DIAG_DRAM_ERR_INFO_clear(&error_info);
  soc_petra_PETRA_DIAG_DRAM_STATUS_INFO_clear(&status_info);

  if (SOC_PB_REV_ABOVE_A0)
  {
    is_new_pll = TRUE;
  }

  SOC_PETRA_FLD_GET(regs->eci.general_controls_reg.dbuff_size, fld_val, 5, exit);
  res = soc_petra_itm_dbuff_internal2size(
          fld_val,
          &dbuff_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  if (is_new_pll)
  {
    SOC_PB_FLD_GET(regs->eci.pll_status_reg.ddr_hs_lock, fld_val, 20, exit);
    if (fld_val != 0x1)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_INIT_DDR_PLL_NOT_LOCKED_ERR, 21, exit);
    }

    SOC_PB_FLD_GET(regs->eci.pll_status_reg.ddr_ab_hs_lock, fld_val, 22, exit);
    if (fld_val != 0x1)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_INIT_DDR_PLL_NOT_LOCKED_ERR, 23, exit);
    }

    SOC_PB_FLD_GET(regs->eci.pll_status_reg.ddr_ef_hs_lock, fld_val, 24, exit);
    if (fld_val != 0x1)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_INIT_DDR_PLL_NOT_LOCKED_ERR, 25, exit);
    }
  }
  else
  {
    SOC_PB_FLD_GET(regs->eci.pll_status_reg.ddr_lock, fld_val, 30, exit);
    if (fld_val != 0x1)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_INIT_DDR_PLL_NOT_LOCKED_ERR, 31, exit);
    }

    SOC_PB_FLD_GET(regs->eci.pll_status_reg.ddr_ab_lock, fld_val, 32, exit);
    if (fld_val != 0x1)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_INIT_DDR_PLL_NOT_LOCKED_ERR, 33, exit);
    }

    SOC_PB_FLD_GET(regs->eci.pll_status_reg.ddr_ef_lock, fld_val, 34, exit);
    if (fld_val != 0x1)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_INIT_DDR_PLL_NOT_LOCKED_ERR, 35, exit);
    }
  }

  for (instance_idx = 0; instance_idx < SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max); ++instance_idx)
  {
    if (hw_adjust->is_valid[instance_idx])
    {
      nof_drams++;
      res = soc_pb_mgmt_hw_adjust_ddr_init(
              unit,
              instance_idx
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

      if (hw_adjust->dram_type != SOC_PETRA_DRAM_TYPE_GDDR3)
      {
        SOC_PB_FLD_ISET(regs->dpi.dpi_general_config_reg.addr_13_on_wdqs_en, 1, instance_idx, 37, exit);
      }
    }
  }

  if(nof_drams == 0)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERR, 14, exit);
  }
  dram_size_per_interface_mbyte = (hw_adjust->dram_size_total_mbyte) / nof_drams;
  res = soc_petra_sw_db_dram_dram_size_set(
    unit,
    dram_size_per_interface_mbyte
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

  for (instance_idx = 0; instance_idx < SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max); ++instance_idx)
  {
    if (hw_adjust->is_valid[instance_idx])
    {
      if (hw_adjust->conf_mode == SOC_PETRA_HW_DRAM_CONF_MODE_PARAMS)
      {
        if (hw_adjust->nof_banks != hw_adjust->dram_conf.params_mode.params.nof_banks)
        {
          SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DRAM_NOF_BANKS_OUT_OF_RANGE_ERR, 41, exit);
        }
        if (hw_adjust->nof_columns != hw_adjust->dram_conf.params_mode.params.nof_cols)
        {
          SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DRAM_NOF_COLS_OUT_OF_RANGE_ERR, 42, exit);
        }

        /*
         *  Save in the SW_DB the DRAM configuration
         */
        if (instance_idx == 0)
        {
          res = soc_petra_sw_db_dram_type_set(
                  unit,
                  hw_adjust->dram_type
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 53, exit);

          res = soc_petra_sw_db_dram_conf_set(
                  unit,
                  &(hw_adjust->dram_conf.params_mode)
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 54, exit);
        }


        res = soc_petra_dram_info_set_unsafe(
                unit,
                instance_idx,
                hw_adjust->dram_conf.params_mode.dram_freq,
                hw_adjust->dram_type,
                &hw_adjust->dram_conf.params_mode.params
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

        sal_msleep(100);

        /*
         *  Verify DRC init complete (SERIAL INIT)
         */
        poll_info.expected_value = 0x1;
        poll_info.busy_wait_nof_iters = SOC_PB_MGMT_INIT_DRAM_BUSY_WAIT_ITERATIONS;
        poll_info.timer_nof_iters     = SOC_PB_MGMT_INIT_DRAM_TIMER_ITERATIONS;
        poll_info.timer_delay_msec    = SOC_PB_MGMT_INIT_DRAM_TIMER_DELAY_MSEC;
        res = soc_petra_status_fld_poll_unsafe(
                unit,
                SOC_PB_REG_DB_ACC_REF(regs->dpi.dpi_init_status_reg.ready),
                instance_idx,
                &poll_info,
                &poll_success
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 159, exit);

        if (poll_success == FALSE)
        {
  #if SOC_PETRA_DEBUG_IS_LVL2
          if (poll_success == FALSE)
          {
            soc_sand_os_printf(
              "The DPI-ready indication failed on DRAM-%u "
              "\n\r",
              instance_idx
            );
          }

          res = soc_petra_diag_dram_status_get_unsafe(
                  unit,
                  instance_idx,
                  &status_info
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 163, exit);

          soc_petra_PETRA_DIAG_DRAM_STATUS_INFO_print(&status_info);
          
          res = soc_petra_diag_dram_diagnostic_get_unsafe(
                  unit,
                  instance_idx,
                  &error_info
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 165, exit);

          soc_petra_PETRA_DIAG_DRAM_ERR_INFO_print(&error_info);
          
  #endif /* SOC_PETRA_DEBUG */
          if (poll_success == FALSE)
          {
            SOC_SAND_SET_ERROR_CODE(SOC_PB_MGMT_DRAM_INIT_FAILS_ERR, 170 + instance_idx, exit);
          }
        }
      }
      else
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DRAM_CONF_MODE_OUT_OF_RANGE_ERR, 52, exit);
      }
    }
  }

  /*
   *  MMU configuration
   */
  SOC_PB_REG_GET(regs->mmu.general_configuration_reg, reg_val, 55, exit);

  switch(hw_adjust->nof_banks) {
  case SOC_PETRA_DRAM_NUM_BANKS_4:
    fld_val = 0x0;
    break;
  case SOC_PETRA_DRAM_NUM_BANKS_8:
    fld_val = 0x1;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_HW_INVALID_NOF_BANKS_FOR_DRAM_TYPE_ERR, 58, exit);
  }

  SOC_PB_FLD_TO_REG(regs->mmu.general_configuration_reg.dram_bank_num, fld_val, reg_val, 60, exit);

  switch(nof_drams) {
  case 2:
    fld_val = 0x3;
    break;
  case 3:
    fld_val = 0x2;
    break;
  case 4:
    fld_val = 0x1;
    break;
  case 6:
    fld_val = 0x0;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_HW_INVALID_NUMBER_OF_DRAM_INTERFACES_ERR, 62, exit);
  }
  SOC_PB_FLD_TO_REG(regs->mmu.general_configuration_reg.dram_num, fld_val, reg_val, 65, exit);

  fld_val = 0x0;
  SOC_PB_FLD_TO_REG(regs->mmu.general_configuration_reg.scrambling_bit_position, fld_val, reg_val, 70, exit);

  fld_val = hw_adjust->nof_columns;
  SOC_PB_FLD_TO_REG(regs->mmu.general_configuration_reg.dram_col_num, fld_val, reg_val, 80, exit);

  fld_val = 0x1;
  SOC_PB_FLD_TO_REG(regs->mmu.general_configuration_reg.interleaving_rvrse_mode, fld_val, reg_val, 90, exit);

  fld_val = 0;
  SOC_PB_FLD_TO_REG(regs->mmu.general_configuration_reg.cons_banks_addr_map_mode, fld_val, reg_val, 95, exit);

  fld_val = (hw_adjust->nof_banks == SOC_PETRA_DRAM_NUM_BANKS_4)?0x2:0x3;
  SOC_PB_FLD_TO_REG(regs->mmu.general_configuration_reg.read_fifos_allowed, fld_val, reg_val, 102, exit);

  fld_val = (hw_adjust->nof_banks == SOC_PETRA_DRAM_NUM_BANKS_4)?0x18:0xc;
  SOC_PB_FLD_TO_REG(regs->mmu.general_configuration_reg.min_read_commands, fld_val, reg_val, 103, exit);

  if (
     (nof_drams == 6) &&
     (hw_adjust->nof_banks == SOC_PETRA_DRAM_NUM_BANKS_4) && (dbuff_size  == SOC_PETRA_ITM_DBUFF_SIZE_BYTES_2048)
     )
  {
    fld_val = 1;
  }
  else if ((nof_drams == 3) && (hw_adjust->nof_banks == SOC_PETRA_DRAM_NUM_BANKS_8) && (dbuff_size  == SOC_PETRA_ITM_DBUFF_SIZE_BYTES_2048))
  {
    fld_val = 1;
  }
  else if ((nof_drams == 3) && (hw_adjust->nof_banks == SOC_PETRA_DRAM_NUM_BANKS_4) && (dbuff_size  == SOC_PETRA_ITM_DBUFF_SIZE_BYTES_2048))
  {
    fld_val = 2;
  }
  else if ((nof_drams == 3) && (hw_adjust->nof_banks == SOC_PETRA_DRAM_NUM_BANKS_4) && (dbuff_size  == SOC_PETRA_ITM_DBUFF_SIZE_BYTES_1024))
  {
    fld_val = 1;
  }
  else
  {
    fld_val = 0;
  }

  SOC_PB_FLD_TO_REG(regs->mmu.general_configuration_reg.layer_periodicity, fld_val, reg_val, 110, exit);

  if ((nof_drams == 6) || (nof_drams == 3))
  {
    fld_val = 0;
  }
  else
  {
    drams_banks_log = soc_sand_log2_round_up(hw_adjust->nof_banks*nof_drams);
    buff_size_log = soc_sand_log2_round_up((hw_adjust->dbuff_size)/64);

    SOC_PB_FLD_GET(regs->mmu.bank_access_controller_configurations_reg.address_map_config_gen, same_row, 112, exit);

    val1 = drams_banks_log + same_row;
    val2 = buff_size_log + same_row;
    if (val1 > val2)
    {
      SOC_SAND_LIMIT_FROM_BELOW(val1, 3);
      fld_val = val1 - 3;
    }
    else
    {
      SOC_SAND_LIMIT_FROM_BELOW(val2, 3);
      fld_val = val2 - 3;
    }
  }

  SOC_PB_FLD_TO_REG(regs->mmu.general_configuration_reg.bank_interleaving_mode, fld_val, reg_val, 115, exit);

  SOC_PB_REG_SET(regs->mmu.general_configuration_reg, reg_val, 117, exit);

  SOC_PB_REG_GET(regs->mmu.bank_access_controller_configurations_reg, reg_val, 119, exit);

  fld_val = 166;
  SOC_PB_FLD_TO_REG(regs->mmu.bank_access_controller_configurations_reg.write_inhibit_cnt_sat, fld_val, reg_val, 120, exit);

  fld_val = (hw_adjust->dram_type == SOC_PETRA_DRAM_TYPE_DDR2)?0x2:0x1;
  SOC_PB_FLD_TO_REG(regs->mmu.bank_access_controller_configurations_reg.address_map_config_gen, fld_val, reg_val, 125, exit);

  fld_val = (hw_adjust->dram_type == SOC_PETRA_DRAM_TYPE_DDR2)?0x2:0x1;
  SOC_PB_FLD_TO_REG(regs->mmu.bank_access_controller_configurations_reg.address_map_config_rd, fld_val, reg_val, 130, exit);

  fld_val = 0x0;
  SOC_PB_FLD_TO_REG(regs->mmu.bank_access_controller_configurations_reg.allow_consecutive16_byte, fld_val, reg_val, 140, exit);

  fld_val = 0x0;
  SOC_PB_FLD_TO_REG(regs->mmu.bank_access_controller_configurations_reg.pbhm, fld_val, reg_val, 145, exit);

  fld_val = 0x1;
  SOC_PB_FLD_TO_REG(regs->mmu.bank_access_controller_configurations_reg.wr_priority_mode, fld_val, reg_val, 150, exit);

  if (hw_adjust->dram_type == SOC_PETRA_DRAM_TYPE_GDDR3)
  {
    fld_val = (hw_adjust->nof_banks == SOC_PETRA_DRAM_NUM_BANKS_4)?0x3:0x4;
    SOC_PB_FLD_TO_REG(regs->mmu.bank_access_controller_configurations_reg.wr_cmd_distance, fld_val, reg_val, 135, exit);
   
  }
  else
  {
    /* DDR2/DDR3 */
    if (hw_adjust->dram_type == SOC_PETRA_DRAM_TYPE_DDR2)
    {
      fld_val = 0x4;
    }
    else
    {
      fld_val = (hw_adjust->nof_banks == SOC_PETRA_DRAM_NUM_BANKS_4)?0x3:0x6;
    }
    SOC_PB_FLD_TO_REG(regs->mmu.bank_access_controller_configurations_reg.wr_cmd_distance, fld_val, reg_val, 137, exit);

  }

  SOC_PB_REG_SET(regs->mmu.bank_access_controller_configurations_reg, reg_val, 155, exit);

  /*
   *  Verify DRC init complete
   */
  poll_info.expected_value = 0x1;
  poll_info.busy_wait_nof_iters = SOC_PB_MGMT_INIT_DRAM_BUSY_WAIT_ITERATIONS;
  poll_info.timer_nof_iters     = SOC_PB_MGMT_INIT_DRAM_TIMER_ITERATIONS;
  poll_info.timer_delay_msec    = SOC_PB_MGMT_INIT_DRAM_TIMER_DELAY_MSEC;
  for (instance_idx = 0; instance_idx < SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max); ++instance_idx)
  {
    if (hw_adjust->is_valid[instance_idx])
    {
      res = soc_petra_status_fld_poll_unsafe(
              unit,
              SOC_PB_REG_DB_ACC_REF(regs->dpi.dpi_init_status_reg.ready),
              instance_idx,
              &poll_info,
              &poll_success
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 159, exit);

      /*
       * check roundtrip
       */
      res = soc_petra_diag_dram_roundtrip_status_check_unsafe(
              unit,
              instance_idx,
              &round_trip_err_count
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);

      if ((poll_success == FALSE) || (round_trip_err_count > 0))
      {
#if SOC_PETRA_DEBUG_IS_LVL2
        if (poll_success == FALSE)
        {
          soc_sand_os_printf(
            "The DPI-ready indication failed on DRAM-%u "
            "\n\r",
            instance_idx
          );
        }

        if (round_trip_err_count > 0)
        {
          soc_sand_os_printf(
            "Round-trip error count on DRAM-%u is %u"
            "\n\r",
            instance_idx,
            round_trip_err_count
          );
        }

        res = soc_petra_diag_dram_status_get_unsafe(
                unit,
                instance_idx,
                &status_info
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 163, exit);

        soc_petra_PETRA_DIAG_DRAM_STATUS_INFO_print(&status_info);
        
        res = soc_petra_diag_dram_diagnostic_get_unsafe(
                unit,
                instance_idx,
                &error_info
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 165, exit);

        soc_petra_PETRA_DIAG_DRAM_ERR_INFO_print(&error_info);
        
#endif /* SOC_PETRA_DEBUG */
        if (poll_success == FALSE)
        {
          SOC_SAND_SET_ERROR_CODE(SOC_PB_MGMT_DRAM_INIT_FAILS_ERR, 170 + instance_idx, exit);
        }

        if (round_trip_err_count > 0)
        {
          SOC_SAND_SET_ERROR_CODE(SOC_PB_MGMT_DRAM_INIT_FAILS_ERR, 180 + instance_idx, exit);
        }
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mgmt_hw_adjust_ddr()", 0, 0);
}

STATIC uint32
  soc_pb_mgmt_hw_adjust_qdr(
    SOC_SAND_IN int     unit,
    SOC_SAND_IN SOC_PB_HW_ADJ_QDR *hw_adjust
  )
{
  uint32
    fld_val,
    qdr_bdb_size_fld_val,
    res;
  SOC_PETRA_POLL_INFO
    poll_info;
  uint8
    poll_success;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_HW_ADJUST_QDR);

  SOC_SAND_CHECK_NULL_INPUT(hw_adjust);

  regs = soc_petra_regs();

  soc_petra_PETRA_POLL_INFO_clear(&poll_info);

  /*
   *  Configure QDR type
   */
  switch (hw_adjust->qdr_type)
  {
  case SOC_PB_HW_QDR_TYPE_QDR:
    SOC_PB_FLD_SET(regs->qdr.output_clock_control_reg.qdr3_mode, 0x0, 20, exit);
    SOC_PB_FLD_SET(regs->qdr.output_clock_control_reg.qdr3_rst, 0x0, 21, exit);
    SOC_PB_FLD_SET(regs->qdr.qdr_cq_edge_select_reg.cq_edge_sel, 0x0, 22, exit);
    break;
  case SOC_PB_HW_QDR_TYPE_QDR2P:
    SOC_PB_FLD_SET(regs->qdr.output_clock_control_reg.qdr3_mode, 0x0, 23, exit);
    SOC_PB_FLD_SET(regs->qdr.output_clock_control_reg.qdr3_rst, 0x0, 24, exit);
    SOC_PB_FLD_SET(regs->qdr.qdr_cq_edge_select_reg.cq_edge_sel, 0x1, 25, exit);
    break;
  case SOC_PB_HW_QDR_TYPE_QDR3:
    SOC_PB_FLD_SET(regs->qdr.output_clock_control_reg.qdr3_mode, 0x1, 26, exit);
    SOC_PB_FLD_SET(regs->qdr.output_clock_control_reg.qdr3_rst, 0x1, 27, exit);
    SOC_PB_FLD_SET(regs->qdr.qdr_cq_edge_select_reg.cq_edge_sel, 0x1, 28, exit);
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_INIT_QDR_TYPE_ILLEGAL_ERR, 29, exit);
  }

  if (!hw_adjust->is_core_clock_freq)
  {
    SOC_PETRA_FLD_GET(regs->eci.test_mux_config_reg.qdr_lock, fld_val, 30, exit);
    if (fld_val != 0x1)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_INIT_QDR_PLL_NOT_LOCKED_ERR, 33, exit);
    }
  }

  /* Value 0 - 3 corresponds to Number of BDBS 0/16/32/64 */
  qdr_bdb_size_fld_val = hw_adjust->qdr_size_mbit;

  /* BDB size */
  SOC_PETRA_FLD_SET(regs->iqm.bdb_configuration_reg.bdb_size, qdr_bdb_size_fld_val, 35, exit);

  SOC_PETRA_FLD_SET(regs->qdr.dll_control_reg.dll_div2_en, 0x1, 36, exit);

  /* Configure the Number of Bits according to the QDR Size */
  switch(hw_adjust->qdr_size_mbit)
  {
    case SOC_PETRA_HW_QDR_SIZE_MBIT_18:
      fld_val = 0x13;
      break;
    case SOC_PETRA_HW_QDR_SIZE_MBIT_36:
      fld_val = 0x14;
      break;
    case SOC_PETRA_HW_QDR_SIZE_MBIT_72:
      fld_val = 0x15;
      break;
    case SOC_PETRA_HW_QDR_SIZE_MBIT_144:
      fld_val = 0x16;
      break;
    default:
      fld_val = 0x15; /* The most common setting - just in case */
  }

  SOC_PETRA_FLD_SET(regs->qdr.bist_address_bits_number_reg.bist_addr_bit_num, fld_val, 70, exit);

  /*
   *  QDR OOR
   */
  res = soc_pb_mgmt_init_qdr_dll_mem_unsafe(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  SOC_PETRA_FLD_SET(regs->qdr.qdr_controller_reset_reg.qdrc_rst_n, 0x1, 38, exit);

  sal_msleep(SOC_PB_MGMT_INIT_TIMER_DELAY_MSEC);
  
  SOC_PETRA_FLD_GET(regs->qdr.qdr_controller_reset_reg.qdrc_rst_n, fld_val, 39, exit);

  SOC_PETRA_FLD_SET(regs->qdr.qdr_phy_reset_reg.qdio_reset, 0x1, 40, exit);
  sal_msleep(SOC_PB_MGMT_INIT_QDR_IO_RESET_DELAY_MSEC);

  /*
   *  QDR initialization - verify ready
   */
  poll_info.expected_value = 0x1;
  poll_info.busy_wait_nof_iters = SOC_PB_MGMT_INIT_QDR_BUSY_WAIT_ITERATIONS;
  poll_info.timer_nof_iters     = SOC_PB_MGMT_INIT_QDR_TIMER_ITERATIONS;
  poll_info.timer_delay_msec    = SOC_PB_MGMT_INIT_QDR_TIMER_DELAY_MSEC;

  res = soc_petra_status_fld_poll_unsafe(
          unit,
          SOC_PETRA_REG_DB_ACC_REF(regs->qdr.qdr_init_status_reg.ready),
          SOC_PETRA_DEFAULT_INSTANCE,
          &poll_info,
          &poll_success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

  if (poll_success == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_MGMT_QDR_NOT_READY_ERR, 42, exit);
  }

  /*
   *  QDR initialization - verify QDRC lock
   */
  res = soc_petra_status_fld_poll_unsafe(
          unit,
          SOC_PETRA_REG_DB_ACC_REF(regs->qdr.qdr_reply_lock_reg.reply_lock),
          SOC_PETRA_DEFAULT_INSTANCE,
          &poll_info,
          &poll_success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  if (poll_success == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_MGMT_QDRC_NOT_LOCKED_ERR, 52, exit);
  }

  /*
   *  QDR initialization - verify training fail is down
   */
  poll_info.expected_value = 0x0;
  res = soc_petra_status_fld_poll_unsafe(
          unit,
          SOC_PETRA_REG_DB_ACC_REF(regs->qdr.qdr_training_fail_reg.fail),
          SOC_PETRA_DEFAULT_INSTANCE,
          &poll_info,
          &poll_success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  if (poll_success == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_MGMT_QDR_NOT_READY_ERR, 62, exit);
  }

  soc_petra_sw_db_qdr_size_set(unit, hw_adjust->qdr_size_mbit);
  soc_petra_sw_db_qdr_protection_type_set(unit, hw_adjust->protection_type);
    
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mgmt_hw_adjust_qdr()", 0, 0);
}

STATIC uint32
  soc_pb_mgmt_hw_adjust_serdes(
    SOC_SAND_IN int            unit,
    SOC_SAND_IN SOC_PETRA_HW_ADJ_SERDES*  hw_adjust
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_HW_ADJUST_SERDES);

  SOC_SAND_CHECK_NULL_INPUT(hw_adjust);

  res = soc_petra_srd_all_set_unsafe(
          unit,
          &(hw_adjust->conf)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mgmt_hw_adjust_serdes()", 0, 0);
}

STATIC uint32
  soc_pb_mgmt_hw_adjust_fabric(
    SOC_SAND_IN int             unit,
    SOC_SAND_IN SOC_PETRA_HW_ADJ_FABRIC*  hw_adjust
  )
{
  uint32
    fld_val = 0,
    res;
  uint8
    is_300mhz,
    is_fabric,
    is_fap20_in_system,
    is_fap21_in_system,
    is_fap2x_in_system,
    is_fe200_fabric,
    is_variable_cell_size_enable,
    is_egr_mc_16k_groups_enable;
  SOC_PETRA_FABRIC_CELL_FORMAT
    cell_format;
  uint32
    reg_idx;
  uint8
    is_mesh,
    is_single_cntxt;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_HW_ADJUST_FABRIC);

  SOC_SAND_CHECK_NULL_INPUT(hw_adjust);

  regs = soc_petra_regs();

  soc_petra_PETRA_FABRIC_CELL_FORMAT_clear(&cell_format);

  is_fap20_in_system = soc_petra_sw_db_is_fap20_in_system_get(unit);
  is_fap21_in_system = soc_petra_sw_db_is_fap21_in_system_get(unit);
  is_fap2x_in_system = SOC_SAND_NUM2BOOL(is_fap20_in_system || is_fap21_in_system);
  is_fe200_fabric = soc_petra_sw_db_is_fe200_fabric_get(unit);
  is_variable_cell_size_enable = soc_petra_sw_db_is_fabric_variable_cell_size_get(unit);

  is_fabric = SOC_SAND_NUM2BOOL(
                 (hw_adjust->connect_mode == SOC_PETRA_FABRIC_CONNECT_MODE_FE) ||
                 (hw_adjust->connect_mode == SOC_PETRA_FABRIC_CONNECT_MODE_MULT_STAGE_FE)
                );

  is_300mhz = SOC_SAND_NUM2BOOL(soc_petra_chip_mega_ticks_per_sec_get(unit) == SOC_PETRA_MGMT_CORE_CLK_RATE_300MHZ);

  cell_format.segmentation_enable = soc_petra_sw_db_is_fabric_packet_segmentation_get(unit);
  cell_format.variable_cell_size_enable = is_variable_cell_size_enable;

  is_egr_mc_16k_groups_enable = soc_petra_sw_db_egr_mc_16k_groups_enable_get(unit);

  if ( (is_fe200_fabric) && (is_variable_cell_size_enable) )
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_CELL_VAR_SIZE_IN_FE200_SYSTEM_ERR, 6, exit);
  }

  if ( (is_fap2x_in_system) && (is_variable_cell_size_enable) )
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_CELL_VARIABLE_IN_FAP20_SYSTEM_ERR, 7, exit);
  }

  if (
      (hw_adjust->connect_mode == SOC_PETRA_FABRIC_CONNECT_MODE_BACK2BACK) ||
      (hw_adjust->connect_mode == SOC_PETRA_FABRIC_CONNECT_MODE_MESH)
     )
  {
    if (cell_format.segmentation_enable == FALSE)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_FULL_PCKT_MODE_IN_MESH_CONF_ERR, 8, exit);
    }
  }

  /*
   *  Cell Format
   */
  res = soc_petra_fabric_cell_format_set_unsafe(
          unit,
          &cell_format
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   *  EGQ - maximum fragment number
   *  Depends on fixed/variable cell size configuration
   */
  fld_val = (is_variable_cell_size_enable == TRUE)? \
    SOC_PB_MGMT_INIT_EGQ_MAX_FRG_VAR:SOC_PB_MGMT_INIT_EGQ_MAX_FRG_FIX;

  SOC_PETRA_FLD_SET(regs->egq.maximum_fragment_number_reg.max_frg_num, fld_val, 15, exit);

  res = soc_petra_fabric_connect_mode_set_unsafe(
    unit,
    hw_adjust->connect_mode
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

  if (is_300mhz && is_fabric && !is_fap2x_in_system)
  {
    SOC_PETRA_FLD_SET(regs->mesh_topology.mesh_config_1_reg.mesh_config_1_reg, 0x2, 27, exit);
    SOC_PETRA_FLD_SET(regs->mesh_topology.mesh_config_2_reg.mesh_config_2_reg, 0x1005, 28, exit);
  }

  if (is_egr_mc_16k_groups_enable == TRUE)
  {
    SOC_PETRA_FLD_SET(regs->eci.general_controls_reg.en_16k_mul, 0x1, 50, exit);
  }

  res = soc_petra_fabric_is_mesh(
      unit,
      &is_mesh,
      &is_single_cntxt
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  
  if (is_single_cntxt)
  {
  for (reg_idx = 0; reg_idx < 3; ++reg_idx)
  {
    SOC_PETRA_FLD_SET(regs->ipt.drambuffer_pointer_queue_threshold_reg[reg_idx].dpq_th_hp, 0x200, 50 + reg_idx, exit);
    SOC_PETRA_FLD_SET(regs->ipt.drambuffer_pointer_queue_threshold_reg[reg_idx].dpq_th_lp, 0x100, 50 + reg_idx, exit);
  }
  for (reg_idx = 3; reg_idx < SOC_PETRA_REGS_DBUFF_PTR_Q_THRESH_NOF_REGS; ++reg_idx)
  {
    SOC_PETRA_REG_SET(regs->ipt.drambuffer_pointer_queue_threshold_reg[reg_idx], 0, 50 + reg_idx, exit);
  }
    SOC_PETRA_FLD_SET(regs->ipt.drambuffer_pointer_queue_size0_1_reg.dpq_size0, 0x200, 60, exit);
    SOC_PETRA_FLD_SET(regs->ipt.drambuffer_pointer_queue_size0_1_reg.dpq_size1, 0x200, 61, exit);
    SOC_PETRA_FLD_SET(regs->ipt.drambuffer_pointer_queue_size2_3_reg.dpq_size2, 0x200, 62, exit);
    SOC_PETRA_FLD_SET(regs->ipt.drambuffer_pointer_queue_size2_3_reg.dpq_size3, 0x200, 63, exit);
    SOC_PETRA_FLD_SET(regs->ipt.drambuffer_pointer_queue_size4_5_reg.dpq_size4, 0x200, 64, exit);
    SOC_PETRA_FLD_SET(regs->ipt.drambuffer_pointer_queue_size4_5_reg.dpq_size5, 0x200, 65, exit);
    SOC_PETRA_REG_SET(regs->ipt.drambuffer_pointer_queue_size6_7_reg, 0, 68, exit);
    SOC_PETRA_REG_SET(regs->ipt.drambuffer_pointer_queue_size8_9_reg, 0, 69, exit);
    SOC_PETRA_REG_SET(regs->ipt.drambuffer_pointer_queue_size10_11_reg, 0, 70, exit);
    SOC_PETRA_REG_SET(regs->ipt.drambuffer_pointer_queue_size12_13_reg, 0, 71, exit);
    SOC_PETRA_REG_SET(regs->ipt.drambuffer_pointer_queue_size14_15_reg, 0, 72, exit);
    SOC_PETRA_FLD_SET(regs->ipt.drambuffer_pointer_queue_start_address0_1_reg.dpq_start0, 0, 80, exit);
    SOC_PETRA_FLD_SET(regs->ipt.drambuffer_pointer_queue_start_address0_1_reg.dpq_start1, 0x2a0, 81, exit);
    SOC_PETRA_FLD_SET(regs->ipt.drambuffer_pointer_queue_start_address2_3_reg.dpq_start2, 0x540, 82, exit);
    SOC_PETRA_FLD_SET(regs->ipt.drambuffer_pointer_queue_start_address2_3_reg.dpq_start3, 0x7e0, 83, exit);
    SOC_PETRA_FLD_SET(regs->ipt.drambuffer_pointer_queue_start_address4_5_reg.dpq_start4, 0xa80, 84, exit);
    SOC_PETRA_FLD_SET(regs->ipt.drambuffer_pointer_queue_start_address4_5_reg.dpq_start5, 0xd20, 85, exit);
    SOC_PETRA_REG_SET(regs->ipt.drambuffer_pointer_queue_start_address6_7_reg, 0, 88, exit);
    SOC_PETRA_REG_SET(regs->ipt.drambuffer_pointer_queue_start_address8_9_reg, 0, 89, exit);
    SOC_PETRA_REG_SET(regs->ipt.drambuffer_pointer_queue_start_address10_11_reg, 0, 90, exit);
    SOC_PETRA_REG_SET(regs->ipt.drambuffer_pointer_queue_start_address12_13_reg, 0, 91, exit);
    SOC_PETRA_REG_SET(regs->ipt.drambuffer_pointer_queue_start_address14_15_reg, 0, 92, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mgmt_hw_adjust_fabric()", 0, 0);
}

/*
 *  Prepare for NIF configuration;
 *  some settings must be configured at early stages,
 *  e.g. the clock configuration should be prior
 *  to SerDes trimming
 */
STATIC uint32
  soc_pb_mgmt_hw_adjust_nif_prep(
    SOC_SAND_IN int     unit,
    SOC_SAND_IN SOC_PB_HW_ADJ_NIF *hw_adjust,
    SOC_SAND_IN  uint8    silent
  )
{
  uint32
    mdio_freq,
    res = SOC_SAND_OK;
  uint8
    is_malgb_enabled,
    is_sgmii_not_qsgmii;
  uint32
    mal_id = 0,
    nif_id = 0,
    idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_HW_ADJUST_NIF);
  SOC_SAND_CHECK_NULL_INPUT(hw_adjust);

  mdio_freq = (hw_adjust->mdio_nof_entries == 0)?SOC_SAND_INTERN_VAL_INVALID_32:hw_adjust->mdio.clk_freq_khz;
  is_malgb_enabled = (hw_adjust->synce_nof_entries == 0)?FALSE:hw_adjust->synce.is_malg_b_enabled;

  res = soc_pb_nif_init(
          unit,
          mdio_freq,
          is_malgb_enabled
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  res = soc_pb_nif_fatp_mode_set_unsafe(
          unit,
          &(hw_adjust->fatp_mode)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 7, exit);

  for (idx = 0; idx < hw_adjust->ilkn_nof_entries; idx++)
  {
    res = soc_pb_nif_ilkn_clk_config(
            unit,
            hw_adjust->ilkn[idx].ilkn_ndx,
            SOC_PETRA_CONNECTION_DIRECTION_BOTH,
            &(hw_adjust->ilkn[idx].conf)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

  for (idx = 0; idx < hw_adjust->gmii_nof_entries; idx++)
  {
    /*
     *  Indexing
     */
    is_sgmii_not_qsgmii = SOC_SAND_NUM2BOOL(SOC_PB_NIF_IS_TYPE_ID(SGMII, hw_adjust->gmii[idx].nif_ndx));
    nif_id = soc_pb_nif2intern_id(hw_adjust->gmii[idx].nif_ndx);
    mal_id = SOC_PB_NIF2MAL_GLBL_ID(nif_id);
      
    res = soc_pb_nif_gmii_clk_config(
            unit,
            mal_id,
            is_sgmii_not_qsgmii,
            hw_adjust->mal[mal_id].conf.topology.is_qsgmii_alt
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mgmt_hw_adjust_nif_prep()", 0, 0);
}


STATIC uint32
  soc_pb_mgmt_hw_adjust_nif(
    SOC_SAND_IN int     unit,
    SOC_SAND_IN SOC_PB_HW_ADJ_NIF *hw_adjust,
    SOC_SAND_IN  uint8    silent
  )
{
  uint32
    res;
  uint32
    idx;
   SOC_PB_NIF_ELK_INFO
    elk;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_HW_ADJUST_NIF);
  SOC_SAND_CHECK_NULL_INPUT(hw_adjust);

  if (hw_adjust->mal_nof_entries != 0)
  {
    SOC_PB_INIT_PRINT_INTERNAL_LVL2("Mal basic configuration");
  }

  for (idx = 0; idx < hw_adjust->mal_nof_entries; idx++)
  {
    res = soc_pb_nif_mal_basic_conf_set_unsafe(
            unit,
            hw_adjust->mal[idx].mal_ndx,
            &(hw_adjust->mal[idx].conf)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

  if (hw_adjust->gmii_nof_entries != 0)
  {
    SOC_PB_INIT_PRINT_INTERNAL_LVL2("GMII");
  }
  for (idx = 0; idx < hw_adjust->gmii_nof_entries; idx++)
  {
    res = soc_pb_nif_gmii_conf_set_unsafe(
            unit,
            hw_adjust->gmii[idx].nif_ndx,
            &(hw_adjust->gmii[idx].conf)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

  if (hw_adjust->spaui_nof_entries != 0)
  {
    SOC_PB_INIT_PRINT_INTERNAL_LVL2("SPAUI extensions");
  }
  for (idx = 0; idx < hw_adjust->spaui_nof_entries; idx++)
  {
    res = soc_pb_nif_spaui_conf_set_unsafe(
            unit,
            hw_adjust->spaui[idx].if_ndx,
            SOC_PETRA_CONNECTION_DIRECTION_BOTH,
            &(hw_adjust->spaui[idx].conf)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

  if (hw_adjust->ilkn_nof_entries != 0)
  {
    SOC_PB_INIT_PRINT_INTERNAL_LVL2("Interlaken");
  }
  for (idx = 0; idx < hw_adjust->ilkn_nof_entries; idx++)
  {
    res = soc_pb_nif_ilkn_set_unsafe(
            unit,
            hw_adjust->ilkn[idx].ilkn_ndx,
            SOC_PETRA_CONNECTION_DIRECTION_BOTH,
            &(hw_adjust->ilkn[idx].conf)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  }

  /*
   *  Fat Pipe
   */

  if (hw_adjust->fatp_nof_entries != 0)
  {
    SOC_PB_INIT_PRINT_INTERNAL_LVL2("Fat Pipe");
  }

  for (idx = 0; idx < hw_adjust->fatp_nof_entries; idx++)
  {
    res = soc_pb_nif_fatp_verify(
            unit,
            hw_adjust->fatp[idx].fatp_ndx,
            &(hw_adjust->fatp[idx].conf)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  
    res = soc_pb_nif_fatp_set_unsafe(
            unit,
            hw_adjust->fatp[idx].fatp_ndx,
            &(hw_adjust->fatp[idx].conf)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 72, exit);
  }

  /*
   *  SynceE
   */
  if(hw_adjust->synce_nof_entries != 0)
  {
    SOC_PB_INIT_PRINT_INTERNAL_LVL2(" - SyncE");
    
    res = soc_pb_nif_synce_mode_set(
            unit,
            hw_adjust->synce.mode
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

    for (idx = SOC_PB_NIF_SYNCE_CLK_ID_0; idx < SOC_PB_NIF_NOF_SYNCE_CLK_IDS; idx++)
    {
      if (SOC_SAND_IS_VAL_IN_RANGE(idx, SOC_PB_NIF_SYNCE_CLK_ID_2, SOC_PB_NIF_SYNCE_CLK_ID_3_OR_VALID) && !(hw_adjust->synce.is_malg_b_enabled))
      {
        if (hw_adjust->synce.conf[idx].enable)
        {
          SOC_SAND_SET_ERROR_CODE(SOC_PB_NIF_SYNCE_MALGB_CLK_WHEN_DISABLED_ERR, 81, exit);
        }
      }
      if (SOC_PB_NIF_SYNCE_IS_CLK_OF_MODE(hw_adjust->synce.mode, idx))
      {
        res = soc_pb_nif_synce_clk_verify(
                unit,
                idx,
                &(hw_adjust->synce.conf[idx])
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 82, exit);

        res = soc_pb_nif_synce_clk_set_unsafe(
                unit,
                idx,
                &(hw_adjust->synce.conf[idx])
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 84, exit);
      }
    }
  }

  /*
   *    ELK
   */
  SOC_PB_NIF_ELK_INFO_clear(&elk);
  if (hw_adjust->elk_nof_entries != 0) {
      elk.enable = hw_adjust->elk.conf.enable;
      elk.mal_id = hw_adjust->elk.conf.mal_id;
  }
  else
  {
      elk.enable = FALSE;
      elk.mal_id = 0;
  }
  res = soc_pb_nif_elk_set_unsafe(
                unit,
                &elk
          );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mgmt_hw_adjust_nif()", 0, 0);
}

/*********************************************************************
*     Initialize a sub-set of the HW interfaces of the device.
*     The function might be called more than once, each time
*     with different fields, indicated to be written to the
*     device
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_mgmt_hw_interfaces_set_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  SOC_PB_HW_ADJUSTMENTS*       hw_adjust,
    SOC_SAND_IN  uint8                silent
  )
{
  uint32
    res;
  uint32
    stage_id = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_HW_INTERFACES_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(hw_adjust);

  if (hw_adjust->core_freq.enable)
  {
    if (soc_pb_sw_db_core_freq_self_freq_get(unit) == SOC_SAND_INTERN_VAL_INVALID_32)
    {
      soc_petra_chip_kilo_ticks_per_sec_set(
        unit,
        hw_adjust->core_freq.frequency * 1000
      );
    }
  }

  if(hw_adjust->dram.enable)
  {
    SOC_PB_INIT_PRINT_INTERNAL_ADVANCE("DDR");
    res = soc_pb_mgmt_hw_adjust_ddr(
            unit,
            &(hw_adjust->dram)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  if(hw_adjust->qdr.enable)
  {
    switch (hw_adjust->qdr.qdr_type)
    {
    case SOC_PB_HW_QDR_TYPE_QDR:
      SOC_PB_INIT_PRINT_INTERNAL_ADVANCE("QDR");
      break;
    case SOC_PB_HW_QDR_TYPE_QDR2P:
      SOC_PB_INIT_PRINT_INTERNAL_ADVANCE("QDR2+");
      break;
    case SOC_PB_HW_QDR_TYPE_QDR3:
      SOC_PB_INIT_PRINT_INTERNAL_ADVANCE("QDR3");
      break;
    default:
      break;
    }
    res = soc_pb_mgmt_hw_adjust_qdr(
            unit,
            &(hw_adjust->qdr)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

  res = soc_pb_mgmt_hw_adjust_nif_prep(
          unit,
          &(hw_adjust->nif),
          silent
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

  if(hw_adjust->serdes.enable)
  {
    SOC_PB_INIT_PRINT_INTERNAL_ADVANCE("SerDes");
    res = soc_pb_mgmt_hw_adjust_serdes(
            unit,
            &(hw_adjust->serdes)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

  if(hw_adjust->fabric.enable)
  {
    SOC_PB_INIT_PRINT_INTERNAL_ADVANCE("Fabric");
    res = soc_pb_mgmt_hw_adjust_fabric(
            unit,
            &(hw_adjust->fabric)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

  SOC_PB_INIT_PRINT_INTERNAL_ADVANCE("NIF configuration");
  res = soc_pb_mgmt_hw_adjust_nif(
          unit,
          &(hw_adjust->nif),
          silent
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mgmt_hw_interfaces_set_unsafe()", 0, 0);
}

/*********************************************************************
*     Initialize a sub-set of the HW interfaces of the device.
*     The function might be called more than once, each time
*     with different fields, indicated to be written to the
*     device
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_mgmt_hw_interfaces_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PB_HW_ADJUSTMENTS      *hw_adjust
  )
{
  uint32
    res;
  uint32
    idx,
    dram_count = 0;
  SOC_PB_MGMT_OPERATION_MODE
    op_mode;
  SOC_PETRA_HW_PLL_PARAMS
    dram_pll_dflt_old;
  SOC_PB_HW_ADJ_DRAM_PLL
    dram_pll_dflt_new;
  uint8
    is_pll_conf_not_set;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_HW_INTERFACES_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(hw_adjust);
  SOC_SAND_MAGIC_NUM_VERIFY(hw_adjust);

  if(hw_adjust->dram.enable)
  {
    soc_petra_PETRA_HW_PLL_PARAMS_clear(&dram_pll_dflt_old);
    SOC_PB_HW_ADJ_DRAM_PLL_clear(&dram_pll_dflt_new);

    SOC_SAND_ERR_IF_OUT_OF_RANGE(
      hw_adjust->dram.conf_mode, SOC_PETRA_HW_DRAM_CONF_MODE_MIN, SOC_PETRA_HW_DRAM_CONF_MODE_MAX,
      SOC_SAND_ERR, 5, exit
    );

    if (hw_adjust->dram.conf_mode == SOC_PETRA_HW_DRAM_CONF_MODE_BUFFER)
    {
      SOC_SAND_ERR_IF_ABOVE_MAX(
        hw_adjust->dram.dram_conf.buffer_mode.buff_len, SOC_PETRA_HW_DRAM_CONF_SIZE_MAX-1,
        SOC_PETRA_HW_DRAM_CONF_LEN_OUT_OF_RANGE_ERR, 10, exit
      );
    }
    if (hw_adjust->dram.conf_mode == SOC_PETRA_HW_DRAM_CONF_MODE_PARAMS)
    {
      res = soc_petra_dram_info_verify(
              unit,
              hw_adjust->dram.dram_conf.params_mode.dram_freq,
              hw_adjust->dram.dram_type,
              &hw_adjust->dram.dram_conf.params_mode.params
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);
    }
    for (idx = 0; idx < SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max); idx ++ )
    {
      if (hw_adjust->dram.is_valid[idx])
      {
        dram_count++;
      }
    }

    if ((dram_count != 2) && (dram_count != 3) && (dram_count != 4) && (dram_count != 6))
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_HW_INVALID_NUMBER_OF_DRAM_INTERFACES_ERR, 15, exit);
    }
    else if ((dram_count == 2) && ((!hw_adjust->dram.is_valid[0]) || (!hw_adjust->dram.is_valid[3])))
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_HW_INVALID_NUMBER_OF_DRAM_INTERFACES_ERR, 16, exit);
    }
    else if ((dram_count == 3) && ((!hw_adjust->dram.is_valid[0]) || (!hw_adjust->dram.is_valid[1]) || (!hw_adjust->dram.is_valid[2])))
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_HW_INVALID_NUMBER_OF_DRAM_INTERFACES_ERR, 17, exit);
    }
    else if ((dram_count == 4) && ((!hw_adjust->dram.is_valid[0]) || (!hw_adjust->dram.is_valid[1]) || (!hw_adjust->dram.is_valid[3]) || (!hw_adjust->dram.is_valid[4])))
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_HW_INVALID_NUMBER_OF_DRAM_INTERFACES_ERR, 18, exit);
    }
    else if ((dram_count == 6) && ((!hw_adjust->dram.is_valid[0]) || (!hw_adjust->dram.is_valid[1]) || (!hw_adjust->dram.is_valid[2]) || (!hw_adjust->dram.is_valid[3]) || (!hw_adjust->dram.is_valid[4]) || (!hw_adjust->dram.is_valid[5])))
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_HW_INVALID_NUMBER_OF_DRAM_INTERFACES_ERR, 19, exit);
    }

    if (
        (hw_adjust->dram.nof_banks ==  SOC_PETRA_DRAM_NUM_BANKS_4) &&
        (hw_adjust->dram.dram_type != SOC_PETRA_DRAM_TYPE_DDR2)
       )
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_HW_INVALID_NOF_BANKS_FOR_DRAM_TYPE_ERR, 18, exit);
    }

    if (
        (hw_adjust->dram.nof_banks !=  SOC_PETRA_DRAM_NUM_BANKS_4) &&
        (hw_adjust->dram.nof_banks !=  SOC_PETRA_DRAM_NUM_BANKS_8)
       )
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_HW_INVALID_NOF_BANKS_ERR, 20, exit);
    }

    SOC_SAND_ERR_IF_OUT_OF_RANGE(
      hw_adjust->dram.nof_columns, SOC_PETRA_DRAM_NUM_COLUMNS_256, SOC_PETRA_DRAM_NUM_COLUMNS_8192,
      SOC_PETRA_HW_NOF_COLUMNS_OUT_OF_RANGE_ERR, 23, exit
    );

    if (SOC_PB_REV_ABOVE_A0)
    {
      SOC_PETRA_COMP(&dram_pll_dflt_new, &(hw_adjust->dram_pll), SOC_PB_HW_ADJ_DRAM_PLL, 1, is_pll_conf_not_set);
      SOC_PB_HW_ADJ_DRAM_PLL_verify(&(hw_adjust->dram_pll));
    }
    else
    {
      SOC_PETRA_COMP(&dram_pll_dflt_old, &(hw_adjust->dram.pll_conf), SOC_PETRA_HW_PLL_PARAMS, 1, is_pll_conf_not_set);
    }

    if (is_pll_conf_not_set)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_DRAM_PLL_CONF_NOT_SET_ERR, 24, exit);
    }
  }

  if(hw_adjust->qdr.enable)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      hw_adjust->qdr.protection_type, SOC_PETRA_HW_NOF_QDR_PROTECT_TYPES-1,
      SOC_PETRA_HW_QDR_PROTECT_TYPE_INVALID_ERR, 25, exit
    );

    SOC_SAND_ERR_IF_ABOVE_MAX(
      hw_adjust->qdr.qdr_size_mbit, SOC_PETRA_HW_NOF_QDR_SIZE_MBITS-1,
      SOC_PETRA_HW_QDR_SIZE_OUT_OF_RANGE_ERR, 27, exit
    );
  }

  if(hw_adjust->serdes.enable)
  {
    res = soc_petra_srd_all_verify(
            unit,
            &(hw_adjust->serdes.conf)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

  if(hw_adjust->fabric.enable)
  {
    res = soc_pb_mgmt_operation_mode_get_unsafe(
             unit,
             &op_mode
           );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    res = soc_petra_fabric_connect_mode_verify(
            unit,
            hw_adjust->fabric.connect_mode
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 88, exit);

    if (op_mode.egr_mc_16k_groups_enable == TRUE)
    {
      if (
            (op_mode.is_fe200_fabric == TRUE) ||
            (hw_adjust->fabric.connect_mode == SOC_PETRA_FABRIC_CONNECT_MODE_MESH) ||
            (hw_adjust->fabric.connect_mode == SOC_PETRA_FABRIC_CONNECT_MODE_BACK2BACK) ||
            (op_mode.is_fap20_in_system == TRUE) ||
            (op_mode.is_fap21_in_system == TRUE)
         )
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_EGR_MC_16K_ENABLE_INCOMPATIBLE_ERR, 92, exit);
      }
    }
  }

  res = soc_petra_stat_if_info_verify(
          unit,
          &(hw_adjust->stat_if.if_conf)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 125, exit);

  if (hw_adjust->stat_if.if_conf.enable == TRUE)
  {
    res = soc_pb_stat_if_report_set_verify(
            unit,
            &(hw_adjust->stat_if.rep_conf)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);
  }
  
  for (idx = 0; idx < hw_adjust->nif.mal_nof_entries; idx++)
  {
    res = soc_pb_nif_mal_basic_conf_verify(
            unit,
            hw_adjust->nif.mal[idx].mal_ndx,
            &(hw_adjust->nif.mal[idx].conf)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);
  }

  for (idx = 0; idx < hw_adjust->nif.ilkn_nof_entries; idx++)
  {
    res = soc_pb_nif_ilkn_verify(
            unit,
            hw_adjust->nif.ilkn[idx].ilkn_ndx,
            SOC_PETRA_CONNECTION_DIRECTION_BOTH,
            &(hw_adjust->nif.ilkn[idx].conf)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);
  }

  for (idx = 0; idx < hw_adjust->nif.gmii_nof_entries; idx++)
  {
    res = soc_pb_nif_gmii_conf_verify(
            unit,
            hw_adjust->nif.gmii[idx].nif_ndx,
            &(hw_adjust->nif.gmii[idx].conf)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 160, exit);
  }

  for (idx = 0; idx < hw_adjust->nif.spaui_nof_entries; idx++)
  {
    res = soc_pb_nif_spaui_conf_verify(
            unit,
            hw_adjust->nif.spaui[idx].if_ndx,
            SOC_PETRA_CONNECTION_DIRECTION_BOTH,
            &(hw_adjust->nif.spaui[idx].conf)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 170, exit);
  }

  res = soc_pb_nif_fatp_mode_verify(
          unit,
          &(hw_adjust->nif.fatp_mode)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);

  if (
      (hw_adjust->nif.fatp_mode.mode == SOC_PB_NIF_FATP_MODE_DISABLED) &&
      (hw_adjust->nif.fatp_nof_entries != 0)
     )
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_NIF_FATP_DISABLED_BUT_SET_ERR, 182, exit);
  }

  if (hw_adjust->nif.mdio_nof_entries > 0)
  {
    SOC_SAND_ERR_IF_OUT_OF_RANGE(hw_adjust->nif.mdio.clk_freq_khz, SOC_PB_MGMT_MDIO_CLK_FREQ_MIN, SOC_PB_MGMT_MDIO_CLK_FREQ_MAX, SOC_PB_MDIO_CLK_FREQ_ERR, 21, exit);
  }

  if(hw_adjust->nif.synce_nof_entries != 0)
  {
    res = soc_pb_nif_synce_mode_verify(
            unit,
            hw_adjust->nif.synce.mode
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 190, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mgmt_hw_interfaces_verify()", 0, 0);
}


/*********************************************************************
*     Initialize of Soc_petra internal blocks.
*     Details: in the H file. (search for prototype)
*********************************************************************/

STATIC uint32
  soc_pb_mgmt_blocks_init_unsafe(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    fld_val,
    res;
  SOC_PETRA_POLL_INFO
    poll_info;
  uint8
    poll_success;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_BLOCKS_INIT_UNSAFE);

  regs = soc_petra_regs();

  soc_petra_PETRA_POLL_INFO_clear(&poll_info);

  SOC_PETRA_FLD_GET(regs->eci.test_mux_config_reg.core_lock, fld_val, 5, exit);
  if (fld_val != 0x1)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_INIT_CORE_PLL_NOT_LOCKED_ERR, 15, exit);
  }

  /*
   *  Out-of-reset all internal blocks
   */
  SOC_PETRA_REG_SET(regs->eci.soc_petra_soft_reset_reg, 0x0 , 20, exit);
  sal_msleep(SOC_PB_MGMT_INIT_TIMER_DELAY_MSEC);

  /*
   *  EGQ initialization - verify init is finished
   */
  poll_info.expected_value = 0x0;
  poll_info.busy_wait_nof_iters = SOC_PB_MGMT_INIT_EGQ_BUSY_WAIT_ITERATIONS;
  poll_info.timer_nof_iters     = SOC_PB_MGMT_INIT_EGQ_TIMER_ITERATIONS;
  poll_info.timer_delay_msec    = SOC_PB_MGMT_INIT_EGQ_TIMER_DELAY_MSEC;

  res = soc_petra_status_fld_poll_unsafe(
          unit,
          SOC_PETRA_REG_DB_ACC_REF(regs->egq.egqblock_init_status_reg.egqblock_init),
          SOC_PETRA_DEFAULT_INSTANCE,
          &poll_info,
          &poll_success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if (poll_success == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MGMT_EGQ_INIT_FAILS_ERR, 35, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mgmt_blocks_init_unsafe()", 0, 0);
}

/*********************************************************************
*     Initialize the device, including:1. Prevent all the
*     control cells. 2. Initialize the device tables and
*     registers to default values. 3. Initialize
*     board-specific hardware interfaces according to
*     configurable information, as passed in hw_adjust. 4.
*     Perform basic device initialization. The configuration
*     can be enabled/disabled as passed in enable_info.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_mgmt_init_sequence_phase1_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PB_HW_ADJUSTMENTS         *hw_adjust,
    SOC_SAND_IN  SOC_PB_INIT_BASIC_CONF        *basic_conf,
    SOC_SAND_IN  SOC_PB_INIT_PORTS             *fap_ports,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    nof_dram_buffs = 0,
    res;
  uint32
    idx,
    stage_id = 0;
  SOC_PB_INIT_DBUFFS_BDRY
    dbuffs_bdries;
  SOC_SAND_SUCCESS_FAILURE
    success;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_INIT_SEQUENCE_PHASE1_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(hw_adjust);
  SOC_SAND_CHECK_NULL_INPUT(basic_conf);
  SOC_SAND_CHECK_NULL_INPUT(fap_ports);

  if (!silent)
  {
    soc_sand_os_printf(
      "\n\r"
      "   Phase 1 initialization: device %u"
      "\n\r",
      unit
    );
  }
  /************************************************************************/
  /* Set Core clock frequency                                             */
  /************************************************************************/
  if (hw_adjust->core_freq.enable)
  {
    if (soc_pb_sw_db_core_freq_self_freq_get(unit) == SOC_SAND_INTERN_VAL_INVALID_32)
    {
      soc_petra_chip_kilo_ticks_per_sec_set(
        unit,
        hw_adjust->core_freq.frequency * 1000
        );
    }
  }

  /************************************************************************/
  /* Prepare internal data                                                */
  /************************************************************************/

  if (hw_adjust->dram.enable == TRUE)
  {
    res = soc_pb_init_dram_nof_buffs_calc(
            hw_adjust->dram.dram_size_total_mbyte,
            hw_adjust->dram.dbuff_size,
            hw_adjust->qdr.protection_type,
            hw_adjust->qdr.qdr_size_mbit,
            &nof_dram_buffs
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

    res = soc_pb_init_dram_buff_boudaries_calc(
            nof_dram_buffs,
            hw_adjust->dram.dbuff_size,
            &dbuffs_bdries
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

  /************************************************************************/
  /* Initialize basic configuration (must be before per-block Out-Of-Reset*/
  /************************************************************************/
  SOC_PB_INIT_PRINT_ADVANCE("Initialize internal blocks, in-reset", 1);
  res = soc_pb_mgmt_init_before_blocks_oor(
          unit,
          &dbuffs_bdries,
          hw_adjust
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  /************************************************************************/
  /* Out-of-reset Soc_petra internal blocks                                   */
  /************************************************************************/
  SOC_PB_INIT_PRINT_ADVANCE("Take internal blocks out-of-reset", 1);
  res = soc_pb_mgmt_blocks_init_unsafe(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /************************************************************************/
  /* Stop all traffic                                                     */
  /************************************************************************/
  SOC_PB_INIT_PRINT_ADVANCE("Stop traffic", 1);
  res = soc_petra_mgmt_enable_traffic_set_unsafe(
          unit,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  /************************************************************************/
  /* Stop Control Cells                                                   */
  /*                                                                      */
  /* Disable the device from sending control cells                        */
  /* prior to FAP-ID setting).                                            */
  /************************************************************************/
  SOC_PB_INIT_PRINT_ADVANCE("Stop control cells", 1)
  res = soc_petra_mgmt_all_ctrl_cells_enable_set_unsafe(
          unit,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  /************************************************************************/
  /* Initialize basic configuration (based on pre-OOR)                    */
  /************************************************************************/
  SOC_PB_INIT_PRINT_ADVANCE("Finalize internal blocks initialization", 1);
  res = soc_pb_mgmt_init_after_blocks_oor(
          unit,
          hw_adjust,
          &dbuffs_bdries
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  /************************************************************************/
  /* Initialize all tables                                                */
  /*                                                                      */
  /* Most tables are zeroed. Some - initialized to non-zero default       */
  /************************************************************************/
  SOC_PB_INIT_PRINT_ADVANCE("Initialize tables defaults", 1);
  res = soc_pb_mgmt_tbls_init(
          unit,
          silent
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_pb_pp_mgmt_tbls_init_unsafe(
          unit,
          silent
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 42, exit);

  /************************************************************************/
  /* Set registers not covered in any functional module, */
  /* with default values different from hardware defaults                 */
  /************************************************************************/
  SOC_PB_INIT_PRINT_ADVANCE("Initialize registers defaults", 1);
  res = soc_pb_mgmt_hw_set_defaults(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  /************************************************************************/
  /* Set board-related configuration (hardware adjustments)               */
  /************************************************************************/
  if (hw_adjust != NULL)
  {
    SOC_PB_INIT_PRINT_ADVANCE("Configure HW interfaces", 1);
    res = soc_pb_mgmt_hw_interfaces_set_unsafe(
            unit,
            hw_adjust,
            silent
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  }
  else
  {
    SOC_PB_INIT_PRINT_ADVANCE("SKIPPING soc_pb_mgmt_hw_interfaces_set", 1);
  }

  /************************************************************************/
  /* Set basic configuration                                              */
  /************************************************************************/
  if (basic_conf != NULL)
  {
    if (basic_conf->credit_worth_enable)
    {
      SOC_PB_INIT_PRINT_ADVANCE("Set credit worth", 1);
      res = soc_petra_mgmt_credit_worth_set_unsafe(
              unit,
              basic_conf->credit_worth
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
    }

    if (basic_conf->stag_enable)
    {
      SOC_PB_INIT_PRINT_ADVANCE("Set statistics tag", 1);
      res = soc_pb_itm_stag_set_unsafe(
              unit,
              &(basic_conf->stag.encoding)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
    }
  }
  else
  {
    SOC_PB_INIT_PRINT_ADVANCE("SKIPPING basic configurations", 1);
  }

  /************************************************************************/
  /* Per functional module, perform initializations                       */
  /* covered by module's functionality.                                   */
  /************************************************************************/
  SOC_PB_INIT_PRINT_ADVANCE("Set default configuration", 1);
  res = soc_pb_mgmt_functional_init(
          unit,
          hw_adjust,
          silent
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

  /************************************************************************/
  /* Set FAP port configuration.                                       */
  /************************************************************************/
  if (fap_ports != NULL)
  {
    if (fap_ports->hdr_type_nof_entries != 0)
    {
      SOC_PB_INIT_PRINT_ADVANCE("Set port header type", 1);
      for (idx = 0; idx < fap_ports->hdr_type_nof_entries; idx++)
      {
        res = soc_petra_port_header_type_set_unsafe(
                unit,
                fap_ports->hdr_type[idx].port_ndx,
                SOC_PETRA_PORT_DIRECTION_INCOMING,
                fap_ports->hdr_type[idx].header_type_in
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

       
        res = soc_petra_port_header_type_set_unsafe(
                unit,
                fap_ports->hdr_type[idx].port_ndx,
                SOC_PETRA_PORT_DIRECTION_OUTGOING,
                fap_ports->hdr_type[idx].header_type_out
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);

		if (fap_ports->hdr_type[idx].header_type_out == SOC_PETRA_PORT_HEADER_TYPE_TM) {
			res = soc_petra_ports_otmh_extension_set_unsafe(
					unit,
					fap_ports->hdr_type[idx].port_ndx,				
					&fap_ports->otmh_ext_en[idx]
				  );
			SOC_SAND_CHECK_FUNC_RESULT(res, 122, exit);
		}
      }
    }

    if (fap_ports->if_map_nof_entries != 0)
    {
      SOC_PB_INIT_PRINT_ADVANCE("Set port to interface mapping", 1);
      for (idx = 0; idx < fap_ports->if_map_nof_entries; idx++)
      {
        if (
            (fap_ports->if_map[idx].conf.if_id == SOC_PETRA_IF_ID_CPU) &&
            (fap_ports->if_map[idx].port_ndx != SOC_PETRA_DEFAULT_CPU_PORT_ID)
           )
        {
          res = soc_petra_port_to_interface_map_set_unsafe(
                    unit,
                    fap_ports->if_map[idx].port_ndx,
                    SOC_PETRA_PORT_DIRECTION_OUTGOING,
                    &(fap_ports->if_map[idx].conf)
                  );
            SOC_SAND_CHECK_FUNC_RESULT(res, 125, exit);
        }
        else
        {
          res = soc_petra_port_to_interface_map_set_unsafe(
                  unit,
                  fap_ports->if_map[idx].port_ndx,
                  SOC_PETRA_PORT_DIRECTION_BOTH,
                  &(fap_ports->if_map[idx].conf)
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);
        }
      }
    }

    if (fap_ports->pp_port_nof_entries != 0)
    {
      SOC_PB_INIT_PRINT_ADVANCE("Set TM port profiles", 1);
      for (idx = 0; idx < fap_ports->pp_port_nof_entries; idx++)
      {
        res = soc_pb_port_pp_port_set_unsafe(
                unit,
                fap_ports->pp_port[idx].profile_ndx,
                &(fap_ports->pp_port[idx].conf),
                &success
               );
        SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);

        if (!SOC_SAND_SUCCESS2BOOL(success))
        {
          SOC_SAND_SET_ERROR_CODE(SOC_PB_INIT_TM_PROFILE_OUT_OF_RSRC_ERR, 135, exit);
        }
      }
    }

    if (fap_ports->tm2pp_port_map_nof_entries != 0)
    {
      SOC_PB_INIT_PRINT_ADVANCE("Set TM port to profile mapping", 1);
      for (idx = 0; idx < fap_ports->tm2pp_port_map_nof_entries; idx++)
      {
        /* Must be verified only after port header-type is set */
        res = soc_pb_port_to_pp_port_map_set_verify(
                unit,
                fap_ports->tm2pp_port_map[idx].port_ndx,
                SOC_PETRA_PORT_DIRECTION_BOTH,
                fap_ports->tm2pp_port_map[idx].pp_port
               );
        SOC_SAND_CHECK_FUNC_RESULT(res, 137, exit);

        res = soc_pb_port_to_pp_port_map_set_unsafe(
                unit,
                fap_ports->tm2pp_port_map[idx].port_ndx,
                SOC_PETRA_PORT_DIRECTION_BOTH,
                fap_ports->tm2pp_port_map[idx].pp_port
               );
        SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);
      }
    }

    if (fap_ports->egr_q_profile_map_nof_entries != 0)
    {
      SOC_PB_INIT_PRINT_ADVANCE("Set OFP to Egress Queue profile mapping", 1);
      for (idx = 0; idx < fap_ports->tm2pp_port_map_nof_entries; idx++)
      {
        res = soc_pb_egr_q_profile_map_set_unsafe(
                unit,
                fap_ports->egr_q_profile_map[idx].port_ndx,
                fap_ports->egr_q_profile_map[idx].conf
               );
        SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);
      }
    }
  } /* (fap_ports != NULL) */
  else
  {
    SOC_PB_INIT_PRINT_ADVANCE("SKIPPING FAP port configuration", 1);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mgmt_init_sequence_phase1_unsafe()", 0, 0);
}

uint32
  soc_pb_mgmt_init_sequence_phase1_verify(
    SOC_SAND_IN     int              unit,
    SOC_SAND_INOUT  SOC_PB_HW_ADJUSTMENTS   *hw_adjust,
    SOC_SAND_IN     SOC_PB_INIT_BASIC_CONF  *basic_conf,
    SOC_SAND_IN     SOC_PB_INIT_PORTS       *fap_ports
  )
{
  uint32
    res = 0;
  uint32
    idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_INIT_SEQUENCE_PHASE1_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(hw_adjust);
  SOC_SAND_MAGIC_NUM_VERIFY(hw_adjust);

#if !SOC_PB_INIT_DBG_PARTIAL_IF_CFG_EN
  /*
   *  The below parameters must be supplied for init.
   *  DRAM and QDR must be either both enabled, or
   *  both disabled (for Soc_petra-TDM in bypass mode)
   */
  if (
      (!hw_adjust->fabric.enable) ||
      (basic_conf == NULL) ||
      !(((hw_adjust->dram.enable)  && (hw_adjust->qdr.enable)) ||
        ((!hw_adjust->dram.enable) && (!hw_adjust->qdr.enable))
       )
     )
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MGMT_BASIC_CONF_NOT_SUPPLIED_ERR, 15, exit);
  }
#endif

  SOC_SAND_MAGIC_NUM_VERIFY(basic_conf);
  if (basic_conf->credit_worth_enable)
  {
    res = soc_petra_mgmt_credit_worth_verify(
            unit,
            basic_conf->credit_worth
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
#if !SOC_PB_INIT_DBG_PARTIAL_IF_CFG_EN
  else
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MGMT_BASIC_CONF_NOT_SUPPLIED_ERR, 22, exit);
  }
#endif

  if (hw_adjust->serdes.enable)
  {
    res = soc_petra_srd_all_verify(
            unit,
            &(hw_adjust->serdes.conf)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 26, exit);
  }

  if (hw_adjust->dram.enable)
  {
    if (hw_adjust->dram.conf_mode == SOC_PETRA_HW_DRAM_CONF_MODE_BUFFER)
    {
      SOC_SAND_ERR_IF_ABOVE_MAX(
        hw_adjust->dram.dram_conf.buffer_mode.buff_len, SOC_PETRA_HW_DRAM_CONF_SIZE_MAX-1,
        SOC_PETRA_HW_DRAM_CONF_LEN_OUT_OF_RANGE_ERR, 10, exit
        );
    }
    if (hw_adjust->dram.conf_mode == SOC_PETRA_HW_DRAM_CONF_MODE_PARAMS)
    {
      res = soc_petra_dram_info_verify(
              unit,
              hw_adjust->dram.dram_conf.params_mode.dram_freq,
              hw_adjust->dram.dram_type,
              &hw_adjust->dram.dram_conf.params_mode.params
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);
    }
  }
  
  for (idx = 0; idx < fap_ports->pp_port_nof_entries; idx++)
  {
    res = soc_pb_port_pp_port_set_verify(
            unit,
            fap_ports->pp_port[idx].profile_ndx,
            &(fap_ports->pp_port[idx].conf)
           );
    SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);
  }
 
  for (idx = 0; idx < fap_ports->egr_q_profile_map_nof_entries; idx++)
  {
    res = soc_pb_egr_q_profile_map_set_verify(
            unit,
            fap_ports->egr_q_profile_map[idx].port_ndx,
            fap_ports->egr_q_profile_map[idx].conf
           );
    SOC_SAND_CHECK_FUNC_RESULT(res, 27, exit);
  }

  
  for (idx = 0; idx < fap_ports->hdr_type_nof_entries; idx++)
  {
    res = soc_petra_port_header_type_verify(
              unit,
              fap_ports->hdr_type[idx].port_ndx,
              SOC_PETRA_PORT_DIRECTION_BOTH,
              fap_ports->hdr_type[idx].header_type_in
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 28, exit);

	  res = soc_petra_port_header_type_verify(
              unit,
              fap_ports->hdr_type[idx].port_ndx,
              SOC_PETRA_PORT_DIRECTION_BOTH,
              fap_ports->hdr_type[idx].header_type_out
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 29, exit);
  }

  for (idx = 0; idx < fap_ports->if_map_nof_entries; idx++)
  {
     if (
            (fap_ports->if_map[idx].conf.if_id == SOC_PETRA_IF_ID_CPU) &&
            (fap_ports->if_map[idx].port_ndx != SOC_PETRA_DEFAULT_CPU_PORT_ID)
           )
        {
          res = soc_petra_port_to_interface_map_verify(
                  unit,
                  fap_ports->if_map[idx].port_ndx,
                  SOC_PETRA_PORT_DIRECTION_OUTGOING,
                  &(fap_ports->if_map[idx].conf)
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
        }
        else
        {
          res = soc_petra_port_to_interface_map_verify(
                  unit,
                  fap_ports->if_map[idx].port_ndx,
                  SOC_PETRA_PORT_DIRECTION_BOTH,
                  &(fap_ports->if_map[idx].conf)
                );
          SOC_SAND_CHECK_FUNC_RESULT(res, 31, exit);
        }
  }

  if (basic_conf->stag_enable)
  {
    res = soc_pb_itm_stag_set_verify(
            unit,
            &(basic_conf->stag.encoding)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);
  }

  if (hw_adjust->core_freq.enable)
  {
    SOC_SAND_ERR_IF_OUT_OF_RANGE(hw_adjust->core_freq.frequency, SOC_PB_MGMT_CORE_FREQ_MIN, SOC_PB_MGMT_CORE_FREQ_MAX, SOC_PB_CORE_FREQ_OUT_OF_RANGE_ERR, 40, exit);
  }

  res = soc_pb_mgmt_hw_interfaces_verify(
          unit,
          hw_adjust
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mgmt_init_sequence_phase1_verify()", 0, 0);
}

/*********************************************************************
*     Out-of-reset sequence. Enable/Disable the device from
*     receiving and transmitting control cells.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_mgmt_init_sequence_phase2_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PB_INIT_OOR               *oor_info,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    res;
  uint32
    idx = 0,
    stage_id = 0;
  SOC_PB_NIF_STATE_INFO
    nif_state_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_INIT_SEQUENCE_PHASE2_UNSAFE);

  SOC_PB_NIF_STATE_INFO_clear(&nif_state_info);
  nif_state_info.is_nif_on = TRUE;
  nif_state_info.serdes_also = FALSE;

  if (!silent)
  {
    soc_sand_os_printf(
      "\n\r"
      "   Phase 2 initialization: device %u"
      "\n\r",
      unit
    );
  }

  if (oor_info->nof_entries > 0)
  {
    SOC_PB_INIT_PRINT_ADVANCE("Take network interfaces out-of-reset", 2);
  }
  else
  {
    SOC_PB_INIT_PRINT_ADVANCE("SKIPPING network interfaces out-of-reset", 2);
  }

  for (idx = 0; idx < oor_info->nof_entries; idx++)
  {
    res = soc_pb_nif_on_off_set_unsafe(
            unit,
            oor_info->nif_id_active[idx],
            &nif_state_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

  SOC_PB_INIT_PRINT_ADVANCE("Enable control cells", 2);
  res = soc_petra_mgmt_all_ctrl_cells_enable_set_unsafe(
          unit,
          TRUE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  
  res = soc_pb_mgmt_init_finalize(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  SOC_PB_INIT_PRINT_ADVANCE("Phase 2 initialization done", 2);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mgmt_init_sequence_phase2_unsafe()", 0, 0);
}

uint32
  soc_pb_mgmt_init_sequence_phase2_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PB_INIT_OOR               *oor_info
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_MGMT_INIT_SEQUENCE_PHASE2_VERIFY);
  SOC_SAND_CHECK_NULL_INPUT(oor_info);

  SOC_PB_STRUCT_VERIFY(SOC_PB_INIT_OOR, oor_info, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_mgmt_init_sequence_phase2_verify()", 0, 0);
}
/*
 *  Init sequence -
 *  per-block initialization, hardware adjustments etc. }
 */

uint32
  SOC_PB_INIT_OOR_verify(
    SOC_SAND_IN  SOC_PB_INIT_OOR *info
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    ind;


  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->nof_entries, SOC_PB_NIF_NOF_NIFS_MAX, SOC_PB_INIT_OOR_NOF_ENTRIES_OUT_OF_RANGE_ERR, 10, exit);

  for (ind = 0; ind < info->nof_entries; ++ind)
  {
    res = soc_pb_nif_id_verify(
            info->nif_id_active[ind]
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_INIT_OOR_verify()",0,0);
}
#if SOC_PB_DEBUG_IS_LVL1
#endif /* SOC_PB_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>


