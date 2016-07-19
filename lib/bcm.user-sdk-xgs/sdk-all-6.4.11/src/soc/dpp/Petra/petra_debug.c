/* $Id: petra_debug.c,v 1.11 Broadcom SDK $
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

#include <soc/dpp/Petra/petra_debug.h>
#include <soc/dpp/Petra/petra_api_debug.h>
#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_tbl_access.h>
#include <soc/dpp/Petra/petra_mgmt.h>
#include <soc/dpp/Petra/petra_chip_defines.h>
#include <soc/dpp/Petra/petra_scheduler_flow_converts.h>
#include <soc/dpp/Petra/petra_init.h>
#include <soc/dpp/Petra/petra_dram.h>
#include <soc/dpp/Petra/petra_sw_db.h>

#ifdef LINK_PB_LIBRARIES
  #include <soc/dpp/Petra/PB_TM/pb_init.h>
#endif

#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/SAND/Utils/sand_conv.h>
#include <soc/dpp/SAND/Management/sand_low_level.h>


/* } */

/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PETRA_DBG_FORCE_MODE_NONE_FLD_VAL         (0x0)
#define SOC_PETRA_DBG_FORCE_MODE_LOCAL_FLD_VAL        (0x1)
#define SOC_PETRA_DBG_FORCE_MODE_FABRIC_FLD_VAL       (0x2)
#define SOC_PETRA_DBG_AUTOCREDIT_RATE_MIN_TH          (15)
#define SOC_PETRA_DBG_AUTOCREDIT_RATE_MAX_TH          (soc_sand_power_of_2(19))
#define SOC_PETRA_DBG_AUTOCREDIT_RATE_MIN_VALUE       (1)
#define SOC_PETRA_DBG_AUTOCREDIT_RATE_MAX_VALUE       (15)
#define SOC_PETRA_DBG_AUTOCREDIT_RATE_OFFSET          (3)
#define SOC_PETRA_DBG_Q_FLUSH_MODE_DELETE_FLD_VAL     (0x1)
#define SOC_PETRA_DBG_Q_FLUSH_MODE_DEQUEUE_FLD_VAL    (0x0)
#define SOC_PETRA_DEBUG_RST_DOMAIN_MAX                (SOC_PETRA_DBG_NOF_RST_DOMAINS-1)
/* $Id: petra_debug.c,v 1.11 Broadcom SDK $
 *  Polling values for the queue flush
 */
#define SOC_PETRA_DBG_Q_FLUSH_BUSY_WAIT_ITERATIONS   100
#define SOC_PETRA_DBG_Q_FLUSH_TIMER_ITERATIONS       100
#define SOC_PETRA_DBG_Q_FLUSH_TIMER_DELAY_MSEC       1

/************************************************************************/
/* Flush Queue register: must be aligned with register database,        */
/* but defined here to minimize access time                             */
/************************************************************************/

/*
 *   Manual Queue Operation : 0x0380 :
 *   ips.manual_queue_operation_reg
 */

#define SOC_PETRA_DBG_ECI_ACCESS_PATTERN_1          0xAAAAAAAA
#define SOC_PETRA_DBG_ECI_ACCESS_PATTERN_2          0x55555555

/* } */

/*************
 *  MACROS   *
 *************/
/* { */
/*
 *  If defined, writes the number of ingress resets to a spare register (SOC_PETRA_DBG_INGR_RESET_CNT_REG_ADDR)
 *
 */
#define SOC_PETRA_DBG_COUNT_INGR_RESETS   1
#define SOC_PETRA_DBG_DPRC_FULL_RESET     1
#define SOC_PETRA_DBG_INGR_RESET_CNT_REG_ADDR 0x0450

/*
 *	Polling on Block initialization indication,
 *  number of Busy Wait iterations
 */
#define SOC_PETRA_DBG_IGRE_RST_POLL_NOF_BW_ITERS 500
/*
 *	Polling on Block initialization indication,
 *  number of Timed Delay iterations
 */
#define SOC_PETRA_DBG_IGRE_RST_POLL_NOF_TD_ITERS 3
/*
 *	Timed Delay, milliseconds
 */
#define SOC_PETRA_DBG_IGRE_RST_POLL_TD_MSEC 16

/*
 * For Soc_petra-A full reset, the size of CFC registers buffer
 * for the registers mirror:
 * Last register is 0x475b, first register is 0x4650, and another slot for the 'Interrupt Mask Register'.
 * Also, we put a signuture patter as the last entry.
 */
#define SOC_PA_DBG_CFC_REGS_START           0x4650
#define SOC_PA_DBG_CFC_REGS_END             0x475b
/* (SOC_SAND_RNG_COUNT(SOC_PA_DBG_CFC_REGS_START, SOC_PA_DBG_CFC_REGS_END)) + 1 + 1) */
#define SOC_PA_DBG_CFC_MIRR_REGS_SIZE_UINT32S 270
#define SOC_PA_DBG_CFC_MIRR_SIGNUTURE       0xABCD01FE

#define SOC_PETRA_DBG_IPT_NON_LOCAL_CONTEXTS 7

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
static uint32 Soc_petra_dbg_reset_counter = 0;
/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

/* 
 *  Relevant when working in Mesh mode, Soc_petra-A only;
 *  retrieves the FAPs (FAP-Id 3 least significant bits, indicating Mesh Context) for which
 *  there are no connecting links.
 *  This API can be used to implement a WA for the following 
 *  Soc_petra-A Errata: in Mesh mode - extraction of Soc_petra can cause remaining Soc_petra devices to get stuck.
 *  The WA is to call this API periodically, or after extracting a Soc_petra device.
 *  The bitmap can be compared to the expected. If the bitmap for a specific FAP device is '1', 
 *  i.e. it has no links, but the device is known to be in System, ingress reset should be applied to the device.
 */
uint32
  soc_petra_dbg_faps_with_no_mesh_links_get_unsafe(
    SOC_SAND_IN  int unit,
    SOC_SAND_OUT uint32  *fap_bitmap
  )
{
  uint32
    res = SOC_SAND_OK,
    reg_val,
    nof_links[SOC_PETRA_DBG_IPT_NON_LOCAL_CONTEXTS],
    fap_id_2_0;
  uint32
    idx;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DBG_FAPS_WITH_NO_MESH_LINKS_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(fap_bitmap);

  regs = soc_petra_regs();

  SOC_PETRA_FLD_GET(regs->eci.general_controls_reg.sel_stat_data_out, reg_val, 10, exit);
  if (reg_val)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DBG_STAT_IF_ENABLED_ERR, 20, exit);
  }

  SOC_PA_REG_SET(regs->eci.mesh_ctxt_reg, 0x80040005, 30, exit);
  SOC_PETRA_REG_GET(regs->eci.lsb_mirror_data_bus_reg, reg_val, 40, exit);
  nof_links[0] = SOC_SAND_GET_BITS_RANGE(reg_val, 5, 0);
  nof_links[1] = SOC_SAND_GET_BITS_RANGE(reg_val, 11, 6);
  nof_links[2] = SOC_SAND_GET_BITS_RANGE(reg_val, 17, 12);
  SOC_PA_REG_SET(regs->eci.mesh_ctxt_reg, 0x00040005, 50, exit);
  SOC_PETRA_REG_GET(regs->eci.msb_mirror_data_bus_reg, reg_val, 60, exit);
  nof_links[3] = SOC_SAND_GET_BITS_RANGE(reg_val, 5, 0);
  nof_links[4] = SOC_SAND_GET_BITS_RANGE(reg_val, 11, 6);
  nof_links[5] = SOC_SAND_GET_BITS_RANGE(reg_val, 17, 12);
  SOC_PA_REG_SET(regs->eci.mesh_ctxt_reg, 0x80040006, 70, exit);
  SOC_PETRA_REG_GET(regs->eci.lsb_mirror_data_bus_reg, reg_val, 80, exit);
  nof_links[6] = SOC_SAND_GET_BITS_RANGE(reg_val, 5, 0);

  /*
   *  Get FAP ID
   */
  SOC_PETRA_REG_GET(regs->eci.identification_reg, reg_val, 90, exit);
  fap_id_2_0 = SOC_SAND_GET_BITS_RANGE(reg_val, 2,0);

  *fap_bitmap = 0;
  for (idx = 0; idx < SOC_PETRA_DBG_IPT_NON_LOCAL_CONTEXTS; ++idx)
  {
    if (nof_links[idx] == 0)
    {
      *fap_bitmap |= (fap_id_2_0 == idx + 1) ? SOC_SAND_BIT(0) : SOC_SAND_BIT(idx + 1);
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_dbg_faps_with_no_mesh_links_get_unsafe()", 0, 0);
}

/*********************************************************************
*     Enable/disable if the traffic route should be forced. If
*     enabled, the traffic route either goes through the
*     fabric or remains local. Otherwise, the traffic route is
*     not forced.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_dbg_route_force_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_DBG_FORCE_MODE      force_mode
  )
{
  uint32
    reg_val = 0,
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DBG_ROUTE_FORCE_SET_UNSAFE);

  regs = soc_petra_regs();

  switch(force_mode)
  {
  case SOC_PETRA_DBG_FORCE_MODE_NONE:
    reg_val = SOC_PETRA_DBG_FORCE_MODE_NONE_FLD_VAL;
    break;

  case SOC_PETRA_DBG_FORCE_MODE_LOCAL:
    reg_val = SOC_PETRA_DBG_FORCE_MODE_LOCAL_FLD_VAL;
    break;

  case SOC_PETRA_DBG_FORCE_MODE_FABRIC:
    reg_val = SOC_PETRA_DBG_FORCE_MODE_FABRIC_FLD_VAL;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DBG_FORCE_MODE_OUT_OF_RANGE_ERR, 10, exit);
  }

  SOC_PETRA_REG_SET(regs->ipt.force_local_or_fabric_reg, reg_val, 20, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_dbg_route_force_set_unsafe()",0,0);
}

/*********************************************************************
*     Enable/disable if the traffic route should be forced. If
*     enabled, the traffic route either goes through the
*     fabric or remains local. Otherwise, the traffic route is
*     not forced.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_dbg_route_force_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_DBG_FORCE_MODE      force_mode
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DBG_ROUTE_FORCE_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    force_mode, SOC_PETRA_DBG_NOF_FORCE_MODES-1,
    SOC_PETRA_DBG_FORCE_MODE_OUT_OF_RANGE_ERR, 10, exit
  );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_dbg_route_force_verify()",0,0);
}

/*********************************************************************
*     Enable/disable if the traffic route should be forced. If
*     enabled, the traffic route either goes through the
*     fabric or remains local. Otherwise, the traffic route is
*     not forced.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_dbg_route_force_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_DBG_FORCE_MODE      *force_mode
  )
{
  uint32
    reg_val = 0,
    res;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_DBG_FORCE_MODE
    tmp_force_mode;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DBG_ROUTE_FORCE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(force_mode);

  regs = soc_petra_regs();

  SOC_PETRA_REG_GET(regs->ipt.force_local_or_fabric_reg.force_local, reg_val, 20, exit);

  switch(reg_val)
  {
  case SOC_PETRA_DBG_FORCE_MODE_NONE_FLD_VAL:
    tmp_force_mode = SOC_PETRA_DBG_FORCE_MODE_NONE;
    break;

  case SOC_PETRA_DBG_FORCE_MODE_LOCAL_FLD_VAL:
    tmp_force_mode = SOC_PETRA_DBG_FORCE_MODE_LOCAL;
    break;

  case SOC_PETRA_DBG_FORCE_MODE_FABRIC_FLD_VAL:
    tmp_force_mode = SOC_PETRA_DBG_FORCE_MODE_FABRIC;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DBG_FORCE_MODE_FLD_OUT_OF_RANGE_ERR, 40, exit);
  }
  *force_mode = tmp_force_mode;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_dbg_route_force_get_unsafe()",0,0);
}

/*********************************************************************
* NAME:
*     soc_petra_dbg_rate2autocredit_rate
* FUNCTION:
*   Calculate the AutoCredit rate (four bits) based on the rate in Mbps.
* INPUT:
*   SOC_SAND_IN  uint32   rate -
*     The rate in Mbps.
*   SOC_SAND_OUT uint32  *autocr_rate_bits
*     The four bits computed from the rate, where
*     rate[Mbps]=CreditSize[Bytes/Credit]*8[bits/Bytes]*1000 /
*     (2^(autocr_rate_bits+3)[clocks/credit] * clock_rate[ns/clock])
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   None.
*********************************************************************/
STATIC uint32
  soc_petra_dbg_rate2autocredit_rate(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   rate,
    SOC_SAND_OUT uint32   *autocr_rate_bits
  )
{
  uint32
    res,
    credit_worth,
    device_ticks_per_sec,
    autocr_rate = 0; /* The credit rate in [clocks/credit] */

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DBG_RATE2AUTOCREDIT_RATE);

  res = soc_petra_mgmt_credit_worth_get_unsafe(
        unit,
        &credit_worth
     );
  SOC_SAND_CHECK_FUNC_RESULT(res, 2, exit);

  if (rate == 0) /* Disabling the auto-generation */
  {
    *autocr_rate_bits = 0;
  }
  else
  {
    device_ticks_per_sec = soc_petra_chip_ticks_per_sec_get(unit);

    res = soc_sand_kbits_per_sec_to_clocks(
            rate * 1000,
            credit_worth,
            device_ticks_per_sec,
            &autocr_rate
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (autocr_rate <= SOC_PETRA_DBG_AUTOCREDIT_RATE_MIN_TH)
    {
      *autocr_rate_bits = SOC_PETRA_DBG_AUTOCREDIT_RATE_MIN_VALUE;
    }
    else if (autocr_rate >= SOC_PETRA_DBG_AUTOCREDIT_RATE_MAX_TH)
    {
      *autocr_rate_bits = SOC_PETRA_DBG_AUTOCREDIT_RATE_MAX_VALUE;
    }
    else /* autocr_rate between 15 and 2^19 */
    {
      *autocr_rate_bits = soc_sand_log2_round_down(autocr_rate) - SOC_PETRA_DBG_AUTOCREDIT_RATE_OFFSET;
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_dbg_rate2autocredit_rate()",0,0);
}

/*********************************************************************
* NAME:
*     soc_petra_dbg_autocredit_rate2rate
* FUNCTION:
*   Calculate the AutoCredit rate (four bits) based on the rate in Mbps.
* INPUT:
*   SOC_SAND_IN  uint32  autocr_rate_bits
*     The four bits computed from the rate.
*   SOC_SAND_OUT uint32  *rate -
*     The rate in Mbps, where
*     rate[Mbps]=CreditSize[Bytes/Credit]*8[bits/Bytes]*1000 /
*     (2^(autocr_rate_bits+3)[clocks/credit] * clock_rate[ns/clock])
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   None.
*********************************************************************/
STATIC uint32
  soc_petra_dbg_autocredit_rate2rate(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   autocr_rate_bits,
    SOC_SAND_OUT uint32   *rate
  )
{
  uint32
    res,
    credit_worth,
    device_ticks_per_sec,
    autocr_rate = 0; /* The credit rate in [clocks/credit] */

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DBG_AUTOCREDIT_RATE2RATE);

  res = soc_petra_mgmt_credit_worth_get_unsafe(
          unit,
          &credit_worth
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 2, exit);

  if (autocr_rate_bits == 0) /* Disabling the auto-generation */
  {
    *rate = 0;
  }
  else
  {
    device_ticks_per_sec = soc_petra_chip_ticks_per_sec_get(unit);

    autocr_rate = soc_sand_power_of_2(autocr_rate_bits + SOC_PETRA_DBG_AUTOCREDIT_RATE_OFFSET);

    res = soc_sand_clocks_to_kbits_per_sec(
            autocr_rate,
            credit_worth,
            device_ticks_per_sec,
            rate      /* in Kbits/sec */
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    *rate /= 1000;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_dbg_autocredit_rate2rate()",0,0);
}



/*********************************************************************
*     Configure the Scheduler AutoCredit parameters.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_dbg_autocredit_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_DBG_AUTOCREDIT_INFO *info,
    SOC_SAND_OUT uint32                  *exact_rate
  )
{
  uint32
    fld_val,
    fld_val2,
    autocr_rate = 0,
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DBG_AUTOCREDIT_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);
  SOC_SAND_CHECK_NULL_INPUT(exact_rate);

  regs = soc_petra_regs();

  if (info->first_queue > info->last_queue)
  {
    fld_val = 0;
    fld_val2 = SOC_PETRA_MAX_QUEUE_ID;
  }
  else
  {
    fld_val = info->first_queue;
    fld_val2 = info->last_queue;
  }
  SOC_PETRA_FLD_SET(regs->ips.auto_credit_mechanism_queue_boundaries_reg.auto_cr_frst_que, fld_val, 30, exit);
  SOC_PETRA_FLD_SET(regs->ips.auto_credit_mechanism_queue_boundaries_reg.auto_cr_last_que, fld_val2, 40, exit);
  /* Computation of the right AutoCredit rate */
  if (info->rate == 0) /* Disabling Autocredit*/
  {
    autocr_rate = 0;
    *exact_rate = 0;
  }
  else
  {
    res = soc_petra_dbg_rate2autocredit_rate(
            unit,
            info->rate,
            &autocr_rate
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
    res = soc_petra_dbg_autocredit_rate2rate(
            unit,
            autocr_rate,
            exact_rate
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 55, exit);
  }
  fld_val = autocr_rate;
  SOC_PETRA_FLD_SET(regs->ips.auto_credit_mechanism_rate_configuration_reg.auto_cr_rate, fld_val, 60, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_dbg_autocredit_set_unsafe()",0,0);
}

/*********************************************************************
*     Configure the Scheduler AutoCredit parameters.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_dbg_autocredit_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_DBG_AUTOCREDIT_INFO *info
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DBG_AUTOCREDIT_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_MAGIC_NUM_VERIFY(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->first_queue, SOC_PETRA_MAX_QUEUE_ID,
    SOC_PETRA_QUEUE_NUM_OUT_OF_RANGE_ERR, 10, exit
   );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    info->last_queue, SOC_PETRA_MAX_QUEUE_ID,
    SOC_PETRA_QUEUE_NUM_OUT_OF_RANGE_ERR, 20, exit
   );

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_dbg_autocredit_verify()",0,0);
}

/*********************************************************************
*     Configure the Scheduler AutoCredit parameters.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_dbg_autocredit_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_DBG_AUTOCREDIT_INFO *info
  )
{
  uint32
    fld_val,
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DBG_AUTOCREDIT_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  regs = soc_petra_regs();

  SOC_PETRA_FLD_GET(regs->ips.auto_credit_mechanism_queue_boundaries_reg.auto_cr_frst_que, info->first_queue, 10, exit);
  SOC_PETRA_FLD_GET(regs->ips.auto_credit_mechanism_queue_boundaries_reg.auto_cr_last_que, info->last_queue, 20, exit);
  SOC_PETRA_FLD_GET(regs->ips.auto_credit_mechanism_rate_configuration_reg.auto_cr_rate, fld_val, 30, exit);

  if (fld_val == 0) /* Disabling Autocredit*/
  {
    info->rate = 0;
  }
  else
  {
    res = soc_petra_dbg_autocredit_rate2rate(
            unit,
            fld_val,
            &(info->rate)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_dbg_autocredit_get_unsafe()",0,0);
}

/*********************************************************************
*     Enable/disable the egress shaping.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_dbg_egress_shaping_enable_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 enable
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DBG_EGRESS_SHAPING_ENABLE_SET_UNSAFE);

  regs = soc_petra_regs();

  if (enable)
  {
    SOC_PETRA_FLD_SET(regs->egq.egress_shaper_enable_settings_reg.sch_enable, 0x1, 20, exit);
  }
  else /* enable == 0*/
  {
    SOC_PETRA_FLD_SET(regs->egq.egress_shaper_enable_settings_reg.sch_enable, 0x0, 30, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_dbg_egress_shaping_enable_set_unsafe()",0,0);
}

/*********************************************************************
*     Enable/disable the egress shaping.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_dbg_egress_shaping_enable_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 enable
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DBG_EGRESS_SHAPING_ENABLE_VERIFY);

  SOC_PETRA_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_dbg_egress_shaping_enable_verify()",0,0);
}

/*********************************************************************
*     Enable/disable the egress shaping.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_dbg_egress_shaping_enable_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint8                 *enable
  )
{
  uint32
    fld_val,
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DBG_EGRESS_SHAPING_ENABLE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(enable);

  regs = soc_petra_regs();

  SOC_PETRA_FLD_GET(regs->egq.egress_shaper_enable_settings_reg.sch_enable, fld_val, 10, exit);
  *enable = SOC_SAND_NUM2BOOL(fld_val);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_dbg_egress_shaping_enable_get_unsafe()",0,0);
}

/*********************************************************************
*     Enable/disable device-level flow control.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_dbg_flow_control_enable_set_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint8  enable
  )
{
  uint32
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DBG_FLOW_CONTROL_ENABLE_SET_UNSAFE);

  regs = soc_petra_regs();

  if (enable)
  {
    SOC_PETRA_FLD_SET(regs->cfc.cfc_enablers_reg.cfc_en, 0x1, 20, exit);
  }
  else /* enable == 0*/
  {
    SOC_PETRA_FLD_SET(regs->cfc.cfc_enablers_reg.cfc_en, 0x0, 30, exit);
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_dbg_flow_control_enable_set_unsafe()",0,0);
}

/*********************************************************************
*     Enable/disable device-level flow control.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_dbg_flow_control_enable_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 enable
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DBG_FLOW_CONTROL_ENABLE_VERIFY);

  SOC_PETRA_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_dbg_flow_control_enable_verify()",0,0);
}

/*********************************************************************
*     Enable/disable device-level flow control.
*     Details: in the H file. (search for prototype)
*********************************************************************/

uint32
  soc_petra_dbg_flow_control_enable_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint8                 *enable
  )
{
  uint32
    fld_val,
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DBG_FLOW_CONTROL_ENABLE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(enable);

  regs = soc_petra_regs();

  SOC_PETRA_FLD_GET(regs->cfc.cfc_enablers_reg.cfc_en, fld_val, 10, exit);
  *enable = SOC_SAND_NUM2BOOL(fld_val);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_dbg_flow_control_enable_get_unsafe()",0,0);
}

/*********************************************************************
*     Flush one queue at the ingress.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_dbg_queue_flush_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 queue_ndx,
    SOC_SAND_IN  SOC_PETRA_DBG_FLUSH_MODE      mode
  )
{
  uint32
    res = SOC_SAND_OK,
    fld_val,
    fld_mode,
    reg_val;
  SOC_PETRA_REGS
    *regs;
  uint8
    poll_success;
  SOC_PETRA_POLL_INFO
    poll_info;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DBG_QUEUE_FLUSH_UNSAFE);

  regs = soc_petra_regs();

  soc_petra_PETRA_POLL_INFO_clear(&poll_info);
  poll_info.expected_value = 0x0;
  poll_info.busy_wait_nof_iters = SOC_PETRA_DBG_Q_FLUSH_BUSY_WAIT_ITERATIONS;
  poll_info.timer_nof_iters     = SOC_PETRA_DBG_Q_FLUSH_TIMER_ITERATIONS;
  poll_info.timer_delay_msec    = SOC_PETRA_DBG_Q_FLUSH_TIMER_DELAY_MSEC;

  SOC_SAND_ERR_IF_ABOVE_MAX(
    queue_ndx, SOC_PETRA_MAX_QUEUE_ID,
    SOC_PETRA_QUEUE_NUM_OUT_OF_RANGE_ERR, 5, exit
  );

  /*
   *  Insert the queue number, the mode and then activate the trigger
   */
  SOC_PETRA_REG_GET(regs->ips.manual_queue_operation_reg, reg_val, 7, exit);

  fld_val = queue_ndx;
  SOC_PETRA_FLD_TO_REG(regs->ips.manual_queue_operation_reg.man_queue_id, fld_val, reg_val, 10, exit);

  switch(mode)
  {
  case SOC_PETRA_DBG_FLUSH_MODE_DEQUEUE:
    fld_mode = SOC_PETRA_DBG_Q_FLUSH_MODE_DEQUEUE_FLD_VAL;
    break;

  case SOC_PETRA_DBG_FLUSH_MODE_DELETE:
    fld_mode = SOC_PETRA_DBG_Q_FLUSH_MODE_DELETE_FLD_VAL;
    break;

  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DBG_Q_FLUSH_MODE_OUT_OF_RANGE_ERR, 20, exit);
  }
  SOC_PETRA_FLD_TO_REG(regs->ips.manual_queue_operation_reg.del_dqcq, fld_mode, reg_val, 30, exit);

  SOC_PETRA_REG_SET(regs->ips.manual_queue_operation_reg, reg_val, 35, exit);

  /*
   *  Second write to the register necessary
   */
  SOC_PETRA_REG_GET(regs->ips.manual_queue_operation_reg, reg_val, 40, exit);

  fld_val = queue_ndx;
  SOC_PETRA_FLD_TO_REG(regs->ips.manual_queue_operation_reg.man_queue_id, fld_val, reg_val, 45, exit);

  SOC_PETRA_FLD_TO_REG(regs->ips.manual_queue_operation_reg.del_dqcq, fld_mode, reg_val, 50, exit);

  fld_val = 0x1;
  SOC_PETRA_FLD_TO_REG(regs->ips.manual_queue_operation_reg.flush_trigger, fld_val, reg_val, 55, exit);

  SOC_PETRA_REG_SET(regs->ips.manual_queue_operation_reg, reg_val, 60, exit);

  /*
   *  Polling of the status bit
   */
  res = soc_petra_status_fld_poll_unsafe(
          unit,
          SOC_PETRA_REG_DB_ACC_REF(regs->ips.manual_queue_operation_reg.flush_trigger),
          SOC_PETRA_DEFAULT_INSTANCE,
          &poll_info,
          &poll_success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 65, exit);


  if (poll_success == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DBG_Q_FLUSH_ALL_TIMEOUT_ERR, 70, exit);
  }


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_dbg_queue_flush_unsafe()",0,0);
}

/*********************************************************************
*     Flush all the queues at the ingress.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_dbg_queue_flush_all_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_DBG_FLUSH_MODE      mode
  )
{
  uint32
    q_ndx,
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DBG_QUEUE_FLUSH_ALL_UNSAFE);

  for (q_ndx = 0; q_ndx < SOC_PETRA_NOF_QUEUES; ++q_ndx)
  {
    res = soc_petra_dbg_queue_flush_unsafe(
            unit,
            q_ndx,
            mode
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_dbg_queue_flush_all_unsafe()",0,0);
}

/*********************************************************************
*     Resets the ingress pass. The following blocks are
*     soft-reset (running soft-init): IPS, IQM, IPT, MMU,
*     DPRC, IRE, IDR, IRR. As part of the reset sequence,
*     traffic is stopped, and re-started (according to the
*     original condition).
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_dbg_ingr_reset_unsafe(
    SOC_SAND_IN  int                 unit
  )
{
  uint32
    res = SOC_SAND_OK;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DBG_INGR_RESET_UNSAFE);

  res = soc_petra_dbg_dev_reset_unsafe(
          unit,
          SOC_PETRA_DBG_RST_DOMAIN_INGR
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_dbg_ingr_reset_unsafe()",0,0);
}

STATIC uint32
  soc_pa_cfc_regs_mirror_retrieve(
    SOC_SAND_IN  int   unit,
    SOC_SAND_OUT uint32 cfc_regs[SOC_PA_DBG_CFC_MIRR_REGS_SIZE_UINT32S]
  )
{
  uint32
    reg_idx = 0;
  uint32
    res;
  SOC_PETRA_REG_FIELD
    reg_desc;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_CFC_REGS_MIRROR_RETRIEVE);

  regs = soc_petra_regs();

  cfc_regs[SOC_PA_DBG_CFC_MIRR_REGS_SIZE_UINT32S-1] = SOC_PA_DBG_CFC_MIRR_SIGNUTURE;

  SOC_PA_REG_GET(regs->cfc.interrupt_mask_reg, cfc_regs[0], 10, exit);
  for(reg_idx = 1; reg_idx < (SOC_PA_DBG_CFC_MIRR_REGS_SIZE_UINT32S-2); reg_idx++)
  {
    reg_desc.addr.base = (SOC_PA_DBG_CFC_REGS_START + (reg_idx-1))*SOC_SAND_REG_SIZE_BYTES;
    reg_desc.addr.step = 0x0;
    SOC_PA_IMPLICIT_REG_GET(reg_desc, cfc_regs[reg_idx], 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pa_cfc_regs_mirror_retrieve()", reg_idx, 0);
}

STATIC uint32
  soc_pa_cfc_regs_mirror_load(
    SOC_SAND_IN  int   unit,
    SOC_SAND_OUT uint32 cfc_regs[SOC_PA_DBG_CFC_MIRR_REGS_SIZE_UINT32S]
  )
{
  uint32
    reg_idx = 0;
  uint32
    res;
  SOC_PETRA_REG_FIELD
    reg_desc;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_CFC_REGS_MIRROR_LOAD);

  regs = soc_petra_regs();

  if(cfc_regs[SOC_PA_DBG_CFC_MIRR_REGS_SIZE_UINT32S-1] != SOC_PA_DBG_CFC_MIRR_SIGNUTURE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DBG_CFC_DB_CORRUPT_ERR, 10, exit);
  }

  SOC_PA_REG_SET(regs->cfc.interrupt_mask_reg, cfc_regs[0], 12, exit);
  for(reg_idx = 1; reg_idx < (SOC_PA_DBG_CFC_MIRR_REGS_SIZE_UINT32S-2); reg_idx++)
  {
    reg_desc.addr.base = (SOC_PA_DBG_CFC_REGS_START + (reg_idx-1))*SOC_SAND_REG_SIZE_BYTES;
    reg_desc.addr.step = 0x0;
    SOC_PA_IMPLICIT_REG_SET(reg_desc, cfc_regs[reg_idx], 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pa_cfc_regs_mirror_load()", reg_idx, 0);
}

STATIC uint32
  soc_petra_ctrl_cells_en_fast(
    SOC_SAND_IN  int   unit,
    SOC_SAND_IN  uint8   enable
  )
{
  uint32
    fld_val,
    disable_val = SOC_SAND_BOOL2NUM_INVERSE(enable),
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_CTRL_CELLS_EN_FAST);

  regs = soc_petra_regs();

  SOC_PETRA_FLD_SET(regs->sch.dvs_config1_reg.force_pause, disable_val, 10, exit);
  
  SOC_PETRA_FLD_SET(regs->fdt.fdt_enabler_reg.discard_dll_pkts, disable_val, 30, exit);

  fld_val = (enable == TRUE)?0x0 : 0x7;
  SOC_PETRA_FLD_SET(regs->fct.fct_enabler_reg.stat_credit_and_rch_ctrl, fld_val, 40, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ctrl_cells_en_fast()", 0, 0);
}

uint32
  soc_petra_dbg_qdr_reset(
    SOC_SAND_IN   int              unit,
    SOC_SAND_IN   uint8              is_in_not_out
  )
{
  uint32
    fld_val,
    res;
  uint32
    iter_idx;
  SOC_PETRA_POLL_INFO
    poll_info;
  uint8
    poll_success;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  regs = soc_petra_regs();

  soc_petra_PETRA_POLL_INFO_clear(&poll_info);

  if (is_in_not_out)
  {
      SOC_PETRA_FLD_SET(regs->qdr.qdr_controller_reset_reg.qdrc_rst_n, 0x0, 8, exit);
      SOC_PETRA_FLD_SET(regs->qdr.qdr_phy_reset_reg.qdio_reset, 0x0, 10, exit);
      SOC_PETRA_FLD_SET(regs->eci.soc_petra_soft_reset_reg.qdio_reset, 0x1, 12, exit);
      SOC_PETRA_FLD_SET(regs->eci.soc_petra_soft_reset_reg.qdio_reset, 0x0, 14, exit);

      SOC_PETRA_FLD_SET(regs->qdr.dll_control_reg.dll_div2_en, 0x1, 36, exit);
  }
  else
  {
#ifdef LINK_PB_LIBRARIES
    if (SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE)
    {
      /*
       *  Initialize the QDR DLL memory
       */
      res = soc_pb_mgmt_init_qdr_dll_mem_unsafe(
              unit
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    }
#endif
    /*
     *  QDR OOR
     */
    SOC_PETRA_FLD_SET(regs->qdr.qdr_controller_reset_reg.qdrc_rst_n, 0x1, 37, exit);

    /*
     * Just delay
     */
    for(iter_idx = 0; iter_idx < SOC_PETRA_DBG_IGRE_RST_POLL_NOF_BW_ITERS; iter_idx++)
    {
      SOC_PETRA_FLD_GET(regs->qdr.qdr_controller_reset_reg.qdrc_rst_n, fld_val, 38, exit);
    }


    SOC_PETRA_FLD_SET(regs->qdr.qdr_phy_reset_reg.qdio_reset, 0x1, 39, exit);
  
    /*
     * Just delay
     */
    for(iter_idx = 0; iter_idx < SOC_PETRA_DBG_IGRE_RST_POLL_NOF_BW_ITERS; iter_idx++)
    {
      SOC_PETRA_FLD_GET(regs->qdr.qdr_controller_reset_reg.qdrc_rst_n, fld_val, 38, exit);
    }

    /*
     *  QDR initialization - verify ready
     */
    poll_info.expected_value = 0x1;
    poll_info.busy_wait_nof_iters = SOC_PETRA_DBG_IGRE_RST_POLL_NOF_BW_ITERS;
    poll_info.timer_nof_iters = SOC_PETRA_DBG_IGRE_RST_POLL_NOF_TD_ITERS;
    poll_info.timer_delay_msec = SOC_PETRA_DBG_IGRE_RST_POLL_TD_MSEC;

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

    if (soc_petra_sw_db_qdr_enable_get(unit))
    {
      /*
       *	Clear QDR Memory and ECC counters
       */
      res = soc_petra_init_qdr_mem_clear(
              unit,
              soc_petra_sw_db_qdr_size_get(unit),
              soc_petra_sw_db_qdr_protection_type_get(unit)
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
    }
  }
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_dbg_qdr_reset()",0,0);
}

/*********************************************************************
 *     Resets only the DPRC
 *     Details: in the H file. (search for prototype)
 *********************************************************************/
uint32
  soc_petra_dbg_dprc_reset_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 dram_ndx
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_DRAM_TYPE
    dram_type;
  SOC_PETRA_HW_DRAM_CONF_PARAMS
    dram_conf;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DBG_DPRC_RESET_UNSAFE);

  sal_memset(&dram_conf, 0x0, sizeof(SOC_PETRA_HW_DRAM_CONF_PARAMS));

  SOC_PETRA_DIFF_DEVICE_CALL(mgmt_hw_adjust_ddr_init,(unit, dram_ndx));

  res = soc_petra_sw_db_dram_type_get(
          unit,
          &dram_type
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_petra_sw_db_dram_conf_get(
          unit,
          &(dram_conf)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  res = soc_petra_dram_info_set_unsafe(
          unit,
          dram_ndx,
          dram_conf.dram_freq,
          dram_type,
          &(dram_conf.params)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 180, exit);

  exit:
    SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_dbg_dprc_reset_unsafe()", dram_ndx, 0);

}


/*********************************************************************
 *     Soft-resets the device. As part of the reset sequence,
 *     traffic is stopped, and re-started (according to the
 *     original condition).
 *     Details: in the H file. (search for prototype)
 *********************************************************************/
uint32
  soc_petra_dbg_dev_reset_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_DBG_RST_DOMAIN      rst_domain
  )
{
  uint32
    soft_init_reg_val,
    soft_init_reg_val_orig,
    autogen_reg_val,
    fmc_scheduler_configs_reg_val_orig,
    fld_val,
    reg_val,
    dram_orig_enabled[SOC_DPP_DEFS_MAX(HW_DRAM_INTERFACES_MAX)],
    res = SOC_SAND_OK;
  uint32
    reg_idx,
    dram_ndx;
  uint8
    poll_success,
    is_traffic_enabled_orig,
    is_ctrl_cells_enabled_orig;
  SOC_PETRA_POLL_INFO
    poll_info;
  SOC_PETRA_REGS
    *regs;
#if SOC_PETRA_DBG_DPRC_FULL_RESET
  SOC_PETRA_DRAM_TYPE
    dram_type;
  SOC_PETRA_HW_DRAM_CONF_PARAMS
    dram_conf;
#endif
#ifdef LINK_PB_LIBRARIES
  uint32
    malg_tx_mlf_rstn_val_orig[SOC_PB_REG_NOF_MALGS],
    ilkn_tx_mlf_rstn_val_orig[SOC_PB_NIF_NOF_ILKN_IDS];
  uint32
    malg_idx,
    ilkn_idx,
    tbl_offset_idx;
  SOC_PB_IRR_TRAFFIC_CLASS_MAPPING_TBL_DATA
    irr_tcm_tbl[8]; /* traffic_class_mapping_tbl.addr.size = 0x0008; */
#endif
  uint8
    is_ingr,
    is_egr,
    is_cfc;
  uint32
    cfc_regs[SOC_PA_DBG_CFC_MIRR_REGS_SIZE_UINT32S];

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DBG_DEV_RESET_UNSAFE);

  regs = soc_petra_regs();

  is_ingr = SOC_SAND_NUM2BOOL((rst_domain == SOC_PETRA_DBG_RST_DOMAIN_INGR) || (rst_domain == SOC_PETRA_DBG_RST_DOMAIN_FULL));
  is_egr = SOC_SAND_NUM2BOOL((rst_domain == SOC_PETRA_DBG_RST_DOMAIN_EGR) || (rst_domain == SOC_PETRA_DBG_RST_DOMAIN_FULL));
  is_cfc = is_egr;

  /*
   *  Reset Counter
   */
#if SOC_PETRA_DBG_COUNT_INGR_RESETS
  {
    SOC_PETRA_REG_FIELD
      spare_fld;

    spare_fld.addr.step = 0;
    spare_fld.addr.base = SOC_SAND_REG_SIZE_BYTES * SOC_PETRA_DBG_INGR_RESET_CNT_REG_ADDR;
    spare_fld.lsb = 0;
    spare_fld.msb = 31;

    SOC_PETRA_IMPLICIT_FLD_SET(spare_fld, Soc_petra_dbg_reset_counter++, 2, exit);
  }
#endif

#ifdef LINK_PB_LIBRARIES
  /*
   *  irr_traffic_class_mapping_tbl values are not preserved upon soft reset for Soc_petra-B.
   *  Save the original values to restore later
   */
  if (SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE)
  {
    for (tbl_offset_idx = 0; tbl_offset_idx < 8; tbl_offset_idx++)
    {
      res = soc_pb_irr_traffic_class_mapping_tbl_get_unsafe(
              unit,
              tbl_offset_idx,
              &(irr_tcm_tbl[tbl_offset_idx])
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 3, exit);
    }
  }
#endif

  if(rst_domain == SOC_PETRA_DBG_RST_DOMAIN_SCH)
  {
    /*
     * Scheduler Only
     */
    res = soc_petra_dbg_sch_reset_unsafe(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
    SOC_PETRA_DO_NOTHING_AND_EXIT;
  }

  /************************************************************************/
  /* Disable Traffic                                                      */
  /************************************************************************/
  /*
   *  Store current traffic-enable-state (just in case: if we got here, it is enabled)
   */
  res = soc_petra_mgmt_enable_traffic_get_unsafe(
          unit,
          &is_traffic_enabled_orig
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_mgmt_enable_traffic_set_unsafe(
          unit,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  if(is_egr)
  {
    /*
     *  Store current cells-enable-state (just in case: if we got here, it is enabled)
     */
    res = soc_petra_mgmt_all_ctrl_cells_enable_get_unsafe(
            unit,
            &is_ctrl_cells_enabled_orig
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 16, exit);

    res = soc_petra_ctrl_cells_en_fast(
            unit,
            FALSE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 18, exit);
  }

  /************************************************************************/
  /* Read original configuration                                          */
  /************************************************************************/
  SOC_PETRA_REG_GET(regs->eci.soc_petra_soft_init_reg, soft_init_reg_val, 20, exit);
  soft_init_reg_val_orig = soft_init_reg_val;

  for (dram_ndx = 0; dram_ndx < SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max); dram_ndx++)
  {
    SOC_PETRA_FLD_IGET(regs->drc.ddr_controller_triggers_reg.ddrrstn, dram_orig_enabled[dram_ndx], dram_ndx, 21, exit);
  }

  /************************************************************************/
  /* IN-RESET                                                             */
  /************************************************************************/
  if(is_ingr)
  {

    SOC_PETRA_REG_GET(regs->ips.fmc_scheduler_configs_reg, fmc_scheduler_configs_reg_val_orig, 22, exit);
    SOC_PETRA_REG_SET(regs->ips.fmc_scheduler_configs_reg, 0x04000000, 25, exit);

    /*
    *  Soft-init: put in-reset IPS, IQM, IPT, MMU, DPRC, IRE, IDR, IRR, FDT
    */
    fld_val = 1;
    for (dram_ndx = 0; dram_ndx < SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max); dram_ndx++)
    {
      if (dram_orig_enabled[dram_ndx])
      {
        SOC_PETRA_FLD_TO_REG(regs->eci.soc_petra_soft_init_reg.dprc_init[dram_ndx], fld_val, soft_init_reg_val, 23, exit);
      }
    }
    SOC_PETRA_FLD_TO_REG(regs->eci.soc_petra_soft_init_reg.ips_init, fld_val, soft_init_reg_val, 24, exit);
    SOC_PETRA_FLD_TO_REG(regs->eci.soc_petra_soft_init_reg.iqm_init, fld_val, soft_init_reg_val, 26, exit);
    SOC_PETRA_FLD_TO_REG(regs->eci.soc_petra_soft_init_reg.ipt_init, fld_val, soft_init_reg_val, 28, exit);
    SOC_PETRA_FLD_TO_REG(regs->eci.soc_petra_soft_init_reg.mmu_init, fld_val, soft_init_reg_val, 30, exit);
    SOC_PETRA_FLD_TO_REG(regs->eci.soc_petra_soft_init_reg.ire_init, fld_val, soft_init_reg_val, 32, exit);
    SOC_PETRA_FLD_TO_REG(regs->eci.soc_petra_soft_init_reg.idr_init, fld_val, soft_init_reg_val, 36, exit);
    SOC_PETRA_FLD_TO_REG(regs->eci.soc_petra_soft_init_reg.irr_init, fld_val, soft_init_reg_val, 38, exit);
    SOC_PETRA_FLD_TO_REG(regs->eci.soc_petra_soft_init_reg.fdt_init, fld_val, soft_init_reg_val, 39, exit);
    SOC_PETRA_REG_SET(regs->eci.soc_petra_soft_init_reg, soft_init_reg_val, 40, exit);

    res = soc_petra_dbg_qdr_reset(
            unit,
            TRUE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 1044, exit);
  } /* is_ingr */

  if(is_egr)
  {
    /* Close FDR */
    SOC_PETRA_REG_GET(regs->fdr.fdr_enablers_reg, reg_val, 1000, exit);
    reg_val |= SOC_SAND_SET_BITS_RANGE(0x7, 26, 24);
    SOC_PETRA_REG_SET(regs->fdr.fdr_enablers_reg, reg_val, 1002, exit);

    /* Close FCR */
    SOC_PETRA_REG_GET(regs->fcr.fcrenablers_and_filter_match_input_link_reg, reg_val, 1000, exit);
    reg_val |= SOC_SAND_SET_BITS_RANGE(0x7, 2, 0);
    SOC_PETRA_REG_SET(regs->fcr.fcrenablers_and_filter_match_input_link_reg, reg_val, 1002, exit);

#ifdef LINK_PB_LIBRARIES
    if (SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE)
    {
      fld_val = 0x1;
      /* Close IPT */
      SOC_PB_REG_GET(regs->ipt.debug_controls_reg, reg_val, 1004, exit);
      SOC_PB_FLD_TO_REG(regs->ipt.debug_controls_reg.fdt_dtq_rd_dis, fld_val, reg_val, 1006, exit);
      SOC_PB_FLD_TO_REG(regs->ipt.debug_controls_reg.egq_dtq_rd_dis, fld_val, reg_val, 1008, exit);
      SOC_PB_REG_SET(regs->ipt.debug_controls_reg, reg_val, 1010, exit);
    }
#endif

    /* Rest scheduler (in-out) */
    res = soc_petra_dbg_sch_reset_unsafe(unit);
    SOC_SAND_CHECK_FUNC_RESULT(res, 1070, exit);

    if (SOC_PETRA_IS_DEV_PETRA_A)
    {
      /* Close EGQ */
      for(reg_idx = 0; reg_idx < SOC_PETRA_NOF_FAP_PORT_REGS; reg_idx++)
      {
        SOC_PETRA_REG_SET(regs->egq.disable_egress_ofp_reg[reg_idx], 0xffffffff, 1012, exit);
      }
    }
#ifdef LINK_PB_LIBRARIES
    else
    {
      /* Stop the link between PQP and FQP in the EGQ */
      for(reg_idx = 0; reg_idx < SOC_PETRA_NOF_FAP_PORT_REGS; reg_idx++)
      {
        SOC_PB_REG_SET(regs->egq.pqp2fqp_ofp_stop_reg[reg_idx], 0xffffffff, 1014, exit);
      } /* For (reg_idx) */

      for(malg_idx = 0; malg_idx < SOC_PB_REG_NOF_MALGS; malg_idx++)
      {
        SOC_PB_REG_GET(regs->nbi.mlf_reset_ports_reg[malg_idx][SOC_PB_REG_NIF_DIRECTION_TX], malg_tx_mlf_rstn_val_orig[malg_idx], 1020, exit);
        SOC_PB_REG_SET(regs->nbi.mlf_reset_ports_reg[malg_idx][SOC_PB_REG_NIF_DIRECTION_TX], 0x0, 1022, exit);
      }

      for(ilkn_idx = 0; ilkn_idx < SOC_PB_NIF_NOF_ILKN_IDS; ilkn_idx++)
      {
        SOC_PB_FLD_GET(regs->nbi.ilknreset_reg.ilkn_controller_rstn[ilkn_idx][SOC_PB_REG_NIF_DIRECTION_TX], ilkn_tx_mlf_rstn_val_orig[ilkn_idx], 1024, exit);
        SOC_PB_FLD_SET(regs->nbi.ilknreset_reg.ilkn_controller_rstn[ilkn_idx][SOC_PB_REG_NIF_DIRECTION_TX], 0x0, 1026, exit);
      }
    } /* Else: Soc_petra-B or above */
#endif
    fld_val = 0x1;
    SOC_PETRA_FLD_TO_REG(regs->eci.soc_petra_soft_init_reg.egq_init, fld_val, soft_init_reg_val, 1030, exit);
    SOC_PETRA_FLD_TO_REG(regs->eci.soc_petra_soft_init_reg.epni_init, fld_val, soft_init_reg_val, 1031, exit);
    SOC_PETRA_FLD_TO_REG(regs->eci.soc_petra_soft_init_reg.fdr_init, fld_val, soft_init_reg_val, 1032, exit);
    SOC_PETRA_FLD_TO_REG(regs->eci.soc_petra_soft_init_reg.fcr_init, fld_val, soft_init_reg_val, 1034, exit);
    SOC_PETRA_FLD_TO_REG(regs->eci.soc_petra_soft_init_reg.olp_init, fld_val, soft_init_reg_val, 1034, exit);
    SOC_PETRA_REG_SET(regs->eci.soc_petra_soft_init_reg, soft_init_reg_val, 1040, exit);
  } /* is_egr */

  if(is_cfc)
  {
    if (SOC_PETRA_IS_DEV_PETRA_A)
    {
      /*
      * CFC
      */
      res = soc_pa_cfc_regs_mirror_retrieve(
              unit,
              cfc_regs
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 1040, exit);

      /*
      * In-Reset
      */
      SOC_PA_FLD_SET(regs->eci.soc_petra_soft_reset_reg.cfc_reset, 0x1, 1042, exit);
    }
#ifdef LINK_PB_LIBRARIES
    else
    {
#ifdef SOC_PB_DBG_CFC_RST_EN
      /*
      * In-Reset (Soft)
      */
      SOC_PB_FLD_SET(regs->eci.soc_petra_soft_init1_reg.cfc_init, 0x1, 1060, exit);
#endif
    }
#endif
  }

  /************************************************************************/
  /* OUT-OF-RESET                                                         */
  /************************************************************************/

  if(is_cfc)
  {
    if (SOC_PETRA_IS_DEV_PETRA_A)
    {
      /*
      * Out-Of-Reset
      */
      SOC_PA_FLD_SET(regs->eci.soc_petra_soft_reset_reg.cfc_reset, 0x0, 1044, exit);
    }
#ifdef LINK_PB_LIBRARIES
    else
    {
#ifdef SOC_PB_DBG_CFC_RST_EN
      /*
      * Out-Of-Reset (Soft)
      */
      SOC_PB_FLD_SET(regs->eci.soc_petra_soft_init1_reg.cfc_init, 0x0, 1062, exit);
#endif
    }
#endif
  }
  
  if(is_ingr)
  {
    /* IDR + MMU - out of soft init */
    fld_val = 0;
    SOC_PETRA_FLD_TO_REG(regs->eci.soc_petra_soft_init_reg.idr_init, fld_val, soft_init_reg_val, 50, exit);
    SOC_PETRA_FLD_TO_REG(regs->eci.soc_petra_soft_init_reg.mmu_init, fld_val, soft_init_reg_val, 52, exit);
    SOC_PETRA_REG_SET(regs->eci.soc_petra_soft_init_reg, soft_init_reg_val, 54, exit);

  /*
    *  Reset Auto-gen
    */
    SOC_PETRA_REG_GET(regs->idr.static_configuration_reg, autogen_reg_val, 60, exit);
    SOC_PETRA_REG_SET(regs->idr.static_configuration_reg, 0x20, 62, exit);
    SOC_PETRA_REG_SET(regs->idr.static_configuration_reg, autogen_reg_val, 64, exit);

    res = soc_petra_dbg_qdr_reset(
            unit,
            FALSE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 1280, exit);

#if (!SOC_PETRA_DBG_DPRC_FULL_RESET)
    /* DPRC - out of soft init*/
    fld_val = 0;
    for (dram_ndx = 0; dram_ndx < SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max); dram_ndx++)
    {
      if (dram_orig_enabled[dram_ndx])
      {
        SOC_PETRA_FLD_TO_REG(regs->eci.soc_petra_soft_init_reg.dprc_init[dram_ndx], fld_val, soft_init_reg_val, 70, exit);
      }
    }
    SOC_PETRA_REG_SET(regs->eci.soc_petra_soft_init_reg, soft_init_reg_val, 72, exit);

    /* DRC in-reset */
    for (dram_ndx = 0; dram_ndx < SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max); dram_ndx++)
    {
      if (dram_orig_enabled[dram_ndx])
      {
        SOC_PETRA_FLD_ISET(regs->drc.ddr_controller_triggers_reg.ddrrstn, 0x0, dram_ndx, 80, exit);
      }
    }

    /* DPI-init */
    for (dram_ndx = 0; dram_ndx < SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max); dram_ndx++)
    {
      if (dram_orig_enabled[dram_ndx])
      {
        SOC_PETRA_REG_ISET(regs->dpi.dpi_init_start_reg, 0x1, dram_ndx, 90, exit);
      }
    }

    /* DRC out-of-reset */
    for (dram_ndx = 0; dram_ndx < SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max); dram_ndx++)
    {
      if (dram_orig_enabled[dram_ndx])
      {
        SOC_PETRA_FLD_ISET(regs->drc.ddr_controller_triggers_reg.ddrrstn, 0x1, dram_ndx, 100, exit);
      }
    }
#else
    sal_memset(&dram_conf, 0x0, sizeof(SOC_PETRA_HW_DRAM_CONF_PARAMS));
    res = soc_petra_sw_db_dram_type_get(
            unit,
            &dram_type
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 1250, exit);

    res = soc_petra_sw_db_dram_conf_get(
            unit,
            &(dram_conf)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 1260, exit);

    for (dram_ndx = 0; dram_ndx < SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max); dram_ndx++)
    {
      if (dram_orig_enabled[dram_ndx])
      {
        SOC_PETRA_DIFF_DEVICE_CALL(mgmt_hw_adjust_ddr_init,(unit, dram_ndx));

        res = soc_petra_dram_info_set_unsafe(
          unit,
          dram_ndx,
          dram_conf.dram_freq,
          dram_type,
          &(dram_conf.params)
          );
        SOC_SAND_CHECK_FUNC_RESULT(res, 1270, exit);
      }
    }
#endif

    SOC_PETRA_REG_SET(regs->ips.fmc_scheduler_configs_reg, fmc_scheduler_configs_reg_val_orig, 2160, exit);
  } /* is_ingr */

  /************************************************************************/
  /* OUT-OF-RESET, Revert to original (Soft-init per-block map)           */
  /************************************************************************/
  /* soft_init_reg_val_orig is probably 0x0 - take all other blocks out of soft init */
  SOC_PETRA_REG_SET(regs->eci.soc_petra_soft_init_reg, soft_init_reg_val_orig, 110, exit);

  /************************************************************************/
  /* Validate/Poll for out-of-reset/init-done indications                 */
  /************************************************************************/

  soc_petra_PETRA_POLL_INFO_clear(&poll_info);
  poll_info.busy_wait_nof_iters = SOC_PETRA_DBG_IGRE_RST_POLL_NOF_BW_ITERS;
  poll_info.timer_nof_iters = SOC_PETRA_DBG_IGRE_RST_POLL_NOF_TD_ITERS;
  poll_info.timer_delay_msec = SOC_PETRA_DBG_IGRE_RST_POLL_TD_MSEC;
  poll_info.expected_value = 0x0;

  res = soc_petra_status_fld_poll_unsafe(
    unit,
    SOC_PETRA_REG_DB_ACC_REF(regs->iqm.iqm_init_reg.iqc_init),
    SOC_PETRA_DEFAULT_INSTANCE,
    &poll_info,
    &poll_success
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 112, exit);

  if (poll_success == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_INGR_RST_INTERN_BLOCK_INIT_ERR, 114, exit);
  }

  res = soc_petra_status_fld_poll_unsafe(
    unit,
    SOC_PETRA_REG_DB_ACC_REF(regs->ips.ips_general_configurations_reg.ips_init_trigger),
    SOC_PETRA_DEFAULT_INSTANCE,
    &poll_info,
    &poll_success
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 116, exit);

  if (poll_success == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_INGR_RST_INTERN_BLOCK_INIT_ERR, 118, exit);
  }

  poll_info.expected_value = 0x1;
  /* DRC ready polling */
  for (dram_ndx = 0; dram_ndx < SOC_DPP_DEFS_GET(unit, hw_dram_interfaces_max); dram_ndx++)
  {
    if (dram_orig_enabled[dram_ndx])
    {
      res = soc_petra_status_fld_poll_unsafe(
        unit,
        SOC_PETRA_REG_DB_ACC_REF(regs->dpi.dpi_init_status_reg.ready),
        dram_ndx,
        &poll_info,
        &poll_success
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);

      if (poll_success == FALSE)
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_INGR_RST_INTERN_BLOCK_INIT_ERR, 130+dram_ndx, exit);
      }
    }
  }

  /*
  *  EGQ initialization - verify init is finished
  */
  poll_info.busy_wait_nof_iters = SOC_PETRA_DBG_IGRE_RST_POLL_NOF_BW_ITERS;
  poll_info.timer_nof_iters = SOC_PETRA_DBG_IGRE_RST_POLL_NOF_TD_ITERS;
  poll_info.timer_delay_msec = SOC_PETRA_DBG_IGRE_RST_POLL_TD_MSEC;
  poll_info.expected_value = 0x0;

  res = soc_petra_status_fld_poll_unsafe(
    unit,
    SOC_PETRA_REG_DB_ACC_REF(regs->egq.egqblock_init_status_reg.egqblock_init),
    SOC_PETRA_DEFAULT_INSTANCE,
    &poll_info,
    &poll_success
    );
  SOC_SAND_CHECK_FUNC_RESULT(res, 140, exit);

  if (poll_success == FALSE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_MGMT_EGQ_INIT_FAILS_ERR, 145, exit);
  }

  if(is_egr)
  {
    if (SOC_PETRA_IS_DEV_PETRA_A)
    {
      /* Open EGQ */
      for(reg_idx = 0; reg_idx < SOC_PETRA_NOF_FAP_PORT_REGS; reg_idx++)
      {
        SOC_PETRA_REG_SET(regs->egq.disable_egress_ofp_reg[reg_idx], 0x0, 2012, exit);
      }
    }
#ifdef LINK_PB_LIBRARIES
    else
    {
      /* Stop the link between PQP and FQP in the EGQ */
      for(reg_idx = 0; reg_idx < SOC_PETRA_NOF_FAP_PORT_REGS; reg_idx++)
      {
        SOC_PB_REG_SET(regs->egq.pqp2fqp_ofp_stop_reg[reg_idx], 0x0, 2014, exit);
      } /* For (reg_idx) */

      for(malg_idx = 0; malg_idx < SOC_PB_REG_NOF_MALGS; malg_idx++)
      {
        SOC_PB_REG_SET(regs->nbi.mlf_reset_ports_reg[malg_idx][SOC_PB_REG_NIF_DIRECTION_TX], malg_tx_mlf_rstn_val_orig[malg_idx], 2020, exit);
      }
      for(ilkn_idx = 0; ilkn_idx < SOC_PB_NIF_NOF_ILKN_IDS; ilkn_idx++)
      {
        SOC_PB_FLD_SET(regs->nbi.ilknreset_reg.ilkn_controller_rstn[ilkn_idx][SOC_PB_REG_NIF_DIRECTION_TX], ilkn_tx_mlf_rstn_val_orig[ilkn_idx], 2024, exit);
      }
    } /* Else: Soc_petra-B or above */
#endif

    /* Open FDR */
    SOC_PETRA_REG_GET(regs->fdr.fdr_enablers_reg, reg_val, 2000, exit);
    reg_val &= SOC_SAND_ZERO_BITS_MASK(26, 24);
    SOC_PETRA_REG_SET(regs->fdr.fdr_enablers_reg, reg_val, 2002, exit);

    /* Open FCR */
    SOC_PETRA_REG_GET(regs->fcr.fcrenablers_and_filter_match_input_link_reg, reg_val, 2000, exit);
    reg_val  &= SOC_SAND_ZERO_BITS_MASK(2, 0);
    SOC_PETRA_REG_SET(regs->fcr.fcrenablers_and_filter_match_input_link_reg, reg_val, 2002, exit);

#ifdef LINK_PB_LIBRARIES
    if (SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE)
    {
      fld_val = 0x0;
      /* Open IPT */
      SOC_PB_REG_GET(regs->ipt.debug_controls_reg, reg_val, 2004, exit);
      SOC_PB_FLD_TO_REG(regs->ipt.debug_controls_reg.fdt_dtq_rd_dis, fld_val, reg_val, 2006, exit);
      SOC_PB_FLD_TO_REG(regs->ipt.debug_controls_reg.egq_dtq_rd_dis, fld_val, reg_val, 2008, exit);
      SOC_PB_REG_SET(regs->ipt.debug_controls_reg, reg_val, 2010, exit);
    }
#endif
  } /* is_egr */

  /************************************************************************/
  /* Restore Configuration if needed                                      */
  /************************************************************************/

  if(is_cfc)
  {
    if (SOC_PETRA_IS_DEV_PETRA_A)
    {
      res = soc_pa_cfc_regs_mirror_load(
        unit,
        cfc_regs
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 2150, exit);
    }
  }

#ifdef LINK_PB_LIBRARIES
  /*
   *  irr_traffic_class_mapping_tbl values are not preserved upon soft reset for Soc_petra-B.
   *  Save the original values to restore later
   */
  if (SOC_PETRA_IS_DEV_PETRA_B_OR_ABOVE)
  {
    for (tbl_offset_idx = 0; tbl_offset_idx < 8; tbl_offset_idx++)
    {
      res = soc_pb_irr_traffic_class_mapping_tbl_set_unsafe(
              unit,
              tbl_offset_idx,
              &(irr_tcm_tbl[tbl_offset_idx])
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 2160, exit);
    }
  }
#endif
  if(is_egr)
  {
    res = soc_petra_ctrl_cells_en_fast(
            unit,
            is_ctrl_cells_enabled_orig
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 2200, exit);
  }

  /************************************************************************/
  /*  Restore traffic                                                     */
  /************************************************************************/
  res = soc_petra_mgmt_enable_traffic_set_unsafe(
          unit,
          is_traffic_enabled_orig
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 3000, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_dbg_dev_reset_unsafe()", rst_domain, 0);
}

uint32
  soc_petra_dbg_dev_reset_verify(
    SOC_SAND_IN  SOC_PETRA_DBG_RST_DOMAIN      rst_domain
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DBG_DEV_RESET_VERIFY);

  SOC_SAND_ERR_IF_ABOVE_MAX(rst_domain, SOC_PETRA_DEBUG_RST_DOMAIN_MAX, SOC_PETRA_DEBUG_RST_DOMAIN_OUT_OF_RANGE_ERR, 10, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_dbg_dev_reset_verify()", rst_domain, 0);
}

/*********************************************************************
*     This function tests the ECI access to Soc_petra.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_dbg_eci_access_tst_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 nof_k_iters,
    SOC_SAND_IN  uint8                 use_dflt_tst_reg,
    SOC_SAND_IN  uint32                  reg1_addr_longs,
    SOC_SAND_IN  uint32                  reg2_addr_longs,
    SOC_SAND_OUT uint8                 *is_valid
  )
{
  uint32
    r1_data,
    r2_data = 0,
    r1_val,
    r2_val = 0,
    r1_val_orig = 0xffffffff,
    r2_val_orig = 0xffffffff,
    res = SOC_SAND_OK;
  uint32
    iter_i;
  SOC_PETRA_REG_ADDR
    r1_addr,
    r2_addr;
  uint8
    eci_valid = TRUE;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DBG_ECI_ACCESS_TST_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(is_valid);

  regs = soc_petra_regs();

  if (use_dflt_tst_reg == TRUE)
  {
    r1_addr.base = SOC_PETRA_REG_DB_ACC(regs->eci.test_reg.addr.base);
    r1_addr.step = SOC_PETRA_REG_DB_ACC(regs->eci.test_reg.addr.step);

    /*
     *	Just in case, to have some harmless init
     */
    r2_addr.base = SOC_PETRA_REG_DB_ACC(regs->eci.test_reg.addr.base);
    r2_addr.step = SOC_PETRA_REG_DB_ACC(regs->eci.test_reg.addr.step);
  }
  else
  {
    r1_addr.base = reg1_addr_longs * SOC_SAND_REG_SIZE_BYTES;
    r1_addr.step = 0x0;

    r2_addr.base = reg2_addr_longs * SOC_SAND_REG_SIZE_BYTES;
    r2_addr.step = 0x0;
  }

  /*
   *	Save the original configuration
   */
  res = soc_petra_read_reg_unsafe(
          0,
          &(r1_addr),
          SOC_PETRA_DEFAULT_INSTANCE,
          &r1_val_orig
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

  if (use_dflt_tst_reg == FALSE)
  {
    res = soc_petra_read_reg_unsafe(
            0,
            &(r2_addr),
            SOC_PETRA_DEFAULT_INSTANCE,
            &r2_val_orig
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 8, exit);
  }

  /*
   *	Perform ECI access, the requested number of iterations
   */
  for (iter_i = 0; iter_i < nof_k_iters * 1000; iter_i++)
  {
    if (use_dflt_tst_reg == TRUE)
    {
      r1_data = (iter_i%2 == 0)?SOC_PETRA_DBG_ECI_ACCESS_PATTERN_1:SOC_PETRA_DBG_ECI_ACCESS_PATTERN_2;
    }
    else
    {
      r1_data = SOC_PETRA_DBG_ECI_ACCESS_PATTERN_1;
      r2_data = SOC_PETRA_DBG_ECI_ACCESS_PATTERN_2;
    }

    /*
     *	Write pattern(s)
     */
    res = soc_petra_write_reg_unsafe(
            0,
            &(r1_addr),
            SOC_PETRA_DEFAULT_INSTANCE,
            r1_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if (use_dflt_tst_reg == FALSE)
    {
      res = soc_petra_write_reg_unsafe(
              0,
              &(r2_addr),
              SOC_PETRA_DEFAULT_INSTANCE,
              r2_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 12, exit);
    }

    /*
     *	Read pattern(s)
     */
    res = soc_petra_read_reg_unsafe(
            0,
            &(r1_addr),
            SOC_PETRA_DEFAULT_INSTANCE,
            &r1_val
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (use_dflt_tst_reg == FALSE)
    {
      res = soc_petra_read_reg_unsafe(
              0,
              &(r2_addr),
              SOC_PETRA_DEFAULT_INSTANCE,
              &r2_val
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 22, exit);
    }

    /*
     *	Validate
     */
    if (use_dflt_tst_reg == TRUE)
    {
      if (r1_val != ~(r1_data))
      {
        eci_valid = FALSE;
#if SOC_PETRA_DEBUG_IS_LVL2
        soc_sand_os_printf(
          "ECI ACCESS ERROR: Address: 0x%04x, Expected: 0x%08x, Actual: 0x%08x\n\r",
          r1_addr.base / SOC_SAND_REG_SIZE_BYTES,
          r1_data,
          r1_val
        );
#endif
      }
    }
    else /* (use_dflt_tst_reg == FALSE) */
    {
      if (r1_val != r1_data)
      {
        eci_valid = FALSE;
#if SOC_PETRA_DEBUG_IS_LVL2
        soc_sand_os_printf(
          "ECI ACCESS ERROR: Address: 0x%04x, Expected: 0x%08x, Actual: 0x%08x\n\r",
          r1_addr.base / SOC_SAND_REG_SIZE_BYTES,
          r1_data,
          r1_val
        );
#endif
      }

      if (r2_val != r2_data)
      {
        eci_valid = FALSE;
#if SOC_PETRA_DEBUG_IS_LVL2
        soc_sand_os_printf(
          "ECI ACCESS ERROR: Address: 0x%04x, Expected: 0x%08x, Actual: 0x%08x\n\r",
          r2_addr.base / SOC_SAND_REG_SIZE_BYTES,
          r2_data,
          r2_val
        );
#endif
      }
    } /* (use_dflt_tst_reg == FALSE) */
  } /* for(;;;) */

  /*
   *	Restore the original configuration
   */
  if (use_dflt_tst_reg == TRUE)
  {
    res = soc_petra_write_reg_unsafe(
            0,
            &(r1_addr),
            SOC_PETRA_DEFAULT_INSTANCE,
            ~(r1_val_orig)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }
  else /* (use_dflt_tst_reg == FALSE) */
  {
    res = soc_petra_write_reg_unsafe(
            0,
            &(r1_addr),
            SOC_PETRA_DEFAULT_INSTANCE,
            r1_val_orig
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 32, exit);

    res = soc_petra_write_reg_unsafe(
            0,
            &(r2_addr),
            SOC_PETRA_DEFAULT_INSTANCE,
            r2_val_orig
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 34, exit);
  }

  *is_valid = eci_valid;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_dbg_eci_access_tst_unsafe()",0,0);
}

/*********************************************************************
 *     Resets the end-to-end scheduler. The reset is performed
 *     by clearing the internal scheduler pipes, and then
 *     performing soft-reset.
 *     Details: in the H file. (search for prototype)
 *********************************************************************/
uint32
  soc_petra_dbg_sch_reset_unsafe(
    SOC_SAND_IN  int unit
  )
{
  uint32
    bmsg_en_fld_val,
    dlm_en_fld_val,
    mc_en_fld_val[SOC_PETRA_NOF_MULTICAST_CLASSES],
    ingr_shp_en_fld_val,
    res = SOC_SAND_OK;
  uint32
    idx;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_SCH_SCHEDULER_INIT_TBL_DATA
    init_tbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DBG_SCH_RESET_UNSAFE);

  regs = soc_petra_regs();

  SOC_PETRA_FLD_SET(regs->sch.dvs_config1_reg.force_pause, 0x1, 10, exit);
  SOC_PETRA_FLD_SET(regs->sch.scheduler_configuration_reg.smpdisable_fabric, 0x1, 20, exit);
  
  SOC_PETRA_FLD_GET(regs->sch.smp_internal_messages.enable, bmsg_en_fld_val, 30, exit);
  SOC_PETRA_FLD_SET(regs->sch.smp_internal_messages.enable, 0x0, 32, exit);

  SOC_PETRA_FLD_GET(regs->sch.dlm_reg.dlm_ena, dlm_en_fld_val, 40, exit);
  SOC_PETRA_FLD_SET(regs->sch.dlm_reg.dlm_ena, 0x0, 42, exit);
  
  for (idx = 0; idx < SOC_PETRA_NOF_MULTICAST_CLASSES; idx++)
  {
    SOC_PETRA_FLD_GET(regs->sch.sch_fabric_multicast_port_configuration_reg.multicast_class[idx].enable, mc_en_fld_val[idx], 50+idx, exit);
    SOC_PETRA_FLD_SET(regs->sch.sch_fabric_multicast_port_configuration_reg.multicast_class[idx].enable, 0x0, 60+idx, exit);
  }

  SOC_PETRA_FLD_GET(regs->sch.ingress_shaping_port_configuration_reg.ingress_shaping_enable, ingr_shp_en_fld_val, 70, exit);
  SOC_PETRA_FLD_SET(regs->sch.ingress_shaping_port_configuration_reg.ingress_shaping_enable, 0x0, 72, exit);

  /*
   * Soft reset the scheduler
   */
  init_tbl.schinit = 0x0;

  res = soc_petra_sch_scheduler_init_tbl_set_unsafe(
          unit,
          0x0,
          &init_tbl
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);


  /*
   * Recover original configuration
   */

  SOC_PETRA_FLD_SET(regs->sch.dvs_config1_reg.force_pause, 0x0, 110, exit);
  SOC_PETRA_FLD_SET(regs->sch.scheduler_configuration_reg.smpdisable_fabric, 0x0, 120, exit);
  SOC_PETRA_FLD_SET(regs->sch.smp_internal_messages.enable, bmsg_en_fld_val, 130, exit);
  SOC_PETRA_FLD_SET(regs->sch.dlm_reg.dlm_ena, dlm_en_fld_val, 140, exit);

  for (idx = 0; idx < SOC_PETRA_NOF_MULTICAST_CLASSES; idx++)
  {
    SOC_PETRA_FLD_SET(regs->sch.sch_fabric_multicast_port_configuration_reg.multicast_class[idx].enable, mc_en_fld_val[idx], 150+idx, exit);
  }

  SOC_PETRA_FLD_SET(regs->sch.ingress_shaping_port_configuration_reg.ingress_shaping_enable, ingr_shp_en_fld_val, 170, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_dbg_sch_reset_unsafe()", 0, 0);
}

/*********************************************************************
*     Test a Flow Shaper as a potential-stuck-shaper. The
 *     criteria is: - The shaper is active, i.e. configured
 *     max-burst and rate are non-zero. - The shaper bucket is
 *     empty (zero-size). Note: this state is a valid state,
 *     since a bucket can be emptied by received credits. The
 *     stages for detecting a stuck shaper are: 1. Detect a
 *     suspected shaper (empty bucket) 2. Start measuring the
 *     credits to the suspected shaper using a programmable
 *     counter 3. Wait for at least 500 milliseconds to see if
 *     the shaper receives credits 4. Confirm a stuck shaper:
 *     check if the shaper bucket is empty. If empty, and no
 *     credits were received over the measurement period, the
 *     shaper is stuck. This confirmation is done by the
 *     dbg_sch_suspect_spr_confirm API.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_dbg_sch_suspect_spr_detect_unsafe(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 flow_id,
    SOC_SAND_OUT uint8 *is_suspect
  )
{
  uint32
    offset = 0,
    reg_val,
    fld_val,
    res = SOC_SAND_OK;
  SOC_PETRA_SCH_INTERNAL_SUB_FLOW_DESC
    flow;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_SCH_SHDS_TBL_DATA
    shds_tbl;
  uint8
    is_suspect_flow = FALSE;
  SOC_PETRA_SCH_TMC_TBL_DATA
    soc_tmctbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DBG_SCH_SUSPECT_SPR_DETECT_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(is_suspect);

  regs = soc_petra_regs();
  /*
   * Read indirect from SHDS table
   */

  offset = flow_id;

  res = soc_petra_sch_tmc_tbl_get_unsafe(
          unit,
          offset,
          &soc_tmctbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (soc_tmctbl.token_count == 0x0)
  {
    offset  = flow_id/2;

    res = soc_petra_sch_shds_tbl_get_unsafe(
            unit,
            offset,
            &shds_tbl
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (flow_id%2 == 0)
    {
      flow.max_burst = shds_tbl.max_burst_even;
      flow.peak_rate_man = shds_tbl.peak_rate_man_even;
      flow.peak_rate_exp = shds_tbl.peak_rate_exp_even;
    }
    else
    {
      flow.max_burst = shds_tbl.max_burst_odd;
      flow.peak_rate_man = shds_tbl.peak_rate_man_odd;
      flow.peak_rate_exp = shds_tbl.peak_rate_exp_odd;
    }

    if ( !(
           ((flow.peak_rate_exp == 0x0) && (flow.peak_rate_man == 0x0)) ||
           (flow.max_burst == 0x0)
          ) )
    {
      is_suspect_flow = TRUE;
    }
  } /* (soc_tmctbl.token_count == 0x0) */

  if (is_suspect_flow)
  {
    /*
     * Set programmable counter to count credits for
     * this flow
     */

    /* 1. Set flow ID and FilterFlowMask should be all ones */
    reg_val = 0x0;
    fld_val = flow_id;
    SOC_PETRA_FLD_TO_REG(regs->sch.credit_counter_configuration_1_reg.filter_flow, fld_val, reg_val, 32, exit);
    fld_val = SOC_PETRA_FLD_MAX(SOC_PETRA_REG_DB_ACC(regs->sch.credit_counter_configuration_1_reg.filter_flow_mask));
    SOC_PETRA_FLD_TO_REG(regs->sch.credit_counter_configuration_1_reg.filter_flow_mask, fld_val, reg_val, 34, exit);
    SOC_PETRA_REG_SET(regs->sch.credit_counter_configuration_1_reg, reg_val, 36, exit);
    
    /* 2. Set FilterByFlow to "1" and FilterBySubFlow to "1" FilterByFap to "0" and CntByGtimer to "0" */
    reg_val = 0x0;
    fld_val = 0x1;
    SOC_PETRA_FLD_TO_REG(regs->sch.credit_counter_configuration_2_reg.filter_by_flow, fld_val, reg_val, 42, exit);
    SOC_PETRA_FLD_TO_REG(regs->sch.credit_counter_configuration_2_reg.filter_by_sub_flow, fld_val, reg_val, 44, exit);
    SOC_PETRA_REG_SET(regs->sch.credit_counter_configuration_2_reg, reg_val, 46, exit);

    /* 3.	Read the counter to clear it */
    SOC_PETRA_REG_GET(regs->sch.credit_counter_reg, reg_val, 50, exit);
  }

  *is_suspect = is_suspect_flow;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_dbg_sch_suspect_spr_detect_unsafe()", 0, 0);
}

uint32
  soc_petra_dbg_sch_suspect_spr_detect_verify(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 flow_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DBG_SCH_SUSPECT_SPR_DETECT_VERIFY);

  /* verify flow id */
  res = soc_petra_sch_flow_id_verify_unsafe(
          unit,
          flow_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_dbg_sch_suspect_spr_detect_verify()", flow_id, 0);
}

/*********************************************************************
*     Confirm a potential-stuck-shaper. The stages for
 *     detecting a stuck shaper are: 1. Detect a suspected
 *     shaper (empty bucket) 2. Start measuring the credits to
 *     the suspected shaper using a programmable counter 3.
 *     Wait for at least 500 milliseconds to see if the shaper
 *     receives credits 4. Confirm a stuck shaper: check if the
 *     shaper bucket is empty. If empty, and no credits were
 *     received over the measurement period, the shaper is
 *     stuck.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_dbg_sch_suspect_spr_confirm_unsafe(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 flow_id,
    SOC_SAND_OUT uint8 *is_confirmed
  )
{
  uint32
    reg_val,
    offset,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;
  uint8
    is_flow_stuck = FALSE;
  SOC_PETRA_SCH_TMC_TBL_DATA
    soc_tmctbl;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DBG_SCH_SUSPECT_SPR_CONFIRM_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(is_confirmed);

  regs = soc_petra_regs();

  SOC_PETRA_REG_GET(regs->sch.credit_counter_reg, reg_val, 10, exit);

  if (reg_val == 0x0)
  {
    offset = flow_id;

    res = soc_petra_sch_tmc_tbl_get_unsafe(
            unit,
            offset,
            &soc_tmctbl
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    if (soc_tmctbl.token_count == 0x0)
    {
      is_flow_stuck = TRUE;
    }
  }
  
  *is_confirmed = is_flow_stuck;
 
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_dbg_sch_suspect_spr_confirm_unsafe()", 0, 0);
}

uint32
  soc_petra_dbg_sch_suspect_spr_confirm_verify(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 flow_id
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DBG_SCH_SUSPECT_SPR_CONFIRM_VERIFY);

  /* verify flow id */
  res = soc_petra_sch_flow_id_verify_unsafe(
          unit,
          flow_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_dbg_sch_suspect_spr_confirm_verify()", 0, 0);
}

#ifdef LINK_PA_LIBRARIES
STATIC
  uint32
    soc_petra_dbg_pcm_value_get(
      SOC_SAND_IN  int                          unit,
      SOC_SAND_IN  uint32                          block_ndx,
      SOC_SAND_IN  SOC_PETRA_REGS_PCMI_CONFIG_REG_FORMAT  *config,
      SOC_SAND_IN  SOC_PETRA_REGS_PCMI_RESULTS_REG_FORMAT *results,
      SOC_SAND_OUT uint32                          *pcm_val
    )
{
  uint32
    res = SOC_SAND_OK,
    core_freq_in_mhz,
    core_freq,
    fld_val;
  SOC_PETRA_POLL_INFO
    poll_info;
  uint8
    success;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  soc_petra_PETRA_POLL_INFO_clear(&poll_info);

  /*
   *  Get the device's core frequency
   */
  res = soc_petra_mgmt_core_frequency_get_unsafe(
          unit,
          &core_freq_in_mhz
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  core_freq = 1000000 * core_freq_in_mhz;

  SOC_PA_IMPLICIT_FLD_ISET(config->cntr_data, core_freq, block_ndx, 20, exit);

  sal_msleep(1000);

  poll_info.busy_wait_nof_iters = 5;
  poll_info.expected_value      = 0x1;
  poll_info.timer_nof_iters     = 1;
  poll_info.timer_delay_msec    = 500;
  res = soc_petra_status_fld_poll_unsafe(
          unit,
          &(results->pcm_iv_clk_exp),
          block_ndx,
          &poll_info,
          &success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  if (success)
  {
    SOC_PA_IMPLICIT_FLD_IGET(results->pcm_ivrgoc_cntr, fld_val, block_ndx, 40, exit);
    *pcm_val = fld_val;
  }
  else
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_DBG_PCM_COUNTER_NOT_EXPIRED_ERR, 50, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_dbg_pcm_value_get()", 0, 0);
}
#endif /* LINK_PA_LIBRARIES */

/*********************************************************************
*     Measures the PCM values for the device's blocks.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_dbg_pcm_readings_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_DBG_PCM_RESULTS     *results
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PA_REGS
    *regs;
  uint32
    ndx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DBG_PCM_READINGS_GET_UNSAFE);

  SOC_PETRA_API_INVALID_FOR_PETRA_B(5, exit);

  SOC_SAND_CHECK_NULL_INPUT(results);

  soc_petra_PETRA_DBG_PCM_RESULTS_clear(results);

  regs = soc_pa_regs();

  res = soc_petra_dbg_pcm_value_get(
          unit,
          SOC_PETRA_DEFAULT_INSTANCE,
          &(regs->eci.pcmi_config_reg),
          &(regs->eci.pcmi_results_reg),
          &(results->eci)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_petra_dbg_pcm_value_get(
          unit,
          SOC_PETRA_DEFAULT_INSTANCE,
          &(regs->ips.pcmi_config_reg),
          &(regs->ips.pcmi_results_reg),
          &(results->ips)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_petra_dbg_pcm_value_get(
          unit,
          SOC_PETRA_DEFAULT_INSTANCE,
          &(regs->iqm.pcmi_config_reg),
          &(regs->iqm.pcmi_results_reg),
          &(results->iqm)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  for (ndx = 0; ndx < SOC_PETRA_BLK_NOF_INSTANCES_DPI; ++ndx)
  {
    res = soc_petra_dbg_pcm_value_get(
            unit,
            ndx,
            &(regs->dpi.pcmi_config_reg),
            &(regs->dpi.pcmi_results_reg),
            &(results->dprc[ndx])
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

  res = soc_petra_dbg_pcm_value_get(
          unit,
          SOC_PETRA_DEFAULT_INSTANCE,
          &(regs->ihp.pcmi_config_reg),
          &(regs->ihp.pcmi_results_reg),
          &(results->ihp)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_petra_dbg_pcm_value_get(
          unit,
          SOC_PETRA_DEFAULT_INSTANCE,
          &(regs->idr.pcmi_config_reg),
          &(regs->idr.pcmi_results_reg),
          &(results->idr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  res = soc_petra_dbg_pcm_value_get(
          unit,
          SOC_PETRA_DEFAULT_INSTANCE,
          &(regs->fdr.pcmi_config_reg),
          &(regs->fdr.pcmi_results_reg),
          &(results->fdr)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  res = soc_petra_dbg_pcm_value_get(
          unit,
          SOC_PETRA_DEFAULT_INSTANCE,
          &(regs->egq.pcmi_config_reg),
          &(regs->egq.pcmi_results_reg),
          &(results->egq1)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  res = soc_petra_dbg_pcm_value_get(
          unit,
          SOC_PETRA_DEFAULT_INSTANCE,
          &(regs->egq.pcmi_config_reg1),
          &(regs->egq.pcmi_results_reg1),
          &(results->egq2)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

  res = soc_petra_dbg_pcm_value_get(
          unit,
          SOC_PETRA_DEFAULT_INSTANCE,
          &(regs->epni.pcmi_config_reg),
          &(regs->epni.pcmi_results_reg),
          &(results->epni)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

  res = soc_petra_dbg_pcm_value_get(
          unit,
          SOC_PETRA_DEFAULT_INSTANCE,
          &(regs->msw.pcmi_config_reg),
          &(regs->msw.pcmi_results_reg),
          &(results->msw)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

  for (ndx = 0; ndx < SOC_PETRA_BLK_NOF_INSTANCES_NIF; ++ndx)
  {
    res = soc_petra_dbg_pcm_value_get(
            unit,
            ndx,
            &(regs->nif.pcmi_config_reg),
            &(regs->nif.pcmi_results_reg),
            &(results->nif1[ndx])
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 120, exit);

    res = soc_petra_dbg_pcm_value_get(
            unit,
            ndx,
            &(regs->nif.pcmi_config_reg1),
            &(regs->nif.pcmi_results_reg1),
            &(results->nif2[ndx])
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 130, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_dbg_pcm_readings_get_unsafe()", 0, 0);
}

/* } */
#include <soc/dpp/SAND/Utils/sand_footer.h>

