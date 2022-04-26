/*
 * ! \file kaps_verifier_xml_parse.h
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef __KAPS_VERIFIER_XML_PARSE_H_INCLUDED
#define __KAPS_VERIFIER_XML_PARSE_H_INCLUDED

#include "kaps_errors.h"
#include "kaps_device.h"
#include "kaps_key.h"
#include "kaps_sw_model.h"
#include "kaps_init.h"

#include "kaps_verifier_parse.h"

struct kaps_device;
struct kaps_acl_db;
struct kaps_lpm_db;
struct kaps_em_db;
struct kaps_ad_db;

/**
 *
 * @file kaps_verifier_xml_parse.h
 *
 * Specifies search scenarios using XML.
 *
 * @addtogroup XML_PARSE_APIS
 * @{
 */

/**
 * The following data structure represents a single
 * database definition, as parsed by the XML parser.
 */

struct distr_info
{
    uint32 max_pfx_len;         /* max_prefix length: max 160, the arr_size= max 161 */
    uint32 count[KAPS_LPM_KEY_MAX_WIDTH_1 + 1]; /* number of prefixes of length i, 0-160 */
};

/**
 *  xml override options
 */

struct xml_override
{
    uint8 struct_valid;
    uint8 capacity_valid;
    uint8 mode_valid;
    uint8 algo_valid;
    uint8 force_to_sram_valid;
    uint8 est_min_capacity_valid;
    uint8 parse_only_valid;
    uint8 del_percent_valid;

    uint32 capacity;
    uint8 id;
    uint8 mode;                 /* batch = 0, incremental = 1; */
    uint8 algo;
    uint8 force_to_sram;
    uint32 est_min_capacity;
    uint32 parse_only;
    uint8 del_percent;
};

/* One instance of this structure contains information about one database specified in the XML */
struct xml_parse_info
{
    enum kaps_db_type type; /**< Type of database, whether it is ACL, LPM, EM, AD or TAP. */
    union
    {
        struct kaps_db *db;         /**< Structure for ACL/LPM/EM database. */
        struct kaps_ad_db *ad_db;   /**< Structure for AD database. */
        struct kaps_stats_db *stats_db;   /**< Structure for stats database. */
        struct kaps_tap_db *tap_db;   /**< Structure for stats database. */
        struct kaps_dma_db *dma_db; /**< Structure for DMA database. */
    } db; /**< Depending on database type, the actual database pointer. */

    char **inputs;                 /**< The array of input seed-file names. */
    struct xml_parse_info *clone;  /**< Clone of this database/table is present */
    struct xml_parse_info *next_tab; /**< List of subtables, if present. */
    struct kaps_parse_key *db_key;  /**< Database key as specified by the user. */
    struct kaps_parse_ad_info *ad_info; /**< Linked List of AD database information */
    struct xml_parse_hb_db_info *hb_db_info; /**< HitBit Database Information */
    struct xml_tap_db_info *tap_db_info; /**< TAP database information */
    struct xml_parse_info *next;   /**< Linked list of parsed information. */
    struct xml_parse_default_entries *default_entries; /**< Default Prefixes */
    struct kaps_db **bc_dbs;        /**< Array of bc databases */
    uint32 has_counters;          /**< This database has Counters (SnC) */
    uint32 index_range_min;      /**< For LPM, to control hit-index range. */
    uint32 index_range_max;      /**< For LPM, to control hit-index range. */
    uint32 line_no;              /**< Line number in XML file. */
    uint16 id;           /**< ID specified in the XML file. */
    uint16 width_1;              /**< Original width in bits of the entries in the DB. */
    uint16 nad_dbs;               /**< Number of AD Databases this DB as */
    uint16 set_num_ab;           /**< Set DBA resource in number of ABs. */
    uint16 set_num_ab_core0;           /**< Set DBA resource in number of ABs. */
    uint16 set_num_ab_core1;           /**< Set DBA resource in number of ABs. */
    uint16 set_uda_mb;           /**< Set UDA resource in MB. */
    uint16 set_uda_mb_core0;           /**< Set UDA resource in MB. */
    uint16 set_uda_mb_core1;           /**< Set UDA resource in MB. */
    uint8 db_default_priority;   /**< OP LPM Specific for db entry priority */
    uint8 container_db;         /**< This is just a container database that has subtables. */
    uint8 algorithmic;          /**< Massively parallel, power controlled, or advanced power controlled. */
    uint8 num_inputs;           /**< Number of input files, if specified in XML. */
    uint8 ninstructions;        /**< Number of instruction this DB participates in */
    uint8 table_mix_percent;    /**< Percentage of distribution of  database */
    uint8 tbl_mix_percent_low;  /**< Percentage distribution low */
    uint8 tbl_mix_percent_high; /**< Percentage distribution high */
    uint8 incremental;          /**< If attribute set for database. */
    int32 parse_upto;           /**< if capacity is 0, parse only these many, else till the file EOF */
    uint32 est_capacity_per_mb; /**< Estimated capacity of the database for 1 Mb of BB*/
    uint32 est_min_capacity;    /**< Minimum capacity estimated for the database */
    uint32 baseline_capacity;   /**< Baseline capacity established for this database in the XML file */
    struct distr_info pfx_disr;   /**< prefixes distribution array, holds num_elmts, len:count */
    struct xml_override *ovride;  /**< XML Override values  */

    /*
     * copied from parser 
     */
    uint32 user_cap_min;
    uint32 user_cap_max;
    uint32 user_ops_min;
    uint32 user_ops_max;
};

/**
 * Parsed Default Prefixes
 */

struct xml_parse_default_entries
{
    uint8 priority;                   /**< Defaut Prefixes Priority */
    struct kaps_parse_record *entries;  /**< Prefixes array */
    uint32 nentries;                 /**< Size of the above array */
    struct xml_parse_default_entries *next; /**< Linked list of default entries */
};

/**
 * Parsed Hit Bit Database Infomation
 */

struct xml_parse_hb_db_info
{
    struct kaps_hb_db *hb_db; /**< Valid HB Database pointer */
    uint32 capacity;       /**< Capacity of the Database */
    uint32 age;            /**< Aging Factor for the database */
};

/**
 * Parsed KPU and result information for a database.
 */

struct xml_inst_desc
{
    uint8 result_id;              /**< Result number. */
    struct xml_parse_info *db_info; /**< Database being searched. */
};

/**
 * This data structure represents a single
 * instruction definition, as parsed by the XML parser.
 */

struct xml_inst_info
{
    struct kaps_instruction *instruction;    /**< Instruction handle. */
    struct xml_inst_desc desc[KAPS_HW_MAX_SEARCH_DB]; /**< Description of the searches in an instruction. */
    struct xml_inst_info *next;             /**< Linked list of instructions. */
    struct kaps_parse_key *master_key;       /**< Master key used for searching. */
    uint32 *conflict_ids;                 /**< Array of conflict ids */
    uint32 num_searches:4;                /**< Number of tables being searched in the instruction. */
    uint32 master_key_width_1:12;         /**< Width of the master key in bits. */
    uint32 type:4;                        /**< ::instruction_type */
    uint32 num_conflicts:8;               /**< size of the below array */
    uint32 bc_dev_no:8;                   /**< Broadcast device no where this instruction belongs to */
    uint32 context_address:24;            /**< Context buffer Address */
    uint32 thread_id:2;                   /**< SMT thread id of this instruction, for op2 no of threads can be 4 */
    uint32 has_hitbit_db:1;               /**< Set to 1 if at least one of the dbs has Hitbits */
};

/**
 * Various operations performed by the test harness
 */

enum test_operation
{
    TEST_PIO,                   /**< Allows some PIO operations to go through */
    TEST_ADD,                   /**< Add an entry to randomly selected database */
    TEST_DELETE,                /**< Delete an entry to randomly selected database */
    TEST_HEAVY_SEARCH_DELETE,   /**< Delete an substantial part entries from a randomly selected database */
    TEST_INSTALL,               /**< Install entries in a randomly selected database */
    TEST_SEARCH,                /**< Search on a random instruction */
    TEST_ITER,                  /**< Iterate the entries */
    TEST_ENTRY,                 /**< Perform random operations on an entry */
    TEST_UPDATE_AD,             /**< Update associated data */
    TEST_WARMBOOT,              /**< Warmboot Save & Restore operation */
    TEST_SHRINK,                /**< Shrink a DB */
    TEST_CRASH_RECVRY,          /**< Enable Crash Recovery */
    TEST_ISSU,                  /**< Perform random ISSU operations*/
    TEST_COUNTERS,              /**< Perform counter verification */
    TEST_HW_RANDOM_SEARCH,      /**< Perform random search on the HW */
    TEST_INJ_AND_FIX_ERROR,     /**< Perform injecting parity error, and fix it */
    TEST_ADD_AND_INSTALL        /**< Add and Install in a single operation*/
};

/**
 * Various target types (terminationg conditions) of a test step
 */
enum test_step_target_type
{
    STEP_TARGET_ITER,
    STEP_TARGET_DB_FULL,
    STEP_TARGET_NUM_ENTRIES,
    STEP_TARGET_DELETE_ENTRIES,
    STEP_TARGET_REPEAT_STEPS
};

/**
 * test step target element iteration count
 */

struct target_element_iter
{
    int32 iter_count;
};

/**
 * test step target element db_full
 */
struct target_element_db_full
{
    int8 db_id;
};

/**
 * test step target element num_entries
 */
struct target_element_num_entries
{
    int8 db_id;
    int32 num_entries;
};

/**
 * test step target element num_entries
 */
struct target_element_delete_entries
{
    int8 db_id;
    int32 target_ndeletes;
    int8 delete_percentage;
};

struct target_element_repeat_steps
{
    int32 step_no;
    int32 num_repeat;
};

/**
 * test step target element, common wrapper
 */
struct test_step_target_element
{
    enum test_step_target_type target_type;
    struct target_element_iter element_iter;
    struct target_element_db_full element_db_full;
    struct target_element_num_entries element_num_entries;
    struct target_element_delete_entries element_delete_entries;
    struct target_element_repeat_steps element_repeat_steps;
};

/**
 * test step target element expression type
 */
enum test_step_target_element_expression_type
{
    LOGICAL_NONE,
    LOGICAL_OR,
    LOGICAL_AND
};

/**
 * test step target. it's a logical_or/logical_and expression of different target elements
 */
struct test_step_target_expression
{
    int8 num_target_element;
    struct test_step_target_element target_elements[50];
    enum test_step_target_element_expression_type expression_type;
};

/**
 * one single operation
 */
struct test_step_operation
{
    enum test_operation operation_type;
    int8 db_instr_id[100];
    int8 num_ids;
    int32 bias;
    int32 is_sub_op_failed[100];
    int32 is_whole_op_failed;
};

/**
 * list of operations performed in a test step and their bias
 */
struct test_step_operation_list
{
    int8 num_operation;
    struct test_step_operation step_operation[100];
    int32 total_bias;
};

enum test_step_type
{
    STEP_REGULAR,
    STEP_SPECIAL
};

enum special_step_operation_type
{
    SPECIAL_STEP_OPERATION_TYPE_WARMBOOT,
    SPECIAL_STEP_OPERATION_TYPE_INSTALL,
    SPECIAL_STEP_OPERATION_TYPE_CRASH_RECOVERY
};

struct special_step_operation_data
{
    int32 no_of_db;
    int32 db_ids[100];
    int32 db_full[100];
    int32 is_full_op_fail;
};

struct special_step_operation
{
    enum special_step_operation_type op_type;
    struct special_step_operation_data op_data;
};

struct test_step_special
{
    struct special_step_operation step_operations[50];
    int32 num_operations;
};

/**
 * one single test step
 */
struct test_step
{
    enum test_step_type step_type;
    struct test_step_special special_step;
    struct test_step_target_expression target_expression;
    struct test_step_operation_list operation_list;
};

/**
 * the dynamic core loop test scenario
 */
struct test_scenario
{
    struct test_step *test_steps;
    int32 num_test_steps;
    int32 curr_executing_step_no;
    char *all_step_file_name;
    FILE *all_steps_file;  /**< Dynamic Test all_step Output file pointer */
};

#define MAX_OVERRIDE_DBS (16)

/**
 * Parses the search scenarios specified in the XML file.
 *
 * @param fname The XML file name.
 * @param device The valid KBP device handle from which all databases are created.
 * @param skip_clones skip parsing of database clones
 * @param parse_info A linked list of parsed information structures returned by the API.
 * @param inst_info A linked list of parsed instruction structures returned by API.
 *
 * The function may return an error code, if the the databases
 * cannot be accommodated on the device or for any other failure
 * reasons. However, the list of parsed_info structures is still
 * returned. As long as the pointer is not NULL, it can be used
 * by the caller for any purposes.
 *
 * It is the responsibility of the caller to
 * to free up the parse_info list using the API xml_parse_destroy().
 *
 * @retval KAPS_OK on success.
 * @retval KAPS_INVALID_ARGUMENT on errors with arguments.
 * @retval KAPS_OUT_OF_MEMORY on running out of heap memory.
 * @retval KAPS_INTERNAL_ERROR on unknown error condition that cannot be handled.
 * @retval KAPS_PARSE_ERROR on errors with XML formats.
 *
 * The API may also return any of the error codes returned by
 * instruction and database-creation APIs.
 */
kaps_status xml_parse_scenarios(
    const char *fname,
    struct kaps_device *device,
    uint32 skip_clones,
    struct xml_parse_info **parse_info,
    struct xml_inst_info **inst_info);

/**
 * Prints a dynamic core loop step.
 *
 * @param fp pointer to the file in which prints should go
 * @param scenario pointer to the parsed dynamic core loop scenario
 * @param step_no the step number of the scenario to print
 *
 * @retval KAPS_OK on success.
 * @retval KAPS_INTERNAL_ERROR on unknown error condition that cannot be handled.
 *
 */
kaps_status print_dynamic_core_loop_step(
    FILE * fp,
    struct test_scenario *scenario,
    int32 step_no);

/**
 * Prints a dynamic core loop scenarios.
 *
 * @param fp pointer to the file in which prints should go
 * @param scenario pointer to the parsed dynamic core loop scenario
 *
 * @retval KAPS_OK on success.
 * @retval KAPS_INTERNAL_ERROR on unknown error condition that cannot be handled.
 *
 */
kaps_status print_dynamic_core_loop_scenario(
    FILE * fp,
    struct test_scenario *scenario);

/**
 * Parses the dynamic core loop scenarios specified in the XML file.
 *
 * @param fname The XML file name.
 * @param scenario pointer to the parsed scenario returned by the API
 *
 * @retval KAPS_OK on success.
 * @retval KAPS_INVALID_ARGUMENT on errors with arguments.
 * @retval KAPS_OUT_OF_MEMORY on running out of heap memory.
 * @retval KAPS_INTERNAL_ERROR on unknown error condition that cannot be handled.
 * @retval KAPS_PARSE_ERROR on errors with XML formats.
 *
 */
kaps_status xml_parse_dynamic_core_loop_scenario(
    const char *fname,
    struct test_scenario **scenario);

/**
 * Frees the list of XML parse_info structures. The individual
 * databases, and device are left untouched; only the memory
 * associated with the parse_info structures is reclaimed. The caller
 * can continue to operate the device and manage databases
 * after this call.
 *
 * @param parse_info The linked list of ::xml_parse_info structures as returned by xml_parse_scenarios().
 * @param inst_info The linked list of ::xml_inst_info structures as returned by xml_parse_scenarios().
 *
 * @retval KAPS_OK on success
 * @retval KAPS_INVALID_ARGUMENT on errors with a function argument.
 */

kaps_status xml_parse_destroy(
    struct xml_parse_info *parse_info,
    struct xml_inst_info *inst_info);

/**
 * Parse the device configuration file and fill parametes in to the kaps_sw_model_config structures
 * this configuration is used by the c-model init() to have the device initialization accoding to
 * the specification given in the config file.
 *
 * @param fname The XML file name with the device config details.
 * @param dev_type The type of the device, 12K, KAPS, OPrime e.t.c.
 * @param m_config The valid kaps_sw_model_config structure to hold the parameters.
 *
 * @retval KAPS_OK on success
 * @retval KAPS_INVALID_ARGUMENT on errors with a function argument.
 */

kaps_status xml_parse_device_config_file(
    const char *fname,
    enum kaps_device_type dev_type,
    struct kaps_sw_model_kaps_config *m_config);

/**
 * Parse the intent (what this case for) and the t_status (under developement or done)
 *
 * @param fname The XML file name with the device config details.
 * @param intent The text which describes the intent of the test case
 * @param t_status The test case status, 1: under development, 0: done
 *
 * @retval KAPS_OK on success
 * @retval KAPS_INVALID_ARGUMENT on errors with a function argument.
 */

kaps_status xml_parse_intent_and_status(
    const char *fname,
    char **intent,
    uint32 *t_status);

/**
 * @}
 */

#endif /* __KAPS_VERIFIER_XML_PARSE_H_INCLUDED */
