/**
 * \file egr_queuing.c
 *
 * Port Ingress reassembly context management functionality for DNX.
 * 
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_EGRESSDNX

#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/cosq.h>
#include <soc/dnx/dbal/dbal.h>
#include <shared/utilex/utilex_bitstream.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_egr_db_access.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <bcm_int/dnx/gtimer/gtimer.h>
#include <bcm_int/dnx/cosq/egress/egq_ofp_rates.h>
#include <bcm_int/dnx/cosq/cosq.h>
#include <bcm_int/dnx/cosq/egress/egr_queuing.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/cosq/egq_ps_db.h>

#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/dnx/port/port.h>
#include <bcm_int/dnx/algo/egq/egq_alloc_mngr.h>

#include "ecgm_dbal.h"

int
dnx_egr_port2egress_offset(
    int unit,
    int core,
    uint32 tm_port,
    uint32 *egr_if)
{
    soc_port_t port;
    shr_error_e rv;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core, tm_port, &port));
    rv = dnx_algo_port_egr_if_get(unit, port, (int *) egr_if);
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT;
}
/*
 * returns the index of the smallest number (1st occurence)
 */
static void
get_smallest_idx(
    uint32 *numbers,
    uint32 nof_numbers,
    uint32 *smallest_idx)
{
    int ii;
    *smallest_idx = 0;
    for (ii = 1; ii < nof_numbers; ii++)
    {
        if (numbers[ii] < numbers[*smallest_idx])
        {
            *smallest_idx = ii;
        }
    }
}
/*
 * rate in mbps , weight received by dividing rate by 12.5G and rounding up to power of 2
 */
static int
dnx_egr_q_nif_rate_to_weight(
    int unit,
    uint32 rate,
    uint32 *weight)
{
    SHR_FUNC_INIT_VARS(unit);

    if (rate == 0)
    {
        *weight = 0;
    }
    else
    {
        /*
         * dividing rate by 12.5G and rounding up to power of 2
         */
        rate = (rate + 12499) / 12500;
        *weight = utilex_power_of_2(utilex_log2_round_up(rate));
    }
    SHR_FUNC_EXIT;
}

/*
 * Returns the index of the last non-zero element in the arr
 */
static uint32
get_last_value(
    uint32 *arr,
    uint32 arr_length)
{
    uint32 i, last_idx = 0;

    for (i = 0; i < arr_length; i++)
    {
        if (arr[i] != 0)
        {
            last_idx = i;
        }
    }
    return last_idx;
}
/*
 * returns TRUE if all numbers are equal
 */
static int
is_all_equal(
    uint32 *numbers,
    uint32 nof_numbers)
{
    uint32 i, comparator = numbers[0];
    for (i = 1; i < nof_numbers; i++)
    {
        if (numbers[i] != comparator)
        {
            return FALSE;
        }
    }
    return TRUE;
}

static int
dnx_egr_lcm_above_sum_get(
    int unit,
    uint32 *numbers,
    uint32 nof_numbers,
    uint32 sum,
    uint32 *lcm)
{
    uint32 curr_iter[BCM_PBMP_PORT_MAX] = { 0 }, current_lcm = 0, smallest_idx = 0;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(numbers, _SHR_E_PARAM, "numbers");
    sal_memcpy(curr_iter, numbers, nof_numbers);

    while (current_lcm < sum)
    {
        if (is_all_equal(curr_iter, nof_numbers))
        {
            if (current_lcm != curr_iter[0])
            {
                current_lcm = curr_iter[0];
                continue;
            }
        }
        get_smallest_idx(curr_iter, nof_numbers, &smallest_idx);
        curr_iter[smallest_idx] += numbers[smallest_idx];
    }
    *lcm = current_lcm;
exit:
    SHR_FUNC_EXIT;
}

static int
dnx_egr_q_nif_cal_set(
    int unit,
    int core)
{
    soc_port_t port;
    int egr_if;
    uint32 total_weight_sum;
    int if_rate_mbps, rcy_fqp_weight;
    bcm_pbmp_t port_bm;
    uint32 calendar_length, nof_padding_if;
    int ii, jj, nof_active_if;
    uint32 current_cal, next_cal;
    uint32 weights[BCM_PBMP_PORT_MAX], lcm_temp[BCM_PBMP_PORT_MAX];
    dnx_ofp_rates_cal_sch_t *calendar;
    uint32 dummy_start;
    dnx_egr_cal_type_e cal_type;
    dnx_algo_port_type_e interface_type;
    int interface_offset, is_master_port;
    int has_speed;
    int core_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    calendar = NULL;
    dummy_start = 0;
    jj = 0;
    SHR_ALLOC(calendar, sizeof(*calendar), "calendar egr_q_nif_cal", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    DNXCMN_CORES_ITER(unit, core, core_id)
    {
        uint32 entry_handle_id;
        dbal_tables_e dbal_table_id;
        dbal_fields_e dbal_field_id;

        for (cal_type = 0; cal_type < NUM_DNX_EGR_CAL_TYPE; cal_type++)
        {
            sal_memset(lcm_temp, 0, sizeof(lcm_temp));
            sal_memset(weights, 0, sizeof(weights));
            nof_active_if = 0;
            total_weight_sum = 0;
            rcy_fqp_weight = 0;

            SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, core_id, DNX_ALGO_PORT_LOGICALS_TYPE_TM_EGR_QUEUING,
                                                       DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY, &port_bm));
            BCM_PBMP_ITER(port_bm, port)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, port, &has_speed));
                if (has_speed == 0)
                {
                    if_rate_mbps = 0;
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_algo_port_interface_rate_get(unit, port, 0, &if_rate_mbps));
                }
                SHR_IF_ERR_EXIT(dnx_algo_port_is_master_get(unit, port, &is_master_port));
                SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &egr_if));
                SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, port, &interface_type));
                SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, port, &interface_offset));
                /*
                 * calculate wait per interface
                 */
                if (!is_master_port)
                {
                    continue;
                }
                /*
                 * FQP recycle interface is special in away that it has 2 egress interfaces.
                 * for the the second interface the egress interface id should be replaced
                 */
                if (DNX_ALGO_PORT_TYPE_IS_RCY(unit, interface_type) && (cal_type == CAL_TYPE_FQP) && interface_offset)
                {
                    egr_if = dnx_data_egr_queuing.params.egr_if_txi_rcy_get(unit);
                }
                /*
                 * calculate weight according speed
                 */
                dnx_egr_q_nif_rate_to_weight(unit, if_rate_mbps, &weights[egr_if]);
                total_weight_sum += weights[egr_if];
                /*
                 * PQP has only 1 recycle interface. therefore, the weight of the interface determined by the sum of both interfaces speeds.
                 */
                if (DNX_ALGO_PORT_TYPE_IS_RCY(unit, interface_type) && (cal_type == CAL_TYPE_PQP))
                {
                    weights[egr_if] += rcy_fqp_weight;
                    rcy_fqp_weight = weights[egr_if];
                }
            }
            for (ii = 0, jj = 0; ii < BCM_PBMP_PORT_MAX; ii++)
            {
                if (weights[ii] != 0)
                {
                    nof_active_if++;
                    lcm_temp[jj++] = weights[ii];
                }
            }
            do
            {
                /*
                 * get LCM (lowest common multiple) for the calendar length
                 */
                SHR_IF_ERR_EXIT(dnx_egr_lcm_above_sum_get
                                (unit, lcm_temp, nof_active_if, total_weight_sum, &calendar_length));
                if (calendar_length > dnx_data_egr_queuing.params.nif_cal_len_get(unit))
                {
                    /*
                     * if the calendar length calculated is bigger than calendar max length, weights needs to be divided by 2 to fit the limitation
                     */
                    total_weight_sum = 0;
                    for (ii = 0, jj = 0; ii < BCM_PBMP_PORT_MAX; ii++)
                    {
                        if (weights[ii] != 0)
                        {
                            lcm_temp[jj++] = weights[ii] = (weights[ii] + 1) / 2;
                            total_weight_sum += weights[ii];
                        }
                    }
                }
            }
            while (calendar_length > dnx_data_egr_queuing.params.nif_cal_len_get(unit));
            /*
             * calculate how many dummy interfaces are required and pad accordingly
             */
            nof_padding_if = calendar_length - total_weight_sum;
            dummy_start = get_last_value(weights, BCM_PBMP_PORT_MAX) + 1;
            for (ii = 0; ii < nof_padding_if; ii++)
            {
                weights[dummy_start + ii] = 1;
            }
            /*
             * Given the optimal calendar length and the
             * corresponding weight (in slots) of each port,
             * build a calendar that will avoid burstiness
             * behavior as much as possible.
             */
            SHR_IF_ERR_EXIT(dnx_ofp_rates_fixed_len_cal_build(unit, weights, dummy_start + nof_padding_if + 1,
                                                              calendar_length,
                                                              dnx_data_egr_queuing.params.nif_cal_len_get(unit), 1,
                                                              calendar->slots));
            /*
             * get the calendar set
             */
            /*
             * Allocate buffer.
             */
            dbal_table_id = DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION;
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
            /*
             * key construction.
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
            /*
             * Get the value of 'current_cal' from FQP_CALENDAR_SET_SEL
             */
            dbal_field_id = DBAL_FIELD_FQP_CALENDAR_SET_SEL;
            dbal_value_field32_request(unit, entry_handle_id, dbal_field_id, INST_SINGLE, (uint32 *) &current_cal);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            next_cal = current_cal ? 0 : 1;
            /*
             * Reuse allocated buffer.
             */
            dbal_table_id = DBAL_TABLE_EGQ_NIF_CAL;
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_table_id, entry_handle_id));
            /*
             * key construction. Fixed part
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SET_SELECT, next_cal);
            /*
             * Replace dummies and the rest of calendar with invalid IFs and write the values to HW
             */
            for (ii = 0; ii < dnx_data_egr_queuing.params.nif_cal_len_get(unit); ii++)
            {
                if (ii >= calendar_length || calendar->slots[ii] >= dummy_start)
                {
                    calendar->slots[ii] = dnx_data_egr_queuing.params.nof_egr_interfaces_get(unit) - 1;
                }
                /*
                 * key construction. Variable part
                 */
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SLOT_ID, (uint32) (ii));
                /*
                 * Set the value of FQP_NIF_PORT_MUX/PQP_NIF_PORT_MUX
                 */
                if (cal_type == CAL_TYPE_FQP)
                {
                    dbal_field_id = DBAL_FIELD_FQP_NIF_PORT_MUX;
                }
                else
                {
                    dbal_field_id = DBAL_FIELD_PQP_NIF_PORT_MUX;
                }
                dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE,
                                             (uint32) (calendar->slots[ii]));
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }
            DBAL_HANDLE_FREE(unit, entry_handle_id);
        }
        /*
         * switch calendar A<->B set
         */
        /*
         * Allocate buffer.
         */
        dbal_table_id = DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        /*
         * key construction.
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        /*
         * Set the value of FQP_CALENDAR_SET_SEL/PQP_CALENDAR_SET_SEL
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FQP_CALENDAR_SET_SEL, INST_SINGLE,
                                     (uint32) (next_cal));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PQP_CALENDAR_SET_SEL, INST_SINGLE,
                                     (uint32) (next_cal));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        DBAL_HANDLE_FREE(unit, entry_handle_id);
    }
exit:
    SHR_FREE(calendar);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/*
 * See egr_queuing.h file
 */
shr_error_e
dnx_egr_recycle_bandwidth_set(
    int unit,
    int core,
    int is_mirror,
    uint32 speed)
{
    dbal_tables_e dbal_table_id;
    uint32 internal_rate;
    uint32 entry_handle_id;
    dbal_fields_e dbal_field_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Not a calendar so the calendar slots and length will be 1
     */
    SHR_IF_ERR_EXIT(dnx_ofp_rates_egq_shaper_rate_to_internal(unit, speed, 1, 1, &internal_rate));

    dbal_table_id = DBAL_TABLE_EGQ_SHAPER_CONFIGURATION;
    if (is_mirror)
    {
        dbal_field_id = DBAL_FIELD_HP_MIRR_RATE;
    }
    else
    {
        dbal_field_id = DBAL_FIELD_RCY_RATE;
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    /*
     * We are assuming that input 'core' may also be 'BCM_CORE_ALL', in which case
     * DBAL will operate on all cores.
     */
    {
        /*
         * key construction
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        /*
         * Set the value
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, internal_rate);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        /*
         * Use already opened table for the next load.
         */
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See egr_queuing.h file
 */
shr_error_e
dnx_egr_recycle_bandwidth_get(
    int unit,
    int core,
    int is_mirror,
    uint32 *speed)
{
    dbal_tables_e dbal_table_id;
    uint32 entry_handle_id;
    dbal_fields_e dbal_field_id;
    uint32 internal_rate;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    dbal_table_id = DBAL_TABLE_EGQ_SHAPER_CONFIGURATION;
    if (is_mirror)
    {
        dbal_field_id = DBAL_FIELD_HP_MIRR_RATE;
    }
    else
    {
        dbal_field_id = DBAL_FIELD_RCY_RATE;
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));

    /*
     * key construction
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    /*
     * Get the value
     */
    dbal_value_field32_request(unit, entry_handle_id, dbal_field_id, INST_SINGLE, &internal_rate);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(dnx_ofp_rates_egq_shaper_rate_from_internal(unit, 1, 1, internal_rate, speed));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See egr_queuing.h file
 */
shr_error_e
dnx_egr_recycle_burst_set(
    int unit,
    int core,
    int is_mirror,
    uint32 burst)
{
    dbal_tables_e dbal_table_id;
    uint32 entry_handle_id;
    dbal_fields_e dbal_field_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    dbal_table_id = DBAL_TABLE_EGQ_SHAPER_CONFIGURATION;
    if (is_mirror)
    {
        dbal_field_id = DBAL_FIELD_HP_MIRR_MAX_BURST;
    }
    else
    {
        dbal_field_id = DBAL_FIELD_RCY_MAX_BURST;
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    /*
     * We are assuming that input 'core' may also be 'BCM_CORE_ALL', in which case
     * DBAL will operate on all cores.
     */
    {
        /*
         * key construction
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        /*
         * Set the value
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, (uint32) (burst));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        /*
         * Use already opened table for the next load.
         */
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See egr_queuing.h file
 */
shr_error_e
dnx_egr_recycle_burst_get(
    int unit,
    int core,
    int is_mirror,
    uint32 *burst)
{
    dbal_tables_e dbal_table_id;
    uint32 entry_handle_id;
    dbal_fields_e dbal_field_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    dbal_table_id = DBAL_TABLE_EGQ_SHAPER_CONFIGURATION;
    if (is_mirror)
    {
        dbal_field_id = DBAL_FIELD_HP_MIRR_MAX_BURST;
    }
    else
    {
        dbal_field_id = DBAL_FIELD_RCY_MAX_BURST;
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    /*
     * key construction
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    /*
     * Get the value
     */
    dbal_value_field32_request(unit, entry_handle_id, dbal_field_id, INST_SINGLE, burst);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See egr_queuing.h file
 */
shr_error_e
dnx_egr_recycle_weight_set(
    int unit,
    int core,
    int is_mirror,
    uint32 weight)
{
    dbal_tables_e dbal_table_id;
    uint32 entry_handle_id;
    dbal_fields_e dbal_field_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    dbal_table_id = DBAL_TABLE_EGQ_SHAPER_CONFIGURATION;
    if (is_mirror)
    {
        dbal_field_id = DBAL_FIELD_HP_MIRR_WEIGHT;
    }
    else
    {
        dbal_field_id = DBAL_FIELD_RCY_WEIGHT;
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    /*
     * We are assuming that input 'core' may also be 'BCM_CORE_ALL', in which case
     * DBAL will operate on all cores.
     */
    {
        /*
         * key construction
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        /*
         * Set the value
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, (uint32) (weight));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        /*
         * Use already opened table for the next load.
         */
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See egr_queuing.h file
 */
shr_error_e
dnx_egr_recycle_weight_get(
    int unit,
    int core,
    int is_mirror,
    uint32 *weight)
{
    dbal_tables_e dbal_table_id;
    uint32 entry_handle_id;
    dbal_fields_e dbal_field_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    dbal_table_id = DBAL_TABLE_EGQ_SHAPER_CONFIGURATION;
    if (is_mirror)
    {
        dbal_field_id = DBAL_FIELD_HP_MIRR_WEIGHT;
    }
    else
    {
        dbal_field_id = DBAL_FIELD_RCY_WEIGHT;
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    /*
     * key construction
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    /*
     * Get the value
     */
    dbal_value_field32_request(unit, entry_handle_id, dbal_field_id, INST_SINGLE, weight);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - map port to egress recycle interface
 *
 * \param [in] unit -  Unit-ID
 * \param [in] port -  port
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_egr_port_recycle_interface_map_set(
    int unit,
    bcm_port_t port)
{
    bcm_core_t core;
    int base_q_pair;
    int interface_offset;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, port, &interface_offset));
    /*
     * write value to HW
     */
    {
        dbal_tables_e dbal_table_id;
        uint32 entry_handle_id;

        dbal_table_id = DBAL_TABLE_EGQ_QPAIR_ATTRIBUTES;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        /*
         * key construction
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QPAIR, base_q_pair);
        /*
         * We assume that 'core' may also be 'BCM_CORE_ALL' and that it may be passed, as is,
         * to DBAL (I.e., we assume that DBAL_CORE_ALL is the same as BCM_CORE_ALL).
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        /*
         * Set the value
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RCY_TXQ_NUM, INST_SINGLE,
                                     (uint32) (interface_offset ? 1 : 0));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See egr_queuing.h file
 */
shr_error_e
dnx_egr_port_recycle_interface_map_get(
    int unit,
    bcm_port_t port,
    int *rcy_if_idx)
{
    bcm_core_t core;
    int base_q_pair;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &base_q_pair));
    /*
     * read value from HW
     */
    {
        dbal_tables_e dbal_table_id;
        uint32 entry_handle_id;
        uint32 *rcy_if_idx_p;

        rcy_if_idx_p = (uint32 *) rcy_if_idx;
        dbal_table_id = DBAL_TABLE_EGQ_QPAIR_ATTRIBUTES;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        /*
         * key construction
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QPAIR, base_q_pair);
        /*
         * We assume that 'core' may also be 'BCM_CORE_ALL' and that it may be passed, as is,
         * to DBAL (I.e., we assume that DBAL_CORE_ALL is the same as BCM_CORE_ALL).
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        /*
         * Get the value: read 1 complement value from HW
         */
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RCY_TXQ_NUM, INST_SINGLE, rcy_if_idx_p);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See egr_queuing.h file
 */
shr_error_e
dnx_egr_port_compensation_set(
    int unit,
    bcm_port_t port,
    int comp)
{
    int base_q_pair;
    bcm_core_t core;
    int field_size;
    /*
     * We define 'field_val' as an array just to slence coverity
     * which requires that, since SHR_BITSET below treats its input
     * as an array, then it should really be an array (even of 1 element).
     */
    uint32 field_val[1];
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));

    field_val[0] = (uint32) comp;
    {
        dbal_tables_e dbal_table_id;

        dbal_table_id = DBAL_TABLE_EGQ_QPAIR_ATTRIBUTES;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        /*
         * key construction
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QPAIR, base_q_pair);
        /*
         * We assume that 'core' may also be 'BCM_CORE_ALL' and that it may be passed, as is,
         * to DBAL (I.e., we assume that DBAL_CORE_ALL is the same as BCM_CORE_ALL).
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        /*
         * write the value to HW at 1 complement
         */
        SHR_IF_ERR_EXIT(dbal_tables_field_size_get(unit, dbal_table_id,
                                                   DBAL_FIELD_CR_ADJUST_VALUE, FALSE, 0, 0, &field_size));
        if (comp < 0)
        {
            field_val[0] = utilex_abs(comp);
            SHR_BITSET(field_val, field_size - 1);
        }
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CR_ADJUST_VALUE, INST_SINGLE, field_val[0]);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See egr_queuing.h file
 */
shr_error_e
dnx_egr_port_compensation_get(
    int unit,
    bcm_port_t port,
    int *comp)
{
    int base_q_pair;
    bcm_core_t core;
    uint32 fld_val[1];
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    {
        dbal_tables_e dbal_table_id;
        uint32 entry_handle_id;

        dbal_table_id = DBAL_TABLE_EGQ_QPAIR_ATTRIBUTES;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        /*
         * key construction
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QPAIR, base_q_pair);
        /*
         * We assume that 'core' may also be 'BCM_CORE_ALL' and that it may be passed, as is,
         * to DBAL (I.e., we assume that DBAL_CORE_ALL is the same as BCM_CORE_ALL).
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        /*
         * Get the value: read 1 complement value from HW
         */
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_CR_ADJUST_VALUE, INST_SINGLE, &(fld_val[0]));
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    }
    *comp = 0;
    SHR_BITCOPY_RANGE((uint32 *) comp, 0, fld_val, 0, 6);
    *comp = (SHR_BITGET(fld_val, 6)) ? *comp * -1 : *comp;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - map egr interface to nif
 *
 * \param [in] unit -  Unit-ID
 * \param [in] port -  port
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_egr_port_to_interface_egress_map_set(
    int unit,
    bcm_port_t port)
{
    int egr_if;
    dnx_algo_port_cdu_access_info_t cdu_info;
    int nif_port;
    bcm_core_t core;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_cdu_access_info_get(unit, port, &cdu_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_nif_first_phy_get(unit, port, 0, &nif_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &egr_if));
    {
        dbal_tables_e dbal_table_id;
        uint32 entry_handle_id;

        dbal_table_id = DBAL_TABLE_EGQ_INTERFACE_ATTRIBUTES;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        /*
         * key construction
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EGQ_IF, (uint32) egr_if);
        /*
         * We assume that 'core' may also be 'BCM_CORE_ALL' and that it may be passed, as is,
         * to DBAL (I.e., we assume that DBAL_CORE_ALL is the same as BCM_CORE_ALL).
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        /*
         * Set the values
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PM_NUM, INST_SINGLE,
                                     (uint32) (cdu_info.inner_cdu_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PM_INTERNAL_PORT, INST_SINGLE,
                                     (uint32) (cdu_info.first_lane_in_cdu));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_PORT_NUM, INST_SINGLE,
                                     (uint32) (nif_port / dnx_data_device.general.nof_cores_get(unit)));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h
 */
shr_error_e
dnx_egr_queuing_interface_alloc(
    int unit,
    bcm_port_t port,
    int master_logical_port,
    bcm_core_t core,
    bcm_port_if_t interface_type,
    int *egr_if)
{
    int if_occupied;
    int i;

    SHR_FUNC_INIT_VARS(unit);
    *egr_if = -1;

    if ((master_logical_port == DNX_ALGO_PORT_INVALID))
    {   /* First port in interface */
        switch (interface_type)
        {
            case SOC_PORT_IF_CPU:
                *egr_if = dnx_data_egr_queuing.params.egr_if_cpu_get(unit);
                break;

            case SOC_PORT_IF_OLP:
                *egr_if = dnx_data_egr_queuing.params.egr_if_olp_get(unit);
                break;

            case SOC_PORT_IF_OAMP:
                *egr_if = dnx_data_egr_queuing.params.egr_if_oam_get(unit);
                break;

            case SOC_PORT_IF_RCY:
                *egr_if = dnx_data_egr_queuing.params.egr_if_rcy_get(unit);
                break;

            case SOC_PORT_IF_SAT:
                *egr_if = dnx_data_egr_queuing.params.egr_if_sat_get(unit);
                break;

            case SOC_PORT_IF_EVENTOR:
                *egr_if = dnx_data_egr_queuing.params.egr_if_eventor_get(unit);
                break;

            default:
                for (i = dnx_data_egr_queuing.params.egr_if_txi_rcy_get(unit) + 1;
                     i < dnx_data_egr_queuing.params.nof_egr_interfaces_get(unit); i++)
                {
                    SHR_IF_ERR_EXIT(dnx_egr_db.interface_occ.get(unit, core, i, &if_occupied));
                    if (if_occupied == FALSE)
                    {
                        *egr_if = i;
                        SHR_IF_ERR_EXIT(dnx_egr_db.interface_occ.set(unit, core, *egr_if, TRUE));
                        break;
                    }
                }
                if (i >= dnx_data_egr_queuing.params.nof_egr_interfaces_get(unit))
                {
                    SHR_ERR_EXIT(_SHR_E_MEMORY, "no free egress interface to allocate for port %d\n", port);
                }
                break;
        }
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, master_logical_port, egr_if));
    }

    if (*egr_if == -1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "invalid egr_if\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - egr interface release per port
 *
 * \param [in] unit -  Unit-ID
 * \param [in] port -  port
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_egr_queuing_interface_free(
    int unit,
    bcm_port_t port)
{
    int egr_if;
    int core;
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_algo_port_valid_verify(unit, port) == _SHR_E_NONE)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &egr_if));
        SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
        SHR_IF_ERR_EXIT(dnx_egr_db.interface_occ.set(unit, core, egr_if, FALSE));
        SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_unset(unit, port));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - configures credit size to HW
 *
 * \param [in] unit -  Unit-ID
 * \param [in] port -  port
 * \param [in] credit_size -  credit size for port
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_egr_queuing_nif_credit_set(
    int unit,
    bcm_port_t port,
    uint32 credit_size)
{

    int core;
    int egr_if;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &egr_if));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    /*
     * configure TXI credit per interface. CREDIT_INIT needs to be set and cleared for the interface after setting the size
     */
    {
        dbal_tables_e dbal_table_id;
        uint32 entry_handle_id;

        dbal_table_id = DBAL_TABLE_EGQ_GLOBAL_CONFIGURATION;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        /*
         * key construction.
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        /*
         * Get required 'credit_size'. It will go into effect when the corresponding
         * bit is set in 'credit_init' (and is written to HW.
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGQ_NIF_CREDIT_SIZE, INST_SINGLE,
                                     (uint32) (credit_size));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        /*
         * Reuse allocated buffer.
         */
        dbal_table_id = DBAL_TABLE_EGQ_INTERFACE_REG_ATTRIBUTES;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_table_id, entry_handle_id));
        /*
         * key construction.
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EGQ_IF, egr_if);
        /*
         * Set the bit corresponding to indicated interface and update, this way, credit size
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CREDIT_INIT_BIT, INST_SINGLE, (uint32) 1);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        /*
         * Clear the bit corresponding to indicated interface (back to default value of '0')
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CREDIT_INIT_BIT, INST_SINGLE, (uint32) 0);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - gets credit size & irdy values from dnx data based on interface speed
 *
 * \param [in] unit -  Unit-ID
 * \param [in] port -  port
 * \param [out] credit_size -  credit size for port
 * \param [out] irdy_thr -  irdy threshold for port
 * \param [out] txq_max_bytes - max bytes threshold for txq transmit
 * \param [out] min_gap_hp - min gap for the consecutive access to HP queue
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_egr_queuing_nif_info_get(
    int unit,
    bcm_port_t port,
    uint32 *credit_size,
    uint32 *irdy_thr,
    uint32 *txq_max_bytes,
    uint32 *min_gap_hp)
{
    uint32 idx;
    int if_speed, if_id;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_interface_rate_get(unit, port, 0, &if_speed));
    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &if_id));

    /*
     * get TXI credit and NRDY values received from ASIC
     * NIF interfaces are searched at the table according speed. Special interfaces are searched according interface ID.
     */
    for (idx = 0; idx < dnx_data_egr_queuing.params.if_speed_params_info_get(unit)->key_size[0]; idx++)
    {
        if (dnx_data_egr_queuing.params.if_speed_params_get(unit, idx)->if_id == if_id)
        {
            break;
        }
        else
        {
            if (if_speed <= dnx_data_egr_queuing.params.if_speed_params_get(unit, idx)->speed)
            {
                break;
            }
        }
    }

    if (idx < dnx_data_egr_queuing.params.if_speed_params_info_get(unit)->key_size[0])
    {
        *credit_size = dnx_data_egr_queuing.params.if_speed_params_get(unit, idx)->crdt_size;
        *irdy_thr = dnx_data_egr_queuing.params.if_speed_params_get(unit, idx)->irdy_thr;
        *txq_max_bytes = dnx_data_egr_queuing.params.if_speed_params_get(unit, idx)->txq_max_bytes;
        *min_gap_hp = dnx_data_egr_queuing.params.if_speed_params_get(unit, idx)->min_gap_hp;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "NIF parameters are not found %d\n", port);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See egr_queuing.h
 */
shr_error_e
dnx_egr_queuing_nif_credit_default_set(
    int unit,
    bcm_port_t port)
{
    uint32 credit_size, irdy_th, txq_max_bytes, min_gap_hp;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_egr_queuing_nif_info_get(unit, port, &credit_size, &irdy_th, &txq_max_bytes, &min_gap_hp));
    SHR_IF_ERR_EXIT(dnx_egr_queuing_nif_credit_set(unit, port, credit_size));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See egr_queuing.h
 */
shr_error_e
dnx_egr_queuing_tcg_cir_sp_en_set(
    int unit,
    bcm_gport_t port,
    uint32 enable)
{
    bcm_core_t core;
    dbal_tables_e dbal_table_id;
    uint32 entry_handle_id;
    dbal_fields_e dbal_field_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    dbal_table_id = DBAL_TABLE_EGQ_SHAPER_CONFIGURATION;
    dbal_field_id = DBAL_FIELD_CIR_SP_EN;

    core = port ? BCM_COSQ_GPORT_CORE_GET(port) : DBAL_CORE_ALL;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    /*
     * key construction
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    /*
     * Set the value
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, (uint32) (enable));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See egr_queuing.h
 */
shr_error_e
dnx_egr_queuing_tcg_cir_sp_en_get(
    int unit,
    bcm_gport_t port,
    uint32 *enable)
{
    bcm_core_t core;
    dbal_tables_e dbal_table_id;
    uint32 entry_handle_id;
    dbal_fields_e dbal_field_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    dbal_table_id = DBAL_TABLE_EGQ_SHAPER_CONFIGURATION;

    core = port ? BCM_COSQ_GPORT_CORE_GET(port) : DBAL_CORE_ALL;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    /*
     * key construction
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    /*
     * Get the value: read 1 complement value from HW
     */
    dbal_field_id = DBAL_FIELD_CIR_SP_EN;
    dbal_value_field32_request(unit, entry_handle_id, dbal_field_id, INST_SINGLE, enable);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See egr_queuing.h
 */
shr_error_e
dnx_egr_queuing_if_thr_set(
    int unit,
    bcm_port_t port)
{
    uint32 credit_size, irdy_thr, txq_max_bytes, min_gap_hp;
    int egr_if;
    bcm_core_t core;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_egr_queuing_nif_info_get(unit, port, &credit_size, &irdy_thr, &txq_max_bytes, &min_gap_hp));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &egr_if));

    {
        dbal_tables_e dbal_table_id;
        uint32 entry_handle_id;

        dbal_table_id = DBAL_TABLE_EGQ_INTERFACE_ATTRIBUTES;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        /*
         * key construction
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EGQ_IF, (uint32) egr_if);
        /*
         * We assume that 'core' may also be 'BCM_CORE_ALL' and that it may be passed, as is,
         * to DBAL (I.e., we assume that DBAL_CORE_ALL is the same as BCM_CORE_ALL).
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        /*
         * Set the values
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TXI_IRDY_TH, INST_SINGLE, (uint32) (irdy_thr));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TXQ_MAX_BYTES_TH, INST_SINGLE,
                                     (uint32) (txq_max_bytes));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIN_GAP_HP, INST_SINGLE, (uint32) (min_gap_hp));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See egr_queuing.h
 */
shr_error_e
dnx_egr_queuing_nif_calendar_set(
    int unit,
    bcm_core_t core)
{
    return dnx_egr_q_nif_cal_set(unit, core);
}

/**
 * \brief - configures egress multicast replication fifo parameters received from dnx data to HW
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core -  core-id
 * \param [in] tdm_exist -  indicates if there is tdm traffic in the system
 *  *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_egr_queuing_emr_fifo_default_set(
    int unit,
    int core,
    uint32 tdm_exist)
{
    uint32 tdm, uc, mc_h, mc_l;
    dbal_tables_e dbal_table_id;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * get EMR fifo depth parameters and configure the HW
     */
    tdm = dnx_data_egr_queuing.params.emr_fifo_get(unit, DNX_EGR_EMR_FIFO_TDM, tdm_exist)->full;
    uc = dnx_data_egr_queuing.params.emr_fifo_get(unit, DNX_EGR_EMR_FIFO_UC, tdm_exist)->full;
    mc_h = dnx_data_egr_queuing.params.emr_fifo_get(unit, DNX_EGR_EMR_FIFO_MC_HIGH, tdm_exist)->full;
    mc_l = dnx_data_egr_queuing.params.emr_fifo_get(unit, DNX_EGR_EMR_FIFO_MC_LOW, tdm_exist)->full;
    /*
     */
    dbal_table_id = DBAL_TABLE_EGQ_MULTICAST_FIFOS;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    /*
     * key construction
     */
    /*
     * We assume that 'core' may also be 'BCM_CORE_ALL' and that it may be passed, as is,
     * to DBAL (I.e., we assume that DBAL_CORE_ALL is the same as BCM_CORE_ALL).
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    /*
     * Set the values
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TDM_FIFO_FULL, INST_SINGLE, (uint32) (tdm));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UNICAST_FIFO_FULL, INST_SINGLE, (uint32) (uc));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MC_HIGH_FIFO_FULL, INST_SINGLE, (uint32) (mc_h));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MC_LOW_FIFO_FULL, INST_SINGLE, (uint32) (mc_l));
    /*
     * get EMR fifo almost full threshold parameters and configure the HW
     */
    tdm = dnx_data_egr_queuing.params.emr_fifo_get(unit, DNX_EGR_EMR_FIFO_TDM, tdm_exist)->almost_full;
    uc = dnx_data_egr_queuing.params.emr_fifo_get(unit, DNX_EGR_EMR_FIFO_UC, tdm_exist)->almost_full;
    mc_h = dnx_data_egr_queuing.params.emr_fifo_get(unit, DNX_EGR_EMR_FIFO_MC_HIGH, tdm_exist)->almost_full;
    mc_l = dnx_data_egr_queuing.params.emr_fifo_get(unit, DNX_EGR_EMR_FIFO_MC_LOW, tdm_exist)->almost_full;
    /*
     * Set the values
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TDM_FIFO_ALMOST_FULL, INST_SINGLE, (uint32) (tdm));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UNICAST_FIFO_DISCARD_TH, INST_SINGLE, (uint32) (uc));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MC_HIGH_FIFO_DISCARD_TH, INST_SINGLE,
                                 (uint32) (mc_h));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MC_LOW_FIFO_DISCARD_TH, INST_SINGLE,
                                 (uint32) (mc_l));
    /*
     * get EMR fifo full threshold parameters and configure the HW
     */
    tdm = dnx_data_egr_queuing.params.emr_fifo_get(unit, DNX_EGR_EMR_FIFO_TDM, tdm_exist)->depth;
    uc = dnx_data_egr_queuing.params.emr_fifo_get(unit, DNX_EGR_EMR_FIFO_UC, tdm_exist)->depth;
    mc_h = dnx_data_egr_queuing.params.emr_fifo_get(unit, DNX_EGR_EMR_FIFO_MC_HIGH, tdm_exist)->depth;
    mc_l = dnx_data_egr_queuing.params.emr_fifo_get(unit, DNX_EGR_EMR_FIFO_MC_LOW, tdm_exist)->depth;
    /*
     * Set the values
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TDM_FIFO_DEPTH, INST_SINGLE, (uint32) (tdm));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UNICAST_FIFO_DEPTH, INST_SINGLE, (uint32) (uc));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MC_HIGH_FIFO_DEPTH, INST_SINGLE, (uint32) (mc_h));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MC_LOW_FIFO_DEPTH, INST_SINGLE, (uint32) (mc_l));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set the flow control mapping CMIC to EGQ
 *
 * \param [in] unit -  Unit-ID
 * \param [in] core -  Core ID
 * \param [in] cpu_cos -  CPU_COS which is channel number + cosq
 * \param [in] qpair -  Q-pair to be mapped to the corresponding CPU_COS
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_egr_queuing_cmic_egq_map_set(
    int unit,
    bcm_core_t core,
    int cpu_cos,
    int qpair)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Allocate handle for flow control mapping CMIC to EGQ
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FC_RX_CPU_MAP, &entry_handle_id));
    /*
     *  Setting key fields
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CPU_CHAN, cpu_cos);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    /*
     *  Setting value fields
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QPAIR, INST_SINGLE, qpair);
    /*
     *  Performing the action
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - enable/disable port qpair flow control
 *
 * \param [in] unit -  Unit-ID
 * \param [in] port -  port
 * \param [in] enable -  enable
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_egr_queuing_port_fc_enable_set(
    int unit,
    soc_port_t port,
    uint32 enable)
{
    int base_q_pair, nof_priorities, cosq;
    bcm_core_t core;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port, &nof_priorities));

    /*
     * Allocate handle for flow control mapping CMIC to EGQ
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGQ_QPAIR_ATTRIBUTES, &entry_handle_id));
    /*
     * set related qpairs
     */
    for (cosq = 0; cosq < nof_priorities; cosq++)
    {
        /*
         * key construction
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QPAIR, (base_q_pair + cosq));
        /*
         * We assume that 'core' may also be 'BCM_CORE_ALL' and that it may be passed, as is,
         * to DBAL (I.e., we assume that DBAL_CORE_ALL is the same as BCM_CORE_ALL).
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        /*
         * Set the value
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORCE_OR_IGNORE_FC, INST_SINGLE,
                                     (uint32) (enable ? 1 : 0));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_egr_queuing_port_add_handle(
    int unit)
{
    bcm_port_t port;
    int base_q_pair, num_priorities;
    int egr_if;
    uint32 ps, tm_port;
    int channelized, core, channel;
    bcm_port_t master_port;
    uint32 is_tdm;
    bcm_gport_t gport;
    uint32 cosq, tdm_exist;
    uint32 queue_priority;
    uint32 tdm_interleve;
    uint32 shaper_mode;
    dnx_algo_port_tdm_mode_e tdm_mode;
    dnx_algo_port_type_e port_type;
    uint32 entry_handle_id;
    dbal_tables_e dbal_table_id;
    uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * get logical port information
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_added_port_get(unit, &port));
    SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, port, &port_type));

    /**
     * omit non egress ports
     */
    if (!DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_type))
    {
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, port, &core, &tm_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port, &num_priorities));
    SHR_IF_ERR_EXIT(dnx_algo_port_is_channelized_get(unit, port, &channelized));
    SHR_IF_ERR_EXIT(dnx_algo_port_channel_get(unit, port, &channel));
    SHR_IF_ERR_EXIT(dnx_algo_port_tdm_get(unit, port, &tdm_mode));
    SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, port, 0, &master_port));
    shaper_mode = dnx_data_egr_queuing.params.port_spr_pkt_mode_get(unit, port)->val;
    tdm_interleve = dnx_data_egr_queuing.params.tdm_interleve_get(unit);
    is_tdm = (tdm_mode == DNX_ALGO_PORT_TDM_MODE_BYPASS || tdm_mode == DNX_ALGO_PORT_TDM_MODE_PACKET) ? 1 : 0;
    tdm_exist = dnx_data_egr_queuing.params.tdm_exist_get(unit);

    /** set mapping between pp_dsp to tm_port, mapping should be 1 to 1 */
    SHR_IF_ERR_EXIT(dnx_port_pp_dsp_to_tm_mapping_set(unit, core, tm_port, tm_port));
    /*
     * Enable Flow Control
     */
    SHR_IF_ERR_EXIT(dnx_egr_queuing_port_fc_enable_set(unit, port, TRUE));
    /*
     * get egress interface
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &egr_if));
    /*
     * per interface configuration most of the configuration relevant only for master port
     */
    /*
     * configure PS/q_pair , channelized, TDM indication
     */
    dbal_table_id = DBAL_TABLE_EGQ_INTERFACE_ATTRIBUTES;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    {
        /*
         * key construction
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EGQ_IF, (uint32) egr_if);
        /*
         * We assume that 'core' may also be 'BCM_CORE_ALL' and that it may be passed, as is,
         * to DBAL (I.e., we assume that DBAL_CORE_ALL is the same as BCM_CORE_ALL).
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        if (port == master_port)
        {
            /*
             * Set the values
             */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IFC_IS_CHANNELIZED, INST_SINGLE,
                                         (uint32) (channelized ? 1 : 0));
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TDM_HP_SHARE_TXQ, INST_SINGLE,
                                         (uint32) (!is_tdm));
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IFC_IS_ACTIVE, INST_SINGLE, (uint32) (1));
            memset(field_val, 0, sizeof(field_val));
            dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_MAP_PS_TO_IFC, INST_SINGLE,
                                             (uint32 *) (field_val));
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            /*
             * Reuse allocated buffer.
             */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_table_id, entry_handle_id));
            /*
             * key construction
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EGQ_IF, (uint32) egr_if);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        }
        if (channelized)
        {
            ps = base_q_pair / DNX_EGR_NOF_Q_PAIRS_IN_PS;
            memset(field_val, 0, sizeof(field_val));
            /*
             * Get the value: The whole array
             */
            dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_MAP_PS_TO_IFC, INST_SINGLE,
                                           (uint32 *) (field_val));
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            SHR_BITSET(field_val, ps);
            /*
             * Set the value: The whole array
             */
            dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_MAP_PS_TO_IFC, INST_SINGLE,
                                             (uint32 *) (field_val));
        }
        else
        {
            /*
             * Set the value
             */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NON_CH_IFC_BASE_QUEUE_PAIR, INST_SINGLE,
                                         (uint32) (base_q_pair));
        }
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        if (port == master_port)
        {
            /**
             * shaper mode configuration per calendar
             */
            /*
             * Reuse allocated buffer.
             */
            dbal_table_id = DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION;
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_table_id, entry_handle_id));
            /*
             * key construction.
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
            /*
             * Get the value: The whole array
             */
            dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_IFC_SPR_PKT_MODE, INST_SINGLE,
                                           (uint32 *) (field_val));
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            if (shaper_mode)
            {
                SHR_BITSET(field_val, egr_if);
            }
            else
            {
                SHR_BITCLR(field_val, egr_if);
            }
            /*
             * Set the value: The whole array
             */
            dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_IFC_SPR_PKT_MODE, INST_SINGLE,
                                             (uint32 *) (field_val));
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            /**
             * map egr interface to nif
             */
            if (DNX_ALGO_PORT_TYPE_IS_NIF(unit, port_type, FALSE, FALSE))
            {
                SHR_IF_ERR_EXIT(dnx_egr_port_to_interface_egress_map_set(unit, port));
            }
            /*
             * configure txq max thr, tdm indication and TDM interleave mode which valid only for ILKN
             */
            /*
             * Reuse allocated buffer.
             */
            dbal_table_id = DBAL_TABLE_EGQ_INTERFACE_ATTRIBUTES;
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_table_id, entry_handle_id));
            /*
             * key construction.
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EGQ_IF, (uint32) egr_if);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
            /*
             * Set the values
             */
            if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_type, 0 /* excule elk */ ))
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INTRLV_NOT_PKT_BOUND, INST_SINGLE,
                                             (uint32) (tdm_interleve ? 1 : 0));
            }

            /*
             * configures the selection of Almost Full minimum gap when All Qs under an interface are AE but the interface is not.
             */
            if (channelized)
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IFC_AE_SEL, INST_SINGLE,
                                             DNX_EGR_IFC_MIN_GAP_CHANNELIZED);
            }
            else if (num_priorities < 4)
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IFC_AE_SEL, INST_SINGLE,
                                             DNX_EGR_IFC_MIN_GAP_1_2);
            }
            else
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IFC_AE_SEL, INST_SINGLE,
                                             DNX_EGR_IFC_MIN_GAP_4_8);
            }

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TXQ_TDM_IRDY_SEL, INST_SINGLE,
                                         (uint32) (is_tdm));
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            /*
             * This table needs to configured for both recycle egress interfaces exactly the same
             */
            if (DNX_ALGO_PORT_TYPE_IS_RCY(unit, port_type))
            {
                /*
                 * key construction (using the same entry_handle_id since it is the same table).
                 */
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EGQ_IF,
                                           (uint32) (dnx_data_egr_queuing.params.egr_if_txi_rcy_get(unit)));
                /*
                 * Set the same values as above
                 */
                if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_type, 0 /* excule elk */ ))
                {
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INTRLV_NOT_PKT_BOUND, INST_SINGLE,
                                                 (uint32) (tdm_interleve ? 1 : 0));
                }
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TXQ_TDM_IRDY_SEL, INST_SINGLE,
                                             (uint32) (is_tdm));
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }
        }
        /*
         * map recycle port to recycle interface (only for recycle ports).
         */
        if (DNX_ALGO_PORT_TYPE_IS_RCY(unit, port_type))
        {
            SHR_IF_ERR_EXIT(dnx_egr_port_recycle_interface_map_set(unit, port));
        }
    }
    /*
     * Reuse allocated buffer.
     */
    dbal_table_id = DBAL_TABLE_EGQ_TM_PORT_ATTRIBUTES;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_table_id, entry_handle_id));
    {
        /*
         * map tm port to base queue pair and setting CGM interface
         */
        /*
         * key construction.
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TM_PORT, (uint32) tm_port);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        /*
         * Set the values
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QPAIR_BASE, INST_SINGLE, (uint32) (base_q_pair));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CGM_INTERFACE, INST_SINGLE, (uint32) (egr_if));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        /*
         * ps priority mode configuration
         */
        SHR_IF_ERR_EXIT(dnx_egr_dsp_pp_priorities_mode_set(unit, core, tm_port, num_priorities));
        /*
         * OTM port attributes - shaper mode, shaper index
         */
        /*
         * Reuse allocated buffer.
         */
        dbal_table_id = DBAL_TABLE_EGQ_OTM_SHAPING_ATTRIBUTES;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_table_id, entry_handle_id));
        /*
         * key construction.
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QPAIR, (uint32) base_q_pair);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        /*
         * Set the values
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_ALMOST_EMPTY_DELAY, INST_SINGLE, TRUE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OTM_SPR_PKT_MODE, INST_SINGLE,
                                     (uint32) (shaper_mode));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OTM_CAL_SPR_INDX, INST_SINGLE,
                                     (uint32) (egr_if));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        /*
         * Reuse allocated buffer.
         */
        dbal_table_id = DBAL_TABLE_EGQ_QPAIR_ATTRIBUTES;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_table_id, entry_handle_id));
        /*
         * key construction. Constant part
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        /*
         * port qpair attributes - interface, TDM indication, priority (TDM to high and the rest to low)
         */
        queue_priority = is_tdm ? BCM_COSQ_SP0 : BCM_COSQ_SP1;
        for (cosq = 0; cosq < num_priorities; cosq++)
        {
            BCM_COSQ_GPORT_PORT_TC_SET(gport, port);
            SHR_IF_ERR_EXIT(bcm_dnx_cosq_control_set(unit, gport, cosq, bcmCosqControlPrioritySelect, queue_priority));
            /*
             * key construction. variable part
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QPAIR, (uint32) (base_q_pair + cosq));
            /*
             * The field QP_IS_TDM is only meaningful for J2C.
             */
            if (tdm_exist)
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QP_IS_TDM, INST_SINGLE,
                                             (uint32) (is_tdm));
            }
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAP_QP_TO_IFC, INST_SINGLE,
                                         (uint32) (egr_if));
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_Q_ALMOST_EMPTY_DELAY, INST_SINGLE,
                                         DNX_EGR_Q_ALMOST_EMPTY_DELAY);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QP_SPR_PKT_MODE, INST_SINGLE,
                                         (uint32) (shaper_mode));
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            /*
             * Setting default qpair scheduling. UC has SP over MC
             */
            BCM_COSQ_GPORT_UCAST_EGRESS_QUEUE_SET(gport, port);
            SHR_IF_ERR_EXIT(bcm_dnx_cosq_gport_sched_set(unit, gport, cosq, BCM_COSQ_SP0, 0));
        }
        /*
         * port tc/dp mapping per port
         */
        SHR_IF_ERR_EXIT(dnx_egr_queuing_default_tc_dp_map_set(unit, port));
        /*
         * port channel configuration
         * Special case for CPU interface : Channel number is actually CPU_COS which should always be EGQ Qpair# mod 64
         */
        /*
         * Reuse allocated buffer.
         */
        dbal_table_id = DBAL_TABLE_EGQ_QPAIR_ATTRIBUTES;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_table_id, entry_handle_id));
        /*
         * key construction. Constant part
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        for (cosq = 0; cosq < num_priorities; cosq++)
        {
            /*
             * key construction. variable part
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QPAIR, (uint32) (base_q_pair + cosq));
            if (DNX_ALGO_PORT_TYPE_IS_CPU(unit, port_type))
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CPU_COS, INST_SINGLE,
                                             (uint32) (channel + cosq));
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SRC_PORT, INST_SINGLE,
                                             (uint32) (tm_port));
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
                /*
                 *  Set the flow control mapping CMIC to EGQ
                 */
                SHR_IF_ERR_EXIT(dnx_egr_queuing_cmic_egq_map_set(unit, core, (channel + cosq), (base_q_pair + cosq)));
            }
            else
            {
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CHANNEL, INST_SINGLE,
                                             (uint32) (channel));
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }
        }
        /*
         * Set tcg shaping packet mode.
         */
        /*
         * Reuse allocated buffer.
         */
        dbal_table_id = DBAL_TABLE_EGQ_TCG_ATTRIBUTES;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_table_id, entry_handle_id));
        /*
         * key construction. Constant part
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        for (cosq = 0; cosq < num_priorities; cosq++)
        {
            /*
             * key construction. variable part
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TCG_INDEX, (uint32) (base_q_pair + cosq));
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TCG_SPR_PKT_MODE, INST_SINGLE,
                                         (uint32) (shaper_mode));
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        }
        /*
         * Set interface shaping for non-channelized ports to maximum. the shaping is determined by port.
         */
        if (!channelized)
        {
            SHR_IF_ERR_EXIT(dnx_ofp_rates_interface_internal_rate_set
                            (unit, core, egr_if, dnx_data_egr_queuing.params.max_credit_number_get(unit)));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See egr_queuing.h file
 */
shr_error_e
dnx_egr_queuing_port_remove_handle(
    int unit)
{
    bcm_port_t port;
    uint32 tm_port, pp_port, internal_port;
    int is_last, is_allocated, core;
    int old_profile, new_profile;
    int base_q_pair, num_priorities;
    int egr_if;
    int channelized;
    int free_ps;
    uint32 ps;
    bcm_port_t next_master_port;
    dnx_algo_port_type_e port_type;
    dnx_cosq_egress_queue_mapping_info_t *mapping_info = NULL;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * omit non egress ports
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_removed_port_get(unit, &port));
    SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, port, &port_type));
    if (!DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_type))
    {
        SHR_EXIT();
    }
    /*
     * get logical port information
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, port, &core, &tm_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, port, &core, &pp_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port, &num_priorities));
    SHR_IF_ERR_EXIT(dnx_algo_port_is_channelized_get(unit, port, &channelized));
    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &egr_if));
    SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, port, DNX_ALGO_PORT_MASTER_F_NEXT, &next_master_port));
    /*
     * update pqp and fqp mux
     */
    /*
     * SHR_IF_ERR_EXIT(MBCM_DNX_SOC_DRIVER_CALL(unit, mbcm_dnx_egr_q_nif_cal_set, (unit, core)));
     */
    SHR_IF_ERR_EXIT(dnx_egr_q_nif_cal_set(unit, core));
    /*
     *  Exchange tc/dp template manager to default
     */
    SHR_ALLOC(mapping_info, sizeof(dnx_cosq_egress_queue_mapping_info_t), "egress queue mapping info handle memory",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    if (mapping_info == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "unit %d, failed to allocate egress queue mapping info handle memory\n", unit);
    }

    sal_memset(mapping_info, 0x0, sizeof(dnx_cosq_egress_queue_mapping_info_t));

    if (SOC_IS_J2C(unit))
    {
        internal_port = pp_port;
    }
    else
    {
        internal_port = tm_port;
    }
    SHR_IF_ERR_EXIT(dnx_am_template_egress_queue_mapping_exchange(unit, internal_port, core,
                                                                  mapping_info, &old_profile, &is_last,
                                                                  &new_profile, &is_allocated));

    /*
     * Map port to default profile
     */
    /*
     * SHR_IF_ERR_EXIT(MBCM_DNX_SOC_DRIVER_CALL(unit, mbcm_dnx_egr_q_profile_map_set, (unit, core, tm_port, new_profile)));
     */

    SHR_IF_ERR_EXIT(dnx_egr_q_profile_map_set(unit, core, internal_port, new_profile));

    /*
     * release base_q_pair
     */
    SHR_IF_ERR_EXIT(dnx_ps_db_base_qpair_free(unit, port, base_q_pair, &free_ps));
    if (free_ps)
    {
        /*
         * configure the PS which belongs to the interface and the the active indication
         */
        uint32 entry_handle_id;
        dbal_tables_e dbal_table_id;
        uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];

        ps = base_q_pair / DNX_EGR_NOF_Q_PAIRS_IN_PS;
        dbal_table_id = DBAL_TABLE_EGQ_INTERFACE_ATTRIBUTES;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        /*
         * key construction.
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EGQ_IF, (uint32) egr_if);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        memset(field_val, 0, sizeof(field_val));
        /*
         * Get the value: The whole array
         */
        dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_MAP_PS_TO_IFC, INST_SINGLE,
                                       (uint32 *) (field_val));
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        SHR_BITCLR(field_val, ps);
        /*
         * Set the value: The whole array
         */
        dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_MAP_PS_TO_IFC, INST_SINGLE,
                                         (uint32 *) (field_val));
        if (next_master_port == DNX_ALGO_PORT_INVALID)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IFC_IS_ACTIVE, INST_SINGLE, (uint32) (0));
        }
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    /*
     * release egress interface only if this is the last port
     */
    if (next_master_port == DNX_ALGO_PORT_INVALID)
    {
        SHR_IF_ERR_EXIT(dnx_egr_queuing_interface_free(unit, port));
    }
    /*
     * clear entry in pp_dsp mapping table
     */
    SHR_IF_ERR_EXIT(dnx_port_pp_dsp_table_mapping_clear(unit, core, tm_port));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_egr_queuing_prio_over_cast_set(
    int unit,
    bcm_port_t port,
    int enable)
{
    bcm_core_t core = 0;
    int base_q_pair;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &base_q_pair));
    {
        uint32 entry_handle_id;
        dbal_tables_e dbal_table_id;

        dbal_table_id = DBAL_TABLE_EGQ_OTM_SHAPING_ATTRIBUTES;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        /*
         * key construction.
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QPAIR, (uint32) base_q_pair);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SP_OVER_WFQ_2P_PORT, INST_SINGLE,
                                     (uint32) (enable));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_egr_queuing_prio_over_cast_get(
    int unit,
    bcm_port_t port,
    int *enable)
{
    bcm_core_t core = 0;
    int base_q_pair;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &base_q_pair));
    {
        uint32 entry_handle_id;
        dbal_tables_e dbal_table_id;

        dbal_table_id = DBAL_TABLE_EGQ_OTM_SHAPING_ATTRIBUTES;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        /*
         * key construction.
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QPAIR, (uint32) base_q_pair);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        /*
         * Get the value: read DBAL_FIELD_OTM_2P_BCM_88640_MODE from HW
         */
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_SP_OVER_WFQ_2P_PORT, INST_SINGLE,
                                   (uint32 *) enable);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - enable/disable port qpair shapers
 *
 * \param [in] unit -  Unit-ID
 * \param [in] port -  port
 * \param [in] enable -  enable
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_egr_queuing_port_q_pair_shaper_enable_set(
    int unit,
    soc_port_t port,
    uint32 enable)
{
    int core, base_q_pair, nof_priorities, cosq;
    uint32 q_pair_shp_en_f;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port, &nof_priorities));
    {
        uint32 entry_handle_id;
        dbal_tables_e dbal_table_id;

        dbal_table_id = DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        /*
         * key construction.
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        /*
         * Get the value: read DBAL_FIELD_QPAIR_SPR_ENA from HW
         */
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_QPAIR_SPR_ENA, INST_SINGLE, &q_pair_shp_en_f);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        if (q_pair_shp_en_f)
        {
            /*
             * Reuse allocated buffer.
             */
            dbal_table_id = DBAL_TABLE_EGQ_QPAIR_ATTRIBUTES;
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_table_id, entry_handle_id));
            /*
             * key construction. Fixed part.
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
            for (cosq = 0; cosq < nof_priorities; cosq++)
            {
                /*
                 * key construction. Variable part.
                 */
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QPAIR, (uint32) (base_q_pair + cosq));
                /*
                 * Set the variable: DBAL_FIELD_QPAIR_SPR_DIS
                 */
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QPAIR_SPR_DIS, INST_SINGLE,
                                             (uint32) (!enable));
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }
        }
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - enable/disable port tcg shapers
 *
 * \param [in] unit -  Unit-ID
 * \param [in] port -  port
 * \param [in] enable -  enable
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_egr_queuing_port_tcg_shaper_enable_set(
    int unit,
    soc_port_t port,
    uint32 enable)
{
    int core, base_q_pair, nof_priorities, cosq;
    uint32 q_pair_shp_en_f;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port, &nof_priorities));
    {
        uint32 entry_handle_id;
        dbal_tables_e dbal_table_id;

        dbal_table_id = DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        /*
         * key construction.
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        /*
         * Get the value: read DBAL_FIELD_TCG_SPR_ENA from HW
         */
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TCG_SPR_ENA, INST_SINGLE, &q_pair_shp_en_f);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        if (q_pair_shp_en_f)
        {
            /*
             * Reuse allocated buffer.
             */
            dbal_table_id = DBAL_TABLE_EGQ_TCG_ATTRIBUTES;
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_table_id, entry_handle_id));
            /*
             * key construction. Fixed part.
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
            for (cosq = 0; cosq < nof_priorities; cosq++)
            {
                /*
                 * key construction. Variable part.
                 */
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TCG_INDEX, (uint32) (base_q_pair + cosq));
                /*
                 * Set the variable: DBAL_FIELD_TCG_SPR_DIS
                 */
                dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TCG_SPR_DIS, INST_SINGLE,
                                             (uint32) (!enable));
                SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
            }
        }
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - enable/disable port otm shapers
 *
 * \param [in] unit -  Unit-ID
 * \param [in] port -  port
 * \param [in] enable -  enable
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_egr_queuing_port_otm_shaper_enable_set(
    int unit,
    soc_port_t port,
    uint32 enable)
{
    int base_q_pair;
    uint32 otm_shaper_en;
    bcm_core_t core;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    {
        uint32 entry_handle_id;
        dbal_tables_e dbal_table_id;

        dbal_table_id = DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        /*
         * key construction.
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        /*
         * Get the value: read DBAL_FIELD_OTM_SPR_ENA from HW
         */
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OTM_SPR_ENA, INST_SINGLE, &otm_shaper_en);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        if (otm_shaper_en)
        {
            /*
             * Reuse allocated buffer.
             */
            dbal_table_id = DBAL_TABLE_EGQ_OTM_SHAPING_ATTRIBUTES;
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_table_id, entry_handle_id));
            /*
             * key construction.
             */
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QPAIR, (uint32) (base_q_pair));
            /*
             * Set the variable: DBAL_FIELD_QPAIR_SPR_DIS
             */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OTM_SPR_DIS, INST_SINGLE,
                                         (uint32) (!enable));
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*********************************************************************
*     Enable/disable the egress shaping.
*     Details: in egr_queuing.h
*********************************************************************/

int
dnx_dbg_egress_shaping_enable_set(
    int unit,
    uint8 enable)
{
    uint32 fld_val;
    int core;
    uint32 entry_handle_id;
    dbal_tables_e dbal_table_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    fld_val = enable;
    dbal_table_id = DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        /*
         * key construction.
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        /*
         * Set the variable: DBAL_FIELD_OTM_SPR_ENA
         * Set the variable: DBAL_FIELD_QPAIR_SPR_ENA
         * Set the variable: DBAL_FIELD_TCG_SPR_ENA
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OTM_SPR_ENA, INST_SINGLE, (uint32) (fld_val));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_QPAIR_SPR_ENA, INST_SINGLE, (uint32) (fld_val));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TCG_SPR_ENA, INST_SINGLE, (uint32) (fld_val));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
dnx_dbg_egress_shaping_enable_get(
    int unit,
    uint8 *enable)
{
    uint32 fld_val;
    uint32 entry_handle_id;
    dbal_tables_e dbal_table_id;
    int core;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(enable, _SHR_E_PARAM, "enable");
    dbal_table_id = DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    fld_val = 0;
    core = 0;
    /*
     * key construction.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    /*
     * Get the value: read DBAL_FIELD_OTM_SPR_ENA from HW
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_OTM_SPR_ENA, INST_SINGLE, &fld_val);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *enable = fld_val;
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - enable/disable port otm shapers
 *
 * \param [in] unit -  Unit-ID
 * \param [in] port -  port
 * \param [in] enable -  enable
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_egr_queuing_interface_shaper_enable_set(
    int unit,
    soc_port_t port,
    uint32 enable)
{
    int egr_if;
    uint32 if_Shaper_en;
    bcm_core_t core;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &egr_if));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    {
        uint32 entry_handle_id;
        dbal_tables_e dbal_table_id;
        uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];

        dbal_table_id = DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        /*
         * key construction.
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        /*
         * Get the value: read DBAL_FIELD_INTERFACE_SPR_ENA from HW
         */
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_INTERFACE_SPR_ENA, INST_SINGLE, &if_Shaper_en);
        /*
         * Get the value: The whole array
         */
        dbal_value_field_arr32_request(unit, entry_handle_id, DBAL_FIELD_IFC_SPR_DIS, INST_SINGLE,
                                       (uint32 *) (field_val));
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        if (if_Shaper_en)
        {
            if (enable)
            {
                SHR_BITCLR(field_val, egr_if);
            }
            else
            {
                SHR_BITSET(field_val, egr_if);
            }
            /*
             * Set the value: The whole array
             */
            dbal_entry_value_field_arr32_set(unit, entry_handle_id, DBAL_FIELD_IFC_SPR_DIS, INST_SINGLE,
                                             (uint32 *) (field_val));
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - enable/disable port all shapers: interface, otm, qpair, TCG
 *
 * \param [in] unit -  Unit-ID
 * \param [in] port -  port
 * \param [in] enable -  enable
 *
 * \return
 *   shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnx_egr_queuing_port_all_shapers_enable_set(
    int unit,
    soc_port_t port,
    uint32 enable)
{
    int is_channelized;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_is_channelized_get(unit, port, &is_channelized));

    /** interface shaper */
    if (is_channelized)
    {
        SHR_IF_ERR_EXIT(dnx_egr_queuing_interface_shaper_enable_set(unit, port, enable));
    }

    /** qpair shaper */
    SHR_IF_ERR_EXIT(dnx_egr_queuing_port_q_pair_shaper_enable_set(unit, port, enable));

    /** tcg shaper */
    SHR_IF_ERR_EXIT(dnx_egr_queuing_port_tcg_shaper_enable_set(unit, port, enable));

    /** port shaper */
    SHR_IF_ERR_EXIT(dnx_egr_queuing_port_otm_shaper_enable_set(unit, port, enable));

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_egr_queuing_port_enable_and_flush_set(
    int unit,
    soc_port_t port,
    uint32 enable,
    uint32 flush)
{
    int core;
    shr_error_e res;
    int base_q_pair, cosq, nof_priorities;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port, &nof_priorities));

    for (cosq = 0; cosq < nof_priorities; cosq++)
    {
        SHR_IF_ERR_EXIT(dnx_ecgm_dbal_queue_disable_set(unit, core, base_q_pair + cosq, enable ? 0 : 1));
    }

    if (enable == FALSE && flush == TRUE)
    {
        /** disable port shapers. Enable port shapers if failed */
        res = dnx_egr_queuing_port_all_shapers_enable_set(unit, port, FALSE);
        if (SHR_FAILURE(res))
        {
            SHR_IF_ERR_EXIT(dnx_egr_queuing_port_all_shapers_enable_set(unit, port, TRUE));
            SHR_IF_ERR_EXIT(res);
        }

        /** set egress port to ignore flow control. Enable port shapers if failed */
        res = dnx_egr_queuing_port_fc_enable_set(unit, port, FALSE);
        if (SHR_FAILURE(res))
        {
            SHR_IF_ERR_EXIT(dnx_egr_queuing_port_all_shapers_enable_set(unit, port, TRUE));
            SHR_IF_ERR_EXIT(res);
        }

        /** make sure that the otm port has no unicast packet left. For failure enable port shapers and flow control */
        SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port, &nof_priorities));

        for (cosq = 0; cosq < nof_priorities; cosq++)
        {
            res =
                dnx_ecgm_dbal_queue_pd_count_zero_poll(unit, core, DNXCMN_TIMEOUT, DNXCMN_MIN_POLLS, base_q_pair + cosq,
                                                       0);
            if (SHR_FAILURE(res))
            {
                SHR_IF_ERR_EXIT(dnx_egr_queuing_port_all_shapers_enable_set(unit, port, TRUE));
                SHR_IF_ERR_EXIT(dnx_egr_queuing_port_fc_enable_set(unit, port, TRUE));
                SHR_IF_ERR_EXIT(res);
            }
        }

        /** make sure that the otm port has no multicast packet left. For failure enable port shapers and flow control */
        for (cosq = 0; cosq < nof_priorities; cosq++)
        {
            res =
                dnx_ecgm_dbal_queue_pd_count_zero_poll(unit, core, DNXCMN_TIMEOUT, DNXCMN_MIN_POLLS, base_q_pair + cosq,
                                                       1);
            if (SHR_FAILURE(res))
            {
                SHR_IF_ERR_EXIT(dnx_egr_queuing_port_all_shapers_enable_set(unit, port, TRUE));
                SHR_IF_ERR_EXIT(dnx_egr_queuing_port_fc_enable_set(unit, port, TRUE));
                SHR_IF_ERR_EXIT(res);
            }
        }

        /** set egress port to not ignore flow control */
        SHR_IF_ERR_EXIT(dnx_egr_queuing_port_fc_enable_set(unit, port, TRUE));

        /** enable port shapers */
        SHR_IF_ERR_EXIT(dnx_egr_queuing_port_all_shapers_enable_set(unit, port, TRUE));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * see .h file
 */
shr_error_e
dnx_egr_queuing_port_enable_get(
    int unit,
    soc_port_t port,
    uint32 *enable)
{
    int core;
    int base_q_pair;
    uint32 disable;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_ecgm_dbal_queue_disable_get(unit, core, base_q_pair, &disable));

    *enable = !disable;

exit:
    SHR_FUNC_EXIT;
}

/*
 * See egr_queuing.h
 */
shr_error_e
dnx_egr_queuing_rqp_counter_info_get(
    int unit,
    int core,
    dnx_egq_counter_info_t * counter_info,
    int *nof_counter)
{
    int size;
    int counter_idx = 0;
    int is_gtimer_enabled = 0;
    uint32 entry_handle_id;
    dbal_tables_e table_id;
    dbal_fields_e field_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    table_id = DBAL_TABLE_EGQ_RQP_COUNTER;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGQ_RQP_COUNTER, &entry_handle_id));

    /** Select the core */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    field_id = DBAL_FIELD_EMR_UNICAST_PACKET_COUNTER;
    DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                             "EMR_UNICAST_PACKET_COUNTER", DNX_EGQ_COUTNER_TYPE_PACKET, TRUE);
    counter_idx++;

    field_id = DBAL_FIELD_EMR_MC_HIGH_PACKET_COUNTER;
    DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                             "EMR_MC_HIGH_PACKET_COUNTER", DNX_EGQ_COUTNER_TYPE_PACKET, TRUE);
    counter_idx++;

    field_id = DBAL_FIELD_EMR_MC_LOW_PACKET_COUNTER;
    DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                             "EMR_MC_LOW_PACKET_COUNTER", DNX_EGQ_COUTNER_TYPE_PACKET, TRUE);
    counter_idx++;

    field_id = DBAL_FIELD_EMR_DISCARDS_PACKET_COUNTER;
    DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                             "EMR_DISCARDS_PACKET_COUNTER", DNX_EGQ_COUTNER_TYPE_PACKET, TRUE);
    counter_idx++;

    field_id = DBAL_FIELD_EMR_TDM_PACKET_COUNTER;
    DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                             "EMR_TDM_PACKET_COUNTER", DNX_EGQ_COUTNER_TYPE_PACKET, TRUE);
    counter_idx++;

    field_id = DBAL_FIELD_CDC_FABRIC_PACKET_CNT;
    DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                             "CDC_FABRIC_PACKET_CNT", DNX_EGQ_COUTNER_TYPE_PACKET, TRUE);
    counter_idx++;

    field_id = DBAL_FIELD_CDC_IPT_PACKET_CNT;
    DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                             "CDC_IPT_PACKET_CNT", DNX_EGQ_COUTNER_TYPE_PACKET, TRUE);
    counter_idx++;

    field_id = DBAL_FIELD_PRP_PACKET_IN_CNT;
    DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                             "PRP_PACKET_IN_CNT", DNX_EGQ_COUTNER_TYPE_PACKET, TRUE);
    counter_idx++;

    field_id = DBAL_FIELD_PRP_PACKET_GOOD_UC_CNT;
    DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                             "PRP_PACKET_GOOD_UC_CNT", DNX_EGQ_COUTNER_TYPE_PACKET, TRUE);
    counter_idx++;

    field_id = DBAL_FIELD_PRP_PACKET_GOOD_MC_CNT;
    DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                             "PRP_PACKET_GOOD_MC_CNT", DNX_EGQ_COUTNER_TYPE_PACKET, TRUE);
    counter_idx++;

    field_id = DBAL_FIELD_PRP_PACKET_GOOD_TDM_CNT;
    DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                             "PRP_PACKET_GOOD_TDM_CNT", DNX_EGQ_COUTNER_TYPE_PACKET, TRUE);
    counter_idx++;

    field_id = DBAL_FIELD_PRP_PACKET_DISCARD_UC_CNT;
    DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                             "PRP_PACKET_DISCARD_UC_CNT", DNX_EGQ_COUTNER_TYPE_PACKET, TRUE);
    counter_idx++;

    field_id = DBAL_FIELD_PRP_PACKET_DISCARD_MC_CNT;
    DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                             "PRP_PACKET_DISCARD_MC_CNT", DNX_EGQ_COUTNER_TYPE_PACKET, TRUE);
    counter_idx++;

    field_id = DBAL_FIELD_PRP_PACKET_DISCARD_TDM_CNT;
    DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                             "PRP_PACKET_DISCARD_TDM_CNT", DNX_EGQ_COUTNER_TYPE_PACKET, TRUE);
    counter_idx++;

    field_id = DBAL_FIELD_PRP_SOP_DISCARD_UC_CNT;
    DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                             "PRP_SOP_DISCARD_UC_CNT", DNX_EGQ_COUTNER_TYPE_PACKET, TRUE);
    counter_idx++;

    field_id = DBAL_FIELD_PRP_SOP_DISCARD_MC_CNT;
    DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                             "PRP_SOP_DISCARD_MC_CNT", DNX_EGQ_COUTNER_TYPE_PACKET, TRUE);
    counter_idx++;

    field_id = DBAL_FIELD_PRP_SOP_DISCARD_TDM_CNT;
    DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                             "PRP_SOP_DISCARD_TDM_CNT", DNX_EGQ_COUTNER_TYPE_PACKET, TRUE);
    counter_idx++;

    field_id = DBAL_FIELD_PRP_TDM_BYPASS_PACKET_CNT;
    DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                             "PRP_TDM_BYPASS_PACKET_CNT", DNX_EGQ_COUTNER_TYPE_PACKET, TRUE);
    counter_idx++;

    *nof_counter = counter_idx;

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dnx_gtimer_get(unit, SOC_BLK_RQP, core, &is_gtimer_enabled));
    for (counter_idx = 0; counter_idx < *nof_counter; counter_idx++)
    {
        size = counter_info[counter_idx].counter_size;
        if (counter_info[counter_idx].has_ovf)
        {
            counter_info[counter_idx].count_ovf =
                is_gtimer_enabled ? 0 : ((counter_info[counter_idx].count_val >> (size - 1)) && 0x1);
            counter_info[counter_idx].count_val = counter_info[counter_idx].count_val & ((1UL << (size - 1)) - 1);
        }
        else
        {
            counter_info[counter_idx].count_ovf = 0;
            counter_info[counter_idx].count_val = counter_info[counter_idx].count_val & ((1UL << size) - 1);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See egr_queuing.h
 */
shr_error_e
dnx_egr_queuing_pqp_counter_configuration_set(
    int unit,
    int core,
    int count_by,
    int count_place)
{
    int is_by_qp = 0, is_by_otm = 0, is_by_if = 0;
    int qp = 0, otm = 0, egq_if = 0;
    int logic_port = 0, base_qp = 0;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    switch (count_by)
    {
        case DNX_EGQ_COUTNER_FILTER_BY_QP:
            is_by_qp = 1;
            qp = count_place;
            break;
        case DNX_EGQ_COUTNER_FILTER_BY_OTM:
            is_by_otm = 1;
            otm = count_place;
            SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core, otm, &logic_port));
            SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logic_port, &base_qp));
            break;
        case DNX_EGQ_COUTNER_FILTER_BY_IF:
            is_by_if = 1;
            egq_if = count_place;
            break;
        case DNX_EGQ_COUTNER_FILTER_BY_NONE:
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected counter by %d\n", count_by);
            break;
    }

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGQ_PQP_COUNTER, &entry_handle_id));

    /** Select the core */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Set counter by qp, hr or egq if */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENQ_COUNT_BY_QP, INST_SINGLE, is_by_qp);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENQ_COUNT_BY_OTM, INST_SINGLE, is_by_otm);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENQ_COUNT_BY_IF, INST_SINGLE, is_by_if);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DEQ_COUNT_BY_QP, INST_SINGLE, is_by_qp);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DEQ_COUNT_BY_OTM, INST_SINGLE, is_by_otm);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DEQ_COUNT_BY_IF, INST_SINGLE, is_by_if);

    /** Set place to track its count   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENQ_QP, INST_SINGLE, qp);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENQ_OTM, INST_SINGLE, otm);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENQ_EGQ_IF, INST_SINGLE, egq_if);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DEQ_QP, INST_SINGLE, qp);
    /** base qpair is requested to set to DEQ_OTM due to HW design   */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DEQ_OTM, INST_SINGLE, base_qp);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DEQ_EGQ_IF, INST_SINGLE, egq_if);

    /** Write to HW */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See egr_queuing.h
 */
shr_error_e
dnx_egr_queuing_pqp_counter_configuration_reset(
    int unit,
    int core)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGQ_PQP_COUNTER, &entry_handle_id));

   /** Select the core */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Disable the filters*/
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See egr_queuing.h
 */
shr_error_e
dnx_egr_queuing_pqp_counter_info_get(
    int unit,
    int core,
    dnx_egq_counter_info_t * counter_info)
{
    int size;
    int counter_idx = 0;
    int is_gtimer_enabled = 0;
    uint32 entry_handle_id;
    dbal_tables_e table_id;
    dbal_fields_e field_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    table_id = DBAL_TABLE_EGQ_PQP_COUNTER;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGQ_PQP_COUNTER, &entry_handle_id));

    /** Select the core */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Get count value*/
    field_id = DBAL_FIELD_ENQ_PACKET_COUNTER;
    DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                             "PQP_ENQ_PACKET_COUNTER", DNX_EGQ_COUTNER_TYPE_PACKET, TRUE);
    counter_idx++;

    field_id = DBAL_FIELD_ENQ_BYTES_COUNTER;
    DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                             "PQP_ENQ_BYTES_COUNTER", DNX_EGQ_COUTNER_TYPE_BYTE, TRUE);
    counter_idx++;

    field_id = DBAL_FIELD_DEQ_UNICAST_PACKET_COUNTER;
    DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                             "PQP_DEQ_UNICAST_PACKET_COUNTER", DNX_EGQ_COUTNER_TYPE_PACKET, TRUE);
    counter_idx++;

    field_id = DBAL_FIELD_DEQ_MULTICAST_PACKET_COUNTER;
    DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                             "PQP_DEQ_MULTICAST_PACKET_COUNTER", DNX_EGQ_COUTNER_TYPE_PACKET, TRUE);
    counter_idx++;

    field_id = DBAL_FIELD_DEQ_DISCARD_UNICAST_PACKET_COUNTER;
    DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                             "PQP_DEQ_DISCARD_UNICAST_PACKET_COUNTER", DNX_EGQ_COUTNER_TYPE_PACKET, TRUE);
    counter_idx++;

    field_id = DBAL_FIELD_DEQ_DISCARD_MULTICAST_PACKET_COUNTER;
    DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                             "PQP_DEQ_DISCARD_MULTICAST_PACKET_COUNTER", DNX_EGQ_COUTNER_TYPE_PACKET, TRUE);
    counter_idx++;

    field_id = DBAL_FIELD_DEQ_UNICAST_BYTES_COUNTER;
    DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                             "PQP_DEQ_UNICAST_BYTES_COUNTER", DNX_EGQ_COUTNER_TYPE_BYTE, TRUE);
    counter_idx++;

    field_id = DBAL_FIELD_DEQ_MULTICAST_BYTES_COUNTER;
    DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                             "PQP_DEQ_MULTICAST_BYTES_COUNTER", DNX_EGQ_COUTNER_TYPE_BYTE, TRUE);
    counter_idx++;

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dnx_gtimer_get(unit, SOC_BLK_PQP, core, &is_gtimer_enabled));
    for (counter_idx = 0; counter_idx < DNX_EGQ_PQP_COUNTER_NUM; counter_idx++)
    {
        size = counter_info[counter_idx].counter_size;
        if (counter_info[counter_idx].has_ovf)
        {
            counter_info[counter_idx].count_ovf =
                is_gtimer_enabled ? 0 : ((counter_info[counter_idx].count_val >> (size - 1)) && 0x1);
            counter_info[counter_idx].count_val = counter_info[counter_idx].count_val & ((1UL << (size - 1)) - 1);
        }
        else
        {
            counter_info[counter_idx].count_ovf = 0;
            counter_info[counter_idx].count_val = counter_info[counter_idx].count_val & ((1UL << size) - 1);
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See egr_queuing.h
 */
shr_error_e
dnx_egr_queuing_epni_counter_configuration_set(
    int unit,
    int core,
    int flag,
    int count_by,
    int count_place)
{
    int port;
    int base_qp;
    int num_priorities;
    int is_count_byte = 0;
    int q_mask, if_mask, mirror_pri_mask;
    int q_val = 0, if_val = 0, mirror_pri = 0;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    dbal_tables_field_max_value_get(unit, DBAL_TABLE_EGQ_EPNI_COUNTER, DBAL_FIELD_Q_MASK, 0, 0, 0, &q_mask);
    dbal_tables_field_max_value_get(unit, DBAL_TABLE_EGQ_EPNI_COUNTER, DBAL_FIELD_IF_MASK, 0, 0, 0, &if_mask);
    dbal_tables_field_max_value_get(unit, DBAL_TABLE_EGQ_EPNI_COUNTER, DBAL_FIELD_MIRR_PRIO_MASK, 0, 0, 0,
                                    &mirror_pri_mask);

    switch (count_by)
    {
        case DNX_EGQ_COUTNER_FILTER_BY_QP:
            q_mask = 0;
            q_val = count_place;
            break;
        case DNX_EGQ_COUTNER_FILTER_BY_OTM:
            port = count_place;
            SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, port, &base_qp));
            SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, port, &num_priorities));
            q_mask = num_priorities - 1;
            q_val = base_qp;
            break;
        case DNX_EGQ_COUTNER_FILTER_BY_IF:
            if_mask = 0;
            if_val = count_place;
            break;
        case DNX_EGQ_COUTNER_FILTER_BY_MIRROR:
            mirror_pri_mask = 0;
            mirror_pri = count_place;
            break;
        case DNX_EGQ_COUTNER_FILTER_BY_NONE:
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unexpected counter by %d\n", count_by);
            break;
    }

    if (flag & DNX_EGQ_CONFIGURATION_COUNT_BYTES)
    {
        is_count_byte = 1;
    }

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGQ_EPNI_COUNTER, &entry_handle_id));

    /** Select the core */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Set counter by qp, hr or egq if */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNT_BYTES, INST_SINGLE, is_count_byte);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_Q_VAL, INST_SINGLE, q_val);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_Q_MASK, INST_SINGLE, q_mask);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IF_VAL, INST_SINGLE, if_val);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IF_MASK, INST_SINGLE, if_mask);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIRR_PRIO_VAL, INST_SINGLE, mirror_pri);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIRR_PRIO_MASK, INST_SINGLE, mirror_pri_mask);

    /** Write to HW */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See egr_queuing.h
 */
shr_error_e
dnx_egr_queuing_epni_counter_configuration_reset(
    int unit,
    int core)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGQ_EPNI_COUNTER, &entry_handle_id));

   /** Select the core */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Disable the filters*/
    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See egr_queuing.h
 */
shr_error_e
dnx_egr_queuing_epni_counter_info_get(
    int unit,
    int core,
    int flag,
    dnx_egq_counter_info_t * counter_info,
    int *nof_counter)
{
    int size;
    int counter_idx = 0;
    int counter_type;
    int is_gtimer_enabled = 0;
    char *name_suffix = NULL;
    uint32 is_count_bytes;
    uint32 entry_handle_id;
    dbal_tables_e table_id;
    dbal_fields_e field_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle to DBAL table */
    table_id = DBAL_TABLE_EGQ_EPNI_COUNTER;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGQ_EPNI_COUNTER, &entry_handle_id));

    /** Select the core */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /** Get the setting of byte or packet */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_COUNT_BYTES, INST_SINGLE, &is_count_bytes);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    if (is_count_bytes)
    {
        counter_type = DNX_EGQ_COUTNER_TYPE_BYTE;
        name_suffix = "_BYTE";
    }
    else
    {
        counter_type = DNX_EGQ_COUTNER_TYPE_PACKET;
        name_suffix = "_PACKET";
    }

    if (flag & DNX_EGQ_EPNI_QUEUE_COUNTER)
    {
        field_id = DBAL_FIELD_Q_CNT;
        DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                                 "EPNI_Q_CNT", counter_type, TRUE);
        counter_idx++;
    }
    if (flag & DNX_EGQ_EPNI_IF_COUNTER)
    {
        field_id = DBAL_FIELD_IFC_CNT;
        DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                                 "EPNI_IFC_CNT", counter_type, TRUE);
        counter_idx++;
    }
    if (flag & DNX_EGQ_EPNI_MIRROR_COUNTER)
    {
        field_id = DBAL_FIELD_MIRR_CNT;
        DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                                 "EPNI_MIRR_CNT", counter_type, TRUE);
        counter_idx++;
    }
    if (flag & DNX_EGQ_NON_PROGRAMMABLE_COUNTER)
    {
        field_id = DBAL_FIELD_DISC_CNT;
        DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                                 "EPNI_DISC_CNT", counter_type, TRUE);
        counter_idx++;

        field_id = DBAL_FIELD_FRWD_DISC_CNT;
        DNX_EGQ_COUNTER_INFO_GET(counter_info[counter_idx], entry_handle_id, table_id, field_id,
                                 "EPNI_FRWD_DISC_CNT", counter_type, TRUE);
        counter_idx++;
    }

    *nof_counter = counter_idx;

    /** Performing the action */
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    SHR_IF_ERR_EXIT(dnx_gtimer_get(unit, SOC_BLK_EPNI, core, &is_gtimer_enabled));
    for (counter_idx = 0; counter_idx < *nof_counter; counter_idx++)
    {
        size = counter_info[counter_idx].counter_size;
        if (counter_info[counter_idx].has_ovf)
        {
            counter_info[counter_idx].count_ovf =
                is_gtimer_enabled ? 0 : ((counter_info[counter_idx].count_val >> (size - 1)) && 0x1);
            counter_info[counter_idx].count_val = counter_info[counter_idx].count_val & ((1UL << (size - 1)) - 1);
        }
        else
        {
            counter_info[counter_idx].count_ovf = 0;
            counter_info[counter_idx].count_val = counter_info[counter_idx].count_val & ((1UL << size) - 1);
        }
        sal_snprintf(counter_info[counter_idx].name + sal_strlen(counter_info[counter_idx].name),
                     sal_strlen(name_suffix) + 1, "%s", name_suffix);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_egr_queuing_port_disable(
    int unit)
{
    bcm_port_t port;
    dnx_algo_port_type_e port_type;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_removed_port_get(unit, &port));
    SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, port, &port_type));

    /**
     * omit non egress ports
     */
    if (!DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_type))
    {
        SHR_EXIT();
    }

    /** disable port qpairs and flush */
    SHR_IF_ERR_EXIT(dnx_egr_queuing_port_enable_and_flush_set(unit, port, FALSE, TRUE));

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 */
shr_error_e
dnx_egr_queuing_init(
    int unit)
{
    uint32 profile;
    int core;
    dnx_cosq_egress_queue_mapping_info_t *mapping_info = NULL;
    uint32 tdm_dp, tdm_tc, tdm_exist;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * get tdm general information
     */
    tdm_tc = dnx_data_egr_queuing.params.tdm_tc_get(unit);
    tdm_dp = dnx_data_egr_queuing.params.tdm_dp_get(unit);
    tdm_exist = dnx_data_egr_queuing.params.tdm_exist_get(unit);
    /*
     * init egr data base
     */
    SHR_IF_ERR_EXIT(dnx_egr_db.init(unit));
    /*
     * allocate reserved egress queuing interface (used to disable interface flow control mapping in LAG SCH)
     */
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        SHR_IF_ERR_EXIT(dnx_egr_db.
                        interface_occ.set(unit, core, dnx_data_egr_queuing.params.reserved_if_get(unit), TRUE));
    }
    /*
     * create port tc/dp default profile
     */
    SHR_ALLOC(mapping_info, sizeof(dnx_cosq_egress_queue_mapping_info_t), "egress queue mapping info handle memory",
              "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    if (mapping_info == NULL)
    {
        SHR_ERR_EXIT(_SHR_E_MEMORY, "unit %d, failed to allocate egress queue mapping info handle memory\n", unit);
    }
    sal_memset(mapping_info, 0x0, sizeof(dnx_cosq_egress_queue_mapping_info_t));
    profile = DNX_EGR_QUEUE_DEFAULT_PROFILE_MAP;
    SHR_IF_ERR_EXIT(dnx_am_template_egress_queue_mapping_create(unit, profile, mapping_info));
    {
        /*
         * egr TDM configuration. consider to move to TDM init
         */
        uint32 entry_handle_id;
        dbal_tables_e dbal_table_id;
        /*
         * All settings are for all-cores.
         */
        core = BCM_CORE_ALL;
        dbal_table_id = DBAL_TABLE_EGQ_GLOBAL_CONFIGURATION;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        /*
         * key construction.
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        /*
         * Set the values. Note that we define, here, a 'group' of HW tables because they
         * all do the same: ERPP_TDM_GENERAL_CONFIGURATIONr, RQP_TDM_GENERAL_CONFIGURATIONr,
         * ETPPA_TDM_GENERAL_CONFIGURATIONr
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_TDM_MODE, INST_SINGLE,
                                     (uint32) (tdm_exist ? 1 : 0));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TDM_PKT_TC, INST_SINGLE, (uint32) (tdm_tc));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TDM_PKT_DP, INST_SINGLE, (uint32) (tdm_dp));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        DBAL_HANDLE_FREE(unit, entry_handle_id);
        /*
         * configures egress multicast replication fifo parameters received from dnx data to HW
         */
        SHR_IF_ERR_EXIT(dnx_egr_queuing_emr_fifo_default_set(unit, core, tdm_exist));
        /*
         * Define TXQ threshold in PDs. values received from ASIC
         */
        dbal_table_id = DBAL_TABLE_EGQ_GLOBAL_CONFIGURATION;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        /*
         * key construction.
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        /*
         * set the value of TXQ_IREADY_TH, TXQ_TDM_IREADY_TH
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TXQ_IREADY_TH, INST_SINGLE,
                                     dnx_data_egr_queuing.params.txq_iready_th_get(unit));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TXQ_TDM_IREADY_TH, INST_SINGLE,
                                     dnx_data_egr_queuing.params.txq_tdm_iready_th_get(unit));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        /*
         * fqp/pqp calendar enable
         */
        /*
         * Reuse allocated buffer.
         */
        dbal_table_id = DBAL_TABLE_EGQ_SHAPER_GLOBAL_CONFIGURATION;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_table_id, entry_handle_id));
        /*
         * key construction.
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        /*
         * set the value of FQP_CALENDAR_SWITCH_EN, PQP_CALENDAR_SWITCH_EN
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FQP_CALENDAR_SWITCH_EN, INST_SINGLE,
                                     (uint32) (1));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PQP_CALENDAR_SWITCH_EN, INST_SINGLE,
                                     (uint32) (1));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        /*
         * Enable invalid OTM to support Egress MC groups with trunk destinations
         */
        /*
         * Reuse allocated buffer.
         */
        dbal_table_id = DBAL_TABLE_EGQ_GLOBAL_CONFIGURATION;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_table_id, entry_handle_id));
        /*
         * key construction.
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        /*
         * set the value of INVALID_OTM_ENA
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INVALID_OTM_ENA, INST_SINGLE, (uint32) (1));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        /*
         * set force mode
         */
        /*
         * Reuse allocated buffer.
         */
        dbal_table_id = DBAL_TABLE_EGQ_SHAPER_CONFIGURATION;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_table_id, entry_handle_id));
        /*
         * key construction.
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        /*
         * set the value of FORCE_FC_MODE
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FORCE_FC_MODE, INST_SINGLE, (uint32) (0));

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    /*
     * setting recycle interface burst to max
     */
    SHR_IF_ERR_EXIT(dnx_egr_recycle_burst_set(unit, _SHR_CORE_ALL, 0, DNX_OFP_RATES_BURST_DEFAULT /
                                              dnx_data_egr_queuing.params.cal_burst_res_get(unit)));
    SHR_IF_ERR_EXIT(dnx_egr_recycle_burst_set(unit, _SHR_CORE_ALL, 1, DNX_OFP_RATES_BURST_DEFAULT /
                                              dnx_data_egr_queuing.params.cal_burst_res_get(unit)));

exit:
    SHR_FREE(mapping_info);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See egr_queuing.h file
 */
shr_error_e
dnx_egr_queuing_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * sw state module deinit is done automatically at device deinit 
     */

    SHR_FUNC_EXIT;
}
/*
 * Procedures imported from arad_egr_queuing.c
 * {
 */

#if (1)
/* { */

void
dnx_egr_ofp_sch_wfq_clear(
    dnx_egr_ofp_sch_wfq_t * info)
{
    SHR_FUNC_INIT_VARS(NO_UNIT);
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    sal_memset(info, 0x0, sizeof(*info));
    info->unsched_weight = 0;
    info->sched_weight = 0;
exit:
    SHR_VOID_FUNC_EXIT;
}

void
dnx_egr_ofp_sch_info_clear(
    dnx_egr_ofp_sch_info_t * info)
{
    dnx_egr_q_prio_e q_pair;

    SHR_FUNC_INIT_VARS(NO_UNIT);
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    sal_memset(info, 0x0, sizeof(*info));
    info->nif_priority = DNX_EGR_OFP_INTERFACE_PRIO_LOW;
    dnx_egr_ofp_sch_wfq_clear(&(info->ofp_wfq));
    dnx_egr_ofp_sch_wfq_clear(&(info->ofp_wfq_high));
    for (q_pair = 0; q_pair < DNX_EGR_NOF_Q_PRIO; ++q_pair)
    {
        dnx_egr_ofp_sch_wfq_clear(&(info->ofp_wfq_q_pair[q_pair]));
    }
exit:
    SHR_VOID_FUNC_EXIT;
}

void
dnx_egr_q_priority_clear(
    dnx_egr_q_priority_t * info)
{
    SHR_FUNC_INIT_VARS(NO_UNIT);
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    sal_memset(info, 0x0, sizeof(*info));
    info->tc = 0;
    info->dp = 0;
exit:
    SHR_VOID_FUNC_EXIT;
}

void
dnx_egr_queuing_tcg_info_clear(
    dnx_egr_queuing_tcg_info_t * info)
{
    SHR_FUNC_INIT_VARS(NO_UNIT);
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");
    sal_memset(info, 0x0, sizeof(*info));
exit:
    SHR_VOID_FUNC_EXIT;
}
/*
 * Read indirect table dwm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
static int
dnx_egq_dwm_tbl_get(
    int unit,
    int core,
    uint32 entry_offset,
    dnx_egq_dwm_tbl_data_t * egq_dwm_tbl_data)
{
    dbal_fields_e dbal_field_id;
    dbal_tables_e dbal_table_id;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(egq_dwm_tbl_data, 0x0, sizeof(*egq_dwm_tbl_data));
    /*
     * Allocate buffer.
     */
    dbal_table_id = DBAL_TABLE_EGQ_QPAIR_ATTRIBUTES;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    /*
     * key construction.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QPAIR, (uint32) (entry_offset));
    /*
     * Get the value of: DBAL_FIELD_MC_OR_MC_LOW_QUEUE_WEIGHT
     * Get the value of: DBAL_FIELD_UC_OR_UC_LOW_QUEUE_WEIGHT
     */
    egq_dwm_tbl_data->mc_or_mc_low_queue_weight = 0;
    egq_dwm_tbl_data->uc_or_uc_low_queue_weight = 0;
    dbal_field_id = DBAL_FIELD_MC_OR_MC_LOW_QUEUE_WEIGHT;
    dbal_value_field32_request(unit, entry_handle_id, dbal_field_id, INST_SINGLE,
                               (uint32 *) (&(egq_dwm_tbl_data->mc_or_mc_low_queue_weight)));
    dbal_field_id = DBAL_FIELD_UC_OR_UC_LOW_QUEUE_WEIGHT;
    dbal_value_field32_request(unit, entry_handle_id, dbal_field_id, INST_SINGLE,
                               (uint32 *) (&(egq_dwm_tbl_data->uc_or_uc_low_queue_weight)));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/*
 * Write indirect table dwm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
static int
dnx_egq_dwm_tbl_set(
    int unit,
    int core,
    uint32 entry_offset,
    dnx_egq_dwm_tbl_data_t * egq_dwm_tbl_data)
{
    dbal_fields_e dbal_field_id;
    dbal_tables_e dbal_table_id;
    uint32 entry_handle_id;
    uint32 dbal_field_val;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Allocate buffer.
     */
    dbal_table_id = DBAL_TABLE_EGQ_QPAIR_ATTRIBUTES;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    /*
     * key construction.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QPAIR, (uint32) (entry_offset));
    /*
     * Set the variable: DBAL_FIELD_MC_OR_MC_LOW_QUEUE_WEIGHT
     * Set the variable: DBAL_FIELD_UC_OR_UC_LOW_QUEUE_WEIGHT
     */
    dbal_field_id = DBAL_FIELD_MC_OR_MC_LOW_QUEUE_WEIGHT;
    dbal_field_val = egq_dwm_tbl_data->mc_or_mc_low_queue_weight;
    dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, (uint32) (dbal_field_val));
    dbal_field_id = DBAL_FIELD_UC_OR_UC_LOW_QUEUE_WEIGHT;
    dbal_field_val = egq_dwm_tbl_data->uc_or_uc_low_queue_weight;
    dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, (uint32) (dbal_field_val));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/* 
 * Set WFQ settings for a specificed queue in the OFP.
 */
static int
dnx_egr_ofp_scheduling_wfq_set(
    int unit,
    uint32 tm_port,
    int core,
    uint32 cosq,
    dnx_egr_ofp_sch_wfq_t * wfq_info)
{
    uint32 offset;
    dnx_egq_dwm_tbl_data_t dwm_tbl_data;
    int base_q_pair;
    dnx_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(wfq_info, _SHR_E_PARAM, "wfq_info");
    /*
     * Retrieve base_q_pair
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core, tm_port, &logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    /*
     * offset set by base_q_pair + cosq
     */
    offset = base_q_pair + cosq;
    dwm_tbl_data.uc_or_uc_low_queue_weight = wfq_info->sched_weight;
    dwm_tbl_data.mc_or_mc_low_queue_weight = wfq_info->unsched_weight;
    SHR_IF_ERR_EXIT(dnx_egq_dwm_tbl_set(unit, core, offset, &dwm_tbl_data));
exit:
    SHR_FUNC_EXIT;
}
/* 
 * Set WFQ settings for a specificed queue in the OFP.
 */
static int
dnx_egr_ofp_scheduling_wfq_get(
    int unit,
    uint32 tm_port,
    int core,
    uint32 cosq,
    dnx_egr_ofp_sch_wfq_t * wfq_info)
{
    uint32 offset;
    dnx_egq_dwm_tbl_data_t dwm_tbl_data;
    int base_q_pair;
    dnx_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(wfq_info, _SHR_E_PARAM, "wfq_info");
    sal_memset(&dwm_tbl_data, 0, sizeof(dwm_tbl_data));
    /*
     * Retrieve base_q_pair
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core, tm_port, &logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    /*
     * offset set by base_q_pair + cosq
     */
    offset = base_q_pair + cosq;
    SHR_IF_ERR_EXIT(dnx_egq_dwm_tbl_get(unit, core, offset, &dwm_tbl_data));
    wfq_info->sched_weight = dwm_tbl_data.uc_or_uc_low_queue_weight;
    wfq_info->unsched_weight = dwm_tbl_data.mc_or_mc_low_queue_weight;
exit:
    SHR_FUNC_EXIT;
}
/*********************************************************************
*     Set per-port egress scheduling information.
*     Details: below
*********************************************************************/
/*********************************************************************
* NAME:
*     dnx_egr_ofp_scheduling_verify
* TYPE:
*   PROC
* DATE:
*   Dec 20 2007
* FUNCTION:
*   See dnx_egr_ofp_scheduling_set
* REMARKS:
*   Some of the scheduling info may be irrelevant -
*   i.e. channelized interface priority is only relevant if the port
*   is mapped to a channelized NIF.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
static int
dnx_egr_ofp_scheduling_verify(
    int unit,
    dnx_fap_port_id_t ofp_ndx,
    dnx_egr_ofp_sch_info_t * info)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

    if (info->nif_priority > (DNX_EGR_OFP_CHNIF_NOF_PRIORITIES - 2))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "info->nif_priority (%d) is above allowed maximum (%d).",
                     info->nif_priority, (DNX_EGR_OFP_CHNIF_NOF_PRIORITIES - 2));
    }
    if (info->nif_priority == DNX_EGR_OFP_INTERFACE_PRIO_MID)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "info->nif_priority (%d) is equal to DNX_EGR_OFP_INTERFACE_PRIO_MID (%d).",
                     info->nif_priority, DNX_EGR_OFP_INTERFACE_PRIO_MID);
    }
    if (info->ofp_wfq.sched_weight > (DNX_EGR_OFP_SCH_WFQ_WEIGHT_MAX))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "info->ofp_wfq.sched_weight (%d) is above allowed maximum (%d).",
                     info->ofp_wfq.sched_weight, (DNX_EGR_OFP_SCH_WFQ_WEIGHT_MAX));
    }
    if (info->ofp_wfq.unsched_weight > (DNX_EGR_OFP_SCH_WFQ_WEIGHT_MAX))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "info->ofp_wfq.unsched_weight (%d) is above allowed maximum (%d).",
                     info->ofp_wfq.unsched_weight, (DNX_EGR_OFP_SCH_WFQ_WEIGHT_MAX));
    }
    if (info->ofp_wfq_high.sched_weight > (DNX_EGR_OFP_SCH_WFQ_WEIGHT_MAX))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "info->ofp_wfq_high.sched_weight (%d) is above allowed maximum (%d).",
                     info->ofp_wfq_high.sched_weight, (DNX_EGR_OFP_SCH_WFQ_WEIGHT_MAX));
    }
    if (info->ofp_wfq_high.unsched_weight > (DNX_EGR_OFP_SCH_WFQ_WEIGHT_MAX))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "info->ofp_wfq_high.unsched_weight (%d) is above allowed maximum (%d).",
                     info->ofp_wfq_high.unsched_weight, (DNX_EGR_OFP_SCH_WFQ_WEIGHT_MAX));
    }
exit:
    SHR_FUNC_EXIT;
}

/*********************************************************************
*     Set per-port egress scheduling information.
*     Details: in the H file. (search for prototype)
*********************************************************************/
int
dnx_egr_ofp_scheduling_set(
    int unit,
    int core,
    uint32 tm_port,
    uint32 cosq,
    dnx_egr_ofp_sch_info_t * info)
{
    uint32 nif_prio_fld_val;
    int nof_q_pairs, base_q_pair;
    uint32 q_pair_in_port;
    dnx_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");
    SHR_IF_ERR_EXIT(dnx_egr_ofp_scheduling_verify(unit, tm_port, info));
    /*
     * Set NIF priority. {
     */
    switch (info->nif_priority)
    {
        case DNX_EGR_OFP_INTERFACE_PRIO_HIGH:
        {
            nif_prio_fld_val = 0x1;
            break;
        }
        case DNX_EGR_OFP_INTERFACE_PRIO_LOW:
        {
            nif_prio_fld_val = 0x0;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "info->nif_priority (%d) is out of range", info->nif_priority);
        }
    }
    /*
     * Retrieve base_q_pair
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core, tm_port, &logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    /*
     * Set the OFP nif-priority. Each bit correspond to a OFP
     */
    {
        dbal_fields_e dbal_field_id;
        dbal_tables_e dbal_table_id;
        uint32 entry_handle_id;
        uint32 dbal_field_val;
        /*
         * Allocate buffer.
         */
        dbal_table_id = DBAL_TABLE_EGQ_QPAIR_ATTRIBUTES;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        /*
         * key construction.
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QPAIR, (uint32) (base_q_pair + cosq));
        /*
         * Set the variable: DBAL_FIELD_QP_IS_HP
         */
        dbal_field_id = DBAL_FIELD_QP_IS_HP;
        dbal_field_val = nif_prio_fld_val;
        dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, (uint32) (dbal_field_val));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    /*
     * Set NIF priority. }
     */
    /*
     * Set WFQ weights {.
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, logical_port, &nof_q_pairs));
    for (q_pair_in_port = 0; q_pair_in_port < nof_q_pairs; ++q_pair_in_port)
    {
        SHR_IF_ERR_EXIT(dnx_egr_ofp_scheduling_wfq_set(unit,
                                                       tm_port,
                                                       core, q_pair_in_port, &(info->ofp_wfq_q_pair[q_pair_in_port])));
    }
    /*
     * Set WFQ weights }
     */
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/*********************************************************************
*     Set per-port egress scheduling information.
*     Details: in the H file. (search for prototype)
*********************************************************************/
int
dnx_egr_ofp_scheduling_get(
    int unit,
    int core,
    uint32 tm_port,
    uint32 cosq,
    dnx_egr_ofp_sch_info_t * info)
{
    uint32 nif_prio_val;
    int base_q_pair, nof_q_pairs;
    uint32 q_pair_in_port;
    dnx_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");
    /*
     * Retrieve base_q_pair
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core, tm_port, &logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    /*
     * Get NIF priority. {
     */
    {
        dbal_fields_e dbal_field_id;
        dbal_tables_e dbal_table_id;
        uint32 entry_handle_id;
        /*
         * Allocate buffer.
         */
        dbal_table_id = DBAL_TABLE_EGQ_QPAIR_ATTRIBUTES;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        /*
         * key construction.
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_QPAIR, (uint32) (base_q_pair + cosq));
        /*
         * Get the value: DBAL_FIELD_QP_IS_HP
         */
        nif_prio_val = 0;
        dbal_field_id = DBAL_FIELD_QP_IS_HP;
        dbal_value_field_arr32_request(unit, entry_handle_id, dbal_field_id, INST_SINGLE, (uint32 *) (&nif_prio_val));
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        info->nif_priority = (nif_prio_val == 0) ? DNX_EGR_OFP_INTERFACE_PRIO_LOW : DNX_EGR_OFP_INTERFACE_PRIO_HIGH;
    }
    /*
     * Get NIF priority. }
     */
    /*
     * Get WFQ weights.
     * Settings should be done via this API, only when the OFP port is a 2 priority mode.
     * The API skip WFQ weights configuration when port is not in 2 priority mode. 
     * Refer to API: () to configure WFQ weights for other modes (1/8 priority mode)
     */
    /*
     * Get WFQ weights {.
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, logical_port, &nof_q_pairs));
    for (q_pair_in_port = 0; q_pair_in_port < nof_q_pairs; ++q_pair_in_port)
    {
        SHR_IF_ERR_EXIT(dnx_egr_ofp_scheduling_wfq_get(unit,
                                                       tm_port,
                                                       core, q_pair_in_port, &(info->ofp_wfq_q_pair[q_pair_in_port])));
    }
    /*
     * Get WFQ weights }
     */
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
static void
dnx_egq_tc_dp_map_tbl_entry_clear(
    dnx_egq_tc_dp_map_tbl_entry_t * info)
{
    SHR_FUNC_INIT_VARS(NO_UNIT);
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");
    sal_memset(info, 0x0, sizeof(*info));
    info->tc = 0;
    info->dp = 0;
    info->is_egr_mc = 0;
    info->map_profile = 0;
exit:
    SHR_VOID_FUNC_EXIT;
}
/*
 *    Internal conversion to the table entry format
 */
static int
dnx_egr_q_prio_map_entry_get(
    int unit,
    dnx_egr_q_prio_mapping_type_e map_type_ndx,
    uint32 tc_ndx,
    uint32 dp_ndx,
    uint32 map_profile_ndx,
    dnx_egq_tc_dp_map_tbl_entry_t * entry)
{
    uint8 is_egr_mc;
    SHR_FUNC_INIT_VARS(unit);
    is_egr_mc = FALSE;
    dnx_egq_tc_dp_map_tbl_entry_clear(entry);
    entry->map_profile = map_profile_ndx;
    entry->dp = dp_ndx;
    entry->tc = tc_ndx;
    switch (map_type_ndx)
    {
        case DNX_EGR_UCAST_TO_SCHED:
        {
            is_egr_mc = FALSE;
            break;
        }
        case DNX_EGR_MCAST_TO_UNSCHED:
        {
            is_egr_mc = TRUE;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Illegal map_type_ndx (%d). See dnx_egr_q_prio_mapping_type_e", map_type_ndx);
        }
    }
    entry->is_egr_mc = is_egr_mc;
exit:
    SHR_FUNC_EXIT;
}
/*
 * Write indirect table tc_dp_map_table_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
static int
dnx_egq_tc_dp_map_tbl_set(
    int unit,
    int core_id,
    dnx_egq_tc_dp_map_tbl_entry_t * entry,
    dnx_egq_tc_dp_map_tbl_data_t * tbl_data)
{
    dbal_fields_e dbal_field_id;
    dbal_tables_e dbal_table_id;
    uint32 entry_handle_id;
    uint32 dbal_field_val;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Allocate buffer.
     */
    dbal_table_id = DBAL_TABLE_EGQ_TC_DP_MAP;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    /*
     * key construction.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYS_TC, (uint32) (entry->tc));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYS_DP, (uint32) (entry->dp));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_EGRESS_MC, (uint32) (entry->is_egr_mc));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAP_PROFILE, (uint32) (entry->map_profile));
    /*
     * Set the variable: DBAL_FIELD_EGRESS_TC
     * Set the variable: DBAL_FIELD_CGM_MC_DP
     */
    dbal_field_id = DBAL_FIELD_EGRESS_TC;
    dbal_field_val = tbl_data->tc;
    dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, (uint32) (dbal_field_val));
    dbal_field_id = DBAL_FIELD_CGM_MC_DP;
    dbal_field_val = tbl_data->dp;
    dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, (uint32) (dbal_field_val));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*********************************************************************
*     Sets egress queue priority per traffic class and drop
 *     precedence.
 *     Details: in the H file: egr_queuing.h
*********************************************************************/
int
dnx_egr_q_prio_set(
    int unit,
    int core,
    dnx_egr_q_prio_mapping_type_e map_type_ndx,
    uint32 tc_ndx,
    uint32 dp_ndx,
    uint32 map_profile_ndx,
    dnx_egr_q_priority_t * priority)
{
    dnx_egq_tc_dp_map_tbl_entry_t entry;
    dnx_egq_tc_dp_map_tbl_data_t tbl_data;

    SHR_FUNC_INIT_VARS(unit);
    dnx_egq_tc_dp_map_tbl_entry_clear(&entry);
    SHR_IF_ERR_EXIT(dnx_egr_q_prio_map_entry_get(unit, map_type_ndx, tc_ndx, dp_ndx, map_profile_ndx, &entry));
    tbl_data.dp = priority->dp;
    tbl_data.tc = priority->tc;
    SHR_IF_ERR_EXIT(dnx_egq_tc_dp_map_tbl_set(unit, core, &entry, &tbl_data));
exit:
    SHR_FUNC_EXIT;
}
static int
dnx_egq_tc_dp_map_tbl_get(
    int unit,
    int core_id,
    dnx_egq_tc_dp_map_tbl_entry_t * entry,
    dnx_egq_tc_dp_map_tbl_data_t * tbl_data)
{
    dbal_fields_e dbal_field_id;
    dbal_tables_e dbal_table_id;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Allocate buffer.
     */
    dbal_table_id = DBAL_TABLE_EGQ_TC_DP_MAP;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    /*
     * key construction.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYS_TC, (uint32) (entry->tc));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYS_DP, (uint32) (entry->dp));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_EGRESS_MC, (uint32) (entry->is_egr_mc));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_MAP_PROFILE, (uint32) (entry->map_profile));
    /*
     * Get the value: DBAL_FIELD_EGRESS_TC
     * Get the value: DBAL_FIELD_CGM_MC_DP
     */
    dbal_field_id = DBAL_FIELD_EGRESS_TC;
    dbal_value_field_arr32_request(unit, entry_handle_id, dbal_field_id, INST_SINGLE, (uint32 *) &(tbl_data->tc));
    dbal_field_id = DBAL_FIELD_CGM_MC_DP;
    dbal_value_field_arr32_request(unit, entry_handle_id, dbal_field_id, INST_SINGLE, (uint32 *) &(tbl_data->dp));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*********************************************************************
*     Sets egress queue priority per traffic class and drop
 *     precedence.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
int
dnx_egr_q_prio_get(
    int unit,
    int core,
    dnx_egr_q_prio_mapping_type_e map_type_ndx,
    uint32 tc_ndx,
    uint32 dp_ndx,
    uint32 map_profile_ndx,
    dnx_egr_q_priority_t * priority)
{
    dnx_egq_tc_dp_map_tbl_entry_t entry;
    dnx_egq_tc_dp_map_tbl_data_t tbl_data;

    SHR_FUNC_INIT_VARS(unit);
    dnx_egr_q_priority_clear(priority);
    dnx_egq_tc_dp_map_tbl_entry_clear(&entry);
    SHR_IF_ERR_EXIT(dnx_egr_q_prio_map_entry_get(unit, map_type_ndx, tc_ndx, dp_ndx, map_profile_ndx, &entry));
    SHR_IF_ERR_EXIT(dnx_egq_tc_dp_map_tbl_get(unit, core, &entry, &tbl_data));
    priority->dp = tbl_data.dp;
    priority->tc = tbl_data.tc;

exit:
    SHR_FUNC_EXIT;
}
/*
 * Read indirect table ppct_tbl from block EGQ,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

static int
dnx_egq_ppct_tbl_get(
    int unit,
    int core_id,
    uint32 internal_port,
    dnx_egq_ppct_tbl_data_t * egq_ppct_tbl_data)
{
    dbal_fields_e dbal_field_id;
    dbal_tables_e dbal_table_id;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Allocate buffer.
     */
    dbal_table_id = DBAL_TABLE_EGRESS_PP_PORT;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    /*
     * key construction.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, (uint32) (internal_port));
    /*
     * Get the value: DBAL_FIELD_COS_MAP_PROFILE
     */
    dbal_field_id = DBAL_FIELD_COS_MAP_PROFILE;
    dbal_value_field_arr32_request(unit, entry_handle_id, dbal_field_id, INST_SINGLE,
                                   (uint32 *) &(egq_ppct_tbl_data->cos_map_profile));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/*
 * Write indirect table ppct_tbl from block EGQ,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */
static int
dnx_egq_ppct_tbl_set(
    int unit,
    int core_id,
    uint32 internal_port,
    dnx_egq_ppct_tbl_data_t * egq_ppct_tbl_data)
{
    dbal_fields_e dbal_field_id;
    dbal_tables_e dbal_table_id;
    uint32 entry_handle_id;
    uint32 dbal_field_val;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     * Allocate buffer.
     */
    dbal_table_id = DBAL_TABLE_EGRESS_PP_PORT;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    /*
     * key construction.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, (uint32) (internal_port));
    /*
     * Set the variable: DBAL_FIELD_COS_MAP_PROFILE
     */
    dbal_field_id = DBAL_FIELD_COS_MAP_PROFILE;
    dbal_field_val = egq_ppct_tbl_data->cos_map_profile;
    dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, (uint32) (dbal_field_val));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*********************************************************************
*     Function description
 *     Details: in the H file. (search for prototype)
*********************************************************************/
int
dnx_egr_q_profile_map_set(
    int unit,
    int core_id,
    uint32 internal_port,
    uint32 map_profile_id)
{
    dnx_egq_ppct_tbl_data_t ppct_tbl_data;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_egq_ppct_tbl_get(unit, core_id, internal_port, &ppct_tbl_data));
    ppct_tbl_data.cos_map_profile = map_profile_id;
    SHR_IF_ERR_EXIT(dnx_egq_ppct_tbl_set(unit, core_id, internal_port, &ppct_tbl_data));
exit:
    SHR_FUNC_EXIT;
}

/*********************************************************************
*     Function description
*     Details: in the H file egr_queuing.h
*********************************************************************/
int
dnx_egr_q_profile_map_get(
    int unit,
    int core_id,
    uint32 tm_port,
    uint32 *map_profile_id)
{
    dnx_egq_ppct_tbl_data_t tbl_data;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_egq_ppct_tbl_get(unit, core_id, tm_port, &tbl_data));
    *map_profile_id = tbl_data.cos_map_profile;
exit:
    SHR_FUNC_EXIT;
}

int
dnx_egr_dsp_pp_priorities_mode_set(
    int unit,
    int core_id,
    uint32 tm_port,
    dnx_egr_port_priority_mode_e priority_mode)
{
    uint32 egq_priority_val;
    uint32 ps;
    int base_q_pair;
    dnx_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core_id, tm_port, &logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    ps = base_q_pair / DNX_EGR_NOF_Q_PAIRS_IN_PS;
    switch (priority_mode)
    {
        case DNX_EGR_PORT_ONE_PRIORITY:
        {
            egq_priority_val = DNX_EGQ_PS_MODE_ONE_PRIORITY_VAL;
            break;
        }
        case DNX_EGR_PORT_TWO_PRIORITIES:
        {
            egq_priority_val = DNX_EGQ_PS_MODE_TWO_PRIORITY_VAL;
            break;
        }
        case DNX_EGR_PORT_FOUR_PRIORITIES:
        {
            egq_priority_val = DNX_EGQ_PS_MODE_FOUR_PRIORITY_VAL;
            break;
        }
        case DNX_EGR_PORT_EIGHT_PRIORITIES:
        {
            egq_priority_val = DNX_EGQ_PS_MODE_EIGHT_PRIORITY_VAL;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid priority mode %d", priority_mode);
        }
    }
    {
        dbal_fields_e dbal_field_id;
        dbal_tables_e dbal_table_id;
        uint32 entry_handle_id;
        uint32 dbal_field_val;
        /*
         * Allocate buffer.
         */
        dbal_table_id = DBAL_TABLE_EGQ_SHAPING_PS_MODE;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        /*
         * key construction.
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core_id);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_SCHEDULER, (uint32) (ps));
        /*
         * Set the variable: DBAL_FIELD_PS_MODE
         */
        dbal_field_id = DBAL_FIELD_PS_MODE;
        dbal_field_val = egq_priority_val;
        dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, (uint32) (dbal_field_val));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_egr_queuing_is_high_priority_queue_get(
    int unit,
    int core,
    uint32 tm_port,
    uint32 cosq,
    int *is_high_priority)
{
    dnx_egr_ofp_sch_info_t info;

    SHR_FUNC_INIT_VARS(unit);
    dnx_egr_ofp_sch_info_clear(&info);
    /*
     * retrieved from HW
     */
    SHR_IF_ERR_EXIT(dnx_egr_ofp_scheduling_get(unit, core, tm_port, cosq, &info));
    if (info.nif_priority == DNX_EGR_OFP_INTERFACE_PRIO_HIGH)
    {
        *is_high_priority = TRUE;
    }
    else
    {
        *is_high_priority = FALSE;
    }
exit:
    SHR_FUNC_EXIT;
}
static int
dnx_egr_queuing_ofp_tcg_set_verify(
    int unit,
    int core,
    uint32 tm_port,
    dnx_egr_q_prio_e priority,
    dnx_egr_queuing_tcg_info_t * tcg_info)
{
    uint32 ps, priority_offset;
    uint32 member_priority, priority_i, tcg_i;
    int base_q_pair;
    int nof_priorities;
    uint8 is_one_member;
    dnx_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core, tm_port, &logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, logical_port, &nof_priorities));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    if (nof_priorities < dnx_data_egr_queuing.params.tcg_min_priorities_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "nof_priorities (%d) is smaller than minimal allowed (%d)",
                     nof_priorities, dnx_data_egr_queuing.params.tcg_min_priorities_get(unit));
    }
    ps = base_q_pair / DNX_NOF_TCGS;
    priority_offset = base_q_pair - (ps * DNX_NOF_TCGS);
    if ((priority_offset + nof_priorities) > dnx_data_egr_queuing.params.nof_egr_q_prio_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "nof_priorities (%d) plus priority_offset (%d) can not be larger than nof_egr_q_prio (%d)",
                     nof_priorities, priority_offset, dnx_data_egr_queuing.params.nof_egr_q_prio_get(unit));
    }
    for (priority_i = 0; priority_i < nof_priorities; ++priority_i)
    {
        /*
         * DNX_TCG_MIN may be changed to be bigger than 0.
         */
        if (((int) (tcg_info->tcg_ndx[priority_i]) < DNX_TCG_MIN) || ((tcg_info->tcg_ndx[priority_i]) > DNX_TCG_MAX))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "Priority %d: Value of corresponding tcg (%d) is out of range (%d,%d).",
                         priority_i, tcg_info->tcg_ndx[priority_i], DNX_TCG_MIN, DNX_TCG_MAX);
        }
    }
    /*
     * If last four tcgs are only a single member TCG. In that case, verify first that
     * the required tcg_ndx is not mapped by other q-pair 
     */
    /*
     * Verify each single member TCG has only one priority that is mapped to it
     */
    if (nof_priorities == DNX_NOF_TCGS_IN_PS)
    {
        for (tcg_i = DNX_EGR_SINGLE_MEMBER_TCG_START; tcg_i <= DNX_EGR_SINGLE_MEMBER_TCG_END; tcg_i++)
        {
            is_one_member = FALSE;
            member_priority = -1;
            for (priority_i = 0; priority_i < nof_priorities; ++priority_i)
            {
                if (tcg_info->tcg_ndx[priority_i] == tcg_i)
                {
                    if (is_one_member)
                    {
                        /*
                         * More than one member set to this tcg
                         */
                        SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                                     "TCG (%d) has more than one priority mapped to it (%d plus %d)",
                                     tcg_i, member_priority, priority_i);
                    }
                    else
                    {
                        is_one_member = TRUE;
                        member_priority = priority_i;
                    }
                }
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*********************************************************************
*     Function description
*       Associate the queue-pair (Port,Priority) to traffic class
*     groups (TCG) attributes.
*     Details: in the H file egr_queuing.h
*********************************************************************/
int
dnx_egr_queuing_ofp_tcg_single_set(
    int unit,
    int core,
    uint32 tm_port,
    dnx_egr_q_prio_e priority,
    dnx_tcg_ndx_t tcg_to_map)
{
    uint32 ps, priority_i, priority_offset, allocation_bmp;
    int base_q_pair, nof_priorities;
    dnx_port_t logical_port;
    dbal_tables_e dbal_table_id;
    uint32 entry_handle_id;
    dbal_fields_e dbal_field_id;
    dnx_egr_queuing_tcg_info_t tcg_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_egr_queuing_ofp_tcg_get(unit, core, tm_port, &tcg_info));
    if (priority >= dnx_data_egr_queuing.params.nof_egr_q_prio_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "priority (%d) is larger than allowed (%d).",
                     priority, dnx_data_egr_queuing.params.nof_egr_q_prio_get(unit) - 1);
    }
    tcg_info.tcg_ndx[priority] = tcg_to_map;
    SHR_IF_ERR_EXIT(dnx_egr_queuing_ofp_tcg_set_verify(unit, core, tm_port, priority, &tcg_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core, tm_port, &logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, logical_port, &nof_priorities));
    if (nof_priorities > dnx_data_egr_queuing.params.nof_egr_q_prio_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Nof_priorities (%d) is larger than the maximum allowed (%d).",
                     nof_priorities, dnx_data_egr_queuing.params.nof_egr_q_prio_get(unit));
    }
    if (DNX_EGR_NOF_Q_PRIO != dnx_data_egr_queuing.params.nof_egr_q_prio_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "DNX_EGR_NOF_Q_PRIO (%d) is is not equal to its setup in DATA (%d).",
                     DNX_EGR_NOF_Q_PRIO, dnx_data_egr_queuing.params.nof_egr_q_prio_get(unit));
    }
    ps = base_q_pair / DNX_NOF_TCGS;
    priority_offset = base_q_pair - (ps * DNX_NOF_TCGS);
    /*
     * Set tcg indices
     */
    dbal_table_id = DBAL_TABLE_EGQ_SHAPING_QPAIR_TCG_MAP;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    /*
     * key construction.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_SCHEDULER, ps);
    SHR_IF_ERR_EXIT(dnx_egr_db.ps.allocation_bmap.get(unit, core, ps, &allocation_bmp));
    for (priority_i = 0; priority_i < DNX_NOF_TCGS_IN_PS; ++priority_i)
    {
        /*
         * Map the TCs which belong to port with given TCG mapping
         */
        if ((priority_i >= priority_offset) && (priority_i < (priority_offset + nof_priorities)))
        {
            /*
             * Only indicated 'priority' is updated in HW. All others, within this priority group'
             * are assumed to be consistent with it (See dnx_egr_queuing_ofp_tcg_set_verify()).
             */
            if (priority == (priority_i - priority_offset))
            {
                dbal_field_id = DBAL_FIELD_PRIO_MAP_ELEMENT;
                dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, priority_i, tcg_to_map);
            }
        }
        else if (allocation_bmp != 0xFF)
        {
            /*
             * If the TCs which do not belong to port are not mapped, they will need to be mapped
             * to default (which out of the range of the port TCG)
             */
            dbal_field_id = DBAL_FIELD_PRIO_MAP_ELEMENT;
            dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, priority_i, priority_i);
        }
    }
    /*
     * Set the variable: DBAL_FIELD_PRIO_MAP_ELEMENT: Mapping for all updated TCGs
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/*
 * For details, see header file egr_queuing.h
 */
int
dnx_egr_queuing_ofp_tcg_single_get(
    int unit,
    int core,
    uint32 tm_port,
    uint32 priority,
    dnx_tcg_ndx_t * tcg_to_map_p)
{
    uint32 priority_offset, ps;
    int base_q_pair;
    soc_port_t logical_port;
    dbal_tables_e dbal_table_id;
    uint32 entry_handle_id;
    dbal_fields_e dbal_field_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (priority >= dnx_data_egr_queuing.params.nof_egr_q_prio_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "priority (%d) is larger than allowed (%d).",
                     priority, dnx_data_egr_queuing.params.nof_egr_q_prio_get(unit) - 1);
    }
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core, tm_port, &logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    ps = base_q_pair / DNX_NOF_TCGS;
    priority_offset = base_q_pair - (ps * DNX_NOF_TCGS);
    if ((priority + priority_offset) >= dnx_data_egr_queuing.params.nof_egr_q_prio_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "priority (%d) is larger than allowed (%d).",
                     priority, dnx_data_egr_queuing.params.nof_egr_q_prio_get(unit) - 1);
    }
    dbal_table_id = DBAL_TABLE_EGQ_SHAPING_QPAIR_TCG_MAP;
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    /*
     * key construction.
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_SCHEDULER, ps);
    /*
     * Get the value of DBAL_FIELD_PRIO_MAP_ELEMENT[priority]
     */
    dbal_field_id = DBAL_FIELD_PRIO_MAP_ELEMENT;
    dbal_value_field32_request(unit, entry_handle_id, dbal_field_id, (priority + priority_offset),
                               (uint32 *) (tcg_to_map_p));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
dnx_egr_queuing_ofp_tcg_get(
    int unit,
    int core,
    uint32 tm_port,
    dnx_egr_queuing_tcg_info_t * tcg_info)
{
    uint32 priority_i;
    int base_q_pair, nof_priorities;
    soc_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(tcg_info, 0, sizeof(*tcg_info));
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core, tm_port, &logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, logical_port, &nof_priorities));
    if (nof_priorities < dnx_data_egr_queuing.params.tcg_min_priorities_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Nof_priorities (%d) is smaller than the minimum allowed (%d).",
                     nof_priorities, dnx_data_egr_queuing.params.tcg_min_priorities_get(unit));
    }
    if (nof_priorities > dnx_data_egr_queuing.params.nof_egr_q_prio_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Nof_priorities (%d) is larger than the maximum allowed (%d).",
                     nof_priorities, dnx_data_egr_queuing.params.nof_egr_q_prio_get(unit));
    }
    if (DNX_EGR_NOF_Q_PRIO != dnx_data_egr_queuing.params.nof_egr_q_prio_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "DNX_EGR_NOF_Q_PRIO (%d) is not equal to its setup in DATA (%d).",
                     DNX_EGR_NOF_Q_PRIO, dnx_data_egr_queuing.params.nof_egr_q_prio_get(unit));
    }
    for (priority_i = 0; priority_i < nof_priorities; ++priority_i)
    {
        SHR_IF_ERR_EXIT(dnx_egr_queuing_ofp_tcg_single_get
                        (unit, core, tm_port, priority_i, &(tcg_info->tcg_ndx[priority_i])));
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
void
dnx_egr_tcg_sch_wfq_clear(
    dnx_egr_tcg_sch_wfq_t * info)
{
    SHR_FUNC_INIT_VARS(NO_UNIT);
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");
    sal_memset(info, 0x0, sizeof(*info));
    info->tcg_weight_valid = FALSE;
exit:
    SHR_VOID_FUNC_EXIT;
}
static int
dnx_egr_queuing_tcg_weight_set_verify(
    int unit,
    int core,
    uint32 tm_port,
    dnx_tcg_ndx_t tcg_ndx,
    dnx_egr_tcg_sch_wfq_t * tcg_weight)
{
    int base_q_pair, nof_priorities;
    dnx_port_t logical_port;

    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core, tm_port, &logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    if (base_q_pair == DNX_EGQ_PORT_ID_INVALID)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                     "Base QPAIR for this logical port (%d) is marked as invalid (%d)", logical_port, base_q_pair);
    }
    /*
     * API functionality only when port is with 8 priorities.
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_priorities_nof_get(unit, logical_port, &nof_priorities));
    if (nof_priorities < dnx_data_egr_queuing.params.tcg_min_priorities_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                     "nof_priorities (%d) is smaller than minimum allowed (%d)",
                     nof_priorities, dnx_data_egr_queuing.params.tcg_min_priorities_get(unit));
    }

    /*
     * Verify TCG
     */
    if (((int) (tcg_ndx) < (DNX_TCG_MIN)) || ((tcg_ndx) > (DNX_TCG_MAX)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "tcg_ndx (%d) is out of range (%d,%d).", tcg_ndx, DNX_TCG_MIN, DNX_TCG_MAX);
    }
    /*
     * Verify TCG weight
     */
    if (tcg_weight->tcg_weight_valid)
    {
        if (((int) (tcg_weight->tcg_weight) < (DNX_EGQ_TCG_WEIGHT_MIN))
            || ((tcg_weight->tcg_weight) > (DNX_EGQ_TCG_WEIGHT_MAX)))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "tcg_weight (%d) is out of range (%d,%d).",
                         tcg_weight->tcg_weight, DNX_EGQ_TCG_WEIGHT_MIN, DNX_EGQ_TCG_WEIGHT_MAX);
        }
    }
exit:
    SHR_FUNC_EXIT;
}

int
dnx_egr_queuing_tcg_weight_set(
    int unit,
    int core,
    uint32 tm_port,
    dnx_tcg_ndx_t tcg_ndx,
    dnx_egr_tcg_sch_wfq_t * tcg_weight)
{
    int base_q_pair;
    uint32 field_val;
    uint32 ps;
    dnx_port_t logical_port;
    dbal_tables_e dbal_table_id;
    uint32 entry_handle_id;
    dbal_fields_e dbal_field_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_egr_queuing_tcg_weight_set_verify(unit, core, tm_port, tcg_ndx, tcg_weight));
    ps = 0;
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core, tm_port, &logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    /*
     * Set TCG weight valid
     */
    {
        /*
         * Allocat buffer for DBAL.
         */
        dbal_table_id = DBAL_TABLE_EGQ_TCG_ATTRIBUTES;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        /*
         * key construction. Constant part
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TCG_INDEX, (uint32) (base_q_pair + tcg_ndx));
        field_val = tcg_weight->tcg_weight_valid ? 0 : 1;
        dbal_field_id = DBAL_FIELD_WFQ_TCG_DIS;
        dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, INST_SINGLE, (uint32) (field_val));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }
    if (tcg_weight->tcg_weight_valid)
    {
        /*
         * Set TCG weight only in case of valid
         */
        ps = base_q_pair / DNX_EGR_NOF_Q_PAIRS_IN_PS;
        /*
         * Reuse allocated buffer.
         */
        dbal_table_id = DBAL_TABLE_EGQ_SHAPING_TCG_WEIGHTS;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_table_id, entry_handle_id));
        /*
         * key construction. Constant part
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_SCHEDULER, (uint32) (ps));
        field_val = tcg_weight->tcg_weight;
        /*
         * Update only the weight that corresponds to indicated TCG index (0 - 7).
         */
        dbal_field_id = DBAL_FIELD_WEIGHT_OF_TCG;
        dbal_entry_value_field32_set(unit, entry_handle_id, dbal_field_id, tcg_ndx, (uint32) (field_val));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        /*
         * Set TCG weight
         */
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
dnx_egr_queuing_tcg_weight_get(
    int unit,
    int core,
    uint32 tm_port,
    dnx_tcg_ndx_t tcg_ndx,
    dnx_egr_tcg_sch_wfq_t * tcg_weight)
{
    uint32 field_val;
    uint32 ps;
    int base_q_pair;
    dnx_port_t logical_port;
    dbal_tables_e dbal_table_id;
    uint32 entry_handle_id;
    dbal_fields_e dbal_field_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_egr_queuing_tcg_weight_set_verify(unit, core, tm_port, tcg_ndx, tcg_weight));
    ps = 0;
    SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core, tm_port, &logical_port));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_q_pair_get(unit, logical_port, &base_q_pair));
    /*
     * Get TCG weight valid
     */
    {
        /*
         * Allocat buffer for DBAL.
         */
        dbal_table_id = DBAL_TABLE_EGQ_TCG_ATTRIBUTES;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
        /*
         * key construction. Constant part
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TCG_INDEX, (uint32) (base_q_pair + tcg_ndx));
        /*
         * Get the value: DBAL_FIELD_WFQ_TCG_DIS
         */
        dbal_field_id = DBAL_FIELD_WFQ_TCG_DIS;
        field_val = 0;
        dbal_value_field32_request(unit, entry_handle_id, dbal_field_id, INST_SINGLE, (uint32 *) (&field_val));
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    }
    tcg_weight->tcg_weight_valid = field_val ? 0 : 1;
    if (tcg_weight->tcg_weight_valid)
    {
        /*
         * Get TCG weight only in case of valid
         */
        ps = base_q_pair / DNX_EGR_NOF_Q_PAIRS_IN_PS;
        /*
         * Reuse allocated buffer.
         */
        dbal_table_id = DBAL_TABLE_EGQ_SHAPING_TCG_WEIGHTS;
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_table_id, entry_handle_id));
        /*
         * key construction. Constant part
         */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_SCHEDULER, (uint32) (ps));
        field_val = tcg_weight->tcg_weight;
        /*
         * Get the weight that corresponds to indicated TCG index (0 - 7).
         */
        dbal_field_id = DBAL_FIELD_WEIGHT_OF_TCG;
        field_val = 0;
        dbal_value_field32_request(unit, entry_handle_id, dbal_field_id, tcg_ndx, (uint32 *) (&field_val));
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
        tcg_weight->tcg_weight = field_val;
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/* } */
#endif

/* } */
