/* $Id: soc_petra_stat_if.c,v 1.5 Broadcom SDK $
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

#include <soc/dpp/Petra/petra_api_framework.h>
#include <soc/dpp/Petra/petra_framework.h>
#include <soc/dpp/Petra/petra_api_stat_if.h>
#include <soc/dpp/Petra/petra_stat_if.h>
#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_sw_db.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

/* $Id: soc_petra_stat_if.c,v 1.5 Broadcom SDK $
 *	Maximal sync-period between consecutive sync patterns transmission on the statistics interface,
 *  in nanoseconds.
 */
#define SOC_PETRA_STAT_IF_CLOCK_NS_MAX          30

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
*     This function configures the working mode of the
*     statistics interface
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_stat_if_info_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_STAT_IF_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();
  uint32
    reg_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_STAT_IF_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_CHECK_NULL_INPUT(info);

  reg_val = info->enable;
  SOC_PETRA_FLD_SET(regs->eci.general_controls_reg.stat_oe, reg_val, 10, exit);

  reg_val = info->enable;
  SOC_PETRA_FLD_SET(regs->eci.general_controls_reg.sel_stat_data_out, reg_val, 20, exit);

  reg_val = info->enable;
  SOC_PETRA_FLD_SET(regs->eci.general_controls_reg.stat_ddr_mode, reg_val, 30, exit);

  if (info->enable)
  {
    reg_val = info->if_phase;
    SOC_PETRA_FLD_SET(regs->eci.general_controls_reg.stat_out_phase, info->if_phase, 40, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_stat_if_info_set_unsafe()",0,0);
}

/*********************************************************************
*     This function configures the working mode of the
*     statistics interface
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_stat_if_info_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_STAT_IF_INFO        *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_STAT_IF_INFO_VERIFY);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_stat_if_info_verify()",0,0);
}

/*********************************************************************
*     This function configures the working mode of the
*     statistics interface
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_stat_if_info_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_STAT_IF_INFO        *info
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();
  uint32
    reg_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_STAT_IF_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_PETRA_FLD_GET(regs->eci.general_controls_reg.sel_stat_data_out, reg_val, 10, exit);
  info->enable = SOC_SAND_NUM2BOOL(reg_val);

  SOC_PETRA_FLD_GET(regs->eci.general_controls_reg.stat_out_phase, reg_val, 20, exit);
  info->if_phase = reg_val;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_stat_if_info_get_unsafe()",0,0);
}

/*********************************************************************
*     This function configures the format of the report sent
*     through the statistics interface
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_stat_if_report_info_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_STAT_IF_REPORT_INFO *info
  )
{
  uint32
    reg_val,
    fld_val,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_STAT_IF_REPORT_INFO_SET_UNSAFE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  SOC_PA_REG_GET(regs->iqm.statistics_report_configurations_reg, reg_val, 5, exit);

  if (info->mode == SOC_PETRA_STAT_IF_REPORT_MODE_BILLING)
  {
    fld_val = 0x1;
    SOC_PA_FLD_TO_REG(regs->iqm.statistics_report_configurations_reg.st_rpt_bill, fld_val, reg_val, 10, exit);

    fld_val = (info->format.billing.mode == SOC_PETRA_STAT_IF_BILLING_MODE_CUD)?0x1:0x0;
    SOC_PA_FLD_TO_REG(regs->iqm.statistics_report_configurations_reg.st_rpt_egr_olif, fld_val, reg_val, 20, exit);
  }
  else if (info->mode == SOC_PETRA_STAT_IF_REPORT_MODE_FAP20V)
  {
    fld_val = 0x0;
    SOC_PA_FLD_TO_REG(regs->iqm.statistics_report_configurations_reg.st_rpt_bill, fld_val, reg_val, 25, exit);

    fld_val = info->format.fap20v.mode == SOC_PETRA_STAT_IF_FAP20V_MODE_PKT_SIZE?0x1:0x0;
    SOC_PA_FLD_TO_REG(regs->iqm.statistics_report_configurations_reg.st_rpt_pckt_size, fld_val, reg_val, 30, exit);

    fld_val = info->format.fap20v.fabric_mc == SOC_PETRA_STAT_IF_MC_MODE_MC_ID?0x1:0x0;;
    SOC_PA_FLD_TO_REG(regs->iqm.statistics_report_configurations_reg.st_rpt_mcid_spt_mc, fld_val, reg_val, 40, exit);

    fld_val = info->format.fap20v.ing_mc == SOC_PETRA_STAT_IF_MC_MODE_MC_ID?0x1:0x0;
    SOC_PA_FLD_TO_REG(regs->iqm.statistics_report_configurations_reg.st_rpt_mcid_ing_rep, fld_val, reg_val, 50, exit);

    fld_val = info->format.fap20v.count_snoop;
    SOC_PA_FLD_TO_REG(regs->iqm.statistics_report_configurations_reg.st_rpt_cnt_snp, fld_val, reg_val, 60, exit);
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
    SOC_SAND_ERR_IF_ABOVE_MAX(fld_val, SOC_PETRA_STAT_IF_CLOCK_NS_MAX, SOC_SAND_VALUE_OUT_OF_RANGE_ERR, 70, exit);
  }

  SOC_PA_FLD_TO_REG(regs->iqm.statistics_report_configurations_reg.st_rpt_sync_prd, fld_val, reg_val, 75, exit);

  SOC_PA_REG_SET(regs->iqm.statistics_report_configurations_reg, reg_val, 80, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_stat_if_report_info_set_unsafe()",0,0);
}

/*********************************************************************
*     This function configures the format of the report sent
*     through the statistics interface
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_stat_if_report_info_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_STAT_IF_REPORT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_STAT_IF_REPORT_INFO_VERIFY);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->mode, SOC_PETRA_STAT_NOF_IF_REPORT_MODES-1, SOC_PETRA_STAT_MODE_OUT_OF_RANGE_ERR, 5, exit);

  if (info->mode == SOC_PETRA_STAT_IF_REPORT_MODE_FAP20V)
  {
    if (info->format.fap20v.mode != SOC_PETRA_STAT_IF_FAP20V_MODE_PKT_SIZE)
    {
      if (info->format.fap20v.fabric_mc == SOC_PETRA_STAT_IF_MC_MODE_MC_ID)
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_STAT_MULT_ID_UNSUPPORTED_ERR, 10, exit);
      }
      if (info->format.fap20v.ing_mc == SOC_PETRA_STAT_IF_MC_MODE_MC_ID)
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_STAT_MULT_ID_UNSUPPORTED_ERR, 20, exit);
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_stat_if_report_info_verify()",0,0);
}

/*********************************************************************
*     This function configures the format of the report sent
*     through the statistics interface
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_stat_if_report_info_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_STAT_IF_REPORT_INFO *info
  )
{
  uint32
    fld_val,
    reg_val,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_STAT_IF_REPORT_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_DRIVER_AND_DEVICE;
  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  SOC_PA_REG_GET(regs->iqm.statistics_report_configurations_reg, reg_val, 5, exit);

  SOC_PA_FLD_FROM_REG(regs->iqm.statistics_report_configurations_reg.st_rpt_bill, fld_val, reg_val, 10, exit);
  info->mode = (fld_val == 0x1)?SOC_PETRA_STAT_IF_REPORT_MODE_BILLING:SOC_PETRA_STAT_IF_REPORT_MODE_FAP20V;

  if (info->mode == SOC_PETRA_STAT_IF_REPORT_MODE_BILLING)
  {
    SOC_PA_FLD_FROM_REG(regs->iqm.statistics_report_configurations_reg.st_rpt_egr_olif, fld_val, reg_val, 20, exit);
    info->format.billing.mode = (fld_val == 0x1)?SOC_PETRA_STAT_IF_BILLING_MODE_CUD:SOC_PETRA_STAT_IF_BILLING_MODE_EGR_Q_NUM;
  }
  if (info->mode == SOC_PETRA_STAT_IF_REPORT_MODE_FAP20V)
  {
    SOC_PA_FLD_FROM_REG(regs->iqm.statistics_report_configurations_reg.st_rpt_pckt_size, fld_val, reg_val, 30, exit);
    info->format.fap20v.mode  = (fld_val == 0x1)?SOC_PETRA_STAT_IF_FAP20V_MODE_PKT_SIZE:SOC_PETRA_STAT_IF_FAP20V_MODE_Q_SIZE;

    SOC_PA_FLD_FROM_REG(regs->iqm.statistics_report_configurations_reg.st_rpt_mcid_spt_mc, fld_val, reg_val, 40, exit);
    info->format.fap20v.fabric_mc = (fld_val == 0x1)?SOC_PETRA_STAT_IF_MC_MODE_MC_ID:SOC_PETRA_STAT_IF_MC_MODE_Q_NUM;

    SOC_PA_FLD_FROM_REG(regs->iqm.statistics_report_configurations_reg.st_rpt_mcid_ing_rep, fld_val, reg_val, 50, exit);
    info->format.fap20v.ing_mc = (fld_val == 0x1)?SOC_PETRA_STAT_IF_MC_MODE_MC_ID:SOC_PETRA_STAT_IF_MC_MODE_Q_NUM;

    SOC_PA_FLD_FROM_REG(regs->iqm.statistics_report_configurations_reg.st_rpt_cnt_snp, fld_val, reg_val, 60, exit);
    info->format.fap20v.count_snoop = SOC_SAND_NUM2BOOL(fld_val);
  }

  SOC_PA_FLD_FROM_REG(regs->iqm.statistics_report_configurations_reg.st_rpt_sync_prd, fld_val, reg_val, 65, exit);
  SOC_SAND_LIMIT_FROM_ABOVE(fld_val, SOC_PETRA_STAT_IF_CLOCK_NS_MAX);
  fld_val = soc_sand_power_of_2(fld_val);

  res = soc_petra_ticks_to_time(
          unit,
          fld_val,
          TRUE,
          1,
          &(info->sync_rate)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_stat_if_report_info_get_unsafe()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

