/** \file ctest_dnx_infra_seq_tests.c
 * 
 * Tests for utilex sequence mechanism
 * 
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/diag.h>
#include <appl/ctest/dnxc/ctest_dnxc_infra_init.h>
#include <appl/ctest/dnxc/ctest_dnxc_system.h>

#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_seq.h>
#include <shared/utilex/utilex_str.h>

#include <soc/dnxc/dnxc_ha.h>

#include <bcm_int/dnx/init/init.h>

#include <bcm/init.h>

extern const dnx_init_step_t dnx_init_deinit_seq[];

/** 
 *  \brief periodic event and stop\start operations
 *   */
static shr_error_e
diag_dnx_infra_init_error_recovery_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    utilex_seq_t seq;
    int steps_to_skip[] = { DNX_INIT_STEP_LINKSCAN };
    int nof_steps_to_skip;
    int first_step_id;
    int ii = 0;
    utilex_seq_err_recovery_test_t test_info;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("STEP", first_step_id);

    nof_steps_to_skip = sizeof(steps_to_skip) / sizeof(int);

    /** by default start from the beginning of the list */
    if (first_step_id == -1)
    {
        first_step_id = UTILEX_SEQ_STEP_INVALID;
    }

    /** Init seq structure */
    {
        SHR_CLI_EXIT_IF_ERR(utilex_seq_t_init(unit, &seq), "Error - utilex_seq_t_init failed!\n");

        /** Configure sequence */
        /** Logging */
        seq.bsl_flags = BSL_LOG_MODULE;
        seq.log_severity = bslSeverityInfo;

        /** Parital seq - if first step is given then test only this test */
        seq.first_step = UTILEX_SEQ_STEP_INVALID;
        if (first_step_id != UTILEX_SEQ_STEP_INVALID)
        {
            seq.last_step = first_step_id;
        }
        else
        {
            seq.last_step = UTILEX_SEQ_STEP_INVALID;
        }

        /** Seq utils */
        seq.time_log_en = 0;
        seq.time_test_en = 0;
        seq.time_thresh = BCM_INIT_TIME_THRESH_DEFAULT;
        seq.mem_log_en = 0;
        seq.mem_test_en = 0;
        seq.swstate_log_en = 0;
        seq.access_only = 0;
        seq.tmp_allow_access_enable = dnxc_ha_tmp_allow_access_enable;
        seq.tmp_allow_access_disable = dnxc_ha_tmp_allow_access_disable;
    }

    /** Convert dnx list to utilex list */
    SHR_CLI_EXIT_IF_ERR(dnx_init_step_list_convert(unit, dnx_init_deinit_seq, &seq.step_list),
                        "Error - dnx_init_step_list_convert failed!\n");

    /** deinit unit */
    SHR_CLI_EXIT_IF_ERR(sh_process_command(unit, "tr 141 noinit=1"), "Error - 'tr 141 noinit=1' failed!\n");

    /** attach unit since instead of running regular init seqeunce - dnx init seq is called directly */
    SHR_CLI_EXIT_IF_ERR(bcm_attach(unit, NULL, NULL, unit), "Error - bcm_attach failed!\n");

    /** run the test */
    SHR_CLI_EXIT_IF_ERR(utilex_seq_error_recovery_test_run
                        (unit, &seq, first_step_id, steps_to_skip, nof_steps_to_skip, &test_info),
                        "Error - Init sequence Error Recovery test failed!\n");

    /** init unit */
    SHR_CLI_EXIT_IF_ERR(sh_process_command(unit, "tr 141 nodeinit=1"), "Error - 'tr 141 nodeinit=1' failed!\n");

    /** print summary table of failed steps in the sequence */
    if (test_info.fail_count > 0)
    {
        BSL_LOG(BSL_LOG_MODULE, ("********************************************************************\n"));
        BSL_LOG(BSL_LOG_MODULE, ("Memory leak test failed!!! The following  %d steps failed\n", test_info.fail_count));
        BSL_LOG(BSL_LOG_MODULE, ("********************************************************************\n"));

        PRT_TITLE_SET("Failed steps summary");
        PRT_COLUMN_ADD("Step name");
        PRT_COLUMN_ADD("Step ID");
        PRT_COLUMN_ADD("Leak size [Bytes]");

        for (ii = 0; ii < test_info.fail_count; ii++)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("%s", test_info.fail_step_name[ii]);
            PRT_CELL_SET("%d", test_info.fail_step_id[ii]);
            PRT_CELL_SET("%d", test_info.fail_mem_size[ii]);
        }

        PRT_COMMIT;

        SHR_CLI_EXIT(_SHR_E_MEMORY, "Test Failed\n");
    }

    SHR_CLI_EXIT(_SHR_E_NONE, "Test PASS\n");

exit:
    SHR_IF_ERR_CONT(dnx_init_step_list_destory(unit, seq.step_list));
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 *  \brief periodic event and stop\start operations
 *   */
static shr_error_e
diag_dnx_infra_init_error_recovery_tests_creation(
    int unit,
    rhlist_t * test_list)
{
    utilex_seq_t seq;
    utilex_seq_step_t *step;
    int is_found;
    uint32 flags;
    char test_args[DNX_INIT_STEP_NAME_SIZE];
    char test_name[DNX_INIT_STEP_NAME_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    /** Init seq structure */
    SHR_CLI_EXIT_IF_ERR(utilex_seq_t_init(unit, &seq), "Error - utilex_seq_t_init failed!\n");

    /** Convert dnx list to utilex list */
    SHR_CLI_EXIT_IF_ERR(dnx_init_step_list_convert(unit, dnx_init_deinit_seq, &seq.step_list),
                        "Error - dnx_init_step_list_convert failed!\n");

    /** set first step */
    step = &(seq.step_list[0]);
    is_found = UTILEX_SEQ_NEXT_STEP_FOUND;
    if ((step->forward == NULL) && (step->backward == NULL))
    {
        /** find next active step */
        SHR_IF_ERR_EXIT(utilex_seq_next_active_step_get(unit, seq.step_list, step->step_id, &step, &is_found));
    }

    flags = CTEST_POSTCOMMIT;

    /** create tests list */
    while (is_found == UTILEX_SEQ_NEXT_STEP_FOUND)
    {
        sal_snprintf(test_args, DNX_INIT_STEP_NAME_SIZE, "STEP=%d", step->step_id);
        sal_snprintf(test_name, DNX_INIT_STEP_NAME_SIZE, "step_%s_id_%d", step->step_name, step->step_id);

        /** replace special chars with '_' char */
        utilex_str_escape(test_name, '_');

        /** add test to list */
        SHR_CLI_EXIT_IF_ERR(sh_sand_test_add(unit, test_list, test_name, test_args, flags), "Add test failed");

        /** find next active step */
        SHR_IF_ERR_EXIT(utilex_seq_next_active_step_get(unit, seq.step_list, step->step_id, &step, &is_found));
    }

exit:
    SHR_IF_ERR_CONT(dnx_init_step_list_destory(unit, seq.step_list));
    SHR_FUNC_EXIT;
}

static sh_sand_man_t diag_dnx_infra_init_error_recovery_test_man = {
    "Test error recovery during init sequence",
    "The test verifies each step on init sequence frees its memory",
    NULL,
    NULL,
};


static sh_sand_option_t diag_dnx_infra_init_error_recovery_test_options[] = {
    {"STEP", SAL_FIELD_TYPE_INT32, "BCM DNX init sequence first step ID to start testing from", "-1"},
    {NULL}
};

sh_sand_cmd_t dnx_infra_init_test_cmds[] = {
    {"ERRor_RECovery", diag_dnx_infra_init_error_recovery_test_cmd, NULL,
     diag_dnx_infra_init_error_recovery_test_options, &diag_dnx_infra_init_error_recovery_test_man,
     NULL, NULL, CTEST_POSTCOMMIT, diag_dnx_infra_init_error_recovery_tests_creation},

    {"PaRaLLel", sh_dnxc_infra_init_parallel_test_cmd, NULL,
     sh_dnxc_infra_init_parallel_test_options, &sh_dnxc_infra_init_parallel_test_man,
     NULL, sh_dnxc_infra_init_parallel_tests, CTEST_POSTCOMMIT},

    {NULL}
};
