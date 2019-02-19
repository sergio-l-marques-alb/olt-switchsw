/* $Id: soc_pb_scheduler_device.c,v 1.5 Broadcom SDK $
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

#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/SAND/Utils/sand_conv.h>

#include <soc/dpp/Petra/petra_scheduler_device.h>
#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_tbl_access.h>
#include <soc/dpp/Petra/petra_chip_defines.h>
#include <soc/dpp/Petra/petra_mgmt.h>
#include <soc/dpp/Petra/petra_sw_db.h>

#include <soc/dpp/Petra/PB_TM/pb_api_general.h>
#include <soc/dpp/Petra/PB_TM/pb_api_nif.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

/* $Id: soc_pb_scheduler_device.c,v 1.5 Broadcom SDK $
 *  Internal Rate resolution - Kilo-bits per second.
 */
#define SOC_PETRA_SCH_DEV_RATE_INTERVAL_RESOLUTION 64

/*
 *  Scheduler index definitions.
 *  Indexes 0-3, 16-19 represent MAL 0-3, 4-7 accordingly.
 *  This corresponds to NIF indexes 0, 4, 8,... 28.
 *  Those indexes are also referred as "channelized indexes".
 *  This does not necessarily mean that the interface is channelized
 *  (for example it can be XAUI non-channelized interface),
 *  but a channelized (SPAUI) interface can be one of these indexes.
 *  These indexes are refered to in scheduler registers as 'ChNif-registers'
 *
 *  Indexes 4-15, 20-31 represent the rest of network indexes
 *  in range 0..31. These can be SGMII interface indexes (non-channelized).
 *  These indexes are refered to in scheduler registers as 'OnePortNif-registers'
 */



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
*     Gets, for a specified device interface, (NIF-Ports,
*     recycling & CPU) its maximal credit rate. This API is
*     only valid for Channelized interface id-s (0, 4, 8... for NIF) - see REMARKS
*     section below.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_sch_if_shaper_rate_get_unsafe(
    SOC_SAND_IN     int           unit,
    SOC_SAND_IN     SOC_PETRA_INTERFACE_ID  if_ndx,
    SOC_SAND_OUT    uint32            *rate
  )
{
  uint32
    res,
    rate_internal,
    credit_rate,
    device_ticks_per_sec,
    credit_worth;
  uint32
    sch_nif_offset,
    mal_idx;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_REG_FIELD
    *max_cr_rate_fld;
  SOC_PETRA_INTERFACE_TYPE
    interface_type;
  uint8
    fast_traversal = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_SCH_IF_SHAPER_RATE_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(rate);

  res = soc_pb_interface_id_verify(if_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  regs = soc_petra_regs();

  res = soc_pb_if_type_from_id(if_ndx,&interface_type);
  SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);

  mal_idx = SOC_PB_IF2MAL_NDX(if_ndx);

  switch(interface_type)
  {
  case SOC_PETRA_IF_TYPE_NIF:
      sch_nif_offset = soc_petra_sch_if2sched_offset(soc_pb_nif2intern_id(if_ndx));
      if (!SOC_PB_NIF_IS_GMII_ONLY_ID(soc_pb_nif2intern_id(if_ndx)))
      {
        max_cr_rate_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.ch_nif_reg[mal_idx].ch_nifxx_max_cr_rate);
      }
      else
      {
        max_cr_rate_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.port_nif_reg[sch_nif_offset].port_nifxx_max_cr_rate);
        fast_traversal = TRUE;
      }
      break;
  case SOC_PETRA_IF_TYPE_CPU:
      max_cr_rate_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.cpu_rate1_reg.cpumax_cr_rate);
      break;
  case SOC_PETRA_IF_TYPE_RCY:
      max_cr_rate_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.rcy_rate1_reg.rcymax_cr_rate);
      break;
  case SOC_PETRA_IF_TYPE_ERP:
      max_cr_rate_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.erp_config_reg.erpmax_cr_rate);
      fast_traversal = TRUE;
      break;
  case SOC_PETRA_IF_TYPE_OLP:
      max_cr_rate_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.olp_config_reg.olpmax_cr_rate);
      fast_traversal = TRUE;
      break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_INVALID_IF_TYPE_ERR, 10, exit);
      break;
  }

  /*
   * Get credit size and device frequency for the following calculations
   */
  res = soc_petra_mgmt_credit_worth_get_unsafe(
          unit,
          &credit_worth
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 2, exit);

  device_ticks_per_sec = soc_petra_chip_ticks_per_sec_get(unit);

  /*
   * Get Device Interface Max Credit Rate
   */
  SOC_PETRA_IMPLICIT_FLD_GET(*max_cr_rate_fld, credit_rate, 3, exit);

  if (0 == credit_rate)
  {
    rate_internal = 0;
  }
  else
  {
    res = soc_sand_clocks_to_kbits_per_sec(
            credit_rate,
            credit_worth,
            device_ticks_per_sec,
            &rate_internal
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 4, exit);

    if (fast_traversal)
    {
      rate_internal = rate_internal * SOC_PETRA_SCH_DEV_RATE_INTERVAL_RESOLUTION;
    }
  }

  *rate = rate_internal;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_sch_if_shaper_rate_get_unsafe()",0,0);
}

/*********************************************************************
*     Sets, for a specified device interface, (NIF-Ports,
*     recycling & CPU) its maximal credit rate. This API is
*     only valid for Channelized interfaces - see REMARKS
*     section below.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_sch_if_shaper_rate_set_unsafe(
    SOC_SAND_IN     int               unit,
    SOC_SAND_IN     SOC_PETRA_INTERFACE_ID      if_ndx,
    SOC_SAND_IN     uint32                rate,
    SOC_SAND_OUT    uint32                *exact_rate
  )
{
  uint32
    res,
    credit_rate,
    device_ticks_per_sec,
    rate_internal = rate,
    exact_rate_internal,
    credit_worth;
  uint32
    sch_nif_offset,
    mal_idx;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_REG_FIELD
    *max_cr_rate_fld;
  SOC_PETRA_INTERFACE_TYPE
    interface_type;
  uint8
    fast_traversal = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_SCH_IF_SHAPER_RATE_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(exact_rate);

  regs = soc_petra_regs();

  res = soc_pb_if_type_from_id(if_ndx,&interface_type);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  mal_idx = SOC_PB_IF2MAL_NDX(if_ndx);

  switch(interface_type)
  {
  case SOC_PETRA_IF_TYPE_NIF:
      if (!SOC_PB_NIF_IS_GMII_ONLY_ID(soc_pb_nif2intern_id(if_ndx)))
      {
        max_cr_rate_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.ch_nif_reg[mal_idx].ch_nifxx_max_cr_rate);
      }
      else
      {
        sch_nif_offset = soc_petra_sch_if2sched_offset(soc_pb_nif2intern_id(if_ndx));
        max_cr_rate_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.port_nif_reg[sch_nif_offset].port_nifxx_max_cr_rate);
        fast_traversal = TRUE;
      }
      break;
  case SOC_PETRA_IF_TYPE_CPU:
      max_cr_rate_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.cpu_rate1_reg.cpumax_cr_rate);
      break;
  case SOC_PETRA_IF_TYPE_RCY:
      max_cr_rate_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.rcy_rate1_reg.rcymax_cr_rate);
      break;
   case SOC_PETRA_IF_TYPE_ERP:
      max_cr_rate_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.erp_config_reg.erpmax_cr_rate);
      fast_traversal = TRUE;
      break;
  case SOC_PETRA_IF_TYPE_OLP:
      max_cr_rate_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.olp_config_reg.olpmax_cr_rate);
      fast_traversal = TRUE;
      break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_INVALID_IF_TYPE_ERR, 10, exit);
      break;
  }

  if (fast_traversal)
  {
    rate_internal = SOC_SAND_DIV_ROUND_UP(rate, SOC_PETRA_SCH_DEV_RATE_INTERVAL_RESOLUTION);
  }

  /*
   * Get credit size and device frequency for the following calculations
   */
  res = soc_petra_mgmt_credit_worth_get_unsafe(
            unit,
            &credit_worth
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 2, exit);

  device_ticks_per_sec = soc_petra_chip_ticks_per_sec_get(unit);

  /*
   * Get Device Interface Max Credit Rate
   */
  if (0 == rate_internal)
  {
    credit_rate = 0;
  }
  else
  {
    res = soc_sand_kbits_per_sec_to_clocks(
            rate_internal,
            credit_worth,
            device_ticks_per_sec,
            &credit_rate
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 3, exit);

    SOC_SAND_LIMIT_FROM_ABOVE(credit_rate, SOC_PETRA_FLD_MAX(*max_cr_rate_fld));

    if (fast_traversal)
    {
      SOC_SAND_LIMIT_FROM_BELOW(credit_rate, SOC_PETRA_SCH_DEV_RATE_INTERVAL_RESOLUTION);
    }
  }

  SOC_PETRA_IMPLICIT_FLD_SET(*max_cr_rate_fld, credit_rate, 4, exit);

  if (0 == credit_rate)
  {
    exact_rate_internal = 0;
  }
  else
  {
    res = soc_sand_clocks_to_kbits_per_sec(
            credit_rate,
            credit_worth,
            device_ticks_per_sec,
            &exact_rate_internal
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);

    if (fast_traversal)
    {
      exact_rate_internal = exact_rate_internal * SOC_PETRA_SCH_DEV_RATE_INTERVAL_RESOLUTION;
    }
  }
  *exact_rate = exact_rate_internal;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_sch_if_shaper_rate_set_unsafe()",if_ndx,0);
}

/*********************************************************************
*     Sets, for a specified device MAC LANE or equivalent, (NIF-Ports,
*     recycling & CPU) its actual credit rate (sum of ports). This API is
*     only valid for Channelized interface id-s (0, 4, 8... for NIF) - see REMARKS
*     section below.
*     Note: for SGMII - configures only the first interface in MAL
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_sch_mal_rate_get_unsafe(
    SOC_SAND_IN     int           unit,
    SOC_SAND_IN     uint32            mal_ndx,
    SOC_SAND_OUT    uint32            *rate
  )
{
   uint32
    res,
    rate_internal,
    credit_rate,
    device_ticks_per_sec,
    credit_worth;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_REG_FIELD
    *sum_of_ports_fld;
  SOC_PETRA_MAL_EQUIVALENT_TYPE
    mal_type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_SCH_MAL_RATE_GET_UNSAFE);

  regs = soc_petra_regs();

  mal_type = soc_pb_mal_type_from_id(mal_ndx);

  switch(mal_type)
  {
  case SOC_PETRA_MAL_TYPE_NIF:
    sum_of_ports_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.ch_nif_reg[mal_ndx].ch_nifxx_sum_of_ports);
      break;
  case SOC_PETRA_MAL_TYPE_CPU:
      sum_of_ports_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.cpu_rates0_reg.cpusum_of_ports);
      break;
  case SOC_PETRA_MAL_TYPE_RCY:
      sum_of_ports_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.rcy_rate0_reg.rcysum_of_ports);
      break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_INVALID_IF_TYPE_ERR, 10, exit);
      break;
  }

  /*
   * Get credit size and device frequency for the following calculations
   */
  res = soc_petra_mgmt_credit_worth_get_unsafe(
          unit,
          &credit_worth
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 2, exit);

  device_ticks_per_sec = soc_petra_chip_ticks_per_sec_get(unit);

  /*
   * Get Device Interface Max Credit Rate
   */
  SOC_PETRA_IMPLICIT_FLD_GET(*sum_of_ports_fld, credit_rate, 3, exit);

  if (0 == credit_rate)
  {
    rate_internal = 0;
  }
  else
  {
    res = soc_sand_clocks_to_kbits_per_sec(
            credit_rate,
            credit_worth,
            device_ticks_per_sec,
            &rate_internal
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 4, exit);
  }

  *rate = rate_internal * SOC_PETRA_SCH_DEV_RATE_INTERVAL_RESOLUTION;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_sch_mal_rate_get_unsafe()",0,0);
}

/*********************************************************************
*     Sets, for a specified device MAC LANE or equivalent, (NIF-Ports,
*     recycling & CPU) its actual credit rate (sum of ports). This API is
*     only valid for Channelized interfaces - see REMARKS
*     section below.
*     Note: for SGMII - configures only the first interface in MAL
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_sch_mal_rate_set_unsafe(
    SOC_SAND_IN     int               unit,
    SOC_SAND_IN     uint32                mal_ndx,
    SOC_SAND_IN     uint32                rate,
    SOC_SAND_OUT    uint32                *exact_rate
  )
{
  uint32
    res,
    rate_internal,
    exact_rate_internal,
    credit_rate,
    device_ticks_per_sec,
    credit_worth;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_REG_FIELD
    *sum_of_ports_fld;
  SOC_PETRA_MAL_EQUIVALENT_TYPE
    mal_type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_SCH_MAL_RATE_SET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(exact_rate);

  regs = soc_petra_regs();

  mal_type = soc_pb_mal_type_from_id(mal_ndx);

  rate_internal = SOC_SAND_DIV_ROUND_UP(rate, SOC_PETRA_SCH_DEV_RATE_INTERVAL_RESOLUTION);

  switch(mal_type)
  {
  case SOC_PETRA_MAL_TYPE_NIF:
    sum_of_ports_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.ch_nif_reg[mal_ndx].ch_nifxx_sum_of_ports);
      break;
  case SOC_PETRA_MAL_TYPE_CPU:
    /*
     * Although OLP is non channelized interface it is treated as such
     * because it shares the same calendar with the CPU.
     */
      sum_of_ports_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.cpu_rates0_reg.cpusum_of_ports);
      break;
  case SOC_PETRA_MAL_TYPE_RCY:
      sum_of_ports_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.rcy_rate0_reg.rcysum_of_ports);
      break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_INVALID_IF_TYPE_ERR, 10, exit);
      break;
  }

  /*
   * Get credit size and device frequency for the following calculations
   */
  res = soc_petra_mgmt_credit_worth_get_unsafe(
            unit,
            &credit_worth
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 2, exit);

  device_ticks_per_sec = soc_petra_chip_ticks_per_sec_get(unit);

  /*
   * Get Device Interface Max Credit Rate
   */
  if (0 == rate_internal)
  {
    credit_rate = 0;
  }
  else
  {
    res = soc_sand_kbits_per_sec_to_clocks(
            rate_internal,
            credit_worth,
            device_ticks_per_sec,
            &credit_rate
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 3, exit);

    SOC_SAND_LIMIT_FROM_ABOVE(credit_rate, SOC_PETRA_FLD_MAX(*sum_of_ports_fld));
    SOC_SAND_LIMIT_FROM_BELOW(credit_rate, SOC_PETRA_SCH_DEV_RATE_INTERVAL_RESOLUTION);
  }

  SOC_PETRA_IMPLICIT_FLD_SET(*sum_of_ports_fld, credit_rate, 4, exit);

  if (0 == credit_rate)
  {
    exact_rate_internal = 0;
  }
  else
  {
    res = soc_sand_clocks_to_kbits_per_sec(
            credit_rate,
            credit_worth,
            device_ticks_per_sec,
            &exact_rate_internal
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
  }

  *exact_rate = exact_rate_internal * SOC_PETRA_SCH_DEV_RATE_INTERVAL_RESOLUTION;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_sch_mal_rate_set_unsafe()",0,0);
}

/*********************************************************************
*     Sets, for a specified device interface, (NIF-Ports,
*     recycling, OLP, ERP) its weight index. Range: 0-7. The
*     actual weight value (one of 8, configurable) is in range
*     1-1023, 0 meaning inactive interface. This API is only
*     only valid for Channelized interface id-s (0, 4, 8... for NIF) - see REMARKS section
*     below.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_sch_device_if_weight_idx_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        if_ndx,
    SOC_SAND_OUT uint32                  *weight_index
  )
{
  uint32
    weight_idx_result,
    res;
  uint32
    nif_idx,
    sch_nif_offset,
    mal_idx;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_REG_FIELD
    *weight_idx_fld;
  SOC_PETRA_INTERFACE_TYPE
    interface_type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_SCH_DEVICE_IF_WEIGHT_GET_UNSAFE);
  SOC_SAND_CHECK_NULL_INPUT(weight_index);

  res = soc_petra_interface_id_verify(unit, if_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  regs = soc_petra_regs();

  res = soc_petra_if_type_from_id(unit, if_ndx, &interface_type);
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  switch(interface_type)
  {
  case SOC_PETRA_IF_TYPE_NIF:
      nif_idx = soc_pb_nif2intern_id(if_ndx);
      mal_idx = soc_petra_sch_if2sched_offset(nif_idx);
      if (!SOC_PB_NIF_IS_GMII_ONLY_ID(nif_idx))
      {
        weight_idx_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.ch_nif_cal_config_reg[mal_idx].ch_nifxx_weight);
      }
      else
      {
        sch_nif_offset = soc_petra_sch_if2sched_offset(nif_idx);
        weight_idx_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.port_nif_reg[sch_nif_offset].port_nifxx_weight);
      }
      break;
  case SOC_PETRA_IF_TYPE_CPU:
      weight_idx_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.cpu_cal_configuration_reg.cpuweight);
      break;
  case SOC_PETRA_IF_TYPE_RCY:
      weight_idx_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.rcy_cal_configuration_reg.rcyweight);
      break;
  case SOC_PETRA_IF_TYPE_OLP:
    weight_idx_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.olp_config_reg.olpweight);
      break;
  case SOC_PETRA_IF_TYPE_ERP:
      weight_idx_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.erp_config_reg.erpweight);
      break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_INVALID_IF_TYPE_ERR, 10, exit);
      break;
  }

  /* Weight index read{ */
  SOC_PETRA_IMPLICIT_FLD_GET(*weight_idx_fld, (weight_idx_result), 20, exit);
  /* Weight index read} */

  *weight_index = weight_idx_result;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_sch_device_if_weight_idx_get_unsafe()",0,0);
}

uint32
  soc_pb_sch_device_if_weight_idx_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INTERFACE_ID        if_ndx,
    SOC_SAND_IN  uint32                  weight_index
  )
{
  uint32
    res;
  uint32
    nif_idx,
    sch_nif_offset,
    mal_idx;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_REG_FIELD
    *weight_idx_fld;
  SOC_PETRA_INTERFACE_TYPE
    interface_type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_SCH_DEVICE_IF_WEIGHT_SET_UNSAFE);

  regs = soc_petra_regs();

  res = soc_petra_if_type_from_id(unit, if_ndx, &interface_type);
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  switch(interface_type)
  {
  case SOC_PETRA_IF_TYPE_NIF:
      nif_idx = soc_pb_nif2intern_id(if_ndx);
      sch_nif_offset = soc_petra_sch_if2sched_offset(nif_idx);
      mal_idx = SOC_PB_NIF2MAL_GLBL_ID(nif_idx);
      if (!SOC_PB_NIF_IS_GMII_ONLY_ID(nif_idx))
      {
        weight_idx_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.ch_nif_cal_config_reg[mal_idx].ch_nifxx_weight);
      }
      else
      {
        weight_idx_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.port_nif_reg[sch_nif_offset].port_nifxx_weight);
      }
      break;
  case SOC_PETRA_IF_TYPE_CPU:
      weight_idx_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.cpu_cal_configuration_reg.cpuweight);
      break;
  case SOC_PETRA_IF_TYPE_RCY:
      weight_idx_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.rcy_cal_configuration_reg.rcyweight);
      break;
  case SOC_PETRA_IF_TYPE_OLP:
    weight_idx_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.olp_config_reg.olpweight);
      break;
  case SOC_PETRA_IF_TYPE_ERP:
      weight_idx_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.erp_config_reg.erpweight);
      break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_SCH_INVALID_IF_TYPE_ERR, 10, exit);
      break;
  }

  /* Weight index write{ */
  SOC_PETRA_IMPLICIT_FLD_SET(*weight_idx_fld, weight_index, 20, exit);
  /* Weight index write} */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_sch_device_if_weight_set_unsafe()",0,0);
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>
