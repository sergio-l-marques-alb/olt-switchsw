/*
 * $Id: diag_sw_state_tests.c,v 1.20 Broadcom SDK $
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 * File:        diag_sw_state_tests.c
 * Purpose:     Diag shell sw state tests
 */

#include <shared/bsl.h>
#include <shared/pbmp.h>
#include <shared/utilex/utilex_framework.h>

#include <sal/appl/sal.h>

#include <appl/diag/shell.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <soc/dnx/swstate/auto_generated/access/example_access.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dnx_state_snapshot_manager.h>
#include <soc/dnxc/swstate/dnxc_sw_state_journal.h>
#include <soc/dnxc/swstate/dnx_sw_state_dump.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
#include <soc/dnxc/swstate/auto_generated/access/dnxc_module_ids_access.h>
#include <soc/dnxc/swstate/dnxc_sw_state_verifications.h>
#include <appl/ctest/dnxc/ctest_dnxc_utils.h>
#include <soc/dnxc/swstate/dnxc_sw_state_dispatcher.h>
#ifdef BCM_WARM_BOOT_SUPPORT
#include <soc/dnxc/swstate/dnxc_sw_state_wb.h>
#endif /* BCM_WARM_BOOT_SUPPORT */

#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

/** used in ctest commands to store soc properties to be restored */
static rhhandle_t dnx_sw_state_example_soc_set_h[SOC_MAX_NUM_DEVICES] = { NULL };

static ctest_soc_property_t ctest_soc_property[] = {
    {"custom_feature_example_init", "1"},
    {NULL}
};

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
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &dnx_sw_state_example_soc_set_h[unit]));

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
    ctest_dnxc_restore_soc_properties(unit, dnx_sw_state_example_soc_set_h[unit]);
    SHR_FUNC_EXIT;
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
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &dnx_sw_state_example_soc_set_h[unit]));

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
    ctest_dnxc_restore_soc_properties(unit, dnx_sw_state_example_soc_set_h[unit]);
    SHR_FUNC_EXIT;
}

static shr_error_e
sw_state_counter_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 expected_value = 5;
    uint32 retrieved_value = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &dnx_sw_state_example_soc_set_h[unit]));

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
    ctest_dnxc_restore_soc_properties(unit, dnx_sw_state_example_soc_set_h[unit]);
    SHR_FUNC_EXIT;
}

static shr_error_e
sw_state_mutex_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint8 is_created = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &dnx_sw_state_example_soc_set_h[unit]));

    SHR_IF_ERR_EXIT(example.mutex_test.is_created(unit, &is_created));
    if (!is_created)
    {
        LOG_CLI((BSL_META("Unit: %d, Mutex is not created!\n"), unit));
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    LOG_CLI((BSL_META("Take the mutex.\n")));
    SHR_IF_ERR_EXIT(example.mutex_test.take(unit, 0));

    LOG_CLI((BSL_META("Give the mutex.\n")));
    SHR_IF_ERR_EXIT(example.mutex_test.give(unit));

    LOG_CLI((BSL_META("Test PASSED\n")));
    sh_process_command(unit, "transaction swstate print");

    SHR_EXIT();

exit:
    ctest_dnxc_restore_soc_properties(unit, dnx_sw_state_example_soc_set_h[unit]);
    SHR_FUNC_EXIT;
}

static shr_error_e
sw_state_sem_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint8 is_created = FALSE;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &dnx_sw_state_example_soc_set_h[unit]));

    SHR_IF_ERR_EXIT(example.sem_test.is_created(unit, &is_created));
    if (!is_created)
    {
        LOG_CLI((BSL_META("Unit: %d, Semaphore is not created!\n"), unit));
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    LOG_CLI((BSL_META("Take the semaphore.\n")));
    SHR_IF_ERR_EXIT(example.sem_test.take(unit, 0));

    LOG_CLI((BSL_META("Give the semaphore.\n")));
    SHR_IF_ERR_EXIT(example.sem_test.give(unit));

    LOG_CLI((BSL_META("Test PASSED\n")));
    sh_process_command(unit, "transaction swstate print");

    SHR_EXIT();

exit:
    ctest_dnxc_restore_soc_properties(unit, dnx_sw_state_example_soc_set_h[unit]);
    SHR_FUNC_EXIT;
}

static shr_error_e
sw_state_input_value_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int value = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &dnx_sw_state_example_soc_set_h[unit]));

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
    ctest_dnxc_restore_soc_properties(unit, dnx_sw_state_example_soc_set_h[unit]);
    SHR_FUNC_EXIT;
}

static shr_error_e
sw_state_DNXData_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
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

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &dnx_sw_state_example_soc_set_h[unit]));

    dnx_data_size = dnx_data_module_testing.example_tests.field_size_get(unit);

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
    ctest_dnxc_restore_soc_properties(unit, dnx_sw_state_example_soc_set_h[unit]);
    SHR_FUNC_EXIT;
}

static shr_error_e
sw_state_alloc_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 test_variable = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &dnx_sw_state_example_soc_set_h[unit]));

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
    ctest_dnxc_restore_soc_properties(unit, dnx_sw_state_example_soc_set_h[unit]);
    SHR_FUNC_EXIT;
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

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &dnx_sw_state_example_soc_set_h[unit]));

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
    ctest_dnxc_restore_soc_properties(unit, dnx_sw_state_example_soc_set_h[unit]);
    SHR_FUNC_EXIT;
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

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &dnx_sw_state_example_soc_set_h[unit]));

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
    ctest_dnxc_restore_soc_properties(unit, dnx_sw_state_example_soc_set_h[unit]);
    SHR_FUNC_EXIT;
}

static shr_error_e
sw_state_bitmap_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int range = 32;
    int count = 0;
    uint8 test_variable = 0;
    uint8 test_variable_fixed = 0;
    SHR_BITDCL input_bmp = 20;
    SHR_BITDCL output_bmp = 0;

    uint32 static_bitmap_size = 1024;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &dnx_sw_state_example_soc_set_h[unit]));

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
    ctest_dnxc_restore_soc_properties(unit, dnx_sw_state_example_soc_set_h[unit]);
    SHR_FUNC_EXIT;
}

static shr_error_e
sw_state_bitmap_access_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    CONST SHR_BITDCL *var_out;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &dnx_sw_state_example_soc_set_h[unit]));

    /*
     * bit-set -> 20 = 0001 0100 
     */
    SHR_IF_ERR_EXIT(example.bitmap_variable_access.bit_set(unit, 2));
    SHR_IF_ERR_EXIT(example.bitmap_variable_access.bit_set(unit, 4));

    /*
     * test init variable
     */
    LOG_CLI((BSL_META("Get the value of the bitmap data from pointer.\n")));
    SHR_IF_ERR_EXIT(example.bitmap_variable_access.get(unit, &var_out));

    LOG_CLI((BSL_META("Check the get data from pointer.\n")));
    if (*var_out != 20)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    LOG_CLI((BSL_META("Test PASSED\n")));

exit:
    ctest_dnxc_restore_soc_properties(unit, dnx_sw_state_example_soc_set_h[unit]);
    SHR_FUNC_EXIT;
}

static shr_error_e
sw_state_buffer_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 nof_buffer_elements = 10;
    int cmp_result = 1;
    uint8 input_array[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    uint8 output_array[10];
    uint32 indx = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &dnx_sw_state_example_soc_set_h[unit]));

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
    ctest_dnxc_restore_soc_properties(unit, dnx_sw_state_example_soc_set_h[unit]);
    SHR_FUNC_EXIT;
}

static shr_error_e
sw_state_default_value_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int cmp_result = 1;
    uint8 test_variable = 0;
    uint32 variable = 0;

    uint32 nof_buffer_elements = 10;
    uint8 input_array_default_value[10] = { 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA };

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &dnx_sw_state_example_soc_set_h[unit]));

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

    LOG_CLI((BSL_META("Compare the default values stored in the buffer and default value input array.\n")));
    example.buffer.memcmp(unit, input_array_default_value, 0, nof_buffer_elements, &cmp_result);
    if (cmp_result == 1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    LOG_CLI((BSL_META("Test PASSED\n")));

    SHR_EXIT();

exit:
    ctest_dnxc_restore_soc_properties(unit, dnx_sw_state_example_soc_set_h[unit]);
    SHR_FUNC_EXIT;
}

static shr_error_e
sw_state_ll_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    sw_state_ll_node_t output_node;
    int value = 0;
    int value1 = 1;
    int value2 = 2;
    int value3 = 3;
    int value4 = 4;
    int value5 = 5;
    int value6 = 6;
    int value7 = 7;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &dnx_sw_state_example_soc_set_h[unit]));

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
    ctest_dnxc_restore_soc_properties(unit, dnx_sw_state_example_soc_set_h[unit]);
    SHR_FUNC_EXIT;
}

static shr_error_e
sw_state_ll_max_nodes_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    sw_state_ll_node_t output_node;
    int value = 0;
    int value1 = 1;
    int value2 = 2;
    int value3 = 3;
    int value4 = 4;
    int value5 = 5;
    int value6 = 6;
    int value7 = 7;
    int value8 = 8;
    int value9 = 9;
    int value10 = 10;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &dnx_sw_state_example_soc_set_h[unit]));

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

    LOG_CLI((BSL_META("insert value 8 at the end of the list\n")));
    SHR_IF_ERR_EXIT(example.ll.add_last(unit, &value8));

    LOG_CLI((BSL_META("insert value 9 at the end of the list\n")));
    SHR_IF_ERR_EXIT(example.ll.add_last(unit, &value9));

    LOG_CLI((BSL_META("insert value 10 at the end of the list\n")));
    SHR_IF_ERR_EXIT(example.ll.add_last(unit, &value10));

    LOG_CLI((BSL_META("print the list, expect to see 1->2->3->4->5->6->7->8->9->10, visual only\n")));
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
    ctest_dnxc_restore_soc_properties(unit, dnx_sw_state_example_soc_set_h[unit]);
    SHR_FUNC_EXIT;
}

static shr_error_e
sw_state_multihead_ll_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
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

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &dnx_sw_state_example_soc_set_h[unit]));

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
    ctest_dnxc_restore_soc_properties(unit, dnx_sw_state_example_soc_set_h[unit]);
    SHR_FUNC_EXIT;
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

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &dnx_sw_state_example_soc_set_h[unit]));

    num_bits = 64;

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
    else
    {
        LOG_CLI((BSL_META("Test PASSED\n")));
    }
    ctest_dnxc_restore_soc_properties(unit, dnx_sw_state_example_soc_set_h[unit]);
    SHR_FUNC_EXIT;
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
    bsl_severity_t original_severity_sw_state = BSL_INFO, original_shell_cmd = BSL_INFO;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * save the original severity level of the sw state and shell severity level to bslSeverityFatal..
     */
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_sw_state);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, bslSeverityFatal);

    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_APPL_SHELL, original_shell_cmd);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_APPL_SHELL, bslSeverityFatal);

    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &dnx_sw_state_example_soc_set_h[unit]));

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
    /*
     * set back the severity level of the shell and sw state to theirs original values.
     */
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_sw_state);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_APPL_SHELL, original_shell_cmd);

    if (SHR_GET_CURRENT_ERR())
    {
        LOG_CLI((BSL_META("Test FAILED\n")));
    }
    else
    {
        LOG_CLI((BSL_META("Test PASSED\n")));
    }
    ctest_dnxc_restore_soc_properties(unit, dnx_sw_state_example_soc_set_h[unit]);
    SHR_FUNC_EXIT;
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
    bsl_severity_t original_severity_sw_state = BSL_INFO, original_shell_cmd = BSL_INFO;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * save the original severity level of the sw state and shell severity level to bslSeverityFatal..
     */
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_sw_state);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, bslSeverityFatal);

    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_APPL_SHELL, original_shell_cmd);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_APPL_SHELL, bslSeverityFatal);

    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &dnx_sw_state_example_soc_set_h[unit]));

    for (indx = 0; indx < 702; ++indx)
    {
        SHR_IF_ERR_EXIT(example.occ2.get_next(unit, &place, &found));

        LOG_CLI((BSL_META_U(unit, "found %u \n"), place));
        if (found)
        {
            SHR_IF_ERR_EXIT(example.occ2.print(unit));
        }

        if ((indx < 700 && place != indx) || (indx >= 700 && found))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "found %u instead of %u\n", place, indx);
        }

        SHR_IF_ERR_EXIT(example.occ2.status_set(unit, place, TRUE));
    }
    /*
     * free one bit on 200
     */
    SHR_IF_ERR_EXIT(example.occ2.status_set(unit, 200, FALSE));

    /*
     * get next free place (has to be 200)
     */
    SHR_IF_ERR_EXIT(example.occ2.get_next(unit, &place, &found));

    LOG_CLI((BSL_META_U(unit, "found %u \n"), place));
    if (found)
    {
        SHR_IF_ERR_EXIT(example.occ2.print(unit));
    }

    if (place != 200)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "found %u instead of 200\n", place);
    }

    SHR_IF_ERR_EXIT(example.occ2.status_set(unit, place, TRUE));
    /*
     * free 4 bits on 630, 0, 150, 320
     */
    expected_places[0] = 0;
    expected_places[1] = 150;
    expected_places[2] = 320;
    expected_places[3] = 630;
    expected_places[4] = 0;

    SHR_IF_ERR_EXIT(example.occ2.status_set(unit, 630, FALSE));
    SHR_IF_ERR_EXIT(example.occ2.status_set(unit, 0, FALSE));
    SHR_IF_ERR_EXIT(example.occ2.status_set(unit, 150, FALSE));
    SHR_IF_ERR_EXIT(example.occ2.status_set(unit, 320, FALSE));

    /*
     * allocate 5 bits 4 should be 630, 0, 150, 320 and one not found.
     */
    for (indx = 0; indx < 5; ++indx)
    {
        SHR_IF_ERR_EXIT(example.occ2.get_next(unit, &place, &found));

        LOG_CLI((BSL_META_U(unit, "found %u \n"), place));
        if (found)
        {
            SHR_IF_ERR_EXIT(example.occ2.print(unit));
        }

        if (place != expected_places[indx])
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "found %u instead of %u\n", place, expected_places[indx]);
        }

        SHR_IF_ERR_EXIT(example.occ2.status_set(unit, place, TRUE));
    }

    LOG_CLI((BSL_META_U(unit, "\n\r ********** sw_state_occ_bm_test_3 : END passed **********\n")));

exit:
    /*
     * set back the severity level of the shell and sw state to theirs original values.
     */
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_sw_state);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_APPL_SHELL, original_shell_cmd);

    if (SHR_GET_CURRENT_ERR())
    {
        LOG_CLI((BSL_META("Test FAILED\n")));
    }
    else
    {
        LOG_CLI((BSL_META("Test PASSED\n")));
    }
    ctest_dnxc_restore_soc_properties(unit, dnx_sw_state_example_soc_set_h[unit]);
    SHR_FUNC_EXIT;
}

static shr_error_e
sw_state_occupation_bitmap_test4_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 indx, place, free_place;
    uint8 found;
    bsl_severity_t original_severity_sw_state = BSL_INFO, original_shell_cmd = BSL_INFO;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * save the original severity level of the sw state and shell severity level to bslSeverityFatal..
     */
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_sw_state);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, bslSeverityFatal);

    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_APPL_SHELL, original_shell_cmd);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_APPL_SHELL, bslSeverityFatal);

    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &dnx_sw_state_example_soc_set_h[unit]));

    for (indx = 0; indx < 702; ++indx)
    {
        SHR_IF_ERR_EXIT(example.occ2.get_next(unit, &place, &found));

        LOG_CLI((BSL_META_U(unit, "found %u \n"), place));
        if (found)
        {
            SHR_IF_ERR_EXIT(example.occ2.print(unit));
        }

        if ((indx < 700 && place != indx) || (indx >= 700 && found))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "found %u instead of %u\n", place, indx);
        }

        SHR_IF_ERR_EXIT(example.occ2.status_set(unit, place, TRUE));
    }
    for (indx = 0; indx < 700; ++indx)
    {
        /*
         * free one bit one by one
         */
        free_place = indx;
        SHR_IF_ERR_EXIT(example.occ2.status_set(unit, free_place, FALSE));
        /*
         * get next free place (has to be free_place)
         */
        SHR_IF_ERR_EXIT(example.occ2.get_next(unit, &place, &found));

        LOG_CLI((BSL_META_U(unit, "found %u \n"), place));
        if (found)
        {
            SHR_IF_ERR_EXIT(example.occ2.print(unit));
        }

        if (place != free_place)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "found %u instead of %u\n", place, free_place);
        }

        SHR_IF_ERR_EXIT(example.occ2.status_set(unit, place, TRUE));

    }

    LOG_CLI((BSL_META_U(unit, "\n\r ********** sw_state_occ_bm_test_4 : END passed **********\n")));

exit:
    /*
     * set back the severity level of the shell and sw state to theirs original values.
     */
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_sw_state);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_APPL_SHELL, original_shell_cmd);

    if (SHR_GET_CURRENT_ERR())
    {
        LOG_CLI((BSL_META("Test FAILED\n")));
    }
    else
    {
        LOG_CLI((BSL_META("Test PASSED\n")));
    }
    ctest_dnxc_restore_soc_properties(unit, dnx_sw_state_example_soc_set_h[unit]);
    SHR_FUNC_EXIT;
}

static shr_error_e
sw_state_sorted_ll_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    sw_state_ll_node_t output_node;
    uint8 values[12] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
    uint8 output_key;
    uint8 curr_val, last_val, found;
    uint32 nof_elements;
    int ret_val;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &dnx_sw_state_example_soc_set_h[unit]));

    LOG_CLI((BSL_META("insert some values to the list: 0, 7, 9, 1, 8, 3, 10\n")));
    SHR_IF_ERR_EXIT(example.sorted_ll.add(unit, &values[0], &values[0]));
    SHR_IF_ERR_EXIT(example.sorted_ll.add(unit, &values[7], &values[7]));
    SHR_IF_ERR_EXIT(example.sorted_ll.add(unit, &values[9], &values[9]));
    SHR_IF_ERR_EXIT(example.sorted_ll.add(unit, &values[1], &values[1]));
    SHR_IF_ERR_EXIT(example.sorted_ll.add(unit, &values[8], &values[8]));
    SHR_IF_ERR_EXIT(example.sorted_ll.add(unit, &values[10], &values[10]));
    SHR_IF_ERR_EXIT(example.sorted_ll.add(unit, &values[3], &values[3]));

    LOG_CLI((BSL_META("print the list, expect to see 0->1->3->7->8->9->0A visual only\n")));
    SHR_IF_ERR_EXIT(example.sorted_ll.print(unit));

    LOG_CLI((BSL_META("get the list's tail\n")));
    SHR_IF_ERR_EXIT(example.sorted_ll.get_last(unit, &output_node));

    SHR_IF_ERR_EXIT(example.sorted_ll.nof_elements(unit, &nof_elements));
    if (nof_elements != 7)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - expected nof_elements to be 7 elements, got %d\n", nof_elements);
    }

    LOG_CLI((BSL_META("verify that the list is sorted\n")));

    last_val = 0;
    SHR_IF_ERR_EXIT(example.sorted_ll.get_first(unit, &output_node));
    SHR_IF_ERR_EXIT(example.sorted_ll.next_node(unit, output_node, &output_node));

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

    /*
     * "verify that same key will not be display more than once & value update
     */

    ret_val = example.sorted_ll.add(unit, &values[8], &values[8]);
    if (ret_val == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - expected error for duplicate entries but got success\n");
    }

    SHR_IF_ERR_EXIT(example.sorted_ll.nof_elements(unit, &nof_elements));
    if (nof_elements != 7)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - expected nof_elements to be 7 elements, got %d\n", nof_elements);
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
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - expected error add_after cannot used in sorted linked list\n");
    }

    ret_val = example.sorted_ll.add_before(unit, output_node, &values[3], &values[3]);
    if (ret_val == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - expected error add_before cannot used in sorted linked list\n");
    }

    ret_val = example.sorted_ll.add_first(unit, &values[8], &values[8]);
    if (ret_val == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - expected error add_first cannot used in sorted linked list\n");
    }

    ret_val = example.sorted_ll.add_last(unit, &values[8], &values[8]);
    if (ret_val == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - expected error add_last cannot used in sorted linked list\n");
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

    LOG_CLI((BSL_META("print the list, expect to see 0->1->3->7->8->10  visual only\n")));
    SHR_IF_ERR_EXIT(example.sorted_ll.print(unit));

    SHR_IF_ERR_EXIT(example.sorted_ll.nof_elements(unit, &nof_elements));
    if (nof_elements != 6)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - expected nof_elements to be 6 elements but got %d\n",
                     nof_elements);
    }

    SHR_IF_ERR_EXIT(example.sorted_ll.get_last(unit, &output_node));
    SHR_IF_ERR_EXIT(example.sorted_ll.remove_node(unit, output_node));
    LOG_CLI((BSL_META("print the list, expect to see 0->1->3->7->8 visual only\n")));
    SHR_IF_ERR_EXIT(example.sorted_ll.print(unit));

    SHR_IF_ERR_EXIT(example.sorted_ll.nof_elements(unit, &nof_elements));
    if (nof_elements != 5)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - expected nof_elements to be 6 elements but got %d\n",
                     nof_elements);
    }

    SHR_IF_ERR_EXIT(example.sorted_ll.add(unit, &values[2], &values[2]));
    SHR_IF_ERR_EXIT(example.sorted_ll.add(unit, &values[4], &values[4]));
    SHR_IF_ERR_EXIT(example.sorted_ll.add(unit, &values[5], &values[5]));
    SHR_IF_ERR_EXIT(example.sorted_ll.add(unit, &values[6], &values[6]));
    SHR_IF_ERR_EXIT(example.sorted_ll.add(unit, &values[9], &values[9]));
    SHR_IF_ERR_EXIT(example.sorted_ll.add(unit, &values[10], &values[10]));
    SHR_IF_ERR_EXIT(example.sorted_ll.add(unit, &values[11], &values[11]));

    LOG_CLI((BSL_META("print the list, expect to see 0->1->2->3->4->->5->6->7->8->9->10->11 visual only\n")));
    SHR_IF_ERR_EXIT(example.sorted_ll.print(unit));

    LOG_CLI((BSL_META("destroy the linked list\n")));

    LOG_CLI((BSL_META("Test PASSED\n")));

    SHR_EXIT();

exit:
    if (SHR_GET_CURRENT_ERR())
    {
        LOG_CLI((BSL_META("Test FAILED\n")));
    }
    ctest_dnxc_restore_soc_properties(unit, dnx_sw_state_example_soc_set_h[unit]);
    SHR_FUNC_EXIT;
}

static shr_error_e
sw_state_bt_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    sw_state_bt_node_t output_node;
    sw_state_bt_node_t tmp_node;
    sw_state_bt_node_t tmp_node2;
    int value = 0;
    int value1 = 1;
    int value2 = 2;
    int value3 = 3;
    int value4 = 4;
    int value5 = 5;
    int value6 = 6;
    int value7 = 7;
    int value8 = 8;
    unsigned int nof_elements = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &dnx_sw_state_example_soc_set_h[unit]));

    LOG_CLI((BSL_META("insert 4\n")));
    SHR_IF_ERR_EXIT(example.bt.add_root(unit, &value4));

    LOG_CLI((BSL_META("get the binary tree's root\n")));
    SHR_IF_ERR_EXIT(example.bt.get_root(unit, &output_node));

    LOG_CLI((BSL_META("add the value 2 in root's left\n")));
    SHR_IF_ERR_EXIT(example.bt.add_left_child(unit, output_node, &value2));

    LOG_CLI((BSL_META("add the value 6 in root'ss right\n")));
    SHR_IF_ERR_EXIT(example.bt.add_right_child(unit, output_node, &value6));

    LOG_CLI((BSL_META("get the head's right\n")));
    SHR_IF_ERR_EXIT(example.bt.get_right_child(unit, output_node, &output_node));

    LOG_CLI((BSL_META("verify the data of this node is 6\n")));
    SHR_IF_ERR_EXIT(example.bt.node_value(unit, output_node, &value));
    if (value != value6)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - unexpected value in node\n");
    }

    LOG_CLI((BSL_META("add the value 7 as right child of 6\n")));
    SHR_IF_ERR_EXIT(example.bt.add_right_child(unit, output_node, &value7));

    LOG_CLI((BSL_META("add the value 5 as left child of 6\n")));
    SHR_IF_ERR_EXIT(example.bt.add_left_child(unit, output_node, &value5));

    LOG_CLI((BSL_META("get the binary tree's root\n")));
    SHR_IF_ERR_EXIT(example.bt.get_root(unit, &output_node));
    LOG_CLI((BSL_META("verify the data of root is 4\n")));
    SHR_IF_ERR_EXIT(example.bt.node_value(unit, output_node, &value));
    if (value != value4)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - unexpected value in node\n");
    }

    LOG_CLI((BSL_META("get the roots left\n")));
    SHR_IF_ERR_EXIT(example.bt.get_left_child(unit, output_node, &output_node));
    LOG_CLI((BSL_META("verify the data of left is 2\n")));
    SHR_IF_ERR_EXIT(example.bt.node_value(unit, output_node, &value));
    if (value != value2)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - unexpected value in node\n");
    }

    LOG_CLI((BSL_META("add the value 1 as left child of 2\n")));
    SHR_IF_ERR_EXIT(example.bt.add_left_child(unit, output_node, &value1));

    LOG_CLI((BSL_META("add the value 3 as right child of 2\n")));
    SHR_IF_ERR_EXIT(example.bt.add_right_child(unit, output_node, &value3));

    SHR_IF_ERR_EXIT(example.bt.nof_elements(unit, &nof_elements));
    LOG_CLI((BSL_META("nof_elements:%d\n"), nof_elements));

    LOG_CLI((BSL_META
             ("print the binary tree, expect to see 4->2->1->N->N->3->N->N->6->5->N->N->7->N->N, visual only\n")));
    SHR_IF_ERR_EXIT(example.bt.print(unit));

    LOG_CLI((BSL_META("remove node 7\n")));
    SHR_IF_ERR_EXIT(example.bt.get_root(unit, &output_node));
    SHR_IF_ERR_EXIT(example.bt.get_right_child(unit, output_node, &output_node));
    /*
     * updating value6 with value8
     */
    SHR_IF_ERR_EXIT(example.bt.node_update(unit, output_node, &value8));

    /*
     * remove the left child
     */
    SHR_IF_ERR_EXIT(example.bt.get_left_child(unit, output_node, &tmp_node));
    SHR_IF_ERR_EXIT(example.bt.get_parent(unit, tmp_node, &tmp_node2));
    if (output_node != tmp_node2)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - unexpected parent node\n");
    }

    SHR_IF_ERR_EXIT(example.bt.set_left_child(unit, output_node, SW_STATE_BT_INVALID));
    SHR_IF_ERR_EXIT(example.bt.set_parent(unit, tmp_node, SW_STATE_BT_INVALID));
    SHR_IF_ERR_EXIT(example.bt.node_free(unit, tmp_node));

    /*
     * remove the right child
     */
    SHR_IF_ERR_EXIT(example.bt.get_right_child(unit, output_node, &tmp_node));
    SHR_IF_ERR_EXIT(example.bt.get_parent(unit, tmp_node, &tmp_node2));
    if (output_node != tmp_node2)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - unexpected parent node\n");
    }
    SHR_IF_ERR_EXIT(example.bt.set_right_child(unit, output_node, SW_STATE_BT_INVALID));
    SHR_IF_ERR_EXIT(example.bt.set_parent(unit, tmp_node, SW_STATE_BT_INVALID));
    SHR_IF_ERR_EXIT(example.bt.node_free(unit, tmp_node));

    SHR_IF_ERR_EXIT(example.bt.nof_elements(unit, &nof_elements));
    LOG_CLI((BSL_META("nof_elements:%d\n"), nof_elements));

    LOG_CLI((BSL_META("print the binary tree, expect to see 4->2->1->N->N->3->N->N->8->N->N, visual only\n")));
    SHR_IF_ERR_EXIT(example.bt.print(unit));
    LOG_CLI((BSL_META("Test PASSED\n")));

    SHR_EXIT();

exit:
    if (SHR_GET_CURRENT_ERR())
    {
        LOG_CLI((BSL_META("Test FAILED\n")));
    }
    ctest_dnxc_restore_soc_properties(unit, dnx_sw_state_example_soc_set_h[unit]);
    SHR_FUNC_EXIT;
}

static shr_error_e
sw_state_htb_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
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

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &dnx_sw_state_example_soc_set_h[unit]));

    LOG_CLI((BSL_META("insert one entry prior to state comparison transaction\n")));
    SHR_IF_ERR_EXIT(example.htb.insert(unit, 0, &key5, &value5, &success));
    if (success == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - insertion of (5,50) failed\n");
    }

    if (dnx_state_comparison_start(unit) != _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Snapshot manager, failed to start.\n");
    }

    LOG_CLI((BSL_META("remove the entry prior to the transaction\n")));
    SHR_IF_ERR_EXIT(example.htb.delete(unit, 0, &key5));

    LOG_CLI((BSL_META("Insert (1,10) & (4,40)\n")));
    SHR_IF_ERR_EXIT(example.htb.insert(unit, 0, &key1, &value1, &success));
    if (success == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - insertion of (1,10) failed\n");
    }
    SHR_IF_ERR_EXIT(example.htb.insert(unit, 0, &key4, &value4, &success));
    if (success == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - insertion of (4,40) failed\n");
    }

    LOG_CLI((BSL_META("find key 4 and expect value 40\n")));
    SHR_IF_ERR_EXIT(example.htb.find(unit, 0, &key4, &value, &found));
    if (value != value4)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - unexpected value in key 4\n");
    }

    LOG_CLI((BSL_META("find key 3 and expect not found error")));
    /*
     * rv =
     */ example.htb.find(unit, 0, &key3, &value, &found);
    if (found != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - found to be 0\n");
    }

    LOG_CLI((BSL_META("find key 1 and expect value 10")));
    SHR_IF_ERR_EXIT(example.htb.find(unit, 0, &key1, &value, &found));
    if (value != value1)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - unexpected value in key 1\n");
    }

    LOG_CLI((BSL_META("remove key 1 and expect")));
    SHR_IF_ERR_EXIT(example.htb.delete(unit, 0, &key1));

    LOG_CLI((BSL_META("find key 1 and expect found to be 0 as it was deleted\n")));
    /*
     * rv =
     */ example.htb.find(unit, 0, &key1, &value, &found);
    if (found != 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - expected found to be 0\n");
    }

    LOG_CLI((BSL_META("Insert (3,30) & (2,20)\n")));
    SHR_IF_ERR_EXIT(example.htb.insert(unit, 0, &key2, &value2, &success));
    if (success == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - insertion of (2,20) failed\n");
    }
    SHR_IF_ERR_EXIT(example.htb.insert(unit, 0, &key3, &value3, &success));
    if (success == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - insertion of (3,30) failed\n");
    }

    LOG_CLI((BSL_META("find key 3 and expect value 30\n")));
    SHR_IF_ERR_EXIT(example.htb.find(unit, 0, &key3, &value, &found));
    if (value != value3)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - unexpected value in key 3\n");
    }

    LOG_CLI((BSL_META("insert the entry that existed prior to the transaction\n")));
    SHR_IF_ERR_EXIT(example.htb.insert(unit, 0, &key5, &value5, &success));
    if (success == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - insertion of (5,50) failed\n");
    }

    LOG_CLI((BSL_META("print the entries in htbl, visual only\n")));
    SHR_IF_ERR_EXIT(example.htb.print(unit, 0));

    /*
     * delete the rest of the entries in the hash table
     */
    LOG_CLI((BSL_META("remove key 2 and expect\n")));
    SHR_IF_ERR_EXIT(example.htb.delete(unit, 0, &key2));

    LOG_CLI((BSL_META("remove key 3 and expect\n")));
    SHR_IF_ERR_EXIT(example.htb.delete(unit, 0, &key3));

    LOG_CLI((BSL_META("remove key 4 and expect\n")));
    SHR_IF_ERR_EXIT(example.htb.delete(unit, 0, &key4));

    if (dnx_state_comparison_end_and_compare(unit) != _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Snapshot manager rollback error.\n");
    }

    LOG_CLI((BSL_META("remove the entry prior to the transaction\n")));
    SHR_IF_ERR_EXIT(example.htb.delete(unit, 0, &key5));

    LOG_CLI((BSL_META("Insert (1,10) & (4,40)\n")));
    SHR_IF_ERR_EXIT(example.htb.insert(unit, 0, &key1, &value1, &success));
    if (success == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - insertion of (1,10) failed\n");
    }
    SHR_IF_ERR_EXIT(example.htb.insert(unit, 0, &key4, &value4, &success));
    if (success == 0)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - insertion of (4,40) failed\n");
    }

    SHR_IF_ERR_EXIT(example.htb.delete_all(unit, 0));

    LOG_CLI((BSL_META("Test PASSED\n")));

    SHR_EXIT();

exit:
    if (SHR_GET_CURRENT_ERR())
    {
        LOG_CLI((BSL_META("Test FAILED\n")));
    }
    ctest_dnxc_restore_soc_properties(unit, dnx_sw_state_example_soc_set_h[unit]);
    SHR_FUNC_EXIT;
}

static shr_error_e
sw_state_htb_arr_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
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

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &dnx_sw_state_example_soc_set_h[unit]));

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

    LOG_CLI((BSL_META("remove the entry prior to the transaction\n")));

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

    LOG_CLI((BSL_META("find key 3 and expect not found error\n")));

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

    LOG_CLI((BSL_META("find key 1 and expect value 10\n")));
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

    LOG_CLI((BSL_META("remove key 1\n")));
    SHR_IF_ERR_EXIT(example.htb_arr.delete(unit, 0, &int_keys[0]));
    SHR_IF_ERR_EXIT(example.htb_arr.delete(unit, 1, &char_keys[0]));

    LOG_CLI((BSL_META("find key 1 and expect found to be 0 as it was deleted\n")));
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
    LOG_CLI((BSL_META("remove key 2\n")));
    SHR_IF_ERR_EXIT(example.htb_arr.delete(unit, 0, &int_keys[1]));
    SHR_IF_ERR_EXIT(example.htb_arr.delete(unit, 1, &char_keys[1]));

    LOG_CLI((BSL_META("remove key 3\n")));
    SHR_IF_ERR_EXIT(example.htb_arr.delete(unit, 0, &int_keys[2]));
    SHR_IF_ERR_EXIT(example.htb_arr.delete(unit, 1, &char_keys[2]));

    LOG_CLI((BSL_META("remove key 4\n")));
    SHR_IF_ERR_EXIT(example.htb_arr.delete(unit, 0, &int_keys[3]));
    SHR_IF_ERR_EXIT(example.htb_arr.delete(unit, 1, &char_keys[3]));

    if (dnx_state_comparison_end_and_compare(unit) != _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Snapshot manager rollback error.\n");
    }

    LOG_CLI((BSL_META("remove the entry prior to the transaction\n")));
    SHR_IF_ERR_EXIT(example.htb_arr.delete(unit, 0, &int_keys[4]));
    SHR_IF_ERR_EXIT(example.htb_arr.delete(unit, 1, &char_keys[4]));

    LOG_CLI((BSL_META("Test PASSED\n")));

    SHR_EXIT();

exit:
    if (SHR_GET_CURRENT_ERR())
    {
        LOG_CLI((BSL_META("Test FAILED\n")));
    }
    ctest_dnxc_restore_soc_properties(unit, dnx_sw_state_example_soc_set_h[unit]);
    SHR_FUNC_EXIT;
}

static shr_error_e
sw_state_enum_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META("Retrieving string for DNX_EXAMPLE_ENUM_NEGATIVE value and validating result.\n")));
    if (sal_strncmp(dnx_example_name_e_get_name(DNX_EXAMPLE_ENUM_NEGATIVE), "DNX_EXAMPLE_ENUM_NEGATIVE",
                    sal_strnlen("DNX_EXAMPLE_ENUM_NEGATIVE", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH)))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - unexpected enum name retrieved for DNX_EXAMPLE_ENUM_NEGATIVE\n");
    }

    LOG_CLI((BSL_META("Retrieving string for DNX_EXAMPLE_ENUM_ZERO value and validating result.\n")));
    if (sal_strncmp(dnx_example_name_e_get_name(DNX_EXAMPLE_ENUM_ZERO), "DNX_EXAMPLE_ENUM_ZERO",
                    sal_strnlen("DNX_EXAMPLE_ENUM_ZERO", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH)))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - unexpected enum name retrieved for DNX_EXAMPLE_ENUM_ZERO\n");
    }

    LOG_CLI((BSL_META("Retrieving string for DNX_EXAMPLE_ENUM_ONE value and validating result.\n")));
    if (sal_strncmp(dnx_example_name_e_get_name(DNX_EXAMPLE_ENUM_ONE), "DNX_EXAMPLE_ENUM_ONE",
                    sal_strnlen("DNX_EXAMPLE_ENUM_ONE", SHR_SAND_DEFAULT_MAX_STRING_OPERATION_LENGTH)))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - unexpected enum name retrieved for DNX_EXAMPLE_ENUM_ONE\n");
    }

    LOG_CLI((BSL_META("Test PASSED\n")));

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sw_state_callback_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dnx_sw_state_callback_test_function_cb test_cb;
    char function_name[SW_STATE_CB_DB_NAME_STR_SIZE];

    SHR_FUNC_INIT_VARS(unit);

    sal_strncpy(function_name, "dnx_sw_state_callback_test_first_function", SW_STATE_CB_DB_NAME_STR_SIZE - 1);

    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &dnx_sw_state_example_soc_set_h[unit]));

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
    ctest_dnxc_restore_soc_properties(unit, dnx_sw_state_example_soc_set_h[unit]);
    SHR_FUNC_EXIT;
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
    int def_cunk_index_part2 = 1;
    int def_cunk_index_part3 = 2;
    int def_cunk_index_part4 = 3;
    int def_cunk_index_part5 = 4;
    int def_cunk_index_part6 = 5;
    int i;
    int rv;
    uint32 dummy_piece_offset;
    uint32 offset_to_reserve;
    uint32 piece_size_arr_1[6] = { 1, 2, 4, 8, 16, 32 };
    uint32 piece_offset_arr_1[6];
    uint32 piece_size_arr_2[6] = { 32, 16, 8, 4, 2, 1 };
    uint32 piece_offset_arr_2[6];
    uint32 piece_size_arr_3[4] = { 1, 2, 2, 4 };
    uint32 piece_offset_arr_3[4];
    uint32 piece_size_arr_4[22] = { 1, 2, 2, 1, 4, 2, 2, 4, 1, 1, 4, 8, 2, 2, 1, 2, 8, 8, 2, 1, 4, 2 };
    uint32 piece_offset_arr_4[22];
    bsl_severity_t original_severity_def_chunk, original_severity_sw_state;

    SHR_FUNC_INIT_VARS(unit);

    LOG_CLI((BSL_META
             ("\n If you want the chunks to be printed please run ' debug SHAREDSWDNX DefragChunkDnx warn ', before the test.\n")));

    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &dnx_sw_state_example_soc_set_h[unit]));

    /** save the original severity level of the defragmented chunk and sw state. */
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SHAREDSWDNX_DEFRAGCHUNKDNX, original_severity_def_chunk);
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_sw_state);

    /** set the severity level of the defragmented chunk and sw state to bslSeverityFatal. */
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SHAREDSWDNX_DEFRAGCHUNKDNX, bslSeverityFatal);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, bslSeverityFatal);

    /** Part 2:
     *  create chunks and add members of different sizes, check
     *  valid sizes are passing and illeagel sizes are failing.
     *  print the chunk after filling with members */

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
     *  add member and make sure that there's place now. */
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

    /** add members of sizes 1,2,2,1,4,2,2,4,1,1,4,8,2,2,1,2,8,8,2,1,4,2 expecet success */
    LOG_CLI((BSL_META("Add members of sizes 1,2,2,1,4,2,2,4,1,1,4,8,2,2,1,2,8,8,2,1,4,2 expect success.\n")));
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

    /**
     * Part 6:
     * reserve with invalid inputs (fail)
     * unreserve with invalid inputs (fail)
     * defrag with invalid inputs (fail)
     * reserve free slots (pass)
     * reserve reserved slots (pass)
     * unreserve reserved slots (pass)
     * allocate to force fragmentation (pass)
     * reserve allocated space  (fail)
     * unreserve allocated space (fail)
     * defrag to clear space (pass)
     * reserve defragmented slot (pass)
     * allocate when reserve occupy available slots (fail)
     */

    /** reserve with invalid inputs */
    LOG_CLI((BSL_META("reserve with invalid inputs - expect failure\n")));
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, bslSeverityFatal);
    rv = example.defragmented_chunk_example.offset_reserve(unit, def_cunk_index_part6, 3, 4);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_sw_state);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "expected this to fail, but it passed, something is wrong\n");
    }
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, bslSeverityFatal);
    rv = example.defragmented_chunk_example.offset_reserve(unit, def_cunk_index_part6, 4, 2);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_sw_state);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "expected this to fail, but it passed, something is wrong\n");
    }
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, bslSeverityFatal);
    rv = example.defragmented_chunk_example.offset_reserve(unit, def_cunk_index_part6, 32, 0);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_sw_state);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "expected this to fail, but it passed, something is wrong\n");
    }

    /** unreserve with invalid inputs */
    LOG_CLI((BSL_META("unreserve with invalid inputs - expect failure\n")));
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, bslSeverityFatal);
    rv = example.defragmented_chunk_example.offset_unreserve(unit, def_cunk_index_part6, 3, 4);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_sw_state);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "expected this to fail, but it passed, something is wrong\n");
    }
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, bslSeverityFatal);
    rv = example.defragmented_chunk_example.offset_unreserve(unit, def_cunk_index_part6, 4, 2);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_sw_state);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "expected this to fail, but it passed, something is wrong\n");
    }
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, bslSeverityFatal);
    rv = example.defragmented_chunk_example.offset_unreserve(unit, def_cunk_index_part6, 32, 0);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_sw_state);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "expected this to fail, but it passed, something is wrong\n");
    }

    /** defrag with invalid inputs */
    LOG_CLI((BSL_META("defrag with invalid inputs - expect failure\n")));
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, bslSeverityFatal);
    rv = example.defragmented_chunk_example.defrag(unit, def_cunk_index_part6, 32, NULL, &dummy_piece_offset);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_sw_state);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "expected this to fail, but it passed, something is wrong\n");
    }
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, bslSeverityFatal);
    rv = example.defragmented_chunk_example.defrag(unit, def_cunk_index_part6, 3, NULL, &dummy_piece_offset);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_sw_state);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "expected this to fail, but it passed, something is wrong\n");
    }

    /** reserve free slots */
    LOG_CLI((BSL_META("reserve free slots - expect success\n")));
    SHR_IF_ERR_EXIT(example.defragmented_chunk_example.offset_reserve(unit, def_cunk_index_part6, 8, 8));
    /** reserve reserved slots */
    LOG_CLI((BSL_META("reserve reserved slots - expect success\n")));
    SHR_IF_ERR_EXIT(example.defragmented_chunk_example.offset_reserve(unit, def_cunk_index_part6, 8, 8));
    /** unreserve reserved slots */
    LOG_CLI((BSL_META("unreserve reserved slots - expect success\n")));
    SHR_IF_ERR_EXIT(example.defragmented_chunk_example.offset_unreserve(unit, def_cunk_index_part6, 8, 8));

    /** allocate to force fragmentation -
     * this method forces fragmentation because allocation is done only in allocation size offset
     * meaning: [0,-1,1,1,1,1,-1,-1,2,2,2,2,-1,-1,-1,-1,-1] after allocation is done where -1 is
     * a clear spot and other values are the sizes exponent value of 2^exponent */
    /** add members of sizes 1,2,2,4 expect success */
    LOG_CLI((BSL_META("Add members of sizes 1,2,2,4 expect success.\n")));
    for (i = 0; i < 4; ++i)
    {
        SHR_IF_ERR_EXIT(example.
                        defragmented_chunk_example.piece_alloc(unit, def_cunk_index_part6, piece_size_arr_3[i], NULL,
                                                               &piece_offset_arr_3[i]));
    }
    /** print chunk */
    if (original_severity_def_chunk <= bslSeverityWarn)
    {
        LOG_CLI((BSL_META("Print chunk.\n")));
        SHR_IF_ERR_EXIT(example.defragmented_chunk_example.print(unit, def_cunk_index_part6));
    }

    /** reserve allocated space */
    /** unreserve allocated space */
    LOG_CLI((BSL_META("reserve allocated space - expect failure\n")));
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, bslSeverityFatal);
    rv = example.defragmented_chunk_example.offset_reserve(unit, def_cunk_index_part6, 4, 0);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_sw_state);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "expected this to fail, but it passed, something is wrong\n");
    }
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, bslSeverityFatal);
    LOG_CLI((BSL_META("unreserve allocated space - expect failure\n")));
    rv = example.defragmented_chunk_example.offset_unreserve(unit, def_cunk_index_part6, 4, 0);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_sw_state);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "expected this to fail, but it passed, something is wrong\n");
    }

    /** defrag to clear space */
    LOG_CLI((BSL_META("defrag to clear space - expect success\n")));
    SHR_IF_ERR_EXIT(example.defragmented_chunk_example.defrag(unit, def_cunk_index_part6, 8, NULL, &offset_to_reserve));
    /** print chunk */
    if (original_severity_def_chunk <= bslSeverityWarn)
    {
        LOG_CLI((BSL_META("Print chunk.\n")));
        SHR_IF_ERR_EXIT(example.defragmented_chunk_example.print(unit, def_cunk_index_part6));
    }

    /** reserve defragmented slot */
    LOG_CLI((BSL_META("reserve defragmented slot - expect success\n")));
    SHR_IF_ERR_EXIT(example.
                    defragmented_chunk_example.offset_reserve(unit, def_cunk_index_part6, 8, offset_to_reserve));
    /** print chunk */
    if (original_severity_def_chunk <= bslSeverityWarn)
    {
        LOG_CLI((BSL_META("Print chunk.\n")));
        SHR_IF_ERR_EXIT(example.defragmented_chunk_example.print(unit, def_cunk_index_part6));
    }

    /** allocate when reserve occupy available slots */
    LOG_CLI((BSL_META("allocate when reserve occupy available slots - expect failure\n")));
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, bslSeverityFatal);
    rv = example.defragmented_chunk_example.piece_alloc(unit, def_cunk_index_part6, 8, NULL, &dummy_piece_offset);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_sw_state);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "expected this to fail, but it passed, something is wrong\n");
    }

    /** unreserve space */
    LOG_CLI((BSL_META("unreserve space - expect success\n")));
    SHR_IF_ERR_EXIT(example.
                    defragmented_chunk_example.offset_unreserve(unit, def_cunk_index_part6, 8, offset_to_reserve));
    /** print chunk */
    if (original_severity_def_chunk <= bslSeverityWarn)
    {
        LOG_CLI((BSL_META("Print chunk.\n")));
        SHR_IF_ERR_EXIT(example.defragmented_chunk_example.print(unit, def_cunk_index_part6));
    }

    /** allocate after unreserve */
    LOG_CLI((BSL_META("allocate after unreserve - expect success\n")));
    SHR_IF_ERR_EXIT(example.
                    defragmented_chunk_example.piece_alloc(unit, def_cunk_index_part6, 8, NULL, &dummy_piece_offset));
    /** print chunk */
    if (original_severity_def_chunk <= bslSeverityWarn)
    {
        LOG_CLI((BSL_META("Print chunk.\n")));
        SHR_IF_ERR_EXIT(example.defragmented_chunk_example.print(unit, def_cunk_index_part6));
    }

    /** free allocated piece */
    LOG_CLI((BSL_META("free allocated piece - expect success\n")));
    SHR_IF_ERR_EXIT(example.defragmented_chunk_example.piece_free(unit, def_cunk_index_part6, dummy_piece_offset));
    /** reserve slots */
    LOG_CLI((BSL_META("reserve slots - expect success\n")));
    SHR_IF_ERR_EXIT(example.defragmented_chunk_example.offset_reserve(unit, def_cunk_index_part6, 1, 8));
    SHR_IF_ERR_EXIT(example.defragmented_chunk_example.offset_reserve(unit, def_cunk_index_part6, 1, 12));
    if (original_severity_def_chunk <= bslSeverityWarn)
    {
        LOG_CLI((BSL_META("Print chunk.\n")));
        SHR_IF_ERR_EXIT(example.defragmented_chunk_example.print(unit, def_cunk_index_part6));
    }

    /** defrag when we have reserved space - make sure it doesn't move it */
    LOG_CLI((BSL_META("defrag when we have reserved space - make sure it doesn't move it - expect failure\n")));
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, bslSeverityFatal);
    rv = example.defragmented_chunk_example.defrag(unit, def_cunk_index_part6, 4, NULL, &offset_to_reserve);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_sw_state);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "expected this to fail, but it passed, something is wrong\n");
    }
    if (original_severity_def_chunk <= bslSeverityWarn)
    {
        LOG_CLI((BSL_META("Print chunk.\n")));
        SHR_IF_ERR_EXIT(example.defragmented_chunk_example.print(unit, def_cunk_index_part6));
    }

    /** alloc when we have reserved space - make sure it doesn't move it */
    LOG_CLI((BSL_META("alloc when we have reserved space - make sure it doesn't move it - expect failure\n")));
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, bslSeverityFatal);
    rv = example.defragmented_chunk_example.piece_alloc(unit, def_cunk_index_part6, 4, NULL, &dummy_piece_offset);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_SWSTATEDNX_GENERAL, original_severity_sw_state);
    if (rv == _SHR_E_NONE)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "expected this to fail, but it passed, something is wrong\n");
    }
    if (original_severity_def_chunk <= bslSeverityWarn)
    {
        LOG_CLI((BSL_META("Print chunk.\n")));
        SHR_IF_ERR_EXIT(example.defragmented_chunk_example.print(unit, def_cunk_index_part6));
    }
    LOG_CLI((BSL_META("PART 6 - DONE.\n\n")));

    /** set back the severity level of the defragmented chunk and sw state to theirs original values. */
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
    ctest_dnxc_restore_soc_properties(unit, dnx_sw_state_example_soc_set_h[unit]);
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
     * debug sw state and allow alloc after init
     */
    sh_process_command(unit, "debug swstatednx +");
    SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_during_test_set(unit, 1));

    /*
     * INIT example module. Should NOT produce an alloc entry in the journal
     */
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
     * Validate mutex journaling
     */
    LOG_CLI((BSL_META("Create new mutex.\n")));
    SHR_IF_ERR_EXIT(example.mutex_test.create(unit));

    /*
     * Sanity check that the journal is off
     */
    LOG_CLI((BSL_META("Validate that journaling is off at the beginning of the test\n")));
    if (dnx_err_recovery_is_on_test(unit))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    /*
     * Start journaling
     */
    LOG_CLI((BSL_META("Start journaling.\n")));
    SHR_IF_ERR_EXIT(dnx_rollback_journal_start(unit));

    LOG_CLI((BSL_META("Validate that journaling has been started.\n")));
    if (!dnx_err_recovery_is_on_test(unit))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }

    /*
     * Should produce a memcpy entry in journal
     */
    LOG_CLI((BSL_META("Change default value for init\n")));
    SHR_IF_ERR_EXIT(example.default_value_tree.default_value_l1.my_variable.set(unit, 10));

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
    dnx_data_size = dnx_data_module_testing.example_tests.field_size_get(unit);
    for (idx = 1; idx < dnx_data_size; idx++)
    {
        SHR_IF_ERR_EXIT(example.default_value_tree.array_dnxdata.default_value_l2.my_variable.set(unit, idx, 10));
    }

    /*
     * Free memory for DNXdata array. Should produce a free entry in the journal
     */
    LOG_CLI((BSL_META("Free memory for DNXdata array\n")));
    SHR_IF_ERR_EXIT(example.default_value_tree.array_dnxdata.free(unit));

    
    LOG_CLI((BSL_META("Take the mutex.\n")));
    SHR_IF_ERR_EXIT(example.mutex_test.take(unit, 0));

    LOG_CLI((BSL_META("Take the mutex again, recursive mutex (should not block thread).\n")));
    SHR_IF_ERR_EXIT(example.mutex_test.take(unit, 0));

    LOG_CLI((BSL_META("Give the mutex.\n")));
    SHR_IF_ERR_EXIT(example.mutex_test.give(unit));

    LOG_CLI((BSL_META("Give the mutex.\n")));
    SHR_IF_ERR_EXIT(example.mutex_test.give(unit));

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
    if (dnx_err_recovery_is_on_test(unit))
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

    LOG_CLI((BSL_META("Destroy the mutex.\n")));
    SHR_IF_ERR_EXIT(example.mutex_test.destroy(unit));

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
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(test_string, 0, 100);
    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &dnx_sw_state_example_soc_set_h[unit]));

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
    ctest_dnxc_restore_soc_properties(unit, dnx_sw_state_example_soc_set_h[unit]);
    SHR_FUNC_EXIT;
}

static shr_error_e
sw_state_module_size_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    long unsigned int total_module_size = 0;
    uint32 in_use = 0;

#ifdef BCM_WARM_BOOT_SUPPORT
    uint32 left = 0;
    int rv = _SHR_E_NONE;
#endif /* BCM_WARM_BOOT_SUPPORT */

    SHR_FUNC_INIT_VARS(unit);

#ifdef BCM_WARM_BOOT_SUPPORT
    rv = dnxc_sw_state_wb_sizes_get(unit, &in_use, &left);
    if (rv == _SHR_E_UNAVAIL)
    {
        LOG_CLI((BSL_META("Could not get the total memory consumption of Warmboot enabled modules.\n")));
        SHR_FUNC_EXIT;
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    for (int module_id = 1; module_id < NOF_MODULE_ID; module_id++)
    {
        if (dnxc_sw_state_dispatcher[unit][module_id].module_size_get != NULL)
        {
            total_module_size += dnxc_sw_state_dispatcher[unit][module_id].module_size_get(unit, module_id);
        }
    }

    if (in_use != total_module_size)
    {
        long unsigned int diff = 0;
        if (in_use > total_module_size)
        {
            diff = in_use - total_module_size;
        }
        else
        {
            diff = total_module_size - in_use;
        }
        LOG_CLI((BSL_META
                 ("There is difference (%lu) between total memory consumption of warmboot enabled modules (%d) and the sum (%lu) of all individual modules in SW State. It may indicate a problem in the sw state memory measurement logic.\n"),
                 diff, in_use, total_module_size));
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
    }
    else
    {
        LOG_CLI((BSL_META("Passed test.\n")));
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

static sh_sand_man_t sw_state_int_test_man = {
    .brief = "Test a simple int varable"
};

static sh_sand_man_t sw_state_mutex_test_man = {
    .brief = "Test mutex options"
};

static sh_sand_man_t sw_state_sem_test_man = {
    .brief = "Test sem options"
};

static sh_sand_man_t sw_state_array_range_test_man = {
    .brief = "SW State range array commands test"
};
static sh_sand_man_t sw_state_DNXData_test_man = {
    .brief = "Test DNXData sw state commands."
};

static sh_sand_man_t sw_state_input_value_test_man = {
    .brief = "SW State intput value verification test.",
    .full = "The test set a valid and invalid value to a variable and expect only the valid value to be set."
};

static sh_sand_man_t sw_state_counter_test_man = {
    .brief = "Test counter options",
    .full =
        "Try to assign value to see how it will be accepted. Increment the input value check if it is incremented correctly, decrement it and check the value again."
};

static sh_sand_man_t sw_state_alloc_after_init_test_man = {
    .brief = "Test allocation after init exception ",
    .full = "Try to allocate memory for variable with and without alloc after init exception."
};

static sh_sand_man_t sw_state_arrays_test_man = {
    .brief = "Test the usage of the arrays in sw state."
};

static sh_sand_man_t sw_state_bitmap_test_man = {
    .brief = "Test bitmap variable usage",
    .full = "Test the entire set of functionality of the bitmap variable."
};

static sh_sand_man_t sw_state_bitmap_access_test_man = {
    .brief = "Test bitmap variable ACCESS_PTR get interface",
    .full = "Test access pointer label set/get function of the bitmap variable."
};

static sh_sand_man_t sw_state_pbmp_test_man = {
    .brief = "Test port bitmap variable usage",
    .full = "Test the entire set of functionality of the port bitmap variable.",
    .synopsis = "ctest swstate pbmp"
};

static sh_sand_man_t sw_state_buffer_test_man = {
    .brief = "Test buffer variable usage",
    .full = "Test the entire set of functionality of the buffer variable."
};

static sh_sand_man_t sw_state_default_value_test_man = {
    .brief = "Test default value for variables",
    .full = "Verify default_value attribute validity set for basic parameter types, bitmaps and buffers."
};

static sh_sand_man_t sw_state_ll_test_man = {
    .brief = "Test linked list variable usage",
    .full = "Test the entire set of functionality of the linked list DS variable."
};

static sh_sand_man_t sw_state_ll_max_nodes_test_man = {
    .brief = "Test linked list variable usage",
    .full = "Test the entire set of functionality of the linked list DS variable for the maximum amount of nodes."
};

static sh_sand_man_t sw_state_multihead_ll_test_man = {
    .brief = "Test multihead linked list variable usage",
    .full = "Test the entire set of functionality of the multihead linked list DS variable."
};

static sh_sand_man_t sw_state_sorted_ll_test_man = {
    .brief = "Test sorted linked list variable usage",
    .full = "Test the entire set of functionality of the sorted linked list DS variable."
};

static sh_sand_man_t sw_state_bt_test_man = {
    .brief = "Test binary tree variable usage",
    .full = "Test the entire set of functionality of the binary tree DS variable."
};

static sh_sand_man_t sw_state_htb_test_man = {
    .brief = "Test hash table variable usage",
    .full = "Test the entire set of functionality of the hash table DS variable."
};

static sh_sand_man_t sw_state_htb_arr_test_man = {
    .brief = "Test hash table array variable usage",
    .full = "Test array of hash tables with different key/value sizes."
};

static sh_sand_man_t sw_state_occ_bitmap_test_man = {
    .brief = "Test occ bitmap variable usage",
    .full = "Test the entire set of functionality of the occupation bitmap DS variable."
};

static sh_sand_man_t sw_state_enum_test_man = {
    .brief = "Test enum get_name function",
    .full = "Test enum get_name function for different enum values."
};

static sh_sand_man_t sw_state_callback_test_man = {
    .brief = "Test callback's functions",
    .full = "Test _register_cb, _unregister_cb and _get_cb functions."
};

static sh_sand_man_t sw_state_defragmented_chunk_test_man = {
    .brief = "Test drefragmented chunks variable usage",
    .full = "Test the entire set of functionality of the drefragmented chunks DS variable.",
    .synopsis = "ctest swstate defragmentedchunk"
};

static sh_sand_man_t sw_state_journal_test_man = {
    .brief = "Swstate journal test",
    .full = "Test the swstate rollback journal"
};

static sh_sand_man_t sw_state_string_test_man = {
    .brief = "Test string variable usage",
    .full = "Test the entire set of functionality of the sw state string variable."
};
static sh_sand_man_t sw_state_module_size_test_man = {
    .brief = "Test for total module size",
    .full =
        "Test checking total memory consumption of warmboot enabled modules and the sum of all individual modules in SW State."
};

sh_sand_cmd_t dnx_swstate_test_cmds[] = {
    {"integer", sw_state_int_test_cmd, NULL, NULL, &sw_state_int_test_man, NULL, NULL,
     CTEST_POSTCOMMIT},
    {"counter", sw_state_counter_test_cmd, NULL, NULL, &sw_state_counter_test_man, NULL, NULL,
     CTEST_POSTCOMMIT},
    {"mutex", sw_state_mutex_test_cmd, NULL, NULL, &sw_state_mutex_test_man, NULL, NULL,
     CTEST_POSTCOMMIT},
    {"semaphore", sw_state_sem_test_cmd, NULL, NULL, &sw_state_sem_test_man, NULL, NULL,
     CTEST_POSTCOMMIT},
    {"data", sw_state_DNXData_test_cmd, NULL, NULL, &sw_state_DNXData_test_man, NULL, NULL,
     CTEST_POSTCOMMIT},
    {"value", sw_state_input_value_test_cmd, NULL, NULL, &sw_state_input_value_test_man, NULL, NULL,
     CTEST_POSTCOMMIT},
    {"range", sw_state_array_range_test_cmd, NULL, NULL, &sw_state_array_range_test_man, NULL, NULL,
     CTEST_POSTCOMMIT},
    {"allocate", sw_state_alloc_test_cmd, NULL, NULL, &sw_state_alloc_after_init_test_man, NULL, NULL,
     CTEST_POSTCOMMIT},
    {"array", sw_state_arrays_test_cmd, NULL, NULL, &sw_state_arrays_test_man, NULL, NULL,
     CTEST_POSTCOMMIT},
    {"bitmap", sw_state_bitmap_test_cmd, NULL, NULL, &sw_state_bitmap_test_man, NULL, NULL,
     CTEST_POSTCOMMIT},
    {"bitmap_access", sw_state_bitmap_access_test_cmd, NULL, NULL, &sw_state_bitmap_access_test_man, NULL, NULL,
     CTEST_POSTCOMMIT},
    {"portbitmap", sw_state_pbmp_test_cmd, NULL, NULL, &sw_state_pbmp_test_man, NULL, NULL,
     CTEST_POSTCOMMIT},
    {"buffer", sw_state_buffer_test_cmd, NULL, NULL, &sw_state_buffer_test_man, NULL, NULL,
     CTEST_POSTCOMMIT},
    {"default", sw_state_default_value_test_cmd, NULL, NULL, &sw_state_default_value_test_man, NULL, NULL,
     CTEST_POSTCOMMIT},
    {"linkedlist", sw_state_ll_test_cmd, NULL, NULL, &sw_state_ll_test_man, NULL, NULL,
     CTEST_POSTCOMMIT},
    {"linkedlist_max_nodes", sw_state_ll_max_nodes_test_cmd, NULL, NULL, &sw_state_ll_max_nodes_test_man, NULL, NULL,
     CTEST_POSTCOMMIT},
    {"multiheadlinkedlist", sw_state_multihead_ll_test_cmd, NULL, NULL, &sw_state_multihead_ll_test_man, NULL, NULL,
     CTEST_POSTCOMMIT},
    {"sorted_ll", sw_state_sorted_ll_test_cmd, NULL, NULL, &sw_state_sorted_ll_test_man, NULL, NULL,
     CTEST_POSTCOMMIT},
    {"binarytree", sw_state_bt_test_cmd, NULL, NULL, &sw_state_bt_test_man, NULL, NULL,
     CTEST_POSTCOMMIT},
    {"hashtable", sw_state_htb_test_cmd, NULL, NULL, &sw_state_htb_test_man, NULL, NULL,
     CTEST_POSTCOMMIT},
    {"hashtable_arr", sw_state_htb_arr_test_cmd, NULL, NULL, &sw_state_htb_arr_test_man, NULL, NULL,
     CTEST_POSTCOMMIT},
    {"occ_bitmap1", sw_state_occupation_bitmap_test1_cmd, NULL, NULL, &sw_state_occ_bitmap_test_man, NULL, NULL,
     CTEST_POSTCOMMIT},
    {"occ_bitmap2", sw_state_occupation_bitmap_test2_cmd, NULL, NULL, &sw_state_occ_bitmap_test_man, NULL, NULL,
     CTEST_POSTCOMMIT},
    {"occ_bitmap3", sw_state_occupation_bitmap_test3_cmd, NULL, NULL, &sw_state_occ_bitmap_test_man, NULL, NULL,
     CTEST_POSTCOMMIT},
    {"occ_bitmap4", sw_state_occupation_bitmap_test4_cmd, NULL, NULL, &sw_state_occ_bitmap_test_man, NULL, NULL,
     CTEST_POSTCOMMIT},
    {"enum", sw_state_enum_test_cmd, NULL, NULL, &sw_state_enum_test_man, NULL, NULL,
     CTEST_POSTCOMMIT},
    {"callback", sw_state_callback_test_cmd, NULL, NULL, &sw_state_callback_test_man, NULL, NULL,
     CTEST_POSTCOMMIT},
    {"defragmentedchunk", sw_state_defragmented_chunk_test_cmd, NULL, NULL, &sw_state_defragmented_chunk_test_man, NULL,
     NULL, CTEST_POSTCOMMIT},
    {"JouRNaL", sw_state_journal_test_cmd, NULL, NULL, &sw_state_journal_test_man, NULL, NULL,
     CTEST_POSTCOMMIT},
    {"STRing", sw_state_string_test_cmd, NULL, NULL, &sw_state_string_test_man, NULL, NULL,
     CTEST_POSTCOMMIT},
    {"module_size", sw_state_module_size_test_cmd, NULL, NULL, &sw_state_module_size_test_man, NULL, NULL,
     CTEST_POSTCOMMIT},
    {NULL}
};
