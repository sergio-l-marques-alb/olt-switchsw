/*
 *
 * $Id:$
 *
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * This file include unit testing for periodic event.
 */

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif
#define _ERR_MSG_MODULE_NAME BSL_LS_APPL_SHELL

#include <soc/portmod/portmod_internal.h>
#include <soc/util.h>
#include <shared/shrextend/shrextend_debug.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>

static int _counter = 0;
static int _counter_2nd = 0;

static shr_error_e
utest_periodic_counter_callback(
    int unit,
    int port)
{

    SHR_FUNC_INIT_VARS(unit);

    _counter++;

    SHR_FUNC_EXIT;
}

static shr_error_e
utest_periodic_counter2nd_callback(
    int unit,
    int port)
{

    SHR_FUNC_INIT_VARS(unit);

    _counter_2nd++;

    SHR_FUNC_EXIT;
}

/*
 * Some Periodic event Ctests assume there is no
 * callback registered in the thread, therefore need to unregister
 * all the callbacks before starting these tests.
 */
static shr_error_e
utest_pe_existing_callback_unregister(
    int unit)
{
    int interval;
    bcm_port_t logical_port;
    bcm_pbmp_t nif_ports;

    SHR_FUNC_INIT_VARS(unit);

    SHR_CLI_EXIT_IF_ERR(portmod_port_periodic_interval_get(unit, &interval),
                        "Failed in portmod_port_periodic_interval_get\n");
    if (interval != sal_sem_FOREVER)
    {
        /*
         * Unregister all the callbacks for NIF ports.
         */
        SHR_CLI_EXIT_IF_ERR(dnx_algo_port_logicals_get
                            (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF, DNX_ALGO_PORT_LOGICALS_F_MASTER_ONLY,
                             &nif_ports), " Failed in dnx_algo_port_logicals_get\n");
        _SHR_PBMP_ITER(nif_ports, logical_port)
        {
            SHR_CLI_EXIT_IF_ERR(portmod_port_periodic_callback_unregister(unit, logical_port),
                                " Failed in portmod_port_periodic_callback_unregister\n");
        }
    }
exit:
    SHR_FUNC_EXIT;
}

#define UTEST_PE_UNREGISTER_COUNT 10

static shr_error_e
utest_periodic_counter_with_unregister_callback(
    int unit,
    int port)
{

    SHR_FUNC_INIT_VARS(unit);

    _counter++;

    if (_counter == UTEST_PE_UNREGISTER_COUNT)
    {
        SHR_CLI_EXIT_IF_ERR(portmod_port_periodic_callback_unregister(unit, port),
                            " Failed in portmod_port_periodic_callback_unregister\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/* Test simple register-unregister */

static shr_error_e
utest_pe_simple_register_unregister(
    int unit,
    int port)
{
    SHR_FUNC_INIT_VARS(unit);
    /*
     * Clear the existing callbacks as the interval of the
     * existing callbacks might affect this test
     */
    SHR_CLI_EXIT_IF_ERR(utest_pe_existing_callback_unregister(unit),
                        "Failed in utest_pe_existing_callback_unregister\n");

    _counter = 0;
    SHR_CLI_EXIT_IF_ERR(portmod_port_periodic_callback_register(unit, port, utest_periodic_counter_callback, 1000),
                        "Failed in portmod_port_periodic_callback_register\n");
    sal_usleep(10000);
    SHR_CLI_EXIT_IF_ERR(portmod_port_periodic_callback_unregister(unit, port),
                        "Failed in portmod_port_periodic_callback_unregister\n");

    /*
     * make sure counter increased
     */
    if (_counter == 0)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "utest_pe_simple_register_unregister failed - periodic not executed\n");
    }

    /*
     * make sure counter isn't increasing anymore
     */
    _counter = 0;

    sal_usleep(10000);
    if (_counter != 0)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "utest_pe_simple_register_unregister failed - periodic keep running\n");
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
utest_pe_simple_register_unregister_test(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int port;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("port", port);

    SHR_CLI_EXIT_IF_ERR(utest_pe_simple_register_unregister(unit, port),
                        "Failed in utest_pe_simple_register_unregister\n");

exit:
    SHR_FUNC_EXIT;
}

/* check fast register-unregister operations */
static shr_error_e
utest_pe_loop_register_unregister(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int port;
    int i;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("port", port);
    /*
     * Clear the existing callbacks as the interval of the
     * existing callbacks might affect this test
     */
    SHR_CLI_EXIT_IF_ERR(utest_pe_existing_callback_unregister(unit),
                        "Failed in utest_pe_existing_callback_unregister\n");

    for (i = 0; i < 1000; i++)
    {
        SHR_CLI_EXIT_IF_ERR(portmod_port_periodic_callback_register(unit, port, utest_periodic_counter_callback, 1000),
                            "Failed in portmod_port_periodic_callback_register\n");
        SHR_CLI_EXIT_IF_ERR(portmod_port_periodic_callback_unregister(unit, port),
                            "Failed in portmod_port_periodic_callback_unregister\n");
    }

    /*
     * make sure counter isn't increasing anymore
     */
    _counter = 0;

    sal_usleep(10000);
    if (_counter != 0)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "utest_pe_loop_register_unregister failed - periodic keep running\n");
    }

    /*
     * make sure mechanisem is still well functional
     */
    SHR_CLI_EXIT_IF_ERR(utest_pe_simple_register_unregister(unit, port),
                        "utest_pe_loop_register_unregister failed - calling utest_pe_simple_register_unregister failed\n");

exit:
    SHR_FUNC_EXIT;
}

/* test switch port callback */
static shr_error_e
utest_pe_callback_change(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int port;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("port", port);
    /*
     * Clear the existing callbacks as the interval of the
     * existing callbacks might affect this test
     */
    SHR_CLI_EXIT_IF_ERR(utest_pe_existing_callback_unregister(unit),
                        "Failed in utest_pe_existing_callback_unregister\n");

    SHR_CLI_EXIT_IF_ERR(portmod_port_periodic_callback_register(unit, port, utest_periodic_counter_callback, 1000),
                        "Failed in portmod_port_periodic_callback_register\n");

    _counter = 0;
    _counter_2nd = 0;

    sal_usleep(10000);

    /*
     * make sure counter increased
     */
    if (_counter == 0)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "utest_pe_callback_change failed - periodic not executed\n");
    }

    /*
     * make sure counter 2nd doesn't increasing anymore
     */
    if (_counter_2nd != 0)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "utest_pe_callback_change failed - 2nd counter is updated\n");
    }

    SHR_CLI_EXIT_IF_ERR(portmod_port_periodic_callback_register(unit, port, utest_periodic_counter2nd_callback, 1000),
                        "Failed in portmod_port_periodic_callback_register\n");

    _counter = 0;
    _counter_2nd = 0;

    sal_usleep(10000);

    /*
     * make sure counter 2nd increased
     */
    if (_counter_2nd == 0)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "utest_pe_callback_change failed - periodic not executed\n");
    }

    /*
     * make sure counter doesn't increasing anymore
     */
    if (_counter != 0)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "utest_pe_callback_change failed - counter is updated\n");
    }

    SHR_CLI_EXIT_IF_ERR(portmod_port_periodic_callback_unregister(unit, port),
                        "Failed in portmod_port_periodic_callback_unregister\n");

    _counter = 0;
    _counter_2nd = 0;

    sal_usleep(10000);

    /*
     * make sure both counter not increasing anymore
     */
    if (_counter != 0)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "utest_pe_callback_change failed - counter is updated\n");
    }

    if (_counter_2nd != 0)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "utest_pe_callback_change failed - _counter_2nd is updated\n");
    }

exit:
    SHR_FUNC_EXIT;
}

/* test forever interval selection */
static shr_error_e
utest_pe_interval_forever_check(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int port;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("port", port);
    /*
     * Clear the existing callbacks as the interval of the
     * existing callbacks might affect this test
     */
    SHR_CLI_EXIT_IF_ERR(utest_pe_existing_callback_unregister(unit),
                        "Failed in utest_pe_existing_callback_unregister\n");
    _counter = 0;
    SHR_CLI_EXIT_IF_ERR(portmod_port_periodic_callback_register(unit, port, utest_periodic_counter_callback, 1000),
                        "Failed in portmod_port_periodic_callback_register\n");
    sal_usleep(10000);

    /*
     * make sure counter increased
     */
    if (_counter == 0)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "utest_pe_interval_forever_check failed - periodic not executed\n");
    }

    /*
     * set interval to forever
     */
    SHR_CLI_EXIT_IF_ERR(portmod_port_periodic_callback_register
                        (unit, port, utest_periodic_counter_callback, sal_sem_FOREVER),
                        "Failed in portmod_port_periodic_callback_register\n");

    sal_usleep(10000);  /* extra sleep in case periodic was in the middle of operation */
    _counter = 0;
    sal_usleep(10000);

    /*
     * make sure counter doesn't increasing anymore
     */
    if (_counter != 0)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "utest_pe_interval_forever_check failed - counter is updated\n");
    }

    /*
     * make sure mechanisem is still well functional
     */
    SHR_CLI_EXIT_IF_ERR(utest_pe_simple_register_unregister(unit, port),
                        "utest_pe_interval_forever_check failed - calling utest_pe_simple_register_unregister failed\n");

exit:
    SHR_FUNC_EXIT;
}

/* test interval selection */
static shr_error_e
utest_pe_interval_selection(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int port1, port2;
    int interval;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("port1", port1);
    SH_SAND_GET_INT32("port2", port2);

    if (port1 == port2)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "utest_pe_interval_selection failed - tests requires diffrent ports \n");
    }
    /*
     * Clear the existing callbacks as the interval of the
     * existing callbacks might affect this test
     */
    SHR_CLI_EXIT_IF_ERR(utest_pe_existing_callback_unregister(unit),
                        "Failed in utest_pe_existing_callback_unregister\n");

    /*
     * check smallest interval is selected: port1 interval should be selected
     */
    SHR_CLI_EXIT_IF_ERR(portmod_port_periodic_callback_register(unit, port1, utest_periodic_counter_callback, 1000),
                        "Failed in portmod_port_periodic_callback_register\n");
    SHR_CLI_EXIT_IF_ERR(portmod_port_periodic_callback_register(unit, port2, utest_periodic_counter2nd_callback, 10000),
                        "Failed in portmod_port_periodic_callback_register\n");

    SHR_CLI_EXIT_IF_ERR(portmod_port_periodic_interval_get(unit, &interval),
                        "Failed in portmod_port_periodic_interval_get\n");
    if (interval != 1000)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL,
                     "utest_pe_interval_selection failed - calling interval not updated correctly 1\n");
    }

    /*
     * check smallest interval is selected: port1 increased, port2 interval should be selected
     */
    SHR_CLI_EXIT_IF_ERR(portmod_port_periodic_callback_register(unit, port1, utest_periodic_counter_callback, 20000),
                        "Failed in portmod_port_periodic_callback_register\n");

    SHR_CLI_EXIT_IF_ERR(portmod_port_periodic_interval_get(unit, &interval),
                        "Failed in portmod_port_periodic_interval_get\n");
    if (interval != 10000)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL,
                     "utest_pe_interval_selection failed - calling interval not updated correctly 2\n");
    }

    /*
     * check forever interval isn't selected: port1 interval should be selected
     */
    SHR_CLI_EXIT_IF_ERR(portmod_port_periodic_callback_register
                        (unit, port2, utest_periodic_counter2nd_callback, sal_sem_FOREVER),
                        "Failed in portmod_port_periodic_callback_register\n");

    SHR_CLI_EXIT_IF_ERR(portmod_port_periodic_interval_get(unit, &interval),
                        "Failed in portmod_port_periodic_interval_get\n");
    if (interval != 20000)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL,
                     "utest_pe_interval_selection failed - calling interval not updated correctly 3\n");
    }

    /*
     * check that unregister update the interval
     */
    /*
     * 1. set interval for port2, port1 should be selected as it's the minimum
     */
    SHR_CLI_EXIT_IF_ERR(portmod_port_periodic_callback_register(unit, port2, utest_periodic_counter2nd_callback, 40000),
                        "Failed in portmod_port_periodic_callback_register\n");
    SHR_CLI_EXIT_IF_ERR(portmod_port_periodic_interval_get(unit, &interval),
                        "Failed in portmod_port_periodic_interval_get\n");
    if (interval != 20000)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL,
                     "utest_pe_interval_selection failed - calling interval not updated correctly 4\n");
    }

    /*
     * unregister port1, port2 interval should be selected
     */
    SHR_CLI_EXIT_IF_ERR(portmod_port_periodic_callback_unregister(unit, port1),
                        "Failed in portmod_port_periodic_callback_unregister\n");
    SHR_CLI_EXIT_IF_ERR(portmod_port_periodic_interval_get(unit, &interval),
                        "Failed in portmod_port_periodic_interval_get\n");
    if (interval != 40000)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL,
                     "utest_pe_interval_selection failed - calling interval not updated correctly 5\n");
    }

    /*
     * turn off the light
     */
    SHR_CLI_EXIT_IF_ERR(portmod_port_periodic_callback_unregister(unit, port2),
                        "Failed in portmod_port_periodic_callback_unregister\n");
    SHR_CLI_EXIT_IF_ERR(portmod_port_periodic_interval_get(unit, &interval),
                        "Failed in portmod_port_periodic_interval_get\n");
    if (interval != sal_sem_FOREVER)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL,
                     "utest_pe_interval_selection failed - calling interval not updated correctly 6\n");
    }

    /*
     * make sure mechanisem is still well functional
     */
    SHR_CLI_EXIT_IF_ERR(utest_pe_simple_register_unregister(unit, port1),
                        "utest_pe_interval_selection failed - calling utest_pe_simple_register_unregister failed\n");

exit:
    SHR_FUNC_EXIT;

}

/* test unregister from periodic
 *
 * This test:
 *  1. Start thread that after UTEST_PE_UNREGISTER_COUNT loops unregister itself
 *  2. Make sure it was actually unregistered
 */
static shr_error_e
utest_pe_periodic_unregister(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int port, interval;
    soc_timeout_t to;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("port", port);
    /*
     * Clear the existing callbacks as the interval of the
     * existing callbacks affect this test
     */
    SHR_CLI_EXIT_IF_ERR(utest_pe_existing_callback_unregister(unit),
                        "Failed in utest_pe_existing_callback_unregister\n");

    _counter = 0;
    SHR_CLI_EXIT_IF_ERR(portmod_port_periodic_callback_register
                        (unit, port, utest_periodic_counter_with_unregister_callback, 1000),
                        "Failed portmod_port_periodic_callback_register\n");

    /*
     * wait for thread to exit
     */
    soc_timeout_init(&to, SECOND_USEC /* 1 sec */ , 100);

    while (_counter < UTEST_PE_UNREGISTER_COUNT)
    {
        if (soc_timeout_check(&to))
        {
            SHR_CLI_EXIT(_SHR_E_INTERNAL, "utest_pe_periodic_unregister failed - periodic didn't unregister\n");
        }
    }

    /*
     * check unregister actually happend
     */
    SHR_CLI_EXIT_IF_ERR(portmod_port_periodic_interval_get(unit, &interval),
                        "Failed in portmod_port_periodic_interval_get\n");
    if (interval != sal_sem_FOREVER)
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL,
                     "utest_pe_interval_selection failed - calling interval not updated correctly 6\n");
    }

    /*
     * make sure mechanisem is still well functional
     */
    SHR_CLI_EXIT_IF_ERR(utest_pe_simple_register_unregister(unit, port),
                        "utest_pe_interval_forever_check failed - calling utest_pe_simple_register_unregister failed\n");

exit:
    SHR_FUNC_EXIT;
}

static sh_sand_man_t utest_pe_simple_register_unregister_test_man = {
    "unit testing for periodic operation in portmod",
    "Test simple register-unregister",
    NULL,
    NULL,
};

static sh_sand_man_t utest_pe_loop_register_unregister_man = {
    "unit testing for periodic operation in portmod",
    "check fast register-unregister operations",
    NULL,
    NULL,
};

static sh_sand_man_t utest_pe_callback_change_man = {
    "unit testing for periodic operation in portmod",
    "test switch port callback",
    NULL,
    NULL,
};

static sh_sand_man_t utest_pe_interval_forever_check_man = {
    "unit testing for periodic operation in portmod",
    "test forever interval",
    NULL,
    NULL,
};

static sh_sand_man_t utest_pe_periodic_unregister_man = {
    "unit testing for periodic operation in portmod",
    "test unregister from periodic event",
    NULL,
    NULL,
};

static sh_sand_man_t utest_pe_interval_selection_man = {
    "unit testing for periodic operation in portmod",
    "test interval selection mechanism",
    NULL,
    NULL,
};

static sh_sand_option_t utest_dnx_portmod_periodic_test_port_option[] = {
    {"port", SAL_FIELD_TYPE_INT32, "port to run test over", "0"},
    {NULL}
};

static sh_sand_invoke_t utest_dnx_portmod_periodic_test_port_tests[] = {
    {"port1", "port=1"},
    {"port13", "port=13"},
    {"port14", "port=14"},
    {"port17", "port=17"},
    {NULL}
};

static sh_sand_option_t utest_dnx_portmod_periodic_test_2ports_option[] = {
    {"port1", SAL_FIELD_TYPE_INT32, "port1 to run test over", "0"},
    {"port2", SAL_FIELD_TYPE_INT32, "port2 to run test over", "0"},
    {NULL}
};

static sh_sand_invoke_t utest_dnx_portmod_periodic_test_2ports_tests[] = {
    {"ports 1,13", "port1=1 port2=13"},
    {"ports 5,17", "port1=5 port2=17"},
    {NULL}
};

sh_sand_cmd_t dnx_infra_portmod_shr_thread_manager_test_cmds[] = {

    {"simple", utest_pe_simple_register_unregister_test, NULL, utest_dnx_portmod_periodic_test_port_option,
     &utest_pe_simple_register_unregister_test_man,
     NULL,
     utest_dnx_portmod_periodic_test_port_tests, CTEST_POSTCOMMIT},

    {"loop", utest_pe_loop_register_unregister, NULL, utest_dnx_portmod_periodic_test_port_option,
     &utest_pe_loop_register_unregister_man,
     NULL,
     utest_dnx_portmod_periodic_test_port_tests, CTEST_POSTCOMMIT},

    {"cb_change", utest_pe_callback_change, NULL, utest_dnx_portmod_periodic_test_port_option,
     &utest_pe_callback_change_man,
     NULL,
     utest_dnx_portmod_periodic_test_port_tests, CTEST_POSTCOMMIT},

    {"interval_forever", utest_pe_interval_forever_check, NULL, utest_dnx_portmod_periodic_test_port_option,
     &utest_pe_interval_forever_check_man,
     NULL,
     utest_dnx_portmod_periodic_test_port_tests, CTEST_POSTCOMMIT},

    {"unregister_from_thread", utest_pe_periodic_unregister, NULL, utest_dnx_portmod_periodic_test_port_option,
     &utest_pe_periodic_unregister_man,
     NULL,
     utest_dnx_portmod_periodic_test_port_tests, CTEST_POSTCOMMIT},

    {"interval_selection", utest_pe_interval_selection, NULL, utest_dnx_portmod_periodic_test_2ports_option,
     &utest_pe_interval_selection_man,
     NULL,
     utest_dnx_portmod_periodic_test_2ports_tests, CTEST_POSTCOMMIT},

    {NULL}
};
