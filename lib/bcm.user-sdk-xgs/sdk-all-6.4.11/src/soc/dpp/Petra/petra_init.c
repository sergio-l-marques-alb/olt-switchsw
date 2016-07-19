/* $Id: petra_init.c,v 1.11 Broadcom SDK $
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
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_general.h>
#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_tbl_access.h>

#include <soc/dpp/Petra/petra_mgmt.h>
#include <soc/dpp/Petra/petra_init.h>
#include <soc/dpp/Petra/petra_egr_queuing.h>
#include <soc/dpp/Petra/petra_fabric.h>
#include <soc/dpp/Petra/petra_flow_control.h>
#include <soc/dpp/Petra/petra_ingress_header_parsing.h>
#include <soc/dpp/Petra/petra_ingress_scheduler.h>
#include <soc/dpp/Petra/petra_nif.h>
#include <soc/dpp/Petra/petra_ports.h>
#include <soc/dpp/Petra/petra_scheduler_end2end.h>
#include <soc/dpp/Petra/petra_scheduler_ports.h>
#include <soc/dpp/Petra/petra_serdes.h>
#include <soc/dpp/Petra/petra_serdes_utils.h>
#include <soc/dpp/Petra/petra_fabric.h>
#include <soc/dpp/Petra/petra_ingress_packet_queuing.h>
#include <soc/dpp/Petra/petra_ingress_traffic_mgmt.h>
#include <soc/dpp/Petra/petra_multicast_ingress.h>
#include <soc/dpp/Petra/petra_sw_db.h>
#include <soc/dpp/Petra/petra_statistics.h>
#include <soc/dpp/Petra/petra_interrupt_service.h>
#include <soc/dpp/Petra/petra_diagnostics.h>
#include <soc/dpp/Petra/petra_dram.h>
#include <soc/dpp/Petra/petra_stat_if.h>
#include <soc/dpp/Petra/petra_packet.h>
#include <soc/dpp/Petra/petra_cell.h>
#include <soc/dpp/Petra/petra_shadow.h>

#ifdef LINK_PB_LIBRARIES
#include <soc/dpp/Petra/PB_TM/pb_tbl_access.h>
#endif

#include <soc/dpp/SAND/Utils/sand_workload_status.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PETRA_MGMT_INIT_TIMER_DELAY_MSEC           50
#define SOC_PETRA_INIT_MEM_CORR_DELAY_MSEC             50

#define SOC_PETRA_MGMT_INIT_EGQ_MAX_FRG_FIX            508
#define SOC_PETRA_MGMT_INIT_EGQ_MAX_FRG_VAR            127

/* $Id: petra_init.c,v 1.11 Broadcom SDK $
 *  QDR wait for init
 */
#define SOC_PETRA_MGMT_INIT_QDR_BUSY_WAIT_ITERATIONS   50
#define SOC_PETRA_MGMT_INIT_QDR_TIMER_ITERATIONS       10
#define SOC_PETRA_MGMT_INIT_QDR_TIMER_DELAY_MSEC       50

#define SOC_PETRA_MGMT_INIT_QDR_IO_RESET_DELAY_MSEC    20

/*
 *  EGQ wait for init
 */
#define SOC_PETRA_MGMT_INIT_EGQ_BUSY_WAIT_ITERATIONS   50
#define SOC_PETRA_MGMT_INIT_EGQ_TIMER_ITERATIONS       10
#define SOC_PETRA_MGMT_INIT_EGQ_TIMER_DELAY_MSEC       50

/*
 *  DRAM wait for init
 */
#define SOC_PETRA_MGMT_INIT_DRAM_BUSY_WAIT_ITERATIONS   50
#define SOC_PETRA_MGMT_INIT_DRAM_TIMER_ITERATIONS       10
#define SOC_PETRA_MGMT_INIT_DRAM_TIMER_DELAY_MSEC       50

/*
 *  SMS - memory correction
 */
#define SOC_PETRA_MGMT_INIT_SMS_BUSY_WAIT_ITERATIONS   50
#define SOC_PETRA_MGMT_INIT_SMS_TIMER_ITERATIONS       10
#define SOC_PETRA_MGMT_INIT_SMS_TIMER_DELAY_MSEC       50

/*
 *	RM values
 */
#define SOC_PETRA_INIT_POLL_SMS_BUSY_WAIT_ITERATIONS   100
#define SOC_PETRA_INIT_POLL_SMS_TIMER_ITERATIONS       100
#define SOC_PETRA_INIT_POLL_SMS_TIMER_DELAY_MSEC       1


/*
 *  DRAM buffers
 */
#define SOC_PETRA_INIT_DRAM_BYTES_FOR_FBC               32
#define SOC_PETRA_INIT_DRAM_FBC_SEQUENCE_SIZE           11
#define SOC_PETRA_INIT_DRAM_BUFF_TO_FBC_DELTA_MIN       256

/*
 *  DRAM Configuration
 */
#define SOC_PETRA_MGMT_HW_ADJUST_DDR_A_BL     0x2
#define SOC_PETRA_MGMT_HW_ADJUST_DDR_P_BL     0x0
#define SOC_PETRA_MGMT_HW_ADJUST_DDR_DIS_BL   0x0
#define SOC_PETRA_MGMT_HW_ADJUST_DDR_A_TL     0x3
#define SOC_PETRA_MGMT_HW_ADJUST_DDR_P_TL     0x1
#define SOC_PETRA_MGMT_HW_ADJUST_DDR_DIS_TL   0x1

/*
 *	Core frequency
 */
#define SOC_PETRA_MGMT_CORE_FREQ_MIN          150
#define SOC_PETRA_MGMT_CORE_FREQ_MAX          250

/* } */

/*************
 *  MACROS   *
 *************/
/* { */

/*
 *  max value of the count
 *   it's equal to the maximum value count field may get - 1
*/

#define SOC_PETRA_INIT_COUNT_MAX(val)   \
            (SOC_PETRA_FLD_MAX(val) - 1)

/*
 *  Prints phase 1 initialization advance.
 *  Assumes the following variables are defined:
 *   - uint8 silent
 *   - uint32 stage_id
 */
#define SOC_PETRA_INIT_PRINT_ADVANCE(str, phase_id)                                \
{                                                                              \
  if (!silent)                                                                 \
  {                                                                            \
    soc_sand_os_printf("    + Phase %u, %.2u: %s\n\r", phase_id, ++stage_id, str); \
  }                                                                            \
}

#define SOC_PETRA_INIT_PRINT_INTERNAL_ADVANCE(str)                  \
{                                                              \
  if (!silent)                                                 \
  {                                                            \
    soc_sand_os_printf("               ++  %.2u: %s\n\r", ++stage_id, str);  \
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
}SOC_PETRA_MGMT_DBUFF_BOUNDARIES;

typedef struct {
  SOC_PETRA_MGMT_DBUFF_BOUNDARIES  fmc;
  SOC_PETRA_MGMT_DBUFF_BOUNDARIES  mmc;
  SOC_PETRA_MGMT_DBUFF_BOUNDARIES  uc;
  SOC_PETRA_MGMT_DBUFF_BOUNDARIES  fbc_fmc;
  SOC_PETRA_MGMT_DBUFF_BOUNDARIES  fbc_mmc;
  SOC_PETRA_MGMT_DBUFF_BOUNDARIES  fbc_uc;
} SOC_PETRA_INIT_DBUFFS_BDRY;

typedef enum
{
  SOC_PETRA_INIT_MBIST_SMS_TYPE_RF = 0,
  SOC_PETRA_INIT_MBIST_SMS_TYPE_512_D = 1,
  SOC_PETRA_INIT_MBIST_SMS_TYPE_512_S = 2,
  SOC_PETRA_INIT_MBIST_SMS_NOF_TYPES = 3
}SOC_PETRA_INIT_MBIST_SMS_TYPE;

/* } */

/*************
 * GLOBALS   *
 *************/
/* { */

/*
 *  Total number of lines in Soc_petra tables.
 */
static uint32
    Soc_petra_mgmt_tbls_total_lines = 0;
static uint32
    Soc_petra_mgmt_tbls_curr_workload = 0;
static
  SOC_PETRA_INIT_RM_VAL
    Soc_petra_init_rm_values[SOC_PETRA_INIT_NOF_SMS_PLUS_1][SOC_PETRA_INIT_NOF_MEMS_PLUS_1];
static
  uint8
    Soc_petra_init_sms_enable[SOC_PETRA_INIT_NOF_SMS_PLUS_1];

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

/*
 *	Get SMS type
 */
STATIC
  SOC_PETRA_INIT_MBIST_SMS_TYPE
    soc_petra_init_mbist_sms_type_get(
      SOC_SAND_IN int                     unit,
      SOC_SAND_IN uint32                     sms_ndx
    )
{
  SOC_PETRA_INIT_MBIST_SMS_TYPE
    sms_type;

  if (
      ((sms_ndx >= 2) && (sms_ndx <= 8))
      || (sms_ndx == 10)
      || (sms_ndx == 12)
      || (sms_ndx == 14)
      || (sms_ndx == 16)
      || (sms_ndx == 19)
      || (sms_ndx == 22)
      || (sms_ndx == 24)
      || (sms_ndx == 26)
      || ((sms_ndx >= 29) && (sms_ndx <= 31))
      || (sms_ndx == 34)
      || (sms_ndx == 35)
      || (sms_ndx == 38)
      || (sms_ndx == 41)
      || (sms_ndx == 42)
    )
  {
    sms_type = SOC_PETRA_INIT_MBIST_SMS_TYPE_RF;
  }
  else if (
          (sms_ndx == 13)
          || (sms_ndx == 17)
          || (sms_ndx == 20)
          || (sms_ndx == 36)
          || (sms_ndx == 39)
    )
  {
    sms_type = SOC_PETRA_INIT_MBIST_SMS_TYPE_512_D;
  }
  else
  {
    sms_type = SOC_PETRA_INIT_MBIST_SMS_TYPE_512_S;
  }

  return sms_type;
}



/*
 *	Get the number of memories per sms
 */
STATIC
  uint32
    soc_petra_init_mbist_nof_mems_per_sms_get(
      SOC_SAND_IN int                     unit,
      SOC_SAND_IN uint32                     sms_ndx
    )
{
  uint32
    nof_mems = 0;

  switch(sms_ndx)
  {
  case 1:
    nof_mems = 3;
    break;

  case 2:
    nof_mems = 2;
    break;

  case 3:
  case 4:
  case 5:
  case 6:
  case 7:
  case 8:
    nof_mems = 1;
    break;

  case 9:
    nof_mems = 65;
    break;

  case 10:
    nof_mems = 23;
    break;

  case 11:
    nof_mems = 3;
    break;

  case 12:
    nof_mems = 42;
    break;

  case 13:
    nof_mems = 28;
    break;

  case 14:
    nof_mems = 9;
    break;

  case 15:
    nof_mems = 40;
    break;

  case 16:
    nof_mems = 26;
    break;

  case 17:
    nof_mems = 37;
    break;

  case 18:
    nof_mems = 19;
    break;

  case 19:
    nof_mems = 18;
    break;

  case 20:
    nof_mems = 6;
    break;

  case 21:
    nof_mems = 33;
    break;

  case 22:
    nof_mems = 46;
    break;

  case 23:
    nof_mems = 1;
    break;

  case 24:
    nof_mems = 127;
    break;

  case 25:
    nof_mems = 4;
    break;

  case 26:
  case 27:
  case 28:
    nof_mems = 2;
    break;

  case 29:
  case 30:
    nof_mems = 45;
    break;

  case 31:
    nof_mems = 4;
    break;

  case 32:
  case 33:
    nof_mems = 18;
    break;

  case 34:
  case 35:
    nof_mems = 15;
    break;

  case 36:
    nof_mems = 17;
    break;

  case 37:
    nof_mems = 14;
    break;

  case 38:
    nof_mems = 10;
    break;

  case 39:
    nof_mems = 15;
    break;

  case 40:
    nof_mems = 60;
    break;

  case 41:
    nof_mems = 9;
    break;

  default:
    break;
  }

  return nof_mems;
}

/*
 *	Polling on mbist trigger
 */
uint32
  soc_petra_init_mbist_trigger_poll(
    SOC_SAND_IN int unit
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_POLL_INFO
    poll_info;
  uint8
    success;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  regs = soc_petra_regs();

  /*
   *	Poll parameters
   */
  soc_petra_PETRA_POLL_INFO_clear(&poll_info);
  poll_info.expected_value = 0x0;
  poll_info.busy_wait_nof_iters = SOC_PETRA_INIT_POLL_SMS_BUSY_WAIT_ITERATIONS;
  poll_info.timer_nof_iters     = SOC_PETRA_INIT_POLL_SMS_TIMER_ITERATIONS;
  poll_info.timer_delay_msec    = SOC_PETRA_INIT_POLL_SMS_TIMER_DELAY_MSEC;

 res = soc_petra_status_fld_poll_unsafe(
         unit,
         SOC_PETRA_REG_DB_ACC_REF(regs->mcc.sms_reg.cmd_active),
         SOC_PETRA_DEFAULT_INSTANCE,
         &poll_info,
         &success
       );
 SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

 if (success == FALSE)
 {
   SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MGMT_SMS_ACTION_TIMOUT_ERR, 20, exit);
 }

 sal_msleep(1);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_init_mbist_trigger_poll()", 0, 0);
}


/*
 *	Change SMS number
 */
STATIC
  uint32
    soc_petra_init_mbist_change_sms_number(
      SOC_SAND_IN int                     unit,
      SOC_SAND_IN uint32                     sms_ndx
    )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  regs = soc_petra_regs();

/*  soc_sand_os_printf(" Changing SMS number to %d \n", sms_ndx); */

  /*
   *	JPC WIR enter
   */
  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x8000c00c, 10, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /*
   *	JPC WIR start
   */
  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x8000c00d, 15, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  /*
   *	JPC WIR enter - select config
   */
  SOC_PETRA_REG_SET(regs->mcc.tap_gen_reg_1, 0x3, 20, exit);
  SOC_PETRA_REG_SET(regs->mcc.tap_gen_reg_2, 0x0, 25, exit);
  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x80006001, 30, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  /*
   *	JPC WDR enter
   */
  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x80006009, 35, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  /*
   *	JPC WDR start
   */
  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x8000600a, 40, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);

  /*
   *	JPC WIR write SMS number
   */
  SOC_PETRA_REG_SET(regs->mcc.tap_gen_reg_1, sms_ndx, 45, exit);
  SOC_PETRA_REG_SET(regs->mcc.tap_gen_reg_2, 0x0, 50, exit);
  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x80006001, 55, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_init_mbist_change_sms_number()", 0, 0);
}

STATIC
  uint32
    soc_petra_init_mbist_command(
      SOC_SAND_IN int                     unit,
      SOC_SAND_IN uint32                     mbist_cmd
    )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  regs = soc_petra_regs();

/*  soc_sand_os_printf(" Performing MBist command %d \n", mbist_cmd); */

  /*
   *	SMS WIR enter
   */
  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x80006006, 10, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /*
   *	SMS WIR Start
   */
  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x80006007, 15, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  /*
   *	SMS WIR Command
   */
  SOC_PETRA_REG_SET(regs->mcc.tap_gen_reg_1, mbist_cmd, 20, exit);
  SOC_PETRA_REG_SET(regs->mcc.tap_gen_reg_2, 0, 25, exit);
  SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x80006001, 30, exit);
  res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 44, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_init_mbist_command()", 0, 0);
}


/*
 *	Load the RM values (from sweep after)
 */
STATIC
  uint32
    soc_petra_init_rm_values_load(
      SOC_SAND_IN int    unit
    )
{
  uint32
    res;
  uint32
    mem_ndx,
    sms_ndx;
  SOC_PETRA_INIT_RM_VAL
    rm_val;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  res = SOC_SAND_OK; sal_memset(&Soc_petra_init_rm_values, 0x0, sizeof(Soc_petra_init_rm_values));
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = SOC_SAND_OK; sal_memset(&Soc_petra_init_sms_enable, 0x0, sizeof(Soc_petra_init_sms_enable));
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  for (sms_ndx = 1; sms_ndx < SOC_PETRA_INIT_NOF_SMS_PLUS_1; ++sms_ndx)
  {
    for (mem_ndx = 1; mem_ndx < SOC_PETRA_INIT_NOF_MEMS_PLUS_1; ++mem_ndx)
    {
      rm_val.enable = FALSE;
      rm_val.val = 0;

      /*
       *	Set the right values
       */
      if (sms_ndx == 10)
      {
        if ((mem_ndx == 1) || (mem_ndx == 4) || (mem_ndx == 5) || (mem_ndx == 6) || (mem_ndx == 12))
        {
          rm_val.enable = TRUE;
          rm_val.val = 4;
        }
        else if ((mem_ndx == 8)  || (mem_ndx == 9))
        {
          rm_val.enable = TRUE;
          rm_val.val = 2;
        }
        else if ((mem_ndx == 10)  || (mem_ndx == 11))
        {
          rm_val.enable = TRUE;
          rm_val.val = 1;
        }
      }
      else if ((sms_ndx == 12) && (mem_ndx == 24))
      {
        rm_val.enable = TRUE;
        rm_val.val = 7;
      }
      else if ((sms_ndx == 19) && ((mem_ndx == 6) || (mem_ndx == 11)))
      {
        rm_val.enable = TRUE;
        rm_val.val = 6;
      }
      else if (sms_ndx == 22)
      {
        if ((mem_ndx == 2) || (mem_ndx == 3) || (mem_ndx == 8))
        {
          rm_val.enable = TRUE;
          rm_val.val = 6;
        }
        else if (
                  ((mem_ndx >= 4)  && (mem_ndx <= 7))
                  || ((mem_ndx >= 9)  && (mem_ndx <= 12))
                  || ((mem_ndx >= 16)  && (mem_ndx <= 24))
                  || ((mem_ndx >= 16)  && (mem_ndx <= 24))
                )
        {
          rm_val.enable = TRUE;
          rm_val.val = 2;
        }
        else if ((mem_ndx == 13) || (mem_ndx == 42))
        {
          rm_val.enable = TRUE;
          rm_val.val = 4;
        }
      }
      else if (sms_ndx == 24)
      {
        if ((mem_ndx >= 25)  && (mem_ndx <= 30))
        {
          rm_val.enable = TRUE;
          rm_val.val = 4;
        }
        else if (mem_ndx == 31)
        {
          rm_val.enable = TRUE;
          rm_val.val = 2;
        }
        else if ((mem_ndx >= 104)  && (mem_ndx <= 127))
        {
          rm_val.enable = TRUE;
          rm_val.val = 7;
        }
      }
      else if (sms_ndx == 26)
      {
        if ((mem_ndx == 1) || (mem_ndx == 2))
        {
          rm_val.enable = TRUE;
          rm_val.val = 6;
        }
      }
      else if (sms_ndx == 29)
      {
        if (mem_ndx == 9)
        {
          rm_val.enable = TRUE;
          rm_val.val = 6;
        }
        else if ((mem_ndx >= 26)  && (mem_ndx <= 29))
        {
          rm_val.enable = TRUE;
          rm_val.val = 7;
        }
      }
      else if (sms_ndx == 30)
      {
        if (mem_ndx == 9)
        {
          rm_val.enable = TRUE;
          rm_val.val = 6;
        }
        else if ((mem_ndx >= 26)  && (mem_ndx <= 29))
        {
          rm_val.enable = TRUE;
          rm_val.val = 7;
        }
      }
      else if (sms_ndx == 38)
      {
        if (mem_ndx == 1)
        {
          rm_val.enable = TRUE;
          rm_val.val = 4;
        }
        else if ((mem_ndx == 2)  || (mem_ndx == 6))
        {
          rm_val.enable = TRUE;
          rm_val.val = 6;
        }
        else if (mem_ndx == 10)
        {
          rm_val.enable = TRUE;
          rm_val.val = 2;
        }
      }
      else if (sms_ndx == 41)
      {
        if (mem_ndx == 8)
        {
          rm_val.enable = TRUE;
          rm_val.val = 4;
        }
      }

      Soc_petra_init_rm_values[sms_ndx][mem_ndx].enable = rm_val.enable;
      Soc_petra_init_rm_values[sms_ndx][mem_ndx].val = rm_val.val;

      if (rm_val.enable == TRUE)
      {
        Soc_petra_init_sms_enable[sms_ndx] = TRUE;
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_init_rm_values_load()", 0, 0);
}

STATIC
  uint32
    soc_petra_init_rm_values_set(
      SOC_SAND_IN int    unit
    )
{
  uint32
    addr_size = 0,
    nof_mems,
    reg_val = 0,
    res;
  SOC_PETRA_REGS
    *regs;
  uint32
    mem_idx,
    mem_ndx,
    sms_ndx;
  SOC_PETRA_INIT_MBIST_SMS_TYPE
    sms_type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);


  regs = soc_petra_regs();

  /*
   * Set MBIST mode
   */
  SOC_PETRA_REG_SET(regs->mcc.memory_configuration_reg, 0x1, 12, exit);

  for (sms_ndx = 1; sms_ndx < SOC_PETRA_INIT_NOF_SMS_PLUS_1; ++sms_ndx)
  {
    if (Soc_petra_init_sms_enable[sms_ndx] == FALSE)
    {
      continue;
    }

    /*
     *	Get SMS type
     */
    sms_type = soc_petra_init_mbist_sms_type_get(
                unit,
                sms_ndx
              );

    nof_mems = soc_petra_init_mbist_nof_mems_per_sms_get(
                 unit,
                 sms_ndx
               );

    /*
     *	Change SMS number
     */
    res = soc_petra_init_mbist_change_sms_number(
            unit,
            sms_ndx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if (sms_type == SOC_PETRA_INIT_MBIST_SMS_TYPE_RF)
    {
      if (sms_ndx < 9)
      {
        addr_size = 7;
      }
      else if (sms_ndx <= 10)
      {
        addr_size = 10;
      }
      else if (sms_ndx <= 14)
      {
        addr_size = 7;
      }
      else if (sms_ndx <= 19)
      {
        addr_size = 8;
      }
      else if (sms_ndx <= 24)
      {
        addr_size = 9;
      }
      else if (sms_ndx <= 30)
      {
        addr_size = 8;
      }
      else if (sms_ndx <= 35)
      {
        addr_size = 6;
      }
      else if (sms_ndx <= 38)
      {
        addr_size = 9;
      }
      else if (sms_ndx <= 41)
      {
        addr_size = 8;
      }

     res = soc_petra_init_mbist_command(
            unit,
            19
          );
     SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

      /*
       *	SMS WDR enter
       */
      SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x80000003, 20, exit);
      res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

      reg_val = 0;
      reg_val |= SOC_SAND_SET_BITS_RANGE(0x2, 6, 5);
      SOC_PETRA_REG_SET(regs->mcc.tap_gen_reg_1, reg_val, 24, exit);

      reg_val = 0;
      SOC_PETRA_REG_SET(regs->mcc.tap_gen_reg_2, reg_val, 26, exit);

      reg_val = 0;
      reg_val |= SOC_SAND_SET_BITS_RANGE(0x1, 0, 0);
      reg_val |= SOC_SAND_SET_BITS_RANGE(0x4, 14 + addr_size, 12 + addr_size);
      SOC_PETRA_REG_SET(regs->mcc.tap_tms_value_reg_1, reg_val, 28, exit);

      reg_val = 0;
      SOC_PETRA_REG_SET(regs->mcc.tap_tms_value_reg_2, reg_val, 30, exit);

      /*
       *	User-Defined command
       */
      reg_val = 0;
      reg_val |= SOC_SAND_SET_BITS_RANGE(0x1, 4, 4);
      reg_val |= SOC_SAND_SET_BITS_RANGE(16 + addr_size, 18, 12);
      reg_val |= SOC_SAND_SET_BITS_RANGE(0x1, 31, 31);
      SOC_PETRA_REG_SET(regs->mcc.sms_reg, reg_val, 32, exit);
      res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);

      /*
       *	Return to "test idle" state
       */
      reg_val = 0;
      SOC_PETRA_REG_SET(regs->mcc.tap_gen_reg_1, reg_val, 36, exit);
      SOC_PETRA_REG_SET(regs->mcc.tap_gen_reg_2, reg_val, 38, exit);

      reg_val = 0xC;
      SOC_PETRA_REG_SET(regs->mcc.tap_tms_value_reg_1, reg_val, 40, exit);

      reg_val = 0;
      SOC_PETRA_REG_SET(regs->mcc.tap_tms_value_reg_2, reg_val, 42, exit);

      /*
       *	User-Defined command
       */
      reg_val = 0;
      reg_val |= SOC_SAND_SET_BITS_RANGE(0x1, 4, 4);
      reg_val |= SOC_SAND_SET_BITS_RANGE(6, 18, 12);
      reg_val |= SOC_SAND_SET_BITS_RANGE(0x1, 31, 31);
      SOC_PETRA_REG_SET(regs->mcc.sms_reg, reg_val, 44, exit);
      res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 46, exit);

      res = soc_petra_init_mbist_command(
              unit,
              13
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 48, exit);
    }
    else
    {
      res = soc_petra_init_mbist_command(
              unit,
              10
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    }

    /*
     *	SMS WDR enter
     */
    SOC_PETRA_REG_SET(regs->mcc.sms_reg, 0x80000003, 52, exit);
    res = soc_petra_init_mbist_trigger_poll(unit);  SOC_SAND_CHECK_FUNC_RESULT(res, 54, exit);

    reg_val = 0;
    SOC_PETRA_REG_SET(regs->mcc.tap_gen_reg_1, reg_val, 56, exit);
    SOC_PETRA_REG_SET(regs->mcc.tap_gen_reg_2, reg_val, 58, exit);

    reg_val = 0;
    reg_val |= SOC_SAND_SET_BITS_RANGE(9, 3, 0);
    SOC_PETRA_REG_SET(regs->mcc.tap_tms_value_reg_1, reg_val, 60, exit);

    reg_val = 0;
    SOC_PETRA_REG_SET(regs->mcc.tap_tms_value_reg_2, reg_val, 62, exit);

    /*
     *	User-Defined command
     */
    reg_val = 0;
    reg_val |= SOC_SAND_SET_BITS_RANGE(0x1, 4, 4);
    reg_val |= SOC_SAND_SET_BITS_RANGE(6, 18, 12);
    reg_val |= SOC_SAND_SET_BITS_RANGE(0x1, 31, 31);
    SOC_PETRA_REG_SET(regs->mcc.sms_reg, reg_val, 64, exit);
    res = soc_petra_init_mbist_trigger_poll(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 68, exit);

    for (mem_idx = 0; mem_idx < nof_mems; ++mem_idx)
    {
      /*
       *	Decreasing loop
       */
      mem_ndx = nof_mems - mem_idx;

      reg_val = 0;
      if (sms_type == SOC_PETRA_INIT_MBIST_SMS_TYPE_RF)
      {
        reg_val |= SOC_SAND_SET_BITS_RANGE(
          Soc_petra_init_rm_values[sms_ndx][mem_ndx].enable, 1, 1);
        reg_val |= SOC_SAND_SET_BITS_RANGE(
          Soc_petra_init_rm_values[sms_ndx][mem_ndx].val, 5, 2);
      }
      else if (sms_type == SOC_PETRA_INIT_MBIST_SMS_TYPE_512_S)
      {
        reg_val |= SOC_SAND_SET_BITS_RANGE(
          Soc_petra_init_rm_values[sms_ndx][mem_ndx].enable, 4, 4);
        reg_val |= SOC_SAND_SET_BITS_RANGE(
          Soc_petra_init_rm_values[sms_ndx][mem_ndx].val, 3, 0);
      }
      else if (sms_type == SOC_PETRA_INIT_MBIST_SMS_TYPE_512_D)
      {
        reg_val |= SOC_SAND_SET_BITS_RANGE(
          Soc_petra_init_rm_values[sms_ndx][mem_ndx].enable, 4, 4);
        reg_val |= SOC_SAND_SET_BITS_RANGE(
          Soc_petra_init_rm_values[sms_ndx][mem_ndx].val, 3, 0);
        reg_val |= SOC_SAND_SET_BITS_RANGE(
          Soc_petra_init_rm_values[sms_ndx][mem_ndx].enable, 9, 9);
        reg_val |= SOC_SAND_SET_BITS_RANGE(
          Soc_petra_init_rm_values[sms_ndx][mem_ndx].val, 8, 5);
      }
      SOC_PETRA_REG_SET(regs->mcc.tap_gen_reg_1, reg_val, 70, exit);

      reg_val = 0;
      SOC_PETRA_REG_SET(regs->mcc.tap_gen_reg_2, reg_val, 72, exit);

      reg_val = 0;
      if (
          (sms_type == SOC_PETRA_INIT_MBIST_SMS_TYPE_RF)
          || (sms_type == SOC_PETRA_INIT_MBIST_SMS_TYPE_512_S)
         )
      {
        reg_val |= SOC_SAND_SET_BITS_RANGE(0x1, 0, 0);
        reg_val |= SOC_SAND_SET_BITS_RANGE(0x4, 7, 5);
      }
      else if (sms_type == SOC_PETRA_INIT_MBIST_SMS_TYPE_512_D)
      {
        reg_val |= SOC_SAND_SET_BITS_RANGE(0x1, 0, 0);
        reg_val |= SOC_SAND_SET_BITS_RANGE(0x4, 13, 11);
      }
      SOC_PETRA_REG_SET(regs->mcc.tap_tms_value_reg_1, reg_val, 76, exit);

      reg_val = 0;
      SOC_PETRA_REG_SET(regs->mcc.tap_tms_value_reg_2, reg_val, 78, exit);

      /*
       *	User-Defined command
       */
      reg_val = 0;
      reg_val |= SOC_SAND_SET_BITS_RANGE(0x1, 4, 4);
      if (
          (sms_type == SOC_PETRA_INIT_MBIST_SMS_TYPE_RF)
          || (sms_type == SOC_PETRA_INIT_MBIST_SMS_TYPE_512_S)
         )
      {
        reg_val |= SOC_SAND_SET_BITS_RANGE(9, 18, 12);
      }
      else if (sms_type == SOC_PETRA_INIT_MBIST_SMS_TYPE_512_D)
      {
        reg_val |= SOC_SAND_SET_BITS_RANGE(15, 18, 12);
      }
      reg_val |= SOC_SAND_SET_BITS_RANGE(0x1, 31, 31);
      SOC_PETRA_REG_SET(regs->mcc.sms_reg, reg_val, 80, exit);
      res = soc_petra_init_mbist_trigger_poll(unit);
      SOC_SAND_CHECK_FUNC_RESULT(res, 82, exit);
    }
  }

  /*
   *	Reset SFP
   */
  SOC_PETRA_REG_SET(regs->mcc.mbist_resets_reg, 0x4, 84, exit);
  SOC_PETRA_REG_SET(regs->mcc.mbist_resets_reg, 0x6, 86, exit);
  SOC_PETRA_REG_SET(regs->mcc.mbist_resets_reg, 0x0, 88, exit);


  /*
   * Exit MBist mode
   */
  SOC_PETRA_REG_SET(regs->mcc.memory_configuration_reg, 0x0, 118, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_init_rm_values_set()", 0, 0);
}


STATIC uint32
  soc_petra_init_mem_correction_enable(
    SOC_SAND_IN int    unit
  )
{
  uint32
    reg_val = 0,
    reg_val2 = 0,
    poll_exit_place = 0,
    res;
  SOC_PETRA_POLL_INFO
    poll_info;
  uint8
    poll_success,
    is_low_sim_active;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INIT_MEM_CORRECTION_ENABLE);

#ifndef SAND_LOW_LEVEL_SIMULATION
  is_low_sim_active = FALSE;
#else
  is_low_sim_active = soc_sand_low_is_sim_active_get();
#endif

  regs = soc_petra_regs();

  soc_petra_PETRA_POLL_INFO_clear(&poll_info);
  poll_info.expected_value = 0x0;
  poll_info.busy_wait_nof_iters = SOC_PETRA_MGMT_INIT_SMS_BUSY_WAIT_ITERATIONS;
  poll_info.timer_nof_iters     = SOC_PETRA_MGMT_INIT_SMS_TIMER_ITERATIONS;
  poll_info.timer_delay_msec    = SOC_PETRA_MGMT_INIT_SMS_TIMER_DELAY_MSEC;

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
  SOC_PETRA_REG_SET(regs->mcc.tap_config_reg, 0x11, 28, exit);

  poll_info.expected_value = 0x0;
  res = soc_petra_status_fld_poll_unsafe(
          unit,
          SOC_PETRA_REG_DB_ACC_REF(regs->mcc.tap_config_reg.tap_cmd),
          SOC_PETRA_DEFAULT_INSTANCE,
          &poll_info,
          &poll_success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 29, exit);

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

  SOC_PETRA_REG_SET(regs->mcc.tap_gen_reg_1, 0x2a, 47, exit);
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
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MGMT_SMS_ACTION_TIMOUT_ERR, 74, exit);
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
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MGMT_SMS_ACTION_TIMOUT_ERR, 92, exit);
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
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MGMT_SMS_ACTION_TIMOUT_ERR, 109, exit);
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
  sal_msleep(SOC_PETRA_INIT_MEM_CORR_DELAY_MSEC);
  poll_info.expected_value = 0x1;
  res = soc_petra_status_fld_poll_unsafe(
          unit,
          SOC_PA_REG_DB_ACC_REF(regs->mcc.sfp_ready_reg.sfp_ready),
          SOC_PETRA_DEFAULT_INSTANCE,
          &poll_info,
          &poll_success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 200, exit);

  if (poll_success == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MGMT_SMS_ACTION_TIMOUT_ERR, 117, exit);
  }

  poll_info.expected_value = 0x7fffffff;
  res = soc_petra_status_fld_poll_unsafe(
          unit,
          SOC_PA_REG_DB_ACC_REF(regs->mcc.ready_sms_statuses1.ready_sms1),
          SOC_PETRA_DEFAULT_INSTANCE,
          &poll_info,
          &poll_success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 202, exit);
  if (poll_success == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MGMT_SMS_ACTION_TIMOUT_ERR, 201, exit);
  }

  poll_info.expected_value = 0x3ff;
  res = soc_petra_status_fld_poll_unsafe(
          unit,
          SOC_PA_REG_DB_ACC_REF(regs->mcc.ready_sms_statuses2.ready_sms2),
          SOC_PETRA_DEFAULT_INSTANCE,
          &poll_info,
          &poll_success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 204, exit);

  if (poll_success == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MGMT_SMS_ACTION_TIMOUT_ERR, 119, exit);
  }

  /*
   * Exit smart mode
   */
  SOC_PETRA_REG_SET(regs->mcc.memory_configuration_reg, 0x0, 118, exit);

  /*
   * Initiate load
   */
  SOC_PETRA_REG_SET(regs->mcc.memory_configuration_reg, 0x8, 120, exit);
  sal_msleep(SOC_PETRA_INIT_MEM_CORR_DELAY_MSEC);
  /*
   * Exit load
   */
  SOC_PETRA_REG_SET(regs->mcc.memory_configuration_reg, 0x0, 121, exit);

  /*
   * Verify if the RM settings should be set
   */
  SOC_PA_REG_GET(regs->eci.udr_reg1_reg, reg_val, 125, exit);
  SOC_PA_REG_GET(regs->mcc.udr_mode_reg, reg_val2, 126, exit);

  if (SOC_SAND_GET_BIT(reg_val, 0) != SOC_SAND_GET_BIT(reg_val2, 0))
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MEM_CORRECTION_RM_BIT_INCONSISTENT_ERR, 1100+reg_val, exit);
  }
  else if (SOC_SAND_GET_BIT(reg_val, 0) == 1)
  {
    /*
     *	Run RM correction
     */
    res = soc_petra_init_rm_values_load(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 122, exit);

    res = soc_petra_init_rm_values_set(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 123, exit);
  }

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
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_init_mem_correction_enable()", 0, 0);
}

uint32
  soc_petra_init_qdr_size_to_bits(
    SOC_SAND_IN  SOC_PETRA_HW_QDR_SIZE_MBIT     qdr_size
  )
{
  uint32
    qdr_size_bits;

  switch(qdr_size) {
  case SOC_PETRA_HW_QDR_SIZE_MBIT_18:
    qdr_size_bits = (18 * 1024 * 1024);
    break;
  case SOC_PETRA_HW_QDR_SIZE_MBIT_36:
    qdr_size_bits = (36 * 1024 * 1024);
    break;
  case SOC_PETRA_HW_QDR_SIZE_MBIT_72:
    qdr_size_bits = (72 * 1024 * 1024);
    break;
  case SOC_PETRA_HW_QDR_SIZE_MBIT_144:
    qdr_size_bits = (144 * 1024 * 1024);
    break;
  default:
    qdr_size_bits = 0;
  }

  return qdr_size_bits;
}

STATIC uint32
  soc_petra_init_dram_nof_buffs_calc(
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

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INIT_DRAM_NOF_BUFFS_CALC);
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
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_init_dram_nof_buffs_calc()", 0, 0);
}

/*
 * Get the number of fbc (cache) buffers needed for given amount of
 * DRAM buffers.
 */
STATIC uint32
  soc_petra_init_dram_fbc_buffs_get(
    SOC_SAND_IN  uint32  buffs_without_fbc,
    SOC_SAND_IN  uint32  buff_size_bytes,
    SOC_SAND_OUT uint32 *fbc_nof_bufs
  )
{
  uint32
    fbcs_for_buff,
    nof_fbc_buffs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INIT_DRAM_FBC_BUFFS_GET);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(
    buff_size_bytes, SOC_PETRA_ITM_DBUFF_SIZE_BYTES_MIN, SOC_PETRA_ITM_DBUFF_SIZE_BYTES_MAX,
    SOC_PETRA_ITM_DRAM_BUF_SIZE_OUT_OF_RANGE_ERR, 10, exit
  );

  fbcs_for_buff = SOC_SAND_DIV_ROUND_UP(buff_size_bytes, SOC_PETRA_INIT_DRAM_BYTES_FOR_FBC);
  nof_fbc_buffs = SOC_SAND_DIV_ROUND_UP(buffs_without_fbc, fbcs_for_buff * SOC_PETRA_INIT_DRAM_FBC_SEQUENCE_SIZE);

  *fbc_nof_bufs = nof_fbc_buffs;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "soc_petra_init_dram_fbc_buffs_get()", 0, 0);
}

STATIC uint32
  soc_petra_init_dram_max_without_fbc_get(
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

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INIT_DRAM_MAX_WITHOUT_FBC_GET);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(
    buff_size_bytes, SOC_PETRA_ITM_DBUFF_SIZE_BYTES_MIN, SOC_PETRA_ITM_DBUFF_SIZE_BYTES_MAX,
    SOC_PETRA_ITM_DRAM_BUF_SIZE_OUT_OF_RANGE_ERR, 10, exit
  );

  soc_sand_u64_multiply_longs(
    buffs_with_fbc,
    (SOC_PETRA_INIT_DRAM_FBC_SEQUENCE_SIZE * buff_size_bytes),
    &dividend
  );

  divisor = SOC_PETRA_INIT_DRAM_FBC_SEQUENCE_SIZE * buff_size_bytes + SOC_PETRA_INIT_DRAM_BYTES_FOR_FBC;

  soc_sand_u64_devide_u64_long(
    &dividend,
    divisor,
    &buffs_no_fbc
  );

  soc_sand_u64_to_long(&buffs_no_fbc, buffs_without_fbc);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "soc_petra_init_dram_max_without_fbc_get()", 0, 0);
}

STATIC uint32
  soc_petra_init_dram_buff_boudaries_calc(
    SOC_SAND_IN  uint32                     total_buffs,
    SOC_SAND_IN  SOC_PETRA_ITM_DBUFF_SIZE_BYTES   dbuff_size,
    SOC_SAND_OUT SOC_PETRA_INIT_DBUFFS_BDRY       *dbuffs
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

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_INIT_DRAM_BUFF_BOUDARIES_CALC);

  SOC_SAND_CHECK_NULL_INPUT(dbuffs);

  /*
   *  Full Multicast
   */
  dbuffs->fmc.start  = 0;
  dbuffs->fmc.end    = SOC_SAND_RNG_LAST(dbuffs->fmc.start, SOC_PETRA_ITM_DBUFF_FMC_MAX);

  /*
   *  Mini Multicast
   */
  dbuffs->mmc.start  = SOC_SAND_RNG_NEXT(dbuffs->fmc.end);
  dbuffs->mmc.end    = SOC_SAND_RNG_LAST(dbuffs->mmc.start, SOC_PETRA_ITM_DBUFF_MMC_MAX);

  /*
   *  FBC-size
   */
  res = soc_petra_init_dram_fbc_buffs_get(
          SOC_SAND_RNG_COUNT(dbuffs->fmc.end, dbuffs->fmc.start),
          dbuff_size,
          &fmc_fbc_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_init_dram_fbc_buffs_get(
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

  res = soc_petra_init_dram_max_without_fbc_get(
          uc_plus_fbc,
          dbuff_size,
          &uc_no_fbc
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_init_dram_fbc_buffs_get(
          uc_no_fbc,
          dbuff_size,
          &uc_fbc_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  /* Set minimal gap between DRAM regular and FBC buffers */
  uc_no_fbc_orig = uc_no_fbc;
  uc_no_fbc -= SOC_PETRA_INIT_DRAM_BUFF_TO_FBC_DELTA_MIN;

  if (uc_no_fbc + uc_fbc_size > uc_plus_fbc)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DRAM_BUFFERS_UC_FBC_OVERFLOW_ERR, 60, exit);
  }

  dbuffs->uc.end = SOC_SAND_RNG_LAST(dbuffs->uc.start, uc_no_fbc);

  /*
   *  Set the FBC to start according to the original
   *  UC buffers calculation. This guarantees the minimal gap of
   *  SOC_PETRA_INIT_DRAM_BUFF_TO_FBC_DELTA_MIN between regular
   *  and FBC buffers.
   */
  dbuffs->fbc_uc.start  = SOC_SAND_RNG_NEXT(SOC_SAND_RNG_LAST(dbuffs->uc.start, uc_no_fbc_orig));
  dbuffs->fbc_uc.end    = SOC_SAND_RNG_LAST(dbuffs->fbc_uc.start, uc_fbc_size);

  if (dbuffs->fbc_uc.end >= dbuffs->fbc_mmc.start)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DRAM_BUFFERS_FBC_OVERFLOW_ERR, 70, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "soc_petra_init_dram_buff_boudaries_calc()", 0, 0);
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
  soc_petra_mgmt_init_before_blocks_oor(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_ITM_DBUFF_SIZE_BYTES  dbuff_size,
    SOC_SAND_IN  SOC_PETRA_INIT_DBUFFS_BDRY      *dbuffs_bdries,
    SOC_SAND_IN  SOC_PETRA_HW_ADJ_QDR            *hw_adjust_qdr,
    SOC_SAND_IN  SOC_PETRA_HW_ADJ_FABRIC         *hw_adjust_fbr,
    SOC_SAND_IN  SOC_PETRA_HW_ADJ_DDR            *hw_adjust_ddr,
    SOC_SAND_IN  SOC_PETRA_HW_ADJ_STAT_IF        *hw_adjust_stat_if,
    SOC_SAND_IN  SOC_PETRA_HW_ADJ_NIF            *hw_adjust_nif
  )
{
  uint32
    reg_val,
    fld_val,
    res;
  uint32
    fatp_nof_ports;
  uint8
    is_stag,
    is_fap2x_coexist;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_INIT_BEFORE_BLOCKS_OOR);

  SOC_SAND_CHECK_NULL_INPUT(hw_adjust_qdr);
  SOC_SAND_CHECK_NULL_INPUT(hw_adjust_fbr);
  SOC_SAND_CHECK_NULL_INPUT(hw_adjust_ddr);
  SOC_SAND_CHECK_NULL_INPUT(hw_adjust_nif);

  regs = soc_petra_regs();
  is_fap2x_coexist = soc_petra_sw_db_is_fap2x_in_system_get(unit);

  /************************************************************************/
  /*  Configure DRAM PLL                                              */
  /************************************************************************/

  /* In-reset */
  SOC_PETRA_FLD_SET(regs->eci.pll_resets_reg.ddr_pll_rst_n, 0x0, 5, exit);
  /* QDR also */
  SOC_PETRA_FLD_SET(regs->eci.pll_resets_reg.qdr_pll_rst_n, 0x0, 6, exit);

  SOC_PETRA_FLD_SET(regs->eci.ddr_pll_config_reg.ddr_pll_m, hw_adjust_ddr->pll_conf.m, 10, exit);
  SOC_PETRA_FLD_SET(regs->eci.ddr_pll_config_reg.ddr_pll_n, hw_adjust_ddr->pll_conf.n, 12, exit);
  /* Out-of-reset */
  SOC_PETRA_FLD_SET(regs->eci.pll_resets_reg.ddr_pll_rst_n, 0x1, 28, exit);

  /************************************************************************/
  /*  Configure QDR PLL                                                   */
  /************************************************************************/

  /*
   * Always TRUE for Soc_petra-A. Set for consistancy
   */
  soc_petra_sw_db_qdr_enable_set(unit, TRUE);

  fld_val = (hw_adjust_qdr->is_core_clock_freq)?0x1:0x0;
  SOC_PETRA_FLD_SET(regs->eci.pll_resets_reg.qdr_pll_sel, fld_val, 30, exit);

  if (!hw_adjust_qdr->is_core_clock_freq)
  {
    /*
     *  Configure PLL
     */
    /* In-reset */
    SOC_PETRA_FLD_SET(regs->eci.qdr_pll_config_reg.qdr_pll_m, hw_adjust_qdr->pll_conf.m, 34, exit);
    SOC_PETRA_FLD_SET(regs->eci.qdr_pll_config_reg.qdr_pll_n, hw_adjust_qdr->pll_conf.n, 36, exit);
    SOC_PETRA_FLD_SET(regs->eci.qdr_pll_config_reg.qdr_pll_p, hw_adjust_qdr->pll_conf.p, 40, exit);

    /* Out-of-reset */
    SOC_PETRA_FLD_SET(regs->eci.pll_resets_reg.qdr_pll_rst_n, 0x1, 28, exit);
  }

  /* Delay after DRAM/QDR PLL Out-Of-Reset */
  sal_msleep(SOC_PETRA_MGMT_INIT_TIMER_DELAY_MSEC);

  /************************************************************************/
  /* Software Database                                                    */
  /************************************************************************/

  res = soc_petra_stat_if_info_set_unsafe(
          unit,
          &hw_adjust_stat_if->if_conf
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 2, exit);

  if (hw_adjust_nif->fat_pipe.enable)
  {
    fatp_nof_ports = hw_adjust_nif->fat_pipe.conf.nof_ports;

    if (SOC_PETRA_REV_ABOVE_A1)
    {
      fld_val = 0x1;
      SOC_PA_FLD_SET(regs->eci.oc768c_and_misc_reg.oc768c_mode, fld_val, 15, exit);
    }
    else
    {
      reg_val = 0x9db6a18f;
      SOC_PA_REG_SET(regs->eci.fatp_conf_reg, reg_val, 7, exit);

      fld_val = 0x1;
      SOC_PA_FLD_SET(regs->eci.fatp_enable_reg.fatp_enable, fld_val, 10, exit);
    }
  }
  else
  {
    fatp_nof_ports = 0;
  }

  soc_petra_sw_db_fat_pipe_nof_ports_set(unit, fatp_nof_ports);


  /************************************************************************/
  /* DRAM Buffers                                                         */
  /************************************************************************/

  /*
   *  ECI
   */

  res = soc_petra_itm_dbuff_size2internal(
          dbuff_size,
          &fld_val
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  SOC_PA_FLD_SET(regs->eci.general_controls_reg.dbuff_size, fld_val, 25, exit);

  SOC_PA_FLD_SET(
    regs->eci.unicast_dbuff_pointers_start_reg.uc_db_ptr_start, dbuffs_bdries->uc.start, 27, exit
  );
  SOC_PA_FLD_SET(
    regs->eci.unicast_dbuff_pointers_end_reg.uc_db_ptr_end, dbuffs_bdries->uc.end, 30, exit
  );

  SOC_PA_FLD_SET(
    regs->eci.mini_multicast_dbuff_pointers_start_reg.mn_mul_db_ptr_start, dbuffs_bdries->mmc.start, 40, exit
  );
  SOC_PA_FLD_SET(
    regs->eci.mini_multicast_dbuff_pointers_end_reg.mn_mul_db_ptr_end, dbuffs_bdries->mmc.end, 50, exit
  );

  SOC_PA_FLD_SET(
    regs->eci.full_multicast_dbuff_pointers_start_reg.fl_mul_db_ptr_start, dbuffs_bdries->fmc.start, 60, exit
  );
  SOC_PA_FLD_SET(
    regs->eci.full_multicast_dbuff_pointers_end_reg.fl_mul_db_ptr_end, dbuffs_bdries->fmc.end, 70, exit
  );

  /************************************************************************/
  /* QDR protection type                                                  */
  /************************************************************************/

  if (hw_adjust_qdr->enable == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MGMT_BASIC_CONF_NOT_SUPPLIED_ERR, 80, exit);
  }

  fld_val =
    (hw_adjust_qdr->protection_type == SOC_PETRA_HW_QDR_PROTECT_TYPE_PARITY)?0x1:0x0;

  SOC_PA_FLD_SET(regs->eci.general_controls_reg.qdr_par_sel, fld_val , 90, exit);

  /************************************************************************/
  /* Fabric                                                               */
  /************************************************************************/

  /*
   *  Mesh Mode
   */
  if (
      (hw_adjust_fbr->connect_mode == SOC_PETRA_FABRIC_CONNECT_MODE_MESH) &&
      (!is_fap2x_coexist)
     )
  {
    SOC_PA_FLD_SET(regs->eci.general_controls_reg.mesh_mode, 0x1, 100, exit);
  }
  else
  {
    /* Not enabled, also for BACK2BACK and single-context devices) */
    SOC_PA_FLD_SET(regs->eci.general_controls_reg.mesh_mode, 0x0, 110, exit);
  }

  /*
   *  CRC enable and send
   */
  fld_val = 0x1;
  SOC_PA_FLD_SET(regs->eci.general_controls_reg.add_dram_crc, fld_val, 120, exit);

  /*
   *	Disable fabric CRC only if fap20/fap21 in system
   */
  fld_val = SOC_SAND_BOOL2NUM(is_fap2x_coexist);
  SOC_PA_FLD_SET(regs->eci.general_controls_reg.no_fab_crc, fld_val, 130, exit);

  if (soc_petra_sw_db_is_fap20_in_system_get(unit))
  {
    fld_val = 0x1; /* Fap20v header */
  }
  else
  {
    fld_val = 0x0; /* Soc_petra header */
  }
  SOC_PA_FLD_SET(regs->eci.general_controls_reg.hdr_type, fld_val, 140, exit);

  res = soc_petra_ports_ftmh_extension_set_unsafe(
          unit,
          hw_adjust_fbr->ftmh_extension
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

  is_stag = soc_petra_sw_db_stag_enable_get(unit);
  fld_val = SOC_SAND_BOOL2NUM(is_stag);

  SOC_PA_FLD_SET(regs->eci.general_controls_reg.stat_tag_en, fld_val, 160, exit);

  /*
   *  MCC + Mbist Resets
   */
  SOC_PA_FLD_SET(regs->eci.soc_petra_soft_reset_reg.mcc_reset, 0x0, 162, exit);

  res = soc_petra_init_mem_correction_enable(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 175, exit);

  /*
   *  DRAM Duty Cycle
   */
  if (SOC_PETRA_REV_ABOVE_A1)
  {
    SOC_PA_FLD_SET(regs->eci.oc768c_and_misc_reg.dcf_a, 0x2, 200, exit);
    SOC_PA_FLD_SET(regs->eci.oc768c_and_misc_reg.dcf_p, 0x1, 202, exit);
    SOC_PA_FLD_SET(regs->eci.oc768c_and_misc_reg.dcf_disable, 0x0, 204, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_mgmt_init_before_blocks_oor()", 0, 0);
}

/*
 *  Complete the initialization based on pre-Out-Of-Reset configuration
 */
STATIC uint32
  soc_petra_mgmt_init_after_blocks_oor(
    SOC_SAND_IN int unit,
    SOC_SAND_IN  SOC_PETRA_ITM_DBUFF_SIZE_BYTES  dbuff_size,
    SOC_SAND_IN  SOC_PETRA_INIT_DBUFFS_BDRY      *dbuffs_bdries
  )
{
  uint32
    fld_val,
    res;
  uint8
    is_pp;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_INIT_AFTER_BLOCKS_OOR);

  regs = soc_petra_regs();

  /************************************************************************/
  /* DRAM Buffers - get configuration from ECI                            */
  /************************************************************************/

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

  /*
   *  TCAM disable if not PP
   */
  is_pp = soc_petra_sw_db_pp_enable_get(unit);

  fld_val = is_pp?0x0:0x1;
  SOC_PA_FLD_SET(regs->ihp.tcam_configuration_reg.tcam_disable, fld_val, 230, exit);
  SOC_PA_FLD_SET(regs->ihp.tcam_configuration_reg.tcam_power_down, fld_val, 240, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_init_after_blocks_oor()", 0, 0);
}

/*
 *	Clear the QDR memory using the BIST mechanism for efficiency
 */
uint32
  soc_petra_init_qdr_mem_clear(
    SOC_SAND_IN int                 unit,
    SOC_SAND_IN SOC_PETRA_HW_QDR_SIZE_MBIT    qdr_size_mbit,
    SOC_SAND_IN SOC_PETRA_HW_QDR_PROTECT_TYPE qdr_protrct_type
  )
{
  uint32
    fld_val,
    qdr_nof_entries,
    res;
  SOC_PETRA_POLL_INFO
    poll_info;
  uint8
    poll_success;
  SOC_PETRA_DIAG_QDR_BIST_TEST_RUN_INFO
    bist;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_HW_ADJUST_QDR);

  regs = soc_petra_regs();

  soc_petra_PETRA_POLL_INFO_clear(&poll_info);

  /*
   *	Clear QDR Memory and ECC counters
   */
  qdr_nof_entries = soc_petra_init_qdr_size_to_bits(qdr_size_mbit) / SOC_PETRA_INIT_QDR_ENTRY_SIZE_BITS;
  soc_petra_PETRA_DIAG_QDR_BIST_TEST_RUN_INFO_clear(&bist);
  bist.address_mode = SOC_PETRA_DIAG_QDR_BIST_ADDR_MODE_NORMAL;
  bist.data_mode = SOC_PETRA_DIAG_QDR_BIST_DATA_MODE_NORMAL;
  bist.data_pattern =
    (qdr_protrct_type == SOC_PETRA_HW_QDR_PROTECT_TYPE_ECC)? SOC_PETRA_DIAG_BIST_DATA_PATTERN_P2:SOC_PETRA_DIAG_BIST_DATA_PATTERN_P1;

  bist.start_addr = 0;
  bist.end_addr = 0x3FFFFF - 1; /* More then maximal possible */
  bist.nof_cmnds_write = qdr_nof_entries;

  res = soc_petra_diag_qdr_BIST_test_start_unsafe(
          unit,
          &bist
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  poll_info.expected_value = 0x1;
  poll_info.busy_wait_nof_iters = SOC_PETRA_MGMT_INIT_QDR_BUSY_WAIT_ITERATIONS;
  poll_info.timer_nof_iters     = SOC_PETRA_MGMT_INIT_QDR_TIMER_ITERATIONS;
  poll_info.timer_delay_msec    = SOC_PETRA_MGMT_INIT_QDR_TIMER_DELAY_MSEC;
  poll_info.expected_value = 0x1;
  res = soc_petra_status_fld_poll_unsafe(
          unit,
          SOC_PETRA_REG_DB_ACC_REF(regs->qdr.bist_finished_reg.bist_finished),
          SOC_PETRA_DEFAULT_INSTANCE,
          &poll_info,
          &poll_success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 72, exit);

  if (poll_success == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MGMT_QDR_NOT_READY_ERR, 74, exit);
  }

  /*
   *	Stop the BIST
   */
  SOC_PETRA_REG_SET(regs->qdr.qdr_bist_enable_reg, 0x0, 80, exit);

   /*
    * Restore the training.
    */
    res = soc_petra_sw_db_qdr_dll_periodic_trng_reg_get(
            unit,
            &fld_val
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);

    SOC_PETRA_REG_SET(regs->qdr.periodic_enable_reg, fld_val ,50,exit);

  /*
   *	Clear ECC counters
   */
  SOC_PETRA_REG_GET(regs->qdr.qdr_ecc_error_counter_reg, fld_val, 80, exit);
  SOC_PETRA_REG_GET(regs->qdr.qdr_ecc_bit_correction_counter_reg, fld_val, 82, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_init_qdr_mem_clear()", 0, 0);
}

/*
 *  Must run before traffic is enabled
 */
STATIC uint32
  soc_petra_mgmt_init_finalize(
    SOC_SAND_IN int unit
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_INIT_SEQUENCE_PHASE2_UNSAFE);

  regs = soc_petra_regs();

  SOC_PETRA_FLD_SET(regs->qdr.periodic_enable_reg.prd_trn_en, 0x0, 10, exit);

  /*
   *	Clear QDR Memory and ECC counters
   */
  res = soc_petra_init_qdr_mem_clear(
          unit,
          soc_petra_sw_db_qdr_size_get(unit),
          soc_petra_sw_db_qdr_protection_type_get(unit)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_init_finalize()", 0, 0);
}

/*
 *  Tables Initialization {
 */
STATIC void
  soc_petra_mgmt_tbl_total_lines_set(void)
{
  uint32
    lines_count_total = 0;

  SOC_PETRA_TBLS
    *tables;
  uint32
    tbl_idx = 0;

  tables = soc_petra_tbls();

  lines_count_total =
    /*SOC_PA_TBL(tables->olp.pge_mem_tbl.addr.size) + */
    SOC_PA_TBL(tables->ire.nif_ctxt_map_tbl.addr.size) +
    SOC_PA_TBL(tables->ire.nif_port2ctxt_bit_map_tbl.addr.size) +
    SOC_PA_TBL(tables->ire.rcy_ctxt_map_tbl.addr.size) +
    SOC_PA_TBL(tables->idr.complete_pc_tbl.addr.size) +
    SOC_PA_TBL(tables->irr.is_ingress_replication_db_tbl.addr.size) +
    SOC_PA_TBL(tables->irr.ingress_replication_multicast_db_tbl.addr.size) +
    SOC_PA_TBL(tables->irr.mirror_table_tbl.addr.size) +
    SOC_PA_TBL(tables->irr.snoop_table_tbl.addr.size) +
    SOC_PA_TBL(tables->irr.glag_to_lag_range_tbl.addr.size) +
    SOC_PA_TBL(tables->irr.smooth_division_tbl.addr.size) +
    SOC_PA_TBL(tables->irr.glag_mapping_tbl.addr.size) +
    SOC_PA_TBL(tables->irr.destination_table_tbl.addr.size) +
    SOC_PA_TBL(tables->irr.glag_next_member_tbl.addr.size) +
    SOC_PA_TBL(tables->irr.rlag_next_member_tbl.addr.size) +
    SOC_PA_TBL(tables->ihp.port_info_tbl.addr.size) +
    SOC_PA_TBL(tables->ihp.port_to_system_port_id_tbl.addr.size) +
    SOC_PA_TBL(tables->ihp.static_header_tbl.addr.size) +
    SOC_PA_TBL(tables->ihp.system_port_my_port_table_tbl.addr.size) +
    SOC_PA_TBL(tables->ihp.ptc_commands1_tbl.addr.size) +
    SOC_PA_TBL(tables->ihp.ptc_commands2_tbl.addr.size) +
    SOC_PA_TBL(tables->ihp.ptc_key_program_lut_tbl.addr.size) +
    SOC_PA_TBL(tables->ihp.key_program0_tbl.addr.size) +
    SOC_PA_TBL(tables->ihp.key_program1_tbl.addr.size) +
    SOC_PA_TBL(tables->ihp.key_program2_tbl.addr.size) +
    SOC_PA_TBL(tables->ihp.key_program3_tbl.addr.size) +
    SOC_PA_TBL(tables->ihp.key_program4_tbl.addr.size) +
    /*SOC_PA_TBL(tables->ihp.programmable_cos_tbl.addr.size) +
    SOC_PA_TBL(tables->ihp.programmable_cos1_tbl.addr.size) + */
    /*SOC_PA_TBL(tables->iqm.bdb_link_list_tbl.addr.size) +
    SOC_PA_TBL(tables->iqm.dynamic_tbl.addr.size) + */
    SOC_PA_TBL(tables->iqm.static_tbl.addr.size) +
    SOC_PA_TBL(tables->iqm.packet_queue_tail_pointer_tbl.addr.size) +
    SOC_PA_TBL(tables->iqm.packet_queue_red_weight_table_tbl.addr.size) +
    SOC_PA_TBL(tables->iqm.credit_discount_table_tbl.addr.size) +
    /*SOC_PA_TBL(tables->iqm.full_user_count_memory_tbl.addr.size) +
    SOC_PA_TBL(tables->iqm.mini_multicast_user_count_memory_tbl.addr.size) + */
    SOC_PA_TBL(tables->iqm.packet_queue_red_parameters_table_tbl.addr.size) +
    SOC_PA_TBL(tables->iqm.vsq_descriptor_rate_class_group_a_tbl.addr.size) +
    SOC_PA_TBL(tables->iqm.vsq_descriptor_rate_class_group_b_tbl.addr.size) +
    SOC_PA_TBL(tables->iqm.vsq_descriptor_rate_class_group_c_tbl.addr.size) +
    SOC_PA_TBL(tables->iqm.vsq_descriptor_rate_class_group_d_tbl.addr.size) +
    SOC_PA_TBL(tables->iqm.vsq_qsize_memory_group_a_tbl.addr.size) +
    SOC_PA_TBL(tables->iqm.vsq_qsize_memory_group_b_tbl.addr.size) +
    SOC_PA_TBL(tables->iqm.vsq_qsize_memory_group_c_tbl.addr.size) +
    SOC_PA_TBL(tables->iqm.vsq_qsize_memory_group_d_tbl.addr.size) +
    SOC_PA_TBL(tables->iqm.vsq_average_qsize_memory_group_a_tbl.addr.size) +
    SOC_PA_TBL(tables->iqm.vsq_average_qsize_memory_group_b_tbl.addr.size) +
    SOC_PA_TBL(tables->iqm.vsq_average_qsize_memory_group_c_tbl.addr.size) +
    SOC_PA_TBL(tables->iqm.vsq_average_qsize_memory_group_d_tbl.addr.size) +
    SOC_PA_TBL(tables->iqm.system_red_parameters_table_tbl.addr.size) +
    SOC_PA_TBL(tables->iqm.system_red_drop_probability_values_tbl.addr.size) +
    SOC_PA_TBL(tables->iqm.system_red_tbl.addr.size) +
    SOC_PA_TBL(tables->ips.system_physical_port_lookup_table_tbl.addr.size) +
    SOC_PA_TBL(tables->ips.destination_device_and_port_lookup_table_tbl.addr.size) +
    SOC_PA_TBL(tables->ips.flow_id_lookup_table_tbl.addr.size) +
    SOC_PA_TBL(tables->ips.queue_type_lookup_table_tbl.addr.size) +
    SOC_PA_TBL(tables->ips.queue_priority_map_select_tbl.addr.size) +
    SOC_PA_TBL(tables->ips.queue_priority_maps_table_tbl.addr.size) +
    SOC_PA_TBL(tables->ips.queue_size_based_thresholds_table_tbl.addr.size) +
    SOC_PA_TBL(tables->ips.credit_balance_based_thresholds_table_tbl.addr.size) +
    SOC_PA_TBL(tables->ips.empty_queue_credit_balance_table_tbl.addr.size) +
    SOC_PA_TBL(tables->ips.credit_watchdog_thresholds_table_tbl.addr.size) +
    /*SOC_PA_TBL(tables->ips.queue_descriptor_table_tbl.addr.size) +
    SOC_PA_TBL(tables->ips.queue_size_table_tbl.addr.size) +
    SOC_PA_TBL(tables->ips.system_red_max_queue_size_table_tbl.addr.size) + */
    /*SOC_PA_TBL(tables->ipt.bdq_tbl.addr.size) +
    SOC_PA_TBL(tables->ipt.pcq_tbl.addr.size) +
    SOC_PA_TBL(tables->ipt.sop_mmu_tbl.addr.size) +
    SOC_PA_TBL(tables->ipt.mop_mmu_tbl.addr.size) +
    SOC_PA_TBL(tables->ipt.fdtctl_tbl.addr.size) +
    SOC_PA_TBL(tables->ipt.fdtdata_tbl.addr.size) +
    SOC_PA_TBL(tables->ipt.egqctl_tbl.addr.size) +
    SOC_PA_TBL(tables->ipt.egqdata_tbl.addr.size) +
    SOC_PA_TBL(tables->dpi.dll_ram_tbl.addr.size) +
    SOC_PA_TBL(tables->rtp.unicast_distribution_memory_for_data_cells_tbl.addr.size) +
    SOC_PA_TBL(tables->rtp.unicast_distribution_memory_for_control_cells_tbl.addr.size) + */
    SOC_PA_TBL(tables->egq.nif_scm_tbl.addr.size) +
    SOC_PA_TBL(tables->egq.nifab_nch_scm_tbl.addr.size) +
    SOC_PA_TBL(tables->egq.rcy_scm_tbl.addr.size) +
    SOC_PA_TBL(tables->egq.cpu_scm_tbl.addr.size) +
    SOC_PA_TBL(tables->egq.ccm_tbl.addr.size) +
    SOC_PA_TBL(tables->egq.pmc_tbl.addr.size) +
    SOC_PA_TBL(tables->egq.cbm_tbl.addr.size) +
    SOC_PA_TBL(tables->egq.dwm_tbl.addr.size) +
    SOC_PA_TBL(tables->egq.pct_tbl.addr.size) +
    SOC_PA_TBL(tables->egq.vlan_table_tbl.addr.size) +
    SOC_PA_TBL(tables->cfc.recycle_to_out_going_fap_port_mapping_tbl.addr.size) +
    SOC_PA_TBL(tables->cfc.nif_a_class_based_to_ofp_mapping_tbl.addr.size) +
    SOC_PA_TBL(tables->cfc.nif_b_class_based_to_ofp_mapping_tbl.addr.size) +
    SOC_PA_TBL(tables->cfc.a_schedulers_based_flow_control_to_ofp_mapping_tbl.addr.size) +
    SOC_PA_TBL(tables->cfc.b_schedulers_based_flow_control_to_ofp_mapping_tbl.addr.size) +
    SOC_PA_TBL(tables->cfc.out_of_band_rx_a_calendar_mapping_tbl.addr.size) +
    SOC_PA_TBL(tables->cfc.out_of_band_rx_b_calendar_mapping_tbl.addr.size) +
    SOC_PA_TBL(tables->cfc.out_of_band_tx_calendar_mapping_tbl.addr.size) +
    SOC_PA_TBL(tables->sch.cal_tbl.addr.size) +
    /*SOC_PA_TBL(tables->sch.drm_tbl.addr.size) +*/
    SOC_PA_TBL(tables->sch.dsm_tbl.addr.size) +
    /*SOC_PA_TBL(tables->sch.fdms_tbl.addr.size) +
    SOC_PA_TBL(tables->sch.shds_tbl.addr.size) +*/
    SOC_PA_TBL(tables->sch.sem_tbl.addr.size) +
    SOC_PA_TBL(tables->sch.fsf_tbl.addr.size) +
    SOC_PA_TBL(tables->sch.fgm_tbl.addr.size) +
    SOC_PA_TBL(tables->sch.shc_tbl.addr.size) +
    SOC_PA_TBL(tables->sch.scc_tbl.addr.size) +
    SOC_PA_TBL(tables->sch.sct_tbl.addr.size) +
    /*SOC_PA_TBL(tables->sch.fqm_tbl.addr.size) +
    SOC_PA_TBL(tables->sch.ffm_tbl.addr.size) +*/
    /*SOC_PA_TBL(tables->sch.soc_tmctbl.addr.size) +
    SOC_PA_TBL(tables->sch.pqs_tbl.addr.size) + */
    SOC_PA_TBL(tables->sch.scheduler_init_tbl.addr.size) +
    /*SOC_PA_TBL(tables->sch.force_status_message_tbl.addr.size) + */ 0;

    for (tbl_idx = 0; tbl_idx < SOC_PETRA_NOF_VSQ_GROUPS; tbl_idx++)
    {
      lines_count_total =
        lines_count_total +
        SOC_PA_TBL(tables->iqm.vsq_flow_control_parameters_table_group_tbl[tbl_idx].addr.size) +
        SOC_PA_TBL(tables->iqm.vsq_queue_parameters_table_group_tbl[tbl_idx].addr.size);
    }

  Soc_petra_mgmt_tbls_total_lines = lines_count_total;
}

STATIC uint32
  soc_petra_mgmt_ire_tbls_init(
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
  SOC_PETRA_TBLS
    *tables;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_IRE_NIF_CTXT_MAP_TBL_DATA
    ire_nif_ctxt_map_tbl_data;
  SOC_PETRA_IRE_NIF_PORT2CTXT_BIT_MAP_TBL_DATA
    ire_nif_port2ctxt_bit_map_tbl_data;
  SOC_PETRA_IRE_RCY_CTXT_MAP_TBL_DATA
    ire_rcy_ctxt_map_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_IRE_TBLS_INIT);

  res = soc_petra_tbls_get(
          &tables
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  regs = soc_petra_regs();

  ret = SOC_SAND_OK; sal_memset(&ire_nif_ctxt_map_tbl_data, 0x0, sizeof(ire_nif_ctxt_map_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 11, exit);

  ret = SOC_SAND_OK; sal_memset(&ire_nif_port2ctxt_bit_map_tbl_data, 0x0, sizeof(ire_nif_port2ctxt_bit_map_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 12, exit);

  ret = SOC_SAND_OK; sal_memset(&ire_rcy_ctxt_map_tbl_data, 0x0, sizeof(ire_rcy_ctxt_map_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 13, exit);

    /*Nif Ctxt Map*/
  nof_lines_total = SOC_PA_TBL(tables->ire.nif_ctxt_map_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->ire.indirect_command_reg.indirect_command_count));
  ire_nif_ctxt_map_tbl_data.fap_port = SOC_PETRA_PORTS_IF_UNMAPPED_INDICATION;
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_ire_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 101, exit);

       res = soc_petra_ire_nif_ctxt_map_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ire_nif_ctxt_map_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 102, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Nif Port2ctxt Bit Map*/
  nof_lines_total = SOC_PA_TBL(tables->ire.nif_port2ctxt_bit_map_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->ire.indirect_command_reg.indirect_command_count));
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_ire_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 103, exit);

       res = soc_petra_ire_nif_port2ctxt_bit_map_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ire_nif_port2ctxt_bit_map_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 104, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Rcy Ctxt Map*/
  nof_lines_total = SOC_PA_TBL(tables->ire.rcy_ctxt_map_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->ire.indirect_command_reg.indirect_command_count));
  ire_rcy_ctxt_map_tbl_data.fap_port = SOC_PETRA_PORTS_IF_UNMAPPED_INDICATION;
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_ire_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 105, exit);

       res = soc_petra_ire_rcy_ctxt_map_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ire_rcy_ctxt_map_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 106, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_ire_tbls_init()", 0, 0);
}

STATIC uint32
  soc_petra_mgmt_irr_tbls_init(
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
    lag_size,
    hash_indx,
    nof_lines_total,
    lines_max,
    first_entry = 0;
  SOC_PETRA_TBLS
    *tables;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_IRR_IS_INGRESS_REPLICATION_DB_TBL_DATA
    irr_is_ingress_replication_db_tbl_data;
  SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL_DATA
    irr_ingress_replication_multicast_db_tbl_data;
  SOC_PETRA_IRR_MIRROR_TABLE_TBL_DATA
    irr_mirror_table_tbl_data;
  SOC_PETRA_IRR_SNOOP_TABLE_TBL_DATA
    irr_snoop_table_tbl_data;
  SOC_PETRA_IRR_GLAG_TO_LAG_RANGE_TBL_DATA
    irr_glag_to_lag_range_tbl_data;
  SOC_PETRA_IRR_SMOOTH_DIVISION_TBL_DATA
    irr_smooth_division_tbl_data;
  SOC_PETRA_IRR_GLAG_MAPPING_TBL_DATA
    irr_glag_mapping_tbl_data;
  SOC_PETRA_IRR_DESTINATION_TABLE_TBL_DATA
    irr_destination_table_tbl_data;
  SOC_PETRA_IRR_GLAG_NEXT_MEMBER_TBL_DATA
    irr_glag_next_member_tbl_data;
  SOC_PETRA_IRR_RLAG_NEXT_MEMBER_TBL_DATA
    irr_rlag_next_member_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_IRR_TBLS_INIT);

  res = soc_petra_tbls_get(
          &tables
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  regs = soc_petra_regs();

  ret = SOC_SAND_OK; sal_memset(&irr_is_ingress_replication_db_tbl_data, 0x0, sizeof(irr_is_ingress_replication_db_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 11, exit);

  ret = SOC_SAND_OK; sal_memset(&irr_ingress_replication_multicast_db_tbl_data, 0x0, sizeof(irr_ingress_replication_multicast_db_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 12, exit);

  ret = SOC_SAND_OK; sal_memset(&irr_mirror_table_tbl_data, 0x0, sizeof(irr_mirror_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 13, exit);

  ret = SOC_SAND_OK; sal_memset(&irr_snoop_table_tbl_data, 0x0, sizeof(irr_snoop_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 14, exit);

  ret = SOC_SAND_OK; sal_memset(&irr_glag_to_lag_range_tbl_data, 0x0, sizeof(irr_glag_to_lag_range_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 15, exit);

  ret = SOC_SAND_OK; sal_memset(&irr_smooth_division_tbl_data, 0x0, sizeof(irr_smooth_division_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 16, exit);

  ret = SOC_SAND_OK; sal_memset(&irr_glag_mapping_tbl_data, 0x0, sizeof(irr_glag_mapping_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 17, exit);

  ret = SOC_SAND_OK; sal_memset(&irr_destination_table_tbl_data, 0x0, sizeof(irr_destination_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 18, exit);

  ret = SOC_SAND_OK; sal_memset(&irr_glag_next_member_tbl_data, 0x0, sizeof(irr_glag_next_member_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 19, exit);

  ret = SOC_SAND_OK; sal_memset(&irr_rlag_next_member_tbl_data, 0x0, sizeof(irr_rlag_next_member_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 20, exit);

  /*Is Ingress Replication DB*/
  nof_lines_total = SOC_PA_TBL(tables->irr.is_ingress_replication_db_tbl.addr.size);
  lines_max = nof_lines_total;

  nof_lines = nof_lines_total;

  res = soc_petra_irr_set_reps_for_tbl_unsafe(unit, nof_lines);
  SOC_SAND_CHECK_FUNC_RESULT(res, 101, exit);

  res = soc_petra_irr_is_ingress_replication_db_tbl_set_unsafe(
           unit,
           0x0,
           &irr_is_ingress_replication_db_tbl_data
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 102, exit);

  /*Ingress Replication Multicast DB*/
  nof_lines_total = SOC_PA_TBL(tables->irr.ingress_replication_multicast_db_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->irr.indirect_command_reg.indirect_command_count));
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_irr_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 103, exit);

       res = soc_petra_irr_ingress_replication_multicast_db_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &irr_ingress_replication_multicast_db_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 104, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Mirror Table*/
  nof_lines_total = SOC_PA_TBL(tables->irr.mirror_table_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->irr.indirect_command_reg.indirect_command_count));
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_irr_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 105, exit);

       res = soc_petra_irr_mirror_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &irr_mirror_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 106, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Snoop Table*/
  nof_lines_total = SOC_PA_TBL(tables->irr.snoop_table_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->irr.indirect_command_reg.indirect_command_count));
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_irr_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 107, exit);

       res = soc_petra_irr_snoop_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &irr_snoop_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 108, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Glag To Lag Range*/
  nof_lines_total = SOC_PA_TBL(tables->irr.glag_to_lag_range_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->irr.indirect_command_reg.indirect_command_count));
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_irr_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 109, exit);

       res = soc_petra_irr_glag_to_lag_range_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &irr_glag_to_lag_range_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Smooth Division*/

  res = soc_petra_irr_set_reps_for_tbl_unsafe(unit, 1);
  SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);
  nof_lines_total = SOC_PA_TBL(tables->irr.smooth_division_tbl.addr.size);

  for (lag_size = 0; lag_size < SOC_PETRA_PORTS_LAG_OUT_MEMBERS_MAX; ++lag_size)
  {
    for (hash_indx = 0; hash_indx < 256 ; ++hash_indx)
    {
      irr_smooth_division_tbl_data.smooth_division = hash_indx % (lag_size + 1);
      res = soc_petra_irr_smooth_division_tbl_set_unsafe(
              unit,
              lag_size,
              hash_indx,
              &irr_smooth_division_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 112, exit);
    }
  }
  first_entry = 0;

  /*Glag Mapping*/
  nof_lines_total = SOC_PA_TBL(tables->irr.glag_mapping_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->irr.indirect_command_reg.indirect_command_count));
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_irr_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 113, exit);
       res = soc_petra_irr_glag_mapping_tbl_set_unsafe(
                 unit,
                 0x0,
                 0x0,
                 &irr_glag_mapping_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 114, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Destination Table*/
  nof_lines_total = SOC_PA_TBL(tables->irr.destination_table_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->irr.indirect_command_reg.indirect_command_count));
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_irr_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 115, exit);

       res = soc_petra_irr_destination_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &irr_destination_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 116, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Glag Next Member*/
  nof_lines_total = SOC_PA_TBL(tables->irr.glag_next_member_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->irr.indirect_command_reg.indirect_command_count));
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_irr_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 117, exit);

       res = soc_petra_irr_glag_next_member_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &irr_glag_next_member_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 118, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Rlag Next Member*/
  nof_lines_total = SOC_PA_TBL(tables->irr.rlag_next_member_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->irr.indirect_command_reg.indirect_command_count));
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_irr_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 119, exit);

       res = soc_petra_irr_rlag_next_member_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &irr_rlag_next_member_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_irr_tbls_init()", 0, 0);
}

STATIC uint32
  soc_petra_mgmt_ihp_tbls_init(
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
  SOC_PETRA_TBLS
    *tables;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_IHP_PORT_INFO_TBL_DATA
    ihp_port_info_tbl_data;
  SOC_PETRA_IHP_PORT_TO_SYSTEM_PORT_ID_TBL_DATA
    ihp_port_to_system_port_id_tbl_data;
  SOC_PETRA_IHP_STATIC_HEADER_TBL_DATA
    ihp_static_header_tbl_data;
  SOC_PETRA_IHP_SYSTEM_PORT_MY_PORT_TABLE_TBL_DATA
    ihp_system_port_my_port_table_tbl_data;
  SOC_PETRA_IHP_PTC_COMMANDS_TBL_DATA
    ihp_ptc_commands_tbl_data;
  /*SOC_PETRA_IHP_PTC_KEY_PROGRAM_LUT_TBL_DATA
    ihp_ptc_key_program_lut_tbl_data;*/
  SOC_PETRA_IHP_KEY_PROGRAM0_TBL_DATA
    ihp_key_program0_tbl_data;
  SOC_PETRA_IHP_KEY_PROGRAM1_TBL_DATA
    ihp_key_program1_tbl_data;
  SOC_PETRA_IHP_KEY_PROGRAM2_TBL_DATA
    ihp_key_program2_tbl_data;
  SOC_PETRA_IHP_KEY_PROGRAM3_TBL_DATA
    ihp_key_program3_tbl_data;
  SOC_PETRA_IHP_KEY_PROGRAM4_TBL_DATA
    ihp_key_program4_tbl_data;
  /*SOC_PETRA_IHP_PROGRAMMABLE_COS_TBL_DATA
    ihp_programmable_cos_tbl_data;
  SOC_PETRA_IHP_PROGRAMMABLE_COS1_TBL_DATA
    ihp_programmable_cos1_tbl_data;*/

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_IHP_TBLS_INIT);

  res = soc_petra_tbls_get(
          &tables
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  regs = soc_petra_regs();

  ret = SOC_SAND_OK; sal_memset(&ihp_port_info_tbl_data, 0x0, sizeof(ihp_port_info_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 11, exit);

  ret = SOC_SAND_OK; sal_memset(&ihp_port_to_system_port_id_tbl_data, 0x0, sizeof(ihp_port_to_system_port_id_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 12, exit);

  ret = SOC_SAND_OK; sal_memset(&ihp_static_header_tbl_data, 0x0, sizeof(ihp_static_header_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 13, exit);

  ret = SOC_SAND_OK; sal_memset(&ihp_system_port_my_port_table_tbl_data, 0x0, sizeof(ihp_system_port_my_port_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 14, exit);

  ret = SOC_SAND_OK; sal_memset(&ihp_ptc_commands_tbl_data, 0x0, sizeof(ihp_ptc_commands_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 15, exit);

  /*ret = SOC_SAND_OK; sal_memset(&ihp_ptc_key_program_lut_tbl_data, 0x0, sizeof(ihp_ptc_key_program_lut_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 17, exit);*/

  ret = SOC_SAND_OK; sal_memset(&ihp_key_program0_tbl_data, 0x0, sizeof(ihp_key_program0_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 18, exit);

  ret = SOC_SAND_OK; sal_memset(&ihp_key_program1_tbl_data, 0x0, sizeof(ihp_key_program1_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 19, exit);

  ret = SOC_SAND_OK; sal_memset(&ihp_key_program2_tbl_data, 0x0, sizeof(ihp_key_program2_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 20, exit);

  ret = SOC_SAND_OK; sal_memset(&ihp_key_program3_tbl_data, 0x0, sizeof(ihp_key_program3_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 21, exit);

  ret = SOC_SAND_OK; sal_memset(&ihp_key_program4_tbl_data, 0x0, sizeof(ihp_key_program4_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 22, exit);

  /*ret = SOC_SAND_OK; sal_memset(&ihp_programmable_cos_tbl_data, 0x0, sizeof(ihp_programmable_cos_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 23, exit);

  ret = SOC_SAND_OK; sal_memset(&ihp_programmable_cos1_tbl_data, 0x0, sizeof(ihp_programmable_cos1_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 24, exit);*/

  /*Port Info*/

  ihp_port_info_tbl_data.append_ftmh = SOC_SAND_BOOL2NUM(TRUE);
  ihp_port_info_tbl_data.custom_command_select = 0x1;
  /*
   *  This default is suitable for TM ports,
   *  setting port header type will change it according to the requested
   *  port type.
   */
  ihp_port_info_tbl_data.header_remove = SOC_SAND_BOOL2NUM(TRUE);

  nof_lines_total = SOC_PA_TBL(tables->ihp.port_info_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->ihp.indirect_command_reg.indirect_command_count));
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_ihp_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 101, exit);

       res = soc_petra_ihp_port_info_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihp_port_info_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 102, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Port To System Port Id*/
  nof_lines_total = SOC_PA_TBL(tables->ihp.port_to_system_port_id_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->ihp.indirect_command_reg.indirect_command_count));
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_ihp_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 103, exit);

       res = soc_petra_ihp_port_to_system_port_id_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihp_port_to_system_port_id_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 104, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Static Header*/
  nof_lines_total = SOC_PA_TBL(tables->ihp.static_header_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->ihp.indirect_command_reg.indirect_command_count));
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_ihp_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 105, exit);

       res = soc_petra_ihp_static_header_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihp_static_header_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 106, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*System Port My Port Table*/
  nof_lines_total = SOC_PA_TBL(tables->ihp.system_port_my_port_table_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->ihp.indirect_command_reg.indirect_command_count));
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_ihp_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 107, exit);

       res = soc_petra_ihp_system_port_my_port_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihp_system_port_my_port_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 108, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Ptc Commands1*/
  nof_lines_total = SOC_PA_TBL(tables->ihp.ptc_commands1_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->ihp.indirect_command_reg.indirect_command_count));
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_ihp_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 109, exit);

       res = soc_petra_ihp_ptc_commands1_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihp_ptc_commands_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Ptc Commands2*/
  nof_lines_total = SOC_PA_TBL(tables->ihp.ptc_commands2_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->ihp.indirect_command_reg.indirect_command_count));
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_ihp_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);

       res = soc_petra_ihp_ptc_commands2_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihp_ptc_commands_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 112, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Ptc Key Program Lut*/
  /*nof_lines_total = SOC_PA_TBL(tables->ihp.ptc_key_program_lut_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->ihp.indirect_command_reg.indirect_command_count));
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_ihp_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 113, exit);

       res = soc_petra_ihp_ptc_key_program_lut_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihp_ptc_key_program_lut_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 114, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;*/

  /*Key Program0*/
  nof_lines_total = SOC_PA_TBL(tables->ihp.key_program0_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->ihp.indirect_command_reg.indirect_command_count));
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_ihp_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 115, exit);

       res = soc_petra_ihp_key_program0_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihp_key_program0_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 116, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Key Program1*/
  nof_lines_total = SOC_PA_TBL(tables->ihp.key_program1_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->ihp.indirect_command_reg.indirect_command_count));
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_ihp_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 117, exit);

       res = soc_petra_ihp_key_program1_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihp_key_program1_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 118, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Key Program2*/
  nof_lines_total = SOC_PA_TBL(tables->ihp.key_program2_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->ihp.indirect_command_reg.indirect_command_count));
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_ihp_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 119, exit);

       res = soc_petra_ihp_key_program2_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihp_key_program2_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Key Program3*/
  nof_lines_total = SOC_PA_TBL(tables->ihp.key_program3_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->ihp.indirect_command_reg.indirect_command_count));
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_ihp_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 121, exit);

       res = soc_petra_ihp_key_program3_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihp_key_program3_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 122, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Key Program4*/
  nof_lines_total = SOC_PA_TBL(tables->ihp.key_program4_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->ihp.indirect_command_reg.indirect_command_count));
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_ihp_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 123, exit);

       res = soc_petra_ihp_key_program4_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihp_key_program4_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 124, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Programmable Cos*/
  /*nof_lines_total = SOC_PA_TBL(tables->ihp.programmable_cos_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->ihp.indirect_command_reg.indirect_command_count));
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_ihp_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 125, exit);

       res = soc_petra_ihp_programmable_cos_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihp_programmable_cos_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 126, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;*/

  /*Programmable Cos1*/
  /*nof_lines_total = SOC_PA_TBL(tables->ihp.programmable_cos1_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->ihp.indirect_command_reg.indirect_command_count));
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_ihp_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 127, exit);

       res = soc_petra_ihp_programmable_cos1_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ihp_programmable_cos1_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 128, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;*/

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_ihp_tbls_init()", 0, 0);
}

STATIC uint32
  soc_petra_mgmt_iqm_tbls_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  int32
    tbl_idx = 0;
  uint32
    nof_lines,
    nof_lines_total,
    lines_max,
    first_entry = 0;
  SOC_PETRA_TBLS
    *tables;
  SOC_PETRA_REGS
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
    iqm_vsq_flow_control_parameters_table_group_tbl_data;
  SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL_DATA
    iqm_vsq_queue_parameters_table_group_tbl_data;
  SOC_PETRA_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_DATA
    iqm_system_red_parameters_table_tbl_data;
  SOC_PETRA_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_DATA
    iqm_system_red_drop_probability_values_tbl_data;
  SOC_PETRA_IQM_SYSTEM_RED_TBL_DATA
    iqm_system_red_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_IQM_TBLS_INIT);

  res = soc_petra_tbls_get(
          &tables
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  regs = soc_petra_regs();

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

  ret = SOC_SAND_OK; sal_memset(&iqm_vsq_flow_control_parameters_table_group_tbl_data, 0x0, sizeof(iqm_vsq_flow_control_parameters_table_group_tbl_data));
  iqm_vsq_flow_control_parameters_table_group_tbl_data.avrg_size_en = TRUE;
  SOC_SAND_CHECK_FUNC_RESULT(ret, 32, exit);

  ret = SOC_SAND_OK; sal_memset(&iqm_vsq_queue_parameters_table_group_tbl_data, 0x0, sizeof(iqm_vsq_queue_parameters_table_group_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 36, exit);

  ret = SOC_SAND_OK; sal_memset(&iqm_system_red_parameters_table_tbl_data, 0x0, sizeof(iqm_system_red_parameters_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 40, exit);

  ret = SOC_SAND_OK; sal_memset(&iqm_system_red_drop_probability_values_tbl_data, 0x0, sizeof(iqm_system_red_drop_probability_values_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 41, exit);

  ret = SOC_SAND_OK; sal_memset(&iqm_system_red_tbl_data, 0x0, sizeof(iqm_system_red_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 42, exit);

  /*Packet Queue Descriptor (Static)*/
  nof_lines_total = SOC_PA_TBL(tables->iqm.static_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->iqm.indirect_command_reg.indirect_command_count));
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
  nof_lines_total = SOC_PA_TBL(tables->iqm.packet_queue_red_weight_table_tbl.addr.size);
  iqm_packet_queue_red_weight_table_tbl_data.avrg_en    = TRUE;
  iqm_packet_queue_red_weight_table_tbl_data.pq_weight  = 2;
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->iqm.indirect_command_reg.indirect_command_count));
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
  nof_lines_total = SOC_PA_TBL(tables->iqm.credit_discount_table_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->iqm.indirect_command_reg.indirect_command_count));
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
  nof_lines_total = SOC_PA_TBL(tables->iqm.packet_queue_red_parameters_table_tbl.addr.size);
  iqm_packet_queue_red_parameters_table_tbl_data.pq_wred_pckt_sz_ignr = TRUE;
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->iqm.indirect_command_reg.indirect_command_count));
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
  nof_lines_total = SOC_PA_TBL(tables->iqm.vsq_descriptor_rate_class_group_a_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->iqm.indirect_command_reg.indirect_command_count));
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
  nof_lines_total = SOC_PA_TBL(tables->iqm.vsq_descriptor_rate_class_group_b_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->iqm.indirect_command_reg.indirect_command_count));
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
  nof_lines_total = SOC_PA_TBL(tables->iqm.vsq_descriptor_rate_class_group_c_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->iqm.indirect_command_reg.indirect_command_count));
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
  nof_lines_total = SOC_PA_TBL(tables->iqm.vsq_descriptor_rate_class_group_d_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->iqm.indirect_command_reg.indirect_command_count));
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

  /*VSQ Flow-Control Parameters table - group A-D*/
  for (tbl_idx = 0; tbl_idx < SOC_PETRA_NOF_VSQ_GROUPS; tbl_idx++)
  {
    nof_lines_total = SOC_PA_TBL(tables->iqm.vsq_flow_control_parameters_table_group_tbl[0].addr.size);
    lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->iqm.indirect_command_reg.indirect_command_count));
    do {
         nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
         nof_lines_total -= nof_lines;

         res = soc_petra_iqm_set_reps_for_tbl_unsafe(unit, nof_lines);
         SOC_SAND_CHECK_FUNC_RESULT(res, 143, exit);

         res = soc_petra_iqm_vsq_flow_control_parameters_table_group_tbl_set_unsafe(
                   unit,
                   tbl_idx,
                   first_entry,
                   &iqm_vsq_flow_control_parameters_table_group_tbl_data
               );
         SOC_SAND_CHECK_FUNC_RESULT(res, 144, exit);
         first_entry += nof_lines;
    } while(nof_lines_total > 0);
    first_entry = 0;
  }

  /*VSQ Queue Parameters table - group A-D*/
  for (tbl_idx = 0; tbl_idx < SOC_PETRA_NOF_VSQ_GROUPS; tbl_idx++)
  {
    nof_lines_total = SOC_PA_TBL(tables->iqm.vsq_queue_parameters_table_group_tbl[0].addr.size);
    lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->iqm.indirect_command_reg.indirect_command_count));
    do {
         nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
         nof_lines_total -= nof_lines;

         res = soc_petra_iqm_set_reps_for_tbl_unsafe(unit, nof_lines);
         SOC_SAND_CHECK_FUNC_RESULT(res, 151, exit);

         res = soc_petra_iqm_vsq_queue_parameters_table_group_tbl_set_unsafe(
                   unit,
                   tbl_idx,
                   0x0,
                   0x0,
                   &iqm_vsq_queue_parameters_table_group_tbl_data
               );
         SOC_SAND_CHECK_FUNC_RESULT(res, 152, exit);
         first_entry += nof_lines;
    } while(nof_lines_total > 0);
    first_entry = 0;
  }


  /*System Red parameters table*/
  nof_lines_total = SOC_PA_TBL(tables->iqm.system_red_parameters_table_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->iqm.indirect_command_reg.indirect_command_count));
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
  nof_lines_total = SOC_PA_TBL(tables->iqm.system_red_drop_probability_values_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->iqm.indirect_command_reg.indirect_command_count));
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
  nof_lines_total = SOC_PA_TBL(tables->iqm.system_red_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->iqm.indirect_command_reg.indirect_command_count));
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

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_iqm_tbls_init()", 0, 0);
}

STATIC uint32
  soc_petra_mgmt_ips_tbls_init(
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
  SOC_PETRA_TBLS
    *tables;
  SOC_PETRA_REGS
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
  /*SOC_PETRA_IPS_QUEUE_DESCRIPTOR_TABLE_TBL_DATA
    ips_queue_descriptor_table_tbl_data;
  SOC_PETRA_IPS_QUEUE_SIZE_TABLE_TBL_DATA
    ips_queue_size_table_tbl_data;
  SOC_PETRA_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL_DATA
    ips_system_red_max_queue_size_table_tbl_data;*/

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_IPS_TBLS_INIT);

  res = soc_petra_tbls_get(
          &tables
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  regs = soc_petra_regs();

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

  /*ret = SOC_SAND_OK; sal_memset(&ips_queue_descriptor_table_tbl_data, 0x0, sizeof(ips_queue_descriptor_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 21, exit);

  ret = SOC_SAND_OK; sal_memset(&ips_queue_size_table_tbl_data, 0x0, sizeof(ips_queue_size_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 22, exit);

  ret = SOC_SAND_OK; sal_memset(&ips_system_red_max_queue_size_table_tbl_data, 0x0, sizeof(ips_system_red_max_queue_size_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 23, exit);*/

  /*System Physical Port Lookup Table*/
  ips_system_physical_port_lookup_table_tbl_data.sys_phy_port = SOC_PETRA_MAX_SYSTEM_PHYSICAL_PORT_ID;
  nof_lines_total = SOC_PA_TBL(tables->ips.system_physical_port_lookup_table_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->ips.indirect_command_reg.indirect_command_count));
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
  nof_lines_total = SOC_PA_TBL(tables->ips.destination_device_and_port_lookup_table_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->ips.indirect_command_reg.indirect_command_count));
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
  nof_lines_total = SOC_PA_TBL(tables->ips.flow_id_lookup_table_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->ips.indirect_command_reg.indirect_command_count));
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
  nof_lines_total = SOC_PA_TBL(tables->ips.queue_type_lookup_table_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->ips.indirect_command_reg.indirect_command_count));
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
  nof_lines_total = SOC_PA_TBL(tables->ips.queue_priority_map_select_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->ips.indirect_command_reg.indirect_command_count));
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
  nof_lines_total = SOC_PA_TBL(tables->ips.queue_priority_maps_table_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->ips.indirect_command_reg.indirect_command_count));
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

  nof_lines_total = SOC_PA_TBL(tables->ips.queue_size_based_thresholds_table_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->ips.indirect_command_reg.indirect_command_count));
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

  nof_lines_total = SOC_PA_TBL(tables->ips.credit_balance_based_thresholds_table_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->ips.indirect_command_reg.indirect_command_count));
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
  nof_lines_total = SOC_PA_TBL(tables->ips.empty_queue_credit_balance_table_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->ips.indirect_command_reg.indirect_command_count));
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
  nof_lines_total = SOC_PA_TBL(tables->ips.credit_watchdog_thresholds_table_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->ips.indirect_command_reg.indirect_command_count));
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

  /*Queue Descriptor Table*/
  /*nof_lines_total = SOC_PA_TBL(tables->ips.queue_descriptor_table_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->ips.indirect_command_reg.indirect_command_count));
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_ips_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 121, exit);

       res = soc_petra_ips_queue_descriptor_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ips_queue_descriptor_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 122, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;*/

  /*Queue Size Table*/
  /*nof_lines_total = SOC_PA_TBL(tables->ips.queue_size_table_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->ips.indirect_command_reg.indirect_command_count));
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_ips_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 123, exit);

       res = soc_petra_ips_queue_size_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ips_queue_size_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 124, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;*/

  /*System Red Max Queue Size Table*/
  /*nof_lines_total = SOC_PA_TBL(tables->ips.system_red_max_queue_size_table_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->ips.indirect_command_reg.indirect_command_count));
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_ips_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 125, exit);

       res = soc_petra_ips_system_red_max_queue_size_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &ips_system_red_max_queue_size_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 126, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;*/

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_ips_tbls_init()", 0, 0);
}

STATIC uint32
  soc_petra_mgmt_egq_tbls_init(
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
  uint32
    tbl_idx = 0;
  SOC_PETRA_TBLS
    *tables;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_EGQ_SCM_TBL_DATA
    egq_nif_scm_tbl_data;
  SOC_PETRA_EGQ_NIFAB_NCH_SCM_TBL_DATA
    egq_nifab_nch_scm_tbl_data;
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
  SOC_PETRA_EGQ_DWM_TBL_DATA
    egq_dwm_tbl_data;
  SOC_PETRA_EGQ_PCT_TBL_DATA
    egq_pct_tbl_data;
  SOC_PETRA_EGQ_PPCT_TBL_DATA
    egq_ppct_tbl_data;
  SOC_PETRA_EGQ_VLAN_TABLE_TBL_DATA
    egq_vlan_table_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_EGQ_TBLS_INIT);

  res = soc_petra_tbls_get(
          &tables
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  regs = soc_petra_regs();

  ret = SOC_SAND_OK; sal_memset(&egq_nif_scm_tbl_data, 0x0, sizeof(egq_nif_scm_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 40, exit);

  ret = SOC_SAND_OK; sal_memset(&egq_nifab_nch_scm_tbl_data, 0x0, sizeof(egq_nifab_nch_scm_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 19, exit);

  ret = SOC_SAND_OK; sal_memset(&egq_rcy_scm_tbl_data, 0x0, sizeof(egq_rcy_scm_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 20, exit);

  ret = SOC_SAND_OK; sal_memset(&egq_cpu_scm_tbl_data, 0x0, sizeof(egq_cpu_scm_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 21, exit);

  ret = SOC_SAND_OK; sal_memset(&egq_ccm_tbl_data, 0x0, sizeof(egq_ccm_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 22, exit);

  ret = SOC_SAND_OK; sal_memset(&egq_pmc_tbl_data, 0x0, sizeof(egq_pmc_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 23, exit);

  ret = SOC_SAND_OK; sal_memset(&egq_cbm_tbl_data, 0x0, sizeof(egq_cbm_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 24, exit);

  ret = SOC_SAND_OK; sal_memset(&egq_dwm_tbl_data, 0x0, sizeof(egq_dwm_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 27, exit);

  ret = SOC_SAND_OK; sal_memset(&egq_pct_tbl_data, 0x0, sizeof(egq_pct_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 30, exit);

  ret = SOC_SAND_OK; sal_memset(&egq_ppct_tbl_data, 0x0, sizeof(egq_ppct_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 31, exit);

  ret = SOC_SAND_OK; sal_memset(&egq_vlan_table_tbl_data, 0x0, sizeof(egq_vlan_table_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 32, exit);

    for (tbl_idx = 0; tbl_idx < SOC_PETRA_NOF_MAC_LANES; tbl_idx++)
  {
    /*Egress Shaper Nif Credit Configuration (Nifa Ch0 Scm)*/
    nof_lines_total = SOC_PA_TBL(tables->egq.nif_scm_tbl.addr.size);
    lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->egq.indirect_command_reg.indirect_command_count));
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

  /*Egress Shaper Nifa And Nifb Non Channeleized Ports Credit Configuration (Nifab Nch Scm)*/
  nof_lines_total = SOC_PA_TBL(tables->egq.nifab_nch_scm_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->egq.indirect_command_reg.indirect_command_count));
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_egq_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 117, exit);

       res = soc_petra_egq_nifab_nch_scm_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &egq_nifab_nch_scm_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 118, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Egress Shaper Recycling Ports Credit Configuration (Rcy Scm)*/
  nof_lines_total = SOC_PA_TBL(tables->egq.rcy_scm_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->egq.indirect_command_reg.indirect_command_count));
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_egq_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 119, exit);

       res = soc_petra_egq_rcy_scm_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &egq_rcy_scm_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Egress Shaper CPUPorts Credit Configuration(Cpu Scm)*/
  nof_lines_total = SOC_PA_TBL(tables->egq.cpu_scm_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->egq.indirect_command_reg.indirect_command_count));
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_egq_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 121, exit);

       res = soc_petra_egq_cpu_scm_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &egq_cpu_scm_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 122, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Egress Shaper Calendar Selector (CCM)*/
  nof_lines_total = SOC_PA_TBL(tables->egq.ccm_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->egq.indirect_command_reg.indirect_command_count));
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_egq_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 123, exit);

       res = soc_petra_egq_ccm_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &egq_ccm_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 124, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Per Port Max Credit Memory (PMC)*/
  nof_lines_total = SOC_PA_TBL(tables->egq.pmc_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->egq.indirect_command_reg.indirect_command_count));
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_egq_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 125, exit);

       res = soc_petra_egq_pmc_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &egq_pmc_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 126, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Egress Shaper Per Port Credit Balance Memory (CBM)*/
  nof_lines_total = SOC_PA_TBL(tables->egq.cbm_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->egq.indirect_command_reg.indirect_command_count));
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_egq_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 127, exit);

       res = soc_petra_egq_cbm_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &egq_cbm_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 128, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Ofp Dequeue Wfq Configuration Memory (DWM)*/
  egq_dwm_tbl_data.mc_or_mc_low_queue_weight = 1;
  egq_dwm_tbl_data.uc_or_uc_low_queue_weight = 1;
  nof_lines_total = SOC_PA_TBL(tables->egq.dwm_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->egq.indirect_command_reg.indirect_command_count));
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_egq_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 133, exit);

       res = soc_petra_egq_dwm_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &egq_dwm_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 134, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;


  /*Port Configuration Table (PCT)*/
  nof_lines_total = SOC_PA_TBL(tables->egq.pct_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->egq.indirect_command_reg.indirect_command_count));
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_egq_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 139, exit);

       res = soc_petra_egq_pct_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &egq_pct_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Per Port Configuration Table (PPCT)*/
  egq_ppct_tbl_data.sys_port_id = SOC_PETRA_MAX_SYSTEM_PHYSICAL_PORT_ID;
  nof_lines_total = SOC_PA_TBL(tables->egq.ppct_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->egq.indirect_command_reg.indirect_command_count));
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_egq_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 150, exit);

       res = soc_petra_egq_ppct_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &egq_ppct_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 151, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Vlan Table Configuration Memory(Vlan Table)*/
  nof_lines_total = SOC_PA_TBL(tables->egq.vlan_table_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->egq.indirect_command_reg.indirect_command_count));
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_egq_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 141, exit);

       res = soc_petra_egq_vlan_table_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &egq_vlan_table_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 142, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_egq_tbls_init()", 0, 0);
}

STATIC uint32
  soc_petra_mgmt_cfc_tbls_init(
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
  SOC_PETRA_TBLS
    *tables;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_CFC_RECYCLE_TO_OUT_GOING_FAP_PORT_MAPPING_TBL_DATA
    cfc_recycle_to_out_going_fap_port_mapping_tbl_data;
  SOC_PETRA_CFC_NIF_CLASS_BASED_TO_OFP_MAPPING_TBL_DATA
    cfc_nif_class_based_to_ofp_mapping_tbl_data;
  SOC_PETRA_CFC_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL_DATA
    cfc_schedulers_based_flow_control_to_ofp_mapping_tbl_data;
  SOC_PETRA_CFC_OUT_OF_BAND_RX_CALENDAR_MAPPING_TBL_DATA
    cfc_out_of_band_rx_calendar_mapping_tbl_data;
  SOC_PETRA_CFC_OUT_OF_BAND_TX_CALENDAR_MAPPING_TBL_DATA
    cfc_out_of_band_tx_calendar_mapping_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_CFC_TBLS_INIT);

  res = soc_petra_tbls_get(
          &tables
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  regs = soc_petra_regs();

  ret = SOC_SAND_OK; sal_memset(&cfc_recycle_to_out_going_fap_port_mapping_tbl_data, 0x0, sizeof(cfc_recycle_to_out_going_fap_port_mapping_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 11, exit);

  ret = SOC_SAND_OK; sal_memset(&cfc_nif_class_based_to_ofp_mapping_tbl_data, 0x0, sizeof(cfc_nif_class_based_to_ofp_mapping_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 12, exit);

  ret = SOC_SAND_OK; sal_memset(&cfc_schedulers_based_flow_control_to_ofp_mapping_tbl_data, 0x0, sizeof(cfc_schedulers_based_flow_control_to_ofp_mapping_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 14, exit);

  ret = SOC_SAND_OK; sal_memset(&cfc_out_of_band_rx_calendar_mapping_tbl_data, 0x0, sizeof(cfc_out_of_band_rx_calendar_mapping_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 16, exit);

  ret = SOC_SAND_OK; sal_memset(&cfc_out_of_band_tx_calendar_mapping_tbl_data, 0x0, sizeof(cfc_out_of_band_tx_calendar_mapping_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 18, exit);

    /*Recycle to Out Going Fap Port Mapping*/
  nof_lines_total = SOC_PA_TBL(tables->cfc.recycle_to_out_going_fap_port_mapping_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->cfc.indirect_command_reg.indirect_command_count));
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_cfc_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 101, exit);

       res = soc_petra_cfc_recycle_to_out_going_fap_port_mapping_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &cfc_recycle_to_out_going_fap_port_mapping_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 102, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*NIF A Class Based to OFP mapping*/
  nof_lines_total = SOC_PA_TBL(tables->cfc.nif_a_class_based_to_ofp_mapping_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->cfc.indirect_command_reg.indirect_command_count));
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_cfc_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 103, exit);

       res = soc_petra_cfc_nif_a_class_based_to_ofp_mapping_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &cfc_nif_class_based_to_ofp_mapping_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 104, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*NIF B Class Based to OFP mapping*/
  nof_lines_total = SOC_PA_TBL(tables->cfc.nif_b_class_based_to_ofp_mapping_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->cfc.indirect_command_reg.indirect_command_count));
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_cfc_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 105, exit);

       res = soc_petra_cfc_nif_b_class_based_to_ofp_mapping_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &cfc_nif_class_based_to_ofp_mapping_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 106, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Out Of Band (A) Scheduler's based flow-control to OFP mapping*/
  nof_lines_total = SOC_PA_TBL(tables->cfc.a_schedulers_based_flow_control_to_ofp_mapping_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->cfc.indirect_command_reg.indirect_command_count));
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_cfc_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 107, exit);

       res = soc_petra_cfc_a_schedulers_based_flow_control_to_ofp_mapping_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &cfc_schedulers_based_flow_control_to_ofp_mapping_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 108, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Out Of Band (B) Scheduler's based flow-control to OFP mapping*/
  nof_lines_total = SOC_PA_TBL(tables->cfc.b_schedulers_based_flow_control_to_ofp_mapping_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->cfc.indirect_command_reg.indirect_command_count));
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_cfc_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 109, exit);

       res = soc_petra_cfc_b_schedulers_based_flow_control_to_ofp_mapping_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &cfc_schedulers_based_flow_control_to_ofp_mapping_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Out Of Band Rx A  calendar mapping*/
  nof_lines_total = SOC_PA_TBL(tables->cfc.out_of_band_rx_a_calendar_mapping_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->cfc.indirect_command_reg.indirect_command_count));
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_cfc_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);

       res = soc_petra_cfc_out_of_band_rx_a_calendar_mapping_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &cfc_out_of_band_rx_calendar_mapping_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 112, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Out Of Band Rx B calendar mapping*/
  nof_lines_total = SOC_PA_TBL(tables->cfc.out_of_band_rx_b_calendar_mapping_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->cfc.indirect_command_reg.indirect_command_count));
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_cfc_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 113, exit);

       res = soc_petra_cfc_out_of_band_rx_b_calendar_mapping_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &cfc_out_of_band_rx_calendar_mapping_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 114, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

  /*Out Of Band Tx  calendar mapping*/
  nof_lines_total = SOC_PA_TBL(tables->cfc.out_of_band_tx_calendar_mapping_tbl.addr.size);
  lines_max = SOC_PETRA_INIT_COUNT_MAX(SOC_PA_REG_DB_ACC(regs->cfc.indirect_command_reg.indirect_command_count));
  do {
       nof_lines = SOC_SAND_MIN(nof_lines_total, lines_max);
       nof_lines_total -= nof_lines;

       res = soc_petra_cfc_set_reps_for_tbl_unsafe(unit, nof_lines);
       SOC_SAND_CHECK_FUNC_RESULT(res, 115, exit);

       res = soc_petra_cfc_out_of_band_tx_calendar_mapping_tbl_set_unsafe(
                 unit,
                 first_entry,
                 &cfc_out_of_band_tx_calendar_mapping_tbl_data
             );
       SOC_SAND_CHECK_FUNC_RESULT(res, 116, exit);
       first_entry += nof_lines;
  } while(nof_lines_total > 0);
  first_entry = 0;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_cfc_tbls_init()", 0, 0);
}

STATIC uint32
  soc_petra_mgmt_sch_tbls_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 is_shadow_en,
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
  SOC_PETRA_TBLS
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
  /*SOC_PETRA_SCH_SCC_TBL_DATA
    sch_scc_tbl_data;*/
  SOC_PETRA_SCH_SCT_TBL_DATA
    sch_sct_tbl_data;
  SOC_PETRA_SCH_FFM_TBL_DATA
    sch_ffm_tbl_data;
  SOC_PETRA_SCH_SCHEDULER_INIT_TBL_DATA
    sch_scheduler_init_tbl_data;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_SCH_TBLS_INIT);

  res = soc_petra_tbls_get(
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

  /*ret = SOC_SAND_OK; sal_memset(&sch_scc_tbl_data, 0x0, sizeof(sch_scc_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 20, exit);*/

  ret = SOC_SAND_OK; sal_memset(&sch_sct_tbl_data, 0x0, sizeof(sch_sct_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 21, exit);

  ret = SOC_SAND_OK; sal_memset(&sch_scheduler_init_tbl_data, 0x0, sizeof(sch_scheduler_init_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 26, exit);

  ret = SOC_SAND_OK; sal_memset(&sch_ffm_tbl_data, 0x0, sizeof(sch_ffm_tbl_data));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 27, exit);

  /*Scheduler Credit Generation Calendar (CAL)*/

  if (is_shadow_en)
  {
    /*
     *  Clear the NIF calendars
     */
    for (first_entry = 0; first_entry < SOC_PA_TBL(tables->sch.cal_tbl.addr.size); first_entry += 0x1000)
    {
      res = soc_petra_sch_set_reps_for_tbl_unsafe(unit, 0x400);
      SOC_SAND_CHECK_FUNC_RESULT(res, 101, exit);

      res = soc_petra_sch_cal_tbl_set_unsafe(
                unit,
                first_entry,
                &sch_cal_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 102, exit);
    }

    /*
     *  Clear the CPU and RCY calendars
     */
    for (first_entry = 0x800; first_entry < 0x3c00; first_entry += 0x1000)
    {
      res = soc_petra_sch_set_reps_for_tbl_unsafe(unit, 0x400);
      SOC_SAND_CHECK_FUNC_RESULT(res, 101, exit);

      res = soc_petra_sch_cal_tbl_set_unsafe(
              unit,
              first_entry,
              &sch_cal_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 102, exit);
    }
  }

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
    for (mal_idx = 0; mal_idx < SOC_PETRA_NOF_MAC_LANES; mal_idx++)
    {
      first_entry = (mal_idx*SOC_PETRA_OFP_NOF_RATES_CAL_SETS + cal_set_idx) * SOC_PETRA_OFP_RATES_CAL_LEN_SCH_OFFSET;
      res = soc_petra_sch_cal_tbl_set_unsafe(
              unit,
              first_entry,
              &sch_cal_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 102, exit);
    }

    first_entry = SOC_PETRA_OFP_RATES_CAL_CPU_BASE_SCH_OFFSET + cal_set_idx * SOC_PETRA_OFP_RATES_CAL_LEN_SCH_OFFSET;
    res = soc_petra_sch_cal_tbl_set_unsafe(
            unit,
            first_entry,
            &sch_cal_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 103, exit);

    first_entry = SOC_PETRA_OFP_RATES_CAL_RCY_BASE_SCH_OFFSET + cal_set_idx * SOC_PETRA_OFP_RATES_CAL_LEN_SCH_OFFSET;
    res = soc_petra_sch_cal_tbl_set_unsafe(
            unit,
            first_entry,
            &sch_cal_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 104, exit);
  }
  first_entry = 0;

  /*Dual Shaper Memory (DSM)*/
  nof_lines = SOC_PA_TBL(tables->sch.dsm_tbl.addr.size);
  res = soc_petra_sch_set_reps_for_tbl_unsafe(unit, nof_lines);
  SOC_SAND_CHECK_FUNC_RESULT(res, 105, exit);

  res = soc_petra_sch_dsm_tbl_set_unsafe(
          unit,
          first_entry,
          &sch_dsm_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 106, exit);

  /*Scheduler Enable Memory (SEM)*/
  nof_lines = SOC_PA_TBL(tables->sch.sem_tbl.addr.size);
  res = soc_petra_sch_set_reps_for_tbl_unsafe(unit, nof_lines);
  SOC_SAND_CHECK_FUNC_RESULT(res, 111, exit);

  res = soc_petra_sch_sem_tbl_set_unsafe(
          unit,
          first_entry,
          &sch_sem_tbl_data
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 112, exit);

  /*Flow Sub-Flow (FSF)*/
  nof_lines = SOC_PA_TBL(tables->sch.fsf_tbl.addr.size);
  res = soc_petra_sch_set_reps_for_tbl_unsafe(unit, nof_lines);
  SOC_SAND_CHECK_FUNC_RESULT(res, 113, exit);

  res = soc_petra_sch_fsf_tbl_set_unsafe(
          unit,
          first_entry,
          &sch_fsf_tbl_data
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 114, exit);

  /*Flow Group Memory (FGM)*/
  nof_lines = SOC_PA_TBL(tables->sch.fgm_tbl.addr.size);
  res = soc_petra_sch_set_reps_for_tbl_unsafe(unit, nof_lines);
  SOC_SAND_CHECK_FUNC_RESULT(res, 115, exit);

  res = soc_petra_sch_fgm_tbl_set_unsafe(
          unit,
          first_entry,
          &sch_fgm_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 116, exit);

  /*HR-Scheduler-Configuration (SHC)*/
  nof_lines = SOC_PA_TBL(tables->sch.shc_tbl.addr.size);
  res = soc_petra_sch_set_reps_for_tbl_unsafe(unit, nof_lines);
  SOC_SAND_CHECK_FUNC_RESULT(res, 117, exit);

  res = soc_petra_sch_shc_tbl_set_unsafe(
            unit,
            first_entry,
            &sch_shc_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 118, exit);

  /*CL-Schedulers Configuration (SCC)*/
  /*nof_lines = SOC_PA_TBL(tables->sch.scc_tbl.addr.size);
  res = soc_petra_sch_set_reps_for_tbl_unsafe(unit, nof_lines);
  SOC_SAND_CHECK_FUNC_RESULT(res, 119, exit);

  res = soc_petra_sch_scc_tbl_set_unsafe(
          unit,
          first_entry,
          &sch_scc_tbl_data
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);*/

  /*CL-Schedulers Type (SCT)*/
  nof_lines = SOC_PA_TBL(tables->sch.sct_tbl.addr.size);
  res = soc_petra_sch_set_reps_for_tbl_unsafe(unit, nof_lines);
  SOC_SAND_CHECK_FUNC_RESULT(res, 121, exit);

  res = soc_petra_sch_sct_tbl_set_unsafe(
          unit,
          first_entry,
          &sch_sct_tbl_data
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 122, exit);

  if (is_shadow_en)
  {
    /* Flow to FIP Mapping (FFM) */
    nof_lines = SOC_PA_TBL(tables->sch.ffm_tbl.addr.size);
    res = soc_petra_sch_set_reps_for_tbl_unsafe(unit, nof_lines);
    SOC_SAND_CHECK_FUNC_RESULT(res, 123, exit);

    res = soc_petra_sch_ffm_tbl_set_unsafe(
            unit,
            first_entry,
            &sch_ffm_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 124, exit);
  }

  /*Scheduler Init*/
  sch_scheduler_init_tbl_data.schinit = 0x1;

  res = soc_petra_sch_scheduler_init_tbl_set_unsafe(
          unit,
          first_entry,
          &sch_scheduler_init_tbl_data
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 132, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_sch_tbls_init()", 0, 0);
}

STATIC uint32
  soc_petra_mgmt_tbls_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 is_shadow_en,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_TBLS_INIT);

  Soc_petra_mgmt_tbls_curr_workload = 0;
  soc_petra_mgmt_tbl_total_lines_set();

  /*res = soc_petra_mgmt_olp_tbls_init(unit, silent);
  SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);*/

  res = soc_petra_mgmt_ire_tbls_init(unit, silent);
  SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);

  res = soc_petra_mgmt_irr_tbls_init(unit, silent);
  SOC_SAND_CHECK_FUNC_RESULT(res, 14, exit);

  res = soc_petra_mgmt_ihp_tbls_init(unit, silent);
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  res = soc_petra_mgmt_iqm_tbls_init(unit, silent);
  SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);

  res = soc_petra_mgmt_ips_tbls_init(unit, silent);
  SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

  /*res = soc_petra_mgmt_ipt_tbls_init(unit, silent);
  SOC_SAND_CHECK_FUNC_RESULT(res, 18, exit);*/

  /*res = soc_petra_mgmt_dpi_tbls_init(unit, silent);
  SOC_SAND_CHECK_FUNC_RESULT(res, 19, exit);*/

  /*res = soc_petra_mgmt_rtp_tbls_init(unit, silent);
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);*/

  res = soc_petra_mgmt_egq_tbls_init(unit, silent);
  SOC_SAND_CHECK_FUNC_RESULT(res, 21, exit);

  res = soc_petra_mgmt_cfc_tbls_init(unit, silent);
  SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);

  res = soc_petra_mgmt_sch_tbls_init(unit, is_shadow_en, silent);
  SOC_SAND_CHECK_FUNC_RESULT(res, 23, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_tbls_init()", 0, 0);
}
/*
 *  Tables Initialization }
 */

/************************************************************************/
/* Initialize registers according to the operation mode                 */
/************************************************************************/
STATIC uint32
  soc_petra_mgmt_op_mode_related_init(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    fld_val,
    res = SOC_SAND_OK;
  uint8
    is_var_size_cells,
    bool_val;
  uint32
    nif_grp_ndx;
  SOC_PETRA_MGMT_PCKT_SIZE
    pckt_size_range;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_OP_MODE_RELATED_INIT);

  regs = soc_petra_regs();

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
    SOC_PA_FLD_ISET(regs->nif.nif_config_reg.serdes_g4_en, fld_val, nif_grp_ndx, 10, exit);
  }

  bool_val = soc_petra_sw_db_is_fap20_and_a1_in_system_get(unit);
  if (bool_val == TRUE)
  {
    SOC_PA_FLD_SET(regs->mesh_topology.mesh_topology_reg.fld1, 0x7, 20, exit);
  }

  is_var_size_cells = soc_petra_sw_db_is_fabric_variable_cell_size_get(unit);

  pckt_size_range.min = (is_var_size_cells)?SOC_PETRA_MGMT_PCKT_SIZE_BYTES_VSC_MIN:SOC_PETRA_MGMT_PCKT_SIZE_BYTES_FSC_MIN;
  pckt_size_range.max = (is_var_size_cells)?SOC_PETRA_MGMT_PCKT_SIZE_BYTES_VSC_MAX:SOC_PETRA_MGMT_PCKT_SIZE_BYTES_FSC_MAX;

  res = soc_petra_mgmt_pckt_size_range_set_unsafe(
          unit,
          SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE_INTERN,
          &pckt_size_range
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_op_mode_related_init()", 0, 0);
}

/*
 *  Init sequence -
 *  per-block initialization, hardware adjustments etc. {
 */
STATIC uint32
  soc_petra_mgmt_functional_init(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_HW_ADJUSTMENTS      *hw_adjust,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    qdr_nof_entries,
    res = SOC_SAND_OK;
  SOC_PETRA_STAT_ALL_STATISTIC_COUNTERS
    counters;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_FUNCTIONAL_INIT);

  soc_petra_PETRA_STAT_ALL_STATISTIC_COUNTERS_clear(&counters);

  qdr_nof_entries = soc_petra_init_qdr_size_to_bits(hw_adjust->qdr.qdr_size_mbit) / SOC_PETRA_INIT_QDR_ENTRY_SIZE_BITS;

  res = soc_petra_mgmt_op_mode_related_init(
          unit
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
  res = soc_petra_interrupt_mask_clear_unsafe(
	  unit
	  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);

  res = soc_petra_interrupt_initial_mask_lift(
	  unit
	  ) ;
  SOC_SAND_CHECK_FUNC_RESULT(res, 52, exit);


  res = soc_petra_egr_queuing_init(
        unit
      );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_fabric_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_petra_flow_control_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_ingress_header_parsing_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  res = soc_petra_ingress_scheduler_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_petra_nif_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  res = soc_petra_interrupt_init(
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


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_functional_init()", 0, 0);
}

STATIC uint32
  soc_petra_mgmt_hw_set_defaults(
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
  SOC_PETRA_REG_FIELD
    fld;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_HW_SET_DEFAULTS);

  regs = soc_petra_regs();

  /*
   *  IDR
   */
  SOC_PETRA_REG_GET(regs->idr.static_configuration_reg, reg_val, 10, exit);

  fld_val = 0x1;
  SOC_PETRA_FLD_TO_REG(regs->idr.static_configuration_reg.fbc_internal_reuse, fld_val, reg_val, 20, exit);

  SOC_PETRA_REG_SET(regs->idr.static_configuration_reg, reg_val, 60, exit);

  /*
   *  Check if CPU uses streaming interface,
   *  configure IRR accordingly
   */
  SOC_PA_FLD_GET(regs->eci.power_down_and_cpu_streaming_if_statuses_reg.cpuif_mode, fld_val, 70, exit);

  fld_val =
   (fld_val == 0x1)?0x0:0x1;

  SOC_PA_FLD_SET(
    regs->ire.cpu_interface_select_non_streaming_packet_path_reg.cpu_if_sel_non_stream_pkt, fld_val,
    80 , exit
  );

  SOC_PA_FLD_SET(regs->idr.dynamic_configuration_reg.check_min_size, 0x1, 82, exit);
  SOC_PA_FLD_SET(regs->idr.dynamic_configuration_reg.check_max_size, 0x1, 84, exit);
  SOC_PA_FLD_SET(regs->idr.dynamic_configuration_reg.check_min_org_size, 0x0, 86, exit);
  SOC_PA_FLD_SET(regs->idr.dynamic_configuration_reg.check_max_org_size, 0x0, 88, exit);
  SOC_PA_FLD_SET(regs->idr.reassembly_timeout_reg.reassembly_timeout, 0xFFFFFF, 90, exit);

  /*
   *  IQM
   */
  SOC_PA_REG_SET(regs->iqm.iqm_enablers_reg, 0x0, 90, exit);
  if (SOC_PETRA_REV_A3_OR_BELOW)
  {
    SOC_PA_FLD_SET(regs->iqm.bdb_configuration_reg.recycle_fr_bdb, 0x0, 95, exit);
  }

  /*
   *  IPS
   */
  SOC_PA_REG_SET(regs->ips.ips_general_configurations_reg, 0xa00, 100, exit);

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
   *  EGQ/EPNI
   */
  SOC_PA_FLD_SET(regs->epni.sequence_number_masking_disable_reg.seqnum_msk_dis, 0x1, 120, exit);

  /*
   *  Scheduler
   */
  SOC_PA_REG_SET(regs->sch.dvs_config0_reg, 0x0, 122, exit);

  fld_val = 0x0;
  SOC_PA_REG_GET(regs->sch.dvs_config1_reg, reg_val, 124, exit);
  SOC_PA_FLD_TO_REG(regs->sch.dvs_config1_reg.cpuforce_pause, fld_val, reg_val, 125, exit);
  SOC_PA_FLD_TO_REG(regs->sch.dvs_config1_reg.rcyforce_pause, fld_val, reg_val, 126, exit);
  SOC_PA_FLD_TO_REG(regs->sch.dvs_config1_reg.olpforce_pause, fld_val, reg_val, 127, exit);
  SOC_PA_FLD_TO_REG(regs->sch.dvs_config1_reg.erpforce_pause, fld_val, reg_val, 128, exit);
  SOC_PA_REG_SET(regs->sch.dvs_config1_reg, reg_val, 129, exit);

  SOC_PA_FLD_SET(regs->sch.rci_params_reg.rciena, 0x1, 138, exit);
  SOC_PA_FLD_SET(regs->sch.rci_params_reg.rciinc_val, 0x20, 139, exit);

  /*
   *  Disable RLAG
   */
  SOC_PA_FLD_SET(regs->eci.rlag2_configuration_reg.rlag2_low, 0x1, 130, exit);
  SOC_PA_FLD_SET(regs->eci.rlag2_configuration_reg.rlag2_high, 0x0, 131, exit);
  SOC_PA_FLD_SET(regs->eci.rlag4_configuration_reg.rlag4_low, 0x1, 132, exit);
  SOC_PA_FLD_SET(regs->eci.rlag4_configuration_reg.rlag4_high, 0x0, 133, exit);

  SOC_PA_FLD_SET(regs->egq.rlag2_system_port_configuration_reg.rlag2_sys_port_id_bot, 0x1, 134, exit);
  SOC_PA_FLD_SET(regs->egq.rlag2_system_port_configuration_reg.rlag2_sys_port_id_top, 0x0, 135, exit);
  SOC_PA_FLD_SET(regs->egq.rlag4_sys_port_id_reg.rlag4_sys_port_id_bot, 0x1, 136, exit);
  SOC_PA_FLD_SET(regs->egq.rlag4_sys_port_id_reg.rlag4_sys_port_id_top, 0x0, 137, exit);

  /*
   *  FDT
   */
  SOC_PA_FLD_SET(regs->fdt.fdt_enabler_reg.del_crc_pkt, 0x1, 140, exit);

  /*
   *  RTP
   */
  SOC_PA_FLD_SET(regs->rtp.rtp_enable_reg.aclm, 0x0, 150, exit);

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
    SOC_PA_FLD_ISET(regs->fabric_mac.enablers_reg.tx_cnt_cfg, 0, inst_idx, 160, exit);

    /*
     * Setting the MACs RX-TX to count data cells
     * The counter_all that counts the good cells to up count the
     * leaky bucket counts non-empty data and control cells
     */
    SOC_PA_FLD_ISET(regs->fabric_mac.enablers_reg.rx_cnt_cfg, 0, inst_idx, 162, exit);
  }

  res = soc_petra_nif_regs_init(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 165, exit);

  /*
   *  Revision-dependand
   */
   if (SOC_PETRA_REV_ABOVE_A1)
   {
      fld.addr.step = 0x0;

      fld.addr.base = 0x3452 * SOC_SAND_REG_SIZE_BYTES;
      fld.lsb = 0x0; fld.msb = 0x0;
      SOC_PA_IMPLICIT_FLD_SET(fld, 0x0, 170, exit);

      fld.addr.base = 0x2e52 * SOC_SAND_REG_SIZE_BYTES;
      fld.lsb = 0x0; fld.msb = 0x0;
      SOC_PA_IMPLICIT_FLD_SET(fld, 0x1, 172, exit);

      fld.addr.base = 0x3853 * SOC_SAND_REG_SIZE_BYTES;
      fld.lsb = 0x0; fld.msb = 0x1;
      SOC_PA_IMPLICIT_FLD_SET(fld, 0x3, 174, exit);

      fld.addr.base = 0x5050 * SOC_SAND_REG_SIZE_BYTES;
      fld.lsb = 0x0; fld.msb = 0x3;
      SOC_PA_IMPLICIT_FLD_SET(fld, 0x0, 176, exit);

      fld.addr.base = 0x5450 * SOC_SAND_REG_SIZE_BYTES;
      fld.lsb = 0x0; fld.msb = 0x3;
      SOC_PA_IMPLICIT_FLD_SET(fld, 0x0, 178, exit);

      fld_val = 0x1;
      for (inst_idx = 0; inst_idx < SOC_PETRA_NOF_MAC_LANES; inst_idx++)
      {
        SOC_PA_REG_IGET(regs->nif_mac_lane.mal_config1_reg, reg_val, inst_idx, 180, exit);
        SOC_PA_FLD_TO_REG(regs->nif_mac_lane.mal_config1_reg.cbfc_edge_detector, fld_val, reg_val, 182, exit);
        SOC_PA_FLD_TO_REG(regs->nif_mac_lane.mal_config1_reg.llfc_edge_detector, fld_val, reg_val, 183, exit);
        SOC_PA_FLD_TO_REG(regs->nif_mac_lane.mal_config1_reg.refresh_timer_events, fld_val, reg_val, 184, exit);
        SOC_PA_REG_ISET(regs->nif_mac_lane.mal_config1_reg, reg_val, inst_idx, 185, exit);
      }

      fld.addr.base = 0x2652 * SOC_SAND_REG_SIZE_BYTES;
      SOC_PA_IMPLICIT_REG_GET(fld, reg_val, 188, exit);
      fld.lsb = 0x0; fld.msb = 0x0; fld_val = 0x1;
      SOC_PA_IMPLICIT_FLD_TO_REG(fld, fld_val, reg_val, 189, exit);
      fld.lsb = 0x1; fld.msb = 0x1; fld_val = 0x1;
      SOC_PA_IMPLICIT_FLD_TO_REG(fld, fld_val, reg_val, 189, exit);
      SOC_PA_IMPLICIT_REG_SET(fld, reg_val, 190, exit);
   }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_hw_set_defaults()", 0, 0);
}

uint32
  soc_pa_mgmt_hw_adjust_ddr_init(
    SOC_SAND_IN int          unit,
    SOC_SAND_IN uint32          instance_idx
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;
 
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_HW_ADJUST_DDR_INIT);

  regs = soc_petra_regs();

  /* DPRC in/out of reset */
  SOC_PETRA_FLD_SET(regs->eci.soc_petra_soft_reset_reg.dprc_reset[instance_idx], 0x1, 10, exit);
  SOC_PETRA_FLD_SET(regs->eci.soc_petra_soft_reset_reg.dprc_reset[instance_idx], 0x0, 20, exit);

  /*
   *  DRAM Duty Cycle
   */
  if (SOC_PETRA_REV_ABOVE_A1)
  {
    SOC_PA_FLD_ISET(regs->drc.dcf.dcf_a, SOC_PETRA_MGMT_HW_ADJUST_DDR_A_BL, instance_idx, 30, exit);
    SOC_PA_FLD_ISET(regs->drc.dcf.dcf_p, SOC_PETRA_MGMT_HW_ADJUST_DDR_P_BL, instance_idx, 40, exit);
    SOC_PA_FLD_ISET(regs->drc.dcf.dcf_disable, SOC_PETRA_MGMT_HW_ADJUST_DDR_DIS_BL, instance_idx, 50, exit);

    SOC_PA_FLD_SET(regs->eci.oc768c_and_misc_reg.dcf_a, SOC_PETRA_MGMT_HW_ADJUST_DDR_A_TL, 60, exit);
    SOC_PA_FLD_SET(regs->eci.oc768c_and_misc_reg.dcf_p, SOC_PETRA_MGMT_HW_ADJUST_DDR_P_TL, 70, exit);
    SOC_PA_FLD_SET(regs->eci.oc768c_and_misc_reg.dcf_disable, SOC_PETRA_MGMT_HW_ADJUST_DDR_DIS_TL, 80, exit);

    SOC_PA_FLD_ISET(regs->dpi.dprc_enable_reg.dprc_en, 0x1, instance_idx, 90, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_mgmt_hw_adjust_ddr_init()", 0, 0);
}
STATIC uint32
  soc_petra_mgmt_hw_adjust_ddr(
    SOC_SAND_IN int          unit,
    SOC_SAND_IN SOC_PETRA_HW_ADJ_DDR*  hw_adjust
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
    acc_idx = 0,
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

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_HW_ADJUST_DDR);

  SOC_SAND_CHECK_NULL_INPUT(hw_adjust);

  regs = soc_petra_regs();

  soc_petra_PETRA_POLL_INFO_clear(&poll_info);
  soc_petra_PETRA_DIAG_DRAM_ERR_INFO_clear(&error_info);
  soc_petra_PETRA_DIAG_DRAM_STATUS_INFO_clear(&status_info);

  SOC_PETRA_FLD_GET(regs->eci.general_controls_reg.dbuff_size, fld_val, 5, exit);
  res = soc_petra_itm_dbuff_internal2size(
          fld_val,
          &dbuff_size
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  /*
   *	CAUTION: the following register has in practice a different
   *           appearance in Soc_petra-B (Alignment with Soc_petra-A).
   */
  SOC_PETRA_FLD_GET(regs->eci.test_mux_config_reg.ddr_lock, fld_val, 30, exit);

  if (fld_val != 0x1)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_INIT_DDR_PLL_NOT_LOCKED_ERR, 33, exit);
  }

  for (instance_idx = 0; instance_idx < SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max); ++instance_idx)
  {
    if (hw_adjust->is_valid[instance_idx])
    {
      nof_drams++;
      res = soc_pa_mgmt_hw_adjust_ddr_init(
              unit,
              instance_idx
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 18, exit);
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
          SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DRAM_NOF_BANKS_OUT_OF_RANGE_ERR, 13, exit);
        }
        if (hw_adjust->nof_columns != hw_adjust->dram_conf.params_mode.params.nof_cols)
        {
          SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DRAM_NOF_COLS_OUT_OF_RANGE_ERR, 14, exit);
        }

        /*
         *	Save in the SW_DB the DRAM configuration
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
      }
      else if (hw_adjust->conf_mode == SOC_PETRA_HW_DRAM_CONF_MODE_BUFFER)
      {
        for (acc_idx = 0; acc_idx < hw_adjust->dram_conf.buffer_mode.buff_len; ++acc_idx)
        {
#ifdef LINK_PA_LIBRARIES
          if (soc_pa_regs_is_dprc_reg(&(hw_adjust->dram_conf.buffer_mode.buff_seq[acc_idx].addr)))
          {
            SOC_PA_IMPLICIT_REG_ISET(hw_adjust->dram_conf.buffer_mode.buff_seq[acc_idx], hw_adjust->dram_conf.buffer_mode.buff_seq[acc_idx].val, instance_idx, 50, exit);
          }
          else
          {
            SOC_PA_IMPLICIT_REG_SET(hw_adjust->dram_conf.buffer_mode.buff_seq[acc_idx], hw_adjust->dram_conf.buffer_mode.buff_seq[acc_idx].val, 51, exit);
          }
#endif
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
  SOC_PA_REG_GET(regs->mmu.general_configuration_reg, reg_val, 55, exit);

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

  SOC_PA_FLD_TO_REG(regs->mmu.general_configuration_reg.dram_bank_num, fld_val, reg_val, 60, exit);

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
  SOC_PA_FLD_TO_REG(regs->mmu.general_configuration_reg.dram_num, fld_val, reg_val, 65, exit);

  fld_val = 0x0;
  SOC_PA_FLD_TO_REG(regs->mmu.general_configuration_reg.scrambling_bit_position, fld_val, reg_val, 70, exit);

  fld_val = hw_adjust->nof_columns;
  SOC_PA_FLD_TO_REG(regs->mmu.general_configuration_reg.dram_col_num, fld_val, reg_val, 80, exit);

  fld_val = 0x1;
  SOC_PA_FLD_TO_REG(regs->mmu.general_configuration_reg.interleaving_rvrse_mode, fld_val, reg_val, 90, exit);

  fld_val = 0;
  SOC_PA_FLD_TO_REG(regs->mmu.general_configuration_reg.cons_banks_addr_map_mode, fld_val, reg_val, 95, exit);

  if (hw_adjust->dram_type == SOC_PETRA_DRAM_TYPE_GDDR3)
  {
    fld_val = (hw_adjust->nof_banks == SOC_PETRA_DRAM_NUM_BANKS_4)?0x2:0x5;
    SOC_PA_FLD_TO_REG(regs->mmu.general_configuration_reg.read_fifos_allowed, fld_val, reg_val, 100, exit);

    fld_val = (hw_adjust->nof_banks == SOC_PETRA_DRAM_NUM_BANKS_4)?0x8:0xF;
    SOC_PA_FLD_TO_REG(regs->mmu.general_configuration_reg.min_read_commands, fld_val, reg_val, 101, exit);
  }
  else
  {
    /* DDR2/DDR3 */
    fld_val = (hw_adjust->nof_banks == SOC_PETRA_DRAM_NUM_BANKS_4)?0x2:0x3;
    SOC_PA_FLD_TO_REG(regs->mmu.general_configuration_reg.read_fifos_allowed, fld_val, reg_val, 102, exit);

    fld_val = (hw_adjust->nof_banks == SOC_PETRA_DRAM_NUM_BANKS_4)?0x8:0xa;
    SOC_PA_FLD_TO_REG(regs->mmu.general_configuration_reg.min_read_commands, fld_val, reg_val, 103, exit);
  }

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

  SOC_PA_FLD_TO_REG(regs->mmu.general_configuration_reg.layer_periodicity, fld_val, reg_val, 110, exit);

  if ((nof_drams == 6) || (nof_drams == 3))
  {
    fld_val = 0;
  }
  else
  {
    drams_banks_log = soc_sand_log2_round_up(hw_adjust->nof_banks*nof_drams);
    buff_size_log = soc_sand_log2_round_up((hw_adjust->dbuff_size)/64);

    SOC_PA_FLD_GET(regs->mmu.bank_access_controller_configurations_reg.address_map_config_gen, same_row, 112, exit);

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

  SOC_PA_FLD_TO_REG(regs->mmu.general_configuration_reg.bank_interleaving_mode, fld_val, reg_val, 115, exit);

  SOC_PA_REG_SET(regs->mmu.general_configuration_reg, reg_val, 117, exit);

  SOC_PA_REG_GET(regs->mmu.bank_access_controller_configurations_reg, reg_val, 119, exit);

  fld_val = 0x60;
  SOC_PA_FLD_TO_REG(regs->mmu.bank_access_controller_configurations_reg.write_inhibit_cnt_sat, fld_val, reg_val, 120, exit);

  fld_val = 0x1;
  SOC_PA_FLD_TO_REG(regs->mmu.bank_access_controller_configurations_reg.address_map_config_gen, fld_val, reg_val, 125, exit);

  fld_val = 0x1;
  SOC_PA_FLD_TO_REG(regs->mmu.bank_access_controller_configurations_reg.address_map_config_rd, fld_val, reg_val, 130, exit);

  fld_val = (hw_adjust->nof_banks == SOC_PETRA_DRAM_NUM_BANKS_4)?0x3:0x5;
  SOC_PA_FLD_TO_REG(regs->mmu.bank_access_controller_configurations_reg.wr_cmd_distance, fld_val, reg_val, 135, exit);

  fld_val = 0x0;
  SOC_PA_FLD_TO_REG(regs->mmu.bank_access_controller_configurations_reg.allow_consecutive16_byte, fld_val, reg_val, 140, exit);

  fld_val = 0x1;
  SOC_PA_FLD_TO_REG(regs->mmu.bank_access_controller_configurations_reg.per_bank_half_mode, fld_val, reg_val, 145, exit);

  if (hw_adjust->dram_type == SOC_PETRA_DRAM_TYPE_GDDR3)
  {
    fld_val = 0x0;
    SOC_PA_FLD_TO_REG(regs->mmu.bank_access_controller_configurations_reg.wr_priority_mode, fld_val, reg_val, 150, exit);
  }
  else
  {
    /* DDR2/DDR3 */
    fld_val = 0x1;
    SOC_PA_FLD_TO_REG(regs->mmu.bank_access_controller_configurations_reg.wr_priority_mode, fld_val, reg_val, 150, exit);
  }

  SOC_PA_REG_SET(regs->mmu.bank_access_controller_configurations_reg, reg_val, 155, exit);

  /*
   *  Verify DRC init complete
   */
  poll_info.expected_value = 0x1;
  poll_info.busy_wait_nof_iters = SOC_PETRA_MGMT_INIT_DRAM_BUSY_WAIT_ITERATIONS;
  poll_info.timer_nof_iters     = SOC_PETRA_MGMT_INIT_DRAM_TIMER_ITERATIONS;
  poll_info.timer_delay_msec    = SOC_PETRA_MGMT_INIT_DRAM_TIMER_DELAY_MSEC;
  for (instance_idx = 0; instance_idx < SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max); ++instance_idx)
  {
    if (hw_adjust->is_valid[instance_idx])
    {
      res = soc_petra_status_fld_poll_unsafe(
              unit,
              SOC_PA_REG_DB_ACC_REF(regs->dpi.dpi_init_status_reg.ready),
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
          SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MGMT_DRAM_INIT_FAILS_ERR, 170 + instance_idx, exit);
        }

        if (round_trip_err_count > 0)
        {
          SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MGMT_DRAM_INIT_FAILS_ERR, 180 + instance_idx, exit);
        }
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_hw_adjust_ddr()", 0, 0);
}

STATIC uint32
  soc_petra_mgmt_hw_adjust_qdr(
    SOC_SAND_IN int          unit,
    SOC_SAND_IN SOC_PETRA_HW_ADJ_QDR*  hw_adjust
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

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_HW_ADJUST_QDR);

  SOC_SAND_CHECK_NULL_INPUT(hw_adjust);

  regs = soc_petra_regs();

  soc_petra_PETRA_POLL_INFO_clear(&poll_info);

  if (!hw_adjust->is_core_clock_freq)
  {
    SOC_PETRA_FLD_GET(regs->eci.test_mux_config_reg.qdr_lock, fld_val, 30, exit);
    if (fld_val != 0x1)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_INIT_QDR_PLL_NOT_LOCKED_ERR, 33, exit);
    }
  }

  /* Value 0 - 3 corresponds to Number of BDBS 0/16/32/64 */
  qdr_bdb_size_fld_val = hw_adjust->qdr_size_mbit;

  /* BDB size */
  SOC_PETRA_FLD_SET(regs->iqm.bdb_configuration_reg.bdb_size, qdr_bdb_size_fld_val, 35, exit);

  SOC_PETRA_FLD_SET(regs->qdr.dll_control_reg.dll_div2_en, 0x1, 36, exit);

  /*
   *  QDR OOR
   */

  SOC_PETRA_FLD_SET(regs->qdr.qdr_controller_reset_reg.qdrc_rst_n, 0x1, 37, exit);
  sal_msleep(SOC_PETRA_MGMT_INIT_TIMER_DELAY_MSEC);
  SOC_PETRA_FLD_GET(regs->qdr.qdr_controller_reset_reg.qdrc_rst_n, fld_val, 38, exit);

  SOC_PETRA_FLD_SET(regs->qdr.qdr_phy_reset_reg.qdio_reset, 0x1, 39, exit);
  sal_msleep(SOC_PETRA_MGMT_INIT_QDR_IO_RESET_DELAY_MSEC);

  /*
   *  QDR initialization - verify ready
   */
  poll_info.expected_value = 0x1;
  poll_info.busy_wait_nof_iters = SOC_PETRA_MGMT_INIT_QDR_BUSY_WAIT_ITERATIONS;
  poll_info.timer_nof_iters     = SOC_PETRA_MGMT_INIT_QDR_TIMER_ITERATIONS;
  poll_info.timer_delay_msec    = SOC_PETRA_MGMT_INIT_QDR_TIMER_DELAY_MSEC;

  res = soc_petra_status_fld_poll_unsafe(
          unit,
          SOC_PETRA_REG_DB_ACC_REF(regs->qdr.qdr_init_status_reg.ready),
          SOC_PETRA_DEFAULT_INSTANCE,
          &poll_info,
          &poll_success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  if (poll_success == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MGMT_QDR_NOT_READY_ERR, 42, exit);
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
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MGMT_QDRC_NOT_LOCKED_ERR, 52, exit);
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
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MGMT_QDR_NOT_READY_ERR, 62, exit);
  }

  soc_petra_sw_db_qdr_size_set(unit, hw_adjust->qdr_size_mbit);
  soc_petra_sw_db_qdr_protection_type_set(unit, hw_adjust->protection_type);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_hw_adjust_qdr()", 0, 0);
}

STATIC uint32
  soc_petra_mgmt_hw_adjust_serdes(
    SOC_SAND_IN int            unit,
    SOC_SAND_IN SOC_PETRA_HW_ADJ_SERDES*  hw_adjust
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_HW_ADJUST_SERDES);

  SOC_SAND_CHECK_NULL_INPUT(hw_adjust);

  res = soc_petra_srd_all_set_unsafe(
          unit,
          &(hw_adjust->conf)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_hw_adjust_serdes()", 0, 0);
}

STATIC uint32
  soc_petra_mgmt_hw_adjust_fabric(
    SOC_SAND_IN int             unit,
    SOC_SAND_IN SOC_PETRA_HW_ADJ_FABRIC*  hw_adjust
  )
{
  uint32
    fld_val = 0,
    res;
  uint8
    is_fap20_in_system,
    is_fap21_in_system,
    is_fap2x_in_system,
    is_fe200_fabric,
    is_a1_or_below_in_system,
    is_variable_cell_size_enable,
    is_egr_mc_16k_groups_enable;
  SOC_PETRA_FABRIC_CELL_FORMAT
    cell_format;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_HW_ADJUST_FABRIC);

  SOC_SAND_CHECK_NULL_INPUT(hw_adjust);

  regs = soc_petra_regs();

  soc_petra_PETRA_FABRIC_CELL_FORMAT_clear(&cell_format);

  is_fap20_in_system = soc_petra_sw_db_is_fap20_in_system_get(unit);
  is_fap21_in_system = soc_petra_sw_db_is_fap21_in_system_get(unit);
  is_fap2x_in_system = SOC_SAND_NUM2BOOL(is_fap20_in_system || is_fap21_in_system);
  is_fe200_fabric = soc_petra_sw_db_is_fe200_fabric_get(unit);
  is_a1_or_below_in_system = soc_petra_sw_db_is_a1_or_below_in_system_get(unit);
  is_variable_cell_size_enable = soc_petra_sw_db_is_fabric_variable_cell_size_get(unit);

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
    SOC_PETRA_MGMT_INIT_EGQ_MAX_FRG_VAR:SOC_PETRA_MGMT_INIT_EGQ_MAX_FRG_FIX;

  SOC_PETRA_FLD_SET(regs->egq.maximum_fragment_number_reg.max_frg_num, fld_val, 15, exit);

  res = soc_petra_fabric_connect_mode_set_unsafe(
    unit,
    hw_adjust->connect_mode
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 17, exit);

  if ((is_a1_or_below_in_system == TRUE) && (is_variable_cell_size_enable == FALSE))
  {
    if(is_fap2x_in_system == TRUE)
    {
      SOC_PA_FLD_SET(regs->mesh_topology.mesh_config_1_reg.mesh_config_1_reg, 0, 20, exit);
      if (hw_adjust->connect_mode != SOC_PETRA_FABRIC_CONNECT_MODE_BACK2BACK)
      {
        SOC_PA_FLD_SET(regs->mesh_topology.mesh_topology_reg.in_system, 0x20, 23, exit);
      }
      SOC_PA_FLD_SET(regs->mesh_topology.mesh_config_0_reg.conf1, 0x20, 25, exit);
      SOC_PA_FLD_SET(regs->mesh_topology.mesh_config_0_reg.conf2, 0x20, 26, exit);
    }
    else
    {
      SOC_PA_FLD_SET(regs->mesh_topology.mesh_config_1_reg.mesh_config_1_reg, 0xaa, 27, exit);
      SOC_PA_FLD_SET(regs->mesh_topology.mesh_config_2_reg.mesh_config_2_reg, 0x207, 28, exit);
    }
  }

  if (is_egr_mc_16k_groups_enable == TRUE)
  {
    SOC_PETRA_FLD_SET(regs->eci.general_controls_reg.en_16k_mul, 0x1, 50, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_hw_adjust_fabric()", 0, 0);
}

/*********************************************************************
*     Initialize a sub-set of the HW interfaces of the device.
*     The function might be called more than once, each time
*     with different fields, indicated to be written to the
*     device
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mgmt_hw_interfaces_set_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  SOC_PETRA_HW_ADJUSTMENTS*    hw_adjust,
    SOC_SAND_IN  uint8                silent
  )
{
  uint32
    res;
  uint32
    idx,
    stage_id = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_HW_INTERFACES_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(hw_adjust);

  if(hw_adjust->dram.enable)
  {
    SOC_PETRA_INIT_PRINT_INTERNAL_ADVANCE("DDR");
    res = soc_petra_mgmt_hw_adjust_ddr(
            unit,
            &(hw_adjust->dram)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }
  if(hw_adjust->qdr.enable)
  {
    SOC_PETRA_INIT_PRINT_INTERNAL_ADVANCE("QDR");
    res = soc_petra_mgmt_hw_adjust_qdr(
            unit,
            &(hw_adjust->qdr)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

  if(hw_adjust->serdes.enable)
  {
    SOC_PETRA_INIT_PRINT_INTERNAL_ADVANCE("SerDes");
    res = soc_petra_mgmt_hw_adjust_serdes(
            unit,
            &(hw_adjust->serdes)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

  if(hw_adjust->fabric.enable)
  {
    SOC_PETRA_INIT_PRINT_INTERNAL_ADVANCE("Fabric");
    res = soc_petra_mgmt_hw_adjust_fabric(
            unit,
            &(hw_adjust->fabric)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

  if (hw_adjust->nif.mal_nof_entries != 0)
  {
    SOC_PETRA_INIT_PRINT_INTERNAL_ADVANCE("NIF basic configuration");
  }
  for (idx = 0; idx < hw_adjust->nif.mal_nof_entries; idx++)
  {
    res = soc_petra_nif_mal_basic_conf_set_unsafe(
            unit,
            hw_adjust->nif.mal[idx].mal_ndx,
            &(hw_adjust->nif.mal[idx].conf)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

  if (hw_adjust->nif.spaui_nof_entries != 0)
  {
    SOC_PETRA_INIT_PRINT_INTERNAL_ADVANCE("NIF SPAUI extensions");
  }
  for (idx = 0; idx < hw_adjust->nif.spaui_nof_entries; idx++)
  {
    res = soc_petra_nif_spaui_conf_set_unsafe(
            unit,
            hw_adjust->nif.spaui[idx].mal_ndx,
            SOC_PETRA_CONNECTION_DIRECTION_BOTH,
            &(hw_adjust->nif.spaui[idx].conf)
          );
     SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

  if (hw_adjust->nif.sgmii_nof_entries != 0)
  {
    SOC_PETRA_INIT_PRINT_INTERNAL_ADVANCE("NIF SGMII configuration");
  }
  for (idx = 0; idx < hw_adjust->nif.sgmii_nof_entries; idx++)
  {
    res = soc_petra_nif_sgmii_conf_set_unsafe(
            unit,
            hw_adjust->nif.sgmii[idx].nif_ndx,
            SOC_PETRA_CONNECTION_DIRECTION_BOTH,
            &(hw_adjust->nif.sgmii[idx].conf)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  }

  if (hw_adjust->nif.fat_pipe_enable)
  {
    if (hw_adjust->nif.fat_pipe.enable)
    {
      SOC_PETRA_INIT_PRINT_INTERNAL_ADVANCE("NIF Fat Pipe - enable");
    }
    else
    {
      SOC_PETRA_INIT_PRINT_INTERNAL_ADVANCE("NIF Fat Pipe - disable");
    }

    res = soc_petra_nif_fat_pipe_set_unsafe(
            unit,
            &(hw_adjust->nif.fat_pipe.conf)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
  }

  res = soc_petra_stat_if_report_info_set_unsafe(
          unit,
          &(hw_adjust->stat_if.rep_conf)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  if (hw_adjust->core_freq.enable)
  {
    soc_petra_chip_kilo_ticks_per_sec_set(
      unit,
      hw_adjust->core_freq.frequency * 1000
    );
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_hw_interfaces_set_unsafe()", 0, 0);
}

/*********************************************************************
*     Initialize a sub-set of the HW interfaces of the device.
*     The function might be called more than once, each time
*     with different fields, indicated to be written to the
*     device
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mgmt_hw_interfaces_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_HW_ADJUSTMENTS      *hw_adjust
  )
{
  uint32
    res;
  uint32
    idx,
    dram_count = 0;
  SOC_PETRA_MGMT_OPERATION_MODE
    op_mode;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_HW_INTERFACES_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(hw_adjust);

  SOC_SAND_MAGIC_NUM_VERIFY(hw_adjust);

  if(hw_adjust->dram.enable)
  {
    SOC_SAND_ERR_IF_OUT_OF_RANGE(
      hw_adjust->dram.conf_mode, SOC_PETRA_HW_DRAM_CONF_MODE_MIN, SOC_PETRA_HW_DRAM_CONF_MODE_MAX,
      SOC_PETRA_DRAM_NOF_CONF_MODE_OUT_OF_RANGE_ERR, 5, exit
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
  }

  if(hw_adjust->qdr.enable)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      hw_adjust->qdr.protection_type, SOC_PETRA_HW_NOF_QDR_PROTECT_TYPES-1,
      SOC_PETRA_HW_QDR_PROTECT_TYPE_INVALID_ERR, 25, exit
    );
  }

  if(hw_adjust->qdr.enable)
  {
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
    res = soc_petra_mgmt_operation_mode_get_unsafe(
             unit,
             &op_mode
           );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    res = soc_petra_fabric_connect_mode_verify(
            unit,
            hw_adjust->fabric.connect_mode
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 88, exit);

    res = soc_petra_ports_ftmh_extension_verify(
            unit,
            hw_adjust->fabric.ftmh_extension
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

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

  for (idx = 0; idx < hw_adjust->nif.mal_nof_entries; idx++)
  {
    res = soc_petra_nif_mal_basic_conf_verify(
            unit,
            hw_adjust->nif.mal[idx].mal_ndx,
            &(hw_adjust->nif.mal[idx].conf)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 95, exit);
  }

  for (idx = 0; idx < hw_adjust->nif.spaui_nof_entries; idx++)
  {
    res = soc_petra_nif_spaui_conf_verify(
            unit,
            hw_adjust->nif.spaui[idx].mal_ndx,
            SOC_PETRA_CONNECTION_DIRECTION_BOTH,
            &(hw_adjust->nif.spaui[idx].conf)
          );
     SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
  }

  for (idx = 0; idx < hw_adjust->nif.sgmii_nof_entries; idx++)
  {
    res = soc_petra_nif_sgmii_conf_verify(
            unit,
            hw_adjust->nif.sgmii[idx].nif_ndx,
            SOC_PETRA_CONNECTION_DIRECTION_BOTH,
            &(hw_adjust->nif.sgmii[idx].conf)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
  }

  if (hw_adjust->nif.fat_pipe_enable)
  {
    res = soc_petra_nif_fat_pipe_verify(
            unit,
            &(hw_adjust->nif.fat_pipe.conf)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);
  }

  res = soc_petra_stat_if_info_verify(
          unit,
          &(hw_adjust->stat_if.if_conf)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 125, exit);

  res = soc_petra_stat_if_report_info_verify(
          unit,
          &(hw_adjust->stat_if.rep_conf)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_hw_interfaces_verify()", 0, 0);
}


/*********************************************************************
*     Initialize of Soc_petra internal blocks.
*     Details: in the H file. (search for prototype)
*********************************************************************/

STATIC uint32
  soc_petra_mgmt_blocks_init_unsafe(
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

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_BLOCKS_INIT_UNSAFE);

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
  sal_msleep(SOC_PETRA_MGMT_INIT_TIMER_DELAY_MSEC);

  /*
   *  EGQ initialization - verify init is finished
   */
  poll_info.expected_value = 0x0;
  poll_info.busy_wait_nof_iters = SOC_PETRA_MGMT_INIT_EGQ_BUSY_WAIT_ITERATIONS;
  poll_info.timer_nof_iters     = SOC_PETRA_MGMT_INIT_EGQ_TIMER_ITERATIONS;
  poll_info.timer_delay_msec    = SOC_PETRA_MGMT_INIT_EGQ_TIMER_DELAY_MSEC;

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
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_blocks_init_unsafe()", 0, 0);
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
  soc_petra_mgmt_init_sequence_phase1_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_HW_ADJUSTMENTS      *hw_adjust,
    SOC_SAND_IN  SOC_PETRA_INIT_BASIC_CONF     *basic_conf,
    SOC_SAND_IN  SOC_PETRA_INIT_PORTS          *fap_ports,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    nof_dram_buffs = 0,
    res;
  uint32
    stage_id = 0,
    idx;
  SOC_PETRA_INIT_DBUFFS_BDRY
    dbuffs_bdries;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_INIT_SEQUENCE_PHASE1_UNSAFE);

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
    soc_petra_chip_kilo_ticks_per_sec_set(
      unit,
      hw_adjust->core_freq.frequency * 1000
    );
  }

  /************************************************************************/
  /* Prepare internal data                                                */
  /************************************************************************/
  res = soc_petra_init_dram_nof_buffs_calc(
          hw_adjust->dram.dram_size_total_mbyte,
          hw_adjust->dram.dbuff_size,
          hw_adjust->qdr.protection_type,
          hw_adjust->qdr.qdr_size_mbit,
          &nof_dram_buffs
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  SOC_PETRA_CLEAR(&dbuffs_bdries, SOC_PETRA_INIT_DBUFFS_BDRY, 1);

  res = soc_petra_init_dram_buff_boudaries_calc(
          nof_dram_buffs,
          hw_adjust->dram.dbuff_size,
          &dbuffs_bdries
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /************************************************************************/
  /* Initialize basic configuration (must be before per-block Out-Of-Reset*/
  /************************************************************************/
  SOC_PETRA_INIT_PRINT_ADVANCE("Initialize internal blocks, in-reset", 1);
  res = soc_petra_mgmt_init_before_blocks_oor(
          unit,
          hw_adjust->dram.dbuff_size,
          &dbuffs_bdries,
          &(hw_adjust->qdr),
          &(hw_adjust->fabric),
          &(hw_adjust->dram),
          &(hw_adjust->stat_if),
          &(hw_adjust->nif)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  /************************************************************************/
  /* Out-of-reset Soc_petra internal blocks                                   */
  /************************************************************************/
  SOC_PETRA_INIT_PRINT_ADVANCE("Take internal blocks out-of-reset", 1);
  res = soc_petra_mgmt_blocks_init_unsafe(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /************************************************************************/
  /* Stop all traffic                                                     */
  /************************************************************************/
  SOC_PETRA_INIT_PRINT_ADVANCE("Stop traffic", 1);
  res = soc_petra_mgmt_enable_traffic_set_unsafe(
          unit,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  /************************************************************************/
  /* Stop Control Cells                                                */
  /*                                                                      */
  /* Disable the device from sending control cells                        */
  /* prior to FAP-ID setting).                                            */
  /************************************************************************/
  SOC_PETRA_INIT_PRINT_ADVANCE("Stop control cells", 1)
  res = soc_petra_mgmt_all_ctrl_cells_enable_set_unsafe(
          unit,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  /************************************************************************/
  /* Initialize basic configuration (based on pre-OOR)                    */
  /************************************************************************/
  SOC_PETRA_INIT_PRINT_ADVANCE("Finalize internal blocks initialization", 1);
  res = soc_petra_mgmt_init_after_blocks_oor(
          unit,
          hw_adjust->dram.dbuff_size,
          &dbuffs_bdries
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);

  /************************************************************************/
  /* Initialize all tables                                                */
  /*                                                                      */
  /* Most tables are zeroed. Some - initialized to non-zero default       */
  /************************************************************************/
  if (basic_conf->mem_shadow_enable)
  {
    SOC_PETRA_INIT_PRINT_ADVANCE("Enable memory shadowing", 1);

    res = soc_petra_shd_indirect_init();
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);  

    res = soc_petra_shd_indirect_malloc(
      unit
      );
    SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
  }
  SOC_PETRA_INIT_PRINT_ADVANCE("Initialize tables defaults", 1);
  res = soc_petra_mgmt_tbls_init(
          unit,
          basic_conf->mem_shadow_enable,
          silent
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  /************************************************************************/
  /* Set registers not covered in any functional module, */
  /* with default values different from hardware defaults                 */
  /************************************************************************/
  SOC_PETRA_INIT_PRINT_ADVANCE("Initialize registers defaults", 1);
  res = soc_petra_mgmt_hw_set_defaults(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  /************************************************************************/
  /* Set board-related configuration (hardware adjustments)               */
  /************************************************************************/
  if (hw_adjust != NULL)
  {
    SOC_PETRA_INIT_PRINT_ADVANCE("Configure HW interfaces", 1);
    res = soc_petra_mgmt_hw_interfaces_set_unsafe(
            unit,
            hw_adjust,
            silent
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  }
  else
  {
    SOC_PETRA_INIT_PRINT_ADVANCE("SKIPPING soc_petra_mgmt_hw_interfaces_set", 1);
  }

  /************************************************************************/
  /* Set basic configuration                                              */
  /************************************************************************/
  if (basic_conf != NULL)
  {
    if (basic_conf->credit_worth_enable)
    {
      SOC_PETRA_INIT_PRINT_ADVANCE("Set credit worth", 1);
      res = soc_petra_mgmt_credit_worth_set_unsafe(
              unit,
              basic_conf->credit_worth
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);
    }

    if (basic_conf->stag_enable)
    {
      SOC_PETRA_INIT_PRINT_ADVANCE("Set statistics tag", 1);
      res = soc_petra_ihp_stag_hdr_data_set_unsafe(
              unit,
              &(basic_conf->stag.fld_conf)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

      res = soc_petra_itm_stag_set_unsafe(
              unit,
              &(basic_conf->stag.encoding)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
    }
  }
  else
  {
    SOC_PETRA_INIT_PRINT_ADVANCE("SKIPPING basic configurations", 1);
  }

  /************************************************************************/
  /* Per functional module, perform initializations                       */
  /* covered by module's functionality.                                   */
  /************************************************************************/
  SOC_PETRA_INIT_PRINT_ADVANCE("Set default configuration", 1);
  res = soc_petra_mgmt_functional_init(
          unit,
          hw_adjust,
          silent
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

  /************************************************************************/
  /* Set FAP port configuration.                                       */
  /************************************************************************/
  if (fap_ports != NULL)
  {
    if (fap_ports->hdr_type_nof_entries != 0)
    {
      SOC_PETRA_INIT_PRINT_ADVANCE("Set port header type", 1);
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
      }
    }

    if (fap_ports->if_map_nof_entries != 0)
    {
      SOC_PETRA_INIT_PRINT_ADVANCE("Set port to interface mapping", 1);
      for (idx = 0; idx < fap_ports->if_map_nof_entries; idx++)
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

    if (fap_ports->packet_hdr_info_nof_entries != 0)
    {
      SOC_PETRA_INIT_PRINT_ADVANCE("Set port header parsing configuration", 1);
      for (idx = 0; idx < fap_ports->packet_hdr_info_nof_entries; idx++)
      {
        res = soc_petra_ihp_port_set(
                unit,
                fap_ports->packet_hdr_info[idx].port_ndx,
                &(fap_ports->packet_hdr_info[idx].conf)
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);
      }
    }
  }
  else
  {
    SOC_PETRA_INIT_PRINT_ADVANCE("SKIPPING FAP port configuration", 1);
  }

  /************************************************************************/
  /* Set programmable port default configuration                          */
  /************************************************************************/
  /*SOC_PETRA_INIT_PRINT_ADVANCE("soc_petra_ihp_prog_n00_load", 1);*/
  res = soc_petra_ihp_prog_n00_load_unsafe(unit);
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_init_sequence_phase1_unsafe()", 0, 0);
}

uint32
  soc_petra_mgmt_init_sequence_phase1_verify(
    SOC_SAND_IN     int              unit,
    SOC_SAND_INOUT  SOC_PETRA_HW_ADJUSTMENTS   *hw_adjust,
    SOC_SAND_IN     SOC_PETRA_INIT_BASIC_CONF  *basic_conf,
    SOC_SAND_IN     SOC_PETRA_INIT_PORTS       *fap_ports
  )
{
  uint32
    res = 0;
  uint32
    idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_INIT_SEQUENCE_PHASE1_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(hw_adjust);
  SOC_SAND_MAGIC_NUM_VERIFY(hw_adjust);

  if (!(hw_adjust->nif.fat_pipe_enable))
  {
    soc_petra_PETRA_HW_ADJ_FAT_PIPE_clear(&(hw_adjust->nif.fat_pipe));
    hw_adjust->nif.fat_pipe_enable = TRUE;
  }

  /*
   *  The below parameters must be supplied for init
   */
  if (
      (!hw_adjust->fabric.enable) ||
      (!hw_adjust->dram.enable) ||
      (!hw_adjust->qdr.enable) ||
      (basic_conf == NULL)
     )
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MGMT_BASIC_CONF_NULL_AT_INIT_ERR, 15, exit);
  }

  SOC_SAND_MAGIC_NUM_VERIFY(basic_conf);
  if (basic_conf->credit_worth_enable)
  {
    res = soc_petra_mgmt_credit_worth_verify(
            unit,
            basic_conf->credit_worth
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

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

  if (basic_conf->stag_enable)
  {
    res = soc_petra_ihp_stag_hdr_data_verify(
            unit,
            &(basic_conf->stag.fld_conf)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

    res = soc_petra_itm_stag_verify(
            unit,
            &(basic_conf->stag.encoding)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 37, exit);
  }

  if (fap_ports != NULL)
  {
    SOC_SAND_MAGIC_NUM_VERIFY(fap_ports);
    for (idx = 0; idx < fap_ports->hdr_type_nof_entries; idx++)
    {
      res = soc_petra_port_header_type_verify(
              unit,
              fap_ports->hdr_type[idx].port_ndx,
              SOC_PETRA_PORT_DIRECTION_BOTH,
              fap_ports->hdr_type[idx].header_type_in
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

	  res = soc_petra_port_header_type_verify(
              unit,
              fap_ports->hdr_type[idx].port_ndx,
              SOC_PETRA_PORT_DIRECTION_BOTH,
              fap_ports->hdr_type[idx].header_type_out
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 45, exit);
    }

    for (idx = 0; idx < fap_ports->if_map_nof_entries; idx++)
    {
      res = soc_petra_port_to_interface_map_verify(
              unit,
              fap_ports->if_map[idx].port_ndx,
              SOC_PETRA_PORT_DIRECTION_BOTH,
              &(fap_ports->if_map[idx].conf)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    }
  }

  if (hw_adjust->core_freq.enable)
  {
    SOC_SAND_ERR_IF_OUT_OF_RANGE(hw_adjust->core_freq.frequency, SOC_PETRA_MGMT_CORE_FREQ_MIN, SOC_PETRA_MGMT_CORE_FREQ_MAX, SOC_PETRA_CORE_FREQ_OUT_OF_RANGE_ERR, 60, exit);
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_init_sequence_phase1_verify()", 0, 0);
}

/*********************************************************************
*     Out-of-reset sequence. Enable/Disable the device from
*     receiving and transmitting control cells.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_mgmt_init_sequence_phase2_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INIT_OOR            *oor_info,
    SOC_SAND_IN  uint8                 silent
  )
{
  uint32
    res;
  uint32
    idx = 0,
    stage_id = 0;
  uint8
    oor_set = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_INIT_SEQUENCE_PHASE2_UNSAFE);

  if (!silent)
  {
    soc_sand_os_printf(
      "\n\r"
      "   Phase 2 initialization: device %u"
      "\n\r",
      unit
    );
  }

  for (idx = 0; idx < SOC_PETRA_NOF_MAC_LANES; idx++)
  {
    if (oor_info->mal[idx] == TRUE)
    {
      oor_set = TRUE;
    }
  }

  if (oor_set == TRUE)
  {
    SOC_PETRA_INIT_PRINT_ADVANCE("Take network interfaces out-of-reset", 2);
  }
  else
  {
    SOC_PETRA_INIT_PRINT_ADVANCE("SKIPPING soc_petra_nif_mal_enable_set", 2);
  }

  for (idx = 0; idx < SOC_PETRA_NOF_MAC_LANES; idx++)
  {
    if (oor_info->mal[idx])
    {
       res = soc_petra_nif_mal_enable_set_unsafe(
              unit,
              idx,
              0x1
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }
  }

  SOC_PETRA_INIT_PRINT_ADVANCE("Validate NIF Topology", 2);
  res = soc_petra_nif_topology_validate_unsafe(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 21, exit);

  SOC_PETRA_INIT_PRINT_ADVANCE("Enable control cells", 2);
  res = soc_petra_mgmt_all_ctrl_cells_enable_set_unsafe(
    unit,
    TRUE
  );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  
  res = soc_petra_mgmt_init_finalize(
          unit
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  SOC_PETRA_INIT_PRINT_ADVANCE("Phase 2 initialization done", 2);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_init_sequence_phase2_unsafe()", 0, 0);
}

uint32
  soc_petra_mgmt_init_sequence_phase2_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INIT_OOR            *oor_info
  )
{
  uint32
    res;
  uint32
    idx;
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_MGMT_INIT_SEQUENCE_PHASE2_VERIFY);
  SOC_SAND_CHECK_NULL_INPUT(oor_info);

  for (idx = 0; idx < SOC_PETRA_NOF_MAC_LANES; idx++)
  {
    if (oor_info->mal[idx])
    {
       res = soc_petra_nif_mal_enable_verify(
              unit,
              idx,
              0x1
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_mgmt_init_sequence_phase2_verify()", 0, 0);
}
/*
 *  Init sequence -
 *  per-block initialization, hardware adjustments etc. }
 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

