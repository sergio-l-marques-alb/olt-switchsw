/*
 * ! \file kaps_verifier.h contains declarations for kaps verifier
 */
/*
 * $Copyright: (c) 2021 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifndef __KAPS_VERIFIER_H_INCLUDED
#define __KAPS_VERIFIER_H_INCLUDED

#include "kaps_verifier_xml_parse.h"
#include "src/appl/ctest/dnx/kaps/ctest_dnx_kaps.h"
#include "kaps_xpt.h"

#define KAPS_VERIFIER_MAX_DB_COUNT  (30)
#define MAX_FILE_NAME_LENGTH 500

struct fast_test_trie;

#define NUM_PIOWR_STATS_TEST 7
#define PROGRESS_BAR_FACTOR 10
#define SWAP(a, b)            \
    do {                     \
        uint32_t temp = a;   \
        a = b;               \
        b = temp;            \
    } while (0);

#define KAPS_VERIFIER_ALLOC_MEM_ERR(T, A)                                          \
    do                                                                  \
    {                                                                   \
        kaps_status __tmp_status = A;                                    \
        if (__tmp_status != KAPS_OK)                                     \
        {                                                               \
            T->flag_error_status = 2;                                   \
            return __tmp_status;                                        \
        }                                                               \
    }                                                                   \
    while (0)

#define KAPS_VERIFIER_ALLOC_ERR(T, A)                                              \
    do                                                                  \
    {                                                                   \
        kaps_status __tmp_status = A;                                    \
        if (__tmp_status != KAPS_OK)                                     \
        {                                                               \
            T->flag_error_status = 1;                                   \
            return __tmp_status;                                        \
        }                                                               \
    }                                                                   \
    while (0)

enum print_html_file
{
    PRINT_BEFORE,               /* print html before install with static resources allocation */
    PRINT_AFTER                 /* print html with dynamic resources allocation */
};

typedef enum kaps_verifier_coreloop_type_t
{
    KAPS_VERIFIER_CORELOOP_TYPE_SEQUENTIAL,
    KAPS_VERIFIER_CORELOOP_TYPE_RANDOM,
    KAPS_VERIFIER_CORELOOP_TYPE_ADDREMOVE,
    KAPS_VERIFIER_CORELOOP_TYPE_USER,
    KAPS_VERIFIER_CORELOOP_TYPE_LAST,
} kaps_verifier_coreloop_type_e;

typedef enum kaps_verifier_warmboot_type_t
{
    KAPS_VERIFIER_WARMBOOT_TYPE_NO_WARMBOOT,
    KAPS_VERIFIER_WARMBOOT_TYPE_INFREQ,
    KAPS_VERIFIER_WARMBOOT_TYPE_FREQ,
    KAPS_VERIFIER_WARMBOOT_TYPE_LAST,
} kaps_verifier_warmboot_type_e;

typedef enum kaps_verifier_xpt_type
{
    KAPS_VERIFIER_BLACKHOLE_XPT,
    KAPS_VERIFIER_KAPS_MODEL_XPT,
    KAPS_VERIFIER_MDB_XPT
} kaps_verifier_xpt_type;

struct kaps_verifier_dbwise_table
{
    uint32_t dbwise_db_idx[KAPS_VERIFIER_MAX_DB_COUNT];
    uint32_t dbwise_db_percent[KAPS_VERIFIER_MAX_DB_COUNT];
    struct test_db_info *db_info[KAPS_VERIFIER_MAX_DB_COUNT];
    uint32_t dbwise_db_count;
};

struct kaps_verifier_db_capacity_row
{

    uint32_t data[KAPS_VERIFIER_MAX_DB_COUNT];
    uint32_t update_rate[2];
    char row_label[20];
    uint32_t heap_size;
};

struct kaps_verifier_db_capacity_table
{
    uint32_t num_db;
    uint32_t db_width[KAPS_VERIFIER_MAX_DB_COUNT];
    enum kaps_db_type type[KAPS_VERIFIER_MAX_DB_COUNT];
    uint32_t num_filled_rows;
    uint32_t num_total_rows;
    struct kaps_verifier_db_capacity_row *row;
    struct test_db_info *db_info[KAPS_VERIFIER_MAX_DB_COUNT];
    uint32_t db_id[KAPS_VERIFIER_MAX_DB_COUNT];
    struct xml_test *tinfo;

    /*
     * Table Limits 
     */
    uint32_t add_update_rate_limit_1;
    uint32_t add_update_rate_limit_2;
    uint32_t del_update_rate_limit_1;
    uint32_t del_update_rate_limit_2;
    uint32_t min_heap;
    uint32_t max_heap;
};

/**
 * @ingroup KAPS_VERIFIER
 * Information about a specific database held by the
 * test harness.
 */

struct test_db_info
{
    struct xml_parse_info *parse_info; /**< Parsed XML information */
    struct kaps_parse_record *entries;  /**< Actual parsed/fit entries */
    uint32_t nentries;                 /**< Number of entries in above array, also treated as ideal capacity */
    uint32_t nact_entries;             /**< Actual number of entries in the file */
    struct kaps_parse_key *file_key;    /**< The key format in the file */
    struct fast_test_trie *fast_trie;  /**< Independent validation of search keys */
    struct kaps_parse_record *pending;  /**< Entries waiting to be installed */
    struct kaps_parse_record *delete_pending;  /**< Entries waiting to be delete */
    uint32_t nadds;                    /**< Stats for number of adds */
    uint32_t ndeletes;                 /**< Stats for number of deletes */
    uint32_t n_heavydeletes;           /**< Stats for number of heavy deletes */
    uint32_t ninstalls;                /**< Stats for number of installs */
    uint32_t nsearches;                /**< Stats for number of searches */
    uint32_t ncalls_timer;             /**< stats for number of calls to timer */
    uint32_t ncalls_aged_entries;      /**< stats for number of calls to aged entries */
    uint32_t n_heavysearches;          /**< Stats for number of heavy searches */
    uint32_t niter;                    /**< Stats for number of entry iterations */
    uint32_t nmisc;                    /**< Stats for number of misc operations */
    uint32_t nad_updates;              /**< Number of AD updates */
    uint32_t nduplicates;              /**< Stats for duplicates */
    uint32_t least_full_point;         /**< Lowest capacity point where table was reported as full */
    uint32_t db_full;                  /**< Number of times database reported full */
    uint32_t ix_full;                  /**< Number of times IX Range reported full */
    uint32_t callback_enabled;         /**< Debug flag */
    uint32_t num_entries_present;      /**< Number of entries present in the DB used for warmboot ops */
    uint8_t is_brimmed;                /**< Flag to specify DB is near full capacity */
    uint8_t is_defer_del;              /**< Flag to specify DB is have defer deletes */
    uint8_t ix_callback_num;           /**< IX Callback Function magic number */
    uint8_t skip_odd_entry;            /**< OP2 Bigger Test specific to skip odd entries */

    struct kaps_entry **ix_to_entry;    /**< Index to entry mapping */
    uint8_t *ix_is_free;               /**< Is the index location free */
    uint32_t ix_lo;                    /**< The lo index range */
    uint32_t ix_hi;                    /**< The hi index range */
    struct xml_parse_info *container_db; /**< If table points to container database */
    struct test_db_info *clone_parent; /**< If clone, points to parent */
    uint32_t *db_inst_info;            /**< Instruction information for the DB */
    int32_t ninstructions;             /**< size of the above array */
    uint32_t *seedp;                   /**< Seed pointer */
    uint8_t need_to_free_ix;           /**< have the clone/tables status, to free the IX memory or not */
    struct kaps_parse_ad_info *ad_db_info; /**< Linked List AD Database information */
    uint8_t has_explicit_bmr;            /**< If database has explicit bmr */
    uint8_t *gmask;                      /**< Global Mask of the database */
    uint32_t *indices;                    /**< Array of indexes for entry records */
    uint32_t pivot;                       /**< Variable to separate the visited flags */
    uint32_t index;                       /**< Holds the last index */
    struct kaps_parse_record **e_to_parse_record; /**< Array to convert from cp Entry to harness Entry */
    uint32_t e_to_parse_record_size;            /**< size of the above array */
    uint32_t end_index_default_entries;          /**< start index for the non default entries */
    uint32_t min_capacity_estimate;       /**< Smallest capacity estimate returned by kaps_db_stats for this db*/
    uint32_t max_capacity_estimate;       /**< Largest capacity estimate returned by kaps_db_stats for this db */
    uint32_t most_recent_capacity_estimate; /**< Most recently measured capacity estimate*/

    /*
     * copied from parser 
     */
    uint32_t user_cap_min;
    uint32_t user_cap_max;
    uint32_t user_ops_min;
    uint32_t user_ops_max;

    uint32_t is_search_loop;             /**< Flag to mark db in search loop */
    struct kaps_parse_record *exp_delete;
    struct kaps_verifier_ad_info *zero_size_ad_info;
};

/**
 * @ingroup KAPS_VERIFIER
 * Information about specific instructions held
 * by the test harness
 */

struct test_instr_info
{
    struct xml_inst_info *info;  /**< Parsed XML information */
    uint32_t target_db_id;
    uint32_t resolution;
    uint32_t dbs_hit[8];
    uint32_t dbs_mis[8];
    uint32_t total_hits[9];     /* t[0] = all_miss, others are hit counts */
    uint32_t nsearches;          /**< Number of searches */
    uint32_t nfail;              /**< Number of failed searches */
    uint32_t *seedp;             /**< Seed pointer */
};

/**
 * @ingroup KAPS_VERIFIER
 * A generic free list structure
 */

struct kaps_verifier_free_list
{
    struct kaps_verifier_free_list *next; /**< Linked list */
};

/**
 * @addtogroup KAPS_VERIFIER_CORE_LOOP
 *
 * Information on expected search results for each
 * database participating in an instruction
 */

struct kaps_verifier_search_info
{
    int32_t num_indices; /**< The size of indices array below */
    int32_t *indices;    /**< The possible set of hit indices */
    struct test_db_info *db_info; /**< The database being searched */
    struct kaps_parse_record *selected_rec; /**< The entry selected for generating key from */
    struct kaps_parse_record *trie_match;   /**< The fast trie match */
    uint8_t key[KAPS_HW_MAX_SEARCH_KEY_WIDTH_8]; /**< The DB search key */
    struct kaps_entry *cp_entry;            /**< Control plane returned hit */
    int32_t cp_index;                      /**< Control plane returned index */
    int32_t priority;                      /**< Control plane returned priority */
    struct test_instr_info *cur_instr;     /**< Instruction we picked for search */
};

struct kaps_verifier_resolution
{
    uint32_t count;
    struct kaps_verifier_search_info search_info[KAPS_INSTRUCTION_MAX_RESULTS];
};

struct kaps_verifier_search_record
{
    union
    {
        struct kaps_verifier_search_info search_info[KAPS_INSTRUCTION_MAX_RESULTS];   /**< Expected Results */
        struct kaps_verifier_resolution resolution[KAPS_INSTRUCTION_MAX_RESULTS];     /**< Expected Resolved Results */
    } exp_results;
    struct test_instr_info *inst_info;                                    /**< Instruction Info */
    struct kaps_complete_search_result hw_result;                                   /**< HW Search Result */
    uint32_t iteration_number:30;                                         /**< Iteration Number */
    int32_t context_address;                                              /**< Context Buffer Address */
    uint8_t key[KAPS_HW_MAX_SEARCH_KEY_WIDTH_8];                           /**< The DB search key */
};

/**
 * @ingroup KAPS_VERIFIER_AD
 * Associated data information stored by the
 * the test harness
 */
struct kaps_verifier_ad_info
{
    struct kaps_ad *ad;  /**< AD handle */
    struct kaps_parse_ad_info *parse_ad_info;
    uint8_t ad_data[KAPS_HW_MAX_UDA_WIDTH_8];/**< Raw AD */
    uint32_t refcount; /**< Reference count to maintain in indirection mode */
};

/**
 * @ingroup KAPS_VERIFIER_ALLOCATOR
 * Harness Allocator to test memory errors
 */

struct kaps_verifier_allocator
{
    struct kaps_allocator *inner_alloc;   /**< Default Allocator */
    struct xml_test *tinfo;              /**< Testharness  information */
    uint32_t enable;                     /**< Flag to indicate to start enabling leaks */
    uint32_t check_status;               /**< Flag to indicate the status for out_of_memory */
};

struct kaps_verifier_input_params
{
    char *device_xml;                /**< XML file name */
    char *input_xml;                 /**< XML file name */
    kaps_verifier_coreloop_type_e coreloop_type;
    kaps_verifier_warmboot_type_e warmboot_type;
    kaps_verifier_xpt_type xpt_type;
    uint32 num_iterations;          /**< Number of iterations to run for */
    uint32 sw_model_dev_id; /**< the device id*/
    uint32 sw_model_sub_type; /**< the device sub type*/
    uint32 sw_model_profile; /**< the device profile*/
    char *vector_file_name;  /**< Name of the file in which the KAPS Model Vectors should be stored */
};

/**
 * @ingroup KAPS_VERIFIER
 * Test harness state
 */
struct xml_test
{
    enum test_operation *operations_array; /**< Operations to be performed in weighted manner */
    struct test_db_info *db_info_array;   /**< Database information and stats */
    struct test_instr_info *instruction_info_array; /**< Instruction information and stats */
    char *flag_output_dir;             /**< Output directory name into which to dump the files */
    char *flag_expected_fail;          /**< If a failure is expected, the error string of the failure */
    struct kaps_device *device;         /**< Device handle */
    void *xpt_info;                    /**< Our own transport */
    FILE *vector_fp;                    /**< File Pointer to the Vector File*/
    char *vector_file_name;               /**< Vector File name */

    struct kaps_verifier_free_list *ad_free_list; /**< AD info */
    struct xml_parse_info *xml_parse_info; /**< XML Parse Info */
    struct xml_inst_info *xml_inst_info;   /**< XML Instruction Info */

    struct kaps_device_config *config;  /**< Device Config Pointer */
    struct kaps_sw_model_kaps_config *mdl_config;  /**< C-model Config */
    void *xpt;                         /**< XPT pointer */
    void *xpt_pp[4];                   /**< XPT pointers for broadcast */
    FILE *issu_dump_file;              /**< File ptr for ISSU dump */
    uint8_t *issu_nv_memory;           /**< Issu NV Memory for storing */
    char device_file_name[MAX_FILE_NAME_LENGTH];                 /**< device config file for c-model init */
    char input_xml_file_name[MAX_FILE_NAME_LENGTH];                 /**< XML input file */
    uint32_t *seedp;                   /**< Seed pointer */
    struct test_db_info *target_db;    /**< Target DB for sequential Search */
    struct test_instr_info *target_inst;   /**< XML Target Instruction Info */
    struct kaps_parse_record *pio_entries; /**< Linked List of callback Entries */
    char *test_step_config_file_name;  /**< Log File name */
    struct test_scenario *scenario;     /**< Dynamic Test Scenario pointer*/
    struct test_db_info *xpt_db;         /**< XPT Target Database */
    struct kaps_verifier_allocator *allocator;                /**< Harness Allocator */
    struct kaps_verifier_search_record *expected_results;     /**< Expected Results to compare */

    struct kaps_parse_record *target_rec;
    struct kaps_parse_record *last_delete_rec;

    uint8_t ops_distr[10000];                /**< Distribution of ops across DBs and tables */
    enum kaps_device_type flag_device;  /**< Device type provided by user */
    kaps_status n_status;               /**< Last Error Status */

    int unit;                          /**< unit number passed by DNX framework */

    int32_t cur_num_searches;          /**< Current number of searches performed */
    int32_t flag_num_iterations;       /**< Number of iterations to run for */
    int32_t num_operations;            /**< Number of operations in array below */
    int32_t num_databases;             /**< Number of databases in array below */
    int32_t num_instructions;          /**< Number of instructions in array below */
    uint32_t num_pio;                  /**< Stats for number of PIO operations */
    uint32_t num_issu;                 /**< Stats for number of ISSU operations */
    uint32_t num_crash_recovery;       /**< Stats for number of Crash Recovery operations */
    uint32_t num_issu_ops;             /**< number of ISSU operations during run */

    uint32_t num_step_iterations;      /**< Stats for number of iterations */
    uint32_t num_iterations;           /**< Stats for number of iterations */

    uint32_t flag_seed;                /**< Random number seed */
    uint32_t flag_seed_reuse;          /**< Random number seed reused for ISSU*/

    kaps_verifier_xpt_type xpt_type;   /**< Black hole, KAPS Model, MDB XPT */
    uint32_t does_xpt_support_search;  /**< Does XPT support search*/
    uint32_t does_xpt_support_replication; /**< Does XPT Replicate the dbs on different cores */
    CTEST_KAPS_MDB_XPT mdb_xpt;            /**< mdb xpt structure*/

    int32_t flag_verbose;              /**< Verbose prints */
    int32_t flag_coherency;            /**< Enable coherency testing */
    int32_t flag_fail_on_continue;     /**< for fix_errors, expected fails are ignored */
    int32_t flag_inner_loop;           /**< Disable test inner loop */
    int32_t flag_warmboot;             /**< Warmboot Enabled 1: Heap Mem Based 2:  File Based*/
    int32_t flag_random;               /**< Random core loop to enable/disable 0: Sequential , 1: Random (Default) */
    int32_t flag_use_pivot;            /**< To separate the visited flags */
    int32_t flag_gen_unique;           /**< generate only unique prefixes for LPM databases */
    int32_t flag_defer_deletes;        /**< enable deferring deletes */

    int32_t global_est_min_capacity;   /**< global estimated minimum capacity for all databases*/

    uint32_t device_init_flags;        /**< Device init Flags */

    uint32_t nv_size;                  /**< Bytes allocated for NV Memory */
    uint32_t peak_dump_size;           /**< Peak size of ISSU NV dump */
    uint32_t peak_file_ops;            /**< Peak NV ops for ISSU */
    uint32_t num_file_ops;             /**< NV ops for current ISSU operation*/
    uint32_t peak_issu_save_time;      /**< Peak time taken by ISSU save */
    uint32_t peak_issu_restore_time;   /**< Peak time taken by ISSU restore */
    uint8_t issu_in_progress;          /**< ISSU is in progress */

    uint8_t adds_in_progress;          /**< Adds are in Progress for batch mode databases */
    uint8_t looking_for_pio;            /**< Flag to indicate is it is looking for any pio */
    uint8_t do_not_perform_xpt_searches; /**< set to 1 when completed the core loop */

    uint32_t searches_in_progress;       /**< Issue the searches in batch mode */
    uint8_t num_bc_devices;              /**< Number of Broadcast Devices */
    uint32_t nentries_to_add;   /* Explicit to create_vector */
    uint8_t flag_crash_recovery;         /**< Flag for crash recovery */
    uint8_t force_misses;                /**< Option for forcing misses while doing searches */
    uint8_t do_not_update_index_change_callbacks;

    volatile uint8_t can_crash;          /**< Can create a crash from test harness*/
    pthread_t parent_thread_id;          /**< Harness parent thread id */

    uint8_t failed_search_id;            /**< In case of search mismatch, contains the index of the parallel search that failed */
    int32_t search_all_active;           /**< Iteration number at which all active entries are searched. */
    struct kaps_allocator *model_alloc, *alloc;  /** Allocator */
    struct kaps_nlm_allocator *al;
    int32_t flag_ISSU;                          /**< ISSU enable/disable 0: disable (Default) , 1: Enable */
    int32_t flag_error_status;                  /** Error/Allocation status value -1: Error, 2: Allocation,*/
    int32_t pass;                               /** Pass/Fail result value */
    uint32_t flags;
    uint32_t flag_silent_steps;                 /**< Disables the detailed steps output during dynamic script execution */
    uint32_t custom_mode;                       /**< Mode Value for custom core loop */

    uint32_t overall_srch_count;                /**< total accumulated search count */
    uint32_t flag_skip_delete_all;              /**< Skip the call to delete all entries at the end. useful speeding up for capacity runs */

    uint32_t print_num_searches;

    uint32_t strict_capacity_checking;
    uint32_t capacity_problem_present;
    uint32_t db_stats_problem_present;

    uint32_t force_seq_ins;     /* To be used with -T ops.xml so that we can add entries to a db in same order as
                                 * dataset */

    uint32_t print_dump;
    uint32_t capacity_tolerance;
    uint32_t baseline_accuracy;

    uint32_t flag_device_dump_out;

    uint32_t cm_add_mode;
    uint32_t cm_percent;
    uint32_t cm_iter;
    uint32_t cm_dump_html;
    uint32_t cm_del_mode;
    uint32_t cm_del_order;
    uint32_t is_custom_description;
    char cm_description[1024];
    uint32_t cm_capacity_tolerance;
    uint32_t cm_update_tolerance;
    uint32_t cm_memory_tolerance;
    uint32_t is_custom_error_description;
    char cm_error_description[1024];
    uint32_t cm_dbwise_add_remove;
    uint32_t verify_db_stats;

    uint32_t block_warmboot_reconcile;

    uint32_t ch_noadd;
    uint32_t ch_nodelete;
    uint32_t ch_initial_num_entries;

    int32_t cmodel_device_type;
    int32_t cmodel_sub_type;
    int32_t cmodel_profile;
    uint32_t maintain_e_to_parse_record_table;
    uint32_t is_add_del_ops_of_multiple_dbs;
};

/**
 * @ingroup KAPS_VERIFIER
 * KAPS_VERIFIER generic 64b register.
 */
typedef struct kaps_verifier_register_64_t
{
    uint64_t address;           /**< Address of the register. */
    char *name;                 /**< Name of the register. */
    uint64_t value;             /**< Value of the register. */
    char *description;          /**< Register brief description. */
} kaps_verifier_reg64_t;

kaps_status kaps_verifier_device_set_property(
    struct xml_test *);

void print_sw_state(
    struct xml_test *tinfo);

void print_html_output_file(
    struct xml_test *tinfo,
    enum print_html_file at);

kaps_status perform_install_on_db(
    struct xml_test *tinfo,
    struct test_db_info *db_info);

kaps_status clean_up_delete_entry(
    struct xml_test *tinfo,
    struct test_db_info *db_info,
    struct kaps_parse_record *add);

struct test_db_info *get_test_info(
    struct xml_test *tinfo,
    struct kaps_db *db);

struct test_db_info *get_test_info_for_xml_db(
    struct xml_test *tinfo,
    struct xml_parse_info *db);

struct kaps_parse_record *get_next_entry(
    struct test_db_info *db_info,
    uint8_t visited);

/**
 * @ingroup KAPS_VERIFIER_ALLOCATOR
 *
 * Creates the allocator which overrides the default allocator
 * function pointers
 *
 * @param tinfo Test harness information
 * @param alloc Default Allocator
 * @param r_alloc Default Allocator which overrides the function pointer
 *
 * @return KAPS_OK on successful match or an error code
 */

kaps_status kaps_verifier_allocator_init(
    struct xml_test *tinfo,
    struct kaps_allocator *alloc,
    struct kaps_allocator * *r_alloc);

shr_error_e kaps_verifier_main(
    int unit,
    sh_sand_control_t * sand_control);

#endif
