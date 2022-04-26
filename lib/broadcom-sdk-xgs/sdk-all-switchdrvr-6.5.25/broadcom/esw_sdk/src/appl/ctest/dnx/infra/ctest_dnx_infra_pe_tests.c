/** \file diag_dnx_ipq_tests.c
 * 
 * Tests for IPQ
 * 
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

#include <appl/diag/sand/diag_sand_framework.h>
#include <shared/shrextend/shrextend_debug.h>
#include <shared/shr_thread_manager.h>
#include <sal/core/thread.h>
#include <sal/core/sync.h>

/* csllbsck for periodic event testing. */
static int
shr_thread_manager_unit_test_callback(
    int unit,
    void *user_data)
{

    int *counter = (int *) user_data;

    (*counter)++;

    return _SHR_E_NONE;
}

static int
shr_thread_manager_unit_test_error_callback(
    int unit,
    void *user_data)
{

    int *counter = (int *) user_data;

    (*counter)++;

    return _SHR_E_FAIL;
}

/** 
 *  \brief periodic event and stop\start operations
 *   */
static shr_error_e
diag_dnx_pe_periodic_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int counter = 0, start_operation;
    shr_thread_manager_handler_t my_handler;
    shr_thread_manager_config_t config;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("start", start_operation);

    shr_thread_manager_config_t_init(&config);
    config.name = "Periodic";
    config.type = SHR_THREAD_MANAGER_TYPE_EVENT;
    config.interval = 100000;
    config.callback = &shr_thread_manager_unit_test_callback;
    config.user_data = (void *) &counter;
    config.bsl_module = BSL_LOG_MODULE;
    if (!start_operation)
    {   /* default is 1, set only if requried */
        config.start_operation = start_operation;
    }

    /*
     * create & start thread - check counter is increasing 
     */
    SHR_CLI_EXIT_IF_ERR(shr_thread_manager_create(unit, &config, &my_handler),
                        "Test FAIL. Failed to create periodic event\n");

    sal_usleep(config.interval * 10);

    /*
     * if thread not started - verify it and start the thread 
     */
    if (!start_operation)
    {

        /*
         * make sure counter isn't increasing 
         */
        if (counter != 0)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "Test FAIL. Counter unexpectedly updated by the thread.\n");
        }

        SHR_CLI_EXIT_IF_ERR(shr_thread_manager_start(my_handler), "Test FAIL. Failed to re-start periodic event\n");

        sal_usleep(config.interval * 10);
    }

    if (counter == 0)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "Test FAIL. Counter wasn't updated by the thread (1).\n");
    }

    /*
     * stop the thread - make sure counter is constant 
     */
    SHR_CLI_EXIT_IF_ERR(shr_thread_manager_stop(my_handler, 0), "Test FAIL. Failed to stop periodic event\n");

    counter = 0;

    sal_usleep(config.interval * 10);

    if (counter != 0)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "Test FAIL. Counter unexpectedly updated by the thread.\n");
    }

    /*
     * restart the thread - make sure counter is increasing again 
     */
    SHR_CLI_EXIT_IF_ERR(shr_thread_manager_start(my_handler), "Test FAIL. Failed to re-start periodic event\n");

    sal_usleep(config.interval * 10);

    if (counter == 0)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "Test FAIL. Counter wasn't updated by the thread (2).\n");
    }

    /*
     * destroy 
     */
    SHR_CLI_EXIT_IF_ERR(shr_thread_manager_destroy(&my_handler), "Test FAIL. Failed to destroy periodic event\n");

    SHR_CLI_EXIT(_SHR_E_NONE, "Test PASS\n");

exit:
    SHR_FUNC_EXIT;
}

#define DIAG_DNX_PE_TRIGGER_COUNT 5

/** 
 *  \brief trigger driven event
 *   */
static shr_error_e
diag_dnx_pe_trigger_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int i, error_test;
    int counter = 0;
    shr_thread_manager_handler_t my_handler;
    shr_thread_manager_config_t config;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("error_test", error_test);

    /*
     * create and start trigger driven event 
     */
    shr_thread_manager_config_t_init(&config);
    config.name = "Trigger";
    config.type = SHR_THREAD_MANAGER_TYPE_EVENT;
    config.interval = sal_sem_FOREVER;
    config.callback = &shr_thread_manager_unit_test_callback;
    config.user_data = (void *) &counter;
    config.bsl_module = BSL_LOG_MODULE;

    if (error_test)
    {
        config.error_threshold = DIAG_DNX_PE_TRIGGER_COUNT;
        config.callback = &shr_thread_manager_unit_test_error_callback;
    }

    SHR_CLI_EXIT_IF_ERR(shr_thread_manager_create(unit, &config, &my_handler),
                        "Test FAIL. Failed to create periodic event\n");

    /*
     * wait for thread to start running 
     */
    sal_usleep(10000 * 10);

    /*
     * counter should stay constant until event will be triggered. 
     */
    if (counter != 0)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "Test FAIL. Counter unexpectedly updated by the thread.\n");
    }

    /*
     * initiate 5 triggers 
     */
    for (i = 0; i < DIAG_DNX_PE_TRIGGER_COUNT; i++)
    {
        SHR_CLI_EXIT_IF_ERR(shr_thread_manager_trigger(my_handler), "Test FAIL. Failed to trigger event\n");
        sal_usleep(10000 * 10);
    }

    /*
     * make sure counter increased once per trigger
     */
    if (counter != DIAG_DNX_PE_TRIGGER_COUNT)
    {
        SHR_CLI_EXIT(_SHR_E_FAIL, "Test FAIL. Counter expected to be %d, actual value %d.\n", DIAG_DNX_PE_TRIGGER_COUNT,
                     counter);
    }

    /*
     * In case of error threashold - we shouldn't be able to trigger another event 
     */
    if (error_test)
    {
        int rv = shr_thread_manager_trigger(my_handler);
        if (rv != _SHR_E_DISABLED)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "Test FAIL. Trigger is expected to return %d, received %d\n", _SHR_E_DISABLED,
                         rv);
        }
    }

    /*
     * destroy 
     */
    SHR_CLI_EXIT_IF_ERR(shr_thread_manager_destroy(&my_handler), "Test FAIL. Failed to destroy periodic event\n");

    SHR_CLI_EXIT(_SHR_E_NONE, "Test PASS\n");

exit:
    SHR_FUNC_EXIT;
}

static sh_sand_man_t diag_dnx_pe_periodic_test_man = {
    "Test periodic operation of period_event infra",
    "Create periodic event." " Chekc that periodic is working." " Destroy the event.",
    NULL,
    NULL,
};

static sh_sand_man_t diag_dnx_pe_trigger_test_man = {
    "Test trigger operation of period_event infra",
    "Create periodic event." " Chekc that periodic is working." " Destroy the event.",
    NULL,
    NULL,
};

static sh_sand_invoke_t diag_dnx_pe_periodic_tests[] = {
    {"periodic", "start=1", CTEST_SKIPPABLE},
    {"create after start", "start=0", CTEST_SKIPPABLE},
    {NULL}
};

static sh_sand_invoke_t diag_dnx_pe_trigger_tests[] = {
    {"trigger", "error_test=0", CTEST_SKIPPABLE},
    {"Error Threashold", "error_test=1", CTEST_SKIPPABLE},
    {NULL}
};

static sh_sand_option_t diag_dnx_pe_periodic_test_options[] = {
    {"start", SAL_FIELD_TYPE_INT32, "1-create also does start. 0-create doesn't start operation", "1"},
    {NULL}
};

static sh_sand_option_t diag_dnx_pe_trigger_test_options[] = {
    {"error_test", SAL_FIELD_TYPE_INT32, "Whether to test also error threashold", "0"},
    {NULL}
};

sh_sand_cmd_t dnx_infra_shr_thread_manager_test_cmds[] = {
    {"periodic", diag_dnx_pe_periodic_test_cmd, NULL, diag_dnx_pe_periodic_test_options, &diag_dnx_pe_periodic_test_man,
     NULL,
     diag_dnx_pe_periodic_tests, CTEST_POSTCOMMIT},
    {"trigger", diag_dnx_pe_trigger_test_cmd, NULL, diag_dnx_pe_trigger_test_options, &diag_dnx_pe_trigger_test_man,
     NULL,
     diag_dnx_pe_trigger_tests, CTEST_POSTCOMMIT},
    {NULL}
};
