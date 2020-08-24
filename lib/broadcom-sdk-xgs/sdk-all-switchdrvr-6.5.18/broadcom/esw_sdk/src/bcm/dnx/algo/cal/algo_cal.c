/** \file algo_cal.c
 *  
 *  Calendar calculation
 *  
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <shared/shrextend/shrextend_debug.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <shared/utilex/utilex_u64.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_ofp_rate_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>

#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/algo/cal/algo_cal.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>

#define DNX_ALGO_CAL_RATE_DEVIATION     (100000000)

/*
 * See algo_cal.h
 */
int
dnx_algo_ofp_rates_fill_shaper_generic_calendar_credits(
    int unit,
    int core,
    dnx_ofp_rates_cal_info_t * cal_info,
    uint32 *ports_rates,
    /*
     * Actual Calendar length
     */
    uint32 calendar_len,
    /*
     * Indicate if last entry is dummy or not
     */
    uint8 add_dummy_tail,
    dnx_ofp_rates_cal_egq_t * calendar)
{
    uint32 slot_idx, port_idx, egq_resolution;
    uint32 *port_credits_p = NULL;
    uint32 temp_calendar_length, dummy_tail_entry, calcal_length, calcal_instances;
    int packet_mode;
    int base_q_pair, nof_priorities;
    bcm_port_t logical_port;
    bcm_pbmp_t port_bm;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(ports_rates, BCM_E_PARAM, "ports_rates");
    SHR_NULL_CHECK(calendar, BCM_E_PARAM, "calendar");

    SHR_ALLOC_SET_ZERO(port_credits_p, sizeof(*port_credits_p) * dnx_data_egr_queuing.params.nof_q_pairs_get(unit),
                       "port_credits_p", "%s%s%s", EMPTY, EMPTY, EMPTY);

    dummy_tail_entry = ((add_dummy_tail == FALSE) ? 0x0 : 0x1);
    /*
     * In case of dummy tail, all slots are taken into account without it
     */
    temp_calendar_length = calendar_len - dummy_tail_entry;
    if (cal_info->cal_type == DNX_OFP_RATES_EGQ_CAL_CHAN_ARB)
    {
        SHR_IF_ERR_EXIT(dnx_ofp_rate_db.calcal_len.get(unit, core, &calcal_length));
        SHR_IF_ERR_EXIT(dnx_ofp_rate_db.otm_cal.
                        nof_calcal_instances.get(unit, core, cal_info->chan_arb_id, &calcal_instances));
    }
    else
    {
        /*
         * When no calcal to be taken into consideration. assume lengths are 1
         */
        calcal_length = 1;
        calcal_instances = 1;
    }

    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, core, DNX_ALGO_PORT_LOGICALS_TYPE_TM_EGR_QUEUING, 0, &port_bm));

    BCM_PBMP_ITER(port_bm, logical_port)
    {
        /*
         * From the logical port get the associated shaper mode (DATA or PACKET), the base Q-pair and number of priorities
         */
        SHR_IF_ERR_EXIT(dnx_egr_queuing_packet_mode_get(unit, logical_port, &packet_mode));
        SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
        SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, logical_port, &nof_priorities));
        for (port_idx = base_q_pair; port_idx < (base_q_pair + nof_priorities); port_idx++)
        {
            uint32 rate;

            if (!ports_rates[port_idx])
            {
                continue;
            }
            if (packet_mode == 0)
            {
                /*
                 * For DATA mode, conversion coefficent is 'credits per bit'.
                 */
                egq_resolution = dnx_data_egr_queuing.params.cal_res_get(unit);
            }
            else
            {
                /*
                 * For PACKET mode, conversion coefficent is 'credits per packet'.
                 */
                egq_resolution = dnx_data_egr_queuing.params.cal_res_packet_mode_get(unit);
            }
            rate = ports_rates[port_idx];
            SHR_IF_ERR_EXIT(dnx_ofp_rates_calculate_credits_from_rate
                            (unit, calcal_length, calcal_instances, calendar_len, egq_resolution, (uint32) packet_mode,
                             rate, &(port_credits_p[port_idx])));
        }
    }
    for (slot_idx = 0; slot_idx < temp_calendar_length; ++slot_idx)
    {
        port_idx = calendar->slots[slot_idx].base_q_pair;
        calendar->slots[slot_idx].credit = port_credits_p[port_idx];
    }
exit:
    SHR_FREE(port_credits_p);
    SHR_FUNC_EXIT;
}
/**
 * \brief -
 * for calendar of length tentative_len
 * calculate number of slots for each object in the calendar and total deviation of the result calendar
 */
static int
dnx_algo_cal_simple_per_len_build(
    int unit,
    uint32 *rates_per_object,
    uint32 nof_objects,
    uint32 total_bandwidth,
    uint32 max_calendar_len,
    uint32 tentative_len,
    uint32 *actual_len,
    uint32 *slots_per_object_p,
    uint32 *deviation)
{
    uint32 obj_id, total_num_slots = 0, calc_deviation;
    uint32 *slots_rates_p = NULL;
    uint32 rem;
    uint32 num_slots, temp1, temp2;
    UTILEX_U64 u64_1, u64_2;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(slots_rates_p, sizeof(*slots_rates_p) * nof_objects, "slots_rates_p", "%s%s%s", EMPTY, EMPTY,
                       EMPTY);

    /*
     * Calculate the number of calendar slots per object
     */
    for (obj_id = 0; obj_id < nof_objects; ++obj_id)
    {
        if (rates_per_object[obj_id] > 0)
        {
            /*
             *  Calculate number of slots for each object according to:
             *
             *                          object_rate * cal_num_of_slots
             *  obj_num_of_cal_slots = ------------------------------------
             *                               total_bandwidth
             *
             *
             */

            utilex_u64_multiply_longs(rates_per_object[obj_id], tentative_len, &u64_1);
            rem = utilex_u64_devide_u64_long(&u64_1, total_bandwidth, &u64_2);
            utilex_u64_to_long(&u64_2, &(num_slots));

            /** Round up in case the remainder is greater then 0, or in case num_slots == 0*/
            num_slots = (rem > 0 ? (num_slots + 1) : ((num_slots == 0) ? num_slots + 1 : num_slots));

            slots_rates_p[obj_id] = num_slots;
            total_num_slots += num_slots;
        }
    }
    if (total_num_slots > max_calendar_len || total_bandwidth == 0 || total_num_slots == 0)
    {
        /** This solution is not acceptable, so return zero */
        *actual_len = 0;
        sal_memset(slots_per_object_p, 0, sizeof(*slots_per_object_p) * nof_objects);
    }
    else
    {

        calc_deviation = 0;
        for (obj_id = 0; obj_id < nof_objects; ++obj_id)
        {
            utilex_u64_multiply_longs(DNX_ALGO_CAL_RATE_DEVIATION, rates_per_object[obj_id], &u64_1);
            rem = utilex_u64_devide_u64_long(&u64_1, total_bandwidth, &u64_2);
            utilex_u64_to_long(&u64_2, &(temp1));
            temp1 = (rem > 0 ? temp1 + 1 : temp1);

            utilex_u64_multiply_longs(DNX_ALGO_CAL_RATE_DEVIATION, slots_rates_p[obj_id], &u64_1);
            rem = utilex_u64_devide_u64_long(&u64_1, total_num_slots, &u64_2);
            utilex_u64_to_long(&u64_2, &(temp2));
            temp2 = (rem > 0 ? temp2 + 1 : temp2);

            calc_deviation += utilex_abs(temp2 - temp1);
        }

        *actual_len = total_num_slots;
        sal_memcpy(slots_per_object_p, slots_rates_p, sizeof(*slots_rates_p) * nof_objects);
        *deviation = calc_deviation;
    }
exit:
    SHR_FREE(slots_rates_p);
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * find calendar length which provides best deviation 
 * and calculate number of slots for each obj_id in this calendar
 */
static int
dnx_algo_cal_simple_len_calculate(
    int unit,
    uint32 *rates_per_object,
    uint32 nof_objects,
    uint32 total_bandwidth,
    uint32 max_calendar_len,
    uint32 *actual_calendar_len,
    uint32 *slots_per_object)
{
    uint32 deviation, min_deviation, tentative_len, best_tentative_len, actual_len;
    uint32 *slots_per_object_temp_p = NULL;

    /** variables for final calendar calculation */
    uint32 *rates_per_object_final;
    uint32 nof_objects_final;
    uint32 total_bandwidth_final;
    uint32 min_rate;

    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(slots_per_object_temp_p, sizeof(*slots_per_object_temp_p) * nof_objects, "slots_per_object_temp_p",
              "%s%s%s", EMPTY, EMPTY, EMPTY);

    /*
     * Determine calendar len.
     * A possible algorithm is to go over all possible length values
     * which ranges from nof_active_ports (1 slot per port) to max_calendar_len.
     * For each value, calculate a solution and its deviation from the required
     * rates. Then pick the most accurate solution, which is the one with the smallest
     * deviation. The algorithm could also be stopped when the deviation is smaller than
     * a pre configured value.
     */
    min_deviation = DNX_ALGO_CAL_RATE_DEVIATION;
    best_tentative_len = 0;
    for (tentative_len = 1; tentative_len <= max_calendar_len; ++tentative_len)
    {
        SHR_IF_ERR_EXIT(dnx_algo_cal_simple_per_len_build(unit,
                                                          rates_per_object, nof_objects,
                                                          total_bandwidth, max_calendar_len,
                                                          tentative_len, &actual_len, slots_per_object_temp_p,
                                                          &deviation));

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

        /*
         * Calendar length is 0, means that all ports' bandwidth is 0.
         * calculate calendar with minimal parameters just to have something to configure
         */

        min_rate = 1;
        rates_per_object_final = &min_rate;
        nof_objects_final = 1;
        total_bandwidth_final = 1;
        best_tentative_len = 1;

    }
    else
    {
        rates_per_object_final = rates_per_object;
        nof_objects_final = nof_objects;
        total_bandwidth_final = total_bandwidth;
    }

    /*
     * Rebuild the best calendar that we found
     */
    SHR_IF_ERR_EXIT(dnx_algo_cal_simple_per_len_build(unit,
                                                      rates_per_object_final, nof_objects_final,
                                                      total_bandwidth_final, max_calendar_len,
                                                      best_tentative_len, &actual_len, slots_per_object_temp_p,
                                                      &deviation));

    *actual_calendar_len = actual_len;
    sal_memcpy(slots_per_object, slots_per_object_temp_p, sizeof(uint32) * nof_objects);

exit:
    SHR_FREE(slots_per_object_temp_p);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * build calendar for provided slotes-per-object and provided calendar length.
 * 
 * The algorith works as the following.
 * 
 * Each time algoritm selects object with maximal number of slots. It allocates slots for this object using
 * two sizes of hops.
 * The hop sizes are : (cal_len/nof_slots_of_max_object + 1) and (cal_len/nof_slots_of_max_object).
 * This results in calendar as spaced as possible without having any leftover while still going over all the calendar.
 *
 * \param [in] unit     - unit id
 * \param [in] slots_per_object - number of slots to allocate to each object
 * \param [in] nof_objects - total number of objects - size of slots_per_object
 * \param [in] calendar_len - length of the required calendar
 * \param [in] max_calendar_len - maximal possible calendar length
 * \param [out] calendar - the result calendar
 * 
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   
 * \see
 *   * None
 */
shr_error_e
dnx_algo_cal_simple_fixed_len_cal_build(
    int unit,
    uint32 *slots_per_object,
    uint32 nof_objects,
    uint32 calendar_len,
    uint32 max_calendar_len,
    uint32 *calendar)
{
    uint32 slot_idx;
    uint32 *slots_per_object_temp_p = NULL;

    uint32 alloc_slots, rem_cal_len, max_obj_idx;
    uint32 hop_size = 0, hop_size_small = 0, nof_big_hops = 0, obj_alloc_slots, free_slot_cnt;

    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(slots_per_object_temp_p, sizeof(*slots_per_object_temp_p) * nof_objects, "slots_per_object_temp_p",
              "%s%s%s", EMPTY, EMPTY, EMPTY);

    /*
     * Clear the calendar
     */
    for (slot_idx = 0; slot_idx < max_calendar_len; ++slot_idx)
    {
        calendar[slot_idx] = DNX_ALGO_CAL_ILLEGAL_OBJECT_ID;
    }

    sal_memcpy(slots_per_object_temp_p, slots_per_object, sizeof(*slots_per_object_temp_p) * nof_objects);

    /*
     *  For each object, try to allocate slots using 2 hop sizes
     */
    alloc_slots = 0;
    rem_cal_len = calendar_len;
    max_obj_idx = utilex_get_index_of_max_member_in_array(slots_per_object_temp_p, nof_objects);
    while (slots_per_object_temp_p[max_obj_idx] > 0)
    {
        /*
         * used as big hop size for the alternate algorithm 
         */
        hop_size = rem_cal_len / slots_per_object_temp_p[max_obj_idx];

        hop_size_small = hop_size - 1;  /* calculating small skip for alternate algorithm */
        /*
         * calculating the number of times we use the big skip for alt algo : 
         *
         * x: nof big hops, y: nof small hops
         *
         * x(skip+1) + y(skip) = cal_size 
         *
         * x+y=nof_slots 
         */
        nof_big_hops = rem_cal_len - slots_per_object[max_obj_idx] * hop_size;
        if (nof_big_hops == 0)  /* if nof big hops == 0, we use small hops only */
        {
            hop_size = hop_size_small;
        }
        obj_alloc_slots = 0;
        free_slot_cnt = hop_size;
        for (slot_idx = 0; slot_idx < calendar_len && obj_alloc_slots < slots_per_object[max_obj_idx]; ++slot_idx)
        {
            if (calendar[slot_idx] == DNX_ALGO_CAL_ILLEGAL_OBJECT_ID)
            {
                if (free_slot_cnt < hop_size)
                {
                    ++free_slot_cnt;
                }
                else
                {
                    calendar[slot_idx] = max_obj_idx;
                    ++alloc_slots;
                    ++obj_alloc_slots;
                    free_slot_cnt = 0;
                    /*
                     * in the alt algo, we use big hops until we reach nof_big_hops == 0, then we move to small hops 
                     * that way we spread the max port on the calendar as much as possible
                     */
                    if (nof_big_hops <= 0)
                    {
                        hop_size = hop_size_small;
                    }
                    else
                    {
                        nof_big_hops--;
                    }
                }
            }
        }

        slots_per_object_temp_p[max_obj_idx] = 0;
        rem_cal_len -= obj_alloc_slots;

        /*
         *  Make sure all slots are allocated and no leftovers remained
         */
        if (slots_per_object[max_obj_idx] != obj_alloc_slots)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Calendar calculation failed - not all slots are allocated\n");
        }

        max_obj_idx = utilex_get_index_of_max_member_in_array(slots_per_object_temp_p, nof_objects);
    }

exit:
    SHR_FREE(slots_per_object_temp_p);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * build best calendar for provided object rates, total bandwidth and maximal calendar length
 *
 * \param [in] unit     - unit id
 * \param [in] rates_per_object - rate of each object port
 * \param [in] nof_objects - number of objects - length of rates_per_object
 * \param [in] total_bandwidth - total bandwidth of the whole calendar
 * \param [in] max_calendar_len - maximal possible calendar length
 * \param [out] calendar_slots - the result calendar
 * \param [out] calendar_len - the length of the result calendar
 * 
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   
 * \see
 *   * None
 */
shr_error_e
dnx_algo_cal_simple_from_rates_to_calendar(
    int unit,
    uint32 *rates_per_object,
    uint32 nof_objects,
    uint32 total_bandwidth,
    uint32 max_calendar_len,
    uint32 *calendar_slots,
    uint32 *calendar_len)
{
    uint32 *slots_per_object_p = NULL;

    SHR_FUNC_INIT_VARS(unit);
    SHR_ALLOC(slots_per_object_p, sizeof(*slots_per_object_p) * nof_objects, "slots_per_object_p", "%s%s%s", EMPTY,
              EMPTY, EMPTY);

    SHR_IF_ERR_EXIT(dnx_algo_cal_simple_len_calculate(unit,
                                                      rates_per_object,
                                                      nof_objects,
                                                      total_bandwidth,
                                                      max_calendar_len, calendar_len, slots_per_object_p));

    /*
     * Given the optimal calendar length and the
     * corresponding weight (in slots) of each port,
     * build a calendar that will avoid burstiness
     * behavior as much as possible.
     */
    SHR_IF_ERR_EXIT(dnx_algo_cal_simple_fixed_len_cal_build(unit,
                                                            slots_per_object_p,
                                                            nof_objects,
                                                            *calendar_len, max_calendar_len, calendar_slots));

exit:
    SHR_FREE(slots_per_object_p);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * get rate of the provided object (object_id) in the provided calendar
 *
 * \param [in] unit     - unit id
 * \param [in] calendar - the input calendar
 * \param [in] calendar_len - the length of the calendar
 * \param [in] total_cal_rate - total bandwidth of the whole calendar
 * \param [in] object_id - object ID
 * \param [out] object_rate - the rate of the object in the calendar
 * 
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   
 * \see
 *   * None
 */
shr_error_e
dnx_algo_cal_simple_object_rate_get(
    int unit,
    uint32 *calendar,
    uint32 calendar_len,
    uint32 total_cal_rate,
    int object_id,
    uint32 *object_rate)
{
    uint32 calc, slot_id;
    UTILEX_U64 u64_1, u64_2;
    uint32 rem;
    int obj_num_of_cal_slots = 0;

    SHR_FUNC_INIT_VARS(unit);

    if (0 == calendar_len)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Failed to calculate calendar");
    }

    *object_rate = 0;

    for (slot_id = 0; slot_id < calendar_len; ++slot_id)
    {
        /*
         * Increase the number of calendar slots of this object by 1 
         */
        if (calendar[slot_id] == object_id)
        {
            ++obj_num_of_cal_slots;
        }
    }

    /*
     *  Calculate credit rate for the port according to:
     *
     *                                 obj_num_of_cal_slots * total_cal_rate[kBits/sec]
     *  port_sch_rate [kbits/sec]= -----------------------------------------------------
     *                                                cal_num_of_slots
     *
     */
    utilex_u64_multiply_longs(obj_num_of_cal_slots, total_cal_rate, &u64_1);
    rem = utilex_u64_devide_u64_long(&u64_1, calendar_len, &u64_2);
    utilex_u64_to_long(&u64_2, &(calc));
    calc = (rem > 0 ? calc + 1 : calc);

    *object_rate = calc;

exit:
    SHR_FUNC_EXIT;
}
