
/*! \file diag_dnx_dbal.h
 * Purpose: External declarations for command functions and
 *          their associated usage strings.
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef CTEST_DNX_DBAL_H_INCLUDED
#define CTEST_DNX_DBAL_H_INCLUDED

/*************
 * INCLUDES  *
 *************/
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dbal/dbal_structures.h>
#include "src/soc/dnx/dbal/dbal_internal.h"
#include "src/appl/diag/dnx/dbal/diag_dnx_dbal_internal.h"
#include <shared/utilex/utilex_framework.h>

/*************
 *  DEFINES  *
 *************/

/*************
 *  MACROES  *
 *************/

#define CTEST_DNX_DBAL_SH_CMD_EXEC(_cmd_) \
    sh_process_command(unit, _cmd_)

/*************
 * GLOBALS   *
 *************/
extern sh_sand_cmd_t dnx_dbal_test_cmds[];
extern const char *strcaseindex(
    const char *s,
    const char *sub);
/*
 * enum describing the available DBAL UT
 */
typedef enum
{
    UPDATE_BEFORE_COMMIT = 0,
    HANDLE_OPERATIONS,
    PARTIAL_ACCESS,
    GROUP_VALIDATIONS,
    SUB_FIELD,
    MULTIPLE_INSTANCES,
    RT_ACCESS,
    WRONG_FIELD_SIZE,
    WRONG_FIELD_ACCESS,
    ENUM_MAPPING,
    ENUM_MAPPING_MUL_DEVICE,
    RANGE_OF_ENTRIES_SET,
    SUPERSET_RES_TYPE,
    RANGE_OF_ENTRIES_CLEAR,
    RANGE_OF_ENTRIES_DOUBLE,
    ALL_INSTANCES,
    UPDATE_HANDLE_WITH_BUFFERS,
    CONST_VALUE,
    DPC_CORE_ANY,
    DPC_CORE_ALL,
    UINT64_FIELDS,
    BOTH_CORES,
    MAC_ENCODING,
    PREDEFINED_FIELD_VAL,
    STRUCT_FIELD_ENCODE_DECODE,
    DYNAMIC_TABLE_VALIDATION,
    DYNAMIC_RES_TYPE,
    ITERATOR_WITH_KEY_RULES,
    ITERATOR_WITH_VALUE_RULES,
    ITERATOR_WITH_RULES_ACTIONS,
    ITERATOR_MULTIPLE_RT,
    ITERATOR_MULTIPLE_RT_MCDB,
    ITERATOR_HIT_BIT_RULE,
    ITERATOR_AGE_RULE,
    ITERATOR_HIT_BIT_RETRIEVED,
    ITERATOR_HIT_BIT_OVER_TABLES,
    TABLE_HITBIT,
    TCAM_CS,
    TCAM_MDB,
    TCAM_KBP,
    COMMIT_FORCED,
    TCAM_BASIC,
    ERR_CODE_NON_DIRECT,
    VALID_IND,
    TABLE_RESTORE,
    HL_TCAM,
    LPM_MASK,
    ENTRIES_COUNTER,
    MERGE_ENTRIES,
    FIELD_ALLOCATOR,
    READONLY_FIELD,
    WRITEONLY_FIELD,
    TRIGGER_FIELD,
    ARR_PREFIX,
    STRUCT_ARR_PREFIX,
    MACT_ITERATOR,
    MACT_ITERATOR_KEY_ONLY,
    MACT_ITERATOR_KEY_DEST_ONLY,
    ITERATOR_RULES_ACTIONS_MACT,
    ITERATOR_RULES_ACTIONS_GET_MACT,
    DEFAULT_VALUE_TEST,
    DISABLED_RESULT_TYPE,
    UPDATE_RESULT_TYPE,
    NONE_DIRECT_MODE,
    TABLE_PROTECTION,
    TABLE_NO_VALIDATIONS,
    COMMIT_HW_ONLY,
    COLLECT_MODE_PERF,
    NON_ALIGNED_DATA_MASK,
    ASYMMETRIC_TABLE,
    MULTI_CORE_ITER,
    ENUM_CHILD,
    DBAL_UT_NOF_TESTS,
} dbal_ut_tests_e;

typedef enum
{
    /*
     * test is supported in all the devices 
     */
    UT_ALL_DEVICES = SAL_BIT(1),

    /*
     * test should be run only in dual core devices 
     */
    UT_DUAL_CORE_DEVICES = SAL_BIT(2),

    UT_QUAD_READY = SAL_BIT(3)
} dbal_ut_supported_device_type_e;

#define DBAL_MAX_NUM_OF_TABLES_PER_TEST                     6

#define DBAL_UT_FILL_TABLE_FOR_5                            DBAL_TABLE_EMPTY
#define DBAL_UT_FILL_TABLE_FOR_4                            DBAL_TABLE_EMPTY, DBAL_UT_FILL_TABLE_FOR_5
#define DBAL_UT_FILL_TABLE_FOR_3                            DBAL_TABLE_EMPTY, DBAL_UT_FILL_TABLE_FOR_4
#define DBAL_UT_FILL_TABLE_FOR_2                            DBAL_TABLE_EMPTY, DBAL_UT_FILL_TABLE_FOR_3
#define DBAL_UT_FILL_TABLE_FOR_1                            DBAL_TABLE_EMPTY, DBAL_UT_FILL_TABLE_FOR_2
#define DBAL_UT_NO_TABLE                                    DBAL_TABLE_EMPTY, DBAL_UT_FILL_TABLE_FOR_1

typedef shr_error_e(
    *DBAL_UT_FUNCTION) (
    int unit,
    dbal_tables_e table_id);

typedef struct
{
    char test_name[DBAL_MAX_STRING_LENGTH];
    dbal_ut_tests_e test_idx;
    uint32 ctest_flags;
    DBAL_UT_FUNCTION test_cb;
    dbal_ut_supported_device_type_e device_type;
    dbal_tables_e table_ids[DBAL_MAX_NUM_OF_TABLES_PER_TEST];

} dbal_ut_test_info_t;

/*************
 * FUNCTIONS *
 *************/
/**
 * \brief
 * This test sets the table key & payload fields number of times
 * before commit.  read the all table and verify all payload
 * fields have the correct value.
 */
shr_error_e dnx_dbal_update_field_before_commit(
    int unit,
    dbal_tables_e table_id,
    uint8 is_rand);

/**
 * \brief
 * This test validate field_unset API and DBAL_HANDLE_COPY macro.
 * it sets the table key & payload fields then unset one field.  read the all entry
 * and verify all payload fields have the correct value.
 */
shr_error_e dnx_dbal_ut_handle_operations(
    int unit,
    dbal_tables_e table_id);

shr_error_e dnx_dbal_ut_update_field_before_commit(
    int unit,
    dbal_tables_e table_id);

extern cmd_result_t diag_dbal_alloc_dealloc_value(
    int unit,
    dbal_fields_e field_id,
    dbal_tables_e table_id,
    int result_type,
    int core_id,
    int element,
    int is_alloc);

/*************
 * UNIT TESTS APIs *
 *************/

shr_error_e dnx_dbal_ut_partial_access(
    int unit,
    dbal_tables_e table_id);

/**
 *  \brief
 *  Validates that in all hard logic tables there is no hw entity that defined in group and as direct mapping. this is
 *  not allowed since the group takes unders considerations that all elements in the group are identical with there
 *  values
 */

shr_error_e dnx_dbal_ut_group_definition_validate(
    int unit,
    dbal_tables_e table_id);

/**
 *  \brief
 *  HL table definition related validations such as pernission of fields..
 */
shr_error_e dnx_dbal_ut_hl_validations(
    int unit,
    dbal_tables_e table_id);

/**
 *  \brief
 *  HL table definition related validations for hw fields.
 */
shr_error_e dnx_dbal_ut_hl_validations_hw_entity(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief
 * This test verify the sub fields functionality.
 * it access table with DESTINATION field and access it in
 * different ways using different sub fields.
 */
shr_error_e dnx_dbal_ut_parent_field_mapping(
    int unit,
    dbal_tables_e table_id);


/**
 * \brief
 * This test write a multiple instances field and read & compare
 * the results. then write & update a multiple instances field
 * and read & compare the results.
 */
shr_error_e dnx_dbal_ut_multiple_instance_field(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief
 * This test verify the result type table functionality.
 * it access table with multiple result types defined and access
 * it in different ways using different result type values.
 */
shr_error_e dnx_dbal_ut_rt_access(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief
 * This test runs ltt and ltt_full tests, with
 * no validation on commit/field set
 */
shr_error_e dnx_dbal_ut_table_no_validations(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief
 * This test checks the flag DBAL_COMMIT_HW_ONLY
 */
shr_error_e dnx_dbal_ut_commit_hw_only(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief
 * This test checks performance in collect mode */
shr_error_e dnx_dbal_ut_collect_mode_perf(
    int unit,
    dbal_tables_e table_id);


/**
 * \brief
 * This is a negative test. This test set the key fields of the
 * table correctly. * For payload fields it sets the fields with
 * MAX values in length of field_nof_bits+2. then commit the
 * changes, this test should FAIL.
 */
shr_error_e dnx_dbal_wrong_field_size(
    int unit,
    dbal_tables_e table_id,
    uint8 is_rand);

shr_error_e dnx_dbal_ut_wrong_field_size(
    int unit,
    dbal_tables_e table_id);

shr_error_e dnx_dbal_ut_wrong_field_access(
    int unit,
    dbal_tables_e table_id);
/**
 * \brief
 * This is a negative test. This test set the key fields of the
 * table correctly. For payload fields it sets the fields with
 * MAX values in length of  field_nof_bits+2. then commit the
 * changes, this test should FAIL.
 * *******************************************************
 */
shr_error_e dnx_dbal_wrong_field_access(
    int unit,
    dbal_tables_e table_id,
    uint8 is_rand);

/**
 * \brief
 * This test verify the enum mapping fields functionality.
 * it access table with ENUM_TEST field and access it in
 * different ways using different enum values including wrong
 * values.
 */
shr_error_e dnx_dbal_ut_enum_mapping(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief
 * This test verify the enum mapping fields functionality in different devices.
 * for different devices the enum mapping could be changed, some of the values might be invalid
 * this test access table with such ENUM field and access it in
 * different ways using different enum values including wrong
 * values.
 */
shr_error_e dnx_dbal_ut_enum_mapping_multiple_device(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief Test the feature setting range of entries in a table and validate that all range is set.
 *   API tested dbal_entry_key_field32_range_set(), dbal_entry_key_field16_range_set, dbal_entry_key_field8_range_set
 * flow:
 * (1)setting all range with value 0x1, validating
 * (2)setting partial range with  value 0xf, validating all range
 * (3)clearing table
 * (4)setting range for two fields with value 0xd, validating
 * (5)clearing table
 *
 */
shr_error_e dnx_dbal_ut_range_of_entries_set(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief Test the feature test the clearing of range of entries
 * flow:
 * (1)setting all range with value 0x1, validating clearing and validating
 * (2)setting partial range with  value 0xf, clearing and validating all range
 * (3)clearing table
 * (4)setting range for two fields with value 0xd, clearing part of the range and validating
 * (5)clearing the rest of the range and validating
 *
 */

shr_error_e dnx_dbal_ut_range_of_entries_clear(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief Test dynamic result type feature
 * flow:
 * (1) setting result type to be superset result type
 * (2) setting all fields in the table
 * (3) negative test - committing the entry
 * (4) getting the value fields after set
 * (5) getting the key fields after set
 *
 */
shr_error_e dnx_dbal_ut_superset_res_type(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief
 * This test checks to consecutive calls to range set
 */
shr_error_e dnx_dbal_ut_range_of_entries_double(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief
 * This test set field with multiple instances using the
 * INST_ALL flag. than check all instances values
 */
shr_error_e dnx_dbal_ut_all_instances_set(
    int unit,
    dbal_tables_e table_id);
/**
 * \brief
 * This test validate the functionality of dbal_entry_handle_key_payload_update(). once calling this API the dbal handle
 * buffers should be updated and also all the handle parameters. this test compare the handle after calling
 * dbal_entry_handle_key_payload_update and a regular handle.
 */
shr_error_e dnx_dbal_ut_update_handle_with_buffers(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief
 * Checks asymmetric table functionality
 */
shr_error_e dnx_dbal_ut_asymmetric_table(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief
 * Check MDB iterator can retrieve all the entries
 * no matter which cores are populated
 */
shr_error_e dnx_dbal_ut_multi_core_iterator(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief
 * This test validated the behavior of fields with const values
 */
shr_error_e dnx_dbal_ut_const_values(
    int unit,
    dbal_tables_e table_id);
/**
 * \brief
 * This test Set and get entries from DPC table using
 * CORE_ANY/CORE_ALL values
 */
shr_error_e dnx_dbal_ut_core_any_dpc(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief
 * This test Set and get entries from DPC table using
 * CORE_ANY/CORE_ALL values
 */
shr_error_e dnx_dbal_ut_core_all_dpc(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief
 * This test verify the enum mapping fields functionality.
 * it access table with ENUM_TEST field and access it in
 * different ways using different enum values including wrong
 * values.
 */
shr_error_e dnx_dbal_ut_uint64_fields(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief for SBC tables validate that both cores are written by using flag DBAL_COMMIT_VALIDATE_OTHER_CORE
 * when getting an entry.
 */
shr_error_e dnx_dbal_ut_both_cores(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief for SBC tables validate that both cores are written by using flag DBAL_COMMIT_VALIDATE_OTHER_CORE
 * when getting an entry.
 */
shr_error_e dnx_dbal_ut_mac_encoding(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief use to validate the max value / reset value setting for field by using the
 *  dbal_entry_key_field_predefine_value_set(), dbal_entry_value_field_predefine_value_set
 */
shr_error_e dnx_dbal_ut_predefine_value(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief
 * Run Struct field encode and decode validation test
 * 1. encode a struct field
 * 2. compare to the expected value
 */
shr_error_e dnx_dbal_ut_struct_field_encode_decode(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief this test validates ACL entry management for KBP.
 *
 */
shr_error_e dnx_dbal_ut_kbp_tcam(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief this test validate dynamic table operations, it creates and destroy tables, adding entries to created tables.
 *
 */
shr_error_e dnx_dbal_ut_dynamic_table_validations(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief
 * Run iterator over multiple result types test
 * 1. Iterate over a multiple results table's results
 * 2. compare to the expected value
 */
shr_error_e dnx_dbal_ut_iterator_mul_result(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief
 * Run iterator multiple entries, part with hit bit set and part without
 * 1. Validate hit bit=0 rule
 * 2. Validate hit bit=1 rule
 */
shr_error_e dnx_dbal_ut_iterator_hit_bit_rule(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief
 * Run LPM HitBit support test for enable and get of HitBit in SW_state
 */
shr_error_e dnx_dbal_ut_table_hitbit(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief
 * Run iterator multiple entries, with different AGE value
 * counte entries with AGE rule
 */
shr_error_e dnx_dbal_ut_iterator_age_rule(
    int unit,
    dbal_tables_e table_id);

shr_error_e dnx_dbal_ut_no_payload(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief
 * Perform simple set get with masked APIs in TCAM CS table
 */
shr_error_e dnx_dbal_ut_tcam_cs_validations(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief
 * In case a field is defined with some offset, check the mask
 * has the same offset
 */
shr_error_e dnx_dbal_ut_non_aligned_data_mask_validation(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief
 * Perform simple set get with masked APIs in an MDB TCAM table
 */
shr_error_e dnx_dbal_ut_mdb_tcam_validations(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief
 * Check functionality of commit forced feature
 * Run this test on a table that has more than one result field,
 * to get better verification (for MDB-LPM currently no such
 * table). First result field expected to be at least 4 bits.
 */
shr_error_e dnx_dbal_ut_commit_forced(
    int unit,
    dbal_tables_e table_id);

/** \brief this test checks basic scenario for TCAM entries using XXX_id DBAL APIs */
shr_error_e dnx_dbal_ut_tcam_table_test(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief
 * Verify the error code of entry_get, entry_clear,
 * commit_update functions
 */
shr_error_e dnx_dbal_ut_non_direct_error_codes(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief
 * This test is currently not used!!
 * Verify DBAL functionality after dynamically setting valid
 * indication
 */
shr_error_e dnx_dbal_ut_field_valid_indication(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief
 * This test is currently not used!!
 * Verify DBAL functionality after dynamically setting valid
 * indication
 */
shr_error_e dnx_dbal_ut_table_restore(
    int unit,
    dbal_tables_e table_id);

/**
 *  \brief Check functionality of access for HL TCAM
 */
shr_error_e dnx_dbal_ut_hl_tcam(
    int unit,
    dbal_tables_e table_id);

/**
 *  \brief Check functionality of mask validation for LPM
 *  Table must have at least 2 key fields
 */
shr_error_e dnx_dbal_ut_lpm_mask(
    int unit,
    dbal_tables_e table_id);

/**
 *  \brief Check Entries counter (for ant table type)
 *  For Direct and TCAM - (nof_entries==0) at any step
 *  For LPM and EM - nof_entries expected to change according to
 *  action taken
 */
shr_error_e dnx_dbal_ut_entries_counter(
    int unit,
    dbal_tables_e table_id);

/**
 *  \brief TBD
 */
shr_error_e dnx_dbal_ut_merge_entries(
    int unit,
    dbal_tables_e table_id);
/**
 * \brief
 * Run Dynamic result type validation test
 * 1. Create a dynamic result type in PPMC table.
 * 2. Validate that new result properties in table
 * 3. MAking some semantic commands with new result
 */
shr_error_e dnx_dbal_ut_dynamic_res_type(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief
 * For all kind of access methods, run iterator with key rules and validate the entries counter that found.
 * For each table:
 *  0. Clear table.
 *  1. Add 3 entries.
 *  2. Run iterator with rule - validate nof found entries.
 *  3. Run iterator with two rules - validate nof found entries.
 */
shr_error_e dnx_dbal_ut_iterator_with_key_rules(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief
 * For all kind of access methods, run iterator with value rules and validate the entries counter that found.
 * For each table:
 *  0. Clear table.
 *  1. Add 3 entries.
 *  2. Run iterator with rule - validate nof found entries.
 *  3. Run iterator with two rules - validate nof found entries.
 */
shr_error_e dnx_dbal_ut_iterator_with_value_rules(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief
 * For all kind of access methods, run iterator with rules and actions and validate the entries counter that found.
 * For each table:
 *  0. Clear table.
 *  1. Add 3 entries.
 *  2. Run iterator with rule - validate nof found entries.
 *  3. Run iterator with two rules - validate nof found entries.
 */
shr_error_e dnx_dbal_ut_iterator_with_rules_actions(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief
 * Run iterator with rules and actions and validate
 * Steps:
 *  0. Clear table.
 *  1. Add 6 entries.
 *  2. Check entry is in Shadow
 *  3. Run iterator with rule - delete entry (flush compatible).
 *  4. Check the deleted entry is deleted from shadow too
 *  5. Run iterator with 2 fields rule - delete entry (non flush compatible [FC]).
 *  6. Update value field by rule (for entry present in 2 RTs, one FC and one non FC)
 *
 */
shr_error_e dnx_dbal_ut_iterator_rules_actions_mact(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief
 * Run iterator with rules to do update and get on the fly
 */
shr_error_e dnx_dbal_ut_iterator_update_get_mact(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief this test perform negative tests for field
 * allocator.try to commit / get entries when the resource was not allocated. also validating iterator.
 */
shr_error_e dnx_dbal_ut_field_allocator(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief this test perform negative tests for field
 * try to write on field which are read-only
 */
shr_error_e dnx_dbal_ut_readonly_field(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief this test perform negative tests for field
 * try to read on field which are not readable: write-only or
 * trigger
 */
shr_error_e dnx_dbal_ut_read_not_readable_field(
    int unit,
    dbal_tables_e table_id);

shr_error_e dnx_dbal_ut_default_values(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief this test perform iterator actions on MACT.
 * It mixes dynamic with static entries and check.
 */
shr_error_e dnx_dbal_ut_arr_prefix(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief this test perform encod/ decode of arr prefix in struct field.
 */
shr_error_e dnx_dbal_ut_struct_arr_prefix(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief this test perform iterator actions on MACT.
 * It mixes dynamic with static entries and check.
 */
shr_error_e dnx_dbal_ut_mact_iterator(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief this test perform iterator actions on MACT.
 * It validates the correctness of key rule only, when a single rule is set (in flush) to handle all kind of entries
 * On chip, with flush machine, it should run with bulk mode, to validate we can add MAX_NOF_RULS rule for keys
 */
shr_error_e dnx_dbal_ut_mact_iterator_key_rule_only(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief this test perform iterator actions on MACT.
 * It validates the correctness of key rule and result fields, that their location on entry is common for all result types -
 * In that case, a single rule is set per entry size in MDB.
 * The location of the result on the entry is depended on MDB entry size.
 * On chip, with flush machine, it should run with bulk mode, to validate we can add MAX_NOF_RULS rule for keys
 */
shr_error_e dnx_dbal_ut_mact_iterator_key_and_dest(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief Negative test. Check that we can't set a disabled
 *  result type.
 *  In table EXAMPLE_TABLE_FOR_HL_WITH_MUL_SW_FIELD,
 *  result type RT0 is disabled for j2a0 */
shr_error_e dnx_dbal_ut_disabled_result_type(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief Entry result type update test
 *  In table DBAL_TABLE_ING_VSI_INFO_DB */
shr_error_e dnx_dbal_ut_update_result_type(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief Validate the dbal iterator hit-bit retrieved value
 * in table MPLS_TERMINATION_SINGLE_LABEL_DB
 */
shr_error_e dnx_dbal_ut_iterator_hitbit_retrieved(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief
 *    This test checks hitbit is retrieved even in case it is
 *    on in the second physical table
 */
shr_error_e dnx_dbal_ut_iterator_hitbit_over_tables(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief validate none_direct_mode - checks that optimze is working correctly
 */
shr_error_e dnx_dbal_ut_none_direct_mode(
    int unit,
    dbal_tables_e table_id);

/**
 * \brief validate none_direct_mode - checks that optimize is working correctly
 */
shr_error_e dnx_dbal_ut_table_protection_validate(
    int unit,
    dbal_tables_e table_id);

#endif /* CTEST_DNX_DBAL_H_INCLUDED */
