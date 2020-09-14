/* \file ctest_dnx_kbp_combo.h
 *
 * Contains all KBP ACL combo test declarations for external usage
 */
/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef CTEST_DNX_KBP_COMBO_H_INCLUDED
#define CTEST_DNX_KBP_COMBO_H_INCLUDED

#define CTEST_DNX_KBP_COMBO_MAX_NOF_GROUPS      16
#define CTEST_DNX_KBP_COMBO_MAX_NOF_OPCODES     16
#define CTEST_DNX_KBP_COMBO_MAX_NOF_TESTS       16
#define CTEST_DNX_KBP_COMBO_MAX_NOF_ENTRIES     32

/*
 * Logging bitmaps
 * Used for enabling different log messages
 */
typedef enum
{
    DNX_KBP_COMBO_LOG_NONE = 0,
    DNX_KBP_COMBO_LOG_STDOUT = (1 << 0),
    DNX_KBP_COMBO_LOG_FILE = (1 << 1),
    DNX_KBP_COMBO_LOG_ERROR = (1 << 2),
    DNX_KBP_COMBO_LOG_INFO = (1 << 3),
    DNX_KBP_COMBO_LOG_GROUP = (1 << 4),
    DNX_KBP_COMBO_LOG_OPCODE = (1 << 5),
    DNX_KBP_COMBO_LOG_ENTRY = (1 << 6),
    DNX_KBP_COMBO_LOG_SEARCH = (1 << 7),
    DNX_KBP_COMBO_LOG_DEFAULT = DNX_KBP_COMBO_LOG_STDOUT | DNX_KBP_COMBO_LOG_ERROR | DNX_KBP_COMBO_LOG_INFO,
    DNX_KBP_COMBO_LOG_CONFIG = DNX_KBP_COMBO_LOG_STDOUT | DNX_KBP_COMBO_LOG_GROUP | DNX_KBP_COMBO_LOG_OPCODE,
    DNX_KBP_COMBO_LOG_EXECUTION = DNX_KBP_COMBO_LOG_STDOUT | DNX_KBP_COMBO_LOG_ENTRY | DNX_KBP_COMBO_LOG_SEARCH,
    DNX_KBP_COMBO_LOG_ALL =
        DNX_KBP_COMBO_LOG_STDOUT | DNX_KBP_COMBO_LOG_FILE | DNX_KBP_COMBO_LOG_ERROR | DNX_KBP_COMBO_LOG_INFO |
        DNX_KBP_COMBO_LOG_GROUP | DNX_KBP_COMBO_LOG_OPCODE | DNX_KBP_COMBO_LOG_ENTRY | DNX_KBP_COMBO_LOG_SEARCH
} dnx_kbp_combo_log_e;

/** Group status indication */
typedef enum
{
    DNX_KBP_COMBO_GROUP_STATUS_INVALID = 0,
    DNX_KBP_COMBO_GROUP_STATUS_VALID,
    DNX_KBP_COMBO_GROUP_STATUS_FAILED_XML_READ,
    DNX_KBP_COMBO_GROUP_STATUS_FAILED_DBAL_TABLE_CREATION,
    DNX_KBP_COMBO_GROUP_STATUS_FAILED_KBP_MNGR_DB_CONFIGURATION,
    DNX_KBP_COMBO_GROUP_STATUS_NOF
} dnx_kbp_combo_group_status_e;

/** Opcode status indication */
typedef enum
{
    DNX_KBP_COMBO_OPCODE_STATUS_INVALID = 0,
    DNX_KBP_COMBO_OPCODE_STATUS_VALID,
    DNX_KBP_COMBO_OPCODE_STATUS_FAILED_XML_READ,
    DNX_KBP_COMBO_OPCODE_STATUS_FAILED_KBP_MNGR_CREATION,
    DNX_KBP_COMBO_OPCODE_STATUS_FAILED_MASTER_KEY_OVERLAY_FIELDS_ADD,
    DNX_KBP_COMBO_OPCODE_STATUS_FAILED_MASTER_KEY_UPDATE,
    DNX_KBP_COMBO_OPCODE_STATUS_FAILED_LOOKUPS_ADD,
    DNX_KBP_COMBO_OPCODE_STATUS_NOF
} dnx_kbp_combo_opcode_status_e;

/** Test status indication */
typedef enum
{
    DNX_KBP_COMBO_TEST_STATUS_INVALID = 0,
    DNX_KBP_COMBO_TEST_STATUS_SKIPPED,
    DNX_KBP_COMBO_TEST_STATUS_IN_PROGRESS,
    DNX_KBP_COMBO_TEST_STATUS_FAILED_STEP_PARSING,
    DNX_KBP_COMBO_TEST_STATUS_FAILED_ENTRY_MANAGEMENT,
    DNX_KBP_COMBO_TEST_STATUS_FAILED_SEARCH,
    DNX_KBP_COMBO_TEST_STATUS_PASSED,
    DNX_KBP_COMBO_TEST_STATUS_NOF
} dnx_kbp_combo_test_status_e;

/** Environment status indication */
typedef enum
{
    DNX_KBP_COMBO_ENV_STATUS_INVALID = 0,
    DNX_KBP_COMBO_ENV_STATUS_INIT_SKIPPED,
    DNX_KBP_COMBO_ENV_STATUS_INIT_IN_PROGRESS,
    DNX_KBP_COMBO_ENV_STATUS_INIT_IN_PROGRESS_WITH_ERROR,
    DNX_KBP_COMBO_ENV_STATUS_SYNC_FAILED,
    DNX_KBP_COMBO_ENV_STATUS_INIT_DONE,
    DNX_KBP_COMBO_ENV_STATUS_INIT_DONE_WITH_ERROR,
    DNX_KBP_COMBO_ENV_STATUS_NOF
} dnx_kbp_combo_env_status_e;

/** Test action indication */
typedef enum
{
    DNX_KBP_COMBO_EXECUTE_ACTION_INVALID = 0,
    DNX_KBP_COMBO_EXECUTE_ACTION_ENTRY_ADD,
    DNX_KBP_COMBO_EXECUTE_ACTION_ENTRY_UPDATE,
    DNX_KBP_COMBO_EXECUTE_ACTION_ENTRY_GET,
    DNX_KBP_COMBO_EXECUTE_ACTION_ENTRY_DELETE,
    DNX_KBP_COMBO_EXECUTE_ACTION_SEARCH,
    DNX_KBP_COMBO_EXECUTE_ACTION_NOF
} dnx_kbp_combo_execute_action_e;

/** Indication for the type of "ENTRY_ADD/UPDATE" action */
typedef enum
{
    DNX_KBP_COMBO_ENTRY_ADD_EXPECTATION_INVALID = 0,
    DNX_KBP_COMBO_ENTRY_ADD_EXPECTATION_PASS,
    DNX_KBP_COMBO_ENTRY_ADD_EXPECTATION_ERROR,
    DNX_KBP_COMBO_ENTRY_ADD_EXPECTATION_EXISTS,
    DNX_KBP_COMBO_ENTRY_ADD_EXPECTATION_NOT_FOUND,
    DNX_KBP_COMBO_ENTRY_ADD_EXPECTATION_NOF
} dnx_kbp_combo_entry_add_expectation_e;

/** Indication for the type of "ENTRY_GET" action */
typedef enum
{
    DNX_KBP_COMBO_ENTRY_GET_EXPECTATION_INVALID = 0,
    DNX_KBP_COMBO_ENTRY_GET_EXPECTATION_FOUND,
    DNX_KBP_COMBO_ENTRY_GET_EXPECTATION_NOT_FOUND,
    DNX_KBP_COMBO_ENTRY_GET_EXPECTATION_VALIDATE,
    DNX_KBP_COMBO_ENTRY_GET_EXPECTATION_VALIDATE_ERROR,
    DNX_KBP_COMBO_ENTRY_GET_EXPECTATION_NOF
} dnx_kbp_combo_entry_get_expectation_e;

/** Indication for the type of "ENTRY_DELETE" action */
typedef enum
{
    DNX_KBP_COMBO_ENTRY_DELETE_EXPECTATION_INVALID = 0,
    DNX_KBP_COMBO_ENTRY_DELETE_EXPECTATION_PASS,
    DNX_KBP_COMBO_ENTRY_DELETE_EXPECTATION_ERROR,
    DNX_KBP_COMBO_ENTRY_DELETE_EXPECTATION_NOT_FOUND,
    DNX_KBP_COMBO_ENTRY_DELETE_EXPECTATION_NOF
} dnx_kbp_combo_entry_delete_expectation_e;

/** Indication for the type of actions related to entry fields */
typedef enum
{
    DNX_KBP_COMBO_ENTRY_FIELD_ADD = 0,
    DNX_KBP_COMBO_ENTRY_FIELD_VALIDATE,
    DNX_KBP_COMBO_ENTRY_FIELD_INFO,
    DNX_KBP_COMBO_ENTRY_FIELD_NOF
} dnx_kbp_combo_entry_field_e;

/** Indication for the test actions */
typedef enum
{
    DNX_KBP_COMBO_TEST_ACTION_EXECUTE = 0,
    DNX_KBP_COMBO_TEST_ACTION_DUMP_INFO,
    DNX_KBP_COMBO_TEST_ACTION_NOF
} dnx_kbp_combo_test_action_e;

/** Indication for the type of the input key values */
typedef enum
{
    DNX_KBP_COMBO_ENTRY_KEY_VALUE_TYPE_VALUE_ONLY = 0,
    DNX_KBP_COMBO_ENTRY_KEY_VALUE_TYPE_VALUE_AND_MASK,
    DNX_KBP_COMBO_ENTRY_KEY_VALUE_TYPE_RANGE,
    DNX_KBP_COMBO_ENTRY_KEY_VALUE_TYPE_NOF,
} dnx_kbp_combo_entry_key_value_type_e;

/** Indication for the type of field when parsing it */
typedef enum
{
    DNX_KBP_COMBO_FIELD_KEY = 0,
    DNX_KBP_COMBO_FIELD_RESULT,
    DNX_KBP_COMBO_FIELD_OVERLAY,
    DNX_KBP_COMBO_FIELD_NOF,
} dnx_kbp_combo_field_indication_e;

/**
 *  \brief
 *  Utility structure for holding information for a single group.
 */
typedef struct
{
    /** Status of the group */
    dnx_kbp_combo_group_status_e status;

    /** Group name */
    char name[DBAL_MAX_STRING_LENGTH];

    /** Output DBAL table ID returned after the table is created */
    dbal_tables_e dbal_table_id;

    /** DBAL table type */
    dbal_table_type_e dbal_table_type;

} dnx_kbp_combo_group_info_t;

/**
 *  \brief
 *  Utility structure for holding information for a single opcode.
 */
typedef struct
{
    /** Status of the opcode */
    dnx_kbp_combo_opcode_status_e status;

    /** Opcode name */
    char name[DBAL_MAX_STRING_LENGTH];

    /** Output KBP opcode ID returned after the opcode is created */
    uint8 opcode_id;

} dnx_kbp_combo_opcode_info_t;

/**
 *  \brief
 *  Utility structure for holding information for a single test.
 */
typedef struct
{
    /** The status of the test */
    dnx_kbp_combo_test_status_e status;

    /** Test name */
    char name[DBAL_MAX_STRING_LENGTH];

} dnx_kbp_combo_test_info_t;

/**
 *  \brief
 *  Utility structure for holding information for all groups, opcodes and tests
 *  in order to be presented in a visually pleasing way.
 */
typedef struct
{
    /** The environment status */
    dnx_kbp_combo_env_status_e status;

    /** The number of used groups */
    int nof_groups;

    /** Info on the status of groups and their configurations */
    dnx_kbp_combo_group_info_t group_info[CTEST_DNX_KBP_COMBO_MAX_NOF_GROUPS];

    /** The number of used opcodes */
    int nof_opcodes;

    /** Info on the status of opcodes and their configurations */
    dnx_kbp_combo_opcode_info_t opcode_info[CTEST_DNX_KBP_COMBO_MAX_NOF_OPCODES];

    /** The number of used tests */
    int nof_tests;

    /** Info on the status of tests and their configurations */
    dnx_kbp_combo_test_info_t test_info[CTEST_DNX_KBP_COMBO_MAX_NOF_TESTS];

} dnx_kbp_combo_env_info_t;

typedef struct
{
    void *groups_root;
    void *opcodes_root;
    void *entries_root;
    void *searches_root;
} dnx_kbp_combo_env_root_nodes_info_t;

shr_error_e dnx_kbp_combo_test(
    int unit,
    char *input_xml,
    char *output,
    int test_id,
    uint8 skip_config);

shr_error_e dnx_kbp_combo_info(
    int unit,
    char *input_xml,
    char *output,
    int test_id);

#endif /* CTEST_DNX_KBP_COMBO_H_INCLUDED */
