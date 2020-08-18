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
#include <appl/reference/dnxc/appl_ref_init_deinit.h>

#include <soc/drv.h>

#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/dnx_err_recovery_manager_utils.h>
#endif

#include <shared/shrextend/shrextend_debug.h>

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

#ifdef BCM_DNX_SUPPORT
    if(SOC_IS_DNX(unit))
    {
        dnx_err_recovery_utils_excluded_thread_add(unit);
    }
#endif

    sal_memset(&appl_params, 0x0, sizeof(appl_dnxc_init_param_t));

    /** if partial or init onnly indication provided then the thread should not deinit the unit since it was already done */
    if (is_partial || is_mgmt)
    {
        appl_params.no_deinit = 1;
    }

    /** run init/deinit sequence */
    init_thread_rv[unit] = appl_dnxc_init_step_list_run(unit, &appl_params);

#ifdef BCM_DNX_SUPPORT
    if(SOC_IS_DNX(unit))
    {
        dnx_err_recovery_utils_excluded_thread_remove(unit);
    }
#endif

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

sh_sand_man_t sh_dnxc_infra_init_parallel_test_man = {
    "Test multi-threaded deinit+init sequence",
    "Run full multi-threaded Deinit+Init sequence, each unit sequence runs in its own thread (multi-threaded)",
    NULL,
    NULL,
};

sh_sand_invoke_t sh_dnxc_infra_init_parallel_tests[] = {
    {"", "", CTEST_POSTCOMMIT}
    ,
    {"partial", "partial", CTEST_POSTCOMMIT}
    ,
    {"mgmt", "mgmt", SH_CMD_SKIP_EXEC}
    ,
    {NULL}
};

sh_sand_option_t sh_dnxc_infra_init_parallel_test_options[] = {
    {"PARTiaL", SAL_FIELD_TYPE_BOOL,
     "Perform full deinit+init sequence but only the init sequence will be multi threaded instead of full sequence",
     "No"}
    ,
    {"MGMT", SAL_FIELD_TYPE_BOOL, "Used for multi threaded init in mgmt system (assumes units 0, 1, 2)", "No"}
    ,
    {NULL}
};
