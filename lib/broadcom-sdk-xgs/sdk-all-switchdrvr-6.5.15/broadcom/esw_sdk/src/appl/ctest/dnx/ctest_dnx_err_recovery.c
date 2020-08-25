/*
 * ! \file ctest_dnx_err_recovery.c 
 * Purpose: shell registers commands for error recovery tests 
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#include <shared/bsl.h>
#include <shared/shrextend/shrextend_error.h>

#include <shared/pbmp.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dnx_state_snapshot_manager.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
#include <soc/dnx/swstate/auto_generated/access/example_access.h>

#include <include/bcm_int/dnx/field/field_entry.h>

#include <soc/dnx/dbal/dbal.h>

#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/ctest/dnx/ctest_dnx_err_recovery.h>
#include <soc/dnxc/dnxc_generic_state_journal.h>

#include <bcm/l3.h>
#include <bcm/oam.h>

#include "dbal/ctest_dnx_dbal.h"

#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

/**
 * \brief - get access to the error recovery manager in order to test internals
 */
extern err_recovery_manager_t dnx_err_recovery_manager[SOC_MAX_NUM_DEVICES];

/**
 * \brief - get access to the snapshot manager in order to test internals
 */
extern dnx_state_snapshot_manager_t dnx_state_snapshot_manager[SOC_MAX_NUM_DEVICES];

/**
 * \brief - sanity check the error recovery manager internal flags
 */
static uint8
err_recovery_basic_test_flags_sanity(
    int unit)
{
    return ((0 == dnx_err_recovery_manager[unit].is_suppressed_counter)
            && (!dnx_err_recovery_manager[unit].is_invalidated)
#ifdef DNX_ERR_RECOVERY_VALIDATION
            && (dnx_err_recovery_manager[unit].api_counter == 0)
#endif
            && (dnx_err_recovery_manager[unit].transaction_counter == 0));
}


/**
 * \brief - test the custom state journal
 */
static shr_error_e
err_recovery_custom_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int var;
    int initial_value;

    SHR_FUNC_INIT_VARS(unit);

    var = initial_value = 5;

    /*
     * don't fail the test if error recovery is disabled at compile-time
     */
    if (DNX_ERR_RECOVERY_IS_DISABLED)
    {
        LOG_CLI((BSL_META("Error recovery is not available\n")));
        SHR_EXIT();
    }

    LOG_CLI((BSL_META("1. Attempting to rollback the state of local variable using memcpy interface\n")));
    {
        if (_SHR_E_NONE != dnx_rollback_journal_start(unit))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Could not start a new Error Recovery transaction.\n");
        }

        SHR_IF_ERR_EXIT(dnxc_generic_state_journal_log_memcpy(unit, sizeof(var), (uint8 *) (&var)));

        var++;

        if (_SHR_E_NONE != dnx_rollback_journal_end(unit, TRUE))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Could not roll-back Error Recovery transaction.\n");
        }

        if (var != initial_value)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Was not able to successfully rollback test variable.\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Test dbal cornercase scenarios
 */
static shr_error_e
err_recovery_dbal_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    dbal_tables_e table_id = DBAL_TABLE_IN_AC_TCAM_DB;

    int ii, jj;
    int access_id_counter = 0;
    uint32 entry_handle_id;
    CONST dbal_logical_table_t *table;
    uint32 access_ids[4] = { 0 };
    uint32 field_size;
    uint32 full_mask;

    uint32 result_before_update = 1;
    uint32 result_after_update = 0;
    uint32 key_before_update = 1;
    uint32 key_after_update = 0;
    uint32 additional_key_value = 0;
    uint32 additional_result_value = 0;
    int core = SOC_CORE_ALL;

    int counter = -1;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * don't fail the test if error recovery is disabled at compile-time
     */
    if (DNX_ERR_RECOVERY_IS_DISABLED)
    {
        LOG_CLI((BSL_META("Error recovery is not available\n")));
        SHR_EXIT();
    }

    /*
     * feature should be initialized at that point
     */
    if (!dnx_err_recovery_manager[unit].is_initialized)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Error recovery feature is not initialized\n");
    }

    SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

    if (table->access_method != DBAL_ACCESS_METHOD_MDB && table->table_type != DBAL_TABLE_TYPE_TCAM)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "this test supported only for MDB indirect TCAM tests\n");
    }

    /**
     * Clear table before the test
     */
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));

    /**
     * Commit single tcam entry before error recovery transaction
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, table_id, &entry_handle_id));

    for (ii = 0; ii < table->nof_key_fields; ii++)
    {
        dbal_fields_e field_id = table->keys_info[ii].field_id;
        SHR_IF_ERR_EXIT(dbal_fields_max_size_get(unit, field_id, &field_size));
        full_mask = (field_size == SAL_UINT32_NOF_BITS) ? (-1) : (SAL_UPTO_BIT(field_size));
        dbal_entry_key_field32_masked_set(unit, entry_handle_id, field_id, key_before_update, full_mask);
    }

    for (ii = 0; ii < table->multi_res_info[0].nof_result_fields; ii++)
    {
        dbal_fields_e field_id = table->multi_res_info[0].results_info[ii].field_id;
        dbal_entry_value_field32_set(unit, entry_handle_id, field_id, INST_SINGLE, result_before_update);
    }

    SHR_IF_ERR_EXIT(dnx_field_entry_access_id_create(unit, core, table_id, 7, &access_ids[0]));
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, access_ids[0]));
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /**
     * Start rollback and comparison journals
     */
    SHR_IF_ERR_EXIT(dnx_state_comparison_start(unit));
    SHR_IF_ERR_EXIT(dnx_rollback_journal_start(unit));

    /**
     * Overwrite the entry key and value inserted prior to the transaction
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, access_ids[0]));

    for (ii = 0; ii < table->nof_key_fields; ii++)
    {
        dbal_fields_e field_id = table->keys_info[ii].field_id;
        dbal_entry_key_field32_set(unit, entry_handle_id, field_id, key_after_update);
    }

    for (ii = 0; ii < table->multi_res_info[0].nof_result_fields; ii++)
    {
        dbal_fields_e field_id = table->multi_res_info[0].results_info[ii].field_id;
        dbal_entry_value_field32_set(unit, entry_handle_id, field_id, INST_SINGLE, result_after_update);
    }

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT_UPDATE));

    /**
     * destroy later the entry that we just added
     */
    access_id_counter++;

    /**
     * Add 2 more entries to the table
     */
    for (jj = 2; jj < 4; jj++)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));

        for (ii = 0; ii < table->nof_key_fields; ii++)
        {
            dbal_fields_e field_id = table->keys_info[ii].field_id;
            SHR_IF_ERR_EXIT(dbal_fields_max_size_get(unit, field_id, &field_size));
            full_mask = (field_size == SAL_UINT32_NOF_BITS) ? (-1) : (SAL_UPTO_BIT(field_size));
            /*
             * key size should not be bigger than the maximum value of the field.
             * Choose maximum value instead
             */
            additional_key_value =
                (key_before_update + jj >
                 table->keys_info[ii].max_value) ? table->keys_info[ii].max_value : (key_before_update + jj);
            dbal_entry_key_field32_masked_set(unit, entry_handle_id, field_id, additional_key_value, full_mask);
        }

        for (ii = 0; ii < table->multi_res_info[0].nof_result_fields; ii++)
        {
            dbal_fields_e field_id = table->multi_res_info[0].results_info[ii].field_id;
            /*
             * result size should not be bigger than the maximum value of the field.
             * Choose maximum value instead
             */
            additional_result_value = (result_before_update + jj > table->multi_res_info[0].results_info[ii].max_value)
                ? table->multi_res_info[0].results_info[ii].max_value : (result_before_update + jj);
            dbal_entry_value_field32_set(unit, entry_handle_id, field_id, INST_SINGLE, additional_result_value);
        }

        SHR_IF_ERR_EXIT(dnx_field_entry_access_id_create(unit, core, table_id, 7, &access_ids[access_id_counter]));
        SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, access_ids[access_id_counter]));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        access_id_counter++;
    }

    /**
     * Rollback transaction operations and compare state
     */
    SHR_IF_ERR_EXIT(dnx_rollback_journal_end(unit, TRUE));
    SHR_IF_ERR_EXIT(dnx_state_comparison_end_and_compare(unit));

    /**
     * Check if there is a single entry in hw
     */
    SHR_IF_ERR_EXIT(diag_dbal_iterator_count_entries(unit, table_id, &counter));

    if (1 != counter)
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "Found %d entries in table, expected 1. table %s\n", counter,
                     dbal_logical_table_to_string(unit, table_id));
    }

    /**
     * Get the state of the entry in hw
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, table_id, entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_entry_handle_access_id_set(unit, entry_handle_id, access_ids[0]));
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    for (ii = 0; ii < table->nof_key_fields; ii++)
    {
        uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
        dbal_fields_e field_id = table->keys_info[ii].field_id;
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, field_id, field_val));
        if (field_val[0] != key_before_update)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "wrong value received for field %s expected %d received %d \n",
                         dbal_field_to_string(unit, field_id), key_before_update, field_val[0]);
        }
    }

    for (ii = 0; ii < table->multi_res_info[0].nof_result_fields; ii++)
    {
        uint32 field_val[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS] = { 0 };
        dbal_fields_e field_id = table->multi_res_info[0].results_info[ii].field_id;
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, field_id, INST_SINGLE, field_val));
        if (field_val[0] != result_before_update)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "wrong value received for field %s expected %d received %d \n",
                         dbal_field_to_string(unit, field_id), result_before_update, field_val[0]);
        }
    }

    /**
     * Cleanup
     */
    SHR_IF_ERR_EXIT(dbal_table_clear(unit, table_id));
    SHR_IF_ERR_EXIT(dnx_field_entry_access_id_destroy_all(unit, table_id));

    LOG_CLI((BSL_META("Test PASSED\n")));
    SHR_EXIT();
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
/**
 * \brief - test dummy DNX API that opts in for error recovery.
 * Modifies api_counter as if it is called withing a top-level BCM API. 
 * Pretends there is one entry in the rollback journal before transaction start.
 */
static shr_error_e
err_recovery_basic_test_dummy_dnx_api(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * there is one entry before the transaction.
     */
    dnx_err_recovery_manager[unit].entry_counter = 1;

    /*
     * pretend this we are inside of a BCM API
     */
    dnx_err_recovery_manager[unit].api_counter = 1;

    DNX_ERR_RECOVERY_START(unit);

    SHR_EXIT();
exit:
    DNX_ERR_RECOVERY_END(unit);

    if (dnx_err_recovery_manager[unit].entry_counter != 0)
    {
        dnx_err_recovery_manager[unit].entry_counter = 0;
    }
    if (dnx_err_recovery_manager[unit].api_counter != 0)
    {
        dnx_err_recovery_manager[unit].api_counter = 0;
    }
    SHR_FUNC_EXIT;
}
#endif

/**
 * \brief - basic error recovery manager test
 */
static shr_error_e
err_recovery_basic_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * don't fail the test if error recovery is disabled at compile-time
     */
    if (DNX_ERR_RECOVERY_IS_DISABLED)
    {
        LOG_CLI((BSL_META("Error recovery is not available\n")));
        SHR_EXIT();
    }

    LOG_CLI((BSL_META("1. Basic testing of internal flags and suppression mechanism\n")));
    {
        /*
         * feature should be initialized at that point
         */
        if (!dnx_err_recovery_manager[unit].is_initialized)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Error recovery feature is not initialized\n");
        }

        /*
         * outside of transaction, sanity check flags
         */
        if (!err_recovery_basic_test_flags_sanity(unit))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Error Recovery manager internals are in an invalid state prior to test.\n");
        }

        /*
         * pretend in an API that opted in for error recovery
         */
        if (_SHR_E_NONE != dnx_rollback_journal_start(unit))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Could not start a new Error Recovery transaction.\n");
        }

        /*
         * journaling should be on at this point
         */
        if (!dnx_err_recovery_is_on(unit))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to start journaling.\n");
        }

        /*
         * temporary disable error recovery, check if we are journaling
         */
        DNX_ERR_RECOVERY_SUPPRESS(unit);
        if (dnx_err_recovery_is_on(unit))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to suppress journaling.\n");
        }

        /*
         * re-enable error recovery, check if we are journaling
         */
        DNX_ERR_RECOVERY_UNSUPPRESS(unit);
        if (!dnx_err_recovery_is_on(unit))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to unsuppress journaling.\n");
        }

        /*
         * rollback empty transaction 
         */
        if (_SHR_E_NONE != dnx_rollback_journal_end(unit, TRUE))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Could not roll-back an empty Error Recovery transaction.\n");
        }

        /*
         * make sure that eror recovery is off after the transaction
         */
        if (dnx_err_recovery_is_on(unit))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to stop journaling.\n");
        }

        /*
         * re-check state at the end of the test
         */
        if (!err_recovery_basic_test_flags_sanity(unit))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Error Recovery manager internals are in an invalid state after test run.\n");
        }
    }

    /*
     * put "transaction end" and "transaction end" in "No Support" region,
     * check that error recovery is off and no additional errors
     */
    LOG_CLI((BSL_META("2. No support region mechanism testing\n")));
    {
        DNX_ERR_RECOVERY_NO_SUPPORT_BEGIN(unit);

        if (_SHR_E_NONE != dnx_rollback_journal_start(unit))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Could not start a new Error Recovery transaction.\n");
        }

        if (dnx_err_recovery_is_on(unit))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to stop journaling.\n");
        }

        if (_SHR_E_NONE != dnx_rollback_journal_end(unit, TRUE))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Could not roll-back an empty Error Recovery transaction.\n");
        }

        if (dnx_err_recovery_is_on(unit))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to stop journaling.\n");
        }

        DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);

        if (dnx_err_recovery_is_on(unit))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to stop journaling.\n");
        }

        /*
         * re-check state at the end of the test
         */
        if (!err_recovery_basic_test_flags_sanity(unit))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Error Recovery manager internals are in an invalid state after test run.\n");
        }
    }

#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
    LOG_CLI((BSL_META("3. Testing Error Recovery regression internals\n")));
    {
        /*
         * transaction begin / end failures should not fail the API
         */
        if (_SHR_E_NONE != err_recovery_basic_test_dummy_dnx_api(unit))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Dummy API returned error when it should not.\n");
        }

        /*
         * transaction begin / end failures should not fail in test mode
         */
        dnx_err_recovery_regression_testing_api_test_mode_change(unit, TRUE);

        if (_SHR_E_NONE == err_recovery_basic_test_dummy_dnx_api(unit))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Dummy API DID NOT return error when it should not.\n");
        }
    }
#endif

    LOG_CLI((BSL_META("Test PASSED\n")));
    SHR_EXIT();
exit:
#ifdef DNX_ERR_RECOVERY_REGRESSION_TESTING
    dnx_err_recovery_regression_testing_api_test_mode_change(unit, FALSE);
#endif
    SHR_FUNC_EXIT;
}

/**
 * \brief - sw state journal error recovery test
 */
static shr_error_e
err_recovery_snapshot_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_oam_group_info_t group_info;

    int rv = 0;
    uint8 group_name[BCM_OAM_GROUP_NAME_LENGTH] = { 0 };

    uint32 nof_buffer_elements = 1024;

    sw_state_htbl_init_info_t init_info;
    int key1 = 10;
    int value1 = 10;
    uint8 success = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * don't fail the test if error recovery is disabled at compile-time
     */
    if (DNX_ERR_RECOVERY_IS_DISABLED)
    {
        LOG_CLI((BSL_META("Error recovery is not available\n")));
        SHR_EXIT();
    }

    SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_during_test_set(unit, 1));

    /*
     * initialize group info structure
     */
    bcm_oam_group_info_t_init(&group_info);

    group_name[0] = 0x1;
    group_name[1] = 0x3;
    group_name[2] = 0x2;
    group_name[3] = 0xd;
    group_name[4] = 0xe;

    sal_memcpy(group_info.name, &group_name, sizeof(group_info.name));

    LOG_CLI((BSL_META("Init the example access module\n")));
    SHR_IF_ERR_EXIT(example.init(unit));

    LOG_CLI((BSL_META("Initiate the value of the fixed buffer elements.\n")));
    SHR_IF_ERR_EXIT(example.buffer_fixed.memset(unit, 0, nof_buffer_elements, 0));

    /*
     * initialize htbl parameters
     */
    init_info.max_nof_elements = 128;
    init_info.expected_nof_elements = 10;
    init_info.hash_function = NULL;
    init_info.print_cb_name = "sw_state_htb_example_entry_print";

    /*
     * set value for my_variable prior to transaction
     */
    SHR_IF_ERR_EXIT(example.my_variable.set(unit, 5));


    LOG_CLI((BSL_META("\nScenario 2: Snapshot before and after Error Recovery transaction.\n")));
    {
        LOG_CLI((BSL_META("Snapshot manager transaction start.\n")));
        if (dnx_state_comparison_start(unit) != _SHR_E_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Snapshot manager, failed to start.\n");
        }

        LOG_CLI((BSL_META("Start error recovery journaling.\n")));
        SHR_IF_ERR_EXIT(dnx_rollback_journal_start(unit));

        /*
         * full overlap, second should be invalidated my_variable
         */
        LOG_CLI((BSL_META("Test entries with fully overlapping buffers.\n")));
        SHR_IF_ERR_EXIT(example.default_value_tree.default_value_l1.my_variable.set(unit, 10));
        SHR_IF_ERR_EXIT(example.default_value_tree.default_value_l1.my_variable.set(unit, 11));

        LOG_CLI((BSL_META("Second buffer entry constains bounds for first buffer.\n")));
        /**
         *LOG_CLI((BSL_META("   2 ... 6    \n"
         *                  "   +-----+    \n"
         *                  "   |     |    \n"
         *                  "   +-----+    \n"
         *                  "0    ...    7 \n"
         *                  "+-----------+ \n"
         *                  "|           | \n"
         *                  "+-----------+ \n")));
         */

        SHR_IF_ERR_EXIT(example.buffer_fixed.memset(unit, 2, 5, 1));
        SHR_IF_ERR_EXIT(example.buffer_fixed.memset(unit, 0, 8, 0));

        LOG_CLI((BSL_META("First buffer containing bounds of second buffer.\n")));
        /**
         *LOG_CLI((BSL_META("7 8 9 10 11  \n"
         *                  "+----------+ \n"
         *                  "|          | \n"
         *                  "+----------+ \n"
         *                  "  8 9 10     \n"
         *                  "  +-----+    \n"
         *                  "  |     |    \n"
         *                  "  +-----+    \n")));
         */

        SHR_IF_ERR_EXIT(example.buffer_fixed.memset(unit, 7, 5, 1));
        SHR_IF_ERR_EXIT(example.buffer_fixed.memset(unit, 8, 3, 0));

        LOG_CLI((BSL_META("Partial overlap, 2nd buffer should be updated.\n")));
        /**
         *LOG_CLI((BSL_META("    13 14 15 \n"
         *                  "    +------+ \n"
         *                  "    |      | \n"
         *                  "    +------+ \n"
         *                  " 12 13 14    \n"
         *                  " +------+    \n"
         *                  " |      |    \n"
         *                  " +------+    \n")));
         */

        SHR_IF_ERR_EXIT(example.buffer_fixed.memset(unit, 13, 3, 1));
        SHR_IF_ERR_EXIT(example.buffer_fixed.memset(unit, 12, 3, 0));

        LOG_CLI((BSL_META("Partial overlap, 2nd buffer should be updated.\n")));
        /**
         *LOG_CLI((BSL_META("16 17 18   \n"
         *                  "+------+   \n"
         *                  "|      |   \n"
         *                  "+------+   \n"
         *                  "   17 18 19\n"
         *                  "   +------+\n"
         *                  "   |      |\n"
         *                  "   +------+\n")));
         */

        SHR_IF_ERR_EXIT(example.buffer_fixed.memset(unit, 16, 3, 1));
        SHR_IF_ERR_EXIT(example.buffer_fixed.memset(unit, 17, 3, 0));

        /*
         * This API opts-in for error recovery. It is used as a nested Error Recovery transaction example. 
         * Should produce entries in DBAL / SWSTATE error recovery and snapshot journals.
         * The error recovery entries are rolled-back at the end of the transaction
         */
        LOG_CLI((BSL_META("Calling API that opts-in for error recovery.\n")));
        rv = bcm_oam_group_create(unit, &group_info);

        if (rv != SOC_E_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error Recovery API returned an unexpected result.\n");
        }

        LOG_CLI((BSL_META("Rollback the Error Recovery transaction.\n")));
        SHR_IF_ERR_EXIT(dnx_rollback_journal_end(unit, TRUE));

        LOG_CLI((BSL_META("Snapshot manager transaction end and state compare.\n")));
        if (dnx_state_comparison_end_and_compare(unit) != _SHR_E_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Snapshot manager rollback error.\n");
        }
    }

    LOG_CLI((BSL_META("\nScenario 3: Snapshot before and after running DBAL error recovery test.\n")));
    {
        LOG_CLI((BSL_META("Snapshot manager transaction start.\n")));
        if (dnx_state_comparison_start(unit) != _SHR_E_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Snapshot manager, failed to start.\n");
        }

        LOG_CLI((BSL_META("Running DBAL an Error Recovery DBAL test ...\n")));
        if (CMD_OK != sh_process_command(unit, "ctest dbal error_recovery table=GLOBAL_LIF_EM"))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "\nDBAL error recovery test failred.\n");
        }

        LOG_CLI((BSL_META("Snapshot manager transaction end and state compare.\n")));
        if (dnx_state_comparison_end_and_compare(unit) != _SHR_E_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Snapshot manager rollback error.\n");
        }
    }

    LOG_CLI((BSL_META("\nScenario 4: Snapshot manager, basic swstate negative test.\n")));
    {
        LOG_CLI((BSL_META("4.1. Basic swstate memory change.\n")));
        LOG_CLI((BSL_META("Snapshot manager transaction start.\n")));
        if (dnx_state_comparison_start(unit) != _SHR_E_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Snapshot manager, failed to start.\n");
        }

        LOG_CLI((BSL_META("Changing sw state in during snapshot transaction\n")));
        SHR_IF_ERR_EXIT(example.my_variable.set(unit, 10));

        LOG_CLI((BSL_META("Snapshot manager transaction end and state compare. Expecting difference in state ...\n")));
        if (dnx_state_comparison_end_and_compare(unit) == _SHR_E_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Difference in system state not found where expected!\n");
        }

        LOG_CLI((BSL_META("4.2. Sw state structure high level journaling.\n")));

        LOG_CLI((BSL_META("Init the hash table\n")));
        SHR_IF_ERR_EXIT(example.htb.create(unit, &init_info));

        LOG_CLI((BSL_META("Snapshot manager transaction start.\n")));
        if (dnx_state_comparison_start(unit) != _SHR_E_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Snapshot manager, failed to start.\n");
        }

        LOG_CLI((BSL_META("Add an entry to hash table during transaction\n")));
        SHR_IF_ERR_EXIT(example.htb.insert(unit, &key1, &value1, &success));
        if (success == 0)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED - insertion of (10,10) failed\n");
        }

        LOG_CLI((BSL_META("Snapshot manager transaction end and state compare. Expecting difference in state ...\n")));
        if (dnx_state_comparison_end_and_compare(unit) == _SHR_E_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Difference in system state not found where expected!\n");
        }

        LOG_CLI((BSL_META("remove key 1 and expect")));
        SHR_IF_ERR_EXIT(example.htb.delete(unit, &key1));

    }


    LOG_CLI((BSL_META("Deinit the example access module\n")));
    SHR_IF_ERR_EXIT(example.deinit(unit));

    LOG_CLI((BSL_META("Test PASSED\n")));
exit:
    dnxc_sw_state_alloc_during_test_set(unit, 0);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - api error recovery test
 */
static shr_error_e
err_recovery_api_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
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
     * Scenario 1: ecmp
     */
    LOG_CLI((BSL_META("Running ecmp error recovery scenario...\n")));
    {
        /*
         * members to be added to the ecmp group 
         */
        int fecs[100];
        /*
         * nf - number of fecs
         */
        int i, nf;
        int fec = 0xA001;
        int rv = 0;
        int count = 0;
        bcm_l3_egress_t l3eg;
        bcm_if_t l3egid;

        /*
         * structure describing the ecmp group
         */
        bcm_l3_egress_ecmp_t ecmps[100];

        /*
         * make sure that eror recovery is off at the beginning of the test
         */
        if (dnx_err_recovery_is_on(unit))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Error recovery is on before test.\n");
        }

        /*
         * fill array with fec ids 
         */
        for (i = 0; i < 100; i++)
        {
            bcm_l3_egress_t_init(&l3eg);
            l3eg.intf = 0x40001002;
            l3egid = fec + i;
            rv = bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID, &l3eg, &l3egid);
            fecs[i] = l3egid;
        }

        /*
         * create ecmp groups, which have 5 to 77 number of members 
         */
        for (nf = 5; nf < 78; nf++)
        {
            bcm_l3_egress_ecmp_t_init(&ecmps[count]);
            ecmps[count].max_paths = 0;
            ecmps[count].dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT;
            ecmps[count].flags = BCM_L3_WITH_ID;
            ecmps[count].ecmp_group_flags = 0;
            ecmps[count].ecmp_intf = nf;
            rv = bcm_l3_egress_ecmp_create(unit, &ecmps[count], nf, fecs);
            if (rv != BCM_E_NONE)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "\nCould not create test data.\nTest failed.\n");
            }
            count++;
        }

        /*
         * at this point the table with the fec members is full 
         */
        /*
         * creates a failing ecmp group, since it doesn't have enough space for all members 
         */
        bcm_l3_egress_ecmp_t_init(&ecmps[count]);
        ecmps[count].max_paths = 0;
        ecmps[count].dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT;
        ecmps[count].flags = BCM_L3_WITH_ID;
        ecmps[count].ecmp_group_flags = 0;
        ecmps[count].ecmp_intf = 78;

        /*
         * Error recovery transaction here, expected to be rolled back on error
         */
        LOG_CLI((BSL_META("Expecting full ...\n")));

        rv = bcm_l3_egress_ecmp_create(unit, &ecmps[count], 78, fecs);
        if (SOC_E_FULL != rv)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "\nTest failed.\n");
        }

        /*
         * if transaction was not successfully rolled-back, then ecmp profile already exists
         * therefore no check for fec member table being full is carried out
         * which results in the API returning SOC_E_NONE, instead of SOC_E_FULL
         */
        LOG_CLI((BSL_META("Expecting full ...\n")));
        rv = bcm_l3_egress_ecmp_create(unit, &ecmps[count], 78, fecs);

        if (SOC_E_FULL != rv)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "\nTest failed.\n");
        }

        /*
         * make sure that eror recovery is off after the transaction
         */
        if (dnx_err_recovery_is_on(unit))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to stop journaling.\n");
        }

        /*
         * cleanup
         */
        for (i = 0; i < count; i++)
        {
            rv = bcm_l3_egress_ecmp_destroy(unit, &ecmps[i]);
            if (rv != BCM_E_NONE)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "\nCould not find entry.\nTest failed.\n");
            }

        }

        for (i = 0; i < 100; i++)
        {
            rv = bcm_l3_egress_destroy(unit, fecs[i]);
            if (rv != BCM_E_NONE)
            {
                SHR_ERR_EXIT(_SHR_E_FAIL, "Test fail\n");
            }
        }
    }

    /*
     * Scenario 2: OAM, nested api
     */
    LOG_CLI((BSL_META("Running OAM error recovery scenario...\n")));
    {
        bcm_oam_group_info_t group_info;
        bcm_oam_group_info_t group_info_result;

        int rv = 0;
        uint8 group_name[BCM_OAM_GROUP_NAME_LENGTH] = { 0 };

        /*
         * initialize group info structure
         */
        bcm_oam_group_info_t_init(&group_info);

        group_name[0] = 0x1;
        group_name[1] = 0x3;
        group_name[2] = 0x2;
        group_name[3] = 0xd;
        group_name[4] = 0xe;

        sal_memcpy(group_info.name, &group_name, sizeof(group_info.name));

        group_info.id = 6;
        group_info.flags |= BCM_OAM_GROUP_WITH_ID;

        /*
         * make sure that eror recovery is off at the beginning of the test
         */
        if (dnx_err_recovery_is_on(unit))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Error recovery is on before test.\n");
        }

        /*
         * pretend in an API that opted in for error recovery
         */
        if (_SHR_E_NONE != dnx_rollback_journal_start(unit))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Could not start a new Error Recovery transaction.\n");
        }

        /*
         * create the group, nested API opts in for error recovery
         * check if group was successfully created
         */
        rv = bcm_oam_group_create(unit, &group_info);

        if (rv != SOC_E_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
        }

        /*
         * rollback the nested and toplevel transaction 
         */
        if (_SHR_E_NONE != dnx_rollback_journal_end(unit, TRUE))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Could not roll-back Error Recovery transaction.\n");
        }

        /*
         * make sure that eror recovery is off after the transaction
         */
        if (dnx_err_recovery_is_on(unit))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to stop journaling.\n");
        }

        /*
         * try to get group, should not exist
         */
        rv = bcm_oam_group_get(unit, group_info.id, &group_info_result);

        if (rv != SOC_E_NOT_FOUND)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Test FAILED\n");
        }
    }

    LOG_CLI((BSL_META("Test PASSED\n")));
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - api error recovery test
 */
static shr_error_e
err_recovery_tcam_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 flags = BCM_FIELD_FLAG_MSB_RESULT_ALIGN;

    bcm_field_group_t fg_id_1;
    bcm_field_group_info_t fg_info;

    int ent_id_1;
    bcm_field_entry_info_t ent_info;
    bcm_field_entry_info_t get_ent_info;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * don't fail the test if error recovery is disabled at compile-time
     */
    if (DNX_ERR_RECOVERY_IS_DISABLED)
    {
        LOG_CLI((BSL_META("Error recovery is not available\n")));
        SHR_EXIT();
    }

    bcm_field_group_info_t_init(&fg_info);
    fg_info.fg_type = bcmFieldGroupTypeTcam;
    fg_info.stage = bcmFieldStageIngressPMF1;

    fg_info.nof_quals = 1;
    fg_info.qual_types[0] = bcmFieldQualifySrcMac;

    fg_info.nof_actions = 1;
    fg_info.action_types[0] = bcmFieldActionPrioIntNew;

    SHR_IF_ERR_EXIT(bcm_field_group_add(unit, flags, &fg_info, &fg_id_1));

    bcm_field_entry_info_t_init(&ent_info);
    bcm_field_entry_info_t_init(&get_ent_info);

    ent_info.priority = 0;

    ent_info.nof_entry_quals = 1;
    ent_info.entry_qual[0].type = fg_info.qual_types[0];
    ent_info.entry_qual[0].value[0] = 1;
    ent_info.entry_qual[0].mask[0] = 0xFFFFFFFF;

    ent_info.nof_entry_actions = fg_info.nof_actions;
    ent_info.entry_action[0].type = fg_info.action_types[0];
    ent_info.entry_action[0].value[0] = 1;

    LOG_CLI((BSL_META("1. Use comparison journal to check if entry add / delete are symmetric\n")));
    {
        SHR_IF_ERR_EXIT(dnx_state_comparison_start(unit));
        SHR_IF_ERR_EXIT(bcm_field_entry_add(unit, 0, fg_id_1, &ent_info, &ent_id_1));
        SHR_IF_ERR_EXIT(bcm_field_entry_delete(unit, fg_id_1, NULL, ent_id_1));
        SHR_IF_ERR_EXIT(dnx_state_comparison_end_and_compare(unit));

        LOG_CLI((BSL_META("Validating that the entry does not exist\n")));

        if (_SHR_E_NOT_FOUND != bcm_field_entry_info_get(unit, fg_id_1, ent_id_1, &get_ent_info))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Entry found when it should not exist.\n");
        }
    }

    LOG_CLI((BSL_META("2. Check rollback for entry add.\n")));
    {
        SHR_IF_ERR_EXIT(dnx_state_comparison_start(unit));

        if (_SHR_E_NONE != dnx_rollback_journal_start(unit))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Could not start a new Error Recovery transaction.\n");
        }

        SHR_IF_ERR_EXIT(bcm_field_entry_add(unit, 0, fg_id_1, &ent_info, &ent_id_1));

        if (_SHR_E_NONE != dnx_rollback_journal_end(unit, TRUE))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Could not rollback entry add.\n");
        }

        if (_SHR_E_NOT_FOUND != bcm_field_entry_info_get(unit, fg_id_1, ent_id_1, &get_ent_info))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Entry not found when it should exist.\n");
        }

        SHR_IF_ERR_EXIT(dnx_state_comparison_end_and_compare(unit));
    }

    LOG_CLI((BSL_META("3. Check rollback for entry delete.\n")));
    {
        SHR_IF_ERR_EXIT(dnx_state_comparison_start(unit));

        SHR_IF_ERR_EXIT(bcm_field_entry_add(unit, 0, fg_id_1, &ent_info, &ent_id_1));

        if (_SHR_E_NONE != dnx_rollback_journal_start(unit))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Could not start a new Error Recovery transaction.\n");
        }

        SHR_IF_ERR_EXIT(bcm_field_entry_delete(unit, fg_id_1, NULL, ent_id_1));

        if (_SHR_E_NONE != dnx_rollback_journal_end(unit, TRUE))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Could not rollback entry add.\n");
        }

        if (_SHR_E_NONE != bcm_field_entry_info_get(unit, fg_id_1, ent_id_1, &get_ent_info))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Entry not found when it should exist.\n");
        }

        SHR_IF_ERR_EXIT(bcm_field_entry_delete(unit, fg_id_1, NULL, ent_id_1));

        SHR_IF_ERR_EXIT(dnx_state_comparison_end_and_compare(unit));
    }

    LOG_CLI((BSL_META("4. Attempting to add twice the same entry. Error expected...\n")));
    {
        bcm_l3_vpbr_entry_t ent;
        bcm_l3_vpbr_entry_t_init(&ent);
        SHR_IF_ERR_EXIT(bcm_l3_vpbr_entry_add(unit, &ent));
        SHR_IF_ERR_EXIT(dnx_state_comparison_start(unit));
        (bcm_l3_vpbr_entry_add(unit, &ent));
        SHR_IF_ERR_EXIT(dnx_state_comparison_end_and_compare(unit));
        SHR_IF_ERR_EXIT(bcm_l3_vpbr_entry_delete(unit, &ent));
    }

    LOG_CLI((BSL_META("5. Attempting to delete twice the same entry. Error expected...\n")));
    {
        SHR_IF_ERR_EXIT(bcm_field_entry_add(unit, 0, fg_id_1, &ent_info, &ent_id_1));
        SHR_IF_ERR_EXIT(bcm_field_entry_delete(unit, fg_id_1, NULL, ent_id_1));
        SHR_IF_ERR_EXIT(dnx_state_comparison_start(unit));
        (bcm_field_entry_delete(unit, fg_id_1, NULL, ent_id_1));
        SHR_IF_ERR_EXIT(dnx_state_comparison_end_and_compare(unit));
    }

    LOG_CLI((BSL_META("6. Attempting to add twice the same entry with defrag. Error expected...\n")));
    {

        bcm_l3_vpbr_entry_t ent;
        bcm_l3_vpbr_entry_t_init(&ent);
        ent.priority = 1;
        SHR_IF_ERR_EXIT(bcm_l3_vpbr_entry_add(unit, &ent));
        SHR_IF_ERR_EXIT(dnx_state_comparison_start(unit));
        /*
         * The latter entry add has smaller priority that triggers defragmentation.
         */
        ent.priority = 0;
        (bcm_l3_vpbr_entry_add(unit, &ent));
        SHR_IF_ERR_EXIT(dnx_state_comparison_end_and_compare(unit));
        SHR_IF_ERR_EXIT(bcm_l3_vpbr_entry_delete(unit, &ent));
    }

exit:
    SHR_FUNC_EXIT;
}

#ifdef DNX_ERR_RECOVERY_VALIDATION
/**
 * \brief - api error recovery test
 */
static shr_error_e
err_recovery_restrict_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    soc_reg_above_64_val_t bank_select;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * don't fail the test if error recovery is disabled at compile-time
     */
    if (DNX_ERR_RECOVERY_IS_DISABLED)
    {
        LOG_CLI((BSL_META("Error recovery is not available\n")));
        SHR_EXIT();
    }


    /*
     * Scenario 4: valid hw access outside of dbal
     */
    LOG_CLI((BSL_META("Check for valid hw access outside of dbal...\n")));
    {
        if (_SHR_E_NONE != dnx_rollback_journal_start(unit))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Could not start a new Error Recovery transaction.\n");
        }

        /*
         * get and set the same value of reigster. Error expected.
         */
        SHR_IF_ERR_EXIT(soc_reg_above_64_get(unit, MDB_REG_500Ar, REG_PORT_ANY, 0, bank_select));

        if (SOC_E_NONE == soc_reg_above_64_set(unit, MDB_REG_500Ar, REG_PORT_ANY, 0, bank_select))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "HW access out of dbal is marked as valid!.\n");
        }

        if (_SHR_E_NONE != dnx_rollback_journal_end(unit, TRUE))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Could not roll-back Error Recovery transaction.\n");
        }

        /*
         * make sure that eror recovery is off after the transaction
         */
        if (dnx_err_recovery_is_on(unit))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to stop journaling.\n");
        }
    }

    LOG_CLI((BSL_META("Test PASSED\n")));
    SHR_EXIT();
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
#endif

static sh_sand_man_t err_recovery_custom_test_man = {
    "Error recovery, test custom jurnal"
};

static sh_sand_man_t err_recovery_dbal_test_man = {
    "Error recovery, test dbal cornercase scenarios"
};

static sh_sand_man_t err_recovery_basic_test_man = {
    "Error recovery, test basic flags"
};

static sh_sand_man_t err_recovery_api_test_man = {
    "Error recovery, test feature on basic apis"
};

static sh_sand_man_t err_recovery_snapshot_test_man = {
    "Error recovery, test feature on basic apis"
};

static sh_sand_man_t err_recovery_tcam_test_man = {
    "Error recovery, test basic tcam apis"
};

#ifdef DNX_ERR_RECOVERY_VALIDATION
static sh_sand_man_t err_recovery_restrict_test_man = {
    "Error recovery, test restriction checks"
};
#endif

sh_sand_cmd_t dnx_err_recovery_test_cmds[] = {
    {"CuSToM", err_recovery_custom_test_cmd, NULL, NULL, &err_recovery_custom_test_man, NULL, NULL, CTEST_PRECOMMIT},
    {"DBaL", err_recovery_dbal_test_cmd, NULL, NULL, &err_recovery_dbal_test_man, NULL, NULL, CTEST_PRECOMMIT},
    {"flag", err_recovery_basic_test_cmd, NULL, NULL, &err_recovery_basic_test_man, NULL, NULL, CTEST_PRECOMMIT},
    {"semantic", err_recovery_api_test_cmd, NULL, NULL, &err_recovery_api_test_man, NULL, NULL, CTEST_PRECOMMIT},
    {"state", err_recovery_snapshot_test_cmd, NULL, NULL, &err_recovery_snapshot_test_man, NULL, NULL, CTEST_PRECOMMIT},
    {"TCam", err_recovery_tcam_test_cmd, NULL, NULL, &err_recovery_tcam_test_man, NULL, NULL, CTEST_PRECOMMIT},
#ifdef DNX_ERR_RECOVERY_VALIDATION
    {"ReSTRiCT", err_recovery_restrict_test_cmd, NULL, NULL, &err_recovery_restrict_test_man, NULL, NULL,
     CTEST_PRECOMMIT},
#endif
    {NULL}
};
