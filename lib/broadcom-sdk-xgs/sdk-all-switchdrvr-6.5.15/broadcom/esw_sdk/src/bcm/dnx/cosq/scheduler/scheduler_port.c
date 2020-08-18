
/** \file scheduler_port.c
 * $Id$
 *
 * e2e scheduler functionality for DNX
 * 
 */

/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

#include <shared/shrextend/shrextend_error.h>
#include <bcm_int/dnx/cosq/ingress/iqs.h>
#include <shared/utilex/utilex_u64.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_sch_config_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_scheduler_access.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/algo/rate/algo_rates.h>
#include <bcm_int/dnx/cosq/egress/egq_ofp_rates.h>
#include <bcm_int/dnx/algo/cal/algo_cal.h>

#include "scheduler_dbal.h"
#include "scheduler_element.h"
#include "scheduler_port.h"
#include "scheduler_allocators.h"

/**
 * \brief - Get number of calendar ticks in one second
 */
static shr_error_e
dnx_scheduler_port_calendar_ticks_per_sec_get(
    int unit,
    dnx_sch_port_calendar_e port_calendar_type,
    uint32 *ticks_per_sec)
{
    uint32 access_period, cal_length, device_ticks_per_sec;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_sch_port_shaper_calendar_params_get(unit, DBAL_CORE_DEFAULT, port_calendar_type,
                                                            &access_period, &cal_length));

    device_ticks_per_sec = DNXCMN_CORE_CLOCK_HZ_GET(unit);
    *ticks_per_sec = UTILEX_DIV_ROUND_UP(device_ticks_per_sec, (cal_length * access_period));

exit:
    SHR_FUNC_EXIT;
}

/**
 * brief -
 * Convert a rate given in Kbits/sec units to quanta
 * The conversion is done according to:
 *                       
 *  Rate [Kbits/Sec] =   Credit [Kbits] * Num_of_clocks_in_sec [clocks/sec] * quanta [1/clocks] 
 *  
 *                                          1
 *  Where quanta = -------------------------------------------------------------
 *                        interval_between_credits_in_clocks [clocks]
 */
static shr_error_e
dnx_scheduler_port_rate_kbits_per_sec_to_quanta_for_divider(
    int unit,
    dnx_sch_port_calendar_e port_calendar_type,
    uint32 rate,                /* in Kbits/sec */
    uint32 credit_div,          /* REBOUNDED/ASSIGNED_CREDIT */
    uint32 *quanta              /* in device clocks */
    )
{
    uint32 calc2, calc;
    uint32 credit_worth;

    uint32 ticks_per_sec;

    SHR_FUNC_INIT_VARS(unit);

    if (0 == rate)
    {
        *quanta = 0;
    }
    else
    {
        /** get credit worth*/
        SHR_IF_ERR_EXIT(dnx_iqs_credit_worth_group_get(unit, 0, &credit_worth));

        /** get ticks per second */
        SHR_IF_ERR_EXIT(dnx_scheduler_port_calendar_ticks_per_sec_get(unit, port_calendar_type, &ticks_per_sec));

        /*
         * calc = credit_worth [kbits] * ticks_per_sec * 8 / (1000 [clocks/sec] * ASSIGNED_CREDIT)
         * full calc = credit_worth [kbits] * 8 * system_frequency(khz)  / (1000 * ASSIGNED_CREDIT*cal_len*acc_period)
         */
        calc = UTILEX_DIV_ROUND(ticks_per_sec * UTILEX_NOF_BITS_IN_CHAR, 1000);
        calc = UTILEX_DIV_ROUND(credit_worth * calc, credit_div);

        /** calc2 = rate / calc [1/clocks] */

        calc2 = (calc == 0 ? 0 : UTILEX_DIV_ROUND_UP(rate, calc));

        *quanta = (calc2 == 0 ? 1 : calc2);

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * brief -
 * Convert a rate given in Kbits/sec units to quanta
 * The conversion is done according to:
 *                       
 *  Rate [Kbits/Sec] =   Credit [Kbits] * Num_of_clocks_in_sec [clocks/sec] * quanta [1/clocks] 
 *  
 *                                          1
 *  Where quanta = -------------------------------------------------------------
 *                        interval_between_credits_in_clocks [clocks]
 */
shr_error_e
dnx_scheduler_port_rate_kbits_per_sec_to_quanta(
    int unit,
    dnx_sch_port_calendar_e port_calendar_type,
    uint32 rate,                /* in Kbits/sec */
    uint32 *quanta              /* in device clocks */
    )
{
    uint32 divider;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_sch_port_shaper_credit_divider_get(unit, &divider));
    SHR_IF_ERR_EXIT(dnx_scheduler_port_rate_kbits_per_sec_to_quanta_for_divider(unit, port_calendar_type, rate, divider,
                                                                                quanta));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *   Convert quanta given in
 *   device clocks to rate in Kbits/sec units.
 *   The conversion is done according to:
 *  Rate [Kbits/Sec] =   Credit [Kbits] * Num_of_clocks_in_sec [clocks/sec] * quanta 
 *  
 *                                          1
 *  Where quanta = -------------------------------------------------------------
 *                        interval_between_credits_in_clocks [clocks]
 */
shr_error_e
dnx_scheduler_port_quanta_to_rate_kbits_per_sec(
    int unit,
    dnx_sch_port_calendar_e port_calendar_type,
    uint32 quanta,              /* in device clocks */
    uint32 *rate                /* in Kbits/sec */
    )
{
    UTILEX_U64 calc2;
    uint32 calc, tmp;
    uint32 credit_worth;
    uint32 ticks_per_sec;
    uint32 credit_div;          /* REBOUNDED/ASSIGNED_CREDIT */

    SHR_FUNC_INIT_VARS(unit);

    if (0 == quanta)
    {
        *rate = 0;
    }
    else
    {
        /** get credit worth*/
        SHR_IF_ERR_EXIT(dnx_iqs_credit_worth_group_get(unit, 0, &credit_worth));

        SHR_IF_ERR_EXIT(dnx_scheduler_port_calendar_ticks_per_sec_get(unit, port_calendar_type, &ticks_per_sec));

        SHR_IF_ERR_EXIT(dnx_sch_port_shaper_credit_divider_get(unit, &credit_div));

        /** calc = credit_worth [kbits] * ticks_per_sec * 8 / (1000 [clocks/sec] * ASSIGNED_CREDIT) */
        /** full calc = credit_worth [kbits] * 8 * system_frequency(khz)  / (1000 * ASSIGNED_CREDIT*cal_len*acc_period) */
        calc = UTILEX_DIV_ROUND(ticks_per_sec * UTILEX_NOF_BITS_IN_CHAR, 1000);
        calc = UTILEX_DIV_ROUND(credit_worth * calc, credit_div);

        utilex_u64_multiply_longs(calc, quanta, &calc2);
        if (utilex_u64_to_long(&calc2, &tmp))
        {
            /** Overflow */
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Overflow when calculating rate\n");
        }

        *rate = tmp;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Calculate maximal possible divider (assigned credit worth)
 * which can provide desired max rate
 */
static shr_error_e
dnx_scheduler_port_shaper_calculate_divider(
    int unit,
    uint32 *divider)
{
    uint32 new_quanta, curr_divider, iterate_divider;

    uint32 max_rate;
    uint32 max_divider;
    uint32 max_quanta_to_add;

    SHR_FUNC_INIT_VARS(unit);

    max_rate = dnx_data_sch.ps.max_port_rate_mbps_get(unit) * 1000; /** Mbit/sec -> Kbit/sec */

    /** Get maximal value of credit worth field */
    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit,
                                                          DBAL_TABLE_SCH_PORT_CREDIT_CONFIGURATION,
                                                          DBAL_FIELD_CREDIT_WORTH, 0, 0, 0, DBAL_PREDEF_VAL_MAX_VALUE,
                                                          &max_divider));

    /** Get maximal value of quanta to add field */
    SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit, DBAL_TABLE_SCH_PORT_TC_SHAPER,
                                                          DBAL_FIELD_QUANTA_TO_ADD, 0, 0, 0, DBAL_PREDEF_VAL_MAX_VALUE,
                                                          &max_quanta_to_add));

    curr_divider = 1;

   /** increase divider to maximum possible, keeping the max rate quanta below 2047 */
    for (iterate_divider = 1; iterate_divider < max_divider; iterate_divider++)
    {
        SHR_IF_ERR_EXIT(dnx_scheduler_port_rate_kbits_per_sec_to_quanta_for_divider(unit, DNX_SCH_PORT_TC_CALENDAR,
                                                                                    max_rate, iterate_divider,
                                                                                    &new_quanta));
        if (new_quanta > max_quanta_to_add)
        {
            break;
        }
        curr_divider = iterate_divider;
    }

    *divider = curr_divider;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Init assigned credit worth
 */
static shr_error_e
dnx_scheduler_port_credit_divider_init(
    int unit)
{
    uint32 new_divider = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_scheduler_port_shaper_calculate_divider(unit, &new_divider));

    SHR_IF_ERR_EXIT(dnx_sch_port_shaper_credit_divider_set(unit, new_divider));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Init Scheduler port TC/TCG calendar configurations
 */
shr_error_e
dnx_scheduler_port_shaper_calendar_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** init CSC/PCS calendar configuration */
    SHR_IF_ERR_EXIT(dnx_sch_port_shaper_config_init(unit));

    /** init assigned credit worth */
    SHR_IF_ERR_EXIT(dnx_scheduler_port_credit_divider_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  set maximal credit rate of Port TC
 */
shr_error_e
dnx_scheduler_port_priority_shaper_rate_set(
    int unit,
    bcm_port_t logical_port,
    uint32 priority_ndx,
    uint32 rate)
{
    uint32 quanta;
    int core;
    int base_hr;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));

    /** calculate quanta */
    SHR_IF_ERR_EXIT(dnx_scheduler_port_rate_kbits_per_sec_to_quanta(unit, DNX_SCH_PORT_TC_CALENDAR, rate, &quanta));

    SHR_IF_ERR_EXIT(dnx_sch_port_tc_bandwidth_set(unit, core, base_hr + priority_ndx, quanta));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  get maximal credit rate of port TC
 */
shr_error_e
dnx_scheduler_port_priority_shaper_rate_get(
    int unit,
    bcm_port_t logical_port,
    int priority_ndx,
    uint32 *rate)
{

    uint32 quanta;
    int core;
    int base_hr;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));

    SHR_IF_ERR_EXIT(dnx_sch_port_tc_bandwidth_get(unit, core, base_hr + priority_ndx, &quanta));

    /** calculate rate from quanta */
    SHR_IF_ERR_EXIT(dnx_scheduler_port_quanta_to_rate_kbits_per_sec(unit, DNX_SCH_PORT_TC_CALENDAR, quanta, rate));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  set maximal credit rate of TCG
 */
shr_error_e
dnx_scheduler_port_tcg_shaper_rate_set(
    int unit,
    bcm_port_t logical_port,
    int tcg_ndx,
    uint32 rate)
{

    uint32 quanta;
    int core;
    int base_hr, global_tcg;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));

    /** calculate quanta */
    SHR_IF_ERR_EXIT(dnx_scheduler_port_rate_kbits_per_sec_to_quanta(unit, DNX_SCH_PORT_TCG_CALENDAR, rate, &quanta));

    global_tcg = base_hr + tcg_ndx;

    SHR_IF_ERR_EXIT(dnx_sch_port_tcg_bandwidth_set(unit, core, global_tcg, quanta));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  get maximal credit rate of TCG
 */
shr_error_e
dnx_scheduler_port_tcg_shaper_rate_get(
    int unit,
    bcm_port_t logical_port,
    int tcg_ndx,
    uint32 *rate)
{

    uint32 quanta;
    int core;
    int base_hr, global_tcg;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));

    global_tcg = base_hr + tcg_ndx;

    SHR_IF_ERR_EXIT(dnx_sch_port_tcg_bandwidth_get(unit, core, global_tcg, &quanta));

    /** calculate rate from quanta */
    SHR_IF_ERR_EXIT(dnx_scheduler_port_quanta_to_rate_kbits_per_sec(unit, DNX_SCH_PORT_TCG_CALENDAR, quanta, rate));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  set max burst of port TC
 */
shr_error_e
dnx_scheduler_port_priority_max_burst_set(
    int unit,
    bcm_port_t logical_port,
    uint32 port_priority,
    uint32 max_burst)
{
    int core, base_hr;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));

    SHR_IF_ERR_EXIT(dnx_sch_port_tc_max_burst_set(unit, core, base_hr + port_priority, max_burst));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  get max burst of port TC
 */
shr_error_e
dnx_scheduler_port_priority_max_burst_get(
    int unit,
    bcm_port_t logical_port,
    uint32 port_priority,
    uint32 *max_burst)
{
    int core, base_hr;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));

    SHR_IF_ERR_EXIT(dnx_sch_port_tc_max_burst_get(unit, core, base_hr + port_priority, max_burst));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  set max burst of port TCG
 */
shr_error_e
dnx_scheduler_port_tcg_max_burst_set(
    int unit,
    bcm_port_t logical_port,
    uint32 tcg_index,
    uint32 max_burst)
{
    int core, base_hr;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));

    SHR_IF_ERR_EXIT(dnx_sch_port_tcg_max_burst_set(unit, core, base_hr + tcg_index, max_burst));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  get max burst of port TCG
 */
shr_error_e
dnx_scheduler_port_tcg_max_burst_get(
    int unit,
    bcm_port_t logical_port,
    uint32 tcg_index,
    uint32 *max_burst)
{
    int core, base_hr;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));

    SHR_IF_ERR_EXIT(dnx_sch_port_tcg_max_burst_get(unit, core, base_hr + tcg_index, max_burst));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  enable/disable port scheduler
 */
shr_error_e
dnx_scheduler_hr_port_enable_set(
    int unit,
    int core,
    int base_hr,
    int nof_priorities,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!enable)
    {
        /*
         * Disabling port.
         * Force flow control on HR before disabling HR.
         * This is required  for Dynamic port during traffic
         * that leads to scheduler being stuck due to toggling of SCH_PORT_ENABLE_PORTEN.
         */
        SHR_IF_ERR_EXIT(dnx_sch_port_hr_force_fc_range_set(unit, core, base_hr, nof_priorities, TRUE));
    }

    SHR_IF_ERR_EXIT(dnx_sch_port_hr_enable_range_set(unit, core, base_hr, nof_priorities, enable));

    if (enable)
    {
        /*
         * Enabling port.
         * Disable force flow control on HR after enabling HR. 
         * This is required  for Dynamic port during traffic
         * that leads to scheduler being stuck due to toggling of SCH_PORT_ENABLE_PORTEN.
         */
        SHR_IF_ERR_EXIT(dnx_sch_port_hr_force_fc_range_set(unit, core, base_hr, nof_priorities, FALSE));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  enable/disable port scheduler
 */
shr_error_e
dnx_scheduler_port_enable_set(
    int unit,
    bcm_port_t logical_port,
    int enable)
{
    int core, base_hr, nof_priorities;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));
    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priorities_nof_get(unit, logical_port, &nof_priorities));

    SHR_IF_ERR_EXIT(dnx_scheduler_hr_port_enable_set(unit, core, base_hr, nof_priorities, enable));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  get group (color) which currently has the lowest total bandwidth
 */
static int
dnx_scheduler_port_hr_min_bw_group_get(
    int unit,
    int core,
    int *group)
{

    int i, res;
    int min_bw = 0, group_bw = 0;

    SHR_FUNC_INIT_VARS(unit);

    *group = 0;
    res = cosq_config.groups_bw.get(unit, core, *group, &group_bw);
    SHR_IF_ERR_EXIT(res);
    min_bw = group_bw;

    for (i = 0; i < dnx_data_sch.se.nof_color_group_get(unit); i++)
    {
        res = cosq_config.groups_bw.get(unit, core, i, &group_bw);
        SHR_IF_ERR_EXIT(res);
        if (group_bw < min_bw)
        {
            min_bw = group_bw;
            *group = i;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 *  calculate group (color) with the lowest total bandwidth to 
 *  color the provide port
 * \note - this function updates swstate which is used for color calculation
 */
static int
dnx_scheduler_port_hr_group_calc(
    int unit,
    bcm_port_t port,
    int *group)
{
    uint32 res, se_id;
    int group_rate, hr_rate;
    int base_hr, interface_max_rate = 0;
    bcm_core_t core;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, port, &base_hr));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));

    SHR_IF_ERR_EXIT(dnx_algo_port_interface_rate_get(unit, port, 0, &interface_max_rate));

    /** get priority 0 HR se_id (all port HRs are associated to the same group) */
    se_id = DNX_SCHEDULER_HR_SE_ID_GET(unit, base_hr);

    res = cosq_config.hr_group_bw.get(unit, core, base_hr, &hr_rate);
    SHR_IF_ERR_EXIT(res);

    /** if group was already set, try to switch the group */
    if (hr_rate)
    {
        SHR_IF_ERR_EXIT(dnx_sch_se_color_group_get(unit, core, se_id, group));

        SHR_IF_ERR_EXIT(cosq_config.groups_bw.get(unit, core, *group, &group_rate));

        if ((group_rate - hr_rate) > 0)
        {
            group_rate = group_rate - hr_rate;
        }
        else
        {
            group_rate = 0;
        }
        SHR_IF_ERR_EXIT(cosq_config.groups_bw.set(unit, core, *group, group_rate));
    }

    /** update new group and hr rate */
    SHR_IF_ERR_EXIT(dnx_scheduler_port_hr_min_bw_group_get(unit, core, group));
    SHR_IF_ERR_EXIT(cosq_config.groups_bw.get(unit, core, *group, &group_rate));
    SHR_IF_ERR_EXIT(cosq_config.groups_bw.set(unit, core, *group, (group_rate + interface_max_rate)));
    SHR_IF_ERR_EXIT(cosq_config.hr_group_bw.set(unit, core, base_hr, interface_max_rate));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Color all HRs of the provided port with given color group
 */
int
dnx_scheduler_port_hr_color_group_set(
    int unit,
    bcm_port_t logical_port,
    int priority,
    int group)
{
    int se_id, base_hr;
    bcm_core_t core;

    SHR_FUNC_INIT_VARS(unit);

    /**  Assign scheduler group */
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));

    /** get se id from base HR and priority */
    se_id = DNX_SCHEDULER_HR_SE_ID_GET(unit, base_hr + priority);

    /** set new group for se id */
    SHR_IF_ERR_EXIT(dnx_sch_se_color_group_set(unit, core, se_id, group));

    /** mark that the HR is colored */
    SHR_IF_ERR_EXIT(dnx_scheduler_db.hr.is_colored.set(unit, core, base_hr + priority, TRUE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Get HR color group
 */
int
dnx_scheduler_port_hr_color_group_get(
    int unit,
    bcm_gport_t logical_port,
    int priority,
    int *group)
{
    int se_id;
    bcm_core_t core;
    int base_hr;

    SHR_FUNC_INIT_VARS(unit);

    /**  Assign scheduler group */
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));

    /** get se id from base HR and priority */
    se_id = DNX_SCHEDULER_HR_SE_ID_GET(unit, base_hr + priority);

    /** get group from se id*/
    SHR_IF_ERR_EXIT(dnx_sch_se_color_group_get(unit, core, se_id, group));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Color all HRs of the provided port 
 * with the color which currently has lowest bandwidth.
 * max interface speed is considered as port bandwidth
 */
int
dnx_scheduler_port_hr_color_decide_and_set(
    int unit,
    bcm_port_t logical_port)
{
    int group;
    int priority_i;
    int nof_priorities;

    SHR_FUNC_INIT_VARS(unit);

    /** all the HRs in a port will have the same group (limitation cause of dual shapers) */
    SHR_IF_ERR_EXIT(dnx_scheduler_port_hr_group_calc(unit, logical_port, &group));      /* get the new group */

    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priorities_nof_get(unit, logical_port, &nof_priorities));

    for (priority_i = 0; priority_i < nof_priorities; priority_i++)
    {
        /** color all HRs priorities*/
        SHR_IF_ERR_EXIT(dnx_scheduler_port_hr_color_group_set(unit, logical_port, priority_i, group));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Uncolor all HRs of the provided port 
 */
int
dnx_scheduler_port_hr_color_unset(
    int unit,
    bcm_port_t logical_port)
{
    int priority, nof_priorities;
    int base_hr;
    int core;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priorities_nof_get(unit, logical_port, &nof_priorities));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));

    for (priority = 0; priority < nof_priorities; priority++)
    {
        /** mark that the HR is not colored */
        SHR_IF_ERR_EXIT(dnx_scheduler_db.hr.is_colored.set(unit, core, base_hr + priority, FALSE));

        
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * configure number of port priorities for the provided port
 */
int
dnx_scheduler_port_priorities_nof_set(
    int unit,
    bcm_port_t logical_port)
{
    int base_hr;
    int core_id;
    int nof_priorities;
    dbal_enum_value_field_nof_ps_priorities_e ps_priority = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priorities_nof_get(unit, logical_port, &nof_priorities));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core_id));

    switch (nof_priorities)
    {
        case 1:
            ps_priority = DBAL_ENUM_FVAL_NOF_PS_PRIORITIES_ONE_PRIORITY;
            break;
        case 2:
            ps_priority = DBAL_ENUM_FVAL_NOF_PS_PRIORITIES_TWO_PRIORITIES;
            break;
        case 4:
            ps_priority = DBAL_ENUM_FVAL_NOF_PS_PRIORITIES_FOUR_PRIORITIES;
            break;
        case 8:
            ps_priority = DBAL_ENUM_FVAL_NOF_PS_PRIORITIES_EIGHT_PRIORITIES;
            break;

        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unexpected number of priorities %d\n", nof_priorities);
            break;
    }

    SHR_IF_ERR_EXIT(dnx_sch_port_priorities_nof_set(unit, core_id, base_hr, ps_priority));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * map all port HRs to TCG 0 as the default
 */
int
dnx_scheduler_port_tc_to_tcg_map_init(
    int unit,
    bcm_port_t logical_port)
{
    int nof_priorities;
    int tc;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priorities_nof_get(unit, logical_port, &nof_priorities));
    for (tc = 0; tc < nof_priorities; tc++)
    {
        /*
         * map all TCs to TCG 0
         * must for 1 and 2 priorities ports
         */
        SHR_IF_ERR_EXIT(dnx_sch_port_tc_to_tcg_map_set(unit, logical_port, tc, 0));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Set max burst of TC and TCG shapers of the port to maximum
 * Set TCG bandwidth of 1 or 2 priority port to maximum.
 * TC bandwidth for all ports and TCG bandwidth for 4 and 8 priorities ports are set in tune
 */
int
dnx_scheduler_port_shapers_init(
    int unit,
    bcm_port_t logical_port)
{
    int base_hr;
    int core;
    int nof_priorities;
    int tc, tcg;
    uint32 max_quanta_to_add;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priorities_nof_get(unit, logical_port, &nof_priorities));
    for (tc = 0; tc < nof_priorities; tc++)
    {
        /** Set Port-TC max burst to maximum */
        SHR_IF_ERR_EXIT(dnx_scheduler_port_priority_max_burst_set(unit, logical_port, tc,
                                                                  dnx_data_sch.ps.max_burst_get(unit)));
    }
    for (tcg = 0; tcg < nof_priorities; tcg++)
    {
        /** Set Port-TCG max burst to maximum */
        SHR_IF_ERR_EXIT(dnx_scheduler_port_tcg_max_burst_set(unit, logical_port, tcg,
                                                             dnx_data_sch.ps.max_burst_get(unit)));
    }

    if (nof_priorities < dnx_data_sch.ps.min_priority_for_tcg_get(unit))
    {

        /*
         * For 1 and 2 priorities port,
         * set TCG bandwidth to maximum.
         * This shaper should not affect the credit rate
         */

        /** Get maximal value of quanta_to_add field */
        SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit,
                                                              DBAL_TABLE_SCH_PORT_TCG_SHAPER,
                                                              DBAL_FIELD_QUANTA_TO_ADD, 0, 0, 0,
                                                              DBAL_PREDEF_VAL_MAX_VALUE, &max_quanta_to_add));

        SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
        SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));

        for (tcg = 0; tcg < nof_priorities; tcg++)
        {
            SHR_IF_ERR_EXIT(dnx_sch_port_tcg_bandwidth_set(unit, core, base_hr + tcg, max_quanta_to_add));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * allocate and configure sch interace for a port
 */
int
dnx_scheduler_interface_allocate(
    int unit,
    bcm_port_t port)
{
    int core, is_master, master_port;
    dnx_algo_port_type_e port_type;
    int prefer_big_calendar;
    int e2e_if, egq_if;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_is_master_get(unit, port, &is_master));

    if (is_master)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
        SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, port, &port_type));

        /** Try to allocate big calendars for ILKN interface */
        prefer_big_calendar = (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_type, 0) ? TRUE : FALSE);

        SHR_IF_ERR_EXIT(dnx_scheduler_allocators_interface_allocate(unit, core, prefer_big_calendar, &e2e_if));

        /** set e2e interface to port mgmt */
        SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_set(unit, port, e2e_if));

        if (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_type))
        {
            /** get egq interface from port mgmt */
            SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &egq_if));

            /** configure egress if to sch if mapping */
            SHR_IF_ERR_EXIT(dnx_sch_egq_if_to_sch_if_map_set(unit, core, egq_if, e2e_if));
        }

        SHR_IF_ERR_EXIT(dnx_sch_interface_enable_set(unit, core, e2e_if, TRUE));
    }
    else
    {
        /** get master port */
        SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, port, 0, &master_port));
        /** get sch interface of master port */
        SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_get(unit, master_port, &e2e_if));
        /** set e2e interface to port mgmt */
        SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_set(unit, port, e2e_if));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * deallocate and configure sch interace for a port
 */
int
dnx_scheduler_interface_deallocate(
    int unit,
    bcm_port_t port)
{
    int e2e_if, egq_if;
    bcm_port_t next_master_port;
    int core;
    dnx_algo_port_type_e port_type;

    SHR_FUNC_INIT_VARS(unit);

    /** deallocate  interface if this is the last port */
    SHR_IF_ERR_EXIT(dnx_algo_port_master_get(unit, port, DNX_ALGO_PORT_MASTER_F_NEXT, &next_master_port));

    if (next_master_port == DNX_ALGO_PORT_INVALID)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, &core));
        SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, port, &port_type));

        SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_get(unit, port, &e2e_if));

        if (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_type))
        {
            /** get egq interface from port mgmt */
            SHR_IF_ERR_EXIT(dnx_algo_port_egr_if_get(unit, port, &egq_if));

            /** set HW e2e interface to 0 */
            SHR_IF_ERR_EXIT(dnx_sch_egq_if_to_sch_if_map_set(unit, core, egq_if, 0));
        }

        /** mark e2e interface as free */
        SHR_IF_ERR_EXIT(dnx_scheduler_allocators_interface_deallocate(unit, core, e2e_if));

        SHR_IF_ERR_EXIT(dnx_sch_interface_enable_set(unit, core, e2e_if, FALSE));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - configure scheduler interface calendar traversal speed.
 * it is calculted from total calendar rate
 */
static int
dnx_scheduler_port_total_calendar_rate_set(
    int unit,
    int core,
    int sch_interface,
    uint32 rate)
{
    uint32 rate_internal, calendar_speed, calendar_speed_max_value;
    uint32 credit_worth;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * CH_NI_FXX_SUM_OF_PORTS defines the calendar traversal rate. 
     * Every ChNIFxxSumOfPorts/128 the next scheduler is visited and may be awarded a credit.
     * ChNIFxxSumOfPorts and the number of the slots it occupies determines the number of credits given to the port.
     * If ChNIFxxSumOfPorts = 0 then the ChNIFxx ports will not receive credits.
     * ChNIFxxSumOfPorts cannot be lower then 128. 
     * To calculate the rate do the following: RateGbps = (Credit_Size*8*128)/(ClkInNano*ChNIFxxSumOfPorts)
     */

    /*
     * To calculate ChNIFxxSumOfPorts, we use the following formula
     * RateGbps/128 = (Credit_Size*8)/(ClkInNano*ChNIFxxSumOfPorts)
     */
    rate_internal = UTILEX_DIV_ROUND_UP(rate, dnx_data_sch.interface.cal_speed_resolution_get(unit));

    if (0 == rate_internal)
    {
        calendar_speed = 0;
    }
    else
    {

        /** Get credit size */
        SHR_IF_ERR_EXIT(dnx_iqs_credit_worth_group_get(unit, 0, &credit_worth));

        SHR_IF_ERR_EXIT(dnx_algo_rates_kbits_per_sec_to_clocks(unit, rate_internal, credit_worth, &calendar_speed));

        /** Get maximal value of calendar speed field */
        SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit, DBAL_TABLE_SCH_IF_CALENDAR_CONFIGURATION,
                                                              DBAL_FIELD_CALENDAR_SPEED, 0, 0, 0,
                                                              DBAL_PREDEF_VAL_MAX_VALUE, &calendar_speed_max_value));

        calendar_speed = UTILEX_RANGE_BOUND(calendar_speed,
                                            dnx_data_sch.interface.cal_speed_resolution_get(unit),
                                            calendar_speed_max_value);
    }

    SHR_IF_ERR_EXIT(dnx_sch_interface_calendar_speed_set(unit, core, sch_interface, calendar_speed));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - return total inteface calendar rate.
 * it is calculted from scheduler interface calendar traversal speed. 
 */
static int
dnx_scheduler_port_total_calendar_rate_get(
    int unit,
    int core,
    uint32 sch_interface,
    uint32 *rate)
{
    uint32 rate_internal, calendar_speed, credit_worth;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_sch_interface_calendar_speed_get(unit, core, sch_interface, &calendar_speed));

    if (calendar_speed == 0)
    {
        *rate = 0;
    }
    else
    {
        /** Get credit size */
        SHR_IF_ERR_EXIT(dnx_iqs_credit_worth_group_get(unit, 0, &credit_worth));

        SHR_IF_ERR_EXIT(dnx_algo_rates_clocks_to_kbits_per_sec(unit, calendar_speed, credit_worth, &rate_internal));

        *rate = rate_internal * dnx_data_sch.interface.cal_speed_resolution_get(unit);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get active calendar id for the provided scheduler interface
 */
static int
dnx_scheduler_interface_active_calendar_get(
    int unit,
    int core,
    int sch_if,
    int *cal_length,
    uint32 *calendar)
{
    int active_cal;

    SHR_FUNC_INIT_VARS(unit);

    *cal_length = 0;

    SHR_IF_ERR_EXIT(dnx_sch_interface_calendar_params_get(unit, core, sch_if, &active_cal, cal_length));

    /** Read active SCH calendar */
    SHR_IF_ERR_EXIT(dnx_sch_interface_calendar_get(unit, core, sch_if, active_cal, *cal_length, calendar));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure new calendar for the provided scheduler interface
 */
static int
dnx_scheduler_interface_active_calendar_config(
    int unit,
    int core,
    uint32 sch_if,
    uint32 cal_length,
    uint32 *calendar)
{
    int active_cal;
    int next_active_cal;
    int other_cal_length;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Check which calendars (SCH - Calendars set 'A' or 'B')
     * are currently active. Then build the non-active calendars,
     * and finally swap between the active calendars and the non-active ones.
     */
    SHR_IF_ERR_EXIT(dnx_sch_interface_calendar_params_get(unit, core, sch_if, &active_cal, &other_cal_length));

    next_active_cal = (active_cal + 1) % 2;

    /** Write non-active SCH calendar */
    SHR_IF_ERR_EXIT(dnx_sch_interface_calendar_set(unit, core, sch_if, next_active_cal, cal_length, calendar));

    /** update the calendar length and set the calendar to be active */
    SHR_IF_ERR_EXIT(dnx_sch_interface_calendar_params_set(unit, core, sch_if, next_active_cal, cal_length));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get port rate from scheduler interface calendar
 */
int
dnx_scheduler_port_rate_hw_get(
    int unit,
    bcm_port_t logical_port,
    uint32 *rate)
{
    uint32 total_cal_rate;
    int sch_cal_len, sch_if;
    bcm_core_t core;

    uint32 sch_cal[dnx_data_sch.interface.big_calendar_size_get(unit)];
    int base_hr;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));

    /** Get scheduler interface */
    SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_get(unit, logical_port, &sch_if));

    sch_cal_len = 0;

    /** Retrieve calendars */
    SHR_IF_ERR_EXIT(dnx_scheduler_interface_active_calendar_get(unit, core, sch_if, &sch_cal_len, sch_cal));

    /** Retrieve total sch rate*/
    SHR_IF_ERR_EXIT(dnx_scheduler_port_total_calendar_rate_get(unit, core, sch_if, &total_cal_rate));

    /** Calculate sch rates*/
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));

    SHR_IF_ERR_EXIT(dnx_algo_cal_simple_object_rate_get(unit, sch_cal, sch_cal_len, total_cal_rate, base_hr, rate));

exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief - Set port rate to swstate for scheduler interface calendar
 */
int
dnx_scheduler_port_rate_sw_set(
    int unit,
    bcm_port_t logical_port,
    uint32 rate)
{
    bcm_core_t core;
    int sch_if;

    SHR_FUNC_INIT_VARS(unit);

    /** Setting rate to software */
    SHR_IF_ERR_EXIT(dnx_scheduler_db.port.rate.set(unit, logical_port, rate));
    SHR_IF_ERR_EXIT(dnx_scheduler_db.port.valid.set(unit, logical_port, TRUE));

    /** Get scheduler interface */
    SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_get(unit, logical_port, &sch_if));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));

    /** mark E2E interface as modified */
    SHR_IF_ERR_EXIT(dnx_scheduler_db.interface.modified.bit_set(unit, core, sch_if));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - configure all modified scheduler interface calendars
 */
int
dnx_scheduler_port_rate_hw_set(
    int unit)
{
    uint32 sch_calendar_len, max_cal_size;
    uint32 total_if_rate = 0, current_if_rate = 0;
    int sch_offset, sch_offset_i;

    int rate;
    uint32 rates_per_hr[dnx_data_sch.flow.nof_hr_get(unit)];
    uint32 calendar[dnx_data_sch.interface.big_calendar_size_get(unit)];

    int is_valid = FALSE;
    uint8 is_modified = FALSE;
    int core;
    uint32 is_rate_decreased = FALSE;
    bcm_pbmp_t ifs_bmp, ports_bmp;
    bcm_port_t port, port_i;
    int base_hr;

    dbal_tables_e table_id;
    int entry_max_value;

    SHR_FUNC_INIT_VARS(unit);

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {

        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, core, DNX_ALGO_PORT_LOGICALS_TYPE_TM_E2E_SCH, 0, &ports_bmp));

        BCM_PBMP_ITER(ports_bmp, port)
        {
            /** Get scheduler offset */
            SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_get(unit, port, &sch_offset));

            SHR_IF_ERR_EXIT(dnx_scheduler_db.interface.modified.bit_get(unit, core, sch_offset, &is_modified));
            if (!is_modified)
            {
                continue;
            }

            sal_memset(rates_per_hr, 0, sizeof(uint32) * dnx_data_sch.flow.nof_hr_get(unit));
            sal_memset(calendar, 0, sizeof(uint32) * dnx_data_sch.interface.big_calendar_size_get(unit));

            /** mark E2E interface as not modified */
            SHR_IF_ERR_EXIT(dnx_scheduler_db.interface.modified.bit_clear(unit, core, sch_offset));

            total_if_rate = 0;

            SHR_IF_ERR_EXIT(dnx_algo_port_channels_get(unit, port, 0, &ifs_bmp));

            /** Retrive rates from the software database*/
            BCM_PBMP_ITER(ifs_bmp, port_i)
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_get(unit, port_i, &sch_offset_i));
                if (sch_offset == sch_offset_i)
                {
                    SHR_IF_ERR_EXIT(dnx_scheduler_db.port.valid.get(unit, port_i, &is_valid));
                    if (is_valid)
                    {
                        SHR_IF_ERR_EXIT(dnx_scheduler_db.port.rate.get(unit, port_i, &rate));
                        SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, port_i, &base_hr));

                        rates_per_hr[base_hr] = rate;
                        total_if_rate += rates_per_hr[base_hr];
                    }
                }
            }

            /*
             * in order to avoid momentary packet loss, the order of setting the traversal rate and calendar is important.
             * If rate is decreased then first calender should be set.
             * Otherwise, if rate is increased, first traversal rate 
             * should be set. 
             */
            SHR_IF_ERR_EXIT(dnx_scheduler_port_total_calendar_rate_get(unit, core, sch_offset, &current_if_rate));

            /** is rate decreased? */
            if (current_if_rate > total_if_rate)
            {
                is_rate_decreased = TRUE;
            }

            if (!is_rate_decreased)
            {
                /** Configure traversal rate*/
                SHR_IF_ERR_EXIT(dnx_scheduler_port_total_calendar_rate_set(unit, core, sch_offset, total_if_rate));
            }

            table_id = DNX_SCH_IF_CALENDAR_DBAL_TABLE_GET(unit, sch_offset);

            /** Get maximal value of calendar entry key field */
            SHR_IF_ERR_EXIT(dbal_tables_field_max_value_get(unit, table_id, DBAL_FIELD_ENTRY, TRUE, 0, 0,
                                                            &entry_max_value));

            max_cal_size = entry_max_value + 1;

            /** Calculate calendar*/
            SHR_IF_ERR_EXIT(dnx_algo_cal_simple_from_rates_to_calendar(unit, rates_per_hr,
                                                                       dnx_data_sch.flow.nof_hr_get(unit),
                                                                       total_if_rate, max_cal_size,
                                                                       calendar, &sch_calendar_len));

            /** Configure calendar */
            SHR_IF_ERR_EXIT(dnx_scheduler_interface_active_calendar_config(unit, core, sch_offset,
                                                                           sch_calendar_len, calendar));

            if (is_rate_decreased)
            {
                /** Configure traversal rate*/
                SHR_IF_ERR_EXIT(dnx_scheduler_port_total_calendar_rate_set(unit, core, sch_offset, total_if_rate));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Set interface shaper bandwidth
 */
int
dnx_scheduler_interface_shaper_rate_set(
    int unit,
    bcm_port_t logical_port,
    uint32 rate)
{
    bcm_core_t core;
    uint32 credit_rate, credit_worth;
    int sch_interface;
    uint32 rate_max_value;

    SHR_FUNC_INIT_VARS(unit);

    /** Get scheduler interface */
    SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_get(unit, logical_port, &sch_interface));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));

    /** Get credit size */
    SHR_IF_ERR_EXIT(dnx_iqs_credit_worth_group_get(unit, 0, &credit_worth));

    /** Get Device Interface Max Credit Rate */
    if (0 == rate)
    {
        credit_rate = 0;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_rates_kbits_per_sec_to_clocks(unit, rate, credit_worth, &credit_rate));

        credit_rate = credit_rate * dnx_data_sch.interface.shaper_resolution_get(unit);

        /** Get maximal value of interface credit rate field */
        SHR_IF_ERR_EXIT(dbal_tables_field_predefine_value_get(unit, DBAL_TABLE_SCH_IF_SHAPER,
                                                              DBAL_FIELD_CREDIT_RATE, 0, 0, 0,
                                                              DBAL_PREDEF_VAL_MAX_VALUE, &rate_max_value));
        credit_rate = UTILEX_RANGE_BOUND(credit_rate,
                                         dnx_data_sch.interface.shaper_resolution_get(unit), rate_max_value);
    }

    SHR_IF_ERR_EXIT(dnx_sch_interface_shaper_bandwidth_hw_set(unit, core, sch_interface, credit_rate));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Get interface shaper bandwidth
 */
int
dnx_scheduler_interface_shaper_rate_get(
    int unit,
    bcm_port_t logical_port,
    uint32 *if_rate)
{
    bcm_core_t core;
    uint32 rate_internal, credit_rate, credit_worth;
    int sch_interface;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_sch_if_get(unit, logical_port, &sch_interface));
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));

    /** Get credit size */
    SHR_IF_ERR_EXIT(dnx_iqs_credit_worth_group_get(unit, 0, &credit_worth));

    /** Get Sch Interface Max Credit Rate */
    SHR_IF_ERR_EXIT(dnx_sch_interface_shaper_bandwidth_hw_get(unit, core, sch_interface, &credit_rate));

    if (0 == credit_rate)
    {
        rate_internal = 0;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_rates_clocks_to_kbits_per_sec(unit, credit_rate, credit_worth, &rate_internal));

        rate_internal = rate_internal * dnx_data_sch.interface.shaper_resolution_get(unit);
    }

    *if_rate = rate_internal;

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Function to set the EIR weight of a logical port TCG
 *  
 * @param unit [in] - unit
 * @param logical_port - logical_port 
 * @param cosq [in] - cosq
 * @param is_valid [in] - is valid
 * @param weight [in] - EIR weight to set
 * 
 * @return int 
 */
int
dnx_scheduler_port_tcg_weight_set(
    int unit,
    bcm_port_t logical_port,
    bcm_cos_queue_t cosq,
    int is_valid,
    int weight)
{

    int core, base_hr, nof_priorities;

    SHR_FUNC_INIT_VARS(unit);

    /** Get port core, base HR and nof priorities */
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));
    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priorities_nof_get(unit, logical_port, &nof_priorities));

    /** Force flow control on HR before setting TCG weight.*/
    SHR_IF_ERR_EXIT(dnx_sch_port_hr_force_fc_range_set(unit, core, base_hr, nof_priorities, TRUE));

    /** Set TCG weight */
    SHR_IF_ERR_EXIT(dnx_sch_port_tcg_weight_set(unit, core, base_hr, cosq, is_valid, weight));

    /** Disable flow control on HR after setting TCG weight value.*/
    SHR_IF_ERR_EXIT(dnx_sch_port_hr_force_fc_range_set(unit, core, base_hr, nof_priorities, FALSE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief Function to get the EIR weight of a logical port TCG
 *  
 * @param unit [in] - unit
 * @param logical_port - logical_port 
 * @param cosq [in] - cosq
 * @param is_valid [out] - is valid
 * @param weight [out] - EIR weight
 * 
 * @return int 
 */
int
dnx_scheduler_port_tcg_weight_get(
    int unit,
    bcm_port_t logical_port,
    bcm_cos_queue_t cosq,
    int *is_valid,
    int *weight)
{

    int core, base_hr, nof_priorities;

    SHR_FUNC_INIT_VARS(unit);

    /** Get port core, base HR and nof priorities */
    SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, logical_port, &core));
    SHR_IF_ERR_EXIT(dnx_algo_port_base_hr_get(unit, logical_port, &base_hr));
    SHR_IF_ERR_EXIT(dnx_algo_port_sch_priorities_nof_get(unit, logical_port, &nof_priorities));

    /** Get TCG weight */
    SHR_IF_ERR_EXIT(dnx_sch_port_tcg_weight_get(unit, core, base_hr, cosq, is_valid, weight));

exit:
    SHR_FUNC_EXIT;
}
