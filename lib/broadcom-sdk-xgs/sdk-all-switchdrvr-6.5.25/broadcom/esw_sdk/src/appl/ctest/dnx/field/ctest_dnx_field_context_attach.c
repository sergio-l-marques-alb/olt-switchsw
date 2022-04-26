
/** \file ctest_dnx_field_context_attach.c
 *
 * test performance of bcm_field_group_context_attach() API
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
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDTESTSDNX

 /**
  * Include files.
  * {
  */
#include <soc/dnxc/swstate/sw_state_defs.h>
/** shared */
#include <shared/shrextend/shrextend_debug.h>
/** appl */
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <bcm_int/dnx/field/field.h>
#include <bcm_int/dnx/field/field_group.h>
#include <bcm_int/dnx/field/field_presel.h>
#include <bcm_int/dnx/field/field_context.h>
#include <bcm_int/dnx/field/field_map.h>
#include <bcm_int/dnx/field/field_init.h>
#include "ctest_dnx_field_context_attach.h"
#include "ctest_dnx_field_utils.h"
#include <appl/diag/dnx/diag_dnx_field.h>

/*
 * }
 */

/* *INDENT-OFF* */

/**
 * \brief
 *   Keyword for test type:
 *   Type can be either BCM or DNX
 */
#define DNX_CTEST_FIELD_ATTACH_NOF_TIMES        "nof_times"
#define DNX_CTEST_FIELD_ATTACH_NOF_QUALS         "nof_quals"
#define DNX_CTEST_FIELD_ATTACH_NOF_ACTIONS     "nof_actions"
#define DNX_CTEST_FIELD_ATTACH_EXPECTED_RATE    "Expected_Time"
/**
 * \brief
 *   Options list for 'context_attach' shell command
 * \remark
 */
sh_sand_option_t Sh_dnx_field_context_attach_options[] = {
     /* Name */                                 /* Type */              /* Description */                                 /* Default */

    {DNX_CTEST_FIELD_ATTACH_NOF_TIMES,          SAL_FIELD_TYPE_UINT32,  "Nof Times",                                    "80"},
    {DNX_CTEST_FIELD_ATTACH_NOF_QUALS,           SAL_FIELD_TYPE_UINT32,  "Nof quals",                                   "5"},
    {DNX_CTEST_FIELD_ATTACH_NOF_ACTIONS,       SAL_FIELD_TYPE_UINT32,  "Nof_actions",                                  "5"},
    {DNX_CTEST_FIELD_ATTACH_EXPECTED_RATE,      SAL_FIELD_TYPE_UINT32,  "Expected_Time",                                "4900000"},
    {NULL}      /* End of options list - must be last. */
};
/**
 * \brief
 *   List of tests for 'context_attach' shell command (to be run on regression, precommit, etc.)
 * \remark
 *   Currently, it will be executed only for regression and precommit.
 */
sh_sand_invoke_t Sh_dnx_field_context_attach_tests[] = {
    {"BCM_fld_context_attach_1", "nof_times=10 nof_actions=5 nof_quals=5 Expected_Time=4900000", CTEST_POSTCOMMIT},
    {NULL}
};
/* *INDENT-ON* */
/**
 *  context shell command leaf details
 */
sh_sand_man_t Sh_dnx_field_context_attach_man = {
    "Field context attach/detach perfromance measure",
    "ctest field attach nof_times=10 nof_actions=[0,5] nof_quals=[0,5] Expected_Time=4500000"
};

/**

 *   This function is the starter for tcam basic test.
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'bcm_error_e').
 * \see
 *   bcm_error_e
 */
static shr_error_e
ctest_dnx_field_context_attach_run(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    shr_error_e rv = _SHR_E_NONE;
    int nof_times;
    bcm_field_group_t fg_id;
    int nof_quals = 3;
    int nof_actions = 2;
    bcm_field_group_info_t fg_info;
    uint32 timers_group = UTILEX_LL_TIMER_NO_GROUP;
    char group_name[UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME];
    int start_time = 0;
    int run_time = 0;
    int expected_time;
    bcm_field_group_attach_info_t attach_info;
    int ii = 0, iter_times;
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get the inputs from Shell
     */
    SH_SAND_GET_UINT32(DNX_CTEST_FIELD_ATTACH_NOF_TIMES, nof_times);
    SH_SAND_GET_UINT32(DNX_CTEST_FIELD_ATTACH_NOF_QUALS, nof_quals);
    SH_SAND_GET_UINT32(DNX_CTEST_FIELD_ATTACH_NOF_ACTIONS, nof_actions);
    SH_SAND_GET_UINT32(DNX_CTEST_FIELD_ATTACH_EXPECTED_RATE, expected_time);

    if (nof_quals > 5 || nof_quals < 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illigal input nof_quals %d , should be between 1 and 5 \n", nof_quals);
    }

    if (nof_actions > 5 || nof_actions < 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Illigal input nof_actions %d , should be between 1 and 5 \n", nof_actions);
    }

    sal_strncpy(group_name, "FG_att performance", UTILEX_LL_TIMER_MAX_NOF_CHARS_IN_TIMER_GROUP_NAME - 1);
    /** Allocate timer group ID with a given name*/
    rv = utilex_ll_timer_group_allocate(group_name, &timers_group);
    if (rv != _SHR_E_NONE || timers_group == UTILEX_LL_TIMER_NO_GROUP)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Allocation of group has failed with error code %d.\n\r", rv);
    }
    utilex_ll_timer_clear_all(timers_group);

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF2;

    fg_info.nof_quals = nof_quals;
    fg_info.qual_types[0] = bcmFieldQualifySrcIp;
    fg_info.qual_types[1] = bcmFieldQualifyLayerRecordType;
    fg_info.qual_types[2] = bcmFieldQualifyForwardingLayerIndex;
    fg_info.qual_types[3] = bcmFieldQualifyColorRaw;
    fg_info.qual_types[4] = bcmFieldQualifyRxTrapCode;

    /*
     * Set actions
     */
    fg_info.nof_actions = nof_actions;

    fg_info.action_types[0] = bcmFieldActionOamRaw;
    fg_info.action_types[1] = bcmFieldActionSnoopRaw;
    fg_info.action_types[2] = bcmFieldActionUDHBase0;
    fg_info.action_types[3] = bcmFieldActionUDHBase1;
    fg_info.action_types[4] = bcmFieldActionUDHBase2;

    sal_strncpy_s((char *) (fg_info.name), "FG_Performance", sizeof(fg_info.name));

    bcm_field_group_attach_info_t_init(&attach_info);

    attach_info.key_info.nof_quals = fg_info.nof_quals;
    attach_info.payload_info.nof_actions = fg_info.nof_actions;

    for (ii = 0; ii < fg_info.nof_quals; ii++)
    {
        attach_info.key_info.qual_types[ii] = fg_info.qual_types[ii];
    }
    for (ii = 0; ii < fg_info.nof_actions; ii++)
    {
        attach_info.payload_info.action_types[ii] = fg_info.action_types[ii];
    }

    attach_info.key_info.qual_info[0].input_type = bcmFieldInputTypeLayerAbsolute;
    attach_info.key_info.qual_info[0].input_arg = 1;
    attach_info.key_info.qual_info[0].offset = 0;
    attach_info.key_info.qual_info[1].input_type = bcmFieldInputTypeLayerRecordsFwd;
    attach_info.key_info.qual_info[1].input_arg = 2;
    attach_info.key_info.qual_info[1].offset = 0;
    attach_info.key_info.qual_info[2].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[3].input_type = bcmFieldInputTypeMetaData;
    attach_info.key_info.qual_info[4].input_type = bcmFieldInputTypeMetaData;

    start_time = sal_time_usecs();
    for (iter_times = 0; iter_times < nof_times; iter_times++)
    {

        utilex_ll_timer_set("bcm_field_group_add ", timers_group, 0);
        SHR_IF_ERR_EXIT(bcm_field_group_add(unit, 0, &fg_info, &fg_id));
        utilex_ll_timer_stop(timers_group, 0);

        utilex_ll_timer_set("bcm_field_group_context_attach", timers_group, 1);
        SHR_IF_ERR_EXIT(bcm_field_group_context_attach(unit, 0, fg_id, 0, &attach_info));
        utilex_ll_timer_stop(timers_group, 1);

        utilex_ll_timer_set("bcm_field_group_context_detach", timers_group, 2);
        SHR_IF_ERR_EXIT(bcm_field_group_context_detach(unit, fg_id, 0));
        utilex_ll_timer_stop(timers_group, 2);
        utilex_ll_timer_set("bcm_field_group_delete", timers_group, 3);
        SHR_IF_ERR_EXIT(bcm_field_group_delete(unit, fg_id));
        utilex_ll_timer_stop(timers_group, 3);
    }
    run_time = sal_time_usecs() - start_time;

    utilex_ll_timer_stop_all(timers_group);
    utilex_ll_timer_print_all(timers_group);
    utilex_ll_timer_clear_all(timers_group);

    LOG_CLI((BSL_META("Runs (%d) Total time of group_context_attach APIs- %d(us)\n"), nof_times, run_time));
    if (expected_time < run_time)
    {

        SHR_ERR_EXIT(_SHR_E_FAIL, "\nRun time took loneger than Expected (%d),RunTime(%d) \n", expected_time, run_time);

    }

exit:
    SHR_IF_ERR_CONT(utilex_ll_timer_group_free(timers_group));
    SHR_FUNC_EXIT;
}

/**
 * \brief - run context init sequence in diag shell
 *
 * \param [in] unit - the unit number in system
 * \param [in] args - pointer to args_t, not passed down
 * \param [in,out] sand_control - pointer to sh_sand_control_t, not
 *  passed down
 *
 *  Usage: run in BCM shell "dnx context_start"
 */
shr_error_e
sh_dnx_field_context_attach_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(ctest_dnx_field_context_attach_run(unit, args, sand_control));

exit:
    SHR_FUNC_EXIT;
}
