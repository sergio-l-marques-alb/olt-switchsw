/*
 * ! \file ctest_dnx_err_recovery.c 
 * Purpose: shell registers commands for error recovery tests 
 */
/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
/*
 * Include files.
 * {
 */
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_error.h>
/** allow drv.h include excplictly */
#ifdef _SOC_DRV_H
#undef _SOC_DRV_H
#endif
#include <soc/drv.h>    /** needed for  SOC_CORE_ALL */
#include <shared/pbmp.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnx/dnx_err_recovery_manager_common.h>
#include <soc/dnx/dnx_er_threading.h>
#include <soc/dnx/dnx_state_snapshot_manager.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_dev_init.h>
#include <soc/dnx/swstate/auto_generated/access/example_access.h>

#include <bcm_int/dnx/field/field_entry.h>
#include <bcm_int/dnx/l3/l3_ecmp.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dbal/dbal_dynamic.h>

#include <appl/diag/sand/diag_sand_framework.h>
#include <soc/dnx/recovery/generic_state_journal.h>
#include <bcm_int/dnx/mdb/mdb_init.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <bcm/l3.h>
#include <bcm/oam.h>

#include "dbal/ctest_dnx_dbal.h"

#include <soc/dnxc/swstate/sw_state_features.h>
#include <soc/dnxc/swstate/dnxc_sw_state_verifications.h>

#include <bcm/switch.h>
#include <bcm_int/dnx_dispatch.h>

#include <soc/dnx/recovery/rollback_journal_utils.h>

#include <appl/diag/diag.h>

#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
/** The number of FEC that will be used for the ECMP groups */
#define DNX_CTEST_ERR_REC_NOF_FEC_MEMBERS 16

/*
 * }
 */
/*
 * Externs
 * {
 */

/**
 * \brief - get access to the error recovery manager in order to test internals
 */
extern err_recovery_manager_t dnx_err_recovery_manager[SOC_MAX_NUM_DEVICES];

/**
 * \brief - get access to the snapshot manager in order to test internals
 */
extern dnx_state_snapshot_manager_t dnx_state_snapshot_manager[SOC_MAX_NUM_DEVICES];

/**
 * \brief - describes data required by the dummy API for the threading test.
 */
typedef struct dnx_err_recovery_test_thread_d
{
    int unit;
    uint8 opts_in;
    int sec_sleep_start;
    int sec_sleep_end;
} dnx_err_recovery_test_thread_t;

/**
 * \brief - sanity check the error recovery manager internal flags
 */
static uint8
err_recovery_basic_test_flags_sanity(
    int unit)
{
    return ((0 == dnx_err_recovery_manager[unit].is_suppressed_counter)
            &&
            (dnx_err_recovery_common_flag_is_on
             (unit, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK, DNX_ERR_RECOVERY_COMMON_IS_ON_NOT_INVALIDATED))
#ifdef DNX_ERR_RECOVERY_VALIDATION_BASIC
            && (dnx_err_recovery_manager[unit].api_counter == 0)
#endif /* DNX_ERR_RECOVERY_VALIDATION_BASIC */
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
    dbal_table_field_input_info_t *keys_info_p = NULL;
    dbal_table_field_input_info_t *results_info_p = NULL;

    SHR_FUNC_INIT_VARS(unit);

    var = initial_value = 5;

    LOG_CLI((BSL_META("1. Attempting to rollback the state of local variable using memcpy interface\n")));
    {
        if (_SHR_E_NONE != dnx_rollback_journal_start(unit))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Could not start a new Error Recovery transaction.\n");
        }

        SHR_IF_ERR_EXIT(dnx_generic_state_journal_log_memcpy(unit, sizeof(var), (uint8 *) (&var)));

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

    LOG_CLI((BSL_META("2. Check if field create is rollbackable\n")));
    {
        dbal_fields_e dynamic_key_field, dynamic_result_field, temp_dynamic_id;

        SHR_IF_ERR_EXIT(dbal_fields_field_create
                        (unit, DBAL_FIELD_TYPE_DEF_UINT, "dynamic field 1", &dynamic_key_field));

        SHR_IF_ERR_EXIT(dnx_state_comparison_start(unit));
        SHR_IF_ERR_EXIT(dnx_rollback_journal_start(unit));

        SHR_IF_ERR_EXIT(dbal_fields_field_create
                        (unit, DBAL_FIELD_TYPE_DEF_VLAN_ID, "dynamic field 2", &dynamic_result_field));
        temp_dynamic_id = dynamic_result_field;

        SHR_IF_ERR_EXIT(dnx_rollback_journal_end(unit, TRUE));
        SHR_IF_ERR_EXIT(dnx_state_comparison_end_and_compare(unit));

        SHR_IF_ERR_EXIT(dbal_fields_field_create
                        (unit, DBAL_FIELD_TYPE_DEF_VLAN_ID, "dynamic field 2", &dynamic_result_field));

        if (temp_dynamic_id != dynamic_result_field)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, " recreated ID %d not equal to the orig ID %d\n", dynamic_result_field,
                         temp_dynamic_id);
        }

        SHR_IF_ERR_EXIT(dbal_fields_field_destroy(unit, dynamic_key_field));
        SHR_IF_ERR_EXIT(dbal_fields_field_destroy(unit, dynamic_result_field));
    }

    LOG_CLI((BSL_META("3. Check if result type is rollbackable\n")));
    {
        /*
         * int rv;
         */
        int nof_result_fields;
        int result_type_hw_value;
        int result_type_size;
        int result_type_index_1;
        int result_type_index_2;

        CONST dbal_logical_table_t *table;
        dbal_tables_e table_id = DBAL_TABLE_ETM_PP_DESCRIPTOR_EXPANSION;
        multi_res_info_t multiple_results_tmp[20];

        dbal_table_field_input_info_t results_info[3] = { {0} };
        int nof_res_types_tmp = 0;
        int idx = 0;

        nof_result_fields = 3;
        results_info[0].field_id = DBAL_FIELD_RESULT_TYPE;
        results_info[0].field_nof_bits = 6;
        results_info[1].field_id = DBAL_FIELD_DESTINATION;
        results_info[1].field_nof_bits = 21;
        results_info[2].field_id = DBAL_FIELD_GLOB_OUT_LIF;
        results_info[2].field_nof_bits = 22;
        result_type_hw_value = 5;
        result_type_size = 6;
        /*
         *******************************
         * Rollback result type create *
         *******************************
         */
        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));

        /*
         * save multi res info
         */
        nof_res_types_tmp = table->nof_result_types;
        for (idx = 0; idx < table->nof_result_types; idx++)
        {
            multiple_results_tmp[idx] = table->multi_res_info[idx];
        }

        /*
         * start rollback journal 
         */
        SHR_IF_ERR_EXIT(dnx_rollback_journal_start(unit));

        LOG_CLI((BSL_META("Add valid dynamic result type and roll it back\n")));
        SHR_IF_ERR_EXIT(dbal_tables_result_type_add(unit, table_id, nof_result_fields, results_info,
                                                    "TESTING_TYPE", result_type_size, result_type_hw_value,
                                                    &result_type_index_1));
        SHR_IF_ERR_EXIT(mdb_init_update_vmv(unit, table->physical_db_id[0]));

        /*
         * rollback result type add 
         */
        SHR_IF_ERR_EXIT(dnx_rollback_journal_end(unit, TRUE));

        /*
         * verify that the result types match
         */
        if (table->nof_result_types != nof_res_types_tmp)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Result type number doesn't match after rollback\n");
        }

        for (idx = 0; idx < table->nof_result_types; idx++)
        {
            if ((multiple_results_tmp[idx].result_type_nof_hw_values !=
                 table->multi_res_info[idx].result_type_nof_hw_values)
                || (multiple_results_tmp[idx].entry_payload_size != table->multi_res_info[idx].entry_payload_size)
                || (multiple_results_tmp[idx].nof_result_fields != table->multi_res_info[idx].nof_result_fields)
                ||
                (sal_strncmp
                 (multiple_results_tmp[idx].result_type_name, table->multi_res_info[idx].result_type_name,
                  DBAL_MAX_STRING_LENGTH)))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Result type idx %d doesn't match\n", idx);
            }
        }

        LOG_CLI((BSL_META("Add it again after the rollback, should NOT fail...\n")));
        SHR_IF_ERR_EXIT(dbal_tables_result_type_add(unit, table_id, nof_result_fields, results_info,
                                                    "TESTING_TYPE", result_type_size, result_type_hw_value,
                                                    &result_type_index_2));
        SHR_IF_ERR_EXIT(mdb_init_update_vmv(unit, table->physical_db_id[0]));

        /*
         * verify that the result type is inserted in the same index
         */
        if (result_type_index_1 != result_type_index_2)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Initial and additional result type indexes don't match\n");
        }

        /*
         ********************************
         * Rollback result type destroy *
         ********************************
         */

        /*
         * save multi res info
         */
        nof_res_types_tmp = table->nof_result_types;
        for (idx = 0; idx < table->nof_result_types; idx++)
        {
            multiple_results_tmp[idx] = table->multi_res_info[idx];
        }

        /*
         * start rollback journal
         */
        SHR_IF_ERR_EXIT(dnx_rollback_journal_start(unit));

        LOG_CLI((BSL_META("Delete the dynamic result type\n")));
        SHR_IF_ERR_EXIT(dbal_tables_dynamic_result_type_delete(unit, table_id, result_type_index_1));

        /*
         * rollback result type add 
         */
        SHR_IF_ERR_EXIT(dnx_rollback_journal_end(unit, TRUE));

        if (table->nof_result_types != nof_res_types_tmp)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Result type number doesn't match after rollback\n");
        }

        for (idx = 0; idx < table->nof_result_types; idx++)
        {
            if ((multiple_results_tmp[idx].result_type_nof_hw_values !=
                 table->multi_res_info[idx].result_type_nof_hw_values)
                || (multiple_results_tmp[idx].entry_payload_size != table->multi_res_info[idx].entry_payload_size)
                || (multiple_results_tmp[idx].nof_result_fields != table->multi_res_info[idx].nof_result_fields)
                ||
                (sal_strncmp
                 (multiple_results_tmp[idx].result_type_name, table->multi_res_info[idx].result_type_name,
                  DBAL_MAX_STRING_LENGTH)))
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Result type idx %d doesn't match\n", idx);
            }
        }
        LOG_CLI((BSL_META("Delete the rolled back entry at the end of the test\n")));
        SHR_IF_ERR_EXIT(dbal_tables_dynamic_result_type_delete(unit, table_id, result_type_index_1));
    }

    LOG_CLI((BSL_META("4. Check if access_info_set for EM table is rollbackable.\n")));
    {
        CONST dbal_logical_table_t *table;
        dbal_tables_e table_id = DBAL_TABLE_EMPTY;
        dbal_logical_table_t pre_change_table_info;
        dbal_logical_table_t post_change_table_info;

        int nof_key_fields = 1;
        int nof_result_fields = 1;

        dbal_table_mdb_access_info_t access_info = { 0 };
        /*
         ***********************************
         * Rollback access info set for EM *
         ***********************************
         */
        /*
         * Create a dynamic table.
         */
        SHR_ALLOC_SET_ZERO(keys_info_p, sizeof(*keys_info_p) * nof_key_fields,
                           "keys_info_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
        SHR_ALLOC_SET_ZERO(results_info_p, sizeof(*results_info_p) * nof_result_fields,
                           "results_info_p", "%s%s%s", EMPTY, EMPTY, EMPTY);
        keys_info_p[0].field_id = 5;
        keys_info_p[0].field_nof_bits = 1;
        results_info_p[0].field_id = 5;
        results_info_p[0].field_nof_bits = 1;
        SHR_IF_ERR_EXIT(dbal_tables_table_create
                        (unit, DBAL_ACCESS_METHOD_MDB, DBAL_TABLE_TYPE_EM, DBAL_CORE_MODE_SBC, nof_key_fields,
                         keys_info_p, nof_result_fields, results_info_p, NULL, &table_id));
        /*
         * Save the table info. 
         * We assume that the pointers don't change and that their content won't be changed by access info set.
         */
        SHR_IF_ERR_EXIT(dbal_tables_table_get(unit, table_id, &table));
        sal_memcpy(&pre_change_table_info, table, sizeof(pre_change_table_info));

        /*
         * start rollback journal 
         */
        SHR_IF_ERR_EXIT(dnx_rollback_journal_start(unit));

        LOG_CLI((BSL_META("Set access info and roll it back\n")));
        access_info.nof_physical_tables = 1;
        access_info.tcam_handler_id = DNX_FIELD_TCAM_HANDLER_INVALID;
        access_info.app_id = 40;
        access_info.app_id_size = 6;
        SHR_IF_ERR_EXIT(mdb_init_logical_table(unit, table_id, &access_info));

        /*
         * Verify that the table info has changed.
         */
        if (0 == sal_memcmp(&pre_change_table_info, table, sizeof(pre_change_table_info)))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "dbal_tables_table_access_info_set did not change the Metadata of table %s\n",
                         dbal_logical_table_to_string(unit, table_id));
        }

        /*
         * Save he table's metadata after the change.
         */
        sal_memcpy(&post_change_table_info, table, sizeof(pre_change_table_info));

        /*
         * rollback access_info
         */
        SHR_IF_ERR_EXIT(dnx_rollback_journal_end(unit, TRUE));

        /*
         * verify that the table's metadata was rollbacked.
         */
        if (sal_memcmp(&pre_change_table_info, table, sizeof(pre_change_table_info)))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Rollback did not resume the metadata of table %s\n",
                         dbal_logical_table_to_string(unit, table_id));
        }

        LOG_CLI((BSL_META("Cahnge it again after the rollback, should NOT fail...\n")));
        SHR_IF_ERR_EXIT(mdb_init_logical_table(unit, table_id, &access_info));

        /*
         * verify that the the table's metatada was changed in exactly the same way.
         */
        if (sal_memcmp(&post_change_table_info, table, sizeof(pre_change_table_info)))
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "After rollback the same action changed the metadata of table %s "
                         "differently.\n", dbal_logical_table_to_string(unit, table_id));
        }

        LOG_CLI((BSL_META("Delete the created table at the end of the test\n")));
        SHR_IF_ERR_EXIT(dbal_tables_table_destroy(unit, table_id));
    }

exit:
    SHR_FREE(keys_info_p);
    SHR_FREE(results_info_p);
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
     * feature should be initialized at that point
     */
    if (!dnx_err_recovery_common_flag_is_on
        (unit, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK, DNX_ERR_RECOVERY_COMMON_IS_ON_INITIALIZED))
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

/**
 * \brief - test dummy L3 ECMP DNX API that opts in for error recovery.
 * Returns error based on weather or not error recovery should be available in the API
 * Pretends there is one entry in the rollback journal before transaction start.
 */
static shr_error_e
err_recovery_basic_test_dummy_l3_ecmp_api(
    int unit,
    uint8 should_have_er)
{
    SHR_FUNC_INIT_VARS(unit);

#ifdef DNX_ERR_RECOVERY_VALIDATION_BASIC
    /*
     * pretend this we are inside of a BCM API
     */
    dnx_err_recovery_manager[unit].api_counter = 1;
#endif /* DNX_ERR_RECOVERY_VALIDATION */

    DNX_ERR_RECOVERY_START_MODULE_ID(unit, bcmModuleL3EgressEcmp);

    if (should_have_er != dnx_err_recovery_is_on_test(unit))
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Error Recovery L3 ecmp test api has unexpected internal status!\n");
    }

    SHR_EXIT();
exit:
    DNX_ERR_RECOVERY_END_MODULE_ID(unit, bcmModuleL3EgressEcmp);

#ifdef DNX_SW_STATE_VERIFICATIONS_BASIC
    if (dnx_err_recovery_manager[unit].api_counter != 0)
    {
        dnx_err_recovery_manager[unit].api_counter = 0;
    }
#endif /* DNX_ERR_RECOVERY_VALIDATION */
    SHR_FUNC_EXIT;
}

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

    LOG_CLI((BSL_META("1. Basic testing of internal flags and suppression mechanism\n")));
    {
        /*
         * feature should be initialized at that point
         */
        if (!dnx_err_recovery_common_flag_is_on
            (unit, DNX_ROLLBACK_JOURNAL_TYPE_ROLLBACK, DNX_ERR_RECOVERY_COMMON_IS_ON_INITIALIZED))
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
        if (!dnx_err_recovery_is_on_test(unit))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to start journaling.\n");
        }

        /*
         * temporary disable error recovery, check if we are journaling
         */
        DNX_ERR_RECOVERY_SUPPRESS(unit);
        if (dnx_err_recovery_is_on_test(unit))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to suppress journaling.\n");
        }

        /*
         * re-enable error recovery, check if we are journaling
         */
        DNX_ERR_RECOVERY_UNSUPPRESS(unit);
        if (!dnx_err_recovery_is_on_test(unit))
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
        if (dnx_err_recovery_is_on_test(unit))
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

        if (dnx_err_recovery_is_on_test(unit))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to stop journaling.\n");
        }

        if (_SHR_E_NONE != dnx_rollback_journal_end(unit, TRUE))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Could not roll-back an empty Error Recovery transaction.\n");
        }

        if (dnx_err_recovery_is_on_test(unit))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to stop journaling.\n");
        }

        DNX_ERR_RECOVERY_NO_SUPPORT_END(unit);

        if (dnx_err_recovery_is_on_test(unit))
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

    LOG_CLI((BSL_META("3. Testing Error Recovery disabled switch control\n")));
    {
        bcm_switch_control_key_t key;
        bcm_switch_control_info_t value;

        key.type = bcmSwitchModuleErrorRecoveryEnable;
        key.index = bcmModuleL3EgressEcmp;

        /*
         * Disable ER support through the designated switch control
         * and validate
         */
        value.value = FALSE;
        SHR_IF_ERR_EXIT(bcm_dnx_switch_control_indexed_set(unit, key, value));

        if (_SHR_E_NONE != err_recovery_basic_test_dummy_l3_ecmp_api(unit, FALSE))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Dummy API returned error when it should not.\n");
        }

        value.value = TRUE;
        SHR_IF_ERR_EXIT(bcm_dnx_switch_control_indexed_set(unit, key, value));

        /*
         * Value has been restored, check if we can journal again for the module
         */
        if (_SHR_E_NONE != err_recovery_basic_test_dummy_l3_ecmp_api(unit, TRUE))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Dummy API returned error when it should not.\n");
        }

    }

    LOG_CLI((BSL_META("Test PASSED\n")));
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

#ifdef DNX_ERR_RECOVERY_VALIDATION
/**
 * \brief - test dummy DNX API that opts or out of error recovery.
 */
static shr_error_e
err_recovery_thread_test_dummy_dnx_api(
    dnx_err_recovery_test_thread_t data)
{
    uint32 idx = 0;
    uint32 changes_count = 10;

    SHR_FUNC_INIT_VARS(data.unit);

    /*
     * sleep the thread before start
     */
    if (data.sec_sleep_start > 0)
    {
        sal_sleep(data.sec_sleep_start);
    }

    /*
     * either opt-in or opt-out based on input parameter.
     */
    if (data.opts_in)
    {
        LOG_CLI((BSL_META("TRANSACTION START.\n")));
        dnx_err_recovery_transaction_start(data.unit, bcmModuleCount);

        if (dnx_err_recovery_manager[data.unit].entry_counter != 0)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Entries exist at the beginning of the the tranasction.\n");
        }
    }
    else
    {
        LOG_CLI((BSL_META("NO SUPPORT BEGIN.\n")));
        dnx_err_recovery_transaction_no_support_counter_inc(data.unit);
    }

    /*
     * change value of my_variable 10 times. Weather or not these changes
     * translate into journal entries depends on the scenario
     */
    LOG_CLI((BSL_META("Entry count in transaction %u\n"), dnx_err_recovery_manager[data.unit].entry_counter));
    LOG_CLI((BSL_META("Changing swstate %u times...\n"), changes_count));
    for (idx = 0; idx < changes_count; idx++)
    {
        DNX_ERR_RECOVERY_SUPPRESS(data.unit);
        SHR_IF_ERR_EXIT(example.mutex_test.take(data.unit, 0));
        DNX_ERR_RECOVERY_UNSUPPRESS(data.unit);

        SHR_IF_ERR_EXIT(example.my_variable.set(data.unit, idx));

        DNX_ERR_RECOVERY_SUPPRESS(data.unit);
        SHR_IF_ERR_EXIT(example.mutex_test.give(data.unit));
        DNX_ERR_RECOVERY_UNSUPPRESS(data.unit);
    }
    LOG_CLI((BSL_META("Entry count in transaction %u\n"), dnx_err_recovery_manager[data.unit].entry_counter));

    /*
     * validate that the entry count maches the number of changes done
     */
    if (data.opts_in && dnx_err_recovery_manager[data.unit].entry_counter != changes_count)
    {
        LOG_CLI((BSL_META("Entry count does not match the expected number\n")));
        assert(0);
    }

    /*
     * sleep the thread before end
     */
    if (data.sec_sleep_end > 0)
    {
        sal_sleep(data.sec_sleep_end);
    }

    SHR_EXIT();
exit:
    /*
     * either end transaction or no support region based on input parameter.
     */
    if (data.opts_in)
    {
        LOG_CLI((BSL_META("TRANSACTION END.\n")));
        dnx_err_recovery_transaction_end(data.unit, bcmModuleCount, FALSE);
    }
    else
    {
        LOG_CLI((BSL_META("NO SUPPORT END.\n")));
        dnx_err_recovery_transaction_no_support_counter_dec(data.unit);
    }

    SHR_FUNC_EXIT;
}

static void
err_recovery_thread_cb(
    void *data_ptr)
{
    dnx_err_recovery_test_thread_t data = ((dnx_err_recovery_test_thread_t *) data_ptr)[0];

    /*
     * no need to inc / dec the API counters. BCM API verification is not being enforced.
     */
    err_recovery_thread_test_dummy_dnx_api(data);

    sal_thread_exit(0);
}

static shr_error_e
err_recovery_thread_test_run_scenario(
    dnx_err_recovery_test_thread_t main_data,
    dnx_err_recovery_test_thread_t bcmERTest_data)
{
    /*
     * Arbitrary chosen, higher priority than SOC_CLI_THREAD_PRI
     */
    int thread_priority = (SOC_CLI_THREAD_PRI + 10);

    sal_thread_t er_tid = SAL_THREAD_ERROR;

    SHR_FUNC_INIT_VARS(main_data.unit);

    er_tid = sal_thread_create("bcmERTest", SAL_THREAD_STKSZ, thread_priority, err_recovery_thread_cb, &bcmERTest_data);

    if (er_tid == SAL_THREAD_ERROR)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Was not able to spawn test thread.\n");
    }

    SHR_IF_ERR_EXIT(err_recovery_thread_test_dummy_dnx_api(main_data));

    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - error recovery threading scenarios
 */
static shr_error_e
err_recovery_thread_test_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    /*
     * data used by the test API on the main and the bcmERTest threads
     */
    dnx_err_recovery_test_thread_t main_data;
    dnx_err_recovery_test_thread_t bcmERTest_data;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * init the example module.
     * test utilizes only swstate entries.
     */
    SHR_IF_ERR_EXIT(dnxc_sw_state_alloc_during_test_set(unit, 1));
    SHR_IF_ERR_EXIT(example.init(unit));
    SHR_IF_ERR_EXIT(example.mutex_test.create(unit));
    SHR_IF_ERR_EXIT(dnx_er_threading_test_skip_validation(unit, TRUE));

    /*
     * Simulate that another thread that supports ER is calling an API
     * that opts-in for ER during a no support region in the current thread.
     * The goal is to validate that a transaction can be opened on a non-main thread.
     *
     */
    LOG_CLI((BSL_META("\n1. Transaction on a non-main thread.\n")));
    {
        /*
         *  Scenario 1.1:
         *
         *  [main] NO_SUPPORT START
         *     [bcmERTest] TRANSACTION START
         *     [bcmERTest] TRANSACTION END
         *  [main] NO_SUPPORT END
         */

        /*
         * main thread API opts-out of ER
         * wait 2 seconds for "bcmERTest thread" to finish "TRANSACTION END" before executing "NO_SUPPORT END"
         */
        main_data.unit = unit;
        main_data.opts_in = FALSE;
        main_data.sec_sleep_start = 0;
        main_data.sec_sleep_end = 2;

        /*
         * bcmERTest thread API opts-in of ER,
         * wait 1 second for "main thread" to finish "NO_SUPPORT START" before executing "TRANSACTION START"
         */
        bcmERTest_data.unit = unit;
        bcmERTest_data.opts_in = TRUE;
        bcmERTest_data.sec_sleep_start = 1;
        bcmERTest_data.sec_sleep_end = 0;

        SHR_IF_ERR_EXIT(err_recovery_thread_test_run_scenario(main_data, bcmERTest_data));

        LOG_CLI((BSL_META("\n")));

        /*
         *  Scenario 1.2:
         *
         *  [main] NO_SUPPORT START
         *     [bcmERTest] TRANSACTION START
         *  [main] NO_SUPPORT END
         *     [bcmERTest] TRANSACTION END
         */

        /*
         * main thread API opts-out of ER,
         * wait 2 seconds for "bcmERTest thread" to finish "TRANSACTION START" before executing "NO_SUPPORT END"
         */
        main_data.unit = unit;
        main_data.opts_in = FALSE;
        main_data.sec_sleep_start = 0;
        main_data.sec_sleep_end = 2;

        /*
         * bcmERTest thread API opts-in of ER,
         * wait 1 second for "main thread" to finish "NO_SUPPORT START" before executing "TRANSACTION START"
         * wait 2 second for "main thread" to finish "NO_SUPPORT END" before executing "TRANSACTION END"
         */
        bcmERTest_data.unit = unit;
        bcmERTest_data.opts_in = TRUE;
        bcmERTest_data.sec_sleep_start = 1;
        bcmERTest_data.sec_sleep_end = 2;
        SHR_IF_ERR_EXIT(err_recovery_thread_test_run_scenario(main_data, bcmERTest_data));

        /*
         * need to wait at most 2 seconds to make sure that the bcmERTest thread has exited,
         * in order to complete the scenario.
         */
        sal_sleep(2);
    }

    /*
     * Simulate that another thread that supports ER is calling an API
     * that opts-out for ER during a currently opened transaction.
     * The goal is to validate that the bcmERTest thread cannot insert entries in the current transaction.
     */
    LOG_CLI((BSL_META("\n2. Transaction on current thread, no support API on a parallel thread.\n")));
    {
        /*
         *  Scenario 2.1:
         *
         *  [main] TRANSACTION START
         *     [bcmERTest] NO_SUPPORT START
         *     [bcmERTest] NO_SUPPORT END
         *  [main] TRANSACTION END
         */

        /*
         * main thread API opts-in of ER
         * wait 2 seconds for "bcmERTest thread" to finish "NO_SUPPORt END" before executing "TRANSACTION END"
         */
        main_data.unit = unit;
        main_data.opts_in = TRUE;
        main_data.sec_sleep_start = 0;
        main_data.sec_sleep_end = 2;

        /*
         * bcmERTest thread API opts-out of ER,
         * wait 1 second for "main thread" to finish "TRANSATION START" before executing "NO_SUPPORT START"
         */
        bcmERTest_data.unit = unit;
        bcmERTest_data.opts_in = FALSE;
        bcmERTest_data.sec_sleep_start = 1;
        bcmERTest_data.sec_sleep_end = 0;

        SHR_IF_ERR_EXIT(err_recovery_thread_test_run_scenario(main_data, bcmERTest_data));

        LOG_CLI((BSL_META("\n")));

        /*
         *  Scenario 2.2:
         *
         *  [main] TRANSACTION START
         *     [bcmERTest] NO_SUPPORT START
         *  [main] TRANSACTION END
         *     [bcmERTest] NO_SUPPORT END
         */

        /*
         * main thread API opts-in of ER,
         * wait 2 seconds for "bcmERTest thread" to finish "NO_SUPPORT START" before executing "TRANSACTION END"
         */
        main_data.unit = unit;
        main_data.opts_in = TRUE;
        main_data.sec_sleep_start = 0;
        main_data.sec_sleep_end = 2;

        /*
         * bcmERTest thread API opts-out of ER,
         * wait 1 second for "main thread" to finish "TRANSATION START" before executing "NO_SUPPORT START"
         * wait 2 second for "main thread" to finish "TRANSACTION END" before executing "NO_SUPPORT END"
         */
        bcmERTest_data.unit = unit;
        bcmERTest_data.opts_in = FALSE;
        bcmERTest_data.sec_sleep_start = 1;
        bcmERTest_data.sec_sleep_end = 2;
        SHR_IF_ERR_EXIT(err_recovery_thread_test_run_scenario(main_data, bcmERTest_data));

        /*
         * need to wait at most 2 seconds to make sure that the bcmERTest thread has exited,
         * in order to complete the scenario.
         */
        sal_sleep(2);
    }

exit:
    /*
     * deinit the example module at the end of the test.
     */
    SHR_IF_ERR_EXIT(dnx_er_threading_test_skip_validation(unit, FALSE));
    example.mutex_test.destroy(unit);
    example.deinit(unit);
    dnxc_sw_state_alloc_during_test_set(unit, 0);
    SHR_FUNC_EXIT;
}
#endif /* DNX_ERR_RECOVERY_VALIDATION */

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
        /*
         * Make sure that the table being tested is empty.
         * The error recovery test is running the ltt test first, which does not guarantee that the
         * state of the table is restored after the ltt test completes. This produces a comparison
         * journal difference for non-empty tables, therefore we need to clear the table first
         */
        if (_SHR_E_NONE != dbal_table_clear(unit, DBAL_TABLE_ING_VSI_INFO_DB))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Cloud not clear table ING_VSI_INFO_DB\n");
        }

        LOG_CLI((BSL_META("Snapshot manager transaction start.\n")));
        if (dnx_state_comparison_start(unit) != _SHR_E_NONE)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Snapshot manager, failed to start.\n");
        }

        LOG_CLI((BSL_META("Running DBAL an Error Recovery DBAL test ...\n")));
        if (CMD_OK != sh_process_command(unit, "ctest dbal error_recovery table=ING_VSI_INFO_DB"))
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
     * Scenario 1: ECMP
     */
    LOG_CLI((BSL_META("Running ECMP error recovery scenario...\n")));
    {
        /*
         * members to be added to the ECMP group
         */
        bcm_if_t fecs[DNX_CTEST_ERR_REC_NOF_FEC_MEMBERS];

        int fec_iter, ecmp_iter;
        uint32 fec, fec_range_size;
        uint32 ecmp_start_id = 50;
        uint32 ecmp_count = ecmp_start_id;
        bcm_l3_egress_t l3eg;
        bcm_if_t l3egid;
        bcm_if_t arp_id = 0x40001002;
        bcm_l3_egress_ecmp_t ecmp;

        /*
         * make sure that error recovery is off at the beginning of the test
         */
        if (dnx_err_recovery_is_on_test(unit))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Error recovery is on before test.\n");
        }

        /** The max NOF FEC for a small consistent table is required */
        if (DNX_CTEST_ERR_REC_NOF_FEC_MEMBERS <
            dnx_data_l3.ecmp.consistent_tables_info_get(unit,
                                                        DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_SMALL_SIZE)->max_nof_unique_members)
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Make sure that DNX_CTEST_ERR_REC_NOF_FEC_MEMBERS is larger or equal to %d.\n",
                         dnx_data_l3.ecmp.consistent_tables_info_get(unit,
                                                                     DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_SMALL_SIZE)->max_nof_unique_members);
        }

        /*
         * fill array with FEC ids
         */
        SHR_IF_ERR_EXIT(dnx_algo_l3_fec_allocation_info_get(unit, 0, &fec, &fec_range_size));

        for (fec_iter = 0; fec_iter < DNX_CTEST_ERR_REC_NOF_FEC_MEMBERS; fec_iter++)
        {
            bcm_l3_egress_t_init(&l3eg);
            l3eg.intf = arp_id;
            l3egid = fec + fec_iter;
            SHR_IF_ERR_EXIT(bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY | BCM_L3_WITH_ID, &l3eg, &l3egid));
            fecs[fec_iter] = l3egid;
        }

        /*
         * When creating a consistent ECMP group with less members than the max_paths a unique resource is taken for only
         * this ECMP group and doesn't shared between the other resources.
         */
        for (ecmp_iter = 0;
             ecmp_iter <
             (L3_ECMP_TOTAL_NOF_CONSISTENT_RESOURCES(unit) /
              L3_ECMP_CONSISTENT_NOF_RESOURCES_TAKEN(unit, DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_SMALL_SIZE));
             ecmp_iter++)
        {
            bcm_l3_egress_ecmp_t_init(&ecmp);
            ecmp.max_paths =
                dnx_data_l3.ecmp.consistent_tables_info_get(unit,
                                                            DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_SMALL_SIZE)->max_nof_unique_members;
            ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT;
            ecmp.flags = BCM_L3_WITH_ID;
            ecmp.ecmp_group_flags = 0;
            ecmp.ecmp_intf = ecmp_count++;
            SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_create
                            (unit, &ecmp,
                             dnx_data_l3.ecmp.consistent_tables_info_get(unit,
                                                                         DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_SMALL_SIZE)->max_nof_unique_members
                             - 1, fecs));

        }

        /*
         * at this point the table with the FEC members is full
         */
        /*
         * creates a failing ECMP group, since it doesn't have enough space for all members
         */
        bcm_l3_egress_ecmp_t_init(&ecmp);
        ecmp.max_paths =
            dnx_data_l3.ecmp.consistent_tables_info_get(unit,
                                                        DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_SMALL_SIZE)->max_nof_unique_members;
        ecmp.dynamic_mode = BCM_L3_ECMP_DYNAMIC_MODE_RESILIENT;
        ecmp.flags = BCM_L3_WITH_ID;
        ecmp.ecmp_group_flags = 0;
        ecmp.ecmp_intf = ecmp_count;

        /*
         * Error recovery transaction here, expected to be rolled back on error
         */
        LOG_CLI((BSL_META("Expecting full ...\n")));

        SHR_IF_NOT_EXCEPT_ERR_EXIT(bcm_l3_egress_ecmp_create
                                   (unit, &ecmp,
                                    dnx_data_l3.ecmp.consistent_tables_info_get(unit,
                                                                                DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_SMALL_SIZE)->max_nof_unique_members
                                    - 1, fecs), _SHR_E_FULL);

        /*
         * if transaction was not successfully rolled-back, then ECMP profile already exists
         * therefore no check for FEC member table being full is carried out
         * which results in the API returning SOC_E_NONE, instead of SOC_E_FULL
         */
        LOG_CLI((BSL_META("Expecting full ...\n")));

        SHR_IF_NOT_EXCEPT_ERR_EXIT(bcm_l3_egress_ecmp_create
                                   (unit, &ecmp,
                                    dnx_data_l3.ecmp.consistent_tables_info_get(unit,
                                                                                DBAL_ENUM_FVAL_ECMP_CONSISTENT_TABLE_SMALL_SIZE)->max_nof_unique_members
                                    - 1, fecs), _SHR_E_FULL);

        /*
         * make sure that error recovery is off after the transaction
         */
        if (dnx_err_recovery_is_on_test(unit))
        {
            SHR_ERR_EXIT(_SHR_E_FAIL, "Failed to stop journaling.\n");
        }

        /*
         * cleanup
         */
        for (ecmp_iter = ecmp_start_id; ecmp_iter < ecmp_count; ecmp_iter++)
        {
            ecmp.ecmp_intf = ecmp_iter;
            SHR_IF_ERR_EXIT(bcm_l3_egress_ecmp_destroy(unit, &ecmp));
        }

        for (fec_iter = 0; fec_iter < DNX_CTEST_ERR_REC_NOF_FEC_MEMBERS; fec_iter++)
        {
            SHR_IF_ERR_EXIT(bcm_l3_egress_destroy(unit, fecs[fec_iter]));
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
        if (dnx_err_recovery_is_on_test(unit))
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
        if (dnx_err_recovery_is_on_test(unit))
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
        if (dnx_err_recovery_is_on_test(unit))
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

static sh_sand_ctest_doc_t err_recovery_basic_test_doc = {
    .priority = 1,
    .type = SHR_SAND_CTEST_TYPE_SEMANTIC,
    .procedures = "1. Basic testing of internal flags and suppression mechanism\n"
        "2. No support region mechanism testing\n" "3. Testing Error Recovery disabled switch control\n",
    .note = "Core of ER sanity"
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

static sh_sand_man_t err_recovery_thread_test_man = {
    "Error recovery, test multi-threading scenarios"
};
#endif

sh_sand_cmd_t dnx_err_recovery_test_cmds[] = {
    {"CuSToM", err_recovery_custom_test_cmd, NULL, NULL, &err_recovery_custom_test_man, NULL, NULL, CTEST_PRECOMMIT},
    {"DBaL", err_recovery_dbal_test_cmd, NULL, NULL, &err_recovery_dbal_test_man, NULL, NULL, CTEST_PRECOMMIT},
    {"flag", err_recovery_basic_test_cmd, NULL, NULL, &err_recovery_basic_test_man, NULL, NULL, CTEST_PRECOMMIT,.doc =
     &err_recovery_basic_test_doc},
    {"semantic", err_recovery_api_test_cmd, NULL, NULL, &err_recovery_api_test_man, NULL, NULL, CTEST_PRECOMMIT},
    {"state", err_recovery_snapshot_test_cmd, NULL, NULL, &err_recovery_snapshot_test_man, NULL, NULL, CTEST_PRECOMMIT},
    {"TCam", err_recovery_tcam_test_cmd, NULL, NULL, &err_recovery_tcam_test_man, NULL, NULL, CTEST_PRECOMMIT},
#ifdef DNX_ERR_RECOVERY_VALIDATION
    {"ReSTRiCT", err_recovery_restrict_test_cmd, NULL, NULL, &err_recovery_restrict_test_man, NULL, NULL,
     CTEST_PRECOMMIT},
    /*
     * introduces several seconds of delay, it is part of the postcommmit session 
     */
    {"THreaD", err_recovery_thread_test_cmd, NULL, NULL, &err_recovery_thread_test_man, NULL, NULL, CTEST_POSTCOMMIT},
#endif /* DNX_ERR_RECOVERY_VALIDATION */
    {NULL}
};
