/* $Id: petra_ofp_rates.c,v 1.13 Broadcom SDK $
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

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_ofp_rates.h>
#include <soc/dpp/Petra/petra_scheduler_device.h>
#include <soc/dpp/Petra/petra_nif.h>
#include <soc/dpp/Petra/petra_ports.h>
#include <soc/dpp/Petra/petra_general.h>
#include <soc/dpp/Petra/petra_sw_db.h>
#include <soc/dpp/Petra/petra_reg_access.h>
#include <soc/dpp/Petra/petra_tbl_access.h>
#include <soc/dpp/Petra/petra_general.h>

#include <soc/dpp/SAND/Utils/sand_u64.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/SAND/Utils/sand_conv.h>

#ifdef LINK_PB_LIBRARIES
  #include <soc/dpp/Petra/PB_TM/pb_ofp_rates.h>
#endif

/* } */

/*************
 * DEFINES   *
 *************/
/* { */
#define SOC_PETRA_OFP_RATES_OLP_GUARANTEED_KBPS             (1 * 1024 * 1024)

/* Initial rate deviation for calendar-building algorithm. */
#define SOC_PETRA_OFP_RATES_CAL_RATE_DEVIATION              (100000000)

/* Number of clocks to traverse a single calendar slot in the EGQ */
#define SOC_PETRA_EGQ_CAL_SLOT_TRAVERSE_IN_CLOCKS           (2)

/* EGQ bandwidth The unit is 1/256th byte */
#define SOC_PETRA_EGQ_UNITS_VAL_IN_BITS                     (256 / 8)

/* Indication of an uninitialized entry */
#define SOC_PETRA_OFP_RATES_ILLEGAL_SCHEDULER_ID            (0xfff)

/* The initial rate given to all port before random test */
#define SOC_PETRA_OFP_RATES_TEST_RANDOM_INITIAL_PORT_RATE   (1000000)

/* */
#define SOC_PETRA_OFP_RATES_INVALID_PORT_ID                 (SOC_PETRA_NOF_FAP_PORTS)

/* */
#define SOC_PETRA_OFP_RATES_NOF_ITERATIONS                  (2)

/* $Id: petra_ofp_rates.c,v 1.13 Broadcom SDK $
 *	Maximal allowed rate difference between requested
 *  and actual, in the rate-configuration test
 */
#define SOC_PETRA_OFP_RATES_TST_MAX_SCH_DELTA_PERCENT       (5)
#define SOC_PETRA_OFP_RATES_TST_MAX_EGQ_DELTA_PERCENT       (5)

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

static uint8
  Soc_petra_ofp_rates_test_random_sch_enable = FALSE;
static uint8
  Soc_petra_ofp_rates_test_random_egq_enable = TRUE;

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

STATIC
  void
    soc_petra_ofp_rates_update_dev_changed_set(
      SOC_SAND_IN  int   unit,
      SOC_SAND_IN  uint8   is_update_dev_changed
    )
{
  soc_petra_sw_db_ofp_rates_update_dev_changed_set(unit, is_update_dev_changed);
}

STATIC uint32
  soc_pa_ofp_rates_init(
    SOC_SAND_IN  int                    unit
  )
{
  uint32
    fld_val,
    exact,
    res = SOC_SAND_OK;
  SOC_PETRA_REGS
    *regs;
  uint8
    pp_enable;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_OFP_RATES_INIT);

  regs = soc_petra_regs();

  fld_val = 0x1;
  SOC_PETRA_FLD_SET(regs->egq.egress_shaper_enable_settings_reg.sch_enable, fld_val, 10, exit);

  res = soc_petra_ofp_rates_update_device_set_unsafe(
          unit,
          TRUE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  soc_petra_ofp_rates_update_dev_changed_set(unit, FALSE);

  /*
   *	If PP is enabled, reserve some rate for the OLP.
   *  The OLP shares a calendar with the EGQ.
   *  This is an additional credit source for the OLP
   *  that guarantees the OLP is not under starvation
   *  even if the CPU is blocked (e.g. Flow Control).
   */
  pp_enable = soc_petra_sw_db_pp_enable_get(unit);
  if (pp_enable)
  {
    res = soc_petra_sch_if_shaper_rate_set_unsafe(
            unit,
            SOC_PETRA_IF_ID_OLP,
            SOC_PETRA_OFP_RATES_OLP_GUARANTEED_KBPS,
            &exact
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_ofp_rates_init()",0,0);
}

uint32
  soc_petra_ofp_rates_init(
    SOC_SAND_IN  int                    unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_RATES_INIT);

  SOC_PETRA_DIFF_DEVICE_CALL(ofp_rates_init,(unit));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_init()",0,0);
}

STATIC uint32
  soc_petra_ofp_rates_ccmmal2mal(
    SOC_SAND_IN  SOC_PETRA_OFP_RATES_EGQ_MAL_ID   ccm_ndx
  )
{
  switch(ccm_ndx)
  {
  case SOC_PETRA_OFP_RATES_EGQ_MAL_00:
  case SOC_PETRA_OFP_RATES_EGQ_MAL_01:
  case SOC_PETRA_OFP_RATES_EGQ_MAL_02:
  case SOC_PETRA_OFP_RATES_EGQ_MAL_03:
  case SOC_PETRA_OFP_RATES_EGQ_MAL_04:
  case SOC_PETRA_OFP_RATES_EGQ_MAL_05:
  case SOC_PETRA_OFP_RATES_EGQ_MAL_06:
  case SOC_PETRA_OFP_RATES_EGQ_MAL_07:
    return ccm_ndx;
  case SOC_PETRA_OFP_RATES_EGQ_MAL_CPU:
    return SOC_PETRA_MAL_ID_CPU;
  case SOC_PETRA_OFP_RATES_EGQ_MAL_RCY:
    return SOC_PETRA_MAL_ID_RCY;
  default:
    return SOC_PETRA_OFP_RATES_EGQ_MAL_INVALID;
  }
}

STATIC SOC_PETRA_OFP_RATES_EGQ_MAL_ID
  soc_petra_ofp_rates_mal2ccmmal(
    SOC_SAND_IN  uint32                    mal_ndx
  )
{
  switch(mal_ndx)
  {
  case SOC_PETRA_MAL_ID_CPU:
  case SOC_PETRA_MAL_ID_OLP:
    return SOC_PETRA_OFP_RATES_EGQ_MAL_CPU;
  case SOC_PETRA_MAL_ID_RCY:
    return SOC_PETRA_OFP_RATES_EGQ_MAL_RCY;
  default:
    return mal_ndx;
  }
}

STATIC uint8
  soc_petra_ofp_rates_is_cal_entry_dummy(
    SOC_SAND_IN  SOC_PETRA_OFP_EGQ_RATES_CAL_ENTRY  *cal_entry
  )
{
  SOC_PETRA_OFP_EGQ_RATES_CAL_ENTRY
    dummy_entry;

  dummy_entry.port_idx = SOC_PETRA_OFP_RATES_INVALID_PORT_ID;
  dummy_entry.credit = 0;

  if (cal_entry)
  {
    return SOC_SAND_NUM2BOOL(0 == sal_memcmp(cal_entry, &dummy_entry, sizeof(SOC_PETRA_OFP_EGQ_RATES_CAL_ENTRY)));
  }

  return FALSE;
}

STATIC uint32
  soc_petra_ofp_rates_mal_egq_from_mal_id(
    uint32                             mal_ndx
  )
{
  uint32
    egq_mal_id = 0,
    mal_type;

  mal_type = soc_petra_mal_type_from_id(mal_ndx);

  switch(mal_type) {
    case SOC_PETRA_MAL_TYPE_NIF:
      egq_mal_id = mal_ndx;
      break;
    case SOC_PETRA_MAL_TYPE_CPU:
    case SOC_PETRA_MAL_TYPE_OLP:
      egq_mal_id = SOC_PETRA_OFP_RATES_EGQ_MAL_CPU;
      break;
    case SOC_PETRA_MAL_TYPE_RCY:
      egq_mal_id = SOC_PETRA_OFP_RATES_EGQ_MAL_RCY;
      break;
    default:
      egq_mal_id = SOC_PETRA_OFP_RATES_EGQ_MAL_INVALID;
      break;
    }

  return egq_mal_id;
}

STATIC uint32
  soc_petra_ofp_rates_special_id_detect(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    mal_ndx,
    SOC_SAND_IN  uint32                    ofp_ndx,
    SOC_SAND_OUT uint8                    *is_channelized_id,
    SOC_SAND_OUT uint8                    *is_sgmii_id,
    SOC_SAND_OUT SOC_PETRA_INTERFACE_ID           *interface_id
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_MAL_EQUIVALENT_TYPE
    mal_type;
  SOC_PETRA_INTERFACE_ID
    if_id;
  SOC_PETRA_NIF_TYPE
    nif_type;
  SOC_PETRA_PORT2IF_MAPPING_INFO
    mapping,
    in_mapping;
  uint8
    is_channelized_nif = FALSE,
    is_nonch = FALSE,
    is_sgmii = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_RATES_SPECIAL_ID_DETECT);
  SOC_SAND_CHECK_NULL_INPUT(is_channelized_id);
  SOC_SAND_CHECK_NULL_INPUT(is_sgmii_id);

  soc_petra_PETRA_PORT2IF_MAPPING_INFO_clear(&mapping);
  soc_petra_PETRA_PORT2IF_MAPPING_INFO_clear(&in_mapping);

  mal_type = soc_petra_mal_type_from_id(mal_ndx);
  if_id = SOC_PETRA_MAL2IF_NDX(mal_ndx);

  if (SOC_PETRA_IS_MAL_EQUIVALENT_ID(mal_ndx))
  {
    nif_type = SOC_PETRA_NIF_TYPE_NONE;
    /*
     * Although OLP is non channelized interface it is treated as such
     * because it shares the same calendar with the CPU.
     */
    is_nonch = SOC_SAND_NUM2BOOL_INVERSE((mal_type != SOC_PETRA_MAL_TYPE_ERP));
  }
  else
  {
    res = soc_petra_nif_type_get_unsafe(
            unit,
            mal_ndx,
            &nif_type,
            &is_channelized_nif
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    is_nonch = !(soc_petra_is_channelized_interface_id(if_id));
  }

  /* Scheduler */
  if (nif_type == SOC_PETRA_NIF_TYPE_SGMII)
  {
    /*
     *  For SGMII, the calendar is configured only for the port
     *  that is mapped to an interface with channelized index (0, 4, 8...).
     *  For other ports - the configuration is straightforward.
     */
    res = soc_petra_port_to_interface_map_get_unsafe(
            unit,
            ofp_ndx,
            &in_mapping,
            &mapping
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    if_id = mapping.if_id;
    is_nonch = !(soc_petra_is_channelized_interface_id(if_id));
    is_sgmii = TRUE;
  }

  *is_channelized_id = !is_nonch;
  *is_sgmii_id = is_sgmii;
  *interface_id = if_id;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_special_id_detect()",0,0);
}

STATIC uint32
  soc_petra_ofp_rates_cal_per_len_build(
    SOC_SAND_IN   int                   unit,
    SOC_SAND_IN   uint32                    *ports_rates,
    SOC_SAND_IN   uint32                   nof_ports,
    SOC_SAND_IN   uint32                    total_credit_bandwidth,
    SOC_SAND_IN   uint32                    max_calendar_len,
    SOC_SAND_IN   uint32                    tentative_len,
    SOC_SAND_OUT  uint32                    *actual_len,
    SOC_SAND_OUT  uint32                   *port_nof_slots,
    SOC_SAND_OUT  uint32                    *deviation
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    port,
    slots_rates[SOC_PETRA_NOF_FAP_PORTS],
    total_num_slots = 0,
    calc_deviation;
  uint32
    rem;
  uint32
    num_slots,
    temp1,
    temp2;
  SOC_SAND_U64
    u64_1,
    u64_2;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_RATES_CAL_PER_LEN_BUILD);

  SOC_SAND_CHECK_NULL_INPUT(actual_len);
  SOC_SAND_CHECK_NULL_INPUT(deviation);

  if (0 == tentative_len)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_OFP_RATES_CAL_LEN_INVALID_ERR, 2, exit);
  }

  SOC_PETRA_CLEAR(slots_rates, uint32, SOC_PETRA_NOF_FAP_PORTS);

  /*
   * Calculate the number of calendar slots per port
   */
  for (port = 0; port < nof_ports; ++port)
  {
    if (ports_rates[port] > 0)
    {
      /*
       *  Calculate number of slots for each port according to:
       *
       *                          port_credit_rate * cal_num_of_slots
       *  port_num_of_cal_slots = ------------------------------------
       *                               total_credit_bandwidth
       *
       *
       */

      soc_sand_u64_multiply_longs(ports_rates[port], tentative_len, &u64_1);
      rem = soc_sand_u64_devide_u64_long(&u64_1, total_credit_bandwidth, &u64_2);
      soc_sand_u64_to_long(&u64_2, &(num_slots));
      num_slots = (rem > 0 ? num_slots + 1 : num_slots);

      slots_rates[port] = num_slots;
      total_num_slots += num_slots;
    }
  }
  if (total_num_slots > max_calendar_len)
  {
    /*
     * This solution is not acceptable, so return zero
     */
    *actual_len = 0;
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_OK, 3, exit);
  }

  if (total_credit_bandwidth == 0 || total_num_slots == 0)
  {
    /*
     * This solution is not acceptable, so return zero
     */
    *actual_len = 0;
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_OK, 3, exit);
  }

  calc_deviation = 0;
  for (port = 0; port < nof_ports; ++port)
  {
    soc_sand_u64_multiply_longs(SOC_PETRA_OFP_RATES_CAL_RATE_DEVIATION, ports_rates[port], &u64_1);
    rem = soc_sand_u64_devide_u64_long(&u64_1, total_credit_bandwidth, &u64_2);
    soc_sand_u64_to_long(&u64_2, &(temp1));
    temp1 = (rem > 0 ? temp1 + 1 : temp1);

    soc_sand_u64_multiply_longs(SOC_PETRA_OFP_RATES_CAL_RATE_DEVIATION, slots_rates[port], &u64_1);
    rem = soc_sand_u64_devide_u64_long(&u64_1, total_num_slots, &u64_2);
    soc_sand_u64_to_long(&u64_2, &(temp2));
    temp2 = (rem > 0 ? temp2 + 1 : temp2);

    calc_deviation += soc_sand_abs(temp2 - temp1);
  }

  *actual_len = total_num_slots;
  SOC_PETRA_COPY(port_nof_slots, slots_rates, uint32, SOC_PETRA_NOF_FAP_PORTS);
  *deviation = calc_deviation;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_cal_per_len_build()",0,0);
}

STATIC uint32
  soc_petra_ofp_rates_cal_len_calculate(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                    *ports_rates,
    SOC_SAND_IN  uint32                   nof_ports,
    SOC_SAND_IN  uint32                    total_credit_bandwidth,
    SOC_SAND_IN  uint32                    max_calendar_len,
    SOC_SAND_OUT uint32                    *actual_calendar_len,
    SOC_SAND_OUT uint32                   *port_nof_slots
  )
{
  uint32
    res,
    deviation,
    min_deviation,
    tentative_len,
    best_tentative_len,
    actual_len;
  uint32
    port_num_slots[SOC_PETRA_NOF_FAP_PORTS];
  uint32
    min_port_rate = 0x1;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_RATES_CAL_LEN_CALCULATE);

  /*
   * Determine calendar len.
   * A possible algorithm is to go over all possible length values
   * which ranges from nof_active_ports (1 slot per port) to max_calendar_len.
   * For each value, calculate a solution and its deviation from the required
   * rates. Then pick the most accurate solution, which is the one with the smallest
   * deviation. The algorithm could also be stopped when the deviation is smaller than
   * a pre configured value.
   */
  min_deviation = SOC_PETRA_OFP_RATES_CAL_RATE_DEVIATION;
  best_tentative_len = 0;
  for (tentative_len = 1; tentative_len <= max_calendar_len; ++tentative_len)
  {
    res = soc_petra_ofp_rates_cal_per_len_build(
            unit,
            ports_rates,
            nof_ports,
            total_credit_bandwidth,
            max_calendar_len,
            tentative_len,
            &actual_len,
            port_num_slots,
            &deviation
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    /*
     * Check if we received a legal solution for this tentative length
     */
    if (actual_len == 0)
    {
      continue;
    }
    if (deviation < min_deviation)
    {
      min_deviation = deviation;
      best_tentative_len = tentative_len;
      if (0 == min_deviation)
      {
        break;
      }
    }
  }
  /*
   * sanity check
   */
  if (0 == best_tentative_len)
  {
    best_tentative_len = 1;

    /*
     *  Minimal calendar
     */
    res = soc_petra_ofp_rates_cal_per_len_build(
          unit,
          &min_port_rate,
          1,
          min_port_rate,
          max_calendar_len,
          min_port_rate,
          &actual_len,
          port_num_slots,
          &deviation
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);
  }
  else
  {
    /*
     * Rebuild the best calendar that we found
     */
    res = soc_petra_ofp_rates_cal_per_len_build(
            unit,
            ports_rates,
            nof_ports,
            total_credit_bandwidth,
            max_calendar_len,
            best_tentative_len,
            &actual_len,
            port_num_slots,
            &deviation
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

  *actual_calendar_len = actual_len;
  SOC_PETRA_COPY(port_nof_slots, port_num_slots, uint32, SOC_PETRA_NOF_FAP_PORTS);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_cal_len_calculate()",0,0);
}

STATIC uint32
  soc_petra_ofp_rates_fixed_len_cal_build(
    SOC_SAND_IN  int                   unit,
    SOC_SAND_IN  uint32                   *port_nof_slots,
    SOC_SAND_IN  uint32                   nof_ports,
    SOC_SAND_IN  uint32                    calendar_len,
    SOC_SAND_IN  uint32                    max_calendar_len,
    SOC_SAND_OUT uint32                   *calendar
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    slot_idx,
    port_idx,
    nof_diff_ports,
    port_num_slots[SOC_PETRA_NOF_FAP_PORTS],
    alloc_slots,
    rem_cal_len,
    max_port_idx,
    hop_size,
    port_alloc_slots,
    free_slot_cnt,
    leftovers[SOC_PETRA_NOF_FAP_PORTS];

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_RATES_FIXED_LEN_CAL_BUILD);

  /*
   *  Verify the input's integrity
   */
  alloc_slots = 0;
  for (port_idx = 0; port_idx < nof_ports; ++port_idx)
  {
    alloc_slots += port_nof_slots[port_idx];
  }
  if (alloc_slots != calendar_len)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_OFP_RATES_CAL_ALLOCATED_AND_REQUESTED_LEN_MISMATCH_ERR, 10, exit);
  }

  /*
   * Clear the calendar
   */
  for (slot_idx = 0; slot_idx < max_calendar_len; ++slot_idx)
  {
    calendar[slot_idx] = SOC_PETRA_OFP_RATES_ILLEGAL_SCHEDULER_ID;
  }

  /*
   *  Count the number of distinct ports in the calendar and initialize the leftovers array
   */
  nof_diff_ports = 0;
  for (port_idx = 0; port_idx < nof_ports; ++port_idx)
  {
    if (port_nof_slots[port_idx] > 0)
    {
      ++nof_diff_ports;
    }
    leftovers[port_idx] = 0;
  }

  SOC_PETRA_COPY(port_num_slots, port_nof_slots, uint32, SOC_PETRA_NOF_FAP_PORTS);

  /*
   *  First pass: for each port, try to allocate as many evenly-spaced slots as possible
   */
  alloc_slots = 0;
  rem_cal_len = calendar_len;
  max_port_idx = soc_sand_get_index_of_max_member_in_array(
                   port_num_slots,
                   nof_ports
                 );
  while (port_num_slots[max_port_idx] > 0)
  {
    hop_size = SOC_SAND_MIN(rem_cal_len / port_num_slots[max_port_idx], nof_diff_ports);
    port_alloc_slots = 0;
    free_slot_cnt = hop_size;
    for (slot_idx = 0; slot_idx < calendar_len && port_alloc_slots < port_nof_slots[max_port_idx]; ++slot_idx)
    {
      if (calendar[slot_idx] == SOC_PETRA_OFP_RATES_ILLEGAL_SCHEDULER_ID)
      {
        if (free_slot_cnt < hop_size)
        {
          ++free_slot_cnt;
        }
        else
        {
          calendar[slot_idx] = max_port_idx;
          ++alloc_slots;
          ++port_alloc_slots;
          free_slot_cnt = 0;
        }
      }
    }

    port_num_slots[max_port_idx] = 0;
    rem_cal_len -= port_alloc_slots;

    /*
     *  Record the number of unallocated slots for the second pass
     */
    leftovers[max_port_idx] = port_nof_slots[max_port_idx] - port_alloc_slots;

    max_port_idx = soc_sand_get_index_of_max_member_in_array(
                     port_num_slots,
                     nof_ports
                   );
  }

  /*
   *  Second pass: fill the holes in the calendar with the remaining slots in a round-robin
   *  fashion
   */
  port_idx = 0;
  free_slot_cnt = nof_diff_ports;
  for (slot_idx = 0; alloc_slots < calendar_len; slot_idx = (slot_idx + 1) % calendar_len)
  {
    if (calendar[slot_idx] == SOC_PETRA_OFP_RATES_ILLEGAL_SCHEDULER_ID)
    {
      if (free_slot_cnt == nof_diff_ports)
      {
        for ( ; leftovers[port_idx] == 0; port_idx = (port_idx + 1) % nof_ports);
        calendar[slot_idx] = port_idx;
        --leftovers[port_idx];
        port_idx = (port_idx + 1) % nof_ports;
        ++alloc_slots;
        free_slot_cnt = 0;
      }
      else
      {
        ++free_slot_cnt;
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_petra_ofp_rates_fixed_len_cal_build()", 0, 0);
}

STATIC uint32
  soc_petra_ofp_rates_active_calendars_retrieve_sch(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    mal_ndx,
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_CAL_SCH      *sch_cal,
    SOC_SAND_OUT uint32                     *sch_cal_len
  )
{
  uint32
    res,
    fld_val = 0,
    sch_len = 0;
  uint32
    offset = 0,
    slot = 0;
  SOC_PETRA_SCH_CAL_TBL_DATA
    sch_data;
  uint32
    sch_to_get;
  SOC_PETRA_REG_FIELD
    *sch_cal_len_fld,
    *sch_cal_sel_fld;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_MAL_EQUIVALENT_TYPE
    mal_type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_RATES_ACTIVE_CALENDARS_RETRIEVE);


  SOC_SAND_CHECK_NULL_INPUT(sch_cal);

  regs = soc_petra_regs();

  *sch_cal_len = 0;

  mal_type = soc_petra_mal_type_from_id(mal_ndx);

  switch(mal_type) {
  case SOC_PETRA_MAL_TYPE_NIF:
    SOC_SAND_ERR_IF_ABOVE_NOF(mal_ndx, SOC_PETRA_MAL_TYPE_NOF_TYPES, SOC_PETRA_OFP_RATES_MAL_TYPE_INVALID_ERR, 10, exit);
    sch_cal_sel_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.ch_nif_cal_config_reg[mal_ndx].dvscalendar_sel_ch_nifxx);
    break;
  case SOC_PETRA_MAL_TYPE_CPU:
  case SOC_PETRA_MAL_TYPE_OLP:
    sch_cal_sel_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.cpu_cal_configuration_reg.dvscalendar_sel_cpu);
    break;
  case SOC_PETRA_MAL_TYPE_RCY:
    sch_cal_sel_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.rcy_cal_configuration_reg.dvscalendar_sel_rcy);
    break;
  case SOC_PETRA_MAL_TYPE_ERP:
    SOC_PETRA_DO_NOTHING_AND_EXIT;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_OFP_RATES_MAL_TYPE_INVALID_ERR, 45, exit);
  }

  /*
   * Check which calendars (EGQ & SCH - Calendars get 'A' or 'B')
   * are currently active.
   */
  SOC_PETRA_IMPLICIT_FLD_GET(*sch_cal_sel_fld, fld_val, 40, exit);
  sch_to_get = fld_val;

  switch(mal_type) {
  case SOC_PETRA_MAL_TYPE_NIF:
    sch_cal_len_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.ch_nif_cal_config_reg[mal_ndx].cal_len[sch_to_get]);
    sch_cal_sel_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.ch_nif_cal_config_reg[mal_ndx].dvscalendar_sel_ch_nifxx);
    break;
  case SOC_PETRA_MAL_TYPE_CPU:
  case SOC_PETRA_MAL_TYPE_OLP:
    sch_cal_len_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.cpu_cal_configuration_reg.cal_len[sch_to_get]);
    sch_cal_sel_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.cpu_cal_configuration_reg.dvscalendar_sel_cpu);
    break;
  case SOC_PETRA_MAL_TYPE_RCY:
    sch_cal_len_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.rcy_cal_configuration_reg.cal_len[sch_to_get]);
    sch_cal_sel_fld = SOC_PETRA_REG_DB_ACC_REF(regs->sch.rcy_cal_configuration_reg.dvscalendar_sel_rcy);
    break;
  case SOC_PETRA_MAL_TYPE_ERP:
    SOC_PETRA_DO_NOTHING_AND_EXIT;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_OFP_RATES_MAL_TYPE_INVALID_ERR, 45, exit);
  }

  /*
   *  Calendar Length
   */
  SOC_PETRA_IMPLICIT_FLD_GET(*sch_cal_len_fld, sch_len, 60, exit);

  /*
   * Read the Active EGQ calendar indirectly
   */

  /*
   * Read the -Active SCH calendar indirectly
   */

  /*
   *  All scheduler calendars share the same table.
   *  The order is:
   *  MAL0-CALA
   *  MAL0-CALB
   *  MAL1-CALA
   *  MAL1-CALB
   *  ...
   *  CPU-CALA
   *  CPU-CALB
   *  RCY-CALA
   *  RCY-CALB
   */
  switch(mal_type) {
  case SOC_PETRA_MAL_TYPE_NIF:
    offset = (mal_ndx*SOC_PETRA_OFP_NOF_RATES_CAL_SETS + sch_to_get) * SOC_PETRA_OFP_RATES_CAL_LEN_SCH_OFFSET;
    break;
  case SOC_PETRA_MAL_TYPE_CPU:
  case SOC_PETRA_MAL_TYPE_OLP:
    offset = SOC_PETRA_OFP_RATES_CAL_CPU_BASE_SCH_OFFSET + sch_to_get * SOC_PETRA_OFP_RATES_CAL_LEN_SCH_OFFSET;
    break;
  case SOC_PETRA_MAL_TYPE_RCY:
    offset = SOC_PETRA_OFP_RATES_CAL_RCY_BASE_SCH_OFFSET + sch_to_get * SOC_PETRA_OFP_RATES_CAL_LEN_SCH_OFFSET;
    break;
  /* must have default. Otherwise, compilation error */
  /* coverity[dead_error_begin : FALSE] */
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_OFP_RATES_MAL_TYPE_INVALID_ERR, 80, exit);
  }

  for (slot = 0; slot < sch_len + 1; ++slot)
  {
    res = soc_petra_sch_cal_tbl_get_unsafe(
            unit,
            offset + slot,
            &sch_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

    sch_cal->slots[slot] = sch_data.hrsel;
  }

  /*
   * The device calendar length is the actual val minus 1
   */
  *sch_cal_len = sch_len + 1;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_active_calendars_retrieve_sch()",0,0);
}

STATIC uint32
  soc_petra_ofp_rates_from_sch_ports_rates_to_calendar(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    mal_ndx,
    SOC_SAND_IN  uint8                    recalc,
    SOC_SAND_IN  uint32                     *ports_rates,
    SOC_SAND_IN  uint32                    nof_ports,
    SOC_SAND_IN  uint32                     total_credit_bandwidth,
    SOC_SAND_IN  uint32                     max_calendar_len,
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_CAL_SCH      *calendar,
    SOC_SAND_OUT uint32                     *calendar_len
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    port_nof_slots[SOC_PETRA_NOF_FAP_PORTS];

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_RATES_FROM_SCH_PORT_RATES_TO_CALENDAR);

  /*
   * Calculate the optimal calendar length and the
   * corresponding weight (in slots) of each port
   */
  if (recalc)
  {
    res = soc_petra_ofp_rates_cal_len_calculate(
            unit,
            ports_rates,
            nof_ports,
            total_credit_bandwidth,
            max_calendar_len,
            calendar_len,
            port_nof_slots
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    /*
     * Given the optimal calendar length and the
     * corresponding weight (in slots) of each port,
     * build a calendar that will avoid burstiness
     * behavior as much as possible.
     */
    res = soc_petra_ofp_rates_fixed_len_cal_build(
            unit,
            port_nof_slots,
            nof_ports,
            *calendar_len,
            max_calendar_len,
            calendar->slots
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  else
  {
    res = soc_petra_ofp_rates_active_calendars_retrieve_sch(
            unit,
            mal_ndx,
            calendar,
            calendar_len
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_from_sch_ports_rates_to_calendar()",0,0);
}

STATIC uint32
  soc_petra_ofp_rates_from_rates_to_calendar(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                     *ports_rates,
    SOC_SAND_IN  uint32                    nof_ports,
    SOC_SAND_IN  uint32                     total_credit_bandwidth,
    SOC_SAND_IN  uint32                     max_calendar_len,
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_CAL_SCH      *calendar,
    SOC_SAND_OUT uint32                     *calendar_len
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    port_nof_slots[SOC_PETRA_NOF_FAP_PORTS];

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_RATES_FROM_SCH_PORT_RATES_TO_CALENDAR);


  res = soc_petra_ofp_rates_cal_len_calculate(
          unit,
          ports_rates,
          nof_ports,
          total_credit_bandwidth,
          max_calendar_len,
          calendar_len,
          port_nof_slots
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   * Given the optimal calendar length and the
   * corresponding weight (in slots) of each port,
   * build a calendar that will avoid burstiness
   * behavior as much as possible.
   */
  res = soc_petra_ofp_rates_fixed_len_cal_build(
          unit,
          port_nof_slots,
          nof_ports,
          *calendar_len,
          max_calendar_len,
          calendar->slots
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_from_sch_ports_rates_to_calendar()",0,0);
}

STATIC uint32
  soc_petra_ofp_rates_fill_shaper_calendar_credits(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    mal_ndx,
    SOC_SAND_IN  uint32                     *ports_rates,
    SOC_SAND_IN  uint32                    nof_ports,
    SOC_SAND_IN  uint32                     calendar_len,
    SOC_SAND_IN  uint32                    *slots_ports,
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_CAL_EGQ      *calendar
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    slot_idx = 0,
    port_idx = 0,
    port_in_slot;
  uint32
    port_num_slots[SOC_PETRA_NOF_FAP_PORTS],
    port_credits[SOC_PETRA_NOF_FAP_PORTS];
  SOC_SAND_U64
    u64_1,
    u64_2;
  uint32
    rem;
  uint32
    calcal_length,
    calcal_instances;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_RATES_FILL_SHAPER_CALENDAR_CREDITS);


  SOC_PETRA_CLEAR(port_num_slots, uint32, SOC_PETRA_NOF_FAP_PORTS);
  SOC_PETRA_CLEAR(port_credits, uint32, SOC_PETRA_NOF_FAP_PORTS);

  /*
   * Fill the calendar slots with the ports_rates information
   */
  for (slot_idx = 0; slot_idx < calendar_len; ++slot_idx)
  {
    port_in_slot = slots_ports[slot_idx];
    calendar->slots[slot_idx].port_idx = port_in_slot;

    if (port_in_slot == SOC_PETRA_OFP_RATES_INVALID_PORT_ID)
    {
      continue;
    }

    if (port_in_slot == SOC_PETRA_OFP_RATES_ILLEGAL_SCHEDULER_ID)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_OFP_RATES_OFP_INDEX_MISMATCH_ERR, 12, exit);
    }

    ++(port_num_slots[port_in_slot]);
  }

 /*
  * Port Credits:
  *
  *                                 port_credits_num * 8/256 [bits] * core_frequency [kilo 1/sec]
  *  port_egq_rate [kbits/sec]= ----------------------------------------------------------------
  *                                           calendar_length * slot_traverse_time
  *
  * And Therefore:
  *
  *                         port_egq_rate [kbits/sec] * calendar_length * slot_traverse_time
  *  port_credits_num   = ---------------------------------------------------------------------
  *                          8/256 [bits] * core_frequency [kilo 1/sec]
  * And Therefore:
  *
  *                         256/8 [1/bits] * port_egq_rate [kbits/sec] * calendar_length * slot_traverse_time
  *  port_credits_num   = ---------------------------------------------------------------------
  *                           core_frequency [kilo 1/sec]
  */

  calcal_length = soc_petra_sw_db_ofp_rates_calcal_length_get(unit);
  calcal_instances = soc_petra_sw_db_ofp_rates_nof_instances_get(unit, mal_ndx);

  for(port_idx = 0; port_idx < SOC_PETRA_NOF_FAP_PORTS; ++port_idx)
  {
    if(!ports_rates[port_idx])
    {
      continue;
    }

    if(port_num_slots[port_idx] == 0)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 19, exit);
    }

    soc_sand_u64_multiply_longs(ports_rates[port_idx], calcal_length * (calendar_len + 1) * SOC_PETRA_EGQ_CAL_SLOT_TRAVERSE_IN_CLOCKS * SOC_PETRA_EGQ_UNITS_VAL_IN_BITS, &u64_1);
    rem = soc_sand_u64_devide_u64_long(&u64_1, calcal_instances * soc_petra_chip_kilo_ticks_per_sec_get(unit), &u64_2);
    soc_sand_u64_to_long(&u64_2, &(port_credits[port_idx]));
    port_credits[port_idx] = (rem > 0 ? port_credits[port_idx] + 1 : port_credits[port_idx]);
  }

  for (slot_idx = 0; slot_idx < calendar_len; ++slot_idx)
  {
    port_idx = slots_ports[slot_idx];
    if (port_idx == SOC_PETRA_OFP_RATES_INVALID_PORT_ID)
    {
      calendar->slots[slot_idx].credit = 0;
    }
    else if(port_num_slots[port_idx] == 0)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_GEN_ERR, 20, exit);
    }
    else if(port_num_slots[port_idx] == 1)
    {
      calendar->slots[slot_idx].credit = port_credits[port_idx];
      --port_num_slots[port_idx];
    }
    else
    {
      calendar->slots[slot_idx].credit = port_credits[port_idx] / port_num_slots[port_idx];

      port_credits[port_idx] -= calendar->slots[slot_idx].credit;
      --port_num_slots[port_idx];
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_fill_shaper_calendar_credits()",slot_idx,port_idx);
}

/*
 *  This functions reads the calculated calendar values from the device
 *  It also reads per-port maximal burst configuration (EGQ).
 *  Note: rates_table is only used to get the per-port shaper (max burst)
 */
STATIC uint32
  soc_petra_ofp_rates_active_calendars_retrieve_egq(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    mal_ndx,
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_CAL_EGQ      *egq_cal,
    SOC_SAND_OUT uint32                     *egq_cal_len,
    SOC_SAND_IN  uint8                    remove_dummies
  )
{
  uint32
    res,
    fld_val = 0,
    reg_idx,
    fld_idx,
    temp_egq_cal_len,
    egq_len = 0;
  uint32
    offset = 0,
    slot = 0;
  SOC_PETRA_EGQ_SCM_TBL_DATA
    egq_data;
  uint32
    egq_to_get;
  SOC_PETRA_REG_FIELD
    *egq_cal_len_fld;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_OFP_EGQ_RATES_CAL_ENTRY
    *cal_slot = NULL;
  SOC_PETRA_MAL_EQUIVALENT_TYPE
    mal_type;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_RATES_ACTIVE_CALENDARS_RETRIEVE);

  SOC_SAND_CHECK_NULL_INPUT(egq_cal);

  regs = soc_petra_regs();

  *egq_cal_len = 0;

  mal_type = soc_petra_mal_type_from_id(mal_ndx);

  /*
   * Check which calendars (EGQ & SCH - Calendars get 'A' or 'B')
   * are currently active.
   */
  SOC_PA_FLD_GET(regs->egq.egress_shaper_enable_settings_reg.spr_set_sel, fld_val, 30, exit);
  egq_to_get = fld_val;

  switch(mal_type) {
  case SOC_PETRA_MAL_TYPE_NIF:
    reg_idx = SOC_PETRA_REG_IDX_GET(mal_ndx, SOC_PETRA_MALS_IN_NIF_GRP);
    fld_idx = SOC_PETRA_FLD_IDX_GET(mal_ndx, SOC_PETRA_MALS_IN_NIF_GRP);
    egq_cal_len_fld = SOC_PA_REG_DB_ACC_REF(regs->egq.egress_shpr_conf_for_nif_cal_len_reg[reg_idx].nif_ch_spr_cal_len[fld_idx]);
    break;
  case SOC_PETRA_MAL_TYPE_CPU:
  case SOC_PETRA_MAL_TYPE_OLP:
    egq_cal_len_fld = SOC_PA_REG_DB_ACC_REF(regs->egq.egress_shpr_conf_for_other_calendars_length_reg.cpu_spr_cal_len);
    break;
  case SOC_PETRA_MAL_TYPE_RCY:
    egq_cal_len_fld = SOC_PA_REG_DB_ACC_REF(regs->egq.egress_shpr_conf_for_other_calendars_length_reg.rcy_spr_cal_len);
    break;
  case SOC_PETRA_MAL_TYPE_ERP:
    SOC_PETRA_DO_NOTHING_AND_EXIT;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_OFP_RATES_MAL_TYPE_INVALID_ERR, 45, exit);
  }

  /*
   *  Calendar Length
   */
  SOC_PA_IMPLICIT_FLD_IGET(*egq_cal_len_fld, egq_len, egq_to_get, 50, exit);

  /*
   * Read the Active EGQ calendar indirectly
   */

  /*
   * EGQ tables are double in size -
   * the second half is for get 'B' calendars/shapers.
   */
  offset = egq_to_get * SOC_PETRA_OFP_RATES_CAL_LEN_EGQ_MAX;
  for (slot = 0; slot < egq_len + 1; ++slot)
  {
    cal_slot = egq_cal->slots + slot;

    switch(mal_type) {
    case SOC_PETRA_MAL_TYPE_NIF:
      res = soc_petra_egq_nif_scm_tbl_get_unsafe(
              unit,
              mal_ndx,
              offset + slot,
              &egq_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);
      break;
    case SOC_PETRA_MAL_TYPE_CPU:
    case SOC_PETRA_MAL_TYPE_OLP:
      res = soc_petra_egq_cpu_scm_tbl_get_unsafe(
              unit,
              offset + slot,
              &egq_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 72, exit);
      break;
    case SOC_PETRA_MAL_TYPE_RCY:
      res = soc_petra_egq_rcy_scm_tbl_get_unsafe(
              unit,
              offset + slot,
              &egq_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 74, exit);
      break;
    case SOC_PETRA_MAL_TYPE_ERP:
      SOC_PETRA_DO_NOTHING_AND_EXIT;
      break;
    /* must default. Otherwise - compilation error */
    /* coverity[dead_error_begin:FALSE] */
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_OFP_RATES_MAL_TYPE_INVALID_ERR, 76, exit);
    }

    cal_slot->credit = egq_data.port_cr_to_add;
    cal_slot->port_idx = egq_data.ofp_index;
  }

  /*
   * The device calendar length is the actual val minus 1
   */
  temp_egq_cal_len = egq_len + 1;

  /*
   * If the last dummy entry should be removed decrease cal_len by 1.
   * cal_slot holds the last entry.
   */
  if (remove_dummies)
  {
    for (slot = 0; slot < temp_egq_cal_len; ++slot)
    {
      cal_slot = egq_cal->slots + slot;
      if (soc_petra_ofp_rates_is_cal_entry_dummy(cal_slot))
      {
        SOC_PETRA_COPY(egq_cal->slots + slot, egq_cal->slots + slot + 1, SOC_PETRA_OFP_EGQ_RATES_CAL_ENTRY, (temp_egq_cal_len - slot - 1));
        temp_egq_cal_len -= 1;
      }
    }
  }

  *egq_cal_len = temp_egq_cal_len;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_active_calendars_retrieve_egq()",0,0);
}

STATIC uint32
  soc_petra_ofp_rates_from_egq_ports_rates_to_calendar(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    mal_ndx,
    SOC_SAND_IN  uint8                    recalc,
    SOC_SAND_IN  uint32                     *ports_rates,
    SOC_SAND_IN  uint32                    nof_ports,
    SOC_SAND_IN  uint32                     total_shaper_bandwidth,
    SOC_SAND_IN  uint32                     max_calendar_len,
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_CAL_EGQ      *calendar,
    SOC_SAND_OUT uint32                     *calendar_len
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    act_nof_ports = 0;
  uint32
    multiplier = 1,
    idx = 0;
  uint32
    loc_calendar_len;
  uint32
    *port_nof_slots = NULL,
    *temp_buff = NULL,
    *slots_ports = NULL;
  uint32
    nof_dup_entries = 0,
    last_port_entry = SOC_PETRA_OFP_RATES_INVALID_PORT_ID;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_RATES_FROM_EGQ_PORT_RATES_TO_CALENDAR);

  SOC_PETRA_ALLOC(port_nof_slots, uint32, SOC_PETRA_NOF_FAP_PORTS);
  SOC_PETRA_ALLOC(slots_ports, uint32, SOC_PETRA_OFP_RATES_CAL_LEN_EGQ_MAX);
  SOC_PETRA_ALLOC(temp_buff, uint32, SOC_PETRA_OFP_RATES_CAL_LEN_EGQ_MAX);

  if (recalc)
  {
    /* Find the real number of ports (ports that have rate greater than zero) */
    for (idx = 0; idx < nof_ports; ++idx)
    {
      act_nof_ports += (ports_rates[idx] > 0 ? 1 : 0);
    }

    do
    {
      nof_dup_entries = 0;

      if ((multiplier *= 2) > SOC_PETRA_OFP_RATES_NOF_ITERATIONS)
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_OFP_RATES_NOF_ITERATIONS_EXCEEDS_LIMITS_ERR, 18, exit);
      }

      /* Minimum calendar length must be greater than the number of ports */
      if (max_calendar_len / multiplier < act_nof_ports)
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_OFP_RATES_CAL_LEN_INVALID_ERR, 12, exit);
      }

      /*
       * Calculate the optimal calendar length and the
       * corresponding weight (in slots) of each port.
       */
      res = soc_petra_ofp_rates_cal_len_calculate(
              unit,
              ports_rates,
              nof_ports,
              total_shaper_bandwidth,
              max_calendar_len / multiplier - 1,
              &loc_calendar_len,
              port_nof_slots
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      /*
       * Given the optimal calendar length and the
       * corresponding weight (in slots) of each port,
       * build a calendar that will avoid burstiness
       * behavior as much as possible.
       */
      res = soc_petra_ofp_rates_fixed_len_cal_build(
              unit,
              port_nof_slots,
              nof_ports,
              loc_calendar_len,
              max_calendar_len / multiplier - 1,
              slots_ports
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      /*
       * For this calendar length calculate the number of
       * duplicate entries in the calendar
       */
      for (idx = 0; idx < loc_calendar_len; ++idx)
      {
        nof_dup_entries += (last_port_entry == slots_ports[idx] ? 1 : 0);
        last_port_entry = slots_ports[idx];
      }
    } while(loc_calendar_len + nof_dup_entries + 1 > max_calendar_len);

    last_port_entry = SOC_PETRA_OFP_RATES_INVALID_PORT_ID;
    for (idx = 0; idx < loc_calendar_len; ++idx)
    {
      if (last_port_entry == slots_ports[idx])
      {
        SOC_SAND_ERR_IF_ABOVE_MAX(
          loc_calendar_len, SOC_PETRA_OFP_RATES_CAL_LEN_EGQ_MAX,
          SOC_PETRA_OFP_RATES_CAL_LEN_INVALID_ERR, 22, exit
        );

        /* Space the calendar, inserting dummies between duplicate entries */
        SOC_PETRA_COPY(temp_buff, slots_ports, uint32, SOC_PETRA_OFP_RATES_CAL_LEN_EGQ_MAX);
        SOC_PETRA_COPY(temp_buff + idx + 1, slots_ports + idx, uint32, loc_calendar_len - idx);
        SOC_PETRA_COPY(slots_ports, temp_buff, uint32, SOC_PETRA_OFP_RATES_CAL_LEN_EGQ_MAX);
        slots_ports[idx] = SOC_PETRA_OFP_RATES_INVALID_PORT_ID;

        /* Increase the calendar length by 1 for each dummy inserted */
        loc_calendar_len += 1;
      }
      last_port_entry = slots_ports[idx];
    }
    /* Insert dummy tail at the end of each calendar */
    loc_calendar_len += 1;
  }
  else
  {
    res = soc_petra_ofp_rates_active_calendars_retrieve_egq(
            unit,
            mal_ndx,
            calendar,
            &loc_calendar_len,
            FALSE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    for (idx = 0; idx < loc_calendar_len; ++idx)
    {
      slots_ports[idx] = calendar->slots[idx].port_idx;
    }
  }

  /* Fill calendar rates as if calendar length was bigger by 1 (dummy tail) */
  res = soc_petra_ofp_rates_fill_shaper_calendar_credits(
          unit,
          mal_ndx,
          ports_rates,
          nof_ports,
          loc_calendar_len - 1,
          slots_ports,
          calendar
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  /* Add the dummy tail */
  calendar->slots[loc_calendar_len - 1].port_idx = SOC_PETRA_OFP_RATES_INVALID_PORT_ID;
  calendar->slots[loc_calendar_len - 1].credit = 0;
  *calendar_len = loc_calendar_len;

exit:
  SOC_PETRA_FREE(port_nof_slots);
  SOC_PETRA_FREE(slots_ports);
  SOC_PETRA_FREE(temp_buff);

  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_from_egq_ports_rates_to_calendar()",0,0);
}

STATIC uint32
  soc_petra_ofp_rates_from_calendar_to_ports_sch_rate(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PETRA_OFP_RATES_CAL_SCH      *calendar,
    SOC_SAND_IN  uint32                     calendar_len,
    SOC_SAND_IN  uint32                     total_sch_rate,
    SOC_SAND_IN  uint32                    nof_ports,
    SOC_SAND_OUT uint32                     *ports_rates
  )
{
  uint32
    port,
    sched,
    calc,
    slot_id;
  SOC_SAND_U64
    u64_1,
    u64_2;
  uint32
    rem;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_RATES_FROM_CALENDAR_TO_PORTS_SCH_RATE);

  SOC_SAND_CHECK_NULL_INPUT(calendar);

  if (0 == calendar_len)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_OFP_RATES_CAL_LEN_INVALID_ERR, 2, exit);
  }

  for (port = 0; port < nof_ports; ++port)
  {
    ports_rates[port] = 0;
  }

  for (slot_id = 0; slot_id < calendar_len; ++slot_id)
  {
    sched = calendar->slots[slot_id];
    if (sched >= nof_ports)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_OFP_RATES_SCH_PORT_ID_OUT_OF_RANGE_ERR, 3, exit);
    }
    /*
     * Increase the number of calendar slots of this port by 1
     */
    ++ports_rates[sched];
  }

  /*
   *  Calculate credit rate for each port according to:
   *
   *                                 port_num_of_cal_slots * total_sch_rate[kBits/sec]
   *  port_sch_rate [kbits/sec]= -----------------------------------------------------
   *                                                cal_num_of_slots
   *
   */
  for (port = 0; port < nof_ports; ++port)
  {
    soc_sand_u64_multiply_longs(ports_rates[port], total_sch_rate, &u64_1);
    rem = soc_sand_u64_devide_u64_long(&u64_1, calendar_len, &u64_2);
    soc_sand_u64_to_long(&u64_2, &(calc));
    calc = (rem > 0 ? calc + 1 : calc);

    ports_rates[port] = calc;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_from_calendar_to_ports_sch_rate()",0,0);
}

STATIC uint32
  soc_petra_ofp_rates_from_calendar_to_ports_egq_rate(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    mal_ndx,
    SOC_SAND_IN  SOC_PETRA_OFP_RATES_CAL_EGQ      *calendar,
    SOC_SAND_IN  uint32                     calendar_len,
    SOC_SAND_IN  uint32                    nof_ports,
    SOC_SAND_OUT uint32                     *ports_rates
  )
{
  uint32
    port_idx,
    calc,
    slot_id;
  const SOC_PETRA_OFP_EGQ_RATES_CAL_ENTRY
    *slot;
  SOC_SAND_U64
    u64_1,
    u64_2;
  uint32
    rem;
  uint32
    calcal_length,
    calcal_instances;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_RATES_FROM_CALENDAR_TO_PORTS_EGQ_RATE);

  SOC_SAND_CHECK_NULL_INPUT(calendar);

  if (0 == calendar_len)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_OFP_RATES_CAL_LEN_INVALID_ERR, 2, exit);
  }

  for (port_idx = 0; port_idx < nof_ports; ++port_idx)
  {
    ports_rates[port_idx] = 0;
  }

  /* Last entry is dummy. Traverse until calendar_len-1 */
  for (slot_id = 0; slot_id < calendar_len - 1; ++slot_id)
  {
    slot = &calendar->slots[slot_id];

    if (slot->port_idx == SOC_PETRA_OFP_RATES_INVALID_PORT_ID)
    {
      continue;
    }
    if (slot->port_idx >= nof_ports)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_OFP_RATES_FAP_PORTS_OUT_OF_RANGE_ERR, 3, exit);
    }
    /*
     * Increase the total sum of credit of this port_idx by the val of this slot
     */
    ports_rates[slot->port_idx] += slot->credit;
  }

  /*
   *  Calculate shaper rate for each port_idx according to:
   *
   *                                 total_port_credit [bits] * core_frequency [kilo-clocks]
   *  port_egq_rate [kbits/sec]= ---------------------------------------------------------
   *                                           calendar_length * slot_traverse_time [clocks]
   *
   */

  calcal_length = soc_petra_sw_db_ofp_rates_calcal_length_get(unit);
  calcal_instances = soc_petra_sw_db_ofp_rates_nof_instances_get(unit, mal_ndx);

  for (port_idx = 0; port_idx < nof_ports; ++port_idx)
  {
    soc_sand_u64_multiply_longs(calcal_instances * ports_rates[port_idx], soc_petra_chip_kilo_ticks_per_sec_get(unit), &u64_1);
    rem = soc_sand_u64_devide_u64_long(&u64_1, calcal_length * calendar_len * SOC_PETRA_EGQ_UNITS_VAL_IN_BITS * SOC_PETRA_EGQ_CAL_SLOT_TRAVERSE_IN_CLOCKS, &u64_2);
    soc_sand_u64_to_long(&u64_2, &(calc));
    calc = (rem > 0 ? calc + 1 : calc);

    ports_rates[port_idx] = calc;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_from_calendar_to_ports_egq_rate()",0,0);
}

/*
 *  This functions writes the calculated calendar values to the device
 *  It also writes per-port maximal burst configuration (EGQ),
 *  to be consistent with the calendar rates
 */
STATIC uint32
  soc_petra_ofp_rates_active_calendars_config(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    mal_ndx,
    SOC_SAND_IN  SOC_PETRA_OFP_RATES_CAL_SCH      *sch_cal,
    SOC_SAND_IN  uint32                     sch_cal_len,
    SOC_SAND_IN  SOC_PETRA_OFP_RATES_CAL_EGQ      *egq_cal,
    SOC_SAND_IN  uint32                     egq_cal_len,
    SOC_SAND_IN  uint32                     *egq_bursts,
    SOC_SAND_IN  uint32                     activ_mals_len,
    SOC_SAND_IN  uint8                    *active_mals,
    SOC_SAND_IN  uint8                    egq_only_arg
  )
{
  uint32
    res,
    fld_val = 0,
    reg_idx,
    fld_idx,
    sch_len = 0,
    egq_len = 0;
  uint32
    offset = 0,
    slot = 0,
    egq_mal_id;
  SOC_PETRA_EGQ_SCM_TBL_DATA
    egq_data;
  SOC_PETRA_SCH_CAL_TBL_DATA
    sch_data;
  SOC_PETRA_EGQ_PMC_TBL_DATA
    pmc_tbl_data;
  uint32
    sch_to_set,
    egq_to_set;
  SOC_PETRA_REG_FIELD
    *egq_cal_len_fld,
    *sch_cal_len_fld,
    *sch_cal_sel_fld;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_MAL_EQUIVALENT_TYPE
    mal_type;
  uint8
    egq_only,
    egq_inactive;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_RATES_ACTIVE_CALENDARS_CONFIG);


  SOC_SAND_CHECK_NULL_INPUT(sch_cal);
  SOC_SAND_CHECK_NULL_INPUT(egq_cal);

  regs = soc_petra_regs();

  egq_mal_id = soc_petra_ofp_rates_mal_egq_from_mal_id(mal_ndx);
  if (egq_mal_id != SOC_PETRA_OFP_RATES_EGQ_MAL_INVALID)
  {
     egq_inactive = active_mals[egq_mal_id]?FALSE:TRUE;
  }
  else
  {
    egq_inactive = FALSE;
  }

  /*
   *  Verify calendar length validity
   */
  SOC_SAND_ERR_IF_OUT_OF_RANGE(
    sch_cal_len, 1, SOC_PETRA_OFP_RATES_CAL_LEN_SCH_MAX,
    SOC_PETRA_OFP_RATES_CAL_LEN_INVALID_ERR, 10, exit
  );

  if (!egq_inactive)
  {
    SOC_SAND_ERR_IF_OUT_OF_RANGE(
      egq_cal_len, 1, SOC_PETRA_OFP_RATES_CAL_LEN_EGQ_MAX,
      SOC_PETRA_OFP_RATES_CAL_LEN_INVALID_ERR, 20, exit
    );
  }

  mal_type = soc_petra_mal_type_from_id(mal_ndx);

  /*
   * Check which calendars (EGQ & SCH - Calendars set 'A' or 'B')
   * are currently active. Then build the non-active calendars,
   * and finally swap between the active calendars and the non-active ones.
   */

  switch(mal_type) {
  case SOC_PETRA_MAL_TYPE_NIF:
    egq_only = egq_only_arg;
    sch_cal_sel_fld = SOC_PA_REG_DB_ACC_REF(regs->sch.ch_nif_cal_config_reg[mal_ndx].dvscalendar_sel_ch_nifxx);
    break;
  case SOC_PETRA_MAL_TYPE_CPU:
    egq_only = egq_only_arg;
    sch_cal_sel_fld = SOC_PA_REG_DB_ACC_REF(regs->sch.cpu_cal_configuration_reg.dvscalendar_sel_cpu);
    break;
  case SOC_PETRA_MAL_TYPE_RCY:
    egq_only = egq_only_arg;
    sch_cal_sel_fld = SOC_PA_REG_DB_ACC_REF(regs->sch.rcy_cal_configuration_reg.dvscalendar_sel_rcy);
    break;
  case SOC_PETRA_MAL_TYPE_ERP:
    sch_cal_sel_fld = NULL;
    SOC_PETRA_DO_NOTHING_AND_EXIT;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_OFP_RATES_MAL_TYPE_INVALID_ERR, 45, exit);
  }

  SOC_PA_FLD_GET(regs->egq.egress_shaper_enable_settings_reg.spr_set_sel, fld_val, 30, exit);
  egq_to_set = (fld_val == SOC_PETRA_OFP_RATES_CAL_SET_A)?SOC_PETRA_OFP_RATES_CAL_SET_B:SOC_PETRA_OFP_RATES_CAL_SET_A;

  if (!egq_only)
  {
    SOC_PA_IMPLICIT_FLD_GET(*sch_cal_sel_fld, fld_val, 40, exit);
    sch_to_set = (fld_val == SOC_PETRA_OFP_RATES_CAL_SET_A)?SOC_PETRA_OFP_RATES_CAL_SET_B:SOC_PETRA_OFP_RATES_CAL_SET_A;
  }
  else
  {
    /* Not supposed to be used */
    sch_to_set = SOC_PETRA_OFP_RATES_CAL_SET_A;
  }

  switch(mal_type) {
  case SOC_PETRA_MAL_TYPE_NIF:
    reg_idx = SOC_PETRA_REG_IDX_GET(mal_ndx, SOC_PETRA_MALS_IN_NIF_GRP);
    fld_idx = SOC_PETRA_FLD_IDX_GET(mal_ndx, SOC_PETRA_MALS_IN_NIF_GRP);
    egq_cal_len_fld = SOC_PA_REG_DB_ACC_REF(regs->egq.egress_shpr_conf_for_nif_cal_len_reg[reg_idx].nif_ch_spr_cal_len[fld_idx]);
    sch_cal_len_fld = SOC_PA_REG_DB_ACC_REF(regs->sch.ch_nif_cal_config_reg[mal_ndx].cal_len[sch_to_set]);
    sch_cal_sel_fld = SOC_PA_REG_DB_ACC_REF(regs->sch.ch_nif_cal_config_reg[mal_ndx].dvscalendar_sel_ch_nifxx);
    break;
  case SOC_PETRA_MAL_TYPE_CPU:
    egq_cal_len_fld = SOC_PA_REG_DB_ACC_REF(regs->egq.egress_shpr_conf_for_other_calendars_length_reg.cpu_spr_cal_len);
    sch_cal_len_fld = SOC_PA_REG_DB_ACC_REF(regs->sch.cpu_cal_configuration_reg.cal_len[sch_to_set]);
    sch_cal_sel_fld = SOC_PA_REG_DB_ACC_REF(regs->sch.cpu_cal_configuration_reg.dvscalendar_sel_cpu);
    break;
  case SOC_PETRA_MAL_TYPE_RCY:
    egq_cal_len_fld = SOC_PA_REG_DB_ACC_REF(regs->egq.egress_shpr_conf_for_other_calendars_length_reg.rcy_spr_cal_len);
    sch_cal_len_fld = SOC_PA_REG_DB_ACC_REF(regs->sch.rcy_cal_configuration_reg.cal_len[sch_to_set]);
    sch_cal_sel_fld = SOC_PA_REG_DB_ACC_REF(regs->sch.rcy_cal_configuration_reg.dvscalendar_sel_rcy);
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_OFP_RATES_MAL_TYPE_INVALID_ERR, 45, exit);
  }

  /*
   * The device calendar length is the actual val minus 1
   */

  sch_len = sch_cal_len - 1;
  egq_len = (egq_inactive) ? 1:(egq_cal_len - 1);

  /*
   *  Calendar length
   */
  SOC_PA_IMPLICIT_FLD_ISET(*egq_cal_len_fld, egq_len, egq_to_set, 50, exit);

  if (!egq_only)
  {
    SOC_PA_IMPLICIT_FLD_SET(*sch_cal_len_fld, sch_len, 60, exit);
  }

  /*
   * Write to the non-Active EGQ calendar indirectly
   */

  /*
   * EGQ tables are double in size -
   * the second half is for set 'B' calendars/shapers.
   */
  for (slot = 0; slot < egq_cal_len; ++slot)
  {
    offset = egq_to_set * SOC_PETRA_OFP_RATES_CAL_LEN_EGQ_MAX;
    egq_data.port_cr_to_add = egq_cal->slots[slot].credit;
    egq_data.ofp_index = egq_cal->slots[slot].port_idx;

    switch(mal_type) {
    case SOC_PETRA_MAL_TYPE_NIF:
      res = soc_petra_egq_nif_scm_tbl_set_unsafe(
              unit,
              mal_ndx,
              offset + slot,
              &egq_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 75, exit);
      break;
    case SOC_PETRA_MAL_TYPE_CPU:
      res = soc_petra_egq_cpu_scm_tbl_set_unsafe(
              unit,
              offset + slot,
              &egq_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 72, exit);
      break;
    case SOC_PETRA_MAL_TYPE_RCY:
      res = soc_petra_egq_rcy_scm_tbl_set_unsafe(
              unit,
              offset + slot,
              &egq_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 74, exit);
      break;
    case SOC_PETRA_MAL_TYPE_ERP:
      SOC_PETRA_DO_NOTHING_AND_EXIT;
      break;
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_OFP_RATES_MAL_TYPE_INVALID_ERR, 76, exit);
    }

    /*Write Port Max Burst Value*/
    if (egq_data.ofp_index != SOC_PETRA_OFP_RATES_INVALID_PORT_ID)
    {
      offset = SOC_PETRA_NOF_FAP_PORTS * egq_to_set + egq_data.ofp_index;
      pmc_tbl_data.port_max_credit = egq_bursts[egq_data.ofp_index];
      res = soc_petra_egq_pmc_tbl_set_unsafe(
              unit,
              offset,
              &pmc_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 77, exit);
    }
  }

  if (!egq_only)
  {

    /*
     * Write to the non-Active SCH calendar indirectly
     */

    /*
     *  All scheduler calendars share the same table.
     *  The order is:
     *  MAL0-CALA
     *  MAL0-CALB
     *  MAL1-CALA
     *  MAL1-CALB
     *  ...
     *  CPU-CALA
     *  CPU-CALB
     *  RCY-CALA
     *  RCY-CALB
     */
    switch(mal_type) {
    case SOC_PETRA_MAL_TYPE_NIF:
      offset = (mal_ndx*SOC_PETRA_OFP_NOF_RATES_CAL_SETS + sch_to_set) * SOC_PETRA_OFP_RATES_CAL_LEN_SCH_OFFSET;
      break;
    case SOC_PETRA_MAL_TYPE_CPU:
      offset = SOC_PETRA_OFP_RATES_CAL_CPU_BASE_SCH_OFFSET + sch_to_set * SOC_PETRA_OFP_RATES_CAL_LEN_SCH_OFFSET;
      break;
    case SOC_PETRA_MAL_TYPE_RCY:
      offset = SOC_PETRA_OFP_RATES_CAL_RCY_BASE_SCH_OFFSET + sch_to_set * SOC_PETRA_OFP_RATES_CAL_LEN_SCH_OFFSET;
      break;
    /* must default. Otherwise - compilation error */
    /* coverity[dead_error_begin:FALSE] */
    default:
      SOC_SAND_SET_ERROR_CODE(SOC_PETRA_OFP_RATES_MAL_TYPE_INVALID_ERR, 80, exit);
    }

    for (slot = 0; slot < sch_cal_len; ++slot)
    {
      sch_data.hrsel = sch_cal->slots[slot];

      res = soc_petra_sch_cal_tbl_set_unsafe(
              unit,
              offset + slot,
              &sch_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
    }
  }/* Not EGQ-only */

  if (!egq_only)
  {
    fld_val = sch_to_set;
    SOC_PA_IMPLICIT_FLD_SET(*sch_cal_sel_fld, fld_val, 110, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_active_calendars_config()",0,0);
}

STATIC uint32
  soc_petra_ofp_rates_active_calendars_retrieve(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    mal_ndx,
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_CAL_SCH      *sch_cal,
    SOC_SAND_OUT uint32                     *sch_cal_len,
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_CAL_EGQ      *egq_cal,
    SOC_SAND_OUT uint32                     *egq_cal_len,
    SOC_SAND_OUT uint32                     *egq_bursts
  )
{
  uint32
    res,
    fld_val = 0,
    ofp_idx;
  uint32
    offset = 0;
  SOC_PETRA_EGQ_PMC_TBL_DATA
    pmc_tbl_data;
  SOC_PETRA_REGS
    *regs;
  uint32
    egq_to_get;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_RATES_ACTIVE_CALENDARS_RETRIEVE);


  SOC_SAND_CHECK_NULL_INPUT(sch_cal);
  SOC_SAND_CHECK_NULL_INPUT(egq_cal);

  regs = soc_petra_regs();

  *egq_cal_len = 0;
  res = soc_petra_ofp_rates_active_calendars_retrieve_egq(
          unit,
          mal_ndx,
          egq_cal,
          egq_cal_len,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  *sch_cal_len = 0;
  res = soc_petra_ofp_rates_active_calendars_retrieve_sch(
          unit,
          mal_ndx,
          sch_cal,
          sch_cal_len
      );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  SOC_PETRA_FLD_GET(regs->egq.egress_shaper_enable_settings_reg.spr_set_sel, fld_val, 30, exit);
  egq_to_get = fld_val;

  for (ofp_idx = 0; ofp_idx < SOC_PETRA_NOF_FAP_PORTS; ++ofp_idx)
  {
    offset = SOC_PETRA_NOF_FAP_PORTS * egq_to_get + ofp_idx;

    res = soc_petra_egq_pmc_tbl_get_unsafe(
          unit,
          offset,
          &pmc_tbl_data
        );
    SOC_SAND_CHECK_FUNC_RESULT(res, 95, exit);
    egq_bursts[ofp_idx] = pmc_tbl_data.port_max_credit;
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_active_calendars_retrieve()",0,0);
}

/*********************************************************************
*     Translate from kilobit-per-second
*     to shaper internal representation:
*     units of 1/256 Bytes per clock
*********************************************************************/

STATIC uint32
  soc_petra_ofp_egq_shaper_rate_to_internal(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                     rate_kbps,
    SOC_SAND_OUT uint32                     *rate_internal
  )
{
  uint32
    device_ticks_per_sec,
    rate_int;
  SOC_SAND_U64
    calc,
    calc2;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_EGQ_SHAPER_RATE_TO_INTERNAL);


  device_ticks_per_sec = soc_petra_chip_ticks_per_sec_get(unit);

  if (device_ticks_per_sec == 0)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_DIV_BY_ZERO_ERR, 10, exit);
  }

  if (rate_kbps == 0)
  {
    rate_int = 0;
  }
  else
  {
    soc_sand_u64_multiply_longs(rate_kbps, (1000 * SOC_PETRA_EGQ_UNITS_VAL_IN_BITS), &calc);
    soc_sand_u64_devide_u64_long(&calc, device_ticks_per_sec, &calc2);

    if (soc_sand_u64_to_long(&calc2, &rate_int))
    {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_OVERFLOW_ERR, 20, exit);
    }
  }

  *rate_internal =  rate_int;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_egq_shaper_rate_to_internal()",0,0);
}

/*********************************************************************
*     Translate from kilobit-per-second
*     to shaper internal representation:
*     units of 1/256 Bytes per clock
*********************************************************************/

STATIC uint32
  soc_petra_ofp_egq_shaper_rate_from_internal(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                     rate_internal,
    SOC_SAND_OUT uint32                     *rate_kbps
  )
{
  uint32
    device_ticks_per_sec,
    rate_kb;
  SOC_SAND_U64
    calc,
    calc2;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_EGQ_SHAPER_RATE_FROM_INTERNAL);

  device_ticks_per_sec = soc_petra_chip_ticks_per_sec_get(unit);
  if (rate_internal == 0)
  {
    rate_kb = 0;
  }
  else
  {
    soc_sand_u64_multiply_longs(rate_internal, device_ticks_per_sec, &calc);
    soc_sand_u64_devide_u64_long(&calc, (1000 * SOC_PETRA_EGQ_UNITS_VAL_IN_BITS), &calc2);
    if (soc_sand_u64_to_long(&calc2, &rate_kb))
    {
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_OVERFLOW_ERR, 10, exit);
    }
  }

  *rate_kbps =  rate_kb;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_egq_shaper_rate_from_internal()",0,0);
}

/*********************************************************************
*   Update the Software Database for the specified MAL
*********************************************************************/
STATIC uint32
  soc_petra_ofp_rates_sw_db_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    mal_idx,
    SOC_SAND_IN  SOC_PETRA_OFP_RATES_TBL_INFO     *ofp_rate_tbl
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    entry_idx;
  SOC_PETRA_SW_DB_DEV_EGR_RATE
    egr_rate;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_RATES_SW_DB_WRITE);


  SOC_SAND_CHECK_NULL_INPUT(ofp_rate_tbl);

  for (entry_idx = 0; entry_idx < SOC_PETRA_NOF_FAP_PORTS; ++entry_idx)
  {
    egr_rate.valid = FALSE;
    res = soc_petra_sw_db_egr_ports_set(
            unit,
            mal_idx,
            entry_idx,
            &egr_rate
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

  for (entry_idx = 0; entry_idx < ofp_rate_tbl->nof_valid_entries; ++entry_idx)
  {
    egr_rate.valid = TRUE;
    egr_rate.sch_rates  = ofp_rate_tbl->rates[entry_idx].sch_rate;
    egr_rate.egq_rates  = ofp_rate_tbl->rates[entry_idx].egq_rate;
    egr_rate.egq_bursts = ofp_rate_tbl->rates[entry_idx].max_burst;
    res = soc_petra_sw_db_egr_ports_set(
            unit,
            mal_idx,
            ofp_rate_tbl->rates[entry_idx].port_id,
            &egr_rate
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_sw_db_set()",0,0);
}

/*********************************************************************
*   Read the Software Database for the specified MAL
*********************************************************************/
STATIC uint32
  soc_petra_ofp_rates_sw_db_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    mal_idx,
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_TBL_INFO     *ofp_rate_tbl,
    SOC_SAND_IN  uint8                    filter_mal
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    mal_i,
    ofp_i;
  SOC_PETRA_SW_DB_DEV_EGR_MAL
    *mal_ports_db = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_RATES_SW_DB_READ);

  SOC_SAND_CHECK_NULL_INPUT(ofp_rate_tbl);

  SOC_PETRA_ALLOC(mal_ports_db, SOC_PETRA_SW_DB_DEV_EGR_MAL, 1);

  res = soc_petra_sw_db_egr_ports_mal_get(
          unit,
          mal_idx,
          mal_ports_db
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  for (ofp_rate_tbl->nof_valid_entries = 0, ofp_i = 0; ofp_i < SOC_PETRA_NOF_FAP_PORTS; ++ofp_i)
  {
    if (mal_ports_db->rates[ofp_i].valid)
    {
      if (filter_mal)
      {
        res = soc_petra_port_ofp_mal_get_unsafe(
                unit,
                ofp_i,
                &mal_i
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 33, exit);

        if (mal_i != mal_idx)
        {
          continue;
        }
      }

      ofp_rate_tbl->rates[ofp_rate_tbl->nof_valid_entries].port_id = ofp_i;
      ofp_rate_tbl->rates[ofp_rate_tbl->nof_valid_entries].sch_rate = mal_ports_db->rates[ofp_i].sch_rates;
      ofp_rate_tbl->rates[ofp_rate_tbl->nof_valid_entries].egq_rate = mal_ports_db->rates[ofp_i].egq_rates;
      ofp_rate_tbl->rates[ofp_rate_tbl->nof_valid_entries++].max_burst = mal_ports_db->rates[ofp_i].egq_bursts;
    }
  }

  for (ofp_i = ofp_rate_tbl->nof_valid_entries; ofp_i < SOC_PETRA_NOF_FAP_PORTS; ++ofp_i)
  {
    soc_petra_PETRA_OFP_RATE_INFO_clear(ofp_rate_tbl->rates + ofp_i);
  }

exit:
  SOC_PETRA_FREE(mal_ports_db);
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_sw_db_get()",0,0);
}

STATIC uint32
  soc_petra_ofp_rates_active_mal_build(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    mal_ndx,
    SOC_SAND_IN  uint8                    is_active,
    SOC_SAND_OUT uint32                    *nof_active,
    SOC_SAND_OUT uint8                    *active_mals
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    entry_idx,
    egq_mal_id,
    active_mals_num = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_RATES_ACTIVE_MAL_BUILD);
  SOC_SAND_CHECK_NULL_INPUT(nof_active);
  SOC_PETRA_CLEAR(active_mals, uint8, SOC_PETRA_OFP_RATES_EGQ_NOF_MALS);

  res = soc_petra_sw_db_egr_ports_active_mals_get(
          unit,
          SOC_PETRA_NOF_MAC_LANES,
          active_mals
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  egq_mal_id = soc_petra_ofp_rates_mal_egq_from_mal_id(mal_ndx);
  if (egq_mal_id != SOC_PETRA_OFP_RATES_EGQ_MAL_INVALID)
  {
      active_mals[egq_mal_id] = is_active;
  }

  for (entry_idx = 0; entry_idx < SOC_PETRA_OFP_RATES_EGQ_NOF_MALS; ++entry_idx)
  {
    if (active_mals[entry_idx] == TRUE)
    {
      active_mals_num++;
    }
  }

  *nof_active = active_mals_num;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_active_mal_build()",0,0);
}

/*********************************************************************
*     Configures Outgoing FAP Ports (OFP) rates, in the
*     end-to-end scheduler and in the egress processor, by
*     setting the calendars, shapers etc. The function
*     calculates from the given table the calendars
*     granularity, writes it to the device and changes the
*     active calendars. It also saves the values in the
*     software database for single-entry changes in the
*     future. For ports mapped to Network Interfaces, The
*     configuration is per MAC Lane. This indicates a single
*     interface for SPAUI/XAUI, or 4 SGMII interfaces.
*     Details: in the H file. (search for prototype)
*********************************************************************/
STATIC uint32
  soc_petra_ofp_rates_internal_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    mal_ndx,
    SOC_SAND_IN  uint8                    recalc,
    SOC_SAND_IN  SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *shaper,
    SOC_SAND_IN  SOC_PETRA_OFP_RATES_TBL_INFO     *ofp_rate_tbl
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_OFP_RATES_CAL_SCH
    *sch_calendar = NULL;
  SOC_PETRA_OFP_RATES_CAL_EGQ
    *egq_calendar = NULL;
  uint32
    *sch_rates = NULL,
    *egq_rates = NULL,
    *egq_bursts = NULL;
  uint32
    exact_shaper = 0,
    sch_calendar_len = 0,
    egq_calendar_len = 0,
    sch_mal_rate_requested = 0,
    egq_mal_rate_requested = 0,
    sch_mal_rate_configured = 0,
    sum_of_port_burst = 0;
  uint32
    entry_idx,
    nof_active_mals = 0,
    ofp_idx = 0,
    mapped_mal_id;
  SOC_PETRA_INTERFACE_ID
    if_id;
  uint8
    is_channelized_id,
    is_sgmii_id;
  SOC_PETRA_OFP_RATES_MAL_SHPR_INFO
    exact_shaper_to_set,
    shaper_to_set;
  uint8
    active_mals[SOC_PETRA_OFP_RATES_EGQ_NOF_MALS];

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_RATES_SET_UNSAFE);


  SOC_SAND_CHECK_NULL_INPUT(ofp_rate_tbl);

  /*
   *  Allocate and clear
   */
  SOC_PETRA_ALLOC(sch_rates, uint32, SOC_PETRA_NOF_FAP_PORTS);
  SOC_PETRA_ALLOC(egq_rates, uint32, SOC_PETRA_NOF_FAP_PORTS);
  SOC_PETRA_ALLOC(egq_bursts, uint32, SOC_PETRA_NOF_FAP_PORTS);
  SOC_PETRA_ALLOC(sch_calendar, SOC_PETRA_OFP_RATES_CAL_SCH, 1);
  SOC_PETRA_ALLOC(egq_calendar, SOC_PETRA_OFP_RATES_CAL_EGQ, 1);

  if (shaper)
  {
    SOC_PETRA_COPY(&shaper_to_set, shaper, SOC_PETRA_OFP_RATES_MAL_SHPR_INFO, 1);
  }

  for (entry_idx = 0; entry_idx < ofp_rate_tbl->nof_valid_entries; ++entry_idx)
  {
    ofp_idx = ofp_rate_tbl->rates[entry_idx].port_id;

    res = soc_petra_ofp_rates_special_id_detect(
            unit,
            mal_ndx,
            ofp_idx,
            &is_channelized_id,
            &is_sgmii_id,
            &if_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    mapped_mal_id = SOC_PETRA_IF2MAL_NDX(if_id);

    if (mapped_mal_id == mal_ndx)
    {
      if (is_channelized_id)
      {
        /*
         *  XAUI/SPAUI/CPU(+OLP)/RCY/SGMII-0,4,8,..
         */
        sch_rates[ofp_idx] = ofp_rate_tbl->rates[entry_idx].sch_rate;
        sch_mal_rate_requested += sch_rates[ofp_idx];
      }
      else if (if_id != SOC_PETRA_IF_ID_NONE)
      {
        /*
         * ERP/SGMII-1,2,3,5,..
         */
        res = soc_petra_sch_if_shaper_rate_set_unsafe(
                unit,
                if_id,
                ofp_rate_tbl->rates[entry_idx].sch_rate,
                &exact_shaper
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
      }

      /* EGQ */
      egq_rates[ofp_idx] = ofp_rate_tbl->rates[entry_idx].egq_rate;
      egq_mal_rate_requested += egq_rates[ofp_idx];
      egq_bursts[ofp_idx] = ofp_rate_tbl->rates[entry_idx].max_burst;
      sum_of_port_burst += ofp_rate_tbl->rates[entry_idx].max_burst;
    } /* (mapped_mal_id != mal_ndx) */
  }

  res = soc_petra_sch_mal_rate_set_unsafe(
          unit,
          mal_ndx,
          sch_mal_rate_requested,
          &sch_mal_rate_configured
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  /* Rates to calendars */
  res = soc_petra_ofp_rates_from_sch_ports_rates_to_calendar(
          unit,
          mal_ndx,
          recalc,
          sch_rates,
          SOC_PETRA_NOF_FAP_PORTS,
          sch_mal_rate_requested,
          SOC_PETRA_OFP_RATES_CAL_LEN_SCH_MAX,
          sch_calendar,
          &sch_calendar_len
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  res = soc_petra_ofp_rates_from_egq_ports_rates_to_calendar(
          unit,
          mal_ndx,
          recalc,
          egq_rates,
          SOC_PETRA_NOF_FAP_PORTS,
          egq_mal_rate_requested,
          SOC_PETRA_OFP_RATES_CAL_LEN_EGQ_MAX,
          egq_calendar,
          &egq_calendar_len
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  res = soc_petra_ofp_rates_active_mal_build(
          unit,
          mal_ndx,
          SOC_SAND_NUM2BOOL(egq_mal_rate_requested != 0),
          &nof_active_mals,
          active_mals
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  /* Write to device */
  res = soc_petra_ofp_rates_active_calendars_config(
          unit,
          mal_ndx,
          sch_calendar,
          sch_calendar_len,
          egq_calendar,
          egq_calendar_len,
          egq_bursts,
          nof_active_mals,
          active_mals,
          FALSE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  /* Set shaper values */
  if (shaper)
  {
    if (shaper_to_set.sch_shaper.rate_update_mode == SOC_PETRA_OFP_SHPR_UPDATE_MODE_SUM_OF_PORTS)
    {
      shaper_to_set.sch_shaper.rate = sch_mal_rate_requested;
    }

    if (shaper_to_set.egq_shaper.rate_update_mode == SOC_PETRA_OFP_SHPR_UPDATE_MODE_SUM_OF_PORTS)
    {
      shaper_to_set.egq_shaper.rate = egq_mal_rate_requested;
    }

    res = soc_petra_ofp_rates_mal_shaper_set_unsafe(
            unit,
            mal_ndx,
            &shaper_to_set,
            &exact_shaper_to_set
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);
  }

exit:
  SOC_PETRA_FREE(sch_rates);
  SOC_PETRA_FREE(egq_rates);
  SOC_PETRA_FREE(egq_bursts);
  SOC_PETRA_FREE(sch_calendar);
  SOC_PETRA_FREE(egq_calendar);
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_internal_unsafe()",mal_ndx,0);
}

STATIC uint32
  soc_pa_ofp_rates_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    mal_ndx,
    SOC_SAND_IN  SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *shaper,
    SOC_SAND_IN  SOC_PETRA_OFP_RATES_TBL_INFO     *ofp_rate_tbl
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    egq_calcal_rates[SOC_PETRA_OFP_RATES_EGQ_NOF_MALS];
  uint32
    egq_calcal_instances[SOC_PETRA_OFP_RATES_EGQ_NOF_MALS];
  uint32
    egq_calcal_mal_rate_requested = 0;
  SOC_PETRA_OFP_RATES_CAL_SCH
    *egq_calcal_calendar = NULL;
  uint32
    act_cal,
    egq_calcal_calendar_len = 0;
  uint32
    idx,
    ofp_i;
  SOC_PETRA_OFP_RATES_EGQ_MAL_ID
    mal_i;
  uint32
    cc_mal_id;
  SOC_PETRA_INTERFACE_ID
    if_id;
  /* The stucture SOC_PETRA_OFP_RATES_TBL_INFO is very big */
  SOC_PETRA_OFP_RATES_TBL_INFO
    *temp_rate_tbl = NULL;
  uint32
    sch_mal_rate = 0;
  uint8
    update_dev_changed,
    is_channelized_id,
    is_sgmii;
  SOC_PETRA_REGS
    *regs;
  SOC_PETRA_EGQ_CCM_TBL_DATA
    ccm_tbl_data;
  SOC_PETRA_OFP_RATES_CAL_SET
    cal2set;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_RATES_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ofp_rate_tbl);

  regs = soc_petra_regs();

  SOC_PETRA_ALLOC(egq_calcal_calendar, SOC_PETRA_OFP_RATES_CAL_SCH, 1);
  SOC_PETRA_ALLOC(temp_rate_tbl, SOC_PETRA_OFP_RATES_TBL_INFO, 1);

  for (idx = 0; idx < SOC_PETRA_OFP_RATES_EGQ_NOF_MALS; ++idx)
  {
    egq_calcal_rates[idx] = 0;
    egq_calcal_instances[idx] = 0;
  }

  /* Update total rate according to the current rates */
  for (idx = 0; idx < ofp_rate_tbl->nof_valid_entries; ++idx)
  {
    res = soc_petra_ofp_rates_special_id_detect(
            unit,
            mal_ndx,
            ofp_rate_tbl->rates[idx].port_id,
            &is_channelized_id,
            &is_sgmii,
            &if_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    if (is_channelized_id)
    {
      sch_mal_rate += ofp_rate_tbl->rates[idx].sch_rate;
    }
  }

  /* Write configuration to sw db */
  res = soc_petra_ofp_rates_sw_db_set(
          unit,
          mal_ndx,
          ofp_rate_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_petra_sw_db_egr_ports_mal_sch_rate_set(
          unit,
          mal_ndx,
          &sch_mal_rate
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  /*
   *  Compute the calendars and write to the device only if configured to do so
   */

  if (soc_petra_sw_db_ofp_rates_update_device_get(unit) == TRUE)
  {
    /* Calculate CalCal according to the sum of ofp rates on each mal */
    egq_calcal_mal_rate_requested = 0;
    for (mal_i = 0; mal_i < SOC_PETRA_OFP_RATES_EGQ_NOF_MALS; ++mal_i)
    {
      soc_petra_PETRA_OFP_RATES_TBL_INFO_clear(temp_rate_tbl);
      res = soc_petra_ofp_rates_sw_db_get(
              unit,
              soc_petra_ofp_rates_ccmmal2mal(mal_i),
              temp_rate_tbl,
              FALSE
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      egq_calcal_rates[mal_i] = 0;
      for (ofp_i = 0; ofp_i < temp_rate_tbl->nof_valid_entries; ++ofp_i)
      {
        egq_calcal_mal_rate_requested += temp_rate_tbl->rates[ofp_i].egq_rate;
        egq_calcal_rates[mal_i] += temp_rate_tbl->rates[ofp_i].egq_rate;
      }
    }

    /* */
    res = soc_petra_ofp_rates_from_rates_to_calendar(
            unit,
            egq_calcal_rates,
            SOC_PETRA_OFP_RATES_EGQ_NOF_MALS,
            egq_calcal_mal_rate_requested,
            SOC_PETRA_OFP_RATES_CALCAL_LEN_EGQ_MAX,
            egq_calcal_calendar,
            &egq_calcal_calendar_len
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    /* Get the active calendar */
    SOC_PETRA_FLD_GET(regs->egq.egress_shaper_enable_settings_reg.spr_set_sel, act_cal, 50, exit);
    cal2set = (act_cal == SOC_PETRA_OFP_RATES_CAL_SET_A)?SOC_PETRA_OFP_RATES_CAL_SET_B:SOC_PETRA_OFP_RATES_CAL_SET_A;

    /* Write CalCal length to the inactive calendar */
    SOC_PETRA_FLD_SET(regs->egq.egress_shaper_calendars_arbitration_cycle_length_reg.cal_cal_len[cal2set], egq_calcal_calendar_len - 1, 65, exit);
    soc_petra_sw_db_ofp_rates_calcal_length_set(unit, egq_calcal_calendar_len);

    /* Write CalCal entries */
    for (idx = 0; idx < egq_calcal_calendar_len; ++idx)
    {
      egq_calcal_instances[egq_calcal_calendar->slots[idx]] += 1;
      ccm_tbl_data.interface_select = egq_calcal_calendar->slots[idx];
      res = soc_petra_egq_ccm_tbl_set_unsafe(
              unit,
              idx + (cal2set) * SOC_PETRA_OFP_RATES_CALCAL_LEN_EGQ_MAX,
              &ccm_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    }

    for (mal_i = 0; mal_i < SOC_PETRA_OFP_RATES_EGQ_NOF_MALS; ++mal_i)
    {
      cc_mal_id = soc_petra_ofp_rates_ccmmal2mal(mal_i);
      soc_petra_sw_db_ofp_rates_nof_instances_set(unit, cc_mal_id, egq_calcal_instances[mal_i]);

      if (egq_calcal_instances[mal_i] != 0)
      {

        /* Read OFP rates from sw db */
        soc_petra_PETRA_OFP_RATES_TBL_INFO_clear(temp_rate_tbl);
        res = soc_petra_ofp_rates_sw_db_get(
                unit,
                cc_mal_id,
                temp_rate_tbl,
                FALSE
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

        /* Call the function that calculates the internal calendar for each mal */
        update_dev_changed = soc_petra_sw_db_ofp_rates_update_dev_changed_get(unit);
        res = soc_petra_ofp_rates_internal_unsafe(
                unit,
                cc_mal_id,
                SOC_SAND_NUM2BOOL((mal_ndx == cc_mal_id) || (mal_ndx ==  SOC_PETRA_MAL_ID_OLP) || (mal_ndx ==  SOC_PETRA_MAL_ID_CPU) || (update_dev_changed)),
                ((mal_ndx == cc_mal_id) || ((mal_ndx == SOC_PETRA_MAL_ID_OLP) && (cc_mal_id == SOC_PETRA_MAL_ID_CPU)) ? shaper : NULL),
                temp_rate_tbl
             );
        SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
      } /* EGQ CALCAL instances != 0 */
    }

    SOC_PETRA_FLD_SET(regs->egq.egress_shaper_enable_settings_reg.spr_set_sel, cal2set, 110, exit);
  }

exit:
  SOC_PETRA_FREE(temp_rate_tbl);
  SOC_PETRA_FREE(egq_calcal_calendar);
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_set_unsafe()",mal_ndx,0);
}

uint32
  soc_petra_ofp_rates_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    mal_ndx,
    SOC_SAND_IN  SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *shaper,
    SOC_SAND_IN  SOC_PETRA_OFP_RATES_TBL_INFO     *ofp_rate_tbl
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_RATES_SET_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(ofp_rates_set_unsafe,(unit, mal_ndx, shaper, ofp_rate_tbl));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_set_unsafe()",0,0);
}

/*********************************************************************
*     Configures Outgoing FAP Ports (OFP) rates, in the
*     end-to-end scheduler and in the egress processor, by
*     setting the calendars, shapers etc. The function
*     calculates from the given table the calendars
*     granularity, writes it to the device and changes the
*     active calendars. It also saves the values in the
*     software database for single-entry changes in the
*     future. For ports mapped to Network Interfaces, The
*     configuration is per MAC Lane. This indicates a single
*     interface for SPAUI/XAUI, or 4 SGMII interfaces.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ofp_rates_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    mal_ndx,
    SOC_SAND_IN  SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *shaper,
    SOC_SAND_IN  SOC_PETRA_OFP_RATES_TBL_INFO     *ofp_rate_tbl
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    idx = 0;
  uint32
    mal_idx_curr,
    mal_idx_last = SOC_PETRA_MAL_ID_NONE;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_RATES_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(shaper);
  SOC_SAND_CHECK_NULL_INPUT(ofp_rate_tbl);

  SOC_SAND_MAGIC_NUM_VERIFY(shaper);
  SOC_SAND_MAGIC_NUM_VERIFY(ofp_rate_tbl);

  SOC_SAND_ERR_IF_OUT_OF_RANGE(
    ofp_rate_tbl->nof_valid_entries, 1, SOC_PETRA_NOF_FAP_PORTS-1,
    SOC_PETRA_OFP_RATES_TBL_NOF_ENTRIES_OUT_OF_RANGE_ERR, 30, exit
  );

  res = soc_petra_mal_equivalent_id_verify(unit, mal_ndx);
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  for (idx = 0; idx < ofp_rate_tbl->nof_valid_entries; ++idx)
  {
    res = soc_petra_fap_port_id_verify(unit, ofp_rate_tbl->rates[idx].port_id, TRUE);
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    SOC_SAND_ERR_IF_ABOVE_MAX(
      ofp_rate_tbl->rates[idx].sch_rate, SOC_PETRA_IF_MAX_RATE_KBPS,
      SOC_PETRA_OFP_RATES_SCH_RATE_OUT_OF_RANGE_ERR, 50, exit
    );

    SOC_SAND_ERR_IF_ABOVE_MAX(
      ofp_rate_tbl->rates[idx].egq_rate, SOC_PETRA_IF_MAX_RATE_KBPS,
      SOC_PETRA_OFP_RATES_EGQ_RATE_OUT_OF_RANGE_ERR, 60, exit
    );

    SOC_SAND_ERR_IF_ABOVE_MAX(
      ofp_rate_tbl->rates[idx].max_burst, SOC_PETRA_OFP_RATES_BURST_LIMIT_MAX,
      SOC_PETRA_OFP_RATES_BURST_OUT_OF_RANGE_ERR, 70, exit
    );

    /*
     *  Verify all are mapped to the same MAL-equivalent
     */
    if (idx == 0)
    {
      res = soc_petra_port_ofp_mal_get_unsafe(
              unit,
              ofp_rate_tbl->rates[idx].port_id,
              &mal_idx_last
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

      if (mal_idx_last != mal_ndx)
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_OFP_MAL_INDEX_MISMATCH_ERR, 85, exit);
      }

      res = soc_petra_mal_equivalent_id_verify(
              unit,
              mal_idx_last
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);
    }
    else
    {
      res = soc_petra_port_ofp_mal_get_unsafe(
              unit,
              ofp_rate_tbl->rates[idx].port_id,
              &mal_idx_curr
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

      if (mal_idx_curr != mal_idx_last)
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_OFP_RATES_PORTS_FROM_DIFFERENT_MALS_ERR, 110, exit)
      }
    }
  }

  res = soc_petra_ofp_rates_mal_shaper_verify(
          unit,
          mal_idx_last,
          shaper
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_verify()",mal_ndx,0);
}

/*********************************************************************
*     Configures Outgoing FAP Ports (OFP) rates, in the
*     end-to-end scheduler and in the egress processor, by
*     setting the calendars, shapers etc. The function
*     calculates from the given table the calendars
*     granularity, writes it to the device and changes the
*     active calendars. It also saves the values in the
*     software database for single-entry changes in the
*     future. For ports mapped to Network Interfaces, The
*     configuration is per MAC Lane. This indicates a single
*     interface for SPAUI/XAUI, or 4 SGMII interfaces.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pa_ofp_rates_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    mal_ndx,
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *shaper,
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_TBL_INFO     *ofp_rate_tbl
    )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_OFP_RATES_CAL_SCH
    *sch_calendar = NULL;
  SOC_PETRA_OFP_RATES_CAL_EGQ
    *egq_calendar = NULL;
  uint32
    sch_mal_rate,
    sch_rate;
  uint32
    *sch_rates = NULL,
    *egq_rates = NULL,
    *egq_bursts = NULL;
  uint32
    sch_calendar_len = 0,
    egq_calendar_len = 0;
  uint32
    mal_id,
    ofp_i;
  SOC_PETRA_INTERFACE_ID
    if_id;
  uint8
    is_channelized_id,
    is_sgmii_id;
  SOC_PETRA_OFP_RATES_MAL_SHPR_INFO
    shaper_to_get;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_RATES_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(shaper);
  SOC_SAND_CHECK_NULL_INPUT(ofp_rate_tbl);

  /*
   *  Allocate and clear
   */
  SOC_PETRA_ALLOC(sch_rates, uint32, SOC_PETRA_NOF_FAP_PORTS);
  SOC_PETRA_ALLOC(egq_rates, uint32, SOC_PETRA_NOF_FAP_PORTS);
  SOC_PETRA_ALLOC(egq_bursts, uint32, SOC_PETRA_NOF_FAP_PORTS);
  SOC_PETRA_ALLOC(sch_calendar, SOC_PETRA_OFP_RATES_CAL_SCH, 1);
  SOC_PETRA_ALLOC(egq_calendar, SOC_PETRA_OFP_RATES_CAL_EGQ, 1);
  SOC_PETRA_COPY(&shaper_to_get, shaper, SOC_PETRA_OFP_RATES_MAL_SHPR_INFO, 1);

  /* Read from device */
  res = soc_petra_ofp_rates_active_calendars_retrieve(
          unit,
          mal_ndx,
          sch_calendar,
          &sch_calendar_len,
          egq_calendar,
          &egq_calendar_len,
          egq_bursts
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_sw_db_egr_ports_mal_sch_rate_get(
          unit,
          mal_ndx,
          &sch_mal_rate
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 35, exit);

  /*
   *  Actual calendars to rates -
   *  Get the exact values written to the device
   *  Those values may differ from the requested due to rounding
   */
  res = soc_petra_ofp_rates_from_calendar_to_ports_sch_rate(
          unit,
          sch_calendar,
          sch_calendar_len,
          sch_mal_rate,
          SOC_PETRA_NOF_FAP_PORTS,
          sch_rates
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

  res = soc_petra_ofp_rates_from_calendar_to_ports_egq_rate(
          unit,
          mal_ndx,
          egq_calendar,
          egq_calendar_len,
          SOC_PETRA_NOF_FAP_PORTS,
          egq_rates
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  for (ofp_rate_tbl->nof_valid_entries = 0, ofp_i = 0; ofp_i < SOC_PETRA_NOF_FAP_PORTS; ++ofp_i)
  {
    res = soc_petra_port_ofp_mal_get_unsafe(
            unit,
            ofp_i,
            &mal_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 85, exit);

    if (mal_ndx == mal_id)
    {
      res = soc_petra_ofp_rates_special_id_detect(
              unit,
              mal_ndx,
              ofp_i,
              &is_channelized_id,
              &is_sgmii_id,
              &if_id
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 85, exit);

      /*
       *  For SGMII, the calendar is configured only for the port
       *  that is mapped to an interface with channelized index (0, 4, 8...).
       *  For other ports - the configuration is straightforward.
       */
      if (is_channelized_id)
      {
        sch_rate = sch_rates[ofp_i];
      }
      else
      {
        res = soc_petra_sch_if_shaper_rate_get_unsafe(
                unit,
                if_id,
                &sch_rate
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
      }

      ofp_rate_tbl->rates[ofp_rate_tbl->nof_valid_entries].port_id = ofp_i;
      ofp_rate_tbl->rates[ofp_rate_tbl->nof_valid_entries].sch_rate = sch_rate;
      ofp_rate_tbl->rates[ofp_rate_tbl->nof_valid_entries].egq_rate = egq_rates[ofp_i];
      ofp_rate_tbl->rates[ofp_rate_tbl->nof_valid_entries++].max_burst = egq_bursts[ofp_i];
    }
  }

  for (ofp_i = ofp_rate_tbl->nof_valid_entries; ofp_i < SOC_PETRA_NOF_FAP_PORTS; ++ofp_i)
  {
    soc_petra_PETRA_OFP_RATE_INFO_clear(ofp_rate_tbl->rates + ofp_i);
  }

  res = soc_petra_ofp_rates_mal_shaper_get_unsafe(
          unit,
          mal_ndx,
          shaper
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

exit:
  SOC_PETRA_FREE(sch_rates);
  SOC_PETRA_FREE(egq_rates);
  SOC_PETRA_FREE(egq_bursts);
  SOC_PETRA_FREE(sch_calendar);
  SOC_PETRA_FREE(egq_calendar);
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_get_unsafe()",mal_ndx,0);
}

uint32
  soc_petra_ofp_rates_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    mal_ndx,
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *shaper,
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_TBL_INFO     *ofp_rate_tbl
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_RATES_GET_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(ofp_rates_get_unsafe,(unit, mal_ndx, shaper, ofp_rate_tbl));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_get_unsafe()",0,0);
}

uint32
  soc_pa_ofp_all_ofp_rates_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint8                    get_exact,
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_TBL_INFO     *ofp_rate_tbl
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    ofp_i;
  SOC_PETRA_OFP_RATES_EGQ_MAL_ID
    mal_i = 0;
  SOC_PETRA_OFP_RATES_MAL_SHPR_INFO
    shaper;
  /* The stucture SOC_PETRA_OFP_RATES_TBL_INFO is very big */
  SOC_PETRA_OFP_RATES_TBL_INFO
    *ofp_table = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_OFP_ALL_OFP_RATES_GET_UNSAFE);

  SOC_PETRA_ALLOC(ofp_table, SOC_PETRA_OFP_RATES_TBL_INFO, 1);
  for (ofp_rate_tbl->nof_valid_entries = 0, mal_i = 0; mal_i < SOC_PETRA_OFP_RATES_EGQ_NOF_MALS; ++mal_i)
  {
    soc_petra_PETRA_OFP_RATES_MAL_SHPR_INFO_clear(&shaper);
    soc_petra_PETRA_OFP_RATES_TBL_INFO_clear(ofp_table);

    if (get_exact)
    {
      /* OLP is included in CPU */
      res = soc_petra_ofp_rates_sw_db_get(
              unit,
              soc_petra_ofp_rates_ccmmal2mal(mal_i),
              ofp_table,
              FALSE
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }
    else
    {
      /* OLP is NOT included in CPU. Will be retrieved later */
      /* petra code. Almost not in use. Ignore coverity defects */
      /* coverity[overrun-call] */
      res = soc_petra_ofp_rates_get_unsafe(
              unit,
              soc_petra_ofp_rates_ccmmal2mal(mal_i),
              &shaper,
              ofp_table
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
    }

    for (ofp_i = 0; ofp_i < ofp_table->nof_valid_entries; ++ofp_i)
    {
      if (ofp_table->rates[ofp_i].port_id != SOC_PETRA_OFP_RATES_ILLEGAL_PORT_ID)
      {
        ofp_rate_tbl->rates[ofp_rate_tbl->nof_valid_entries].port_id = ofp_table->rates[ofp_i].port_id;
        ofp_rate_tbl->rates[ofp_rate_tbl->nof_valid_entries].egq_rate = soc_petra_sw_db_ofp_rates_nof_instances_get(unit, soc_petra_ofp_rates_ccmmal2mal(mal_i)) ? ofp_table->rates[ofp_i].egq_rate : 0;
        ofp_rate_tbl->rates[ofp_rate_tbl->nof_valid_entries].sch_rate = ofp_table->rates[ofp_i].sch_rate;
        ofp_rate_tbl->rates[ofp_rate_tbl->nof_valid_entries++].max_burst = ofp_table->rates[ofp_i].max_burst;
      }
    }
  }

  if (!get_exact) /* OLP */
  {
    soc_petra_PETRA_OFP_RATES_MAL_SHPR_INFO_clear(&shaper);
    soc_petra_PETRA_OFP_RATES_TBL_INFO_clear(ofp_table);

    /* Petra code. Almost not in use. Ignore coverity defects */
    /* coverity[overrun-call] */
    res = soc_petra_ofp_rates_get_unsafe(
            unit,
            SOC_PETRA_MAL_ID_OLP,
            &shaper,
            ofp_table
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    for (ofp_i = 0; ofp_i < ofp_table->nof_valid_entries; ++ofp_i)
    {
      if (ofp_table->rates[ofp_i].port_id != SOC_PETRA_OFP_RATES_ILLEGAL_PORT_ID)
      {
        ofp_rate_tbl->rates[ofp_rate_tbl->nof_valid_entries].port_id = ofp_table->rates[ofp_i].port_id;
        ofp_rate_tbl->rates[ofp_rate_tbl->nof_valid_entries].egq_rate = ofp_table->rates[ofp_i].egq_rate;
        ofp_rate_tbl->rates[ofp_rate_tbl->nof_valid_entries].sch_rate = ofp_table->rates[ofp_i].sch_rate;
        ofp_rate_tbl->rates[ofp_rate_tbl->nof_valid_entries++].max_burst = ofp_table->rates[ofp_i].max_burst;
      }
    }
  }

  for (ofp_i = ofp_rate_tbl->nof_valid_entries; ofp_i < SOC_PETRA_NOF_FAP_PORTS; ++ofp_i)
  {
    soc_petra_PETRA_OFP_RATE_INFO_clear(ofp_rate_tbl->rates + ofp_i);
  }

exit:
  SOC_PETRA_FREE(ofp_table);
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_ofp_all_ofp_rates_get_unsafe()",mal_i,0);
}

uint32
  soc_petra_ofp_all_ofp_rates_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint8                    get_exact,
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_TBL_INFO     *ofp_rate_tbl
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_ALL_OFP_RATES_GET_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(ofp_all_ofp_rates_get_unsafe,(unit, get_exact, ofp_rate_tbl));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_all_ofp_rates_get_unsafe()",0,0);
}

/*********************************************************************
*     Configures a single Outgoing FAP Port (OFP) rate, in the
*     end-to-end scheduler and in the egress processor, by
*     setting the calendars, shapers etc. The function
*     re-calculates the appropriate values from the current
*     values and the updated info. It also saves the values in
*     the software database for single-entry changes in the
*     future. For Network Interfaces, The configuration is per
*     MAC Lane. This indicates a single interface for
*     SPAUI/XAUI, or 4 SGMII interfaces.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pa_ofp_rates_single_port_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    ofp_ndx,
    SOC_SAND_IN  SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *shaper,
    SOC_SAND_IN  SOC_PETRA_OFP_RATE_INFO          *ofp_rate
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    ofp_idx,
    mal_idx;
  SOC_PETRA_OFP_RATES_TBL_INFO
    *ofp_rate_tbl = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_OFP_RATES_SINGLE_PORT_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(shaper);
  SOC_SAND_CHECK_NULL_INPUT(ofp_rate);

  /*
   *  Allocate and initialize ofp rates tables
   */
  SOC_PETRA_ALLOC_AND_CLEAR_STRUCT(ofp_rate_tbl, PETRA_OFP_RATES_TBL_INFO);

  /*
   *  Get the MAL index of the requested port
   */
  res = soc_petra_port_ofp_mal_get_unsafe(
          unit,
          ofp_ndx,
          &mal_idx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  /*
   *  Get current configuration as was requested from
   *  the software database
   *  Note: the table is build so that entry index is OFP index
   *  The rest of the function code relies on it.
   */
  soc_petra_PETRA_OFP_RATES_TBL_INFO_clear(ofp_rate_tbl);
  res = soc_petra_ofp_rates_sw_db_get(
          unit,
          mal_idx,
          ofp_rate_tbl,
          TRUE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  for (ofp_idx = 0; ofp_idx < ofp_rate_tbl->nof_valid_entries; ++ofp_idx)
  {
    if (ofp_rate_tbl->rates[ofp_idx].port_id == ofp_ndx)
    {
      break;
    }
  }

  ofp_rate_tbl->nof_valid_entries += (ofp_idx == ofp_rate_tbl->nof_valid_entries ? 1 : 0);

  SOC_PETRA_COPY(ofp_rate_tbl->rates + ofp_idx, ofp_rate, SOC_PETRA_OFP_RATE_INFO, 1);

  res = soc_petra_ofp_rates_set_unsafe(
          unit,
          mal_idx,
          shaper,
          ofp_rate_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_PETRA_FREE(ofp_rate_tbl);
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_ofp_rates_single_port_set_unsafe()",ofp_ndx,0);
}

uint32
  soc_petra_ofp_rates_single_port_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    ofp_ndx,
    SOC_SAND_IN  SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *shaper,
    SOC_SAND_IN  SOC_PETRA_OFP_RATE_INFO          *ofp_rate
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_RATES_SINGLE_PORT_SET_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(ofp_rates_single_port_set_unsafe,(unit, ofp_ndx, shaper, ofp_rate));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_single_port_set_unsafe()",0,0);
}

uint32
  soc_petra_ofp_rates_single_port_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    ofp_ndx,
    SOC_SAND_IN  SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *shaper,
    SOC_SAND_IN  SOC_PETRA_OFP_RATE_INFO          *ofp_rate
  )
{
  uint32
    res = SOC_SAND_OK;
  /* The stucture SOC_PETRA_OFP_RATES_TBL_INFO is very big */
  SOC_PETRA_OFP_RATES_TBL_INFO
    *rates_tbl = NULL;
  SOC_PETRA_OFP_RATES_MAL_SHPR_INFO
    dummy_shaper;
  uint32
    mal_idx,
    idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_RATES_UPDATE_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(ofp_rate);

  soc_petra_PETRA_OFP_RATES_MAL_SHPR_INFO_clear(&dummy_shaper);

  if (ofp_rate->port_id != ofp_ndx)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_OFP_RATES_OFP_INDEX_MISMATCH_ERR, 10, exit)
  }

  res = soc_petra_port_ofp_mal_get_unsafe(
          unit,
          ofp_ndx,
          &mal_idx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  if (mal_idx == SOC_PETRA_IF_ID_NONE)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_OFP_RATES_PORT_HAS_NO_IF_ERR, 33, exit);
  }

  SOC_PETRA_ALLOC(rates_tbl, SOC_PETRA_OFP_RATES_TBL_INFO, 1);
  soc_petra_PETRA_OFP_RATES_TBL_INFO_clear(rates_tbl);
  
  res = soc_petra_ofp_rates_sw_db_get(
          unit,
          mal_idx,
          rates_tbl,
          TRUE
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  for (idx = 0; idx < rates_tbl->nof_valid_entries; ++idx)
  {
    if (rates_tbl->rates[idx].port_id == ofp_ndx)
    {
      break;
    }
  }

  rates_tbl->nof_valid_entries += (idx == rates_tbl->nof_valid_entries ? 1 : 0);

  SOC_PETRA_COPY(rates_tbl->rates + idx, ofp_rate, SOC_PETRA_OFP_RATE_INFO, 1);

  res = soc_petra_ofp_rates_verify(
          unit,
          mal_idx,
          shaper,
          rates_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

exit:
  SOC_PETRA_FREE(rates_tbl);
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_single_port_verify()",ofp_ndx,0);
}

/*********************************************************************
*     Get a single Outgoing FAP Port (OFP) rate/burst
*     configuration.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pa_ofp_rates_single_port_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    ofp_ndx,
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *shaper,
    SOC_SAND_OUT SOC_PETRA_OFP_RATE_INFO          *ofp
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PETRA_OFP_RATES_TBL_INFO
    *ofp_rate_tbl = NULL;
  uint32
    entry_i,
    mal_idx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_OFP_RATES_SINGLE_PORT_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(ofp);

  /*
   *  Allocate and initialize ofp rates tables
   */
  SOC_PETRA_ALLOC_AND_CLEAR_STRUCT(ofp_rate_tbl, PETRA_OFP_RATES_TBL_INFO);

  soc_petra_PETRA_OFP_RATE_INFO_clear(ofp);
  ofp->port_id = ofp_ndx;

  /*
   *  Get the MAL index of the requested port
   */
  res = soc_petra_port_ofp_mal_get_unsafe(
          unit,
          ofp_ndx,
          &mal_idx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_petra_ofp_rates_get_unsafe(
          unit,
          mal_idx,
          shaper,
          ofp_rate_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  for (entry_i = 0; entry_i < ofp_rate_tbl->nof_valid_entries; ++entry_i)
  {
    if (ofp_rate_tbl->rates[entry_i].port_id == ofp_ndx)
    {
      ofp->port_id = ofp_rate_tbl->rates[entry_i].port_id;
      ofp->sch_rate = ofp_rate_tbl->rates[entry_i].sch_rate;
      ofp->egq_rate = ofp_rate_tbl->rates[entry_i].egq_rate;
      ofp->max_burst = ofp_rate_tbl->rates[entry_i].max_burst;

      SOC_SAND_SET_ERROR_CODE(SOC_SAND_OK, 22, exit);
    }
  }

exit:
  SOC_PETRA_FREE(ofp_rate_tbl);
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_ofp_rates_single_port_get_unsafe()",ofp_ndx,0);
}

uint32
  soc_petra_ofp_rates_single_port_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 ofp_ndx,
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *shaper,
    SOC_SAND_OUT SOC_PETRA_OFP_RATE_INFO       *ofp
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_RATES_SINGLE_PORT_GET_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(ofp_rates_single_port_get_unsafe,(unit, ofp_ndx, shaper, ofp));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_single_port_get_unsafe()",0,0);
}

/*********************************************************************
*     Configure MAL-level shaping (rate and burst) for the EGQ.
*     This is required when the
*     shaping rate is different from the accumulated rate of
*     the OFP-s mapped to the NIF.
*     Note: both calendars (active and inactive) are configured
*********************************************************************/

STATIC uint32
  soc_petra_ofp_rates_egq_shaper_config(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    mal_ndx,
    SOC_SAND_IN  uint8                    rate_enable,
    SOC_SAND_IN  uint32                     rate,
    SOC_SAND_OUT uint32                     *exact_rate
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    fld_val;
  uint32
    cal_set;
  SOC_PETRA_REG_FIELD
    *egq_shpr_rate_fld,
    *egq_shpr_burst_fld;
  SOC_PETRA_MAL_EQUIVALENT_TYPE
    mal_type;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_RATES_EGQ_SHAPER_CONFIG);

  regs = soc_petra_regs();

  mal_type = soc_petra_mal_type_from_id(mal_ndx);

  switch(mal_type) {
  case SOC_PETRA_MAL_TYPE_NIF:
    egq_shpr_rate_fld = SOC_PA_REG_DB_ACC_REF(regs->egq.egress_shpr_conf_nif_reg[mal_ndx].spr_rate);
    egq_shpr_burst_fld = SOC_PA_REG_DB_ACC_REF(regs->egq.egress_shpr_conf_nif_reg[mal_ndx].spr_max_burst);
    break;
  case SOC_PETRA_MAL_TYPE_CPU:
    egq_shpr_rate_fld = SOC_PA_REG_DB_ACC_REF(regs->egq.egress_shpr_conf_for_eci_ports_reg.cpu_spr_rate);
    egq_shpr_burst_fld = SOC_PA_REG_DB_ACC_REF(regs->egq.egress_shpr_conf_for_eci_ports_reg.cpu_spr_max_burst);
    break;
  case SOC_PETRA_MAL_TYPE_RCY:
    egq_shpr_rate_fld = SOC_PA_REG_DB_ACC_REF(regs->egq.egress_shpr_conf_for_recycle_interface_ports_reg.rcy_spr_rate);
    egq_shpr_burst_fld = SOC_PA_REG_DB_ACC_REF(regs->egq.egress_shpr_conf_for_recycle_interface_ports_reg.rcy_spr_max_burst);
    break;
  case SOC_PETRA_MAL_TYPE_ERP:
    SOC_PETRA_DO_NOTHING_AND_EXIT;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_OFP_RATES_MAL_TYPE_INVALID_ERR, 10, exit);
  }

  if (rate_enable)
  {
    res = soc_petra_ofp_egq_shaper_rate_to_internal(
            unit,
            rate,
            &fld_val
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    for (cal_set = SOC_PETRA_OFP_RATES_CAL_SET_A; cal_set < SOC_PETRA_OFP_NOF_RATES_CAL_SETS; ++cal_set)
    {
      /* Set shaper rate */
      SOC_PA_IMPLICIT_FLD_ISET(*egq_shpr_rate_fld, fld_val, cal_set, 30, exit);
      /* Set shaper max burst */
      SOC_PA_IMPLICIT_FLD_ISET(*egq_shpr_burst_fld, 0x3ff, cal_set, 30, exit);
    }
  }

  /*
   *  Get the exact values
   */
  SOC_PA_IMPLICIT_FLD_IGET(*egq_shpr_rate_fld, fld_val, SOC_PETRA_OFP_RATES_CAL_SET_A, 50, exit);

  res = soc_petra_ofp_egq_shaper_rate_from_internal(
          unit,
          fld_val,
          exact_rate
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_egq_shaper_config()",mal_ndx,0);
}

/*********************************************************************
*     Retrieve MAL-level shaping (rate and burst) from the EGQ.
*********************************************************************/
STATIC uint32
  soc_petra_ofp_rates_egq_shaper_retrieve(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    mal_ndx,
    SOC_SAND_IN  uint8                    rate_enable,
    SOC_SAND_OUT  uint32                    *rate
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    fld_val;
  SOC_PETRA_REG_FIELD
    *egq_shpr_rate_fld;
  SOC_PETRA_MAL_EQUIVALENT_TYPE
    mal_type;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_RATES_EGQ_SHAPER_RETRIEVE);

  regs = soc_petra_regs();

  mal_type = soc_petra_mal_type_from_id(mal_ndx);

  switch(mal_type) {
  case SOC_PETRA_MAL_TYPE_NIF:
    egq_shpr_rate_fld = SOC_PA_REG_DB_ACC_REF(regs->egq.egress_shpr_conf_nif_reg[mal_ndx].spr_rate);
    break;
  case SOC_PETRA_MAL_TYPE_CPU:
  case SOC_PETRA_MAL_TYPE_OLP:
    egq_shpr_rate_fld = SOC_PA_REG_DB_ACC_REF(regs->egq.egress_shpr_conf_for_eci_ports_reg.cpu_spr_rate);
    break;
  case SOC_PETRA_MAL_TYPE_RCY:
    egq_shpr_rate_fld = SOC_PA_REG_DB_ACC_REF(regs->egq.egress_shpr_conf_for_recycle_interface_ports_reg.rcy_spr_rate);
    break;
  case SOC_PETRA_MAL_TYPE_ERP:
    SOC_PETRA_DO_NOTHING_AND_EXIT;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_OFP_RATES_MAL_TYPE_INVALID_ERR, 10, exit);
  }

  if (rate_enable)
  {
    SOC_PA_IMPLICIT_FLD_IGET(*egq_shpr_rate_fld, fld_val, SOC_PETRA_OFP_RATES_CAL_SET_A, 30, exit);

    res = soc_petra_ofp_egq_shaper_rate_from_internal(
            unit,
            fld_val,
            rate
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_egq_shaper_retrieve()",mal_ndx,0);
}

/*********************************************************************
*     Configure MAL-level shaping. This is required when the
*     shaping rate is different from the accumulated rate of
*     the OFP-s mapped to the NIF.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pa_ofp_rates_mal_shaper_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    mal_ndx,
    SOC_SAND_IN  SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *shaper,
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *exact_shaper
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    sch_rate_to_set = 0,
    egq_rate_to_set = 0,
    sch_rate_all_ports = 0,
    egq_rate_all_ports = 0,
    egq_burst_all_ports = 0;
  uint32
    ofp_idx;
  uint8
    sch_rate_enable = FALSE,
    egq_rate_enable = FALSE;
  SOC_PETRA_SW_DB_DEV_EGR_MAL
    *mal_ports_db = NULL;
  SOC_PETRA_INTERFACE_ID
    if_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_RATES_MAL_SHAPER_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(shaper);
  SOC_SAND_CHECK_NULL_INPUT(exact_shaper);

  SOC_PETRA_ALLOC(mal_ports_db, SOC_PETRA_SW_DB_DEV_EGR_MAL, 1);

  if_id = SOC_PETRA_MAL2IF_NDX(mal_ndx);

  if (mal_ndx == SOC_PETRA_MAL_ID_ERP)
  {
    sch_rate_all_ports = shaper->sch_shaper.rate;
  }
  else
  {
    res = soc_petra_sw_db_egr_ports_mal_get(
            unit,
            mal_ndx,
            mal_ports_db
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 5, exit);
  
    for (ofp_idx = 0; ofp_idx < SOC_PETRA_NOF_FAP_PORTS; ++ofp_idx)
    {
      if (mal_ports_db->rates[ofp_idx].valid)
      {
        sch_rate_all_ports += mal_ports_db->rates[ofp_idx].sch_rates;
        egq_rate_all_ports += mal_ports_db->rates[ofp_idx].egq_rates;
        egq_burst_all_ports += mal_ports_db->rates[ofp_idx].egq_bursts;
      }
    }
  }

  switch(shaper->sch_shaper.rate_update_mode) {
  case SOC_PETRA_OFP_SHPR_UPDATE_MODE_OVERRIDE:
    sch_rate_to_set = shaper->sch_shaper.rate;
    sch_rate_enable = TRUE;
    break;
  case SOC_PETRA_OFP_SHPR_UPDATE_MODE_SUM_OF_PORTS:
    sch_rate_to_set = sch_rate_all_ports;
    sch_rate_enable = TRUE;
    break;
  case SOC_PETRA_OFP_SHPR_UPDATE_MODE_DONT_TUCH:
    sch_rate_enable = FALSE;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_OFP_SHPR_MODE_OUT_OF_RANGE_ERR, 10, exit)
  }

  switch(shaper->egq_shaper.rate_update_mode) {
  case SOC_PETRA_OFP_SHPR_UPDATE_MODE_OVERRIDE:
    egq_rate_to_set = shaper->egq_shaper.rate;
    egq_rate_enable = TRUE;
    break;
  case SOC_PETRA_OFP_SHPR_UPDATE_MODE_SUM_OF_PORTS:
    egq_rate_to_set = egq_rate_all_ports;
    egq_rate_enable = TRUE;
    break;
  case SOC_PETRA_OFP_SHPR_UPDATE_MODE_DONT_TUCH:
    egq_rate_enable = FALSE;
    break;
  default:
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_OFP_SHPR_MODE_OUT_OF_RANGE_ERR, 20, exit)
  }

  if (sch_rate_enable)
  {
    res = soc_petra_sch_if_shaper_rate_set_unsafe(
            unit,
            if_id,
            sch_rate_to_set,
            &(exact_shaper->sch_shaper.rate)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
  }

  SOC_SAND_ERR_IF_ABOVE_NOF(mal_ndx, SOC_PETRA_BLK_NOF_INSTANCES_MAL, SOC_PETRA_NIF_MAL_INDEX_OUT_OF_RANGE_ERR, 45, exit);

  res = soc_petra_ofp_rates_egq_shaper_config(
          unit,
          mal_ndx,
          egq_rate_enable,
          egq_rate_to_set,
          &(exact_shaper->egq_shaper.rate)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

exit:
  SOC_PETRA_FREE(mal_ports_db);
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_mal_shaper_set_unsafe()",mal_ndx,0);
}

uint32
  soc_petra_ofp_rates_mal_shaper_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_IN  SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *shaper,
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *exact_shaper
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SYS_PHYS_TO_LOCAL_PORT_MAP_SET_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(ofp_rates_mal_shaper_set_unsafe,(unit, mal_ndx, shaper, exact_shaper));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_mal_shaper_set_unsafe()",0,0);
}

/*********************************************************************
*     Configure MAL-level shaping. This is required when the
*     shaping rate is different from the accumulated rate of
*     the OFP-s mapped to the NIF.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ofp_rates_mal_shaper_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    mal_ndx,
    SOC_SAND_IN  SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *shaper
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_RATES_MAL_SHAPER_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(shaper);
  SOC_SAND_MAGIC_NUM_VERIFY(shaper);

  res = soc_petra_mal_equivalent_id_verify(
          unit,
          mal_ndx
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  SOC_SAND_ERR_IF_ABOVE_MAX(
    shaper->sch_shaper.rate_update_mode, SOC_PETRA_OFP_NOF_SHPR_UPDATE_MODES,
    SOC_PETRA_OFP_SHPR_MODE_OUT_OF_RANGE_ERR, 20, exit
  );

  SOC_SAND_ERR_IF_ABOVE_MAX(
    shaper->egq_shaper.rate_update_mode, SOC_PETRA_OFP_NOF_SHPR_UPDATE_MODES,
    SOC_PETRA_OFP_SHPR_MODE_OUT_OF_RANGE_ERR, 30, exit
  );

  if (shaper->sch_shaper.rate_update_mode == SOC_PETRA_OFP_SHPR_UPDATE_MODE_OVERRIDE)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      shaper->sch_shaper.rate, SOC_PETRA_IF_MAX_RATE_KBPS,
      SOC_PETRA_OFP_RATES_SCH_RATE_OUT_OF_RANGE_ERR, 50, exit
    );
  }

  if (shaper->egq_shaper.rate_update_mode == SOC_PETRA_OFP_SHPR_UPDATE_MODE_OVERRIDE)
  {
    SOC_SAND_ERR_IF_ABOVE_MAX(
      shaper->egq_shaper.rate, SOC_PETRA_IF_MAX_RATE_KBPS,
      SOC_PETRA_OFP_RATES_EGQ_RATE_OUT_OF_RANGE_ERR, 60, exit
    );
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_mal_shaper_verify()",mal_ndx,0);
}

/*********************************************************************
*     Configure MAL-level shaping. This is required when the
*     shaping rate is different from the accumulated rate of
*     the OFP-s mapped to the NIF.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pa_ofp_rates_mal_shaper_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    mal_ndx,
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *shaper
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    sch_rate_to_get = 0,
    egq_rate_to_get = 0;
  SOC_PETRA_INTERFACE_ID
    if_id;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_OFP_RATES_MAL_SHAPER_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(shaper);

  if_id = SOC_PETRA_MAL2IF_NDX(mal_ndx);

  res = soc_petra_sch_if_shaper_rate_get_unsafe(
          unit,
          if_id,
          &sch_rate_to_get
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  SOC_SAND_ERR_IF_ABOVE_NOF(mal_ndx, SOC_PETRA_BLK_NOF_INSTANCES_MAL, SOC_PETRA_NIF_MAL_INDEX_OUT_OF_RANGE_ERR, 45, exit);

  res = soc_petra_ofp_rates_egq_shaper_retrieve(
          unit,
          mal_ndx,
          TRUE,
          &egq_rate_to_get
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  shaper->sch_shaper.rate = sch_rate_to_get;
  shaper->egq_shaper.rate = egq_rate_to_get;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_ofp_rates_mal_shaper_get_unsafe()",mal_ndx,0);
}

uint32
  soc_petra_ofp_rates_mal_shaper_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 mal_ndx,
    SOC_SAND_OUT SOC_PETRA_OFP_RATES_MAL_SHPR_INFO *shaper
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_SYS_PHYS_TO_LOCAL_PORT_MAP_SET_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(ofp_rates_mal_shaper_get_unsafe,(unit, mal_ndx, shaper));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_mal_shaper_get_unsafe()",0,0);
}

/*********************************************************************
*     Configures Fat pipe rate, in the end-to-end scheduler
*     and in the egress processor, by setting the calendars,
*     shapers etc. The function calculates from the given
*     table the calendars granularity, writes it to the device
*     and changes the active calendars. It also saves the
*     values in the software database for single-entry changes
*     in the future.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ofp_fat_pipe_rate_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PETRA_OFP_FAT_PIPE_RATE_INFO *fatp_rate
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    sch_per_port_rate,
    egq_per_port_rate,
    egq_per_port_burst;
  uint32
    port_idx,
    nof_fatp_ports,
    mal_idx;
  SOC_PETRA_PORT2IF_MAPPING_INFO
    map_info,
    in_map_info;
  SOC_PETRA_OFP_RATES_MAL_SHPR_INFO
    *shaper = NULL;
  SOC_PETRA_OFP_RATES_TBL_INFO
    *ofp_rate_tbl = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_FAT_PIPE_RATE_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(fatp_rate);

  SOC_PETRA_ALLOC_AND_CLEAR_STRUCT(shaper, PETRA_OFP_RATES_MAL_SHPR_INFO);
  SOC_PETRA_ALLOC_AND_CLEAR_STRUCT(ofp_rate_tbl, PETRA_OFP_RATES_TBL_INFO);

  soc_petra_PETRA_PORT2IF_MAPPING_INFO_clear(&map_info);
  soc_petra_PETRA_PORT2IF_MAPPING_INFO_clear(&in_map_info);

  nof_fatp_ports = soc_petra_sw_db_fat_pipe_nof_ports_get(unit);

  if (nof_fatp_ports == 0)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_FAT_PIPE_NOT_SET_ERR, 10, exit);
  }

  egq_per_port_rate  = fatp_rate->egq_rate / nof_fatp_ports;
  egq_per_port_burst = fatp_rate->max_burst / nof_fatp_ports;

  /*
   *  For all ports
   */

  shaper->sch_shaper.rate_update_mode = SOC_PETRA_OFP_SHPR_UPDATE_MODE_SUM_OF_PORTS;

  shaper->egq_shaper.rate_update_mode = SOC_PETRA_OFP_SHPR_UPDATE_MODE_SUM_OF_PORTS;
  shaper->egq_shaper.rate = egq_per_port_rate;

  /*
   *  Configure one entry at a time
   */
  ofp_rate_tbl->nof_valid_entries = 1;

  for (port_idx = SOC_PETRA_FAT_PIPE_FAP_PORT_ID; port_idx < SOC_PETRA_FAT_PIPE_FAP_PORT_ID + nof_fatp_ports; ++port_idx)
  {
    sch_per_port_rate = SOC_PETRA_IS_FAT_PIPE_FIRST(port_idx)?fatp_rate->sch_rate:0;

    res = soc_petra_port_to_interface_map_get_unsafe(
            unit,
            port_idx,
            &in_map_info,
            &map_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    res = soc_petra_interface_id_verify(unit, map_info.if_id);
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    mal_idx = SOC_PETRA_NIF2MAL_NDX(map_info.if_id);

    shaper->sch_shaper.rate = sch_per_port_rate;

    ofp_rate_tbl->rates[0].port_id   = port_idx;
    ofp_rate_tbl->rates[0].sch_rate  = sch_per_port_rate;
    ofp_rate_tbl->rates[0].egq_rate  = egq_per_port_rate;
    ofp_rate_tbl->rates[0].max_burst = egq_per_port_burst;

    res = soc_petra_ofp_rates_verify(
            unit,
            mal_idx,
            shaper,
            ofp_rate_tbl
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    res = soc_petra_ofp_rates_set_unsafe(
            unit,
            mal_idx,
            shaper,
            ofp_rate_tbl
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);
  }

exit:
  SOC_PETRA_FREE(shaper);
  SOC_PETRA_FREE(ofp_rate_tbl);
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_fat_pipe_rate_set_unsafe()",0,0);
}

/*********************************************************************
*     Configures Fat pipe rate, in the end-to-end scheduler
*     and in the egress processor, by setting the calendars,
*     shapers etc. The function calculates from the given
*     table the calendars granularity, writes it to the device
*     and changes the active calendars. It also saves the
*     values in the software database for single-entry changes
*     in the future.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ofp_fat_pipe_rate_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PETRA_OFP_FAT_PIPE_RATE_INFO *fatp_rate
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_FAT_PIPE_RATE_VERIFY);

  SOC_SAND_CHECK_NULL_INPUT(fatp_rate);

  SOC_SAND_MAGIC_NUM_VERIFY(fatp_rate);

  /*
   *  The rest is verified in the _unsafe function
   */
  SOC_PETRA_DO_NOTHING_AND_EXIT;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_fat_pipe_rate_verify()",0,0);
}

/*********************************************************************
*     Configures Fat pipe rate, in the end-to-end scheduler
*     and in the egress processor, by setting the calendars,
*     shapers etc. The function calculates from the given
*     table the calendars granularity, writes it to the device
*     and changes the active calendars. It also saves the
*     values in the software database for single-entry changes
*     in the future.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ofp_fat_pipe_rate_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_OUT SOC_PETRA_OFP_FAT_PIPE_RATE_INFO *fatp_rate
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    nof_fatp_ports,
    mal_idx;
  SOC_PETRA_PORT2IF_MAPPING_INFO
    map_info,
    in_map_info;
  SOC_PETRA_OFP_RATES_MAL_SHPR_INFO
    *shaper = NULL;
  SOC_PETRA_OFP_RATES_TBL_INFO
    *ofp_rate_tbl = NULL;
  SOC_PETRA_OFP_RATES_MAL_SHPR_INFO
    *exact_shaper = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_FAT_PIPE_RATE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(fatp_rate);

  soc_petra_PETRA_OFP_FAT_PIPE_RATE_INFO_clear(fatp_rate);

  SOC_PETRA_ALLOC_AND_CLEAR_STRUCT(shaper, PETRA_OFP_RATES_MAL_SHPR_INFO);
  SOC_PETRA_ALLOC_AND_CLEAR_STRUCT(ofp_rate_tbl, PETRA_OFP_RATES_TBL_INFO);
  SOC_PETRA_ALLOC_AND_CLEAR_STRUCT(exact_shaper, PETRA_OFP_RATES_MAL_SHPR_INFO);

  soc_petra_PETRA_PORT2IF_MAPPING_INFO_clear(&map_info);
  soc_petra_PETRA_PORT2IF_MAPPING_INFO_clear(&in_map_info);

  nof_fatp_ports = soc_petra_sw_db_fat_pipe_nof_ports_get(unit);

  if (nof_fatp_ports == 0)
  {
    SOC_PETRA_DO_NOTHING_AND_EXIT;
  }

  res = soc_petra_port_to_interface_map_get_unsafe(
          unit,
          SOC_PETRA_FAT_PIPE_FAP_PORT_ID,
          &in_map_info,
          &map_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_petra_interface_id_verify(unit, map_info.if_id);
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  mal_idx = SOC_PETRA_NIF2MAL_NDX(map_info.if_id);

  res = soc_petra_ofp_rates_get_unsafe(
          unit,
          mal_idx,
          shaper,
          ofp_rate_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  if (ofp_rate_tbl->nof_valid_entries != 1)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_FAT_PIPE_MULTIPLE_PORTS_ERR, 60, exit);
  }
  if (ofp_rate_tbl->rates[0].port_id != SOC_PETRA_FAT_PIPE_FAP_PORT_ID)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PETRA_FAT_PIPE_MUST_BE_FIRST_ERR, 70, exit);
  }

  fatp_rate->sch_rate = ofp_rate_tbl->rates[0].sch_rate;
  fatp_rate->egq_rate = ofp_rate_tbl->rates[0].egq_rate * nof_fatp_ports;
  fatp_rate->max_burst = ofp_rate_tbl->rates[0].max_burst * nof_fatp_ports;

exit:
  SOC_PETRA_FREE(shaper);
  SOC_PETRA_FREE(ofp_rate_tbl);
  SOC_PETRA_FREE(exact_shaper);
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_fat_pipe_rate_get_unsafe()",0,0);
}


uint32
  soc_pa_ofp_rates_egq_calendar_validate_unsafe(
    SOC_SAND_IN  int                    unit
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    egq_calcal_passes = 0,
    curr_port_index = SOC_PETRA_OFP_RATES_INVALID_PORT_ID,
    last_port_index = SOC_PETRA_OFP_RATES_INVALID_PORT_ID,
    idx = 0;
  uint32
    egq_calcal_calendar_len,
    curr_port_crdit = 0,
    last_port_crdit = 0,
    act_cal;
  SOC_PETRA_EGQ_CCM_TBL_DATA
    ccm_tbl_data;
  SOC_PETRA_OFP_RATES_CAL_EGQ
    *egq_calendar = NULL,
    *egq_cal_ptr = NULL;
  SOC_PETRA_OFP_EGQ_RATES_CAL_ENTRY
    *egq_cal_entry_ptr = NULL;
  SOC_PETRA_REGS
    *regs = soc_petra_regs();
  uint8
    next_time_stop;
  uint32
    *egq_cal_lens = NULL,
    *current_zero = NULL,
    *current_slot = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_OFP_RATES_EGQ_CALENDAR_VALIDATE_UNSAFE);

  SOC_PETRA_ALLOC(egq_cal_lens, uint32, SOC_PETRA_OFP_RATES_EGQ_NOF_MALS);
  SOC_PETRA_ALLOC(current_zero, uint32, SOC_PETRA_OFP_RATES_EGQ_NOF_MALS);
  SOC_PETRA_ALLOC(current_slot, uint32, SOC_PETRA_OFP_RATES_EGQ_NOF_MALS);
  SOC_PETRA_ALLOC(egq_calendar, SOC_PETRA_OFP_RATES_CAL_EGQ, SOC_PETRA_OFP_RATES_EGQ_NOF_MALS);

  for (idx = 0; idx < SOC_PETRA_OFP_RATES_EGQ_NOF_MALS; ++idx)
  {
    res = soc_petra_ofp_rates_active_calendars_retrieve_egq(
            unit,
            soc_petra_ofp_rates_ccmmal2mal(idx),
            egq_calendar + idx,
            egq_cal_lens + idx,
            FALSE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 11, exit);
  }

  SOC_PETRA_FLD_GET(regs->egq.egress_shaper_enable_settings_reg.spr_set_sel, act_cal, 50, exit);
  SOC_PETRA_FLD_GET(regs->egq.egress_shaper_calendars_arbitration_cycle_length_reg.cal_cal_len[act_cal], egq_calcal_calendar_len, 65, exit);
  egq_calcal_calendar_len += 1;

  for (next_time_stop = FALSE, egq_calcal_passes = 1; ; ++egq_calcal_passes)
  {
    for (idx = 0; idx < egq_calcal_calendar_len; ++idx)
    {
      res = soc_petra_egq_ccm_tbl_get_unsafe(
              unit,
              idx + (act_cal) * SOC_PETRA_OFP_RATES_CALCAL_LEN_EGQ_MAX,
              &ccm_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 13, exit);

      egq_cal_ptr = egq_calendar + ccm_tbl_data.interface_select;
      egq_cal_entry_ptr = egq_cal_ptr->slots + current_slot[ccm_tbl_data.interface_select];

      curr_port_index = egq_cal_entry_ptr->port_idx;
      curr_port_crdit = egq_cal_entry_ptr->credit;

      if ((curr_port_index == last_port_index) && (last_port_crdit > 0) && (curr_port_crdit > 0))
      {
        SOC_SAND_SET_ERROR_CODE(SOC_PETRA_OFP_RATES_CONSECUTIVE_CREDIT_DISTRIBUTION_ERR, 22, exit);
      }

      last_port_index = curr_port_index;
      last_port_crdit = curr_port_crdit;

      current_slot[ccm_tbl_data.interface_select] = (current_slot[ccm_tbl_data.interface_select] + 1) % egq_cal_lens[ccm_tbl_data.interface_select];
    }

    if (!sal_memcmp(current_slot, current_zero, SOC_PETRA_OFP_RATES_EGQ_NOF_MALS * sizeof(uint32)))
    {
      if (next_time_stop)
      {
        break;
      }
      else
      {
        next_time_stop = TRUE;
      }
    }
  }

exit:

  SOC_PETRA_FREE(egq_cal_lens);
  SOC_PETRA_FREE(current_zero);
  SOC_PETRA_FREE(current_slot);
  SOC_PETRA_FREE(egq_calendar);

  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_ofp_rates_egq_calendar_validate_unsafe()", egq_calcal_passes, idx);
}

uint32
  soc_petra_ofp_rates_egq_calendar_validate_unsafe(
    SOC_SAND_IN  int                    unit
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_RATES_EGQ_CALENDAR_VALIDATE_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(ofp_rates_egq_calendar_validate_unsafe,(unit));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_egq_calendar_validate_unsafe()",0,0);
}

/*********************************************************************
*     Update the device after the computation of the
*     calendars.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pa_ofp_rates_update_device_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 is_device_updated
  )
{
  uint8
    is_du_before;
  SOC_PETRA_OFP_RATES_TBL_INFO
    *ofp_rate_tbl = NULL;
  uint32
    cc_mal_id,
    ofp_idx = 0,
    mal_idx = 0;
  uint32
    sch_mal_rate,
    res;
  SOC_PETRA_OFP_RATES_MAL_SHPR_INFO
    shaper,
    exact_shaper;
  SOC_PETRA_SW_DB_DEV_EGR_MAL
    *mal_ports_db = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PA_OFP_RATES_UPDATE_DEVICE_SET_UNSAFE);

  soc_petra_PETRA_OFP_RATES_MAL_SHPR_INFO_clear(&shaper);
  soc_petra_PETRA_OFP_RATES_MAL_SHPR_INFO_clear(&exact_shaper);

  SOC_PETRA_ALLOC_AND_CLEAR_STRUCT(ofp_rate_tbl, PETRA_OFP_RATES_TBL_INFO);
  SOC_PETRA_ALLOC(mal_ports_db, SOC_PETRA_SW_DB_DEV_EGR_MAL, 1);

  is_du_before = soc_petra_sw_db_ofp_rates_update_device_get(unit);
  soc_petra_sw_db_ofp_rates_update_device_set(unit, is_device_updated);

  /*
   *  If the update device indication becomes true, then compute and write
   *  the calendars into the device
   */
  if ((is_du_before == FALSE)
     && (is_device_updated == TRUE)
     )
  {
    soc_petra_ofp_rates_update_dev_changed_set(unit, TRUE);

    soc_petra_PETRA_OFP_RATES_TBL_INFO_clear(ofp_rate_tbl);

    for (mal_idx = 0; mal_idx < SOC_PETRA_OFP_RATES_EGQ_NOF_MALS; ++mal_idx)
    {
      cc_mal_id = soc_petra_ofp_rates_ccmmal2mal(mal_idx);
      res = soc_petra_sw_db_egr_ports_mal_sch_rate_get(
              unit,
              cc_mal_id,
              &sch_mal_rate
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

      res = soc_petra_ofp_rates_mal_shaper_get_unsafe(
              unit,
              cc_mal_id,
              &shaper
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      shaper.sch_shaper.rate_update_mode = SOC_PETRA_OFP_SHPR_UPDATE_MODE_SUM_OF_PORTS;
      shaper.sch_shaper.rate = sch_mal_rate;

      res = soc_petra_ofp_rates_mal_shaper_set_unsafe(
              unit,
              cc_mal_id,
              &shaper,
              &exact_shaper
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    }

    mal_idx = 0;

    res = soc_petra_ofp_rates_sw_db_get(
            unit,
            mal_idx,
            ofp_rate_tbl,
            TRUE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    res = soc_petra_ofp_rates_set_unsafe(
            unit,
            mal_idx,
            &shaper,
            ofp_rate_tbl
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

    soc_petra_ofp_rates_update_dev_changed_set(unit, FALSE);
  }

  SOC_PETRA_DO_NOTHING_AND_EXIT;

exit:
  SOC_PETRA_FREE(ofp_rate_tbl);
  SOC_PETRA_FREE(mal_ports_db);
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pa_ofp_rates_update_device_set_unsafe()",mal_idx,ofp_idx);
}

uint32
  soc_petra_ofp_rates_update_device_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 is_device_updated
  )
{
  uint32
    res;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_RATES_UPDATE_DEVICE_SET_UNSAFE);

  SOC_PETRA_DIFF_DEVICE_CALL(ofp_rates_update_device_set_unsafe,(unit, is_device_updated));

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_update_device_set_unsafe()",0,0);
}

/*********************************************************************
*     Update the device after the computation of the
*     calendars.
*     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_petra_ofp_rates_update_device_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint8                 *is_device_updated
  )
{
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_RATES_UPDATE_DEVICE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(is_device_updated);

  *is_device_updated = soc_petra_sw_db_ofp_rates_update_device_get(unit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_update_device_get_unsafe()",0,0);
}

#if SOC_PETRA_DEBUG

uint32
  soc_petra_ofp_rates_test_random_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint8                    silent
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    mal_idx,
    idx1 = 0,
    idx2 = 0;
  uint32
    init_rate,
    delta_rate_sch,
    delta_rate_egq;
  uint32
    base_sch_rate,
    base_egq_rate;
  SOC_PETRA_OFP_RATES_MAL_SHPR_INFO
    dummy_shaper;
  SOC_PETRA_OFP_RATES_EGQ_MAL_ID
    ccm_idx;
  /* The stucture SOC_PETRA_OFP_RATES_TBL_INFO is very big */
  SOC_PETRA_OFP_RATES_TBL_INFO
    *ofp_rate_tbl_orig = NULL;
  SOC_PETRA_OFP_RATES_TBL_INFO
    *ofp_rate_tbl_set = NULL;
  SOC_PETRA_OFP_RATES_TBL_INFO
    *ofp_rate_tbl_get = NULL;
  SOC_PETRA_OFP_RATES_MAL_SHPR_INFO
    shaper[SOC_PETRA_OFP_RATES_EGQ_NOF_MALS];
  uint8
    sch_enable = Soc_petra_ofp_rates_test_random_sch_enable;
  uint8
    egq_enable = Soc_petra_ofp_rates_test_random_egq_enable;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_OFP_RATES_TEST_RANDOM_UNSAFE);

  if (!silent)
  {
    soc_sand_os_printf("Initializing EGQ rate of all open ports\r\n");
  }

  SOC_PETRA_ALLOC(ofp_rate_tbl_set, SOC_PETRA_OFP_RATES_TBL_INFO, 1);
  SOC_PETRA_ALLOC(ofp_rate_tbl_get, SOC_PETRA_OFP_RATES_TBL_INFO, 1);
  SOC_PETRA_ALLOC(ofp_rate_tbl_orig, SOC_PETRA_OFP_RATES_TBL_INFO, 1);
  soc_petra_PETRA_OFP_RATES_TBL_INFO_clear(ofp_rate_tbl_orig);
  
  res = soc_petra_ofp_all_ofp_rates_get_unsafe(
          unit,
          TRUE,
          ofp_rate_tbl_orig
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  for(ccm_idx = 0; ccm_idx < SOC_PETRA_OFP_RATES_EGQ_NOF_MALS; ++ccm_idx)
  {
    soc_petra_PETRA_OFP_RATES_TBL_INFO_clear(ofp_rate_tbl_get);
    soc_petra_PETRA_OFP_RATES_MAL_SHPR_INFO_clear(&shaper[ccm_idx]);

    /* petra code. Almost not in use. Ignore coveriti defects */
    /* coverity[overrun-call] */
    res = soc_petra_ofp_rates_get_unsafe(
            unit,
            soc_petra_ofp_rates_ccmmal2mal(ccm_idx),
            &shaper[ccm_idx],
            ofp_rate_tbl_get
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit_restore);

    init_rate = SOC_PETRA_OFP_RATES_TEST_RANDOM_INITIAL_PORT_RATE / ofp_rate_tbl_get->nof_valid_entries;

    for (idx1 = 0; idx1 < ofp_rate_tbl_get->nof_valid_entries; ++idx1)
    {
      ofp_rate_tbl_get->rates[idx1].sch_rate = sch_enable ? init_rate : ofp_rate_tbl_get->rates[idx1].sch_rate;
      ofp_rate_tbl_get->rates[idx1].egq_rate = egq_enable ? init_rate : ofp_rate_tbl_get->rates[idx1].egq_rate;

      if (!silent)
      {
        soc_sand_os_printf("Port: %2u CCM %2u sch_rate %8u egq_rate %8u\r\n", ofp_rate_tbl_get->rates[idx1].port_id, ccm_idx, init_rate, init_rate);
      }
    }

    res = soc_petra_ofp_rates_set_unsafe(
            unit,
            soc_petra_ofp_rates_ccmmal2mal(ccm_idx),
            &shaper[ccm_idx],
            ofp_rate_tbl_get
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit_restore);
  }

  base_sch_rate = (uint32)(sal_rand()) % 10000000;
  base_egq_rate = (uint32)(sal_rand()) % 10000000;

  if (!silent)
  {
    soc_sand_os_printf("Setting MAL rates:\r\n");
  }

  for(ccm_idx = 0; ccm_idx < SOC_PETRA_OFP_RATES_EGQ_NOF_MALS; ++ccm_idx)
  {
    SOC_PETRA_OFP_RATES_MAL_SHPR_INFO
      exact_shaper;

    soc_petra_PETRA_OFP_RATES_MAL_SHPR_INFO_clear(&shaper[ccm_idx]);
    soc_petra_PETRA_OFP_RATES_MAL_SHPR_INFO_clear(&exact_shaper);

    res = soc_petra_ofp_rates_mal_shaper_get_unsafe(
            unit,
            soc_petra_ofp_rates_ccmmal2mal(ccm_idx),
            &shaper[ccm_idx]
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit_restore);

    shaper[ccm_idx].sch_shaper.rate_update_mode = SOC_PETRA_OFP_SHPR_UPDATE_MODE_SUM_OF_PORTS;
    shaper[ccm_idx].sch_shaper.rate = sch_enable ? base_sch_rate + (uint32)(sal_rand()) % base_sch_rate : shaper[ccm_idx].sch_shaper.rate;
    shaper[ccm_idx].sch_shaper.rate_update_mode = sch_enable ? SOC_PETRA_OFP_SHPR_UPDATE_MODE_OVERRIDE : shaper[ccm_idx].sch_shaper.rate_update_mode;
    shaper[ccm_idx].egq_shaper.rate_update_mode = SOC_PETRA_OFP_SHPR_UPDATE_MODE_SUM_OF_PORTS;
    shaper[ccm_idx].egq_shaper.rate = egq_enable ? base_egq_rate + (uint32)(sal_rand()) % base_egq_rate : shaper[ccm_idx].egq_shaper.rate;
    shaper[ccm_idx].egq_shaper.rate_update_mode = egq_enable ? SOC_PETRA_OFP_SHPR_UPDATE_MODE_OVERRIDE : shaper[ccm_idx].egq_shaper.rate_update_mode;

    if (!silent)
    {
      soc_sand_os_printf("CCM %2u sch_rate %8u egq_rate %8u\r\n", ccm_idx, shaper[ccm_idx].sch_shaper.rate, shaper[ccm_idx].egq_shaper.rate);
    }

    res = soc_petra_ofp_rates_mal_shaper_set_unsafe(
            unit,
            soc_petra_ofp_rates_ccmmal2mal(ccm_idx),
            &shaper[ccm_idx],
            &exact_shaper
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit_restore);
  }

  if (!silent)
  {
    soc_sand_os_printf("Setting random rates:\r\n");
  }

  soc_petra_PETRA_OFP_RATES_TBL_INFO_clear(ofp_rate_tbl_set);
  for(idx1 = 0; idx1 < SOC_PETRA_NOF_FAP_PORTS; ++idx1)
  {
    res = soc_petra_port_ofp_mal_get_unsafe(
            unit,
            idx1,
            &mal_idx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 85, exit_restore);

    if (mal_idx != SOC_PETRA_IF_ID_NONE)
    {
      ccm_idx = soc_petra_ofp_rates_mal2ccmmal(mal_idx);

      res = soc_petra_ofp_rates_single_port_get_unsafe(
              unit,
              idx1,
              &dummy_shaper,
              &ofp_rate_tbl_set->rates[idx1]
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit_restore);

      ofp_rate_tbl_set->rates[idx1].sch_rate  = sch_enable ? (uint32)(sal_rand()) % shaper[ccm_idx].sch_shaper.rate : ofp_rate_tbl_set->rates[idx1].sch_rate;
      ofp_rate_tbl_set->rates[idx1].egq_rate  = egq_enable ? (uint32)(sal_rand()) % shaper[ccm_idx].egq_shaper.rate : ofp_rate_tbl_set->rates[idx1].egq_rate;
      ofp_rate_tbl_set->rates[idx1].port_id   = idx1;
      ofp_rate_tbl_set->rates[idx1].max_burst = SOC_PETRA_OFP_RATES_BURST_LIMIT_MAX;

      if (!silent)
      {
        soc_sand_os_printf("Port: %2u CCM %2u sch_rate %8u egq_rate %8u\r\n", idx1, ccm_idx, ofp_rate_tbl_set->rates[idx1].sch_rate, ofp_rate_tbl_set->rates[idx1].egq_rate);
      }

      shaper[ccm_idx].sch_shaper.rate_update_mode = SOC_PETRA_OFP_SHPR_UPDATE_MODE_SUM_OF_PORTS;
      shaper[ccm_idx].sch_shaper.rate -= sch_enable ? ofp_rate_tbl_set->rates[idx1].sch_rate : 0;
      shaper[ccm_idx].egq_shaper.rate -= egq_enable ? ofp_rate_tbl_set->rates[idx1].egq_rate : 0;

      res = soc_petra_ofp_rates_single_port_set_unsafe(
              unit,
              idx1,
              &shaper[ccm_idx],
              &ofp_rate_tbl_set->rates[idx1]
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit_restore);

      soc_petra_PETRA_OFP_RATES_TBL_INFO_clear(ofp_rate_tbl_get);
      res = soc_petra_ofp_all_ofp_rates_get_unsafe(
              unit,
              FALSE,
              ofp_rate_tbl_get
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit_restore);

      for(idx2 = 0; idx2 < ofp_rate_tbl_get->nof_valid_entries; ++idx2)
      {
        delta_rate_sch = delta_rate_egq = 0;
        res = soc_petra_port_ofp_mal_get_unsafe(
                unit,
                ofp_rate_tbl_get->rates[idx2].port_id,
                &mal_idx
              );
        SOC_SAND_CHECK_FUNC_RESULT(res, 85, exit_restore);

        if (mal_idx != SOC_PETRA_IF_ID_NONE)
        {
          if (ofp_rate_tbl_set->rates[ofp_rate_tbl_get->rates[idx2].port_id].port_id != SOC_PETRA_OFP_RATES_ILLEGAL_PORT_ID)
          {
            delta_rate_sch = sch_enable ? 1000 * SOC_SAND_DELTA(ofp_rate_tbl_set->rates[ofp_rate_tbl_get->rates[idx2].port_id].sch_rate, ofp_rate_tbl_get->rates[idx2].sch_rate) / ofp_rate_tbl_set->rates[ofp_rate_tbl_get->rates[idx2].port_id].sch_rate : 0;
            delta_rate_egq = egq_enable ? 1000 * SOC_SAND_DELTA(ofp_rate_tbl_set->rates[ofp_rate_tbl_get->rates[idx2].port_id].egq_rate, ofp_rate_tbl_get->rates[idx2].egq_rate) / ofp_rate_tbl_set->rates[ofp_rate_tbl_get->rates[idx2].port_id].egq_rate : 0;
            
            if (
                (delta_rate_sch > SOC_PETRA_OFP_RATES_TST_MAX_SCH_DELTA_PERCENT) ||
                (delta_rate_egq > SOC_PETRA_OFP_RATES_TST_MAX_EGQ_DELTA_PERCENT)
               )
            {
              soc_sand_os_printf(
                "Actual and expected rate difference above maximal allowed:\n\r"
                "SCH actual: %u, requested: %u, delta: %u%%\n\r"
                "EGQ actual: %u, requested: %u, delta: %u%%\n\r",
                ofp_rate_tbl_get->rates[idx2].sch_rate,
                ofp_rate_tbl_set->rates[ofp_rate_tbl_get->rates[idx2].port_id].sch_rate,
                delta_rate_sch,
                ofp_rate_tbl_get->rates[idx2].egq_rate,
                ofp_rate_tbl_set->rates[ofp_rate_tbl_get->rates[idx2].port_id].egq_rate,
                delta_rate_egq
              );

              SOC_SAND_SET_ERROR_CODE(SOC_PETRA_OFP_RATES_ACTUAL_AND_EXPECTED_RATE_DIFFERENCE_ERR, 12, exit);
            }
          } /* Valid port-id */
        } /* Valid MAL (inner)*/
      } /* for(ofp_rate_tbl entries) */
    }
  }

exit_restore:

  if (!silent)
  {
    soc_sand_os_printf("Restoring original values.\r\n");
  }
  for(idx1 = 0; idx1 < ofp_rate_tbl_orig->nof_valid_entries; ++idx1)
  {
    if (ofp_rate_tbl_orig->rates[idx1].port_id != SOC_PETRA_OFP_RATES_ILLEGAL_PORT_ID)
    {
      res = soc_petra_ofp_rates_single_port_set_unsafe(
              unit,
              ofp_rate_tbl_orig->rates[idx1].port_id,
              &shaper[ccm_idx],
              ofp_rate_tbl_orig->rates + idx1
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
    }
  }
exit:
  SOC_PETRA_FREE(ofp_rate_tbl_orig);
  SOC_PETRA_FREE(ofp_rate_tbl_set);
  SOC_PETRA_FREE(ofp_rate_tbl_get);  
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_ofp_rates_test_random_unsafe()",idx1,idx2);
}

#endif

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

