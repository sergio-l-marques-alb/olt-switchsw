/** \file ctest_dnx_infra_seq_tests.c
 * 
 * Tests for utilex sequence mechanism
 * 
 */
/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

#include <shared/shrextend/shrextend_debug.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/diag.h>
#include <appl/reference/dnxc/appl_ref_init_deinit.h>
#include <soc/util.h>

#include <appl/ctest/dnxc/ctest_dnxc_infra_init.h>
#include <appl/ctest/dnxc/ctest_dnxc_system.h>
#include <appl/ctest/dnxc/ctest_dnxc_utils.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/utilex/utilex_seq.h>
#include <shared/utilex/utilex_str.h>
#include <soc/dnxc/dnxc_ha.h>

#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/dnxc/drv_dnxc_utils.h>

#include <soc/dnxc/dnxc_regression_utils.h>
#include <appl/reference/dnxc/appl_ref_init_utils.h>

#ifdef BCM_DNXF_SUPPORT
#include <bcm_int/dnxf/init.h>
#endif

#ifdef BCM_DNX_SUPPORT
#include <bcm_int/dnx/init/init.h>
#include <sal/core/sync.h>
#include <soc/dnx/dnx_er_threading.h>
#endif

/**
 * \brief - init sequence maximal stack size
 */
#define SH_DNXC_INIT_STEP_NAME_SIZE 256

/**
 * \brief - init sequence maximal stack size
 */
#define SH_DNXC_INIT_SEQ_STACK_SIZE (128 * 1024)

/**
 * \brief - Number of units in mgmt system
 */
#define SH_DNXC_MGMT_SYS_NOF_UNITS (3)

/**
 * \brief - return value of each init thread
 */
int init_thread_rv[BCM_LOCAL_UNITS_MAX];

/**
 * \brief - valid units bitmap
 */
uint32 valid_units_bmp[1];

/**
 * Indication for only multi threaded init (instead of full deinit-init multithreaded procedure)
 */
int is_partial = 0;

/**
 * Indication to perfrom only init seqeunce, meaning that the device is deinited
 */
int is_mgmt = 0;

/**
 *  \brief init sequence background thread
 *   */
void
sh_dnxc_infra_init_thread(
    void *unit_ptr)
{
    int unit = *(int *) (unit_ptr);
    appl_dnxc_init_param_t appl_params;

    sal_memset(&appl_params, 0x0, sizeof(appl_dnxc_init_param_t));

    /** if partial or init only indication provided then the thread should not deinit the unit since it was already done */
    if (is_partial || is_mgmt)
    {
        appl_params.no_deinit = 1;
    }

    /** run init/deinit sequence */
    init_thread_rv[unit] = appl_dnxc_init_step_list_run(unit, &appl_params);

    /** destroy the thread */
    sal_thread_exit(0);
}

/*
 *  See .h file
 */
shr_error_e
sh_dnxc_infra_init_parallel_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int unit_i, is_failed = 0;
    int init_thread_units[BCM_LOCAL_UNITS_MAX];
    sal_thread_t thread_id;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_BOOL("PARTiaL", is_partial);
    SH_SAND_GET_BOOL("MGMT", is_mgmt);

    /** get active units */
    valid_units_bmp[0] = 0;
    if (is_mgmt)
    {
        /** assume units 0,1,2 are valid */
        for (unit_i = 0; unit_i < SH_DNXC_MGMT_SYS_NOF_UNITS; unit_i++)
        {
            SHR_BITSET(valid_units_bmp, unit_i);
        }
    }
    else
    {
        /** query for valid units */
        for (unit_i = 0; unit_i < BCM_LOCAL_UNITS_MAX; unit_i++)
        {
            if (bcm_unit_valid(unit_i))
            {
                SHR_BITSET(valid_units_bmp, unit_i);
            }
        }
    }

    /** deinit all units */
    if (is_partial)
    {
        appl_dnxc_init_param_t appl_params;

        sal_memset(&appl_params, 0x0, sizeof(appl_dnxc_init_param_t));
        appl_params.no_deinit = 0;
        appl_params.no_init = 1;

        SHR_BIT_ITER(valid_units_bmp, BCM_LOCAL_UNITS_MAX, unit_i)
        {
            SHR_IF_ERR_EXIT(appl_dnxc_init_step_list_run(unit_i, &appl_params));
        }
    }

    /**  run the bg init/deinit thread */
    SHR_BIT_ITER(valid_units_bmp, BCM_LOCAL_UNITS_MAX, unit_i)
    {
        /**
         * Init return value to some default which can't be a real DNX return value
         * This way we can verify whether the thread is done by polling on this value till it changes
         */
        init_thread_rv[unit_i] = _SHR_E_LIMIT;
        init_thread_units[unit_i] = unit_i;
        thread_id = sal_thread_create("DnxInitBg", SH_DNXC_INIT_SEQ_STACK_SIZE, SOC_BG_THREAD_PRI,
                                      sh_dnxc_infra_init_thread, &init_thread_units[unit_i]);

        if ((thread_id == 0) || (thread_id == SAL_THREAD_ERROR))
        {
            SHR_CLI_EXIT(_SHR_E_INTERNAL, "TDnxInitBg: ERROR: cannot create thread for Unit %d\n", unit_i);
        }
    }

    /** wait till all threads are done and verify the result, allow maximum init timeout of 5 minutes per unit */
    {
        sal_usecs_t time_out;
        int min_polls, nof_units;
        soc_timeout_t to;

        min_polls = 1; /** irelevant */
        SHR_BITCOUNT_RANGE(valid_units_bmp, nof_units, 0, BCM_LOCAL_UNITS_MAX);
        time_out = (5 * 60 * 1000 * 1000) * nof_units; /** 5 minutes per unit */
        soc_timeout_init(&to, time_out, min_polls);

        SHR_BIT_ITER(valid_units_bmp, BCM_LOCAL_UNITS_MAX, unit_i)
        {
            int *rv = &init_thread_rv[unit_i];

            /** wait till init/deinit sequence finishes */
            while (*rv == _SHR_E_LIMIT)
            {
                if (soc_timeout_check(&to))
                {
                    init_thread_rv[unit_i] = _SHR_E_TIMEOUT;
                    break;
                }
                else
                {
                    /** sleep 1 sec and then check again */
                    sal_sleep(1);
                }
            }
        }
    }

    /** Verify failures */
    SHR_BIT_ITER(valid_units_bmp, BCM_LOCAL_UNITS_MAX, unit_i)
    {
        int *rv = &init_thread_rv[unit_i];

        if (*rv != _SHR_E_NONE)
        {
            BSL_LOG(BSL_LOG_MODULE, ("Multi thread init failed for Unit %d\n", unit_i));
            is_failed = TRUE;
        }
    }

    if (is_failed)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Test FAIL\n");
    }
    else
    {
        SHR_CLI_EXIT(_SHR_E_NONE, "Test PASS\n");
    }

exit:
    SHR_FUNC_EXIT;
}
/* *INDENT-OFF* */
sh_sand_man_t sh_dnxc_infra_init_parallel_test_man = {
    "Test multi-threaded deinit+init sequence",
    "Run full multi-threaded Deinit+Init sequence, each unit sequence runs in its own thread (multi-threaded)",
    NULL,
    NULL,
};

sh_sand_invoke_t sh_dnxc_infra_init_parallel_tests[] = {
    {"",        "",         CTEST_POSTCOMMIT},
    {"partial", "partial",  CTEST_POSTCOMMIT},
    {"mgmt",    "mgmt",     SH_CMD_SKIP_EXEC},
    {NULL}
};

sh_sand_option_t sh_dnxc_infra_init_parallel_test_options[] = {
    {"PARTiaL", SAL_FIELD_TYPE_BOOL, "Perform full deinit+init sequence but only the init sequence will be multi threaded instead of full sequence",    "No"},
    {"MGMT",    SAL_FIELD_TYPE_BOOL, "Used for multi threaded init in mgmt system (assumes units 0, 1, 2)",                                             "No"},
    {NULL}
};

#ifdef BCM_DNX_SUPPORT
extern const dnx_init_step_t dnx_init_deinit_seq[];
#endif /** BCM_DNX_SUPPORT */

#ifdef BCM_DNXF_SUPPORT
extern const dnxf_init_step_t dnxf_init_deinit_seq[];
#endif /** BCM_DNXF_SUPPORT */

/** Convert dnx/dnxf list to utilex list */
static shr_error_e
diag_dnxc_init_step_list_convert(
        int unit,
        utilex_seq_step_t ** step_list)
{

    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_DNX_SUPPORT
    if (soc_is(unit, DNX_DEVICE))
    {
        SHR_CLI_EXIT_IF_ERR(dnx_init_step_list_convert(unit, dnx_init_deinit_seq, step_list),
                            "Error - dnx_init_step_list_convert failed!\n");
    }
#endif
#ifdef BCM_DNXF_SUPPORT
    if (soc_is(unit, DNXF_DEVICE))
    {        /** Convert dnx list to utilex list */
        SHR_CLI_EXIT_IF_ERR(dnxf_init_step_list_convert(unit, dnxf_init_deinit_seq, step_list),
                            "Error - dnxf_init_step_list_convert failed!\n");
    }
#endif

exit:
    SHR_FUNC_EXIT;
}
/** Free the allocation of the dnx/dnxf step list */
static shr_error_e
diag_dnxc_init_step_list_destory(
        int unit,
        utilex_seq_step_t * step_list)
{
    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_DNX_SUPPORT
    if (soc_is(unit, DNX_DEVICE))
    {
        SHR_IF_ERR_CONT(dnx_init_step_list_destory(unit, step_list));
    }
#endif
#ifdef BCM_DNXF_SUPPORT
    if (soc_is(unit, DNXF_DEVICE))
    {
        SHR_IF_ERR_CONT(dnxf_init_step_list_destory(unit, step_list));
    }
#endif

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

#ifdef BCM_DNX_SUPPORT
    const static int dnx_steps_to_skip_arr[] = {DNX_INIT_STEP_LINKSCAN};
#endif
#ifdef BCM_DNXF_SUPPORT
    const static int dnxf_steps_to_skip_arr[] = {DNXF_INIT_STEP_LINKSCAN};
#endif

/** Get pointer to the skip list of init steps to be tested per device type */
static void
diag_dnxc_init_steps_to_skip_list_get(
        int unit,
        const int ** steps_to_skip_arr,
        int * nof_steps_to_skip)
{
#ifdef BCM_DNX_SUPPORT
    if (soc_is(unit, DNX_DEVICE)) {
        *steps_to_skip_arr = dnx_steps_to_skip_arr;
        *nof_steps_to_skip = sizeof(dnx_steps_to_skip_arr)/sizeof(int);
    }
#endif
#ifdef BCM_DNXF_SUPPORT
    if (soc_is(unit, DNXF_DEVICE)) {
        *steps_to_skip_arr = dnxf_steps_to_skip_arr;
        *nof_steps_to_skip = sizeof(dnxf_steps_to_skip_arr)/sizeof(int);
    }
#endif
}

/**
 * DIAG Init Step name test - Start
 * {
 */
static shr_error_e
dnxc_infra_init_step_name_is_duplicated_in_list(
    int unit,
    utilex_seq_step_t * step_list,
    int step_id,
    char *step_name,
    uint8 *is_duplicated)
{
    utilex_seq_step_t *current_step;
    int is_found;

    SHR_FUNC_INIT_VARS(unit);

    *is_duplicated = FALSE;

    /** Set first step, only compare with leaf steps from the list */
    current_step = &(step_list[0]);
    is_found = UTILEX_SEQ_NEXT_STEP_FOUND;
    if ((current_step->forward == NULL) && (current_step->backward == NULL))
    {
        /** Find next active step */
        SHR_IF_ERR_EXIT(utilex_seq_next_active_step_get(unit, step_list, current_step->step_id, &current_step, &is_found));
    }

    /** Iterrate over all steps in the step list */
    while (is_found == UTILEX_SEQ_NEXT_STEP_FOUND)
    {
        /** Compare the name of the current step with the name of the step that needs to be verified */
        if ((sal_strncmp(step_name, current_step->step_name, SH_DNXC_INIT_STEP_NAME_SIZE) == 0) && (step_id != current_step->step_id)) {
            *is_duplicated = TRUE;
            BSL_LOG(BSL_LOG_MODULE, ("***************************************************************\n"));
            BSL_LOG(BSL_LOG_MODULE, ("* Step name %s for steps with ID=%d and ID=%d is the same! *\n", step_name, step_id, current_step->step_id));
            BSL_LOG(BSL_LOG_MODULE, ("***************************************************************\n"));
            SHR_EXIT();
        }

        /** Find next active step */
        SHR_IF_ERR_EXIT(utilex_seq_next_active_step_get(unit, step_list, current_step->step_id, &current_step, &is_found));
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 *  \brief Test to verify each init step has a unique name
 */
shr_error_e
sh_dnxc_infra_init_step_name_verify_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    utilex_seq_t seq;
    utilex_seq_step_t *step;
    int step_id = -1, step_id_start = -1, step_id_end = -1;
    uint8 is_duplicated = FALSE;
    int is_found;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("STEP", step_id);


    /** Init seq structure */
    SHR_CLI_EXIT_IF_ERR(utilex_seq_t_init(unit, &seq), "Error - utilex_seq_t_init failed!\n");

    /** Convert dnx/dnxf list to utilex list */
    SHR_CLI_EXIT_IF_ERR(diag_dnxc_init_step_list_convert(unit, &seq.step_list), "diag_dnxc_init_step_list_convert failed!\n");

    /*
     * If specified Step ID is ALL, we need to check all step names
     */
    if (step_id == -1)
    {
#ifdef BCM_DNX_SUPPORT
        if (soc_is(unit, DNX_DEVICE)) {
            step_id_start = DNX_INIT_STEP_FIRST_STEP;
            step_id_end = DNX_INIT_STEP_COUNT - 1;
        }
#endif
#ifdef BCM_DNXF_SUPPORT
        if (soc_is(unit, DNXF_DEVICE)) {
            step_id_start = DNXF_INIT_STEP_FIRST_STEP;
            step_id_end = DNXF_INIT_STEP_COUNT - 1;
        }
#endif
    }
    else
    {
        step_id_start = step_id_end = step_id;
    }

    for (step_id = step_id_start; step_id <= step_id_end; step_id++)
    {
        /** Lookup step by its ID */
        SHR_IF_ERR_EXIT(utilex_seq_find_step_by_id(unit, seq.step_list, step_id, &step, &is_found));
        if (is_found == FALSE)
        {
            /** If step is not found in list, no need to verify */
            continue;
        }
        else
        {
            /** Compare step name with the names of leaf steps from the list */
            SHR_IF_ERR_EXIT(dnxc_infra_init_step_name_is_duplicated_in_list(unit, seq.step_list, step->step_id, step->step_name, &is_duplicated));
            if (is_duplicated == TRUE)
            {
                SHR_CLI_EXIT(_SHR_E_FAIL, "Step name %s is duplicated!!!\n", step->step_name);
            }
        }


    }

    BSL_LOG(BSL_LOG_MODULE, ("*****************************\n"));
    BSL_LOG(BSL_LOG_MODULE, ("* No duplicate names found! *\n"));
    BSL_LOG(BSL_LOG_MODULE, ("*****************************\n"));

exit:
    SHR_CLI_EXIT_IF_ERR(diag_dnxc_init_step_list_destory(unit, seq.step_list), "diag_dnxc_init_step_list_destory failed!\n");

    PRT_FREE;
    SHR_FUNC_EXIT;
}

sh_sand_invoke_t sh_dnxc_infra_init_step_name_verify_test[] = {
    {"all_steps",   "",  CTEST_POSTCOMMIT},
    {NULL}
};

sh_sand_man_t sh_dnxc_infra_init_step_name_verify_man = {
    "Run name verification test on a given step",
    "The test goal verifying that each step has a unique name",
    NULL,
    NULL,
};

/**
 * DIAG Init Step name test - End
 * }
 */

/**
 * DIAG Error Recovery test - Start
 * {
 */
/**
 *  \brief test recovery from error in init seq
 *   */
shr_error_e
diag_dnxc_infra_init_error_recovery_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    utilex_seq_t seq;
    const int * steps_to_skip_arr_ptr;
    int nof_steps_to_skip;
    int first_step_id;
    int ii = 0;
    utilex_seq_err_recovery_test_t test_info;
    rhhandle_t ctest_soc_set_h = NULL;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("STEP", first_step_id);

    /** disable multithreading*/
    {
        ctest_soc_property_t ctest_soc_property[] = {
            {"custom_feature_multithread_en", "0"},
            {"dma_desc_aggregator_chain_length_max*", NULL},
            {"dma_desc_aggregator_buff_size_kb*", NULL},
            {"dma_desc_aggregator_timeout_usec*", NULL},
            {"dma_desc_aggregator_chain_length_max", "0"},
            {"dma_desc_aggregator_buff_size_kb", "0"},
            {"dma_desc_aggregator_timeout_usec", "0"},
            {NULL}
        };
        SHR_CLI_EXIT_IF_ERR(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &ctest_soc_set_h),
                            "failed setting soc properties");
    }

    diag_dnxc_init_steps_to_skip_list_get(unit, &steps_to_skip_arr_ptr, &nof_steps_to_skip);

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
        seq.mem_log_en = 0;
        seq.mem_test_en = 0;
        seq.swstate_log_en = 0;
        seq.access_only = 0;
        seq.tmp_allow_access_enable = dnxc_ha_tmp_allow_access_enable;
        seq.tmp_allow_access_disable = dnxc_ha_tmp_allow_access_disable;
    }

    /** Convert dnx/dnxf list to utilex list */
    SHR_CLI_EXIT_IF_ERR(diag_dnxc_init_step_list_convert(unit, &seq.step_list), "diag_dnxc_init_step_list_convert failed!\n");

    /** deinit unit */
    SHR_CLI_EXIT_IF_ERR(sh_process_command(unit, "tr 141 noinit=1"), "Error - 'tr 141 noinit=1' failed!\n");

#ifdef BCM_DNX_SUPPORT
    /**
     * DNXF device does all attach and init in its attach function!
     */
    if (soc_is(unit, DNX_DEVICE))
    {
        /** attach unit since instead of running regular init seqeunce - dnx init seq is called directly */
        SHR_CLI_EXIT_IF_ERR(bcm_attach(unit, NULL, NULL, unit), "Error - bcm_attach failed!\n");
    }
#endif

    /** run the test */
    SHR_CLI_EXIT_IF_ERR(utilex_seq_error_recovery_test_run
                        (unit, &seq, first_step_id, steps_to_skip_arr_ptr, nof_steps_to_skip, &test_info),
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
    ctest_dnxc_restore_soc_properties(unit, ctest_soc_set_h);
    SHR_CLI_EXIT_IF_ERR(diag_dnxc_init_step_list_destory(unit, seq.step_list), "diag_dnxc_init_step_list_destory failed!\n");

    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 *  \brief periodic event and stop\start operations
 *   */
shr_error_e
diag_dnxc_infra_init_error_recovery_tests_creation(
    int unit,
    rhlist_t * test_list)
{
    utilex_seq_t seq;
    utilex_seq_step_t *step;
    int is_found;
    uint32 flags;
    uint8 is_duplicated;
    char test_args[SH_DNXC_INIT_STEP_NAME_SIZE];
    char test_name[SH_DNXC_INIT_STEP_NAME_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    seq.step_list = NULL;

    /** Init seq structure */
    SHR_CLI_EXIT_IF_ERR(utilex_seq_t_init(unit, &seq), "Error - utilex_seq_t_init failed!\n");

    /** Convert dnx/dnxf list to utilex list */
    SHR_CLI_EXIT_IF_ERR(diag_dnxc_init_step_list_convert(unit, &seq.step_list), "diag_dnxc_init_step_list_convert failed!\n");

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
        is_duplicated = FALSE;
        sal_snprintf(test_args, SH_DNXC_INIT_STEP_NAME_SIZE, "STEP=%d", step->step_id);
        sal_snprintf(test_name, SH_DNXC_INIT_STEP_NAME_SIZE, "step_%s", step->step_name);

        /** replace special chars with '_' char */
        utilex_str_escape(test_name, '_');

        /** Compare step name with the names of leaf steps from the list */
        SHR_IF_ERR_EXIT(dnxc_infra_init_step_name_is_duplicated_in_list(unit, seq.step_list, step->step_id, step->step_name, &is_duplicated));
        if (is_duplicated != TRUE)
        {
            /** add test to list */
            SHR_CLI_EXIT_IF_ERR(sh_sand_test_add(unit, test_list, test_name, test_args, flags), "Add test failed\n");
        }
        else
        {
            BSL_LOG(BSL_LOG_MODULE, ("****************************************************************************\n"));
            BSL_LOG(BSL_LOG_MODULE, ("* Error recovery test for step %s not created due to duplicated step name! *\n", step->step_name));
            BSL_LOG(BSL_LOG_MODULE, ("****************************************************************************\n"));
            SHR_CLI_EXIT(_SHR_E_FAIL, "Ctests creation failed!!!\n");
        }

        /** find next active step */
        SHR_IF_ERR_EXIT(utilex_seq_next_active_step_get(unit, seq.step_list, step->step_id, &step, &is_found));
    }

exit:
    if (seq.step_list != NULL)
    {
        SHR_CLI_EXIT_IF_ERR(diag_dnxc_init_step_list_destory(unit, seq.step_list),
                                "diag_dnxc_init_step_list_destory failed!\n");
    }

    SHR_FUNC_EXIT;
}

sh_sand_man_t diag_dnxc_infra_init_error_recovery_test_man = {
    "Test error recovery during init sequence",
    "The test verifies each step on init sequence frees its memory",
    NULL,
    NULL,
};

/**
 * DIAG Error Recovery test - End
 * {
 */

#ifndef ADAPTER_SERVER_MODE
/**
 * DIAG Init time test - Start
 * {
 */
/** Callback function for dnx/dnxf appl init time thresholds per step*/
static shr_error_e
dnxc_appl_time_thresh_cb(
    int unit,
    int step_id,
    uint32 flags,
    sal_usecs_t * time_thresh)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_CLI_EXIT_IF_ERR(appl_dnxc_per_dev_step_time_threshold_get(unit, step_id, time_thresh),
                        "Error - appl_dnxc_per_dev_step_time_threshold_get failed!\n");

exit:
    SHR_FUNC_EXIT;
}
#endif

/**
 * DIAG Init time test - Start
 * {
 */
/** Callback function for dnx/dnxf BCM init time thresholds per step*/
static shr_error_e
dnxc_time_thresh_cb(
    int unit,
    int step_id,
    uint32 flags,
    sal_usecs_t * time_thresh)
{

    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_DNX_SUPPORT
    if (soc_is(unit, DNX_DEVICE))
    {
        SHR_CLI_EXIT_IF_ERR(dnx_time_thresh_cb(unit, step_id, flags, time_thresh),
                            "Error - dnx_time_thresh_cb failed!\n");
    }
#endif
#ifdef BCM_DNXF_SUPPORT
    if (soc_is(unit, DNXF_DEVICE))
    {
        SHR_CLI_EXIT_IF_ERR(dnxf_time_thresh_cb(unit, step_id, flags, time_thresh),
                            "Error - dnxf_time_thresh_cb failed!\n");
    }
#endif

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
diag_dnxc_infra_init_time_prt_test_info(int unit, char title[], utilex_seq_time_test_t test_info,
    int debug_flag, int coefficient) {
    int i;
    char step_time[36] = "Time step took (avg.) [microseconds]";

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("%s", title);
    PRT_COLUMN_ADD("Step name");
    PRT_COLUMN_ADD("Step ID");
    PRT_COLUMN_ADD("Step time threshold [microseconds]");
    PRT_COLUMN_ADD("Time step took (avg.) [microseconds]");
    if(debug_flag){

        PRT_COLUMN_ADD("Computed threshold (%d %% x avg. step time) [microseconds]", coefficient);
    }

    for (i = 0; i < test_info.count; i++)
    {
       PRT_ROW_ADD(PRT_ROW_SEP_NONE);
       PRT_CELL_SET("%s", test_info.step_name[i]);
       PRT_CELL_SET("%d", test_info.step_id[i]);
       PRT_CELL_SET("%d", test_info.step_thresh[i]);
       PRT_CELL_SET("%d", test_info.time[i]);
       if(debug_flag){
           PRT_CELL_SET("%d", test_info.computed_thresh[i]);
       }
       /** Print time to the regression DB */
       sal_snprintf(step_time, 36, "%d", test_info.time[i]);
       dnxc_regression_utils_print("APT_RESULT", step_time);
    }

    PRT_COMMIT;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 *  \brief The test is performed by verifying that each step is taking less time than a pre-defined threshold
 *   */
static shr_error_e
diag_dnxc_infra_time_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control,
    utilex_seq_t seq)
{
    int first_step_id;
    utilex_seq_time_test_t test_info_fail;
    utilex_seq_time_test_t test_info_success;
    rhhandle_t ctest_soc_set_h = NULL;
    int step_nof_iterations;
    int debug_flag;
    int coefficient;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("STEP", first_step_id);

    SH_SAND_GET_BOOL("DEBUG", debug_flag);

    SH_SAND_GET_INT32("runs", step_nof_iterations);

    SH_SAND_GET_INT32("coefficient", coefficient);

    /** disable multithreading*/
    {
        ctest_soc_property_t ctest_soc_property[] = {
            {"custom_feature_multithread_en", "0"},
            {NULL}
        };
        SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &ctest_soc_set_h));
    }

    /** by default start from the beginning of the list */
    if (first_step_id == -1)
    {
        first_step_id = UTILEX_SEQ_STEP_INVALID;
    }

    /** Init seq structure */
    {
        /** Configure sequence */
        /** Logging */
        seq.bsl_flags = BSL_LOG_MODULE;
        seq.log_severity = bslSeverityInfo;

        /** Partial seq - if first step is given then test only this test */
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
        seq.mem_log_en = 0;
        seq.mem_test_en = 0;
        seq.swstate_log_en = 0;
        seq.access_only = 0;
        seq.tmp_allow_access_enable = dnxc_ha_tmp_allow_access_enable;
        seq.tmp_allow_access_disable = dnxc_ha_tmp_allow_access_disable;
    }

    /** deinit unit */
    SHR_CLI_EXIT_IF_ERR(sh_process_command(unit, "tr 141 noinit=1"), "Error - 'tr 141 noinit=1' failed!\n");

#ifdef BCM_DNX_SUPPORT
    /**
     * DNXF device does all attach and init in its attach function!
     */
    if (soc_is(unit, DNX_DEVICE))
    {
        /** attach unit since instead of running regular init seqeunce - dnx init seq is called directly */
        SHR_CLI_EXIT_IF_ERR(bcm_attach(unit, NULL, NULL, unit), "Error - bcm_attach failed!\n");
    }
#endif

    /** run the test */
    SHR_CLI_EXIT_IF_ERR(utilex_seq_time_test_run(unit, &seq, first_step_id, step_nof_iterations, &test_info_fail,
                                                 &test_info_success, debug_flag, coefficient),
                            "Error - Init sequence Time test failed!\n");


    /** init unit */
    SHR_CLI_EXIT_IF_ERR(sh_process_command(unit, "tr 141 nodeinit=1"), "Error - 'tr 141 nodeinit=1' failed!\n");


    /** print summary table of success steps in the sequence for debugging threshold purpose*/

    if(test_info_success.count > 0 && debug_flag) {

        SHR_CLI_EXIT_IF_ERR(diag_dnxc_infra_init_time_prt_test_info(unit, "Success steps summary", test_info_success, debug_flag, coefficient),
                            "Error - could not write summarized table for success steps");
    }


    /** print summary table of failed steps in the sequence */
    if (test_info_fail.count > 0)
    {
        BSL_LOG(BSL_LOG_MODULE, ("********************************************************************\n"));
        BSL_LOG(BSL_LOG_MODULE, ("Time test failed!!! The following  %d steps failed\n", test_info_fail.count));
        BSL_LOG(BSL_LOG_MODULE, ("********************************************************************\n"));

        SHR_CLI_EXIT_IF_ERR(diag_dnxc_infra_init_time_prt_test_info(unit, "Failed steps summary", test_info_fail, debug_flag, coefficient),
            "Error - could not write summarized table for failed steps");

        SHR_CLI_EXIT(_SHR_E_MEMORY, "Test Failed\n");
    }

    SHR_CLI_EXIT(_SHR_E_NONE, "Test PASS\n");

exit:
    ctest_dnxc_restore_soc_properties(unit, ctest_soc_set_h);
    SHR_IF_ERR_CONT(diag_dnxc_init_step_list_destory(unit, seq.step_list));
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 *  \brief The test is performed by verifying that each step in INIT sequence steps is taking less time than a pre-defined threshold
 *   */
shr_error_e
diag_dnxc_infra_init_time_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    utilex_seq_t seq;
    sal_usecs_t time_stamp_start = sal_time_usecs();
    sal_usecs_t time_test;

    SHR_FUNC_INIT_VARS(unit);

    SHR_CLI_EXIT_IF_ERR(utilex_seq_t_init(unit, &seq), "Error - utilex_seq_t_init failed!\n");
    /** Convert dnx/dnxf list to utilex list */
    SHR_CLI_EXIT_IF_ERR(diag_dnxc_init_step_list_convert(unit, &seq.step_list), "diag_dnxc_init_step_list_convert failed!\n");
    seq.time_thresh_cb = dnxc_time_thresh_cb;
    SHR_CLI_EXIT_IF_ERR(diag_dnxc_infra_time_test_cmd(unit, args, sand_control,seq), "diag_dnxc_infra_time_test_cmd failed!\n");

exit:
    time_test = sal_time_usecs() - time_stamp_start;
    BSL_LOG(BSL_LOG_MODULE, ("time for test: %d\n", time_test));
    SHR_FUNC_EXIT;
}

#ifndef ADAPTER_SERVER_MODE
/**
 *  \brief The test is performed by verifying that each step  in APPL sequence is taking less time than a pre-defined threshold
 *   */
static shr_error_e
diag_dnxc_infra_appl_init_time_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    utilex_seq_t seq;
    const appl_dnxc_init_step_t *appl_steps;

    SHR_FUNC_INIT_VARS(unit);

    SHR_CLI_EXIT_IF_ERR(utilex_seq_t_init(unit, &seq), "Error - utilex_seq_t_init failed!\n");
    appl_dnxc_step_list_get(unit, &appl_steps);
    /** Convert dnx/dnxf list to utilex list */
    SHR_CLI_EXIT_IF_ERR(appl_dnxc_init_seq_convert(unit, appl_steps, 1, &seq), "diag_dnxc_init_step_list_convert failed!\n");
    seq.time_thresh_cb = dnxc_appl_time_thresh_cb;
    SHR_CLI_EXIT_IF_ERR(diag_dnxc_infra_time_test_cmd(unit, args, sand_control, seq), "diag_dnxc_infra_time_test_cmd failed!\n");

exit:
    SHR_FUNC_EXIT;
}
#endif

/**
 *  \brief Create time test for each step  to add in the post-commit regression
 */
static shr_error_e
diag_dnxc_infra_time_tests_creation(
    int unit,
    rhlist_t * test_list,
    utilex_seq_t seq)
{
    utilex_seq_step_t *step;
    int is_found;
    uint32 flags;
    uint8 is_duplicated;
    char test_args[SH_DNXC_INIT_STEP_NAME_SIZE];
    char test_name[SH_DNXC_INIT_STEP_NAME_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    /** Set first step, only create tests for leaf steps */
    step = &(seq.step_list[0]);
    if ((step->forward == NULL) && (step->backward == NULL))
    {
        /** find next active step */
        SHR_IF_ERR_EXIT(utilex_seq_next_active_step_get(unit, seq.step_list, step->step_id, &step, &is_found));
    }
    is_found = UTILEX_SEQ_NEXT_STEP_FOUND;
    flags = CTEST_POSTCOMMIT;

    /** create tests list */
    while (is_found == UTILEX_SEQ_NEXT_STEP_FOUND)
    {
        is_duplicated = FALSE;

        sal_snprintf(test_args, SH_DNXC_INIT_STEP_NAME_SIZE, "STEP=%d", step->step_id);
        sal_snprintf(test_name, SH_DNXC_INIT_STEP_NAME_SIZE, "step_%s", step->step_name);

        /** replace special chars with '_' char */
        utilex_str_escape(test_name, '_');

        /** Compare step name with the names of leaf steps from the list */
        SHR_IF_ERR_EXIT(dnxc_infra_init_step_name_is_duplicated_in_list(unit, seq.step_list, step->step_id, step->step_name, &is_duplicated));
        if (is_duplicated != TRUE)
        {
            /** add test to list */
            SHR_CLI_EXIT_IF_ERR(sh_sand_test_add(unit, test_list, test_name, test_args, flags), "Add test failed\n");
        }
        else
        {
            BSL_LOG(BSL_LOG_MODULE, ("******************************************************************\n"));
            BSL_LOG(BSL_LOG_MODULE, ("* Time test for step %s not created due to duplicated step name! *\n", step->step_name));
            BSL_LOG(BSL_LOG_MODULE, ("******************************************************************\n"));
            SHR_CLI_EXIT(_SHR_E_FAIL, "Ctests creation failed!!!\n");
        }

        /** find next active step */
        SHR_IF_ERR_EXIT(utilex_seq_next_active_step_get(unit, seq.step_list, step->step_id, &step, &is_found));
    }

exit:
    SHR_IF_ERR_CONT(diag_dnxc_init_step_list_destory(unit, seq.step_list));
    SHR_FUNC_EXIT;
}

/**
 *  \brief Create time test for each step of the init sequence to add in the post-commit regression
 */
shr_error_e
diag_dnxc_infra_init_time_tests_creation(
    int unit,
    rhlist_t * test_list)
{
    utilex_seq_t seq;
    SHR_FUNC_INIT_VARS(unit);

    SHR_CLI_EXIT_IF_ERR(utilex_seq_t_init(unit, &seq), "Error - utilex_seq_t_init failed!\n");

    /** Convert dnx/dnxf list to utilex list */
    SHR_CLI_EXIT_IF_ERR(diag_dnxc_init_step_list_convert(unit, &seq.step_list), "diag_dnxc_init_step_list_convert failed!\n");

    SHR_CLI_EXIT_IF_ERR(diag_dnxc_infra_time_tests_creation(unit, test_list, seq),"diag_dnxc_infra_time_tests_creation failed!\n");

exit:
    SHR_FUNC_EXIT;
}

#ifndef ADAPTER_SERVER_MODE
/**
 *  \brief Create time test for each step of the APPL sequence to add in the post-commit regression
 */
static shr_error_e
diag_dnxc_infra_appl_time_tests_creation(
    int unit,
    rhlist_t * test_list)
{
    utilex_seq_t seq;

    const appl_dnxc_init_step_t *appl_steps;

    SHR_FUNC_INIT_VARS(unit);

    SHR_CLI_EXIT_IF_ERR(utilex_seq_t_init(unit, &seq), "Error - utilex_seq_t_init failed!\n");

    /** Convert dnx/dnxf list to utilex list */
    appl_dnxc_step_list_get(unit, &appl_steps);
    /** Convert dnx/dnxf list to utilex list */
    SHR_CLI_EXIT_IF_ERR(appl_dnxc_init_seq_convert(unit, /*seq.step_list*/ appl_steps, 1, &seq), "diag_dnxc_init_step_list_convert failed!\n");
    SHR_CLI_EXIT_IF_ERR(diag_dnxc_infra_time_tests_creation(unit, test_list, seq), "diag_dnxc_infra_time_tests_creation failed!\n");

exit:
    SHR_FUNC_EXIT;
}
#endif



sh_sand_man_t diag_dnxc_infra_init_time_test_man = {
    "Run time test on a given step",
    "The test is performed by verifying that each step is taking less time than a pre-defined threshold",
    NULL,
    NULL,
};

/**
 * DIAG Init time test - End
 * {
 */

sh_sand_option_t diag_dnxc_infra_init_test_options[] = {
    {"STEP", SAL_FIELD_TYPE_INT32, "BCM DNX/DNXF init sequence first step ID to start testing from", "-1"},
    {"DEBUG", SAL_FIELD_TYPE_BOOL, "Print the average performance of each run",                      "No"},
    {"runs", SAL_FIELD_TYPE_INT32, "Step number of runs", "5"},
    {"coefficient", SAL_FIELD_TYPE_INT32, "percentage multiplication for computed threshold time", "120"},
    {NULL}
};

/**
 * DIAG Error Recovery test - End
 * }
 */

sh_sand_cmd_t dnxc_infra_init_test_cmds[] = {
    {"ERRor_RECovery", diag_dnxc_infra_init_error_recovery_test_cmd, NULL,
     diag_dnxc_infra_init_test_options, &diag_dnxc_infra_init_error_recovery_test_man,
     NULL, NULL, CTEST_POSTCOMMIT, diag_dnxc_infra_init_error_recovery_tests_creation},

#ifndef ADAPTER_SERVER_MODE
    {"TIME", diag_dnxc_infra_init_time_test_cmd, NULL,
    diag_dnxc_infra_init_test_options, &diag_dnxc_infra_init_time_test_man,
    NULL, NULL, CTEST_POSTCOMMIT, diag_dnxc_infra_init_time_tests_creation},

    {"APPL_TIME", diag_dnxc_infra_appl_init_time_test_cmd, NULL,
    diag_dnxc_infra_init_test_options, &diag_dnxc_infra_init_time_test_man,
    NULL, NULL, CTEST_POSTCOMMIT, diag_dnxc_infra_appl_time_tests_creation},
#endif

    {"PaRaLLel", sh_dnxc_infra_init_parallel_test_cmd, NULL,
     sh_dnxc_infra_init_parallel_test_options, &sh_dnxc_infra_init_parallel_test_man,
     NULL, sh_dnxc_infra_init_parallel_tests, CTEST_POSTCOMMIT},

    {"Name_VERify", sh_dnxc_infra_init_step_name_verify_test_cmd, NULL,
     diag_dnxc_infra_init_test_options, &sh_dnxc_infra_init_step_name_verify_man,
     NULL, sh_dnxc_infra_init_step_name_verify_test, CTEST_POSTCOMMIT},

    {NULL}
};

/* *INDENT-ON* */
