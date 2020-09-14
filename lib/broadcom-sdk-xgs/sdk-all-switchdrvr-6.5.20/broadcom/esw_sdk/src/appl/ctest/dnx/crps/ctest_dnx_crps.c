/** \file ctest_dnx_crps.c
 *
 * Main ctests for crps
 *
 */
/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_CNT

/*************
 * INCLUDES  *
 *************/
/** appl */
#include <appl/diag/sand/diag_sand_prt.h>
/** bcm */
#include <bcm_int/dnx/algo/crps/crps_ctrs_db.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <src/bcm/dnx/stat/crps/crps_mgmt_internal.h>
#include <src/bcm/dnx/stat/crps/crps_engine.h>
/** sal */

static shr_error_e
dnx_set_counter_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 engine_id;
    int core_id, core_idx;
    uint32 sw_counter_id;
    uint32 *value_arr_ptr;
    uint64 value64;
    int lock_taken = FALSE;
    const dnx_data_crps_engine_engines_info_t *engine_info;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("engine", engine_id);
    SH_SAND_GET_UINT32("counter", sw_counter_id);
    SH_SAND_GET_ARRAY32("value", value_arr_ptr);
    SH_SAND_GET_INT32("core", core_id);

    if (engine_id >= dnx_data_crps.engine.nof_engines_get(unit))
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "engine=%d, nof_engines=%d\n", engine_id,
                     dnx_data_crps.engine.nof_engines_get(unit));
    }
    engine_info = dnx_data_crps.engine.engines_info_get(unit, engine_id);
    if (sw_counter_id >= engine_info->nof_counters * DNX_CRPS_MGMT_SUB_COUNT)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "counter=%d, nof_counters=%d\n", sw_counter_id,
                     engine_info->nof_counters * DNX_CRPS_MGMT_SUB_COUNT);
    }
    COMPILER_64_SET(value64, value_arr_ptr[1], value_arr_ptr[0]);

    SHR_IF_ERR_EXIT(dnx_crps_counter_state_lock_take(unit));
    lock_taken = TRUE;
    DNXCMN_CORES_ITER(unit, core_id, core_idx)
    {
        SHR_IF_ERR_EXIT(dnx_crps_ctrs_db_counter_set
                        (unit, DNX_CRPS_CTRS_OVERWRITE_COUNTER_VALUE, core_idx, engine_id,
                         sw_counter_id / DNX_CRPS_MGMT_SUB_COUNT, (sw_counter_id & 0x1),
                         bcmStatCounterFormatPacketsAndBytes, value64));
    }

exit:
    if (lock_taken)
    {
        SHR_IF_ERR_EXIT(dnx_crps_counter_state_lock_give(unit));
    }
    SHR_FUNC_EXIT;
}

static sh_sand_option_t dnx_crps_set_list_options[] = {
    {"engine", SAL_FIELD_TYPE_UINT32, "engine_id", NULL},
    {"counter", SAL_FIELD_TYPE_UINT32, "sw counter index", NULL},
    {"value", SAL_FIELD_TYPE_ARRAY32, "uint64 counter value", NULL},
    {NULL}
};

static sh_sand_man_t dnx_crps_set_man = {
    .brief = "set a value for a specific sw counter.",
    .full = "'Set' command sets a uin64 counter value in specific sw counter"
        "SW holds 2 counters for each HW counter entry,"
        "therefore, parameter <counter> should not rise above (2*nof_hw_counters_entries_per_engine -1). \n"
        "(Note: If core wasn't given, 'Set' will be made for all cores)",
    .synopsis = NULL,
    .examples = "engine=2 counter=222 value=22222\n" "engine=12 counter=444 value=444"
};

static sh_sand_invoke_t dnx_crps_set_tests[] = {
    {"crps_set", "engine=2 counter=102 value=502"},
    {"crps_clear", "engine=2 counter=102 value=0"},
    {NULL}
};

/**
 * \brief DNX CRPS ctests
 * List of the supported ctests, pointer to command function and
 * command usage function. This is the entry point for CRPS 
 * ctest commands 
 */
sh_sand_cmd_t dnx_crps_test_cmds[] = {
    {"Set", dnx_set_counter_cmd, NULL, dnx_crps_set_list_options, &dnx_crps_set_man, NULL, dnx_crps_set_tests},
    {NULL}
};
