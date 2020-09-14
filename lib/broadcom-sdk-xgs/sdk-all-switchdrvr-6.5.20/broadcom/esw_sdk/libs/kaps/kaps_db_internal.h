/*******************************************************************************
 *
 * Copyright 2012-2019 Broadcom Corporation
 *
 * This program is the proprietary software of Broadcom Corporation and/or its
 * licensors, and may only be used, duplicated, modified or distributed pursuant
 * to the terms and conditions of a separate, written license agreement executed
 * between you and Broadcom (an "Authorized License").  Except as set forth in an
 * Authorized License, Broadcom grants no license (express or implied), right to
 * use, or waiver of any kind with respect to the Software, and Broadcom expressly
 * reserves all rights in and to the Software and all intellectual property rights
 * therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS
 * SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.
 *
 * Except as expressly set forth in the Authorized License,
 *
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use all
 * reasonable efforts to protect the confidentiality thereof, and to use this
 * information only in connection with your use of Broadcom integrated circuit
 * products.
 *
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED
 * "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS
 * OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
 * TO THE SOFTWARE. BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES
 * OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE,
 * LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION
 * OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF
 * USE OR PERFORMANCE OF THE SOFTWARE.
 *
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM
 * OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT,
 * OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR
 * USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT
 * ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE
 * LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF
 * ANY LIMITED REMEDY.
 *
 *******************************************************************************/

#ifndef __KAPS_DB_INTERNAL_H
#define __KAPS_DB_INTERNAL_H

#include "kaps_portable.h"
#include "kaps_list.h"
#include "kaps_db.h"
#include "kaps_resource.h"
#include "kaps_bitmap.h"
#include "kaps_db_wb.h"

#ifdef __cplusplus
extern "C"
{
#endif

    struct kaps_ad;

#define KAPS_GET_DB_PARENT(db) ((db)->parent ? (db)->parent : (db))

/**
 * List node to database pointer
 */

#define KAPS_SSDBLIST_TO_ENTRY(ptr) CONTAINER_OF((ptr), struct kaps_db, node)

/**
 * Maximum number of dbs supported on left half or right half of an AB.
 */

#define KAPS_DB_MAX_LEFT_HALF_DBS     1
#define KAPS_DB_MAX_RIGHT_HALF_DBS    1

/**
 * Internal Statistics per database for PIO operations
 */

    struct kaps_db_internal_stats
    {
        uint32_t num_of_piowrs;
                              /**< Number of PIOWRs */
        uint32_t num_of_shuffles;
                              /**< Number of Shuffles */
        uint32_t num_blk_ops;
                            /**< Number of block operations */
    };

/**
 * @brief Enumeration to place db on left or right half of AB.
 */

    enum kaps_db_place_on_half
    {
        KAPS_DB_PLACE_ON_BOTH_HALVES = 0,
                                       /**< Default value for any db placement */
        KAPS_DB_PLACE_ON_LEFT_HALF,    /**< Place db on the left half of AB */
        KAPS_DB_PLACE_ON_RIGHT_HALF,   /**< Place db on the right half of AB */
        KAPS_DB_SPAN_BOTH_HALVES       /**< Monolithic db that spans both halves */
    };

/**
 * Statistics per database for PIO operations
 */

    struct kaps_db_pio_stats
    {
        uint32_t num_of_piowrs;
                              /**< Number of PIOWRs */
        uint32_t num_of_piords;
                              /**< Number of PIORDs */
        uint32_t num_of_shuffles;
                              /**< Number of Shuffles */
        uint32_t num_of_worst_case_shuffles;
                                         /**< Number of Worst case shuffles taken for entry */
        uint32_t num_ix_cbs;/**< Number of index change callbacks */
        uint32_t num_blk_ops;
                            /**< Number of block operations */
        uint32_t num_ad_writes;
                            /**< Number of AD writes */
    };

/**
 * Actual implementations of functions for ACL, LPM and EM
 */
    struct kaps_db_fn_impl
    {
        kaps_status(
    *db_add_ace) (
    struct kaps_db * db,
    uint8_t * data,
    uint8_t * mask,
    uint32_t priority,
    struct kaps_entry ** entry);
        kaps_status(
    *db_update_ace) (
    struct kaps_db * db,
    struct kaps_entry * entry,
    uint8_t * data,
    uint8_t * mask);
        kaps_status(
    *db_relative_add_ace) (
    struct kaps_db * db,
    uint8_t * data,
    uint8_t * mask,
    enum kaps_entry_position entry_position,
    struct kaps_entry * relative_entry,
    struct kaps_entry ** entry,
    uint32_t priority);
        kaps_status(
    *db_add_prefix) (
    struct kaps_db * db,
    uint8_t * prefix,
    uint32_t length,
    struct kaps_entry ** entry);
        kaps_status(
    *entry_add_range) (
    struct kaps_db * db,
    struct kaps_entry * entry,
    uint16_t lo,
    uint16_t hi,
    int32_t range_no);
        kaps_status(
    *entry_add_ad) (
    struct kaps_db * db,
    struct kaps_entry * entry,
    struct kaps_ad * ad);
        kaps_status(
    *entry_add_hb) (
    struct kaps_db * db,
    struct kaps_entry * entry,
    struct kaps_hb * hb);
        kaps_status(
    *get_hb) (
    struct kaps_db * db,
    struct kaps_entry * entry,
    struct kaps_hb ** hb);
        kaps_status(
    *db_delete_entry) (
    struct kaps_db * db,
    struct kaps_entry * entry);
        kaps_status(
    *db_install) (
    struct kaps_db * db);
        kaps_status(
    *db_search) (
    struct kaps_db * db,
    uint8_t * key,
    struct kaps_entry ** entry,
    int32_t * index,
    int32_t * prio_len);
        kaps_status(
    *get_prefix_handle) (
    struct kaps_db * db,
    uint8_t * prefix,
    uint32_t length,
    struct kaps_entry ** entry);
        kaps_status(
    *update_range_encoding_for_entry) (
    struct kaps_db *,
    struct kaps_entry *,
    uint32_t);
        kaps_status(
    *db_stats) (
    struct kaps_db * db,
    struct kaps_db_stats * stats);
        kaps_status(
    *db_delete_all_entries) (
    struct kaps_db * db);
        kaps_status(
    *db_delete_all_pending_entries) (
    struct kaps_db * db);
        kaps_status(
    *db_fix_ab_location) (
    struct kaps_device * device,
    struct kaps_db * db,
    uint32_t hw_ab_index,
    struct kaps_entry ** entry);
        kaps_status(
    *db_fix_uda_location) (
    struct kaps_device * device,
    struct kaps_db * db,
    uint32_t uda_address,
    uint32_t * found_p);
        kaps_status(
    *db_process_hit_bits) (
    struct kaps_device * device,
    struct kaps_db * db);
        kaps_status(
    *db_get_algo_hit_bit_value) (
    struct kaps_device * device,
    struct kaps_db * db,
    struct kaps_entry * entry,
    uint8_t clear_on_read,
    uint32_t * bit_value);
        kaps_status(
    *compute_result_index) (
    struct kaps_ab_info * ab,
    int32_t ab_loc,
    uint32_t * result_idx);
        kaps_status(
    *get_data_len) (
    struct kaps_db * db,
    struct kaps_entry * e,
    uint32_t * len_1,
    uint8_t ** data);

        kaps_status(
    *db_set_key) (
    struct kaps_db * db,
    struct kaps_key * key);
        kaps_status(
    *db_set_property) (
    struct kaps_db * db,
    uint32_t property,
    va_list vl);
        kaps_status(
    *db_get_property) (
    struct kaps_db * db,
    uint32_t property,
    va_list vl);
        kaps_status(
    *entry_set_property) (
    struct kaps_db * db,
    struct kaps_entry * entry,
    uint32_t property,
    va_list vl);
        kaps_status(
    *db_add_em) (
    struct kaps_db * db,
    uint8_t * data,
    struct kaps_entry ** entry);
        kaps_status(
    *entry_print) (
    struct kaps_db * db,
    struct kaps_entry * entry,
    FILE * fp);
        kaps_status(
    *db_add_table) (
    struct kaps_db * db,
    uint32_t id,
    uint32_t is_clone,
    struct kaps_db ** table);
        kaps_status(
    *entry_get_priority) (
    struct kaps_db * db,
    struct kaps_entry * entry,
    uint32_t * prio_length);
        kaps_status(
    *entry_get_index) (
    struct kaps_db * db,
    struct kaps_entry * entry,
    int32_t * nindices,
    int32_t ** indices);
        kaps_status(
    *entry_get_info) (
    struct kaps_db * db,
    struct kaps_entry * entry,
    struct kaps_entry_info * info);
        kaps_status(
    *update_ad) (
    struct kaps_db * db,
    struct kaps_ad * ad_handle,
    struct kaps_entry * e,
    uint32_t num_bytes);
        kaps_status(
    *get_ad) (
    struct kaps_db * db,
    struct kaps_entry * e,
    struct kaps_ad * ad_handle,
    uint32_t * uda_address,
    struct kaps_device ** device);
        kaps_status(
    *save_property) (
    struct kaps_db * db,
    struct kaps_wb_cb_functions * cb_fun);
        kaps_status(
    *restore_property) (
    struct kaps_db * db,
    struct kaps_wb_cb_functions * cb_fun);
        kaps_status(
    *save_state) (
    struct kaps_db * db,
    struct kaps_wb_cb_functions * cb_fun);
        kaps_status(
    *restore_state) (
    struct kaps_db * db,
    struct kaps_wb_cb_functions * cb_fun);
        kaps_status(
    *save_cr_state) (
    struct kaps_db * db,
    struct kaps_wb_cb_functions * cb_fun);
        kaps_status(
    *restore_cr_state) (
    struct kaps_db * db,
    struct kaps_wb_cb_functions * cb_fun);
        uint32_t(
    *cr_calc_entry_mem) (
    struct kaps_db * db);
        kaps_status(
    *cr_store_entry) (
    struct kaps_db * db_p,
    struct kaps_entry * entry_p);
        kaps_status(
    *cr_update_entry) (
    struct kaps_db * db_p,
    struct kaps_entry * entry_p);
        kaps_status(
    *cr_store_ad_entry) (
    struct kaps_db * ad_db,
    struct kaps_ad * ad_handle);
        kaps_status(
    *cr_store_trigger) (
    struct kaps_db * db_p,
    void *trigger);
        kaps_status(
    *cr_delete_entry) (
    struct kaps_db * db_p,
    void *entry_p,
    uint32_t is_start);
        kaps_status(
    *cr_entry_shuffle) (
    struct kaps_db * db_p,
    void *entry_p,
    uint32_t is_start);
        kaps_status(
    *cr_entry_flush) (
    struct kaps_db * db_p,
    void *entry_p,
    uint32_t is_start);
        kaps_status(
    *cr_entry_update_ad) (
    struct kaps_device * device,
    struct kaps_entry * entry_p,
    struct kaps_ad * ad_handle,
    uint8_t * value);
        kaps_status(
    *cr_reconcile) (
    struct kaps_db * db_p);
        kaps_status(
    *reconcile_end) (
    struct kaps_db * db);
        kaps_status(
    *set_used) (
    struct kaps_entry * entry);
        kaps_status(
    *reset_used) (
    struct kaps_entry * entry);
        kaps_status(
    *lock_db) (
    struct kaps_db * db);
        kaps_status(
    *advanced_search_and_repair) (
    struct kaps_db * db);
        kaps_status(
    *db_destroy) (
    struct kaps_db * db);
    };

    struct kaps_db_common_info
    {
        uint32_t mapped_to_acl:1;     /**< LPM / EM database mapped to ACL */
        uint32_t is_em:1;             /**< The database is EM */
        uint32_t user_specified:1;    /**< Resources have been user specified */
        uint32_t user_specified_range:1;
                                      /**< Range units have been specified by user */
        uint32_t user_specified_lsn_width:1;
                                          /**< LSN width has been specified by user */
        uint32_t user_specified_pwr_bgt:1;
                                      /**< User specified power budget. */
        uint32_t finalized_resources:1;
                                      /**< hw_res structure is valid */
        uint32_t ad_spare_bits:1;     /**< If we have 24b of spare space in AD */
        uint32_t cascade_override:1;  /**< User specified cascade device */
        uint32_t cascade_device:2;    /**< Cascade device number if cascade override is specified */
        uint32_t calc_shuffle_stats:1;/**<Enable shuffle stats calculations */
        uint32_t start_blk:10;        /**< used for some optimization */
        uint32_t cascaded_across_devices:1;
                                        /**< If the database lives on more than one KBP */
        uint32_t hw_res_alloc:1;      /**< Has the hw_res structure been allocated for this db. Only if bit is set the structure must be freed */
        uint32_t split_uda_space:1;   /**< Need to split the UDA space into upper and lower half */
        uint32_t is_extra_byte_added:1;
                                      /**< Indicates if extra byte is internally added to MS portion of the entry*/
        uint32_t manually_placed_resources:1;
                                          /**< User specified which blocks to use within each h/w resource */
        uint32_t enable_dynamic_allocation:1;
                                          /**< enables dynamic allocation of DBA and UDA*/
        uint32_t utilise_uda_holes:1; /**< Try to utilise the UDA holes while allocating UDA space for LSN*/
        uint32_t enable_db_compaction:1;
                                     /**< enables DB compaction*/
        uint32_t priority_pattern:2;  /**< Indicates the priority pattern of this database */
        uint32_t is_algo:1;           /**< user specified algo type */
        uint32_t treat_holes_as_pad:1;/**< Treat user added holes as padding field and it becomes implicit BMR */
        uint32_t custom_index:1;      /**< User specified index space */
        uint32_t has_ranges:1;        /** does this database have ranges ? */
        uint32_t use_narrow_abs:1;    /**< Use narrow ABs for narrow entries. Useful for tables */
        uint32_t send_priority_over_ad:2;
                                      /**< LPM Specific send priority over AD */
        uint32_t dummy_db:2;
                         /**< Dummy database */
        uint32_t is_cascaded:1;      /**< Indicates if it is physically cascaded in both core */
        uint32_t is_xl_db:1;         /**< user set property for op2 specific for cascaded databases  */
        uint32_t locality:2;         /**< user set locality to indicate resource sharing  */
        uint32_t replicate_db:1;     /**< replicate the DB for multi thread access  */
        uint32_t is_replicate_db_user_specified:1;
                                               /**< is the above replicate_db specified by user*/
        uint32_t multicast_db;       /**< specifies multicast database */
        uint32_t db_cascade_alternate_addition:1;      /**< Add prefixes in ping and pong order (OP2 Specific)  */
        int32_t cir_granularity;      /**< Minimum number of CIR ranges to be allocated every time*/
        int32_t prev_entry_priority;  /**< Holds the priority of the last added entry */
        uint8_t *index_range_used;    /**< index range used bitmap */
        uint32_t index_range_bmp_size;/**< index range bitmap size in number of bits*/
        uint32_t estimated_min_priority;
                                      /**< Heuristics for DBA manager */
        uint32_t estimated_min_priority_user_specified_value;
                                                           /**< User supplied value for the above heuristics*/
        uint32_t estimated_max_priority;
                                      /**< Heuristics for DBA manager */
        uint32_t estimated_max_priority_user_specified_value;
                                                           /**< User supplied value  for the above heuristics*/
        uint8_t defer_deletes_to_install;
                                      /**< Defer deletes to install */
        uint8_t compaction_done;      /**< Flag to check if compaction is done */

        enum kaps_db_insertion_mode insertion_mode;
                                                /**< 0: none, 1: normal, 2: entry with index, 3:relative insertion */
        uint32_t capacity;            /**< Minimum required capacity */
        int32_t max_capacity;         /**< Maximum capacity limit*/
        int32_t cur_capacity;         /**< Current number of entries in the database (including all the tables in it) */
        uint32_t index_range_min;     /**< For all DBs to control hit index range */
        uint32_t index_range_max;     /**< For all DBs to control hit index range */

        int16_t num_uit;              /**< tmp information during allocation */
        int16_t num_ab;               /**< minimum num of ABs to allocate for this DB */
        int16_t num_ab_core[2];               /**< minimum num of ABs to allocate for this DB */
        int16_t num_ab_currently_allocated;
                                         /**< num of ABs currently allocated to this DB */
        int16_t num_small_ab_currently_allocated;
                                               /**< num of small ABs currently allocated to this DB */
        uint16_t start_hb_blk;        /**< starting hit bit block assigned to the DB*/
        uint16_t num_hb_blks;         /**< number of hit bit blocks assigned to the DB*/
        uint32_t kaps2_large_hb_blk_bmp;
                                      /**< The hit bit blocks assigned to the DB in KAPS2-large mode*/
        int8_t range_units;           /**< tmp info during allocation */
        int32_t uda_mb;               /**< Mb of UDA used for AD/alg node of databases,
                                           for printing only */
        int32_t uda_mb_core[2];
        int32_t uda_mb_currently_allocated;
                                        /**< Mb of UDA currently allocated to this DB */

        uint8_t meta_priority;        /**< Default meta priority for the entries added to the db */

        int16_t user_num_ab;          /**< User specified DBA */
        int32_t user_uda_mb;          /**< User specified UDA */

        kaps_db_index_callback callback_fn;
                                       /**< User installed callback function */
        void *callback_handle;        /**< User provided callback handle */

        uint16_t fit_error;           /**< If non zero, flags all failed allocations for database */
        uint8_t dba_key_width_16;     /**< Number of 16b words that need to be sent to DBA (OP specific) */
        uint8_t cascade_map;          /**< Bitmap of resources on cascaded devices */

        uint32_t *user_hdl_table_size; /**< User handle to entry table size */
        void **user_hdl_to_entry;     /**< User handle to entry table */

        struct kaps_db_pio_stats pio_stats;
                                        /**< PIOWR/RD and Shuffle stats */

        union
        {
            struct kaps_db *ad;        /**< Associated data database, if this is regular db */
            struct kaps_db *db;        /**< The database using the AD, if this is an AD database */
        } ad_info;

        union
        {
            struct kaps_db *hb;        /**< HB database, if this is regular db */
            struct kaps_db *db;        /**< The database using the HB, if this is an HB database */
        } hb_info;

        union
        {
            struct kaps_db *counter;    /**< Counter database, if this is regular db */
            struct kaps_db *db;         /**< The database using the Counter db, if this is an Counter database */
        } counter_info;

        uint16_t entry_size;          /**< Size of entry in bytes, this is for CR*/
        uint16_t nv_db_iter;          /**< nv_offset_iter */
        struct kaps_entry **del_entry_list;
                                        /**< pending list for delete entries */
        uint32_t num_pending_del_entries;
                                      /**< number of entries in delete pending list */
        uint32_t max_pending_del_entries;
                                      /**< Maximum size of the delete pending list */
        uint8_t is_upd_level_enabled;/**< UPD level property set */
        uint32_t max_pfx_len;        /**< Maximum prefix length for distribution */
        uint32_t pfx_dist_list[KAPS_LPM_KEY_MAX_WIDTH_1 + 1];
                                                          /**< prefix distribution list. index is prefix length
                                                              index 0: pfx len 0, index 1: pfx len 1,.......index 32: pfx len 32 */
        uint32_t pfx_dist[KAPS_LPM_KEY_MAX_WIDTH_1 + 1];
                                                     /**< Internal prefix distribution based on priorities */
        uint32_t pfx_dist_count[KAPS_LPM_KEY_MAX_WIDTH_1 + 1];
                                                           /**< Internal prefix distribution count based on priorities */
        uint32_t pfx_dist_80b[KAPS_LPM_KEY_MAX_WIDTH_1 + 1];
                                                         /**< Internal prefix distribution based on priorities */
        uint32_t pfx_dist_count_80b[KAPS_LPM_KEY_MAX_WIDTH_1 + 1];
                                                               /**< Internal prefix distribution count based on priorities */
        uint32_t is_pfx_dist_available:1;
                                       /**< External prefix distribution available */
        uint32_t default_pfx_distr:1;  /**< 1 Means default prefix distribution is enabled else disabled */
        uint32_t is_pfx_dist_dba_mgr:1;/**< Internal prefix distribution available? */
        uint32_t is_pfx_dist_80b_dba_mgr:1;/**< Internal prefix distribution available? */
        FILE *fp;                      /**< FILE pointer for logging */
        FILE *fp_wb;                   /**< FILE pointer for warmboot dump on asserts */

        int32_t total_bb_rows;              /**< KAPS Specific */
    };

    struct kaps_test_summary
    {
        uint64_t expected_count[10];
        uint64_t received_count[10];
        float miss_rate[10];
        int32_t loop_count;
    };

/**
 * Every internal database carries this data structure at the
 * beginning. This is used by the resource management module to
 * populate resources
 */

    struct kaps_db
    {
        struct kaps_list_node node;        /**< To chain databases */
        enum kaps_db_type type;        /**< Database type */
        struct kaps_db *parent;        /**< If this is a sub table, then points to parent container */
        struct kaps_db *next_tab;      /**< List of tables within the database */
        struct kaps_key *key;          /**< Database key specification */
        struct kaps_device *device;    /**< Device handle */
        struct kaps_db_fn_impl *fn_table;
                                       /**< Actual function implementations */
        struct kaps_instruction **instructions;
                                            /**< List of instruction this DB participates in */
        int16_t ninstructions;        /**< Number of instructions in the array above */
        uint16_t seq_num;             /**< Unique number assigned to each database */

        union
        {
            int16_t key_width_1;      /**< Original unmodified user key width in bits, valid for ACL/LPM/EM databases only */
            int16_t ad_width_1;       /**< Width of AD in bits, valid for AD databases only */
        } width;

        union
        {
            struct kaps_db_hw_resource *db_res;/**< Assigned HW resources for ACL & LPM databases */
            struct kaps_ad_db_hw_resource *ad_res;
                                               /**< Assigned HW resources for AD databases */
        } hw_res;

        struct kaps_ad_db_hw_resource ad_res_internal;
                                                   /**< Assigned HW resources for Internal AD databases */

        struct kaps_c_list db_list;        /**< All entries in database */
        struct kaps_c_list db_pending_list;/**< All entries in database */

        uint8_t offset_without_ranges_8;
                                      /**< Key offset after excluding ranges */
        struct kaps_db_common_info *common_info;
                                               /**< Information common to the Database and its tables */

        struct kaps_lpm_mgr *lpm_mgr;   /* Pointer to the LPM Mgr corresponding to this database */

        struct kaps_db *clone_of;      /**< If this database is a clone, this points to the database/table of which this is a clone. is_clone bit must be set */
        void *handle;                 /**< Internal use */

        uint32_t tid;                 /**< Control plane database identifier (database ID) */
        uint32_t is_clone:1;          /**< This database is a clone */
        uint32_t has_clones:1;        /**< This database has clones */
        uint32_t has_tables:1;        /**< This database has sub-tables */
        uint32_t bmr_no:3;            /**< bmr used for this DB. valid only for ACL DB*/
        uint32_t has_dup_db:1;        /**< this DB is duplicated one */
        uint32_t has_dup_ad:1;        /**< AD is duplicated */
        uint32_t is_destroyed:1;      /**< Database has been destroyed */
        uint32_t is_destroy_in_progress:1;
                                      /**< Database destroy is currently in progress */
        uint32_t is_internal:1;       /**< Database is internal, not created by user */
        uint32_t is_main_bc_db:1;     /**< Database is main_db, not broadcast */
        uint32_t is_bc_required:1;    /**< is broadcast feature required for this DB, used by Resource manager */
        uint32_t is_public:1;         /**< indicates if the database is possibly a public database */
        uint32_t is_replicated_internally:1;
                                         /**< op2 specific, set only when the DB is replicated to other core internally by SDK */
        uint32_t place_on_which_half:2;
                                      /**< Place db on left or right half of AB (12K specific) */
        uint32_t scale_up_sb_granularity:5;
                                        /**< Number of SBs to assign on one half before moving on to other half */
        uint32_t scale_up_num_left_abs:8;
                                      /**< Number of continuous ABs to assign on left half */
        uint32_t scale_up_num_right_abs:8;
                                      /**< Number of continuous ABs to assign on right half */
        uint32_t relative_ins_update_in_progress:1; /**< Use relative insertion mode API*/
        uint32_t is_type_a:1;         /**< Indicates if database is type A or type B (KAPS specific)*/
        uint32_t has_counters:1;      /**< This database has Counters (SnC) */
        uint32_t user_specified_counter_type:1;
                                            /**< User specified counter type for the database */
        uint32_t user_specified_counter_cycle:1;
                                             /**< User specified counter cycle for the database */
        uint32_t is_accessed_from_core0:1;
                                      /**< Indicates if it is accessed from core 0*/
        uint32_t is_accessed_from_core1:1;
                                      /**< Indicates if it is accessed from core 1*/
        uint32_t is_accessed_from_both_core:1;
                                          /**< Indicates if it is accessed from both core*/
        uint32_t placed_at_core:1;    /**< Indicates which core the DB has been physically placed*/
        uint8_t is_bmr_assigned;      /**< Has valid BMR value assigned or not */
        uint8_t ltr_db_id;            /**< Assigned by resource management, that identifies the database in LTR */
        int16_t highest_wide_ab_index;/**< Highest indexed wide AB currently allocated to this DB */
        int16_t lowest_narrow_ab_index;
                                      /**< Lowest indexed narrow AB currently allocated to this DB */

        uint32_t num_algo_levels_in_db; /**< Number of algorithmic levels in the database*/                             

        char *description;            /**< User specified name */
        uintptr_t stale_ptr;          /**< Pointer before ISSU call */
        uintptr_t stale_left_db_ptr;  /**< Paired left database pointer before ISSU call */
        uintptr_t hb_ad_stale_ptr;    /**< HB AD pointer before ISSU call */

        struct kaps_db *main_bc_db;    /**< Pointer to main broadcast device when broadcasting is enabled*/
        uint32_t bc_bitmap;           /**< Broadcast bitmap value for this DB */
        struct kaps_db *common_uda_owner_db;
                                         /**< Pointer to the owner DB when common UDA Mgr is in use */
        int8_t num_other_dbs_with_shared_uda;
                                          /**< number of such DBs mentioned above */
        struct kaps_db *common_ad_owner_db;
                                         /**< Pointer to the owner DB when common AD is in use */
        int8_t num_other_dbs_with_shared_ad;
                                         /**< number of such DBs mentioned above */
        struct kaps_db *paired_db;     /**< This db shares an AB vertically with the current db (12K specific) */

        struct kaps_entry *relative_entry;
                                       /** <insert relative to this entry */
        enum kaps_entry_position relative_entry_position;
                                                      /**< relative position before/after */
    };

/**
 * Pointer to KBP entry from database list node
 */

#define KAPS_DBLIST_TO_KAPS_ENTRY(ptr) CONTAINER_OF((ptr), struct kaps_entry, db_node)

/**
 * Common KBP entry structure which is the first part of
 * every ACL/LPM or EM entry
 *
 * db_node      Node to chain the entries that are part of table or database
 * ad_handle    AD information
 * db_seq_num   The database seq_num to which this entry belongs
 * hb_user_handle  HitBit user handle for this entry
 * counter_user_handle Counter user handle for this entry
 * user_handle  User handle for this entry
 *
 */

#define KAPS_ENTRY_STRUCT_MEMBERS      \
    struct kaps_list_node db_node;         \
    struct kaps_ad *ad_handle;         \
    uint32_t db_seq_num:8;            \
    uint32_t hb_user_handle:24;       \
    uint32_t counter_user_handle:27;  \
    uint32_t core:1;                  \
    uint32_t user_handle:27;

    struct kaps_entry
    {
    KAPS_ENTRY_STRUCT_MEMBERS};

/**
 * Initialize the LPM database
 *
 * @param device valid device handle
 * @param id a unique LPM database identifier. This is a control plane identifier only
 * @param capacity the minimum required capacity for this database
 * @param dbp database handle initialized and returned on success
 *
 * @return KAPS_OK on success or an error code
 */

    kaps_status kaps_lpm_db_init(
    struct kaps_device *device,
    uint32_t id,
    uint32_t capacity,
    struct kaps_db **dbp);

/**
 * Copy the required parameters from
 * original db to broadcast DB
 *
 * @param ori_db valid original db handle
 * @param bc_db valid broadcast db handle
 *
 * @return KAPS_OK on success or an error code
 */

    kaps_status kaps_lpm_db_create_broadcast(
    struct kaps_db *ori_db,
    struct kaps_db *bc_db);

/**
 * Returns the database handle that maintains the resources
 *
 * @param db valid database handle
 *
 * @return database handle which maintains resources for this database
 */
    struct kaps_db *kaps_lpm_get_res_db(
    struct kaps_db *db);

/**
 * Destroy all LPM resources
 *
 * @param device valid device handle
 *
 * @return KAPS_OK on success or an error code
 */

    kaps_status kaps_lpm_destroy(
    struct kaps_device *device);

/**
 * Returns the main database. If parent database is passed, then same database is returned.
 * If table is passed, then parent database is returned
 * If clone is passed, then parent of the corresponding table of the clone is returned
 *
 * @param db input database
 *
 * @return the main database corresponding to input database
 */
    struct kaps_db *kaps_db_get_main_db(
    struct kaps_db *db);

/**
* Searches the given device and returns the pointer to DB which
* matches with the given id and type and has no parent
*
* @param device Valid device handle.
* @param id DB id to search.
* @param type DB type to search.
*
* @return Pointer to the db on success, NULL otherwise.
*/

    struct kaps_db *kaps_db_get_bc_db_on_device(
    struct kaps_device *device,
    struct kaps_db *inp_db);

    kaps_status kaps_db_init_internal(
    struct kaps_device *device,
    enum kaps_db_type type,
    uint32_t id,
    uint32_t capacity,
    struct kaps_db **dbp,
    int32_t is_broadcast);

#ifdef __cplusplus
}
#endif
#endif
