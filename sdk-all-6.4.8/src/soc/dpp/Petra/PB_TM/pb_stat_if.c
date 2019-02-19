/* $Id: soc_pb_stat_if.c,v 1.6 Broadcom SDK $
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
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>

#include <soc/dpp/Petra/PB_TM/pb_api_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_stat_if.h>

#include <soc/dpp/Petra/petra_chip_regs.h>
#include <soc/dpp/Petra/petra_reg_access.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_STAT_IF_BILLING_INFO_MODE_MAX                       (SOC_PB_STAT_NOF_IF_BILLING_MODES-1)
#define SOC_PB_STAT_IF_FAP20V_INFO_MODE_MAX                        (SOC_PB_STAT_NOF_IF_FAP20V_MODES-1)
#define SOC_PB_STAT_IF_FAP20V_INFO_FABRIC_MC_MAX                   (SOC_PB_STAT_NOF_IF_MC_MODES-1)
#define SOC_PB_STAT_IF_FAP20V_INFO_ING_REP_MC_MAX                  (SOC_PB_STAT_NOF_IF_MC_MODES-1)
#define SOC_PB_STAT_IF_FAP20V_INFO_CNM_REPORT_MAX                  (SOC_PB_STAT_NOF_IF_CNM_MODES-1)
#define SOC_PB_STAT_IF_REPORT_INFO_PARITY_MODE_MAX                 (SOC_PB_STAT_NOF_IF_PARITY_MODES-1)
#define SOC_PB_STAT_IF_BIST_PATTERN_WORD1_MAX                      (0xFFFFF)
#define SOC_PB_STAT_IF_BIST_PATTERN_WORD2_MAX                      (0xFFFFF)
#define SOC_PB_STAT_IF_BIST_INFO_EN_MODE_MAX                       (SOC_PB_STAT_NOF_IF_BIST_EN_MODES-1)
#define SOC_PB_STAT_IF_BIST_INFO_NOF_DUPLICATIONS_MAX              (63)
#define SOC_PB_STAT_IF_BIST_INFO_NOF_IDLE_CLOCKS_MAX               (126)

/* $Id: soc_pb_stat_if.c,v 1.6 Broadcom SDK $
 *	Field values
 */
#define SOC_PB_STAT_IF_REPORT_MODE_FLD_VAL_FAP20V                  (0)
#define SOC_PB_STAT_IF_REPORT_MODE_FLD_VAL_BILLING_CUD             (1)
#define SOC_PB_STAT_IF_REPORT_MODE_FLD_VAL_BILLING_EGR_Q_NB        (2)
#define SOC_PB_STAT_IF_REPORT_MODE_FLD_VAL_BILLING_VSI_VLAN        (3)
#define SOC_PB_STAT_IF_REPORT_MODE_FLD_VAL_BILLING_BOTH_LIFS       (4)
#define SOC_PB_STAT_IF_FAP20V_MODE_FLD_VAL_Q_SIZE                  (0)
#define SOC_PB_STAT_IF_FAP20V_MODE_FLD_VAL_PKT_SIZE                (1)

#define SOC_PB_STAT_IF_BIST_EN_FLD_VAL_DIS                         (0)
#define SOC_PB_STAT_IF_BIST_EN_FLD_VAL_EN                          (1)
#define SOC_PB_STAT_IF_BIST_MODE_FLD_VAL_PATTERN                   (0)
#define SOC_PB_STAT_IF_BIST_MODE_FLD_VAL_SHIFT                     (1)
#define SOC_PB_STAT_IF_BIST_MODE_FLD_VAL_PRBS                      (2)


/*
 *	Maximal sync-period between consecutive sync patterns transmission on the statistics interface,
 *  in nanoseconds.
 */
#define SOC_PB_STAT_IF_CLOCK_NS_MAX                                (30)

#define SOC_PB_STAT_BIST_IDLE_CLOCK_RESOLUTION                     (2)

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

/*********************************************************************
*     This function configures the format of the report sent
 *     through the statistics interface
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_stat_if_report_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_STAT_IF_REPORT_INFO      *info
  )
{
  uint32
    reg_val,
    fld_val,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_STAT_IF_REPORT_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  SOC_PB_REG_GET(regs->iqm.statistics_report_configurations_reg, reg_val, 5, exit);

  fld_val = 1; /* info->enable sets ddr mode in general controls */
  SOC_PB_FLD_TO_REG(regs->iqm.statistics_report_configurations_reg.st_rpt_ddr_mode, fld_val, reg_val, 7, exit);

  if (info->mode == SOC_PETRA_STAT_IF_REPORT_MODE_BILLING)
  {
    switch(info->format.billing.mode)
    {
    case SOC_PB_STAT_IF_BILLING_MODE_CUD:
      fld_val = SOC_PB_STAT_IF_REPORT_MODE_FLD_VAL_BILLING_CUD;
      break;
    
    case SOC_PB_STAT_IF_BILLING_MODE_EGR_Q_NB:
      fld_val = SOC_PB_STAT_IF_REPORT_MODE_FLD_VAL_BILLING_EGR_Q_NB;
      break;

    case SOC_PB_STAT_IF_BILLING_MODE_VSI_VLAN:
      fld_val = SOC_PB_STAT_IF_REPORT_MODE_FLD_VAL_BILLING_VSI_VLAN;
      break;

    case SOC_PB_STAT_IF_BILLING_MODE_BOTH_LIFS:
      fld_val = SOC_PB_STAT_IF_REPORT_MODE_FLD_VAL_BILLING_BOTH_LIFS;
      break;

    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_STAT_BIILING_MODE_OUT_OF_RANGE_ERR, 10, exit);
    }

    SOC_PB_FLD_TO_REG(regs->iqm.statistics_report_configurations_reg.st_rpt_bill, fld_val, reg_val, 15, exit);
  }
  else if (info->mode == SOC_PETRA_STAT_IF_REPORT_MODE_FAP20V)
  {
    fld_val = SOC_PB_STAT_IF_REPORT_MODE_FLD_VAL_FAP20V;
    SOC_PB_FLD_TO_REG(regs->iqm.statistics_report_configurations_reg.st_rpt_bill, fld_val, reg_val, 20, exit);

    switch(info->format.fap20v.mode)
    {
    case SOC_PB_STAT_IF_FAP20V_MODE_Q_SIZE:
      fld_val = SOC_PB_STAT_IF_FAP20V_MODE_FLD_VAL_Q_SIZE;
      break;

    case SOC_PB_STAT_IF_FAP20V_MODE_PKT_SIZE:
      fld_val = SOC_PB_STAT_IF_FAP20V_MODE_FLD_VAL_PKT_SIZE;
      break;

    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_STAT_FAP20V_MODE_OUT_OF_RANGE_ERR, 25, exit);
    }
    SOC_PB_FLD_TO_REG(regs->iqm.statistics_report_configurations_reg.st_rpt_pckt_size, fld_val, reg_val, 30, exit);

    fld_val = info->format.fap20v.fabric_mc;
    SOC_PB_FLD_TO_REG(regs->iqm.statistics_report_configurations_reg.st_rpt_mcid_spt_mc, fld_val, reg_val, 35, exit);

    fld_val = info->format.fap20v.ing_rep_mc;
    SOC_PB_FLD_TO_REG(regs->iqm.statistics_report_configurations_reg.st_rpt_mcid_ing_rep, fld_val, reg_val, 40, exit);

    fld_val = info->format.fap20v.count_snoop;
    SOC_PB_FLD_TO_REG(regs->iqm.statistics_report_configurations_reg.st_rpt_cnt_snp, fld_val, reg_val, 45, exit);

    fld_val = info->format.fap20v.single_copy_reported;
    SOC_PB_FLD_TO_REG(regs->iqm.statistics_report_configurations_reg.st_rpt_mcid_once, fld_val, reg_val, 50, exit);

    fld_val = info->format.fap20v.cnm_report;
    SOC_PB_FLD_TO_REG(regs->iqm.statistics_report_configurations_reg.st_rpt_cnm_en, fld_val, reg_val, 55, exit);

    fld_val = info->format.fap20v.is_original_pkt_size;
    SOC_PB_FLD_TO_REG(regs->iqm.statistics_report_configurations_reg.st_rpt_show_org_pckt_size, fld_val, reg_val, 60, exit);
  }
  else
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_STAT_MODE_OUT_OF_RANGE_ERR, 62, exit);
  }

  if (info->sync_rate == 0)
  {
    fld_val = 0x0;
  }
  else
  {
    res = soc_petra_chip_time_to_ticks(
            unit,
            info->sync_rate,
            TRUE,
            1,
            FALSE,
            &fld_val
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 65, exit);

    fld_val = soc_sand_log2_round_down(fld_val);

    SOC_SAND_LIMIT_FROM_BELOW(fld_val, 0x1);
    SOC_SAND_ERR_IF_ABOVE_MAX(fld_val, SOC_PB_STAT_IF_CLOCK_NS_MAX, SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 72, exit);
  }

  SOC_PB_FLD_TO_REG(regs->iqm.statistics_report_configurations_reg.st_rpt_sync_prd, fld_val, reg_val, 75, exit);

  fld_val = info->parity_mode;
  SOC_PB_FLD_TO_REG(regs->iqm.statistics_report_configurations_reg.st_rpt_par_en, fld_val, reg_val, 77, exit);

  SOC_PB_REG_SET(regs->iqm.statistics_report_configurations_reg, reg_val, 80, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_stat_if_report_set_unsafe()", 0, 0);
}

uint32
  soc_pb_stat_if_report_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_STAT_IF_REPORT_INFO      *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_STAT_IF_REPORT_INFO_SET_VERIFY);

  SOC_PB_STRUCT_VERIFY(SOC_PB_STAT_IF_REPORT_INFO, info, 10, exit);

  if (info->mode == SOC_PETRA_STAT_IF_REPORT_MODE_FAP20V)
  {
    if (info->format.fap20v.mode != SOC_PB_STAT_IF_FAP20V_MODE_PKT_SIZE)
    {
      if (info->format.fap20v.fabric_mc)
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_STAT_MULT_ID_UNSUPPORTED_ERR, 10, exit);
      }
      if (info->format.fap20v.ing_rep_mc)
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_STAT_MULT_ID_UNSUPPORTED_ERR, 20, exit);
      }
      if (info->format.fap20v.cnm_report)
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_STAT_MULT_ID_UNSUPPORTED_ERR, 30, exit);
      }
   }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_stat_if_report_set_verify()", 0, 0);
}

/*********************************************************************
*     This function configures the format of the report sent
 *     through the statistics interface
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_stat_if_report_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_PB_STAT_IF_REPORT_INFO      *info
  )
{
  uint32
    fld_val,
    reg_val,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;
  SOC_PB_STAT_IF_BILLING_MODE
    billing_mode;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_STAT_IF_REPORT_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_STAT_IF_REPORT_INFO_clear(info);

  regs = soc_petra_regs();

  SOC_PB_REG_GET(regs->iqm.statistics_report_configurations_reg, reg_val, 5, exit);

  SOC_PB_FLD_FROM_REG(regs->iqm.statistics_report_configurations_reg.st_rpt_bill, fld_val, reg_val, 10, exit);
  info->mode = (fld_val == SOC_PB_STAT_IF_REPORT_MODE_FLD_VAL_FAP20V)?SOC_PETRA_STAT_IF_REPORT_MODE_FAP20V:SOC_PETRA_STAT_IF_REPORT_MODE_BILLING;

  if (info->mode == SOC_PETRA_STAT_IF_REPORT_MODE_BILLING)
  {
    switch(fld_val)
    {
    case SOC_PB_STAT_IF_REPORT_MODE_FLD_VAL_BILLING_CUD:
      billing_mode = SOC_PB_STAT_IF_BILLING_MODE_CUD;
      break;

    case SOC_PB_STAT_IF_REPORT_MODE_FLD_VAL_BILLING_EGR_Q_NB:
      billing_mode = SOC_PB_STAT_IF_BILLING_MODE_EGR_Q_NB;
      break;

    case SOC_PB_STAT_IF_REPORT_MODE_FLD_VAL_BILLING_VSI_VLAN:
      billing_mode = SOC_PB_STAT_IF_BILLING_MODE_VSI_VLAN;
      break;

    case SOC_PB_STAT_IF_REPORT_MODE_FLD_VAL_BILLING_BOTH_LIFS:
      billing_mode = SOC_PB_STAT_IF_BILLING_MODE_BOTH_LIFS;
      break;

    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_STAT_BIILING_MODE_OUT_OF_RANGE_ERR, 15, exit);
    }
    info->format.billing.mode = billing_mode;
  }

  else if (info->mode == SOC_PETRA_STAT_IF_REPORT_MODE_FAP20V)
  {
    SOC_PB_FLD_FROM_REG(regs->iqm.statistics_report_configurations_reg.st_rpt_pckt_size, fld_val, reg_val, 20, exit);
    switch(fld_val)
    {
    case SOC_PB_STAT_IF_FAP20V_MODE_FLD_VAL_Q_SIZE:
      info->format.fap20v.mode = SOC_PB_STAT_IF_FAP20V_MODE_Q_SIZE;
      break;

    case SOC_PB_STAT_IF_FAP20V_MODE_FLD_VAL_PKT_SIZE:
      info->format.fap20v.mode = SOC_PB_STAT_IF_FAP20V_MODE_PKT_SIZE;
      break;

    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_STAT_FAP20V_MODE_OUT_OF_RANGE_ERR, 25, exit);
    }

    SOC_PB_FLD_FROM_REG(regs->iqm.statistics_report_configurations_reg.st_rpt_mcid_spt_mc, fld_val, reg_val, 30, exit);
    info->format.fap20v.fabric_mc = SOC_SAND_NUM2BOOL(fld_val);

    SOC_PB_FLD_FROM_REG(regs->iqm.statistics_report_configurations_reg.st_rpt_mcid_ing_rep, fld_val, reg_val, 35, exit);
    info->format.fap20v.ing_rep_mc = SOC_SAND_NUM2BOOL(fld_val);

    SOC_PB_FLD_FROM_REG(regs->iqm.statistics_report_configurations_reg.st_rpt_cnt_snp, fld_val, reg_val, 40, exit);
    info->format.fap20v.count_snoop = SOC_SAND_NUM2BOOL(fld_val);

    SOC_PB_FLD_FROM_REG(regs->iqm.statistics_report_configurations_reg.st_rpt_mcid_once, fld_val, reg_val, 45, exit);
    info->format.fap20v.single_copy_reported = SOC_SAND_NUM2BOOL(fld_val);

    SOC_PB_FLD_FROM_REG(regs->iqm.statistics_report_configurations_reg.st_rpt_cnm_en, fld_val, reg_val, 55, exit);
    info->format.fap20v.cnm_report = SOC_SAND_NUM2BOOL(fld_val);

    SOC_PB_FLD_FROM_REG(regs->iqm.statistics_report_configurations_reg.st_rpt_show_org_pckt_size, fld_val, reg_val, 60, exit);
    info->format.fap20v.is_original_pkt_size = SOC_SAND_NUM2BOOL(fld_val);
  }

  SOC_PB_FLD_FROM_REG(regs->iqm.statistics_report_configurations_reg.st_rpt_sync_prd, fld_val, reg_val, 65, exit);
  SOC_SAND_LIMIT_FROM_ABOVE(fld_val, SOC_PB_STAT_IF_CLOCK_NS_MAX);
  fld_val = soc_sand_power_of_2(fld_val);

  res = soc_petra_ticks_to_time(
          unit,
          fld_val,
          TRUE,
          1,
          &(info->sync_rate)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  SOC_PB_FLD_FROM_REG(regs->iqm.statistics_report_configurations_reg.st_rpt_par_en, fld_val, reg_val, 75, exit);
  info->parity_mode = SOC_SAND_NUM2BOOL(fld_val);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_stat_if_report_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Set the Statistic Interface under a BIST mode. Discard
 *     the statistic reports and allow the user to present test
 *     data on the statistic interface as a debug feature.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_stat_if_bist_set_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_STAT_IF_BIST_INFO        *info
  )
{
  uint32
    reg_val,
    fld_val,
    bist_mode_fld,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_STAT_IF_BIST_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  /*
   *	Set the pattern first
   */
  if (info->en_mode == SOC_PB_STAT_IF_BIST_EN_MODE_PATTERN)
  {
    fld_val = info->pattern.word1;
    SOC_PB_FLD_SET(regs->iqm.stat_if_bist_patern_reg[0].bist_ptrn0, fld_val, 50, exit);

    fld_val = info->pattern.word2;
    SOC_PB_FLD_SET(regs->iqm.stat_if_bist_patern_reg[1].bist_ptrn0, fld_val, 55, exit);
 }
  else if (info->en_mode == SOC_PB_STAT_IF_BIST_EN_MODE_WALKING_ONE)
  {
    fld_val = 0x1;
    SOC_PB_FLD_SET(regs->iqm.stat_if_bist_patern_reg[0].bist_ptrn0, fld_val, 60, exit);

    fld_val = 0x0;
    SOC_PB_FLD_SET(regs->iqm.stat_if_bist_patern_reg[1].bist_ptrn0, fld_val, 65, exit);
  }

  /*
   *	Set the BIST configuration
   */
  SOC_PB_REG_GET(regs->iqm.stat_if_bist_reg, reg_val, 5, exit);

  switch(info->en_mode)
  {
  case SOC_PB_STAT_IF_BIST_EN_MODE_DIS:
    fld_val = SOC_PB_STAT_IF_BIST_EN_FLD_VAL_DIS;
    bist_mode_fld = 0;
    break;

  case SOC_PB_STAT_IF_BIST_EN_MODE_PATTERN:
    fld_val = SOC_PB_STAT_IF_BIST_EN_FLD_VAL_EN;
    bist_mode_fld = SOC_PB_STAT_IF_BIST_MODE_FLD_VAL_PATTERN;
    break;

  case SOC_PB_STAT_IF_BIST_EN_MODE_WALKING_ONE:
    fld_val = SOC_PB_STAT_IF_BIST_EN_FLD_VAL_EN;
    bist_mode_fld = SOC_PB_STAT_IF_BIST_MODE_FLD_VAL_SHIFT;
    break;

  case SOC_PB_STAT_IF_BIST_EN_MODE_PRBS:
    fld_val = SOC_PB_STAT_IF_BIST_EN_FLD_VAL_EN;
    bist_mode_fld = SOC_PB_STAT_IF_BIST_MODE_FLD_VAL_PRBS;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_STAT_EN_MODE_OUT_OF_RANGE_ERR, 10, exit);
  }
  SOC_PB_FLD_TO_REG(regs->iqm.stat_if_bist_reg.bist_en, fld_val, reg_val, 15, exit);
  SOC_PB_FLD_TO_REG(regs->iqm.stat_if_bist_reg.bist_mode, bist_mode_fld, reg_val, 20, exit);

  fld_val = info->nof_duplications;
  SOC_PB_FLD_TO_REG(regs->iqm.stat_if_bist_reg.bist_burst_actions, fld_val, reg_val, 25, exit);

  fld_val = info->nof_idle_clocks / SOC_PB_STAT_BIST_IDLE_CLOCK_RESOLUTION;
  SOC_PB_FLD_TO_REG(regs->iqm.stat_if_bist_reg.bist_gap, fld_val, reg_val, 30, exit);

  SOC_PB_REG_SET(regs->iqm.stat_if_bist_reg, reg_val, 35, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_stat_if_bist_set_unsafe()", 0, 0);
}

uint32
  soc_pb_stat_if_bist_set_verify(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  SOC_PB_STAT_IF_BIST_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_STAT_IF_BIST_SET_VERIFY);

  SOC_PB_STRUCT_VERIFY(SOC_PB_STAT_IF_BIST_INFO, info, 10, exit);
  if (info->en_mode != SOC_PB_STAT_IF_BIST_EN_MODE_PATTERN)
  {
    if (info->pattern.word1)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_STAT_WORD1_INVALID_ERR, 20, exit);
    }
    if (info->pattern.word2)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_STAT_WORD2_INVALID_ERR, 30, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_stat_if_bist_set_verify()", 0, 0);
}

/*********************************************************************
*     Set the Statistic Interface under a BIST mode. Discard
 *     the statistic reports and allow the user to present test
 *     data on the statistic interface as a debug feature.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_stat_if_bist_get_unsafe(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_OUT SOC_PB_STAT_IF_BIST_INFO        *info
  )
{
  uint32
    fld_val,
    reg_val,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;
  SOC_PB_STAT_IF_BIST_EN_MODE
    en_mode;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_STAT_IF_BIST_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PB_STAT_IF_BIST_INFO_clear(info);

  regs = soc_petra_regs();

  SOC_PB_REG_GET(regs->iqm.stat_if_bist_reg, reg_val, 5, exit);

  SOC_PB_FLD_FROM_REG(regs->iqm.stat_if_bist_reg.bist_en, fld_val, reg_val, 10, exit);
  switch(fld_val)
  {
  case SOC_PB_STAT_IF_BIST_EN_FLD_VAL_DIS:
    en_mode = SOC_PB_STAT_IF_BIST_EN_MODE_DIS;
    break;

  case SOC_PB_STAT_IF_BIST_EN_FLD_VAL_EN:
    SOC_PB_FLD_FROM_REG(regs->iqm.stat_if_bist_reg.bist_mode, fld_val, reg_val, 15, exit);

    switch(fld_val)
    {
    case SOC_PB_STAT_IF_BIST_MODE_FLD_VAL_PATTERN:
      en_mode = SOC_PB_STAT_IF_BIST_EN_MODE_PATTERN;
      break;

    case SOC_PB_STAT_IF_BIST_MODE_FLD_VAL_SHIFT:
      en_mode = SOC_PB_STAT_IF_BIST_EN_MODE_WALKING_ONE;
      break;

    case SOC_PB_STAT_IF_BIST_MODE_FLD_VAL_PRBS:
      en_mode = SOC_PB_STAT_IF_BIST_EN_MODE_PRBS;
      break;

    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PB_STAT_EN_MODE_OUT_OF_RANGE_ERR, 20, exit);
    }
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_STAT_EN_MODE_OUT_OF_RANGE_ERR, 25, exit);
  }
  info->en_mode = en_mode;

  if (en_mode == SOC_PB_STAT_IF_BIST_EN_MODE_PATTERN)
  {
    SOC_PB_FLD_GET(regs->iqm.stat_if_bist_patern_reg[0].bist_ptrn0, fld_val, 30, exit);
    info->pattern.word1 = fld_val;

    SOC_PB_FLD_GET(regs->iqm.stat_if_bist_patern_reg[1].bist_ptrn0, fld_val, 35, exit);
    info->pattern.word2 = fld_val;
  }

  SOC_PB_FLD_FROM_REG(regs->iqm.stat_if_bist_reg.bist_burst_actions, fld_val, reg_val, 40, exit);
  info->nof_duplications = fld_val;

  SOC_PB_FLD_FROM_REG(regs->iqm.stat_if_bist_reg.bist_gap, fld_val, reg_val, 45, exit);
  info->nof_idle_clocks = fld_val * SOC_PB_STAT_BIST_IDLE_CLOCK_RESOLUTION;


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_stat_if_bist_get_unsafe()", 0, 0);
}

uint32
  SOC_PB_STAT_IF_BILLING_INFO_verify(
    SOC_SAND_IN  SOC_PB_STAT_IF_BILLING_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->mode, SOC_PB_STAT_IF_BILLING_INFO_MODE_MAX, SOC_PB_STAT_BIILING_MODE_OUT_OF_RANGE_ERR, 10, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_STAT_IF_BILLING_INFO_verify()",0,0);
}

uint32
  SOC_PB_STAT_IF_FAP20V_INFO_verify(
    SOC_SAND_IN  SOC_PB_STAT_IF_FAP20V_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->mode, SOC_PB_STAT_IF_FAP20V_INFO_MODE_MAX, SOC_PB_STAT_FAP20V_MODE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->fabric_mc, SOC_PB_STAT_IF_FAP20V_INFO_FABRIC_MC_MAX, SOC_PB_STAT_FABRIC_MC_OUT_OF_RANGE_ERR, 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->ing_rep_mc, SOC_PB_STAT_IF_FAP20V_INFO_ING_REP_MC_MAX, SOC_PB_STAT_ING_REP_MC_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->cnm_report, SOC_PB_STAT_IF_FAP20V_INFO_CNM_REPORT_MAX, SOC_PB_STAT_CNM_REPORT_OUT_OF_RANGE_ERR, 15, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_STAT_IF_FAP20V_INFO_verify()",0,0);
}

uint32
  SOC_PB_STAT_IF_REPORT_MODE_INFO_verify(
    SOC_SAND_IN  SOC_PETRA_STAT_IF_REPORT_MODE    report_mode,
    SOC_SAND_IN  SOC_PB_STAT_IF_REPORT_MODE_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  switch(report_mode)
  {
  case SOC_PETRA_STAT_IF_REPORT_MODE_BILLING:
    SOC_PB_STRUCT_VERIFY(SOC_PB_STAT_IF_BILLING_INFO, &(info->billing), 10, exit);
    break;

  case SOC_PETRA_STAT_IF_REPORT_MODE_FAP20V:
    SOC_PB_STRUCT_VERIFY(SOC_PB_STAT_IF_FAP20V_INFO, &(info->fap20v), 11, exit);
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PB_STAT_REPORT_MODE_OUT_OF_RANGE_ERR, 10, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_STAT_IF_REPORT_MODE_INFO_verify()",0,0);
}

uint32
  SOC_PB_STAT_IF_REPORT_INFO_verify(
    SOC_SAND_IN  SOC_PB_STAT_IF_REPORT_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  res = SOC_PB_STAT_IF_REPORT_MODE_INFO_verify(
          (info->mode),
          &(info->format)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->parity_mode, SOC_PB_STAT_IF_REPORT_INFO_PARITY_MODE_MAX, SOC_PB_STAT_PARITY_MODE_OUT_OF_RANGE_ERR, 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_STAT_IF_REPORT_INFO_verify()",0,0);
}

uint32
  SOC_PB_STAT_IF_BIST_PATTERN_verify(
    SOC_SAND_IN  SOC_PB_STAT_IF_BIST_PATTERN *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->word1, SOC_PB_STAT_IF_BIST_PATTERN_WORD1_MAX, SOC_PB_STAT_WORD1_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->word2, SOC_PB_STAT_IF_BIST_PATTERN_WORD2_MAX, SOC_PB_STAT_WORD2_OUT_OF_RANGE_ERR, 11, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_STAT_IF_BIST_PATTERN_verify()",0,0);
}

uint32
  SOC_PB_STAT_IF_BIST_INFO_verify(
    SOC_SAND_IN  SOC_PB_STAT_IF_BIST_INFO *info
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->en_mode, SOC_PB_STAT_IF_BIST_INFO_EN_MODE_MAX, SOC_PB_STAT_EN_MODE_OUT_OF_RANGE_ERR, 10, exit);
  SOC_PB_STRUCT_VERIFY(SOC_PB_STAT_IF_BIST_PATTERN, &(info->pattern), 11, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->nof_duplications, SOC_PB_STAT_IF_BIST_INFO_NOF_DUPLICATIONS_MAX, SOC_PB_STAT_NOF_DUPLICATIONS_OUT_OF_RANGE_ERR, 12, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->nof_idle_clocks, SOC_PB_STAT_IF_BIST_INFO_NOF_IDLE_CLOCKS_MAX, SOC_PB_STAT_NOF_IDLE_CLOCKS_OUT_OF_RANGE_ERR, 13, exit);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_STAT_IF_BIST_INFO_verify()",0,0);
}
#if SOC_PB_DEBUG_IS_LVL1
#endif /* SOC_PB_DEBUG_IS_LVL1 */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

