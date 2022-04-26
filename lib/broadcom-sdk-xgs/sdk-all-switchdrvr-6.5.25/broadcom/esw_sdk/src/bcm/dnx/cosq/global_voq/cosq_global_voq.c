/** \file cosq_global_voq.c
 * $Id$
 *
 * COSQ Global VOQ functionality \n
 *
 *
 */

/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_COSQ

/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_bitstream.h>
#include <shared/bitop.h>
#include <shared/error.h>
#include <sal/compiler.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_global_voq.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_cosq_aqm.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ingr_congestion.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnxc/dnxc_time.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_tables.h>
#include <bcm/cosq.h>
#include <bcm/types.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/cosq_global_voq_access.h>
#include <bcm_int/dnx/cosq/global_voq/cosq_global_voq.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/cosq/cosq.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
/*
 * }
 */

/*
 * Defines
 * {
 */

#define DNX_GLOBAL_VOQ_RESERVED_TC_PROFILE_INDEX    0
#define DNX_GLOBAL_VOQ_RESERVED_BASE_INDEX          0

#define DNX_GLOBAL_VOQ_CONTROL_PHASE_MODE_ECI     0
#define DNX_GLOBAL_VOQ_CONTROL_PHASE_MODE_SDK     1

/**
 * Used with suspend DB
 */
enum dnx_cosq_global_voq_mechanism_type_e
{
    GLOBAL_VOQ_MECHANISM_TYPE_DROP_GREEN,
    GLOBAL_VOQ_MECHANISM_TYPE_DROP_YELLOW,
    GLOBAL_VOQ_MECHANISM_TYPE_CNI,
    GLOBAL_VOQ_MECHANISM_TYPE_COUNT
};

static const dbal_tables_e mechanism_tables[] = {
    DBAL_TABLE_GLOBAL_VOQ_DROP_GREEN_PROFILE,
    DBAL_TABLE_GLOBAL_VOQ_DROP_YELLOW_PROFILE,
    DBAL_TABLE_GLOBAL_VOQ_CNI_PROFILE
};

/*
 * }
 */

static shr_error_e dnx_cosq_global_voq_control_tc_profile_hw_set(
    int unit,
    uint32 tc_profile_index,
    uint32 tc_index,
    uint32 base_offset);

static shr_error_e dnx_cosq_global_voq_port_internal_set(
    int unit,
    uint32 flags,
    bcm_core_t core,
    uint32 out_tm_port,
    uint32 tc_profile_index,
    uint32 bin_profile_index);

static shr_error_e dnx_cosq_global_voq_port_hw_get(
    int unit,
    bcm_core_t core,
    uint32 out_tm_port,
    uint32 *tc_profile_index,
    uint32 *bin_profile_index,
    uint32 *base_offset,
    uint32 *is_set);

static shr_error_e dnx_cosq_global_voq_control_tc_profile_hw_get(
    int unit,
    uint32 tc_profile_index,
    uint32 tc_index,
    uint32 *base_offset);

static shr_error_e dnx_cosq_global_voq_control_bin_profile_hw_get(
    int unit,
    uint32 bin_profile,
    uint32 bin_index,
    uint32 *threshold);

static shr_error_e dnx_cosq_global_voq_profile_hw_set(
    int unit,
    int profile_index,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint32 arg);

static shr_error_e dnx_cosq_global_voq_profile_hw_get(
    int unit,
    int profile_index,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint32 *arg);

static shr_error_e dnx_cosq_global_voq_prob_is_enable_get(
    int unit,
    int profile_id,
    dbal_tables_e table_id,
    uint32 *is_enable);

static shr_error_e
dnx_cosq_global_voq_suspend_db_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_db.suspend_db.mechanism_state.alloc(unit));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_tc_profile_cnt_init(
    int unit)
{
    bcm_core_t core;

    SHR_FUNC_INIT_VARS(unit);

    /** Initialize tc_profile_cnt to */
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        /*
         * DNX_GLOBAL_VOQ_RESERVED_BASE_INDEX uses TC profile 0.
         */
        SHR_IF_ERR_EXIT(dnx_cosq_global_voq_db.tc_profile_cnt.inc(unit, core, 0 /** tc_profile_index */ , 1));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_tc_profile_init(
    int unit)
{
    int tc_index;

    SHR_FUNC_INIT_VARS(unit);

    /** initialize reserved TC profile to all use offset 0 */
    for (tc_index = 0; tc_index < DNX_COSQ_NOF_TC; ++tc_index)
    {
        SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_tc_profile_hw_set
                        (unit, DNX_GLOBAL_VOQ_RESERVED_TC_PROFILE_INDEX, tc_index, 0));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_aggregation_res_mngr_init(
    int unit)
{
    dnx_algo_res_create_data_t data;
    bcm_core_t core;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_db.aggregation_res_mngr.alloc(unit));

    /** create aggregation res mngr */
    sal_memset(&data, 0, sizeof(dnx_algo_res_create_data_t));

    /** reserve base_index 0 to be used to aggregate unmapped ports */
    data.first_element = 1;
    data.nof_elements = dnx_data_global_voq.general.aggregation_entries_nof_get(unit) - 1;
    data.flags = 0;
    sal_strncpy(data.name, DNX_COSQ_GLOBAL_VOQ_AGGR_ENTRIES_RESOURCE, DNX_ALGO_RES_MNGR_MAX_NAME_LENGTH - 1);

    /** create a resource manager per core */
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        /** create resource manager */
        SHR_IF_ERR_EXIT(dnx_cosq_global_voq_db.aggregation_res_mngr.create(unit, core, &data, NULL));
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_cosq_global_voq_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_COSQ_GLOBAL_VOQ_FEATURE_VERIFY(unit);

    /** init sw state */
    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_db.init(unit));

    /** create tc profile nounter per core. */
    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_db.tc_profile_cnt.alloc(unit));

    /** create aggregation res mngr */
    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_aggregation_res_mngr_init(unit));

    /** init default tc_profile */
    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_tc_profile_init(unit));

    /** init TC profile use counter */
    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_tc_profile_cnt_init(unit));

    /** init suspend db  */
    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_suspend_db_init(unit));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_control_cycles_to_time_calc(
    int unit,
    int cycles,
    int *time)
{
    dnxc_time_t dnxc_time;
    uint32 nof_clock_cycles;

    SHR_FUNC_INIT_VARS(unit);

    nof_clock_cycles = cycles;

    SHR_IF_ERR_EXIT(dnxc_time_clock_cycles_to_time_get(unit, nof_clock_cycles, DNXC_TIME_UNIT_NSEC, &dnxc_time));

    *time = dnxc_time.time;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_control_time_to_cycles_calc(
    int unit,
    int time,
    int *cycles)
{
    dnxc_time_t dnxc_time;
    uint32 nof_clock_cycles;

    SHR_FUNC_INIT_VARS(unit);

    dnxc_time.time_units = DNXC_TIME_UNIT_NSEC;
    dnxc_time.time = time;

    SHR_IF_ERR_EXIT(dnxc_time_time_to_clock_cycles_get(unit, &dnxc_time, &nof_clock_cycles));

    *cycles = nof_clock_cycles;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_control_granularity_verify(
    int unit,
    int granularity)
{
    int index;
    int is_found = 0;
    int nof_entries = 0;

    SHR_FUNC_INIT_VARS(unit);

    nof_entries = dnx_data_global_voq.general.valid_quanta_sizes_info_get(unit)->key_size[0];

    /** iterate over valid_quanta_sizes table a search for a match */
    for (index = 0; index < nof_entries; ++index)
    {
        int entry_quanta_size = dnx_data_global_voq.general.valid_quanta_sizes_get(unit, index)->quanta_size;
        if (granularity == entry_quanta_size)
        {
            SHR_EXIT();
        }
    }

    if (!is_found)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "granularity value %d is not supported", granularity);
    }
exit:

    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_control_hw_to_granularity_get(
    int unit,
    int word_shift,
    int *granularity)
{
    int word_size = dnx_data_ingr_congestion.info.words_resolution_get(unit);
    uint64 local_granularity = 1;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * granularity = 2^cfg * word_size
     */
    COMPILER_64_SHL(local_granularity, word_shift);
    COMPILER_64_UMUL_32(local_granularity, word_size);

    /*
     * User supplies KB
     */
    COMPILER_64_UDIV_32(local_granularity, 1024);

    *granularity = local_granularity;

    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_control_granularity_to_hw_get(
    int unit,
    int granularity,
    int *word_shift)
{
    int word_size = dnx_data_ingr_congestion.info.words_resolution_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    /*
     * User supplies KB
     */
    granularity = granularity * 1024;

    /*
     * granularity = 2^cfg * word_size
     */
    SHR_IF_ERR_EXIT(utilex_log2_round(granularity / word_size, (uint32 *) word_shift));

exit:

    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_control_hw_set(
    int unit,
    dbal_fields_e field_id,
    int val)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GLOBAL_VOQ_INGRESS_CONFIG, &entry_handle_id));

    dbal_entry_value_field32_set(unit, entry_handle_id, field_id, INST_SINGLE, val);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_control_hw_get(
    int unit,
    dbal_fields_e field_id,
    int *val)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GLOBAL_VOQ_INGRESS_CONFIG, &entry_handle_id));

    dbal_value_field32_request(unit, entry_handle_id, field_id, INST_SINGLE, (uint32 *) val);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_control_granularity_set(
    int unit,
    int granularity)
{
    int words_shift;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_granularity_verify(unit, granularity));

    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_granularity_to_hw_get(unit, granularity, &words_shift));

    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_hw_set(unit, DBAL_FIELD_QUANTA_WORD_SIZE_SHIFT, words_shift));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_control_granularity_get(
    int unit,
    int *granularity)
{
    int words_shift;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_hw_get(unit, DBAL_FIELD_QUANTA_WORD_SIZE_SHIFT, &words_shift));

    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_hw_to_granularity_get(unit, words_shift, granularity));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_control_refresh_peroiod_verify(
    int unit,
    int refresh_period)
{
    int min_refresh_interval = dnx_data_global_voq.general.min_refresh_interval_get(unit);
    int time;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_cycles_to_time_calc(unit, min_refresh_interval, &time));

    /** verify that value is not less than minimum allowed */
    if (refresh_period < time)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "refresh period must be larger than %d nano-seconds", time);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_control_refresh_period_set(
    int unit,
    int refresh_period)
{
    int refresh_period_cycles;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_refresh_peroiod_verify(unit, refresh_period));

    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_time_to_cycles_calc(unit, refresh_period, &refresh_period_cycles));

    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_hw_set(unit, DBAL_FIELD_REFRESH_PERIOD, refresh_period_cycles));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_control_refresh_period_get(
    int unit,
    int *refresh_period)
{
    int refresh_period_cycles;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_hw_get(unit, DBAL_FIELD_REFRESH_PERIOD, &refresh_period_cycles));

    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_cycles_to_time_calc(unit, refresh_period_cycles, refresh_period));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_control_phase_mode_verify(
    int unit,
    int phase_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_RANGE_VERIFY(phase_mode, 0, 1, _SHR_E_PARAM, "Invalid phase_mode %d\n", phase_mode);

exit:
    SHR_FUNC_EXIT;
}

/**
 *
 * @param unit
 * @param phase_mode        0 - ECI, 1 - SDK
 * @return
 */
static shr_error_e
dnx_cosq_global_voq_control_phase_mode_set(
    int unit,
    int phase_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_phase_mode_verify(unit, phase_mode));

    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_hw_set(unit, DBAL_FIELD_PHASE_MODE, phase_mode));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_control_phase_mode_get(
    int unit,
    int *phase_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_hw_get(unit, DBAL_FIELD_PHASE_MODE, phase_mode));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_control_egress_phase_transition_timeout_set(
    int unit,
    int phase_transition_timeout)
{
    int phase_transition_timeout_cycles;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_time_to_cycles_calc
                    (unit, phase_transition_timeout, &phase_transition_timeout_cycles));

    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_hw_set
                    (unit, DBAL_FIELD_PHASE_TRANSITION_TIMEOUT, phase_transition_timeout_cycles));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_control_egress_phase_transition_timeout_get(
    int unit,
    int *phase_transition_timeout)
{
    int phase_transition_timeout_cycles;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_hw_get
                    (unit, DBAL_FIELD_PHASE_TRANSITION_TIMEOUT, &phase_transition_timeout_cycles));

    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_cycles_to_time_calc
                    (unit, phase_transition_timeout_cycles, phase_transition_timeout));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_control_phase_start_verify(
    int unit,
    int is_start)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_VAL_VERIFY(is_start, 1, _SHR_E_PARAM, "only value 1 is supported\n");

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_control_phase_start_set(
    int unit,
    dbal_fields_e field_id,
    int is_start)
{
    int current_phase;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_phase_start_verify(unit, is_start));

    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_hw_get(unit, field_id, &current_phase));

    /** toggle between 0 and 1, based on current value in HW */
    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_hw_set(unit, field_id, 1 - current_phase));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_control_ingress_phase_start_set(
    int unit,
    int is_start)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_phase_start_set(unit, DBAL_FIELD_PHASE_INGRESS, is_start));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_control_egress_phase_start_set(
    int unit,
    int is_start)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_phase_start_set(unit, DBAL_FIELD_PHASE_EGRESS, is_start));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_control_phase_start_get(
    int unit,
    int *is_start)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "get operation is not supported for this control type\n");

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_control_egress_nof_consecutive_down_set(
    int unit,
    int nof_consecutive_down)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_hw_set(unit, DBAL_FIELD_CONSECUTIVE_DOWN, nof_consecutive_down));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_control_egress_nof_consecutive_down_get(
    int unit,
    int *nof_consecutive_down)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_hw_get(unit, DBAL_FIELD_CONSECUTIVE_DOWN, nof_consecutive_down));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_control_egress_suspend_set_verify(
    int unit,
    int is_suspend)
{
    int is_suspended;
    SHR_FUNC_INIT_VARS(unit);

    /** read current suspend state */
    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_db.suspend_db.is_suspended.get(unit, &is_suspended));

    if (is_suspended & (is_suspend == 1))
    {
        /** Deny option to put the system in to 'suspend' mode if already suspended */
        SHR_ERR_EXIT(_SHR_E_CONFIG, "device is already in a suspended state\n");
    }

    if (!is_suspended & (is_suspend == 0))
    {
        /** Deny option to put the system out of 'suspend' mode if not suspended */
        SHR_ERR_EXIT(_SHR_E_CONFIG, "device is already in a not suspended state\n");
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_control_egress_suspend_activate(
    int unit)
{
    uint32 prfile_nof = dnx_data_cosq_aqm.general.profile_nof_get(unit);
    uint32 profile_index;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * indicate that the selected core is suspended so that
     * modification to TC profile and port mapping will be denied
     */
    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_db.suspend_db.is_suspended.set(unit, 1));

    /*
     * iterate over all profiles and mechanisms
     */
    for (profile_index = 0; profile_index < prfile_nof; ++profile_index)
    {
        int mechanism_index;
        uint32 is_enabled, is_prob_enabled;

        for (mechanism_index = 0; mechanism_index < GLOBAL_VOQ_MECHANISM_TYPE_COUNT; ++mechanism_index)
        {
            dbal_tables_e table_id = mechanism_tables[mechanism_index];

            /*
             * sample current state
             */
            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_profile_hw_get
                            (unit, profile_index, table_id, DBAL_FIELD_MAX_ENABLE, &is_enabled));

            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_profile_hw_get
                            (unit, profile_index, table_id, DBAL_FIELD_PROB_ENABLE, &is_prob_enabled));

            /*
             * store samples state
             */
            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_db.suspend_db.mechanism_state.is_enabled.set
                            (unit, profile_index, mechanism_index, is_enabled));

            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_db.suspend_db.mechanism_state.is_prob_enabled.set
                            (unit, profile_index, mechanism_index, is_prob_enabled));

            /*
             * Disable global VOQ mechanisms
             */
            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_profile_hw_set
                            (unit, profile_index, table_id, DBAL_FIELD_MAX_ENABLE, 0));

            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_profile_hw_set
                            (unit, profile_index, table_id, DBAL_FIELD_PROB_ENABLE, 0));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_control_egress_suspend_resume(
    int unit)
{
    uint32 prfile_nof = dnx_data_cosq_aqm.general.profile_nof_get(unit);
    uint32 profile_index;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * iterate over all profiles and mechanisms
     */
    for (profile_index = 0; profile_index < prfile_nof; ++profile_index)
    {
        int mechanism_index;
        int is_enabled, is_prob_enabled;

        for (mechanism_index = 0; mechanism_index < GLOBAL_VOQ_MECHANISM_TYPE_COUNT; ++mechanism_index)
        {
            dbal_tables_e table_id = mechanism_tables[mechanism_index];

            /*
             * store samples state
             */
            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_db.suspend_db.mechanism_state.is_enabled.get
                            (unit, profile_index, mechanism_index, &is_enabled));

            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_db.suspend_db.mechanism_state.is_prob_enabled.get
                            (unit, profile_index, mechanism_index, &is_prob_enabled));

            /*
             * resume global VOQ mechanisms
             */
            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_profile_hw_set
                            (unit, profile_index, table_id, DBAL_FIELD_MAX_ENABLE, is_enabled));

            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_profile_hw_set
                            (unit, profile_index, table_id, DBAL_FIELD_PROB_ENABLE, is_prob_enabled));
        }
    }

    /*
     * indicate that the system is not in suspended mode
     */
    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_db.suspend_db.is_suspended.set(unit, 0));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_control_egress_suspend_set(
    int unit,
    int is_suspend)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_cosq_global_voq_control_egress_suspend_set_verify(unit, is_suspend));

    if (is_suspend)
    {
        SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_egress_suspend_activate(unit));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_egress_suspend_resume(unit));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_control_egress_suspend_get_verify(
    int unit,
    int *is_suspend)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(is_suspend, _SHR_E_PARAM, "is_suspend");

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_control_egress_suspend_get(
    int unit,
    int *is_suspend)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_cosq_global_voq_control_egress_suspend_get_verify(unit, is_suspend));

    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_db.suspend_db.is_suspended.get(unit, is_suspend));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_control_device_verify(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_COSQ_GLOBAL_VOQ_FEATURE_VERIFY(unit);

    /** verify port */
    if (port != 0 && BCM_COSQ_GPORT_CORE_GET(port) != BCM_CORE_ALL)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Control type expects port 0 or BCM_CORE_ALL gport only\n");
    }

    /*
     * parameters that are specific to set operation are verified later on.
     * The following switch-case should only verify parameters common to both
     * set and get operation
     */

    /** verify type specific values */
    switch (type)
    {
        case bcmCosqControlGlobalVoqIngressGranularity:
        case bcmCosqControlGlobalVoqIngressRefreshPeriod:
        case bcmCosqControlGlobalVoqPhaseMode:
        case bcmCosqControlGlobalVoqEgressPhaseTransitionTimeout:
        case bcmCosqControlGlobalVoqEgressPhaseStart:
        case bcmCosqControlGlobalVoqIngressPhaseStart:
        case bcmCosqControlGlobalVoqEgressNofConsecutiveDown:
        case bcmCosqControlGlobalVoqEgressSuspend:
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid control type %d", type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_control_device_set_verify(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_device_verify(unit, port, cosq, type, arg));

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_cosq_global_voq_control_device_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int arg)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_cosq_global_voq_control_device_set_verify(unit, gport, cosq, type, arg));

    switch (type)
    {
        case bcmCosqControlGlobalVoqIngressGranularity:
            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_granularity_set(unit, arg));
            break;
        case bcmCosqControlGlobalVoqIngressRefreshPeriod:
            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_refresh_period_set(unit, arg));
            break;
        case bcmCosqControlGlobalVoqPhaseMode:
            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_phase_mode_set(unit, arg));
            break;
        case bcmCosqControlGlobalVoqEgressPhaseTransitionTimeout:
            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_egress_phase_transition_timeout_set(unit, arg));
            break;
        case bcmCosqControlGlobalVoqEgressPhaseStart:
            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_egress_phase_start_set(unit, arg));
            break;
        case bcmCosqControlGlobalVoqIngressPhaseStart:
            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_ingress_phase_start_set(unit, arg));
            break;
        case bcmCosqControlGlobalVoqEgressNofConsecutiveDown:
            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_egress_nof_consecutive_down_set(unit, arg));
            break;
        case bcmCosqControlGlobalVoqEgressSuspend:
            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_egress_suspend_set(unit, arg));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Invalid cosq control type %d\n", type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_control_device_get_verify(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(arg, _SHR_E_PARAM, "arg");

    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_device_verify(unit, port, cosq, type, 0));

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_cosq_global_voq_control_device_get(
    int unit,
    bcm_gport_t port,
    bcm_cos_queue_t cosq,
    bcm_cosq_control_t type,
    int *arg)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_cosq_global_voq_control_device_get_verify(unit, port, cosq, type, arg));

    switch (type)
    {
        case bcmCosqControlGlobalVoqIngressGranularity:
            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_granularity_get(unit, arg));
            break;
        case bcmCosqControlGlobalVoqIngressRefreshPeriod:
            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_refresh_period_get(unit, arg));
            break;
        case bcmCosqControlGlobalVoqPhaseMode:
            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_phase_mode_get(unit, arg));
            break;
        case bcmCosqControlGlobalVoqEgressPhaseTransitionTimeout:
            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_egress_phase_transition_timeout_get(unit, arg));
            break;
        case bcmCosqControlGlobalVoqEgressPhaseStart:
        case bcmCosqControlGlobalVoqIngressPhaseStart:
            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_phase_start_get(unit, arg));
            break;
        case bcmCosqControlGlobalVoqEgressNofConsecutiveDown:
            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_egress_nof_consecutive_down_get(unit, arg));
            break;
        case bcmCosqControlGlobalVoqEgressSuspend:
            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_egress_suspend_get(unit, arg));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Invalid cosq control type %d\n", type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_tc_profile_verify(
    int unit,
    bcm_gport_t tc_profile)
{
    int tc_profile_index;

    SHR_FUNC_INIT_VARS(unit);

    if (!BCM_GPORT_IS_GLOBAL_VOQ_TC_PROFILE(tc_profile))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid tc_profile\n");
    }

    tc_profile_index = BCM_GPORT_GLOBAL_VOQ_TC_PROFILE_GET(tc_profile);

    SHR_RANGE_VERIFY(tc_profile_index, 0, dnx_data_global_voq.general.tc_profile_nof_get(unit) - 1,
                     _SHR_E_PARAM, "Invalid tc_profile %d\n", tc_profile_index);

exit:
    SHR_FUNC_EXIT;

}

static shr_error_e
dnx_cosq_global_voq_bin_profile_verify(
    int unit,
    bcm_gport_t bin_profile)
{
    int bin_profile_index;

    SHR_FUNC_INIT_VARS(unit);

    if (!BCM_GPORT_IS_GLOBAL_VOQ_BIN_PROFILE(bin_profile))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid tc_profile\n");
    }

    bin_profile_index = BCM_GPORT_GLOBAL_VOQ_BIN_PROFILE_GET(bin_profile);

    SHR_RANGE_VERIFY(bin_profile_index, 0, dnx_data_global_voq.general.bin_profile_nof_get(unit) - 1, _SHR_E_PARAM,
                     "Invalid tc_profile %d\n", bin_profile_index);

exit:
    SHR_FUNC_EXIT;

}

static shr_error_e
dnx_cosq_global_voq_tc_profile_length_get(
    int unit,
    uint32 tc_profile_index,
    uint32 *tc_length)
{
    uint32 offset;
    uint32 tc_index;
    uint32 max_offset = 0;

    SHR_FUNC_INIT_VARS(unit);

    for (tc_index = 0; tc_index < DNX_COSQ_NOF_TC; ++tc_index)
    {
        SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_tc_profile_hw_get(unit, tc_profile_index, tc_index, &offset));

        max_offset = UTILEX_MAX(max_offset, offset);
    }

    *tc_length = max_offset + 1;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_out_tm_port_get(
    int unit,
    bcm_gport_t gport,
    bcm_core_t * core,
    uint32 *out_tm_port)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, gport, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));

    SHR_IF_ERR_EXIT(dnx_algo_port_out_tm_port_get(unit, gport_info.local_port, core, out_tm_port));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_port_hw_get(
    int unit,
    bcm_core_t core,
    uint32 out_tm_port,
    uint32 *tc_profile_index,
    uint32 *bin_profile_index,
    uint32 *base_offset,
    uint32 *is_set)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GLOBAL_VOQ_PORT_MAP, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_TM_PORT, out_tm_port);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_BASE, INST_SINGLE, base_offset);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TC_PROFILE, INST_SINGLE, tc_profile_index);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_BIN_PROFILE, INST_SINGLE, bin_profile_index);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    *is_set = (*base_offset != 0) ? 1 : 0;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

static shr_error_e
dnx_cosq_global_voq_port_hw_set(
    int unit,
    bcm_core_t core,
    uint32 out_tm_port,
    uint32 base_offset,
    uint32 tc_profile_index,
    uint32 bin_profile_index)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GLOBAL_VOQ_PORT_MAP, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_TM_PORT, out_tm_port);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BASE, INST_SINGLE, base_offset);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TC_PROFILE, INST_SINGLE, tc_profile_index);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BIN_PROFILE, INST_SINGLE, bin_profile_index);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_port_hw_clear(
    int unit,
    bcm_core_t core,
    uint32 out_tm_port)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_port_hw_set(unit, core, out_tm_port, 0, 0, 0));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_port_set_verify(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    bcm_cosq_global_voq_info_t * info,
    uint32 *status)
{
    uint32 valid_flags;
    uint32 tc_length;
    uint32 out_tm_port;
    bcm_core_t core;
    int nof_free_elements, is_suspended;
    uint32 is_set;
    uint32 base_index;
    uint32 tc_profile_index;
    uint32 bin_profile_index_dummy, tc_profile_index_dummy;
    SHR_FUNC_INIT_VARS(unit);

    DNX_COSQ_GLOBAL_VOQ_FEATURE_VERIFY(unit);

    valid_flags = BCM_COSQ_GLOBAL_VOQ_UNSET;

    /** verify pointers */
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");
    SHR_NULL_CHECK(status, _SHR_E_PARAM, "status");

    /** verify supported fags */
    SHR_MASK_VERIFY(flags, valid_flags, _SHR_E_PARAM, "provided unsupported flags\n");

    /** verify port value */
    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_out_tm_port_get(unit, gport, &core, &out_tm_port));

    /** verify tc_profile */
    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_tc_profile_verify(unit, info->tc_profile));

    /** verify bin_profile */
    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_bin_profile_verify(unit, info->bin_profile));

    tc_profile_index = BCM_GPORT_GLOBAL_VOQ_TC_PROFILE_GET(info->tc_profile);

    /** check tc_profile length - this indicates how many aggregation entries should be allocated to this port */
    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_tc_profile_length_get(unit, tc_profile_index, &tc_length));

    /** get the number of free elements in the aggregation table */
    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_db.aggregation_res_mngr.nof_free_elements_get(unit, core, &nof_free_elements));

    /** verify that the Global VOQ CNI/Drop enforcement is not suppressed */
    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_db.suspend_db.is_suspended.get(unit, &is_suspended));
    if (is_suspended)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "device is in suspended mode, modifications are not allowed\n");
    }

    /** check if already allocated */
    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_port_hw_get(unit, core, out_tm_port, &tc_profile_index_dummy,
                                                    &bin_profile_index_dummy, &base_index, &is_set));

    if (_SHR_IS_FLAG_SET(flags, BCM_COSQ_GLOBAL_VOQ_UNSET))
    {
        if (!is_set)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "port is not mapped\n");
        }

        /** base index 0 is reserved */
        if (base_index == DNX_GLOBAL_VOQ_RESERVED_BASE_INDEX)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "base index 0 is reserved\n");

        }
    }
    else
    {
        if (is_set)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "port is already mapped. You must first unset it\n");
        }

        /*
         * At this point it is enough to verify that the number_of_free_elements >= tc_length
         * even though the table might be fragmented.
         *
         * During the set operation, if needed the table is going to be defragmented
         */
        if (nof_free_elements < tc_length)
        {
            SHR_ERR_EXIT(_SHR_E_FULL, "not enough aggregation entries\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

typedef struct dnx_cosq_global_voq_defrag_s
{
    uint32 out_tm_port;
    uint32 base_index;
    uint32 tc_profile;
    uint32 bin_profile;
} dnx_cosq_global_voq_defrag_t;

static shr_error_e
dnx_cosq_global_voq_aggregation_table_defragment(
    int unit,
    bcm_core_t core)
{
    uint32 *tc_length_arr = NULL;
    dnx_cosq_global_voq_defrag_t *info_arr = NULL;
    int tc_profile_nof = dnx_data_global_voq.general.tc_profile_nof_get(unit);
    int nof_out_tm_port = dnx_data_port.general.nof_out_tm_ports_get(unit);
    uint32 tc_profile_index, bin_profile_index, out_tm_port;
    int info_cnt = 0;
    uint32 entry_tc_length;
    uint32 base_offset;
    int i;

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_ERR_EXIT(tc_length_arr, sizeof(int) * tc_profile_nof, "tc_length_arr", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    SHR_ALLOC_ERR_EXIT(info_arr, sizeof(dnx_cosq_global_voq_defrag_t) * nof_out_tm_port, "info_arr",
                       "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    sal_memset(info_arr, 0, sizeof(bcm_cosq_global_voq_info_t) * nof_out_tm_port);

    /*
     * Get all tc length
     */
    for (tc_profile_index = 0; tc_profile_index < tc_profile_nof; ++tc_profile_index)
    {
        SHR_IF_ERR_EXIT(dnx_cosq_global_voq_tc_profile_length_get
                        (unit, tc_profile_index, &tc_length_arr[tc_profile_index]));
    }

    /*
     * get all the mapped OUT_TM_PORTs and their tc profile.
     */
    for (out_tm_port = 0, info_cnt = 0; out_tm_port < dnx_data_port.general.nof_out_tm_ports_get(unit); ++out_tm_port)
    {
        uint32 is_set;

        SHR_IF_ERR_EXIT(dnx_cosq_global_voq_port_hw_get(unit, core, out_tm_port, &tc_profile_index,
                                                        &bin_profile_index, &base_offset, &is_set));

        /*
         * entry DNX_GLOBAL_VOQ_RESERVED_BASE_INDEX will not
         * be part of the info_arr since it will return is_set=false
         * Therefore, we do not need to take it into consideration in the defrag process.
         */
        if (is_set)
        {
            entry_tc_length = tc_length_arr[tc_profile_index];

            info_arr[info_cnt].out_tm_port = out_tm_port;
            info_arr[info_cnt].base_index = base_offset;
            info_arr[info_cnt].tc_profile = tc_profile_index;
            info_arr[info_cnt].bin_profile = bin_profile_index;

            base_offset += entry_tc_length;
            info_cnt++;
        }
    }

    /** clear the entire table */
    for (i = 0; i < info_cnt; ++i)
    {
        SHR_IF_ERR_EXIT(dnx_cosq_global_voq_port_internal_set(unit,
                                                              BCM_COSQ_GLOBAL_VOQ_UNSET,
                                                              core,
                                                              info_arr[i].out_tm_port,
                                                              info_arr[i].tc_profile, info_arr[i].bin_profile));
    }

    /** fill the table using info_arr */
    for (i = 0; i < info_cnt; ++i)
    {
        SHR_IF_ERR_EXIT(dnx_cosq_global_voq_port_internal_set(unit,
                                                              0,
                                                              core,
                                                              info_arr[i].out_tm_port,
                                                              info_arr[i].tc_profile, info_arr[i].bin_profile));
    }

exit:
    SHR_FREE(tc_length_arr);
    SHR_FREE(info_arr);

    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_port_internal_set(
    int unit,
    uint32 flags,
    bcm_core_t core,
    uint32 out_tm_port,
    uint32 tc_profile_index,
    uint32 bin_profile_index)
{
    bcm_cosq_global_voq_info_t info;
    uint32 status;
    bcm_port_t port;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_out_tm_to_logical_get(unit, core, out_tm_port, &port));

    BCM_GPORT_GLOBAL_VOQ_TC_PROFILE_SET(info.tc_profile, tc_profile_index);
    BCM_GPORT_GLOBAL_VOQ_BIN_PROFILE_SET(info.bin_profile, bin_profile_index);

    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_port_set(unit, BCM_COSQ_GLOBAL_VOQ_UNSET, port, &info, &status));

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_cosq_global_voq_port_set(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    bcm_cosq_global_voq_info_t * info,
    uint32 *status)
{
    uint32 tc_length;
    uint32 base_index;
    bcm_core_t core;
    uint32 out_tm_port;
    uint32 is_set;
    uint32 tc_profile_index, bin_profile_index;

    SHR_FUNC_INIT_VARS(unit);

    *status = 0;

    SHR_INVOKE_VERIFY_DNXC(dnx_cosq_global_voq_port_set_verify(unit, flags, gport, info, status));

    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_out_tm_port_get(unit, gport, &core, &out_tm_port));

    if (_SHR_IS_FLAG_SET(flags, BCM_COSQ_GLOBAL_VOQ_UNSET))
    {
        /** clear */
        /** get base_index to free */
        SHR_IF_ERR_EXIT(dnx_cosq_global_voq_port_hw_get(unit, core, out_tm_port, &tc_profile_index,
                                                        &bin_profile_index, &base_index, &is_set));

        /** get tc length */
        SHR_IF_ERR_EXIT(dnx_cosq_global_voq_tc_profile_length_get(unit, tc_profile_index, &tc_length));

        /** free from res_mngr */
        SHR_IF_ERR_EXIT(dnx_cosq_global_voq_db.
                        aggregation_res_mngr.free_several(unit, core, tc_length, base_index, NULL));

        /** clear HW entry */
        SHR_IF_ERR_EXIT(dnx_cosq_global_voq_port_hw_clear(unit, core, out_tm_port));

        /** decrease TC profile occurrences */
        SHR_IF_ERR_EXIT(dnx_cosq_global_voq_db.tc_profile_cnt.dec(unit, core, tc_profile_index, tc_length));
    }
    else
    {
        /** add */
        int rv;

        tc_profile_index = BCM_GPORT_GLOBAL_VOQ_TC_PROFILE_GET(info->tc_profile);
        SHR_IF_ERR_EXIT(dnx_cosq_global_voq_tc_profile_length_get(unit, tc_profile_index, &tc_length));

        /** attempts to allocate 'tc_length' entries */
        rv = dnx_cosq_global_voq_db.aggregation_res_mngr.allocate_several(unit, core, DNX_ALGO_RES_ALLOCATE_SIMULATION,
                                                                          tc_length, NULL, (int *) &base_index);

        if (rv == _SHR_E_NONE)
        {
            /** allocate */
            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_db.
                            aggregation_res_mngr.allocate_several(unit, core, 0, tc_length, NULL, (int *) &base_index));
        }
        else if (rv == _SHR_E_RESOURCE)
        {
            /** We already verified that after defragmentation there will be a continuoes chunk to place the port to */

            /** need to defragment */
            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_aggregation_table_defragment(unit, core));

            /** allocate */
            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_db.
                            aggregation_res_mngr.allocate_several(unit, core, 0, tc_length, NULL, (int *) &base_index));

            *status = BCM_COSQ_GLOBAL_VOQ_STATUS_DEFRAGMENTED;
        }
        else
        {
            SHR_IF_ERR_EXIT(rv);
        }

        /** write to HW */
        {
            uint32 tc_profile_index = BCM_GPORT_GLOBAL_VOQ_TC_PROFILE_GET(info->tc_profile);
            uint32 bin_profile_index = BCM_GPORT_GLOBAL_VOQ_BIN_PROFILE_GET(info->bin_profile);

            /** add new entry to HW */
            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_port_hw_set(unit, core, out_tm_port, base_index,
                                                            tc_profile_index, bin_profile_index));
        }

        /** increase TC profile occurrences */
        SHR_IF_ERR_EXIT(dnx_cosq_global_voq_db.tc_profile_cnt.inc(unit, core, tc_profile_index, tc_length));
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_port_get_verify(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    bcm_cosq_global_voq_info_t * info,
    uint32 *is_set)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_COSQ_GLOBAL_VOQ_FEATURE_VERIFY(unit);

    /** verify pointers */
    SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");
    SHR_NULL_CHECK(is_set, _SHR_E_PARAM, "is_set");

    /** flags param is not used */
    SHR_MASK_VERIFY(flags, 0, _SHR_E_PARAM, "provided unsupported flags\n");

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_cosq_global_voq_port_get(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    bcm_cosq_global_voq_info_t * info,
    uint32 *is_set)
{
    bcm_core_t core;
    uint32 out_tm_port;
    uint32 base_offset;
    uint32 tc_profile_index, bin_profile_index;

    SHR_FUNC_INIT_VARS(unit);

    info->tc_profile = 0;
    info->bin_profile = 0;

    SHR_INVOKE_VERIFY_DNXC(dnx_cosq_global_voq_port_get_verify(unit, flags, gport, info, is_set));

    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_out_tm_port_get(unit, gport, &core, &out_tm_port));

    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_port_hw_get(unit, core, out_tm_port, &tc_profile_index,
                                                    &bin_profile_index, &base_offset, is_set));

    if (*is_set)
    {
        BCM_GPORT_GLOBAL_VOQ_BIN_PROFILE_SET(info->bin_profile, bin_profile_index);
        BCM_GPORT_GLOBAL_VOQ_BIN_PROFILE_SET(info->tc_profile, tc_profile_index);
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_control_tc_profile_hw_set(
    int unit,
    uint32 tc_profile_index,
    uint32 tc_index,
    uint32 base_offset)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GLOBAL_VOQ_TC_MAP, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC_PROFILE, tc_profile_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC_INDEX, tc_index);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BASE_OFFSET, INST_SINGLE, base_offset);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_control_tc_profile_hw_get(
    int unit,
    uint32 tc_profile_index,
    uint32 tc_index,
    uint32 *base_offset)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GLOBAL_VOQ_TC_MAP, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC_PROFILE, tc_profile_index);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC_INDEX, tc_index);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_BASE_OFFSET, INST_SINGLE, (uint32 *) base_offset);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_tc_profile_set_verify(
    int unit,
    bcm_gport_t tc_profile,
    int count,
    bcm_cos_t * priority,
    bcm_cos_queue_t * cosq)
{
    int tc_profile_index;
    int i, is_suspended;
    bcm_core_t core;

    SHR_FUNC_INIT_VARS(unit);

    DNX_COSQ_GLOBAL_VOQ_FEATURE_VERIFY(unit);

    SHR_NULL_CHECK(priority, _SHR_E_PARAM, "priority");
    SHR_NULL_CHECK(cosq, _SHR_E_PARAM, "cosq");

    /** verify count */
    SHR_RANGE_VERIFY(count, 1, DNX_COSQ_NOF_TC, _SHR_E_PARAM, "Invalid count %d\n", count);

    /*
     * verify tc profile value
     */
    tc_profile_index = BCM_GPORT_GLOBAL_VOQ_TC_PROFILE_GET(tc_profile);

    /** tc profile 0 is reserved */
    SHR_RANGE_VERIFY(tc_profile_index, 1, dnx_data_global_voq.general.tc_profile_nof_get(unit) - 1,
                     _SHR_E_PARAM, "Invalid tc_profile %d\n", tc_profile_index);

    /** make sure the TC profile is not used */
    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        uint32 tc_profile_cnt;

        SHR_IF_ERR_EXIT(dnx_cosq_global_voq_db.tc_profile_cnt.get(unit, core, tc_profile_index, &tc_profile_cnt));

        SHR_VAL_VERIFY(tc_profile_cnt, 0, _SHR_E_PARAM,
                       "Configuring a TC profile while it is in use is not permitted\n");
    }

    /*
     * verify array contents
     */
    for (i = 0; i < count; ++i)
    {
        SHR_RANGE_VERIFY(priority[i], BCM_COS_MIN, BCM_COS_MAX, _SHR_E_PARAM, "Invalid priority %d\n", priority[i]);

        SHR_RANGE_VERIFY(cosq[i], BCM_COS_MIN, BCM_COS_MAX, _SHR_E_PARAM, "Invalid cosq %d\n", cosq[i]);
    }

    /** verify that the Global VOQ CNI/Drop enforcement is not suppressed */
    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_db.suspend_db.is_suspended.get(unit, &is_suspended));
    if (is_suspended)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "device is in suspended mode, modifications are not allowed\n");
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_cosq_global_tc_profile_set(
    int unit,
    bcm_gport_t tc_profile,
    int count,
    bcm_cos_t * priority,
    bcm_cos_queue_t * cosq)
{
    int arr_index;
    uint32 tc_profile_index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_cosq_global_tc_profile_set_verify(unit, tc_profile, count, priority, cosq));

    tc_profile_index = BCM_GPORT_GLOBAL_VOQ_TC_PROFILE_GET(tc_profile);

    /** User over-writes profiles explicitly */
    for (arr_index = 0; arr_index < count; ++arr_index)
    {
        int tc_index = priority[arr_index];
        int base_offset = cosq[arr_index];

        SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_tc_profile_hw_set(unit, tc_profile_index, tc_index, base_offset));
    }

exit:
    SHR_FUNC_EXIT;

}

static shr_error_e
dnx_cosq_global_tc_profile_get_verify(
    int unit,
    bcm_gport_t tc_profile,
    int count,
    bcm_cos_t * priority,
    bcm_cos_queue_t * cosq)
{

    SHR_FUNC_INIT_VARS(unit);

    DNX_COSQ_GLOBAL_VOQ_FEATURE_VERIFY(unit);

    SHR_NULL_CHECK(priority, _SHR_E_PARAM, "priority");
    SHR_NULL_CHECK(cosq, _SHR_E_PARAM, "cosq");

    /** verify count */
    SHR_RANGE_VERIFY(count, 1, DNX_COSQ_NOF_TC, _SHR_E_PARAM, "Invalid count %d\n", count);

    /*
     * verify tc profile value
     */
    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_tc_profile_verify(unit, tc_profile));

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_cosq_global_tc_profile_get(
    int unit,
    bcm_gport_t tc_profile,
    int count,
    bcm_cos_t * priority,
    bcm_cos_queue_t * cosq)
{
    int arr_index;
    uint32 tc_profile_index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_cosq_global_tc_profile_get_verify(unit, tc_profile, count, priority, cosq));

    tc_profile_index = BCM_GPORT_GLOBAL_VOQ_TC_PROFILE_GET(tc_profile);

    for (arr_index = 0; arr_index < count; ++arr_index)
    {
        uint32 base_offset;
        uint32 tc_index = priority[arr_index];

        SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_tc_profile_hw_get(unit, tc_profile_index, tc_index, &base_offset));

        cosq[arr_index] = base_offset;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_control_bin_profile_verify(
    int unit,
    bcm_gport_t bin_profile,
    bcm_cos_queue_t bin_index,
    bcm_cosq_control_t type,
    uint32 threshold)
{
    int bin_profile_index;
    int bin_index_nof = dnx_data_global_voq.general.bin_index_nof_get(unit);
    int bin_profile_nof = dnx_data_global_voq.general.bin_profile_nof_get(unit);
    int max_threshold = dnx_data_global_voq.general.max_bin_threshold_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    DNX_COSQ_GLOBAL_VOQ_FEATURE_VERIFY(unit);

    /** verify bin_profile */
    bin_profile_index = BCM_GPORT_GLOBAL_VOQ_BIN_PROFILE_GET(bin_profile);
    SHR_RANGE_VERIFY(bin_profile_index, 0, bin_profile_nof - 1, _SHR_E_PARAM, "Invalid bin_profile %d\n",
                     bin_profile_index);

    /** verify bin_index */
    SHR_RANGE_VERIFY(bin_index, 0, bin_index_nof - 1, _SHR_E_PARAM, "Invalid bin_index %d\n", bin_index);

    /** verify threshold */
    SHR_RANGE_VERIFY((int) threshold, 0, max_threshold, _SHR_E_PARAM, "Invalid threshold %d\n", ((int) threshold));

    SHR_VAL_VERIFY(type, bcmCosqControlGlobalVoqBinThreshold, _SHR_E_UNAVAIL, "unsupported control type\n");

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_control_bin_profile_hw_set(
    int unit,
    uint32 bin_profile,
    uint32 bin_index,
    uint32 threshold)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GLOBAL_VOQ_BIN_MAP, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BIN_PROFILE, bin_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BIN_INDEX, bin_index);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BIN_UPPER, INST_SINGLE, threshold);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_cosq_global_voq_control_bin_profile_set(
    int unit,
    bcm_gport_t bin_profile,
    bcm_cos_queue_t bin_index,
    bcm_cosq_control_t type,
    uint32 threshold)
{
    uint32 bin_profile_index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_cosq_global_voq_control_bin_profile_verify
                           (unit, bin_profile, bin_index, type, threshold));

    bin_profile_index = BCM_GPORT_GLOBAL_VOQ_BIN_PROFILE_GET(bin_profile);

    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_bin_profile_hw_set(unit, bin_profile_index, bin_index, threshold));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_control_bin_profile_get_verify(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t bin_index,
    bcm_cosq_control_t type,
    uint32 *threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(threshold, _SHR_E_PARAM, "threshold");

    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_bin_profile_verify(unit, gport, bin_index, type, 0));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_control_bin_profile_hw_get(
    int unit,
    uint32 bin_profile,
    uint32 bin_index,
    uint32 *threshold)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_GLOBAL_VOQ_BIN_MAP, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BIN_PROFILE, bin_profile);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_BIN_INDEX, bin_index);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_BIN_UPPER, INST_SINGLE, (uint32 *) threshold);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_cosq_global_voq_control_bin_profile_get(
    int unit,
    bcm_gport_t bin_profile,
    bcm_cos_queue_t bin_index,
    bcm_cosq_control_t type,
    uint32 *threshold)
{
    uint32 bin_profile_index;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_cosq_global_voq_control_bin_profile_get_verify
                           (unit, bin_profile, bin_index, type, threshold));

    bin_profile_index = BCM_GPORT_GLOBAL_VOQ_BIN_PROFILE_GET(bin_profile);

    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_control_bin_profile_hw_get(unit, bin_profile_index, bin_index, threshold));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_profile_table_get(
    int unit,
    int is_cni,
    int is_yellow,
    dbal_tables_e * table_id)
{
    SHR_FUNC_INIT_VARS(unit);

    if (is_cni)
    {
        *table_id = DBAL_TABLE_GLOBAL_VOQ_CNI_PROFILE;
    }
    else
    {
        if (is_yellow)
        {
            *table_id = DBAL_TABLE_GLOBAL_VOQ_DROP_YELLOW_PROFILE;
        }
        else
        {
            *table_id = DBAL_TABLE_GLOBAL_VOQ_DROP_GREEN_PROFILE;

        }
    }

    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_profile_hw_get(
    int unit,
    int profile_index,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint32 *arg)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, profile_index);

    dbal_value_field32_request(unit, entry_handle_id, field_id, INST_SINGLE, (uint32 *) arg);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_profile_hw_set(
    int unit,
    int profile_index,
    dbal_tables_e table_id,
    dbal_fields_e field_id,
    uint32 arg)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PROFILE_ID, profile_index);

    dbal_entry_value_field32_set(unit, entry_handle_id, field_id, INST_SINGLE, arg);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_profile_set_verify(
    int unit,
    bcm_gport_t port,
    bcm_cosq_control_t type,
    int arg)
{
    uint32 is_enabled;
    uint32 aqm_profile_index;

    SHR_FUNC_INIT_VARS(unit);

    DNX_COSQ_GLOBAL_VOQ_FEATURE_VERIFY(unit);

    aqm_profile_index = BCM_GPORT_AQM_PROFILE_GET(port);

    switch (type)
    {
        case bcmCosqControlGlobalVoqEcnEnable:
        case bcmCosqControlGlobalVoqDropGreenEnable:
        case bcmCosqControlGlobalVoqDropYellowEnable:
        case bcmCosqControlGlobalVoqEcnThreshold:
        case bcmCosqControlGlobalVoqDropGreenThreshold:
        case bcmCosqControlGlobalVoqDropYellowThreshold:
            /** nothing to verify */
            break;
        case bcmCosqControlGlobalVoqEcnProbEnable:
            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_prob_is_enable_get
                            (unit, aqm_profile_index, DBAL_TABLE_GLOBAL_VOQ_CNI_PROFILE, &is_enabled));

            if (!is_enabled && arg == TRUE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "type=%d not supported - ecn marking mechanism have to be enabled for this profile \n",
                             type);
            }
            break;
        case bcmCosqControlGlobalVoqDropProbGreenEnable:
            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_prob_is_enable_get
                            (unit, aqm_profile_index, DBAL_TABLE_GLOBAL_VOQ_DROP_GREEN_PROFILE, &is_enabled));

            if (!is_enabled && arg == TRUE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "type=%d not supported - drop-green mechanism have to be enabled for this profile \n",
                             type);
            }
            break;
        case bcmCosqControlGlobalVoqDropProbYellowEnable:
            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_prob_is_enable_get
                            (unit, aqm_profile_index, DBAL_TABLE_GLOBAL_VOQ_DROP_YELLOW_PROFILE, &is_enabled));

            if (!is_enabled && arg == TRUE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "type=%d not supported - drop-yellow mechanism have to be enabled for this profile \n",
                             type);
            }

            break;
        case bcmCosqControlGlobalVoqEcnProbThreshold:
            /*
             * check if MarkCNI Probability is enabled - if it is -
             * must update the alpha value for the formula calculation
             */
            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_prob_is_enable_get
                            (unit, aqm_profile_index, DBAL_TABLE_GLOBAL_VOQ_CNI_PROFILE, &is_enabled));

            if (!is_enabled)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "type=%d not supported - cni marking probability mechanism is not enabled for this profile \n",
                             type);
            }
            break;
        case bcmCosqControlGlobalVoqDropProbGreenThreshold:
            /*
             * check if Probability is enabled - if it is -
             * must update the alpha value for the formula calculation
             */
            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_prob_is_enable_get
                            (unit, aqm_profile_index, DBAL_TABLE_GLOBAL_VOQ_DROP_GREEN_PROFILE, &is_enabled));

            if (!is_enabled)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "type=%d not supported - drop-green probability mechanism is not enabled for this profile \n",
                             type);
            }
            break;
        case bcmCosqControlGlobalVoqDropProbYellowThreshold:
            /*
             * check if Probability is enabled - if it is -
             * must update the alpha value for the formula calculation
             */
            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_prob_is_enable_get
                            (unit, aqm_profile_index, DBAL_TABLE_GLOBAL_VOQ_DROP_YELLOW_PROFILE, &is_enabled));

            if (!is_enabled)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "type=%d not supported - drop-yellow probability mechanism is not enabled for this profile \n",
                             type);
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "type=%d not supported for an AQM profile \n", type);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_profile_get_verify(
    int unit,
    bcm_gport_t port,
    bcm_cosq_control_t type,
    int *arg)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_COSQ_GLOBAL_VOQ_FEATURE_VERIFY(unit);

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief
*  function to calculate the new alpha value for the
*  probabilistic EGRESS aqm based marking cni/drop
*  mechanism
*
* \param [in] unit -unit id
* \param [in] profile_index - profile_id
* \param [in] table_id - table corresponding to the mechanism,
* \param [in] is_min_changed - min threshold or max threshold is
*        given
* \param [in] threshold_value - the threshold value that was
*        updated from user with API
* \param [out] shift_value - the new calculated shift value to
*        be set together with the new threshold value
* \return
*    shr_error_e
* \see
*   NONE
*/
static shr_error_e
dnx_cosq_global_voq_probability_marking_alpha_value_get(
    int unit,
    int profile_index,
    dbal_tables_e table_id,
    int is_min_changed,
    int threshold_value,
    uint32 *shift_value)
{
    uint32 threshold_value_get = 0;
    uint64 delta_value = COMPILER_64_INIT(0, 0);
    dbal_fields_e min_max_field_id;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * read from HW the other value - if user updates min, need
     * to get from HW the max threshold according to packet type
     */
    min_max_field_id = is_min_changed ? DBAL_FIELD_MIN_THRESH : DBAL_FIELD_MAX_THRESH;

    /** get the current threshold value */
    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_profile_hw_get
                    (unit, profile_index, table_id, min_max_field_id, &threshold_value_get));

    /*
     * The following clause implements:
     *   delta_value = (is_min_changed) ? (threshold_value_get - threshold_value) : (threshold_value - threshold_value_get);
     */
    if (is_min_changed)
    {
        COMPILER_64_SET(delta_value, 0, (threshold_value_get - threshold_value));
    }
    else
    {
        COMPILER_64_SET(delta_value, 0, (threshold_value - threshold_value_get));
    }

    /** calculate according to the formula */
    dnx_cosq_probability_marking_alpha_value_calculation(unit, delta_value, shift_value);

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_prob_is_enable_get(
    int unit,
    int profile_id,
    dbal_tables_e table_id,
    uint32 *is_enable)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_profile_hw_get(unit, profile_id, table_id, DBAL_FIELD_PROB_ENABLE, is_enable));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_probability_marking_get(
    int unit,
    int profile_id,
    bcm_cosq_control_t type,
    int is_cni,
    int is_yellow,
    uint32 *arg)
{
    dbal_tables_e table_id;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_profile_table_get(unit, is_cni, is_yellow, &table_id));

    switch (type)
    {
        case bcmCosqControlGlobalVoqEcnEnable:
        case bcmCosqControlGlobalVoqDropGreenEnable:
        case bcmCosqControlGlobalVoqDropYellowEnable:
            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_profile_hw_get(unit, profile_id, table_id, DBAL_FIELD_MAX_ENABLE, arg));
            break;
        case bcmCosqControlGlobalVoqEcnThreshold:
        case bcmCosqControlGlobalVoqDropGreenThreshold:
        case bcmCosqControlGlobalVoqDropYellowThreshold:
            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_profile_hw_get(unit, profile_id, table_id, DBAL_FIELD_MAX_THRESH, arg));

            break;
        case bcmCosqControlGlobalVoqEcnProbEnable:
        case bcmCosqControlGlobalVoqDropProbGreenEnable:
        case bcmCosqControlGlobalVoqDropProbYellowEnable:
            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_profile_hw_get
                            (unit, profile_id, table_id, DBAL_FIELD_PROB_ENABLE, arg));

            break;
        case bcmCosqControlGlobalVoqEcnProbThreshold:
        case bcmCosqControlGlobalVoqDropProbYellowThreshold:
        case bcmCosqControlGlobalVoqDropProbGreenThreshold:
            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_profile_hw_get(unit, profile_id, table_id, DBAL_FIELD_MIN_THRESH, arg));

            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "type=%d not supported for an AQM profile \n", type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_cosq_global_voq_probability_marking_set(
    int unit,
    int profile_id,
    bcm_cosq_control_t type,
    int is_cni,
    int is_yellow,
    uint32 arg)
{
    uint32 is_enabled, shift_value;
    dbal_tables_e table_id;

    const uint32 MIN_CHANGED = 1;
    const uint32 MAX_CHANGED = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_cosq_global_voq_profile_table_get(unit, is_cni, is_yellow, &table_id));

    switch (type)
    {
        case bcmCosqControlGlobalVoqEcnEnable:
        case bcmCosqControlGlobalVoqDropGreenEnable:
        case bcmCosqControlGlobalVoqDropYellowEnable:
            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_profile_hw_set(unit, profile_id, table_id, DBAL_FIELD_MAX_ENABLE, arg));
            break;
        case bcmCosqControlGlobalVoqEcnThreshold:
        case bcmCosqControlGlobalVoqDropGreenThreshold:
        case bcmCosqControlGlobalVoqDropYellowThreshold:
            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_profile_hw_set(unit, profile_id, table_id, DBAL_FIELD_MAX_THRESH, arg));

            /** check if MarkCNI Probability is enabled - if it is - must
             *  update the alpha value for the formula calculation */
            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_prob_is_enable_get(unit, profile_id, table_id, &is_enabled));

            if (is_enabled)
            {
                SHR_IF_ERR_EXIT(dnx_cosq_global_voq_probability_marking_alpha_value_get
                                (unit, profile_id, table_id, MAX_CHANGED, arg, &shift_value));

                SHR_IF_ERR_EXIT(dnx_cosq_global_voq_profile_hw_set
                                (unit, profile_id, table_id, DBAL_FIELD_MIN_THRESH_SHIFT, shift_value));
            }
            break;
        case bcmCosqControlGlobalVoqEcnProbEnable:
        case bcmCosqControlGlobalVoqDropProbGreenEnable:
        case bcmCosqControlGlobalVoqDropProbYellowEnable:
            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_profile_hw_set
                            (unit, profile_id, table_id, DBAL_FIELD_PROB_ENABLE, arg));

            break;
        case bcmCosqControlGlobalVoqEcnProbThreshold:
        case bcmCosqControlGlobalVoqDropProbYellowThreshold:
        case bcmCosqControlGlobalVoqDropProbGreenThreshold:

            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_profile_hw_set(unit, profile_id, table_id, DBAL_FIELD_MIN_THRESH, arg));

            /*
             * check if MarkCNI Probability is enabled - if it is -
             * must update the alpha value for the formula calculation
             */
            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_prob_is_enable_get(unit, profile_id, table_id, &is_enabled));

            if (is_enabled)
            {
                SHR_IF_ERR_EXIT(dnx_cosq_global_voq_probability_marking_alpha_value_get
                                (unit, profile_id, table_id, MIN_CHANGED, arg, &shift_value));

                SHR_IF_ERR_EXIT(dnx_cosq_global_voq_profile_hw_set
                                (unit, profile_id, table_id, DBAL_FIELD_MIN_THRESH_SHIFT, shift_value));
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "type=%d not supported for an AQM profile \n", type);
            break;
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_cosq_global_voq_profile_set(
    int unit,
    bcm_gport_t port,
    bcm_cosq_control_t type,
    int arg)
{
    uint32 aqm_profile_index;
    const uint32 CNI = 1;
    const uint32 DROP = 0;
    const uint32 YELLOW = 1;
    const uint32 GREEN = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_cosq_global_voq_profile_set_verify(unit, port, type, arg));

    aqm_profile_index = BCM_GPORT_AQM_PROFILE_GET(port);

    switch (type)
    {
        case bcmCosqControlGlobalVoqDropProbYellowEnable:
        case bcmCosqControlGlobalVoqDropProbYellowThreshold:
        case bcmCosqControlGlobalVoqDropYellowEnable:
        case bcmCosqControlGlobalVoqDropYellowThreshold:
            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_probability_marking_set
                            (unit, aqm_profile_index, type, DROP, YELLOW, arg));
            break;
        case bcmCosqControlGlobalVoqDropProbGreenEnable:
        case bcmCosqControlGlobalVoqDropProbGreenThreshold:
        case bcmCosqControlGlobalVoqDropGreenEnable:
        case bcmCosqControlGlobalVoqDropGreenThreshold:
            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_probability_marking_set
                            (unit, aqm_profile_index, type, DROP, GREEN, arg));
            break;
        case bcmCosqControlGlobalVoqEcnProbEnable:
        case bcmCosqControlGlobalVoqEcnProbThreshold:
        case bcmCosqControlGlobalVoqEcnEnable:
        case bcmCosqControlGlobalVoqEcnThreshold:
            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_probability_marking_set(unit, aqm_profile_index, type, CNI, 0, arg));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "type=%d not supported for an AQM profile \n", type);
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_cosq_global_voq_profile_get(
    int unit,
    bcm_gport_t port,
    bcm_cosq_control_t type,
    int *arg)
{
    uint32 aqm_profile_index;

    const uint32 CNI = 1;
    const uint32 DROP = 0;
    const uint32 YELLOW = 1;
    const uint32 GREEN = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_INVOKE_VERIFY_DNXC(dnx_cosq_global_voq_profile_get_verify(unit, port, type, arg));

    aqm_profile_index = BCM_GPORT_AQM_PROFILE_GET(port);

    switch (type)
    {
        case bcmCosqControlGlobalVoqDropProbYellowEnable:
        case bcmCosqControlGlobalVoqDropProbYellowThreshold:
        case bcmCosqControlGlobalVoqDropYellowEnable:
        case bcmCosqControlGlobalVoqDropYellowThreshold:
            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_probability_marking_get
                            (unit, aqm_profile_index, type, DROP, YELLOW, (uint32 *) arg));
            break;
        case bcmCosqControlGlobalVoqDropProbGreenEnable:
        case bcmCosqControlGlobalVoqDropProbGreenThreshold:
        case bcmCosqControlGlobalVoqDropGreenEnable:
        case bcmCosqControlGlobalVoqDropGreenThreshold:
            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_probability_marking_get
                            (unit, aqm_profile_index, type, DROP, GREEN, (uint32 *) arg));
            break;
        case bcmCosqControlGlobalVoqEcnProbEnable:
        case bcmCosqControlGlobalVoqEcnProbThreshold:
        case bcmCosqControlGlobalVoqEcnEnable:
        case bcmCosqControlGlobalVoqEcnThreshold:
            SHR_IF_ERR_EXIT(dnx_cosq_global_voq_probability_marking_get
                            (unit, aqm_profile_index, type, CNI, 0, (uint32 *) arg));
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "type=%d not supported for an AQM profile \n", type);
    }

exit:
    SHR_FUNC_EXIT;
}

/** see .h file */
shr_error_e
dnx_cosq_global_voq_is_global_voq_config_profile_type(
    int unit,
    bcm_cosq_control_t type,
    int *is_global_voq_type)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (type)
    {
        case bcmCosqControlGlobalVoqDropProbYellowEnable:
        case bcmCosqControlGlobalVoqDropProbYellowThreshold:
        case bcmCosqControlGlobalVoqDropYellowEnable:
        case bcmCosqControlGlobalVoqDropYellowThreshold:
        case bcmCosqControlGlobalVoqDropProbGreenEnable:
        case bcmCosqControlGlobalVoqDropProbGreenThreshold:
        case bcmCosqControlGlobalVoqDropGreenEnable:
        case bcmCosqControlGlobalVoqDropGreenThreshold:
        case bcmCosqControlGlobalVoqEcnProbEnable:
        case bcmCosqControlGlobalVoqEcnProbThreshold:
        case bcmCosqControlGlobalVoqEcnEnable:
        case bcmCosqControlGlobalVoqEcnThreshold:
            *is_global_voq_type = TRUE;
            break;
        default:
            *is_global_voq_type = FALSE;
            break;
    }

    SHR_FUNC_EXIT;
}
