/*
 * $Id: diag_sw_state_tests.c,v 1.20 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:        diag_sw_state_tests.c
 * Purpose:     Diag shell sw state tests
 */

#include "appl/ctest/dnx/ctest_dnx_swstate.h"

#include <shared/bsl.h>
#include <shared/pbmp.h>
#include <shared/utilex/utilex_framework.h>

#include <sal/appl/sal.h>
#include <soc/drv.h>

#include <appl/diag/shell.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <soc/dnx/swstate/auto_generated/access/example_access.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dnx_state_snapshot_manager.h>
#include <soc/dnxc/swstate/dnxc_sw_state_journal.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>

#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

#define CTEST_SW_STATE_FUNC_INIT_VARS(unit) \
    SHR_FUNC_INIT_VARS(unit); \
    sh_process_command(unit, "debug swstatednx +");\
    SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_during_test_set(unit, 1));\
    if (dnx_rollback_journal_start(unit) != _SHR_E_NONE)\
    {\
        SHR_ERR_EXIT(_SHR_E_FAIL, "Could not start err recovery transaction \r\n");\
    }

#define CTEST_SW_STATE_FUNC_EXIT \
    if (dnx_rollback_journal_end(unit, TRUE) != _SHR_E_NONE)\
    {\
        SHR_ERR_EXIT(_SHR_E_FAIL, "Could not roll-back err recovery transaction \r\n");\
    }\
    SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_during_test_set(unit, 0));\
    sh_process_command(unit, "debug swstatednx -"); \
    SHR_FUNC_EXIT

uint8
dnx_sw_state_callback_test_first_function(
    int unit)
{
    return 0;
}

uint8
dnx_sw_state_callback_test_second_function(
    int unit)
{
    return 1;
}

static shr_error_e
sw_state_int_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 value = 0;
    CTEST_SW_STATE_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META("Init the example access module\n")));
    SHR_IF_ERR_EXIT(example.init(unit));

    LOG_CLI((BSL_META("set my_variable to 1234\n")));
    SHR_IF_ERR_EXIT(example.my_variable.set(unit, 1234));

    LOG_CLI((BSL_META("get my_variable\n")));
    SHR_IF_ERR_EXIT(example.my_variable.get(unit, &value));

    LOG_CLI((BSL_META("compare the output to 1234\n")));

    if (value == 1234)
    {
        LOG_CLI((BSL_META("Test PASSED\n")));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    SHR_EXIT();

exit:
    CTEST_SW_STATE_FUNC_EXIT;
}

static shr_error_e
sw_state_array_range_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 source_array[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    uint32 destination_array[10];
    uint32 fill_array[3];
    uint32 indx = 0;
    uint32 bool_variable = 1;
    CTEST_SW_STATE_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META("Init the example access module\n")));
    SHR_IF_ERR_EXIT(example.init(unit));

    LOG_CLI((BSL_META("Write the test source_array to array_range_example\n")));
    SHR_IF_ERR_EXIT(example.array_range_example.range_write(unit, 10, source_array));

    LOG_CLI((BSL_META("Read the array_range_example to destination_array\n")));
    SHR_IF_ERR_EXIT(example.array_range_example.range_read(unit, 10, destination_array));

    LOG_CLI((BSL_META("Fill the second ,third and fourth array_range_example array cells with value 0.\n")));
    SHR_IF_ERR_EXIT(example.array_range_example.range_fill(unit, 1, 3, 0));

    LOG_CLI((BSL_META("Read the second ,third and fourth array_range_example array cells.\n")));
    for (indx = 1; indx < 4; indx++)
    {
        SHR_IF_ERR_EXIT(example.array_range_example.get(unit, indx, &fill_array[indx - 1]));
    }

    LOG_CLI((BSL_META("Compare the source array and the description array\n")));
    for (indx = 0; indx < 10; indx++)
    {
        if (source_array[indx] != destination_array[indx])
        {
            bool_variable = 0;
        }
    }

    LOG_CLI((BSL_META("Check the fill array values. \n")));
    for (indx = 0; indx < 3; indx++)
    {
        if (fill_array[indx] != 0)
        {
            bool_variable = 0;
        }
    }

    if (bool_variable)
    {
        LOG_CLI((BSL_META("Test PASSED \n")));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    SHR_EXIT();

exit:
    CTEST_SW_STATE_FUNC_EXIT;
}

static shr_error_e
sw_state_counter_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 expected_value = 5;
    uint32 retrieved_value = 0;

    CTEST_SW_STATE_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META("Init the example access module\n")));
    SHR_IF_ERR_EXIT(example.init(unit));

    LOG_CLI((BSL_META("Set the counter to 5.\n")));
    SHR_IF_ERR_EXIT(example.counter_test.set(unit, expected_value));

    /*
     * Increment by 1 
     */
    LOG_CLI((BSL_META("Increment the value of the counter by 1.\n")));
    SHR_IF_ERR_EXIT(example.counter_test.inc(unit, 1));
    expected_value++;

    LOG_CLI((BSL_META("Retrieve the counter value to a local variable.\n")));
    SHR_IF_ERR_EXIT(example.counter_test.get(unit, &retrieved_value));

    LOG_CLI((BSL_META("Check is the incremented value correct\n")));
    if (expected_value != retrieved_value)
    {
        LOG_CLI((BSL_META("Unit: %d, EXPECTED VALUE: %d, RETRIEVED VALUE: %d.\n"), unit, expected_value,
                 retrieved_value));
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    /*
     * Decrement by 1 
     */
    LOG_CLI((BSL_META("Decrement the value of the counter by 1.\n")));
    SHR_IF_ERR_EXIT(example.counter_test.dec(unit, 1));
    expected_value--;

    LOG_CLI((BSL_META("Retrieve the counter value to a local variable.\n")));
    SHR_IF_ERR_EXIT(example.counter_test.get(unit, &retrieved_value));

    LOG_CLI((BSL_META("Check is the decremented value correct\n")));
    if (expected_value != retrieved_value)
    {
        LOG_CLI((BSL_META("Unit: %d, EXPECTED VALUE: %d, RETRIEVED VALUE: %d.\n"), unit, expected_value,
                 retrieved_value));
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    /*
     * Increment by 5 
     */
    LOG_CLI((BSL_META("Increment the value of the counter by 1.\n")));
    SHR_IF_ERR_EXIT(example.counter_test.inc(unit, 5));
    expected_value += 5;

    LOG_CLI((BSL_META("Retrieve the counter value to a local variable.\n")));
    SHR_IF_ERR_EXIT(example.counter_test.get(unit, &retrieved_value));

    LOG_CLI((BSL_META("Check is the incremented value correct\n")));
    if (expected_value != retrieved_value)
    {
        LOG_CLI((BSL_META("Unit: %d, EXPECTED VALUE: %d, RETRIEVED VALUE: %d.\n"), unit, expected_value,
                 retrieved_value));
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    /*
     * Decrement by 5 
     */
    LOG_CLI((BSL_META("Decrement the value of the counter by 5.\n")));
    SHR_IF_ERR_EXIT(example.counter_test.dec(unit, 5));
    expected_value -= 5;

    LOG_CLI((BSL_META("Retrieve the counter value to a local variable.\n")));
    SHR_IF_ERR_EXIT(example.counter_test.get(unit, &retrieved_value));

    LOG_CLI((BSL_META("Check is the decremented value correct\n")));
    if (expected_value != retrieved_value)
    {
        LOG_CLI((BSL_META("Unit: %d, EXPECTED VALUE: %d, RETRIEVED VALUE: %d.\n"), unit, expected_value,
                 retrieved_value));
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    LOG_CLI((BSL_META("Test PASSED\n")));

    SHR_EXIT();

exit:
    CTEST_SW_STATE_FUNC_EXIT;
}

static shr_error_e
sw_state_mutex_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    CTEST_SW_STATE_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META("Init the example access module\n")));
    SHR_IF_ERR_EXIT(example.init(unit));

    LOG_CLI((BSL_META("Create new mutex.\n")));
    SHR_IF_ERR_EXIT(example.mutex_test.create(unit));

    LOG_CLI((BSL_META("Take the mutex.\n")));
    SHR_IF_ERR_EXIT(example.mutex_test.take(unit, 0));

    LOG_CLI((BSL_META("Give the mutex.\n")));
    SHR_IF_ERR_EXIT(example.mutex_test.give(unit));

    LOG_CLI((BSL_META("Test PASSED\n")));

    SHR_EXIT();

exit:
    CTEST_SW_STATE_FUNC_EXIT;
}

static shr_error_e
sw_state_input_value_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int value = 0;
    CTEST_SW_STATE_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META("Init the example access module\n")));
    SHR_IF_ERR_EXIT(example.init(unit));

    LOG_CLI((BSL_META("set value_range_test to 1 Range is from 1 to 10\n")));
    SHR_IF_ERR_EXIT(example.value_range_test.set(unit, 1));

    LOG_CLI((BSL_META("set value_range_test to 1 Range is from 1 to 10\n")));
    SHR_IF_ERR_EXIT(example.value_range_test.get(unit, &value));

    LOG_CLI((BSL_META("set value_range_test to 20 and expecting to return error \n")));
    if (value == 1 && (example.value_range_test.set(unit, 20) == _SHR_E_INTERNAL))
    {
        LOG_CLI((BSL_META("Test PASSED\n")));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    SHR_EXIT();

exit:
    CTEST_SW_STATE_FUNC_EXIT;
}

static shr_error_e
sw_state_DNXData_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 nof_elements = 30000;
    uint32 DNXData_array_member = 0;
    uint32 DNXData_array_table_data_member = 0;
    uint32 DNXData_array_alloc_exception_member = 0;

    /*
     * Multi-dimension DNX data arrays
     */
    uint32 dnx_data_size = 0;
    uint32 nof_elements_small = 3;
    uint32 dnxdata_array_dyn_dnxdata_member = 0;
    uint32 dnxdata_array_dnxdata_dyn_member = 0;
    uint32 dnxdata_array_dnxdata_dnxdata_member = 0;
    uint32 dnxdata_array_static_dnxdata_dyn_member = 0;
    uint32 dnxdata_array_static_dyn_dnxdata_member = 0;
    uint32 dnxdata_array_static_dnxdata_dnxdata_member = 0;
    uint32 idx1 = 0;

    CTEST_SW_STATE_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META("Init the example access module\n")));
    SHR_IF_ERR_EXIT(example.init(unit));

    dnx_data_size = dnx_data_module_testing.dbal.vrf_field_size_get(unit);

    /*
     * ALLOC
     */
    LOG_CLI((BSL_META("Allocate the DNXData_array\n")));
    SHR_IF_ERR_EXIT(example.DNXData_array.alloc(unit));

    LOG_CLI((BSL_META("Allocate memory for the DNXData_array_table_data\n")));
    SHR_IF_ERR_EXIT(example.DNXData_array_table_data.alloc(unit));

    LOG_CLI((BSL_META("Allocate memory for the DNXData_array_alloc_exception.\n")));
    SHR_IF_ERR_EXIT(example.DNXData_array_alloc_exception.alloc(unit, (int) nof_elements));

    LOG_CLI((BSL_META("Allocate memory for two-dimensional array : [?][DNX_DATA].\n")));
    example.DNXData_array_dyn_dnxdata.alloc(unit, nof_elements_small);

    LOG_CLI((BSL_META("Allocate memory for two-dimensional array : [DNX_DATA][?].\n")));
    example.DNXData_array_dnxdata_dyn.alloc(unit, nof_elements_small);

    LOG_CLI((BSL_META("Allocate memory for two-dimensional array : [DNX_DATA][DNX_DATA].\n")));
    example.DNXData_array_dnxdata_dnxdata.alloc(unit);

    LOG_CLI((BSL_META
             ("Allocate memory for three-dimensional array : [3][DNX_DATA.module_testing.dbal.vrf_field_size][?].\n")));
    for (idx1 = 0; idx1 < 3; idx1++)
    {
        example.DNXData_array_static_dnxdata_dyn.alloc(unit, (int) idx1, nof_elements_small);
    }

    LOG_CLI((BSL_META
             ("Allocate memory for three-dimensional array : [3][?][DNX_DATA.module_testing.dbal.vrf_field_size].\n")));
    for (idx1 = 0; idx1 < 3; idx1++)
    {
        example.DNXData_array_static_dyn_dnxdata.alloc(unit, (int) idx1, nof_elements_small);
    }

    LOG_CLI((BSL_META
             ("Allocate memory for three-dimensional array : [3][DNX_DATA.module_testing.dbal.vrf_field_size][DNX_DATA.module_testing.dbal.vrf_field_size].\n")));
    for (idx1 = 0; idx1 < 3; idx1++)
    {
        example.DNXData_array_static_dnxdata_dnxdata.alloc(unit, (int) idx1);
    }

    /*
     * SET
     */
    LOG_CLI((BSL_META("Save some data in the first member of the DNXData_array\n")));
    SHR_IF_ERR_EXIT(example.DNXData_array.set(unit, 0, 1));

    LOG_CLI((BSL_META("Save some data in the first member of the DNXData_array_table_data\n")));
    SHR_IF_ERR_EXIT(example.DNXData_array_table_data.set(unit, 0, 2));

    LOG_CLI((BSL_META("Save some data in the first member of the DNXData_array_alloc_exception\n")));
    SHR_IF_ERR_EXIT(example.DNXData_array_alloc_exception.set(unit, 0, 3));

    LOG_CLI((BSL_META("Save some data in the last member of two-dimensional array : [?][DNX_DATA].\n")));
    SHR_IF_ERR_EXIT(example.DNXData_array_dyn_dnxdata.set(unit, nof_elements_small - 1, dnx_data_size - 1, 4));

    LOG_CLI((BSL_META("Save some data in the last member of two-dimensional array : [DNX_DATA][?].\n")));
    SHR_IF_ERR_EXIT(example.DNXData_array_dnxdata_dyn.set(unit, dnx_data_size - 1, nof_elements_small - 1, 5));

    LOG_CLI((BSL_META("Save some data in the last member of two-dimensional array : [DNX_DATA][DNX_DATA].\n")));
    SHR_IF_ERR_EXIT(example.DNXData_array_dnxdata_dnxdata.set(unit, dnx_data_size - 1, dnx_data_size - 1, 6));

    LOG_CLI((BSL_META
             ("Save some data in the last member of three-dimensional array : [3][DNX_DATA.module_testing.dbal.vrf_field_size][?].\n")));
    SHR_IF_ERR_EXIT(example.
                    DNXData_array_static_dnxdata_dyn.set(unit, 2, dnx_data_size - 1, nof_elements_small - 1, 7));

    LOG_CLI((BSL_META
             ("Save some data in the last member of three-dimensional array : [3][?][DNX_DATA.module_testing.dbal.vrf_field_size].\n")));
    SHR_IF_ERR_EXIT(example.
                    DNXData_array_static_dyn_dnxdata.set(unit, 2, nof_elements_small - 1, dnx_data_size - 1, 8));

    LOG_CLI((BSL_META
             ("Save some data in the last member of three-dimensional array : [3][DNX_DATA.module_testing.dbal.vrf_field_size][DNX_DATA.module_testing.dbal.vrf_field_size].\n")));
    SHR_IF_ERR_EXIT(example.DNXData_array_static_dnxdata_dnxdata.set(unit, 2, dnx_data_size - 1, dnx_data_size - 1, 9));

    /*
     * GET
     */
    LOG_CLI((BSL_META("Get the data from the first member of the DNXData_array\n")));
    SHR_IF_ERR_EXIT(example.DNXData_array.get(unit, 0, &DNXData_array_member));

    LOG_CLI((BSL_META("Get the data from the first member of the DNXData_array_table_data\n")));
    SHR_IF_ERR_EXIT(example.DNXData_array_table_data.get(unit, 0, &DNXData_array_table_data_member));

    LOG_CLI((BSL_META("Get the data from  the first member of the DNXData_array_alloc_exception\n")));
    SHR_IF_ERR_EXIT(example.DNXData_array_alloc_exception.get(unit, 0, &DNXData_array_alloc_exception_member));

    LOG_CLI((BSL_META("Get the data from the last member of two-dimensional array : [?][DNX_DATA].\n")));
    SHR_IF_ERR_EXIT(example.DNXData_array_dyn_dnxdata.get(unit, nof_elements_small - 1, dnx_data_size - 1,
                                                          &dnxdata_array_dyn_dnxdata_member));

    LOG_CLI((BSL_META("Get the data from the last member of two-dimensional array : [DNX_DATA][?].\n")));
    SHR_IF_ERR_EXIT(example.DNXData_array_dnxdata_dyn.get(unit, dnx_data_size - 1, nof_elements_small - 1,
                                                          &dnxdata_array_dnxdata_dyn_member));

    LOG_CLI((BSL_META("Get the data from the last member of two-dimensional array : [DNX_DATA][DNX_DATA].\n")));
    SHR_IF_ERR_EXIT(example.DNXData_array_dnxdata_dnxdata.get(unit, dnx_data_size - 1, dnx_data_size - 1,
                                                              &dnxdata_array_dnxdata_dnxdata_member));

    LOG_CLI((BSL_META
             ("Get the data from the last member of three-dimensional array : [3][DNX_DATA.module_testing.dbal.vrf_field_size][?].\n")));
    SHR_IF_ERR_EXIT(example.DNXData_array_static_dnxdata_dyn.get(unit, 2, dnx_data_size - 1, nof_elements_small - 1,
                                                                 &dnxdata_array_static_dnxdata_dyn_member));

    LOG_CLI((BSL_META
             ("Get the data from the last member of three-dimensional array : [3][?][DNX_DATA.module_testing.dbal.vrf_field_size].\n")));
    SHR_IF_ERR_EXIT(example.DNXData_array_static_dyn_dnxdata.get(unit, 2, nof_elements_small - 1, dnx_data_size - 1,
                                                                 &dnxdata_array_static_dyn_dnxdata_member));

    LOG_CLI((BSL_META
             ("Get the data from the last member of three-dimensional array : [3][?][DNX_DATA.module_testing.dbal.vrf_field_size].\n")));
    SHR_IF_ERR_EXIT(example.DNXData_array_static_dnxdata_dnxdata.get(unit, 2, dnx_data_size - 1, dnx_data_size - 1,
                                                                     &dnxdata_array_static_dnxdata_dnxdata_member));

    LOG_CLI((BSL_META
             ("Check the readed and saved data in DNXData_array and DNXData_array_alloc_exception are equal.\n")));
    if ((DNXData_array_member == 1) && (DNXData_array_table_data_member == 2)
        && (DNXData_array_alloc_exception_member == 3) && (dnxdata_array_dyn_dnxdata_member == 4)
        && (dnxdata_array_dnxdata_dyn_member == 5) && (dnxdata_array_dnxdata_dnxdata_member == 6)
        && (dnxdata_array_static_dnxdata_dyn_member == 7) && (dnxdata_array_static_dyn_dnxdata_member == 8)
        && (dnxdata_array_static_dnxdata_dnxdata_member == 9))
    {
        LOG_CLI((BSL_META("Test PASSED\n")));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    SHR_EXIT();

exit:
    CTEST_SW_STATE_FUNC_EXIT;
}

static shr_error_e
sw_state_alloc_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 test_variable = 0;
    CTEST_SW_STATE_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META("Init the example access module\n")));
    SHR_IF_ERR_EXIT(example.init(unit));

    /*
     * for this specific test turn off the alloc after init exception 
     */
    SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_during_test_set(unit, 0));

    LOG_CLI((BSL_META("Allocate memory after init\n")));


    if (example.alloc_after_init_variable.alloc(unit, 10) == _SHR_E_NONE)
    {
        LOG_CLI((BSL_META("Set a value to the first member of the alloc_after_init_variable array.\n")));
        SHR_IF_ERR_EXIT(example.alloc_after_init_variable.set(unit, 0, 100));

        LOG_CLI((BSL_META("Get the value from the first member of the alloc_after_init_variable array.\n")));
        SHR_IF_ERR_EXIT(example.alloc_after_init_variable.get(unit, 0, &test_variable));

        LOG_CLI((BSL_META("Compare the set and get values.\n")));
        if (test_variable == 100)
        {
            LOG_CLI((BSL_META("Test PASSED\n")));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    SHR_EXIT();

exit:
    CTEST_SW_STATE_FUNC_EXIT;
}

static shr_error_e
sw_state_pointer_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint8 is_allocated = 0;
    uint32 test_variable = 0;
    CTEST_SW_STATE_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META("Init the example access module.\n")));
    SHR_IF_ERR_EXIT(example.init(unit));

    LOG_CLI((BSL_META("Allocate the pointer variable.\n")));
    SHR_IF_ERR_EXIT(example.pointer.alloc(unit));

    LOG_CLI((BSL_META("Check is the pointer allocated.\n")));
    SHR_IF_ERR_EXIT(example.pointer.is_allocated(unit, &is_allocated));
    if (is_allocated == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    LOG_CLI((BSL_META("Set a value to the pointer variable.\n")));
    SHR_IF_ERR_EXIT(example.pointer.set(unit, 100));

    LOG_CLI((BSL_META("Get the value from pointer variable.\n")));
    SHR_IF_ERR_EXIT(example.pointer.get(unit, &test_variable));

    LOG_CLI((BSL_META("Compare the set and get values.\n")));
    if (test_variable == 100)
    {
        LOG_CLI((BSL_META("Test PASSED\n")));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    SHR_EXIT();

exit:
    CTEST_SW_STATE_FUNC_EXIT;
}

static shr_error_e
sw_state_arrays_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 input_array[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    uint32 output_array[10];
    uint32 nof_elements = 10;
    uint32 indx = 0;
    uint32 indx2 = 0;
    uint32 bool_variable = 1;
    CTEST_SW_STATE_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META("Init the example access module.\n")));
    SHR_IF_ERR_EXIT(example.init(unit));

    LOG_CLI((BSL_META("Write the test input_array to my_array.\n")));
    for (indx = 0; indx < nof_elements; indx++)
    {
        SHR_IF_ERR_EXIT(example.my_array.set(unit, indx, input_array[indx]));
    }

    LOG_CLI((BSL_META("Save the my_array's cells in to output_array\n")));
    for (indx = 0; indx < nof_elements; indx++)
    {
        SHR_IF_ERR_EXIT(example.my_array.get(unit, indx, &output_array[indx]));
    }

    LOG_CLI((BSL_META("Compare the input array and the output array\n")));
    for (indx = 0; indx < nof_elements; indx++)
    {
        if (input_array[indx] != output_array[indx])
        {
            bool_variable = 0;
        }
    }

    LOG_CLI((BSL_META("Write the test input_array in each row of the two_dimentional_array.\n")));
    for (indx = 0; indx < nof_elements; indx++)
    {
        for (indx2 = 0; indx2 < nof_elements; indx2++)
        {
            SHR_IF_ERR_EXIT(example.two_dimentional_array.set(unit, indx, indx2, input_array[indx2]));
        }
    }

    LOG_CLI((BSL_META("Save a row of two_dimentional_array in to output_array\n")));
    for (indx = 0; indx < nof_elements; indx++)
    {
        SHR_IF_ERR_EXIT(example.two_dimentional_array.get(unit, 5, indx, &output_array[indx]));
    }

    LOG_CLI((BSL_META("Compare the input array and the output array\n")));
    for (indx = 0; indx < nof_elements; indx++)
    {
        if (input_array[indx] != output_array[indx])
        {
            bool_variable = 0;
        }
    }

    LOG_CLI((BSL_META("Allocate the dynamic_array array.\n")));
    SHR_IF_ERR_EXIT(example.dynamic_array.alloc(unit, nof_elements));

    LOG_CLI((BSL_META("Save the test input_array to dynamic_array array.\n")));
    for (indx = 0; indx < nof_elements; indx++)
    {
        SHR_IF_ERR_EXIT(example.dynamic_array.set(unit, indx, input_array[indx]));
    }

    LOG_CLI((BSL_META("Save the dynamic_array's cells in to output_array\n")));
    for (indx = 0; indx < nof_elements; indx++)
    {
        SHR_IF_ERR_EXIT(example.dynamic_array.get(unit, indx, &output_array[indx]));
    }

    LOG_CLI((BSL_META("Compare the input array and the output array\n")));
    for (indx = 0; indx < nof_elements; indx++)
    {
        if (input_array[indx] != output_array[indx])
        {
            bool_variable = 0;
        }
    }

    LOG_CLI((BSL_META("Allocate the dynamic_array_static array.\n")));
    SHR_IF_ERR_EXIT(example.dynamic_array_static.alloc(unit, nof_elements));

    LOG_CLI((BSL_META("Save the test input_array to dynamic_array_static array.\n")));
    for (indx = 0; indx < nof_elements; indx++)
    {
        SHR_IF_ERR_EXIT(example.dynamic_array_static.set(unit, indx, 0, input_array[indx]));
    }

    LOG_CLI((BSL_META("Save the dynamic_array_static's cells in to output_array\n")));
    for (indx = 0; indx < nof_elements; indx++)
    {
        SHR_IF_ERR_EXIT(example.dynamic_array_static.get(unit, indx, 0, &output_array[indx]));
    }

    LOG_CLI((BSL_META("Compare the input array and the output array\n")));
    for (indx = 0; indx < nof_elements; indx++)
    {
        if (input_array[indx] != output_array[indx])
        {
            bool_variable = 0;
        }
    }

    LOG_CLI((BSL_META("Allocate the dynamic_dynamic_array array.\n")));
    SHR_IF_ERR_EXIT(example.dynamic_dynamic_array.alloc(unit, nof_elements, nof_elements));

    LOG_CLI((BSL_META("Allocate the dynamic_dynamic_array array.\n")));
    for (indx = 0; indx < nof_elements; indx++)
    {
        for (indx2 = 0; indx2 < nof_elements; indx2++)
        {
            SHR_IF_ERR_EXIT(example.dynamic_dynamic_array.set(unit, indx, indx2, input_array[indx2]));
        }
    }

    LOG_CLI((BSL_META("Save a row of dynamic_dynamic_array in to output_array\n")));
    for (indx = 0; indx < nof_elements; indx++)
    {
        SHR_IF_ERR_EXIT(example.dynamic_dynamic_array.get(unit, 5, indx, &output_array[indx]));
    }

    LOG_CLI((BSL_META("Compare the input array and the output array\n")));
    for (indx = 0; indx < nof_elements; indx++)
    {
        if (input_array[indx] != output_array[indx])
        {
            bool_variable = 0;
        }
    }

    if (bool_variable)
    {
        LOG_CLI((BSL_META("Test PASSED \n")));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    SHR_EXIT();

exit:
    CTEST_SW_STATE_FUNC_EXIT;
}

static shr_error_e
sw_state_pbmp_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int count = 0;
    uint8 test_variable = 0;
    uint32 range = sizeof(_shr_pbmp_t);
    _shr_pbmp_t test_pbmp;

    CTEST_SW_STATE_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META("Init the example access module.\n")));
    SHR_IF_ERR_EXIT(example.init(unit));

    LOG_CLI((BSL_META("Clear swstate port bitmap and test bitmap.\n")));
    SHR_IF_ERR_EXIT(example.pbmp_variable.pbmp_clear(unit));
    _SHR_PBMP_CLEAR(test_pbmp);

    LOG_CLI((BSL_META("Check swstate port bitmap and test bitmap have been cleared, compare them.\n")));
    SHR_IF_ERR_EXIT(example.pbmp_variable.pbmp_eq(unit, test_pbmp, &test_variable));
    if (0 == test_variable)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    LOG_CLI((BSL_META("Range add all ports to swstate port bitmap starting from index 0.\n")));
    SHR_IF_ERR_EXIT(example.pbmp_variable.pbmp_ports_range_add(unit, 0, range));
    SHR_IF_ERR_EXIT(example.pbmp_variable.pbmp_member(unit, 0, &test_variable));
    if (0 == test_variable)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    LOG_CLI((BSL_META("Copy to test port bitmap.\n")));
    SHR_IF_ERR_EXIT(example.pbmp_variable.pbmp_get(unit, &test_pbmp));

    LOG_CLI((BSL_META("Remove port 0 from pbmp.\n")));
    SHR_IF_ERR_EXIT(example.pbmp_variable.pbmp_port_remove(unit, 0));

    LOG_CLI((BSL_META("Test pbmp AND.\n")));
    SHR_IF_ERR_EXIT(example.pbmp_variable.pbmp_and(unit, test_pbmp));
    SHR_IF_ERR_EXIT(example.pbmp_variable.pbmp_member(unit, 0, &test_variable));
    if (0 != test_variable)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    LOG_CLI((BSL_META("Test pbmp OR.\n")));
    SHR_IF_ERR_EXIT(example.pbmp_variable.pbmp_or(unit, test_pbmp));
    SHR_IF_ERR_EXIT(example.pbmp_variable.pbmp_member(unit, 0, &test_variable));
    if (0 == test_variable)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    LOG_CLI((BSL_META("Test pbmp XOR and COUNT.\n")));
    SHR_IF_ERR_EXIT(example.pbmp_variable.pbmp_clear(unit));
    SHR_IF_ERR_EXIT(example.pbmp_variable.pbmp_xor(unit, test_pbmp));
    SHR_IF_ERR_EXIT(example.pbmp_variable.pbmp_count(unit, &count));
    if (count != range)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    LOG_CLI((BSL_META("Test pbmp FLIP.\n")));
    SHR_IF_ERR_EXIT(example.pbmp_variable.pbmp_port_flip(unit, 0));
    SHR_IF_ERR_EXIT(example.pbmp_variable.pbmp_member(unit, 0, &test_variable));
    if (0 != test_variable)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    LOG_CLI((BSL_META("Test pbmp NEGATE.\n")));
    SHR_IF_ERR_EXIT(example.pbmp_variable.pbmp_negate(unit, test_pbmp));
    SHR_IF_ERR_EXIT(example.pbmp_variable.pbmp_member(unit, 0, &test_variable));
    if (0 != test_variable)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    LOG_CLI((BSL_META("Test pbmp port set.\n")));
    SHR_IF_ERR_EXIT(example.pbmp_variable.pbmp_port_set(unit, 0));
    SHR_IF_ERR_EXIT(example.pbmp_variable.pbmp_member(unit, 0, &test_variable));
    if (0 == test_variable)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    LOG_CLI((BSL_META("Test pbmp ASSIGN.\n")));
    SHR_IF_ERR_EXIT(example.pbmp_variable.pbmp_assign(unit, test_pbmp));
    SHR_IF_ERR_EXIT(example.pbmp_variable.pbmp_member(unit, 0, &test_variable));
    if (0 == test_variable)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    LOG_CLI((BSL_META("Test pbmp is null.\n")));
    SHR_IF_ERR_EXIT(example.pbmp_variable.pbmp_is_null(unit, &test_variable));
    if (0 != test_variable)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    LOG_CLI((BSL_META("Test PASSED\n")));

exit:
    CTEST_SW_STATE_FUNC_EXIT;
}

static shr_error_e
sw_state_bitmap_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 nof_bits_to_alloc = 32;
    int range = 32;
    int count = 0;
    uint8 is_allocated = 0;
    uint8 test_variable = 0;
    uint8 test_variable_fixed = 0;
    SHR_BITDCL input_bmp = 20;
    SHR_BITDCL output_bmp = 0;

    uint32 static_bitmap_size = 1024;

    CTEST_SW_STATE_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META("Init the example access module.\n")));
    SHR_IF_ERR_EXIT(example.init(unit));

    LOG_CLI((BSL_META("Allocate the memory for the dnx data bitmap variable.\n")));
    SHR_IF_ERR_EXIT(example.bitmap_dnx_data.alloc_bitmap(unit));

    LOG_CLI((BSL_META("Check the allocation of the dnx data bitmap variable.\n")));
    SHR_IF_ERR_EXIT(example.bitmap_dnx_data.is_allocated(unit, &is_allocated));
    if (is_allocated == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    LOG_CLI((BSL_META("Allocate the memory for the bitmap variable.\n")));
    SHR_IF_ERR_EXIT(example.bitmap_variable.alloc_bitmap(unit, nof_bits_to_alloc));

    LOG_CLI((BSL_META("Check the allocation of the bitmap variable.\n")));
    SHR_IF_ERR_EXIT(example.bitmap_variable.is_allocated(unit, &is_allocated));
    if (is_allocated == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    LOG_CLI((BSL_META("Set the bit 0 of the bitmap variable.\n")));
    SHR_IF_ERR_EXIT(example.bitmap_variable.bit_set(unit, 0));

    LOG_CLI((BSL_META("Set the last bit of the STATIC size bitmap variable.\n")));
    SHR_IF_ERR_EXIT(example.bitmap_fixed.bit_set(unit, static_bitmap_size - 1));

    LOG_CLI((BSL_META("Get the bit 0 of the bitmap variable.\n")));
    SHR_IF_ERR_EXIT(example.bitmap_variable.bit_get(unit, 0, &test_variable));

    LOG_CLI((BSL_META("Get the last bit of the STATIC size bitmap variable.\n")));
    SHR_IF_ERR_EXIT(example.bitmap_fixed.bit_get(unit, static_bitmap_size - 1, &test_variable_fixed));

    LOG_CLI((BSL_META("Check the get variable.\n")));
    if ((test_variable != 1) || (test_variable_fixed != 1))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    LOG_CLI((BSL_META("Clear the bit 0 of the bitmap variable.\n")));
    SHR_IF_ERR_EXIT(example.bitmap_variable.bit_clear(unit, 0));

    LOG_CLI((BSL_META("Clear the last bit the STATIC size bitmap variable.\n")));
    SHR_IF_ERR_EXIT(example.bitmap_fixed.bit_clear(unit, static_bitmap_size - 1));

    LOG_CLI((BSL_META("Get the bit 0 of the bitmap variable.\n")));
    SHR_IF_ERR_EXIT(example.bitmap_variable.bit_get(unit, 0, &test_variable));

    LOG_CLI((BSL_META("Get the last bit of the STATIC size bitmap variable.\n")));
    SHR_IF_ERR_EXIT(example.bitmap_fixed.bit_get(unit, static_bitmap_size - 1, &test_variable_fixed));

    LOG_CLI((BSL_META("Check the get variable.\n")));
    if ((test_variable != 0) || (test_variable_fixed != 0))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    LOG_CLI((BSL_META("Write the input_bitmap in the sw state bitmap variable.\n")));
    SHR_IF_ERR_EXIT(example.bitmap_variable.bit_range_write(unit, 0, 0, range, &input_bmp));

    LOG_CLI((BSL_META("Read the sw state bitmap variable in the output_bmp.\n")));
    SHR_IF_ERR_EXIT(example.bitmap_variable.bit_range_read(unit, 0, 0, range, &output_bmp));

    LOG_CLI((BSL_META("Compare the output_bmp and input_bmp.\n")));
    if (output_bmp != input_bmp)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    LOG_CLI((BSL_META("Clear the data in the sw state bitmap variable.\n")));
    SHR_IF_ERR_EXIT(example.bitmap_variable.bit_range_clear(unit, 0, range));

    LOG_CLI((BSL_META("Bitwise \"and\" between the sw state bitmap variable and input_bmp. The result sould be 0.\n")));
    SHR_IF_ERR_EXIT(example.bitmap_variable.bit_range_and(unit, 0, range, &input_bmp));

    LOG_CLI((BSL_META("Read the sw state bitmap variable in the output_bmp.\n")));
    SHR_IF_ERR_EXIT(example.bitmap_variable.bit_range_read(unit, 0, 0, range, &output_bmp));

    LOG_CLI((BSL_META("Check the output from the bitwise operation \"and\".\n")));
    if (output_bmp != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    LOG_CLI((BSL_META("Bitwise \"or\" between the sw state bitmap variable and input_bmp.\n")));
    SHR_IF_ERR_EXIT(example.bitmap_variable.bit_range_or(unit, 0, range, &input_bmp));

    LOG_CLI((BSL_META("Read the sw state bitmap variable in the output_bmp.\n")));
    SHR_IF_ERR_EXIT(example.bitmap_variable.bit_range_read(unit, 0, 0, range, &output_bmp));

    LOG_CLI((BSL_META("Check the output from the bitwise operation \"or\".\n")));
    if (output_bmp != 20)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    LOG_CLI((BSL_META("Write the input_bitmap in the sw state bitmap variable.\n")));
    SHR_IF_ERR_EXIT(example.bitmap_variable.bit_range_write(unit, 0, 0, range, &input_bmp));

    LOG_CLI((BSL_META("Negate the data in the sw state bitmap variable.\n")));
    SHR_IF_ERR_EXIT(example.bitmap_variable.bit_range_negate(unit, 0, range));

    LOG_CLI((BSL_META("Bitwise \"and\" between the sw state bitmap variable and input_bmp.\n")));
    SHR_IF_ERR_EXIT(example.bitmap_variable.bit_range_and(unit, 0, range, &input_bmp));

    LOG_CLI((BSL_META("Read the sw state bitmap variable in the output_bmp.\n")));
    SHR_IF_ERR_EXIT(example.bitmap_variable.bit_range_read(unit, 0, 0, range, &output_bmp));

    LOG_CLI((BSL_META("Check the output from the bitwise operation \"and\".\n")));
    if (output_bmp != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    LOG_CLI((BSL_META("Write the input_bitmap in the sw state bitmap variable.\n")));
    SHR_IF_ERR_EXIT(example.bitmap_variable.bit_range_write(unit, 0, 0, range, &input_bmp));

    LOG_CLI((BSL_META("Count the bitmap \"ones\".\n")));
    SHR_IF_ERR_EXIT(example.bitmap_variable.bit_range_count(unit, 0, range, &count));

    LOG_CLI((BSL_META("Check count of the \"ones\".\n")));
    if (count != 2)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    LOG_CLI((BSL_META("Test PASSED\n")));

exit:
    CTEST_SW_STATE_FUNC_EXIT;
}

static shr_error_e
sw_state_buffer_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint8 is_allocated = 0;
    uint32 nof_buffer_elements = 10;
    int cmp_result = 1;
    uint8 input_array[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    uint8 output_array[10];
    uint32 indx = 0;
    CTEST_SW_STATE_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META("Init the example access module.\n")));
    SHR_IF_ERR_EXIT(example.init(unit));

    LOG_CLI((BSL_META("Allocate the buffer variable.\n")));
    SHR_IF_ERR_EXIT(example.buffer.alloc(unit, nof_buffer_elements));

    LOG_CLI((BSL_META("Check is the buffer allocated.\n")));
    SHR_IF_ERR_EXIT(example.buffer.is_allocated(unit, &is_allocated));
    if (is_allocated == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    LOG_CLI((BSL_META("Initiate the value of the buffer elements.\n")));
    SHR_IF_ERR_EXIT(example.buffer.memset(unit, 0, nof_buffer_elements, 0));

    LOG_CLI((BSL_META("Write the input array in the buffer.\n")));
    SHR_IF_ERR_EXIT(example.buffer.memwrite(unit, input_array, 0, nof_buffer_elements));

    LOG_CLI((BSL_META("Compare the buffer and input array.\n")));
    example.buffer.memcmp(unit, input_array, 0, nof_buffer_elements, &cmp_result);

    if (cmp_result == 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    LOG_CLI((BSL_META("Save the buffer variables in the output array.\n")));
    SHR_IF_ERR_EXIT(example.buffer.memread(unit, output_array, 0, nof_buffer_elements));

    LOG_CLI((BSL_META("Compare the input and output arrays.\n")));
    for (indx = 0; indx < nof_buffer_elements; indx++)
    {
        if (input_array[indx] != output_array[indx])
        {
            cmp_result = 1;
        }
    }

    if (cmp_result == 0)
    {
        LOG_CLI((BSL_META("Test PASSED\n")));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    SHR_EXIT();

exit:
    CTEST_SW_STATE_FUNC_EXIT;
}

static shr_error_e
sw_state_default_value_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int cmp_result = 1;
    uint8 is_allocated = 0;
    uint8 test_variable = 0;
    uint32 nof_bits_to_alloc = 32;
    uint32 variable = 0;

    uint32 nof_buffer_elements = 10;
    uint8 input_array_default_value[10] = { 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA };

    CTEST_SW_STATE_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META("Init the example access module\n")));
    SHR_IF_ERR_EXIT(example.init(unit));

    /********************************/
    /*
     * check default value for init
     */
    /********************************/
    LOG_CLI((BSL_META("get default value for init\n")));
    SHR_IF_ERR_EXIT(example.default_value_tree.default_value_l1.my_variable.get(unit, &variable));

    if (variable != 5)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    /*****************************************/
    /*
     * check default value for DNXdata array
     */
    /*****************************************/
    LOG_CLI((BSL_META("alloc memory for DNXdata array\n")));
    SHR_IF_ERR_EXIT(example.default_value_tree.array_dnxdata.alloc(unit));

    LOG_CLI((BSL_META("get default value for DNXData array\n")));
    SHR_IF_ERR_EXIT(example.default_value_tree.array_dnxdata.default_value_l2.my_variable.get(unit, 0, &variable));

    if (variable != 5)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    /*****************************************/
    /*
     * check default value for dynamic array
     */
    /*****************************************/
    LOG_CLI((BSL_META("alloc memory for dynamic array\n")));
    SHR_IF_ERR_EXIT(example.default_value_tree.array_dnxdata.array_dynamic.alloc(unit, 0, 5));

    LOG_CLI((BSL_META("get default value for static array\n")));
    SHR_IF_ERR_EXIT(example.default_value_tree.array_dnxdata.array_dynamic.array_static_static.
                    my_variable.get(unit, 0, 0, 0, 0, &variable));

    if (variable != 5)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    /**********************************/
    /*
     * check default value for bitmap
     */
    /**********************************/
    LOG_CLI((BSL_META("Allocate the memory for the bitmap variable.\n")));
    SHR_IF_ERR_EXIT(example.bitmap_variable.alloc_bitmap(unit, nof_bits_to_alloc));

    LOG_CLI((BSL_META("Check the allocation of the bitmap variable.\n")));
    SHR_IF_ERR_EXIT(example.bitmap_variable.is_allocated(unit, &is_allocated));
    if (is_allocated == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    LOG_CLI((BSL_META("Get the default value at bit 0 of the bitmap variable.\n")));
    SHR_IF_ERR_EXIT(example.bitmap_variable.bit_get(unit, 0, &test_variable));

    LOG_CLI((BSL_META("Check the default value for bitmap.\n")));
    if (test_variable != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    /**********************************/
    /*
     * check default value for buffer
     */
    /**********************************/
    LOG_CLI((BSL_META("Allocate the buffer variable.\n")));
    SHR_IF_ERR_EXIT(example.buffer.alloc(unit, nof_buffer_elements));

    LOG_CLI((BSL_META("Check is the buffer allocated.\n")));
    SHR_IF_ERR_EXIT(example.buffer.is_allocated(unit, &is_allocated));
    if (is_allocated == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    LOG_CLI((BSL_META("Compare the default values stored in the buffer and default value input array.\n")));
    example.buffer.memcmp(unit, input_array_default_value, 0, nof_buffer_elements, &cmp_result);
    if (cmp_result == 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    LOG_CLI((BSL_META("Test PASSED\n")));

    SHR_EXIT();

exit:
    CTEST_SW_STATE_FUNC_EXIT;
}

static shr_error_e
sw_state_ll_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    sw_state_ll_init_info_t init_info;
    sw_state_ll_node_t output_node;
    int value = 0;
    int value1 = 1;
    int value2 = 2;
    int value3 = 3;
    int value4 = 4;
    int value5 = 5;
    int value6 = 6;
    int value7 = 7;

    CTEST_SW_STATE_FUNC_INIT_VARS(unit);

    sal_memset(&init_info, 0, sizeof(sw_state_ll_init_info_t));
    init_info.max_nof_elements = 10;
    init_info.expected_nof_elements = 10;

    LOG_CLI((BSL_META("Init the example access module\n")));
    SHR_IF_ERR_EXIT(example.init(unit));

    LOG_CLI((BSL_META("init the linked list\n")));
    SHR_IF_ERR_EXIT(example.ll.create_empty(unit, &init_info));

    LOG_CLI((BSL_META("insert 1 & 4\n")));
    SHR_IF_ERR_EXIT(example.ll.add_first(unit, &value1));
    SHR_IF_ERR_EXIT(example.ll.add_last(unit, &value4));

    LOG_CLI((BSL_META("get the list's head\n")));
    SHR_IF_ERR_EXIT(example.ll.get_first(unit, &output_node));

    LOG_CLI((BSL_META("add the value 2 after head\n")));
    SHR_IF_ERR_EXIT(example.ll.add_after(unit, output_node, &value2));

    LOG_CLI((BSL_META("get the head's next\n")));
    SHR_IF_ERR_EXIT(example.ll.next_node(unit, output_node, &output_node));

    LOG_CLI((BSL_META("verify the data of next is 2\n")));
    SHR_IF_ERR_EXIT(example.ll.node_value(unit, output_node, &value));
    if (value != value2)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - unexpected value in node\n");
    }

    LOG_CLI((BSL_META("add the value 3 after 2\n")));
    SHR_IF_ERR_EXIT(example.ll.add_after(unit, output_node, &value3));

    LOG_CLI((BSL_META("insert value 5 at the end of the list\n")));
    SHR_IF_ERR_EXIT(example.ll.add_last(unit, &value5));

    LOG_CLI((BSL_META("insert value 7 at the end of the list\n")));
    SHR_IF_ERR_EXIT(example.ll.add_last(unit, &value7));

    LOG_CLI((BSL_META("get the list's tail\n")));
    SHR_IF_ERR_EXIT(example.ll.get_last(unit, &output_node));

    LOG_CLI((BSL_META("add the value 6 before 7 (current tail)\n")));
    SHR_IF_ERR_EXIT(example.ll.add_before(unit, output_node, &value6));

    LOG_CLI((BSL_META("print the list, expect to see 1->2->3->4->5->6->7, visual only\n")));
    SHR_IF_ERR_EXIT(example.ll.print(unit));

    LOG_CLI((BSL_META("remove the second node\n")));
    SHR_IF_ERR_EXIT(example.ll.get_first(unit, &output_node));
    SHR_IF_ERR_EXIT(example.ll.next_node(unit, output_node, &output_node));
    SHR_IF_ERR_EXIT(example.ll.remove_node(unit, output_node));

    LOG_CLI((BSL_META("Test PASSED\n")));

    SHR_EXIT();

exit:
    if (SHR_GET_CURRENT_ERR())
    {
        LOG_CLI((BSL_META("Test FAILED\n")));
    }
    CTEST_SW_STATE_FUNC_EXIT;

}

static shr_error_e
sw_state_multihead_ll_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    sw_state_ll_init_info_t init_info;
    sw_state_ll_node_t output_node;
    int ll_index;
    int value = 0;
    int value1 = 1;
    int value2 = 2;
    int value3 = 3;
    int value4 = 4;
    int value5 = 5;
    int value6 = 6;
    int value7 = 7;

    CTEST_SW_STATE_FUNC_INIT_VARS(unit);

    sal_memset(&init_info, 0, sizeof(sw_state_ll_init_info_t));
    init_info.max_nof_elements = 15;
    init_info.expected_nof_elements = 14;
    init_info.nof_heads = 2;

    LOG_CLI((BSL_META("Init the example access module\n")));
    SHR_IF_ERR_EXIT(example.init(unit));

    LOG_CLI((BSL_META("init the linked list\n")));
    SHR_IF_ERR_EXIT(example.multihead_ll.create_empty(unit, &init_info));

    LOG_CLI((BSL_META("insert 1 & 4 to list #0\n")));
    SHR_IF_ERR_EXIT(example.multihead_ll.add_first(unit, 0, &value1));
    SHR_IF_ERR_EXIT(example.multihead_ll.add_last(unit, 0, &value4));

    LOG_CLI((BSL_META("insert 4 & 1 to list #1\n")));
    SHR_IF_ERR_EXIT(example.multihead_ll.add_first(unit, 1, &value4));
    SHR_IF_ERR_EXIT(example.multihead_ll.add_last(unit, 1, &value1));

    for (ll_index = 0; ll_index < 2; ll_index++)
    {
        LOG_CLI((BSL_META("get list #0's head\n")));
        SHR_IF_ERR_EXIT(example.multihead_ll.get_first(unit, ll_index, &output_node));

        LOG_CLI((BSL_META("add the value 2 after head\n")));
        SHR_IF_ERR_EXIT(example.multihead_ll.add_after(unit, ll_index, output_node, &value2));

        LOG_CLI((BSL_META("get the head's next\n")));
        SHR_IF_ERR_EXIT(example.multihead_ll.next_node(unit, ll_index, output_node, &output_node));

        LOG_CLI((BSL_META("verify the data of next is 2\n")));
        SHR_IF_ERR_EXIT(example.multihead_ll.node_value(unit, output_node, &value));
        if (value != value2)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - unexpected value in node\n");
        }

        LOG_CLI((BSL_META("add the value 3 after 2\n")));
        SHR_IF_ERR_EXIT(example.multihead_ll.add_after(unit, ll_index, output_node, &value3));

        LOG_CLI((BSL_META("insert value 5 at the end of the list\n")));
        SHR_IF_ERR_EXIT(example.multihead_ll.add_last(unit, ll_index, &value5));

        LOG_CLI((BSL_META("insert value 7 at the end of the list\n")));
        SHR_IF_ERR_EXIT(example.multihead_ll.add_last(unit, ll_index, &value7));

        LOG_CLI((BSL_META("get the list's tail\n")));
        SHR_IF_ERR_EXIT(example.multihead_ll.get_last(unit, ll_index, &output_node));

        LOG_CLI((BSL_META("add the value 6 before 7 (current tail)\n")));
        SHR_IF_ERR_EXIT(example.multihead_ll.add_before(unit, ll_index, output_node, &value6));
    }

    LOG_CLI((BSL_META("print the list, expect to see 1->2->3->4->5->6->7, visual only\n")));
    SHR_IF_ERR_EXIT(example.multihead_ll.print(unit));

    LOG_CLI((BSL_META("remove the second node (only on list #0)\n")));
    SHR_IF_ERR_EXIT(example.multihead_ll.get_first(unit, 0, &output_node));
    SHR_IF_ERR_EXIT(example.multihead_ll.next_node(unit, 0, output_node, &output_node));
    SHR_IF_ERR_EXIT(example.multihead_ll.remove_node(unit, 0, output_node));

    LOG_CLI((BSL_META("Test PASSED\n")));

    SHR_EXIT();

exit:
    if (SHR_GET_CURRENT_ERR())
    {
        LOG_CLI((BSL_META("Test FAILED\n")));
    }
    CTEST_SW_STATE_FUNC_EXIT;

}

/*
* Verification that a MC-Group that requires more than available will fail to open
* 1.  Open many large groups (8*2K Ingress) and (7*2K Egress) MC-Groups.
*     And open one more Egress MC-Group with 1 entry (MC-ID = 16K-2K = 14K).
* 2.  Then try and open an Ingress MC-Group with 2K entries (MC-ID = 1).
*     Expect INSUFFICIENT MEMORY value to be TRUE.
* 3.  Delete the Egress group with 1 entry (MC-ID = 14K).
* 4.  Open an Ingress MC-Group with 2K entries (MC-ID = 1). Expect success.
*/

static shr_error_e
sw_state_occupation_bitmap_test1_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 indx, place, num_bits;
    uint8 found;
    sw_state_occ_bitmap_init_info_t btmp_init_info;

    CTEST_SW_STATE_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META("Init the example access module\n")));
    SHR_IF_ERR_EXIT(example.init(unit));

    num_bits = 64;
    SHR_IF_ERR_EXIT(sw_state_occ_bm_init_info_clear(&btmp_init_info));

    btmp_init_info.size = num_bits;

    SHR_IF_ERR_EXIT(example.occ.create(unit, &btmp_init_info));

    for (indx = 0; indx < (num_bits + 2); ++indx)
    {
        SHR_IF_ERR_EXIT(example.occ.get_next(unit, &place, &found));
        if (indx >= num_bits)
        {
            /*
             * At this point, the bitmap is full. We should get a zero 'found'
             * and, then, loop.
             */

            LOG_CLI((BSL_META_U(unit,
                                "sw_state_occ_bm_test_1: Trying to find '0' on a full bitmap. indx %d num_bits %d found %d\r\n"),
                     (int) indx, (int) num_bits, (int) found));

            if (found)
            {

                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                             "Failed in sw_state_occ_bm_get_next: Got non-zero 'found' on a full bitmap. indx %d num_bits %d found %d\r\n",
                             (int) indx, (int) num_bits, (int) found);
            }
            continue;
        }

        if (indx == 19)
        {
            /*
             * At this point, 19 bits have bit set (from BIT0 to BIT18)
             * so the image of the bit map is:
             * 0xFF 0xFF 0x07
             */
            uint32 local_place, start, end, ii;
            uint8 local_found;
            uint8 val;
            uint8 forward[2];
            forward[0] = TRUE;
            forward[1] = FALSE;
            for (ii = 0; ii < 2; ii++)
            {
                start = 14;
                end = 21;
                SHR_IF_ERR_EXIT(example.
                                occ.get_next_in_range(unit, start, end, forward[ii], 0, &local_place, &local_found));
                if (ii == 0)
                {
                    /*
                     * This is a forward search so the first '0' is the one following the
                     * last bit that was set (BIT18)
                     */
                    if (!(local_found && (local_place == 19)))
                    {

                        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                     "Failed in sw_state_occ_bm_destroy: Found should be TRUE and place should be 19\r\n"
                                     "==> local_found %d local_place %d\r\n", (int) local_found, (int) local_place);

                    }
                }
                else
                {
                    /*
                     * This is a backward search so the first '0' is the one at the
                     * end of the range (BIT21)
                     */
                    if (!(local_found && (local_place == 21)))
                    {

                        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                     "Failed in sw_state_occ_bm_destroy: Found should be TRUE and place should be 21\r\n"
                                     "==> local_found %d local_place %d\r\n", (int) local_found, (int) local_place);
                    }
                }
                start = 15;
                end = 21;
                val = 1;
                SHR_IF_ERR_EXIT(example.
                                occ.get_next_in_range(unit, start, end, forward[ii], val, &local_place, &local_found));
                if (ii == 0)
                {
                    /*
                     * This is a forward search so the first '1' is the first
                     * bit in the range (BIT15)
                     */
                    if (!(local_found && (local_place == 15)))
                    {

                        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                     "Failed in sw_state_occ_bm_destroy: Found should be TRUE and place should be 17\r\n"
                                     "==> local_found %d local_place %d\r\n", (int) local_found, (int) local_place);
                    }
                }
                else
                {
                    /*
                     * This is a backward search so the first '1' is the
                     * last bit that was set (BIT18)
                     */
                    if (!(local_found && (local_place == 18)))
                    {
                        SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                     "Failed in sw_state_occ_bm_destroy: Found should be TRUE and place should be 19\r\n"
                                     "==> local_found %d local_place %d\r\n", (int) local_found, (int) local_place);
                    }
                }
            }

        }

        LOG_CLI((BSL_META_U(unit, "found %u \n"), place));
        if (found)
        {
            SHR_IF_ERR_EXIT(example.occ.print(unit));
        }

        SHR_IF_ERR_EXIT(example.occ.status_set(unit, place, TRUE));
    }

    LOG_CLI((BSL_META_U(unit, "\n\r ********** sw_state_occ_bm_test_1 : END. passed **********\n")));

exit:
    if (SHR_GET_CURRENT_ERR())
    {
        LOG_CLI((BSL_META("Test FAILED\n")));
    }
    CTEST_SW_STATE_FUNC_EXIT;
}

static shr_error_e
sw_state_occupation_bitmap_test2_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 indx, place;
    uint32 expected_places[5];
    uint8 found;
    sw_state_occ_bitmap_init_info_t btmp_init_info;

    CTEST_SW_STATE_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META("Init the example access module\n")));
    SHR_IF_ERR_EXIT(example.init(unit));

    SHR_IF_ERR_EXIT(sw_state_occ_bm_init_info_clear(&btmp_init_info));
    btmp_init_info.size = 64;

    SHR_IF_ERR_EXIT(example.occ.create(unit, &btmp_init_info));

    for (indx = 0; indx < 66; ++indx)
    {
        SHR_IF_ERR_EXIT(example.occ.get_next(unit, &place, &found));

        LOG_CLI((BSL_META_U(unit, "found %u \n"), place));
        if (found)
        {
            SHR_IF_ERR_EXIT(example.occ.print(unit));
        }

        SHR_IF_ERR_EXIT(example.occ.status_set(unit, place, TRUE));
    }
    /*
     * free one bit on 20
     */
    SHR_IF_ERR_EXIT(example.occ.status_set(unit, 20, FALSE));
    /*
     * get next free place (has to be 20)
     */

    SHR_IF_ERR_EXIT(example.occ.get_next(unit, &place, &found));

    LOG_CLI((BSL_META_U(unit, "found %u \n"), place));
    if (found)
    {
        SHR_IF_ERR_EXIT(example.occ.print(unit));
    }

    if (place != 20)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "found %u instead of 20\n", place);
    }

    SHR_IF_ERR_EXIT(example.occ.status_set(unit, place, TRUE));
    /*
     * free 4 bits on 63, 0, 15, 32
     */
    expected_places[0] = 0;
    expected_places[1] = 15;
    expected_places[2] = 32;
    expected_places[3] = 63;
    expected_places[4] = 0;

    SHR_IF_ERR_EXIT(example.occ.status_set(unit, 63, FALSE));
    SHR_IF_ERR_EXIT(example.occ.status_set(unit, 0, FALSE));
    SHR_IF_ERR_EXIT(example.occ.status_set(unit, 15, FALSE));
    SHR_IF_ERR_EXIT(example.occ.status_set(unit, 32, FALSE));

    /*
     * allocate 5 bits 4 should be 63, 0, 15, 32 and one not found.
     */
    for (indx = 0; indx < 5; ++indx)
    {
        SHR_IF_ERR_EXIT(example.occ.get_next(unit, &place, &found));

        LOG_CLI((BSL_META_U(unit, "found %u \n"), place));
        if (found)
        {
            SHR_IF_ERR_EXIT(example.occ.print(unit));
        }

        if (place != expected_places[indx])
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "found %u instead of %u\n", place, expected_places[indx]);
        }

        SHR_IF_ERR_EXIT(example.occ.status_set(unit, place, TRUE));
    }

    LOG_CLI((BSL_META_U(unit, "\n\r ********** sw_state_occ_bm_test_2 : END passed **********\n")));

exit:
    if (SHR_GET_CURRENT_ERR())
    {
        LOG_CLI((BSL_META("Test FAILED\n")));
    }
    CTEST_SW_STATE_FUNC_EXIT;

}

static shr_error_e
sw_state_occupation_bitmap_test3_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 indx, place;
    uint32 expected_places[5];
    uint8 found;
    sw_state_occ_bitmap_init_info_t btmp_init_info;

    CTEST_SW_STATE_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META("Init the example access module\n")));
    SHR_IF_ERR_EXIT(example.init(unit));

    SHR_IF_ERR_EXIT(sw_state_occ_bm_init_info_clear(&btmp_init_info));

    btmp_init_info.size = 700;

    SHR_IF_ERR_EXIT(example.occ.create(unit, &btmp_init_info));

    for (indx = 0; indx < 702; ++indx)
    {
        SHR_IF_ERR_EXIT(example.occ.get_next(unit, &place, &found));

        LOG_CLI((BSL_META_U(unit, "found %u \n"), place));
        if (found)
        {
            SHR_IF_ERR_EXIT(example.occ.print(unit));
        }

        if ((indx < 700 && place != indx) || (indx >= 700 && found))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "found %u instead of %u\n", place, indx);
        }

        SHR_IF_ERR_EXIT(example.occ.status_set(unit, place, TRUE));
    }
    /*
     * free one bit on 200
     */
    SHR_IF_ERR_EXIT(example.occ.status_set(unit, 200, FALSE));

    /*
     * get next free place (has to be 200)
     */
    SHR_IF_ERR_EXIT(example.occ.get_next(unit, &place, &found));

    LOG_CLI((BSL_META_U(unit, "found %u \n"), place));
    if (found)
    {
        SHR_IF_ERR_EXIT(example.occ.print(unit));
    }

    if (place != 200)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "found %u instead of 200\n", place);
    }

    SHR_IF_ERR_EXIT(example.occ.status_set(unit, place, TRUE));
    /*
     * free 4 bits on 630, 0, 150, 320
     */
    expected_places[0] = 0;
    expected_places[1] = 150;
    expected_places[2] = 320;
    expected_places[3] = 630;
    expected_places[4] = 0;

    SHR_IF_ERR_EXIT(example.occ.status_set(unit, 630, FALSE));
    SHR_IF_ERR_EXIT(example.occ.status_set(unit, 0, FALSE));
    SHR_IF_ERR_EXIT(example.occ.status_set(unit, 150, FALSE));
    SHR_IF_ERR_EXIT(example.occ.status_set(unit, 320, FALSE));

    /*
     * allocate 5 bits 4 should be 630, 0, 150, 320 and one not found.
     */
    for (indx = 0; indx < 5; ++indx)
    {
        SHR_IF_ERR_EXIT(example.occ.get_next(unit, &place, &found));

        LOG_CLI((BSL_META_U(unit, "found %u \n"), place));
        if (found)
        {
            SHR_IF_ERR_EXIT(example.occ.print(unit));
        }

        if (place != expected_places[indx])
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "found %u instead of %u\n", place, expected_places[indx]);
        }

        SHR_IF_ERR_EXIT(example.occ.status_set(unit, place, TRUE));
    }

    LOG_CLI((BSL_META_U(unit, "\n\r ********** sw_state_occ_bm_test_3 : END passed **********\n")));

exit:
    if (SHR_GET_CURRENT_ERR())
    {
        LOG_CLI((BSL_META("Test FAILED\n")));
    }
    CTEST_SW_STATE_FUNC_EXIT;
}

static shr_error_e
sw_state_occupation_bitmap_test4_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 indx, place, free_place;
    uint8 found;
    sw_state_occ_bitmap_init_info_t btmp_init_info;

    CTEST_SW_STATE_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META("Init the example access module\n")));
    SHR_IF_ERR_EXIT(example.init(unit));

    SHR_IF_ERR_EXIT(sw_state_occ_bm_init_info_clear(&btmp_init_info));

    btmp_init_info.size = 700;

    SHR_IF_ERR_EXIT(example.occ.create(unit, &btmp_init_info));

    for (indx = 0; indx < 702; ++indx)
    {
        SHR_IF_ERR_EXIT(example.occ.get_next(unit, &place, &found));

        LOG_CLI((BSL_META_U(unit, "found %u \n"), place));
        if (found)
        {
            SHR_IF_ERR_EXIT(example.occ.print(unit));
        }

        if ((indx < 700 && place != indx) || (indx >= 700 && found))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "found %u instead of %u\n", place, indx);
        }

        SHR_IF_ERR_EXIT(example.occ.status_set(unit, place, TRUE));
    }
    for (indx = 0; indx < 700; ++indx)
    {
        /*
         * free one bit one by one
         */
        free_place = indx;
        SHR_IF_ERR_EXIT(example.occ.status_set(unit, free_place, FALSE));
        /*
         * get next free place (has to be free_place)
         */
        SHR_IF_ERR_EXIT(example.occ.get_next(unit, &place, &found));

        LOG_CLI((BSL_META_U(unit, "found %u \n"), place));
        if (found)
        {
            SHR_IF_ERR_EXIT(example.occ.print(unit));
        }

        if (place != free_place)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "found %u instead of %u\n", place, free_place);
        }

        SHR_IF_ERR_EXIT(example.occ.status_set(unit, place, TRUE));

    }

    LOG_CLI((BSL_META_U(unit, "\n\r ********** sw_state_occ_bm_test_4 : END passed **********\n")));

exit:
    if (SHR_GET_CURRENT_ERR())
    {
        LOG_CLI((BSL_META("Test FAILED\n")));
    }
    CTEST_SW_STATE_FUNC_EXIT;
}

static shr_error_e
sw_state_sorted_ll_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    sw_state_ll_init_info_t init_info;
    sw_state_ll_node_t output_node;
    uint8 values[12] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    uint8 output_key;
    uint8 curr_val, last_val, found;
    uint32 nof_elements;
    int ret_val;

    CTEST_SW_STATE_FUNC_INIT_VARS(unit);

    sal_memset(&init_info, 0, sizeof(sw_state_ll_init_info_t));
    init_info.max_nof_elements = 12;
    init_info.expected_nof_elements = 10;
    init_info.key_cmp_cb_name = "sw_state_sorted_list_cmp";

    LOG_CLI((BSL_META("Init the example access module\n")));
    SHR_IF_ERR_EXIT(example.init(unit));

    LOG_CLI((BSL_META("init the sorted linked list\n")));
    SHR_IF_ERR_EXIT(example.sorted_ll.create_empty(unit, &init_info));

    LOG_CLI((BSL_META("insert some values to the list: 2, 7, 9, 1, 8, 3, 10\n")));
    SHR_IF_ERR_EXIT(example.sorted_ll.add(unit, &values[2], &values[2]));
    SHR_IF_ERR_EXIT(example.sorted_ll.add(unit, &values[7], &values[7]));
    SHR_IF_ERR_EXIT(example.sorted_ll.add(unit, &values[9], &values[9]));
    SHR_IF_ERR_EXIT(example.sorted_ll.add(unit, &values[1], &values[1]));
    SHR_IF_ERR_EXIT(example.sorted_ll.add(unit, &values[8], &values[8]));
    SHR_IF_ERR_EXIT(example.sorted_ll.add(unit, &values[3], &values[3]));
    SHR_IF_ERR_EXIT(example.sorted_ll.add(unit, &values[10], &values[10]));

    SHR_IF_ERR_EXIT(example.sorted_ll.nof_elements(unit, &nof_elements));
    if (nof_elements != 7)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - expected nof_elements to be 7 elements, got %d\n", nof_elements);
    }

    LOG_CLI((BSL_META("verify that the list is sorted\n")));

    last_val = 0;
    SHR_IF_ERR_EXIT(example.sorted_ll.get_first(unit, &output_node));
    while (DNX_SW_STATE_LL_IS_NODE_VALID(output_node))
    {
        SHR_IF_ERR_EXIT(example.sorted_ll.node_value(unit, output_node, &curr_val));
        if (curr_val <= last_val)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - the list is not sorted\n");
        }
        last_val = curr_val;
        SHR_IF_ERR_EXIT(example.sorted_ll.next_node(unit, output_node, &output_node));
    }

    SHR_IF_ERR_EXIT(example.sorted_ll.find(unit, &output_node, &values[3], &found));
    SHR_IF_ERR_EXIT(example.sorted_ll.node_key(unit, output_node, &output_key));
    if (output_key != 3)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - find expect value 3 but got %d\n", output_key);
    }

    ret_val = example.sorted_ll.add_after(unit, output_node, &values[3], &values[3]);
    if (ret_val == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - expected error for duplicate entries but got success\n");
    }

    ret_val = example.sorted_ll.add_before(unit, output_node, &values[3], &values[3]);
    if (ret_val == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - expected error for duplicate entries but got success\n");
    }

    ret_val = example.sorted_ll.add_after(unit, output_node, &values[7], &values[7]);
    if (ret_val == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - expected error for duplicate entries but got success\n");
    }

    ret_val = example.sorted_ll.add(unit, &values[8], &values[8]);
    if (ret_val == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - expected error for duplicate entries but got success\n");
    }

    ret_val = example.sorted_ll.add_first(unit, &values[8], &values[8]);
    if (ret_val == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - expected error for duplicate entries but got success\n");
    }
    SHR_IF_ERR_EXIT(example.sorted_ll.add_first(unit, &values[0], &values[0]));

    ret_val = example.sorted_ll.add_last(unit, &values[8], &values[8]);
    if (ret_val == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - expected error for duplicate entries but got success\n");
    }
    SHR_IF_ERR_EXIT(example.sorted_ll.add_last(unit, &values[11], &values[11]));

    SHR_IF_ERR_EXIT(example.sorted_ll.find(unit, &output_node, &values[3], &found));
    SHR_IF_ERR_EXIT(example.sorted_ll.add_after(unit, output_node, &values[4], &values[4]));

    SHR_IF_ERR_EXIT(example.sorted_ll.nof_elements(unit, &nof_elements));
    if (nof_elements != 10)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - expected nof_elements to be 10 elements but got %d\n",
                     nof_elements);
    }

    SHR_IF_ERR_EXIT(example.sorted_ll.find(unit, &output_node, &values[10], &found));

    SHR_IF_ERR_EXIT(example.sorted_ll.node_update(unit, output_node, &values[5]));

    SHR_IF_ERR_EXIT(example.sorted_ll.previous_node(unit, output_node, &output_node));

    SHR_IF_ERR_EXIT(example.sorted_ll.node_key(unit, output_node, &output_key));

    if (output_key != 9)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - expected previous node to be 9 but got %d\n", output_key);
    }

    SHR_IF_ERR_EXIT(example.sorted_ll.remove_node(unit, output_node));
    SHR_IF_ERR_EXIT(example.sorted_ll.nof_elements(unit, &nof_elements));
    if (nof_elements != 9)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - expected nof_elements to be 10 elements but got %d\n",
                     nof_elements);
    }

    LOG_CLI((BSL_META("destroy the linked list\n")));

    LOG_CLI((BSL_META("Test PASSED\n")));

    SHR_EXIT();

exit:
    if (SHR_GET_CURRENT_ERR())
    {
        LOG_CLI((BSL_META("Test FAILED\n")));
    }
    CTEST_SW_STATE_FUNC_EXIT;

}

static shr_error_e
sw_state_htb_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    sw_state_htbl_init_info_t init_info;
    int value = 0;
    int value1 = 10;
    int value2 = 20;
    int value3 = 30;
    int value4 = 40;
    int value5 = 50;
    int key1 = 10;
    int key2 = 20;
    int key3 = 30;
    int key4 = 40;
    int key5 = 50;
    /*
     * int rv; 
     */
    uint8 success = 0;
    uint8 found = 0;

    CTEST_SW_STATE_FUNC_INIT_VARS(unit);

    init_info.max_nof_elements = 128;
    init_info.expected_nof_elements = 10;
    init_info.hash_function = NULL;
    init_info.print_cb_name = "sw_state_htb_example_entry_print";

    LOG_CLI((BSL_META("Init the example access module\n")));
    SHR_IF_ERR_EXIT(example.init(unit));

    LOG_CLI((BSL_META("Init the hash table\n")));
    SHR_IF_ERR_EXIT(example.htb.create(unit, &init_info));

    LOG_CLI((BSL_META("insert one entry prior to state comparison transaction\n")));
    SHR_IF_ERR_EXIT(example.htb.insert(unit, &key5, &value5, &success));
    if (success == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - insertion of (5,50) failed\n");
    }

    if (dnx_state_comparison_start(unit) != _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Snapshot manager, failed to start.\n");
    }

    LOG_CLI((BSL_META("remove the entry prior to the transaction")));
    SHR_IF_ERR_EXIT(example.htb.delete(unit, &key5));

    LOG_CLI((BSL_META("Insert (1,10) & (4,40)\n")));
    SHR_IF_ERR_EXIT(example.htb.insert(unit, &key1, &value1, &success));
    if (success == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - insertion of (1,10) failed\n");
    }
    SHR_IF_ERR_EXIT(example.htb.insert(unit, &key4, &value4, &success));
    if (success == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - insertion of (4,40) failed\n");
    }

    LOG_CLI((BSL_META("find key 4 and expect value 40\n")));
    SHR_IF_ERR_EXIT(example.htb.find(unit, &key4, &value, &found));
    if (value != value4)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - unexpected value in key 4\n");
    }

    LOG_CLI((BSL_META("find key 3 and expect not found error")));
    /*
     * rv = 
     */ example.htb.find(unit, &key3, &value, &found);
    if (found != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - found to be 0\n");
    }

    LOG_CLI((BSL_META("find key 1 and expect value 10")));
    SHR_IF_ERR_EXIT(example.htb.find(unit, &key1, &value, &found));
    if (value != value1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - unexpected value in key 1\n");
    }

    LOG_CLI((BSL_META("remove key 1 and expect")));
    SHR_IF_ERR_EXIT(example.htb.delete(unit, &key1));

    LOG_CLI((BSL_META("find key 1 and expect found to be 0 as it was deleted")));
    /*
     * rv = 
     */ example.htb.find(unit, &key1, &value, &found);
    if (found != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - expected found to be 0\n");
    }

    LOG_CLI((BSL_META("Insert (3,30) & (2,20)\n")));
    SHR_IF_ERR_EXIT(example.htb.insert(unit, &key2, &value2, &success));
    if (success == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - insertion of (2,20) failed\n");
    }
    SHR_IF_ERR_EXIT(example.htb.insert(unit, &key3, &value3, &success));
    if (success == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - insertion of (3,30) failed\n");
    }

    LOG_CLI((BSL_META("find key 3 and expect value 30\n")));
    SHR_IF_ERR_EXIT(example.htb.find(unit, &key3, &value, &found));
    if (value != value3)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - unexpected value in key 3\n");
    }

    LOG_CLI((BSL_META("insert the entry that existed prior to the transaction\n")));
    SHR_IF_ERR_EXIT(example.htb.insert(unit, &key5, &value5, &success));
    if (success == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - insertion of (5,50) failed\n");
    }

    LOG_CLI((BSL_META("print the entries in htbl, visual only\n")));
    SHR_IF_ERR_EXIT(example.htb.print(unit));

    /*
     * delete the rest of the entries in the hash table
     */
    LOG_CLI((BSL_META("remove key 2 and expect")));
    SHR_IF_ERR_EXIT(example.htb.delete(unit, &key2));

    LOG_CLI((BSL_META("remove key 3 and expect")));
    SHR_IF_ERR_EXIT(example.htb.delete(unit, &key3));

    LOG_CLI((BSL_META("remove key 4 and expect")));
    SHR_IF_ERR_EXIT(example.htb.delete(unit, &key4));

    if (dnx_state_comparison_end_and_compare(unit) != _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Snapshot manager rollback error.\n");
    }

    LOG_CLI((BSL_META("remove the entry prior to the transaction")));
    SHR_IF_ERR_EXIT(example.htb.delete(unit, &key5));

    LOG_CLI((BSL_META("Test PASSED\n")));

    SHR_EXIT();

exit:
    if (SHR_GET_CURRENT_ERR())
    {
        LOG_CLI((BSL_META("Test FAILED\n")));
    }
    CTEST_SW_STATE_FUNC_EXIT;

}

static shr_error_e
sw_state_htb_arr_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    sw_state_htbl_init_info_t init_info0, init_info1;
    int int_value = 0;
    char char_value = 0;
    int int_values[5] = { 10, 20, 30, 40, 50 };
    char char_values[5] = { 10, 20, 30, 40, 50 };
    int int_keys[5] = { 1, 2, 3, 4, 5 };
    char char_keys[5] = { 1, 2, 3, 4, 5 };
    /*
     * int rv; 
     */
    uint8 success = 0;
    uint8 found = 0;

    CTEST_SW_STATE_FUNC_INIT_VARS(unit);

    init_info0.max_nof_elements = 128;
    init_info0.expected_nof_elements = 10;
    init_info0.hash_function = NULL;
    init_info0.print_cb_name = "sw_state_htb_example_entry_print";

    init_info1.max_nof_elements = 128;
    init_info1.expected_nof_elements = 10;
    init_info1.hash_function = NULL;
    init_info1.print_cb_name = NULL;

    LOG_CLI((BSL_META("Init the example access module\n")));
    SHR_IF_ERR_EXIT(example.init(unit));

    LOG_CLI((BSL_META("Init the hash tables\n")));
    SHR_IF_ERR_EXIT(example.htb_arr.create(unit, 0, &init_info0, sizeof(int), sizeof(int)));
    SHR_IF_ERR_EXIT(example.htb_arr.create(unit, 1, &init_info1, sizeof(char), sizeof(char)));

    LOG_CLI((BSL_META("insert one entry prior to state comparison transaction\n")));
    SHR_IF_ERR_EXIT(example.htb_arr.insert(unit, 0, &int_keys[4], &int_values[4], &success));
    if (success == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - insertion of (5,50) to htb_arr[0] failed\n");
    }

    SHR_IF_ERR_EXIT(example.htb_arr.insert(unit, 1, &char_keys[4], &char_values[4], &success));
    if (success == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - insertion of (5,50) to htb_arr[1] failed\n");
    }

    if (dnx_state_comparison_start(unit) != _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Snapshot manager, failed to start.\n");
    }

    LOG_CLI((BSL_META("remove the entry prior to the transaction")));

    SHR_IF_ERR_EXIT(example.htb_arr.delete(unit, 0, &int_keys[4]));
    SHR_IF_ERR_EXIT(example.htb_arr.delete(unit, 1, &char_keys[4]));

    LOG_CLI((BSL_META("Insert (1,10) & (4,40)\n")));
    SHR_IF_ERR_EXIT(example.htb_arr.insert(unit, 0, &int_keys[0], &int_values[0], &success));
    if (success == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - insertion of (1,10) failed\n");
    }
    SHR_IF_ERR_EXIT(example.htb_arr.insert(unit, 0, &int_keys[3], &int_values[3], &success));
    if (success == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - insertion of (4,40) failed\n");
    }

    SHR_IF_ERR_EXIT(example.htb_arr.insert(unit, 1, &char_keys[0], &char_values[0], &success));
    if (success == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - insertion of (1,10) failed\n");
    }
    SHR_IF_ERR_EXIT(example.htb_arr.insert(unit, 1, &char_keys[3], &char_values[3], &success));
    if (success == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - insertion of (4,40) failed\n");
    }

    LOG_CLI((BSL_META("find key 4 and expect value 40\n")));
    SHR_IF_ERR_EXIT(example.htb_arr.find(unit, 0, &int_keys[3], &int_value, &found));
    if (int_value != int_values[3])
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - unexpected value in key 4\n");
    }

    SHR_IF_ERR_EXIT(example.htb_arr.find(unit, 1, &char_keys[3], &char_value, &found));
    if (char_value != char_values[3])
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - unexpected value in key 4\n");
    }

    LOG_CLI((BSL_META("find key 3 and expect not found error")));

    example.htb_arr.find(unit, 0, &int_keys[2], &int_value, &found);
    if (found != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - found to be 0\n");
    }

    example.htb_arr.find(unit, 1, &char_keys[2], &char_value, &found);
    if (found != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - found to be 0\n");
    }

    LOG_CLI((BSL_META("find key 1 and expect value 10")));
    SHR_IF_ERR_EXIT(example.htb_arr.find(unit, 0, &int_keys[0], &int_value, &found));
    if (int_value != int_values[0])
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - unexpected value in key 1\n");
    }

    SHR_IF_ERR_EXIT(example.htb_arr.find(unit, 1, &char_keys[0], &char_value, &found));
    if (char_value != char_values[0])
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - unexpected value in key 1\n");
    }

    LOG_CLI((BSL_META("remove key 1")));
    SHR_IF_ERR_EXIT(example.htb_arr.delete(unit, 0, &int_keys[0]));
    SHR_IF_ERR_EXIT(example.htb_arr.delete(unit, 1, &char_keys[0]));

    LOG_CLI((BSL_META("find key 1 and expect found to be 0 as it was deleted")));
    example.htb_arr.find(unit, 0, &int_keys[0], &int_value, &found);
    if (found != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - expected found to be 0\n");
    }

    example.htb_arr.find(unit, 1, &char_keys[0], &char_value, &found);
    if (found != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - expected found to be 0\n");
    }

    LOG_CLI((BSL_META("Insert (3,30) & (2,20)\n")));

    SHR_IF_ERR_EXIT(example.htb_arr.insert(unit, 0, &int_keys[1], &int_values[1], &success));
    if (success == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - insertion of (2,20) failed\n");
    }
    SHR_IF_ERR_EXIT(example.htb_arr.insert(unit, 1, &char_keys[1], &char_values[1], &success));
    if (success == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - insertion of (2,20) failed\n");
    }
    SHR_IF_ERR_EXIT(example.htb_arr.insert(unit, 0, &int_keys[2], &int_values[2], &success));
    if (success == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - insertion of (3,30) failed\n");
    }
    SHR_IF_ERR_EXIT(example.htb_arr.insert(unit, 1, &char_keys[2], &char_values[2], &success));
    if (success == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - insertion of (3,30) failed\n");
    }

    LOG_CLI((BSL_META("find key 3 and expect value 30\n")));
    SHR_IF_ERR_EXIT(example.htb_arr.find(unit, 0, &int_keys[2], &int_value, &found));
    if (int_value != int_values[2])
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - unexpected value in key 3\n");
    }
    SHR_IF_ERR_EXIT(example.htb_arr.find(unit, 1, &char_keys[2], &char_value, &found));
    if (char_value != char_values[2])
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - unexpected value in key 3\n");
    }

    LOG_CLI((BSL_META("insert the entry that existed prior to the transaction\n")));
    SHR_IF_ERR_EXIT(example.htb_arr.insert(unit, 0, &int_keys[4], &int_values[4], &success));
    if (success == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - insertion of (5,50) failed\n");
    }
    SHR_IF_ERR_EXIT(example.htb_arr.insert(unit, 1, &char_keys[4], &char_values[4], &success));
    if (success == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - insertion of (5,50) failed\n");
    }

    /*
     * delete the rest of the entries in the hash table
     */
    LOG_CLI((BSL_META("remove key 2")));
    SHR_IF_ERR_EXIT(example.htb_arr.delete(unit, 0, &int_keys[1]));
    SHR_IF_ERR_EXIT(example.htb_arr.delete(unit, 1, &char_keys[1]));

    LOG_CLI((BSL_META("remove key 3")));
    SHR_IF_ERR_EXIT(example.htb_arr.delete(unit, 0, &int_keys[2]));
    SHR_IF_ERR_EXIT(example.htb_arr.delete(unit, 1, &char_keys[2]));

    LOG_CLI((BSL_META("remove key 4")));
    SHR_IF_ERR_EXIT(example.htb_arr.delete(unit, 0, &int_keys[3]));
    SHR_IF_ERR_EXIT(example.htb_arr.delete(unit, 1, &char_keys[3]));

    if (dnx_state_comparison_end_and_compare(unit) != _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Snapshot manager rollback error.\n");
    }

    LOG_CLI((BSL_META("remove the entry prior to the transaction")));
    SHR_IF_ERR_EXIT(example.htb_arr.delete(unit, 0, &int_keys[4]));
    SHR_IF_ERR_EXIT(example.htb_arr.delete(unit, 1, &char_keys[4]));

    LOG_CLI((BSL_META("Test PASSED\n")));

    SHR_EXIT();

exit:
    if (SHR_GET_CURRENT_ERR())
    {
        LOG_CLI((BSL_META("Test FAILED\n")));
    }
    CTEST_SW_STATE_FUNC_EXIT;

}

static shr_error_e
sw_state_enum_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    CTEST_SW_STATE_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META("Retrieving string for DNX_EXAMPLE_ENUM_NEGATIVE value and validating result.\n")));
    if (sal_strncmp(dnx_example_name_e_get_name(DNX_EXAMPLE_ENUM_NEGATIVE), "DNX_EXAMPLE_ENUM_NEGATIVE",
                    sal_strlen("DNX_EXAMPLE_ENUM_NEGATIVE")))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - unexpected enum name retrieved for DNX_EXAMPLE_ENUM_NEGATIVE\n");
    }

    LOG_CLI((BSL_META("Retrieving string for DNX_EXAMPLE_ENUM_ZERO value and validating result.\n")));
    if (sal_strncmp(dnx_example_name_e_get_name(DNX_EXAMPLE_ENUM_ZERO), "DNX_EXAMPLE_ENUM_ZERO",
                    sal_strlen("DNX_EXAMPLE_ENUM_ZERO")))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - unexpected enum name retrieved for DNX_EXAMPLE_ENUM_ZERO\n");
    }

    LOG_CLI((BSL_META("Retrieving string for DNX_EXAMPLE_ENUM_ONE value and validating result.\n")));
    if (sal_strncmp(dnx_example_name_e_get_name(DNX_EXAMPLE_ENUM_ONE), "DNX_EXAMPLE_ENUM_ONE",
                    sal_strlen("DNX_EXAMPLE_ENUM_ONE")))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - unexpected enum name retrieved for DNX_EXAMPLE_ENUM_ONE\n");
    }

    LOG_CLI((BSL_META("Test PASSED\n")));

    SHR_EXIT();

exit:
    CTEST_SW_STATE_FUNC_EXIT;
}

static shr_error_e
sw_state_callback_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_sw_state_callback_test_function_cb test_cb;
    char function_name[SW_STATE_CB_DB_NAME_STR_SIZE];

    CTEST_SW_STATE_FUNC_INIT_VARS(unit);

    sal_strncpy(function_name, "dnx_sw_state_callback_test_first_function", SW_STATE_CB_DB_NAME_STR_SIZE - 1);

    LOG_CLI((BSL_META("Init the example access module\n")));
    SHR_IF_ERR_EXIT(example.init(unit));

    LOG_CLI((BSL_META("Register the name of the callback\n")));
    SHR_IF_ERR_EXIT(example.cb.register_cb(unit, function_name));

    LOG_CLI((BSL_META("Get the callback for the register cb name.\n")));
    SHR_IF_ERR_EXIT(example.cb.get_cb(unit, &test_cb));

    if (test_cb(unit) != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - unexpected value returned by the test callback.\n");
    }

    sal_strncpy(function_name, "dnx_sw_state_callback_test_second_function", SW_STATE_CB_DB_NAME_STR_SIZE - 1);

    LOG_CLI((BSL_META("Register the name of the callback\n")));
    SHR_IF_ERR_EXIT(example.cb.register_cb(unit, function_name));

    LOG_CLI((BSL_META("Get the callback for the register cb name.\n")));
    SHR_IF_ERR_EXIT(example.cb.get_cb(unit, &test_cb));

    if (test_cb(unit) != 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - unexpected value returned by the test callback.\n");
    }

    sal_strncpy(function_name, "dnx_sw_state_callback_test_error_check_function", SW_STATE_CB_DB_NAME_STR_SIZE - 1);

    LOG_CLI((BSL_META("Register the name of the callback\n")));

    if (example.cb.register_cb(unit, function_name) != _SHR_E_NOT_FOUND)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - unexpected value returned by the test callback.\n");
    }

    LOG_CLI((BSL_META("Unregister the name of the callback.\n")));
    SHR_IF_ERR_EXIT(example.cb.unregister_cb(unit));

    LOG_CLI((BSL_META("Test PASSED\n")));

    SHR_EXIT();

exit:
    CTEST_SW_STATE_FUNC_EXIT;
}

static shr_error_e
sw_state_multi_set_test1_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    sw_state_multi_set_info_t init_info;

    SW_STATE_MULTI_SET_KEY key[4];

    int num_members_for_test = 4;
    uint8 success = 0;
    uint8 first_appear = 0;
    uint32 data_indx = 0;
    int duplication_counter = 0;
    int added_members = 0;

    /*
     * Test no. 1:
     * 'max_duplications' is set greater than '1'
     * Add 'num_members_for_test' members. Do it to the maximum set for
     * ref_counter per member (max_duplications) and to the maximum count allowed for
     * the whole multi-set (global_max).
     */

    CTEST_SW_STATE_FUNC_INIT_VARS(unit);

    sal_memset(&init_info, 0x0, sizeof(sw_state_multi_set_info_t));
    init_info.nof_members = 10;
    init_info.member_size = sizeof(key);
    init_info.max_duplications = 3;
    init_info.print_cb_name = "sw_state_htb_example_entry_print";

    LOG_CLI((BSL_META("Init the example access module.\n")));
    SHR_IF_ERR_EXIT(example.init(unit));

    LOG_CLI((BSL_META("Create the multi set table.\n")));
    SHR_IF_ERR_EXIT(example.multi_set_example.create(unit, &init_info));

    for (; duplication_counter < init_info.max_duplications; duplication_counter++)
    {
        for (added_members = 1; added_members < (num_members_for_test + 1); added_members++)
        {
            sal_memset(key, 0, sizeof(key));
            key[0] = 100;
            key[1] = added_members;
            SHR_IF_ERR_EXIT(example.multi_set_example.add(unit, key, &data_indx, &first_appear, &success));
        }
    }

    SHR_IF_ERR_EXIT(example.multi_set_example.print(unit, 0));

    SHR_EXIT();

exit:
    if (SHR_GET_CURRENT_ERR())
    {
        LOG_CLI((BSL_META("Test FAILED\n")));
    }
    else
    {
        LOG_CLI((BSL_META("Test PASSED\n")));
    }
    CTEST_SW_STATE_FUNC_EXIT;

}

static shr_error_e
sw_state_multi_set_test2_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    sw_state_multi_set_info_t init_info;

    SW_STATE_MULTI_SET_KEY key[4];
    SW_STATE_MULTI_SET_KEY ref_key[4];

    uint8 success = 0;
    uint8 first_appear = 0;
    uint32 data_index = 0;
    uint32 nof_additions = 0;
    uint32 ref_count = 0;
    int member_index = 0;

    /*
     * Test no. 2:
     * 'max_duplications' is set greater than '1'
     * Add one member and increase ref_count to the maximum allowed.
     * Then, try to increase further and get error.
     */

    CTEST_SW_STATE_FUNC_INIT_VARS(unit);

    sal_memset(&init_info, 0x0, sizeof(sw_state_multi_set_info_t));
    init_info.nof_members = 2;
    init_info.member_size = sizeof(key);
    init_info.max_duplications = 3;
    init_info.print_cb_name = "sw_state_htb_example_entry_print";

    LOG_CLI((BSL_META("Init the example access module.\n")));
    SHR_IF_ERR_EXIT(example.init(unit));

    LOG_CLI((BSL_META("Create the multi set table.\n")));
    SHR_IF_ERR_EXIT(example.multi_set_example.create(unit, &init_info));

    sal_memset(key, 0, sizeof(key));
    key[0] = 64;
    key[1] = 11;
    key[2] = 10;
    key[3] = 9;

    SHR_IF_ERR_EXIT(example.multi_set_example.add(unit, &key[0], &data_index, &first_appear, &success));
    if (!success)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
        LOG_CLI((BSL_META("Failed to add first member to the multi set.\n")));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(example.multi_set_example.add_at_index(unit, key, data_index, &first_appear, &success));
    if (!success)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
        LOG_CLI((BSL_META("Failed to add second instance (ref_counter) to member data_indx (%d) on multiset.\n"),
                 data_index));
        SHR_EXIT();
    }
    if (first_appear)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
        LOG_CLI((BSL_META("Add second instance to member %d on multiset but 'first_appear' is non-zero!\n"),
                 data_index));
        SHR_EXIT();
    }

    /*
     * Increase ref_counter to 'max_duplications' (by setting 'nof_additions' to UTILEX_U32_MAX)
     */
    nof_additions = UTILEX_U32_MAX;
    SHR_IF_ERR_EXIT(example.multi_set_example.add_at_index_multiple(unit, key, data_index, nof_additions, &first_appear,
                                                                    &success));
    if (!success)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
        LOG_CLI((BSL_META("Failed to add max instance (ref_counter) to member data_indx (%d) on multiset.\n"),
                 data_index));
        SHR_EXIT();
    }
    if (first_appear)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
        LOG_CLI((BSL_META("Add max instance to member %d on multiset but 'first_appear' is non-zero!\n"), data_index));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(example.multi_set_example.get_by_index(unit, data_index, ref_key, &ref_count));
    if (ref_count != init_info.max_duplications)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
        LOG_CLI((BSL_META("Member %d on multiset has ref_count %d which it should be %d!\r\n"), data_index,
                 (int) ref_count, (int) (init_info.max_duplications)));
        SHR_EXIT();
    }
    if (sal_memcmp(key, ref_key, init_info.member_size) != 0)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
        LOG_CLI((BSL_META("Member %d on multiset has wrong key!\r\n"), data_index));

        for (member_index = 0; member_index < init_info.member_size; member_index++)
        {
            LOG_CLI((BSL_META("%02X"), ref_key[member_index]));
        }
        LOG_CLI((BSL_META_U(unit, "\r\n")));
        SHR_EXIT();
    }
    /*
     * Now try to add one to reference counter (ref_count). Should fail on 'success'!
     */
    SHR_IF_ERR_EXIT(example.multi_set_example.add_at_index(unit, key, data_index, &first_appear, &success));
    if (success)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
        LOG_CLI((BSL_META
                 ("Succeeded to add one to ref_counter although it should be already larger than max_duplications: member(%d) on multiset.\n"),
                 data_index));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(example.multi_set_example.print(unit, 0));

    SHR_EXIT();

exit:
    if (SHR_GET_CURRENT_ERR())
    {
        LOG_CLI((BSL_META("Test FAILED\n")));
    }
    else
    {
        LOG_CLI((BSL_META("Test PASSED\n")));
    }
    CTEST_SW_STATE_FUNC_EXIT;

}

static shr_error_e
sw_state_multi_set_test3_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    sw_state_multi_set_info_t init_info;

    SW_STATE_MULTI_SET_KEY key[4];
    SW_STATE_MULTI_SET_KEY ref_key[4];

    uint8 success = 0;
    uint8 first_appear = 0;
    uint32 data_index = 0;
    uint32 ref_count = 0;
    int member_index = 0;

    /*
     * Test no. 3:
     * 'max_duplications' is set to '1'
     * Add one member a few times. Should not fail even beyond 'max_duplications'.
     * Then, add new members over the number of assigned member count and get error.
     */

    CTEST_SW_STATE_FUNC_INIT_VARS(unit);

    sal_memset(&init_info, 0x0, sizeof(sw_state_multi_set_info_t));
    init_info.nof_members = 1;
    init_info.member_size = sizeof(key);
    init_info.max_duplications = 1;
    init_info.print_cb_name = "sw_state_htb_example_entry_print";

    LOG_CLI((BSL_META("Init the example access module.\n")));
    SHR_IF_ERR_EXIT(example.init(unit));

    LOG_CLI((BSL_META("Create the multi set table.\n")));
    SHR_IF_ERR_EXIT(example.multi_set_example.create(unit, &init_info));

    sal_memset(key, 0, sizeof(key));
    key[0] = 37;
    key[1] = 35;
    key[2] = 33;
    key[3] = 31;

    SHR_IF_ERR_EXIT(example.multi_set_example.add(unit, &key[0], &data_index, &first_appear, &success));
    if (!success)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
        LOG_CLI((BSL_META("Failed to add first member to the multi set.\n")));
        SHR_EXIT();
    }

    /*
     * Add the same member again. Should succeed.
     */
    SHR_IF_ERR_EXIT(example.multi_set_example.add(unit, &key[0], &data_index, &first_appear, &success));

    if (!success)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
        LOG_CLI((BSL_META("Failed to add first member to the multi set.\n")));
        SHR_EXIT();
    }

    if (first_appear)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
        LOG_CLI((BSL_META("Add second instance to member %d on multiset but 'first_appear' is non-zero!\n"),
                 data_index));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(example.multi_set_example.get_by_index(unit, data_index, ref_key, &ref_count));

    if (ref_count != init_info.max_duplications)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
        LOG_CLI((BSL_META("Member %d on multiset has ref_count %d which it should be %d!\r\n"), data_index,
                 (int) ref_count, (int) (init_info.max_duplications)));
        SHR_EXIT();
    }

    if (sal_memcmp(key, ref_key, init_info.member_size) != 0)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
        LOG_CLI((BSL_META("Member %d on multiset has wrong key!\r\n"), data_index));

        for (member_index = 0; member_index < init_info.member_size; member_index++)
        {
            LOG_CLI((BSL_META("%02X"), ref_key[member_index]));
        }
        LOG_CLI((BSL_META_U(unit, "\r\n")));
        SHR_EXIT();
    }

    /*
     * Now try to add one member. This is beyond assigned member count. Should fail on 'success'!
     * Should NOT fail on error on ret_val.
     */
    sal_memset(key, 0, sizeof(key));
    key[0] = 19;
    key[1] = 10;
    key[2] = 9;
    key[3] = 6;

    SHR_IF_ERR_EXIT(example.multi_set_example.add(unit, &key[0], &data_index, &first_appear, &success));

    if (success)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
        LOG_CLI((BSL_META("Succeeded to add member to multiset beyond max (%d).\n"), init_info.nof_members));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(example.multi_set_example.print(unit, 0));

    SHR_EXIT();

exit:
    if (SHR_GET_CURRENT_ERR())
    {
        LOG_CLI((BSL_META("Test FAILED\n")));
    }
    else
    {
        LOG_CLI((BSL_META("Test PASSED\n")));
    }
    CTEST_SW_STATE_FUNC_EXIT;

}

static shr_error_e
sw_state_multi_set_test4_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    sw_state_multi_set_info_t init_info;

    SW_STATE_MULTI_SET_KEY key[4];
    SW_STATE_MULTI_SET_KEY ref_key[4];

    uint8 success = 0;
    uint8 first_appear = 0;
    uint32 data_index = 0;
    uint32 ref_count = 0;
    int member_index = 0;
    uint32 loc_ref_count = 0;

    /*
     * Test no. 4:
     * 'max_duplications' is set to '2'
     * Add one member a few times. Should fail after 'max_duplications'.
     */

    CTEST_SW_STATE_FUNC_INIT_VARS(unit);

    sal_memset(&init_info, 0x0, sizeof(sw_state_multi_set_info_t));
    init_info.nof_members = 1;
    init_info.member_size = sizeof(key);
    init_info.max_duplications = 2;
    init_info.print_cb_name = "sw_state_htb_example_entry_print";

    LOG_CLI((BSL_META("Init the example access module.\n")));
    SHR_IF_ERR_EXIT(example.init(unit));

    LOG_CLI((BSL_META("Create the multi set table.\n")));
    SHR_IF_ERR_EXIT(example.multi_set_example.create(unit, &init_info));

    sal_memset(key, 0, sizeof(key));
    key[0] = 32;
    key[1] = 10;
    key[2] = 9;
    key[3] = 8;

    SHR_IF_ERR_EXIT(example.multi_set_example.add(unit, &key[0], &data_index, &first_appear, &success));

    loc_ref_count++;

    if (!success)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
        LOG_CLI((BSL_META("Failed to add first member to the multi set.\n")));
        SHR_EXIT();
    }

    /*
     * Add the same member again. Should succeed.
     */
    SHR_IF_ERR_EXIT(example.multi_set_example.add(unit, &key[0], &data_index, &first_appear, &success));

    loc_ref_count++;

    if (!success)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
        LOG_CLI((BSL_META("Failed to add first member to the multi set.\n")));
        SHR_EXIT();
    }

    if (first_appear)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
        LOG_CLI((BSL_META("Add second instance to member %d on multiset but 'first_appear' is non-zero!\n"),
                 data_index));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(example.multi_set_example.get_by_index(unit, data_index, ref_key, &ref_count));

    if (ref_count != loc_ref_count)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
        LOG_CLI((BSL_META("Member %d on multiset has ref_count %d which it should be %d!\r\n"), data_index,
                 (int) ref_count, (int) (loc_ref_count)));
        SHR_EXIT();
    }

    if (sal_memcmp(key, ref_key, init_info.member_size) != 0)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
        LOG_CLI((BSL_META("Member %d on multiset has wrong key!\r\n"), data_index));

        for (member_index = 0; member_index < init_info.member_size; member_index++)
        {
            LOG_CLI((BSL_META("%02X"), ref_key[member_index]));
        }
        LOG_CLI((BSL_META_U(unit, "\r\n")));
        SHR_EXIT();
    }

    /*
     * Now try to add one member. This is beyond assigned member count. Should fail on 'success'!
     * Should NOT fail on error on ret_val.
     */
    SHR_IF_ERR_EXIT(example.multi_set_example.add(unit, &key[0], &data_index, &first_appear, &success));

    if (success)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
        LOG_CLI((BSL_META("Succeeded to add member to multiset beyond max (%d).\n"), init_info.nof_members));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(example.multi_set_example.print(unit, 0));

    SHR_EXIT();

exit:
    if (SHR_GET_CURRENT_ERR())
    {
        LOG_CLI((BSL_META("Test FAILED\n")));
    }
    else
    {
        LOG_CLI((BSL_META("Test PASSED\n")));
    }
    CTEST_SW_STATE_FUNC_EXIT;

}

/**
 * short unit test for chunk
 */
static shr_error_e
sw_state_defragmented_chunk_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int def_cunk_index_part1 = 0;
    int def_cunk_index_part2 = 1;
    int def_cunk_index_part3 = 2;
    int def_cunk_index_part4 = 3;
    int def_cunk_index_part5 = 4;
    int i;
    int rv;
    uint32 dummy_piece_offset;
    uint32 piece_size_arr_1[6] = { 1, 2, 4, 8, 16, 32 };
    uint32 piece_offset_arr_1[6];
    uint32 piece_size_arr_2[6] = { 32, 16, 8, 4, 2, 1 };
    uint32 piece_offset_arr_2[6];
    uint32 piece_size_arr_3[4] = { 1, 2, 2, 4 };
    uint32 piece_offset_arr_3[4];
    uint32 piece_size_arr_4[22] = { 1, 2, 2, 1, 4, 2, 2, 4, 1, 1, 4, 8, 2, 2, 1, 2, 8, 8, 2, 1, 4, 2 };
    uint32 piece_offset_arr_4[22];
    sw_state_defragmented_chunk_init_info_t init_info;
    bsl_severity_t original_severity_def_chunk, original_severity_sw_state;

    CTEST_SW_STATE_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META
             ("\n If you want the chunks to be printed please run ' debug SHAREDSWDNX DefragChunkDnx warn ', before the test.\n")));

    LOG_CLI((BSL_META("Init the example access module.\n")));
    SHR_IF_ERR_EXIT(example.init(unit));

    /*
     * save the original severity level of the defragmented chunk and sw state.
     */
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SHAREDSWDNX_DEFRAGCHUNKDNX, original_severity_def_chunk);
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_sw_state);

    /*
     * set the severity level of the defragmented chunk and sw state to bslSeverityFatal.
     */
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SHAREDSWDNX_DEFRAGCHUNKDNX, bslSeverityFatal);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, bslSeverityFatal);

    /** Part 1:
     *  create */
    /** initiate init_info */
    init_info.chunk_size = 64;
    sal_strncpy(init_info.move_function, "", SW_STATE_CB_DB_NAME_STR_SIZE - 1);
    /** create chunk */
    LOG_CLI((BSL_META("Create chunk.\n")));
    SHR_IF_ERR_EXIT(example.defragmented_chunk_example.create(unit, def_cunk_index_part1, &init_info));

    LOG_CLI((BSL_META("PART 1 - Create and destroy. DONE.\n\n")));

    /** Part 2:
     *  create chunks and add members of different sizes, check
     *  valid sizes are passing and illeagel sizes are failing.
     *  print the chunk after filling with members */
    /** create defrag chunk of size 64 */
    LOG_CLI((BSL_META("Create chunk.\n")));
    SHR_IF_ERR_EXIT(example.defragmented_chunk_example.create(unit, def_cunk_index_part2, &init_info));
    /** add members of sizes 1,2,4,8,16,32 expecet success */
    LOG_CLI((BSL_META("Add members of sizes 1,2,4,8,16,32 expecet success.\n")));
    for (i = 0; i < 6; ++i)
    {
        SHR_IF_ERR_EXIT(example.
                        defragmented_chunk_example.piece_alloc(unit, def_cunk_index_part2, piece_size_arr_1[i], NULL,
                                                               &piece_offset_arr_1[i]));
    }
    /** add member of size 2 expect failure */
    LOG_CLI((BSL_META("Add member of size 2 expect failure.\n")));
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, bslSeverityFatal);
    rv = example.defragmented_chunk_example.piece_alloc(unit, def_cunk_index_part2, 2, NULL, &dummy_piece_offset);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_sw_state);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "expected this to fail, but it passed, something is wrong\n");
    }
    /** print chunk */
    if (original_severity_def_chunk <= bslSeverityWarn)
    {
        LOG_CLI((BSL_META("Print chunk.\n")));
        SHR_IF_ERR_EXIT(example.defragmented_chunk_example.print(unit, def_cunk_index_part2));
    }

    LOG_CLI((BSL_META("PART 2 - Create, alloc and print. DONE.\n\n")));

    /** Part 3:
     *  create trunk.
     *  add member which is too big, should fail.
     *  add member which is not a power of 2, should fail.
     *  add members in a manner that should force intensive scanning
     *  of the chunk.
     *  add a member which doesn't fit into remaining slots - should
     *  fail.
     *  free member to free some slots.
     *  add membe and make sure that there's place now.
     */
    /** create defrag chunk of size 64 */
    LOG_CLI((BSL_META("Create chunk.\n")));
    SHR_IF_ERR_EXIT(example.defragmented_chunk_example.create(unit, def_cunk_index_part3, &init_info));
    /** add member of size too big to fit in chunk expect failure */
    LOG_CLI((BSL_META("Add member of size too big to fit in chunk expect failure.\n")));
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, bslSeverityFatal);
    rv = example.defragmented_chunk_example.piece_alloc(unit, def_cunk_index_part3, 128, NULL, &dummy_piece_offset);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_sw_state);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "expected this to fail, but it passed, something is wrong\n");
    }
    /** add member which is not a power of 2 expect failure */
    LOG_CLI((BSL_META("Add member which is not a power of 2 expect failure.\n")));
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, bslSeverityFatal);
    rv = example.defragmented_chunk_example.piece_alloc(unit, def_cunk_index_part3, 24, NULL, &dummy_piece_offset);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_sw_state);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "expected this to fail, but it passed, something is wrong\n");
    }
    /** add members of sizes 32,16,8,4,2,1 expect success */
    LOG_CLI((BSL_META("Add members of sizes 32,16,8,4,2,1 expect success.\n")));
    for (i = 0; i < 6; ++i)
    {
        SHR_IF_ERR_EXIT(example.
                        defragmented_chunk_example.piece_alloc(unit, def_cunk_index_part3, piece_size_arr_2[i], NULL,
                                                               &piece_offset_arr_2[i]));
    }
    /** print chunk */
    if (original_severity_def_chunk <= bslSeverityWarn)
    {
        LOG_CLI((BSL_META("Print chunk.\n")));
        SHR_IF_ERR_EXIT(example.defragmented_chunk_example.print(unit, def_cunk_index_part3));
    }
    /** add member of size 32 expect failure */
    LOG_CLI((BSL_META("Add member of size 32 expect failure.\n")));
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, bslSeverityFatal);
    rv = example.defragmented_chunk_example.piece_alloc(unit, def_cunk_index_part3, 32, NULL, &dummy_piece_offset);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_sw_state);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "expected this to fail, but it passed, something is wrong\n");
    }
    /** free member of size 32 expect success */
    LOG_CLI((BSL_META("Free member of size 32 expect success.\n")));
    SHR_IF_ERR_EXIT(example.defragmented_chunk_example.piece_free(unit, def_cunk_index_part3, piece_offset_arr_2[0]));
    /** add member of size 32 expect success */
    LOG_CLI((BSL_META("Add member of size 32 expect success.\n")));
    SHR_IF_ERR_EXIT(example.
                    defragmented_chunk_example.piece_alloc(unit, def_cunk_index_part3, 32, NULL, &dummy_piece_offset));
    /** print chunk */
    if (original_severity_def_chunk <= bslSeverityWarn)
    {
        LOG_CLI((BSL_META("Print chunk.\n")));
        SHR_IF_ERR_EXIT(example.defragmented_chunk_example.print(unit, def_cunk_index_part3));
    }

    LOG_CLI((BSL_META("PART 3 - Create, alloc, free and print. DONE.\n\n")));

    /** Part 4:
     *  check defrag routine is function when multiple defrags
     *  are needed */
    /** create a defrag chunk of size 17 */
    init_info.chunk_size = 17;
    sal_strncpy(init_info.move_function, "", SW_STATE_CB_DB_NAME_STR_SIZE - 1);
    SHR_IF_ERR_EXIT(example.defragmented_chunk_example.create(unit, def_cunk_index_part4, &init_info));
    /** add members of sizes 1,2,2,4 expect success */
    LOG_CLI((BSL_META("Add members of sizes 1,2,2,4 expect success.\n")));
    for (i = 0; i < 4; ++i)
    {
        SHR_IF_ERR_EXIT(example.
                        defragmented_chunk_example.piece_alloc(unit, def_cunk_index_part4, piece_size_arr_3[i], NULL,
                                                               &piece_offset_arr_3[i]));
    }
    /** print chunk */
    if (original_severity_def_chunk <= bslSeverityWarn)
    {
        LOG_CLI((BSL_META("Print chunk.\n")));
        SHR_IF_ERR_EXIT(example.defragmented_chunk_example.print(unit, def_cunk_index_part4));
    }
    /** add member of size 8 expect success - should force
     *  defragmentation */
    LOG_CLI((BSL_META("Add member of size 8 expect success - should force defragmentation.\n")));
    SHR_IF_ERR_EXIT(example.
                    defragmented_chunk_example.piece_alloc(unit, def_cunk_index_part4, 8, NULL, &dummy_piece_offset));
    /** print chunk */
    if (original_severity_def_chunk <= bslSeverityWarn)
    {
        LOG_CLI((BSL_META("Print chunk.\n")));
        SHR_IF_ERR_EXIT(example.defragmented_chunk_example.print(unit, def_cunk_index_part4));
    }
    /** add member of size 1 expect failure */
    LOG_CLI((BSL_META("Add member of size 1 expect failure.\n")));
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, bslSeverityFatal);
    rv = example.defragmented_chunk_example.piece_alloc(unit, def_cunk_index_part4, 1, NULL, &dummy_piece_offset);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_sw_state);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "expected this to fail, but it passed, something is wrong\n");
    }
    /** print chunk */
    if (original_severity_def_chunk <= bslSeverityWarn)
    {
        LOG_CLI((BSL_META("Print chunk.\n")));
        SHR_IF_ERR_EXIT(example.defragmented_chunk_example.print(unit, def_cunk_index_part4));
    }

    LOG_CLI((BSL_META("PART 4 - Check defrag routine is function when multiple defrags are needed. DONE.\n\n")));

    /**
     * Part 5:
     * fill chunk
     * empty chunk
     * fill chunk again
     */
    init_info.chunk_size = 64;
    sal_strncpy(init_info.move_function, "", SW_STATE_CB_DB_NAME_STR_SIZE - 1);
    LOG_CLI((BSL_META("Create chunk.\n")));
    SHR_IF_ERR_EXIT(example.defragmented_chunk_example.create(unit, def_cunk_index_part5, &init_info));
    /** add members of sizes 1,2,2,1,4,2,2,4,1,1,4,8,2,2,1,2,8,8,2,1,4,2 expecet success */
    LOG_CLI((BSL_META("Add members of sizes 1,2,2,1,4,2,2,4,1,1,4,8,2,2,1,2,8,8,2,1,4,2 expecet success.\n")));
    for (i = 0; i < 22; ++i)
    {
        SHR_IF_ERR_EXIT(example.
                        defragmented_chunk_example.piece_alloc(unit, def_cunk_index_part5, piece_size_arr_4[i], NULL,
                                                               &piece_offset_arr_4[i]));
    }
    /** print chunk */
    if (original_severity_def_chunk <= bslSeverityWarn)
    {
        LOG_CLI((BSL_META("Print chunk.\n")));
        SHR_IF_ERR_EXIT(example.defragmented_chunk_example.print(unit, def_cunk_index_part5));
    }
    /** remove members expect success */
    LOG_CLI((BSL_META("Remove members expect success.\n")));
    for (i = 0; i < 22; ++i)
    {
        SHR_IF_ERR_EXIT(example.
                        defragmented_chunk_example.piece_free(unit, def_cunk_index_part5, piece_offset_arr_4[i]));
    }
    /** print chunk */
    if (original_severity_def_chunk <= bslSeverityWarn)
    {
        LOG_CLI((BSL_META("Print chunk.\n")));
        SHR_IF_ERR_EXIT(example.defragmented_chunk_example.print(unit, def_cunk_index_part5));
    }
    /** add members of sizes 1,2,2,1,4,2,2,4,1,1,4,8,2,2,1,2,8,8,2,1,4,2 expecet success */
    LOG_CLI((BSL_META("Add members of sizes 1,2,2,1,4,2,2,4,1,1,4,8,2,2,1,2,8,8,2,1,4,2 expecet success.\n")));
    for (i = 0; i < 22; ++i)
    {
        SHR_IF_ERR_EXIT(example.
                        defragmented_chunk_example.piece_alloc(unit, def_cunk_index_part5, piece_size_arr_4[i], NULL,
                                                               &piece_offset_arr_4[i]));
    }
    /** print chunk */
    if (original_severity_def_chunk <= bslSeverityWarn)
    {
        LOG_CLI((BSL_META("Print chunk.\n")));
        SHR_IF_ERR_EXIT(example.defragmented_chunk_example.print(unit, def_cunk_index_part5));
    }
    LOG_CLI((BSL_META("PART 5 - Fill chunk, empty chunk and fill it again. DONE.\n\n")));

    /*
     * set back the severity level of the defragmented chunk and sw state to theirs original values.
     */
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SHAREDSWDNX_DEFRAGCHUNKDNX, original_severity_def_chunk);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_sw_state);

exit:
    if (SHR_GET_CURRENT_ERR())
    {
        LOG_CLI((BSL_META("Test FAILED\n")));
    }
    else
    {
        LOG_CLI((BSL_META("Test PASSED\n")));
    }
    CTEST_SW_STATE_FUNC_EXIT;
}

static shr_error_e
sw_state_resource_tag_bitmap_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    sw_state_resource_tag_bitmap_create_info_t create_info;
    sw_state_resource_tag_bitmap_alloc_info_t res_tag_bitmap_alloc_info;
    int element = 0;
    int element_iter = 0;
    int nof_free_elements = 0;
    int allocated_element_map[20];
    uint32 grain_tag[5];
    int grain_index = 0;
    CTEST_SW_STATE_FUNC_INIT_VARS(unit);

    sal_memset(allocated_element_map, 0, sizeof(int) * 20);
    sal_memset(grain_tag, 0, sizeof(uint32) * 5);

    sal_memset(&create_info, 0x0, sizeof(sw_state_resource_tag_bitmap_create_info_t));
    create_info.low_id = 0;
    create_info.count = 20;
    create_info.grain_size = 4;
    create_info.max_tag_value = 5;
    create_info.flags = SW_STATE_RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN;

    sal_memset(&res_tag_bitmap_alloc_info, 0, sizeof(sw_state_resource_tag_bitmap_alloc_info_t));
    res_tag_bitmap_alloc_info.align = 1;
    res_tag_bitmap_alloc_info.offs[0] = 0;
    res_tag_bitmap_alloc_info.tag = 0;
    res_tag_bitmap_alloc_info.pattern = 1;
    res_tag_bitmap_alloc_info.length = 1;
    res_tag_bitmap_alloc_info.repeats = 1;
    res_tag_bitmap_alloc_info.flags |= SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_SPARSE;

    /*
     * Turning on the alloc during test flag (set it to 1)
     */
    SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_during_test_set(unit, 1));

    LOG_CLI((BSL_META("Init the example access module.\n")));
    SHR_IF_ERR_EXIT(example.init(unit));

    LOG_CLI((BSL_META("Create the tag bitmap.\n")));
    SHR_IF_ERR_EXIT(example.res_tag_bitmap_test.create(unit, create_info));

    LOG_CLI((BSL_META("Allocate 5 elements with tag=0. They should be allocated in grains: 0 and 1\n")));
    for (element_iter = 0; element_iter < 5; element_iter++)
    {
        SHR_IF_ERR_EXIT(example.res_tag_bitmap_test.bitmap_alloc(unit, res_tag_bitmap_alloc_info, &element));
        allocated_element_map[element] = 1;
    }

    res_tag_bitmap_alloc_info.tag = 1;
    LOG_CLI((BSL_META("Allocate 3 elements with tag=1. They should be allocated in grain: 2\n")));
    for (element_iter = 0; element_iter < 3; element_iter++)
    {
        SHR_IF_ERR_EXIT(example.res_tag_bitmap_test.bitmap_alloc(unit, res_tag_bitmap_alloc_info, &element));
        allocated_element_map[element] = 1;
    }

    res_tag_bitmap_alloc_info.tag = 4;
    LOG_CLI((BSL_META("Allocate 4 elements with tag=4. They should be allocated in grain: 3\n")));
    for (element_iter = 0; element_iter < 4; element_iter++)
    {
        SHR_IF_ERR_EXIT(example.res_tag_bitmap_test.bitmap_alloc(unit, res_tag_bitmap_alloc_info, &element));
        allocated_element_map[element] = 1;
    }

    LOG_CLI((BSL_META("Get the number of free elements in the resource. They should be: 8.\n")));
    SHR_IF_ERR_EXIT(example.res_tag_bitmap_test.nof_free_elements_get(unit, &nof_free_elements));
    if (nof_free_elements != 8)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
        LOG_CLI((BSL_META
                 ("Return wrong number of free elements in the resource.\n They should be 8 but it return %d.\n"),
                 nof_free_elements));
        SHR_EXIT();
    }

    LOG_CLI((BSL_META("Get the grain's tags.\n")));
    for (grain_index = 0; grain_index < create_info.count / create_info.grain_size; grain_index++)
    {
        SHR_IF_ERR_EXIT(example.res_tag_bitmap_test.tag_get(unit, grain_index * create_info.grain_size,
                                                            &grain_tag[grain_index]));
    }

    LOG_CLI((BSL_META("Checks the grain's tag's values.\n")));
    if (grain_tag[0] != 0 || grain_tag[1] != 0 || grain_tag[2] != 1 || grain_tag[3] != 4)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
        LOG_CLI((BSL_META("Return wrong tag value.")));
        SHR_EXIT();
    }

    LOG_CLI((BSL_META("Free all allocated elements.\n")));
    for (element_iter = 0; element_iter < 20; element_iter++)
    {
        if (allocated_element_map[element_iter] == 1)
        {
            SHR_IF_ERR_EXIT(example.res_tag_bitmap_test.bitmap_free(unit, res_tag_bitmap_alloc_info, element_iter));
        }
    }

    LOG_CLI((BSL_META("Test PASSED.\n")));
    SHR_EXIT();

exit:
    /*
     * Turning off the alloc during test flag (set it to 1)
     */
    SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_during_test_set(unit, 0));

    if (SHR_GET_CURRENT_ERR())
    {
        LOG_CLI((BSL_META("Test FAILED.\n")));
    }
    CTEST_SW_STATE_FUNC_EXIT;

}

/**
 * /brief - create a res tag bitmap, allocate a few elements.
 * Used in the journal test to test journal allocation rollback.
 */
static shr_error_e
sw_state_journal_test_alloc_helper(
    int unit)
{
    int element = 0;
    int element_iter = 0;
    int allocated_element_map[20] = { 0 };

    sw_state_resource_tag_bitmap_create_info_t create_info;
    sw_state_resource_tag_bitmap_alloc_info_t res_tag_bitmap_alloc_info;

    SHR_FUNC_INIT_VARS(unit);

    sal_memset(allocated_element_map, 0, sizeof(int) * 20);

    sal_memset(&create_info, 0x0, sizeof(sw_state_resource_tag_bitmap_create_info_t));
    create_info.low_id = 0;
    create_info.count = 20;
    create_info.grain_size = 4;
    create_info.max_tag_value = 5;
    create_info.flags = SW_STATE_RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN;

    sal_memset(&res_tag_bitmap_alloc_info, 0, sizeof(sw_state_resource_tag_bitmap_alloc_info_t));
    res_tag_bitmap_alloc_info.align = 1;
    res_tag_bitmap_alloc_info.offs[0] = 0;
    res_tag_bitmap_alloc_info.tag = 0;
    res_tag_bitmap_alloc_info.pattern = 1;
    res_tag_bitmap_alloc_info.length = 1;
    res_tag_bitmap_alloc_info.repeats = 1;
    res_tag_bitmap_alloc_info.flags |= SW_STATE_RESOURCE_TAG_BITMAP_ALLOC_SPARSE;

    LOG_CLI((BSL_META("Create the tag bitmap.\n")));
    SHR_IF_ERR_EXIT(example.res_tag_bitmap_test.create(unit, create_info));

    LOG_CLI((BSL_META("Allocate 5 elements with tag=0. They should be allocated in grains: 0 and 1\n")));
    for (element_iter = 0; element_iter < 5; element_iter++)
    {
        SHR_IF_ERR_EXIT(example.res_tag_bitmap_test.bitmap_alloc(unit, res_tag_bitmap_alloc_info, &element));
        allocated_element_map[element] = 1;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * /brief - snapshot journal test
 */
static shr_error_e
sw_state_journal_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int idx = 0;
    uint8 test_variable = 0;
    uint32 variable = 0;
    uint32 dnx_data_size = 0;
    uint32 nof_bits_to_alloc = 32;
    int range = 31;
    _shr_pbmp_t test_pbmp;
    uint32 indx = 0;
    uint32 nof_elements = 10;
    uint32 input_array[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

    SHR_FUNC_INIT_VARS(unit);

    /*
     * don't fail the test if error recovery is disabled at compile-time
     */
    if (DNX_ERR_RECOVERY_IS_DISABLED)
    {
        LOG_CLI((BSL_META("Error recovery is not available\n")));
        SHR_EXIT();
    }

    /*
     * debug sw state and allow alloc after init
     */
    sh_process_command(unit, "debug swstatednx +");
    SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_during_test_set(unit, 1));

    /*
     * INIT example module. Should NOT produce an alloc entry in the journal
     */
    LOG_CLI((BSL_META("Init the example access module\n")));
    SHR_IF_ERR_EXIT(example.init(unit));

    /*
     * Alloc a dynamic array which has default values set
     * Should NOT produce an alloc entry in the journal
     */
    LOG_CLI((BSL_META("Alloc memory for DNXdata array\n")));
    SHR_IF_ERR_EXIT(example.default_value_tree.array_dnxdata.alloc(unit));

    /*
     * Alloc memory for default bitmap and clear bits
     */
    LOG_CLI((BSL_META("Allocate the memory for the bitmap variable.\n")));
    SHR_IF_ERR_EXIT(example.bitmap_variable.alloc_bitmap(unit, nof_bits_to_alloc));

    LOG_CLI((BSL_META("Clear the data in the sw state bitmap variable.\n")));
    SHR_IF_ERR_EXIT(example.bitmap_variable.bit_range_clear(unit, 0, range));

    /*
     * Clear port bitmap
     */
    LOG_CLI((BSL_META("Clear the data in the sw state port bitmap variable.\n")));
    SHR_IF_ERR_EXIT(example.pbmp_variable.pbmp_clear(unit));
    _SHR_PBMP_CLEAR(test_pbmp);

    /*
     * Sanity check that the journal is off
     */
    LOG_CLI((BSL_META("Validate that journaling is off at the beginning of the test\n")));
    if (dnx_err_recovery_is_on(unit))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    /*
     * Start journaling
     */
    LOG_CLI((BSL_META("Start journaling.\n")));
    SHR_IF_ERR_EXIT(dnx_rollback_journal_start(unit));

    LOG_CLI((BSL_META("Validate that journaling has been started.\n")));
    if (!dnx_err_recovery_is_on(unit))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    /*
     * Should produce a memcpy entry in journal
     */
    LOG_CLI((BSL_META("Change default value for init\n")));
    SHR_IF_ERR_EXIT(example.default_value_tree.default_value_l1.my_variable.set(unit, 10));

    /*
     * Alloc in journal scenario
     */
    SHR_IF_ERR_EXIT(sw_state_journal_test_alloc_helper(unit));

    /*
     * Validate temporary suppression of the feature
     */
    SHR_IF_ERR_EXIT(dnx_err_recovery_tmp_allow(unit, FALSE));
    SHR_IF_ERR_EXIT(example.default_value_tree.array_dnxdata.default_value_l2.my_variable.set(unit, 0, 10));
    SHR_IF_ERR_EXIT(dnx_err_recovery_tmp_allow(unit, TRUE));

    /*
     * Iterate through all of the elements of the array, change value.
     * Should produce 'dnx_data_size - 1' number of memcpy elements in the journal
     */
    dnx_data_size = dnx_data_module_testing.dbal.vrf_field_size_get(unit);
    for (idx = 1; idx < dnx_data_size; idx++)
    {
        SHR_IF_ERR_EXIT(example.default_value_tree.array_dnxdata.default_value_l2.my_variable.set(unit, idx, 10));
    }

    /*
     * Free memory for DNXdata array. Should produce a free entry in the journal
     */
    LOG_CLI((BSL_META("Free memory for DNXdata array\n")));
    SHR_IF_ERR_EXIT(example.default_value_tree.array_dnxdata.free(unit));

    
    /*
     * Validate mutex journaling
     */
    LOG_CLI((BSL_META("Create new mutex.\n")));
    SHR_IF_ERR_EXIT(example.mutex_test.create(unit));

    LOG_CLI((BSL_META("Take the mutex.\n")));
    SHR_IF_ERR_EXIT(example.mutex_test.take(unit, 0));

    LOG_CLI((BSL_META("Take the mutex again, recursive mutex (should not block thread).\n")));
    SHR_IF_ERR_EXIT(example.mutex_test.take(unit, 0));

    LOG_CLI((BSL_META("Give the mutex.\n")));
    SHR_IF_ERR_EXIT(example.mutex_test.give(unit));

    LOG_CLI((BSL_META("Give the mutex.\n")));
    SHR_IF_ERR_EXIT(example.mutex_test.give(unit));

    LOG_CLI((BSL_META("Destroy the mutex.\n")));
    SHR_IF_ERR_EXIT(example.mutex_test.destroy(unit));

    /*
     * Journal bitmap operations
     */
    LOG_CLI((BSL_META("Set the bit 0 of the bitmap variable.\n")));
    SHR_IF_ERR_EXIT(example.bitmap_variable.bit_set(unit, 0));

    LOG_CLI((BSL_META("Set the data in the sw state bitmap variable.\n")));
    SHR_IF_ERR_EXIT(example.bitmap_variable.bit_range_set(unit, 1, range - 1));

    /*
     * Free memory for bitmap variable. Should produce a free entry in the journal
     */
    LOG_CLI((BSL_META("Free the allocated memory of the bitmap variable.\n")));
    SHR_IF_ERR_EXIT(example.bitmap_variable.free(unit));

    /*
     * Journal port bitmap operations
     */
    SHR_IF_ERR_EXIT(example.pbmp_variable.pbmp_ports_range_add(unit, 0, 2));
    SHR_IF_ERR_EXIT(example.pbmp_variable.pbmp_port_set(unit, 2));
    SHR_IF_ERR_EXIT(example.pbmp_variable.pbmp_or(unit, test_pbmp));

    /*
     * Check if ROLL-BACK and CLEAR the sw state journal works
     */
    LOG_CLI((BSL_META("ROLL-BACK and CLEAR journal\n")));
    SHR_IF_ERR_EXIT(dnx_rollback_journal_end(unit, TRUE));

    /*
     * Sanity check that the journal is off
     */
    LOG_CLI((BSL_META("Validate that journaling is off after roll_back\n")));
    if (dnx_err_recovery_is_on(unit))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    /*
     * Check if value restoration was successful.
     * Just get one entry, instead of validating the entire array
     */
    LOG_CLI((BSL_META("Check if default value was properly restored\n")));
    SHR_IF_ERR_EXIT(example.default_value_tree.array_dnxdata.default_value_l2.my_variable.get(unit, 1, &variable));
    if (variable != 5)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    /*
     * Validate bitmap journaling
     */
    LOG_CLI((BSL_META("Check if bitmap variable set was restored properly.\n")));
    SHR_IF_ERR_EXIT(example.bitmap_variable.bit_get(unit, 0, &test_variable));
    if (test_variable != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }
    LOG_CLI((BSL_META("Check if bitmap variable set range was restored properly.\n")));
    SHR_IF_ERR_EXIT(example.bitmap_variable.bit_get(unit, 1, &test_variable));
    if (test_variable != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    /*
     * Validate port bitmap journaling
     */
    LOG_CLI((BSL_META("Check if port bitmap was restored properly.\n")));
    SHR_IF_ERR_EXIT(example.pbmp_variable.pbmp_member(unit, 0, &test_variable));
    if (0 != test_variable)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    SHR_IF_ERR_EXIT(example.pbmp_variable.pbmp_member(unit, 1, &test_variable));
    if (0 != test_variable)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    SHR_IF_ERR_EXIT(example.pbmp_variable.pbmp_member(unit, 2, &test_variable));
    if (0 != test_variable)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    /*
     * Validated that suppression worked
     */
    LOG_CLI((BSL_META("Check if feature suppression is working correctly\n")));
    SHR_IF_ERR_EXIT(example.default_value_tree.array_dnxdata.default_value_l2.my_variable.get(unit, 0, &variable));
    if (variable != 10)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    /*
     * Check default value of variable outside of array
     */
    LOG_CLI((BSL_META("Check if journaling was working prior to suppression\n")));
    SHR_IF_ERR_EXIT(example.default_value_tree.default_value_l1.my_variable.get(unit, &variable));
    if (variable != 5)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    /*
     * Free memory for DNXdata array after journaling has been disabled
     */
    LOG_CLI((BSL_META("Free memory for DNXdata array\n")));
    SHR_IF_ERR_EXIT(example.default_value_tree.array_dnxdata.free(unit));

    /*
     * Free memory for bitmap variable after journaling has been disabled
     */
    LOG_CLI((BSL_META("Free the allocated memory of the bitmap variable.\n")));
    SHR_IF_ERR_EXIT(example.bitmap_variable.free(unit));

    /*
     * Fire up a new transaction to validate that the alloc was successfully rolled back.
     * If alloc rollback was not successful there will be a double allocation attempted error.
     */
    LOG_CLI((BSL_META("Start journaling to validate alloc...\n")));
    SHR_IF_ERR_EXIT(dnx_rollback_journal_start(unit));

    SHR_IF_ERR_EXIT(sw_state_journal_test_alloc_helper(unit));

    LOG_CLI((BSL_META("Roll-back the allocations\n")));
    SHR_IF_ERR_EXIT(dnx_rollback_journal_end(unit, TRUE));

    LOG_CLI((BSL_META("\nValidate nested transactions...\n")));
    LOG_CLI((BSL_META("Start top-level transaction...\n")));
    SHR_IF_ERR_EXIT(dnx_rollback_journal_start(unit));

    for (indx = 0; indx < nof_elements; indx++)
    {
        SHR_IF_ERR_EXIT(example.my_array.set(unit, indx, input_array[indx]));
    }

    LOG_CLI((BSL_META("Start nested transaction...\n")));
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_start(unit));
        SHR_IF_ERR_EXIT(example.my_array.set(unit, 0, 10));
        LOG_CLI((BSL_META("Start nested empty transaction...\n")));
        {
            SHR_IF_ERR_EXIT(dnx_rollback_journal_start(unit));
            SHR_IF_ERR_EXIT(dnx_rollback_journal_end(unit, FALSE));
        }
        SHR_IF_ERR_EXIT(dnx_rollback_journal_end(unit, FALSE));
        LOG_CLI((BSL_META("Commit nested transaction...\n")));

        LOG_CLI((BSL_META("Check value after transaction commit...\n")));
        SHR_IF_ERR_EXIT(example.my_array.get(unit, 0, &variable));
        if (10 != variable)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
        }
    }

    LOG_CLI((BSL_META("Start nested transaction...\n")));
    {
        SHR_IF_ERR_EXIT(dnx_rollback_journal_start(unit));
        SHR_IF_ERR_EXIT(example.my_array.set(unit, 0, 1));
        SHR_IF_ERR_EXIT(dnx_rollback_journal_end(unit, TRUE));
        LOG_CLI((BSL_META("Rollback nested transaction...\n")));

        LOG_CLI((BSL_META("Check value after transaction commit...\n")));
        SHR_IF_ERR_EXIT(example.my_array.get(unit, 0, &variable));
        if (10 != variable)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
        }
    }

    LOG_CLI((BSL_META("Rollback top-level transaction...\n")));
    SHR_IF_ERR_EXIT(dnx_rollback_journal_end(unit, TRUE));

    LOG_CLI((BSL_META("Check value after top level transaction commit...\n")));
    SHR_IF_ERR_EXIT(example.my_array.get(unit, 0, &variable));
    if (0 != variable)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    /*
     * Deinit the example module after the journal has been stopped
     */
    LOG_CLI((BSL_META("Deinit the example access module\n")));
    SHR_IF_ERR_EXIT(example.deinit(unit));

    LOG_CLI((BSL_META("Test PASSED\n")));

    SHR_EXIT();

exit:
    dnxc_sw_state_alloc_during_test_set(unit, 0);
    sh_process_command(unit, "debug swstatednx -");
    SHR_FUNC_EXIT;
}

static shr_error_e
sw_state_string_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int cmp_result;
    char test_string[100];
    uint32 size;
    CTEST_SW_STATE_FUNC_INIT_VARS(unit);

    sal_memset(test_string, 0, 100);
    LOG_CLI((BSL_META("Init the example access module.\n")));
    SHR_IF_ERR_EXIT(example.init(unit));

    SHR_IF_ERR_EXIT(example.string.stringncpy(unit, ""));
    SHR_IF_ERR_EXIT(example.string.stringncpy(unit, ""));
    SHR_IF_ERR_EXIT(example.string.stringncmp(unit, test_string, &cmp_result));
    if (cmp_result != 0)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
        LOG_CLI((BSL_META("The string should be empty.\n")));
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(example.string.stringlen(unit, &size));
    if (size != 0)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
        LOG_CLI((BSL_META("The size should be 0, but it is: (%d) \n"), size));
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(example.string.stringncpy(unit, "Test string."));
    SHR_IF_ERR_EXIT(example.string.stringget(unit, test_string));
    SHR_IF_ERR_EXIT(example.string.stringncmp(unit, test_string, &cmp_result));
    if (cmp_result)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
        LOG_CLI((BSL_META("The received and the saved string are not equal.\n")));
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(example.string.stringncpy
                    (unit,
                     "TEst string veru long string to test the size check 11111222223333344444555556666677777788888899999"));
    SHR_IF_ERR_EXIT(example.string.stringncmp(unit, test_string, &cmp_result));
    if (cmp_result != -1)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
        LOG_CLI((BSL_META("The test_string is smaller but in didn't return -1.\n")));
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(example.string.stringncpy(unit, "test string"));
    SHR_IF_ERR_EXIT(example.string.stringncmp(unit, test_string, &cmp_result));
    if (cmp_result != 1)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
        LOG_CLI((BSL_META("The test_string is bigger but it didn't return 1.\n")));
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(example.string.stringncpy(unit, "Tes"));
    SHR_IF_ERR_EXIT(example.string.stringncat(unit, "t string."));
    SHR_IF_ERR_EXIT(example.string.stringncmp(unit, test_string, &cmp_result));
    if (cmp_result)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
        LOG_CLI((BSL_META("The received and the saved string are not equal.\n")));
        SHR_EXIT();
    }
    SHR_IF_ERR_EXIT(example.string.stringlen(unit, &size));
    if (size != 12)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_INTERNAL);
        LOG_CLI((BSL_META("The size is different (%d) than the used during the creation.\n"), size));
        SHR_EXIT();
    }

    LOG_CLI((BSL_META("Test PASSED.\n")));
    SHR_EXIT();

exit:
    if (SHR_GET_CURRENT_ERR())
    {
        LOG_CLI((BSL_META("Test FAILED.\n")));
    }
    CTEST_SW_STATE_FUNC_EXIT;

}

static shr_error_e
sw_state_large_ds_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    sw_state_htbl_init_info_t htb_init_info;
    sw_state_multi_set_info_t multiset_init_info;
    sw_state_occ_bitmap_init_info_t btmp_init_info;
    sw_state_resource_tag_bitmap_create_info_t res_tag_bitmap_create_info;

    CTEST_SW_STATE_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META
             ("The test will try to create large size DSs, but the addional arguments form the xml will overwrite the lagre number of els.\n")));

    LOG_CLI((BSL_META("Init the example access module\n")));
    SHR_IF_ERR_EXIT(example.init(unit));

    /*
     * Init info with large nof_elements.
     * There is is nof_element arguments in the xml file for this variable and we are expecting to replace the huge value.
     */
    sal_memset(&htb_init_info, 0, sizeof(sw_state_htbl_init_info_t));
    htb_init_info.max_nof_elements = 123456789;
    htb_init_info.expected_nof_elements = 10;
    htb_init_info.hash_function = NULL;
    htb_init_info.print_cb_name = "sw_state_htb_example_entry_print";

    LOG_CLI((BSL_META("Create the hash table.\n")));
    SHR_IF_ERR_EXIT(example.large_DS_examples.large_htb.create(unit, &htb_init_info));

    sal_memset(&multiset_init_info, 0, sizeof(sw_state_multi_set_info_t));
    multiset_init_info.nof_members = 2;
    multiset_init_info.member_size = 32;
    multiset_init_info.max_duplications = 3;
    multiset_init_info.print_cb_name = "sw_state_htb_example_entry_print";

    LOG_CLI((BSL_META("Create the multi set table.\n")));
    SHR_IF_ERR_EXIT(example.large_DS_examples.large_multi_set_example.create(unit, &multiset_init_info));

    SHR_IF_ERR_EXIT(sw_state_occ_bm_init_info_clear(&btmp_init_info));

    sal_memset(&btmp_init_info, 0, sizeof(sw_state_occ_bitmap_init_info_t));
    btmp_init_info.size = 123456789;

    SHR_IF_ERR_EXIT(example.large_DS_examples.large_occ.create(unit, &btmp_init_info));

    sal_memset(&res_tag_bitmap_create_info, 0x0, sizeof(sw_state_resource_tag_bitmap_create_info_t));
    res_tag_bitmap_create_info.low_id = 0;
    res_tag_bitmap_create_info.count = 123456789;
    res_tag_bitmap_create_info.grain_size = 1;
    res_tag_bitmap_create_info.max_tag_value = 5;
    res_tag_bitmap_create_info.flags = SW_STATE_RESOURCE_TAG_BITMAP_CREATE_GET_NOF_ELEMENTS_PER_GRAIN;

    LOG_CLI((BSL_META("Create the tag bitmap.\n")));
    SHR_IF_ERR_EXIT(example.large_DS_examples.large_res_tag_bitmap_test.create(unit, res_tag_bitmap_create_info));

    LOG_CLI((BSL_META("Test PASSED\n")));

    SHR_EXIT();

exit:
    if (SHR_GET_CURRENT_ERR())
    {
        LOG_CLI((BSL_META("Test FAILED\n")));
    }
    CTEST_SW_STATE_FUNC_EXIT;

}

static sh_sand_man_t sw_state_int_test_man = {
    "Test a simple int varable"
};

static sh_sand_man_t sw_state_mutex_test_man = {
    "Test mutex options"
};

static sh_sand_man_t sw_state_array_range_test_man = {
    "SW State range array commands test"
};
static sh_sand_man_t sw_state_DNXData_test_man = {
    "Test DNXData sw state commands."
};

static sh_sand_man_t sw_state_input_value_test_man = {
    "SW State intput value verification test.",
    "The test set a valid and invalid value to a variable and expect only the valid value to be set."
};

static sh_sand_man_t sw_state_counter_test_man = {
    "Test counter options",
    "Try to assign value to see how it will be accepted. Increment the input value check if it is incremented correctly, decrement it and check the value again."
};

static sh_sand_man_t sw_state_alloc_after_init_test_man = {
    "Test allocation after init exception ",
    "Try to allocate memory for variable with and without alloc after init exception."
};

static sh_sand_man_t sw_state_pointer_test_man = {
    "Test pointer variable usage."
};

static sh_sand_man_t sw_state_arrays_test_man = {
    "Test the usage of the arrays in sw state."
};

static sh_sand_man_t sw_state_bitmap_test_man = {
    "Test bitmap variable usage",
    "Test the entire set of functionality of the bitmap variable."
};

static sh_sand_man_t sw_state_pbmp_test_man = {
    "Test port bitmap variable usage",
    "Test the entire set of functionality of the port bitmap variable.",
    "ctest swstate pbmp",
    "ctest swstate pbmp",
};

static sh_sand_man_t sw_state_buffer_test_man = {
    "Test buffer variable usage",
    "Test the entire set of functionality of the buffer variable."
};

static sh_sand_man_t sw_state_default_value_test_man = {
    "Test default value for variables",
    "Verify default_value attribute validity set for basic parameter types, bitmaps and buffers."
};

static sh_sand_man_t sw_state_ll_test_man = {
    "Test linked list variable usage",
    "Test the entire set of functionality of the linked list DS variable."
};

static sh_sand_man_t sw_state_multihead_ll_test_man = {
    "Test multihead linked list variable usage",
    "Test the entire set of functionality of the multihead linked list DS variable."
};

static sh_sand_man_t sw_state_sorted_ll_test_man = {
    "Test sorted linked list variable usage",
    "Test the entire set of functionality of the sorted linked list DS variable."
};

static sh_sand_man_t sw_state_htb_test_man = {
    "Test hash table variable usage",
    "Test the entire set of functionality of the hash table DS variable."
};

static sh_sand_man_t sw_state_htb_arr_test_man = {
    "Test hash table array variable usage",
    "Test array of hash tables with different key/value sizes."
};

static sh_sand_man_t sw_state_occ_bitmap_test_man = {
    "Test occ bitmap variable usage",
    "Test the entire set of functionality of the occupation bitmap DS variable."
};

static sh_sand_man_t sw_state_enum_test_man = {
    "Test enum get_name function",
    "Test enum get_name function for different enum values."
};

static sh_sand_man_t sw_state_callback_test_man = {
    "Test callback's functions",
    "Test _register_cb, _unregister_cb and _get_cb functions."
};

static sh_sand_man_t sw_state_multi_set_test_man = {
    "Test multi_set variable usage",
    "Test the entire set of functionality of the multi set DS variable."
};

static sh_sand_man_t sw_state_defragmented_chunk_test_man = {
    "Test drefragmented chunks variable usage",
    "Test the entire set of functionality of the drefragmented chunks DS variable.",
    "ctest swstate defragmentedchunk",
    "ctest swstate defragmentedchunk",
};

static sh_sand_man_t sw_resource_tag_bitmap_test_man = {
    "Test resource tag bitmap variable usage",
    "Test the entire set of functionality of the resource tag bitmap DS variable.",
    "ctest swstate ReSouRCeTagBitmap",
    "ctest swstate ReSouRCeTagBitmap",
};

static sh_sand_man_t sw_state_journal_test_man = {
    "Swstate journal test",
    "Test the swstate rollback journal"
};

static sh_sand_man_t sw_state_string_test_man = {
    "Test string variable usage",
    "Test the entire set of functionality of the sw state string variable."
};

static sh_sand_man_t sw_state_large_ds_test_man = {
    "Test large DS alloc usage",
    "Test the large DS alloc size checks."
};

sh_sand_cmd_t dnx_swstate_test_cmds[] = {
    {"integer", sw_state_int_test_cmd, NULL, NULL, &sw_state_int_test_man, NULL, NULL, CTEST_PRECOMMIT},
    {"counter", sw_state_counter_test_cmd, NULL, NULL, &sw_state_counter_test_man, NULL, NULL, CTEST_PRECOMMIT},
    {"mutex", sw_state_mutex_test_cmd, NULL, NULL, &sw_state_mutex_test_man, NULL, NULL, CTEST_PRECOMMIT},
    {"data", sw_state_DNXData_test_cmd, NULL, NULL, &sw_state_DNXData_test_man, NULL, NULL, CTEST_PRECOMMIT},
    {"value", sw_state_input_value_test_cmd, NULL, NULL, &sw_state_input_value_test_man, NULL, NULL, CTEST_PRECOMMIT},
    {"range", sw_state_array_range_test_cmd, NULL, NULL, &sw_state_array_range_test_man, NULL, NULL, CTEST_PRECOMMIT},
    {"allocate", sw_state_alloc_test_cmd, NULL, NULL, &sw_state_alloc_after_init_test_man, NULL, NULL, CTEST_PRECOMMIT},
    {"pointer", sw_state_pointer_test_cmd, NULL, NULL, &sw_state_pointer_test_man, NULL, NULL, CTEST_PRECOMMIT},
    {"array", sw_state_arrays_test_cmd, NULL, NULL, &sw_state_arrays_test_man, NULL, NULL, CTEST_PRECOMMIT},
    {"bitmap", sw_state_bitmap_test_cmd, NULL, NULL, &sw_state_bitmap_test_man, NULL, NULL, CTEST_PRECOMMIT},
    {"portbitmap", sw_state_pbmp_test_cmd, NULL, NULL, &sw_state_pbmp_test_man, NULL, NULL, CTEST_PRECOMMIT},
    {"buffer", sw_state_buffer_test_cmd, NULL, NULL, &sw_state_buffer_test_man, NULL, NULL, CTEST_PRECOMMIT},
    {"default", sw_state_default_value_test_cmd, NULL, NULL, &sw_state_default_value_test_man, NULL, NULL,
     CTEST_PRECOMMIT},
    {"linkedlist", sw_state_ll_test_cmd, NULL, NULL, &sw_state_ll_test_man, NULL, NULL, CTEST_PRECOMMIT},
    {"multiheadlinkedlist", sw_state_multihead_ll_test_cmd, NULL, NULL, &sw_state_multihead_ll_test_man, NULL, NULL,
     CTEST_PRECOMMIT},
    {"sorted_ll", sw_state_sorted_ll_test_cmd, NULL, NULL, &sw_state_sorted_ll_test_man, NULL, NULL, CTEST_PRECOMMIT},
    {"hashtable", sw_state_htb_test_cmd, NULL, NULL, &sw_state_htb_test_man, NULL, NULL, CTEST_PRECOMMIT},
    {"hashtable_arr", sw_state_htb_arr_test_cmd, NULL, NULL, &sw_state_htb_arr_test_man, NULL, NULL, CTEST_PRECOMMIT},
    {"occ_bitmap1", sw_state_occupation_bitmap_test1_cmd, NULL, NULL, &sw_state_occ_bitmap_test_man, NULL, NULL,
     CTEST_PRECOMMIT},
    {"occ_bitmap2", sw_state_occupation_bitmap_test2_cmd, NULL, NULL, &sw_state_occ_bitmap_test_man, NULL, NULL,
     CTEST_PRECOMMIT},
    {"occ_bitmap3", sw_state_occupation_bitmap_test3_cmd, NULL, NULL, &sw_state_occ_bitmap_test_man, NULL, NULL,
     CTEST_PRECOMMIT},
    {"occ_bitmap4", sw_state_occupation_bitmap_test4_cmd, NULL, NULL, &sw_state_occ_bitmap_test_man, NULL, NULL,
     CTEST_PRECOMMIT},
    {"enum", sw_state_enum_test_cmd, NULL, NULL, &sw_state_enum_test_man, NULL, NULL, CTEST_PRECOMMIT},
    {"callback", sw_state_callback_test_cmd, NULL, NULL, &sw_state_callback_test_man, NULL, NULL, CTEST_PRECOMMIT},
    {"multiset1", sw_state_multi_set_test1_cmd, NULL, NULL, &sw_state_multi_set_test_man, NULL, NULL, CTEST_PRECOMMIT},
    {"multiset2", sw_state_multi_set_test2_cmd, NULL, NULL, &sw_state_multi_set_test_man, NULL, NULL, CTEST_PRECOMMIT},
    {"multiset3", sw_state_multi_set_test3_cmd, NULL, NULL, &sw_state_multi_set_test_man, NULL, NULL, CTEST_PRECOMMIT},
    {"multiset4", sw_state_multi_set_test4_cmd, NULL, NULL, &sw_state_multi_set_test_man, NULL, NULL, CTEST_PRECOMMIT},
    {"defragmentedchunk", sw_state_defragmented_chunk_test_cmd, NULL, NULL, &sw_state_defragmented_chunk_test_man, NULL,
     NULL, CTEST_PRECOMMIT},
    {"ReSouRCeTagBitmap", sw_state_resource_tag_bitmap_test_cmd, NULL, NULL, &sw_resource_tag_bitmap_test_man, NULL,
     NULL, CTEST_PRECOMMIT},
    {"JouRNaL", sw_state_journal_test_cmd, NULL, NULL, &sw_state_journal_test_man, NULL, NULL, CTEST_PRECOMMIT},
    {"STRing", sw_state_string_test_cmd, NULL, NULL, &sw_state_string_test_man, NULL, NULL, CTEST_PRECOMMIT},
    {"LARGE_DataStruct", sw_state_large_ds_test_cmd, NULL, NULL, &sw_state_large_ds_test_man, NULL, NULL,
     CTEST_PRECOMMIT},
    {NULL}
};
